// EnvConfig.cpp: implementation of the CEnvConfig class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "EnvConfig.h"
#include "PlugInLoader.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CEnvConfig* CEnvConfig::m_pInstance = NULL;

CEnvConfig::CEnvConfig()
{	
	if( !m_home_dir.GetLength() )
	{
		TCHAR	szFileName[_MAX_PATH];		

		GetModuleFileName(GetModuleHandle(NULL),szFileName,_MAX_PATH);				
		m_home_dir = szFileName;
		int index = m_home_dir.ReverseFind('\\');		
		if(index>0)
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
  if(m_pInstance==NULL)  
	  m_pInstance = new CEnvConfig();
  return m_pInstance;
}

void CEnvConfig::FreeInstance()
{
	if(m_pInstance!=NULL)
		delete m_pInstance;
	m_pInstance = NULL;
}