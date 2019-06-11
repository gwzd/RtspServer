// CPlugInLoader.h: interface for the CPlugInLoader class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PLUGINLOADER_H__C1462961_7882_4DBE_8047_A55A6BED1266__INCLUDED_)
#define AFX_PLUGINLOADER_H__C1462961_7882_4DBE_8047_A55A6BED1266__INCLUDED_

#pragma warning(disable:4786)

#include <list>
#include <vector>
#include "utilities.h"
#include "prototypes.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#define HOME_DIR			 _T("DF1500")
#define PLUGIN_DIR			 _T("\\PlugIns\\")
#define PLUGIN_MAIN_DLL      _T("PlugInMain.dll")

class CPluginInfo
{
private:
	FUNC_PTR_LIST  m_func_ptr_list;
public:
	TCHAR       ModulePath[MAX_PATH];
	PLUGIN_INFO plugin;
	TCHAR	   szType[128];
	TCHAR	   szSubType[128];
	DWORD      InterfaceVersion; //1.0	
	DWORD	   FailedValue;
	HMODULE	   hModule;

	LPSTR GetType()
	{
		return plugin.smanufactuer;
	}

	LPSTR GetSubType()
	{
		return plugin.DeviceTypeName;
	}

	BOOL GetPluginInfo(PLUGIN_INFO *pInfo)
	{
		ZeroMemory(pInfo, sizeof(PLUGIN_INFO));
		ZeroMemory(&plugin, sizeof(PLUGIN_INFO));
		if (m_func_ptr_list.pfnISYS_GetPluginInfo == NULL)
		{
			GSetError(_T("当前的插件不支持模块信息获取！"));
			return FALSE;;
		}
		else
		{
			(*m_func_ptr_list.pfnISYS_GetPluginInfo)(&plugin);

		}
		pInfo = &plugin;
		return TRUE;
	}

	BOOL Initialize(void *pvReserved, DWORD reserved)
	{
		FailedValue = INVALID_DEV_HANDLE;
		if (m_func_ptr_list.pfnISYS_Initialize == NULL)
		{
			GSetError(_T("当前的插件不支持初始化！"));
			return FALSE;
		}
		else
		{
			try
			{
				if (!(*m_func_ptr_list.pfnISYS_Initialize)(pvReserved, reserved))
				{
					GSetError(_T("初始化插件失败！"));
				}
				//初始化插件信息
				if (GetPluginInfo(&plugin))
				{
					FailedValue = plugin.invalid_handle_value;
				}
				else
				{
					FailedValue = INVALID_DEV_HANDLE;
				}

				return TRUE;
			}
			catch (...)
			{
				GSetError(_T("调用ISYS_Initialize发生异常！"));
				return FALSE;
			}
		}
		return FALSE;

	}
	/*	BOOL Uninitialize()
	{
	if(m_func_ptr_list.pfnISYS_Uninitialize==NULL)
	{
	GSetError(_T("当前的插件不支持反初始化！"));
	return FALSE;
	}
	else
	{
	if(!(*m_func_ptr_list.pfnISYS_Uninitialize)())
	{
	GSetError(_T("插件反初始化失败！"));
	return FALSE;
	}
	else
	return TRUE;
	}

	}
	*/
	/////09-01-14 fjw
	void Uninitialize()
	{
		if (m_func_ptr_list.pfnISYS_Uninitialize == NULL)
		{
			GSetError(_T("当前的插件不支持反初始化！"));
			return;
		}
		(*m_func_ptr_list.pfnISYS_Uninitialize)();

	}

	CPluginInfo();
	virtual ~CPluginInfo();
	virtual FUNC_PTR_LIST* GetFunctionPtr();
};



typedef std::list<CPluginInfo*> PlugInList;
typedef  PlugInList::iterator PlugInListIter;

class CPlugInLoader
{
private:
	PlugInList m_plugins;
	CString   m_plugin_home;
	CPlugInLoader();
	virtual ~CPlugInLoader();
protected:
	BOOL LoadPlugIn(LPCTSTR lpszPath, LPCTSTR lpszType, LPCTSTR lpszSubType = NULL);
	BOOL InitFunctionList(CPluginInfo& info);
public:
	TCHAR PluginError[1024];

	static CPlugInLoader* m_pInstance;

	static CPlugInLoader* GetInstance()
	{
		if (m_pInstance == NULL)
		{
			m_pInstance = new CPlugInLoader();
		}
		return m_pInstance;
	}

	static void FreeInstance()
	{
		if (m_pInstance != NULL)
		{
			delete m_pInstance;
			m_pInstance = NULL;
		}
	}

	LPCTSTR SetPlugInHome(LPCTSTR lpszHomeDir);
	void LoadAllPlugIn();
	CPluginInfo* GetPlugIn(LPCTSTR lpszType, LPCTSTR lpszSubType = NULL);
	void FreePlugIn();
	BOOL DelayLoadPlugIn(LPCTSTR lpszType, LPCTSTR lpszSubType);
	void SearchAllPlugIn(std::vector<std::string> &type_names, std::vector<std::string> &subtype_names);
};

#if  1
#define GET_FUNC_ADDR(x,y) { \
			x.GetFunctionPtr()->pfn##y = (y)GetProcAddress(x.hModule,#y); \
if(x.GetFunctionPtr()->pfn##y==NULL) {x.GetFunctionPtr()->ExportNums++;} }
#else
#define GET_FUNC_ADDR(x,y) { \
			x.GetFunctionPtr()->pfn##y = (y)Jans_GetProcAddress(x.hModule,"_"###y); \
if(x.GetFunctionPtr()->pfn##y==NULL) {x.GetFunctionPtr()->ExportNums++;} }
#endif




#endif // !defined(AFX_PLUGINLOADER_H__C1462961_7882_4DBE_8047_A55A6BED1266__INCLUDED_)
