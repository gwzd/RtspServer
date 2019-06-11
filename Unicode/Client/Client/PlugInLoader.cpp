// CPlugInLoader.cpp: implementation of the CPlugInLoader class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "PlugInLoader.h"
#include "Utilities.h"
#include "EnvConfig.h"
#include "Shlwapi.h"


#pragma comment(lib,"shlwapi")

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//CPluginInfo body
FARPROC Jans_GetProcAddress(HMODULE hModule, LPCSTR lpszAddress)
{
	return GetProcAddress(hModule, lpszAddress);
}

CPlugInLoader* CPlugInLoader::m_pInstance = NULL;
CPluginInfo::CPluginInfo()
{
	hModule = NULL;
	InterfaceVersion = 1;
	//MSG(11);
}

CPluginInfo::~CPluginInfo()
{
}

FUNC_PTR_LIST* CPluginInfo::GetFunctionPtr()
{
	return &m_func_ptr_list;
}

//CPlugInLoader body
CPlugInLoader::CPlugInLoader()
{
	memset(PluginError, 0, 1024);
	m_plugin_home = CEnvConfig::Instance()->GetHomeDir();
	m_plugin_home = m_plugin_home + PLUGIN_DIR;
}

CPlugInLoader::~CPlugInLoader()
{
	FreePlugIn();
}

void CPlugInLoader::LoadAllPlugIn()
{
	WIN32_FIND_DATA fData;
	//查找一级目录	
	HANDLE hFind = FindFirstFile(m_plugin_home + _T("*.*"), &fData);
	if (hFind == INVALID_HANDLE_VALUE)		return;

	BOOL bRet = hFind != INVALID_HANDLE_VALUE;
	while (bRet)
	{
		if (FILE_ATTRIBUTE_DIRECTORY & fData.dwFileAttributes)
		{

			if (fData.cFileName[0] == '.' && (fData.cFileName[1] == '\0' || (fData.cFileName[1] == '.' && fData.cFileName[2] == '\0')))
			{
				bRet = ::FindNextFile(hFind, (LPWIN32_FIND_DATA)&fData);
				continue;
			}

			CString strKeyName(fData.cFileName);
			CString strPlugin_path(m_plugin_home + fData.cFileName + _T("\\") + PLUGIN_MAIN_DLL);

			if (PathFileExists(strPlugin_path))
				LoadPlugIn(strPlugin_path, strKeyName);

			//查找二级目录
			WIN32_FIND_DATA fSubData;
			HANDLE hSubFind = FindFirstFile(m_plugin_home + fData.cFileName + _T("\\*.*"), &fSubData);
			BOOL bSubRet = hSubFind != INVALID_HANDLE_VALUE;
			while (bSubRet)
			{
				if (FILE_ATTRIBUTE_DIRECTORY & fSubData.dwFileAttributes)
				{
					if (fSubData.cFileName[0] == '.' && (fSubData.cFileName[1] == '\0' || (fSubData.cFileName[1] == '.' && fSubData.cFileName[2] == '\0')))
					{
						bSubRet = ::FindNextFile(hSubFind, (LPWIN32_FIND_DATA)&fSubData);
						continue;
					}

					CString strSubKeyName(fSubData.cFileName);
					CString strSubPluginPath(m_plugin_home + fData.cFileName + _T("\\") + fSubData.cFileName + _T("\\") + PLUGIN_MAIN_DLL);
					//printf("\n加载子目录[%s]文件%s \n",fData.cFileName,strSubPluginPath.c_str());
					if (PathFileExists(strSubPluginPath))
						LoadPlugIn(strSubPluginPath, strKeyName, strSubKeyName);
				}
				bSubRet = ::FindNextFile(hSubFind, (LPWIN32_FIND_DATA)&fSubData);
			}
			FindClose(hSubFind);
		}
		bRet = ::FindNextFile(hFind, (LPWIN32_FIND_DATA)&fData);
	}
	FindClose(hFind);
}

BOOL CPlugInLoader::DelayLoadPlugIn(LPCTSTR lpszType, LPCTSTR lpszSubType)
{
	BOOL bLoaded = FALSE;

	//查找一级目录	
	CString strPlugin_path;
	if (lpszSubType != NULL&&wcslen(lpszSubType)>1)
			strPlugin_path = m_plugin_home + lpszType + _T("\\") + lpszSubType + _T("\\") + PLUGIN_MAIN_DLL;
	else	strPlugin_path = m_plugin_home + lpszType + _T("\\") + PLUGIN_MAIN_DLL;

	if (PathFileExists(strPlugin_path))
		return LoadPlugIn(strPlugin_path, lpszType, lpszSubType);

	wsprintf(PluginError, _T("%s PathFileExists failed"), strPlugin_path);
	GSetError(PluginError);
	return FALSE;
}

BOOL CPlugInLoader::LoadPlugIn(LPCTSTR lpszPath, LPCTSTR lpszType, LPCTSTR lpszSubType)
{
	HMODULE hModule = LoadLibraryEx(lpszPath, NULL, LOAD_WITH_ALTERED_SEARCH_PATH);
	//	HMODULE hModule =  LoadLibraryEx("C:\\DF1500\\PlugIns\\DFHK\\PLAY\\PlugIn_Main.dll",NULL,LOAD_WITH_ALTERED_SEARCH_PATH);	
	//	HMODULE hModule =  LoadLibrary(lpszPath);	
	if (hModule)
	{
		CPluginInfo* pPlug_in = new CPluginInfo();

		pPlug_in->hModule = hModule;
		wcsncpy(pPlug_in->ModulePath, lpszPath, _MAX_PATH);
		wcsncpy(pPlug_in->szType, lpszType, 128);
		if (lpszSubType != NULL)
				wcsncpy(pPlug_in->szSubType, lpszSubType, 128);
		else	pPlug_in->szSubType[0] = '\0';

		InitFunctionList(*pPlug_in);
		pPlug_in->Initialize(NULL, NULL);
		m_plugins.push_back(pPlug_in);
		return TRUE;
	}

	wsprintf(PluginError, _T("\n LoadLibraryEx  %s failed!,错误代码: %d \n"), lpszPath, GetLastError());
	GSetError(PluginError);
	GLogError(PluginError);
	return FALSE;
}

BOOL CPlugInLoader::InitFunctionList(CPluginInfo& plugin)
{
	memset(plugin.GetFunctionPtr(), 0, sizeof(FUNC_PTR_LIST));

	if (plugin.hModule)
	{
		GET_FUNC_ADDR(plugin, IDVR_ConnectDevice)
		GET_FUNC_ADDR(plugin, IDVR_DisConnectDevice)
		GET_FUNC_ADDR(plugin, IDVR_StartPreview)
//		GET_FUNC_ADDR(plugin,IDVR_SelectCamera)
		GET_FUNC_ADDR(plugin, IDVR_StopPreview)
		GET_FUNC_ADDR(plugin, IDVR_CapturePicture)
		GET_FUNC_ADDR(plugin, IDVR_StartRecord)
		GET_FUNC_ADDR(plugin, IDVR_GetRecordState)
		GET_FUNC_ADDR(plugin, IDVR_StopRecord)
		GET_FUNC_ADDR(plugin, IDVR_SetVideoParams)
		GET_FUNC_ADDR(plugin, IDVR_GetVideoParams)
		GET_FUNC_ADDR(plugin, IDVR_StartSound)
		GET_FUNC_ADDR(plugin, IDVR_SetVolume)
		GET_FUNC_ADDR(plugin, IDVR_StopSound)
		GET_FUNC_ADDR(plugin, IDVR_StartAudioPhone)
		GET_FUNC_ADDR(plugin, IDVR_StopAudioPhone)
		GET_FUNC_ADDR(plugin, IDVR_PTZControl)
		GET_FUNC_ADDR(plugin, IDVR_SetIOStatus)
		GET_FUNC_ADDR(plugin, IDVR_GetIOStatus)
		GET_FUNC_ADDR(plugin, IDVR_SetIOValue)
		GET_FUNC_ADDR(plugin, IDVR_GetIOValue)
		GET_FUNC_ADDR(plugin, IDVR_SetAlarmSubscribe)
		GET_FUNC_ADDR(plugin, IDVR_SetAlarmUnsubscribe)
//		GET_FUNC_ADDR(plugin,IDVR_StartAlarmListen)
//		GET_FUNC_ADDR(plugin,IDVR_StopAlarmListen)
		GET_FUNC_ADDR(plugin, IDVR_GetDeviceInfo)
		GET_FUNC_ADDR(plugin, IDVR_GetChannelInfo)
		GET_FUNC_ADDR(plugin, ISYS_GetLastError)
//		GET_FUNC_ADDR(plugin,IPLY_QueryDevRecord)
//		GET_FUNC_ADDR(plugin,IPLY_DownloadFile)
		GET_FUNC_ADDR(plugin, IPLY_CloseFile)
		GET_FUNC_ADDR(plugin, IPLY_Play)
		GET_FUNC_ADDR(plugin, IPLY_Stop)
		GET_FUNC_ADDR(plugin, IPLY_CapturePicture)
		GET_FUNC_ADDR(plugin, IPLY_RefreshPlay)
		GET_FUNC_ADDR(plugin, IPLY_Pause)
		GET_FUNC_ADDR(plugin, IPLY_Fast)
		GET_FUNC_ADDR(plugin, IPLY_Slow)
		GET_FUNC_ADDR(plugin, IPLY_SetPlayPos)
		GET_FUNC_ADDR(plugin, IPLY_GetPlayPos)
		GET_FUNC_ADDR(plugin, IPLY_SetVolume)
		GET_FUNC_ADDR(plugin, IPLY_StartSound)
		GET_FUNC_ADDR(plugin, IPLY_StopSound)
		GET_FUNC_ADDR(plugin, IPLY_Step)
		GET_FUNC_ADDR(plugin, IPLY_SetVideoParams)
		GET_FUNC_ADDR(plugin, IPLY_GetVideoParams)
		GET_FUNC_ADDR(plugin, IPLY_GetTotalTime)
		GET_FUNC_ADDR(plugin, IPLY_GetPlayedTime)
		GET_FUNC_ADDR(plugin, IPLY_GetTotalFrames)
		GET_FUNC_ADDR(plugin, IPLY_GetPlayedFrames)

		//后添加
		GET_FUNC_ADDR(plugin, IDVR_StartDeviceRecord)
		GET_FUNC_ADDR(plugin, IDVR_StopDeviceRecord)
		GET_FUNC_ADDR(plugin, IDVR_GetDeviceRecordStatus)
//		GET_FUNC_ADDR(plugin,IPLY_StopDownloadFile)
//		GET_FUNC_ADDR(plugin,IPLY_CancelQueryDevRecord)
//		GET_FUNC_ADDR(plugin,IPLY_GetDownloadPos)

		GET_FUNC_ADDR(plugin, IPLY_OpenFile)

		//模块支持
		GET_FUNC_ADDR(plugin, ISYS_GetPluginInfo)
		GET_FUNC_ADDR(plugin, ISYS_Initialize)
		GET_FUNC_ADDR(plugin, ISYS_Uninitialize)
		/////09-01-14 fjw

		GET_FUNC_ADDR(plugin, IDVR_ShowDeviceSettingDlg)

		////////////teng
		GET_FUNC_ADDR(plugin, IDVR_StartRealData)
		GET_FUNC_ADDR(plugin, IDVR_StopRealData)
		GET_FUNC_ADDR(plugin, IPLY_OpenPlayStream)
		GET_FUNC_ADDR(plugin, IPLY_InputStreamData)
		GET_FUNC_ADDR(plugin, IPLY_StreamPause)
		GET_FUNC_ADDR(plugin, IPLY_ResetStreamBuffer)
		GET_FUNC_ADDR(plugin, IPLY_StreamPlay)
		GET_FUNC_ADDR(plugin, IPLY_StreamStop)
		GET_FUNC_ADDR(plugin, IPLY_CloseStream)
		/////////////teng

		GET_FUNC_ADDR(plugin, IPLY_SetDisplayRegion)
		GET_FUNC_ADDR(plugin, IPLY_RefreshPlayEx)
		GET_FUNC_ADDR(plugin, IPLY_RegisterDrawFun)

		printf("\n InitFunctionList...%d functions exported", plugin.GetFunctionPtr()->ExportNums);
	}
	return plugin.GetFunctionPtr()->ExportNums>0;
}

void CPlugInLoader::FreePlugIn()
{
	for (PlugInListIter it = m_plugins.begin(); it != m_plugins.end(); it++)
	{
		(*it)->Uninitialize();
		FreeLibrary((*it)->hModule);
		delete (*it);
	}
	m_plugins.clear();
}

CPluginInfo* CPlugInLoader::GetPlugIn(LPCTSTR lpszType, LPCTSTR lpszSubType)
{
	for (PlugInListIter it = m_plugins.begin(); it != m_plugins.end(); it++)
	{
		if (wcsicmp((*it)->szType, lpszType) == 0 &&
			wcsicmp((*it)->szSubType, lpszSubType) == 0)
		{
			return *it;
		}
	}

	if (DelayLoadPlugIn(lpszType, lpszSubType))
		return m_plugins.back();

	return NULL;
}


