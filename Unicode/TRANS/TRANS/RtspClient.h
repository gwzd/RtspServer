#ifndef RTSPCH_
#define RTSPCH_

#define RTSP_SEVER_PORT 554		// Well-known RTSP server port
#define MAX_STR			10240	// Max string lenget
#define MAX_BUF			1024	// Size of receive buffer
#define MAX_LINE		1024	// No single line in the response should be more than
#define MAX_RESP_SIZE	3072
#define MAX_MEDIA		6		// This many media may be present (all-media, audio, video, text, ...)

// State definitions

#define IDLE		0
#define PLAYING		1
#define PAUSED		2

#define DESCRIBE	1
#define SETUP		2
#define PLAY		3
#define PAUSE		4
#define TEARDOWN	5
#define ANNOUNCE    6
#define OPTIONS     7
#define GET         8


/*********************
		      WIN32 compatibility */

#ifdef UNIX
#define MB_OK 0
#define SOCKET_ERROR -1
#define SOCKET int
#define stricmp strcasecmp
#define strnicmp strncasecmp
#define ZeroMemory bzero
#define TRUE 1
#define FALSE 0
#endif
enum ErrorCode  
{
	ErrNoIniSock = 100,         //Could not initialize sockets; sock类错误以1开头
	ErrNoSockVersion = 101,     //Could not find WinSock1.1
	ErrNoOpenSocket = 102,      //Could not open the socket!
	ErrNoSendTimeO = 103,       //Could not set SendTimeOut!
	ErrNoRecvTimeO = 104,       //Could not set RecvTimeOut!
    ErrNoConnectServer = 105,   //Could not connect to the server!
		
	ErrReqDescribe = 300,       //  Failed sending DESCRIBE request!;req类错误以3开头
	ErrReqSetup = 301,          //  Failed sending Setup request!
	ErrReqPlay = 302,           //  Failed sending Play request!
	ErrReqPause = 303,          // Failed sending Pause request!
	ErrReqTearDown = 304,       // Failed sending TearDown request!

	ErrRecvDescribe = 400,      // Failed receive DESCRIBE response!;recv类错误以4开头
    ErrRecvSetup = 401,         // Failed receive setup response!;
    ErrRecvPlay = 402,          // Failed receive play response!;
    ErrRecvPause = 403,         // Failed receive play response!;
    ErrRecvTearDown = 404,      // Failed receive pause response!;

	ErrUrlInvalid  = 500,       //input invalid Url；Url类错误以5开头
    ErrUrlBadProtool = 501,     //Bad protool,no RTSP
	ErrUrlNoFile = 502,         // no file specified,Url中无“/”

	ErrNoLocalhost = 601,       //no localhost	
	ErrNoOpneStream = 701,   //HikOpenStream Faild
	ErrHikGetPortError = 702
};

struct CodeStringError
{
	ErrorCode code;
	char ErrorString[200];
};

struct packetheader
{
	int head;
	int body;
	int tail;
	
};

#define MAX_BUFFER_SIZE  (2048*2048)
#define MAX_RTP_BUFFER_SIZE  65535


struct SESSION_MEDIA {
	char setupInfo[MAX_STR];     // :video, :audio etc.
	int uniPort;
	int multiPort;
	int multiTTL;
	char srcAddr[MAX_STR];
	char type[MAX_STR];          // audio, video, application ...
};

#include "winsock2.h"
#include "ThreadMutex.h"
//#include "PlugInLoader.h"
#include "Utilities.h"
// #include "Prototypes.h"
// #include "clntsink.h"


typedef void (_stdcall *RealDataCallBack)(HANDLE hChanHandle, 										 
										  BYTE *pBuffer,
										  DWORD dwBufSize,
										  DWORD dwCookie,
										  DWORD dwParam);

class MyRTPSession; 

class CRTSPclient {
public:
	CRTSPclient ();

	/////////11.10////////////////////
    BOOL TCPConnect();
	void CloseTCPSocket();
	int ReadTCPData(DWORD DClient);
	SOCKET data_socket;               //TCP连接时的SOCKET
	int tcpPort;                      //TCP连接时传输数据的端口
	
	/////////11.10////////////////////
	
	RealDataCallBack m_pRecFunc;
	DWORD m_dwCookie;
	DWORD m_dwExParamIN; 

	int Connect ();
	int Stop ();
	int Resume ();
	int Disconnect ();
	const char* GetTitle ();
	int GetState()
	{
		CGuard<CThreadMutex> lock(m_mutex);
		return m_nState;
	}
	void SetState(int state)
	{
		CGuard<CThreadMutex> lock(m_mutex);
		m_nState = state;
	}

	MyRTPSession *GetRTPSession()
	{
		return m_pRTPSession;
	}
	int ReadRTPData(DWORD DClient,MyRTPSession* pRTPSession);
	int InitRTPSession(int nPortBase,MyRTPSession** ppRTPSession);
	int UninitRTPSession();

	int sendAnnounce();
	int sendOptions(unsigned int ucommand,
		                      int iparam1,
							  int iparam2, 
							  int iparam3, 
							  int iparam4,
							  char* lpszParm5);   //云台控制 +快进、慢放等

	int sendGets(const char* strURL,
		         int RecType,
				 const char* strTime);//发送得到文件列表


	char host[MAX_STR];               // DNS host name
	int serverport;
	char file[MAX_STR];               // SDP file name, session description
	HWND hWnd;
	SOCKET serv_socket;
	char strURL[MAX_STR];
	char m_fileHeader[MAX_STR];                  //文件头setup回复中收到的
	DWORD m_headerLength;

	char m_szRemoteFileName[MAX_STR];            //回放的文件名字
	char m_szDvrType[10];                     //设备类型

                           
	__int64     lfreq;                       //获取当前时间
	__int64     lstart;
	__int64     lstop;
	__int64     lruntime;  
                                                             //调用插件 
                   
//	BOOL LoadPlugin(LPCTSTR lpszType,LPCTSTR lpszSubType);      
	BOOL intialize(void *pvReserved,DWORD reserved);
//	BOOL FreePlugin();
//	BOOL GetFunction();
//     CPlugInLoader *m_pPlugloader;
// 	CPluginInfo*  m_pPluginfo;
//    
// 	IPLY_StreamPlay  pfnIPLY_StreamPlay;
// 	IPLY_OpenPlayStream pfnIPLY_OpenPlayStream;
// 	IPLY_StreamStop pfnIPLY_StreamStop;
// 	IPLY_CloseStream pfnIPLY_CloseStream;
// 	IPLY_InputStreamData pfnIPLY_InputStreamData;        //还少RefreshPLAY  readrealheader 
// 	ISYS_GetLastError    pfnISYS_GetLastError;
// 	IPLY_StreamRefreshPlay pfnIPLY_StreamRefreshPlay;
// 	IPLY_CapturePicture pfnIPLY_CapturePicture;
// 	IPLY_SetVideoParams pfnIPLY_SetVideoParams;
//     IPLY_GetVideoParams pfnIPLY_GetVideoParams;
// 
//     IPLY_StartSound	pfnIPLY_StartSound;
// 	IPLY_StopSound  pfnIPLY_StopSound;
// 	IPLY_SetVolume  pfnIPLY_SetVolume; 
// 
// 	//10.21
// 	IPLY_Pause      pfnIPLY_Pause;
// 	IPLY_Fast       pfnIPLY_Fast;
// 	IPLY_Slow       pfnIPLY_Slow;
// 	IPLY_Play       pfnIPLY_Play;
	char*   m_PBRecv;//回放远程文件时，收到的RTSP命令中的值（当前播放文件的进度值、时间长度、帧数等）
	DWORD  m_PBRecvLength;

	TCHAR LastError[1000];                // 返回命令的错误信息
	void CloseSocket();
	void CloseThread();
	int RecvResponse(int type ,int mediaCtrl);
	BOOL CreatPlayer(LPCTSTR lpszType,LPCTSTR lpszSubType);
    BOOL StreamPlay();
	int ParseInput (const char * serverURL); 

	int m_hPort;
    int CloseStream ();
	int Conntype;                        //转发方式0:tcp 1:UDP

    int ServConnect ();



private:
	int m_nState;                        // 状态变量
	int TCPSocketBind();               //TCP方式传输数据时 11.10
	
	int cSeq;	                         // Increased by 1, always
	char session[MAX_STR];               // Unique RTSP session identifier
	char sessName[MAX_STR];              // Session name extracted from SDP
//	char accept[MAX_STR];		         // We can parse only SDP
	SESSION_MEDIA media[MAX_MEDIA];      // We support up to MAX_MEDIA descriptions
			                             // media[0] is reserved for "all media"
	int m_StreamNum;                     // ... and we actually have this many     

	int GetSomePort ();
		
	MyRTPSession *m_pRTPSession;
	HANDLE m_hThread;
	DWORD  dwThreadId;
	
	HANDLE m_sleepThread;
	DWORD  dwSleepThreadId;

	CThreadMutex m_mutex;	
    int sendDescribe();
	int sendSetup(int mediaCtrl);
	int sendPlay();
	int sendTeardown();
	int addHeader(char * reqString);
    int sendv(char* send_p,int sendplen);
	void addSession(char * reqString);

	int receive(char* respstr);

	unsigned char Rtp_Buffer[MAX_RTP_BUFFER_SIZE];  //rtp读取数据缓冲区
	//<<<<<<< .mine
    unsigned char TCP_Buffer[MAX_BUFFER_SIZE];
	int Rtp_Buffer_plength;

	int recvTCPData(const int datalength,char* pBuffer);
	int  m_iUserData;


};
const char* GetErrorMessage(ErrorCode code);
#endif
