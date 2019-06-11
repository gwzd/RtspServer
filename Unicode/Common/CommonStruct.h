
#pragma once

//////////////////////////////////////////////////////////////////////////
//通用接口相关定义

//宏定义
#define MAX_NAME				64
#define MAX_CHANNEL				128
#define DEF_LEN_64				64
#define DEF_LEN_512				512
#define MAXLEN_IP				16
#define MAXLEN_STR				50

/*
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
*/

//网络传输类型
enum TRANSTYPE
{
	TT_TCP = 0,              //	TCP方式直连
	TT_UDP = 1,              //	UDP方式直连
	TT_MULTICAST = 2,        //	组播方式直连
	TT_TCP_TRANS = 3,        //	TCP方式流媒体转发
	TT_UDP_TRANS = 4,        //	UDP方式流媒体转发
	TT_MULTICAST_TRANS = 5   //	组播方式流媒体转发
};

//报警类型
enum ALARMTYPE
{
	ALARM_IO = 0,						//	I/O信号量报警，dwChnNo：报警序号，从1开始
	ALARM_DISKFULL = 1,					//	硬盘满，dwChnNo：硬盘号，从1开始
	ALARM_VIDEOLOST = 2,				//	视频丢失，dwChnNo：视频通道号，从1开始
	ALARM_VIDEOMOTION = 3,				//	视频移动侦测，dwChnNo：视频通道号
	ALARM_DISKNOTFORMATTED = 4,			//	硬盘未格式化，dwChnNo：硬盘号
	ALARM_DISKERROR = 5,				//	硬盘错误，dwChnNo：硬盘号
	ALARM_VIDEOSHELTER = 6,				//	视频遮挡报警，dwChnNo：视频通道号
	ALARM_VIDEOFORMAT = 7,				//	视频制式不匹配，dwChnNo：视频通道号
	ALARM_ILLEGALCONN = 8,				//	非法访问设备
	ALARM_VCA_TRAVERSE_PLANE = 1001,	//	穿越警戒面
	ALARM_VCA_ENTER_AREA = 1002,		//	进入区域
	ALARM_VCA_EXIT_AREA = 1003,			//	离开区域
	ALARM_VCA_INTRUSION = 1004,			//	周界入侵
	ALARM_VCA_LOITER = 1005,			//	徘徊
	ALARM_VCA_LEFT_TAKE = 1006,			//	丢包、捡包
	ALARM_VCA_PARKING = 1007,			//	停车
	ALARM_VCA_RUN = 1008,				//	奔跑
	ALARM_VCA_HIGH_DENSITY = 1009,		//	区域内人员密度
	ALARM_VCA_VIOLENT_MOTION = 1010,	//	剧烈运动检测
	ALARM_VCA_REACH_HIGHT = 1011,		//	攀高检测
	ALARM_VCA_GET_UP = 1012,			//	起身检测
	ALARM_VCA_TARRY = 1013,				//	人员滞留
	ALARM_VCA_STICK_UP = 1014,			//	贴纸条
	ALARM_VCA_INSTALL_SCANNER = 1015	//	安装读卡器
};

//串口类型
enum COMTYPE
{
	COM_NONE = 0,       //无
	COM_232 = 1,        //透明232串口
	COM_485 = 2,        //485口
};

//摄像机切换类型
enum SWITCHTYPE
{
	SWITCH_MANUAL = 0,     //手动切换
	SWITCH_AUTO = 1,       //通用自动切换
	SWITCH_SYN = 2,        //同步切换
	SWITCH_GROUPAUTO = 3,  //群组自动切换
};

//I/O类型
enum IOTYPE
{
	IO_IN = 0,     //输入
	IO_OUT = 1,    //输出
};

//云台镜头控制类型
enum CTRLTYPE
{
	CT_STOP = 0,				//动作停止
	CT_TILT_UP = 1,				//云台上仰
	CT_TILT_DOWN = 2,			//云台下俯
	CT_PAN_LEFT = 3,			//云台左转
	CT_PAN_RIGHT = 4,			//云台右转
	CT_PAN_AUTO = 5,			//云台自动
	CT_ZOOM_IN = 6,				//倍率变大
	CT_ZOOM_OUT = 7,			//倍率变小
	CT_FOCUS_NEAR = 8,			//焦点前调
	CT_FOCUS_FAR = 9,			//焦点后调
	CT_IRIS_ENLARGE = 10,		//光圈扩大
	CT_IRIS_SHRINK = 11,		//光圈缩小
	CT_CALL_PRESET = 12,		//转到预置
	CT_SET_PRESET = 13,			//设置预置
	CT_CLE_PRESET = 14,			//清除预置
	CT_AUX1_PWRON = 15,			//辅助开关1开
	CT_AUX1_PWROFF = 16,		//辅助开关1关
	CT_AUX2_PWRON = 17,			//辅助开关2开
	CT_AUX2_PWROFF = 18,		//辅助开关2关
	CT_AUX3_PWRON = 19,			//辅助开关3开
	CT_AUX3_PWROFF = 20,		//辅助开关3关
	CT_AUX4_PWRON = 21,			//辅助开关4开
	CT_AUX4_PWROFF = 22,		//辅助开关4关
	CT_TILT_LEFTUP = 23,		//云台左上
	CT_TILT_RIGHTUP = 24,		//云台右上
	CT_TILT_LEFTDOWN= 25,		//云台左下
	CT_TILT_RIGHTDOWN= 26,		//云台右下
	CT_CRUISE_ADD=30,			//巡航：增加预置点
	CT_CRUISE_DWELL=31,			//巡航：设置停顿时间
	CT_CRUISE_SPEED=32,			//巡航：设置巡航速度
	CT_CRUISE_REMOVE=33,		//巡航：从巡航队列中删除预置点
	CT_CRUISE_RUN=34,			//巡航：运行
	CT_CRUISE_STOP=35,			//巡航：停止
	CT_TRACK_STARTREC=40,		//轨迹：开始记录
	CT_TRACK_STOPREC=41,		//轨迹：停止记录
	CT_TRACK_RUN=42,			//轨迹：运行
	CT_RECTZOOM=50				//区域缩放（3D定位）
};

//录像类型
enum RECORDTYPE
{
	MANUAL_REC = 0, //手动录像
	PLAN_REC = 1,   //计划录像
	ALARM_REC = 2,  //报警录像
	ALL_REC = 3,    //全部录像
};

//录像时间类型
typedef struct tagQueryTime
{
	unsigned int ufromyear;		// 起始年
	unsigned int ufrommonth;	// 起始月
	unsigned int ufromday;		// 起始日
	unsigned int ufromhour;		// 起始时
	unsigned int ufromminute;	// 起始分
	unsigned int ufromsecond;	// 起始秒
	unsigned int utoyear;		// 终止年
	unsigned int utomonth;		// 终止月
	unsigned int utoday;		// 终止日
	unsigned int utohour;		// 终止时
	unsigned int utominute;		// 终止分
	unsigned int utosecond;		// 终止秒
}QUERYTIME, *PQUERYTIME;

//模块信息
typedef struct { 
	char	description[128];		//模块描述
	char	ssoftversion[64];		//软件版本
	char	smanufactuer[64];		//生产厂家  
	char	DeviceTypeName[128];	//设备型号
	long invalid_handle_value;		//默认-1
}PLUGIN_INFO,*PPLUGIN_INFO;

//设备信息
typedef struct tagDeviceInfo
{
	char sdevicetype[DEF_LEN_64];	//设备型号
	char ssoftversion[DEF_LEN_64];	//软件版本
	char smanufactuer[DEF_LEN_64];	//生产厂家
	int nchannelnum;				//主通道数量
	int ndisknumber;				//磁盘数量
	int nioinputnum;				//IO输入数量
	int niooutputnum;				//IO输出数量
}SDEVICEINFO,*PDEVICEINFO;

//通道信息
typedef struct tagChannelNode
{
	char sname[MAX_NAME];			//通道名称
	DWORD dwReserve1;
}SCHANNODE,*PCHANNODE;

typedef struct tagChannelInfo
{
	unsigned int uchannels;			//通道总数
	SCHANNODE schannode[MAX_CHANNEL];
}CHANNELINFO,*PCHANNELINFO;

typedef struct
{
	char szProxySvrIP[16];	//转发服务器IP地址
	WORD wProxySvrPort;		//转发服务器端口号
	DWORD dwReserved;		//保留参数
}VIDEOPROXY, *PVIDEOPROXY;

//时间结构指针
typedef struct tagDevTime
{
	unsigned int ufromyear;		// 年
	unsigned int ufrommonth;	// 月
	unsigned int ufromday;		// 日
	unsigned int ufromhour;		// 时
	unsigned int ufromminute;	// 分
	unsigned int ufromsecond;	// 秒
}DEVTIME, *PDEVTIME;

//设备通道状态结构指针
typedef struct{
	BYTE byRecordStatic;		 //通道是否在录像,0-不录像,1-录像
	BYTE bySignalStatic;		 //连接的信号状态,0-正常,1-信号丢失
	DWORD dwBitRate;			 //实际码率
	DWORD dwLinkNum;			 //客户端连接的个数
}DEV_CHANNELSTATE,*LPDEV_CHANNELSTATE;

typedef struct{
	DWORD dwVolume;				 //硬盘的容量
	DWORD dwFreeSpace;			 //硬盘的剩余空间
	DWORD dwHardDiskStatic;		 //硬盘的状态,休眠,活动,不正常等,1-休眠,2-不正常,3-休眠硬盘出错
}DEV_DISKSTATE,*LPDEV_DISKSTATE;

typedef struct{
	DEV_DISKSTATE  struDiskState[16];	//硬盘状态
	DEV_CHANNELSTATE struChanState[32];	//通道的状态
	BYTE  byAlarmInStatus[32];			//报警端口的状态,0-没有报警,1-有报警
	BYTE  byAlarmOutStatus[16];			//报警输出端口的状态,0-没有输出,1-有报警输出
}DEV_STAT,*LPDEV_STAT;

typedef struct
{
	DWORD dwVal;					//报警标志（1：报警，0：报警消失）--- 为了与早期版本兼容
	char  szAlmDevIP[MAXLEN_IP];	//报警设备ip
	int	  nAlmDevChan;				//报警设备通道号,1开始
	int	  nRuleID;					//HK iVms规则号，0～7
	char  szRuleName[32];			//HK iVms规则名称
	char  szDes[DEF_LEN_512];		//报警描述
	DWORD dwPicLen;					//报警抓拍图片大小
	BYTE* pPicData;					//报警抓拍图片数据(jpg)
}ALM_PARAM, *PALM_PARAM;

