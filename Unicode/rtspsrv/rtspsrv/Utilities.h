#pragma once

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

/*
#define ERR_MSG_LEN   1024
class CLastError
{
	TCHAR	m_szLastErrorMsg[ERR_MSG_LEN];
protected:
	CLastError(){
		memset(m_szLastErrorMsg,0,sizeof(TCHAR)*ERR_MSG_LEN);
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
			strcpy(m_szLastErrorMsg,lpszError);
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
		}		
		return m_pInstance;
	}
	static void FreeInstance()
	{	
		if(m_pInstance!=NULL)
		{
			delete m_pInstance;
			m_pInstance =NULL;
		}
	}
};

*/
#if 0
#define GSetError(x)	 CLastError::GetInstance()->SetError(x)
#define GGetError()		 CLastError::GetInstance()->GetLastError()
#else
#define GSetError(x) 
#define GGetError()	 
#endif

struct MessageTable{
	int  code;
	char message[256];
};

LPCTSTR GetErrorMessage(int nErrCode);
