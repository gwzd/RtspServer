#pragma once

#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/asio.hpp>
#include <boost/array.hpp>

namespace RTSP {
	namespace RTSPSrv {

		class stream_connection:public boost::enable_shared_from_this<stream_connection>,
			private boost::noncopyable

		{
			/// Socket for the connection.
			boost::asio::ip::tcp::socket socket_;
			//unsigned char buff[1024*100];		

			/// Buffer for incoming data.
			boost::array<char,4096> buffer_;		

		private:
			/// Handle completion of a read operation.
			void handle_read(const boost::system::error_code& e,
				std::size_t bytes_transferred);
			void handle_write(const boost::system::error_code& e);
		public:
			//unsigned char* getdata(void *buf,size_t len) { memcpy(buff,buf,len);return buff;}
			void start();
			boost::asio::ip::tcp::socket& socket();
			void send(unsigned char* buffer,size_t len);
			void async_send(unsigned char* buffer,size_t len);

			explicit stream_connection(boost::asio::io_service& io_service);
			virtual ~stream_connection(void);
		};
		typedef boost::shared_ptr<stream_connection> stream_connection_ptr;
		typedef boost::weak_ptr<stream_connection> stream_weak_ptr;
	}
}