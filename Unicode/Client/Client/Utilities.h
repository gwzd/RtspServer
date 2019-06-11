#pragma once
#include <time.h>

/*
#pragma comment( lib, "USER32" )

#define MTASSERT(a) _ASSERTE(a)

#define MTVERIFY(a) if (!(a)) PrintError(#a,__FILE__, __LINE__, GetLastError())

__inline void PrintError(LPSTR linedesc, LPSTR filename, int lineno, DWORD errnum)
{
	LPSTR lpBuffer;
	char errbuf[256];
#ifdef _WINDOWS
	char modulename[MAX_PATH];
#else // _WINDOWS
	DWORD numread;
#endif // _WINDOWS

	FormatMessage( FORMAT_MESSAGE_ALLOCATE_BUFFER
			| FORMAT_MESSAGE_FROM_SYSTEM,
			NULL,
			errnum,
			LANG_NEUTRAL,
			(LPTSTR)&lpBuffer,
			0,
			NULL );

	wsprintf(errbuf, "\nThe following call failed at line %d in %s:\n\n"
               "    %s\n\nReason: %s\n", lineno, filename, linedesc, lpBuffer);
#ifndef _WINDOWS
	WriteFile(GetStdHandle(STD_ERROR_HANDLE), errbuf, strlen(errbuf), &numread, FALSE );
	Sleep(3000);
#else
	GetModuleFileName(NULL, modulename, MAX_PATH);
	MessageBox(NULL, errbuf, modulename, MB_ICONWARNING|MB_OK|MB_TASKMODAL|MB_SETFOREGROUND);
#endif
	exit(EXIT_FAILURE);
}
*/
// inline BOOL FileExists(LPCTSTR lpszFilePath)
// {
// 	CFileFind find;
// 	BOOL ret = find.FindFile(lpszFilePath);
// 	find.Close();
// 	return ret;
// }

inline long SafeStrLen(LPCTSTR lpsz)
{
	if((DWORD)lpsz == 0xcdcdcdcd) return 0;	
	return (lpsz == NULL) ? 0 : lstrlen(lpsz);
}
//int SplitString(const char *szSource,const char* szDelimiter,CStringArray &arrString);

#include<fstream>

#define ERR_MSG_LEN   1024

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
 		TCHAR szPath[_MAX_PATH];
		InitializeCriticalSection(&m_cs);
 		GetWindowsDirectory(szPath,_MAX_PATH);
		TCHAR strLogFilePath[_MAX_PATH];
		wsprintf(strLogFilePath,_T("%s\\ClientRtsp.log"),szPath);
//		MessageBox(NULL,strLogFilePath,0,0);
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
		else	m_szLastErrorMsg[0] = '\0';
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
		SYSTEMTIME  t;
		GetLocalTime(&t);
		wsprintf(m_szTimeBuf, _T("%d-%02d-%02d %02d:%02d:%02d:%03d"),
			t.wYear, t.wMonth,
			t.wDay, t.wHour,
			t.wMinute, t.wSecond, t.wMilliseconds);
		return (LPCTSTR)m_szTimeBuf;
	}

	void LogError(LPCTSTR lpszFormat, ...);
 
	void LogErrorMsg(LPCTSTR lpszError)
	{
		__try	{
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
