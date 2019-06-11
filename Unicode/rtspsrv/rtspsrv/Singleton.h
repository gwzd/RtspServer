#ifndef SINGLETON_H
#define SINGLETON_H
#include"ThreadMutex.h"

template< class T >
class Singleton
{
	class InstanceHolder
	{
	public:
		InstanceHolder() : mObject(0) {}
		~InstanceHolder() { delete mObject; }
		T* get() { return mObject; }
		void set(T* p) { delete mObject; mObject = p; }

	private:
		InstanceHolder(const InstanceHolder&);
		InstanceHolder& operator=(const InstanceHolder&);

	private:
		T* mObject;
	};

public:
	static T* instance()
	{
		CGuard<CThreadMutex> guard(mMutex);
		if (!mFlag)
		{
			if (!mFlag)
			{
				mInstance.set(new T());
				mFlag = 1;
			}
		}
		return mInstance.get();
	}

private:
	static InstanceHolder     mInstance;
	static volatile int       mFlag;
	static CThreadMutex mMutex;

	Singleton();
	Singleton(const Singleton&);

	Singleton& operator=(const Singleton&);
};

template<class T> typename Singleton<T>::InstanceHolder Singleton<T>::mInstance;
template<class T> volatile int Singleton<T>::mFlag = 0;
template<class T> CThreadMutex Singleton<T>::mMutex;

#endif
