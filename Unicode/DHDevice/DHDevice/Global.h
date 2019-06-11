 
//
//	Global.h --- 宏, 结构定义, 全局函数,变量声明
//

#ifndef _GLOBAL_H_
#define _GLOBAL_H_

#include <afxtempl.h>
#include "ThreadMutex.h"
#include "ComDev.h"
//添加设备SDK头文件
#include "dhnetsdk.h"
#include "dhplay.h"


//#include "SDK\ShowHCRemoteWnd.h"

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
#define nPort    1


//-------------------------------------------------------------------------------------
//        结构定义
//-------------------------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////

//视频通道类
typedef void(CALLBACK *RealDataCallBack)(HANDLE hChanHandle, 										 
										 BYTE *pBuffer,
										DWORD dwBufSize,
										DWORD dwCookie, 
										DWORD dwParam2);
class CChannel
{
public:
	CChannel()
	{
		nChanNo			= 0;
		bIsRecording	= FALSE;
		lChanHandle		= DFJK_INVALID_HANDLE;
		lSoundHandle	= DFJK_INVALID_HANDLE;
		pfnRealDataCallBack = NULL;
		pStreamHeader = NULL;
		dwStreamheaderLen = 0;
	}
	~CChannel()
	{
		if (pStreamHeader)
		{
			delete []pStreamHeader;
			pStreamHeader = NULL;
			dwStreamheaderLen = 0;
		}
	}

public:
	//char	szChanName[MAXLEN_STR];
	short	nChanNo;			//通道号
	long	lChanHandle;		//通道句柄
	BOOL	bIsRecording;		//是否正在录像
	//DWORD	dwRecordTime;		//录像时间
	//DWORD	dwStartRecordTime;	//开始录像时间(TickCount)
	//int	iConnectType;		//网络连接类型，1:TCP, 2:UDP, 3:Multi, 4:RTP
	//CVideoStream* pVideoStream;
	long	lSoundHandle;		//通道监听句柄；for DL
	RealDataCallBack  pfnRealDataCallBack;
	DWORD	dwCookie;
	BYTE*	pStreamHeader;
	DWORD	dwStreamheaderLen;
};


//回放类
class CRecPlay
{
public:
	CRecPlay()
	{
		lPlayHandle	= DFJK_INVALID_HANDLE;

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
};

//RVU类
class CRvu
{
public:
	CRvu();
	~CRvu();

//	BOOL AddDownload(CRecDownload* pDown);		//增加一个下载类实例
//	BOOL RemoveDownload(CRecDownload* pDown);		//增加一个下载类实例

public:
	char	szRvuName[MAXLEN_STR];
	char	szRvuIP[MAXLEN_IP];
	char	szRvuMultiIP[MAXLEN_IP];
	char	szRvuUserName[MAXLEN_STR];
	char	szRvuPassword[MAXLEN_STR];
	long	lRvuHandle;					//RVU句柄
	long	lStartChannel;				//起始通道号
	int		iChannelNumber;				//通道数
	WORD	wServerPort;				//服务器端口号
	WORD	wServerType;				//DVR类型
	DWORD	dwAlarmInNum;				//报警输入个数
	DWORD	dwAlarmOutNum;				//报警输出个数
	DWORD	dwDiskNum;					//硬盘个数
	long	lAlarmHandle;				//报警句柄
	long	lSerialHandle;				//透明串口句柄
	long	l485Handle;					//透明485口句柄
	long	lVoiceComHandle;			//语音对讲句柄
	CPtrArray m_arrayChan;				//通道列表
//	CChannel* pChan[DFJK_MAX_CHAN];		//通道类实例指针
	BYTE	bAlarmIn[DFJK_MAX_ALARMIN];	//DVS报警输入状态
	BYTE	bAlarmVideoMotion[DFJK_MAX_ALARMIN];		//移动侦测报警状态
	BYTE    bAlarmVideoLost[DFJK_MAX_ALARMIN];
	BYTE	bAlarmVideoShelter[DFJK_MAX_ALARMIN];
	int		iConnectType;				//网络连接类型，1:TCP, 2:UDP, 3:Multi
	//MRPARAM mrParam[DFJK_MAX_TRANS];
//	CPtrArray m_arrayDownload;			//下载列表
	DWORD	dwAlarmUserData;			//报警回调函数用户数据
};

class CRvuManager
{
public:
	CThreadMutex m_mutex;
	CPtrArray m_arrayRvu;

public:
	BOOL AddRvu(CRvu* pRvu);					//增加一个Rvu实例
	BOOL RemoveRvu(CRvu* pRvu);					//删除一个Rvu实例
	BOOL RemoveRvuByHandle(HANDLE hRvu);		//删除一个Rvu实例（根据hRvu）

	BOOL AddChan(CRvu* pRvu, CChannel* pChan);	//增加一个Channel实例
	BOOL RemoveChan(CChannel* pChan);			//删除一个Channel实例

	CRvu* GetRvuByHandle(long lRvuHandle);		//根据Rvu句柄获取Rvu实例
	CRvu* GetRvuByChanHandle(long lChanHandle);		//根据某通道句柄，获取所属Rvu实例
	CChannel* GetChan(HANDLE hRvu, short nChan);	//根据Rvu句柄获取通道类实例

	void ClearAll();							//释放一切
};

//-------------------------------------------------------------------------------------
//        全局变量声明
//-------------------------------------------------------------------------------------

extern CRvuManager g_rvuMgr;

extern TCHAR g_szLastError[DFJK_MAXLEN_LASTERROR];	//Last Error

extern TCHAR g_szDLLPath[MAX_PATH];					//DLL 所在路径

//-------------------------------------------------------------------------------------
//        全局函数声明
//-------------------------------------------------------------------------------------

//从扩展参数获取Tag值
BOOL gF_GetTagValFromExParam(LPCTSTR lpszExParam, LPCTSTR lpszTagName, CString& strVal);

#endif

