//
// request.hpp
// ~~~~~~~~~~~
//
// Copyright (c) 2003-2008 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef HTTP_SERVER3_REQUEST_HPP
#define HTTP_SERVER3_REQUEST_HPP

#include <string>
#include <vector>
#include <boost/asio.hpp>
#include "header.hpp"

namespace RTSP {
namespace RTSPSrv {

/// A request received from a client.
struct request
{
	std::string method;
	std::string uri;
	int http_version_major;
	int http_version_minor;
	std::vector<header> headers;
	boost::asio::ip::tcp::endpoint peer;
	boost::asio::ip::address server_addr;
};

std::string headers_to_string(const std::vector<header>& headers);
std::string format(const char *fmt, ...);
std::wstring format(const wchar_t *fmt, ...);

} // namespace server3
} // namespace http

#endif // HTTP_SERVER3_REQUEST_HPP
