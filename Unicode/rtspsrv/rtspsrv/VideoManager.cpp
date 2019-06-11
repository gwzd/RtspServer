#include "stdafx.h"
#include "VideoManager.h"
#include "SessionManager.h"
#include "VideoSourceFactory.h"
#include <boost/lexical_cast.hpp>
#include "IVideoSource.h"
#include "xlog.h"
#include <tchar.h>
#include "rtspsrv.h"
#include <set>

using namespace std;

namespace RTSP {
	namespace RTSPSrv {

		VideoStreamManager::VideoStreamManager():dwSessionIdSeed(1)
		{	
		}

		VideoStreamManager::~VideoStreamManager()
		{
		}

		BOOL VideoStreamManager::CreateVideoSource(const RTSPSessionItem& item,VideoSource **ppVideoSource)
		{
			std::string proxy; 
			VideoSource *pSource = NULL;
			try	{
				CVideoSourceFactory* pFactory = CVideoSourceFactorySingleton::instance();	

				if(!pFactory)
				{				
					//X_LOG0(XLOG_FATAL,_T("视频源工厂初始化错误！"));//CVideoSourceFactory初始化异常
					return FALSE;
				}

				if(!item.proxy.empty())
				{
					pSource = pFactory->createVideoSource("TRANS");	
					proxy = "CONN_PROXY_INFO="+item.proxy+";"+"TYPENAME="+item.devtype;
				}

				pSource = pFactory->createVideoSource(item.devtype.c_str());	
			}
			catch(...)
			{
				//X_LOG1(XLOG_FATAL, _T("创建视频源时发生致命错误，错误类型:%s"), CString(item.devtype.c_str()));//Fatal error in createVideoSource!,		
				return FALSE;
			}

			if(pSource==NULL)
			{
				//X_LOG1(XLOG_ERROR, _T("无法识别设备类型:%s"), CString(item.devtype.c_str()));				//未知的设备类型
				return FALSE;
			}

			//X_LOG2(XLOG_DEBUG, _T("正在连接设备...,设备类型:%s,设备地址:%s"), CString(item.devtype.c_str()), CString(item.dvrip.c_str()));//开始连接设备

			if(pSource->ConnectDevice(item.dvrip.c_str(),item.port,item.conntype,
				item.username.c_str(), item.password.c_str(), proxy.c_str()))
			{
				*ppVideoSource = pSource;
				return TRUE;
			}

			int nError = pSource->GetLastError();		
			delete pSource;	
			*ppVideoSource = NULL;
			//X_LOG2(XLOG_DEBUG, _T("连接远端设备失败:%s,错误编码:%d"), CString(item.dvrip.c_str()), nError);
			return FALSE;
		}

		//返回是否为空
		bool VideoStreamManager::RemoveInvalidSession()
		{
			CGuard<CThreadMutex> guard(m_mutex);
			time_t now;
			time(&now);
			std::list<VideoSourceItem>::iterator it = m_sessions.begin();	
			while(it!=m_sessions.end())
			{	
				VideoSourceItem& vsi =*it;				
				if(difftime(now,it->timestamp)>30)
				{
					CleanVideoItem(vsi);
					it = m_sessions.erase(it);
				}
				else	++it;
			}
			return m_sessions.empty();
		}

		BOOL  VideoStreamManager::getStreamByID(int sessionID,VideoSourceItem& item)//8.15  增加一个根据sessID获取vItem的函数
		{
            CGuard<CThreadMutex> guard(m_mutex);

			std::list<VideoSourceItem>::iterator it = m_sessions.begin();	
			for(;it!=m_sessions.end();)
			{	
				VideoSourceItem& vsi =*it;				

				if(vsi.sessionid == sessionID)
				{														
					item = *it;							
					return TRUE;
				}
				++it;
			}
			return FALSE;
		}

		//7.28  增加sessionID
		//2012-10-26 增加端口号参数
		BOOL VideoStreamManager::GetLiveStreamByDVR(int streamtype,int sessionID,LPCSTR ip, unsigned short port, short channel,const std::string& file,VideoSourceItem& item)
		{
			if(!RemoveInvalidSession())
			{
				CGuard<CThreadMutex> guard(m_mutex);

 				//不缓存文件流     8.15将注释去掉
 				if(!file.empty())
 					return FALSE;

				std::list<VideoSourceItem>::iterator it = m_sessions.begin();	
				for(;it!=m_sessions.end();)
				{	
					VideoSourceItem& vsi =*it;
					CString	Ip = CString(vsi.dvrip.c_str());
					USES_CONVERSION;
					if(( !Ip.Compare(A2W(ip)) && vsi.port == port && vsi.channel == channel && vsi.streamtype == streamtype) && ((file.empty()&&vsi.file.empty())/*||(file==vsi.file)&&(vsi.sessionid == sessionID)*/))  //后面的注释8.15
					{														
						item = *it;							
						return TRUE;
					}
					++it;
				}
			}

			return FALSE;
		}

		void VideoSource::OnRealDataArrival(long sessionId, unsigned char* pBuffer, unsigned long Length)
		{
			VideoStreamManager *pMgr =  (VideoStreamManager *)VideoStreamManagerSingleton::instance();
			VideoSourceItem item;
			static unsigned long ulSeqence =0 ;
			//X_LOG3(XLOG_TRACE, _T("收到媒体数据,Session:%d,长度:%d,顺序:%d"), sessionId, Length, ulSeqence++);

			//比较大的瓶颈，访问太频繁
			if(pMgr->GetSessionByHandle(sessionId,item))
			{			
				if(!item.stopped)
				{
					if(item.sender)
						item.sender->notifyAll(pBuffer,Length);								
				}
				time(&item.timestamp);				
				pMgr->SetSessionByHandle(sessionId,item);
			}	
		}

		//云台控制函数入口
		BOOL VideoStreamManager::DoPTZCommand(const RTSPSessionItem& item,VideoSourceItem& vitem,DWORD command,const std::vector<DWORD>& params,const std::string& protocol)
		{
			if(vitem.pSource)
			{
				if(params.size()>=3)
				{
					if(!vitem.pSource->PtzControl(vitem.channel,command,params[0],params[1],protocol.c_str()))
					{
						std::cout<<"控制云台失败,返回的错误号:"<<vitem.pSource->GetLastError()<<std::endl;
						return FALSE;
					}
					return TRUE;
				}
			}
			return FALSE;			
		}
		
		unsigned short VideoStreamManager::GetSenderPort(const RTSPSessionItem& item,VideoSourceItem &vitem)
		{
			if(vitem.sender->uRtpPort!=0)
				return vitem.sender->uRtpPort;

			int nRetryCount=5; //最大重试次数
			unsigned short lPortbase = item.portbase;
			bool bFailed = !vitem.sender->CreateSender(item.portbase,"0.0.0.0");
			while(bFailed && nRetryCount>=0)	
			{
				X_LOG1(XLOG_DEBUG,_T("创建RTP发送器失败!端口:%d重试..."),item.portbase);
				long lPortbase = RTSPSessionManager::getNextPort();
				long lDummy = RTSPSessionManager::getNextPort();								 
				bFailed = !vitem.sender->CreateSender((short)lPortbase,"0.0.0.0");								 
				if(!bFailed)
				{
					RTSPSessionItem newitem(item);
					newitem.portbase = (WORD)lPortbase;
					RTSPSessionManagerSingleton::instance()->SetSessionByHandle(item.sessionid,newitem);
					break;
				}
				--nRetryCount;
			}
			if(!bFailed)
			{
				vitem.sender->uRtpPort = lPortbase;		
				X_LOG1(XLOG_DEBUG,_T("成功创建RTP发送器，端口:%d。"),lPortbase);
			}
			return vitem.sender->uRtpPort;
		}

		BOOL VideoStreamManager::CreateVideoStream(const RTSPSessionItem& item,VideoSourceItem& vitem)
		{	
			
			vitem.channel = item.channel;
			vitem.dvrip = item.dvrip;			
			vitem.username = item.username;
			vitem.password = item.password;
			vitem.port = item.port;	
			vitem.devtype = item.devtype;
			vitem.streamtype = item.streamtype;	
			vitem.pSource = NULL;
			vitem.file = item.file;
			vitem.nHeaderLength = -1;
			vitem.stopped = false;
			memset(vitem.header,0,512);
			vitem.sessionid = CreateSessionId();	
			
			VideoSource* pSource = NULL;
			std::string proxy; 

			try{
				CVideoSourceFactory* pFactory = CVideoSourceFactorySingleton::instance();	
				
				if(!pFactory)
				{				
					X_LOG0(XLOG_FATAL,_T("视频源工厂初始化异常！"));
					return FALSE;
				}

				if(!item.proxy.empty())
				{
					pSource = pFactory->createVideoSource("TRANS");	
					proxy = "CONN_PROXY_INFO="+item.proxy+";"+"TYPENAME="+item.devtype;
					X_LOG1(XLOG_INFO,_T("开始使用转发,地址:%s"),CString(item.proxy.c_str()));
				}
				else
				{				
					pSource = pFactory->createVideoSource(item.devtype.c_str());
				}
			
			}
			catch(...)
			{
				//Fatal error in CVideoSourceFactory::createVideoSource
				X_LOG1(XLOG_FATAL, _T("创建视频源工厂时发生致命错误!,错误类型:%s"), CString(item.devtype.c_str()));
				return FALSE;
			}

			if(pSource==NULL)
			{
				X_LOG1(XLOG_ERROR, _T("未知的设备类型:%s"), CString(item.devtype.c_str()));
				return FALSE;
			}

							
			
			X_LOG2(XLOG_DEBUG, _T("开始连接设备...,类型:%s,IP:%s"), CString(item.devtype.c_str()), CString(item.dvrip.c_str()));

			if (pSource->ConnectDevice(item.dvrip.c_str(), item.port, item.conntype, 
				item.username.c_str(), item.password.c_str(), proxy.c_str()))
			{
				vitem.pSource = pSource;
				LOG4CPLUS_DEBUG(logger, _T("连接的设备成功:[") << CString(item.devtype.c_str()) << _T("]IP:[") << CString(item.dvrip.c_str()) << _T("]连接方式:") << ((item.conntype == 0) ? _T("[TCP") : _T("[UDP")) << _T("]通道:[") << item.channel << _T("]客户地址:[") << CString(item.clientip.c_str()) << _T("]") << _T("转发:") << CString(item.proxy.c_str()) << std::endl);

				bool bStartVideo = false;

				try{ 

					if(!item.file.empty())
						bStartVideo = pSource->StartRemotePlayDataReceive(vitem.sessionid,item.channel,item.file.c_str());
					else
						bStartVideo = pSource->StartRealDataReceive(vitem.sessionid,vitem.channel,vitem.streamtype);
				}
				catch(...)
				{
					if(!item.file.empty())
					{
						X_LOG1(XLOG_FATAL, _T("回放远程视频文件发生异常！文件名:%s."), CString(item.file.c_str()));
					}
					else
					{
						X_LOG1(XLOG_FATAL,_T("预览远程视频发生异常！通道号:%d."),vitem.channel);
					}
				}

				if(bStartVideo)
				{			
					try{
						vitem.sender = boost::shared_ptr<PacketSender>(new PacketSender());
						if(vitem.sender)
						{			
							time(&vitem.timestamp);
							AddSession(vitem);			
						}
						else
						{
							X_LOG0(XLOG_FATAL,_T("分配数据包对象发生异常!"));//PacketSender
							pSource->DisconnectDevice();
							int nError = pSource->GetLastError();		
							delete pSource;	
							return FALSE;
						}
					}
					catch(...)
					{
						X_LOG0(XLOG_FATAL,_T("发送数据报工具，创建socket发生异常!"));//PacketSender::createSender
						pSource->DisconnectDevice();
						int nError = pSource->GetLastError();		
						delete pSource;	
						return FALSE;
					}															
					return TRUE;
				}
				else
				{
					pSource->DisconnectDevice();
					int nError = pSource->GetLastError();			
					delete pSource;			
					X_LOG2(XLOG_WARNING, _T("播放远端视频错误:IP:%s,错误号:%d"), CString(item.dvrip.c_str()), nError);
					return FALSE;
				}
			}
			else
			{		
				int nError = pSource->GetLastError();		
				delete pSource;				
				X_LOG2(XLOG_DEBUG, _T("连接远端设备错误:%s,错误号:%d"), CString(item.dvrip.c_str()), nError);
				return FALSE;
			}						
			return FALSE;
		}

		
		BOOL VideoStreamManager::DestroyRealStream(DWORD dwSessionId)
		{
			VideoSourceItem vitem;
			if(GetSessionByHandle(dwSessionId,vitem))
			{
				//先停止视频流分发
				vitem.stopped = true;
				SetSessionByHandle(vitem.sessionid,vitem);
				
				X_LOG2(XLOG_DEBUG, _T("停止一个视频源,IP地址:%s通道号:%d"), CString(vitem.dvrip.c_str()), vitem.channel);
				RemoveSession(vitem.sessionid);		
				return CleanVideoItem(vitem);				
			}
			return FALSE;
		}

		BOOL VideoStreamManager::AddObserver(DWORD dwSessionId,ObserverPtr info)
		{
			VideoSourceItem item;
			if(!GetSessionByHandle(dwSessionId,item))
			{		
				return FALSE;	
			}
				
			if(item.sender)
			{
				item.sender->addObserver(info);
			}
			return SetSessionByHandle(dwSessionId,item);	
		}

		BOOL VideoStreamManager::RemoveObserver(DWORD dwSessionId,ObserverPtr info)
		{
			VideoSourceItem item;
			
			if(!GetSessionByHandle(dwSessionId,item))
			{		
				return FALSE;	
			}

			if(item.sender)
			{
				item.sender->removeObserver(info);						
				if(item.sender->empty())
				{
					DestroyRealStream(dwSessionId);
				}
			}
			return SetSessionByHandle(dwSessionId,item);	
		}

		BOOL VideoStreamManager::GetFileHeader(DWORD dwSessionId,unsigned char* pBuffer,unsigned long &Length)
		{
			VideoSourceItem item;
			if(GetSessionByHandle(dwSessionId,item))
			{								
				try{
					if(item.pSource)
					{
						return item.pSource->GetFileHeader(pBuffer,Length);
					}
					else
					{
						X_LOG0(XLOG_ERROR,_T("获取视频流头时还未建立视频源连接."));
					}
				}
				catch(...)
				{
					X_LOG0(XLOG_ERROR,_T("获取视频流头时发生异常."));
				}					
				return FALSE;
			}
			else
			{
				X_LOG0(XLOG_ERROR,_T("无效的会话信息."));
			}

			return FALSE;	
		}

		BOOL VideoStreamManager::DoExtendCommand(const RTSPSessionItem& item,VideoSourceItem& vitem,DWORD dwCommand,const std::vector<DWORD>& params,const std::string& param5, std::vector<header>& headers)
		{
			switch(dwCommand)
			{
			case NET_DVR_PLAYFAST:
				return vitem.pSource->RemotePlayFast();
				break;
			case NET_DVR_PLAYSLOW:
				return vitem.pSource->RemotePlaySlow();
				break;
			case NET_DVR_PLAYPAUSE:
				return vitem.pSource->RemotePlayPause();
				break;			
			case NET_DVR_PLAYRESTART:
				return vitem.pSource->RemotePlayRestore();
				break;
			case NET_DVR_PLAYNORMAL:				
				return vitem.pSource->RemotePlayNormal();
				break;
			case NET_DVR_PLAYFRAME:
				return vitem.pSource->RemotePlayStep();
				break;
			case NET_DVR_PLAYSETPOS:				
				return vitem.pSource->RemotePlaySetPos((float)params[0]/100);
				break;
			case NET_DVR_PLAYGETPOS:
				{
					float fPos;
					if(vitem.pSource->RemotePlayGetPos((float*)&fPos))
					{
						headers.resize(1);
						headers[0].name="Value";
						headers[0].value= boost::lexical_cast<string>(int(fPos*100));	
						return TRUE;
					}
					else
						return FALSE;					
				}
				break;

			/*case NET_DVR_PLAYGETTIME:
				{
					float fPos;
					if(vitem.pSource->RemotePlayGetPos((float*)&fPos))
					{
						headers.resize(1);
						headers[0].name="Value";
						headers[0].value= boost::lexical_cast<string>(int(fPos*100));	
						return TRUE;
					}
					else
						return FALSE;					
				}
				break;
			case NET_DVR_PLAYGETFRAME:
				{
					float fPos;
					if(vitem.pSource->RemotePlayGetPos((float*)&fPos))
					{
						headers.resize(1);
						headers[0].name="Value";
						headers[0].value= boost::lexical_cast<string>(int(fPos*100));	
						return TRUE;
					}
					else
						return FALSE;					
				}
				break;
			case NET_DVR_GETTOTALFRAMES:
				{
					float fPos;
					if(vitem.pSource->RemotePlayGetPos((float*)&fPos))
					{
						headers.resize(1);
						headers[0].name="Value";
						headers[0].value= boost::lexical_cast<string>(int(fPos*100));	
						return TRUE;
					}
					else
						return FALSE;					
				}
				break;
			case NET_DVR_GETTOTALTIME:
				{
					float fPos;
					if(vitem.pSource->RemotePlayGetPos((float*)&fPos))
					{
						headers.resize(1);
						headers[0].name="Value";
						headers[0].value= boost::lexical_cast<string>(int(fPos*100));	
						return TRUE;
					}
					else
						return FALSE;					
				}
				break;*/
			case NET_DVR_PLAYFILELIST:
				{
					string result;
					QUERYTIME querytime;
					//TODO:解析日期格式
					if(param5.length()!=29)
						return FALSE;
					CString strStart(param5.substr(0,14).c_str());
					CString strEnd(param5.substr(15, 14).c_str());

					querytime.ufromyear  = _ttoi(strStart.Mid(0,4));
					querytime.ufrommonth = _ttoi(strStart.Mid(4, 2));
					querytime.ufromday = _ttoi(strStart.Mid(6, 2));
					querytime.ufromhour = _ttoi(strStart.Mid(8, 2));
					querytime.ufromminute = _ttoi(strStart.Mid(10, 2));
					querytime.ufromsecond = _ttoi(strStart.Mid(12, 2));

					querytime.utoyear = _ttoi(strEnd.Mid(0, 4));
					querytime.utomonth = _ttoi(strEnd.Mid(4, 2));
					querytime.utoday = _ttoi(strEnd.Mid(6, 2));
					querytime.utohour = _ttoi(strEnd.Mid(8, 2));
					querytime.utominute = _ttoi(strEnd.Mid(10, 2));
					querytime.utosecond = _ttoi(strEnd.Mid(12, 2));

					/*querytime.ufromyear  = 2009;
					querytime.ufrommonth = 10;
					querytime.ufromday = 10;
					querytime.ufromhour = 0;
					querytime.ufromminute = 0;
					querytime.ufromsecond = 0;
					querytime.utoyear  = 2009;
					querytime.utomonth = 10;
					querytime.utoday = 30;
					querytime.utohour = 0;
					querytime.utominute = 0;
					querytime.utosecond = 0;*/

					if(vitem.pSource->RemotePlayGetFileList(item.channel,params[0],&querytime,CString(result.c_str())))
					{
						headers.resize(1);
						headers[0].name="Value";
						headers[0].value= result;
						return TRUE;
					}
				}
				break;
			}
			
			return FALSE;
		}

		void VideoStreamManager::StopAllVideoStream()
		{
			CGuard<CThreadMutex> guard(m_mutex);
			time_t now;
			time(&now);
			std::list<VideoSourceItem>::iterator it = m_sessions.begin();	
			while(it!=m_sessions.end())
			{	
				VideoSourceItem& vsi =*it;								
				CleanVideoItem(vsi);
				it = m_sessions.erase(it);				
			}			
		}

		BOOL VideoStreamManager::CleanVideoItem( VideoSourceItem &vitem )
		{
			if(vitem.sender)
			{
				vitem.sender->stopNotify();
				vitem.sender.reset();		
			}
			if(vitem.pSource)
			{
				try{

					if(vitem.file.empty())
						vitem.pSource->StopRealDataReceive(vitem.channel);
					else
						vitem.pSource->StopRemotePlayDataReceive();

					vitem.pSource->DisconnectDevice();
					delete vitem.pSource;	


					return TRUE;
				}
				catch(...)
				{
					X_LOG2(XLOG_ERROR, _T("停止一个视频源时发生异常,IP地址:%s通道号:%d"), CString(vitem.dvrip.c_str()), vitem.channel);
				}									
			}	
			return FALSE;
		}

	}
}