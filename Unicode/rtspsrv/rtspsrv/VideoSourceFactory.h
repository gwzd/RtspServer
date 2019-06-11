#pragma once

#include "Singleton.h"
#include "IVideoSource.h"

namespace RTSP {
	namespace RTSPSrv {
		class CVideoSourceFactory{
			public:
				VideoSource* createVideoSource(LPCSTR devicetype);				
		};

		typedef Singleton<CVideoSourceFactory> CVideoSourceFactorySingleton;
	}
}