 
//
//	Global.h --- 宏, 结构定义, 全局函数,变量声明
//

#ifndef _GLOBAL_H_
#define _GLOBAL_H_

#include <afxtempl.h>
#include "ThreadMutex.h"

//添加设备SDK头文件
#include "dhplay.h"

//-------------------------------------------------------------------------------------
//        宏定义
//-------------------------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
//宏定义
#define DFJK_MAXLEN_LASTERROR		1024		//错误信息缓冲区大小

#define DFJK_INVALID_HANDLE			0		//无效句柄定义（是插件内部使用的，根据本插件情况修改）

#define DFJK_MAX_CHAN			32		//每台设备最多通道数
#define DFJK_MAX_LINK			50		//每台设备最多连接数（同一个通道可能连多次）
#define DFJK_MAX_ALARMIN		16		//每台设备最多报警输入
#define DFJK_MAX_ALARMOUT		16		//每台设备最多报警输出
#define DFJK_MAX_TRANS			8		//视频最多转发次数

#define MAXLEN_STR				50
#define MAXLEN_IP				16


//-------------------------------------------------------------------------------------
//        结构定义
//-------------------------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
//回放类
class CRecPlay
{
public:
	CRecPlay()
	{
		lPlayHandle		= DFJK_INVALID_HANDLE;
		dwTotalSize		= 0;
		dwPlayedSize	= 0;
		byPlayedPercent	= 0;

		cStr[0] = 0;
		flag = FALSE;
	}
	~CRecPlay()
	{
	}

public:
	long lPlayHandle;		//播放句柄
	char  cStr[128];
    BOOL flag;  //0:按文件播放；1：按时间播放

	
	DWORD	dwTotalSize;		//文件总大小
	DWORD	dwPlayedSize;		//当前已经播放的大小
	BYTE	byPlayedPercent;	//播放百分比
	//BOOL	bIsPlaying;			//是否正在播放
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

