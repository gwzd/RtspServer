#pragma once
#include <time.h>
#include<fstream>

#define ERR_MSG_LEN   1024

inline long SafeStrLen(LPCTSTR lpsz)
{
	if((DWORD)lpsz == 0xcdcdcdcd) return 0;	
	return (lpsz == NULL) ? 0 : lstrlen(lpsz);
}

class CLastError
{
	TCHAR	m_szLastErrorMsg[ERR_MSG_LEN];
	TCHAR   m_szTimeBuf[256];
	BOOL	m_bInitInHeap;	
	std::ofstream m_func_log;
    CRITICAL_SECTION m_cs;
protected:
 	explicit CLastError()		
	{				
 		TCHAR szPath[MAX_PATH];
		InitializeCriticalSection(&m_cs);
 		GetWindowsDirectory(szPath,MAX_PATH);
		TCHAR strLogFilePath[MAX_PATH];
		wsprintf(strLogFilePath,_T("%s\\CTransRTsp.log"),szPath);
	//	MessageBox(NULL,strLogFilePath,0,0);
		//CString strLogFilePath(szPath);
 	//	strLogFilePath+="\\CameraVideo.log";
 		m_func_log.open(strLogFilePath,std::ios_base::trunc|std::ios_base::out);
 		memset(m_szLastErrorMsg,0,sizeof(TCHAR)*ERR_MSG_LEN);
 		LogErrorMsg(_T("------------日志开始------------"));
 	};
public:	
	LPTSTR   GetMsgBuffer() {	
		return m_szLastErrorMsg;
	}
	LPCTSTR  GetLastError()
	{
		return m_szLastErrorMsg;
	}

	void SetError(LPCTSTR lpszError) 
	{					
		if(lpszError!=NULL)
		{			
			wcsncpy(m_szLastErrorMsg,lpszError,ERR_MSG_LEN-1);
			m_szLastErrorMsg[ERR_MSG_LEN]=0;
		}
		else
		{
			m_szLastErrorMsg[0] = '\0';
		}
	}

	static CLastError* m_pInstance;

	static CLastError* GetInstance()
	{		
		if(m_pInstance==NULL)
		{
			m_pInstance =new CLastError();
			m_pInstance->m_bInitInHeap = TRUE;
		}		
		return m_pInstance;
	}

	LPCTSTR GetTime()
	{
	  struct tm *local;
	  time_t t;
	  t=time(NULL);
	  local=localtime(&t);

	  wsprintf(m_szTimeBuf,_T("%d-%d-%d %d:%d:%d"),
		  local->tm_year,local->tm_mon,
		  local->tm_mday,local->tm_hour,
		  local->tm_min,local->tm_sec);
 		return (LPCTSTR)m_szTimeBuf;

	}
	void LogError(LPCTSTR lpszFormat,...);
 
	void LogErrorMsg(LPCTSTR lpszError)
	{
		__try
		{
			EnterCriticalSection(&m_cs);
		
			m_func_log<<(LPCTSTR)GetTime()<<" "<<lpszError<<std::endl;
			m_func_log.flush();
		}
		__finally{
			LeaveCriticalSection(&m_cs);
		}
		
	}
	static void FreeInstance()
	{	
		if(m_pInstance!=NULL)
		{
			m_pInstance->LogErrorMsg(_T("------------日志结束------------"));
			m_pInstance->m_func_log.close();
			DeleteCriticalSection(&m_pInstance->m_cs);
			if(m_pInstance->m_bInitInHeap)
			{
				m_pInstance->m_bInitInHeap = FALSE;
				delete m_pInstance;				
				m_pInstance =NULL;
			}
			
		}
	}
};

#if 1
#define GSetError(x)	 CLastError::GetInstance()->SetError(x)
#define GGetError()		 CLastError::GetInstance()->GetLastError()
#define GLogError(x)	 CLastError::GetInstance()->LogErrorMsg(x)
#define GLogTrace        CLastError::GetInstance()->LogError
#define GSetLogError(x)  CLastError::GetInstance()->SetError(x);CLastError::GetInstance()->LogErrorMsg(x)
#else
#define GSetError(x) 
#define GGetError()	
#define GLogError
#endif

struct MessageTable{
	int  code;
	char message[256];
};

LPCTSTR GetErrorMessage(int nErrCode);
#ifndef _countof
#define _countof(_Array) (sizeof(_Array) / sizeof(_Array[0]))
#endif
