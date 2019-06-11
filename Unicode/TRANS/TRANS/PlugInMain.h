
// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the DFJK_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// DFJK_API functions as being imported from a DLL, wheras this DLL sees symbols
// defined with this macro as being exported.
#ifdef DFJK_EXPORTS
#define DFJK_API __declspec(dllexport)
#else
#define DFJK_API __declspec(dllimport)
#endif

#define CALL_TYPE   WINAPI //建议改为PASCAL 


#define NET_DVR_PLAYSTART			101//开始播放
#define NET_DVR_GETPLAYLIST			102//获取文件列表
#define NET_DVR_PLAYPAUSE			103//暂停播放
#define NET_DVR_PLAYRESTART			104//恢复播放
#define NET_DVR_PLAYFAST			105//快放
#define NET_DVR_PLAYSLOW			106//慢放
#define NET_DVR_PLAYNORMAL			107//正常速度
#define NET_DVR_PLAYFRAME			108//单帧放

#define NET_DVR_PLAYSETPOS			112//改变文件回放的进度
#define NET_DVR_PLAYGETPOS			113//获取文件回放的进度
#define NET_DVR_PLAYGETTIME			114//获取当前已经播放的时间
#define NET_DVR_PLAYGETFRAME		115//获取当前已经播放的帧数
#define NET_DVR_GETTOTALFRAMES  	116//获取当前播放文件总的帧数
#define NET_DVR_GETTOTALTIME    	117//获取当前播放文件总的时间
#define NET_DVR_THROWBFRAME			120//丢B帧

#include "winsock2.h"
#include "CommonFunc.h"
#include "RtspClient.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
BOOL DFJK_API CALL_TYPE IDVR_ConnectDevice(LPCTSTR  lpszServerIP, 
											DWORD dwPort,
											LPCTSTR lpszUserName, 
											LPCTSTR lpszPassword, 
											TRANSTYPE tType, 
											HANDLE *phDevHandle, 
											LPCTSTR pExParamIn,
											DWORD dwExParamInLen); 

BOOL DFJK_API CALL_TYPE IDVR_DisConnectDevice(HANDLE hDevHandle);
BOOL DFJK_API CALL_TYPE ISYS_GetPluginInfo(PLUGIN_INFO *pInfo);
BOOL DFJK_API CALL_TYPE ISYS_Initialize(void *pvReserved,DWORD reserved);
BOOL DFJK_API CALL_TYPE ISYS_Uninitialize();

BOOL DFJK_API CALL_TYPE ISYS_GetLastError(LPTSTR lpszErrorMsg,
                                          UINT nMsgLen);


BOOL DFJK_API CALL_TYPE IDVR_StartRealData(HANDLE hDevHandle,
										   int iChannel, 
										   int iStreamType,
										   HANDLE *phChanHandle, 
										   DWORD dwCookie, 
										   LPCTSTR pExParamIn, 
										   DWORD dwExParamInLen, 
										   RealDataCallBack callbackHandler);

BOOL DFJK_API CALL_TYPE IDVR_StopRealData(HANDLE hChanHandle);
BOOL DFJK_API CALL_TYPE IDVR_GetRealDataHeader(HANDLE hChanHandle,
									           unsigned char *pBuffer,
										       DWORD dwBuffLen,
										       DWORD *pdwHeaderLen);

//10.30
BOOL DFJK_API CALL_TYPE IDVR_QueryDevRecord ( HANDLE hDevHandle, 
											 int iChannel, 
											 RECORDTYPE RecType, 
											 PQUERYTIME pTime, 
											 LPTSTR *ppExParamOut, 
											 DWORD* pdwExParamOutLen);
BOOL DFJK_API CALL_TYPE IDVR_PauseRemotePlay (HANDLE hPlay);	
BOOL DFJK_API CALL_TYPE IDVR_ResumeRemotePlay (HANDLE hPlay);
//BOOL DFJK_API CALL_TYPE IDVR_StepRemotePlay (HANDLE hPlay);	
BOOL DFJK_API CALL_TYPE IDVR_FastRemotePlay (HANDLE hPlay);
BOOL DFJK_API CALL_TYPE IDVR_SlowRemotePlay (HANDLE hPlay);
BOOL DFJK_API CALL_TYPE IDVR_NormalRemotePlay (HANDLE hPlay);
BOOL DFJK_API CALL_TYPE IDVR_GetRemotePlayPos (HANDLE hPlay, 
											   float *pfPos);
BOOL DFJK_API CALL_TYPE IDVR_SetRemotePlayPos (HANDLE hPlay,
											   float fPos);

BOOL DFJK_API CALL_TYPE IDVR_RemotePlay (HANDLE hDevHandle, 
					                     LPCTSTR lpszSrcFileName, 
					                     HWND hWnd, 
					                     HANDLE* hPlay);

BOOL DFJK_API CALL_TYPE IDVR_StopRemotePlay (HANDLE hPlay);


BOOL DFJK_API CALL_TYPE IDVR_SetRemotePlayDataCallBack (HANDLE hPlay, 
														DWORD dwCookie, 
														LPCTSTR pExParamIn, 
														DWORD dwExParamInLen, 
														RealDataCallBack callbackHandler);

BOOL DFJK_API CALL_TYPE IDVR_RemotePlaySaveData (HANDLE hPlay, 
												 LPCTSTR lpszRecFileName, 
												 LPCTSTR pExParamIn,
												 DWORD dwExParamInLen);


BOOL DFJK_API CALL_TYPE IDVR_StopRemotePlaySaveData (HANDLE hPlay);


BOOL DFJK_API CALL_TYPE ISYS_FreeMemory(void* pMem);

BOOL DFJK_API CALL_TYPE IDVR_RemotePlayByTime (HANDLE hDevHandle, 
											   int iChannel, 
											   PQUERYTIME pTime, 
											   HWND hWnd, 
											   HANDLE* hPlay);

BOOL DFJK_API CALL_TYPE IDVR_StepRemotePlay (HANDLE hPlay);
*/
#ifdef __cplusplus
}
#endif
