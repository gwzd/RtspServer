

#include "stdafx.h"
#include "RtspClient.h"
#include "assert.h"
#include "Utilities.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <vector>
#include <functional>
#include "deelx.h"       //正则表达式
#include "rtp.h"

using namespace std;
using namespace jrtplib;

#define TIMEOUT 9000
#define RTPMAXPACKSIZE 3072
#define CONNECTTIMEOUT 10     //connect 函数超时 10S
#define TCPBUFSIZE  2048*2048


///////////11.10//////////
#define CLIENT_PORT_BASE		14000
static long  lLastPort = 0;
#define CLIENT_TCPPORT_BASE      15000
static long TCPlLastPort = 0;
///////////11.10//////////

CThreadMutex	g_mutex;
SYSTEMTIME		sys;

static CodeStringError StringError[] =
{
	{ ErrNoIniSock, "Could not initialize sockets" },
	{ ErrNoSockVersion, "Could not find WinSock1.1" },
	{ ErrNoOpenSocket, "Could not open the socket!" },
	{ ErrNoSendTimeO, "Could not set SendTimeOut!" },
	{ ErrNoRecvTimeO, "Could not set RecvTimeOut!" },
	{ ErrNoConnectServer, "Could not connect to the server!" },
	{ ErrReqDescribe, "Failed sending DESCRIBE request!" },
	{ ErrReqSetup, "Failed sending Setup request!" },
	{ ErrReqPlay, "Failed sending Play request!" },
	{ ErrReqPause, "Failed sending Pause request!" },
	{ ErrReqTearDown, "Failed sending TearDown request!" },
	{ ErrRecvDescribe, "Failed receive DESCRIBE response!" },
	{ ErrRecvSetup, "Failed receive setup response!;" },
	{ ErrRecvPlay, "Failed receive play response!" },
	{ ErrRecvPause, "Failed receive pause response!" },
	{ ErrRecvTearDown, "Failed receive pause response!" },
	{ ErrUrlInvalid, "input invalid Url" },
	{ ErrUrlBadProtool, "Bad protool,no RTSP" },
	{ ErrUrlNoFile, "no file specified,Url中无/" },
	{ ErrNoLocalhost, "no localhost" },
	{ ErrNoOpneStream, "FailED HIk_OpenStream" },
	{ ErrHikGetPortError, "GetPort ERROR!" }
};

const char* GetErrorMessage(ErrorCode code)
{
	for (int i = 0; i<sizeof(StringError) / sizeof(StringError[0]); i++)
	{
		if (StringError[i].code == code)
			return StringError[i].ErrorString;
	}

	return "";
}

UINT WINAPI ReadMediaData(LPVOID lpParam)
{
	CRTSPclient* pClient = (CRTSPclient*)lpParam;
	DWORD DClient = (DWORD)lpParam;

	while (pClient->GetState() == PLAYING)
	{
		///////////11.10//////////	
		if (pClient->Conntype == 0)
		{
			int nret = pClient->ReadTCPData(DClient);
			if (nret == -1)	return 0;
		}

		if (pClient->Conntype == 1)
			pClient->ReadRTPData(DClient, pClient->GetRTPSession());

		// pClient->ReadRTPData(DClient,pClient->GetRTPSession());
		///////////11.10//////////
	}

	return 0;
}

enum ParseRecvError
{
	ErrPRecvSocket = 500,                               //socket recv函数错误
	ErrPNosuccess = 501,                               //response没有200，请求失败
	ErrPCseq = 502,                                     //Cseq 不一致
	ErrDescribeStreamNum = 503,                          //StreamNumber != 1 
	ErrNoRtsp = 504                                      //Rtsp协议不同
};

UINT WINAPI SendAnnounce(LPVOID lpParam)
{
	CRTSPclient* pClient = (CRTSPclient*)lpParam;
	double dsec;
	int bRet = -1;

	while (pClient->GetState() == PLAYING)
	{
		QueryPerformanceCounter((LARGE_INTEGER*)&pClient->lstop);
		pClient->lruntime = pClient->lstop - pClient->lstart;
		dsec = (double)(pClient->lruntime*1.0 / pClient->lfreq*1.0);
		if (dsec >= 20)
		{
			//	pClient->SendRequest(ANNOUNCE,0); 			
			pClient->sendAnnounce();

			if ((bRet = pClient->RecvResponse(ANNOUNCE, 0)) != 0)
			{
				if (bRet == ErrPRecvSocket)
				{
					wcscpy(pClient->LastError, _T("Send Announce Failed"));
					GSetLogError(pClient->LastError);
				}
			}

			QueryPerformanceCounter((LARGE_INTEGER*)&pClient->lstart);
		}
		else	Sleep(20);
	}

	return 0;
}

CRTSPclient::CRTSPclient()
{
	memset(this, 0, sizeof(CRTSPclient));

	m_hPort = -1;
	serverport = 554;
}

BOOL CRTSPclient::TCPConnect()
{
	struct sockaddr_in server;
	server.sin_addr.S_un.S_addr = inet_addr(host);
	server.sin_family = AF_INET;
	server.sin_port = htons(tcpPort);

	//暂设为非阻塞方式            ////////5.7
	unsigned long ul = 1;
	int res = 0;
	BOOL ret = ioctlsocket(data_socket, FIONBIO, (unsigned long*)&ul);
	if (ret == SOCKET_ERROR)
	{
		res = WSAGetLastError();
		GSetLogError(_T("TCPScoket设为非阻塞方式failed!"));
		return FALSE;
	}

	//	GLogError(_T("ioctsocket 1 true"));
	bool bRet = false;
	if (connect(data_socket, (struct sockaddr*)&server, sizeof(server)) == SOCKET_ERROR)
	{
		///////////5.7
		timeval tm;
		tm.tv_sec = CONNECTTIMEOUT;
		tm.tv_usec = 0;
		fd_set set;
		FD_ZERO(&set);
		FD_SET(data_socket, &set);
		int error = -1;
		int len = sizeof(int);
		if (select(data_socket + 1, NULL, &set, NULL, &tm) > 0)
		{
			getsockopt(data_socket, SOL_SOCKET, SO_ERROR, (char*)&error, &len);
			if (error == 0)	bRet = true;
			else			bRet = false;
		}
		else	bRet = false;
	}

	bRet = true;

	//再设回阻塞模式
	unsigned long ul1 = 0;
	ret = ioctlsocket(data_socket, FIONBIO, (unsigned long*)&ul1);
	if (ret == SOCKET_ERROR)
	{
		::closesocket(data_socket);
		res = WSAGetLastError();
		wsprintf(LastError, _T("TCPScoket设回阻塞方式failed，soketerror：%d"), res);
		GSetLogError(LastError);
		return FALSE;
	}

	if (!bRet)
	{
		res = -22;	//wyz内部使用
		::closesocket(data_socket);
		GSetLogError(_T("TCPScoket connect to the server TimeOut!"));
		return FALSE;
	}

	return TRUE;
}

int CRTSPclient::TCPSocketBind()
{
	CGuard<CThreadMutex> lock(g_mutex);
	int candidate = CLIENT_TCPPORT_BASE + (TCPlLastPort + 1) % 1000;        // so use ports 11000-12000

	int res = 0;
	int socket_type = SOCK_STREAM;

	if (data_socket != NULL)	closesocket(data_socket);

	m_StreamNum = 0;

	data_socket = socket(AF_INET, socket_type, 0);
	int nodelay = 1;
	setsockopt(data_socket, IPPROTO_TCP, TCP_NODELAY, (char *)&nodelay, sizeof(nodelay));

	if (data_socket == INVALID_SOCKET)
	{
		GSetLogError(_T("Could not open the TCPsocket!"));
		return ErrNoOpenSocket;
	}

	int timeOut = TIMEOUT;
	if (setsockopt(data_socket, SOL_SOCKET, SO_SNDTIMEO, (char *)&timeOut, sizeof(timeOut)) == SOCKET_ERROR)
	{
		GSetLogError(_T("TCPScoket Set SendTimeOut Failed!"));
		return ErrNoSendTimeO;
	}

	if (setsockopt(data_socket, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeOut, sizeof(timeOut)) == SOCKET_ERROR)
	{
		GSetLogError(_T("TCPScoket Set ReceiveTimeOut Failed!"));
		return ErrNoRecvTimeO;
	}

	///////////////////5.7
	BOOL succsess = FALSE;
	SOCKADDR_IN addrSock;
	addrSock.sin_family = AF_INET;
	addrSock.sin_addr.S_un.S_addr = htonl(INADDR_ANY);

	int retval;
	TCHAR strtemp[200];

	int count = 0;
	while (!succsess)
	{
		addrSock.sin_port = htons(candidate);
		retval = ::bind(data_socket, (SOCKADDR*)&addrSock, sizeof(SOCKADDR));
		if (SOCKET_ERROR == retval)
		{
			if (count >= 200)
			{
				wsprintf(strtemp, _T("TCP socket tried %d times"), count);
				GSetLogError(strtemp);
				return -1;
			}
			wsprintf(strtemp, _T("TCP socket 端口%d绑定失败"), candidate);
			GSetLogError(strtemp);
			candidate = CLIENT_TCPPORT_BASE + (candidate - CLIENT_TCPPORT_BASE + 1) % 1000;

			count++;
		}
		else	succsess = TRUE;
	}

	wsprintf(strtemp, _T("TCP socket 端口%d绑定成功"), candidate);
	GSetLogError(strtemp);
	TCPlLastPort = candidate;
	return 0;
}

/*********************'
ServConnect: parse server URL, complete host, port and file information and
create the serv_socket
**********/
int CRTSPclient::ServConnect()
{
	int res = 0;
	int socket_type = SOCK_STREAM;
	struct sockaddr_in server;

	if (serv_socket != NULL)	closesocket(serv_socket);

	SetState(IDLE);
	memset(session, 0, MAX_STR);
	memset(sessName, 0, MAX_STR);

	m_StreamNum = 0;

	TCHAR strtemp[1024];
	wsprintf(strtemp, _T("---host = %s,serverport = %d"), host, serverport);
	GLogError(strtemp);

	server.sin_addr.S_un.S_addr = inet_addr(host);
	server.sin_family = AF_INET;
	server.sin_port = htons(serverport);

	serv_socket = socket(AF_INET, socket_type, 0);
	int nodelay = 1;
	setsockopt(serv_socket, IPPROTO_TCP, TCP_NODELAY, (char *)&nodelay, sizeof(nodelay));

	if (serv_socket == INVALID_SOCKET)
	{
		GSetLogError(_T("Could not open the socket!"));
		return ErrNoOpenSocket;
	}

	int timeOut = TIMEOUT;
	if (setsockopt(serv_socket, SOL_SOCKET, SO_SNDTIMEO, (char *)&timeOut, sizeof(timeOut)) == SOCKET_ERROR)
	{
		GSetLogError(_T("Set SendTimeOut Failed!"));
		return ErrNoSendTimeO;
	}

	if (setsockopt(serv_socket, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeOut, sizeof(timeOut)) == SOCKET_ERROR)
	{
		GSetLogError(_T("Set ReceiveTimeOut Failed!"));
		return ErrNoRecvTimeO;
	}

	//暂设为非阻塞方式            ////////5.7
	unsigned long ul = 1;

	BOOL ret = ioctlsocket(serv_socket, FIONBIO, (unsigned long*)&ul);
	if (ret == SOCKET_ERROR)
	{
		res = WSAGetLastError();

		GSetLogError(_T("设为非阻塞方式failed!"));
		return FALSE;
	}

	///////////////////5.7
	bool bRet = false;
	if (connect(serv_socket, (struct sockaddr*)&server, sizeof(server)) == SOCKET_ERROR)
	{
		///////////5.7
		timeval tm;
		tm.tv_sec = CONNECTTIMEOUT;
		tm.tv_usec = 0;
		fd_set set;
		FD_ZERO(&set);
		FD_SET(serv_socket, &set);
		int error = -1;
		int len = sizeof(int);
		if (select(serv_socket + 1, NULL, &set, NULL, &tm) > 0)
		{
			getsockopt(serv_socket, SOL_SOCKET, SO_ERROR, (char*)&error, &len);
			if (error == 0)	bRet = true;
			else			bRet = false;
		}
		else	bRet = false;
	}
	else	bRet = true;

	//再设回阻塞模式
	unsigned long ul1 = 0;
	ret = ioctlsocket(serv_socket, FIONBIO, (unsigned long*)&ul1);
	if (ret == SOCKET_ERROR)
	{
		::closesocket(serv_socket);
		res = WSAGetLastError();
		wsprintf(LastError, _T("设回阻塞方式failed，soketerror：%d"), res);
		GSetLogError(LastError);
		return FALSE;
	}

	if (!bRet)
	{
		res = -22;	//wyz内部使用
		::closesocket(serv_socket);
		GSetLogError(_T("connect to the server TimeOut!"));
		return ErrNoConnectServer;
	}

	return res;
}

int CRTSPclient::Connect()
{
	int  res = 0;
	int bRet = -1;

	if ((res = ServConnect()) != 0)
	{
		return res;
	}

	if (sendDescribe() != 0)
	{
		CloseSocket();
		GSetLogError(_T("send Describe failed!"));
		return ErrReqDescribe;
	}

	else	{
		if ((bRet = RecvResponse(DESCRIBE, 0)) != 0)
		{
			if (bRet == ErrPRecvSocket)
			{
				CloseSocket();
				GSetLogError(_T("Failed receive Desribe Response"));
			}
			if (bRet == ErrPNosuccess)   //根据错误码做相应的操作
			{
			}
			if (bRet == ErrDescribeStreamNum)
			{
				if (Conntype == 1)  //////////11.10////////
				{
					UninitRTPSession();
				}
			}

			CloseSocket();
			return bRet;
		}
	}

	if (m_StreamNum == 1)                                   //暂不考虑m_StreamNum不等于1的情况
	{
		if (sendSetup(1) != 0)
		{
			GSetLogError(_T("Send Setup Failed!"));
			if (Conntype == 1)  //////////11.10////////
				UninitRTPSession();

			CloseSocket();
			return ErrReqSetup;
		}
		else
		{
			if ((bRet = RecvResponse(SETUP, 1)) != 0)
			{
				if (bRet == ErrPRecvSocket)
				{
					GSetLogError(_T("Failed receive Setup Response!"));
				}
				if (bRet == ErrPNosuccess)             //根据错误码做相应操作
				{
				}

				if (Conntype == 1)/////11.10//////
					UninitRTPSession();

				CloseSocket();
				return ErrRecvSetup;
			}
		}
	}

	SetState(PLAYING);
	QueryPerformanceFrequency((LARGE_INTEGER*)&lfreq);
	QueryPerformanceCounter((LARGE_INTEGER*)&lstart);

	//发送PLAY请求  
	if (sendPlay() != 0)
	{
		GSetLogError(_T("Failed send Play Request!"));
		Disconnect();
		return ErrReqPlay;
	}
	else
	{
		if ((bRet = RecvResponse(PLAY, 0)) != 0)
		{
			if (bRet == ErrPRecvSocket)
			{
				GSetLogError(_T("Failed receive Play Response!"));
			}
			if (bRet == ErrPNosuccess)                  //根据返回码做相应的操作
			{
			}

			SetState(IDLE);
			Stop();
			CloseThread();

			if (Conntype == 1)    //11.10
				UninitRTPSession();

			if (Conntype == 0)    //11.10
				CloseTCPSocket();

			CloseSocket();
			return ErrRecvPlay;
		}
	}

	m_hThread = (HANDLE)_beginthreadex(NULL, 0, ReadMediaData, (LPVOID)this, 0, (UINT*)&dwThreadId);
	m_sleepThread = (HANDLE)_beginthreadex(NULL, 0, SendAnnounce, (LPVOID)this, 0, (UINT*)&dwSleepThreadId);

	return res;
}

int CRTSPclient::Stop()
{
	if (sendTeardown())
		RecvResponse(TEARDOWN, 0);   //收到收不到回复都一样，所以没有判断错误处理		

	return 0;
}

int CRTSPclient::Resume()                      //不知道这个函数有什么用，connect里面都有了
{
	// 	int res =0;
	// 	if (GetState() == PAUSED) 
	// 	{
	// 		if (SendRequest (PLAY, 0) == 0) 
	// 		{
	// 			RecvResponse (PLAY, 0);
	// 			SetState(PLAYING);
	// 		}
	// 		else
	// 			res =1;
	// 	}
	return 0;
}

int CRTSPclient::Disconnect()
{
	SetState(IDLE);

	if (m_pRTPSession != NULL && Conntype == 1)
		m_pRTPSession->AbortWait();

	Stop();
	if (Conntype == 0)	CloseTCPSocket();

	CloseThread();

	if (Conntype == 1)	UninitRTPSession();

	CloseSocket();
	return 0;
}

int CRTSPclient::CloseStream()
{
	if (m_hPort == -1)
	{
		CloseSocket();
		return 0;
	}

	return 0;
}

void CRTSPclient::CloseThread()
{
	GLogError(_T("Enter CloseThread"));

	if (m_hThread != NULL)
	{
		if (WaitForSingleObject(m_hThread, 5000) == WAIT_TIMEOUT)
		{
			::TerminateThread(m_hThread, 444);
			GLogError(_T("----Close hThread wait timeout, terminated!"));
		}
		if (m_hThread != NULL)
		{
			CloseHandle(m_hThread);
			m_hThread = NULL;
		}
	}

	if (m_sleepThread != NULL)
	{
		if (WaitForSingleObject(m_sleepThread, 5000) == WAIT_TIMEOUT)
		{
			::TerminateThread(m_sleepThread, 555);
			GLogError(_T("----Close hThread wait timeout, terminated!"));
		}

		if (m_sleepThread != NULL)
		{
			CloseHandle(m_sleepThread);
			GLogError(_T("----Close sleepThread"));
			m_sleepThread = NULL;
		}
	}
	GLogError(_T("----Close Thread"));
}

void CRTSPclient::CloseSocket()
{
	if (serv_socket)
#ifdef WIN32
		closesocket(serv_socket);
#else
		close(serv_socket);
#endif
	serv_socket = NULL;
	GLogError(_T("---CloseSocket"));
}

void CRTSPclient::CloseTCPSocket()   //11.10
{
	if (data_socket)
#ifdef WIN32
		closesocket(data_socket);
#else
		close(data_socket);
#endif
	data_socket = NULL;
	GLogError(_T("---CloseTCPSocket"));
}

const char* CRTSPclient::GetTitle()
{
	return sessName;
}

int CRTSPclient::GetSomePort()
{
	CGuard<CThreadMutex> lock(g_mutex);
	int candidate = CLIENT_PORT_BASE + (lLastPort + 2) % 1000;        // so use ports 10000-12000
	bool sucess = FALSE;
	int initRtp = -1;
	TCHAR strtemp[100];

	int count = 0;
	while (!sucess)
	{
		initRtp = InitRTPSession(candidate, &m_pRTPSession);
		if (initRtp == -100)	return -1;

		if (initRtp < 0)
		{
			if (count <= 200)
			{
				wsprintf(strtemp, _T("InitRTPSession tryied %d times"), count);
				GLogError(strtemp);
				return -1;
			}
			wsprintf(strtemp, _T("InitRTPSession(%d)failed"), candidate);
			GLogError(strtemp);
			candidate += 2;
		}
		else	sucess = TRUE;
	}

	lLastPort = candidate;                               //lastPort：最后 绑定的端口

	wsprintf(strtemp, _T("InitRTPSession(%d) successed"), candidate);
	GLogError(strtemp);
	return 0;
}

int CRTSPclient::sendPlay()
{
	const char *szCrLf = "\r\n";
	char reqString[MAX_STR];
	memset(reqString, 0, MAX_STR);

	strcpy(reqString, "PLAY ");
	strcat(reqString, strURL);
	addHeader(reqString);
	addSession(reqString);
	strcat(reqString, szCrLf);
	strcat(reqString, "\0");
	GLogError(CString(reqString));

	int reqlen = strlen(reqString);
	if (sendv(reqString, reqlen) == 1)	return 0;

	return ErrReqPlay;
}

void CRTSPclient::addSession(char * reqString)
{
	char *szCrLf = "\r\n";
	if (strlen(session) > 0)
	{
		strcat(reqString, "Session: ");
		strcat(reqString, session);
		strcat(reqString, szCrLf);
	}
}

int CRTSPclient::sendDescribe()
{
	const char *szCrLf = "\r\n";
	char reqString[MAX_STR];
	memset(reqString, 0, MAX_STR);

	strcpy(reqString, "DESCRIBE ");
	strcat(reqString, strURL);
	addHeader(reqString);

	strcat(reqString, "Accept: ");
	strcat(reqString, "application/sdp");
	strcat(reqString, szCrLf);

	if (strlen(m_szRemoteFileName) > 0)
	{
		//	strcat(reqString,"File: file/"); //10.20
		strcat(reqString, "File: ");    //10.20
		strcat(reqString, m_szRemoteFileName);
		strcat(reqString, szCrLf);
	}

	strcat(reqString, szCrLf);
	strcat(reqString, "\0");
	GLogError(CString(reqString));

	int reqlen = strlen(reqString);

	if (sendv(reqString, reqlen) == 1)	return 0;

	return ErrReqDescribe;
}

int CRTSPclient::sendSetup(int mediaCtrl)
{
	const char *szCrLf = "\r\n";
	char reqString[MAX_STR];
	memset(reqString, 0, MAX_STR);

	strcpy(reqString, "SETUP ");
	strcat(reqString, strURL);
	strcat(reqString, media[mediaCtrl].setupInfo);
	addHeader(reqString);

	if (Conntype == 0)
		strcat(reqString, "Transport: RTP/AVP/TCP;unicast");

	if (Conntype == 1)
		strcat(reqString, "Transport: RTP/AVP/UDP;unicast");

	if (mediaCtrl != 0)
	{
		char str[13];
		sprintf(str, "%d-%d", media[mediaCtrl].uniPort, media[mediaCtrl].uniPort + 1);
		strcat(reqString, ";client_port=");
		strcat(reqString, str);
	}
	strcat(reqString, szCrLf);
	strcat(reqString, szCrLf);
	strcat(reqString, "\0");

	GLogError(CString(reqString));
	int reqlen = strlen(reqString);
	if (sendv(reqString, reqlen) == 1)	return 0;

	return ErrReqSetup;
}

int CRTSPclient::sendTeardown()
{
	const char *szCrLf = "\r\n";
	char reqString[MAX_STR];
	memset(reqString, 0, MAX_STR);

	strcpy(reqString, "TEARDOWN ");
	strcat(reqString, strURL);
	addHeader(reqString);
	addSession(reqString);
	strcat(reqString, szCrLf);
	strcat(reqString, "\0");
	GLogError(CString(reqString));

	int reqlen = strlen(reqString);
	if (sendv(reqString, reqlen) == 1)	return 0;

	return ErrReqTearDown;
}

int CRTSPclient::sendAnnounce()
{
	const char *szCrLf = "\r\n";
	char reqString[MAX_STR];
	memset(reqString, 0, MAX_STR);

	strcpy(reqString, "ANNOUNCE ");
	strcat(reqString, strURL);
	addHeader(reqString);
	addSession(reqString);
	strcat(reqString, szCrLf);
	strcat(reqString, "\0");

	int reqlen = strlen(reqString);
	if (sendv(reqString, reqlen) == 1)	return 0;

	return  -1;
}

int  CRTSPclient::sendGets(const char* strURL,
	int RecType,
	const char* strTime)//发送得到文件列表
{
	const char *szCrLf = "\r\n";
	char reqString[MAX_STR];
	memset(reqString, 0, MAX_STR);

	strcpy(reqString, "GET ");
	strcat(reqString, strURL);
	strcat(reqString, "/file/QUERYLIST");
	strcat(reqString, " RTSP/1.0");
	strcat(reqString, szCrLf);

	char strtemp[100];
	ZeroMemory(strtemp, 100);

	sprintf(strtemp, "RecordType: %d\r\nDateRange: %s\r\n",
		RecType, strTime);

	strcat(reqString, strtemp);

	strcat(reqString, szCrLf);
	strcat(reqString, "\0");

	GLogError(CString(reqString));
	int reqlen = strlen(reqString);
	if (sendv(reqString, reqlen) == 1)	return 0;

	return  -1;
}

int CRTSPclient::sendOptions(unsigned int ucommand,
	int iparam1,
	int iparam2,
	int iparam3,
	int iparam4,
	char* lpszParm5)
{
	const char *szCrLf = "\r\n";
	char reqString[MAX_STR];
	memset(reqString, 0, MAX_STR);

	if ((lpszParm5 == NULL) || (strlen(lpszParm5) == 0))
	{
		lpszParm5 = "EDVR";
	}

	strcpy(reqString, "OPTIONS ");
	strcat(reqString, strURL);
	addHeader(reqString);
	addSession(reqString);
	//add the options code
	char strtemp[100];
	ZeroMemory(strtemp, 100);
	sprintf(strtemp, "command: %d\r\nparam1: %d\r\nparam2: %d\r\nparam3: %d\r\nparam4: %d\r\nparam5: %s\r\n",
		ucommand, iparam1, iparam2, iparam3, iparam4, lpszParm5);

	strcat(reqString, strtemp);
	strcat(reqString, szCrLf);
	strcat(reqString, "\0");

	GLogError(CString(reqString));
	int reqlen = strlen(reqString);
	if (sendv(reqString, reqlen) == 1)		return 0;

	return  -1;
}

int CRTSPclient::addHeader(char * reqString)
{
	const char *szCrLf = "\r\n";
	char buf[20];

	strcat(reqString, " RTSP/1.0");
	strcat(reqString, szCrLf);

	sprintf(buf, "%d", ++cSeq); // itoa was here
	strcat(reqString, "CSeq: ");
	strcat(reqString, buf);
	strcat(reqString, szCrLf);
	strcat(reqString, "\0");
	return 0;
}


int CRTSPclient::sendv(char* buff, int sendplen)
{
	int cur = 0;
	while (sendplen > 0)
	{
		if ((cur = send(serv_socket, buff, sendplen - cur, 0)) == SOCKET_ERROR)
			return -1;

		buff += cur;
		sendplen -= cur;
	}

	return 1;
}

///////////////////////////////////////
// RecvResponse
void split(const string& s,
	char* c,
	vector<string>& v)
{
	string::size_type i = 0;
	string::size_type j = s.find(c);
	int len = strlen(c);

	while (j != string::npos)
	{
		v.push_back(s.substr(i, j - i));
		i = j + len;
		j = s.find(c, j + len);

		if (j == string::npos)
			v.push_back(s.substr(i, s.length()));
	}
}

BOOL matchString(int i, char* regexp, char* fromstr, char* deststr)//正则表达式匹配
{
	CRegexpT <char> regexp1(regexp);
	int start, end;

	MatchResult resul = regexp1.Match(fromstr);

	if (resul.IsMatched())
	{
		start = resul.GetGroupStart(i);
		end = resul.GetGroupEnd(i);
		if (start == -1)
			return FALSE;

		memcpy(deststr, fromstr + start, end - start);
		return TRUE;
	}

	return FALSE;
}

int CRTSPclient::receive(char* respstr)  //返回接收的长度
{
	int resplen = -1;
	if ((resplen = recv(serv_socket, respstr, MAX_BUF, 0)) != SOCKET_ERROR)
	{
		GLogError(CString(respstr));
	}
	return resplen;

}

int CRTSPclient::RecvResponse(int type, int mediaCtrl)
{
	char respStr[MAX_STR];
	char istr[100];
	char strTemp[100];
	ZeroMemory(respStr, MAX_STR);
	ZeroMemory(istr, 100);
	ZeroMemory(strTemp, 100);

	int resplen = 0;
	char *szCrLf = "\r\n";
	vector<string> tempv;
	int i;
	int lencur = 0;
	int bodylen = 0;

	int recvLen = receive(respStr);
	if (recvLen == -1)	return  ErrPRecvSocket;

	resplen = recvLen;
	split(respStr, szCrLf, tempv);

	if (tempv.empty())
	{
		recvLen = receive(respStr + recvLen);
		if (resplen == -1)		return  ErrPRecvSocket;

		resplen = resplen + recvLen;
		split(respStr, szCrLf, tempv);
	}  //细想来，这个似乎有点多余。只判断没有接收到一行的情况，怪怪的

	char* strRtsp = "\\w*RTSP/1\\.0 ([1-5][0-9]{2}))";
	char* strSession = "Session: *([0-9]{1,})";
	char* strContLen = "Content-[lL]ength: *([0-9]{1,})";
	char* strCseq = "CSeq: *([0-9]{1,})";
	char* strTransPort = "\\w*source=([0-9]{1,3}\\.[0-9]{1,3}\\.[0-9]{1,3}\\.[0-9]{1,3})";

	for (i = 0; i < tempv.size(); i++)
	{
		lencur = lencur + tempv[i].size() + 2;

		if (tempv[i].size() < 2)	break;

		sprintf(istr, "%s", tempv[i].c_str());
		if (i == 0)
		{
			if (matchString(1, strRtsp, istr, strTemp))            //有问题，
			{
				int rescode = atoi(strTemp);
				ZeroMemory(strTemp, 100);

				if (rescode != 200)
				{
					m_PBRecvLength = 0;
					if (rescode == 404)		return 0;

					GSetLogError(CString(istr));

					return ErrPNosuccess;
				}
			}
			else
			{
				GSetLogError(_T("Rtsp协议不同"));
				return ErrNoRtsp;
			}
		}
		else
		{
			if (matchString(1, strContLen, istr, strTemp))       //contenlength
			{
				bodylen = atoi(strTemp);
				ZeroMemory(strTemp, 100);
			}
			else
			{
				if (matchString(1, strTransPort, istr, strTemp))  //transport
				{
					strcpy(media[mediaCtrl].srcAddr, strTemp);
					ZeroMemory(strTemp, 100);
				}
				else
				{
					if (matchString(1, strSession, istr, strTemp))   //Session
					{
						strcpy(session, strTemp);
						ZeroMemory(strTemp, 100);
					}
				}//endsession
			}//endtransport			 	 
		}//endcontlen
	}//endfor


	if (resplen < lencur + bodylen)
	{
		recvLen = receive(respStr + resplen);
		if (resplen == -1)		return  ErrPRecvSocket;

		resplen = resplen + recvLen;
		split(respStr, szCrLf, tempv);
	}

	char* sdps = "s=(.*)";                                  //analyze sdp
	char* sdpm = "m=(\\w*) *(\\w*) *(\\w*/\\w*) *(\\w*)";

	//	char* sdpa = "a=control:\\s*([\\w\\./]+)";
	//10.28
	char* sdpa = "a=control:(.*)";
	//10.28
	int mediano = 0;
	int res = 0;

	if (type == DESCRIBE)
	{
		for (int j = i + 1; j < tempv.size(); j++)
		{
			sprintf(istr, "%s", tempv[j].c_str());

			if (matchString(1, sdps, istr, strTemp))
			{
				strcpy(sessName, strTemp);
				ZeroMemory(strTemp, 100);
			}
			else
			{
				if (matchString(1, sdpm, istr, strTemp))
				{
					mediano++;           //没有考虑多个流的情况，没有判断流的最大数目
					strcpy(media[mediano].type, strTemp);
					ZeroMemory(strTemp, 100);
					if (matchString(2, sdpm, istr, strTemp))
					{
						media[mediano].multiPort = atoi(strTemp);
						ZeroMemory(strTemp, 100);
					}

					if (Conntype == 0)//11.10
					{
						if ((res = TCPSocketBind()) != 0)	return res;

						media[mediano].uniPort = TCPlLastPort;
					}

					if (Conntype == 1)  //11.10
					{
						if ((res = GetSomePort()) != 0)		return res;

						media[mediano].uniPort = lLastPort;
					}
				}
				else
				{
					if (matchString(1, sdpa, istr, strTemp))
					{
						sprintf(media[mediano].setupInfo, "/%s", strTemp);
						ZeroMemory(strTemp, 100);
					}
				}
			}
		}

		if (mediano <= 0)
		{
			GSetLogError(_T("Describe Response streamnum = 0"));
			return ErrDescribeStreamNum;
		}
		m_StreamNum = mediano;
	}

	//////////11.10///////////
	char* strDataServPort = "server_port=(.*)";
	if (type == SETUP)
	{
		if (matchString(1, strDataServPort, istr, strTemp))
		{
			tcpPort = atoi(strTemp);
			ZeroMemory(strTemp, 100);

			if (Conntype == 0)
			{
				if (!TCPConnect())	return -1;
			}
		}
		else
		{
			GSetLogError(_T("SETUP 回复没有端口"));
			return -1;
		}
	}
	//////////11.10///////////

	//10.21	 
	//10.26
	int ilength = resplen - lencur;

	if (type == GET)  //GET类型时因为收到的RTSP命令长度可能会很长，所以将contentbody部分放到m_PBRecv，然后在
		//while(resplen < lencur + bodylen)	{}里把所有的contentbody收全。
	{
		if (bodylen > 0)
		{
			m_PBRecvLength = bodylen;
			TCHAR strtemp[100];
			wsprintf(strtemp, _T("m_PBRecvLength = %d"), m_PBRecvLength);
			GLogError(strtemp);
			m_PBRecv = new char[bodylen + 1];
			if (m_PBRecv != NULL)
			{
				memset(m_PBRecv, 0, bodylen + 1);
				memcpy(m_PBRecv, respStr + lencur, ilength);
			}
			else
			{
				GSetLogError(_T("申请内存失败。。"));
				return -1;
			}
		}
		else	m_PBRecvLength = 0;
	}
	//10.26

	while (resplen < lencur + bodylen)
	{
		//	recvLen = receive(respStr + resplen);
		recvLen = receive(m_PBRecv + ilength);
		if (recvLen == -1)		return  ErrPRecvSocket;

		resplen = resplen + recvLen;
		ilength = ilength + recvLen;
		//split(respStr,szCrLf,tempv);			
	}

	if (type == OPTIONS)
	{
		if (bodylen > 0)
		{
			m_PBRecv = new char[bodylen + 1];
			if (m_PBRecv == NULL)	return 0;

			memset(m_PBRecv, 0, bodylen + 1);
			m_PBRecvLength = bodylen;
			memcpy(m_PBRecv, respStr + lencur, bodylen);
		}
		else	m_PBRecvLength = 0;
	}

	if (type == PLAY)
	{
		if (bodylen > 0)
		{
			m_headerLength = bodylen;
			memcpy(m_fileHeader, respStr + lencur, m_headerLength);
		}
		wsprintf(LastError, _T("File Header length is%d"), m_headerLength);
		GLogError(LastError);
	}

	return 0;
}

int CRTSPclient::InitRTPSession(int nPortBase, MyRTPSession** ppRTPSession)
{
	*ppRTPSession = NULL;
	MyRTPSession *pSession = new MyRTPSession();

	if (pSession == NULL)
	{
		GSetLogError(_T("InitRTPSession can't get memory"));
		return -100;
	}

	RTPUDPv4TransmissionParams transparams;
	RTPSessionParams sessParams;
	sessParams.SetOwnTimestampUnit(1.0 / 8000.0);

	sessParams.SetAcceptOwnPackets(true);
	transparams.SetPortbase(nPortBase);
	sessParams.SetUsePollThread(false);

	sessParams.SetReceiveMode(RTPTransmitter::ReceiveMode::AcceptAll);
	sessParams.SetMaximumPacketSize(RTPMAXPACKSIZE);                      //这个值需要修改

	int status = pSession->Create(sessParams, &transparams);

	if (status<0)
	{
		GLogError(_T("初始化RTPSessionFailed"));

		delete pSession;
		pSession = NULL;
		return status;
	}

	*ppRTPSession = pSession;
	pSession = NULL;
	return 0;
}

int CRTSPclient::UninitRTPSession()
{
	if (m_pRTPSession != NULL)
	{
		GLogError(_T("UninitRTPSession start"));
		m_pRTPSession->BYEDestroy(RTPTime(10, 0), 0, 0);

		delete m_pRTPSession;
		m_pRTPSession = NULL;
	}

	GLogError(_T("UninitRTPSession end"));
	return 0;
}

int CRTSPclient::recvTCPData(const int datalength, char* pBuffer)
{
	int recvgrobal = 0;
	while (recvgrobal < datalength)
	{
		int recvlen = -1;
		if ((recvlen = recv(data_socket, pBuffer + recvgrobal, datalength - recvgrobal, 0)) != SOCKET_ERROR)
		{
			//	GLogError(respstr);	
			recvgrobal = recvgrobal + recvlen;
			if (recvlen == 0)
				return SOCKET_ERROR;//对方端口已关闭,不是SOCKET_ERROR 04-28 滕艳芝
		}
		else	return SOCKET_ERROR;
	}

	return recvgrobal;
}

int CRTSPclient::ReadTCPData(DWORD DClient)
{
	int dataLength;
	CRTSPclient* pClient = (CRTSPclient*)DClient;

	packetheader struheader;
	int headLength = 0;

	dataLength = recv(data_socket, (char*)&struheader, sizeof(packetheader), 0);
	if (dataLength != SOCKET_ERROR)
	{
		if (dataLength == sizeof(packetheader))
		{
//			TCHAR err[1024];
// 			wsprintf(err,_T("struheaderis %02x,%02x"), struheader.head,struheader.tail);
// 			GLogError(err);		

			if (struheader.head == 0xEB90EB90 && struheader.tail == 0x90EB90EB)
			{
//				wsprintf(err,_T("第%d包 body长度为%d。"), numcount,struheader.body);
//				GLogError(err);

				//ZeroMemory(TCP_Buffer,TCPBUFSIZE);  

				if (struheader.body > TCPBUFSIZE)
				{
					GLogError(_T("长度大于缓冲区."));
					return -1;
				}
				else
				{
					dataLength = recvTCPData(struheader.body, (char*)TCP_Buffer);
					if (dataLength == SOCKET_ERROR)
					{
						Sleep(250);
						return -1;
					}
					if (struheader.body != dataLength)
						return -1;

//  				sprintf(err,"第%d包 长度为%d。",numcount,dataLength);
// 					numcount++;
//  				GLogError(err);

					if (dataLength>0)
					{
// 						if (hWnd != NULL)  //10.23
// 							pfnIPLY_InputStreamData((HANDLE)pClient->m_hPort, (unsigned char*)TCP_Buffer, dataLength,NULL,0);  

						if (m_pRecFunc != NULL)
						{
							//Client里用，
							//m_pRecFunc((DWORD)this,m_iUserData,1,(char*)TCP_Buffer,dataLength);

							//TRANS里用，二者使用不同的回调函数
							pClient->m_pRecFunc(this, TCP_Buffer, dataLength, m_dwCookie, m_dwExParamIN);
						}
					}
				}
			}
		}
		else if (dataLength == 0)	return -1;
	}
	else
	{
		Sleep(250); //发生错误时延时
		return -1;
	}

	return 0;

}

int numcount = 0;
int CRTSPclient::ReadRTPData(DWORD DClient, MyRTPSession* pRTPSession)
{
	int status;
	unsigned char *pfBuffer;
	int dataLength;
	CRTSPclient* pClient = (CRTSPclient*)DClient;
	char buffLen[100];
	memset(buffLen, 0, 100);


	RTPTime tm(5, 0);
	bool bDataAval = false;
	pRTPSession->WaitForIncomingData(tm, &bDataAval);

	if (bDataAval)
	{

		if (pRTPSession->GotoFirstSourceWithData())
		{
			do{
				RTPPacket *pack;

				while ((pack = pRTPSession->GetNextPacket()) != NULL)
				{
					dataLength = pack->GetPayloadLength();
					pfBuffer = (unsigned char*)pack->GetPayloadData();

//	 				numcount ++;
//	 				wsprintf(buffLen,_T("第%d包，payload长度=%d."),numcount,dataLength);
//  				GLogError(buffLen);	

					if (dataLength > 0 && pfBuffer != NULL)
					{

						//下面为分包后的优化过程。当包长超长后，将RTP数据分包。marker为最后一个分包。

						if (pack->HasMarker())
						{

							if (Rtp_Buffer_plength == 0)
							{
								if (pClient->m_pRecFunc != NULL)
								{
//	 								numcount ++;
//	 								ssprintf(buffLen,_T("第%d包，长度=%d."),numcount,Rtp_Buffer_plength);
//  								GLogError(buffLen);	
									pClient->m_pRecFunc(this, pfBuffer, dataLength, m_dwCookie, m_dwExParamIN);
								}
							}
							else
							{
								if (Rtp_Buffer_plength + dataLength > MAX_RTP_BUFFER_SIZE)
									GLogError(_T("大于RTP数据接收缓冲区的长度"));
								else
								{
									memcpy(Rtp_Buffer + Rtp_Buffer_plength, pfBuffer, dataLength);
									Rtp_Buffer_plength += dataLength;

									if (pClient->m_pRecFunc != NULL)
									{
// 										numcount ++;
// 										wsprintf(buffLen,_T("第%d包，长度=%d."),numcount,Rtp_Buffer_plength);
// 										GLogError(buffLen);

										pClient->m_pRecFunc(this, Rtp_Buffer, Rtp_Buffer_plength, m_dwCookie, m_dwExParamIN);
									}
								}
							}

							Rtp_Buffer_plength = 0;   //整包处理完毕，长度清零.
						}
						else
						{
							if (Rtp_Buffer_plength + dataLength > MAX_RTP_BUFFER_SIZE)
							{
								GLogError(_T("大于RTP数据接收缓冲区的长度"));
							}
							else
							{
								memcpy(Rtp_Buffer + Rtp_Buffer_plength, pfBuffer, dataLength);
								Rtp_Buffer_plength += dataLength;
							}
						}
					}

					pRTPSession->DeletePacket(pack);
				}
			} while (pRTPSession->GotoNextSourceWithData());
		}
	}

	status = pRTPSession->Poll();
	return 0;
}

int CRTSPclient::ParseInput(const char * serverURL)
{
	serverport = 1;
	//sprintf(serverURL,"rtsp://192.0.1.179/192.0.1.173:8000:HIK-DS8000HC:0:0:admin:12345/av_stream");
	int res = 0;
	char *strServerIP, *strServerPort, *strFile;  // host, port, file

	char st[MAX_STR];
	memset(st, 0, MAX_STR);      //teng new

	strServerIP = (LPSTR)strstr(serverURL, "//");
	if (strServerIP == NULL)
		strServerIP = (char *)serverURL; //
	else
	{
		strServerIP += 2;
		if (strnicmp(serverURL, "RTSP", 4) != 0)
			return 1; // Bad protocol
	}

	strFile = strchr(strServerIP, '/');
	if (strFile == NULL)
		return 2; // No file specified, there's no default like in HTTP

	strServerPort = strchr(strServerIP, ':');

	int size = (size_t)(strFile - strServerPort - 1);
	if (size <= 0)
	{
		serverport = RTSP_SEVER_PORT;
		strncpy(host, strServerIP, (size_t)(strFile - strServerIP));
	}
	else
	{
		strncpy(st, strServerPort + 1, (size_t)(strFile - strServerPort - 1));
		serverport = atoi(st);
		strncpy(host, strServerIP, (size_t)(strServerPort - strServerIP));
	}

	strcpy(file, strFile + 1);
	return res;
}
