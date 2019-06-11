#pragma once
#include<string>
#include "Prototypes.h"

namespace RTSP {
	namespace RTSPSrv {

class VideoSource
{
public:
	virtual bool ConnectDevice(LPCSTR dvrip, unsigned short port, int type, LPCSTR username, LPCSTR password, LPCSTR proxy = NULL) = 0;
	virtual void DisconnectDevice()=0;
	virtual bool StartRealDataReceive(long lSessionId, long nChannel,long streamtype)=0;
	virtual void StopRealDataReceive(long nChannel)=0;
	virtual int  GetLastError()=0;	
	virtual bool PtzControl( long nChannel,DWORD command,DWORD param1,DWORD param2,LPCSTR  lpszParam3)=0;
	virtual bool StartRemotePlayDataReceive(long lSessionId,long nChannel,LPCSTR lpszFile)=0;
	virtual void StopRemotePlayDataReceive()=0;
	virtual bool PrepareStream(long nChannel,long streamtype)=0;
	virtual bool GetFileHeader(unsigned char* pBuffer, unsigned long& Length)=0;
	virtual bool RemotePlayFast()=0;
	virtual bool RemotePlaySlow()=0;
	virtual bool RemotePlayNormal()=0;
	virtual bool RemotePlayStep()=0;
	virtual bool RemotePlayPause()=0;
	virtual bool RemotePlayRestore()=0;
	virtual bool RemotePlaySetPos(float fPos)=0;
	virtual bool RemotePlayGetPos(float *fPos)=0;
		
	/*virtual bool RemotePlayGetPlayedTime(float *fPos)=0;
	virtual bool RemotePlayGetTotalTime(DWORD *nFrames)=0;
	virtual bool RemotePlayGetPlayedFrames(DWORD *nFrames)=0;
	virtual bool RemotePlayGetTotalFrames(DWORD *nFrames)=0;*/	

	virtual bool RemotePlayGetFileList(long nChannel,DWORD dwRecType,PQUERYTIME querytime,CString& result)=0;
	static void OnRealDataArrival(long SessionID, unsigned char* pBuffer, unsigned long Length);
};

	}
}
