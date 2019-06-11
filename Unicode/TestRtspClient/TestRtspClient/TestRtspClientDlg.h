
// TestRtspClientDlg.h : 头文件
//

#pragma once

#include "clntsink.h"

// CTestRtspClientDlg 对话框
class CTestRtspClientDlg : public CDialogEx, IHikClientAdviseSink
{
// 构造
public:
	CTestRtspClientDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_TESTRTSPCLIENT_DIALOG };

	CButton	m_ctlStop;
	CStatic	m_ctlInfo;
	CButton	m_ctlPlayBtn;
	CStatic	m_show;
	CStatic	m_show2;
	CString	m_strUrl;
	CString	m_strDvrIP;
	CString	m_strPassword;
	CString	m_strPlugInName;
	UINT	m_strPort;
	CString	m_strUserName;
	UINT	m_uPort;
	UINT	m_uChannel;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
	afx_msg void OnUp();
	afx_msg void OnDown();
	afx_msg void OnStop1();
	afx_msg void OnPlay2();
	afx_msg void OnButton4();
	afx_msg void OnBtnslow();
	afx_msg void OnBtnpause();
	afx_msg void OnBtnresume();
	afx_msg void OnBtnsetpos();
	afx_msg void OnBtnsetpos2();
	afx_msg void OnBtngetpos();
	afx_msg void OnBtngetfilelist();
	afx_msg void OnButton7();
	afx_msg void OnBtnstopfileplay();
	afx_msg void OnButton9();
	afx_msg void OnButton10();
	afx_msg void OnBtndownload();
	afx_msg void OnBtnstopdownload();
	afx_msg void OnBTNNormalPlay();
	afx_msg void OnFilePlayByTime();
	afx_msg void OnBtnstopfileplayontime();
	afx_msg void OnBtnstopfileplay2();
	afx_msg void OnBtntrans();
	afx_msg void OnButtonPlay();
	afx_msg void OnRecplay();
	afx_msg void OnButtonTureleft();
	afx_msg void OnBtstop();
	afx_msg void OnBtStop();
	virtual BOOL DestroyWindow();

	int OnPosLength(unsigned long nLength);
	int OnPresentationOpened(int success);
	int OnPresentationClosed();
	int OnPreSeek(unsigned long uOldTime, unsigned long uNewTime);
	int OnPostSeek(unsigned long uOldTime, unsigned long uNewTime);
	int OnStop();
	int OnPause(unsigned long uTime);
	int OnBegin(unsigned long uTime);
	int OnRandomBegin(unsigned long uTime);
	int OnContacting(const char* pszHost);
	int OnPutErrorMsg(const char* pError);
	int OnBuffering(unsigned int uFlag, unsigned short uPercentComplete);
	int OnChangeRate(int flag);
	int OnDisconnect();
	bool m_bPlayed;
public:
	DWORD m_StartTimer;
	unsigned long  m_iTotalRxData;

	afx_msg void OnBnClickedButtonRecore();
	afx_msg void OnBnClickedButtonStoprecore();
public:

	afx_msg void OnBnClickedButtonzxlrecoed();
	afx_msg void OnDestroy();
};
