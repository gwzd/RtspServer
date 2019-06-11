
#include "stdafx.h"
#include "Request_Preprocess.h"
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string/split.hpp> 
#include <boost/algorithm/string/classification.hpp>
#include <boost/regex.hpp>
#include "SessionManager.h"
#include "VideoSourceFactory.h"
#include "VideoManager.h"
#include "xlog.h"
#include "Prototypes.h"

using namespace std;
using namespace boost::algorithm; 

namespace RTSP {
	namespace RTSPSrv {

		const static commandMapItem commandMap[] =
		{
			{ methodSetup, "SETUP" },
			{ methodDescribe, "DESCRIBE" },
			{ methodAnnounce, "ANNOUNCE" },
			{ methodPlay, "PLAY" },
			{ methodTeardown, "TEARDOWN" },
			{ methodOptions, "OPTIONS" },
			{ methodGet, "GET" },
			{ methodPost, "POST" }
		};

// 		static BYTE pbHKHead[] = {	0x34,0x48,0x4b,0x48,0xfe,0xb3,0xd0,0xd6,0x08,0x03,0x04,0x20,0x00,0x00,0x00,0x00,
// 			0x01,0x10,0x01,0x10,0x01,0x10,0x10,0x00,0x80,0x3e,0x00,0x00,0x60,0x01,0x20,0x01,
// 			0x11,0x10,0x00,0x00,0x0e,0x00,0x00,0x00};

		static std::string& trim(std::string &s, const std::string& sub)
		{
			if (s.empty())		return s;

			s.erase(0, s.find_first_not_of(sub));
			s.erase(s.find_last_not_of(sub) + 1);
			return s;
		}

		Request_Preprocess::Request_Preprocess()
		{
		}

		CommandType Request_Preprocess::getCommandType(const request& req)
		{
			for (int i = 0; i<DIM(commandMap); i++)
			{
				if (req.method == commandMap[i].method)
					return commandMap[i].methodId;
			}
			return methodUnknown;
		}

		std::string Request_Preprocess::getHeadersItem(const request& req, const std::string& name)
		{
			for (std::size_t i = 0; i < req.headers.size(); ++i)
			{
				const header& h = req.headers[i];
				if (h.name == name)
					return h.value;
			}
			return std::string("");
		}

		bool Request_Preprocess::GetTransportInfo(RTSPSessionItem &item, const std::string &clientport, reply &rep)
		{
			std::string pattern = "RTP/AVP/(TCP|UDP);(destination=([\\d\\.]+);){0,1}unicast;client_port=(\\d{1,5})\\-(\\d{1,5})";
			//std::string pattern = "RTP/AVP/(TCP|UDP);destination=([\\d\\.]+);unicast;client_port=(\\d{1,5})\\-(\\d{1,5})";
			boost::regex expression(pattern);
			boost::cmatch what;

			item.conntype = TT_TCP;

			if (regex_match(clientport.c_str(), what, expression))
			{
				/*for(unsigned int i=0;i<what.size();i++)
				std::cout<<"参数:"<<i<<" "<<what[i].str()<<std::endl;*/
				if (what[1].str() == "UDP")
					item.conntype = TT_UDP;
				try	{
					if (!what[3].str().empty())
						item.recvip = what[3].str();

					//clientport
					item.clientport = boost::lexical_cast<int>(what[4].str());
				}
				catch (const boost::bad_lexical_cast& e)
				{
					UNREFERENCED_PARAMETER(e);
					rep.content = "错误的通讯参数,必须指定通讯协议和端口";
					return false;
				}
			}
			else
			{
				rep.content = "错误的通讯参数,必须指定通讯协议和端口";
				return false;
			}
			return true;
		}

		bool Request_Preprocess::getUriInfo(const string& uri, RTSPSessionItem& session)
		{
			//rtsp://172.20.43.49:554/172.20.43.41:8000:HIK-DS8000HC:0:0:admin:12345/av_stream/real RTSP/1.0
			//TODO:回放录像文件
			//rtsp://172.20.43.49:554/172.20.43.41:8000:HIK-DS8000HC:0:0:admin:12345/av_stream/file/e:\20090103100711-20090103235959-172.20.43.41-03-jans.mp4 RTSP/1.0
			//TODO:回放硬盘录像机录像文件
			//rtsp://172.20.43.49:554/172.20.43.41:8000:HIK-DS8000HC:0:0:admin:12345/av_stream/vod/20090103100711-20090103235959.mp4 RTSP/1.0	
			std::size_t rtsp_pos = uri.find("rtsp://");

			if (rtsp_pos == string::npos)	return false;

			//增加了盘符的支持,修改了DVRIP地址的限制,支持普通的字符及括号
			//	std::string pattern = "rtsp://(([0-9]{1,3}\\.[0-9]{1,3}\\.[0-9]{1,3}\\.[0-9]{1,3}(:\\d{1,5}){0,1}/)*)([\\w\\d.\\(\\)]+):([0-9]{1,5}):([\\w\\d-]{1,}):(\\d{1,5}):(\\d{1,5}):([\\w\\d]{1,10}):([\\w\\d]{1,10})/av_stream((/real)|(/file/([\\w&:\\\\\\-\\d.]{1,})))";				
			std::string pattern = "rtsp://(([0-9]{1,3}\\.[0-9]{1,3}\\.[0-9]{1,3}\\.[0-9]{1,3}(:\\d{1,5}){0,1}/)*)([\\w\\d.\\(\\)]+):([0-9]{1,5}):([\\w\\d-]{1,}):(\\d{1,5}):(\\d{1,5}):([\\w\\d]{1,10}):([\\w\\d]{1,15})/av_stream((/real)|(/file))";
			boost::regex expression(pattern);
			boost::cmatch what;
			if (boost::regex_match(uri.c_str(), what, expression))
			{
				//for(int i=0;i<what.size();i++)
				//	std::cout<<"参数:"<<i<<" "<<what[i].str()<<std::endl;
				try	{
					//如果只有一级，则不进行转发
					if (what[1].str().empty() || what[1].str().size() <= what[2].str().size())
						session.proxy.clear();
					else
					{
						//std::cout<<"转发路径1："<<what[1].str()<<std::endl;
						size_t pos = what[1].str().find_first_of("/");
						session.proxy = what[1].str().substr(pos + 1);
						trim(session.proxy, "/");
						//std::cout<<"转发路径2："<<session.proxy<<std::endl;
					}

					session.dvrip = what[4].str();
					session.port = boost::lexical_cast<unsigned short>(what[5].str());
					session.devtype = what[6].str();
					session.channel = boost::lexical_cast<short>(what[7].str());
					session.streamtype = boost::lexical_cast<short>(what[8].str());
					session.username = what[9].str();
					session.password = what[10].str();
					session.file = what[14].str();

					return true;
				}
				catch (const boost::bad_lexical_cast& e)
				{
					UNREFERENCED_PARAMETER(e);
					X_LOG1(XLOG_ERROR, _T("错误的请求参数:%s"), CString(uri.c_str()));
					return false;
				}
			}
			else
				X_LOG1(XLOG_DEBUG, _T("非法请求格式:%s"), CString(uri.c_str()));

			return false;
		}

		bool Request_Preprocess::process(const request& req, reply& rep)
		{
			string  strSquence = getHeadersItem(req,"CSeq");
			int sequence = -1;
			if(!strSquence.empty())
				sequence = boost::lexical_cast<int>(strSquence);
			string  request_type = getHeadersItem(req,"File");

			if(request_type.empty())	request_type = "real";			
			else						request_type = "file/"+request_type;

			switch(getCommandType(req))
			{
			case methodDescribe:
				{
					string sdp = 
						"v=0\r\n" 
//						"o=StreamingServer 3443478079 1109641162000 IN IP4 172.20.48.240\r\n"  
						"s=Dongfang Electric stream session\r\n"  
						"u=rtsp://www.dongfang-china.com\r\n"  
						"e=webscada@163.com\r\n"  
						"c=IN IP4 "+req.server_addr.to_string()+"\r\n"  
						"b=AS:94\r\n"  
						"t=0 0\r\n"  
						"a=control:*\r\n"  
						"a=range:npt=0-  70.00000\r\n"  
						"m=video 0 RTP/AVP 96\r\n"  
						"b=AS:79\r\n"  
						"a=rtpmap:96 X-SV3V-ES/90000\r\n"
						"a=control:"+request_type+"\r\n"
						"a=x-bufferdelay:4.97\r\n";			

					//设置回复结果
					rep.status = reply::ok;
					rep.content = sdp;					
					rep.headers.resize(6);
					rep.headers[0].name="CSeq";
					rep.headers[0].value= boost::lexical_cast<string>(sequence);	
					rep.headers[1].name="Cache-Control";
					rep.headers[1].value="must-revalidate";
					rep.headers[2].name="Content-length";
					rep.headers[2].value=boost::lexical_cast<string>((DWORD)sdp.size());	
					rep.headers[3].name="Content-Type";
					rep.headers[3].value="application/sdp";
					rep.headers[4].name="x-Accept-Retransmit";
					rep.headers[4].value="our-retransmit";
					rep.headers[5].name="x-Accept-Dynamic-Rate";
					rep.headers[5].value="1";
					//rep.headers[6].name="Content-Base";
					//rep.headers[6].value="rtsp://172.20.48.238/1.mov/";					
					return true;
				}
				break;
			case methodSetup:
				{
					RTSPSessionManager* pMgr = RTSPSessionManagerSingleton::instance();
					RTSPSessionItem item;					
					item.file.clear();
					item.proxy.clear();
					item.hSession = INVALID_DEV_HANDLE;					
					item.clientip = req.peer.address().to_string();		

					if(!pMgr)
					{
						rep.status = reply::internal_server_error;
						rep.content = "用户会话管理器初始化异常";
						X_LOG0(XLOG_ERROR,_T("用户会话管理器初始化错误."));						
						return true;
					}
					
					if(!getUriInfo(req.uri,item))		return false;
					
					//通道都从1开始
					if(item.channel<1)
					{
						rep.content = "错误的通道号,通道是以1开始的";
						return false;
					}

					item.dwSeqenceNumber = sequence;					
					item.sessionid = pMgr->createSessionId();					

					//设置回复结果
					rep.status = reply::ok;			
					rep.content.clear();
					rep.headers.resize(3);
					rep.headers[0].name="CSeq";
					rep.headers[0].value= boost::lexical_cast<string>(sequence);					
					rep.headers[1].name="Session";
					rep.headers[1].value=boost::lexical_cast<string>(item.sessionid);
					rep.headers[2].name="Transport";

					string clientport = getHeadersItem(req,"Transport");		

					if(!GetTransportInfo(item, clientport, rep))
						return true;//防止出现400 Bad Request错误

					VideoSourceItem vitem;
					
					//先取流
					if(!GetVideoStream(vitem,item, rep))
						return true; //防止出现400 Bad Request错误

					std::string strTransport;
					long lPortbase;

					if(item.conntype==TT_UDP)
					{												
						VideoStreamManager *pVideoMgr =  (VideoStreamManager *)VideoStreamManagerSingleton::instance();
						lPortbase = pVideoMgr->GetSenderPort(item,vitem);
						strTransport = format("source=%s;server_port=%d-%d",req.server_addr.to_string().c_str(),lPortbase,lPortbase+1);						
					}
					else if(item.conntype==TT_TCP)
					{
						//TODO:使用固定的视频端口12000
						strTransport = format("source=%s;server_port=%d-%d",req.server_addr.to_string().c_str(),12000,12001);						
					}

					rep.headers[2].value=getHeadersItem(req,"Transport")+";"+strTransport;										
					
					item.portbase = (WORD)lPortbase;	//仅用于Udp发送时用	
					if(item.recvip.empty())
							item.recvip=item.clientip;
					item.pUserData = boost::shared_ptr<ObserverInfo>(new ObserverInfo(item.recvip.c_str(),item.clientport,item.conntype));
					item.status = reply::ok;			

					time(&item.timestamp);
					item.hSession = (HANDLE)vitem.sessionid;	//8.15 setup时就将vitem的sessID赋值给item
					pMgr->AddSession(item);						
					return true;
				}
				break;
			case methodAnnounce:
				{					
					string session = getHeadersItem(req,"Session");
					int sessionid;

					try{
						sessionid = boost::lexical_cast<int>(session);
					}
					catch(const boost::bad_lexical_cast& e)
					{
						UNREFERENCED_PARAMETER(e);
						return false;
					}

					RTSPSessionItem item;
					RTSPSessionManager* pMgr = RTSPSessionManagerSingleton::instance();					
					if(!pMgr)
					{
						rep.status = reply::internal_server_error;
						rep.content = "用户会话管理器初始化异常";
						X_LOG0(XLOG_ERROR,_T("用户会话管理器初始化错误."));
						return true;
					}

					if(!pMgr->GetSessionByHandle(sessionid,item))
						return false;
					
					time(&item.timestamp);

					if(!pMgr->SetSessionByHandle(sessionid,item))
						return false;

					//设置回复结果
					rep.status = item.status;
					rep.content.clear();
					rep.headers.resize(2);
					rep.headers[0].name="CSeq";
					rep.headers[0].value= boost::lexical_cast<string>(sequence);	
					rep.headers[1].name="Session";
					rep.headers[1].value= boost::lexical_cast<string>(sessionid);						
					return true;
				}
				break;
			case methodPlay:
				{
					string session = getHeadersItem(req,"Session");
					int sessionid;					
					
					try{
						sessionid = boost::lexical_cast<int>(session);
					}
					catch(const boost::bad_lexical_cast& e)
					{
						UNREFERENCED_PARAMETER(e);
						return false;
					}	

					RTSPSessionItem item;
					RTSPSessionManager* pMgr = RTSPSessionManagerSingleton::instance();
					if(!pMgr)
					{
						rep.status = reply::internal_server_error;
						rep.content = "用户会话管理器初始化异常";
						X_LOG0(XLOG_ERROR,_T("用户会话管理器初始化错误."));
						return true;
					}

					if(!pMgr->GetSessionByHandle(sessionid,item))
						return false;

					VideoSourceItem vitem;
					//8.15 add	
					VideoStreamManager *pVideoMgr = (VideoStreamManager *)VideoStreamManagerSingleton::instance();
					if (!pVideoMgr)
					{
						rep.status = reply::internal_server_error;
						rep.content = "视频源管理器初始化异常！";
						X_LOG0(XLOG_FATAL, _T("视频源管理器初始化异常！"));
						return false;
					}

					if (!pVideoMgr->getStreamByID((int)item.hSession, vitem))
					{
						//创建流失败，或者创建发送类失败
						rep.status = reply::connect_time_out;
						rep.content = "获取视频流或配置发送端口时失败!";
						return true; //防止出现400 Bad Request错误
					}
					////8.15 add

					//8.15 注释掉下面的内容
//   				   if(!GetVideoStream(vitem,item, rep))
// 				   {
// 					   //创建流失败，或者创建发送类失败
// 					   rep.status = reply::connect_time_out;
// 					   rep.content = "获取视频流或配置发送端口时失败!";					   
// 					   return true; //防止出现400 Bad Request错误
// 				   }										
// 					
// 					item.hSession = (HANDLE)vitem.sessionid;	

					X_LOG4(XLOG_DEBUG,_T("数据接收客户端(session=%d,hsession=%d)地址[%s:%d]"),sessionid,vitem.sessionid,CString(item.recvip.c_str()),item.clientport);

					if(item.pUserData==NULL)
						item.pUserData = boost::shared_ptr<ObserverInfo>(new ObserverInfo(item.clientip.c_str(),item.clientport,item.conntype));

					pMgr->SetSessionByHandle(sessionid,item);				

					if(vitem.sender)
					{											
						//创建RTP会话  8.15 注释					
// 						VideoStreamManager *pVideoMgr =  (VideoStreamManager *)VideoStreamManagerSingleton::instance();										
// 						if(!pVideoMgr)
// 						{
// 							rep.status = reply::internal_server_error;
// 							rep.content = "视频源管理器初始化异常！";
// 							X_LOG0(XLOG_FATAL,_T("视频源管理器初始化异常！"));
// 							return false;
// 						}	

						vitem.sender->addObserver(item.pUserData);
						pVideoMgr->SetSessionByHandle(vitem.sessionid,vitem);
						//cout<<"item.channel:"<<item.channel<<" item.streamtype:"<<item.streamtype<<std::endl;
						vitem.pSource->PrepareStream(item.channel,item.streamtype);

						rep.status = reply::ok;																													
						
						//没有获取过是-1
						if(vitem.nHeaderLength<0)
						{
							//2016.9.17 zxl 取消打印
							X_LOG1(XLOG_DEBUG,_T("从插件获取文件头,插件类型:[%s]"),CString(item.devtype.c_str()));

							BOOL bRet=FALSE;
							//没有文件头的话重试3次
							int nRetrys = 3;
							do
							{
								//先传入后传出
								unsigned long nBufferLength = 512;	
								bRet = pVideoMgr->GetFileHeader(vitem.sessionid,vitem.header,nBufferLength);
								if(!bRet)
								{
									Sleep(1000);
									nRetrys--;
								}
								else
								{
									//成功置数组内容
									vitem.nHeaderLength = nBufferLength;
									pVideoMgr->SetSessionByHandle(vitem.sessionid,vitem);
									break;
								}
							} while(nRetrys>0&&!bRet);

							if(!bRet)
								X_LOG1(XLOG_WARNING,_T("插件类型[%s]获取文件头失败!"),CString(item.devtype.c_str()));
						}
						else
							X_LOG1(XLOG_DEBUG,_T("从缓存中直接取文件头,插件类型:[%s]"),CString(item.devtype.c_str()));

						if(vitem.nHeaderLength>0)
								rep.content.append((const char*)vitem.header,vitem.nHeaderLength);
						else	rep.content.clear();

						rep.headers.resize(2);
						rep.headers[0].name="CSeq";
						rep.headers[0].value= boost::lexical_cast<string>(sequence);	
						rep.headers[1].name="Content-Length";
						rep.headers[1].value=boost::lexical_cast<string>(vitem.nHeaderLength);							
						return true;
					}
					else
					{
						item.pUserData.reset();
						pMgr->SetSessionByHandle(sessionid,item);
					}
				}
				break;
			case methodTeardown:
				//TODO:停止发送，关闭会话,清理资源,
				//设置回复结果
				{					
					string session = getHeadersItem(req,"Session");
					int sessionid = -1;
					if(session.empty())
					{
						session = getHeadersItem(req,"User-agent");
					}										
					try{
							sessionid = boost::lexical_cast<int>(session);
					}
					catch(const boost::bad_lexical_cast& e)
					{
						UNREFERENCED_PARAMETER(e);
						X_LOG1(XLOG_FATAL,_T("解析会话数据:%s"),CString(session.c_str()));
						return false;
					}

					RTSPSessionItem item;					
					RTSPSessionManager* pMgr = RTSPSessionManagerSingleton::instance();
					if(!pMgr)
					{
						rep.status = reply::internal_server_error;
						rep.content = "用户会话管理器初始化异常";
						X_LOG0(XLOG_ERROR,_T("用户会话管理器初始化错误."));
						return true;
					}

					if(!pMgr->GetSessionByHandle(sessionid,item))
					{
						X_LOG1(XLOG_ERROR,_T("无效的Session[%d]！"),sessionid);
						return false;
					}
					rep.status = reply::ok;
					rep.content.clear();
					rep.headers.resize(1);
					rep.headers[0].name="CSeq";
					rep.headers[0].value= boost::lexical_cast<string>(sequence);
					VideoStreamManager *pVideoMgr =  (VideoStreamManager *)VideoStreamManagerSingleton::instance();
					if(!pVideoMgr)
					{
						rep.status = reply::internal_server_error;
						rep.content = "视频源管理器初始化异常！";
						X_LOG0(XLOG_FATAL,_T("视频源管理器初始化异常！"));
						return true;
					}			

					pMgr->CleanSession(sessionid);
					return true;
				}
								
				break;
			case methodOptions:
				{					
					string  command= getHeadersItem(req,"command");
					string  session= getHeadersItem(req,"Session");
					string  stop= getHeadersItem(req,"param1");
					string  speed= getHeadersItem(req,"param2");
					string  param3= getHeadersItem(req,"param3");
					string  param4= getHeadersItem(req,"param4");					
					string  param5 = getHeadersItem(req,"param5");					
					
					DWORD dwCommand;
					DWORD dwStop;
					DWORD dwSpeed;
					DWORD dwParam3;
					DWORD dwParam4;
					long  lSessionId;
					vector<DWORD> params;

					try{
						dwCommand = boost::lexical_cast<DWORD>(command);
						lSessionId = boost::lexical_cast<long>(session);	
						dwStop = boost::lexical_cast<DWORD>(stop);
						dwSpeed = boost::lexical_cast<DWORD>(speed);
						dwParam3 = boost::lexical_cast<DWORD>(param3);
						dwParam4 = boost::lexical_cast<DWORD>(param4);											
						params.push_back(dwStop);
						params.push_back(dwSpeed);					
						params.push_back(dwParam3);					
						params.push_back(dwParam4);					
					}
					catch(const boost::bad_lexical_cast& e)
					{
						UNREFERENCED_PARAMETER(e);
						return false;
					}
												
					RTSPSessionItem item;
					RTSPSessionManager* pMgr = RTSPSessionManagerSingleton::instance();
					if(!pMgr)
					{
						rep.status = reply::internal_server_error;
						rep.content = "用户会话管理器初始化异常";
						X_LOG0(XLOG_ERROR,_T("用户会话管理器初始化错误."));
						return true;
					}

					if(!pMgr->GetSessionByHandle(lSessionId,item))
						return false;
					
					//cout<<"command:"<<dwCommand<<"("<<dwStop<<","<<dwSpeed<<","<<param3<<","<<param4<<")"<<std::endl;					

					VideoStreamManager *pVideoMgr =  (VideoStreamManager *)VideoStreamManagerSingleton::instance();					
					if(!pVideoMgr)
					{
						rep.status = reply::internal_server_error;
						rep.content = "视频源管理器初始化异常！";
						X_LOG0(XLOG_FATAL,_T("视频源管理器初始化异常！"));
						return true;
					}
					
					VideoSourceItem vitem;
					if(!pVideoMgr->GetSessionByHandle((DWORD)item.hSession,vitem))
					{
						X_LOG3(XLOG_ERROR, _T("无法获取连接的视频流信息.IP:%s,通道:%d,文件:%s"), CString(item.dvrip.c_str()), item.channel, CString(item.file.c_str()));
						return false;
					}	 

					if( dwCommand >= NET_DVR_PLAYPAUSE && dwCommand <= NET_DVR_GETTOTALTIME )
					{
						vector<header> result;
						if(pVideoMgr->DoExtendCommand(item,vitem,dwCommand,params,param5,result))	
						{							
							rep.status = reply::ok;							
							if(!result.empty())
							{
								rep.content.clear();
								rep.headers.resize(2);
								rep.headers[0].name="CSeq";
								rep.headers[0].value= boost::lexical_cast<string>(sequence);
								rep.headers[1].name="Content-Length";
								rep.headers[1].value=boost::lexical_cast<string>(result[0].value.length());
								rep.content.append(result[0].value);
							}
							return true;
						}
						else
						{
							rep.status = reply::unsupported; 
							rep.content.clear();
							rep.headers.resize(1);
							rep.headers[0].name="CSeq";
							rep.headers[0].value= boost::lexical_cast<string>(sequence);
							return true;
						}
					}
					else if(pVideoMgr->DoPTZCommand(item,vitem,dwCommand,params,param5))					
					{
						rep.status = reply::ok;
						rep.content.clear();
						rep.headers.resize(1);
						rep.headers[0].name = "CSeq";
						rep.headers[0].value = boost::lexical_cast<string>(sequence);
						return true;
					}
					else
					{
						rep.status = reply::unsupported;
						rep.content.clear();
						rep.headers.resize(1);
						rep.headers[0].name="CSeq";
						rep.headers[0].value= boost::lexical_cast<string>(sequence);
						return true;
					}
				}
				break;				
			case methodGet:
				{
					RTSPSessionItem item;						
					
					if(!getUriInfo(req.uri,item))	return false;

					if(item.file!="QUERYLIST")
					{
						rep.status = reply::unsupported;
						return true;
					}
					else
					{
						VideoStreamManager *pVideoMgr =  (VideoStreamManager *)VideoStreamManagerSingleton::instance();
						VideoSourceItem vitem;
						vitem.pSource = NULL;						
						
						if(!pVideoMgr->CreateVideoSource(item,&vitem.pSource))						
						{	
							rep.status = reply::connect_time_out;
							return true;
						}

						vector<header> result;
						vector<DWORD> params;
						DWORD dwRecType = ALL_REC;
						string  strRecType = getHeadersItem(req,"RecordType");
						string  strDateRange = getHeadersItem(req,"DateRange");
						if(!strRecType.empty())
							dwRecType = boost::lexical_cast<DWORD>(strRecType);
						if(strDateRange.empty())
							strDateRange = "20091001000000-29991231235959";
						params.push_back(dwRecType);

						if(pVideoMgr->DoExtendCommand(item,vitem,NET_DVR_PLAYFILELIST,params,strDateRange,result))
						{														
							if(!result.empty())
							{
								rep.status =reply::ok;
								rep.content.clear();
								rep.headers.resize(1);								
								rep.headers[0].name="Content-Length";
								rep.headers[0].value=boost::lexical_cast<string>(result[0].value.length());
								rep.content = result[0].value;								
							}
							else	rep.status = reply::not_found;
						}
						else	rep.status = reply::service_unavailable;

						if(vitem.pSource)
						{
							vitem.pSource->DisconnectDevice();
							delete vitem.pSource;
						}
						return true;
					}
				}
				break;
			default:				
				return false;
			}
			return false;
		}

		bool Request_Preprocess::GetVideoStream(VideoSourceItem &vitem, const RTSPSessionItem &item, reply &rep) 
		{
			//创建RTP会话					
			VideoStreamManager *pVideoMgr =  (VideoStreamManager *)VideoStreamManagerSingleton::instance();										
			if(!pVideoMgr)
			{
				rep.status = reply::internal_server_error;
				rep.content = "视频源管理器初始化异常！";
				X_LOG0(XLOG_FATAL,_T("视频源管理器初始化异常！"));
				return false;
			}	

			//std::cout<<"\n播放的是:"<<(item.file.empty()?"实时流！\n":item.file)<<std::endl;

//			if(!pVideoMgr->GetLiveStreamByDVR(item.dvrip.c_str(),item.channel,item.file,vitem))										
			//7.28  增加sessionID的处理
			//2012-10-26 增加端口号参数
			if(!pVideoMgr->GetLiveStreamByDVR(item.streamtype,item.sessionid,item.dvrip.c_str(),item.port,item.channel,item.file,vitem))										
			{
				try{
					if(!pVideoMgr->CreateVideoStream(item,vitem))						
					{
						rep.status = reply::connect_time_out;
						rep.content = "无法连接设备！";
						return false;
					}
				}
				catch(...)
				{
					X_LOG3(XLOG_FATAL, _T("创建视频流时发生异常!设备类型:%s,IP:%s通道:%d"), CString(item.devtype.c_str()), CString(item.dvrip.c_str()), item.channel);
					rep.status = reply::service_unavailable;
					return false;
				}						
			}
			else
			{
				LOG4CPLUS_DEBUG(logger, _T("获取共享流,IP:") << CString(item.dvrip.c_str()) << _T(" 通道:") << item.channel << std::endl);
//				S_LOG(DEBUG, _T("获取共享流,IP:") << CString(item.dvrip) << _T(" 通道:") << item.channel << std::endl);
			}
			return true;
		}
	}
}

