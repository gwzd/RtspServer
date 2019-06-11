// DFJK.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include "PlugInMain.h"
#include "VectorRecord.h"
#include <stdio.h>
#include <string.h>

#include <vector>
#include <functional>
#include <iostream>
#include "Utilities.h"
#include "RtspClient.h"

TCHAR g_szLastError[1200];

//#pragma  comment(lib,"client")

BOOL APIENTRY DllMain(HANDLE hModule,
	DWORD  ul_reason_for_call,
	LPVOID lpReserved
	)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}

	return TRUE;
}
//网络传输类型

typedef struct _tagRTSPADDR
{
	char ServerIP[16];
	WORD ServerPort;
	char file[256];
	char  m_username[256];
	char  m_password[256];
	//char  hostinfo[256];
	char  m_transPath[256];
	char  m_remotefile[256];
	TRANSTYPE m_transtype;

}RTSPADDR, *PRTSPADDR;

using namespace std;

static char * __cdecl strtok_mt(
	const char * input,
	char ** nextoken,
	const char * control,
	char *buffer
	)
{
	unsigned char *str;
	const unsigned char *ctrl = (const unsigned char *)control;

	unsigned char map[32];
	int count;

	/* Clear control map */
	for (count = 0; count < 32; count++)
		map[count] = 0;

	/* Set bits in delimiter table */
	do {
		map[*ctrl >> 3] |= (1 << (*ctrl & 7));
	} while (*ctrl++);

	/* Initialize str. If string is NULL, set str to the saved
	* pointer (i.e., continue breaking tokens out of the string
	* from the last strtok call) */
	if (input)	str = (unsigned char*)input;
	else		str = (unsigned char*)*nextoken;

	/* Find beginning of token (skip over leading delimiters). Note that
	* there is no token iff this loop sets str to point to the terminal
	* null (*str == '\0') */
	while ((map[*str >> 3] & (1 << (*str & 7))) && *str)
		str++;

	input = (char*)str;
	strcpy(buffer, input);

	/* Find the end of the token. If it is not the end of the string,
	* put a null there. */
	for (; *str; str++)
	{
		if (map[*str >> 3] & (1 << (*str & 7))) {
			int len = (char*)(str)-input;
			strncpy(buffer, input, len);
			buffer[len] = 0;
			*str++;
			break;
		}
	}
	/* Update nextoken (or the corresponding field in the per-thread data
	* structure */
	*nextoken = (char*)str;

	/* Determine if a token has been found. */
	if (input == (char*)str)	return NULL;

	return buffer;
}

int SplitString(const char *szSource, const char* szDelimiter, vector<std::string> &arrString)
{
	char * delimiter = (char*)szDelimiter;
	if (szDelimiter == NULL)
		delimiter = ";";
	int count = 0;
	arrString.clear();
	if (szSource != NULL)
	{
		char *token;
		char *nextoken;
		char buffer[1024];
		token = (char*)strtok_mt(szSource, &nextoken, delimiter, buffer);
		if (token != NULL)
		{
			arrString.push_back(token);
			count++;
			while (token != NULL)
			{
				token = (char*)strtok_mt(NULL, &nextoken, delimiter, buffer);
				if (token != NULL)
				{
					arrString.push_back(token);
					count++;
				}
				else	break;
			}
		}
	}
	return count;
}

BOOL DFJK_API CALL_TYPE ISYS_GetLastError(
	LPTSTR lpszErrorMsg,
	UINT nBufferLen
	)
{
	if (wcslen(GGetError())>nBufferLen)
	{
		wsprintf(lpszErrorMsg, _T("错误字符串太长，溢出。。"));
		return FALSE;
	}

	if (lpszErrorMsg != NULL)
	{
		wsprintf(lpszErrorMsg, GGetError());
		return TRUE;
	}

	return FALSE;
}

std::string GetTagValue(const std::string& strExp, LPCSTR lpszTagName)
{
	char *szBuf;

	USES_CONVERSION;
	szBuf = (LPSTR)lpszTagName;
	size_t nPos1 = strExp.find(szBuf);
	if (nPos1 != string::npos)
	{
		nPos1 += strlen(szBuf);
		size_t nPos2 = strExp.find(";", nPos1);
		if (nPos2 != string::npos)
			return strExp.substr(nPos1, nPos2 - nPos1);

		return strExp.substr(nPos1);
	}

	return "";
}

//////////连接设备函数
/*
lpszServerIP(设备IP)、
dwPort（设备端口）、
lpszUserName(用户名字)、
lpszPassword（用户密码）、
tType(转发类型)（0、tcp，1、UDP）、
pExParamIn的格式为：CONN_PROXY_INFO=172.20.42.134:554;TYPENAME=DFHK。
dwExParamInLen（pExParamIn的长度，目前没用）

将参数值传入PRTSPADDR类型结构体，并输出结构体地址。

并没有真正的链接设备。
*/

extern "C"  BOOL DFJK_API CALL_TYPE IDVR_ConnectDevice(LPCSTR  lpszServerIP,
	DWORD dwPort,
	LPCSTR lpszUserName,
	LPCSTR lpszPassword,
	TRANSTYPE tType,
	HANDLE *phDevHandle,
	LPCSTR pExParamIn,
	DWORD dwExParamInLen)
	//解析出转发路径的第一个serverIP和serverPort
{
	*phDevHandle = NULL;

	if (pExParamIn == NULL)
	{
		GSetLogError(_T("IDVR_ConnectDevice failed! TransPath Is NULL!"));
		return FALSE;
	}

	GLogError(CString(pExParamIn));

	USES_CONVERSION;
	string ExParamIn = pExParamIn;
	std::string strTransPath = GetTagValue(ExParamIn, "CONN_PROXY_INFO");
	std::string strDeviceType = GetTagValue(ExParamIn, "TYPENAME");
	std::string strRemoteFile = GetTagValue(ExParamIn, "REMOTEFILE");

	if (strTransPath.empty())
	{
		GSetLogError(_T("IDVR_ConnectDevice failed! TransPath Is NULL!"));
		return FALSE;
	}

	if (strDeviceType.empty())
	{
		GSetLogError(_T("IDVR_ConnectDevice failed! TYPENAME Is NULL!"));
		return FALSE;
	}

	PRTSPADDR RtspAddr = new RTSPADDR;

	if (RtspAddr == NULL)
	{
		GSetLogError(_T("IDVR_ConnectDevice failed! Can't get memory!"));
		return FALSE;
	}

	memset(RtspAddr->file, 0, 256);
	memset(RtspAddr->m_password, 0, 256);
	memset(RtspAddr->m_remotefile, 0, 256);
	memset(RtspAddr->m_username, 0, 256);
	memset(RtspAddr->ServerIP, 0, 16);
	RtspAddr->ServerPort = 554;

	if (tType>2)
		RtspAddr->m_transtype = (TRANSTYPE)(tType - 3); //传入的连接方式 
	else
		RtspAddr->m_transtype = tType;  //0,1传入的连接方式

	// 	/////////////
	// 	char err[100];
	// 	sprintf(err,"tType = %d ------",tType);
	// 	GLogError(err);		
	// 	////////////

	if (!strRemoteFile.empty())
		sprintf(RtspAddr->m_remotefile, "%s", strRemoteFile.c_str());

	sprintf(RtspAddr->m_transPath, "%s", strTransPath.c_str());

	vector<string> tempv;
	vector<string> endpointArray;

	SplitString(strTransPath.c_str(), "/", tempv);

	if (tempv.empty())	SplitString(strTransPath.c_str(), ":", endpointArray);
	else				SplitString(tempv[0].c_str(), ":", endpointArray);

	if (endpointArray.empty())
	{
		GSetLogError(_T("IDVR_ConnectDevice failed! TransPath is not correct!"));
		delete RtspAddr;
		return FALSE;
	}

	if (endpointArray.size()>1)
		RtspAddr->ServerPort = atoi(endpointArray[1].c_str());

	sprintf(RtspAddr->ServerIP, "%s", endpointArray[0].c_str());
	sprintf(RtspAddr->file, "%s:%d:%s:", lpszServerIP, dwPort, strDeviceType.c_str());  //only DFHK  

	memset(RtspAddr->m_username, 0, 256);
	memset(RtspAddr->m_password, 0, 256);

	if (lpszUserName != NULL)
		strcpy(RtspAddr->m_username, lpszUserName);
	if (lpszPassword != NULL)
		strcpy(RtspAddr->m_password, lpszPassword);


	*phDevHandle = (HANDLE)RtspAddr;

	TCHAR strerr[200];
	wsprintf(strerr, _T("IDVR_ConnectDevice transType = %d,serverIP is %s,dwPort is %d,userName is %s,password is %s"),
		tType, lpszServerIP, dwPort, lpszUserName, lpszPassword);
	GLogError(strerr);

	GLogError(_T("IDVR_ConnectDevice return true."));
	return TRUE;
}

///////////////////// 云台控制函数
/*
通过发送Options命令来进行云台控制
*/

extern "C"  BOOL DFJK_API CALL_TYPE IDVR_PTZControlByChannel(HANDLE hChanHandle,
	CTRLTYPE command,
	DWORD dwParam1,
	DWORD dwParam2,
	LPCSTR pExParamIn,
	DWORD dwExParamInLen)
{
	if (hChanHandle == NULL || (long)hChanHandle == -1)
	{
		wsprintf(g_szLastError, _T(" IDVR_PTZControlByChannel failed,hchanHandle = NULL！"));
		GSetLogError(g_szLastError);
		return FALSE;
	}

	CRTSPclient* RtspClient = (CRTSPclient*)hChanHandle;
	if (RtspClient)
	{
		RtspClient->sendOptions(command, dwParam1, dwParam2, 0, 0, (char*)pExParamIn);
		return (RtspClient->RecvResponse(OPTIONS, 0) == 0);
	}

	return FALSE;
}

extern "C"  BOOL DFJK_API CALL_TYPE IDVR_DisConnectDevice(HANDLE hDevHandle)
{
	if (hDevHandle)
	{
		PRTSPADDR RtspAddr = (PRTSPADDR)hDevHandle;
		delete RtspAddr;
		RtspAddr = NULL;
		return TRUE;
	}
	else	return FALSE;
}

extern "C" BOOL DFJK_API CALL_TYPE IDVR_StartRealData(HANDLE hDevHandle,
	int iChannel,
	int iStreamType,
	HANDLE *phChanHandle,
	DWORD dwCookie,
	LPCSTR pExParamIn,
	DWORD dwExParamInLen,
	RealDataCallBack callbackHandler)
{
	GLogError(_T("enter IDVR_StartRealData "));
	if (hDevHandle == NULL || (long)hDevHandle == -1)
	{
		wsprintf(g_szLastError, _T(" IDVR_StartRealData failed,hDevHandle = NULL！"));
		GSetLogError(g_szLastError);
		return FALSE;
	}

	*phChanHandle = NULL;

	CRTSPclient* RtspClient = new CRTSPclient();
	if (RtspClient == NULL)
	{
		GSetLogError(_T("IDVR_StartRealData can't get memory."));
		return -1;
	}
	PRTSPADDR RtspAddr = (PRTSPADDR)hDevHandle;

	if (strlen(RtspAddr->m_remotefile) > 0)
		strcpy(RtspClient->m_szRemoteFileName, RtspAddr->m_remotefile);

	RtspClient->Conntype = RtspAddr->m_transtype;

	/////////////
	TCHAR err[100];
	wsprintf(err, _T("RtspClient->Conntype = %d"), RtspClient->Conntype);
	GLogError(err);
	////////////

	strcpy(RtspClient->host, RtspAddr->ServerIP);
	RtspClient->serverport = RtspAddr->ServerPort;

	sprintf(RtspClient->strURL, "rtsp://%s/%s%d:%d:%s:%s/av_stream", RtspAddr->m_transPath, RtspAddr->file,
		iChannel, iStreamType, RtspAddr->m_username, RtspAddr->m_password);

	RtspClient->hWnd = NULL;
	RtspClient->m_pRecFunc = callbackHandler;

	RtspClient->m_dwCookie = (DWORD)dwCookie;
	RtspClient->m_dwExParamIN = (DWORD)pExParamIn;


	int i = RtspClient->Connect();
	if (i == 0)
	{
		*phChanHandle = RtspClient;
		return TRUE;
	}

	delete RtspClient;
	RtspClient = NULL;
	return FALSE;

}
extern "C"  BOOL DFJK_API CALL_TYPE IDVR_RemotePlay(HANDLE hDevHandle,
	LPCSTR lpszSrcFileName,
	HWND hWnd,
	HANDLE* hPlay)
{
	CRTSPclient* RtspClient = new CRTSPclient();

	if (RtspClient == NULL)
	{
		GSetLogError(_T("IDVR_RemotePlay  can't get memory."));
		return FALSE;
	}

	PRTSPADDR RtspAddr = (PRTSPADDR)hDevHandle;

	USES_CONVERSION;
	strcpy(RtspClient->m_szRemoteFileName, lpszSrcFileName);

	RtspClient->Conntype = RtspAddr->m_transtype;

	/////////////
	TCHAR err[100];
	wsprintf(err, _T("RtspClient->Conntype = %d"), RtspClient->Conntype);
	GLogError(err);
	////////////

	strcpy(RtspClient->host, RtspAddr->ServerIP);
	RtspClient->serverport = RtspAddr->ServerPort;

	sprintf(RtspClient->strURL, "rtsp://%s/%s%d:%d:%s:%s/av_stream", RtspAddr->m_transPath, RtspAddr->file,
		1, 0, RtspAddr->m_username, RtspAddr->m_password);

	RtspClient->hWnd = NULL;

	int i = RtspClient->Connect();
	if (i == 0)
	{
		*hPlay = RtspClient;
		return TRUE;
	}

	delete RtspClient;
	return FALSE;
}

extern "C"  BOOL DFJK_API CALL_TYPE IDVR_StopRemotePlay(HANDLE hPlay)
{
	if (hPlay == NULL || (long)hPlay == -1)
	{
		GSetLogError(_T("IDVR_SetRemotePlayDataCallBack hPlay = NULL"));
		return FALSE;
	}

	CRTSPclient* RtspClient = (CRTSPclient*)hPlay;
	if (RtspClient)
	{
		if (RtspClient->Disconnect() != 0)
		{
			GSetLogError(_T("Disconnect Failed!"));

			delete RtspClient;
			RtspClient = NULL;

			return FALSE;
		}

		delete RtspClient;
		RtspClient = NULL;
		return TRUE;
	}

	return FALSE;
}


extern "C"  BOOL DFJK_API CALL_TYPE IDVR_SetRemotePlayDataCallBack(HANDLE hPlay,
	DWORD dwCookie,
	LPCSTR pExParamIn,
	DWORD dwExParamInLen,
	RealDataCallBack callbackHandler)
{
	if (hPlay == NULL || (long)hPlay == -1)
	{
		GSetLogError(_T("IDVR_SetRemotePlayDataCallBack hPlay = NULL"));
		return FALSE;
	}

	CRTSPclient* RtspClient = (CRTSPclient*)hPlay;
	RtspClient->m_pRecFunc = callbackHandler;

	RtspClient->m_dwCookie = (DWORD)dwCookie;
	RtspClient->m_dwExParamIN = (DWORD)pExParamIn;

	return TRUE;
}

extern "C"  BOOL DFJK_API CALL_TYPE ISYS_GetPluginInfo(PLUGIN_INFO *pInfo)
{
	pInfo->invalid_handle_value = -1;
	strcpy(pInfo->DeviceTypeName, "TRANS");
	return FALSE;
}

extern "C"  BOOL DFJK_API CALL_TYPE ISYS_Initialize(void *pvReserved, DWORD reserved)
{
	WORD wVersionRequested;
	WSADATA wsaData;
	int err;

	UNREFERENCED_PARAMETER(pvReserved);
	UNREFERENCED_PARAMETER(reserved);

	wVersionRequested = MAKEWORD(1, 1);

	err = WSAStartup(wVersionRequested, &wsaData);
	if (err != 0)
	{
		GSetLogError(_T("Could not initialize sockets,Rtspclient cannot start!"));
		return false;
	}

	return TRUE;
}

extern "C"  void DFJK_API CALL_TYPE ISYS_Uninitialize()
{
	WSACleanup();
	CLastError::FreeInstance();
	GLogError(_T("FiniStreamClientLib Success"));
//	return TRUE;
}

extern "C" BOOL DFJK_API CALL_TYPE IDVR_StopRealData(HANDLE hChanHandle)
{
	if (hChanHandle == NULL || (long)hChanHandle == -1)
	{
		wsprintf(g_szLastError, _T(" IDVR_StopRealData failed,hchanHandle = NULL！"));
		GSetLogError(g_szLastError);
		return FALSE;
	}

	CRTSPclient* RtspClient = (CRTSPclient*)hChanHandle;
	if (RtspClient)
	{
		if (RtspClient->Disconnect() != 0)
		{
			GSetLogError(_T("Disconnect Failed!"));
			delete RtspClient;
			RtspClient = NULL;
			return FALSE;
		}

		delete RtspClient;
		RtspClient = NULL;
		return TRUE;
	}

	return FALSE;
}

extern "C"	BOOL DFJK_API CALL_TYPE IDVR_GetRealDataHeader(HANDLE hChanHandle,
	unsigned char *pBuffer,
	DWORD dwBuffLen,
	DWORD *pdwHeaderLen)
{
	if (hChanHandle == NULL || (long)hChanHandle == -1)
	{
		wsprintf(g_szLastError, _T(" IDVR_GetRealDataHeader failed,hchanHandle = NULL！"));
		GSetLogError(g_szLastError);
		return FALSE;
	}

	CRTSPclient* RtspClient = (CRTSPclient*)hChanHandle;

	if (RtspClient)
	{
		if (dwBuffLen < RtspClient->m_headerLength)
		{
			wsprintf(g_szLastError, _T(" HIKS_GetRealDataHeader failed,缓冲区长度太小！"));
			GSetLogError(g_szLastError);
			return FALSE;
		}

		*pdwHeaderLen = RtspClient->m_headerLength;
		if (RtspClient->m_headerLength >0)
		{
			memcpy(pBuffer, RtspClient->m_fileHeader, RtspClient->m_headerLength);
			return TRUE;
		}
		else	return TRUE;
	}

	return FALSE;
}

extern "C" BOOL DFJK_API CALL_TYPE ISYS_FreeMemory(void* pMem)
{
	if (pMem)
	{
		delete[]pMem;
		pMem = NULL;
		return TRUE;
	}
	return FALSE;
}

//10.30
extern "C" BOOL DFJK_API CALL_TYPE IDVR_QueryDevRecord(HANDLE hDevHandle,
	int iChannel,
	RECORDTYPE RecType,
	PQUERYTIME pTime,
	LPSTR *ppExParamOut,
	DWORD* pdwExParamOutLen)
{
	if (hDevHandle == NULL || (long)hDevHandle == 0)
	{
		wsprintf(g_szLastError, _T("IDVR_QueryDevRecord hDevHandle=NULL"));
		GSetLogError(g_szLastError);
		return FALSE;
	}

	PRTSPADDR RtspAddr = (PRTSPADDR)hDevHandle;

	CRTSPclient* RtspClient = new CRTSPclient();

	if (RtspClient == NULL)
	{
		GSetLogError(_T("IDVR_QueryDevRecord,get memory failed."));
		return FALSE;
	}

	strcpy(RtspClient->host, RtspAddr->ServerIP);
	RtspClient->serverport = RtspAddr->ServerPort;

	sprintf(RtspClient->strURL, "rtsp://%s/%s%d:%d:%s:%s/av_stream", RtspAddr->m_transPath, RtspAddr->file,
		iChannel, 0, RtspAddr->m_username, RtspAddr->m_password);

	if ((RtspClient->ParseInput(RtspClient->strURL)) == 0)
	{
		if (RtspClient->ServConnect() == 0)
		{
			GLogError(_T("ServConnect success"));
		}
		else
		{
			GSetLogError(_T("不能连接流媒体服务器"));
			return FALSE;
		}
	}
	else
	{
		wsprintf(g_szLastError, _T("URL字符串格式不对"));
		GSetLogError(g_szLastError);
	}

	char strtime[30];
	memset(strtime, 0, 30);
	sprintf(strtime, "%4d%02d%02d%02d%02d%02d-%4d%02d%02d%02d%02d%02d",
		pTime->ufromyear,
		pTime->ufrommonth,
		pTime->ufromday,
		pTime->ufromhour,
		pTime->ufromminute,
		pTime->ufromsecond,
		pTime->utoyear,
		pTime->utomonth,
		pTime->utoday,
		pTime->utohour,
		pTime->utominute,
		pTime->utosecond
		);

	RtspClient->sendGets(RtspClient->strURL, RecType, strtime);
	int iRet = RtspClient->RecvResponse(GET, 0);

	RtspClient->CloseSocket();
	if (iRet == 0)
	{
		*pdwExParamOutLen = RtspClient->m_PBRecvLength;

		if (RtspClient->m_PBRecvLength != 0)
		{
			*ppExParamOut = new char[RtspClient->m_PBRecvLength + 1];

			if (*ppExParamOut == NULL)
			{
				delete[] RtspClient->m_PBRecv;
				RtspClient->m_PBRecv = NULL;
				delete RtspClient;
				RtspClient = NULL;

				GSetLogError(_T("IDVR_QueryDevRecord failed. get filelistmemory failed"));
				return FALSE;
			}

			strcpy(*ppExParamOut, RtspClient->m_PBRecv);
			delete[] RtspClient->m_PBRecv;
			RtspClient->m_PBRecv = NULL;
		}

		delete RtspClient;
		RtspClient = NULL;
		return TRUE;
	}
	else
	{
		GLogError(_T("Receive get failed"));
		if (RtspClient->m_PBRecv != NULL && RtspClient->m_PBRecvLength != 0)
		{
			delete[] RtspClient->m_PBRecv;
			RtspClient->m_PBRecv = NULL;
		}

		delete RtspClient;
		RtspClient = NULL;
		return FALSE;
	}
}

extern "C" BOOL DFJK_API CALL_TYPE IDVR_PauseRemotePlay(HANDLE hPlay)
{
	if (hPlay == NULL)
	{
		wsprintf(g_szLastError, _T("hSession = NULL IDVR_PauseRemotePlay FAILED!"));
		GSetLogError(g_szLastError);
		return -1;
	}

	CRTSPclient* RtspClient = (CRTSPclient*)hPlay;
	RtspClient->sendOptions(NET_DVR_PLAYPAUSE, 0, 0, 0, 0, "");
	if (RtspClient->RecvResponse(OPTIONS, 0) == 0)
		return TRUE;

	return FALSE;
}

extern "C" BOOL DFJK_API CALL_TYPE IDVR_ResumeRemotePlay(HANDLE hPlay)
{
	if (hPlay == NULL)
	{
		wsprintf(g_szLastError, _T("hSession = NULL IDVR_ResumeRemotePlay FAILED!"));
		GSetLogError(g_szLastError);
		return FALSE;
	}

	CRTSPclient* RtspClient = (CRTSPclient*)hPlay;
	RtspClient->sendOptions(NET_DVR_PLAYRESTART, 0, 0, 0, 0, "");
	if (RtspClient->RecvResponse(OPTIONS, 0) == 0)
		return TRUE;

	return FALSE;
}

extern "C" BOOL DFJK_API CALL_TYPE IDVR_FastRemotePlay(HANDLE hPlay)
{
	if (hPlay == NULL)
	{
		wsprintf(g_szLastError, _T("hSession = NULL IDVR_FastRemotePlay FAILED!"));
		GSetLogError(g_szLastError);
		return FALSE;
	}

	CRTSPclient* RtspClient = (CRTSPclient*)hPlay;
	RtspClient->sendOptions(NET_DVR_PLAYFAST, 0, 0, 0, 0, "");
	if (RtspClient->RecvResponse(OPTIONS, 0) == 0)
		return TRUE;

	return FALSE;
}

extern "C" BOOL DFJK_API CALL_TYPE IDVR_SlowRemotePlay(HANDLE hPlay)
{
	if (hPlay == NULL)
	{
		wsprintf(g_szLastError, _T("hSession = NULL IDVR_SlowRemotePlay FAILED!"));
		GSetLogError(g_szLastError);
		return FALSE;
	}

	CRTSPclient* RtspClient = (CRTSPclient*)hPlay;
	RtspClient->sendOptions(NET_DVR_PLAYSLOW, 0, 0, 0, 0, "");
	if (RtspClient->RecvResponse(OPTIONS, 0) == 0)
		return TRUE;

	return FALSE;
}

extern "C" BOOL DFJK_API CALL_TYPE IDVR_NormalRemotePlay(HANDLE hPlay)
{
	if (hPlay == NULL)
	{
		wsprintf(g_szLastError, _T("hSession = NULL IDVR_NormalRemotePlay FAILED!"));
		GSetLogError(g_szLastError);
		return FALSE;
	}

	CRTSPclient* RtspClient = (CRTSPclient*)hPlay;
	RtspClient->sendOptions(NET_DVR_PLAYNORMAL, 0, 0, 0, 0, "");
	if (RtspClient->RecvResponse(OPTIONS, 0) == 0)
		return TRUE;

	return FALSE;
}

extern "C" BOOL DFJK_API CALL_TYPE IDVR_StepRemotePlay(HANDLE hPlay)
{
	if (hPlay == NULL)
	{
		wsprintf(g_szLastError, _T("hSession = NULL IDVR_StepRemotePlay FAILED!"));
		GSetLogError(g_szLastError);
		return FALSE;
	}

	CRTSPclient* RtspClient = (CRTSPclient*)hPlay;
	RtspClient->sendOptions(NET_DVR_PLAYFRAME, 0, 0, 0, 0, "");
	if (RtspClient->RecvResponse(OPTIONS, 0) == 0)
		return TRUE;

	return FALSE;
}

extern "C" BOOL DFJK_API CALL_TYPE IDVR_GetRemotePlayPos(HANDLE hPlay, float *pfPos)
{
	if (hPlay == NULL)
	{
		wsprintf(g_szLastError, _T("hPlay = NULL IDVR_GetRemotePlayPos FAILED!"));
		GSetLogError(g_szLastError);
		return TRUE;
	}

	CRTSPclient* RtspClient = (CRTSPclient*)hPlay;
	RtspClient->sendOptions(NET_DVR_PLAYGETPOS, 0, 0, 0, 0, "");
	if (RtspClient->RecvResponse(OPTIONS, 0) == 0)
	{
		*pfPos = (float)(atof(RtspClient->m_PBRecv) / 100.0);
		delete[] RtspClient->m_PBRecv;
		RtspClient->m_PBRecv = NULL;

		wsprintf(g_szLastError, _T("server return play back = %d"), RtspClient->m_PBRecv);
		GLogError(g_szLastError);
		return TRUE;
	}

	return FALSE;
}

extern "C" BOOL DFJK_API CALL_TYPE IDVR_SetRemotePlayPos(HANDLE hPlay, float fPos)
{
	if (hPlay == NULL)
	{
		wsprintf(g_szLastError, _T("hPlay = NULL IDVR_SetRemotePlayPos FAILED!"));
		GSetLogError(g_szLastError);
		return FALSE;
	}

	CRTSPclient* RtspClient = (CRTSPclient*)hPlay;

	int pos = (int)(fPos * 100);
	RtspClient->sendOptions(NET_DVR_PLAYSETPOS, pos, 0, 0, 0, "");
	if (RtspClient->RecvResponse(OPTIONS, 0) == 0)
	{
		wsprintf(g_szLastError, _T("server return play back = %d"), RtspClient->m_PBRecv);
		GLogError(g_szLastError);
		return TRUE;
	}

	return FALSE;
}

extern "C" BOOL DFJK_API CALL_TYPE IDVR_RemotePlayByTime(HANDLE hDevHandle,
	int iChannel,
	PQUERYTIME pTime,
	HWND hWnd,
	HANDLE* hPlay)
{
	CRTSPclient* RtspClient = new CRTSPclient();
	if (RtspClient == NULL)
	{
		GSetLogError(_T("IDVR_RemotePlayByTime  failed. get memory failed."));
		return FALSE;
	}

	PRTSPADDR RtspAddr = (PRTSPADDR)hDevHandle;

	char strtime[30];
	memset(strtime, 0, 30);
	sprintf(strtime, "%4d%02d%02d%02d%02d%02d-%4d%02d%02d%02d%02d%02d",
		pTime->ufromyear,
		pTime->ufrommonth,
		pTime->ufromday,
		pTime->ufromhour,
		pTime->ufromminute,
		pTime->ufromsecond,
		pTime->utoyear,
		pTime->utomonth,
		pTime->utoday,
		pTime->utohour,
		pTime->utominute,
		pTime->utosecond
		);

	char lpszSrcFileName[60];
	memset(lpszSrcFileName, 0, 60);
	sprintf(lpszSrcFileName, "DateRange:%s", strtime);

	strcpy(RtspClient->m_szRemoteFileName, lpszSrcFileName);

	RtspClient->Conntype = RtspAddr->m_transtype;

	strcpy(RtspClient->host, RtspAddr->ServerIP);
	RtspClient->serverport = RtspAddr->ServerPort;

	sprintf(RtspClient->strURL, "rtsp://%s/%s%d:%d:%s:%s/av_stream",
		RtspAddr->m_transPath,
		RtspAddr->file,
		iChannel,
		0,
		RtspAddr->m_username,
		RtspAddr->m_password);

	RtspClient->hWnd = NULL;

	int i = RtspClient->Connect();
	if (i == 0)
	{
		*hPlay = RtspClient;
		return TRUE;
	}

	delete RtspClient;
	return FALSE;
}

//10.30
