#pragma once
#include <list>
#include "Prototypes.h"
#include "StreamConnection.h"
#include <jrtplib\RTPSession.h>
#include <jrtplib\RTPIPv4Address.h>
#include <jrtplib\RTPSessionParams.h>
#include <jrtplib\RTPUDPv4Transmitter.h>
#include "threadmutex.h"

using namespace jrtplib;

namespace RTSP {
	namespace RTSPSrv {

#define MAX_PACKET_SIZE	((1024 * 64) - 1)
#define MAX_RTP_PAYLOAD  MAX_PACKET_SIZE-200

		class ObserverInfo :public boost::enable_shared_from_this<ObserverInfo>,
			private boost::noncopyable{
		private:
			std::string ip_;
			short port_;
			stream_connection_ptr stream_;
			int protcol_;//0-TCP,1-UDP 
		public:	

			std::string ip() const { return ip_;}

			short port() const { return port_;}

			int  protcol() const { return protcol_;}
			void send(unsigned char* buffer, size_t len);
			void async_send(unsigned char* buffer, size_t len);

			ObserverInfo(const char* ip,short port,int protcol)
			{
				ip_ = ip;
				port_ = port;
				protcol_ = protcol;
			}

			void SetStreamSocket(stream_connection_ptr& s)
			{
				protcol_ = TT_TCP; //TCP·½Ê½
				stream_ = s;
			}

			/*bool operator==(const ObserverInfo& val) const
			{
				return ip_ == val.ip_ && port_  == val.port_ && protcol_ == val.protcol_ && stream_ == val.stream_ ;
			}*/
			ObserverInfo& operator=(const ObserverInfo& val)
			{
				ip_ = val.ip_;
				port_  = val.port_;
				protcol_ = val.protcol_;
				stream_ = val.stream_;

				return *this;
			}

			virtual ~ObserverInfo();
		};
		typedef boost::shared_ptr<ObserverInfo> ObserverInfoPtr;
		typedef boost::weak_ptr<ObserverInfo> ObserverPtr;

		class PacketSender{
		public:
			unsigned short uRtpPort;
		public:
			PacketSender():
			  nTcpClientNum(0),
			  nUdpClientNum(0),
			  uRtpPort(0)
			{
			}
			void addObserver(ObserverPtr info);
			void removeObserver(ObserverPtr info);
			void stopNotify();
			bool empty()
			{
				return observers_.empty();
			}

			void notifyAll(void * buffer,size_t len);
			bool CreateSender(short port,const char* bind_ip);

			virtual ~PacketSender()
			{
				rtpSession.ClearDestinations();
				rtpSession.Destroy();
			}
		private:
			std::list<ObserverPtr> observers_;
			long nTcpClientNum;
			long nUdpClientNum;
			RTPSession rtpSession;
			CThreadMutex m_hObserverMutex;
		};
		bool operator== (const boost::weak_ptr<ObserverInfo>& a, const boost::weak_ptr<ObserverInfo>& b);
	}
}