// CPlugInLoader.cpp: implementation of the CPlugInLoader class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "PlugInLoader.h"
#include "Utilities.h"
#include "EnvConfig.h"
#include "Shlwapi.h"
#include "xlog.h"


#pragma comment(lib,"shlwapi")
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
//#define CONCAT(s1,s2) #s1 ## #s2
//#define MSG(n) TRACE(CONCAT(Message at line\x20,n));;

//CPluginInfo body
FARPROC Jans_GetProcAddress(HMODULE  hModule,LPCSTR lpszAddress)
{
	return GetProcAddress(hModule,lpszAddress);
}

CPlugInLoader* CPlugInLoader::m_pInstance=NULL;

CPluginInfo::CPluginInfo ()
{
	hModule = NULL;
	InterfaceVersion = 1;
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

	BOOL bRet = hFind!=INVALID_HANDLE_VALUE;
	while(bRet)
	{				
		if( FILE_ATTRIBUTE_DIRECTORY & fData.dwFileAttributes) 
		{ 
			if(fData.cFileName[0] == '.' && (fData.cFileName[1] == '\0' || (fData.cFileName[1] == '.' && fData.cFileName[2] == '\0')))
			{
				bRet = ::FindNextFile(hFind, (LPWIN32_FIND_DATA)&fData);
				continue;
			}

			CString strKeyName = fData.cFileName;
			CString strPlugin_path = m_plugin_home + fData.cFileName + _T("\\") + PLUGIN_MAIN_DLL;			
			
			if ( PathFileExists(strPlugin_path) )				
			{
				USES_CONVERSION;
				LoadPlugIn(T2A(strPlugin_path), T2A(strKeyName));
			}
			//查找二级目录
			WIN32_FIND_DATA fSubData;
			HANDLE hSubFind = FindFirstFile((m_plugin_home + fData.cFileName + _T("\\*.*")), &fSubData);
			BOOL bSubRet = hSubFind != INVALID_HANDLE_VALUE;
			while(bSubRet)
			{									
				if( FILE_ATTRIBUTE_DIRECTORY & fSubData.dwFileAttributes) 
				{
					if(fSubData.cFileName[0] == '.' && (fSubData.cFileName[1] == '\0' || (fSubData.cFileName[1] == '.' && fSubData.cFileName[2] == '\0')))
					{
						bSubRet = ::FindNextFile(hSubFind, (LPWIN32_FIND_DATA)&fSubData);	
						continue;
					}

					CString strSubKeyName = fSubData.cFileName;
					CString strSubPluginPath = m_plugin_home + fData.cFileName + _T("\\") + fSubData.cFileName + _T("\\") + PLUGIN_MAIN_DLL;
					//printf("\n加载子目录[%s]文件%s \n",fData.cFileName,strSubPluginPath.c_str());
					if ( PathFileExists(strSubPluginPath) )	{
						USES_CONVERSION;
						LoadPlugIn(T2A(strSubPluginPath),T2A(strKeyName),T2A(strSubKeyName));
					}
				}
				bSubRet = ::FindNextFile(hSubFind, (LPWIN32_FIND_DATA)&fSubData);	
			}
			FindClose(hSubFind);
		} 	
		bRet = ::FindNextFile(hFind, (LPWIN32_FIND_DATA)&fData);
	} 
	FindClose(hFind);
}

BOOL CPlugInLoader::DelayLoadPlugIn(LPCSTR lpszType,LPCSTR lpszSubType)
{
	BOOL bLoaded = FALSE;
	//查找一级目录	
	CString	strPlugin_path;
	USES_CONVERSION;
	if( lpszSubType != NULL && strlen(lpszSubType) > 1 )
			strPlugin_path = m_plugin_home + A2W(lpszType) + _T("\\") + A2W(lpszSubType) + _T("\\") + PLUGIN_MAIN_DLL;
	else	strPlugin_path = m_plugin_home + A2W(lpszType) + _T("\\") + PLUGIN_MAIN_DLL;

	if(PathFileExists(strPlugin_path))	{
		USES_CONVERSION;
		return LoadPlugIn(T2A(strPlugin_path),lpszType,lpszSubType);
	}

	return FALSE;
}
//加载插件
BOOL CPlugInLoader::LoadPlugIn(LPCSTR lpszPath,LPCSTR lpszType,LPCSTR lpszSubType)
{				
	USES_CONVERSION;
	X_LOG3(XLOG_DEBUG, _T("开始加载插件....,类型:[%s],子类型:[%s]\n插件路径:%s"),A2W(lpszType),A2W(lpszSubType),A2W(lpszPath));
	//::wprintf(_T("开始加载插件....,类型:[%s],子类型:[%s]\n插件路径:%s \r\n"),A2W(lpszType),A2W(lpszSubType),A2W(lpszPath));
	//USES_CONVERSION;
	HMODULE hModule =  LoadLibraryExW(A2W(lpszPath),0,LOAD_WITH_ALTERED_SEARCH_PATH);	
	if(hModule)
	{	
		CPluginInfo* pPlug_in = new CPluginInfo();				
		pPlug_in->hModule = hModule;		
		
		memset(pPlug_in->szType,0,128);
		memset(pPlug_in->szSubType,0,128);
		memset(pPlug_in->ModulePath,0,MAX_PATH);

		USES_CONVERSION;
		wcscpy(pPlug_in->ModulePath,A2W(lpszPath));				
		
		InitFunctionList(*pPlug_in);
		
		if(!pPlug_in->Initialize(NULL,0))
		{			
			X_LOG2(XLOG_WARNING,_T("初始化插件[%s,%s]失败!"),lpszType,lpszSubType);		
			delete pPlug_in;
			return FALSE;
		}

		X_LOG3(XLOG_DEBUG, _T("从路径%s加载类型为[%s]子类型为[%s]的插件成功！"),A2W(lpszPath),A2W(lpszType),A2W(lpszSubType));
		strcpy(pPlug_in->szType,lpszType);
		if(lpszSubType!=NULL)
			strcpy(pPlug_in->szSubType,lpszSubType);
		m_plugins.push_back(pPlug_in);
		return TRUE;
	}
	else
	{
		X_LOG2(XLOG_DEBUG, _T("加载动态连接库失败,路径:[%s],错误代码:[%d]"),lpszPath,GetLastError());
		return FALSE;
	}
}

BOOL CPlugInLoader::InitFunctionList(CPluginInfo& plugin)
{	
	memset(plugin.GetFunctionPtr(),0,sizeof(FUNC_PTR_LIST));

	if(plugin.hModule)
	{
		//系统类
		GET_FUNC_ADDR(plugin,ISYS_GetLastError)
		GET_FUNC_ADDR(plugin,ISYS_GetPluginInfo)
		GET_FUNC_ADDR(plugin,ISYS_Initialize)
		GET_FUNC_ADDR(plugin,ISYS_Uninitialize)
		GET_FUNC_ADDR(plugin,ISYS_FreeMemory)
		GET_FUNC_ADDR(plugin,ISYS_GetInvalidHandleValue)

		//设备类
		GET_FUNC_ADDR(plugin,IDVR_ConnectDevice)
		GET_FUNC_ADDR(plugin,IDVR_DisConnectDevice)
		GET_FUNC_ADDR(plugin,IDVR_StartPreview)
		GET_FUNC_ADDR(plugin,IDVR_StartRealData)
		GET_FUNC_ADDR(plugin,IDVR_GetRealDataHeader)
		GET_FUNC_ADDR(plugin,IDVR_StopRealData)
		GET_FUNC_ADDR(plugin,IDVR_PrepareStream)
		GET_FUNC_ADDR(plugin,IDVR_SwitchCamera)
		GET_FUNC_ADDR(plugin,IDVR_StopPreview)
		GET_FUNC_ADDR(plugin,IDVR_CapturePicture)
		GET_FUNC_ADDR(plugin,IDVR_StartRecord)
		GET_FUNC_ADDR(plugin,IDVR_GetRecordState)
		GET_FUNC_ADDR(plugin,IDVR_StopRecord)
		GET_FUNC_ADDR(plugin,IDVR_SetVideoParams)
		GET_FUNC_ADDR(plugin,IDVR_GetVideoParams)
		GET_FUNC_ADDR(plugin,IDVR_StartSound)
		GET_FUNC_ADDR(plugin,IDVR_SetVolume)
		GET_FUNC_ADDR(plugin,IDVR_StopSound)
		GET_FUNC_ADDR(plugin,IDVR_StartAudioPhone)
		GET_FUNC_ADDR(plugin,IDVR_StopAudioPhone)
		GET_FUNC_ADDR(plugin,IDVR_PTZControl)
		GET_FUNC_ADDR(plugin,IDVR_PTZControlByChannel)
		GET_FUNC_ADDR(plugin,IDVR_GetDeviceInfo)
		GET_FUNC_ADDR(plugin,IDVR_GetChannelInfo)
		GET_FUNC_ADDR(plugin,IDVR_SetDeviceTime)
		GET_FUNC_ADDR(plugin,IDVR_StartDeviceRecord)
		GET_FUNC_ADDR(plugin,IDVR_StopDeviceRecord)
		GET_FUNC_ADDR(plugin,IDVR_GetDeviceRecordStatus)
		GET_FUNC_ADDR(plugin,IDVR_ShowDeviceSettingDlg)
		GET_FUNC_ADDR(plugin,IDVR_RestartDevice)

		//报警和数据类
		GET_FUNC_ADDR(plugin,IDVR_SetIOStatus)
		GET_FUNC_ADDR(plugin,IDVR_GetIOStatus)
		GET_FUNC_ADDR(plugin,IDVR_SetIOValue)
		GET_FUNC_ADDR(plugin,IDVR_GetIOValue)
		GET_FUNC_ADDR(plugin,IDVR_SetAlarmSubscribe)
		GET_FUNC_ADDR(plugin,IDVR_SetAlarmUnsubscribe)

		//远程检索、回放、下载等
		GET_FUNC_ADDR(plugin,IDVR_QueryDevRecord)
		GET_FUNC_ADDR(plugin,IDVR_CancelQueryDevRecord)
		GET_FUNC_ADDR(plugin,IDVR_DownloadFile)
		GET_FUNC_ADDR(plugin,IDVR_StopDownloadFile)
		GET_FUNC_ADDR(plugin,IDVR_GetDownloadPos)
		GET_FUNC_ADDR(plugin,IDVR_DownloadFileByTime)
		GET_FUNC_ADDR(plugin,IDVR_RemotePlay)
		GET_FUNC_ADDR(plugin,IDVR_RemotePlayByTime)
		GET_FUNC_ADDR(plugin,IDVR_StopRemotePlay)
		GET_FUNC_ADDR(plugin,IDVR_PauseRemotePlay)
		GET_FUNC_ADDR(plugin,IDVR_ResumeRemotePlay)
		GET_FUNC_ADDR(plugin,IDVR_StepRemotePlay)
		GET_FUNC_ADDR(plugin,IDVR_FastRemotePlay)
		GET_FUNC_ADDR(plugin,IDVR_SlowRemotePlay)
		GET_FUNC_ADDR(plugin,IDVR_NormalRemotePlay)
		GET_FUNC_ADDR(plugin,IDVR_GetRemotePlayPos)
		GET_FUNC_ADDR(plugin,IDVR_SetRemotePlayPos)
		GET_FUNC_ADDR(plugin,IDVR_RemotePlayCapturePicture)
		GET_FUNC_ADDR(plugin,IDVR_RemotePlaySaveData)
		GET_FUNC_ADDR(plugin,IDVR_StopRemotePlaySaveData)
		GET_FUNC_ADDR(plugin,IDVR_SetRemotePlayDataCallBack)

		X_LOG1(XLOG_DEBUG,_T("获取导出函数列表,%d函数被导出"),plugin.GetFunctionPtr()->ExportNums);		
	}
	return plugin.GetFunctionPtr()->ExportNums>0;
}

void CPlugInLoader::FreePlugIn()
{
	CGuard<CThreadMutex> guard(m_mutex);
	for(PlugInListIter it=m_plugins.begin();it!=m_plugins.end();++it)
	{	
		try{
			X_LOG2(XLOG_DEBUG, _T("开始卸载插件...,类型:[%s]子类型:[%s]"),CString((*it)->GetType()),CString((*it)->GetSubType()));		
			(*it)->Uninitialize();
			X_LOG2(XLOG_DEBUG, _T("卸载插件成功！,类型:[%s]子类型:[%s]"),CString((*it)->GetType()),CString((*it)->GetSubType()));		
		}
		catch(...)
		{
			X_LOG2(XLOG_FATAL, _T("反初始化插件发生异常,类型:[%s],错误代码:[%d]"), CString((*it)->GetType()), GetLastError());
			throw;
		}
		
		FreeLibrary((*it)->hModule);
		delete (*it);
	}
	m_plugins.clear();
}

CPluginInfo* CPlugInLoader::GetPlugIn(LPCSTR lpszType,LPCSTR lpszSubType)
{	
	CGuard<CThreadMutex> guard(m_mutex);
	for (PlugInListIter it = m_plugins.begin(); it != m_plugins.end(); ++it)
	{
		USES_CONVERSION;
		CString	Type = A2W(lpszType);
		CString	SubType = A2W(lpszSubType);
		bool bVerifySubPlugIn = true;
		bool bVerifyMainPlugIn = (Type.Compare(A2W((*it)->szType)) == 0);

		if (lpszSubType != NULL && SubType.GetLength() > 0)
				bVerifySubPlugIn = (SubType.Compare(A2W((*it)->szSubType)) == 0);
		else	bVerifySubPlugIn = true;

		if (bVerifyMainPlugIn && bVerifySubPlugIn)
		{
			if (lpszSubType != NULL)
			{
				X_LOG2(XLOG_DEBUG, _T("从缓存中加载插件...,类型:[%s]子类型:[%s]"), A2W(lpszType), A2W(lpszSubType));
			}
			else
			{
				X_LOG1(XLOG_DEBUG, _T("从缓存中加载插件...,类型:[%s]"), lpszType);
			}

			return *it;
		}
	}

	if (DelayLoadPlugIn(lpszType, lpszSubType))		return m_plugins.back();

	return NULL;
}


