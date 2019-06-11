
#pragma once

#ifdef DFJK_EXPORTS
#define DFJK_API __declspec(dllexport)
#else
#define DFJK_API __declspec(dllimport)
#endif

#define CALL_TYPE   WINAPI //建议改为PASCAL 

//#include "Global.h"
#include "CommonStruct.h"

#ifdef __cplusplus
extern "C" {
#endif


//-------------------------------------------------------------------------------------
//        结构定义
//-------------------------------------------------------------------------------------
//数据流回调函数
typedef void(CALLBACK *RealDataCallBack)(HANDLE hChanHandle, 										 
										 BYTE *pBuffer,
										 DWORD dwBufSize,
										 DWORD dwCookie, 
										DWORD dwParam2);

//绘图回调函数
typedef void(CALLBACK *DrawCallBack)(HANDLE hPlayHandle, HDC hDC, void* pUserData);

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
//  2 设备相关函数
//////////////////////////////////////////////////////////////////////////

/**************************************************************************
(2-1) IDVR_ConnectDevice
功能说明：连接站端设备
输入参数：	lpszRvuIP-Rvu IP地址
dwPort-Rvu网络端口
lpszUserName-用户名
lpszPassWord-密码
iType-网络传输类型: 0-TCP, 1:UDP, 2: 组播, 3: RTP
pExParamIn－扩展参数
dwExParamInLen－扩展参数长度
输出参数：	hDevHandle-设备句柄
函数返回：TRUE：执行成功 / FALSE：执行失败
/**************************************************************************/
BOOL DFJK_API CALL_TYPE IDVR_ConnectDevice(LPCSTR  lpszRvuIP, 
										   DWORD dwPort,
										   LPCSTR lpszUserName,
										   LPCSTR lpszPassWord,
										   TRANSTYPE tType, 
										   HANDLE *hDevHandle,
										   LPCSTR pExParamIn,
										   DWORD dwExParamInLen);

/**************************************************************************
(2-2) IDVR_DisConnectDevice
功能说明：断开站端设备连接
输入参数：hDevHandle-设备句柄
输出参数：(无)
函数返回：TRUE：执行成功 / FALSE：执行失败
/**************************************************************************/
BOOL DFJK_API CALL_TYPE IDVR_DisConnectDevice(HANDLE hDevHandle);

/**************************************************************************
(2-3) IDVR_StartPreview
功能说明：启动该通道图像预览
输入参数：hDevHandle-设备句柄
hWnd-预览图像窗口
iChannel-指定预览通道(通道号从1开始)
iStreamType-码流类型，0：主码流，1：次码流
pExParamIn－扩展参数，格式化参数字符串
dwExParamInLen－扩展参数长度
输出参数：	hChanHandle-打开的通道句柄
函数返回：TRUE：执行成功 / FALSE：执行失败
/**************************************************************************/
BOOL DFJK_API CALL_TYPE IDVR_StartPreview(HANDLE hDevHandle,
										  HWND hWnd,
										  int iChannel,
										  int iStreamType,
										  HANDLE *hChanHandle, 
										  LPCSTR pExParamIn,
										  DWORD dwExParamInLen);

/**************************************************************************
(2-4) IDVR_StartRealData
功能说明：启动视频通道图像实时数据回调接收
输入参数：hDevHandle-设备句柄
iChannel-指定预览通道(通道号从1开始)
iStreamType-码流类型，0：主码流，1：次码流
pExParamIn－扩展参数，格式化参数字符串[8]
dwExParamInLen－扩展参数长度
callbackHandler 获取实时音视频流的回调函数
输出参数：phChanHandle-打开的通道句柄
函数返回：TRUE：执行成功 / FALSE：执行失败
/**************************************************************************/


BOOL DFJK_API  CALL_TYPE IDVR_StartRealData(HANDLE hDevHandle,
											int iChannel,
											int iStreamType,
											HANDLE *phChanHandle,
                                            DWORD dwCookie,
											LPCSTR pExParamIn,
											DWORD dwExParamInLen,
											RealDataCallBack callbackHandler);
					
/**************************************************************************
(2-5) IDVR_GetRealDataHeader
功能说明：获取视频通道图像实时数据的数据头
实现要求：必须实现
输入参数：hChanHandle -通道句柄
pBuffer - 接收数据头缓冲区
dwBuffLen -接收数据头缓冲区长度
输出参数：pdwHeaderLen 返回实际数据头长度
函数返回：TRUE：执行成功 / FALSE：执行失败
/**************************************************************************/
BOOL DFJK_API  CALL_TYPE IDVR_GetRealDataHeader(HANDLE hChanHandle,
												unsigned char *pBuffer,
												DWORD dwBuffLen,
												DWORD *pdwHeaderLen);

/**************************************************************************
(2-6) IDVR_StopRealData
功能说明：停止视频通道图像实时数据回调接收
输入参数：hChanHandle -通道句柄
输出参数： 
函数返回：TRUE：执行成功 / FALSE：执行失败 
/**************************************************************************/
BOOL DFJK_API  CALL_TYPE IDVR_StopRealData(HANDLE hChanHandle);

/**************************************************************************
(2-7) IDVR_SwitchCamera
功能说明：实现对摄像机的切换
实现要求：可选实现
输入参数：hDevHandle-设备句柄
hChanHandle-通道句柄
pExParamIn－扩展参数，格式化参数字符串[8]
dwExParamInLen－扩展参数长度
输出参数：(无)
函数返回：TRUE：执行成功 / FALSE：执行失败
/**************************************************************************/
BOOL DFJK_API  CALL_TYPE IDVR_SwitchCamera(HANDLE hDevHandle, 
										   LPCSTR lpszProtocol, int nAddress, 
										   int nCamNo, int nMonNo, SWITCHTYPE switchType,
										   COMTYPE comType, DWORD dwParam,
										   LPCSTR pExParamIn, DWORD dwExParamInLen);

/**************************************************************************
(2-8) IDVR_StopPreview
功能说明：停止该通道图像预览
输入参数：hChanHandle-通道句柄
输出参数：(无)
函数返回：TRUE：执行成功 / FALSE：执行失败
/**************************************************************************/
BOOL DFJK_API  CALL_TYPE IDVR_StopPreview(HANDLE hChanHandle);

/**************************************************************************
(2-9) IDVR_CapturePicture
功能说明：实时播放视频时抓取指定通道图片
输入参数：hChanHandle - 通道句柄
lpszFileName -- 图片保存指定文件名,最长100字符
pExParamIn－扩展参数，格式化参数字符串[8]
dwExParamInLen－扩展参数长度
输出参数：（无）
函数返回：TRUE：执行成功 / FALSE：执行失败 
/**************************************************************************/
BOOL DFJK_API CALL_TYPE IDVR_CapturePicture(HANDLE hChanHandle,
											LPCSTR lpszFileName,
											LPCSTR pExParamIn,
											DWORD dwExParamInLen);

/**************************************************************************
(2-10) IDVR_StartRecord
功能说明：启动录像（在本地进行录像）
输入参数：hChanHandle - 通道句柄
lpszFileName -- 图片保存指定文件名,最长100字符
pExParamIn－扩展参数，格式化参数字符串[8]
dwExParamInLen－扩展参数长度
输出参数：（无）
函数返回：TRUE：执行成功 / FALSE：执行失败 
/**************************************************************************/
BOOL DFJK_API CALL_TYPE IDVR_StartRecord(HANDLE hChanHandle,
										 LPCSTR lpszFileName, 
										 LPCSTR pExParamIn, 
										 DWORD dwExParamInLen);

/**************************************************************************
(2-11) IDVR_GetRecordState
功能说明：获取通道的本地录像状态
输入参数：hChanHandle - 通道句柄
输出参数：pnState ------ 该通道录像状态，0：未录像 1：正在录像
函数返回：TRUE：执行成功 / FALSE：执行失败
/**************************************************************************/
BOOL DFJK_API CALL_TYPE IDVR_GetRecordState(HANDLE hChanHandle,int *pnState);

/**************************************************************************
(2-12) IDVR_StopRecord
功能说明：停止录像 （本地录像）
输入参数：hChanHandle - 通道句柄
输出参数：（无）
函数返回：TRUE：执行成功 / FALSE：执行失败
/**************************************************************************/
BOOL DFJK_API CALL_TYPE IDVR_StopRecord(HANDLE hChanHandle);

/**************************************************************************
(2-13) IDVR_SetVideoParams
功能说明：设置通道视频参数 
实现要求：必须实现
输入参数：hChanHandle-通道句柄
iBrightness-亮度0-255
iContrast-对比度0-255
iSaturation-饱和度0-255
iHue-色度0-255
bDefault：若为1，则设置为默认参数（此时亮度等值无效）
输出参数：(无)
函数返回：TRUE：执行成功 / FALSE：执行失败
/**************************************************************************/
BOOL DFJK_API CALL_TYPE IDVR_SetVideoParams(HANDLE hChanHandle, 
											int iBrightness, 
											int iContrast, 
											int iSaturation,
											int iHue, 
											BOOL bDefault);

/**************************************************************************
(2-14) IDVR_GetVideoParams
功能说明：设置通道视频参数 
实现要求：必须实现
输入参数：hChanHandle-通道句柄
iBrightness-亮度0-255
iContrast-对比度0-255
iSaturation-饱和度0-255
iHue-色度0-255
输出参数：(无)
函数返回：TRUE：执行成功 / FALSE：执行失败
/**************************************************************************/
BOOL DFJK_API CALL_TYPE IDVR_GetVideoParams(HANDLE hChanHandle,
											int *piBrightness,
											int *piContrast,
											int *piSaturation,
											int *piHue);

/**************************************************************************
(2-15) IDVR_StartSound
功能说明：打开通道声音监听 
实现要求：可选实现
输入参数：hChanHandle-通道句柄
pExParamIn－扩展参数，格式化参数字符串[8]
dwExParamInLen－扩展参数长度
输出参数：(无)
函数返回：TRUE：执行成功 / FALSE：执行失败
/**************************************************************************/
BOOL DFJK_API CALL_TYPE IDVR_StartSound(HANDLE hChanHandle, LPCSTR pExParamIn, DWORD dwExParamInLen);

/**************************************************************************
(2-16) IDVR_SetVolume
功能说明：设置通道监听声音音量 
实现要求：可选实现
输入参数：hChanHandle-通道句柄
          lVolume-音量值(0 - 100)
输出参数：(无)
函数返回：TRUE：执行成功 / FALSE：执行失败
/**************************************************************************/
BOOL DFJK_API CALL_TYPE IDVR_SetVolume(HANDLE hChanHandle,LONG lVolume);

/**************************************************************************
(2-17) IDVR_StopSound
功能说明：停止通道声音监听 
实现要求：可选实现
输入参数：hChanHandle-通道句柄
输出参数：(无)
函数返回：TRUE：执行成功 / FALSE：执行失败
/**************************************************************************/
BOOL DFJK_API CALL_TYPE IDVR_StopSound(HANDLE hChanHandle);

/**************************************************************************
(2-18) IDVR_StartAudioPhone
功能说明：启动语音对讲
实现要求：可选实现
输入参数：hDevHandle-设备句柄
pExParamIn－扩展参数，格式化参数字符串[8]
dwExParamInLen－扩展参数长度
输出参数：(无)
函数返回：TRUE：执行成功 / FALSE：执行失败
/**************************************************************************/
BOOL DFJK_API CALL_TYPE IDVR_StartAudioPhone(HANDLE hDevHandle, LPCSTR pExParamIn, DWORD dwExParamInLen);

/**************************************************************************
(2-19) IDVR_StopAudioPhone
功能说明：停止语音对讲 
实现要求：可选实现
输入参数：hDevHandle-设备句柄
输出参数：(无)
函数返回：TRUE：执行成功 / FALSE：执行失败
/**************************************************************************/
BOOL DFJK_API CALL_TYPE IDVR_StopAudioPhone(HANDLE hDevHandle);

/**************************************************************************
(2-20) IDVR_PTZControl
功能说明：云台等辅助设备的控制操作 
实现要求：必须实现
输入参数：command-控制命令类型
lpszProtocol-协议
dwParam1-云台水平速度（范围：0-10）；预置位命令时为预置位号
dwParam2-云台垂直速度（范围：0-10）
pExParamIn－扩展参数，格式化参数字符串[8]。可用于传入PTZ控制辅助信息
扩展字符串TAG字典：
PROTOCOL     云台控制协议
dwExParamInLen－扩展参数长度
输出参数：(无)
函数返回：TRUE：执行成功 / FALSE：执行失败
/**************************************************************************/
BOOL DFJK_API CALL_TYPE IDVR_PTZControl(HANDLE hDevHandle,
										int iChannel,
										CTRLTYPE command,
										LPCSTR lpszProtocol,
										DWORD dwParam1,
										DWORD dwParam2,
										LPCSTR pExParamIn, 
										DWORD dwExParamInLen);

/**************************************************************************
(2-??) IDVR_PTZControlByChannel
功能说明：根据视频通道预览句柄控制云台等辅助设备 
实现要求：必须实现
输入参数：hChanHandle - 视频通道句柄
          其他参数定义同函数IDVR_PTZControl
输出参数：(无)
函数返回：TRUE：执行成功 / FALSE：执行失败
/**************************************************************************/
BOOL DFJK_API CALL_TYPE IDVR_PTZControlByChannel(HANDLE hChanHandle,CTRLTYPE command, DWORD dwParam1, DWORD dwParam2, LPCSTR pExParamIn, DWORD dwExParamInLen);


/**************************************************************************
(2-21) IDVR_GetDeviceInfo
功能说明：获取设备相关信息
输入参数：hDevHandle-设备句柄
输出参数：pDeviceInfo-设备状态信息存储结构
函数返回：TRUE：执行成功 / FALSE：执行失败
/**************************************************************************/
BOOL DFJK_API CALL_TYPE IDVR_GetDeviceInfo(HANDLE hDevHandle,PDEVICEINFO pDeviceInfo);

/**************************************************************************
(2-22) IDVR_GetChannelInfo
功能说明：获取设备通道相关信息
输入参数：hDevHandle-设备句柄
输出参数：pDeviceInfo-通道信息存储结构
函数返回：TRUE：执行成功 / FALSE：执行失败
/**************************************************************************/
BOOL DFJK_API CALL_TYPE IDVR_GetChannelInfo(HANDLE hDevHandle,PCHANNELINFO pChannelInfo);

/**************************************************************************
(2-23) IDVR_SetDeviceTime
功能说明：远程校时站端视频主机（DVR）
实现要求：必须实现
输入参数：hDevHandle-设备句柄
          pTime-时间结构指针
输出参数：(无)
函数返回：TRUE：执行成功 / FALSE：执行失败 
/**************************************************************************/
BOOL DFJK_API CALL_TYPE IDVR_SetDeviceTime(HANDLE hDevHandle, PDEVTIME pTime);

/**************************************************************************
(2-24) IDVR_StartDeviceRecord
功能说明：远程启动现场视频主机（DVR）某通道进行录像
实现要求：必须实现
输入参数：hDevHandle-设备句柄
lChannel-通道号（从1开始）
lRecordType－暂时无效
pExParamIn－扩展参数，格式化参数字符串[8]
dwExParamInLen－扩展参数长度
输出参数：(无)
函数返回：TRUE：执行成功 / FALSE：执行失败 
/**************************************************************************/
BOOL DFJK_API CALL_TYPE IDVR_StartDeviceRecord(HANDLE hDevHandle,
											   long lChannel,
											   long lRecordType,
											   LPCSTR pExParamIn,
											   DWORD dwExParamInLen);

/**************************************************************************
(2-25) IDVR_StopDeviceRecord
功能说明：远程停止现场视频主机（DVR）某通道录像
实现要求：必须实现
输入参数：hDevHandle-设备句柄
lChannel-通道号（从1开始）
输出参数：(无)
函数返回：TRUE：执行成功 / FALSE：执行失败 
/**************************************************************************/
BOOL DFJK_API CALL_TYPE IDVR_StopDeviceRecord(HANDLE hDevHandle,long lChannel);

/**************************************************************************
(2-26) IDVR_GetDeviceRecordStatus
功能说明：获取现场视频主机（DVR）某通道录像状态
实现要求：可选实现
输入参数：hDevHandle-设备句柄
lChannel-通道号（从1开始）
pExParamIn－扩展参数，格式化参数字符串[8]
dwExParamInLen－扩展参数长度
输出参数：piState-状态值指针（状态值1为正在录像，0为没有录像）
函数返回：TRUE：执行成功 / FALSE：执行失败 
/**************************************************************************/
BOOL DFJK_API CALL_TYPE IDVR_GetDeviceRecordStatus(HANDLE hDevHandle,
												   long lChannel,
												   int *piState,
												   LPCSTR pExParamIn,
												   DWORD dwExParamInLen);

/**************************************************************************
(2-27) IDVR_ShowDeviceSettingDlg
功能说明：打开设备设置对话框
实现要求：可选实现
输入参数：hDevHandle-设备句柄
pExParamIn－扩展参数，格式化参数字符串[8]
dwExParamInLen－扩展参数长度
输出参数：（无）
函数返回：TRUE：执行成功 / FALSE：执行失败
/**************************************************************************/
BOOL DFJK_API CALL_TYPE IDVR_ShowDeviceSettingDlg(HANDLE hDevHandle, LPCSTR pExParamIn, DWORD dwExParamInLen);

/**************************************************************************
(2-28) IDVR_RestartDevice
功能说明：远程重启站端设备
实现要求：必须实现
输入参数：hDevHandle-设备句柄
输出参数：（无）
函数返回：TRUE：执行成功 / FALSE：执行失败 
/**************************************************************************/
BOOL DFJK_API CALL_TYPE IDVR_RestartDevice (HANDLE hDevHandle);

/**************************************************************************
(2-29) IDVR_GetDeviceState
功能说明：获取RVU设备工作状态
实现要求：必须实现
输入参数：hDevHandle-设备句柄
输出参数：pStat --- 返回RVU工作状态结构指针
函数返回：TRUE：执行成功 / FALSE：执行失败
/**************************************************************************/
BOOL DFJK_API CALL_TYPE IDVR_GetDeviceState (HANDLE hDevHandle, DEV_STAT* pStat);

//////////////////////////////////////////////////////////////////////////
//  3  报警和数据类函数
//////////////////////////////////////////////////////////////////////////

/**************************************************************************
(3-1) IDVR_SetIOStatus
功能说明：设置站端设备端口的状态量（数字量）值 
实现要求：可选实现
输入参数：hDevHandle-设备句柄
          iIOPort-输出端口
          iValue-设置的量值
pExParamIn－扩展参数，格式化参数字符串[8]。可用于传入I/O设备相关参数
dwExParamInLen－扩展参数长度
输出参数：(无)
函数返回：TRUE：执行成功 / FALSE：执行失败
/**************************************************************************/
BOOL DFJK_API CALL_TYPE IDVR_SetIOStatus(HANDLE hDevHandle, int iIOPort, int iValue,
										 LPCSTR lpszProtocol, int nAddress, IOTYPE ioType, COMTYPE comType,
										 LPCSTR pExParamIn, DWORD dwExParamInLen);

/**************************************************************************
(3-2) IDVR_GetIOStatus
功能说明：获取站端设备端口的状态量（数字量）值 
实现要求：可选实现
输入参数：hDevHandle-设备句柄
iIOPort-输入端口
pExParamIn－扩展参数，格式化参数字符串[8]。可用于传入I/O设备相关参数
dwExParamInLen－扩展参数长度
输出参数：*piValue-获取的量值
ppExParamOut－扩展输出参数，格式化参数字符串。可用于获取I/O设备相关值
pdwExParamOutLen－扩展输出参数长度
函数返回：TRUE：执行成功 / FALSE：执行失败
/**************************************************************************/
BOOL DFJK_API CALL_TYPE IDVR_GetIOStatus(HANDLE hDevHandle,
										 int iIOPort,
										 int *piValue,
										 LPCSTR lpszProtocol, int nAddress, IOTYPE ioType, COMTYPE comType, int nTimeout,
										 LPCSTR pExParamIn,
										 DWORD dwExParamInLen,
										 LPSTR* ppExParamOut,
										 DWORD* pdwExParamOutLen);

/**************************************************************************
(3-3) IDVR_SetIOValue
功能说明：设置站端设备端口的模拟量值 
实现要求：可选实现
输入参数：hDevHandle-设备句柄
          iIOPort-输出端口
          fValue-设置的量值
pExParamIn－扩展参数，格式化参数字符串[8]。可用于传入I/O设备相关参数
dwExParamInLen－扩展参数长度
输出参数：(无)
函数返回：TRUE：执行成功 / FALSE：执行失败
/**************************************************************************/
BOOL DFJK_API CALL_TYPE IDVR_SetIOValue(HANDLE hDevHandle,
										int iIOPort,
										float fValue,
										LPCSTR lpszProtocol, int nAddress, IOTYPE ioType, COMTYPE comType,
										LPCSTR pExParamIn,
										DWORD dwExParamInLen);

/**************************************************************************
(3-4) IDVR_GetIOValue
功能说明：获取站端设备端口的模拟量值
实现要求：可选实现
输入参数：hDevHandle-设备句柄
          IOPort-输入端口
pExParamIn－扩展参数，格式化参数字符串[8]。可用于传入I/O设备相关参数
dwExParamInLen－扩展参数长度
输出参数：pfValue-获取的量值
ppExParamOut－扩展输出参数，返回格式化参数字符串[8]。可用于获取I/O设备相关值
pdwExParamOutLen－扩展输出参数长度
函数返回：TRUE：执行成功 / FALSE：执行失败
/**************************************************************************/
BOOL DFJK_API CALL_TYPE IDVR_GetIOValue(HANDLE hDevHandle,
										int iIOPort,
										float *pfValue, 
										LPCSTR lpszProtocol, int nAddress, IOTYPE ioType, COMTYPE comType, int nTimeout,
										LPCSTR pExParamIn, 
										DWORD dwExParamInLen, 
										LPSTR* ppExParamOut, 
										DWORD* pdwExParamOutLen);

/**************************************************************************
(3-5) IDVR_SetAlarmSubscribe
功能说明：预订现场设备报警信息
实现要求：可选实现
输入参数：hDevHandle-设备句柄
dwUser - 用户数据
pExParamIn－扩展参数，格式化参数字符串[8]
dwExParamInLen－扩展参数长度
Cbf-回调函数，用于实时传出报警信息。定义为：
typedef int(CALLBACK *CBF_AlarmInfo)(HANDLE hdevice, DWORD dwUser, DWORD dwAlarmType, DWORD dwChanNo, void *pReserved);
dwUser-主函数传递的用户参数
dwAlarmType-报警点类型编号，具体为：
	0:信号量报警。dwChanNo:报警序号1开始,
	1:硬盘满； 4:硬盘未格式化；5:硬盘出错。dwChanNo:硬盘号,1开始
	2:视频信号丢失；3:移动侦测；6:视频遮挡；7:视频制式不匹配。dwChanNo:通道号,1开始
	8:非法访问服务器。
dwChanNo-报警点关联的通道编号
输出参数：(无)
函数返回：TRUE：执行成功 / FALSE：执行失败
/**************************************************************************/
typedef int(CALLBACK *CBF_AlarmInfo)(HANDLE hdevice, DWORD dwUser, ALARMTYPE alarmType, DWORD dwChanNo, void *pReserved);

BOOL DFJK_API CALL_TYPE IDVR_SetAlarmSubscribe(HANDLE hDevHandle,
											   DWORD dwUser,
											   CBF_AlarmInfo cbf,
											   LPCSTR pExParamIn,
											   DWORD dwExParamInLen);

/**************************************************************************
(3-6) IDVR_SetAlarmUnsubscribe
功能说明：停止预订现场设备报警信息
实现要求：可选实现
输入参数：hDevHandle-设备句柄
输出参数：(无)
函数返回：TRUE：执行成功 / FALSE：执行失败
函数版本：V1.0
其他说明：(无)
/**************************************************************************/
BOOL DFJK_API CALL_TYPE IDVR_SetAlarmUnsubscribe(HANDLE hDevHandle);

/**************************************************************************
(3-7) IDVR_AlarmSet
功能说明：对现场报警进行布防与撤防
实现要求：必须实现
输入参数：hDevHandle-设备句柄
输出参数：(无)
函数返回：TRUE：执行成功 / FALSE：执行失败
/**************************************************************************/
BOOL DFJK_API CALL_TYPE IDVR_AlarmSet(HANDLE hDevHandle, 
									  int iChannel, 
									  LPCSTR lpszProtocol, 
									  int nDevAddr, 
									  COMTYPE comType, 
									  BOOL bEnable, 
									  DWORD dwParam);

/**************************************************************************
(3-8) IDVR_GetAlarmSetStatus
功能说明：获取现场报警设备布防/撤防状态
实现要求：必须实现
输入参数：hDevHandle-设备句柄
		  iChannel-报警输入号（取值：0、1、2、…）
		  lpszProtocol-报警设备协议，若RVU自身报警，则为空字符串或"EDVR"
		  nDevAddr - 报警设备地址（1、2、3、......）
		  comType- 报警设备所连接的RVU通讯串口类型,定义参考函数IDVR_SetIOStatus
		  dwParam - 保留参数，传入0
输出参数：pEnabled - TRUE:布防，FALSE：撤防（当iChannel>0时有效）
		  ppExParamOut－扩展输出参数，返回格式化参数字符串[8]。用于获取多个报警的布防/撤防状态
		  pdwExParamOutLen－扩展输出参数长度
函数返回：TRUE：执行成功 / FALSE：执行失败
/**************************************************************************/
BOOL CALL_TYPE IDVR_GetAlarmSetStatus(HANDLE hDevHandle, int iChannel, LPCSTR lpszProtocol, 
									  int nDevAddr, COMTYPE comType, BOOL* pEnabled, 
									  LPSTR* ppExParamOut, DWORD* pdwExParamOutLen);

/**************************************************************************
(3-9) IDVR_SerialStart
功能说明：打开设备透明串口
实现要求：必须实现
输入参数：  hDevHandle-设备句柄
			lSerialPort -串口号，1-232 串口，2-485 串口
			dwUser - 用户数据
			fSerialDataCallBack：数据回调函数，
回调函数参数说明： 
	hSerial - 串口句柄
	pRecvDataBuffer：存放接收到数据的缓冲区指针 
	dwBufSize：缓冲区的大小 
	dwUser：上面传入的用户数据
输出参数：phSerial - 输出的串口句柄
函数返回：TRUE：执行成功 / FALSE：执行失败
/**************************************************************************/
typedef void(CALLBACK *CBF_SerialDataCallBack)(HANDLE hSerial, char *pRecvDataBuffer, DWORD dwBufSize, DWORD dwUser);
BOOL DFJK_API CALL_TYPE IDVR_SerialStart(HANDLE hDevHandle, 
									     LONG lSerialPort,
										 CBF_SerialDataCallBack cbf,
										 DWORD dwUser, 
										 HANDLE* phSerial);

/**************************************************************************
(3-10) IDVR_SerialSend
功能说明：向透明串口发送数据
实现要求：必须实现
输入参数：  hSerial-透明串口句柄
			lChannel -硬盘录像机的视频通道号, 以 485 建立透明通道时,指明往哪个通道送数据；以 232 建立透明通道时设置成 0;
			pSendBuf：要发送的数据缓冲区指针 
			dwBufSize：缓冲区大小 
输出参数：（无）
函数返回：TRUE：执行成功 / FALSE：执行失败
/**************************************************************************/
BOOL DFJK_API CALL_TYPE IDVR_SerialSend(HANDLE hSerial, LONG lChannel, char *pSendBuf, DWORD dwBufSize);

/**************************************************************************
(3-11) IDVR_SerialStop
功能说明：关闭透明串口
实现要求：必须实现
输入参数：hSerial-透明串口句柄
输出参数：（无）
函数返回：TRUE：执行成功 / FALSE：执行失败
/**************************************************************************/
BOOL DFJK_API CALL_TYPE IDVR_SerialStop(HANDLE hSerial);

//////////////////////////////////////////////////////////////////////////
//  4	远程检索、回放、下载等
//////////////////////////////////////////////////////////////////////////

/**************************************************************************
(4-1) IDVR_QueryDevRecord
功能说明：查询RVU设备内的符合条件的录像文件
实现要求：必须实现
输入参数：hDevHandle-设备句柄，
iChannel-通道编号(从1开始)，
RecType-录像类型，
输出参数：ppExParamOut－扩展输出参数，返回XML字符串。
函数返回：TRUE：执行成功 / FALSE：执行失败
/**************************************************************************/
BOOL DFJK_API CALL_TYPE IDVR_QueryDevRecord(HANDLE hDevHandle,
											int iChannel,
											RECORDTYPE RecType,
											PQUERYTIME ptime,
											LPSTR* ppExParamOut,
											DWORD* pdwExParamOutLen);

/**************************************************************************
(4-2) IDVR_CancelQueryDevRecord
功能说明：取消正在进行的录像文件的查询请求
实现要求：可选实现
输入参数：hDevHandle-设备句柄
输出参数：(无)
函数返回：TRUE：执行成功 / FALSE：执行失败
/**************************************************************************/
BOOL DFJK_API CALL_TYPE IDVR_CancelQueryDevRecord(HANDLE hDevHandle);

/**************************************************************************
(4-3) IDVR_DownloadFile
功能说明：下载RVU中的录像文件到本地
实现要求：必须实现
输入参数：hDevHandle-设备句柄
lpszSrcFileName-选定的现场录像文件名（全路径名）
lpszLocalFileName -本地存储文件名（全路径名）
输出参数：phDownHandle - 下载句柄
函数返回：TRUE：执行成功 / FALSE：执行失败
/**************************************************************************/
BOOL DFJK_API CALL_TYPE IDVR_DownloadFile(HANDLE hDevHandle, LPCSTR lpszSrcFileName, LPCSTR lpszLocalFileName, HANDLE* phDownHandle);

/**************************************************************************
(4-4) IDVR_StopDownloadFile
功能说明：停止下载现场设备中的录像文件
实现要求：可选实现
输入参数：hDownHandle - 下载句柄
输出参数：(无)
函数返回：TRUE：执行成功 / FALSE：执行失败

/**************************************************************************/
BOOL DFJK_API CALL_TYPE IDVR_StopDownloadFile(HANDLE hDownHandle);

/**************************************************************************
(4-5) IDVR_GetDownloadPos
功能说明：获取当前下载进度
实现要求：可选实现
输入参数：hDownHandle - 下载句柄
输出参数：piCurPos-获得的当前下载进度值指针 （<0:失败，0～99:下载进度，100:下载结束，>100:由于网络原因或DVR忙,下载异常终止）
函数返回：TRUE：执行成功 / FALSE：执行失败
/**************************************************************************/
BOOL DFJK_API CALL_TYPE IDVR_GetDownloadPos(HANDLE hDownHandle, int *piCurPos);

/**************************************************************************
(4-?) IDVR_DownloadFileByTime
功能说明：按时间下载现场设备中的录像文件到本地
实现要求：必须实现
输入参数：hDevHandle-设备句柄
iChannel - 视频通道号，1开始
pTime - 时间范围
lpszLocalFileName -本地存储文件名（全路径名）
输出参数：phDownHandle - 下载句柄
函数返回：TRUE：执行成功 / FALSE：执行失败
/**************************************************************************/
BOOL DFJK_API CALL_TYPE IDVR_DownloadFileByTime(HANDLE hDevHandle, int iChannel,  PQUERYTIME  pTime, LPCSTR lpszLocalFileName, HANDLE* phDownHandle);


/**************************************************************************
(4-7) IDVR_RemotePlay
功能说明：回放远程录像文件（RVU内的录像文件）
实现要求：必须实现
输入参数：hDevHandle-设备句柄
lpszSrcFileName - RVU内的录像文件名
hWnd - 回放窗口句柄
输出参数：hPlay - 回放句柄
函数返回：TRUE：执行成功 / FALSE：执行失败
/**************************************************************************/
BOOL DFJK_API CALL_TYPE IDVR_RemotePlay (HANDLE hDevHandle, LPCSTR lpszSrcFileName, HWND hWnd, HANDLE* hPlay);

/**************************************************************************
(4-8) IDVR_RemotePlayByTime
功能说明：按时间回放远程录像文件
实现要求：必须实现
输入参数：hDevHandle-设备句柄
iChannel - 视频通道号，1开始
pTime - 时间范围
hWnd - 回放窗口句柄
输出参数：hPlay - 回放句柄
函数返回：TRUE：执行成功 / FALSE：执行失败
/**************************************************************************/
BOOL DFJK_API CALL_TYPE IDVR_RemotePlayByTime (HANDLE hDevHandle, int iChannel, PQUERYTIME pTime, HWND hWnd, HANDLE* hPlay);

/**************************************************************************
(4-9) IDVR_StopRemotePlay
功能说明：停止回放远程录像文件
实现要求：必须实现
输入参数：hPlay - 回放句柄
输出参数：(无)
函数返回：TRUE：执行成功 / FALSE：执行失败
/**************************************************************************/
BOOL DFJK_API CALL_TYPE IDVR_StopRemotePlay(HANDLE hPlay);

/**************************************************************************
(4-10) IDVR_PauseRemotePlay
功能说明：暂停回放远程录像文件
实现要求：必须实现
输入参数：hPlay - 回放句柄
输出参数：(无)
函数返回：TRUE：执行成功 / FALSE：执行失败
/**************************************************************************/
BOOL DFJK_API CALL_TYPE IDVR_PauseRemotePlay(HANDLE hPlay);

/**************************************************************************
(4-11) IDVR_ResumeRemotePlay
功能说明：恢复回放远程录像文件（暂停后恢复播放）
实现要求：必须实现
输入参数：hPlay - 回放句柄
输出参数：(无)
函数返回：TRUE：执行成功 / FALSE：执行失败
/**************************************************************************/
BOOL DFJK_API CALL_TYPE IDVR_ResumeRemotePlay(HANDLE hPlay);

/**************************************************************************
(4-12) IDVR_StepRemotePlay
功能说明：单帧回放远程录像文件
实现要求：必须实现
输入参数：hPlay - 回放句柄
输出参数：(无)
函数返回：TRUE：执行成功 / FALSE：执行失败
/**************************************************************************/
BOOL DFJK_API CALL_TYPE IDVR_StepRemotePlay(HANDLE hPlay);

/**************************************************************************
(4-13) IDVR_FastRemotePlay
功能说明：快进回放远程录像文件
实现要求：必须实现
输入参数：hPlay - 回放句柄
输出参数：(无)
函数返回：TRUE：执行成功 / FALSE：执行失败
/**************************************************************************/
BOOL DFJK_API CALL_TYPE IDVR_FastRemotePlay(HANDLE hPlay);

/**************************************************************************
(4-14) IDVR_SlowRemotePlay
功能说明：慢速回放远程录像文件
实现要求：必须实现
输入参数：hPlay - 回放句柄
输出参数：(无)
函数返回：TRUE：执行成功 / FALSE：执行失败
/**************************************************************************/
BOOL DFJK_API CALL_TYPE IDVR_SlowRemotePlay(HANDLE hPlay);

/**************************************************************************
(4-15) IDVR_NormalRemotePlay
功能说明：正常速度回放远程录像文件（用于快、慢、单帧播放后恢复）
实现要求：必须实现
输入参数：hPlay - 回放句柄
输出参数：(无)
函数返回：TRUE：执行成功 / FALSE：执行失败
/**************************************************************************/
BOOL DFJK_API CALL_TYPE IDVR_NormalRemotePlay(HANDLE hPlay);

/**************************************************************************
(4-16) IDVR_GetRemotePlayPos
功能说明：获取远程回放进度
实现要求：必须实现
输入参数：hPlay - 回放句柄
输出参数：pfPos - 进度（0.0～1.0）
函数返回：TRUE：执行成功 / FALSE：执行失败
/**************************************************************************/
BOOL DFJK_API CALL_TYPE IDVR_GetRemotePlayPos(HANDLE hPlay, float *pfPos);

/**************************************************************************
(4-17) IDVR_SetRemotePlayPos
功能说明：设置远程回放进度（拖动播放）
实现要求：必须实现
输入参数：hPlay - 回放句柄
fPos：进度值（0.0～1.0）
输出参数：(无)
函数返回：TRUE：执行成功 / FALSE：执行失败
/**************************************************************************/
BOOL DFJK_API CALL_TYPE IDVR_SetRemotePlayPos(HANDLE hPlay, float fPos);

/**************************************************************************
(4-18) IDVR_RemotePlayCapturePicture
功能说明：远程回放时抓图
实现要求：必须实现
输入参数：hPlay - 回放句柄
lpszFileName -  图片保存指定文件名,最长100字符
pExParamIn－扩展参数，格式化参数字符串[8]
dwExParamInLen－扩展参数长度
输出参数：(无)
函数返回：TRUE：执行成功 / FALSE：执行失败
/**************************************************************************/
BOOL DFJK_API CALL_TYPE IDVR_RemotePlayCapturePicture(HANDLE hPlay,
												  LPCSTR lpszFileName,
												  LPCSTR pExParamIn,
												  DWORD dwExParamInLen);

/**************************************************************************
(4-19) IDVR_RemotePlaySaveData
功能说明：远程回放时保存数据
实现要求：推荐实现
输入参数：hPlay - 回放句柄
lpszRecFileName -  保存数据的录像文件名
pExParamIn－扩展参数，格式化参数字符串[8]
dwExParamInLen－扩展参数长度
输出参数：(无)
函数返回：TRUE：执行成功 / FALSE：执行失败
/**************************************************************************/
BOOL DFJK_API CALL_TYPE IDVR_RemotePlaySaveData(HANDLE hPlay,
												LPCSTR lpszRecFileName,
												LPCSTR pExParamIn,
												DWORD dwExParamInLen);

/**************************************************************************
(4-20) IDVR_StopRemotePlaySaveData
功能说明：停止远程回放时保存数据
实现要求：推荐实现
输入参数：hPlay - 回放句柄
输出参数：(无)
函数返回：TRUE：执行成功 / FALSE：执行失败
/**************************************************************************/
BOOL DFJK_API CALL_TYPE IDVR_StopRemotePlaySaveData(HANDLE hPlay);

/**************************************************************************
(4-21) IDVR_StopRemotePlaySaveData
功能说明：设置远程回放时数据流回调函数
实现要求：推荐实现
输入参数：hPlay - 回放句柄
		  dwCookie -回调函数中引入的用户标识数据
	  	  pExParamIn－扩展参数，格式化参数字符串[8]，必须为NULL
		  dwExParamInLen－扩展参数长度 ，必须为0
		  callbackHandler 播放数据回调过程，定义参函数IDVR_StartRealData
输出参数：（无）
函数返回：TRUE：执行成功 / FALSE：执行失败
/**************************************************************************/
BOOL DFJK_API CALL_TYPE IDVR_SetRemotePlayDataCallBack (HANDLE hPlay, DWORD dwCookie, LPCSTR pExParamIn, DWORD dwExParamInLen, RealDataCallBack callbackHandler);

/**************************************************************************
(4-22) IDVR_GetRemoteTotalTime
功能说明：获取远程播放时的总播放时间
实现要求：必须实现
输入参数：hPlay - 回放句柄
输出参数：pdwTotalTime -总播放时间（单位：秒）
函数返回：TRUE：执行成功 / FALSE：执行失败
/**************************************************************************/
BOOL DFJK_API CALL_TYPE IDVR_GetRemoteTotalTime(HANDLE hPlay, DWORD *pdwTotalTime);

/**************************************************************************
(4-23) IDVR_GetRemotePlayedTime
功能说明：设置远程回放时数据流回调函数
实现要求：推荐实现
输功能说明：获取远程播放时的当前播放时间
实现要求：必须实现
输入参数：hPlay - 回放句柄
输出参数：pdwPlayedTime-当前播放时间（单位：秒）
函数返回：TRUE：执行成功 / FALSE：执行失败
/**************************************************************************/
BOOL DFJK_API CALL_TYPE IDVR_GetRemotePlayedTime (HANDLE hPlay, DWORD *pdwPlayedTime);

/**************************************************************************
(4-24) IDVR_GetRemoteTotalFrames
功能说明：获取远程回放时总的帧数
实现要求：必须实现
输入参数：hPlay - 回放句柄
输出参数：pdwTotalFrames-总的播放帧数
函数返回：TRUE：执行成功 / FALSE：执行失败
/**************************************************************************/
BOOL DFJK_API CALL_TYPE IDVR_GetRemoteTotalFrames (HANDLE hPlay, DWORD *pdwTotalFrames);

/**************************************************************************
(4-25) IDVR_GetRemotePlayedFrames
功能说明：获取远程回放时的当前播放帧数
实现要求：必须实现	
输入参数：hPlay - 回放句柄
输出参数：pdwPlayedFrames-当前播放的帧数
函数返回：TRUE：执行成功 / FALSE：执行失败
/**************************************************************************/
BOOL DFJK_API CALL_TYPE IDVR_GetRemotePlayedFrames (HANDLE hPlay, DWORD *pdwPlayedFrames);

/**************************************************************************
(4-26) IDVR_GetPlayBackOsdTime
功能说明：获取远程回放时的视频叠加时间
实现要求：必须实现
输入参数：hPlay - 回放句柄
输出参数：pOsdTime - 视频叠加时间，定义参考IDVR_SetDeviceTime
函数返回：TRUE：执行成功 / FALSE：执行失败
/**************************************************************************/
BOOL DFJK_API CALL_TYPE IDVR_GetPlayBackOsdTime(HANDLE hPlay, PDEVTIME pOsdTime);

/**************************************************************************
功能说明：动态产生一个关键帧
实现要求：可选实现
输入参数：hDevHandle-设备句柄
lChannel-通道号
streamtype-0为主码流 1为子码流
返回值：TRUE 表示成功，FALSE 表示失败。 
/**************************************************************************/
BOOL DFJK_API CALL_TYPE IDVR_PrepareStream(HANDLE hDevHandle,long lChannel,int sTreamtype);

/**************************************************************************
功能说明：设置解码时丢弃 B 帧的个数, dwNum :0 不丢,1-丢一个,2-丢 2 个 
参数说明: 
hChanHandle-通道句柄 
m_ dwNum-丢弃 B 帧的个数,取值为 0，1，2，0-不丢，1-丢 1 个 B 帧，2-丢 2 个 B 帧；在多
路播放时，将 B 帧丢弃可以降低 CPU 的利用率，不过当一路时，最好不丢弃 B帧 
返回值：TRUE 表示成功，FALSE 表示失败。  
/**************************************************************************/

BOOL  DFJK_API CALL_TYPE IDVR_ThrowBFrame(HANDLE hChanHandle,DWORD dwNum);


/**************************************************************************
功能说明：语音对讲
参数说明: 
返回值：TRUE 表示成功，FALSE 表示失败。  
/**************************************************************************/
typedef void(CALLBACK *CBF_VoiceData)(
  LONG		lVoiceComHandle,
  char     *pRecvDataBuffer,
  DWORD    dwBufSize,
  BYTE     byAudioFlag,
  void     *pUser
);
//语音对讲（支持回调），用IDVR_StopAudioPhone结束对讲
BOOL DFJK_API CALL_TYPE IDVR_StartAudioPhone_EX(HANDLE hDevHandle, DWORD dwVoiceChan, BOOL bNeedCBNoEncData,
		CBF_VoiceData  cbVoiceData, void *pUser);

//语音广播
typedef void(CALLBACK *CBF_VoiceDataBroad)(char *pRecvDataBuffer, DWORD dwBufSize, void *pUser);
BOOL DFJK_API CALL_TYPE IDVR_StartAudioBroadcast(CBF_VoiceDataBroad cbVoiceData, void *pUser);
BOOL DFJK_API CALL_TYPE IDVR_AudioBroadcast_AddDev(HANDLE hDevHandle, DWORD dwVoiceChan);
BOOL DFJK_API CALL_TYPE IDVR_AudioBroadcast_DelDev(HANDLE hDevHandle);
BOOL DFJK_API CALL_TYPE IDVR_StopAudioBroadcast();

//设备异常 （未加...）
//typedef void (CALLBACK* CBF_ExceptionCallBack)(HANDLE hDevHandle, DWORD dwType, LONG lHandle, void *pUser);
//BOOL DFJK_API CALL_TYPE IDVR_SetExceptionCallBack(CBF_ExceptionCallBack cbException, void *pUser);

//解码 (url格式DF1500通用url)
BOOL DFJK_API CALL_TYPE IDVR_Decode(HANDLE hDevHandle, DWORD dwDecodeChan, BOOL bStart, LPCSTR lpszUrl);

//保存和恢复设备配置参数（bGetorSet--FALSE：获取，TRUE：保存）
BOOL DFJK_API CALL_TYPE IDVR_ConfigFile(HANDLE hDevHandle, BOOL bGetorSet, LPCSTR lpsFileName);


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
BOOL DFJK_API CALL_TYPE IPLY_OpenFile(LPCSTR lpszFileName, HANDLE *phFileHandle);

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
BOOL DFJK_API CALL_TYPE IPLY_CapturePicture(HANDLE hFileHandle,LPCSTR lpszBmpFileName);

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
//BOOL DFJK_API CALL_TYPE ISYS_CustomFunction(LPCSTR pParamIn, DWORD dwParamInLen, LPTSTR *ppParamOut, DWORD* pdwParamOutLen);

//电子放大
BOOL DFJK_API  CALL_TYPE IPLY_SetDisplayRegion(HANDLE hPlayHandle,DWORD nRegionNum, RECT *pSrcRect, HWND hDestWnd, BOOL bEnable);
BOOL DFJK_API  CALL_TYPE IPLY_RefreshPlayEx(HANDLE hPlayHandle, DWORD nRegionNum);


//注册绘图回调函数
BOOL DFJK_API CALL_TYPE IPLY_RegisterDrawFun(HANDLE hPlayHandle, DrawCallBack callbackDraw, void* pUserData);

#ifdef __cplusplus
};
#endif
