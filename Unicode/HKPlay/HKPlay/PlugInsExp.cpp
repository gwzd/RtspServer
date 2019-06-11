#include "stdafx.h"
#include "PlugIns.h"
#include "Global.h"
#include "CommonFunc.h"

//////////////////////////////////////////////////////////////////////////
//实时音视频流的回调函数

/*
static BYTE g_pbHead[] = { 0x34,0x48,0x4b,0x48,0xfe,0xb3,0xd0,0xd6,0x08,0x03,0x04,0x20,0x00,0x00,0x00,0x00,
0x01,0x10,0x01,0x10,0x01,0x10,0x10,0x00,0x80,0x3e,0x00,0x00,0x60,0x01,0x20,0x01,
0x11,0x10,0x00,0x00,0x0e,0x00,0x00,0x00};
*/
// static BYTE g_pbHead[] = { 0x34,0x48,0x4b,0x48,0xfe,0xb3,0xd0,0xd6,0x08,0x03,0x04,0x20,0x00,0x00,0x00,0x00,
// 0x03,0x10,0x01,0x10,0x01,0x10,0x10,0x00,0x80,0x3e,0x00,0x00,0xc0,0x02,0x40,0x02,
// 0x11,0x10,0x00,0x00,0x05,0x00,0x00,0x00};
//////////////////////////////////////////////////////////////////////////
//1、	系统类函数	5
extern "C" BOOL CALL_TYPE ISYS_GetLastError(LPTSTR lpszErrorMsg, UINT nMsgLen)
{
	if(lpszErrorMsg)
	{
		wcsncpy(lpszErrorMsg, g_szLastError, nMsgLen-1);
		lpszErrorMsg[nMsgLen-1] = 0;
		return TRUE;
	}
	return FALSE;
}

extern "C" BOOL CALL_TYPE ISYS_GetInvalidHandleValue(HANDLE *phHandle)
{
	*phHandle = (HANDLE)NULL;
	return TRUE;
}

extern "C" BOOL CALL_TYPE ISYS_GetPluginInfo (PPLUGIN_INFO pInfo)
{
	if (pInfo == NULL)
	{
		wsprintf(g_szLastError, _T("ISYS_GetPluginInfo: 参数非法"));
		return FALSE;
	}
	strcpy(pInfo->DeviceTypeName, "DFHK");
	strcpy(pInfo->smanufactuer, "DFE");
	strcpy(pInfo->ssoftversion, "2.0.0.1");
	strcpy(pInfo->description, "DFHK回放插件");
	return TRUE;
}

extern "C" BOOL CALL_TYPE ISYS_Initialize(void *pReserved, DWORD dwReserved)
{
	return TRUE;
}

extern "C" void CALL_TYPE ISYS_Uninitialize()
{

}

extern "C" BOOL DFJK_API CALL_TYPE ISYS_FreeMemory(void* pMem)
{
	if (pMem)
	{
		delete []pMem;
		pMem = NULL;
		return TRUE;
	}
	return FALSE;
}

//////////////////////////////////////////////////////////////////////////

//打开本地录像文件 文件名 文件打开句柄指针
extern "C" BOOL CALL_TYPE IPLY_OpenFile(LPCSTR lpszFileName, HANDLE *phFileHandle)
{
	*phFileHandle = NULL;

	if (lpszFileName == NULL)
	{
		wsprintf(g_szLastError, _T("IPLY_OpenFile error: 无效参数"));
		return FALSE;
	}

	long m_lport = DFJK_INVALID_HANDLE;
	if(!(PlayM4_GetPort)(&m_lport))
	{
		wsprintf(g_szLastError, _T("IPLY_OpenFile failed: Get Port failed"));
		return FALSE;
	}

	if (!PlayM4_OpenFile(m_lport, (char*)lpszFileName))
	{
		wsprintf(g_szLastError, _T("IPLY_OpenFile: 打开文件失败"));
		return FALSE;
	}
	CRecPlay* pPlay = new CRecPlay();
	pPlay->lPlayHandle = m_lport;


	if (PlayM4_GetPictureSize(m_lport, &pPlay->lWidth, &pPlay->lHeight))
	{
		// if video format is HCIF, then double the height
		if( (pPlay->lWidth == CIF_WIDTH*2) && (pPlay->lHeight <= CIF_HEIGHT_PAL) )
			pPlay->lHeight *= 2;
	}

	if (pPlay->lWidth == 0)
		pPlay->lWidth = 704;
	if (pPlay->lHeight == 0)
		pPlay->lHeight = 576;

	*phFileHandle = (HANDLE)pPlay;

	return TRUE;
}
//关闭文件 文件句柄
extern "C" BOOL CALL_TYPE IPLY_CloseFile(HANDLE hFileHandle)
{
	if (hFileHandle == NULL)
	{
		wsprintf(g_szLastError, _T("IPLY_CloseFile error: Invalid handle!"));
		return FALSE;
	}
	CRecPlay* pPlay = (CRecPlay*)hFileHandle;
	BOOL bRet = FALSE;

	bRet = PlayM4_CloseFile(pPlay->lPlayHandle);
	if (!bRet)
	{
		wsprintf(g_szLastError, _T("IPLY_CloseFile failed, err = %d"), PlayM4_GetLastError(pPlay->lPlayHandle));
	}
	PlayM4_FreePort(pPlay->lPlayHandle);

	delete pPlay;

	return bRet;
}
//播放
extern "C" BOOL CALL_TYPE IPLY_Play(HANDLE hFileHandle, HWND hWnd)
{
	if (hFileHandle == NULL)
	{
		wsprintf(g_szLastError, _T("IPLY_Play error: Invalid handle!"));
		return FALSE;
	}

	if (hWnd == NULL)
	{
		wsprintf(g_szLastError, _T("IPLY_Play error: Invalid Params!"));
		return FALSE;
	}

	CRecPlay* pPlay = (CRecPlay*)hFileHandle;

	PlayM4_ResetBuffer(pPlay->lPlayHandle, BUF_VIDEO_RENDER);
	PlayM4_ResetBuffer(pPlay->lPlayHandle, BUF_AUDIO_RENDER);
	PlayM4_ResetBuffer(pPlay->lPlayHandle, BUF_VIDEO_SRC);
	PlayM4_ResetBuffer(pPlay->lPlayHandle, BUF_AUDIO_SRC);

	BOOL bRet = FALSE;
	bRet = PlayM4_Play(pPlay->lPlayHandle, hWnd);
	if (!bRet)
	{
		wsprintf(g_szLastError, _T("IPLY_Play failed, err = %d"), PlayM4_GetLastError(pPlay->lPlayHandle));
	}

	//PlayM4_SetDisplayCallBack((LONG)hFileHandle, DisplayCBFun);

	return bRet;
}

extern "C" BOOL CALL_TYPE IPLY_Stop(HANDLE hFileHandle)
{
	if (hFileHandle == NULL)
	{
		wsprintf(g_szLastError, _T("IPLY_Stop error: Invalid handle!"));
		return FALSE;
	}
	
	CRecPlay* pPlay = (CRecPlay*)hFileHandle;
	BOOL bRet = FALSE;
	bRet = PlayM4_Stop(pPlay->lPlayHandle);
	if (!bRet)
	{
		wsprintf(g_szLastError, _T("IPLY_Stop failed, err = %d"), PlayM4_GetLastError(pPlay->lPlayHandle));
	}
	return bRet;
}

extern "C" BOOL CALL_TYPE IPLY_CapturePicture(HANDLE hFileHandle,LPCSTR lpszBmpFileName)
{
	if (hFileHandle == NULL)
	{
		wsprintf(g_szLastError, _T("IPLY_CapturePicture error: Invalid handle!"));
		return FALSE;
	}

	if (lpszBmpFileName == NULL)
	{
		wsprintf(g_szLastError, _T("IPLY_CapturePicture error: 无效参数"));
		return FALSE;
	}
	CString strFileName = CString(lpszBmpFileName);
	CRecPlay* pPlay = (CRecPlay*)hFileHandle;
	PlayM4_GetPictureSize(pPlay->lPlayHandle,&pPlay->lWidth,&pPlay->lHeight);

	DWORD   nBufSize;
	if (strFileName.Right(3).CompareNoCase(_T("jpg")) == 0)
	{
		nBufSize	= pPlay->lWidth * pPlay->lHeight * 3 / 2;
	}
	else if (strFileName.Right(3).CompareNoCase(_T("bmp")) == 0)
	{
		nBufSize	= sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + pPlay->lWidth * pPlay->lHeight * 5;
	}

	PBYTE	pImage		= NULL;
	DWORD   pImageSize	= 0;
	BOOL	bRet = FALSE;
	TRACE("%d",nBufSize);
	try
	{
		pImage = new BYTE[nBufSize];
		if(NULL == pImage)
		{
			throw 0;
		}
		BOOL bret = FALSE;
		if (strFileName.Right(3).CompareNoCase(_T("jpg")) == 0)
		{
			bret = PlayM4_GetJPEG(pPlay->lPlayHandle, pImage, nBufSize, &pImageSize);
		}
		else if (strFileName.Right(3).CompareNoCase(_T("bmp")) == 0)
		{
			bret = PlayM4_GetBMP(pPlay->lPlayHandle, pImage, nBufSize, &pImageSize);
		}
		if(bret)
		{
			CFile picfile;
			if(picfile.Open(strFileName, CFile::modeCreate|CFile::modeWrite))
			{
				picfile.Write(pImage, pImageSize);
				picfile.Close();
				bRet = TRUE;
			}
		}
	}
	catch (CFileException* e)
	{
		//e->ReportError();
		e->Delete();
	}
	catch(...)
	{
	}

	if(pImage != NULL)
	{
		delete []pImage;
		pImage = NULL;
	}
	if (!bRet)
	{
		wsprintf(g_szLastError, _T("IPLY_CapturePicture failed, err = %d"), PlayM4_GetLastError(pPlay->lPlayHandle));
	}

	return bRet;
}

extern "C" BOOL CALL_TYPE IPLY_RefreshPlay(HANDLE hFileHandle)
{
	if (hFileHandle == NULL)
	{
		wsprintf(g_szLastError, _T("IPLY_RefreshPlay error: Invalid handle!"));
		return FALSE;
	}

	CRecPlay* pPlay = (CRecPlay*)hFileHandle;
	BOOL bRet = FALSE;
	bRet = PlayM4_RefreshPlay(pPlay->lPlayHandle);
	if (!bRet)
	{
		wsprintf(g_szLastError, _T("IPLY_RefreshPlay failed, err = %d"), PlayM4_GetLastError(pPlay->lPlayHandle));
	}
	return bRet;
}

extern "C" BOOL CALL_TYPE IPLY_Pause(HANDLE hFileHandle,BOOL bPause)
{
	if (hFileHandle == NULL)
	{
		wsprintf(g_szLastError, _T("IPLY_Pause error: Invalid handle!"));
		return FALSE;
	}

	CRecPlay* pPlay = (CRecPlay*)hFileHandle;
	BOOL bRet = FALSE;
	bRet = PlayM4_Pause(pPlay->lPlayHandle, bPause);
	if (!bRet)
	{
		wsprintf(g_szLastError, _T("IPLY_Pause failed, err = %d"), PlayM4_GetLastError(pPlay->lPlayHandle));
	}
	return bRet;
}

extern "C" BOOL CALL_TYPE IPLY_Fast(HANDLE hFileHandle)
{
	if (hFileHandle == NULL)
	{
		wsprintf(g_szLastError, _T("IPLY_Fast error: Invalid handle!"));
		return FALSE;
	}

	CRecPlay* pPlay = (CRecPlay*)hFileHandle;
	BOOL bRet = FALSE;
	bRet = PlayM4_Fast(pPlay->lPlayHandle);
	if (!bRet)
	{
		wsprintf(g_szLastError, _T("IPLY_Fast failed, err = %d"), PlayM4_GetLastError(pPlay->lPlayHandle));
	}
	return bRet;
}

extern "C" BOOL CALL_TYPE IPLY_Slow(HANDLE hFileHandle)
{
	if (hFileHandle == NULL)
	{
		wsprintf(g_szLastError, _T("IPLY_Slow error: Invalid handle!"));
		return FALSE;
	}

	CRecPlay* pPlay = (CRecPlay*)hFileHandle;
	BOOL bRet = FALSE;
	bRet = PlayM4_Slow(pPlay->lPlayHandle);
	if (!bRet)
	{
		wsprintf(g_szLastError, _T("IPLY_Slow failed, err = %d"), PlayM4_GetLastError(pPlay->lPlayHandle));
	}
	return bRet;
}

extern "C" BOOL CALL_TYPE IPLY_FastBack(HANDLE hFileHandle)
{
	wsprintf(g_szLastError, _T("IPLY_FastBack error: 尚未实现!"));
	return FALSE;
}

extern "C" BOOL CALL_TYPE IPLY_SlowBack(HANDLE hFileHandle)
{
	wsprintf(g_szLastError, _T("IPLY_SlowBack error: 尚未实现!"));
	return FALSE;
}

extern "C" BOOL CALL_TYPE IPLY_Step(HANDLE hFileHandle)
{
	if (hFileHandle == NULL)
	{
		wsprintf(g_szLastError, _T("IPLY_Step error: Invalid handle!"));
		return FALSE;
	}

	CRecPlay* pPlay = (CRecPlay*)hFileHandle;
	BOOL bRet = FALSE;
	bRet = PlayM4_OneByOne(pPlay->lPlayHandle);
	if (!bRet)
	{
		wsprintf(g_szLastError, _T("IPLY_Step failed, err = %d"), PlayM4_GetLastError(pPlay->lPlayHandle));
	}
	return bRet;
}

extern "C" BOOL CALL_TYPE IPLY_StepBack(HANDLE hFileHandle)
{
	if (hFileHandle == NULL)
	{
		wsprintf(g_szLastError, _T("IPLY_StepBack error: Invalid handle!"));
		return FALSE;
	}

	CRecPlay* pPlay = (CRecPlay*)hFileHandle;
	BOOL bRet = FALSE;
	bRet = PlayM4_OneByOneBack(pPlay->lPlayHandle);
	if (!bRet)
	{
		wsprintf(g_szLastError, _T("IPLY_StepBack failed, err = %d"), PlayM4_GetLastError(pPlay->lPlayHandle));
	}
	return bRet;
}

extern "C" BOOL CALL_TYPE IPLY_SetPlayPos(HANDLE hFileHandle, float fRelaPos)
{
	if (hFileHandle == NULL)
	{
		wsprintf(g_szLastError, _T("IPLY_SetPlayPos error: Invalid handle!"));
		return FALSE;
	}

	CRecPlay* pPlay = (CRecPlay*)hFileHandle;
	BOOL bRet = FALSE;
	bRet = PlayM4_SetPlayPos(pPlay->lPlayHandle, fRelaPos);
	if (!bRet)
	{
		wsprintf(g_szLastError, _T("IPLY_SetPlayPos failed, err = %d"), PlayM4_GetLastError(pPlay->lPlayHandle));
	}
	return bRet;
}

extern "C" BOOL CALL_TYPE IPLY_GetPlayPos(HANDLE hFileHandle, float *pfRelaPos)
{
	*pfRelaPos = 0;
	if (hFileHandle == NULL)
	{
		wsprintf(g_szLastError, _T("IPLY_GetPlayPos error: Invalid handle!"));
		return FALSE;
	}

	CRecPlay* pPlay = (CRecPlay*)hFileHandle;
	float fCurPos = PlayM4_GetPlayPos(pPlay->lPlayHandle);
	*pfRelaPos = fCurPos;
	return TRUE;
}

extern "C" BOOL CALL_TYPE IPLY_GetTotalTime(HANDLE hFileHandle, DWORD *pdwFileTime)
{
	*pdwFileTime = 0;
	if (hFileHandle == NULL)
	{
		wsprintf(g_szLastError, _T("IPLY_GetTotalTime error: Invalid handle!"));
		return FALSE;
	}

	CRecPlay* pPlay = (CRecPlay*)hFileHandle;
	*pdwFileTime = PlayM4_GetFileTime(pPlay->lPlayHandle);
	return TRUE;
}

extern "C" BOOL CALL_TYPE IPLY_GetPlayedTime(HANDLE hFileHandle,DWORD *pdwPlayedTime)
{
	*pdwPlayedTime = 0;
	if (hFileHandle == NULL)
	{
		wsprintf(g_szLastError, _T("IPLY_GetPlayedTime error: Invalid handle!"));
		return FALSE;
	}

	CRecPlay* pPlay = (CRecPlay*)hFileHandle;
	*pdwPlayedTime = PlayM4_GetPlayedTime(pPlay->lPlayHandle);
	return TRUE;
}

extern "C" BOOL CALL_TYPE IPLY_GetTotalFrames(HANDLE hFileHandle, DWORD *pdwTotalFrames)
{
	*pdwTotalFrames = 0;
	if (hFileHandle == NULL)
	{
		wsprintf(g_szLastError, _T("IPLY_GetTotalFrames error: Invalid handle!"));
		return FALSE;
	}

	CRecPlay* pPlay = (CRecPlay*)hFileHandle;
	*pdwTotalFrames = PlayM4_GetFileTotalFrames(pPlay->lPlayHandle);
	return TRUE;
}

extern "C" BOOL CALL_TYPE IPLY_GetPlayedFrames(HANDLE hFileHandle, DWORD *pdwPlayedFrames)
{
	*pdwPlayedFrames = 0;
	if (hFileHandle == NULL)
	{
		wsprintf(g_szLastError, _T("IPLY_GetPlayedFrames error: Invalid handle!"));
		return FALSE;
	}

	CRecPlay* pPlay = (CRecPlay*)hFileHandle;
	*pdwPlayedFrames = PlayM4_GetPlayedFrames(pPlay->lPlayHandle);
	return TRUE;
}

extern "C" BOOL CALL_TYPE IPLY_StartSound(HANDLE hFileHandle)
{
	if (hFileHandle == NULL)
	{
		wsprintf(g_szLastError, _T("IPLY_StartSound error: Invalid handle!"));
		return FALSE;
	}

	CRecPlay* pPlay = (CRecPlay*)hFileHandle;
	BOOL bRet = FALSE;
	bRet = PlayM4_PlaySound(pPlay->lPlayHandle);
	if (!bRet)
	{
		wsprintf(g_szLastError, _T("IPLY_StartSound failed, err = %d"), PlayM4_GetLastError(pPlay->lPlayHandle));
	}
	return bRet;
}

extern "C" BOOL CALL_TYPE IPLY_StopSound(HANDLE hFileHandle)
{
// 	if (hFileHandle == NULL)
// 	{
// 		wsprintf(g_szLastError, "IPLY_StopSound error: Invalid handle!");
// 		return FALSE;
// 	}
// 
// 	CRecPlay* pPlay = (CRecPlay*)hFileHandle;
	BOOL bRet = FALSE;
	bRet = PlayM4_StopSound();
	if (!bRet)
	{
		wsprintf(g_szLastError, _T("IPLY_StopSound failed"));
	}
	return bRet;
}

extern "C" BOOL CALL_TYPE IPLY_SetVolume(HANDLE hFileHandle, DWORD dwVolume)
{
	if (hFileHandle == NULL)
	{
		wsprintf(g_szLastError, _T("IPLY_SetVolume error: Invalid handle!"));
		return FALSE;
	}

	CRecPlay* pPlay = (CRecPlay*)hFileHandle;
	BOOL bRet = FALSE;
	if (dwVolume < 0)
		dwVolume = 0;
	if (dwVolume > 100)
		dwVolume = 100;
	
	WORD wVol = (WORD)( dwVolume * 0xFFFF / 100 );	//SDK范围：0～0xFFFF, 本接口要求0～100，需要转换

	bRet = PlayM4_SetVolume(pPlay->lPlayHandle, wVol);
	if (!bRet)
	{
		wsprintf(g_szLastError, _T("IPLY_SetVolume failed, err = %d"), PlayM4_GetLastError(pPlay->lPlayHandle));
	}

	return bRet;
}

extern "C" BOOL CALL_TYPE IPLY_SetVideoParams(HANDLE hFileHandle,int iBrightness,int iContrast, int iSaturation, int iHue, BOOL bDefault)
{
	if (hFileHandle == NULL)
	{
		wsprintf(g_szLastError, _T("IPLY_SetVideoParams error: Invalid handle!"));
		return FALSE;
	}

	CRecPlay* pPlay = (CRecPlay*)hFileHandle;
	BOOL bRet = FALSE;

	long lPlayHandle = pPlay->lPlayHandle;
	if (pPlay->bIsRemotePlay)
	{
		lPlayHandle = pPlay->nPlayerIndex;
	}
TRACE("ply plug: IPLY_SetVideoParams: lPlayHandle = %d, PlayerIndex=%d\n", lPlayHandle, pPlay->nPlayerIndex);
	//本接口函数参数要求范围：0～255，SDK为0～128（默认64），需要转换
	int nB, nC, nS, nH;
	if (bDefault)
	{
		nB = 64;
		nC = 64;
		nS = 64;
		nH = 64;
	}
	else
	{
		nB = iBrightness * 128 / 255;
		nC = iContrast * 128 / 255;
		nS = iSaturation * 128 / 255;
		nH = iHue * 128 / 255;
	}

	bRet = PlayM4_SetColor(lPlayHandle, 0, nB, nC, nS, nH);
	if (!bRet)
	{
		wsprintf(g_szLastError, _T("IPLY_SetVideoParams failed, err = %d"), PlayM4_GetLastError(lPlayHandle));
	}
	return bRet;
}

extern "C" BOOL CALL_TYPE IPLY_GetVideoParams(HANDLE hFileHandle, int *piBrightness, int *piContrast, int *piSaturation, int *piHue)
{
	if (hFileHandle == NULL)
	{
		wsprintf(g_szLastError, _T("IPLY_GetVideoParams error: Invalid handle!"));
		return FALSE;
	}

	CRecPlay* pPlay = (CRecPlay*)hFileHandle;
	BOOL bRet = FALSE;

	long lPlayHandle = pPlay->lPlayHandle;
	if (pPlay->bIsRemotePlay)
	{
		lPlayHandle = pPlay->nPlayerIndex;
	}

	//本接口函数参数要求范围：0～255，SDK为0～128（默认64），需要转换
	int nB, nC, nS, nH;
	bRet = PlayM4_GetColor(lPlayHandle, 0, &nB, &nC, &nS, &nH);
	if (!bRet)
	{
		wsprintf(g_szLastError, _T("IPLY_GetVideoParams failed, err = %d"), PlayM4_GetLastError(lPlayHandle));
		*piBrightness = 0;
		*piContrast = 0;
		*piSaturation = 0;
		*piHue = 0;
	}
	else
	{
		*piBrightness = nB * 255 / 128;
		*piContrast = nC * 255 / 128;
		*piSaturation = nS * 255 / 128;
		*piHue = nH * 255 / 128;
	}
	return bRet;
}

extern "C" BOOL CALL_TYPE IPLY_OpenPlayStream(BYTE* pStreamHeader,
								   DWORD dwHeaderLen,
								   DWORD dwStreamLen,
								   DWORD dwParam1,
								   HANDLE *phStreamHandle)
{
	*phStreamHandle = NULL;//打开的流句柄

	if (pStreamHeader == NULL || dwStreamLen == 0)//流数据头 流数据头长度 保留参数
	{
		wsprintf(g_szLastError, _T("IPLY_OpenPlayStream error: Invalid params!"));
		return FALSE;
	}

	long lport;
	if(!(PlayM4_GetPort)(&lport))
	{
		wsprintf(g_szLastError, _T("IPLY_OpenPlayStream failed, err = %d"), PlayM4_GetLastError(lport));
		return FALSE;
	}

	TRACE("ply plug: IPLY_OpenPlayStream: lport = %d\n", lport);

	PlayM4_SetStreamOpenMode(lport, STREAME_REALTIME);	
//	PlayM4_SetStreamOpenMode(lport, STREAME_FILE);	

	BOOL bRet = FALSE;
// 	if (pStreamHeader == NULL)
// 	{
// 		bRet = PlayM4_OpenStream(lport,g_pbHead,40, dwStreamLen);
// 	}
// 	else
//	{
		bRet = PlayM4_OpenStream(lport,pStreamHeader,dwHeaderLen,  dwStreamLen);
//	}

	if (!bRet)
	{
		wsprintf(g_szLastError, _T("IPLY_OpenPlayStream failed, err = %d"), PlayM4_GetLastError(lport));
		return FALSE;
	}

	CRecPlay * pStream = new CRecPlay();
	pStream->lPlayHandle = lport;

	if (PlayM4_GetPictureSize(lport, &pStream->lWidth, &pStream->lHeight))	//流方式下，此接口必须播放之后凋才有效！否则都返回352,288
	{
		// if video format is HCIF, then double the height
		if( (pStream->lWidth == CIF_WIDTH*2) && (pStream->lHeight <= CIF_HEIGHT_PAL) )
			pStream->lHeight *= 2;
	}

	if (pStream->lWidth == 0)
		pStream->lWidth = 704;
	if (pStream->lHeight == 0)
		pStream->lHeight = 576;

//	TRACE("Pic size = %d, %d\n", pStream->lWidth, pStream->lHeight);

	*phStreamHandle = pStream;

	return TRUE;
}

extern "C" BOOL CALL_TYPE IPLY_InputStreamData(HANDLE hStreamHandle,BYTE* pBuffer,DWORD dwStreamLen,DWORD* dwParam1,DWORD dwParam2)
{
	CRecPlay * pStream = (CRecPlay*)hStreamHandle;
	BOOL bRet = FALSE;
	if (pStream)
	{
		bRet = PlayM4_InputData(pStream->lPlayHandle, pBuffer, dwStreamLen);
		if (!bRet)
		{
			wsprintf(g_szLastError, _T("IPLY_InputStreamData failed, err = %d"), PlayM4_GetLastError(pStream->lPlayHandle));
		}
	}
	return bRet;
}

extern "C" BOOL CALL_TYPE IPLY_StreamPlay(HANDLE hStreamHandle,HWND hWnd)
{
	CRecPlay * pStream = (CRecPlay*)hStreamHandle;
	BOOL bRet = FALSE;
	if (pStream)
	{
		bRet = PlayM4_Play(pStream->lPlayHandle, hWnd);
		if(bRet)
		{												
			PlayM4_RefreshPlay(pStream->lPlayHandle);
		}
		else
		{
			wsprintf(g_szLastError, _T("IPLY_StreamPlay failed, err = %d"), PlayM4_GetLastError(pStream->lPlayHandle));
		}
	}
	return bRet;
}

extern "C" BOOL CALL_TYPE IPLY_StreamPause(HANDLE hStreamHandle,BOOL bPause)
{
	CRecPlay * pStream = (CRecPlay*)hStreamHandle;
	BOOL bRet = FALSE;
	if (pStream)
	{
		bRet = PlayM4_Pause(pStream->lPlayHandle, bPause);
		if (!bRet)
		{
			wsprintf(g_szLastError, _T("IPLY_StreamPause failed, err = %d"), PlayM4_GetLastError(pStream->lPlayHandle));
		}
	}
	return bRet;
}

extern "C" BOOL CALL_TYPE IPLY_ResetStreamBuffer(HANDLE hStreamHandle)
{
	return TRUE;
}

extern "C" BOOL CALL_TYPE IPLY_StreamRefreshPlay( HANDLE hStreamHandle)
{
	return TRUE;
}

extern "C" BOOL CALL_TYPE IPLY_StreamStop(HANDLE hStreamHandle)
{
	CRecPlay * pStream = (CRecPlay*)hStreamHandle;
	BOOL bRet = FALSE;
	if (pStream)
	{
		bRet = PlayM4_Stop(pStream->lPlayHandle);
		if (!bRet)
		{
			wsprintf(g_szLastError, _T("IPLY_StreamStop failed, err = %d"), PlayM4_GetLastError(pStream->lPlayHandle));
		}
	}
	return bRet;
}

extern "C" BOOL CALL_TYPE IPLY_CloseStream(HANDLE hStreamHandle)
{
	CRecPlay * pStream = (CRecPlay*)hStreamHandle;
	BOOL bRet = FALSE;
	if (pStream)
	{
		PlayM4_ResetBuffer(pStream->lPlayHandle, BUF_VIDEO_RENDER);
		PlayM4_ResetBuffer(pStream->lPlayHandle, BUF_AUDIO_RENDER);
		PlayM4_ResetBuffer(pStream->lPlayHandle, BUF_VIDEO_SRC);
       	PlayM4_ResetBuffer(pStream->lPlayHandle, BUF_AUDIO_SRC);

		bRet = PlayM4_CloseStream(pStream->lPlayHandle);
		if (!bRet)
		{
			wsprintf(g_szLastError, _T("IPLY_CloseStream failed, err = %d"), PlayM4_GetLastError(pStream->lPlayHandle));
		}

		PlayM4_FreePort(pStream->lPlayHandle);
		delete pStream;
	}
	return bRet;
}

//extern "C" BOOL CALL_TYPE ISYS_CustomFunction(LPCTSTR pParamIn, DWORD dwParamInLen, LPTSTR *ppParamOut, DWORD* pdwParamOutLen)
//{
//	return FALSE;
//}

//电子放大
extern "C" BOOL CALL_TYPE IPLY_SetDisplayRegion(HANDLE hPlayHandle,DWORD nRegionNum, RECT *pSrcRect, HWND hDestWnd, BOOL bEnable)
{
	if (hPlayHandle == NULL)
	{
		wsprintf(g_szLastError, _T("IPLY_SetDisplayRegion error: Invalid handle!"));
		return FALSE;
	}

	CRecPlay * pPlay = (CRecPlay*)hPlayHandle;
	BOOL bRet = FALSE;
	if (pPlay)
	{
		if (pSrcRect)
		{
			long lW,lH;
			PlayM4_GetPictureSize(pPlay->lPlayHandle,&lW,&lH);	//不能用pPlay->lWidth和lHeight，不准确
					//PlayM4_GetPictureSize必须播放之后调用才准确！

			CRect rc = pSrcRect;
			RECT rcLast;
			rc.NormalizeRect();
			rcLast.left = rc.left*lW/ 255;
			rcLast.right = rc.right*lW / 255;
			rcLast.top = rc.top*lH/ 255;
			rcLast.bottom = rc.bottom*lH / 255;

			bRet = PlayM4_SetDisplayRegion(pPlay->lPlayHandle, 0, &rcLast, hDestWnd, bEnable);
		}
		else
			bRet = PlayM4_SetDisplayRegion(pPlay->lPlayHandle, 0, pSrcRect, hDestWnd, bEnable);
	}
	if (!bRet)
	{
		wsprintf(g_szLastError, _T("IPLY_SetDisplayRegion failed, err = %d"), PlayM4_GetLastError(pPlay->lPlayHandle));
	}

	return bRet;
}

//刷新显示窗口（用IPLY_SetDisplayRegion刷新的窗口）
extern "C" BOOL CALL_TYPE IPLY_RefreshPlayEx(HANDLE hPlayHandle, DWORD nRegionNum)
{
	if (hPlayHandle == NULL)
	{
		wsprintf(g_szLastError, _T("IPLY_RefreshPlayEx error: Invalid handle!"));
		return FALSE;
	}

	CRecPlay * pPlay = (CRecPlay*)hPlayHandle;
	BOOL bRet = FALSE;
	if (pPlay)
	{
		bRet = PlayM4_RefreshPlayEx(pPlay->lPlayHandle, nRegionNum);
	}
	if (!bRet)
	{
		wsprintf(g_szLastError, _T("IPLY_RefreshPlayEx failed, err = %d"), PlayM4_GetLastError(pPlay->lPlayHandle));
	}

	return bRet;
}

//绘图回调函数
void CALLBACK DrawFun(long nPort,HDC hDc,LONG nUser)
{
	if(nUser != 0)
	{
		CRecPlay * pPlay = (CRecPlay*)nUser;
		if(pPlay->pfnDrawCallBack != NULL)
		{
			pPlay->pfnDrawCallBack( (HANDLE) pPlay, hDc, pPlay->pUserData);
		}
	}
}

//注册绘图回调函数
extern "C" BOOL CALL_TYPE IPLY_RegisterDrawFun(HANDLE hPlayHandle, DrawCallBack callbackDraw, void* pUserData)
{
	if (hPlayHandle == NULL)
	{
		wsprintf(g_szLastError, _T("IPLY_RegisterDrawFun error: Invalid handle!"));
		return FALSE;
	}

	CRecPlay * pPlay = (CRecPlay*)hPlayHandle;
	BOOL bRet = FALSE;
	if (pPlay)
	{
		pPlay->pUserData = pUserData;
		pPlay->pfnDrawCallBack = callbackDraw;
		bRet = PlayM4_RegisterDrawFun(pPlay->lPlayHandle, DrawFun, (LONG)pPlay);
	}
	if (!bRet)
	{
		wsprintf(g_szLastError, _T("IPLY_RegisterDrawFun failed, err = %d"), PlayM4_GetLastError(pPlay->lPlayHandle));
	}

	return bRet;
}
