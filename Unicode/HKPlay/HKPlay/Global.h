 
//
//	Global.h --- 宏, 结构定义, 全局函数,变量声明
//

#ifndef _GLOBAL_H_
#define _GLOBAL_H_

#include <afxtempl.h>
#include "ThreadMutex.h"
#include "CommonFunc.h"

//添加设备SDK头文件
#include "PlayM4.h"

//-------------------------------------------------------------------------------------
//        宏定义
//-------------------------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
//宏定义
#define DFJK_MAXLEN_LASTERROR		1024		//错误信息缓冲区大小

#define DFJK_INVALID_HANDLE			-1		//无效句柄定义（是插件内部使用的，根据本插件情况修改）

#define DFJK_MAX_CHAN			32		//每台设备最多通道数
#define DFJK_MAX_LINK			50		//每台设备最多连接数（同一个通道可能连多次）
#define DFJK_MAX_ALARMIN		16		//每台设备最多报警输入
#define DFJK_MAX_ALARMOUT		16		//每台设备最多报警输出

#define DFJK_MAX_TRANS			8		//视频最多转发次数

#define MAXLEN_STR				50
#define MAXLEN_IP				16

#define CIF_WIDTH				352
#define	CIF_HEIGHT_PAL			288

//通道基类（包括实时预览、远程录像回放）
class CChannelBase
{
public:
	CChannelBase()
	{
		pStreamHeader = NULL;
		dwStreamheaderLen = 0;
		pfnRealDataCallBack = NULL;
		pfnDrawCallBack = NULL;
		hWaitHeaderEvent = NULL;
		dwCookie = 0;
		pUserData = NULL;
	}
	~CChannelBase()
	{
		if (pStreamHeader)
		{
			delete []pStreamHeader;
			pStreamHeader = NULL;
		}
	}

	BYTE*	pStreamHeader;
	DWORD	dwStreamheaderLen;
	RealDataCallBack  pfnRealDataCallBack;
	DrawCallBack pfnDrawCallBack;
	HANDLE hWaitHeaderEvent;
	DWORD	dwCookie;
	void*	pUserData;
};

//////////////////////////////////////////////////////////////////////////
//回放类（与设备插件必须相同）
class CRecPlay: public CChannelBase
{
public:
	CRecPlay()
	{
		lPlayHandle		= DFJK_INVALID_HANDLE;
		nPlayerIndex	= DFJK_INVALID_HANDLE;
		lWidth			= 0;
		lHeight			= 0;
		bRemotePlayByTime = FALSE;
		bIsRemotePlay	= FALSE;
		hVirtualPlayWnd	= NULL;
	}
	~CRecPlay()
	{
	}

public:
	long	lPlayHandle;		//播放句柄
	int		nPlayerIndex;		//播放句柄
	long	lWidth;				//图像宽度
	long	lHeight;			//图像高度
	BOOL	bIsRemotePlay;		//是否为远程回放
	BOOL	bRemotePlayByTime;	//是否为远程按时间回放
	HWND	hVirtualPlayWnd;	//回调方式预览用
	HANDLE  hWaitHeaderEvent;	//等待回调
};

//-------------------------------------------------------------------------------------
//        全局变量声明
//-------------------------------------------------------------------------------------

extern TCHAR g_szLastError[DFJK_MAXLEN_LASTERROR];	//Last Error

extern TCHAR g_szDLLPath[MAX_PATH];					//DLL 所在路径

//-------------------------------------------------------------------------------------
//        全局函数声明
//-------------------------------------------------------------------------------------

//从扩展参数获取Tag值
BOOL gF_GetTagValFromExParam(LPCTSTR lpszExParam, LPCTSTR lpszTagName, CString& strVal);

#endif

