//
// server.hpp
// ~~~~~~~~~~
//
// Copyright (c) 2003-2008 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef HTTP_SERVER3_SERVER_HPP
#define HTTP_SERVER3_SERVER_HPP

#include <boost/asio.hpp>
#include <string>
#include <vector>
#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>
#include "connection.hpp"
#include "request_handler.hpp"
#include "StreamConnection.h"
#include "GoodyTwoShoes.h"
#include "io_service_pool.hpp"

namespace RTSP {
namespace RTSPSrv {

/// The top-level class of the HTTP server.
//boost::noncopyable这是一个单例类，该类在整个系统中只能拥有一个对象
class server
  : private boost::noncopyable
{
public:
	/// Construct the server to listen on the specified TCP address and port, and
	/// serve up files from the given directory.
	explicit server(const std::string& address, const std::string& port, const std::string& videoport,
		const std::string& doc_root, std::size_t thread_pool_size);

	/// Run the server's io_service loop.
	void run();

	/// Stop the server.
	void stop();

private:
	/// Handle completion of an asynchronous accept operation.
	void handle_accept(const boost::system::error_code& e);

	void server::handle_video_accept(const boost::system::error_code& e);

	/// The number of threads that will call io_service::run().
	std::size_t thread_pool_size_;

	/// The io_service used to perform asynchronous operations.
	io_service_pool io_service_pool_;

	/// Acceptor used to listen for incoming connections.
	boost::asio::ip::tcp::acceptor acceptor_;

	/// Acceptor used to listen for incoming connections.
	boost::asio::ip::tcp::acceptor video_acceptor_;

	/// The next connection to be accepted.
	connection_ptr new_connection_;

	stream_connection_ptr stream_connection_;

	GoodyTwoShoes thegoody;
	/// The handler for all incoming requests.
	request_handler request_handler_;
};

} // namespace server3
} // namespace http

#endif // HTTP_SERVER3_SERVER_HPP
