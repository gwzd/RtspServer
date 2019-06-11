#ifndef __CLIENT_H__
#define __CLIENT_H__

#ifdef CLIENT_EXPORTS
#define CLIENT_API __declspec(dllexport)
#else
#define CLIENT_API __declspec(dllimport)
#endif

#define CALL_TYPE   WINAPI //建议改为PASCAL 
#include "clntsink.h"
#include "Prototypes.h"


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


#ifdef __cplusplus
extern "C" {
#endif


typedef DWORD HSESSION;

/*
  192.0.0.145是流媒体服务器的IP地址
  rtsp://192.0.0.145/stream.264      //VOD 相对路径
  rtsp://192.0.0.145/d:/流媒体/hikstream/files/stream.264      //VOD 绝对路径
  rtsp://192.0.0.12/192.0.1.200:8000:DS-8000HC:0:0:admin:12345/av_stream //实时流
*/

 
int CLIENT_API CALL_TYPE  InitStreamClientLib(void);  //初始化ACE
int CLIENT_API CALL_TYPE  FiniStreamClientLib(void);  //关闭ACE

HSESSION CLIENT_API CALL_TYPE  HIKS_CreatePlayer(IHikClientAdviseSink* pSink, 
												 void* pWndSiteHandle, 
												 pDataRec pRecFunc, 
												 LPCTSTR lpszType,
												 pMsgBack pMsgFunc=0,
												 int TransMethod=0
												 );//创建Player

int CLIENT_API CALL_TYPE  HIKS_OpenURL(HSESSION hSession,
									   const char* pszURL,
									   int iusrdata,
									   const char* oPenfilename);//打开URL

int CLIENT_API CALL_TYPE  HIKS_Play(HSESSION hSession);//播放

int CLIENT_API CALL_TYPE  HIKS_RandomPlay(HSESSION hSession,unsigned long timepos);//随机位置播放,用于视频的前后拖动

int CLIENT_API CALL_TYPE  HIKS_Pause(HSESSION hSession); //暂停

int CLIENT_API CALL_TYPE  HIKS_Resume(HSESSION hSession);//恢复

int CLIENT_API CALL_TYPE  HIKS_Stop(HSESSION hSession);//停止,销毁Player

int CLIENT_API CALL_TYPE  HIKS_GetCurTime(HSESSION hSession,unsigned long *utime);//获取当前的播放时间，1/64秒为单位, 目前返回都是0

int CLIENT_API CALL_TYPE  HIKS_ChangeRate(HSESSION hSession,int scale);//改变播放速率,在快进或慢进的时候用

int CLIENT_API CALL_TYPE  HIKS_Destroy(HSESSION hSession);//销毁Player

int CLIENT_API CALL_TYPE  HIKS_GetVideoParams(HSESSION hSession, 
											  int *piBrightness, 
											  int *piContrast, 
											  int *piSaturation, 
											  int *piHue);

int CLIENT_API CALL_TYPE  HIKS_SetVideoParams(HSESSION hSession,  
											  int iBrightness,
											  int iContrast, 
											  int iSaturation, 
											  int iHue,
											  BOOL bDefault);

int CLIENT_API CALL_TYPE  HIKS_PTZControl(HSESSION hSession, 
										  unsigned int ucommand, 
										  int iparam1,
										  int iparam2, 
										  int iparam3, 
										  int iparam4,
										  LPCSTR lpszParam5);

int CLIENT_API CALL_TYPE  HIKS_SetVolume(HSESSION hSession,
										 unsigned short volume);//设置音量 参数volume的范围是0-65535

int CLIENT_API CALL_TYPE  HIKS_OpenSound(HSESSION hSession, 
										 bool bExclusive=false);

int CLIENT_API CALL_TYPE  HIKS_CloseSound(HSESSION hSession);

int CLIENT_API CALL_TYPE  HIKS_ThrowBFrameNum(HSESSION hSession,
											  unsigned int nNum);//丢B祯

int CLIENT_API CALL_TYPE  HIKS_GrabPic(HSESSION hSession,
									   const char* szPicFileName, 
									   unsigned short byPicType);

CLIENT_API  LPCTSTR CALL_TYPE HIKS_GetLastError();

int CLIENT_API CALL_TYPE  HIKS_GetRealDataHeader(HSESSION hSession,											
												char *pBuffer,
												DWORD dwBuffLen,
												DWORD *pdwHeaderLen);//得到文件头

int CLIENT_API CALL_TYPE HIKS_PlayBackControl(HSESSION hSession,
											  DWORD dwControlCode,
											  DWORD dwInValue,
											  DWORD *lpOutValue);//远程回放控制函数
//dwControlCode 按照定义来
//当 dwControlCode=104时，dwInValue为窗口句柄。

int CLIENT_API CALL_TYPE HIKS_QueryDevRecord(const char* strURL,
											 int RecType,
											 char* pTime,
// 											 DWORD dwbufferLen,
// 											 char* ppExParamOut,
                                             LPSTR* ppExParamOut,
											 DWORD* pdwExParamOutLen   
											 );//获取远程文件列表

BOOL CLIENT_API CALL_TYPE HIKS_ISYS_FreeMemory(void* pMem);

//------------------------------------------------------------------------------------------------------------------------------------	
//2012-07-04 lyq	
//电子放大begin
BOOL CLIENT_API  CALL_TYPE HIKS_SetDisplayRegion(HSESSION hSession,
												 DWORD nRegionNum, 
												 RECT *pSrcRect, 
												 HWND hDestWnd, 
												 BOOL bEnable);

BOOL CLIENT_API  CALL_TYPE HIKS_RefreshPlayEx(HSESSION hSession,
											  DWORD nRegionNum);


BOOL CLIENT_API CALL_TYPE HIKS_RegisterDrawFun(HSESSION hSession, 
											   DrawCallBack callbackDraw,
											   void* pUserData);
//电子放大end
//2012-07-04 lyq
//------------------------------------------------------------------------------------------------------------------------------------	
	
#ifdef __cplusplus
}
#endif

#endif



