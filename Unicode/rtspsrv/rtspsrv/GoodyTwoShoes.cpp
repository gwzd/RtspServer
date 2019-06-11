#include "StdAfx.h"
#include "goodytwoshoes.h"
#include <boost\bind.hpp>
#include "xlog.h"

using namespace boost;
using namespace boost::asio;
using namespace boost::asio::ip;

GoodyTwoShoes::GoodyTwoShoes(boost::asio::io_service& io_service)
: socket_(io_service, udp::endpoint(udp::v4(), 9506))
{
	start_tattle();
}

void GoodyTwoShoes::start_tattle()
{
	socket_.async_receive_from(
		boost::asio::buffer(recv_buffer_), remote_endpoint_,
		boost::bind(&GoodyTwoShoes::tattle, this,
		boost::asio::placeholders::error,
		boost::asio::placeholders::bytes_transferred));
}

std::string GoodyTwoShoes::ParserRequest(const char* lpszRequest)
{
	if(lpszRequest!=NULL&&strlen(lpszRequest)>0)
	{
		if(strcmp(lpszRequest,"?")==0)
			return "1";
	}
	return "0";
}

void GoodyTwoShoes::tattle(const boost::system::error_code& error,
					std::size_t bytes_transferred)
{
	if (!error || error == boost::asio::error::message_size)
	{
		//printf("received %s\n",recv_buffer_.c_array()); 

		try{
			boost::shared_ptr<std::string> message(
				new std::string(ParserRequest(recv_buffer_.c_array())));

			if (message->length()>0)
			{
				socket_.async_send_to(boost::asio::buffer(*message), remote_endpoint_,
					boost::bind(&GoodyTwoShoes::snitch, this, message,
					boost::asio::placeholders::error,
					boost::asio::placeholders::bytes_transferred));
			}
		}
		catch (...)
		{
			X_LOG1(XLOG_FATAL, _T("回复心跳查询时发生异常!查询内容:[%s]."), CString(recv_buffer_.c_array()));
			throw;
		}
		//printf("send %d\n",message->length());

		start_tattle();
	}
}

void GoodyTwoShoes::snitch(boost::shared_ptr<std::string> /*message*/,
				 const boost::system::error_code& /*error*/,
				 std::size_t /*bytes_transferred*/)
{
}