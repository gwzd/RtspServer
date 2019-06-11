#pragma once

#include "CommonStruct.h"

#define CALL_TYPE   WINAPI //建议改为PASCAL 


#define INVALID_DEV_HANDLE     0

#define MAXNUM_TRANS			8			//最大流媒体转发次数


//函数指针类型声明


//系统类
typedef BOOL(CALL_TYPE *ISYS_GetLastError)(LPTSTR lpszErrorMsg, UINT nMsgLen);
typedef BOOL(CALL_TYPE *ISYS_GetPluginInfo)(PLUGIN_INFO *pInfo);
typedef BOOL(CALL_TYPE *ISYS_Initialize)(void *pReserved, DWORD dwReserved);
typedef void (CALL_TYPE *ISYS_Uninitialize)();
typedef BOOL(CALL_TYPE *ISYS_FreeMemory)(void* pMem);
typedef BOOL(CALL_TYPE *ISYS_GetInvalidHandleValue)(HANDLE *phHandle);

//设备类
typedef BOOL(CALL_TYPE *IDVR_ConnectDevice)(LPCSTR lpszServerIP, DWORD dwPort, LPCSTR lpszUserName, LPCSTR lpszPassword, TRANSTYPE tType, HANDLE *phDevHandle, LPCSTR pExParamIn, DWORD dwExParamInLen);
typedef BOOL(CALL_TYPE *IDVR_DisConnectDevice)(HANDLE hDevHandle);
typedef BOOL(CALL_TYPE *IDVR_StartPreview)(HANDLE hDevHandle, HWND hWnd, int iChannel, int iStreamType, HANDLE *phChanHandle, LPCSTR pExParamIn, DWORD dwExParamInLen);

typedef void(_stdcall *RealDataCallBack)(HANDLE hChanHandle, BYTE *pBuffer, DWORD dwBufSize, DWORD dwCookie, DWORD dwParam);
typedef BOOL(CALL_TYPE *IDVR_StartRealData)(HANDLE hDevHandle, int iChannel, int iStreamType, HANDLE *phChanHandle, DWORD dwCookie,
	LPCSTR pExParamIn, DWORD dwExParamInLen,
	RealDataCallBack callbackHandler);
typedef BOOL(CALL_TYPE *IDVR_GetRealDataHeader)(HANDLE hChanHandle, unsigned char *pBuffer, DWORD dwBuffLen, DWORD *pdwHeaderLen);
typedef BOOL(CALL_TYPE *IDVR_StopRealData)(HANDLE hChanHandle);
typedef BOOL (CALL_TYPE *IDVR_PrepareStream)(HANDLE hDevHandle,long lChannel,int sTreamtype);


typedef BOOL(CALL_TYPE *IDVR_SwitchCamera)(HANDLE hDevHandle, LPCSTR lpszProtocol, int nAddress, int nCamNo, int nMonNo, SWITCHTYPE switchType, COMTYPE comType, DWORD dwParam, LPCSTR pExParamIn, DWORD dwExParamInLen);
typedef BOOL(CALL_TYPE *IDVR_StopPreview)(HANDLE hChanHandle);
typedef BOOL(CALL_TYPE *IDVR_CapturePicture)(HANDLE hChanHandle, LPCSTR csFileName, LPCSTR pExParamIn, DWORD dwExParamInLen);
typedef BOOL(CALL_TYPE *IDVR_StartRecord)(HANDLE hChanHandle, LPCSTR csFileName, LPCSTR pExParamIn, DWORD dwExParamInLen);
typedef BOOL(CALL_TYPE *IDVR_GetRecordState)(HANDLE hChanHandle, int *pnState);
typedef BOOL(CALL_TYPE *IDVR_StopRecord)(HANDLE hChanHandle);
typedef BOOL(CALL_TYPE *IDVR_SetVideoParams)(HANDLE hChanHandle, int iBrightness, int iContrast, int iSaturation, int iHue, BOOL bDefault);
typedef BOOL(CALL_TYPE *IDVR_GetVideoParams)(HANDLE hChanHandle, int *piBrightness, int *piContrast, int *piSaturation, int *piHue);
typedef BOOL(CALL_TYPE *IDVR_StartSound)(HANDLE hChanHandle, LPCSTR pExParamIn, DWORD dwExParamInLen);
typedef BOOL(CALL_TYPE *IDVR_SetVolume)(HANDLE hChanHandle, LONG lVolume);
typedef BOOL(CALL_TYPE *IDVR_StopSound)(HANDLE hChanHandle);
typedef BOOL(CALL_TYPE *IDVR_StartAudioPhone)(HANDLE hDevHandle, LPCSTR pExParamIn, DWORD dwExParamInLen);
typedef BOOL(CALL_TYPE *IDVR_StopAudioPhone)(HANDLE hDevHandle);
typedef BOOL(CALL_TYPE *IDVR_PTZControl)(HANDLE hDevHandle, int iChannel, CTRLTYPE command, LPCSTR lpszProtocol, DWORD dwParam1, DWORD dwParam2, LPCSTR pExParamIn, DWORD dwExParamInLen);
typedef BOOL(CALL_TYPE *IDVR_PTZControlByChannel)(HANDLE hChanHandle, CTRLTYPE command, DWORD dwParam1, DWORD dwParam2, LPCSTR pExParamIn, DWORD dwExParamInLen);
typedef BOOL(CALL_TYPE *IDVR_GetDeviceInfo)(HANDLE hDevHandle, PDEVICEINFO pDeviceInfo);
typedef BOOL(CALL_TYPE *IDVR_GetChannelInfo)(HANDLE hDevHandle, PCHANNELINFO pChannelInfo);
typedef BOOL(CALL_TYPE *IDVR_SetDeviceTime)(HANDLE hDevHandle, PDEVTIME pTime);
typedef BOOL(CALL_TYPE *IDVR_StartDeviceRecord)(HANDLE hChanHandle, long lChannel, long lRecordType, LPCSTR pExParamIn, DWORD dwExParamInLen);
typedef BOOL(CALL_TYPE *IDVR_StopDeviceRecord)(HANDLE hChanHandle, long lChannel);
typedef BOOL(CALL_TYPE *IDVR_GetDeviceRecordStatus)(HANDLE hDevHandle, long lChannel, int *piState, LPCSTR pExParamIn, DWORD dwExParamInLen);
typedef BOOL(CALL_TYPE *IDVR_ShowDeviceSettingDlg)(HANDLE hDevHandle, LPCSTR pExParamIn, DWORD dwExParamInLen);
typedef BOOL(CALL_TYPE *IDVR_RestartDevice)(HANDLE hDevHandle);

//报警和数据类
typedef BOOL(CALL_TYPE *IDVR_SetIOStatus)			(HANDLE hDevHandle, int iIOPort, int iValue, LPCSTR lpszProtocol, int nAddress, IOTYPE ioType, COMTYPE comType, LPCSTR pExParamIn, DWORD dwExParamInLen);
typedef BOOL(CALL_TYPE *IDVR_GetIOStatus)			(HANDLE hDevHandle, int iIOPort, int *piValue, LPCSTR lpszProtocol, int nAddress, IOTYPE ioType, COMTYPE comType, int nTimeout, LPCSTR pExParamIn, DWORD dwExParamInLen, LPSTR* ppExParamOut, DWORD* pdwExParamOutLen);
typedef BOOL(CALL_TYPE *IDVR_SetIOValue)			(HANDLE hDevHandle, int IOPort, float Value, LPCSTR lpszProtocol, int nAddress, IOTYPE ioType, COMTYPE comType, LPCSTR pExParamIn, DWORD dwExParamInLen);
typedef BOOL(CALL_TYPE *IDVR_GetIOValue)			(HANDLE hDevHandle, int IOPort, float *Value, LPCSTR lpszProtocol, int nAddress, IOTYPE ioType, COMTYPE comType, int nTimeout, LPCSTR pExParamIn, DWORD dwExParamInLen, LPSTR* ppExParamOut, DWORD* pdwExParamOutLen);

typedef int  (CALLBACK *CBF_AlarmInfo)(HANDLE hdevice, DWORD dwUser, int iAlarmType, DWORD dwChanNo, void* reserved);
typedef BOOL(CALL_TYPE *IDVR_SetAlarmSubscribe)	(HANDLE hDevHandle, DWORD dwUser, CBF_AlarmInfo cbf, LPCSTR pExParamIn, DWORD dwExParamInLen);
typedef BOOL(CALL_TYPE *IDVR_SetAlarmUnsubscribe)	(HANDLE hDevHandle);

//远程检索与回放类函数
typedef BOOL(CALL_TYPE *IDVR_QueryDevRecord)		(HANDLE hDevHandle, int iChannel, RECORDTYPE RecType, PQUERYTIME ptime, LPSTR* ppExParamOut, DWORD* pdwExParamOutLen);
typedef BOOL(CALL_TYPE *IDVR_CancelQueryDevRecord) (HANDLE hDevHandle);
typedef BOOL(CALL_TYPE *IDVR_DownloadFile)			(HANDLE hDevHandle, LPCSTR lpszSrcFileName, LPCSTR lpszLocalFileName, HANDLE* phDown);
typedef BOOL(CALL_TYPE *IDVR_StopDownloadFile)     (HANDLE hDownHandle);
typedef BOOL(CALL_TYPE *IDVR_GetDownloadPos)       (HANDLE hDownHandle, int *piCurPos);
typedef BOOL(CALL_TYPE *IDVR_DownloadFileByTime)	(HANDLE hDevHandle, int iChannel, PQUERYTIME pTime, LPCSTR lpszLocalFileName, HANDLE* phDownHandle);

typedef BOOL(CALL_TYPE *IDVR_RemotePlay)           (HANDLE hDevHandle, LPCSTR lpszSrcFileName, HWND hWnd, HANDLE *phPlay);
typedef BOOL(CALL_TYPE *IDVR_RemotePlayByTime)		(HANDLE hDevHandle, int iChannel, PQUERYTIME pTime, HWND hWnd, HANDLE* hPlay);
typedef BOOL(CALL_TYPE *IDVR_StopRemotePlay)		(HANDLE hPlay);


typedef BOOL(CALL_TYPE *IDVR_PauseRemotePlay)		(HANDLE hPlay);
typedef BOOL(CALL_TYPE *IDVR_ResumeRemotePlay)		(HANDLE hPlay);
typedef BOOL(CALL_TYPE *IDVR_StepRemotePlay)		(HANDLE hPlay);
typedef BOOL(CALL_TYPE *IDVR_FastRemotePlay)		(HANDLE hPlay);
typedef BOOL(CALL_TYPE *IDVR_SlowRemotePlay)		(HANDLE hPlay);
typedef BOOL(CALL_TYPE *IDVR_NormalRemotePlay)		(HANDLE hPlay);
typedef BOOL(CALL_TYPE *IDVR_GetRemotePlayPos)		(HANDLE hPlay, float *pfPos);
typedef BOOL(CALL_TYPE *IDVR_SetRemotePlayPos)		(HANDLE hPlay, float fPos);
typedef BOOL(CALL_TYPE *IDVR_RemotePlayCapturePicture)	(HANDLE hPlay, LPCSTR lpszFileName, LPCSTR pExParamIn, DWORD dwExParamInLen);
typedef BOOL(CALL_TYPE *IDVR_RemotePlaySaveData)	(HANDLE hPlay, LPCSTR lpszRecFileName, LPCSTR pExParamIn, DWORD dwExParamInLen);
typedef BOOL(CALL_TYPE *IDVR_StopRemotePlaySaveData)	(HANDLE hPlay);
typedef BOOL(CALL_TYPE *IDVR_SetRemotePlayDataCallBack)(HANDLE hPlay, DWORD dwCookie, LPCSTR pExParamIn, DWORD dwExParamInLen, RealDataCallBack callbackHandler);




//独立回放类函数
typedef BOOL(CALL_TYPE *IPLY_OpenFile)             (LPCSTR cFileName, HANDLE *phFileHandle);
typedef BOOL(CALL_TYPE *IPLY_CloseFile)			(HANDLE hFileHandle);
typedef BOOL(CALL_TYPE *IPLY_Play)				    (HANDLE hFileHandle, HWND hWnd);
typedef BOOL(CALL_TYPE *IPLY_Stop)				    (HANDLE hFileHandle);
typedef BOOL(CALL_TYPE *IPLY_CapturePicture)		(HANDLE hFileHandle, LPCSTR lpszBmpFileName);
typedef BOOL(CALL_TYPE *IPLY_RefreshPlay)		    (HANDLE hFileHandle);
typedef BOOL(CALL_TYPE *IPLY_Pause)				(HANDLE hFileHandle, BOOL bPause);
typedef BOOL(CALL_TYPE *IPLY_Fast)(HANDLE hFileHandle);
typedef BOOL(CALL_TYPE *IPLY_Slow)(HANDLE hFileHandle);
typedef BOOL(CALL_TYPE *IPLY_FastBack)(HANDLE hFileHandle);
typedef BOOL(CALL_TYPE *IPLY_SlowBack)(HANDLE hFileHandle);
typedef BOOL(CALL_TYPE *IPLY_Step)(HANDLE hFileHandle);
typedef BOOL(CALL_TYPE *IPLY_StepBack)(HANDLE hFileHandle);
typedef BOOL(CALL_TYPE *IPLY_SetPlayPos)(HANDLE hFileHandle, float fRelaPos);
typedef BOOL(CALL_TYPE *IPLY_GetPlayPos)(HANDLE hFileHandle, float *fRelaPos);
typedef BOOL(CALL_TYPE *IPLY_GetTotalTime)(HANDLE hFileHandle, DWORD *pdwFileTime);
typedef BOOL(CALL_TYPE *IPLY_GetPlayedTime)(HANDLE hFileHandle, DWORD *pdwPlayedTime);
typedef BOOL(CALL_TYPE *IPLY_GetTotalFrames)(HANDLE hFileHandle, DWORD *pdwTotalFrames);
typedef BOOL(CALL_TYPE *IPLY_GetPlayedFrames)(HANDLE hFileHandle, DWORD *pdwPlayedFrames);
typedef BOOL(CALL_TYPE *IPLY_StartSound)(HANDLE hFileHandle);
typedef BOOL(CALL_TYPE *IPLY_StopSound)(HANDLE hFileHandle);
typedef BOOL(CALL_TYPE *IPLY_SetVolume)(HANDLE hFileHandle, DWORD dwVolume);
typedef BOOL(CALL_TYPE *IPLY_SetVideoParams)(HANDLE hFileHandle, int iBrightness, int iContrast, int iSaturation, int iHue, BOOL bDefault);
typedef BOOL(CALL_TYPE *IPLY_GetVideoParams)(HANDLE hFileHandle, int *iBrightness, int *iContrast, int *iSaturation, int *iHue);

typedef BOOL(CALL_TYPE *IPLY_OpenPlayStream)		(unsigned char* pStreamHeader, DWORD dwHeaderLen, \
	DWORD dwStreamLen, DWORD dwParam1, HANDLE *phStreamHandle);
typedef BOOL(CALL_TYPE *IPLY_InputStreamData)		(HANDLE hStreamHandle, unsigned char* pBuffer, \
	DWORD dwStreamLen, DWORD* dwParam1, DWORD dwParam2);
typedef BOOL(CALL_TYPE *IPLY_StreamPlay)(HANDLE hStreamHandle, HWND hWnd);
typedef BOOL(CALL_TYPE *IPLY_StreamPause)(HANDLE hStreamHandle, BOOL bPause);
typedef BOOL(CALL_TYPE *IPLY_ResetStreamBuffer)(HANDLE hStreamHandle);
typedef BOOL(CALL_TYPE *IPLY_StreamRefreshPlay)(HANDLE hStreamHandle);
typedef BOOL(CALL_TYPE *IPLY_StreamStop)(HANDLE hStreamHandle);
typedef BOOL(CALL_TYPE *IPLY_CloseStream)(HANDLE hStreamHandle);

//------------------------------------------------------------------------------------------------------------------------------------	
//2012-07-04 lyq
//电子放大begin
//绘图回调函数
typedef void(_stdcall *DrawCallBack)(DWORD sid, HDC hDC, void* pUserData);
typedef BOOL(CALL_TYPE *IPLY_SetDisplayRegion)(HANDLE hPlayHandle, DWORD nRegionNum, RECT *pSrcRect, HWND hDestWnd, BOOL bEnable);
typedef BOOL(CALL_TYPE *IPLY_RefreshPlayEx)(HANDLE hPlayHandle, DWORD nRegionNum);
typedef BOOL(CALL_TYPE *IPLY_RegisterDrawFun)(HANDLE hPlayHandle, DrawCallBack callbackDraw, void* pUserData);
//电子放大end
//2012-07-04 lyq
//------------------------------------------------------------------------------------------------------------------------------------	

//////////////////////////////////////////////////////////////////////////

typedef struct {
	ISYS_GetLastError			pfnISYS_GetLastError;
	ISYS_GetPluginInfo			pfnISYS_GetPluginInfo;
	ISYS_Initialize				pfnISYS_Initialize;
	ISYS_Uninitialize			pfnISYS_Uninitialize;
	ISYS_FreeMemory				pfnISYS_FreeMemory;
	ISYS_GetInvalidHandleValue	pfnISYS_GetInvalidHandleValue;

	IDVR_ConnectDevice			pfnIDVR_ConnectDevice;
	IDVR_DisConnectDevice		pfnIDVR_DisConnectDevice;
	IDVR_StartPreview			pfnIDVR_StartPreview;
	IDVR_StartRealData			pfnIDVR_StartRealData;
	IDVR_GetRealDataHeader		pfnIDVR_GetRealDataHeader;
	IDVR_StopRealData			pfnIDVR_StopRealData;
	IDVR_PrepareStream			pfnIDVR_PrepareStream;

	IDVR_SwitchCamera			pfnIDVR_SwitchCamera;
	IDVR_StopPreview			pfnIDVR_StopPreview;
	IDVR_CapturePicture			pfnIDVR_CapturePicture;
	IDVR_StartRecord			pfnIDVR_StartRecord;
	IDVR_GetRecordState			pfnIDVR_GetRecordState;
	IDVR_StopRecord				pfnIDVR_StopRecord;
	IDVR_SetVideoParams			pfnIDVR_SetVideoParams;
	IDVR_GetVideoParams			pfnIDVR_GetVideoParams;
	IDVR_StartSound				pfnIDVR_StartSound;
	IDVR_SetVolume				pfnIDVR_SetVolume;
	IDVR_StopSound				pfnIDVR_StopSound;
	IDVR_StartAudioPhone		pfnIDVR_StartAudioPhone;
	IDVR_StopAudioPhone			pfnIDVR_StopAudioPhone;
	IDVR_PTZControl				pfnIDVR_PTZControl;
	IDVR_PTZControlByChannel	pfnIDVR_PTZControlByChannel;
	IDVR_GetDeviceInfo			pfnIDVR_GetDeviceInfo;
	IDVR_GetChannelInfo			pfnIDVR_GetChannelInfo;
	IDVR_SetDeviceTime			pfnIDVR_SetDeviceTime;
	IDVR_StartDeviceRecord		pfnIDVR_StartDeviceRecord;
	IDVR_StopDeviceRecord		pfnIDVR_StopDeviceRecord;
	IDVR_GetDeviceRecordStatus  pfnIDVR_GetDeviceRecordStatus;
	IDVR_ShowDeviceSettingDlg	pfnIDVR_ShowDeviceSettingDlg;
	IDVR_RestartDevice			pfnIDVR_RestartDevice;

	//报警和数据类
	IDVR_SetIOStatus			pfnIDVR_SetIOStatus;
	IDVR_GetIOStatus			pfnIDVR_GetIOStatus;
	IDVR_SetIOValue				pfnIDVR_SetIOValue;
	IDVR_GetIOValue				pfnIDVR_GetIOValue;
	IDVR_SetAlarmSubscribe		pfnIDVR_SetAlarmSubscribe;
	IDVR_SetAlarmUnsubscribe	pfnIDVR_SetAlarmUnsubscribe;


	//远程检索、回放、下载等
	IDVR_QueryDevRecord				pfnIDVR_QueryDevRecord;
	IDVR_CancelQueryDevRecord		pfnIDVR_CancelQueryDevRecord;
	IDVR_DownloadFile				pfnIDVR_DownloadFile;
	IDVR_StopDownloadFile			pfnIDVR_StopDownloadFile;
	IDVR_GetDownloadPos				pfnIDVR_GetDownloadPos;
	IDVR_DownloadFileByTime			pfnIDVR_DownloadFileByTime;
	IDVR_RemotePlay					pfnIDVR_RemotePlay;
	IDVR_RemotePlayByTime			pfnIDVR_RemotePlayByTime;
	IDVR_StopRemotePlay				pfnIDVR_StopRemotePlay;
	IDVR_PauseRemotePlay			pfnIDVR_PauseRemotePlay;
	IDVR_ResumeRemotePlay			pfnIDVR_ResumeRemotePlay;
	IDVR_StepRemotePlay				pfnIDVR_StepRemotePlay;
	IDVR_FastRemotePlay				pfnIDVR_FastRemotePlay;
	IDVR_SlowRemotePlay				pfnIDVR_SlowRemotePlay;
	IDVR_NormalRemotePlay			pfnIDVR_NormalRemotePlay;
	IDVR_GetRemotePlayPos			pfnIDVR_GetRemotePlayPos;
	IDVR_SetRemotePlayPos			pfnIDVR_SetRemotePlayPos;
	IDVR_RemotePlayCapturePicture	pfnIDVR_RemotePlayCapturePicture;
	IDVR_RemotePlaySaveData			pfnIDVR_RemotePlaySaveData;
	IDVR_StopRemotePlaySaveData		pfnIDVR_StopRemotePlaySaveData;
	IDVR_SetRemotePlayDataCallBack	pfnIDVR_SetRemotePlayDataCallBack;

	//独立回放类函数
	IPLY_OpenFile			pfnIPLY_OpenFile;
	IPLY_CloseFile			pfnIPLY_CloseFile;
	IPLY_Play				pfnIPLY_Play;
	IPLY_Stop				pfnIPLY_Stop;
	IPLY_CapturePicture		pfnIPLY_CapturePicture;
	IPLY_RefreshPlay		pfnIPLY_RefreshPlay;
	IPLY_Pause				pfnIPLY_Pause;
	IPLY_Fast				pfnIPLY_Fast;
	IPLY_Slow				pfnIPLY_Slow;
	IPLY_FastBack			pfnIPLY_FastBack;
	IPLY_SlowBack			pfnIPLY_SlowBack;
	IPLY_Step				pfnIPLY_Step;
	IPLY_StepBack			pfnIPLY_StepBack;
	IPLY_SetPlayPos			pfnIPLY_SetPlayPos;
	IPLY_GetPlayPos			pfnIPLY_GetPlayPos;
	IPLY_GetTotalTime		pfnIPLY_GetTotalTime;
	IPLY_GetPlayedTime		pfnIPLY_GetPlayedTime;
	IPLY_GetTotalFrames		pfnIPLY_GetTotalFrames;
	IPLY_GetPlayedFrames	pfnIPLY_GetPlayedFrames;
	IPLY_StartSound			pfnIPLY_StartSound;
	IPLY_StopSound			pfnIPLY_StopSound;
	IPLY_SetVolume			pfnIPLY_SetVolume;
	IPLY_SetVideoParams		pfnIPLY_SetVideoParams;
	IPLY_GetVideoParams		pfnIPLY_GetVideoParams;

	IPLY_OpenPlayStream		pfnIPLY_OpenPlayStream;
	IPLY_InputStreamData	pfnIPLY_InputStreamData;
	IPLY_StreamPlay			pfnIPLY_StreamPlay;
	IPLY_StreamPause		pfnIPLY_StreamPause;
	IPLY_ResetStreamBuffer	pfnIPLY_ResetStreamBuffer;
	IPLY_StreamRefreshPlay	pfnIPLY_StreamRefreshPlay;
	IPLY_StreamStop			pfnIPLY_StreamStop;
	IPLY_CloseStream		pfnIPLY_CloseStream;

	//------------------------------------------------------------------------------------------------------------------------------------	
	//2012-07-04 lyq
	//电子放大begin
	IPLY_SetDisplayRegion	pfnIPLY_SetDisplayRegion;
	IPLY_RefreshPlayEx		pfnIPLY_RefreshPlayEx;
	IPLY_RegisterDrawFun	pfnIPLY_RegisterDrawFun;
	//电子放大end		
	//2012-07-04 lyq
	//------------------------------------------------------------------------------------------------------------------------------------	

	int ExportNums;
} FUNC_PTR_LIST, *PFUNC_PTR_LIST;
