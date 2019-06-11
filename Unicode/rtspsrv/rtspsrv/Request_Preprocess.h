#pragma once
#include <boost/noncopyable.hpp>
#include "reply.hpp"
#include "request.hpp"
#include <string>
#include "SessionManager.h"
#include "VideoManager.h"
namespace RTSP {

	template<typename T, size_t N> 
		unsigned char (& dimension_help_fun(T(&ts)[N]))[N]; 
	#define DIM(a) (sizeof(dimension_help_fun(a)))

	namespace RTSPSrv {
		enum CommandType{
			methodUnknown =-1,
			methodSetup =0,
			methodDescribe=1,
			methodAnnounce=2,
			methodPlay=3,
			methodTeardown=4,
			methodOptions=5,
			methodGet=6,
			methodPost=7
		};

		struct commandMapItem
		{
			CommandType methodId;
			char method[250];
		};


		class Request_Preprocess
			: private boost::noncopyable
		{
		public:
			explicit Request_Preprocess();
			bool process(const request& req,reply& rep);
			bool GetTransportInfo( RTSPSessionItem &item, const std::string &clientport, reply &rep );			
			bool GetVideoStream(VideoSourceItem &vitem, const RTSPSessionItem &item, reply &rep); 			
			CommandType getCommandType(const request& req);
			std::string getHeadersItem(const request& req,const std::string& name);
			bool getUriInfo(const std::string& uri,RTSPSessionItem& session);				
		protected:
			
		private:			
			
		};
	}
}