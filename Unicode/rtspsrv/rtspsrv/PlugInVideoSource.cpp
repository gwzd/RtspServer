#include "stdafx.h"
#include "PlugInVideoSource.h"
#include <boost/algorithm/string/predicate.hpp>
#include "xlog.h"
#include <tchar.h>
namespace RTSP {
	namespace RTSPSrv {

		static void _stdcall CallBackDataHandler(HANDLE hChanHandle, 							  
			BYTE *pBuffer,
			DWORD dwBufSize,
			DWORD dwCookie, 
			DWORD dwParam)
		{						
			VideoSource::OnRealDataArrival(dwCookie,pBuffer,dwBufSize);			
		}

		CPlugInVideoSource::CPlugInVideoSource()
		{
			m_hDevHandle = NULL;
		}

		CPlugInVideoSource::~CPlugInVideoSource()
		{
		}

		bool CPlugInVideoSource::ConnectDevice(LPCSTR dvrip, unsigned short port, int type, LPCSTR username, LPCSTR password, LPCSTR proxy)
		{
			IDVR_ConnectDevice pfnIDVR_ConnectDevice = m_pInfo->GetFunctionPtr()->pfnIDVR_ConnectDevice;
			if(!pfnIDVR_ConnectDevice)
			{
				X_LOG0(XLOG_FATAL,_T("插件未实现IDVR_ConnectDevice函数"));
				return false;
			}
			bool ret = false;
			//2016.9.17 zxl 取消打印
			//X_LOG6(XLOG_DEBUG,_T("开始调用IDVR_ConnectDevice('%s',%d,'%s','%s',%d,&hDevHandle,'%s')函数..."),dvrip,port,username,password,type,proxy);
			ret = (*pfnIDVR_ConnectDevice)(dvrip,port,username,password,(TRANSTYPE)type,&m_hDevHandle,proxy,0)!=FALSE;			
			//2016.9.17 zxl 取消打印
			//X_LOG1(XLOG_DEBUG,_T("结束调用IDVR_ConnectDevice函数.返回值:%s."),ret?_T("成功"):_T("失败"));		
			return ret;						
		}

		void CPlugInVideoSource::DisconnectDevice()
		{
			IDVR_DisConnectDevice pfnIDVR_DisConnectDevice = m_pInfo->GetFunctionPtr()->pfnIDVR_DisConnectDevice;
			if(!pfnIDVR_DisConnectDevice)
			{
				X_LOG0(XLOG_FATAL,_T("插件未实现IDVR_DisConnectDevice函数"));
				return;
			}
			BOOL ret = FALSE;
			//2016.9.17 zxl 取消打印
			//X_LOG0(XLOG_DEBUG,_T("开始调用IDVR_DisConnectDevice函数..."));
			ret = (*pfnIDVR_DisConnectDevice)(m_hDevHandle);
			//2016.9.17 zxl 取消打印
			//X_LOG1(XLOG_DEBUG,_T("结束调用IDVR_DisConnectDevice函数.返回值:%s."),ret?_T("成功"):_T("失败"));					
		}

		bool CPlugInVideoSource::StartRealDataReceive(long lSessionId, long nChannel,long streamtype)
		{
			IDVR_StartRealData pfnIDVR_StartRealData = m_pInfo->GetFunctionPtr()->pfnIDVR_StartRealData;

			if(!pfnIDVR_StartRealData)
			{
				X_LOG0(XLOG_FATAL,_T("插件未实现IDVR_StartRealData函数"));
				return false;
			}

			m_hChanHandle = NULL;
			bool ret = false;
			//2016.9.17 zxl 取消打印
			X_LOG3(XLOG_DEBUG,_T("开始调用IDVR_StartRealData(hDevHandle,%d,%d,hChanHandle,%d,NULL,NULL,callbackHandler)函数..."),nChannel,streamtype,lSessionId);
			ret = (*pfnIDVR_StartRealData)(m_hDevHandle,
				nChannel,
				streamtype,
				&m_hChanHandle,
				(DWORD)lSessionId,
				NULL,
				NULL,
				CallBackDataHandler)!=FALSE;
			//2016.9.17 zxl 取消打印
			X_LOG1(XLOG_DEBUG,_T("结束调用IDVR_StartRealData函数.返回值:%s."),ret?_T("成功"):_T("失败"));								
			return ret;
		}
		//云台控制函数
		bool CPlugInVideoSource::PtzControl( long nChannel,DWORD command,DWORD param1,DWORD param2,LPCSTR lpszParam3 )
		{
			IDVR_PTZControlByChannel pfnIDVR_PTZControlByChannel = m_pInfo->GetFunctionPtr()->pfnIDVR_PTZControlByChannel;
			BOOL ret = false;
			if(pfnIDVR_PTZControlByChannel)
			{
				//2016.9.17 zxl 取消打印
				//X_LOG5(XLOG_DEBUG,_T("开始调用IDVR_PTZControlByChannel(m_hChanHandle,%d,%d,'%s',%d,%d,NULL,NULL)函数..."),nChannel,command,lpszParam3,param1,param2);
				ret = (*pfnIDVR_PTZControlByChannel)(m_hChanHandle,(CTRLTYPE)command,param1,param2,NULL,NULL);
				//2016.9.17 zxl 取消打印
				//X_LOG1(XLOG_DEBUG,_T("结束调用IDVR_PTZControlByChannel函数.返回值:%s."),ret?_T("成功"):_T("失败"));			
				if (ret)
				{
					//TRACE(_T("ptz ctrl success! \r\n"));
					//X_LOG1(XLOG_DEBUG,_T("ptz ctrl success! command is %d \r\n"),command);
					return true;
				}
				else
				{
					//CString str_showError;
					//str_showError.Format(_T("ptz ctrl failed! command is %d \r\n"),command);
					X_LOG1(XLOG_DEBUG,_T("ptz ctrl failed! command is %d \r\n"),command);	
					//TRACE(str_showError);
					return false;
				}
			}
			else
			{
				X_LOG0(XLOG_DEBUG,_T("暂时未实现以通道模式控制云台，先调用通用云台控制函数..."));//插件未实现IDVR_PTZControlByChannel函数,调用IDVR_PTZControl函数.
				IDVR_PTZControl pfnIDVR_PTZControl = m_pInfo->GetFunctionPtr()->pfnIDVR_PTZControl;
				if(!pfnIDVR_PTZControl)
				{
					X_LOG0(XLOG_FATAL,_T("通用云台控制函数未能正常工作!"));//插件未实现IDVR_PTZControl函数
					return false;
				}

				BOOL ret = false;
				//2016.9.17 zxl 取消打印
				//X_LOG5(XLOG_DEBUG,_T("开始调用IDVR_PTZControl(hDevHandle,%d,%d,'%s',%d,%d,NULL,NULL)函数..."),nChannel,command,lpszParam3,param1,param2);
				ret = (*pfnIDVR_PTZControl)(m_hDevHandle,nChannel,(CTRLTYPE)command,lpszParam3,param1,param2,NULL,NULL);
				//2016.9.17 zxl 取消打印
				//X_LOG1(XLOG_DEBUG,_T("结束调用IDVR_PTZControl函数.返回值:%s."),ret?_T("成功"):_T("失败"));			
				if (ret)	return true;
				return false;
			}
		}

		int  CPlugInVideoSource::GetLastError()
		{
			TCHAR buf[1024];
			ISYS_GetLastError pfnISYS_GetLastError = m_pInfo->GetFunctionPtr()->pfnISYS_GetLastError;			
			if( pfnISYS_GetLastError && (*pfnISYS_GetLastError)(buf,1024))
				//X_LOG1(XLOG_DEBUG, _T("详细的错误信息%s"), CString(A2W(buf)));
			return 0;
		}

		void CPlugInVideoSource::StopRealDataReceive(long nChannel)
		{
			IDVR_StopRealData pfnIDVR_StopRealData = m_pInfo->GetFunctionPtr()->pfnIDVR_StopRealData;
			if(!pfnIDVR_StopRealData)
			{
				X_LOG0(XLOG_FATAL, _T("插件未实现停止录像函数"));//IDVR_StopRealData
				return;
			}
			BOOL ret = FALSE;
			//2016.9.17 zxl 取消打印
			//X_LOG0(XLOG_DEBUG,_T("开始调用IDVR_StopRealData函数..."));
			ret = (*pfnIDVR_StopRealData)(m_hChanHandle);
			//2016.9.17 zxl 取消打印
			//X_LOG1(XLOG_DEBUG,_T("结束调用IDVR_StopRealData函数.返回值:%s."),ret?_T("成功"):_T("失败"));					
		}
		
		bool CPlugInVideoSource::StartRemotePlayDataReceive(long lSessionId,long nChannel,LPCSTR lpszFile)
		{
			if(lpszFile==NULL)
			{
				X_LOG0(XLOG_FATAL, _T("StartRemotePlayDataReceive提供的参数错误：文件名为空！"));
				return false;
			}

			CString file = CString(lpszFile);
			if ( !((file.GetLength() == 39) && (file.Find(_T("DateRange:") ==0))) )
//			if(! ( boost::algorithm::starts_with(file,"DateRange:") && file.length()==39))
			{
				IDVR_RemotePlay pfnIDVR_RemotePlay = m_pInfo->GetFunctionPtr()->pfnIDVR_RemotePlay;
				IDVR_SetRemotePlayDataCallBack  pfnIDVR_SetRemotePlayDataCallBack = m_pInfo->GetFunctionPtr()->pfnIDVR_SetRemotePlayDataCallBack;
				if(pfnIDVR_RemotePlay!=NULL && pfnIDVR_SetRemotePlayDataCallBack!=NULL)
				{
					BOOL ret = FALSE;
					//2016.9.17 zxl 取消打印
					//X_LOG2(XLOG_DEBUG,_T("开始调用IDVR_RemotePlay(%d,'%s')函数..."),lSessionId,lpszFile);
					ret = (*pfnIDVR_RemotePlay)(m_hDevHandle,lpszFile,NULL,&m_hChanHandle);
					if(ret)
					{
						//2016.9.17 zxl 取消打印
						//X_LOG0(XLOG_DEBUG,_T("开始调用IDVR_SetRemotePlayDataCallBack函数..."));
						ret = (*pfnIDVR_SetRemotePlayDataCallBack)(m_hChanHandle,lSessionId,NULL,0,CallBackDataHandler);
						//2016.9.17 zxl 取消打印
						//X_LOG1(XLOG_DEBUG,_T("结束调用IDVR_SetRemotePlayDataCallBack函数.返回值:%s."),ret?_T("成功"):_T("失败"));						
					}	
					//2016.9.17 zxl 取消打印
					//X_LOG1(XLOG_DEBUG,_T("结束调用IDVR_RemotePlay函数.返回值:%s."),ret?_T("成功"):_T("失败"));	
					if (ret)	return true;

					return false;
				}
				else	X_LOG0(XLOG_FATAL, _T("插件未实现IDVR_RemotePlay或IDVR_SetRemotePlayDataCallBack函数！"));
			}
			else
			{
				IDVR_RemotePlayByTime pfnIDVR_RemotePlayByTime = m_pInfo->GetFunctionPtr()->pfnIDVR_RemotePlayByTime;
				IDVR_SetRemotePlayDataCallBack  pfnIDVR_SetRemotePlayDataCallBack = m_pInfo->GetFunctionPtr()->pfnIDVR_SetRemotePlayDataCallBack;
				if(pfnIDVR_RemotePlayByTime!=NULL && pfnIDVR_SetRemotePlayDataCallBack!=NULL)
				{
					BOOL ret = FALSE;
					QUERYTIME querytime;
					CString	params = file.Mid(10,29);
					if (params.GetLength() != 29)
					{
						X_LOG0(XLOG_FATAL, _T("StartRemotePlayDataReceive提供的参数错误：并非正确的文件格式！"));
						return FALSE;
					}
					CString strStart = params.Left(14);
					CString strEnd = params.Right(14);

					querytime.ufromyear  = _ttoi(strStart.Left(4));
					querytime.ufrommonth = _ttoi(strStart.Mid(4,2));
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

					//2016.9.17 zxl 取消打印
					//X_LOG3(XLOG_DEBUG,_T("开始调用IDVR_RemotePlayByTime(...,%d,%d,'%s',NULL,...)函数..."),lSessionId,nChannel,lpszFile);
					ret = (*pfnIDVR_RemotePlayByTime)(m_hDevHandle,nChannel,&querytime,NULL,&m_hChanHandle);
					if(ret)
					{
						//2016.9.17 zxl 取消打印
						//X_LOG0(XLOG_DEBUG,_T("开始调用IDVR_SetRemotePlayDataCallBack函数..."));
						ret = (*pfnIDVR_SetRemotePlayDataCallBack)(m_hChanHandle,lSessionId,NULL,0,CallBackDataHandler);
						//X_LOG1(XLOG_DEBUG,_T("结束调用IDVR_SetRemotePlayDataCallBack函数.返回值:%s."),ret?_T("成功"):_T("失败"));						
					}				
					//X_LOG1(XLOG_DEBUG,_T("结束调用IDVR_RemotePlayByTime函数.返回值:%s."),ret?_T("成功"):_T("失败"));	
					if (ret)	return true;
					return false;
				}
				else	X_LOG0(XLOG_FATAL, _T("插件未实现IDVR_RemotePlayByTime或IDVR_SetRemotePlayDataCallBack函数！"));
			}
			
			return false;
		}

		void CPlugInVideoSource::StopRemotePlayDataReceive()
		{
			IDVR_StopRemotePlay pfnIDVR_StopRemotePlay = m_pInfo->GetFunctionPtr()->pfnIDVR_StopRemotePlay;			
			if(!pfnIDVR_StopRemotePlay)
			{
				X_LOG0(XLOG_FATAL, _T("插件未实现IDVR_StopRemotePlay函数"));
				return;
			}
			
			BOOL ret = FALSE;
			//2016.9.17 zxl 取消打印
			//X_LOG0(XLOG_DEBUG,_T("开始调用IDVR_StopRemotePlay函数..."));
			ret = (*pfnIDVR_StopRemotePlay)(m_hChanHandle);				
			//X_LOG1(XLOG_DEBUG,_T("结束调用IDVR_RemotePlay函数.返回值:%s."),ret?_T("成功"):_T("失败"));				
			return;
		}

		bool CPlugInVideoSource::GetFileHeader(unsigned char* pBuffer, unsigned long& Length)
		{
			IDVR_GetRealDataHeader pfnIDVR_GetRealDataHeader =  m_pInfo->GetFunctionPtr()->pfnIDVR_GetRealDataHeader;		
			if(!pfnIDVR_GetRealDataHeader)
			{
				X_LOG0(XLOG_FATAL, _T("插件未实现IDVR_GetRealDataHeader函数"));
				return false;
			}
			BOOL ret = FALSE;
			DWORD dwHeaderLen = 0;
			//2016.9.17 zxl 取消打印
			//X_LOG1(XLOG_DEBUG,_T("开始调用IDVR_GetRealDataHeader函数,缓冲区长度%d..."),Length);
			ret = (*pfnIDVR_GetRealDataHeader)(m_hChanHandle,pBuffer,Length,&dwHeaderLen);
			//X_LOG2(XLOG_DEBUG,_T("结束调用IDVR_GetRealDataHeader函数.返回值:%s,返回文件头长度:[%d]"),ret?_T("成功"):_T("失败"),dwHeaderLen);
			Length = dwHeaderLen;
			if (ret)	return true;

			return false;
		}

		bool CPlugInVideoSource::RemotePlayFast()
		{
			IDVR_FastRemotePlay pfnIDVR_FastRemotePlay = m_pInfo->GetFunctionPtr()->pfnIDVR_FastRemotePlay;
			
			if(pfnIDVR_FastRemotePlay!=NULL)
			{
				BOOL ret = FALSE;
				//2016.9.17 zxl 取消打印
				//X_LOG0(XLOG_DEBUG,_T("开始调用IDVR_FastRemotePlay()函数..."));
				ret = (*pfnIDVR_FastRemotePlay)(m_hChanHandle);	
				//X_LOG1(XLOG_DEBUG,_T("结束调用IDVR_FastRemotePlay函数.返回值:%s."),ret?_T("成功"):_T("失败"));	
				if (ret)	return true;
				return		false;
			}

			X_LOG0(XLOG_FATAL, _T("插件未实现IDVR_FastRemotePlay函数！"));
			return false;
		}

		bool CPlugInVideoSource::RemotePlaySlow()
		{
			IDVR_SlowRemotePlay pfnIDVR_SlowRemotePlay = m_pInfo->GetFunctionPtr()->pfnIDVR_SlowRemotePlay;

			if(pfnIDVR_SlowRemotePlay!=NULL)
			{
				BOOL ret = FALSE;
				//2016.9.17 zxl 取消打印
				//X_LOG0(XLOG_DEBUG,_T("开始调用IDVR_SlowRemotePlay()函数..."));
				ret = (*pfnIDVR_SlowRemotePlay)(m_hChanHandle);	
				//X_LOG1(XLOG_DEBUG,_T("结束调用IDVR_SlowRemotePlay函数.返回值:%s."),ret?_T("成功"):_T("失败"));	
				if (ret)	return true;

				return false;
			}

			X_LOG0(XLOG_FATAL, _T("插件未实现IDVR_SlowRemotePlay函数！"));
			return false;
		}

		bool CPlugInVideoSource::RemotePlayNormal()
		{
			IDVR_NormalRemotePlay pfnIDVR_NormalRemotePlay = m_pInfo->GetFunctionPtr()->pfnIDVR_NormalRemotePlay;

			if(pfnIDVR_NormalRemotePlay!=NULL)
			{
				BOOL ret = FALSE;
				//2016.9.17 zxl 取消打印
				//X_LOG0(XLOG_DEBUG,_T("开始调用IDVR_NormalRemotePlay()函数..."));
				ret = (*pfnIDVR_NormalRemotePlay)(m_hChanHandle);	
				//X_LOG1(XLOG_DEBUG,_T("结束调用IDVR_NormalRemotePlay函数.返回值:%s."),ret?_T("成功"):_T("失败"));	
				if (ret)	return true;

				return false;
			}

			X_LOG0(XLOG_FATAL, _T("插件未实现IDVR_NormalRemotePlay函数！"));
			return false;
		}

		bool CPlugInVideoSource::RemotePlayStep()
		{
			IDVR_StepRemotePlay pfnIDVR_StepRemotePlay = m_pInfo->GetFunctionPtr()->pfnIDVR_StepRemotePlay;

			if(pfnIDVR_StepRemotePlay!=NULL)
			{
				BOOL ret = FALSE;
				//2016.9.17 zxl 取消打印
				//X_LOG0(XLOG_DEBUG,_T("开始调用IDVR_StepRemotePlay()函数..."));
				ret = (*pfnIDVR_StepRemotePlay)(m_hChanHandle);	
				//X_LOG1(XLOG_DEBUG,_T("结束调用IDVR_StepRemotePlay函数.返回值:%s."),ret?_T("成功"):_T("失败"));	
				if (ret)	return true;
				return false;
			}

			X_LOG0(XLOG_FATAL, _T("插件未实现IDVR_StepRemotePlay函数！"));
			return false;
		}

		bool CPlugInVideoSource::RemotePlayPause()
		{
			IDVR_PauseRemotePlay pfnIDVR_PauseRemotePlay = m_pInfo->GetFunctionPtr()->pfnIDVR_PauseRemotePlay;

			if(pfnIDVR_PauseRemotePlay!=NULL)
			{
				BOOL ret = FALSE;
				//2016.9.17 zxl 取消打印
				//X_LOG0(XLOG_DEBUG,_T("开始调用IDVR_PauseRemotePlay()函数..."));
				ret = (*pfnIDVR_PauseRemotePlay)(m_hChanHandle);	
				//X_LOG1(XLOG_DEBUG,_T("结束调用IDVR_PauseRemotePlay函数.返回值:%s."),ret?_T("成功"):_T("失败"));	
				if (ret)	return true;
				return false;
			}

			X_LOG0(XLOG_FATAL, _T("插件未实现IDVR_PauseRemotePlay函数！"));
			return false;
		}

		bool CPlugInVideoSource::RemotePlayRestore()
		{
			IDVR_ResumeRemotePlay pfnIDVR_ResumeRemotePlay = m_pInfo->GetFunctionPtr()->pfnIDVR_ResumeRemotePlay;

			if(pfnIDVR_ResumeRemotePlay!=NULL)
			{
				BOOL ret = FALSE;
				//2016.9.17 zxl 取消打印
				//X_LOG0(XLOG_DEBUG,_T("开始调用IDVR_ResumeRemotePlay()函数..."));
				ret = (*pfnIDVR_ResumeRemotePlay)(m_hChanHandle);	
				//X_LOG1(XLOG_DEBUG,_T("结束调用IDVR_ResumeRemotePlay函数.返回值:%s."),ret?_T("成功"):_T("失败"));	
				if (ret)	return true;
				return false;
			}

			X_LOG0(XLOG_FATAL, _T("插件未实现IDVR_ResumeRemotePlay函数！"));
			return false;
		}

		bool CPlugInVideoSource::RemotePlaySetPos(float fPos)
		{
			IDVR_SetRemotePlayPos pfnIDVR_SetRemotePlayPos = m_pInfo->GetFunctionPtr()->pfnIDVR_SetRemotePlayPos;

			if(pfnIDVR_SetRemotePlayPos!=NULL)
			{
				BOOL ret = FALSE;
				//2016.9.17 zxl 取消打印
				//X_LOG1(XLOG_DEBUG,_T("开始调用IDVR_SetRemotePlayPos(%3.2f)函数..."),fPos);
				ret = (*pfnIDVR_SetRemotePlayPos)(m_hChanHandle,fPos);	
				//X_LOG1(XLOG_DEBUG,_T("结束调用IDVR_SetRemotePlayPos函数.返回值:%s."),ret?_T("成功"):_T("失败"));	
				if (ret)	return true;
				return false;
			}

			X_LOG0(XLOG_FATAL, _T("插件未实现IDVR_SetRemotePlayPos函数！"));
			return false;
		}

		bool CPlugInVideoSource::RemotePlayGetPos(float *pfPos)
		{
			IDVR_GetRemotePlayPos pfnIDVR_GetRemotePlayPos = m_pInfo->GetFunctionPtr()->pfnIDVR_GetRemotePlayPos;

			if(pfnIDVR_GetRemotePlayPos!=NULL)
			{
				BOOL ret = FALSE;
				//2016.9.17 zxl 取消打印
				//X_LOG0(XLOG_DEBUG,_T("开始调用IDVR_GetRemotePlayPos()函数..."));
				ret = (*pfnIDVR_GetRemotePlayPos)(m_hChanHandle,pfPos);	
				//X_LOG2(XLOG_DEBUG,_T("结束调用IDVR_GetRemotePlayPos(fPos=>%3.2f)函数.返回值:%s."),*pfPos,ret?_T("成功"):_T("失败"));	
				if (ret)	return true;
				return false;
			}

			X_LOG0(XLOG_FATAL, _T("插件未实现IDVR_GetRemotePlayPos函数！"));
			return false;			
		}

		/*bool CPlugInVideoSource::RemotePlayGetPlayedTime(float *fPos)
		{
			return false;
		}

		bool CPlugInVideoSource::RemotePlayGetTotalTime(DWORD *nFrames)
		{
			return false;
		}

		bool CPlugInVideoSource::RemotePlayGetPlayedFrames(DWORD *nFrames)
		{
			return false;
		}

		bool CPlugInVideoSource::RemotePlayGetTotalFrames(DWORD *nFrames)
		{
			return false;
		}
		*/

		bool CPlugInVideoSource::RemotePlayGetFileList(long nChannel,DWORD dwRecType,PQUERYTIME querytime,CString& result)
		{
			IDVR_QueryDevRecord pfnIDVR_QueryDevRecord = m_pInfo->GetFunctionPtr()->pfnIDVR_QueryDevRecord;
			ISYS_FreeMemory pfnISYS_FreeMemory = m_pInfo->GetFunctionPtr()->pfnISYS_FreeMemory;

			if(pfnIDVR_QueryDevRecord!=NULL && pfnISYS_FreeMemory!=NULL)
			{
				BOOL ret = FALSE;
				//2016.9.17 zxl 取消打印
				//X_LOG0(XLOG_DEBUG,_T("开始调用IDVR_GetRemotePlayPos()函数..."));
				LPSTR lpResult = NULL;
				DWORD dwResultLength = 0;
				ret = (*pfnIDVR_QueryDevRecord)(m_hDevHandle,nChannel,(RECORDTYPE)dwRecType,querytime,&lpResult,&dwResultLength);
				if(lpResult!=NULL)
				{
					result = lpResult;
					try{
						(*pfnISYS_FreeMemory)(lpResult);
					}
					catch(...)
					{
						X_LOG0(XLOG_FATAL, _T("调用ISYS_FreeMemory时发生异常!!"));
						throw;
					}					
					lpResult = NULL;
					return false;
				}				
				//X_LOG1(XLOG_DEBUG,_T("结束调用IDVR_QueryDevRecord函数.返回值:%s."),ret?_T("成功"):_T("失败"));	
				return true;
			}

			X_LOG0(XLOG_FATAL, _T("插件未实现IDVR_QueryDevRecord或ISYS_FreeMemory函数！"));
			return false;
		}

		bool  CPlugInVideoSource::PrepareStream(long nChannel,long streamtype)
		{
			IDVR_PrepareStream pfnIDVR_PrepareStream = m_pInfo->GetFunctionPtr()->pfnIDVR_PrepareStream;			

			if(pfnIDVR_PrepareStream!=NULL)
			{
				BOOL ret = FALSE;
				//2016.9.17 zxl 取消打印
				//X_LOG2(XLOG_DEBUG,_T("开始调用IDVR_PrepareStream函数(hDevHandle,%d,%d)..."),nChannel,streamtype);
				ret = (*pfnIDVR_PrepareStream)(m_hDevHandle,nChannel,streamtype);	
				//X_LOG1(XLOG_DEBUG,_T("结束调用IDVR_PrepareStream函数.返回值:%s."),ret? _T("成功"):_T("失败"));	
				if(!ret)
				{
					GetLastError();
					return false;
				}
				return true; 
			}

			X_LOG0(XLOG_DEBUG, _T("插件未实现IDVR_PrepareStream函数！"));
			return false;
		}
	}
}