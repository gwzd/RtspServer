#include "stdafx.h"
/*#include <windows.h>*/
#include <vector>
#include <stdio.h>
#include "ThreadMutex.h"

class Channel
{
	
};
class RecordSession
{
public:
	HANDLE hChannel;
	BOOL   bIsRecording;
	FILE   *fp;
	BYTE   cStreamHeader[1024];
	int    nStreamHeaderLen;    
	Channel  CHANN[25];
	int chnnum;
};



class RecordSessionManager
{
public:
	static RecordSessionManager *m_pInstance;
	CThreadMutex m_mutex;
	std::vector<RecordSession> m_sessions;
public:
		static RecordSessionManager *GetInstance()
		{
			if(!m_pInstance)
				m_pInstance = new RecordSessionManager();
			return m_pInstance;
		}
		static void FreeInstance()
		{
			if(m_pInstance)
			{
				delete m_pInstance;
				m_pInstance = NULL;
			}			
		}
	
	void AddSession(const RecordSession& session)
	{
		CGuard<CThreadMutex> guard(m_mutex);
		m_sessions.push_back(session);
	}
	
	BOOL GetSessionByHandle(HANDLE hChannel,RecordSession& session)  
	{
		CGuard<CThreadMutex> guard(m_mutex);
		std::vector<RecordSession>::iterator it = m_sessions.begin();
		for(;it!=m_sessions.end();it++)
		{
			if((*it).hChannel==hChannel)
			{
				session = *it;
				return TRUE;
			}
		}
		return FALSE;
	    			
	}
	
	BOOL SetSessionByHandle(HANDLE hChannel,CONST RecordSession& session)
	{
		CGuard<CThreadMutex> guard(m_mutex);
		std::vector<RecordSession>::iterator it = m_sessions.begin();
		for(;it!=m_sessions.end();it++)
		{
			if((*it).hChannel==hChannel)
			{
				*it = session;
				return TRUE;
			}
		}
// 		if(it==m_sessions.end())
// 		{
// 			m_sessions.push_back(session);
// 		}
		return FALSE;	
	}
	
	void RemoveSession(HANDLE hChannel)
	{
		CGuard<CThreadMutex> guard(m_mutex);
		std::vector<RecordSession>::iterator it = m_sessions.begin();
		for(;it!=m_sessions.end();it++)
		{
			if((*it).hChannel==hChannel)
			{
				m_sessions.erase(it);
				return;
			}
		}		
	}
	
};
