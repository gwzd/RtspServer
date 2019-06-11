// EnvConfig.h: interface for the CEnvConfig class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ENVCONFIG_H__28BB8EC9_BF88_4CCF_9F4E_6B502DB1E26E__INCLUDED_)
#define AFX_ENVCONFIG_H__28BB8EC9_BF88_4CCF_9F4E_6B502DB1E26E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include <string>

class CEnvConfig 
{
private:
	CString m_home_dir;
protected:
	CEnvConfig();
	virtual ~CEnvConfig();
public:
	static CEnvConfig	*m_pInstance;

	static CEnvConfig*	Instance();
	static void FreeInstance();
	const CString GetHomeDir() const { return  m_home_dir; }
};

#endif // !defined(AFX_ENVCONFIG_H__28BB8EC9_BF88_4CCF_9F4E_6B502DB1E26E__INCLUDED_)
