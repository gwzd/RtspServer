// PlugIns.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include "PlugIns.h"

#include "Global.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//
//	Note!
//
//		If this DLL is dynamically linked against the MFC
//		DLLs, any functions exported from this DLL which
//		call into MFC must have the AFX_MANAGE_STATE macro
//		added at the very beginning of the function.
//
//		For example:
//
//		extern "C" BOOL PASCAL EXPORT ExportedFunction()
//		{
//			AFX_MANAGE_STATE(AfxGetStaticModuleState());
//			// normal function body here
//		}
//
//		It is very important that this macro appear in each
//		function, prior to any calls into MFC.  This means that
//		it must appear as the first statement within the 
//		function, even before any object variable declarations
//		as their constructors may generate calls into the MFC
//		DLL.
//
//		Please see MFC Technical Notes 33 and 58 for additional
//		details.
//

/////////////////////////////////////////////////////////////////////////////
// CPlugInsApp

BEGIN_MESSAGE_MAP(CPlugInsApp, CWinApp)
	//{{AFX_MSG_MAP(CPlugInsApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPlugInsApp construction

CPlugInsApp::CPlugInsApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CPlugInsApp object

CPlugInsApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CPlugInsApp initialization

BOOL CPlugInsApp::InitInstance() 
{
	TRACE(">>>>>>>>> DFHK Play Plugin DLL InitInstance\n ");

	//获得本DLL所在路径
	TCHAR szModName[MAX_PATH];
	::GetModuleFileName(m_hInstance, szModName, 255);	//得到DLL所在路径
	//::GetModuleFileName(NULL, szModName, 255);		//得到调用者的路径

	TCHAR pdriver[10], ppath[MAX_PATH];
	_tsplitpath(szModName, pdriver, ppath, NULL, NULL);
	wsprintf(g_szDLLPath, _T("%s%s"), pdriver, ppath);

	return TRUE;
}

int CPlugInsApp::ExitInstance() 
{
	TRACE(">>>>>>>>> DFHK Play Plugin DLL ExitInstance\n ");

	return CWinApp::ExitInstance();
}

