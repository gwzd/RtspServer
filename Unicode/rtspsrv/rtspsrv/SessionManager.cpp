#include "stdafx.h"
#include "ThreadMutex.h"
#include "SessionManager.h"
#define   WIN32_LEAN_AND_MEAN   
#include <time.h>
#include "VideoManager.h"
#include "xlog.h"
#include "Prototypes.h"


#define  VIDEO_PORT_BASE 12000
#define  VIDEO_PORT_MAX  62000

namespace RTSP {
	namespace RTSPSrv {

		long  RTSPSessionManager::SessionId = 1;
		long  RTSPSessionManager::StartPort = VIDEO_PORT_BASE - 1;

		RTSPSessionManager::RTSPSessionManager()
		{
			hStopEvent = ::CreateEvent(NULL, FALSE, FALSE, NULL);
		}

		void  RTSPSessionManager::StopHeartBeatCheck()
		{
			SetEvent(hStopEvent);
		}

		bool RTSPSessionManager::IsHeartBeatCheckStopped()
		{
			if (WaitForSingleObject(hStopEvent, 1000) == WAIT_OBJECT_0)
				return true;

			return false;
		}

		void RTSPSessionManager::HeartBeatCheck()
		{
			while (!IsHeartBeatCheckStopped())
			{
				CGuard<CThreadMutex> guard(m_mutex);
				std::list<RTSPSessionItem>::iterator it = m_sessions.begin();
				while (it != m_sessions.end())
				{
					//X_LOG0(XLOG_INFO,_T("心跳检查...."));
					time_t now;
					time(&now);
					if (difftime(now, it->timestamp)>30) //30秒
					{
						VideoStreamManager *pVideoMgr = (VideoStreamManager *)VideoStreamManagerSingleton::instance();
						if (!pVideoMgr)
						{
							X_LOG0(XLOG_FATAL, _T("视频源管理器初始化异常！"));
							continue;
						}
						pVideoMgr->RemoveObserver(static_cast<DWORD>((long)it->hSession), it->pUserData);
						X_LOG1(XLOG_DEBUG, _T("心跳检查...移除会话:%d"), it->sessionid);
						it = m_sessions.erase(it);
					}
					else	++it;
				}
			}

			CloseHandle(hStopEvent);
			X_LOG0(XLOG_INFO, _T("心跳检查已停止！"));
		}

		void RTSPSessionManager::CleanSession(unsigned long sessionId)
		{
			RTSPSessionItem item;
			if (!GetSessionByHandle(sessionId, item))	return;

			RemoveSession(sessionId);

			VideoStreamManager *pVideoMgr = (VideoStreamManager *)VideoStreamManagerSingleton::instance();
			if (!pVideoMgr)
			{
				X_LOG0(XLOG_FATAL, _T("视频源管理器初始化异常！"));
				return;
			}
			else	pVideoMgr->RemoveObserver(static_cast<DWORD>((long)item.hSession), item.pUserData);

			item.pUserData.reset();
		}

		/*
		void RTSPSessionManager::HeartBeatCheck()
		{
			while(!IsHeartBeatCheckStopped())
			{
				do
				{
					CGuard<CThreadMutex> guard(m_mutex);
					_set_error_mode(_OUT_TO_STDERR);
					std::vector<RTSPSessionItem>::iterator it = m_sessions.begin();
					while(it!=m_sessions.end())
					{
						time_t now;
						time(&now);
						if(difftime(now,it->timestamp)>30) //30秒
						{
							VideoStreamManager *pVideoMgr =  (VideoStreamManager *)VideoStreamManagerSingleton::instance();
							if(!pVideoMgr)
							{
								X_LOG0(XLOG_FATAL,"视频源管理器初始化异常！");
								continue;
							}
							pVideoMgr->RemoveObserver(static_cast<DWORD>((long)it->hSession), it->pUserData);
							X_LOG1(XLOG_DEBUG,"心跳检查...移除会话:%d",it->sessionid);
							it = m_sessions.erase(it);
						}
						else
						{
							++it;
						}

						if(IsHeartBeatCheckStopped())
						break;
					}
				}while(0);

				if(IsHeartBeatCheckStopped())
					break;

				Sleep(1000);
			}

			X_LOG0(XLOG_INFO,"心跳检查已停止！");
		}
		*/

		unsigned short RTSPSessionManager::getNextPort()
		{
			if (StartPort >= VIDEO_PORT_MAX)	StartPort = VIDEO_PORT_BASE;

			return (WORD)InterlockedIncrement(&StartPort);
		}

		bool RTSPSessionManager::GetSessionItemByPeer(const std::string& ip, unsigned short port, RTSPSessionItem* item)
		{
			CGuard<CThreadMutex> guard(m_mutex);
			std::list<RTSPSessionItem>::iterator it = m_sessions.begin();
			for (; it != m_sessions.end(); ++it)
			{
				RTSPSessionItem& rsi = *it;
				//std::cout<<"ip "<<rsi.clientip<<":"<<ip<<" port "<<rsi.clientport<<":"<<port<<std::endl;
				if (rsi.clientip == ip && rsi.clientport == port && rsi.conntype == TT_TCP)
				{
					*item = *it;
					return true;
				}
			}
			return false;
		}

	}
}






