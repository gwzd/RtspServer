
#pragma once

#include<string>
#include "Singleton.h"
#include "SessionManager1.h"
#include "SessionManager.h"
#include "IVideoSource.h"

#define NET_DVR_PLAYSTART			101//开始播放
#define NET_DVR_PLAYFILELIST		102//获取文件列表
#define NET_DVR_PLAYPAUSE			103//暂停播放
#define NET_DVR_PLAYRESTART			104//恢复播放
#define NET_DVR_PLAYFAST			105//快放
#define NET_DVR_PLAYSLOW			106//慢放
#define NET_DVR_PLAYNORMAL			107//正常速度
#define NET_DVR_PLAYFRAME			108//单帧放

#define NET_DVR_PLAYSETPOS			112//改变文件回放的进度
#define NET_DVR_PLAYGETPOS			113//获取文件回放的进度
#define NET_DVR_PLAYGETTIME			114//获取当前已经播放的时间
#define NET_DVR_PLAYGETFRAME		115//获取当前已经播放的帧数
#define NET_DVR_GETTOTALFRAMES  	116//获取当前播放文件总的帧数
#define NET_DVR_GETTOTALTIME    	117//获取当前播放文件总的时间
#define NET_DVR_THROWBFRAME			120//丢B帧

namespace RTSP {	
	namespace RTSPSrv {
		
		struct VideoSourceItem
		{		
			DWORD  sessionid;	
			std::string dvrip;
			std::string username;
			std::string password;
			std::string devtype;
			short streamtype;
			short channel;
			short port;
			std::string file;			
			VideoSource *pSource;
			boost::shared_ptr<PacketSender>sender;	
			BYTE   header[512];
			int nHeaderLength;
			time_t timestamp;
			bool stopped;
		};

		struct RTSPSessionItem;

		class VideoStreamManager:public SessionManager<VideoSourceItem,DWORD>
		{
		public:	
			VideoStreamManager();
			virtual ~VideoStreamManager();	
			BOOL CreateVideoStream(const RTSPSessionItem& item,VideoSourceItem& vitem);
			BOOL CreateVideoSource(const RTSPSessionItem& item,VideoSource **ppVideoSource);
			BOOL DoPTZCommand(const RTSPSessionItem& item,VideoSourceItem& vitem,DWORD command,const std::vector<DWORD>& params,const std::string& strProtocol);			
			BOOL DoExtendCommand(const RTSPSessionItem& item,VideoSourceItem& vitem,DWORD dwCommand,const std::vector<DWORD>& params,const std::string& param5,std::vector<header>& headers);
			//7.28 增加sessionID 
			//2012-10-26 增加端口号参数
			BOOL GetLiveStreamByDVR(int streamtype,int sessionID,LPCSTR ip,unsigned short port, short channel,const std::string& file,VideoSourceItem& item);
			BOOL AddObserver(DWORD dwSessionId,ObserverPtr info);	
			BOOL RemoveObserver(DWORD dwSessionId,ObserverPtr info);
			BOOL GetFileHeader(DWORD dwSessionId,unsigned char* pBuffer,unsigned long &Length);
			DWORD CreateSessionId() { return InterlockedIncrement(&dwSessionIdSeed); }	
			unsigned short VideoStreamManager::GetSenderPort(const RTSPSessionItem& item,VideoSourceItem &vitem);
			void StopAllVideoStream();
			BOOL getStreamByID(int sessionID,VideoSourceItem& item);//8.15  增加一个根据sessID获取vItem的函数
		protected:	
			BOOL DestroyRealStream(DWORD dwSessionId);
			BOOL CleanVideoItem( VideoSourceItem &vitem );
			bool RemoveInvalidSession();
			
		private:
			LONG dwSessionIdSeed;	
		};
		typedef Singleton<VideoStreamManager> VideoStreamManagerSingleton;
		
	}
}