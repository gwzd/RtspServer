
//
//	Global.cpp --- 全局函数定义, 变量定义
//
#include "StdAfx.h"
#include "Global.h"


//导入设备SDK lib
#pragma comment(lib, "PlayCtrl.lib")

//-------------------------------------------------------------------------------------
//        全局变量
//-------------------------------------------------------------------------------------

TCHAR g_szLastError[DFJK_MAXLEN_LASTERROR] = { 0 };

TCHAR g_szDLLPath[MAX_PATH] = { 0 };

//-------------------------------------------------------------------------------------
//        全局函数
//-------------------------------------------------------------------------------------

//从扩展参数获取Tag值
BOOL gF_GetTagValFromExParam(LPCTSTR lpszExParam, LPCTSTR lpszTagName, CString& strVal)
{
	strVal = "";
	CString strUrl = lpszExParam;
	CString strName = lpszTagName;
	if (strUrl.IsEmpty() || strName.IsEmpty())
		return FALSE;

	int nPos1 = strUrl.Find(lpszTagName, 0);
	if (nPos1 == -1)
		return FALSE;

	int nPos2 = strUrl.Find(_T(";"), nPos1);
	if (nPos2 == -1)
		return FALSE;

	CString strSub = strUrl.Mid(nPos1, nPos2-nPos1);
	if (strSub.IsEmpty())
		return FALSE;

	strVal = strSub.Mid(strName.GetLength()+1);

	return TRUE;
}
