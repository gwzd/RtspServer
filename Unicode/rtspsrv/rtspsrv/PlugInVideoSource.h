#pragma once
#include "IVideoSource.h"
#include "PlugInLoader.h"
#include "xlog.h"

namespace RTSP {
	namespace RTSPSrv {

		class CPlugInVideoSource : public VideoSource
		{
		protected:
			CPluginInfo* m_pInfo;
			HANDLE m_hDevHandle;
			HANDLE m_hChanHandle;
		public:
			CPlugInVideoSource();
			~CPlugInVideoSource();
			void SetPlugIn(CPluginInfo *pInfo) { m_pInfo = pInfo; }
			virtual bool ConnectDevice(LPCSTR dvrip, unsigned short port, int type, LPCSTR username, LPCSTR password, LPCSTR proxy);
			virtual void DisconnectDevice();
			virtual bool StartRealDataReceive(long lSessionId, long nChannel, long streamtype);
			virtual bool PrepareStream(long nChannel, long streamtype);
			virtual bool PtzControl(long nChannel, DWORD command, DWORD param1, DWORD param2, LPCSTR  lpszParam3);
			virtual int  GetLastError();
			virtual void StopRealDataReceive(long nChannel);
			virtual bool StartRemotePlayDataReceive(long lSessionId, long nChannel, LPCSTR lpszFile);
			virtual void StopRemotePlayDataReceive();
			virtual bool GetFileHeader(unsigned char* pBuffer, unsigned long& Length);
			virtual bool RemotePlayFast();
			virtual bool RemotePlaySlow();
			virtual bool RemotePlayNormal();
			virtual bool RemotePlayStep();
			virtual bool RemotePlayPause();
			virtual bool RemotePlayRestore();
			virtual bool RemotePlaySetPos(float fPos);
			virtual bool RemotePlayGetPos(float *fPos);
			virtual bool RemotePlayGetFileList(long nChannel, DWORD dwRecType, PQUERYTIME querytime, CString& result);

			/*bool RemotePlayGetPlayedTime(float *fPos);
			bool RemotePlayGetTotalTime(DWORD *nFrames);
			bool RemotePlayGetPlayedFrames(DWORD *nFrames);
			bool RemotePlayGetTotalFrames(DWORD *nFrames);
			*/
		};

		inline CPlugInVideoSource* createPlugInVideoSource(LPCSTR lpszTypeName, LPCSTR lpszSubTypeName)
		{
			//X_LOG2(XLOG_DEBUG, _T("创建插件型视频源对象:[%s],[%s]"), CString(lpszTypeName), CString(lpszSubTypeName));
			CPluginInfo *pInfo = CPlugInLoader::GetInstance()->GetPlugIn(lpszTypeName, lpszSubTypeName);
			if (pInfo != NULL)
			{
				//X_LOG2(XLOG_DEBUG,_T("加载插件:[%s],[%s] 成功！"),lpszTypeName,lpszSubTypeName);
				CPlugInVideoSource *pVideoSource = new CPlugInVideoSource();
				pVideoSource->SetPlugIn(pInfo);
				return pVideoSource;
			}

			//X_LOG2(XLOG_ERROR, _T("加载插件:[%s],[%s] 失败！"), lpszTypeName, lpszSubTypeName);
			return NULL;
		}
	}
}
