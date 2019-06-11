#pragma warning(push)
#pragma warning(disable:4996)
#ifndef _X_LOG_H_
#define _X_LOG_H_

#include <stdio.h>
#include <errno.h>

#define USE_LOG4CPLUS
#ifdef USE_LOG4CPLUS
	#include <log4cplus/loggingmacros.h>
    #include <log4cplus/configurator.h>
    #include <string>

using namespace log4cplus;

	static log4cplus::Logger logger = log4cplus::Logger::getInstance(_T("Log"));
	static void init_log(LPCTSTR path)
    {
        log4cplus::PropertyConfigurator::doConfigure(path);   
    }

    #define XLOG_ALL		log4cplus::TRACE_LOG_LEVEL
    #define XLOG_TRACE		log4cplus::TRACE_LOG_LEVEL
    #define XLOG_DEBUG		log4cplus::DEBUG_LOG_LEVEL
    #define XLOG_INFO		log4cplus::INFO_LOG_LEVEL
    #define XLOG_NOTICE		log4cplus::INFO_LOG_LEVEL
    #define XLOG_WARNING	log4cplus::WARN_LOG_LEVEL
    #define XLOG_ERROR		log4cplus::ERROR_LOG_LEVEL
    #define XLOG_FATAL		log4cplus::FATAL_LOG_LEVEL

	/*
    #define X_LOG(l,...) \
    do { \
        if(logger.isEnabledFor(l)) { \
            char __buf__internal__[2046]={0}; \
            snprintf(__buf__internal__,2045,__VA_ARGS__); \
            logger.forcedLog(l, __buf__internal__, __FILE__, __LINE__); \
        } \
    } while(0);
	*/
	
	#define S_LOG(l,x)      LOG4CPLUS_MACRO_BODY(logger,x,l)
	
	
	#define X_LOG0(l,x) \
		do { \
			if(logger.isEnabledFor(l)) { \
				TCHAR __buf__internal__[2046]={0}; \
				_snwprintf_s(__buf__internal__,2045,2045,x); \
				wcscat_s(__buf__internal__,2045,_T("\r\n")); \
				logger.forcedLog(l, __buf__internal__, __FILE__, __LINE__); \
			} \
		} while(0);

	#define X_LOG1(l,x,y) \
		do { \
			if(logger.isEnabledFor(l)) { \
				TCHAR __buf__internal__[2046]={0}; \
				_snwprintf_s(__buf__internal__,2045,2045,x,y); \
				wcscat_s(__buf__internal__,2045,_T("\r\n")); \
				logger.forcedLog(l, __buf__internal__, __FILE__, __LINE__); \
			} \
		} while(0);
	
	#define X_LOG2(l,x,y,z) \
		do { \
			if(logger.isEnabledFor(l)) { \
				TCHAR __buf__internal__[2046]={0}; \
				_snwprintf_s(__buf__internal__,2045,2045,x,y,z); \
				wcscat_s(__buf__internal__,2045,_T("\r\n")); \
				logger.forcedLog(l, __buf__internal__, __FILE__, __LINE__); \
			} \
		} while(0);

	
	#define X_LOG3(l,x,y,z,a) \
		do { \
			if(logger.isEnabledFor(l)) { \
				TCHAR __buf__internal__[2046]={0}; \
				_snwprintf_s(__buf__internal__,2045,2045,x,y,z,a); \
				wcscat_s(__buf__internal__,2045,_T("\r\n")); \
				logger.forcedLog(l, __buf__internal__, __FILE__, __LINE__); \
			} \
		} while(0);

	#define X_LOG4(l,x,y,z,a,b) \
		do { \
			if(logger.isEnabledFor(l)) { \
				TCHAR __buf__internal__[2046]={0}; \
				_snwprintf_s(__buf__internal__,2045,2045,x,y,z,a,b); \
				wcscat_s(__buf__internal__,2045,_T("\r\n")); \
				logger.forcedLog(l, __buf__internal__, __FILE__, __LINE__); \
			} \
		} while(0);


	#define X_LOG5(l,x,y,z,a,b,c) \
		do { \
			if(logger.isEnabledFor(l)) { \
				TCHAR __buf__internal__[2046]={0}; \
				_snwprintf_s(__buf__internal__,2045,2045,x,y,z,a,b,c); \
				wcscat_s(__buf__internal__,2045,_T("\r\n")); \
				logger.forcedLog(l, __buf__internal__, __FILE__, __LINE__); \
			} \
		} while(0);
	
	#define X_LOG6(l,x,y,z,a,b,c,d) \
		do { \
			if(logger.isEnabledFor(l)) { \
				TCHAR __buf__internal__[2046]={0}; \
				_snwprintf_s(__buf__internal__,2045,2045,x,y,z,a,b,c,d); \
				wcscat_s(__buf__internal__,2045,_T("\r\n")); \
				logger.forcedLog(l, __buf__internal__, __FILE__, __LINE__); \
			} \
		} while(0);


	#define X_LOG7(l,x,y,z,a,b,c,d,e) \
		do { \
			if(logger.isEnabledFor(l)) { \
				TCHAR __buf__internal__[2046]={0}; \
				_snwprintf_s(__buf__internal__,2045,2045,x,y,z,a,b,c,d,e); \
				wcscat_s(__buf__internal__,2045,_T("\r\n")); \
				logger.forcedLog(l, __buf__internal__, __FILE__, __LINE__); \
			} \
		} while(0);


	#define X_LOG8(l,x,y,z,a,b,c,d,e,f) \
		do { \
			if(logger.isEnabledFor(l)) { \
				TCHAR __buf__internal__[2046]={0}; \
				_snwprintf_s(__buf__internal__,2045,2045,x,y,z,a,b,c,d,e,f); \
				wcscat_s(__buf__internal__,2045,_T("\r\n")); \
				logger.forcedLog(l, __buf__internal__, __FILE__, __LINE__); \
			} \
		} while(0);





#elif define USE_ACE_LOG
    #include "ace/Log_Msg.h"
    #include "ace/Trace.h"
    #define XLOG_ALL		LM_TRACE
    #define XLOG_TRACE		LM_TRACE
    #define XLOG_DEBUG		LM_DEBUG
    #define XLOG_INFO		LM_INFO
    #define XLOG_NOTICE		LM_NOTICE
    #define XLOG_WARNING	LM_WARNING
    #define XLOG_ERROR		LM_ERROR
    #define XLOG_FATAL		LM_CRITICAL
    #define X_LOG(l,...) do{ \
        ACE_DEBUG((l,"[%T|%t] %s-%s:%d",__FILE__,__FUNCTION__,__LINE__)); \
        ACE_DEBUG((l,__VA_ARGS__)); \
        }while(0)
#else
    #include <pthread.h>
    #include <time.h>
    #include <sys/time.h>
    #define XLOG_LEVEL 0xFF
    typedef enum{
        XLOG_FATAL=0x1,
        XLOG_ERROR=0x2,
        XLOG_WARNING=0x4,
        XLOG_NOTICE=0x8,
        XLOG_INFO=0x10,
        XLOG_DEBUG=0x20,
        XLOG_TRACE=0x40,
        XLOG_ALL=0x80
    }XLogLevel;

    #define X_LOG(l,) do{ \
        if(XLOG_LEVEL&l){ \
                struct timeval now;\
                gettimeofday(&now,0); \
                struct tm *ptm=localtime(&(now.tv_sec)); \
                printf("[%d|%d:%d:%d.%d] [%s/%s/%d] ",pthread_self(),ptm->tm_hour,ptm->tm_min,ptm->tm_sec,now.tv_usec,__FILE__,__FUNCTION__,__LINE__); \
                printf( __VA_ARGS__); \
            } \
        }while(0)
#endif

#define die(str)	{X_LOG(XLOG_WARNING,str); return;}

#define die_0(str)	{X_LOG(XLOG_WARNING,str); return 0; }

#define die_1(str)	{X_LOG(XLOG_WARNING,str); return -1; }

#define die_ns(str)	{X_LOG(XLOG_WARNING,str); return ""; }

/**//*safe func return empty,0,-1*/
#define SAFE_FUNC(func) if((func)<0) \
    { \
        X_LOG(XLOG_ERROR,"[%s:%d]error!error[%d:%s]\
	n",__FILE__,__LINE__,errno,strerror(errno)); \
        exit(-1); \
    }

/**//*safe func but 1 err return empty,0,-1*/
#define SAFE_FUNC_BUT_ERR1(func,ERR1) do \
    { \
        if((func)<0){ \
		X_LOG(XLOG_ERROR,"[%s:%d]error!error[%d:%s]\n",__FILE__,__LINE__,errno,strerror(errno)); \
            if(errno!=ERR1) exit(-1); \
        } \
        else break; \
    }while(1)

/**//*safe func but 2 err return empty,0,-1*/
#define SAFE_FUNC_BUT_ERR2(func,ERR1,ERR2) do \
    { \
        if((func)<0){ \
            X_LOG(XLOG_ERROR,"[%s:%d]error!error[%d:%s]\n",__FILE__,__LINE__,errno,strerror(errno)); \
            if(errno!=ERR1&&errno!=ERR2)  exit(-1); \
        } \
        else break; \
    }while(1)
#endif
#pragma warning(pop)