
// TestRtspClientDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "TestRtspClient.h"
#include "TestRtspClientDlg.h"
#include "afxdialogex.h"
#include "client.h"
#include <string>
#include <vector>
#include <functional>
#include <mmsystem.h>

using namespace std;

#pragma comment(lib,"Winmm.lib")

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CString MyGetProfileString(LPCTSTR lpszSection, LPCTSTR lpszKey, LPCTSTR lpszDefault = NULL)
{
	TCHAR buf[_MAX_PATH];
	TCHAR szRet[_MAX_PATH];
	GetModuleFileName(AfxGetApp()->m_hInstance, buf, _MAX_PATH);
	wcscat(buf, _T(".ini"));
	::GetPrivateProfileString(lpszSection, lpszKey, lpszDefault, szRet, _MAX_PATH, buf);
	return szRet;
}

INT MyGetProfileInt(LPCTSTR lpszSection, LPCTSTR lpszKey, int nDefault)
{
	TCHAR buf[_MAX_PATH];
	TCHAR buf2[_MAX_PATH];
	TCHAR szRet[_MAX_PATH];
	GetModuleFileName(AfxGetApp()->m_hInstance, buf, _MAX_PATH);
	wcscat(buf, _T(".ini"));
	_itot(nDefault, buf2, 10);
	::GetPrivateProfileString(lpszSection, lpszKey, buf2, szRet, _MAX_PATH, buf);
	return _ttoi(szRet);
	//::WritePrivateProfileString(
}

void MyWriteProfileString(LPCTSTR lpszSection, LPCTSTR lpszKey, LPCTSTR lpszValue)
{
	TCHAR buf[_MAX_PATH];
	GetModuleFileName(AfxGetApp()->m_hInstance, buf, _MAX_PATH);
	wcscat(buf, _T(".ini"));
	::WritePrivateProfileString(lpszSection, lpszKey, lpszValue, buf);
	//::WritePrivateProfileString(
}

void MyWriteProfileInt(LPCTSTR lpszSection, LPCTSTR lpszKey, int nValue)
{
	TCHAR buf[_MAX_PATH];
	TCHAR buf2[_MAX_PATH];
	GetModuleFileName(AfxGetApp()->m_hInstance, buf, _MAX_PATH);
	wcscat(buf, _T(".ini"));
	::WritePrivateProfileString(lpszSection, lpszKey, _itot(nValue, buf2, 10), buf);
}


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CTestRtspClientDlg 对话框




CTestRtspClientDlg::CTestRtspClientDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CTestRtspClientDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_strUrl = _T("");
	m_strDvrIP = _T("");
	m_strPassword = _T("");
	m_strPlugInName = _T("");
	m_strUserName = _T("");
	m_uPort = 0;
	m_uChannel = 0;

	m_bPlayed = false;

	m_strUrl = MyGetProfileString(_T("连接参数"), _T("流媒体服务器"));
	m_strDvrIP = MyGetProfileString(_T("连接参数"), _T("DVR地址"));
	m_uPort = MyGetProfileInt(_T("连接参数"), _T("端口"), 3000);
	m_strPlugInName = MyGetProfileString(_T("连接参数"), _T("插件名称"));
	m_uChannel = MyGetProfileInt(_T("连接参数"), _T("通道"), 1);
	m_strUserName = MyGetProfileString(_T("连接参数"), _T("用户"));
	m_strPassword = MyGetProfileString(_T("连接参数"), _T("密码"));
}

void CTestRtspClientDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
//	DDX_Control(pDX, IDC_STATIC_INFO, m_ctlInfo);
	DDX_Control(pDX, IDC_BUTTON_PLAY, m_ctlPlayBtn);
	DDX_Control(pDX, IDC_STATIC1, m_show);
//	DDX_Control(pDX, IDC_Show2, m_show2);
	DDX_Text(pDX, IDC_EDIT_URL, m_strUrl);
	DDX_Text(pDX, IDC_EDIT_DVRIP, m_strDvrIP);
	DDX_Text(pDX, IDC_EDIT_PASSWORD, m_strPassword);
	DDX_Text(pDX, IDC_EDIT_PLUGINNAME, m_strPlugInName);
	DDX_Text(pDX, IDC_EDIT_USERNAME, m_strUserName);
	DDX_Text(pDX, IDC_EDIT_PORT, m_uPort);
	DDV_MinMaxUInt(pDX, m_uPort, 0, 65535);
	DDX_Text(pDX, IDC_EDIT_CHANNEL, m_uChannel);
	DDV_MinMaxUInt(pDX, m_uChannel, 0, 100);
}

BEGIN_MESSAGE_MAP(CTestRtspClientDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
//	ON_BN_CLICKED(IDC_BUTTON4, OnButton4)
//	ON_BN_CLICKED(IDC_BTNSLOW, OnBtnslow)
//	ON_BN_CLICKED(IDC_BTNPAUSE, OnBtnpause)
//	ON_BN_CLICKED(IDC_BTNRESUME, OnBtnresume)
//	ON_BN_CLICKED(IDC_BTNSETPOS, OnBtnsetpos)
//	ON_BN_CLICKED(IDC_BTNSETPOS2, OnBtnsetpos2)
//	ON_BN_CLICKED(IDC_BTNGETPOS, OnBtngetpos)
//	ON_BN_CLICKED(IDC_BTNGETFILELIST, OnBtngetfilelist)
//	ON_BN_CLICKED(IDC_BUTTON7, OnButton7)
//	ON_BN_CLICKED(IDC_BUTTON9, OnButton9)
//	ON_BN_CLICKED(IDC_BUTTON10, OnButton10)
//	ON_BN_CLICKED(IDC_BTNDOWNLOAD, OnBtndownload)
//	ON_BN_CLICKED(IDC_BTNSTOPDOWNLOAD, OnBtnstopdownload)
//	ON_BN_CLICKED(IDC_BTNNormalPlay, OnBTNNormalPlay)
//	ON_BN_CLICKED(IDC_BUTTON8, OnFilePlayByTime)
//	ON_BN_CLICKED(IDC_BTNSTOPFILEPLAYONTIME, OnBtnstopfileplayontime)
//	ON_BN_CLICKED(IDC_BTNSTOPFILEPLAY2, OnBtnstopfileplay2)
	ON_BN_CLICKED(IDC_BUTTON_PLAY, &CTestRtspClientDlg::OnButtonPlay)
	ON_BN_CLICKED(IDC_RECPLAY, &CTestRtspClientDlg::OnRecplay)
	ON_BN_CLICKED(IDC_BUTTON_TURELEFT2, &CTestRtspClientDlg::OnButtonTureleft)
	ON_BN_CLICKED(IDC_BUTTON3, &CTestRtspClientDlg::OnBtStop)
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTON_Recore, &CTestRtspClientDlg::OnBnClickedButtonRecore)
	ON_BN_CLICKED(IDC_BUTTON_StopRecore, &CTestRtspClientDlg::OnBnClickedButtonStoprecore)
	ON_BN_CLICKED(IDC_BUTTON_zxlRecoed, &CTestRtspClientDlg::OnBnClickedButtonzxlrecoed)
	ON_WM_DESTROY()
END_MESSAGE_MAP()


// CTestRtspClientDlg 消息处理程序

BOOL CTestRtspClientDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	int i_init = InitStreamClientLib();

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CTestRtspClientDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CTestRtspClientDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CTestRtspClientDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

FILE	*fp = NULL;
DWORD	g_FileLength = 0;

int _stdcall RecDataCallBck(DWORD sid,
	int iusrdata,
	int idatatype,
	char* pdata,
	int ilen)
{
	TRACE("enter RecDataCallback\r\n");
	TRACE("Datelength = %d", ilen);
	fp = fopen("d:\\record\\trecod.mp4", "ab+");
	if (fp != NULL)
	{
		fwrite(pdata, ilen, 1, fp);
		g_FileLength = g_FileLength + ilen;
		fclose(fp);
		fp = NULL;
	}

	return 1;
}

HSESSION g_hSession = -1;
HSESSION hSession2 = -1;
HSESSION g_hSess2 = -1;
HSESSION g_hSess3 = -1;

void split(const string& s, char* c,
	vector<string>& v) {
	string::size_type i = 0;
	string::size_type j = s.find(c);
	int len = strlen(c);

	while (j != string::npos)
	{
		v.push_back(s.substr(i, j - i));
		i = j + len;
		j = s.find(c, j + len);

		if (j == string::npos)
			v.push_back(s.substr(i, s.length()));
	}
}    /////分割字符串函数。


int CTestRtspClientDlg::OnPosLength(unsigned long nLength)
{
	return 0;
}

int CTestRtspClientDlg::OnPresentationOpened(int success)
{
	return 0;
}

int CTestRtspClientDlg::OnPresentationClosed()
{
	return 0;
}

int CTestRtspClientDlg::OnPreSeek(unsigned long uOldTime, unsigned long uNewTime)
{
	return 0;
}

int CTestRtspClientDlg::OnPostSeek(unsigned long uOldTime, unsigned long uNewTime)
{
	return 0;
}

int CTestRtspClientDlg::OnStop()
{
	return 0;
}

int CTestRtspClientDlg::OnPause(unsigned long uTime)
{
	return 0;
}

int CTestRtspClientDlg::OnBegin(unsigned long uTime)
{
	return 0;
}

int CTestRtspClientDlg::OnRandomBegin(unsigned long uTime)
{
	return 0;
}

int CTestRtspClientDlg::OnContacting(const char* pszHost)
{
	return 0;
}

int CTestRtspClientDlg::OnPutErrorMsg(const char* pError)
{
	return 0;
}

int CTestRtspClientDlg::OnBuffering(unsigned int uFlag, unsigned short uPercentComplete)
{
	return 0;
}

int CTestRtspClientDlg::OnChangeRate(int flag)
{
	return 0;
}

int CTestRtspClientDlg::OnDisconnect()
{
	return 0;
}



void CTestRtspClientDlg::OnPlay2()
{
	// TODO: Add your control notification handler code here
	int bRet = -1;
	hSession2 = HIKS_CreatePlayer(this, m_show2.GetSafeHwnd(), NULL, _T("NARI"), NULL, 0);
	if (hSession2 != -1)
	{
		bRet = HIKS_OpenURL(g_hSession, "rtsp://172.20.48.246:554/172.20.42.134:554/172.20.43.249:554/172.20.43.41:8000:NARI:2:0:admin:12345/av_stream", 0, "");
		//		bRet = HIKS_OpenURL(hSession2,"rtsp://172.20.42.134:554/172.20.43.141:8008:NANRUI:1:0:admin:12345/av_stream",1234,"");
		//		bRet = HIKS_OpenURL(hSession2,"rtsp://172.20.42.134:554/172.20.43.89:37777:DFDH:1:0:dh:dh/av_stream",1234,"1234.mp4");
		if (bRet == 1)
		{
			DWORD plength = 100;
			DWORD headerlenth = 0;
			//  HIKS_GetRealDataHeader(hSession,pBuffer,plength,&headerlenth);

			if (HIKS_Play(hSession2) == 1)
			{
				////以下内容插件没有实现
				//			    int a,b,c,d;
				//	            HIKS_GetVideoParams(hSession,&a,&b,&c,&d);

				// 			    if (HIKS_GrabPic(hSession,"c:\\DF1500\\1.bmp",0) == -1)
				// 			    {
				// 	    	       MessageBox(HIKS_GetLastError());     
				// 			    }
				// 			    if (HIKS_SetVideoParams(hSession, 100,100, 120, 150,FALSE) == -1)
				// 			    {
				// 			       MessageBox(HIKS_GetLastError());     
				// 		    	}

			}
			else
			{
				AfxMessageBox(HIKS_GetLastError());
				HIKS_Stop(hSession2);
			}

		}
		else
		{
			MessageBox(HIKS_GetLastError());
		}

	}
	else
		MessageBox(HIKS_GetLastError());
}

void CTestRtspClientDlg::OnButton4()
{
	// TODO: Add your control notification handler code here
	//   int nRet =	HIKS_PTZControl(hSession,2,4,4,2,2,"");
	//   if (nRet == -1)
	//   {
	// 	  MessageBox(HIKS_GetLastError());
	//   }
	int Nret;
	Nret = HIKS_PlayBackControl(g_hSess2, 105, 0, NULL);
	if (Nret == -1)
	{
		MessageBox(HIKS_GetLastError());
	}
}

void CTestRtspClientDlg::OnBtnslow()
{
	// TODO: Add your control notification handler code here
	int Nret;
	Nret = HIKS_PlayBackControl(g_hSess2, 106, 0, NULL);
	if (Nret == -1)
	{
		MessageBox(HIKS_GetLastError());
	}
}

void CTestRtspClientDlg::OnBtnpause()
{
	// TODO: Add your control notification handler code here
	int Nret;
	Nret = HIKS_PlayBackControl(g_hSess2, 103, 0, NULL);
	if (Nret == -1)
	{
		MessageBox(HIKS_GetLastError());
	}
}

void CTestRtspClientDlg::OnBtnresume()
{
	// TODO: Add your control notification handler code here
	int Nret;
	Nret = HIKS_PlayBackControl(g_hSess2, 104, 0, NULL);
	if (Nret == -1)
	{
		MessageBox(HIKS_GetLastError());
	}
}

void CTestRtspClientDlg::OnBtnsetpos()
{
	// TODO: Add your control notification handler code here
	int Nret;
	Nret = HIKS_PlayBackControl(g_hSess2, 112, 50, NULL);
	if (Nret == -1)
	{
		MessageBox(HIKS_GetLastError());
	}
}

void CTestRtspClientDlg::OnBtnsetpos2()
{
	// TODO: Add your control notification handler code here
	int Nret;
	Nret = HIKS_PlayBackControl(g_hSess2, 112, 80, NULL);
	if (Nret == -1)
	{
		MessageBox(HIKS_GetLastError());
	}
}

HSESSION  g_hSess4 = -1;

void CTestRtspClientDlg::OnBtngetpos()
{
	// TODO: Add your control notification handler code here
	DWORD lpos;
	int Nret;
	Nret = HIKS_PlayBackControl(g_hSess2, 113, 0, &lpos);
	if (Nret == -1)
	{
		MessageBox(HIKS_GetLastError());
	}

	TCHAR strPos[100];
	wsprintf(strPos, _T("Get Pos is %d."), lpos);
	MessageBox(strPos);
}

void CTestRtspClientDlg::OnBtngetfilelist()
{
	// TODO: Add your control notification handler code here

	//	char str[95536];
	//	memset(str,0,95536);
	LPSTR str;
	DWORD strlen;
	int nret;

	nret = HIKS_QueryDevRecord("rtsp://172.20.41.37:554/172.20.42.134:554/172.20.43.141:8000:DFHK:1:0:admin:12345/av_stream",
		1,
		"20091021012900-20091026083200",
		//95536,
		&str,
		&strlen);
	if (nret == -1)
	{
		MessageBox(HIKS_GetLastError());
	}
	else
	{
		CString sstr;
		sstr.Format(_T("%s"), str);

		int length = sstr.GetLength();
		CString ststr;
		ststr.Format(_T("recvLen=%d,realLen =%d"), strlen, length);
		MessageBox(ststr);
		HIKS_ISYS_FreeMemory(str);
	}
}

void CTestRtspClientDlg::OnButton7()
{
	// TODO: Add your control notification handler code here

	UpdateData(TRUE);
	int bRet = -1;
	char filename[100];
	memset(filename, 0, 100);
	//	m_filename = "ch:0&&filename:&&size:9007&&driveno:0&&startcluster:3796&&"
	//sprintf(filename,"%s",m_filename);

	//	MessageBox(filename);
	g_hSess2 = HIKS_CreatePlayer(this, m_show2.GetSafeHwnd(), NULL, _T("DFHK"), NULL, 1);
	if (g_hSess2 != -1)
	{

		//"ch01_00100497000200.mp4"  ch:0&&filename:&&size:9007&&driveno:0&&startcluster:3796&&
		bRet = HIKS_OpenURL(g_hSess2, "rtsp://172.20.42.134:554/172.20.43.141:8000:DFHK:1:0:admin:12345/av_stream", 1234, "ch01_30000036000000.mp4");
		//	bRet = HIKS_OpenURL(g_hSess2,"rtsp://172.20.41.37:554/172.20.42.221:37777:DAHUA:1:0:dh:dh/av_stream",1234,"ch:0&&filename:&&size:9007&&driveno:0&&startcluster:3796&&");	
		if (bRet == 1)
		{
			DWORD plength = 100;
			DWORD headerlenth = 0;

			//  HIKS_GetRealDataHeader(hSession,pBuffer,plength,&headerlenth);
			if (HIKS_Play(g_hSess2) == 1)
			{
				// 				if ((bRet = HIKS_OpenSound(g_hSess2)) == -1)
				// 				{
				// 					AfxMessageBox("打开声音失败");
				// 				}
			}
			else
			{
				MessageBox(HIKS_GetLastError());
				HIKS_Stop(g_hSess2);
			}
		}
		else	MessageBox(HIKS_GetLastError());
	}
	else	MessageBox(HIKS_GetLastError());
}

void CTestRtspClientDlg::OnBtnstopfileplay()
{
	// TODO: Add your control notification handler code here
	AfxMessageBox(_T("!!!!!!"));
	if (HIKS_Stop(g_hSess2) == 0)	g_hSess2 = -1;
}

void CTestRtspClientDlg::OnButton9()
{
	// TODO: Add your control notification handler code here	
}

void CTestRtspClientDlg::OnButton10()
{
	// TODO: Add your control notification handler code here
}

static BYTE pbHead[] = { 0x34, 0x48, 0x4b, 0x48, 0xfe, 0xb3, 0xd0, 0xd6, 0x08, 0x03, 0x04, 0x20, 0x00, 0x00, 0x00, 0x00,
0x01, 0x10, 0x01, 0x10, 0x01, 0x10, 0x10, 0x00, 0x80, 0x3e, 0x00, 0x00, 0x60, 0x01, 0x20, 0x01,
0x11, 0x10, 0x00, 0x00, 0x0e, 0x00, 0x00, 0x00 };

void CTestRtspClientDlg::OnBtndownload()
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	int bRet = -1;
	char filename[50];
	memset(filename, 0, 50);
	//sprintf(filename,"%s",m_filename);

	g_hSess3 = HIKS_CreatePlayer(this, NULL, (pDataRec)RecDataCallBck, _T("DFHK"), NULL, 1);
	if (g_hSess3 != -1)
	{
		//"ch01_00100497000200.mp4"
		bRet = HIKS_OpenURL(g_hSess3, "rtsp://172.20.41.37:554/172.20.42.134/172.20.43.141:8000:DFHK:1:0:admin:12345/av_stream", 1234, filename);
		if (bRet == 1)
		{
			DWORD plength = 100;
			DWORD headerlenth = 0;

			//  HIKS_GetRealDataHeader(g_hSess3,pBuffer,plength,&headerlenth);
			fp = fopen("d:\\record\\trecod.mp4", "wb+");
			if (fp != NULL)
			{
				//  fwrite(pBuffer,headerlenth,1,fp);
				fwrite(pbHead, 40, 1, fp);
				g_FileLength = g_FileLength + 40;
				fclose(fp);
				fp = NULL;
			}
			HIKS_PlayBackControl(g_hSess3, 105, 0, NULL);
			HIKS_PlayBackControl(g_hSess3, 105, 0, NULL);
			HIKS_PlayBackControl(g_hSess3, 105, 0, NULL);
			//	   HIKS_PlayBackControl(g_hSess3,105,0,NULL);
		}
		else	MessageBox(HIKS_GetLastError());
	}
	else	MessageBox(HIKS_GetLastError());
}

void CTestRtspClientDlg::OnBtnstopdownload()
{
	// TODO: Add your control notification handler code here
	if (fp != NULL)		fclose(fp);

	if (HIKS_Stop(g_hSess3) == 0)	g_hSess3 = -1;
}

void CTestRtspClientDlg::OnBTNNormalPlay()
{
	// TODO: Add your control notification handler code here
	int Nret;
	Nret = HIKS_PlayBackControl(g_hSess2, 107, 0, NULL);
	if (Nret == -1)		MessageBox(HIKS_GetLastError());
}

void CTestRtspClientDlg::OnFilePlayByTime()
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	int bRet = -1;
	char filename[100];
	memset(filename, 0, 100);
	//	m_filename = "ch:0&&filename:&&size:9007&&driveno:0&&startcluster:3796&&"
	//sprintf(filename,"%s",m_filename);

	//	MessageBox(filename);
	g_hSess4 = HIKS_CreatePlayer(this, m_show2.GetSafeHwnd(), NULL, _T("DFHK"), NULL, 1);
	if (g_hSess4 != -1)
	{
		//"ch01_00100497000200.mp4"  ch:0&&filename:&&size:9007&&driveno:0&&startcluster:3796&&172.20.41.37:554/1
		bRet = HIKS_OpenURL(g_hSess4, "rtsp://172.20.41.37/172.20.42.134:554/172.20.43.141:8000:DFHK:1:0:admin:12345/av_stream", 1234, "DateRange:20091021000000-20091026235959");
		if (bRet == 1)
		{
			DWORD plength = 100;
			DWORD headerlenth = 0;

			//  HIKS_GetRealDataHeader(hSession,pBuffer,plength,&headerlenth);
			if (HIKS_Play(g_hSess4) == 1)
			{
				// 				if ((bRet = HIKS_OpenSound(g_hSess2)) == -1)
				// 				{
				// 					AfxMessageBox("打开声音失败");
				// 				}			
			}
			else
			{
				MessageBox(HIKS_GetLastError());
				HIKS_Stop(g_hSess4);
			}
		}
		else	MessageBox(HIKS_GetLastError());
	}
	else	 MessageBox(HIKS_GetLastError());
}

void CTestRtspClientDlg::OnBtnstopfileplayontime()
{
	// TODO: Add your control notification handler code here
	AfxMessageBox(_T("2222"));
	if (HIKS_Stop(g_hSess4) == 0)	g_hSess4 = -1;
}

void CTestRtspClientDlg::OnBtnstopfileplay2()
{
	// TODO: Add your control notification handler code here
	if (HIKS_Stop(g_hSess2) == 0)	g_hSess2 = -1;
}

void CTestRtspClientDlg::OnBtntrans()
{
	// TODO: Add your control notification handler code here
	int bRet = -1;
	g_hSession = HIKS_CreatePlayer(this, m_show.GetSafeHwnd(), NULL, _T("DFHK"), NULL, 1);
	if (g_hSession != -1)
	{
		//		bRet = HIKS_OpenURL(g_hSession,"rtsp://172.20.41.37:554/172.20.42.221:37777:DFDH:1:0:dh:dh/av_stream",1234,"");
		bRet = HIKS_OpenURL(g_hSession, "rtsp://172.20.41.37:554/172.20.42.134:554/172.20.43.141:8000:DFHK:1:0:admin:12345/av_stream", 1234, "");
		if (bRet == 1)
		{
			DWORD plength = 100;
			DWORD headerlenth = 0;

			//  HIKS_GetRealDataHeader(hSession,pBuffer,plength,&headerlenth);
			if (HIKS_Play(g_hSession) == 1)
			{
				////以下内容插件没有实现
				//				int a,b,c,d;
				//		        HIKS_GetVideoParams(hSession,&a,&b,&c,&d);

				//	 			if (HIKS_GrabPic(hSession,"c:\\DF1500\\1.bmp",0) == -1)
				//	 			{
				//	 	    	    MessageBox(HIKS_GetLastError());     
				//				}
				//	 			if (HIKS_SetVideoParams(hSession, 100,100, 120, 150,FALSE) == -1)
				//	 			{
				//	 				MessageBox(HIKS_GetLastError());     
				//	 			}

				//	 			if ((bRet = HIKS_OpenSound(g_hSession)) == -1)
				// 	 			{
				// 	 				AfxMessageBox("打开声音失败");
				// 	 			}
			}
			else
			{
				MessageBox(HIKS_GetLastError());
				HIKS_Stop(g_hSession);
			}
		}
		else	MessageBox(HIKS_GetLastError());
	}
	else	MessageBox(HIKS_GetLastError());
}

BOOL CTestRtspClientDlg::DestroyWindow()
{
	// TODO: Add your specialized code here and/or call the base class
	FiniStreamClientLib();
	return CDialog::DestroyWindow();
}

inline unsigned __int64 GetCycleCount()
{
	__asm _emit 0x0F
	__asm _emit 0x31
}

int __stdcall DataRecFunc(DWORD sid, int iusrdata, int idatatype, char* pdata, int ilen)
{
	CTestRtspClientDlg *pDlg = (CTestRtspClientDlg*)iusrdata;
	static int i = 1;
	pDlg->m_iTotalRxData += ilen;

	if (pDlg&&i++ % 15 == 0)
	{
		CString strMsg;

		DWORD i64TimeDiff = timeGetTime() - pDlg->m_StartTimer;
		if (i64TimeDiff != 0)
		{
			strMsg.Format(_T("收到数据,长度 %d 字节, 当前码率:%d kbps"), ilen, pDlg->m_iTotalRxData * 8 / i64TimeDiff);
			pDlg->m_StartTimer = timeGetTime();
			pDlg->m_iTotalRxData = 0;
		}
//		pDlg->m_ctlInfo.SetWindowText(strMsg);
		//PostMessage(pDlg->m_ctlInfo.GetSafeHwnd(),WM_SETTEXT,0,(LPARAM)(LPCTSTR)strMsg);
	}

	return 0;
}

void CTestRtspClientDlg::OnButtonPlay()
{
	UpdateData();

	MyWriteProfileString(_T("连接参数"), _T("流媒体服务器"), m_strUrl);
	MyWriteProfileString(_T("连接参数"), _T("DVR地址"), m_strDvrIP);
	MyWriteProfileInt(_T("连接参数"), _T("端口"), m_uPort);
	MyWriteProfileString(_T("连接参数"), _T("插件名称"), m_strPlugInName);
	MyWriteProfileInt(_T("连接参数"), _T("通道"), m_uChannel);
	MyWriteProfileString(_T("连接参数"), _T("用户"), m_strUserName);
	MyWriteProfileString(_T("连接参数"), _T("密码"), m_strPassword);

	if (!m_bPlayed)
	{
		int bRet = -1;
		g_hSession = HIKS_CreatePlayer(this, m_show.GetSafeHwnd(), DataRecFunc, m_strPlugInName, NULL, 0);
		if (g_hSession != -1)
		{
			TCHAR buffer[1024];
			wsprintf(buffer, _T("rtsp://%s/%s:%d:%s:%d:0:%s:%s/av_stream"),
				m_strUrl, m_strDvrIP, m_uPort, m_strPlugInName, m_uChannel, m_strUserName, m_strPassword);
			USES_CONVERSION;
			char	*url = T2A(buffer);

			bRet = HIKS_OpenURL(g_hSession, url, (int)this, "");
			m_StartTimer = timeGetTime();
			if (bRet == 1)
			{
				char pBuffer[100];
				DWORD plength = 100;
				DWORD headerlenth = 0;

				//  HIKS_GetRealDataHeader(hSession,pBuffer,plength,&headerlenth);
				if (HIKS_Play(g_hSession) == 1)
				{
					m_bPlayed = true;
					m_ctlPlayBtn.SetWindowText(_T("停止播放"));
				}
				else
				{
					MessageBox(HIKS_GetLastError());
					HIKS_Stop(g_hSession);
				}
			}
			else	MessageBox(HIKS_GetLastError());
		}
		else	MessageBox(HIKS_GetLastError());
	}
	else
	{
		if (HIKS_Stop(g_hSession) == 0)		g_hSession = -1;

		m_ctlPlayBtn.SetWindowText(_T("开始播放"));
		m_show.Invalidate();
		m_show.UpdateWindow();
		m_bPlayed = false;
	}
}

void CTestRtspClientDlg::OnRecplay()
{
	// TODO: Add your control notification handler code here

	UpdateData();

	MyWriteProfileString(_T("连接参数"), _T("流媒体服务器"), m_strUrl);
	MyWriteProfileString(_T("连接参数"), _T("DVR地址"), m_strDvrIP);
	MyWriteProfileInt(_T("连接参数"), _T("端口"), m_uPort);
	MyWriteProfileString(_T("连接参数"), _T("插件名称"), m_strPlugInName);
	MyWriteProfileInt(_T("连接参数"), _T("通道"), m_uChannel);
	MyWriteProfileString(_T("连接参数"), _T("用户"), m_strUserName);
	MyWriteProfileString(_T("连接参数"), _T("密码"), m_strPassword);

	if (!m_bPlayed)
	{
		int bRet = -1;
		g_hSession = HIKS_CreatePlayer(this, m_show.GetSafeHwnd(), DataRecFunc, m_strPlugInName, NULL, 0);
		if (g_hSession != -1)
		{
			char buffer[1024];
			sprintf(buffer, "rtsp://%s/%s:%d:%s:%d:0:%s:%s/av_stream",
				m_strUrl, m_strDvrIP, m_uPort, m_strPlugInName, m_uChannel, m_strUserName, m_strPassword);

			bRet = HIKS_OpenURL(g_hSession, buffer, (int)this, "DateRange:20110811070101-20110811170600");
			m_StartTimer = timeGetTime();
			if (bRet == 1)
			{
				DWORD plength = 100;
				DWORD headerlenth = 0;

				//  HIKS_GetRealDataHeader(hSession,pBuffer,plength,&headerlenth);
				if (HIKS_Play(g_hSession) == 1)
				{
					m_bPlayed = true;
					m_ctlPlayBtn.SetWindowText(_T("停止播放"));
				}
				else
				{
					MessageBox(HIKS_GetLastError());
					HIKS_Stop(g_hSession);
				}
			}
			else	MessageBox(HIKS_GetLastError());
		}
		else	MessageBox(HIKS_GetLastError());
	}
	else
	{
		if (HIKS_Stop(g_hSession) == 0)		g_hSession = -1;

		m_ctlPlayBtn.SetWindowText(_T("开始播放"));
		m_show.Invalidate();
		m_show.UpdateWindow();
		m_bPlayed = false;
	}
}

void CTestRtspClientDlg::OnButtonTureleft()
{
	int nRet = HIKS_PTZControl(g_hSession, 3, 2, 2, 0, 0, "");
	if (nRet == -1)		MessageBox(HIKS_GetLastError());
}

void CTestRtspClientDlg::OnBtStop()
{
	// TODO: Add your control notification handler code here
	int temp = 0;
	int nRet = HIKS_PTZControl(g_hSession, 0, 0, 0, 0, 0, "");
	if (nRet == -1)	   MessageBox(HIKS_GetLastError());
}

//录制视频按钮
void CTestRtspClientDlg::OnBnClickedButtonRecore()
{
	// TODO: 在此添加控件通知处理程序代码
}

//停止视频录制
void CTestRtspClientDlg::OnBnClickedButtonStoprecore()
{
	// TODO: 在此添加控件通知处理程序代码
}

void CTestRtspClientDlg::OnBnClickedButtonzxlrecoed()
{
	// TODO: 在此添加控件通知处理程序代码
}


void CTestRtspClientDlg::OnDestroy()
{
	__super::OnDestroy();

	// TODO:  在此处添加消息处理程序代码
	FiniStreamClientLib();
}

