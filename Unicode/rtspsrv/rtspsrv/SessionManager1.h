#pragma once
#include "ThreadMutex.h"
#include <list>


template <class SessionItem,class SessionId>
class SessionManager
{
public:
	virtual void AddSession(const SessionItem& session)
	{
		CGuard<CThreadMutex> guard(m_mutex);
		m_sessions.push_back(session);
	}
	
	virtual BOOL GetSessionByHandle(SessionId hChannel,SessionItem& session)
	{
		CGuard<CThreadMutex> guard(m_mutex);
		std::list<SessionItem>::iterator it = m_sessions.begin();
		for(;it!=m_sessions.end();++it)
		{
			if((*it).sessionid==hChannel)
			{
				session = *it;
				return TRUE;
			}
		}
		return FALSE;
	}
	
	virtual BOOL SetSessionByHandle(SessionId hChannel,const SessionItem& session)
	{
		CGuard<CThreadMutex> guard(m_mutex);
		std::list<SessionItem>::iterator it = m_sessions.begin();
		for(;it!=m_sessions.end();++it)
		{
			if((*it).sessionid==hChannel)
			{
				*it = session;
				return TRUE;
			}
		}
		
		return FALSE;
	}

	virtual void RemoveSession(SessionId hChannel)
	{
		CGuard<CThreadMutex> guard(m_mutex);
		std::list<SessionItem>::iterator it = m_sessions.begin();
		for(;it!=m_sessions.end();++it)
		{
			if((*it).sessionid==hChannel)
			{
				m_sessions.erase(it);
				return;
			}
		}
	}

	virtual void RemoveAllSession()
	{
		CGuard<CThreadMutex> guard(m_mutex);
		std::list<SessionItem>::iterator it = m_sessions.begin();
		for(;it!=m_sessions.end();++it)
		{
			m_sessions.erase(it);
		}
	}
public:
	~SessionManager(){}	
protected:	
	CThreadMutex			  m_mutex;
	std::list<SessionItem> m_sessions;
};

