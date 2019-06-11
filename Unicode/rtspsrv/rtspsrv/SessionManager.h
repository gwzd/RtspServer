#pragma once

#include "Singleton.h"
#include "SessionManager1.h"
#include <iostream>
#include "reply.hpp"
#include <boost/asio.hpp>
#include "PacketSender.h"

namespace RTSP {
	namespace RTSPSrv {

		struct RTSPSessionItem
		{
			DWORD  sessionid;
			DWORD  dwSeqenceNumber;
			std::string dvrip;
			unsigned short port;
			int conntype;
			std::string username;
			std::string password;
			std::string devtype;
			short streamtype;
			short channel;
			std::string clientip;
			std::string recvip;
			std::string proxy;
			unsigned short portbase;
			reply::status_type status;
			std::string file;
			short clientport;
			HANDLE hSession;
			ObserverInfoPtr pUserData;
			time_t timestamp;
		};

		class RTSPSessionManager :public SessionManager<RTSPSessionItem, DWORD>
		{

		public:
			DWORD createSessionId()
			{
				return InterlockedIncrement(&SessionId);
			}
			static unsigned short getNextPort();
			void CleanSession(unsigned long sessionId);
			bool GetSessionItemByPeer(const std::string& ip, unsigned short port, RTSPSessionItem* item);
			void HeartBeatCheck();
			bool IsHeartBeatCheckStopped();
			void StopHeartBeatCheck();
			RTSPSessionManager();
		private:
			HANDLE hStopEvent;
			static long SessionId;
			static long StartPort;
		};
		typedef Singleton<RTSPSessionManager> RTSPSessionManagerSingleton;
	}
}
