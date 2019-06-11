//
// reply.cpp
// ~~~~~~~~~
//
// Copyright (c) 2003-2008 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
#include "stdafx.h"
#include "reply.hpp"
#include <string>
#include <boost/lexical_cast.hpp>
#include <sstream>

namespace RTSP {
namespace RTSPSrv {

namespace status_strings {

const std::string ok =						"RTSP/1.0 200 OK\r\n";
const std::string created =					"RTSP/1.0 201 Created\r\n";
const std::string accepted =				"RTSP/1.0 202 Accepted\r\n";
const std::string no_content =				"RTSP/1.0 204 No Content\r\n";
const std::string multiple_choices =		"RTSP/1.0 300 Multiple Choices\r\n";
const std::string moved_permanently =		"RTSP/1.0 301 Moved Permanently\r\n";
const std::string moved_temporarily =		"RTSP/1.0 302 Moved Temporarily\r\n";
const std::string not_modified =			"RTSP/1.0 304 Not Modified\r\n";
const std::string bad_request =				"RTSP/1.0 400 Bad Request\r\n";
const std::string unauthorized =			"RTSP/1.0 401 Unauthorized\r\n";
const std::string forbidden =				"RTSP/1.0 403 Forbidden\r\n";
const std::string not_found =				"RTSP/1.0 404 Not Found\r\n";
const std::string connect_time_out =		"RTSP/1.0 405 Time out\r\n";
const std::string connect_reset_byremote =	"RTSP/1.0 406 Connection Reset by Remote Host \r\n";
const std::string internal_server_error =	"RTSP/1.0 500 Internal Server Error\r\n";
const std::string not_implemented =			"RTSP/1.0 501 Not Implemented\r\n";
const std::string bad_gateway =				"RTSP/1.0 502 Bad Gateway\r\n";
const std::string service_unavailable =		"RTSP/1.0 503 Service Unavailable\r\n";
const std::string unsupported =				"RTSP/1.0 504 Unsupported Command\r\n";


boost::asio::const_buffer to_buffer(reply::status_type status)
{
	switch (status)
	{
	case reply::ok:
		return boost::asio::buffer(ok);
	case reply::created:
		return boost::asio::buffer(created);
	case reply::accepted:
		return boost::asio::buffer(accepted);
	case reply::no_content:
		return boost::asio::buffer(no_content);
	case reply::multiple_choices:
		return boost::asio::buffer(multiple_choices);
	case reply::moved_permanently:
		return boost::asio::buffer(moved_permanently);
	case reply::moved_temporarily:
		return boost::asio::buffer(moved_temporarily);
	case reply::not_modified:
		return boost::asio::buffer(not_modified);
	case reply::bad_request:
		return boost::asio::buffer(bad_request);
	case reply::unauthorized:
		return boost::asio::buffer(unauthorized);
	case reply::forbidden:
		return boost::asio::buffer(forbidden);
	case reply::not_found:
		return boost::asio::buffer(not_found);
	case reply::connect_time_out:
		return boost::asio::buffer(connect_time_out);
	case reply::connect_reset_byremote:
		return boost::asio::buffer(connect_reset_byremote);
	case reply::internal_server_error:
		return boost::asio::buffer(internal_server_error);
	case reply::not_implemented:
		return boost::asio::buffer(not_implemented);
	case reply::bad_gateway:
		return boost::asio::buffer(bad_gateway);
	case reply::service_unavailable:
		return boost::asio::buffer(service_unavailable);
	case reply::unsupported:
		return boost::asio::buffer(unsupported);
	default:
		return boost::asio::buffer(internal_server_error);
	}
}

} // namespace status_strings

namespace misc_strings {

	const char name_value_separator[] = { ':', ' ' };
	const char crlf[] = { '\r', '\n' };

} // namespace misc_strings

std::vector<boost::asio::const_buffer> reply::to_buffers()
{
	std::vector<boost::asio::const_buffer> buffers;
	buffers.push_back(status_strings::to_buffer(status));
	for (std::size_t i = 0; i < headers.size(); ++i)
	{
		header& h = headers[i];
		buffers.push_back(boost::asio::buffer(h.name));
		buffers.push_back(boost::asio::buffer(misc_strings::name_value_separator));
		buffers.push_back(boost::asio::buffer(h.value));
		buffers.push_back(boost::asio::buffer(misc_strings::crlf));
	}
	buffers.push_back(boost::asio::buffer(misc_strings::crlf));
	buffers.push_back(boost::asio::buffer(content));
	return buffers;
}

namespace stock_replies {

	const char ok[] = "";
	const char created[] = "";
	const char accepted[] = "";
	const char no_content[] = "";
	const char multiple_choices[] = "";
	const char moved_permanently[] = "";
	const char moved_temporarily[] = "";
	const char not_modified[] = "";
	const char bad_request[] = "";
	const char unauthorized[] = "";
	const char forbidden[] = "";
	const char not_found[] = "";
	const char connect_time_out[] = "";
	const char connect_reset_byremote[] = "";
	const char internal_server_error[] = "";
	const char not_implemented[] = "";
	const char bad_gateway[] = "";
	const char service_unavailable[] = "";

	std::string to_string(reply::status_type status)
	{
		switch (status)
		{
		case reply::ok:
			return ok;
		case reply::created:
			return created;
		case reply::accepted:
			return accepted;
		case reply::no_content:
			return no_content;
		case reply::multiple_choices:
			return multiple_choices;
		case reply::moved_permanently:
			return moved_permanently;
		case reply::moved_temporarily:
			return moved_temporarily;
		case reply::not_modified:
			return not_modified;
		case reply::bad_request:
			return bad_request;
		case reply::unauthorized:
			return unauthorized;
		case reply::forbidden:
			return forbidden;
		case reply::not_found:
			return not_found;
		case reply::internal_server_error:
			return internal_server_error;
		case reply::not_implemented:
			return not_implemented;
		case reply::bad_gateway:
			return bad_gateway;
		case reply::service_unavailable:
			return service_unavailable;
		default:
			return internal_server_error;
		}
	}
} // namespace stock_replies

reply reply::stock_reply(reply::status_type status)
{
	reply rep;
	rep.status = status;
	rep.content = stock_replies::to_string(status);
	rep.headers.resize(2);
	rep.headers[0].name = "Content-Length";
	rep.headers[0].value = boost::lexical_cast<std::string>(rep.content.size());
	rep.headers[1].name = "Content-Type";
	rep.headers[1].value = "text/html";
	return rep;
}

} // namespace server3
} // namespace http
