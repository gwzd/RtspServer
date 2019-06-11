//
// request_handler.cpp
// ~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2008 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
#include "stdafx.h"
#include "request_handler.hpp"
#include <fstream>
#include <sstream>
#include <string>
#include <boost/lexical_cast.hpp>
#include "mime_types.hpp"
#include "reply.hpp"
#include "request.hpp"
#include "xlog.h"

namespace RTSP {
	namespace RTSPSrv {

		request_handler::request_handler(const std::string& doc_root)
			: doc_root_(doc_root)
		{
		}

		void request_handler::handle_request(const request& req, reply& rep)
		{
			// Decode url to path.
			std::string request_path;
			//解码req.uri并放到request_path中
			rep = reply::stock_reply(reply::bad_request);
			if (!url_decode(req.uri, request_path))
			{
				X_LOG1(XLOG_ERROR, _T("协议错误:%s"), req.uri.c_str());
				return;
			}
			//2016.9.17 zxl 取消打印信息
			//X_LOG5(XLOG_DEBUG,_T("请求[来自%s:%d]:%s %s\n%s"),CString(req.peer.address().to_string().c_str()),req.peer.port(),CString(req.method.c_str()),CString(request_path.c_str()),CString(headers_to_string(req.headers).c_str()));

			try{
				preprocess_.process(req, rep);
				//2016.9.17 zxl 取消打印
				//X_LOG2(XLOG_DEBUG,_T("响应:%d RSTP/1.0\r\n%s"),rep.status,CString(headers_to_string(rep.headers).c_str()));
			}
			catch (...)
			{
				X_LOG0(XLOG_FATAL, _T("处理请求时发生异常!"));
				throw;
			}
			return;

			//必须是绝对路径且不包含 "..".
			if (request_path.empty() || request_path[0] != '/'
				|| request_path.find("..") != std::string::npos)
			{
				rep = reply::stock_reply(reply::bad_request);
				return;
			}

			// 如果请求目录,默认文档就是index.html
			if (request_path[request_path.size() - 1] == '/')
			{
				request_path += "index.html";
			}

			//检测文件的扩展名
			std::size_t last_slash_pos = request_path.find_last_of("/");
			std::size_t last_dot_pos = request_path.find_last_of(".");
			std::string extension;
			if (last_dot_pos != std::string::npos && last_dot_pos > last_slash_pos)
			{
				extension = request_path.substr(last_dot_pos + 1);
			}

			// Open the file to send back.
			std::string full_path = doc_root_ + request_path;
			std::ifstream is(full_path.c_str(), std::ios::in | std::ios::binary);
			if (!is)
			{
				rep = reply::stock_reply(reply::not_found);
				return;
			}

			// Fill out the reply to be sent to the client.
			rep.status = reply::ok;
			char buf[512];
			//读取一个文件头
			while (is.read(buf, sizeof(buf)).gcount() == 40)
				rep.content.append(buf, (UINT)is.gcount());

			rep.headers.resize(2);
			rep.headers[0].name = "Content-Length";
			rep.headers[0].value = boost::lexical_cast<std::string>(rep.content.size());
			rep.headers[1].name = "Content-Type";
			rep.headers[1].value = mime_types::extension_to_type(extension);
		}

		bool request_handler::url_decode(const std::string& in, std::string& out)
		{
			out.clear();
			out.reserve(in.size());
			for (std::size_t i = 0; i < in.size(); ++i)
			{
				if (in[i] == '%')
				{
					if (i + 3 <= in.size())
					{
						int value;
						std::istringstream is(in.substr(i + 1, 2));
						if (is >> std::hex >> value)
						{
							out += static_cast<char>(value);
							i += 2;
						}
						else	return false;
					}
					else	return false;
				}
				else if (in[i] == '+')
					out += ' ';
				else
					out += in[i];
			}
			return true;
		}

	} // namespace RTSP
} // namespace RTSPSrv
