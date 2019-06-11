#include "StdAfx.h"
#include ".\streamconnection.h"
#include <boost/bind.hpp>
#include "SessionManager.h"
#include <boost/lexical_cast.hpp>
#include "xlog.h"

namespace RTSP {
	namespace RTSPSrv {

	//	static volatile  long g_stream_counter = 0;
		void stream_init_count()
		{
		//	InterlockedIncrement(&g_stream_counter);			
		}
		void stream_uninit_count()
		{
		//	InterlockedDecrement(&g_stream_counter);
		//	printf("stream_connection instance:%d\n",g_stream_counter);
		}
		boost::asio::ip::tcp::socket& stream_connection::socket()
		{
			return socket_;
		}

		stream_connection::stream_connection(boost::asio::io_service& io_service):
		socket_(io_service)
		{
			stream_init_count();		
			//socket_.set_option(boost::asio::ip::tcp::socket::reuse_address(true));
		}
		
		void stream_connection::async_send(unsigned char* buffer,size_t len)
		{			
			socket_.async_send(boost::asio::buffer(buffer,len),
				boost::bind(&stream_connection::handle_write,
				shared_from_this(), boost::asio::placeholders::error));
		}

		void stream_connection::handle_write(const boost::system::error_code& e)
		{
			if(e)
			{
				boost::system::error_code ignored_ec;
				socket_.shutdown(boost::asio::ip::tcp::socket::shutdown_both, ignored_ec);	
			}
		}

		void stream_connection::send(unsigned char* buffer,size_t len)
		{
			socket_.send(boost::asio::buffer(buffer,len));
		}

		stream_connection::~stream_connection(void)
		{			
			stream_uninit_count();
			socket_.close();
		}

		void stream_connection::handle_read(const boost::system::error_code& e,
			std::size_t bytes_transferred)
		{
			if(bytes_transferred>0)
			{
				int dwSessionId =0;
				try{
					dwSessionId = boost::lexical_cast<int>(buffer_.data());
				}			
				catch(const boost::bad_lexical_cast&)
				{
					socket_.async_read_some(
						boost::asio::buffer(buffer_),      
						boost::bind(&stream_connection::handle_read, shared_from_this(),
						boost::asio::placeholders::error,
						boost::asio::placeholders::bytes_transferred)
						);
				}

				RTSPSessionItem item;
				RTSPSessionManager *pMgr =  (RTSPSessionManager *)RTSPSessionManagerSingleton::instance();		

				if(dwSessionId!=0 && pMgr->GetSessionByHandle(dwSessionId,item))		
				{
					ObserverInfoPtr  pInfo = item.pUserData;
					if(pInfo)			
					{
						pInfo->SetStreamSocket(shared_from_this());	
						//2016.9.17 zxl 取消打印
						//X_LOG1(XLOG_DEBUG,_T("收到会话Id=%d，成功设置Stream句柄!"),dwSessionId);
						//stream_connection_->socket().set_option(boost::asio::ip::tcp::socket::reuse_address(true));
					}										
				}
				else
				{
					X_LOG1(XLOG_ERROR,_T("收到会话Id=%d，但是无效!"),dwSessionId);						
				}
			}
		}

		void stream_connection::start()
		{
			socket_.async_read_some(
				boost::asio::buffer(buffer_),
				boost::bind(&stream_connection::handle_read, shared_from_this(),
				boost::asio::placeholders::error,
				boost::asio::placeholders::bytes_transferred)
				);
		}

	}
}