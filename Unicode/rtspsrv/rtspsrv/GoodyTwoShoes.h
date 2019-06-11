#pragma once
#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/asio.hpp>
#include <boost/array.hpp>

class GoodyTwoShoes
{
public:
	GoodyTwoShoes(boost::asio::io_service& io_service);		
public:	

private:
	void start_tattle();	
	std::string ParserRequest(const char* lpszRequest);
	void tattle(const boost::system::error_code& error,	std::size_t /*bytes_transferred*/);
	

	void snitch(boost::shared_ptr<std::string> /*message*/,
		const boost::system::error_code& /*error*/,
		std::size_t /*bytes_transferred*/);	

	boost::asio::ip::udp::socket socket_;
	boost::asio::ip::udp::endpoint remote_endpoint_;
	boost::array<char, 1000> recv_buffer_;
};
