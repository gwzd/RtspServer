// EnvConfig.cpp: implementation of the CEnvConfig class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "EnvConfig.h"
#include "PlugInLoader.h"

extern HINSTANCE g_pIntance;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CEnvConfig* CEnvConfig::m_pInstance = NULL;

CEnvConfig::CEnvConfig()
{
	//	m_home_dir = "C:\\DF1500";
	if (m_home_dir.IsEmpty())
	{
		TCHAR szFileName[_MAX_PATH];

		GetModuleFileName(g_pIntance, szFileName, _MAX_PATH);
		//	GetCurrentDirectory(MAX_PATH,szFileName); 
		m_home_dir = szFileName;
		int index = m_home_dir.ReverseFind('\\');
		if (index>0)
		{
			szFileName[index] = '\0';
			m_home_dir = szFileName;
		}
	}
}

CEnvConfig::~CEnvConfig()
{
}

CEnvConfig* CEnvConfig::Instance()
{
	if (m_pInstance == NULL)
		m_pInstance = new CEnvConfig();
	return m_pInstance;
}

void CEnvConfig::FreeInstance()
{
	if (m_pInstance != NULL)
		delete m_pInstance;
	m_pInstance = NULL;
}
