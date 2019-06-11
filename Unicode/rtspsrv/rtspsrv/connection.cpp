//
// connection.cpp
// ~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2008 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
#include "stdafx.h"
#include "connection.hpp"
#include <vector>
#include <boost/bind.hpp>
#include "request_handler.hpp"
#include "xlog.h"

namespace RTSP {
namespace RTSPSrv {

connection::connection(boost::asio::io_service& io_service,
    request_handler& handler)
	: strand_(io_service),   //teng 9.5 handle串行化
	socket_(io_service),
	request_handler_(handler),
	timer_(io_service)
{
}

boost::asio::ip::tcp::socket& connection::socket()
{
	return socket_;
}

void connection::start()
{
	socket_.async_read_some(boost::asio::buffer(buffer_),
		strand_.wrap(   //teng 9.5 
		boost::bind(&connection::handle_read, shared_from_this(),
		boost::asio::placeholders::error,
		boost::asio::placeholders::bytes_transferred)));

	timer_.expires_from_now(boost::posix_time::seconds(30));
	timer_.async_wait(strand_.wrap(boost::bind(&connection::close, shared_from_this(), boost::asio::placeholders::error)));
}

void connection::handle_read(const boost::system::error_code& e,    //teng 9.5 和开发区的相同
    std::size_t bytes_transferred)
{
	if (!e)
	{
// 		if(timer_.expires_from_now(boost::posix_time::seconds(30))>0)
// 		{
// 			//printf("重新设定Timer!\n");
// 			timer_.async_wait(boost::bind(&connection::close, shared_from_this(), boost::asio::placeholders::error));  
// 		}
// 		else
// 		{
// 			//printf("设定Timer已过期!\n");
// 		}

		boost::tribool result;
		boost::tie(result, boost::tuples::ignore) = request_parser_.parse(
			request_, buffer_.data(), buffer_.data() + bytes_transferred);

		if (timer_.expires_from_now(boost::posix_time::seconds(30)) >0)
		{
			timer_.async_wait(boost::bind(&connection::close, shared_from_this(), boost::asio::placeholders::error));
		}

		if (result)
		{
			//处理请求,将结果存放到reply_
			request_.peer = socket().remote_endpoint();
			request_.server_addr = socket().local_endpoint().address();
			request_handler_.handle_request(request_, reply_);
			//发送响应
			boost::asio::async_write(socket_, reply_.to_buffers(),
				strand_.wrap(
				boost::bind(&connection::handle_write, shared_from_this(),
				boost::asio::placeholders::error)));
		}
		else if (!result)
		{
			reply_ = reply::stock_reply(reply::bad_request);
			boost::asio::async_write(socket_, reply_.to_buffers(),
				strand_.wrap(
				boost::bind(&connection::handle_write, shared_from_this(),
				boost::asio::placeholders::error)));
		}
		else
		{
			socket_.async_read_some(boost::asio::buffer(buffer_),
				strand_.wrap(
				boost::bind(&connection::handle_read, shared_from_this(),
				boost::asio::placeholders::error,
				boost::asio::placeholders::bytes_transferred)));

			timer_.expires_from_now(boost::posix_time::seconds(30));
			timer_.async_wait(strand_.wrap(boost::bind(&connection::close, shared_from_this(), boost::asio::placeholders::error)));
		}
	}

	// If an error occurs then no new asynchronous operations are started. This
	// means that all shared_ptr references to the connection object will
	// disappear and the object will be destroyed automatically after this
	// handler returns. The connection class's destructor closes the socket.
}

void connection::close(const boost::system::error_code& error)
{	
	//if(error.value()==995)
	//	return;
	if (!error)
	{
		X_LOG3(XLOG_DEBUG, _T("连接关闭[错误代码=%d],远端地址[%s:%d]"), error.value(), CString(socket_.remote_endpoint().address().to_string().c_str()), socket_.remote_endpoint().port());

		boost::system::error_code ignored_ec;
		try
		{
			socket_.shutdown(boost::asio::ip::tcp::socket::shutdown_send, ignored_ec);
		}
		catch (boost::exception_detail::clone_impl<boost::exception_detail::error_info_injector<boost::system::system_error> >& e){
			X_LOG1(XLOG_FATAL, _T("关闭socket时发生异常!%s."), CString(e.what()));
		}
	}
}

void connection::handle_write(const boost::system::error_code& e)
{
	//X_LOG3(XLOG_DEBUG,_T("handle_write连接关闭[Error=%d],远端地址[%s:%d]"),e.value(),CString(socket_.remote_endpoint().address().to_string().c_str()),socket_.remote_endpoint().port());
	if (!e)
	{
		// Initiate graceful connection closure. 	
		if (timer_.expires_from_now()>boost::posix_time::seconds(0))
		{
			request_parser_.reset();
			request_.method.clear();
			request_.uri.clear();
			request_.http_version_major = 0;
			request_.http_version_minor = 0;
			request_.headers.clear();
			socket_.async_read_some(boost::asio::buffer(buffer_),
				strand_.wrap(
				boost::bind(&connection::handle_read, shared_from_this(),
				boost::asio::placeholders::error,
				boost::asio::placeholders::bytes_transferred)));
		}
		else
		{
			boost::system::error_code ignored_ec;
			close(e);
		}
	}

	// No new asynchronous operations are started. This means that all shared_ptr
	// references to the connection object will disappear and the object will be
	// destroyed automatically after this handler returns. The connection class's
	// destructor closes the socket.
}

} // namespace server3
} // namespace http
