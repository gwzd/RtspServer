#include "stdafx.h"
#include "PlugIns.h"
#include "Global.h"
#include "CommonFunc.h"

//////////////////////////////////////////////////////////////////////////
//实时音视频流的回调函数


//消息回调处理函数
CBF_AlarmInfo g_CBF_AlarmInfo = NULL;

static BYTE pbHead[] = { 0x34, 0x48, 0x4b, 0x48, 0xfe, 0xb3, 0xd0, 0xd6, 0x08, 0x03, 0x04, 0x20, 0x00, 0x00, 0x00, 0x00,
0x01, 0x10, 0x01, 0x10, 0x01, 0x10, 0x10, 0x00, 0x80, 0x3e, 0x00, 0x00, 0x60, 0x01, 0x20, 0x01,
0x11, 0x10, 0x00, 0x00, 0x0e, 0x00, 0x00, 0x00 };

void CALLBACK PlayCallBack(LONG lPlayHandle, DWORD dwTotalSize, DWORD dwDownLoadSize, DWORD dwUser)
{
	char* cStr = (char*)dwUser;
	if (-1 != dwDownLoadSize)
	{
		sprintf(cStr, "%d_%d", dwDownLoadSize, dwTotalSize);
	}

}

BOOL CALLBACK MessCallBack(LONG lCommand, LONG lLoginID, char *pBuf, DWORD dwBufLen, char *pchDVRIP, LONG nDVRPort, DWORD dwUser)
{
	TRACE("------Alarm: type=%d, IP = %s\n", lCommand, pchDVRIP);
	CRvu* pRvu = g_rvuMgr.GetRvuByHandle(lLoginID);
	int i = 0;
	BOOL bRet = FALSE;
	BYTE m_dwAlarm[DFJK_MAX_ALARMIN];
	BYTE m_dwDisk[32];
	DWORD dwVal = 0;
	switch (lCommand)
	{
	case DH_ALARM_ALARM_EX:
		bRet = TRUE;
		memcpy(m_dwAlarm, pBuf, dwBufLen);
		for (i = 0; i<DFJK_MAX_ALARMIN; i++)
		{
			if (m_dwAlarm[i] != pRvu->bAlarmIn[i])
			{
				if (pRvu->bAlarmIn[i] == 1)
				{
					TRACE("------报警消失 (外部报警): IP = %s, Chn=%d\n", pchDVRIP, i + 1);
					dwVal = 0;
				}
				else
				{
					TRACE("------Alarm (外部报警): IP = %s, Chn=%d\n", pchDVRIP, i + 1);
					dwVal = 1;
				}
				if (g_CBF_AlarmInfo)
				{
					g_CBF_AlarmInfo((HANDLE)pRvu, pRvu->dwAlarmUserData, ALARM_IO, i + 1, &dwVal);
				}
			}
		}
		memcpy(pRvu->bAlarmIn, pBuf, dwBufLen);
		break;
	case DH_MOTION_ALARM_EX:
		bRet = TRUE;
		memcpy(m_dwAlarm, pBuf, dwBufLen);
		for (i = 0; i<DFJK_MAX_ALARMIN; i++)
		{
			if (m_dwAlarm[i] != pRvu->bAlarmVideoMotion[i])
			{
				if (pRvu->bAlarmVideoMotion[i] == 1)
				{
					TRACE("------报警消失 (移动侦测): IP = %s, Chn=%d\n", pchDVRIP, i + 1);
					dwVal = 0;
				}
				else
				{
					TRACE("------Alarm (移动侦测): IP = %s, Chn=%d\n", pchDVRIP, i + 1);
					dwVal = 1;
				}
				if (g_CBF_AlarmInfo)
				{
					g_CBF_AlarmInfo((HANDLE)pRvu, pRvu->dwAlarmUserData, ALARM_VIDEOMOTION, i + 1, &dwVal);
				}
			}
		}
		memcpy(pRvu->bAlarmVideoMotion, pBuf, dwBufLen);
		break;
	case DH_VIDEOLOST_ALARM_EX:
		bRet = TRUE;
		memcpy(m_dwAlarm, pBuf, dwBufLen);
		for (i = 0; i<DFJK_MAX_ALARMIN; i++)
		{
			if (m_dwAlarm[i] != pRvu->bAlarmVideoLost[i])
			{
				if (pRvu->bAlarmVideoLost[i] == 1)
				{
					TRACE("------报警消失 (视频丢失): IP = %s, Chn=%d\n", pchDVRIP, i + 1);
					dwVal = 0;
				}
				else
				{
					TRACE("------Alarm (视频丢失): IP = %s, Chn=%d\n", pchDVRIP, i + 1);
					dwVal = 1;
				}
				if (g_CBF_AlarmInfo)
				{
					g_CBF_AlarmInfo((HANDLE)pRvu, pRvu->dwAlarmUserData, ALARM_VIDEOLOST, i + 1, &dwVal);
				}
			}
		}
		memcpy(pRvu->bAlarmVideoLost, pBuf, dwBufLen);
		break;
	case DH_SHELTER_ALARM_EX:
		bRet = TRUE;
		memcpy(m_dwAlarm, pBuf, dwBufLen);
		for (i = 0; i<DFJK_MAX_ALARMIN; i++)
		{
			if (m_dwAlarm[i] != pRvu->bAlarmVideoShelter[i])
			{
				if (pRvu->bAlarmVideoShelter[i] == 1)
				{
					TRACE("------报警消失 (视频遮挡): IP = %s, Chn=%d\n", pchDVRIP, i + 1);
					dwVal = 0;
				}
				else
				{
					TRACE("------Alarm (视频遮挡): IP = %s, Chn=%d\n", pchDVRIP, i + 1);
					dwVal = 1;
				}
				if (g_CBF_AlarmInfo)
				{
					g_CBF_AlarmInfo((HANDLE)pRvu, pRvu->dwAlarmUserData, ALARM_VIDEOSHELTER, i + 1, &dwVal);
				}
			}
		}
		memcpy(pRvu->bAlarmVideoShelter, pBuf, dwBufLen);
		break;
		//	case DH_SOUND_DETECT_ALARM_EX:
		//		bRet = TRUE;
		//		memcpy(m_dwAlarm,pBuf,dwBufLen);
		//		for(i = 0;i<16;i++)
		//		{
		//			if(1 == m_dwAlarm[i])
		//			{
		//				if(g_CBF_AlarmInfo)
		//				{
		//					g_CBF_AlarmInfo((HANDLE)pRvu, pRvu->dwAlarmUserData, ALARM_IO, i+1, NULL);
		//				}
		//			}
		//		}
	case DH_DISKFULL_ALARM_EX:
		bRet = TRUE;
		if (1 == *(DWORD*)pBuf)
		{
			g_CBF_AlarmInfo((HANDLE)pRvu, pRvu->dwAlarmUserData, ALARM_DISKFULL, 1, NULL);
		}
		break;
	case DH_DISKERROR_ALARM_EX:
		bRet = TRUE;
		memcpy(m_dwDisk, pBuf, dwBufLen);
		for (i = 0; i<32; i++)
		{
			if (1 == m_dwDisk[i])
			{
				g_CBF_AlarmInfo((HANDLE)pRvu, pRvu->dwAlarmUserData, ALARM_DISKERROR, i + 1, NULL);
			}
		}
		break;
	default:
		TRACE("------Unkown Alarm: type=%d\n", lCommand);
	}

	return bRet;
}

void CALLBACK DisConnectFunc(LONG lLoginID, char *pchDVRIP, LONG nDVRPort, DWORD dwUser)
{
	//	CString str;
	//	str.Format("%s","网络连接中断!");
	//	AfxMessageBox(str);
}

//////////////////////////////////////////////////////////////////////////
//1、	系统类函数	5
extern "C" BOOL CALL_TYPE ISYS_GetLastError(LPTSTR lpszErrorMsg, UINT nMsgLen)
{
	if ((nMsgLen <0) || (nMsgLen>DFJK_MAXLEN_LASTERROR))
		return FALSE;
	else
	{
		if ((g_szLastError) && (sizeof(g_szLastError)) <= nMsgLen)
		{
			wsprintf(lpszErrorMsg, g_szLastError);
			lpszErrorMsg[nMsgLen - 1] = 0;
			return TRUE;
		}
		else
			return FALSE;
	}
}

extern "C" BOOL CALL_TYPE ISYS_GetInvalidHandleValue(HANDLE *phHandle)
{
	*phHandle = (HANDLE)NULL;
	return TRUE;
}

extern "C" BOOL CALL_TYPE ISYS_GetPluginInfo(PPLUGIN_INFO pInfo)
{
	if (pInfo == NULL)
	{
		wsprintf(g_szLastError, _T("ISYS_GetPluginInfo: 参数非法"));
		return FALSE;
	}
	strcpy(pInfo->DeviceTypeName, "DFDH");
	strcpy(pInfo->smanufactuer, "DFE");
	strcpy(pInfo->ssoftversion, "1.0.0.1");
	strcpy(pInfo->description, "DH DVR");
	return TRUE;
}

extern "C" BOOL CALL_TYPE ISYS_Initialize(void *pReserved, DWORD dwReserved)
{
	CLIENT_Init(DisConnectFunc, 100);
	return TRUE;
}

extern "C" void CALL_TYPE ISYS_Uninitialize()
{
	CLIENT_Cleanup();
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

//////////////////////////////////////////////////////////////////////////
//2、	设备类函数	7
extern "C" BOOL CALL_TYPE IDVR_ConnectDevice(LPCSTR  lpszRvuIP, DWORD dwPort, LPCSTR lpszUserName, LPCSTR lpszPassword, TRANSTYPE tType, HANDLE *phDevHandle, LPCSTR pExParamIn, DWORD dwExParamInLen)
{
	*phDevHandle = NULL;

	if (lpszRvuIP == NULL || dwPort == 0 || lpszUserName == NULL || lpszPassword == NULL)
	{
		wsprintf(g_szLastError, _T("IDVR_ConnectDevice: 参数非法"));
		return FALSE;
	}

	if (strlen(lpszRvuIP) < 7 || strlen(lpszUserName) == 0)
	{
		wsprintf(g_szLastError, _T("IDVR_ConnectDevice: 参数非法"));
		return FALSE;
	}

	if (tType > TT_MULTICAST && pExParamIn == NULL)
	{
		wsprintf(g_szLastError, _T("IDVR_ConnectDevice: 扩展参数非法"));
		return FALSE;
	}

	//判断是否重复登录?
	//....

	//开始连接Rvu
	NET_DEVICEINFO deviceInfo;
	CString sTemp;
	int nError = 0;
	long lRvuHandle = CLIENT_Login((char *)lpszRvuIP, (WORD)dwPort, (char *)lpszUserName, (char *)lpszPassword, &deviceInfo, &nError);
	if (lRvuHandle == DFJK_INVALID_HANDLE)
	{
		if (1 == nError)
			wsprintf(g_szLastError, _T("密码不正确!"));
		else if (2 == nError)
			wsprintf(g_szLastError, _T("帐号不存在!"));
		else if (3 == nError)
			wsprintf(g_szLastError, _T("等待登录返回超时!"));
		else if (4 == nError)
			wsprintf(g_szLastError, _T("帐号已登录!"));
		else if (5 == nError)
			wsprintf(g_szLastError, _T("帐号已被锁定!"));
		else if (6 == nError)
			wsprintf(g_szLastError, _T("帐号已被列为黑名单!"));
		else if (7 == nError)
			wsprintf(g_szLastError, _T("资源不足，系统忙!"));
		else if (9 == nError)
			wsprintf(g_szLastError, _T("找不到网络主机!"));
		else
			wsprintf(g_szLastError, _T("登入失败!"));
		return FALSE;
	}
	TRACE("====Login Ruv(IP:%s) 成功, lRvuHandle=%d\n", lpszRvuIP, lRvuHandle);



	CRvu* pRvu = new CRvu();
	if (pRvu == NULL)
	{
		return FALSE;
	}

	USES_CONVERSION;

	strncpy(pRvu->szRvuIP, lpszRvuIP, MAXLEN_IP);
	strncpy(pRvu->szRvuUserName, lpszUserName, MAXLEN_STR);
	strncpy(pRvu->szRvuPassword, lpszPassword, MAXLEN_STR);

	pRvu->iConnectType = tType;
	pRvu->wServerType = deviceInfo.byDVRType;
	pRvu->iChannelNumber = deviceInfo.byChanNum;
	pRvu->dwAlarmInNum = deviceInfo.byAlarmInPortNum;
	pRvu->dwAlarmOutNum = deviceInfo.byAlarmOutPortNum;
	pRvu->dwDiskNum = deviceInfo.byDiskNum;

	pRvu->lRvuHandle = lRvuHandle;

	//设置消息回调函数
	CLIENT_SetDVRMessCallBack(MessCallBack, (DWORD)pRvu->dwAlarmUserData);
	g_rvuMgr.AddRvu(pRvu);

	*phDevHandle = pRvu;

	return TRUE;
}

extern "C" BOOL CALL_TYPE IDVR_DisConnectDevice(HANDLE hDevHandle)
{
	CRvu* pRvu = (CRvu*)hDevHandle;
	if (pRvu == NULL)
	{
		wsprintf(g_szLastError, _T("IDVR_DisConnectDevice error: Invalid Handle"));
		return FALSE;
	}
	//断开所有视频连接
	short i;
	for (i = 0; i<pRvu->m_arrayChan.GetSize(); i++)
	{
		CChannel* pChan = (CChannel*)pRvu->m_arrayChan.GetAt(i);
		IDVR_StopPreview((HANDLE)pChan);
	}
	//关闭其它资源（如透明串口等）


	//断开设备
	BOOL bRet = FALSE;
	if (pRvu->lRvuHandle != DFJK_INVALID_HANDLE)
	{
		bRet = CLIENT_Logout(pRvu->lRvuHandle);
		if (!bRet)
		{
			wsprintf(g_szLastError, _T("Logout(%s)失败! code=%d"), pRvu->szRvuIP, CLIENT_GetLastError());
		}
		else
		{
			TRACE("====Logout Rvu(IP:%s)成功!\n", pRvu->szRvuIP);
		}
	}

	//删除类实例
	g_rvuMgr.RemoveRvu(pRvu);

	return bRet;
}

extern "C" BOOL CALL_TYPE IDVR_StartPreview(HANDLE hDevHandle, HWND hWnd, int iChannel, int iStreamType, HANDLE *phChanHandle, LPCSTR pExParamIn, DWORD dwExParamInLen)
{
	*phChanHandle = NULL;

	CRvu* pRvu = (CRvu*)hDevHandle;
	if (pRvu == NULL)
	{
		wsprintf(g_szLastError, _T("IDVR_StartPreview error: Invalid Dev Handle"));
		return FALSE;
	}

	if (iChannel <= 0 || iChannel > pRvu->iChannelNumber)
	{
		wsprintf(g_szLastError, _T("IDVR_StartPreview error: Invalid Params!"));
		return FALSE;
	}

	DH_RealPlayType rType = DH_RType_Realplay_0;
	if (iStreamType == 1)
	{
		rType = DH_RType_Realplay_1;
	}

	long lChanHandle = CLIENT_RealPlayEx(pRvu->lRvuHandle, iChannel - 1, hWnd, rType);
	if (lChanHandle == DFJK_INVALID_HANDLE)
	{
		wsprintf(g_szLastError, _T("预览RVU(IP:%s)通道%d失败"), pRvu->szRvuIP, iChannel);
		return FALSE;
	}

	//增加通道实例
	CChannel* pChan = new CChannel();
	if (pChan == NULL)
	{
		return FALSE;
	}
	pChan->nChanNo = iChannel;
	pChan->lChanHandle = lChanHandle;
	g_rvuMgr.AddChan(pRvu, pChan);

	*phChanHandle = pChan;

	return TRUE;
}

extern "C" BOOL CALL_TYPE IDVR_StopPreview(HANDLE hChanHandle)
{
	CChannel* pChan = (CChannel*)hChanHandle;
	if (pChan == NULL)
	{
		wsprintf(g_szLastError, _T("IDVR_StopPreview error: Invalid Handle!"));
		return FALSE;
	}

	//停止录像
	if (pChan->bIsRecording)
	{
		IDVR_StopRecord(hChanHandle);
	}

	//停止预览
	BOOL bRet = FALSE;
	if (pChan->lChanHandle != DFJK_INVALID_HANDLE)
	{
		bRet = CLIENT_StopRealPlayEx(pChan->lChanHandle);
		if (!bRet)
		{
			wsprintf(g_szLastError, _T("停止预览RVU 通道%d失败"), pChan->nChanNo);
		}
	}

	//删除通道类实例
	g_rvuMgr.RemoveChan(pChan);

	return TRUE;
}

void CALLBACK RealDataCallBackEx(LONG lRealHandle, DWORD dwDataType, BYTE *pBuffer,
	DWORD dwBufSize, LONG lParam, DWORD dwUser)
{
	if (dwUser != NULL)
	{
		CChannel* pChannel = (CChannel*)dwUser;
		if (pChannel->pfnRealDataCallBack != NULL)
			pChannel->pfnRealDataCallBack((HANDLE)pChannel, pBuffer, dwBufSize,
			pChannel->dwCookie, dwDataType);
	}
}


extern "C" BOOL CALL_TYPE IDVR_StartRealData(HANDLE hDevHandle, int iChannel, int iStreamType, HANDLE *phChanHandle,
	DWORD dwCookie, LPCSTR pExParamIn, DWORD dwExParamInLen, RealDataCallBack callbackHandler)
{
	*phChanHandle = NULL;

	CRvu* pRvu = (CRvu*)hDevHandle;
	if (pRvu == NULL)
	{
		wsprintf(g_szLastError, _T("IDVR_StartRealData error: Invalid Dev Handle"));
		return FALSE;
	}

	if (iChannel <= 0 || iChannel > pRvu->iChannelNumber)
	{
		wsprintf(g_szLastError, _T("IDVR_StartRealData error: 通道号错误(1～%d)"), pRvu->iChannelNumber);
		return FALSE;
	}
	DH_RealPlayType rType = DH_RType_Realplay_0;
	if (iStreamType == 1)
	{
		rType = DH_RType_Realplay_1;
	}
	LONG lChanHandle = CLIENT_RealPlayEx(pRvu->lRvuHandle, iChannel - 1, 0, rType);
	if (lChanHandle == DFJK_INVALID_HANDLE)
	{
		wsprintf(g_szLastError, _T("预览RVU(IP:%s)通道%d失败"), pRvu->szRvuIP, iChannel);
		return FALSE;
	}
	CChannel* pChan = new CChannel();
	pChan->nChanNo = iChannel;
	pChan->lChanHandle = lChanHandle;
	pChan->dwCookie = dwCookie;
	pChan->pfnRealDataCallBack = callbackHandler;
	g_rvuMgr.AddChan(pRvu, pChan);

	CLIENT_SetRealDataCallBackEx(lChanHandle, RealDataCallBackEx, (DWORD)pChan, 0x1f);
	*phChanHandle = pChan;
	return TRUE;
}

extern "C" BOOL CALL_TYPE IDVR_GetRealDataHeader(HANDLE hChanHandle, unsigned char *pBuffer, DWORD dwBuffLen, DWORD *pdwHeaderLen)
{

	return TRUE;
}

extern "C" BOOL CALL_TYPE IDVR_StopRealData(HANDLE hChanHandle)
{
	CChannel* pChan = (CChannel*)hChanHandle;
	if (pChan == NULL)
	{
		wsprintf(g_szLastError, _T("IDVR_StopRealData error: Invalid Handle!"));
		return FALSE;
	}
	BOOL bRet = FALSE;
	if (pChan->lChanHandle != DFJK_INVALID_HANDLE)
	{
		//停止捕获数据
		CLIENT_SetRealDataCallBack(pChan->lChanHandle, NULL, 0);

		//停止预览
		bRet = CLIENT_StopRealPlay(pChan->lChanHandle);
		if (!bRet)
		{
			wsprintf(g_szLastError, _T("IDVR_StopRealData:停止预览RVU 通道%d失败"), pChan->nChanNo);
		}
	}

	//删除通道类实例
	g_rvuMgr.RemoveChan(pChan);

	return bRet;
}

extern "C" BOOL CALL_TYPE IDVR_SwitchCamera(HANDLE hDevHandle,
	LPCSTR lpszProtocol, int nAddress,
	int nCamNo, int nMonNo, SWITCHTYPE switchType,
	COMTYPE comType, DWORD dwParam,
	LPCSTR pExParamIn, DWORD dwExParamInLen)
{
	wsprintf(g_szLastError, _T("IDVR_SwitchCamera error: 尚未实现!"));
	return FALSE;
}

extern "C" BOOL CALL_TYPE IDVR_CapturePicture(HANDLE hChanHandle, LPCSTR lpszFileName, LPCSTR pExParamIn, DWORD dwExParamInLen)
{
	CChannel* pChan = (CChannel*)hChanHandle;
	if (pChan == NULL)
	{
		wsprintf(g_szLastError, _T("IDVR_CapturePicture error: Invalid Handle!"));
		return FALSE;
	}
	if (lpszFileName == NULL)
	{
		wsprintf(g_szLastError, _T("IDVR_CapturePicture error: file name is NULL"));
		return FALSE;
	}

	if (strlen(lpszFileName) <= 0)
	{
		wsprintf(g_szLastError, _T("IDVR_CapturePicture error: file name is empty"));
		return FALSE;
	}

	if (pChan->lChanHandle == DFJK_INVALID_HANDLE)
	{
		wsprintf(g_szLastError, _T("IDVR_CapturePicture error: Invalid Chan handle\n"));
		return FALSE;
	}

	if (strlen(lpszFileName) > 100)
	{
		wsprintf(g_szLastError, _T("IDVR_CapturePicture error: 图片文件名长度不能超过100字节"));
		return FALSE;
	}

	BOOL bRet = FALSE;
	bRet = CLIENT_CapturePicture(pChan->lChanHandle, (char*)lpszFileName);

	if (!bRet)
		wsprintf(g_szLastError, _T("IDVR_CapturePicture failed! Error code = %d"), CLIENT_GetLastError());

	return bRet;
}

extern "C" BOOL CALL_TYPE IDVR_StartRecord(HANDLE hChanHandle, LPCSTR lpszFileName, LPCSTR pExParamIn, DWORD dwExParamInLen)
{
	CChannel* pChan = (CChannel*)hChanHandle;
	if (pChan == NULL)
	{
		wsprintf(g_szLastError, _T("IDVR_StartRecord error: Invalid Handle!"));
		return FALSE;
	}
	if (lpszFileName == NULL)
	{
		wsprintf(g_szLastError, _T("IDVR_StartRecord error: file name is NULL"));
		return FALSE;
	}

	if (strlen(lpszFileName) <= 0)
	{
		wsprintf(g_szLastError, _T("IDVR_StartRecord error: file name is empty"));
		return FALSE;
	}

	if (pChan->lChanHandle == DFJK_INVALID_HANDLE)
	{
		wsprintf(g_szLastError, _T("IDVR_StartRecord error: Invalid Chan handle\n"));
		pChan->bIsRecording = FALSE;
		return FALSE;
	}

	//开始录像
	BOOL bRet = FALSE;
	bRet = CLIENT_SaveRealData(pChan->lChanHandle, (char*)lpszFileName);
	if (!bRet)
		wsprintf(g_szLastError, _T("IDVR_StartRecord failed! Error code = %d"), CLIENT_GetLastError());

	//更新标志
	pChan->bIsRecording = bRet;

	return bRet;
}

extern "C" BOOL CALL_TYPE IDVR_StopRecord(HANDLE hChanHandle)
{
	CChannel* pChan = (CChannel*)hChanHandle;
	if (pChan == NULL)
	{
		wsprintf(g_szLastError, _T("IDVR_StopRecord error: Invalid Handle!"));
		return FALSE;
	}

	if (pChan->lChanHandle == DFJK_INVALID_HANDLE)
	{
		wsprintf(g_szLastError, _T("IDVR_StopRecord error: Invalid Chan handle\n"));
		pChan->bIsRecording = FALSE;
		return FALSE;
	}

	//停止录像
	BOOL bRet = FALSE;
	bRet = CLIENT_StopSaveRealData(pChan->lChanHandle);
	if (!bRet)
		wsprintf(g_szLastError, _T("IDVR_StopRecord failed!: Error code = %d"), CLIENT_GetLastError());

	//更新标志
	pChan->bIsRecording = FALSE;

	return bRet;
}

extern "C" BOOL CALL_TYPE IDVR_GetRecordState(HANDLE hChanHandle, int *pnState)
{
	CChannel* pChan = (CChannel*)hChanHandle;
	if (pChan == NULL)
	{
		wsprintf(g_szLastError, _T("IDVR_GetRecordState error: Invalid Handle!"));
		return FALSE;
	}
	*pnState = pChan->bIsRecording;
	return TRUE;
}

extern "C" BOOL CALL_TYPE IDVR_SetVideoParams(HANDLE hChanHandle, int iBrightness, int iContrast, int iSaturation, int iHue, BOOL bDefault)
{
	CChannel* pChan = (CChannel*)hChanHandle;
	if (pChan == NULL)
	{
		wsprintf(g_szLastError, _T("IDVR_SetVideoParams error: Invalid Handle!"));
		return FALSE;
	}

	if (pChan->lChanHandle == DFJK_INVALID_HANDLE)
	{
		wsprintf(g_szLastError, _T("IDVR_SetVideoParams error: Invalid Chan handle\n"));
		pChan->bIsRecording = FALSE;
		return FALSE;
	}
	unsigned char dwB, dwC, dwS, dwH;
	if (bDefault)	//默认值
	{
		dwB = 127;
		dwC = 127;
		dwS = 127;
		dwH = 127;
	}
	else
	{
		dwB = (unsigned char)(iBrightness);
		dwC = (unsigned char)(iContrast);
		dwS = (unsigned char)(iSaturation);
		dwH = (unsigned char)(iHue);
	}
	BOOL bRet = CLIENT_ClientSetVideoEffect(pChan->lChanHandle, dwB, dwC, dwS, dwH);
	if (!bRet)
		wsprintf(g_szLastError, _T("IDVR_SetVideoParams: Error code = %d"), CLIENT_GetLastError());
	return bRet;
}

extern "C" BOOL CALL_TYPE IDVR_GetVideoParams(HANDLE hChanHandle, int *piBrightness, int *piContrast, int *piSaturation, int *piHue)
{
	CChannel* pChan = (CChannel*)hChanHandle;
	if (pChan == NULL)
	{
		wsprintf(g_szLastError, _T("IDVR_GetVideoParams error: Invalid Handle!"));
		return FALSE;
	}

	if (pChan->lChanHandle == DFJK_INVALID_HANDLE)
	{
		wsprintf(g_szLastError, _T("IDVR_GetVideoParams error: Invalid Chan handle\n"));
		pChan->bIsRecording = FALSE;
		return FALSE;
	}
	BOOL bRet = FALSE;
	BYTE dwB, dwC, dwS, dwH;
	bRet = CLIENT_ClientGetVideoEffect((LONG)pChan->lChanHandle, &dwB, &dwC, &dwS, &dwH);
	if (bRet)
	{

		*piBrightness = (int)(dwB);
		*piContrast = (int)(dwC);
		*piSaturation = (int)(dwS);
		*piHue = (int)(dwH);
	}
	else
		wsprintf(g_szLastError, _T("IDVR_GetVideoParams: Error code = %d"), CLIENT_GetLastError());
	return bRet;
}

extern "C" BOOL CALL_TYPE IDVR_StartSound(HANDLE hChanHandle, LPCSTR pExParamIn, DWORD dwExParamInLen)
{
	CChannel* pChan = (CChannel*)hChanHandle;
	if (pChan == NULL)
	{
		wsprintf(g_szLastError, _T("IDVR_StartSound error: Invalid Handle!"));
		return FALSE;
	}
	BOOL bRet = CLIENT_OpenSound(pChan->lChanHandle);
	if (!bRet)
		wsprintf(g_szLastError, _T("IDVR_StartSound: Error code = %d"), CLIENT_GetLastError());
	return bRet;
}

extern "C" BOOL CALL_TYPE IDVR_StopSound(HANDLE hChanHandle)
{
	CChannel* pChan = (CChannel*)hChanHandle;
	if (pChan == NULL)
	{
		wsprintf(g_szLastError, _T("IDVR_StopSound error: Invalid Handle!"));
		return FALSE;
	}

	BOOL bRet = FALSE;
	bRet = CLIENT_CloseSound();
	if (!bRet)
		wsprintf(g_szLastError, _T("IDVR_StopSound failed: Error code = %d"), CLIENT_GetLastError());

	return bRet;
}

extern "C" BOOL CALL_TYPE IDVR_SetVolume(HANDLE hChanHandle, LONG lVolume)
{

	CChannel* pChan = (CChannel*)hChanHandle;
	if (pChan == NULL)
	{
		wsprintf(g_szLastError, _T("IDVR_SetVolume error: Invalid Handle!"));
		return FALSE;
	}
	if (lVolume < 0)
		lVolume = 0;
	if (lVolume > 100)
		lVolume = 100;
	WORD wVol = (WORD)(lVolume * 0xFFFF / 100);

	BOOL bRet = CLIENT_SetVolume(pChan->lChanHandle, wVol);
	if (!bRet)
		wsprintf(g_szLastError, _T("IDVR_SetVolume: Error code = %d"), CLIENT_GetLastError());
	return bRet;
}
void CALLBACK AudioDataCallBack(LONG lTalkHandle, char *pDataBuf, DWORD dwBufSize, BYTE byAudioFlag, DWORD dwUser)
{
	if (0 == byAudioFlag)
	{
		//It means it is the audio data from the local reocrd library
		long lSendLen = CLIENT_TalkSendData(lTalkHandle, pDataBuf, dwBufSize);
		if (lSendLen != (long)dwBufSize)
		{
			//Error occurred when sending the user audio data to the device
		}
	}
	else if (1 == byAudioFlag)
	{
		//It means it has received the audio data from the device.
		CLIENT_AudioDec(pDataBuf, dwBufSize);
	}
}

extern "C" BOOL CALL_TYPE IDVR_StartAudioPhone(HANDLE hDevHandle, LPCSTR pExParamIn, DWORD dwExParamInLen)
{
	CRvu* pRvu = (CRvu*)hDevHandle;
	if (pRvu == NULL)
	{
		wsprintf(g_szLastError, _T("IDVR_StartAudioPhone error: Invalid Dev Handle"));
		return FALSE;
	}

	//设置音频编码
	// 	DHDEV_TALKFORMAT_LIST lstTalkEncode;
	// 
	// 	DHDEV_TALKDECODE_INFO curTalkMode;
	// 	curTalkMode.encodeType = DH_TALK_DEFAULT;	// DH_TALK_DEFAULT is default way. Usually it is pcm type without head.
	// 	curTalkMode.dwSampleRate = 8000;
	// 	curTalkMode.nAudioBit = 8;
	// 
	// 
	// 	int retlen = 0;
	// 	BOOL bSuccess = CLIENT_QueryDevState(pRvu->lRvuHandle, DH_DEVSTATE_TALK_ECTYPE, (char*)&lstTalkEncode, sizeof(DHDEV_TALKFORMAT_LIST), &retlen, 2000);
	// 
	// 	if (!bSuccess)
	// 	{
	// 		TRACE("CLIENT_QueryDevState failed\n");
	// 	return FALSE;
	// 	}
	// 
	// 	curTalkMode = lstTalkEncode.type[0];
	// 
	// 	TRACE("Encode Type = %d \n",curTalkMode.encodeType);
	// 	 bSuccess = CLIENT_SetDeviceMode(pRvu->lRvuHandle, DH_TALK_ENCODE_TYPE, &curTalkMode);


	BOOL bSuccess = CLIENT_SetDeviceMode(pRvu->lRvuHandle, DH_TALK_CLIENT_MODE, NULL);

	if (!bSuccess)
	{
		wsprintf(g_szLastError, _T("语音对讲失败, Error code=%d"), CLIENT_GetLastError());
		return FALSE;
	}
	LONG lRet = CLIENT_StartTalkEx(pRvu->lRvuHandle, AudioDataCallBack, NULL);
	if (lRet == DFJK_INVALID_HANDLE)
	{
		wsprintf(g_szLastError, _T("语音对讲失败, Error code=%d"), CLIENT_GetLastError());
		return FALSE;
	}
	else
	{
		//启动本地声音
		bSuccess = CLIENT_RecordStart();
		if (!bSuccess)
		{
			CLIENT_StopTalkEx(lRet);
			wsprintf(g_szLastError, _T("语音对讲CLIENT_RecordStart失败, Error code=%d"), CLIENT_GetLastError());
			return FALSE;
		}
		//保存句柄
		pRvu->lVoiceComHandle = lRet;
		//	CLIENT_SetAudioClientVolume(lRet, 100);
	}
	return TRUE;
}

extern "C" BOOL CALL_TYPE IDVR_StopAudioPhone(HANDLE hDevHandle)
{
	CRvu* pRvu = (CRvu*)hDevHandle;
	if (pRvu == NULL)
	{
		wsprintf(g_szLastError, _T("IDVR_StopAudioPhone error: Invalid Dev Handle"));
		return FALSE;
	}
	BOOL bRet = FALSE;
	if (pRvu->lVoiceComHandle != DFJK_INVALID_HANDLE)
	{
		CLIENT_RecordStop();
		bRet = CLIENT_StopTalkEx(pRvu->lVoiceComHandle);
		if (!bRet)
		{
			wsprintf(g_szLastError, _T("停止语音对讲失败, Error code=%d"), CLIENT_GetLastError());
		}
	}
	//复位句柄
	pRvu->lVoiceComHandle = DFJK_INVALID_HANDLE;

	return bRet;
}

extern "C" BOOL CALL_TYPE IDVR_PTZControl(HANDLE hDevHandle, int iChannel, CTRLTYPE command, LPCSTR lpszProtocol, DWORD dwParam1, DWORD dwParam2, LPCSTR pExParamIn, DWORD dwExParamInLen)
{
	CRvu* pRvu = (CRvu*)hDevHandle;
	if (pRvu == NULL)
	{
		wsprintf(g_szLastError, _T("IDVR_PTZControl error: Invalid Handle"));
		return FALSE;
	}

	CString strProtocol = CString(lpszProtocol);
	BOOL bRet = FALSE;
	if (strProtocol == _T("EDVR") || strProtocol == _T(""))
	{
		if (iChannel <= 0 || iChannel > pRvu->iChannelNumber)
		{
			wsprintf(g_szLastError, _T("IDVR_PTZControl error: Invalid chan No(1～%d)"), pRvu->iChannelNumber);
			return FALSE;
		}
		if (dwParam1 <1)	dwParam1 = 1;
		if (dwParam1 > 8)	dwParam1 = 8;

		static DWORD olddwPTZCmd = 0;
		DWORD dwPTZCmd = 0;
		BOOL dwStop = 0;

		DWORD param1 = 0;
		DWORD param2 = 0;
		BOOL bPreset = FALSE;
		switch (command)
		{
		case CT_STOP:
			dwPTZCmd = olddwPTZCmd;
			dwStop = TRUE;
			break;
		case CT_TILT_UP:
			dwPTZCmd = DH_PTZ_UP_CONTROL;
			param1 = 0;
			param2 = dwParam1;
			break;
		case CT_TILT_DOWN:
			dwPTZCmd = DH_PTZ_DOWN_CONTROL;
			param1 = 0;
			param2 = dwParam1;
			break;
		case CT_PAN_LEFT:
			dwPTZCmd = DH_PTZ_LEFT_CONTROL;
			param1 = 0;
			param2 = dwParam1;
			break;
		case CT_PAN_RIGHT:
			dwPTZCmd = DH_PTZ_RIGHT_CONTROL;
			param1 = 0;
			param2 = dwParam1;
			break;
//	    case CT_PAN_AUTO:
//		    dwPTZCmd = ;
//		    break;
		case CT_ZOOM_IN:
			dwPTZCmd = DH_PTZ_ZOOM_ADD_CONTROL;
			param1 = 0;
			param2 = dwParam1;
			break;
		case CT_ZOOM_OUT:
			dwPTZCmd = DH_PTZ_ZOOM_DEC_CONTROL;
			param1 = 0;
			param2 = dwParam1;
			break;
		case CT_FOCUS_NEAR:
			dwPTZCmd = DH_PTZ_FOCUS_ADD_CONTROL;
			param1 = 0;
			param2 = dwParam1;
			break;
		case CT_FOCUS_FAR:
			dwPTZCmd = DH_PTZ_FOCUS_DEC_CONTROL;
			param1 = 0;
			param2 = dwParam1;
			break;
		case CT_IRIS_ENLARGE:
			dwPTZCmd = DH_PTZ_APERTURE_ADD_CONTROL;
			param1 = 0;
			param2 = dwParam1;
			break;
		case CT_IRIS_SHRINK:
			dwPTZCmd = DH_PTZ_APERTURE_DEC_CONTROL;
			param1 = 0;
			param2 = dwParam1;
			break;

		case CT_CALL_PRESET:
			dwPTZCmd = DH_PTZ_POINT_MOVE_CONTROL;
			param1 = 0;
			param2 = dwParam1;
			bPreset = TRUE;
			break;
		case CT_SET_PRESET:
			dwPTZCmd = DH_PTZ_POINT_SET_CONTROL;
			param1 = 0;
			param2 = dwParam1;
			bPreset = TRUE;
			break;
		case CT_CLE_PRESET:
			dwPTZCmd = DH_PTZ_POINT_DEL_CONTROL;
			param1 = 0;
			param2 = dwParam1;
			bPreset = TRUE;
			break;
		case CT_AUX1_PWRON:
			dwPTZCmd = DH_PTZ_LAMP_CONTROL;
			dwStop = 0;
			break;
		case CT_AUX1_PWROFF:
			dwPTZCmd = DH_PTZ_LAMP_CONTROL;
			dwStop = 1;
			break;
/*	    case CT_AUX2_PWRON:
			dwPTZCmd = DH_PTZ_LAMP_CONTROL;
			dwStop = 0;
			break;
		case CT_AUX2_PWROFF:
			dwPTZCmd = DH_PTZ_LAMP_CONTROL;
			dwStop = 1;
			break;
		case CT_AUX3_PWRON:
			dwPTZCmd = DH_PTZ_LAMP_CONTROL;
			dwStop = 0;
			break;
		case CT_AUX3_PWROFF:
			dwPTZCmd = DH_PTZ_LAMP_CONTROL;
			dwStop = 1;
			break;  */
		case CT_TILT_LEFTUP:
			dwPTZCmd = DH_EXTPTZ_LEFTTOP;
			param1 = dwParam2;
			param2 = dwParam1;
			break;
		case CT_TILT_RIGHTUP:
			dwPTZCmd = DH_EXTPTZ_RIGHTTOP;
			param1 = dwParam2;
			param2 = dwParam1;
			break;
		case CT_TILT_LEFTDOWN:
			dwPTZCmd = DH_EXTPTZ_LEFTDOWN;
			param1 = dwParam2;
			param2 = dwParam1;
			break;
		case CT_TILT_RIGHTDOWN:
			dwPTZCmd = DH_EXTPTZ_RIGHTDOWN;
			param1 = dwParam2;
			param2 = dwParam1;
			break;
		case CT_RECTZOOM:	//3D定位（HK IP球特有功能）
		{
			if (pExParamIn == NULL || dwExParamInLen == 0)
			{
				wsprintf(g_szLastError, _T("3D定位控制失败：扩展参数为空"));
				return FALSE;
			}
			RECT rcZoom, rcWin;
			CString strRet;

			BOOL bRetval;
			bRetval = AfxExtractSubString(strRet, CString(pExParamIn), 0, ',');
			if (bRetval) rcZoom.left = _wtoi(strRet);
			bRetval = AfxExtractSubString(strRet, CString(pExParamIn), 1, ',');
			if (bRetval) rcZoom.top = _wtoi(strRet);
			bRetval = AfxExtractSubString(strRet, CString(pExParamIn), 2, ',');
			if (bRetval) rcZoom.right = _wtoi(strRet);
			bRetval = AfxExtractSubString(strRet, CString(pExParamIn), 3, ',');
			if (bRetval) rcZoom.bottom = _wtoi(strRet);

			bRetval = AfxExtractSubString(strRet, CString(pExParamIn), 4, ',');
			if (bRetval) rcWin.left = _wtoi(strRet);
			bRetval = AfxExtractSubString(strRet, CString(pExParamIn), 5, ',');
			if (bRetval) rcWin.top = _wtoi(strRet);
			bRetval = AfxExtractSubString(strRet, CString(pExParamIn), 6, ',');
			if (bRetval) rcWin.right = _wtoi(strRet);
			bRetval = AfxExtractSubString(strRet, CString(pExParamIn), 7, ',');
			if (bRetval) rcWin.bottom = _wtoi(strRet);

			CPoint Origin;
			CPoint SendPoint;

			Origin.x = (rcWin.left + rcWin.right) / 2;
			Origin.y = (rcWin.top + rcWin.bottom) / 2;

			int dx = (rcZoom.left + rcZoom.right) / 2;
			int dy = (rcZoom.top + rcZoom.bottom) / 2;

			int width = rcWin.right - rcWin.left;
			int height = rcWin.bottom - rcWin.top;

			SendPoint.x = (dx - Origin.x) * 8192 * 2 / width;
			SendPoint.y = (dy - Origin.y) * 8192 * 2 / height;

			int width2 = rcZoom.right - rcZoom.left;
			int height2 = rcZoom.bottom - rcZoom.top;
			int multiple = 0;

			if (height2 != 0 && width2 != 0)
			{
				multiple = (width * height) / (abs(width2) * height2);
			}
			bRet = CLIENT_DHPTZControlEx(pRvu->lRvuHandle, iChannel - 1, DH_EXTPTZ_FASTGOTO, SendPoint.x, SendPoint.y, multiple, FALSE);

			CString str;
			str.Format(_T("para1 = %d,para2 = %d, para= %d"), SendPoint.x, SendPoint.y, multiple);

			if (!bRet)
			{
				wsprintf(g_szLastError, _T("3D定位控制失败：VSNET_ClientPtzYA3dZoom() 返回失败"));
			}

			return TRUE;
		}
		break;
		default:
			wsprintf(g_szLastError, _T("IDVR_PTZControl:未知或不支持的控制类型"));
			return FALSE;
		}
		olddwPTZCmd = dwPTZCmd;
		if (!bPreset)
		{
			CLIENT_DHPTZControlEx(pRvu->lRvuHandle, iChannel - 1, dwPTZCmd, (unsigned char)param1, (unsigned char)param2, 0, dwStop);
		}
		else
		{
			CLIENT_DHPTZControlEx(pRvu->lRvuHandle, iChannel - 1, dwPTZCmd, param1, param2, 0, dwStop);
		}

	}
	else
	{
		wsprintf(g_szLastError, _T("IDVR_PTZControl error: 不支持的PTZ协议：%s"), strProtocol);
		return FALSE;
	}

	return TRUE;
}

extern "C" BOOL CALL_TYPE IDVR_PTZControlByChannel(HANDLE hChanHandle, CTRLTYPE command, DWORD dwParam1, DWORD dwParam2, LPCSTR pExParamIn, DWORD dwExParamInLen)
{
	return FALSE;
	CChannel* pChan = (CChannel*)hChanHandle;
	if (pChan == NULL)
	{
		wsprintf(g_szLastError, _T("IDVR_PTZControlByChannel error: Invalid Handle"));
		return FALSE;
	}

	BOOL bRet = FALSE;

	//DH SDK限制（速度范围：1～8）
	if (dwParam1 <= 0)		dwParam1 = 1;
	if (dwParam1 > 8)		dwParam1 = 8;

	static DWORD olddwPTZCmd = -1;
	DWORD dwPTZCmd = 0;
	DWORD dwStop = 0;
	BOOL bPreset = FALSE;
	bRet = FALSE;
	switch(command)
	{
	case CT_STOP:
		dwPTZCmd = olddwPTZCmd;
		dwStop = 1;
		break;
	case CT_TILT_LEFTUP:
		dwPTZCmd = DH_EXTPTZ_LEFTTOP;
		break;
	case CT_TILT_RIGHTUP:
		dwPTZCmd = DH_EXTPTZ_RIGHTTOP;
		break;
	case CT_TILT_LEFTDOWN:
		dwPTZCmd = DH_EXTPTZ_LEFTDOWN;
		break;
	case CT_TILT_RIGHTDOWN:
		dwPTZCmd = DH_EXTPTZ_RIGHTDOWN;
		break;
	case CT_TILT_UP:
		dwPTZCmd = DH_PTZ_UP_CONTROL;
		break;
	case CT_TILT_DOWN:
		dwPTZCmd = DH_PTZ_DOWN_CONTROL;
		break;
	case CT_PAN_LEFT:
		dwPTZCmd = DH_PTZ_LEFT_CONTROL;
		break;
	case CT_PAN_RIGHT:
		dwPTZCmd = DH_PTZ_RIGHT_CONTROL;
		break;
/*	case CT_PAN_AUTO:
		dwPTZCmd = PAN_AUTO;
		break;
*/	case CT_ZOOM_IN:
		dwPTZCmd = DH_PTZ_ZOOM_ADD_CONTROL;
		break;
	case CT_ZOOM_OUT:
		dwPTZCmd = DH_PTZ_ZOOM_DEC_CONTROL;
		break;
	case CT_FOCUS_NEAR:
		dwPTZCmd = DH_PTZ_FOCUS_DEC_CONTROL;
		break;
	case CT_FOCUS_FAR:
		dwPTZCmd = DH_PTZ_FOCUS_ADD_CONTROL;
		break;
	case CT_IRIS_ENLARGE:
		dwPTZCmd = DH_PTZ_APERTURE_ADD_CONTROL;
		break;
	case CT_IRIS_SHRINK:
		dwPTZCmd = DH_PTZ_APERTURE_DEC_CONTROL;
		break;
	case CT_CALL_PRESET:
		dwPTZCmd = DH_PTZ_POINT_MOVE_CONTROL;
		bPreset = TRUE;
		break;
	case CT_SET_PRESET:
		dwPTZCmd = DH_PTZ_POINT_SET_CONTROL;
		bPreset = TRUE;
		break;
	case CT_CLE_PRESET:
		dwPTZCmd = DH_PTZ_POINT_DEL_CONTROL;
		bPreset = TRUE;
		break;
	case CT_AUX1_PWRON:	//辅助开关: 辅助号与宏定义的对应关系是在DVS中设置为Pelco-d协议时测试后得到的
		dwPTZCmd = DH_EXTPTZ_AUXIOPEN;
		dwStop = 0;
		break;
	case CT_AUX1_PWROFF:
		dwPTZCmd = DH_EXTPTZ_AUXICLOSE;
		dwStop = 1;
		break;
/*	case CT_AUX2_PWRON:
		dwPTZCmd = LIGHT_PWRON;
		dwStop = 0;
		break;
	case CT_AUX2_PWROFF:
		dwPTZCmd = LIGHT_PWRON;
		dwStop = 1;
		break;
	case CT_AUX3_PWRON:
		dwPTZCmd = AUX_PWRON1;
		dwStop = 0;
		break;
	case CT_AUX3_PWROFF:
		dwPTZCmd = AUX_PWRON1;
		dwStop = 1;
		break;
*/	default:
		wsprintf(g_szLastError, _T("IDVR_PTZControl error:未知或不支持的控制类型:%d"), command);
		return FALSE;
	}

//	bRet = CLIENT_DHPTZControl(pChan->lChanHandle, dwPTZCmd, dwParam1, dwParam2, 0, dwStop);
	if (!bRet)
		wsprintf(g_szLastError, _T("CLIENT_DHPTZControl failed: Error code = %d"), CLIENT_GetLastError());

	olddwPTZCmd = dwPTZCmd;
	return bRet;
}

extern "C" BOOL CALL_TYPE IDVR_GetDeviceInfo(HANDLE hDevHandle, PDEVICEINFO pDeviceInfo)
{
	CRvu* pRvu = (CRvu*)hDevHandle;
	if (pRvu == NULL)
	{
		wsprintf(g_szLastError, _T("IDVR_GetDeviceInfo error: Invalid Handle"));
		return FALSE;
	}

	if (pDeviceInfo == NULL)
	{
		wsprintf(g_szLastError, _T("IDVR_GetDeviceInfo: Invalid Params!"));
		return FALSE;
	}
	pDeviceInfo->nchannelnum = pRvu->iChannelNumber;
	pDeviceInfo->ndisknumber = pRvu->dwDiskNum;
	pDeviceInfo->nioinputnum = pRvu->dwAlarmInNum;
	pDeviceInfo->niooutputnum = pRvu->dwAlarmOutNum;
	sprintf(pDeviceInfo->sdevicetype, "DF1500-EDVR");
	sprintf(pDeviceInfo->smanufactuer, "DongFang");
	sprintf(pDeviceInfo->ssoftversion, "V1.0");

	return TRUE;
}

extern "C" BOOL CALL_TYPE IDVR_GetChannelInfo(HANDLE hDevHandle, PCHANNELINFO pChannelInfo)
{
	CRvu* pRvu = (CRvu*)hDevHandle;
	if (pRvu == NULL)
	{
		wsprintf(g_szLastError, _T("IDVR_GetChannelInfo error: Invalid Handle"));
		return FALSE;
	}

	if (pChannelInfo == NULL)
	{
		wsprintf(g_szLastError, _T("IDVR_GetChannelInfo: Invalid Params!"));
		return FALSE;
	}
	pChannelInfo->uchannels = pRvu->iChannelNumber;
	DHDEV_CHANNEL_CFG pc;
	DWORD dwRetBytes;
	for (UINT i = 0; i<pChannelInfo->uchannels; i++)
	{
		if (CLIENT_GetDevConfig(pRvu->lRvuHandle, DH_DEV_CHANNELCFG, i, &pc, sizeof(DHDEV_CHANNEL_CFG), &dwRetBytes))
		{
			sprintf(pChannelInfo->schannode[i].sname, "%s", pc.szChannelName);
		}
		else
			pChannelInfo->schannode[i].sname[0] = 0;
	}

	return TRUE;
}

extern "C" BOOL CALL_TYPE IDVR_SetDeviceTime(HANDLE hDevHandle, PDEVTIME pTime)
{
	CRvu* pRvu = (CRvu*)hDevHandle;
	if (pRvu == NULL)
	{
		wsprintf(g_szLastError, _T("IDVR_SetDeviceTime error: Invalid Handle"));
		return FALSE;
	}

	if (pTime == NULL)
	{
		wsprintf(g_szLastError, _T("IDVR_SetDeviceTime: Invalid Params!"));
		return FALSE;
	}

	NET_TIME CurTime;
	CurTime.dwYear = pTime->ufromyear;
	CurTime.dwMonth = pTime->ufrommonth;
	CurTime.dwDay = pTime->ufromday;
	CurTime.dwHour = pTime->ufromhour;
	CurTime.dwMinute = pTime->ufromminute;
	CurTime.dwSecond = pTime->ufromsecond;
	if (!CLIENT_SetDevConfig(pRvu->lRvuHandle, DH_DEV_TIMECFG, -1, &CurTime, sizeof(NET_TIME)))
	{
		wsprintf(g_szLastError, _T("IDVR_SetDeviceTime failed, err=%d"), CLIENT_GetLastError());
		return FALSE;
	}

	return TRUE;
}

extern "C" BOOL CALL_TYPE IDVR_StartDeviceRecord(HANDLE hDevHandle, long lChannel, long lRecordType, LPCSTR pExParamIn, DWORD dwExParamInLen)
{
	CRvu* pRvu = (CRvu*)hDevHandle;
	if (pRvu == NULL)
	{
		wsprintf(g_szLastError, _T("IDVR_SetDeviceTime error: Invalid Handle"));
		return FALSE;
	}
	char m_State[DFJK_MAX_CHAN];
	memset(m_State, 0, DFJK_MAX_CHAN);
	CLIENT_QueryRecordState(pRvu->lRvuHandle, m_State, DFJK_MAX_CHAN, &pRvu->iChannelNumber);
	m_State[lChannel - 1] = 1;
	BOOL bRet = CLIENT_SetupRecordState(pRvu->lRvuHandle, m_State, pRvu->iChannelNumber);
	return bRet;
}

extern "C" BOOL CALL_TYPE IDVR_StopDeviceRecord(HANDLE hDevHandle, long lChannel)
{
	CRvu* pRvu = (CRvu*)hDevHandle;
	if (pRvu == NULL)
	{
		wsprintf(g_szLastError, _T("IDVR_SetDeviceTime error: Invalid Handle"));
		return FALSE;
	}
	char m_State[DFJK_MAX_CHAN];
	memset(m_State, 0, DFJK_MAX_CHAN);
	CLIENT_QueryRecordState(pRvu->lRvuHandle, m_State, DFJK_MAX_CHAN, &pRvu->iChannelNumber);
	m_State[lChannel - 1] = 0;
	BOOL bRet = CLIENT_SetupRecordState(pRvu->lRvuHandle, m_State, pRvu->iChannelNumber);
	return bRet;
}

extern "C" BOOL CALL_TYPE IDVR_GetDeviceRecordStatus(HANDLE hDevHandle, long lChannel, int *piState, LPCSTR pExParamIn, DWORD dwExParamInLen)
{
	CRvu* pRvu = (CRvu*)hDevHandle;
	if (pRvu == NULL)
	{
		wsprintf(g_szLastError, _T("IDVR_SetDeviceTime error: Invalid Handle"));
		return FALSE;
	}
	char m_State[DFJK_MAX_CHAN];
	memset(m_State, 0, DFJK_MAX_CHAN);
	CLIENT_QueryRecordState(pRvu->lRvuHandle, m_State, DFJK_MAX_CHAN, &pRvu->iChannelNumber);
	*piState = m_State[lChannel - 1];
	return TRUE;
}

typedef BOOL(FAR _stdcall * LPShowDlg)(LONG lServerID, CString strUserName, int iLanguageType, BOOL bCenter, int x, int y);

extern "C" BOOL CALL_TYPE IDVR_ShowDeviceSettingDlg(HANDLE hDevHandle, LPCSTR pExParamIn, DWORD dwExParamInLen)
{
	CRvu* pRvu = (CRvu*)hDevHandle;
	if (pRvu == NULL)
	{
		wsprintf(g_szLastError, _T("IDVR_ShowDeviceSettingDlg error: Invalid Dev Handle"));
		return FALSE;
	}

	BOOL bRet = FALSE;
	LPShowDlg fShowDlg = NULL;
	TCHAR szDLL[MAX_PATH];
	wsprintf(szDLL, _T("%sDHRemCfgWnd.dll"), g_szDLLPath);
	HINSTANCE  hDLL = ::LoadLibrary(szDLL);
	if (hDLL)
	{
		fShowDlg = (LPShowDlg)::GetProcAddress(hDLL, "CLIENT_ShowRemSettingWnd");
		if (fShowDlg)
		{
			CString str = CString(pRvu->szRvuUserName);
			fShowDlg(pRvu->lRvuHandle, str, 0, TRUE, 0, 0);
			bRet = TRUE;
		}
		FreeLibrary(hDLL);
	}

	if (!bRet)
	{
		wsprintf(g_szLastError, _T("IDVR_ShowDeviceSettingDlg failed, Check ShowHCRemCfgWnd.dll"));
	}
	return TRUE;
}

extern "C" BOOL CALL_TYPE IDVR_RestartDevice(HANDLE hDevHandle)
{
	CRvu* pRvu = (CRvu*)hDevHandle;
	if (pRvu == NULL)
	{
		wsprintf(g_szLastError, _T("IDVR_RestartDevice error: Invalid Dev Handle"));
		return FALSE;
	}
	if (!CLIENT_ControlDevice(pRvu->lRvuHandle, DH_CTRL_REBOOT, NULL))
	{
		wsprintf(g_szLastError, _T("IDVR_RestartDevice failed: %d"), CLIENT_GetLastError());
		return FALSE;
	}
	return TRUE;
}


extern "C" BOOL CALL_TYPE IDVR_GetDeviceState(HANDLE hDevHandle, DEV_STAT* pStat)
{
	CRvu* pRvu = (CRvu*)hDevHandle;
	if (pRvu == NULL)
	{
		wsprintf(g_szLastError, _T("IDVR_GetChannelInfo error: Invalid Handle"));
		return FALSE;
	}
	//获取设备状态
	/*
	NET_DVR_WORKSTATE WorkState;
	UINT i;
	if(!NET_DVR_GetDVRWorkState(pRvu->lRvuHandle, &WorkState))
	*/
	int retlen = 0;
	int nRet = -1;
	char *recordcap;
	DWORD dwNetConn;

	//	DWORD dwBitRate;
	UINT i;

	for (i = 0; i < (UINT)pRvu->iChannelNumber; i++)
	{

		nRet = CLIENT_QueryDevState(pRvu->lRvuHandle, DH_DEVSTATE_RECORDING,
			(char*)&recordcap, sizeof(int), 0, 100);
		pStat->struChanState[i].byRecordStatic = (BYTE)recordcap;

		nRet = CLIENT_QueryDevState(pRvu->lRvuHandle, DH_DEVSTATE_CONN,
			(char*)&dwNetConn, 11 * sizeof(DWORD), 0, 100);
		pStat->struChanState[i].bySignalStatic = (BYTE)dwNetConn;

		//		nRet = CLIENT_QueryDevState(pRvu->lRvuHandle, DH_DEVSTATE_BITRATE, (char *)dwBitRate, 0, &retlen);
		//		pStat->struChanState[i].dwBitRate = dwBitRate;

		//获取设备接连
		//pStat->struChanState[i].dwLinkNum = WorkState.struChanStatic[i].dwLinkNum;
	}


	/*for (i=0;i < pRvu->dwAlarmInNum;i++)
	{
	pStat->byAlarmInStatus[i] = WorkState.byAlarmInStatic[i];
	}
	for (i=0;i < pRvu->dwAlarmOutNum;i++)
	{
	pStat->byAlarmOutStatus[i] = WorkState.byAlarmOutStatic[i];
	}
	*/
	return TRUE;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//3、	报警和数据类函数	20
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

extern "C" BOOL CALL_TYPE IDVR_SetIOStatus(HANDLE hDevHandle, int iIOPort, int iValue,
	LPCSTR lpszProtocol, int nAddress, IOTYPE ioType, COMTYPE comType,
	LPCSTR pExParamIn, DWORD dwExParamInLen)
{
	CRvu* pRvu = (CRvu*)hDevHandle;
	if (pRvu == NULL)
	{
		wsprintf(g_szLastError, _T("IDVR_RestartDevice error: Invalid Dev Handle"));
		return FALSE;
	}
	//获取协议
	CString strProtocol = CString(lpszProtocol);
	BOOL bRet = FALSE;
	if (strProtocol == "EDVR" || strProtocol == "")
	{
		if (ioType != IO_OUT)
		{
			wsprintf(g_szLastError, _T("IDVR_SetIOStatus error: 只能控制状态量输出"));
			return FALSE;
		}
		if (iIOPort <= 0 || iIOPort > (int)pRvu->dwAlarmOutNum)
		{
			wsprintf(g_szLastError, _T("IDVR_SetIOStatus: IO号无效。范围:1-%d"), pRvu->dwAlarmOutNum);
			return FALSE;
		}

		ALARM_CONTROL m_OutState;

		m_OutState.index = iIOPort - 1;
		m_OutState.state = iValue;
		bRet = CLIENT_IOControl(pRvu->lRvuHandle, DH_ALARMOUTPUT, &m_OutState, sizeof(ALARM_CONTROL));
		TRACE("===更改状态，端口%d,值%d\n", iIOPort - 1, iValue);
		if (!bRet)
		{
			wsprintf(g_szLastError, _T("IDVR_SetIOStatus Failed, err = %d"), CLIENT_GetLastError());
		}
	}
	else
	{
		wsprintf(g_szLastError, _T("IDVR_SetIOStatus error: Unknown protocol: %s"), strProtocol);
	}
	return bRet;
}

extern "C" BOOL CALL_TYPE IDVR_GetIOStatus(HANDLE hDevHandle, int iIOPort, int *piValue, LPCSTR lpszProtocol,
	int nAddress, IOTYPE ioType, COMTYPE comType, int nTimeout, LPCSTR pExParamIn,
	DWORD dwExParamInLen, LPSTR* ppExParamOut, DWORD* pdwExParamOutLen)
{
	*piValue = 0;
	*ppExParamOut = NULL;
	*pdwExParamOutLen = 0;

	CRvu* pRvu = (CRvu*)hDevHandle;
	if (pRvu == NULL)
	{
		wsprintf(g_szLastError, _T("IDVR_GetIOStatus error: Invalid Dev Handle"));
		return FALSE;
	}

	CString strProtocol = CString(lpszProtocol);
	BOOL bRet = FALSE;

	if (strProtocol == _T("EDVR") || strProtocol == _T(""))
	{
		if (ioType == IO_IN)
		{
			ALARM_CONTROL m_InState[DFJK_MAX_ALARMIN];
			int m_InNum = 0;
			bRet = CLIENT_QueryIOControlState(pRvu->lRvuHandle, DH_ALARMINPUT, NULL, 0, &m_InNum);
			if (bRet)
			{
				if (iIOPort<0 && iIOPort>m_InNum)
				{
					wsprintf(g_szLastError, _T("IDVR_GetIOStatus error:Invalid IOPort 0-%d"), m_InNum - 1);
					return FALSE;
				}
				bRet = CLIENT_QueryIOControlState(pRvu->lRvuHandle, DH_ALARMINPUT, m_InState, sizeof(ALARM_CONTROL)*m_InNum, &m_InNum);
				if (!bRet)
				{
					wsprintf(g_szLastError, _T("IDVR_GetIOStatus Fail: %d"), CLIENT_GetLastError());
					return FALSE;
				}
			}
			if (iIOPort == 0)  //批量获取
			{
				*piValue = -1;	//随意设置一个值
				CString strExtOut;
				CString strTemp;
				strExtOut.Format(_T("COUNT=%d;VAL="), m_InNum);
				for (int i = 0; i<m_InNum; i++)
				{
					strTemp.Format(_T("%d,"), m_InState[i].state);
					strExtOut += strTemp;
				}
				strExtOut += _T(";");
				DWORD dwLen = strExtOut.GetLength();
				*ppExParamOut = new char[dwLen + 1];
				if (*ppExParamOut == NULL)
				{
					wsprintf(g_szLastError, _T("IDVR_GetIOStatus Fail: 内存不足"));
					return FALSE;
				}
				USES_CONVERSION;
				strcpy(*ppExParamOut, T2A(strExtOut));
				*pdwExParamOutLen = dwLen;
			}
			else
			{
				*piValue = m_InState[iIOPort - 1].state;
			}
		}
		else if (ioType == IO_OUT)
		{
			ALARM_CONTROL m_OutState[DFJK_MAX_ALARMIN];
			int m_OutNum = 0;
			bRet = CLIENT_QueryIOControlState(pRvu->lRvuHandle, DH_ALARMOUTPUT, NULL, 0, &m_OutNum);
			if (bRet)
			{
				if (iIOPort<0 && iIOPort>m_OutNum)
				{
					wsprintf(g_szLastError, _T("IDVR_GetIOStatus error:Invalid IOPort 0-%d"), m_OutNum - 1);
					return FALSE;
				}
				bRet = CLIENT_QueryIOControlState(pRvu->lRvuHandle, DH_ALARMOUTPUT, m_OutState, sizeof(ALARM_CONTROL)*m_OutNum, &m_OutNum);
				if (!bRet)
				{
					wsprintf(g_szLastError, _T("IDVR_GetIOStatus Fail: %d"), CLIENT_GetLastError());
					return FALSE;
				}
			}
			if (iIOPort == 0)  //批量获取
			{
				*piValue = -1;	//随意设置一个值
				CString strExtOut;
				CString strTemp;
				strExtOut.Format(_T("COUNT=%d;VAL="), m_OutNum);
				for (int i = 0; i<m_OutNum; i++)
				{
					strTemp.Format(_T("%d,"), m_OutState[i].state);
					strExtOut += strTemp;
				}
				strExtOut += _T(";");
				DWORD dwLen = strExtOut.GetLength();
				*ppExParamOut = new char[dwLen + 1];
				if (*ppExParamOut == NULL)
				{
					wsprintf(g_szLastError, _T("IDVR_GetIOStatus Fail: 内存不足"));
					return FALSE;
				}
				USES_CONVERSION;
				strcpy(*ppExParamOut, T2A(strExtOut));
				*pdwExParamOutLen = dwLen;
			}
			else
			{
				*piValue = m_OutState[iIOPort - 1].state;
			}

		}
	}
	else
	{
		wsprintf(g_szLastError, _T("IDVR_GetIOStatus error: Unknown protocol: %s"), strProtocol);
	}
	return TRUE;
}

extern "C" BOOL CALL_TYPE IDVR_SetIOValue(HANDLE hDevHandle, int iIOPort, float fValue,
	LPCSTR lpszProtocol, int nAddress, IOTYPE ioType, COMTYPE comType,
	LPCSTR pExParamIn, DWORD dwExParamInLen)
{
	wsprintf(g_szLastError, _T("IDVR_SetIOValue error: 尚未实现此函数"));
	return FALSE;
}

extern "C" BOOL CALL_TYPE IDVR_GetIOValue(HANDLE hDevHandle, int iIOPort, float *pfValue,
	LPCSTR lpszProtocol, int nAddress, IOTYPE ioType, COMTYPE comType, int nTimeout,
	LPCSTR pExParamIn, DWORD dwExParamInLen, LPSTR* ppExParamOut, DWORD* pdwExParamOutLen)
{

	return TRUE;
}

extern "C" BOOL CALL_TYPE IDVR_SetAlarmSubscribe(HANDLE hDevHandle, DWORD dwUser, CBF_AlarmInfo cbf, LPCSTR pExParamIn, DWORD dwExParamInLen)
{
	//ADD CODE
	CRvu* pRvu = (CRvu*)hDevHandle;
	if (pRvu == NULL)
	{
		wsprintf(g_szLastError, _T("IDVR_RestartDevice error: Invalid Dev Handle"));
		return FALSE;
	}
	BOOL bRet = CLIENT_StartListenEx(pRvu->lRvuHandle);
	g_CBF_AlarmInfo = cbf;
	pRvu->dwAlarmUserData = dwUser;
	TRACE("======报警订阅\n");
	return bRet;
}

extern "C" BOOL CALL_TYPE IDVR_SetAlarmUnsubscribe(HANDLE hDevHandle)
{
	//ADD CODE
	CRvu* pRvu = (CRvu*)hDevHandle;
	if (pRvu == NULL)
	{
		wsprintf(g_szLastError, _T("IDVR_RestartDevice error: Invalid Dev Handle"));
		return FALSE;
	}
	BOOL bRet = CLIENT_StopListen(pRvu->lRvuHandle);

	return bRet;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//4、	回放类函数	24
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

extern "C" BOOL CALL_TYPE IDVR_QueryDevRecord(HANDLE hDevHandle,
	int iChannel,
	RECORDTYPE RecType,
	PQUERYTIME ptime,
	LPSTR* ppExParamOut,
	DWORD* pdwExParamOutLen)
{
	*ppExParamOut = NULL;
	*pdwExParamOutLen = 0;

	if (hDevHandle == NULL)
	{
		wsprintf(g_szLastError, _T("IDVR_QueryDevRecord error: Invalid Handle"));
		return FALSE;
	}
	CRvu* pRvu = (CRvu*)hDevHandle;
	if (pRvu == NULL)
	{
		wsprintf(g_szLastError, _T("IDVR_QueryDevRecord error: Invalid Dev Handle"));
		return FALSE;
	}
	if (ptime == NULL)
	{
		wsprintf(g_szLastError, _T("IDVR_QueryDevRecord: 参数ptime非法"));
		return FALSE;
	}
	if (iChannel <= 0 || iChannel > pRvu->iChannelNumber)
	{
		wsprintf(g_szLastError, _T("IDVR_QueryDevRecord error: Invalid chan No(1～%d)"), pRvu->iChannelNumber);
		return FALSE;
	}
	int nRecordFileType = 0;

	if (RecType == ALARM_REC)
	{
		nRecordFileType = 3;
	}
	NET_TIME StartTime;
	NET_TIME StopTime;

	StartTime.dwYear = ptime->ufromyear;
	StartTime.dwMonth = ptime->ufrommonth;
	StartTime.dwDay = ptime->ufromday;
	StartTime.dwHour = ptime->ufromhour;
	StartTime.dwMinute = ptime->ufromminute;
	StartTime.dwSecond = ptime->ufromsecond;
	StopTime.dwYear = ptime->utoyear;
	StopTime.dwMonth = ptime->utomonth;
	StopTime.dwDay = ptime->utoday;
	StopTime.dwHour = ptime->utohour;
	StopTime.dwMinute = ptime->utominute;
	StopTime.dwSecond = ptime->utosecond;

	LONG lFileHandle = CLIENT_FindFile(pRvu->lRvuHandle, iChannel - 1, nRecordFileType, "",
		&StartTime, &StopTime, false, 2000);
	if (lFileHandle == DFJK_INVALID_HANDLE)
	{
		wsprintf(g_szLastError, _T("IDVR_QueryDevRecord: 查询失败, Error code=%d"), CLIENT_GetLastError());
		return FALSE;
	}

	NET_RECORDFILE_INFO stpThisDlgInfo;
	CString strXmlHeader;
	strXmlHeader.Format(_T("<?xml version='1.0' ?>"));
	CString strXmlFileBody = _T("");
	CString strTemp = _T("");
	LONG lCount = 0;

	BOOL bRet;
	bRet = CLIENT_FindNextFile(lFileHandle, &stpThisDlgInfo);
	if (!bRet)
	{
		wsprintf(g_szLastError, _T("IDVR_QueryDevRecord:查无记录，Error code=%d"), CLIENT_GetLastError());
		return FALSE;
	}
	while (bRet == 1)
	{
		lCount++;
		CString filename;
		filename.Format(_T("%02d_%d_%d%02d%02d%02d%02d%02d_%d%02d%02d%02d%02d%02d_%d_%02d_%d"), stpThisDlgInfo.ch, stpThisDlgInfo.size,
			stpThisDlgInfo.starttime.dwYear, stpThisDlgInfo.starttime.dwMonth, stpThisDlgInfo.starttime.dwDay, stpThisDlgInfo.starttime.dwHour,
			stpThisDlgInfo.starttime.dwMinute, stpThisDlgInfo.starttime.dwSecond, stpThisDlgInfo.endtime.dwYear, stpThisDlgInfo.endtime.dwMonth,
			stpThisDlgInfo.endtime.dwDay, stpThisDlgInfo.endtime.dwHour, stpThisDlgInfo.endtime.dwMinute, stpThisDlgInfo.endtime.dwSecond, stpThisDlgInfo.nRecordFileType,
			stpThisDlgInfo.driveno, stpThisDlgInfo.startcluster);
		strTemp.Format(_T("<File name='%s' length='%d' starttime='%d-%02d-%02d %02d:%02d:%02d' endtime='%d-%02d-%02d %02d:%02d:%02d' type='%d'/>\n"),
			filename, stpThisDlgInfo.size, stpThisDlgInfo.starttime.dwYear, stpThisDlgInfo.starttime.dwMonth,
			stpThisDlgInfo.starttime.dwDay, stpThisDlgInfo.starttime.dwHour, stpThisDlgInfo.starttime.dwMinute,
			stpThisDlgInfo.starttime.dwSecond, stpThisDlgInfo.endtime.dwYear, stpThisDlgInfo.endtime.dwMonth,
			stpThisDlgInfo.endtime.dwDay, stpThisDlgInfo.endtime.dwHour, stpThisDlgInfo.endtime.dwMinute,
			stpThisDlgInfo.endtime.dwSecond, stpThisDlgInfo.nRecordFileType);
		strXmlFileBody += strTemp;
		bRet = CLIENT_FindNextFile(lFileHandle, &stpThisDlgInfo);
		if (!bRet)
		{
			wsprintf(g_szLastError, _T("IDVR_QueryDevRecord: 查找结束"));
			break;
		}
	}
	CLIENT_FindClose(lFileHandle);
	strTemp.Format(_T("<Result Count='%d'>\n"), lCount);
	strXmlHeader = strXmlHeader + strTemp + strXmlFileBody;
	strTemp.Format(_T("</Result>"));
	strXmlHeader = strXmlHeader + strTemp;

	const long lLength = strXmlHeader.GetLength();
	*pdwExParamOutLen = lLength;

	*ppExParamOut = new char[lLength + 1];
	USES_CONVERSION;
	strcpy(*ppExParamOut, T2A(strXmlHeader));
	return TRUE;
}

extern "C" BOOL CALL_TYPE IDVR_CancelQueryDevRecord(HANDLE hDevHandle)
{
	//ADD CODE
	return FALSE;
}

extern "C" BOOL CALL_TYPE IDVR_DownloadFile(HANDLE hDevHandle, LPCSTR lpszSrcFileName, LPCSTR lpszLocalFileName, HANDLE* phDownHandle)
{
	*phDownHandle = NULL;
	CRvu* pRvu = (CRvu*)hDevHandle;
	if (pRvu == NULL)
	{
		wsprintf(g_szLastError, _T("IDVR_RestartDevice error: Invalid Dev Handle"));
		return FALSE;
	}
	if (lpszSrcFileName == NULL || lpszLocalFileName == NULL)
	{
		wsprintf(g_szLastError, _T("IDVR_DownloadFile error: Invalid param!"));
		return FALSE;
	}


	NET_RECORDFILE_INFO RecFileInfo;
	CString sStr;
	sStr.Format(_T("%s"), lpszSrcFileName);
	CStringArray strArray;
	CString sStrtemp;
	while (-1 != sStr.Find(_T("_"), 0))
	{
		sStrtemp = sStr.Left(sStr.Find(_T("_"), 0));
		sStr = sStr.Right(sStr.GetLength() - sStr.Find(_T("_"), 0) - 1);
		strArray.Add(sStrtemp);
	}
	if (sStr.GetLength()>0)
	{
		strArray.Add(sStr);
	}

	RecFileInfo.ch = _wtoi(strArray[0]);
	RecFileInfo.size = _wtoi(strArray[1]);
	RecFileInfo.starttime.dwYear = _wtoi(strArray[2].Mid(0, 4));
	RecFileInfo.starttime.dwMonth = _wtoi(strArray[2].Mid(4, 2));
	RecFileInfo.starttime.dwDay = _wtoi(strArray[2].Mid(6, 2));
	RecFileInfo.starttime.dwHour = _wtoi(strArray[2].Mid(8, 2));
	RecFileInfo.starttime.dwMinute = _wtoi(strArray[2].Mid(10, 2));
	RecFileInfo.starttime.dwSecond = _wtoi(strArray[2].Mid(12, 2));
	RecFileInfo.endtime.dwYear = _wtoi(strArray[3].Mid(0, 4));
	RecFileInfo.endtime.dwMonth = _wtoi(strArray[3].Mid(4, 2));
	RecFileInfo.endtime.dwDay = _wtoi(strArray[3].Mid(6, 2));
	RecFileInfo.endtime.dwHour = _wtoi(strArray[3].Mid(8, 2));
	RecFileInfo.endtime.dwMinute = _wtoi(strArray[3].Mid(10, 2));
	RecFileInfo.endtime.dwSecond = _wtoi(strArray[3].Mid(12, 2));
	RecFileInfo.nRecordFileType = _wtoi(strArray[4]);
	RecFileInfo.driveno = _wtoi(strArray[5]);
	RecFileInfo.startcluster = _wtoi(strArray[6]);

	long lDownloadFileHandle = CLIENT_DownloadByRecordFile(pRvu->lRvuHandle, &RecFileInfo,
		(char *)lpszLocalFileName, NULL, (DWORD)100);
	if (lDownloadFileHandle == DFJK_INVALID_HANDLE)
	{
		wsprintf(g_szLastError, _T("IDVR_DownloadFile: 下载文件失败,Error code=%d"), CLIENT_GetLastError());
		return FALSE;
	}
	*phDownHandle = (HANDLE)lDownloadFileHandle;
	return TRUE;
}



extern "C" BOOL CALL_TYPE IDVR_StopDownloadFile(HANDLE hDownHandle)
{
	if (hDownHandle == (HANDLE)DFJK_INVALID_HANDLE)
	{
		wsprintf(g_szLastError, _T("IDVR_StopDownloadFile Error: 尚未开始下载"));
		return FALSE;
	}

	BOOL bRet = CLIENT_StopDownload((LONG)hDownHandle);
	if (bRet)
		g_szLastError[0] = 0;
	else
		wsprintf(g_szLastError, _T("IDVR_StopDownloadFile Error，code=%d"), CLIENT_GetLastError());

	hDownHandle = (HANDLE)DFJK_INVALID_HANDLE;

	return bRet;
}

extern "C" BOOL CALL_TYPE IDVR_GetDownloadPos(HANDLE hDownHandle, int *piCurPos)
{
	if (hDownHandle == (HANDLE)DFJK_INVALID_HANDLE)
	{
		wsprintf(g_szLastError, _T("IDVR_GetDownloadPos: 尚未开始下载"));
		return FALSE;
	}
	int nTotalSize;
	int nDownLoadSize;
	BOOL bRet = CLIENT_GetDownloadPos((LONG)hDownHandle, &nTotalSize, &nDownLoadSize);
	*piCurPos = nDownLoadSize * 100 / nTotalSize;
	return  bRet;
}

extern "C" BOOL CALL_TYPE IDVR_DownloadFileByTime(HANDLE hDevHandle, int iChannel, PQUERYTIME  pTime, LPCSTR lpszLocalFileName, HANDLE* phDownHandle)
{
	CRvu* pRvu = (CRvu*)hDevHandle;
	if (pRvu == NULL)
	{
		wsprintf(g_szLastError, _T("IDVR_DownloadFileByTime error: Invalid Dev Handle"));
		return FALSE;
	}
	NET_TIME stime, etime;
	stime.dwYear = pTime->ufromyear;
	stime.dwMonth = pTime->ufrommonth;
	stime.dwDay = pTime->ufromday;
	stime.dwHour = pTime->ufromhour;
	stime.dwMinute = pTime->ufromminute;
	stime.dwSecond = pTime->ufromsecond;
	etime.dwYear = pTime->utoyear;
	etime.dwMonth = pTime->utomonth;
	etime.dwDay = pTime->utoday;
	etime.dwHour = pTime->utohour;
	etime.dwMinute = pTime->utominute;
	etime.dwSecond = pTime->utosecond;

	LONG lDownID = CLIENT_DownloadByTime(pRvu->lRvuHandle, iChannel - 1, 0, &stime, &etime, (char *)lpszLocalFileName, NULL, (DWORD)100);
	if (lDownID == DFJK_INVALID_HANDLE)
	{
		wsprintf(g_szLastError, _T("IDVR_DownloadFileByTime: 下载文件失败,Error code=%d"), CLIENT_GetLastError());
		return FALSE;
	}
	*phDownHandle = (HANDLE)lDownID;
	return TRUE;
}

extern "C" BOOL CALL_TYPE IDVR_RemotePlayByTime(HANDLE hDevHandle, int iChannel, PQUERYTIME pTime, HWND hWnd, HANDLE* hPlay)
{
	CRvu* pRvu = (CRvu*)hDevHandle;
	if (pRvu == NULL)
	{
		wsprintf(g_szLastError, _T("IDVR_StopAudioPhone error: Invalid Dev Handle"));
		return FALSE;
	}
	if (pTime == NULL)
	{
		wsprintf(g_szLastError, _T("IDVR_PlayBackByTime error: Invalid Params"));
		return FALSE;
	}

	if (iChannel <= 0 || iChannel > pRvu->iChannelNumber)
	{
		wsprintf(g_szLastError, _T("IDVR_PlayBackByTime error: Invalid chan No(1～%d)"), pRvu->iChannelNumber);
		return FALSE;
	}
	NET_RECORDFILE_INFO RecFileInfo;
	RecFileInfo.starttime.dwYear = pTime->ufromyear;
	RecFileInfo.starttime.dwMonth = pTime->ufrommonth;
	RecFileInfo.starttime.dwDay = pTime->ufromday;
	RecFileInfo.starttime.dwHour = pTime->ufromhour;
	RecFileInfo.starttime.dwMinute = pTime->ufromminute;
	RecFileInfo.starttime.dwSecond = pTime->ufromsecond;
	RecFileInfo.endtime.dwYear = pTime->utoyear;
	RecFileInfo.endtime.dwMonth = pTime->utomonth;
	RecFileInfo.endtime.dwDay = pTime->utoday;
	RecFileInfo.endtime.dwHour = pTime->utohour;
	RecFileInfo.endtime.dwMinute = pTime->utominute;
	RecFileInfo.endtime.dwSecond = pTime->utosecond;

	CRecPlay *pRemote = new CRecPlay();
	LONG lPlayFileHandle = CLIENT_PlayBackByTime(pRvu->lRvuHandle, iChannel - 1, &(RecFileInfo.starttime), &(RecFileInfo.endtime), hWnd,
		PlayCallBack, (DWORD)&pRemote->cStr);
	if (lPlayFileHandle == DFJK_INVALID_HANDLE)
	{
		wsprintf(g_szLastError, _T("IDVR_RemotePlayByTime: 播放文件失败error code=%d"), CLIENT_GetLastError());
		return FALSE;
	}
	pRemote->lPlayHandle = lPlayFileHandle;
	pRemote->flag = TRUE;
	*hPlay = (HANDLE)pRemote;
	return TRUE;
}

extern "C" BOOL CALL_TYPE IDVR_RemotePlay(HANDLE hDevHandle, LPCSTR lpszSrcFileName, HWND hWnd, HANDLE* hPlay)
{
	CRvu* pRvu = (CRvu*)hDevHandle;
	if (pRvu == NULL)
	{
		wsprintf(g_szLastError, _T("IDVR_RemotePlay error: Invalid Dev Handle"));
		return FALSE;
	}
	if (lpszSrcFileName == NULL)
	{
		wsprintf(g_szLastError, _T("IDVR_RemotePlay error: Invalid param!"));
		return FALSE;
	}
	NET_RECORDFILE_INFO RecFileInfo;
	CString str;
	str.Format(_T("%s"), lpszSrcFileName);
	CStringArray strArray;
	CString strTemp;
	strArray.RemoveAll();

	while (str.Find(_T("_")) != -1)
	{
		strTemp = str.Left(str.Find(_T("_")));
		str = str.Right(str.GetLength() - str.Find(_T("_")) - 1);
		strArray.Add(strTemp);
	}
	if (str.GetLength()>0)
	{
		strArray.Add(str);
	}
	RecFileInfo.ch = _wtoi(strArray[0]);
	RecFileInfo.size = _wtoi(strArray[1]);
	RecFileInfo.starttime.dwYear = _wtoi(strArray[2].Mid(0, 4));
	RecFileInfo.starttime.dwMonth = _wtoi(strArray[2].Mid(4, 2));
	RecFileInfo.starttime.dwDay = _wtoi(strArray[2].Mid(6, 2));
	RecFileInfo.starttime.dwHour = _wtoi(strArray[2].Mid(8, 2));
	RecFileInfo.starttime.dwMinute = _wtoi(strArray[2].Mid(10, 2));
	RecFileInfo.starttime.dwSecond = _wtoi(strArray[2].Mid(12, 2));
	RecFileInfo.endtime.dwYear = _wtoi(strArray[3].Mid(0, 4));
	RecFileInfo.endtime.dwMonth = _wtoi(strArray[3].Mid(4, 2));
	RecFileInfo.endtime.dwDay = _wtoi(strArray[3].Mid(6, 2));
	RecFileInfo.endtime.dwHour = _wtoi(strArray[3].Mid(8, 2));
	RecFileInfo.endtime.dwMinute = _wtoi(strArray[3].Mid(10, 2));
	RecFileInfo.endtime.dwSecond = _wtoi(strArray[3].Mid(12, 2));
	RecFileInfo.nRecordFileType = _wtoi(strArray[4]);
	RecFileInfo.driveno = _wtoi(strArray[5]);
	RecFileInfo.startcluster = _wtoi(strArray[6]);

	CRecPlay *pRemote = new CRecPlay();
	LONG lPlayFileHandle = CLIENT_PlayBackByRecordFile(pRvu->lRvuHandle, &RecFileInfo, hWnd, PlayCallBack, (DWORD)&pRemote->cStr);
	if (lPlayFileHandle == DFJK_INVALID_HANDLE)
	{
		wsprintf(g_szLastError, _T("IDVR_RemotePlay: 播放文件失败"));
		return FALSE;
	}
	pRemote->lPlayHandle = lPlayFileHandle;
	*hPlay = (HANDLE)pRemote;
	return TRUE;
}


extern "C" BOOL CALL_TYPE IDVR_StopRemotePlay(HANDLE hPlay)
{
	CRecPlay *pRemote = (CRecPlay*)hPlay;
	if (pRemote == NULL)
	{
		wsprintf(g_szLastError, _T("IDVR_StopRemotePlay error: Invalid Play Handle"));
		return FALSE;
	}
	CLIENT_StopPlayBack(pRemote->lPlayHandle);
	pRemote->lPlayHandle = DFJK_INVALID_HANDLE;
	delete pRemote;
	return TRUE;
}

extern "C" BOOL CALL_TYPE IDVR_PauseRemotePlay(HANDLE hPlay)
{
	CRecPlay *pRemote = (CRecPlay*)hPlay;
	if (pRemote == NULL)
	{
		wsprintf(g_szLastError, _T("IDVR_PauseRemotePlay error: Invalid Play Handle"));
		return FALSE;
	}
	CLIENT_PausePlayBack(pRemote->lPlayHandle, TRUE);
	return TRUE;
}

extern "C" BOOL CALL_TYPE IDVR_ResumeRemotePlay(HANDLE hPlay)
{
	CRecPlay *pRemote = (CRecPlay*)hPlay;
	if (pRemote == NULL)
	{
		wsprintf(g_szLastError, _T("IDVR_ResumeRemotePlay error: Invalid Play Handle"));
		return FALSE;
	}
	CLIENT_PausePlayBack(pRemote->lPlayHandle, FALSE);
	return TRUE;
}

extern "C" BOOL CALL_TYPE IDVR_StepRemotePlay(HANDLE hPlay)
{
	CRecPlay *pRemote = (CRecPlay*)hPlay;
	if (pRemote == NULL)
	{
		wsprintf(g_szLastError, _T("IDVR_StepRemotePlay error: Invalid Play Handle"));
		return FALSE;
	}
	CLIENT_StepPlayBack(pRemote->lPlayHandle, FALSE);
	return TRUE;
}

extern "C" BOOL CALL_TYPE IDVR_FastRemotePlay(HANDLE hPlay)
{
	CRecPlay *pRemote = (CRecPlay*)hPlay;
	if (pRemote == NULL)
	{
		wsprintf(g_szLastError, _T("IDVR_FastRemotePlay error: Invalid Play Handle"));
		return FALSE;
	}
	CLIENT_FastPlayBack(pRemote->lPlayHandle);
	return TRUE;
}

extern "C" BOOL CALL_TYPE IDVR_SlowRemotePlay(HANDLE hPlay)
{
	CRecPlay *pRemote = (CRecPlay*)hPlay;
	if (pRemote == NULL)
	{
		wsprintf(g_szLastError, _T("IDVR_SlowRemotePlay error: Invalid Play Handle"));
		return FALSE;
	}
	CLIENT_SlowPlayBack(pRemote->lPlayHandle);
	return TRUE;
}

extern "C" BOOL CALL_TYPE IDVR_NormalRemotePlay(HANDLE hPlay)
{
	CRecPlay *pRemote = (CRecPlay*)hPlay;
	if (pRemote == NULL)
	{
		wsprintf(g_szLastError, _T("IDVR_NormalRemotePlay error: Invalid Play Handle"));
		return FALSE;
	}
	CLIENT_NormalPlayBack(pRemote->lPlayHandle);
	return TRUE;
}

extern "C" BOOL CALL_TYPE IDVR_GetRemotePlayPos(HANDLE hPlay, float *pfPos)
{
	CRecPlay *pRemote = (CRecPlay*)hPlay;
	if (pRemote == NULL)
	{
		wsprintf(g_szLastError, _T("IDVR_GetRemotePlayPos error: Invalid Play Handle"));
		return FALSE;
	}
	CString strTemp = CString(pRemote->cStr);
	LONG lPlayedSize = _wtol(strTemp.Left(strTemp.Find(_T("_"))));
	LONG lTotalSize = _wtoi(strTemp.Right(strTemp.GetLength() - strTemp.Find(_T("_")) - 1));
	*pfPos = (float)lPlayedSize / (float)lTotalSize;
	*pfPos *= 1.0000f;
	return TRUE;
}

extern "C" BOOL CALL_TYPE IDVR_SetRemotePlayPos(HANDLE hPlay, float fPos)
{
	CRecPlay *pRemote = (CRecPlay*)hPlay;
	if (pRemote == NULL)
	{
		wsprintf(g_szLastError, _T("IDVR_SetRemotePlayPos error: Invalid Play Handle"));
		return FALSE;
	}
	CString strTemp = CString(pRemote->cStr);
	LONG totalsize = _wtol(strTemp.Right(strTemp.GetLength() - strTemp.Find(_T("_")) - 1));
	if (pRemote->flag)
	{
		CLIENT_SeekPlayBack(pRemote->lPlayHandle, (unsigned int)(fPos*totalsize), 0xffffffff);
		return TRUE;
	}
	else
	{
		CLIENT_SeekPlayBack(pRemote->lPlayHandle, 0xffffffff, (unsigned int)(fPos*totalsize));
		return TRUE;
	}
}

extern "C" BOOL CALL_TYPE IDVR_RemotePlayCapturePicture(HANDLE hPlay, LPCSTR lpszFileName, LPCSTR pExParamIn, DWORD dwExParamInLen)
{
	return FALSE;
}

extern "C" BOOL CALL_TYPE IDVR_RemotePlaySaveData(HANDLE hPlay, LPCSTR lpszRecFileName, LPCSTR pExParamIn, DWORD dwExParamInLen)
{
	return FALSE;
}

extern "C" BOOL CALL_TYPE IDVR_StopRemotePlaySaveData(HANDLE hPlay)
{
	return FALSE;
}

extern "C" BOOL CALL_TYPE IDVR_SetRemotePlayDataCallBack(HANDLE hPlay, DWORD dwCookie, LPCSTR pExParamIn, DWORD dwExParamInLen, RealDataCallBack callbackHandler)
{
	return FALSE;
}

//extern "C" BOOL CALL_TYPE ISYS_CustomFunction(LPCSTR pParamIn, DWORD dwParamInLen, LPTSTR *ppParamOut, DWORD* pdwParamOutLen)
//{
//	return FALSE;
//}
