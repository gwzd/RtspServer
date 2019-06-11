#ifdef DFJK_EXPORTS
#define DFJK_API __declspec(dllexport)
#else
#define DFJK_API __declspec(dllimport)
#endif

#define CALL_TYPE   __stdcall

#include "Global.h"

#ifdef __cplusplus
extern "C" {
#endif

//////////////////////////////////////////////////////////////////////////
//通用接口相关定义

//宏定义
#define MAX_NAME				64
#define MAX_CHANNEL				128
#define DEF_LEN_64				64

//报警类型
enum ALARMTYPE
{
	ALARM_IO = 0,			// I/O信号量报警，dwChnNo：报警序号，从1开始
	ALARM_DISKFULL = 1,		// 硬盘满，dwChnNo：硬盘号，从1开始
	ALARM_VIDEOLOST = 2,	//视频丢失，dwChnNo：视频通道号，从1开始
	ALARM_VIDEOMOTION = 3,	// 视频移动侦测，dwChnNo：视频通道号
	ALARM_DISKNOTFORMATTED = 4, //硬盘未格式化，dwChnNo：硬盘号
	ALARM_DISKERROR = 5,	//硬盘错误，dwChnNo：硬盘号
	ALARM_VIDEOSHELTER = 6,	// 视频遮挡报警，dwChnNo：视频通道号
	ALARM_VIDEOFORMAT = 7,	// 视频制式不匹配，dwChnNo：视频通道号
	ALARM_ILLEGALCONN = 8,	// 非法访问设备
};

//录像类型
enum RECORDTYPE
{
	MANUAL_REC = 0, //手动录像
	PLAN_REC = 1,   //计划录像
	ALARM_REC = 2,  //报警录像
	ALL_REC = 3,    //全部录像
};

//录像时间类型
typedef struct tagQueryTime
{
	unsigned int ufromyear;		// 起始年
	unsigned int ufrommonth;	// 起始月
	unsigned int ufromday;		// 起始日
	unsigned int ufromhour;		// 起始时
	unsigned int ufromminute;	// 起始分
	unsigned int ufromsecond;	// 起始秒
	unsigned int utoyear;		// 终止年
	unsigned int utomonth;		// 终止月
	unsigned int utoday;		// 终止日
	unsigned int utohour;		// 终止时
	unsigned int utominute;		// 终止分
	unsigned int utosecond;		// 终止秒
}QUERYTIME, *PQUERYTIME;

//模块信息
typedef struct { 
	char description[128];		//模块描述
	char ssoftversion[64];		//软件版本
	char smanufactuer[64];		//生产厂家  
	char DeviceTypeName[128];	//设备型号
}PLUGIN_INFO,*PPLUGIN_INFO;

//时间结构指针
typedef struct tagDevTime
{
	unsigned int ufromyear;		// 年
	unsigned int ufrommonth;	// 月
	unsigned int ufromday;		// 日
	unsigned int ufromhour;		// 时
	unsigned int ufromminute;	// 分
	unsigned int ufromsecond;	// 秒
}DEVTIME, *PDEVTIME;

//////////////////////////////////////////////////////////////////////////
//  1 系统相关函数
//////////////////////////////////////////////////////////////////////////
/**************************************************************************
(1-1) SYS_GetLastError
功能说明：接口错误信息获取
输入参数：nBuffLen - 缓冲区大小
输出参数：lpszErrorMsg-返回错误信息字符串缓冲区指针，空字符串表示没有错误。
函数返回：TRUE：执行成功 / FALSE：执行失败 
/**************************************************************************/
BOOL DFJK_API CALL_TYPE ISYS_GetLastError(LPTSTR lpszErrorMsg, UINT nMsgLen);

/**************************************************************************
(1-2) ISYS_GetInvalidHandleValue
功能说明：获取无效句柄用函数方式提供
输入参数：（无）
输出参数：phHandle-无效句柄值指针
函数返回：TRUE：执行成功 / FALSE：执行失败
/**************************************************************************/
BOOL DFJK_API CALL_TYPE ISYS_GetInvalidHandleValue(HANDLE *phHandle);

/**************************************************************************
(1-3) ISYS_GetPluginInfo
功能说明：获取本套接口库信息
输入参数：(无)
输出参数：pInfo- PLUGIN_INFO结构指针
函数返回：TRUE：执行成功 / FALSE：执行失败
/**************************************************************************/
BOOL DFJK_API CALL_TYPE ISYS_GetPluginInfo(PLUGIN_INFO *pInfo);

/**************************************************************************
(1-4) ISYS_Initialize
功能说明：模块初始化函数，模块加载后最先调用
输入参数：pReserved-必须为零
输出参数：(无)
函数返回：TRUE：执行成功 / FALSE：执行失败
/**************************************************************************/
BOOL DFJK_API CALL_TYPE ISYS_Initialize(void *pReserved, DWORD dwReserved);

/**************************************************************************
(1-5) ISYS_Uninitialize
功能说明：模块反初始化函数，在模块卸载前调用。用于释放本模块加载的所有资源。
输入参数：(无)
输出参数：(无)
函数返回：(无)
/**************************************************************************/
void DFJK_API CALL_TYPE ISYS_Uninitialize();

//释放内存
BOOL DFJK_API CALL_TYPE ISYS_FreeMemory(void* pMem);

//////////////////////////////////////////////////////////////////////////
//  5	独立回放类
//////////////////////////////////////////////////////////////////////////


/**************************************************************************
(5-1) IPLY_OpenFile
功能说明：打开本地录像文件
实现要求：必须实现
输入参数：lpszFileName -录像文件名（全路径名）
输出参数：phFileHandle-文件打开句柄指针
函数返回：TRUE：执行成功 / FALSE：执行失败
/**************************************************************************/
BOOL DFJK_API CALL_TYPE IPLY_OpenFile(LPCTSTR lpszFileName, HANDLE *phFileHandle);

/**************************************************************************
(5-2) IPLY_CloseFile
功能说明：关闭已打开的本地录像文件
实现要求：必须实现
输入参数：hFileHandle-录像文件句柄 
输出参数：(无)
函数返回：TRUE：执行成功 / FALSE：执行失败
/**************************************************************************/
BOOL DFJK_API CALL_TYPE IPLY_CloseFile(HANDLE hFileHandle);

/**************************************************************************
(5-3) IPLY_Play
功能说明：播放录像文件
实现要求：必须实现
输入参数：hFileHandle-打开的录像文件句柄
          hWnd-显示窗口句柄
输出参数：(无)
函数返回：TRUE：执行成功 / FALSE：执行失败
/**************************************************************************/
BOOL DFJK_API CALL_TYPE IPLY_Play(HANDLE hFileHandle, HWND hWnd);

/**************************************************************************
(5-4) IPLY_Stop
功能说明：停止播放录像文件
实现要求：必须实现
输入参数： hFileHandle-文件打开句柄
输出参数：（无）
函数返回：TRUE：执行成功 / FALSE：执行失败
/**************************************************************************/
BOOL DFJK_API CALL_TYPE IPLY_Stop(HANDLE hFileHandle);

/**************************************************************************
(4-10) IPLY_CapturePicture
功能说明：回放时抓取图片
实现要求：必须实现
输入参数：hFileHandle-录像播放句柄
lpszBmpFileName -图片保存文件名（全路径名）
输出参数：(无)
函数返回：TRUE：执行成功 / FALSE：执行失败
/**************************************************************************/
BOOL DFJK_API CALL_TYPE IPLY_CapturePicture(HANDLE hFileHandle,LPCTSTR lpszBmpFileName);

/**************************************************************************
(4-11) IPLY_RefreshPlay
功能说明：刷新显示播放窗口
实现要求：可选实现
输入参数：hFileHandle-录像播放句柄
输出参数：(无)
函数返回：TRUE：执行成功 / FALSE：执行失败
/**************************************************************************/
BOOL DFJK_API CALL_TYPE IPLY_RefreshPlay(HANDLE hFileHandle);

/**************************************************************************
(4-12) IPLY_Pause
功能说明：暂停播放录像
实现要求：必须实现
输入参数：hFileHandle-录像播放句柄
bPause-TRUE:暂停播放；FALSE:继续播放
输出参数：(无)
函数返回：TRUE：执行成功 / FALSE：执行失败
/**************************************************************************/
BOOL DFJK_API CALL_TYPE IPLY_Pause(HANDLE hFileHandle,BOOL bPause);

/**************************************************************************
(4-13) IPLY_Fast
功能说明：快进播放
实现要求：必须实现
输入参数：hFileHandle-录像播放句柄
输出参数：(无)
函数返回：TRUE：执行成功 / FALSE：执行失败
/**************************************************************************/
BOOL DFJK_API CALL_TYPE IPLY_Fast(HANDLE hFileHandle);

/**************************************************************************
(4-14) IPLY_Slow
功能说明：慢速播放
实现要求：必须实现
输入参数：hFileHandle-录像播放句柄
输出参数：(无)
函数返回：TRUE：执行成功 / FALSE：执行失败
/**************************************************************************/
BOOL DFJK_API CALL_TYPE IPLY_Slow(HANDLE hFileHandle);

/**************************************************************************
(4-15) IPLY_FastBack
功能说明：快速后退播放
实现要求：必须实现
输入参数：hFileHandle-录像播放句柄
输出参数：(无)
函数返回：TRUE：执行成功 / FALSE：执行失败
/**************************************************************************/
BOOL DFJK_API CALL_TYPE IPLY_FastBack(HANDLE hFileHandle);

/**************************************************************************
(4-16) IPLY_SlowBack
功能说明：慢速后退播放
实现要求：必须实现
输入参数：hFileHandle-录像播放句柄
输出参数：(无)
函数返回：TRUE：执行成功 / FALSE：执行失败
/**************************************************************************/
BOOL DFJK_API CALL_TYPE IPLY_SlowBack(HANDLE hFileHandle);

/**************************************************************************
(4-17) IPLY_Step
功能说明：单帧播放
实现要求：必须实现
输入参数：hFileHandle-录像播放句柄
输出参数：(无)
函数返回：TRUE：执行成功 / FALSE：执行失败
/**************************************************************************/
BOOL DFJK_API CALL_TYPE IPLY_Step(HANDLE hFileHandle);

/**************************************************************************
(4-??) IPLY_StepBack
功能说明：单帧倒放本地录像文件
实现要求：必须实现
输入参数：hFileHandle-录像播放句柄
输出参数：(无)
函数返回：TRUE：执行成功 / FALSE：执行失败
/**************************************************************************/
BOOL DFJK_API CALL_TYPE IPLY_StepBack(HANDLE hFileHandle);


/**************************************************************************
(4-18) IPLY_SetPlayPos
功能说明：设置文件播放位置
实现要求：必须实现
输入参数：hFileHandle-录像播放句柄
          fRelaPos-相对播放位置（0.0～1.0）
输出参数：(无)
函数返回：TRUE：执行成功 / FALSE：执行失败
/**************************************************************************/
BOOL DFJK_API CALL_TYPE IPLY_SetPlayPos(HANDLE hFileHandle, float fRelaPos);

/**************************************************************************
(4-19) IPLY_GetPlayPos
功能说明：获取当前播放位置
实现要求：必须实现
输入参数：hFileHandle-录像播放句柄
输出参数：pfRelaPos-当前播放位置指针
函数返回：TRUE：执行成功 / FALSE：执行失败
/**************************************************************************/
BOOL DFJK_API CALL_TYPE IPLY_GetPlayPos(HANDLE hFileHandle, float *pfRelaPos);

/**************************************************************************
(4-20) IPLY_GetTotalTime
功能说明：获取文件总的播放时间
实现要求：必须实现
输入参数：hFileHandle-录像播放句柄
pdwFileTime-文件总的播放时间（单位：秒）
输出参数：(无)
函数返回：TRUE：执行成功 / FALSE：执行失败
/**************************************************************************/
BOOL DFJK_API CALL_TYPE IPLY_GetTotalTime(HANDLE hFileHandle, DWORD *pdwFileTime);

/**************************************************************************
(4-21) IPLY_GetPlayedTime
功能说明：获取录像文件当前播放时间
实现要求：必须实现
输入参数：hFileHandle-录像播放句柄
pdwPlayedTime-录像文件当前播放时间（单位：秒）
输出参数：(无)
函数返回：TRUE：执行成功 / FALSE：执行失败
/**************************************************************************/
BOOL DFJK_API CALL_TYPE IPLY_GetPlayedTime(HANDLE hFileHandle,DWORD *pdwPlayedTime);

/**************************************************************************
(4-22) IPLY_GetTotalFrames
功能说明：获取录像文件总的帧数
实现要求：可选实现
输入参数：hFileHandle-录像播放句柄
pdwTotalFrames-获取文件总的播放帧数
输出参数：(无)
函数返回：TRUE：执行成功 / FALSE：执行失败
/**************************************************************************/
BOOL DFJK_API CALL_TYPE IPLY_GetTotalFrames(HANDLE hFileHandle, DWORD *pdwTotalFrames);

/**************************************************************************
(4-23) IPLY_GetPlayedFrames
功能说明：获取录像文件当前播放帧数
实现要求：可选实现
输入参数：hFileHandle-录像播放句柄
pdwPlayedFrames-当前播放的帧数
输出参数：(无)
函数返回：TRUE：执行成功 / FALSE：执行失败
/**************************************************************************/
BOOL DFJK_API CALL_TYPE IPLY_GetPlayedFrames(HANDLE hFileHandle, DWORD *pdwPlayedFrames);

/**************************************************************************
(4-24) IPLY_StartSound
功能说明：回放录像文件时播放声音
实现要求：可选实现
输入参数：hFileHandle-录像播放句柄
输出参数：(无)
函数返回：TRUE：执行成功 / FALSE：执行失败
/**************************************************************************/
BOOL DFJK_API CALL_TYPE IPLY_StartSound(HANDLE hFileHandle);

/**************************************************************************
(4-25) IPLY_StopSound
功能说明：回放录像文件时停止播放声音
实现要求：可选实现
输入参数：hFileHandle-录像播放句柄
输出参数：(无)
函数返回：TRUE：执行成功 / FALSE：执行失败
/**************************************************************************/
BOOL DFJK_API CALL_TYPE IPLY_StopSound(HANDLE hFileHandle);

/**************************************************************************
(4-26) IPLY_SetVolume
功能说明：回放时设置声音音量
实现要求：可选实现
输入参数：hFileHandle-录像播放句柄
dwVolume-音量值（0-100）
输出参数：(无)
函数返回：TRUE：执行成功 / FALSE：执行失败
/**************************************************************************/
BOOL DFJK_API CALL_TYPE IPLY_SetVolume(HANDLE hFileHandle, DWORD dwVolume);

/**************************************************************************
(4-27) IPLY_SetVideoParams
功能说明：设置回放时的图像参数
实现要求：可选实现
输入参数：hFileHandle-录像播放句柄
iBrightness-亮度0-255
iContrast-对比度0-255
iSaturation-饱和度0-255
iHue-色度0-255
bDefault - 若为1，则设置为默认参数（此时亮度等值无效）
输出参数：(无)
函数返回：TRUE：执行成功 / FALSE：执行失败
/**************************************************************************/
BOOL DFJK_API CALL_TYPE IPLY_SetVideoParams(HANDLE hFileHandle,
											int iBrightness,
											int iContrast,
											int iSaturation,
											int iHue,
											BOOL bDefault);

/**************************************************************************
(4-28) IPLY_GetVideoParams
功能说明：获取回放时的图像参数
实现要求：可选实现
输入参数：hFileHandle-录像播放句柄
输出参数：piBrightness-亮度0-255
piContrast-对比度0-255
piSaturation-饱和度0-255
piHue-色度0-255
函数返回：TRUE：执行成功 / FALSE：执行失败
/**************************************************************************/
BOOL DFJK_API CALL_TYPE IPLY_GetVideoParams(HANDLE hFileHandle,
											int *piBrightness,
											int *piContrast,
											int *piSaturation,
											int *piHue);

/**************************************************************************
(4-29) IPLY_OpenPlayStream
功能说明：建立流模式播放缓冲区
实现要求：必须实现
输入参数： 
pStreamHeader - 流数据数据头
dwHeaderLen   -  流数据数据头长度
dwStreamLen   -  流数据缓冲区长度
dwParam1	    - 保留参数
输出参数：
phStreamHandle - 返回建立流的句柄
函数返回：TRUE：执行成功 / FALSE：执行失败
/**************************************************************************/
BOOL DFJK_API CALL_TYPE IPLY_OpenPlayStream(BYTE* pStreamHeader,
											DWORD dwHeaderLen,
											DWORD dwStreamLen,
											DWORD dwParam1,
											HANDLE *phStreamHandle);

/**************************************************************************
(4-30) IPLY_InputStreamData
功能说明：向流模式播放缓冲区中加入接收的实时数据
实现要求：必须实现
输入参数： 
hStreamHandle- 流句柄
pBuffer -  输入实时数据
dwStreamLen   -  输入实时数据长度
dwParam1	    - 保留参数
dwParam2		- 保留参数
输出参数：
函数返回：TRUE：执行成功 / FALSE：执行失败
/**************************************************************************/
BOOL DFJK_API CALL_TYPE IPLY_InputStreamData(HANDLE hStreamHandle,
											 BYTE* pBuffer,
											 DWORD dwStreamLen,
											 DWORD* dwParam1,
											 DWORD dwParam2);

/**************************************************************************
(4-31) IPLY_StreamPlay
功能说明：以流模式播放
实现要求：必须实现
输入参数： 
hStreamHandle- 流句柄
pBuffer -  输入实时数据
dwStreamLen   -  输入实时数据长度
dwParam1	    - 保留参数
dwParam2		- 保留参数
输出参数：
函数返回：TRUE：执行成功 / FALSE：执行失败
/**************************************************************************/
BOOL DFJK_API CALL_TYPE IPLY_StreamPlay(HANDLE hStreamHandle,HWND hWnd);

/**************************************************************************
(4-32) IPLY_StreamPause
功能说明：在流模式播放时暂停或恢复
实现要求：可选实现
输入参数： 
hStreamHandle- 流句柄
pBuffer -  输入实时数据
dwStreamLen   -  输入实时数据长度
dwParam1	    - 保留参数
dwParam2		- 保留参数
输出参数：
函数返回：TRUE：执行成功 / FALSE：执行失败
/**************************************************************************/
BOOL DFJK_API CALL_TYPE IPLY_StreamPause(HANDLE hStreamHandle,BOOL bPause);

/**************************************************************************
(4-33) IPLY_ResetStreamBuffer
功能说明：复位流模式播放数据缓冲区
实现要求：必须实现
输入参数： 
hStreamHandle- 流句柄
输出参数：
函数返回：TRUE：执行成功 / FALSE：执行失败
/**************************************************************************/
BOOL DFJK_API CALL_TYPE IPLY_ResetStreamBuffer(HANDLE hStreamHandle);

/**************************************************************************
(4-34) IPLY_StreamRefreshPlay
功能说明：向流模式刷新播放窗口
实现要求：必须实现
输入参数： 
hStreamHandle- 流句柄
输出参数：
函数返回：TRUE：执行成功 / FALSE：执行失败
/**************************************************************************/
BOOL DFJK_API CALL_TYPE IPLY_StreamRefreshPlay( HANDLE hStreamHandle);


BOOL DFJK_API  CALL_TYPE IPLY_StreamStop(HANDLE hStreamHandle);


BOOL DFJK_API  CALL_TYPE IPLY_CloseStream(HANDLE hStreamHandle);


//用户自定义函数
//BOOL DFJK_API CALL_TYPE ISYS_CustomFunction(LPCTSTR pParamIn, DWORD dwParamInLen, LPTSTR *ppParamOut, DWORD* pdwParamOutLen);

//电子放大
BOOL DFJK_API  CALL_TYPE IPLY_SetDisplayRegion(HANDLE hPlayHandle,DWORD nRegionNum, RECT *pSrcRect, HWND hDestWnd, BOOL bEnable);
BOOL DFJK_API  CALL_TYPE IPLY_RefreshPlayEx(HANDLE hPlayHandle, DWORD nRegionNum);


//注册绘图回调函数
//绘图回调函数
typedef void(CALLBACK *DrawCallBack)(HANDLE hPlayHandle, HDC hDC, void* pUserData);
BOOL DFJK_API CALL_TYPE IPLY_RegisterDrawFun(HANDLE hPlayHandle, DrawCallBack callbackDraw, void* pUserData);

#ifdef __cplusplus
};
#endif
