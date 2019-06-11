#pragma once
//example: CGuard<CThreadMutex> guard(lockSomething);
class CThreadMutex
{
public:
	CThreadMutex()
	{
		::InitializeCriticalSection(&m_cs);
	}
	~CThreadMutex()
	{
		::DeleteCriticalSection(&m_cs);
	}
	void Enter() const
	{
		::EnterCriticalSection((LPCRITICAL_SECTION)&m_cs);
	}
	void Leave() const
	{
		::LeaveCriticalSection((LPCRITICAL_SECTION)&m_cs);
	}
	
#if(_WIN32_WINNT >= 0x0400)
	BOOL TryEnter() const
	{
		return ::TryEnterCriticalSection((LPCRITICAL_SECTION)&m_cs);
	} 
#endif /* _WIN32_WINNT >= 0x0400 */
	
private:
	CRITICAL_SECTION m_cs;
	
	int HIDDEN_COPY(CThreadMutex);	
};

// Gurad class that use stack to autoly perform lock and unloack action
template <class MUTEX>
class CGuard
{
public:
	CGuard(const MUTEX& mutex)
		:m_oMutex(mutex)
	{
		m_oMutex.Enter();
	}
	~CGuard()
	{
		m_oMutex.Leave();
	}
private:
	const MUTEX& m_oMutex;
	CGuard(const CGuard&);
	CGuard& operator = (const CGuard&);
};
