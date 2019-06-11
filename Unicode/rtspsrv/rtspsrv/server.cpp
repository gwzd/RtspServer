//
// server.cpp
// ~~~~~~~~~~
//
// Copyright (c) 2003-2008 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
#include "stdafx.h"
#include "server.hpp"
#include <boost/thread.hpp>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <vector>
#include "VideoManager.h"
#include "SessionManager.h"
#include "xlog.h"

namespace RTSP {
	namespace RTSPSrv {

		server::server(const std::string& address, const std::string& port, const std::string& videoport,
			const std::string& doc_root, std::size_t io_service_pool_size)
			: io_service_pool_(io_service_pool_size),
			acceptor_(io_service_pool_.get_io_service()),
			video_acceptor_(io_service_pool_.get_io_service()),
			new_connection_(new connection(io_service_pool_.get_io_service(), request_handler_)),
			request_handler_(doc_root),
			stream_connection_(new stream_connection(io_service_pool_.get_io_service())),
			thegoody(io_service_pool_.get_io_service())

		{
			// Open the acceptor with the option to reuse the address (i.e. SO_REUSEADDR).
			boost::asio::ip::tcp::resolver resolver(io_service_pool_.get_io_service());
			boost::asio::ip::tcp::resolver::query query(address, port);

			boost::asio::ip::tcp::resolver::query query_video(address, videoport);

			boost::asio::ip::tcp::endpoint endpoint = *resolver.resolve(query);

			acceptor_.open(endpoint.protocol());
			acceptor_.set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));
			acceptor_.bind(endpoint);
			acceptor_.listen();
			acceptor_.async_accept(new_connection_->socket(),
				boost::bind(&server::handle_accept, this,
				boost::asio::placeholders::error));

			boost::asio::ip::tcp::endpoint video_endpoint = *resolver.resolve(query_video);

			video_acceptor_.open(video_endpoint.protocol());
			video_acceptor_.set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));
			video_acceptor_.bind(video_endpoint);
			video_acceptor_.listen();

			video_acceptor_.async_accept(stream_connection_->socket(),
				boost::bind(&server::handle_video_accept, this,
				boost::asio::placeholders::error));
		}

		void server::run()
		{
			io_service_pool_.run();
		}

		void server::stop()
		{
			RTSPSessionManager *pMgr = (RTSPSessionManager *)RTSPSessionManagerSingleton::instance();
			pMgr->StopHeartBeatCheck();
			VideoStreamManager *pVideoMgr = (VideoStreamManager *)VideoStreamManagerSingleton::instance();
			pVideoMgr->StopAllVideoStream();
			io_service_pool_.stop();
		}

		void server::handle_video_accept(const boost::system::error_code& e)
		{
			RTSPSessionItem item;
			RTSPSessionManager *pMgr = (RTSPSessionManager *)RTSPSessionManagerSingleton::instance();

			if (pMgr->GetSessionItemByPeer(stream_connection_->socket().remote_endpoint().address().to_string(), stream_connection_->socket().remote_endpoint().port(), &item))
			{
				ObserverInfoPtr  pInfo = item.pUserData;
				if (pInfo)
				{
					pInfo->SetStreamSocket(stream_connection_);
					//stream_connection_->socket().set_option(boost::asio::ip::tcp::socket::reuse_address(true));
				}
			}
			else
			{
				X_LOG2(XLOG_ERROR, _T("无法找到所属的会话%s:%d"),
					CString(stream_connection_->socket().remote_endpoint().address().to_string().c_str()),
					stream_connection_->socket().remote_endpoint().port());
			}
			stream_connection_->start();
			stream_connection_.reset(new stream_connection(io_service_pool_.get_io_service()));
			video_acceptor_.async_accept(stream_connection_->socket(),
				boost::bind(&server::handle_video_accept, this,
				boost::asio::placeholders::error));
		}

		void server::handle_accept(const boost::system::error_code& e)
		{
			if (!e)
			{
				new_connection_->start();
				new_connection_.reset(new connection(io_service_pool_.get_io_service(), request_handler_));
				acceptor_.async_accept(new_connection_->socket(),
					boost::bind(&server::handle_accept, this,
					boost::asio::placeholders::error));
			}
		}

	} // namespace server3
} // namespace http
