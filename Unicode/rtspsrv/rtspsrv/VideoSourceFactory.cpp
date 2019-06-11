#include "stdafx.h"
#include "IVideoSource.h"
#include "VideoSourceFactory.h"
#include "PlugInVideoSource.h"
namespace RTSP {
	namespace RTSPSrv {

		VideoSource*  CVideoSourceFactory::createVideoSource(LPCSTR devicetype)
		{
			VideoSource* pVideoSource = NULL;
			//2016.9.17 zxl 取消打印
			//X_LOG1(XLOG_DEBUG,_T("创建视频源对象[%s]"),devicetype);
			if (stricmp("TRANS", devicetype) == 0)
					return createPlugInVideoSource("TRANS", "");
			else	return createPlugInVideoSource(devicetype, "DEVICE");
		}
	}
}

  