#include "stdafx.h"
#include "Client.h"
#include "RtspClient.h"
#include <stdio.h>

extern long  g_lLastPort;
extern long g_TCPlLastPort;

TCHAR g_szLastError[1200];
HINSTANCE g_pIntance = NULL;

#define  CHECK_FUNCTION(obj,func)   \
	{						\
	if(obj->pfn##func==NULL)	{	\
		wsprintf(g_szLastError,_T("函数没有##func##导出!"));\
		GSetLogError(g_szLastError);\
		return 	-1;	\
		}\
} \

BOOL APIENTRY DllMain(HANDLE hModule,
	DWORD  ul_reason_for_call,
	LPVOID lpReserved
	)
{
	g_pIntance = (HINSTANCE)hModule;

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

void rtrim(std::string& s, char c)
{
	if (s.empty())	return;

	std::string::iterator p;
	for (p = s.end(); p != s.begin() && *--p == c;);

	if (*p != c)
		p++;

	s.erase(p, s.end());
}

CLIENT_API  int  CALL_TYPE InitStreamClientLib(void)  //初始化
{
	memset(g_szLastError, 0, 1200);

	WORD wVersionRequested;
	WSADATA wsaData;
	int err;

	wVersionRequested = MAKEWORD(1, 1);

	err = WSAStartup(wVersionRequested, &wsaData);
	if (err != 0)	return -1;

	srand((UINT)time(NULL));
	GLogError(_T("初始化流媒体插件成功。"));

	int randNum = rand();
	if (randNum % 2 == 1)
		randNum = randNum++;

	g_lLastPort = randNum % 1000;
	g_TCPlLastPort = rand() % 1000;
	return 0;
}

extern "C"  int CLIENT_API CALL_TYPE FiniStreamClientLib(void)  //关闭
{
	WSACleanup();
	CLastError::FreeInstance();
	GLogError(_T("开始释放插件..."));
	CPlugInLoader::FreeInstance();
	GLogError(_T("释放插件成功。"));
	return 0;
}

//10.26---
extern "C" BOOL CLIENT_API CALL_TYPE HIKS_ISYS_FreeMemory(void* pMem)
{
	if (pMem)
	{
		delete[]pMem;
		pMem = NULL;
		return TRUE;
	}
	return FALSE;
}
//10.26----

extern "C" HSESSION CLIENT_API CALL_TYPE HIKS_CreatePlayer(IHikClientAdviseSink* pSink,
	void* pWndSiteHandle,
	pDataRec pRecFunc,
	LPCTSTR lpszType,
	pMsgBack pMsgFunc,
	int TransMethod)//创建Player
{
	if (TransMethod < 0 || TransMethod > 2)
	{
		GSetLogError(_T("zhuanfaleixingcuowu"));
		return -1;
	}
	TCHAR strTemp[300];
	wsprintf(strTemp, _T(" ---HIKS_CreatePlayer return LpszType=%s,pMasgFunc = %d,TransMethod=%d"),
							lpszType, (DWORD)pRecFunc, TransMethod);
	GLogError(strTemp);

	CRTSPclient* RtspClient = new CRTSPclient();
	if (RtspClient == NULL)
	{
		GSetLogError(_T("HIKS_CreatePlayer can't get memory"));
		return -1;
	}

	DWORD hSession = (DWORD)RtspClient;

	RtspClient->hWnd = (HWND)pWndSiteHandle;
	RtspClient->m_pRecFunc = pRecFunc;
	RtspClient->Conntype = TransMethod;   //转发方式

	if (RtspClient->CreatPlayer(lpszType, _T("PLAY")))     //调用插件
	{
		USES_CONVERSION;
		strcpy(RtspClient->m_szDvrType, T2A(lpszType));

		TCHAR strTemp[300];
		wsprintf(strTemp, _T(" ---HIKS_CreatePlayer return HSession = %d, \
						 		LpszType=%s,pMasgFunc = %d,TransMethod=%d"),
								hSession, lpszType, (DWORD)pRecFunc, TransMethod);

		GLogError(strTemp);
		return hSession;
	}
	else
	{
		GSetLogError(lpszType);
	}
	delete RtspClient;
	RtspClient = NULL;
	return -1;
}

extern "C"  int CLIENT_API CALL_TYPE HIKS_OpenURL(HSESSION hSession,
	const char* pszURL,
	int iusrdata,
	const char* lpszRemoteFileName)//打开URL  //suc 1,fail -1
{
	//	rtsp://192.0.1.179/192.0.1.173:8000:HIK-DS8000HC:0:0:admin:12345/av_stream  //暂时先考虑一个流媒体服务器转发的情况
	/*
	TCHAR strTemp[500];
	wsprintf(strTemp, _T("HIKS_OpenURL hSession = %d,pszURL = %s,iusrdata = %d"), hSession, CString(pszURL), iusrdata);

	GLogError(strTemp);
	AfxMessageBox(strTemp);
	if (hSession == -1)
	{
		wcscpy(g_szLastError, _T("HIKS_OpenURL hSession 无效"));
		GSetLogError(g_szLastError);
		return -1;
	}*/

	CRTSPclient* RtspClient = (CRTSPclient*)hSession;
	if (RtspClient)
	{
		if (lpszRemoteFileName != NULL&&strlen(lpszRemoteFileName)>1)
		{
			strcpy(RtspClient->m_szRemoteFileName, lpszRemoteFileName);

			std::string strTemp(RtspClient->m_szRemoteFileName);
			rtrim(strTemp, '\n');
			rtrim(strTemp, '\r');
			strcpy(RtspClient->m_szRemoteFileName, strTemp.c_str());
		}

		strcpy(RtspClient->strURL, pszURL);
		RtspClient->m_iUserData = iusrdata;
		if ((RtspClient->ParseInput(pszURL)) == 0)
		{
			if (RtspClient->ServConnect() != 0)
			{
				wsprintf(g_szLastError, _T("流媒体服务器连接失败！%s"), GGetError());
				GSetError(g_szLastError);
				return -1;
			}
			if (RtspClient->Connect() == 0)
			{
				GLogError(_T("Open URL success-----"));
				return 1;
			}
			if (lpszRemoteFileName != NULL&&strlen(lpszRemoteFileName)>1)
			{
				wsprintf(g_szLastError, _T("网络故障或文件没有找到！%s"), GGetError());
				GSetError(g_szLastError);
				return -1;
			}

			wsprintf(g_szLastError, _T("视频连接失败！%s"), GGetError());
			GSetError(g_szLastError);
			return -1;
		}
		else
		{
			wsprintf(g_szLastError, _T("URL字符串格式不对"));
			GSetLogError(g_szLastError);
		}
	}

	return -1;
}

extern "C" int CLIENT_API CALL_TYPE HIKS_Play(HSESSION hSession)//播放
{
	TCHAR strTemp[40];
	wsprintf(strTemp, _T("HIKS_Play(%d)"), hSession);
	GLogError(strTemp);

	if (hSession == -1)
	{
		wcscpy(g_szLastError, _T("HIKS_Play hSession 无效"));
		GSetLogError(g_szLastError);
		return -1;
	}

	CRTSPclient* RtspClient = (CRTSPclient*)hSession;

	if (RtspClient)
	{
		if (RtspClient->StreamPlay())
		{
			GLogError(_T("play success----"));
			return 1;
		}
	}
	return -1;
}

extern "C" int  CLIENT_API CALL_TYPE HIKS_Stop(HSESSION hSession)//停止,销毁Player
{
	TCHAR strTemp[40];
	wsprintf(strTemp, _T("HIKS_Stop(%d)"), hSession);
	GLogError(strTemp);

	if (hSession == -1)
	{
		wsprintf(g_szLastError, _T("HIKS_Stop hSession 无效"));
		GSetLogError(g_szLastError);
		return -1;
	}

	CRTSPclient* RtspClient = (CRTSPclient*)hSession;
	if (RtspClient != NULL)
	{
		if (RtspClient->Disconnect() == 0)
		{
			delete RtspClient;
			RtspClient = NULL;
			return 0;
		}
	}

	return -1;
}

LPCTSTR  CALL_TYPE HIKS_GetLastError()
{
	wsprintf(g_szLastError, GGetError());
	return g_szLastError;
}

extern "C" int CLIENT_API CALL_TYPE  HIKS_Destroy(HSESSION hSession)//销毁Player作废 4.13
{
//	if (hSession == -1)
// 	{
//		sprintf(g_szLastError," HIKS_Destroy hSession 无效" ); 
//		return -1;
//	}
// 
//	CRTSPclient* RtspClient = (CRTSPclient*)hSession;
//
//	if (RtspClient)
//	{
//		if (RtspClient ->CloseStream() == 0)
// 		   return 0;
// 	}
	return -1;
}

extern "C" int CLIENT_API CALL_TYPE  HIKS_GetRealDataHeader(HSESSION hSession,
	char *pBuffer,
	DWORD dwBuffLen,
	DWORD *pdwHeaderLen)//得到文件头
{
	if (hSession == -1)
	{
		wsprintf(g_szLastError, _T("hSession = -1 HIKS_GetFileHeader FAILED!"));
		GLogError(g_szLastError);
		GSetError(_T("调用函数参数值错误！"));
		return -1;
	}

	CRTSPclient* RtspClient = (CRTSPclient*)hSession;
	if (RtspClient)
	{
		if (dwBuffLen < RtspClient->m_headerLength)
		{
			wsprintf(g_szLastError, _T(" HIKS_GetRealDataHeader failed,缓冲区长度太小！"));
			GLogError(g_szLastError);
			GSetError(_T("传入缓冲区长度太小！"));
			return -1;
		}

		*pdwHeaderLen = RtspClient->m_headerLength;
		if (RtspClient->m_headerLength >0)
		{
			memcpy(pBuffer, RtspClient->m_fileHeader, RtspClient->m_headerLength);
			return 1;
		}
	}

	return -1;
}

extern "C" int CLIENT_API CALL_TYPE  HIKS_GrabPic(HSESSION hSession,
	const char* szPicFileName,
	unsigned short byPicType)
	//success 1,fail -1;  unsigned short byPicType没用
{
	if (hSession == -1)
	{
		wsprintf(g_szLastError, _T("hSession = -1 HIKS_GrabPic FAILED!"));
		GSetLogError(g_szLastError);
		return -1;
	}

	CRTSPclient* RtspClient = (CRTSPclient*)hSession;
	CHECK_FUNCTION(RtspClient, IPLY_CapturePicture);

	if (RtspClient->pfnIPLY_CapturePicture((HANDLE)RtspClient->m_hPort, szPicFileName))
	{
		wsprintf(g_szLastError, _T("HIKS_GrabPic(%d,%s)success"), hSession, szPicFileName);
		GLogError(g_szLastError);
		return 1;
	}

	TCHAR bufString[300];
	RtspClient->pfnISYS_GetLastError(bufString, 300);

	wsprintf(g_szLastError, _T("HIKS_GrabPic(%d,%s)failed,error infor= %s"), hSession, szPicFileName, bufString);
	GLogError(g_szLastError);
	GSetError(_T("底层插件错误"));
	return -1;
}

extern "C" int CLIENT_API CALL_TYPE  HIKS_GetVideoParams(HSESSION hSession,
	int *piBrightness,
	int *piContrast,
	int *piSaturation,
	int *piHue)
{

	if (hSession == -1)
	{
		wsprintf(g_szLastError, _T("hSession = -1 HIKS_GrabPic FAILED!"));
		GSetError(g_szLastError);
		return -1;
	}

	CRTSPclient* RtspClient = (CRTSPclient*)hSession;
	if (strnicmp(RtspClient->m_szDvrType, "DFDH", 4) == 0)
	{
		GSetError(_T("大华的插件不支持 GetVideoParams!"));
		return -1;
	}

	CHECK_FUNCTION(RtspClient, IPLY_GetVideoParams);
	if (RtspClient->pfnIPLY_GetVideoParams((HANDLE)RtspClient->m_hPort, piBrightness, piContrast, piSaturation, piHue))
		return 1;

	TCHAR bufString[300];
	RtspClient->pfnISYS_GetLastError(bufString, 300);

	wsprintf(g_szLastError, _T("HIKS_GetVideoParams failed,error infor= %s"), bufString);
	GSetError(g_szLastError);
	GLogError(_T("底层插件错误"));
	return -1;
}

extern "C" int CLIENT_API CALL_TYPE  HIKS_SetVideoParams(HSESSION hSession,
	int iBrightness,
	int iContrast,
	int iSaturation,
	int iHue,
	BOOL bDefault)
{
	if (hSession == -1)
	{
		wsprintf(g_szLastError, _T("hSession = -1 HIKS_GrabPic FAILED!"));
		GSetLogError(g_szLastError);
		return -1;
	}

	CRTSPclient* RtspClient = (CRTSPclient*)hSession;

	if (strnicmp(RtspClient->m_szDvrType, "DFDH", 4) == 0)
	{
		GSetError(_T("大华的插件不支持SetVideoParams !"));
		return -1;
	}

	CHECK_FUNCTION(RtspClient, IPLY_SetVideoParams);
	if (RtspClient->pfnIPLY_SetVideoParams((HANDLE)RtspClient->m_hPort, iBrightness, iContrast, iSaturation, iHue, bDefault))
	{
		wsprintf(g_szLastError, _T("HIKS_SetVideoParams successes"));
		GLogError(g_szLastError);
		return 1;
	}

	TCHAR bufString[300];
	RtspClient->pfnISYS_GetLastError(bufString, 300);

	wsprintf(g_szLastError, _T("HIKS_SetVideoParams failed,error infor= %s"), bufString);
	GSetError(g_szLastError);
	GLogError(g_szLastError);
	return -1;
}

extern "C" int CLIENT_API CALL_TYPE HIKS_PTZControl(HSESSION hSession,            //成功为1，失败为0-1；
	unsigned int ucommand,
	int iparam1,
	int iparam2,
	int iparam3,
	int iparam4,
	LPCSTR lpszParam5)             //新添加的协议字符串lpszParam5
{
	if (hSession == -1)
	{
		wsprintf(g_szLastError, _T("hSession = -1 HIKS_GrabPic FAILED!"));
		GSetError(g_szLastError);
		return -1;
	}

	CRTSPclient* RtspClient = (CRTSPclient*)hSession;

	if (RtspClient)
	{
		RtspClient->sendOptions(ucommand, iparam1, iparam2, iparam3, iparam4, (char*)lpszParam5);
		if (RtspClient->RecvResponse(OPTIONS, 0) == 0)
			return 1;

		return -1;
	}
	return -1;
}

extern "C" int CLIENT_API CALL_TYPE  HIKS_RandomPlay(HSESSION hSession, unsigned long timepos)
{
	GSetLogError(_T("HIKS_RandomPlay 暂未实现"));
	return -1;
}

extern "C" int CLIENT_API CALL_TYPE  HIKS_Pause(HSESSION hSession)
{
	GSetLogError(_T("HIKS_Pause 暂未实现"));
	return -1;
}

extern "C"int CLIENT_API CALL_TYPE  HIKS_Resume(HSESSION hSession)//恢复
{
	GSetLogError(_T("HIKS_Resume 暂未实现"));
	return -1;
}

extern "C" int CLIENT_API CALL_TYPE  HIKS_GetCurTime(HSESSION hSession, unsigned long *utime)//获取当前的播放时间，1/64秒为单位, 目前返回都是0
{
	GSetLogError(_T("HIKS_GetCurTime 暂未实现"));
	return -1;
}

extern "C" int CLIENT_API CALL_TYPE  HIKS_ChangeRate(HSESSION hSession, int scale)//改变播放速率,在快进或慢进的时候用
{
	GSetLogError(_T("HIKS_ChangeRate 暂未实现"));
	return -1;
}

extern "C" int CLIENT_API CALL_TYPE  HIKS_SetVolume(HSESSION hSession, unsigned short volume)//设置音量 参数volume的范围是0-65535
{
	if (hSession == -1)
	{
		wsprintf(g_szLastError, _T("hSession = -1  HIKS_SetVolume FAILED!"));
		GSetError(g_szLastError);
		return -1;
	}

	CRTSPclient* RtspClient = (CRTSPclient*)hSession;

	CHECK_FUNCTION(RtspClient, IPLY_SetVolume);
	BOOL bret = RtspClient->pfnIPLY_SetVolume((HANDLE)RtspClient->m_hPort, volume);
	if (!bret)
	{
		TCHAR bufString[300];
		RtspClient->pfnISYS_GetLastError(bufString, 300);

		wsprintf(g_szLastError, _T("IPLY_SetVolume failed,error infor= %s"), bufString);
		GSetError(g_szLastError);
		GLogError(g_szLastError);
		return -1;
	}

	return 0;
}

extern "C" int CLIENT_API CALL_TYPE  HIKS_OpenSound(HSESSION hSession, bool bExclusive)   //10.15
{
	if (hSession == -1)
	{
		wsprintf(g_szLastError, _T("hSession = -1 HIKS_OpenSound FAILED!"));
		GSetError(g_szLastError);
		return -1;
	}

	CRTSPclient* RtspClient = (CRTSPclient*)hSession;
	CHECK_FUNCTION(RtspClient, IPLY_StartSound);

	BOOL bret = RtspClient->pfnIPLY_StartSound((HANDLE)RtspClient->m_hPort);
	if (!bret)
	{
		TCHAR bufString[300];
		RtspClient->pfnISYS_GetLastError(bufString, 300);

		wsprintf(g_szLastError, _T("IPLY_StartSound failed,error infor= %s"), bufString);
		GSetError(g_szLastError);
		GLogError(g_szLastError);
		return -1;
	}

//	GLogError(_T("OpenSound Success"));
	return  1;
}

extern "C" int CLIENT_API CALL_TYPE  HIKS_CloseSound(HSESSION hSession)
{
	if (hSession == -1)
	{
		wsprintf(g_szLastError, _T("hSession = -1 HIKS_CloseSound FAILED!"));
		GSetLogError(g_szLastError);
		return -1;
	}

	CRTSPclient* RtspClient = (CRTSPclient*)hSession;

	CHECK_FUNCTION(RtspClient, IPLY_StopSound);
	BOOL bret = (RtspClient->pfnIPLY_StopSound((HANDLE)RtspClient->m_hPort));
	if (bret)	return 0;

	TCHAR bufString[300];
	RtspClient->pfnISYS_GetLastError(bufString, 300);

	wsprintf(g_szLastError, _T("IPLY_StopSound failed,error infor= %s"), bufString);
	GSetError(g_szLastError);
	GLogError(g_szLastError);
	return -1;
}

extern "C" int CLIENT_API CALL_TYPE  HIKS_ThrowBFrameNum(HSESSION hSession, unsigned int nNum)//丢B祯
{
	GSetLogError(_T("HIKS_ThrowBFrameNum 暂未实现"));
	return  -1;
}

extern "C" int CLIENT_API CALL_TYPE HIKS_PlayBackControl(HSESSION hSession,
	DWORD dwControlCode,
	DWORD dwInValue,
	DWORD *lpOutValue)//远程回放控制函数
{
	//	GLogError(_T("Enter HIKS_PlayBackControl---");
	if (hSession == -1)
	{
		wsprintf(g_szLastError, _T("hSession = -1 HIKS_PlayBackControl FAILED!"));
		GSetLogError(g_szLastError);
		return -1;
	}

	CRTSPclient* RtspClient = (CRTSPclient*)hSession;

	CHECK_FUNCTION(RtspClient, IPLY_Fast);
	CHECK_FUNCTION(RtspClient, IPLY_Slow);
	CHECK_FUNCTION(RtspClient, IPLY_Play);

	if (dwControlCode == NET_DVR_PLAYFAST)
	{
		if (RtspClient->hWnd != NULL)
		{
			BOOL bret = RtspClient->pfnIPLY_Fast((HANDLE)RtspClient->m_hPort);
			if (!bret)
			{
				TCHAR bufString[300];
				RtspClient->pfnISYS_GetLastError(bufString, 300);

				wsprintf(g_szLastError, _T("IPLY_Fast FAILED!,error infor= %s"), bufString);
				GSetLogError(g_szLastError);
				//	return -1;
			}
		}

		if (dwControlCode == NET_DVR_PLAYSLOW)
		{
			BOOL bret = RtspClient->pfnIPLY_Slow((HANDLE)RtspClient->m_hPort);
			if (!bret)
			{
				TCHAR bufString[300];
				RtspClient->pfnISYS_GetLastError(bufString, 300);

				wsprintf(g_szLastError, _T("IPLY_Slow failed,error infor= %s"), bufString);
				GSetLogError(g_szLastError);
				//	return -1;
			}
		}

		if (dwControlCode == NET_DVR_PLAYNORMAL)
		{
			BOOL bret = RtspClient->pfnIPLY_Play((HANDLE)RtspClient->m_hPort, RtspClient->hWnd);
			if (!bret)
			{
				TCHAR bufString[300];
				RtspClient->pfnISYS_GetLastError(bufString, 300);

				wsprintf(g_szLastError, _T("IPLY_Play failed,error infor= %s"), bufString);
				//	GSetLogError(g_szLastError);
				return -1;
			}
		}
	}

	RtspClient->sendOptions(dwControlCode, dwInValue, 0, 0, 0, "");
	if (RtspClient->RecvResponse(OPTIONS, 0) == 0)
	{
		if (lpOutValue != NULL)
		{
			if (RtspClient->m_PBRecvLength != 0)
			{
				*lpOutValue = atoi(RtspClient->m_PBRecv);
				delete[] RtspClient->m_PBRecv;
				RtspClient->m_PBRecv = NULL;
				wsprintf(g_szLastError, _T("server return play back = %d"), RtspClient->m_PBRecv);
				GLogError(g_szLastError);
			}
			else	return -1;
		}
		return 1;
	}
	return -1;
}

extern "C"  int CLIENT_API CALL_TYPE HIKS_QueryDevRecord(const char* strURL,
	int RecType,
	char* pTime,
	// 														 DWORD dwbufferLen,
	// 														 char* ppExParamOut,
	LPSTR* ppExParamOut,
	DWORD* pdwExParamOutLen
	)//获取远程文件列表
{
	CRTSPclient* RtspClient = new CRTSPclient();

	if (RtspClient == NULL)
	{
		GSetLogError(_T("HIKS_QueryDevRecord can't get memory"));
		return -1;
	}

	strcpy(RtspClient->strURL, strURL);
	if ((RtspClient->ParseInput(strURL)) == 0)
	{
		if (RtspClient->ServConnect() == 0)
		{
			GLogError(_T("ServConnect success"));
		}
		else
		{
			wsprintf(g_szLastError, _T("流媒体服务器连接失败！%s"), GGetError());
			GSetError(g_szLastError);
			return -1;
		}
	}
	else
	{
		wsprintf(g_szLastError, _T("URL字符串格式不对"));
		GSetLogError(g_szLastError);
	}

	RtspClient->sendGets(strURL, RecType, pTime);
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
				return -1;
			}

			strcpy(*ppExParamOut, RtspClient->m_PBRecv);

			delete[] RtspClient->m_PBRecv;
			RtspClient->m_PBRecv = NULL;
		}

		if (RtspClient->m_PBRecvLength == 0)
		{
			GSetError(_T("这个时段没有文件！"));
			return -1;
		}

		delete RtspClient;
		RtspClient = NULL;
		return 1;
	}
	else
	{
		wsprintf(g_szLastError, _T("没有您要查找的文件！%s"), GGetError());
		GSetError(g_szLastError);

		if (RtspClient->m_PBRecv != NULL && RtspClient->m_PBRecvLength != 0)
		{
			delete[] RtspClient->m_PBRecv;
			RtspClient->m_PBRecv = NULL;
		}

		delete RtspClient;
		RtspClient = NULL;
		return -1;
	}
}

//------------------------------------------------------------------------------------------------------------------------------------	
//2012-07-04 lyq														 
//电子放大begin
extern "C" BOOL CLIENT_API  CALL_TYPE HIKS_SetDisplayRegion(HSESSION hSession,
	DWORD nRegionNum,
	RECT *pSrcRect,
	HWND hDestWnd,
	BOOL bEnable)
{
	if (hSession == -1)
	{
		wsprintf(g_szLastError, _T("hSession = -1 HIKS_SetDisplayRegion FAILED!"));
		GSetLogError(g_szLastError);
		return -1;
	}

	CRTSPclient* RtspClient = (CRTSPclient*)hSession;

	if (RtspClient->pfnIPLY_SetDisplayRegion == NULL)
		return -1;

	if (RtspClient->pfnIPLY_SetDisplayRegion((HANDLE)RtspClient->m_hPort,
		nRegionNum,
		pSrcRect,
		hDestWnd,
		bEnable))
	{
		wsprintf(g_szLastError, _T("HIKS_SetDisplayRegion(%d,%d,%d)success"), hSession, nRegionNum, bEnable);
		GLogError(g_szLastError);
		return TRUE;
	}
	else
	{
		TCHAR bufString[300];
		RtspClient->pfnISYS_GetLastError(bufString, 300);

		wsprintf(g_szLastError, _T("HIKS_SetDisplayRegion(%d,%d,%d)failed,error info= %s"), hSession, nRegionNum, bEnable, bufString);
		GLogError(g_szLastError);
		GSetError(_T("底层插件错误"));
		return FALSE;
	}
}

extern "C" BOOL CLIENT_API  CALL_TYPE HIKS_RefreshPlayEx(HSESSION hSession,
	DWORD nRegionNum)
{
	if (hSession == -1)
	{
		wsprintf(g_szLastError, _T("hSession = -1 HIKS_RefreshPlayEx FAILED!"));
		GSetLogError(g_szLastError);
		return -1;
	}

	CRTSPclient* RtspClient = (CRTSPclient*)hSession;

	if (RtspClient->pfnIPLY_RefreshPlayEx == NULL)
		return -1;

	if (RtspClient->pfnIPLY_RefreshPlayEx((HANDLE)RtspClient->m_hPort, nRegionNum))
	{
		wsprintf(g_szLastError, _T("HIKS_RefreshPlayEx(%d,%d)success"), hSession, nRegionNum);
		GLogError(g_szLastError);
		return TRUE;
	}
	else
	{
		TCHAR bufString[300];
		RtspClient->pfnISYS_GetLastError(bufString, 300);

		wsprintf(g_szLastError, _T("HIKS_RefreshPlayEx(%d,%d)failed,error info= %s"), hSession, nRegionNum, bufString);
		GLogError(g_szLastError);
		GSetError(_T("底层插件错误"));
		return FALSE;
	}
}

//注册绘图回调函数
extern "C" BOOL CLIENT_API CALL_TYPE HIKS_RegisterDrawFun(HSESSION hSession,
	DrawCallBack callbackDraw,
	void* pUserData)
{
	if (hSession == -1)
	{
		wsprintf(g_szLastError, _T("hSession = -1 HIKS_RegisterDrawFun FAILED!"));
		GSetLogError(g_szLastError);
		return -1;
	}

	CRTSPclient* RtspClient = (CRTSPclient*)hSession;

	if (RtspClient->pfnIPLY_RegisterDrawFun == NULL)	return -1;

	if (RtspClient->pfnIPLY_RegisterDrawFun((HANDLE)(RtspClient->m_hPort), callbackDraw, pUserData))
	{
		wsprintf(g_szLastError, _T("HIKS_RegisterDrawFun(%d)success"), hSession);
		GLogError(g_szLastError);
		return TRUE;
	}
	else
	{
		TCHAR bufString[300];
		RtspClient->pfnISYS_GetLastError(bufString, 300);

		wsprintf(g_szLastError, _T("HIKS_RegisterDrawFun(%d)failed,error info= %s"), hSession, bufString);
		GLogError(g_szLastError);
		GSetError(_T("底层插件错误"));
		return FALSE;
	}
}

//电子放大end
//2012-07-04 lyq
//------------------------------------------------------------------------------------------------------------------------------------	

