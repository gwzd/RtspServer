#ifndef SHOWREMOTE_HC
#define SHOWREMOTE_HC

#ifdef SHOW_HCREMOTE_EXPORT
#define NET_SDK_API extern "C"__declspec(dllexport)
#else
#define NET_SDK_API  extern "C"__declspec(dllimport)
#endif

//最大主节点数
#define MAX_MAINNODENUM			15
//最大子结点数
#define MAX_SUBNODENUM			20

//远程配置库界面风格
#define STYLE_9000				1
#define STYLE_8000				2

//重启客户端消息
#define UM_MSG_REBOOTSOFTWEAR	WM_USER+1100


//语言类型说明
#define HC_LANGUAGE_CHI		0
#define HC_LANGUAGE_ENG		1
#define HC_LANGUAGE_TRAD    2
#define HC_LANGUAGE_RUS		3
#define HC_LANGUAGE_PLS		4
#define HC_LANGUAGE_FRCH    5
#define HC_LANGUAGE_SPA		6
#define HC_LANGUAGE_CORA	7
#define HC_LANGUAGE_GMN		8
#define HC_LANGUAGE_ITAL	9

#define HC_LANGUAGE_CZECH	13
#define HC_LANGUAGE_SVK		14
#define HC_LANGUAGE_DUTCH	15


typedef struct _RemConfigPara
{
	LONG lServerID;				//用户ID
	char sUserName[32];			//用户名称
	int iLanguageType;			//语言类型
	BOOL bShowIPMode;			//显示IP模块配置
	HWND hParentWnd;			//远程配置的父窗口
	COLORREF crBKColor;			//远程配置背景色(保留)
	COLORREF crFontColor;		//远程配置字体颜色(保留)
	BYTE byStyle;				//远程配置库界面风格(保留)
	BYTE byDecoder;				//是否为解码器，0-不是，1-是新解码器，2-是老解码器	
	BYTE byRes2[32];		
}NET_DVR_REMCONFIGPARA, *LPNET_DVR_REMCONFIGPARA;


//调用顺序说明：
//////////////////////////////////////////////////////////////////////////
/************************************************************************/
/* 
调用顺序：

	NET_DVR_SetRemConfigNode ->	NET_DVR_ShowRemConfigWnd


	在调用NET_DVR_ShowRemConfigWnd之前需要先调用NET_DVR_SetRemConfigNode
	以确定所要调用的远程配置的配置内容

	eg:调用一个有IP模块通道配置的并且配置内容完整的远程配置

	//显示IP通道配置界面
	NET_DVR_REMCONFIGPARA stRemConfigPara;
	stRemConfigPara.lServerID   = m_lUserID;
	strcpy(stRemConfigPara.sUserName, m_sUserName);
	stRemConfigPara.iLanguageType = HC_LANGUAGE_CHI;
	stRemConfigPara.bShowIPMode = TRUE;

	//设置具有全部内容的远程配置

	NET_DVR_SetRemConfigNode(0xff, 0xff, TRUE);

	//调用远程配置
	NET_DVR_ShowRemConfigWnd(&stRemConfigPara);

                                                                     */
/************************************************************************/


//接口说明
//////////////////////////////////////////////////////////////////////////

/************************************************************************/
/* 
  Function:    		NET_DVR_SetRemConfigNode(int iMainNode, int iSubNode, BOOL bEnable = TRUE)
  
  Description:		设置远程配置的配置项

  Input:			int iMainNode	---		配置树的主节点索引值
					int iSubNode	---		配置树的子节点索引值
					BOOL bEnable	---		启用标识符	

  OutPut:			NULL

  Return:			BOOL		---		保留
                                                                     */
/************************************************************************/
NET_SDK_API BOOL __stdcall NET_DVR_SetRemConfigNode(int iMainNode, int iSubNode, BOOL bEnable = TRUE);


/************************************************************************/
/* 
附加说明:

	iMainNode从 0 ~  11 分别代表远程配置各个配置的根节点,0xff代表全部根节点
	
	即：设备参数、通道参数、网络参数、串口参数、报警参数、异常参数、用户管理、
		交易配置、远程升级、硬盘管理、6467ATM配置(当设备类型为DS81XX_AH_S才有效)
		码分器配置(当从矩阵配置进入编码器配置时有效)

	iSubNode从1 ~ 19 分别代表跟各个根节点下的子配置节点,0xff代表全部字节点

		想要添加子节点配置，根节点配置必须是启用状态
		例如：要能添加配置的移动侦测功能，移动侦测功能是通道配置的一个子配置功能
		则通道参数必须启用，即：NET_DVR_SetRemConfog(1, 0, TRUE);
	
	iMainNode = 0,iSubNode = 0
	iSubNode从1 ~ 2	对应于设备参数的设备信息、版本信息。

	iMainNode = 1,
	iSubNode从1 ~ 8	对应于通道参数的显示设置、视频设置、
									录像计划、移动侦测、视频丢失、
									遮挡报警、视频遮盖、字符叠加

	iMainNode = 2,
	iSubNode从1 ~ 6	对应于网络参数的网络设置、PPPOE设置
									DDNS设置、NTP设置、NFS设置、
									Email设置
	iMainNode = 3,
	iSubNode从1 ~ 2 对应于串口参数的RS232设置、RS485设置

	iMainNode = 4,
	iSubNode从1 ~ 2 对应于报警参数的报警输入、报警输出


	使用方法说明:
	eg1:只有录像计划的远程配置
	NET_DVR_SetRemConfigNode(1, 0, TRUE);	//启用通道参数
	NET_DVR_SetRemConfigNode(1, 3, TRUE);	//启用录像计划

	eg2:只有全部网络参数配置的远程配置
	NET_DVR_SetRemConfigNode(2, 0xff, TRUE);

	eg3:只缺少全部网络参数配置的远程配置
	NET_DVR_SetRemConfigNode(0xff, 0xff, TRUE);//全部配置
	NET_DVR_SetRemConfigNode(2, 0xff, FALSE);	//去掉网络配置

                                                                     */
/************************************************************************/




/************************************************************************/
/* 
  Function:    		NET_DVR_ShowRemConfigWnd(LPNET_DVR_REMCONFIGPARA pstRemConfigPara);
  
  Description:		调用远程配置

  Input:			LPNET_DVR_REMCONFIGPARA pstRemConfigPara	----	远程配置参数结构体指针

  OutPut:			NULL

  Return:			BOOL
                                                                     */
/************************************************************************/
NET_SDK_API BOOL __stdcall NET_DVR_ShowRemConfigWnd(LPNET_DVR_REMCONFIGPARA pstRemConfigPara);
#endif 

