
#pragma once

#include <map>
#include <list>
#include <algorithm>

struct NullLocker{ 
	inline void lock() {}; 
	inline void unlock() {}; 
}; 

template <typename T>
struct arg_type_traits {
	typedef const T& result;
};

template <typename T>
struct arg_type_traits<T&> {
	typedef T& result;
};

template <typename T>
struct arg_type_traits<const T&> {
	typedef const T& result;
};

struct CriticalSectionLocker{ 
	CriticalSectionLocker() {::InitializeCriticalSection(&cs_);} 
	~CriticalSectionLocker() {::DeleteCriticalSection(&cs_);} 
	inline void lock() {::EnterCriticalSection(&cs_);} 
	inline void unlock() {::LeaveCriticalSection(&cs_);} 
private: 
	CRITICAL_SECTION cs_; 
};

template < 
	class ObserverT, 
	class LockerT = NullLocker, 
	class ContainerT = std::list<ObserverT *> 
> 
class BasicSubject : protected LockerT
{
public:
	BasicSubject() : withinLoop_(false) {}

	void addObserver(ObserverT *observer) {
		lock();
		if (withinLoop_) 
			modifyActionBuf_.insert(std::make_pair(true, observer));
		else
			observers_.push_back(observer);        
		unlock();
	}

	size_t size() {
		return 	observers_.size();
	}

	void removeObserver(ObserverT *observer) {
		lock();
		if (withinLoop_)
			modifyActionBuf_.insert(std::make_pair(false, observer));
		else
			observers_.erase(
			remove(observers_.begin(), observers_.end(), observer),
			observers_.end());
		unlock();
	}

	template <typename ReturnT>
		void notifyAll(ReturnT (ObserverT::*pfn)())  {
			lock();
			beginLoop();
			for (ContainerT::iterator it = observers_.begin(), itEnd = observers_.end(); it != itEnd; ++it)
				((*it)->*pfn)();
			endLoop();
			unlock();
		}	

	template <typename ReturnT, typename Arg1T>
		void notifyAll(ReturnT (ObserverT::*pfn)(Arg1T), 
		typename arg_type_traits<Arg1T>::result arg1)   {
			lock();
			beginLoop();
			for (ContainerT::iterator it = observers_.begin(),itEnd = observers_.end(); it != itEnd; ++it)
				((*it)->*pfn)(arg1);
			endLoop();
			unlock();
		}

	template <typename ReturnT, typename Arg1T, typename Arg2T>
		void notifyAll(ReturnT (ObserverT::*pfn)(Arg1T, Arg2T), 
		typename arg_type_traits<Arg1T>::result arg1, 
		typename arg_type_traits<Arg2T>::result arg2 )  {
			lock();
			beginLoop();
			for (ContainerT::iterator it = observers_.begin(), itEnd = observers_.end(); it != itEnd; ++it)
				((*it)->*pfn)(arg1, arg2);
			endLoop();
			unlock();
		}

	template <typename ReturnT, typename Arg1T, typename Arg2T,typename Arg3T>
		void notifyAll(ReturnT (ObserverT::*pfn)(Arg1T, Arg2T,Arg3T), 
		typename arg_type_traits<Arg1T>::result arg1, 
		typename arg_type_traits<Arg2T>::result arg2,
		typename arg_type_traits<Arg3T>::result arg3)  {
			lock();
			beginLoop();
			for (ContainerT::iterator it = observers_.begin(), itEnd = observers_.end(); it != itEnd; ++it)
				((*it)->*pfn)(arg1, arg2,arg3);
			endLoop();
			unlock();
		}

private:
	inline void beginLoop() {
		withinLoop_ = true;
	}

	void endLoop() {
		if (!modifyActionBuf_.empty()) {
			for (std::multimap<bool, ObserverT*>::iterator it = modifyActionBuf_.begin(),
				itEnd = modifyActionBuf_.end(); it != itEnd; ++it) {
					if (it->first)
						observers_.push_back(it->second);
					else
						observers_.erase(
						remove(observers_.begin(), observers_.end(), it->second),
						observers_.end());
				}
				modifyActionBuf_.clear();
		}       
		withinLoop_ = false;
	}

protected:
	ContainerT observers_;

private:
	bool withinLoop_;
	std::multimap<bool, ObserverT*> modifyActionBuf_;
};
