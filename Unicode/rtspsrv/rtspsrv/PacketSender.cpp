#include "stdafx.h"
#include "Prototypes.h"
#include "PacketSender.h"
#include <string>
#include <boost/bind.hpp>
#include <boost/exception/exception.hpp>
#include <list>
#include "xlog.h"

namespace RTSP {
	namespace RTSPSrv {

		std::string GetAddressString(uint32_t ip, uint16_t port)
		{
			char str[24];

			RTP_SNPRINTF(str, 24, "%d.%d.%d.%d:%d", (int)((ip >> 24) & 0xFF), (int)((ip >> 16) & 0xFF), (int)((ip >> 8) & 0xFF),
				(int)(ip & 0xFF), (int)port);
			return std::string(str);
		}

		bool operator== (const boost::weak_ptr<ObserverInfo>& a, const boost::weak_ptr<ObserverInfo>& b) {
			return a.lock() == b.lock();
		}

		ObserverInfo::~ObserverInfo()
		{
			//printf("ObserverInfo deconstructed...%s:%d\n",ip_.c_str(),port_);
		}

		void ObserverInfo::send(unsigned char* buffer, size_t len)
		{
			stream_->send(buffer, len);
		}

		void ObserverInfo::async_send(unsigned char* buffer, size_t len)
		{
			stream_connection_ptr obj = stream_;
			if (obj)
				obj->async_send(buffer, len);
		}

		void PacketSender::addObserver(ObserverPtr infow)
		{
			//确保是线程安全 2011.4.15
			CGuard<CThreadMutex> guard(m_hObserverMutex);
			ObserverInfoPtr info(infow.lock());
			if (info)
			{
				if (info->protcol() == TT_UDP)
				{
					unsigned long intIP = inet_addr(info->ip().c_str());
					intIP = ntohl(intIP);
					RTPIPv4Address rtpAddr(intIP, info->port());
					//增加接收目标
					int status = rtpSession.AddDestination(rtpAddr);

					if (status<0) {
						std::string stdErrStr = RTPGetErrorString(status);
						X_LOG2(XLOG_ERROR, _T("增加接收目标[%s]失败,错误信息:%s"), CString(GetAddressString(intIP, info->port()).c_str()), CString(stdErrStr.c_str()));
					}
					InterlockedIncrement(&nUdpClientNum);
				}
				else
					InterlockedIncrement(&nTcpClientNum);
			}
			observers_.push_back(info);
		}

		void PacketSender::stopNotify()
		{
			CGuard<CThreadMutex> guard(m_hObserverMutex);
			observers_.clear();
		}

		void PacketSender::removeObserver(ObserverPtr infow)
		{
			//确保是线程安全 2011.4.15
			CGuard<CThreadMutex> guard(m_hObserverMutex);
			ObserverInfoPtr info(infow.lock());
			observers_.remove(infow);
			//printf("observer removed!\n");
			if (info)
			{
				if (info->protcol() == TT_TCP)
				{
					InterlockedDecrement(&nTcpClientNum);
					/*if(info->stream()!=NULL)
					info->stream()->socket().shutdown(boost::asio::ip::tcp::socket::shutdown_send);*/
				}
				else if (info->protcol() == TT_UDP)
				{
					InterlockedDecrement(&nUdpClientNum);
					unsigned long intIP = inet_addr(info->ip().c_str());

					intIP = ntohl(intIP);
					RTPIPv4Address rtpAddr(intIP, info->port());
					int status = rtpSession.DeleteDestination(rtpAddr);
					if (status<0) {
						std::string stdErrStr = RTPGetErrorString(status);
						//std::cout<< stdErrStr<<std::endl;
						X_LOG2(XLOG_ERROR, _T("删除接收目标[%s]失败,错误信息:%s"), CString(GetAddressString(intIP, info->port()).c_str()), CString(stdErrStr.c_str()));
					}
				}
			}
		}

		bool PacketSender::CreateSender(short port, const char* bind_ip)
		{
			RTPSessionParams sessParams;
			sessParams.SetOwnTimestampUnit(1.0 / 30.0); //30 video frames per second
			sessParams.SetUsePollThread(false);
			sessParams.SetAcceptOwnPackets(true);
			sessParams.SetMaximumPacketSize(MAX_PACKET_SIZE);

			//setup transmission parameters
			RTPUDPv4TransmissionParams transParams;
			transParams.SetPortbase(port);
			int status = -1;
			try
			{
				status = rtpSession.Create(sessParams, &transParams);
				if (status<0) {
					std::string stdErrStr = RTPGetErrorString(status);
					X_LOG2(XLOG_ERROR, _T("创建数据发送Socket失败！端口:%d,错误原因:%s"), port, CString(stdErrStr.c_str()));
					//std::cout<< "Create:"<<stdErrStr<<std::endl;			
				}
			}
			catch (...)
			{
				X_LOG0(XLOG_FATAL, _T("创建数据发送Socket发生异常！"));
				throw;
			}

			return status >= 0;
		}

		void PacketSender::notifyAll(void * buffer, size_t len)
		{
			if (nUdpClientNum > 0)
			{
				long lRestDataLength = (long)(len - MAX_RTP_PAYLOAD);
				long lRestDataOffset = 0;

				if (lRestDataLength>0)
				{
					long lCurrentSendLength = MAX_RTP_PAYLOAD;
					bool bMarked = false;
					while (lRestDataLength>0)
					{
						int status;
						status = rtpSession.SendPacket((unsigned char*)buffer + lRestDataOffset, lCurrentSendLength, 0, bMarked, 10UL);
						//移动发送指针
						lRestDataOffset += lCurrentSendLength;
						//缓冲区剩余内容
						lRestDataLength = (long)(len - lRestDataOffset);

						//如果还需要分包
						if (lRestDataLength > MAX_RTP_PAYLOAD)
							lCurrentSendLength = MAX_RTP_PAYLOAD;
						else
						{
							lCurrentSendLength = lRestDataLength;
							bMarked = true;
						}

						if (status<0)
						{
							X_LOG1(XLOG_ERROR, _T("发送RTP数据出错,详情:%s"), CString(RTPGetErrorString(status).c_str()));
						}
					}
				}
				else
				{
					int status = rtpSession.SendPacket(buffer, len, 0, true, 10UL);
					if (status<0)
					{
						X_LOG1(XLOG_ERROR, _T("发送RTP数据出错,详情:%s"), CString(RTPGetErrorString(status).c_str()));
					}
				}
			}

			if (nTcpClientNum>0)
			{
				//确保是线程安全 2011.4.15
				CGuard<CThreadMutex> guard(m_hObserverMutex);
				try{
					if (!observers_.empty())
					{
						std::list<ObserverPtr>::iterator it = observers_.begin();
						while (it != observers_.end())
						{
							ObserverInfoPtr obj(it->lock());
							if (obj)
							{
								if (obj->protcol() == TT_TCP)
								{
									struct LengthIndicator{
										int head;
										int body;
										int tail;
									};

									LengthIndicator lenInc;
									lenInc.head = 0xeb90eb90;
									lenInc.body = (int)len;
									lenInc.tail = 0x90eb90eb;

									//TCP方式发送头
									try{
										//obj->send((unsigned char*)&lenInc,sizeof(LengthIndicator));
										//obj->send((unsigned char*)buffer,len);

										obj->async_send((unsigned char*)&lenInc, sizeof(LengthIndicator));
										obj->async_send((unsigned char*)buffer, len);
									}
									catch (boost::exception_detail::clone_impl<boost::exception_detail::error_info_injector<boost::system::system_error> >& ex2){
										X_LOG3(XLOG_FATAL, _T("发送Tcp数据流时发生异常4！(Observer=%s:%d),原因:%s."), CString(obj->ip().c_str()), obj->port(), CString(ex2.what()));
										//throw;											
									}
								}
							}
							else
							{
								X_LOG0(XLOG_WARNING, _T("存在无效的数据接收者！"));
							}
							++it;
						}
					}
				}
				catch (boost::exception_detail::clone_impl<boost::exception_detail::error_info_injector<boost::system::system_error> >& ex2){
					X_LOG1(XLOG_FATAL, _T("发送Tcp数据流时发生异常1！%s."), CString(ex2.what()));
					throw;
				}
				catch (const std::exception & std_e)
				{
					X_LOG1(XLOG_FATAL, _T("发送Tcp数据流时发生异常3！%s."), CString(std_e.what()));
					throw;
				}
				catch (...)
				{
					X_LOG0(XLOG_FATAL, _T("发送Tcp数据流发生异常！"));
					throw;
				}
			}
		}
	}
}