// ClientTryLogin.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "ClientTryLogin.h"
#include "ClientTryLoginDlg.h"
#include "SessionManager.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CClientTryLoginApp

BEGIN_MESSAGE_MAP(CClientTryLoginApp, CWinApp)
	//{{AFX_MSG_MAP(CClientTryLoginApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CClientTryLoginApp construction

CClientTryLoginApp::CClientTryLoginApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CClientTryLoginApp object

CClientTryLoginApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CClientTryLoginApp initialization

BOOL CClientTryLoginApp::InitInstance()
{
	AfxEnableControlContainer();

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

	// 禁止程序多开
	HANDLE hSingleApp = ::CreateMutex(NULL, TRUE, "PW_ClientTryLogin");
	if( ::GetLastError() == ERROR_ALREADY_EXISTS )
	{
		::CloseHandle(hSingleApp);
		::MessageBox(NULL, _T("已经有一个程序实例正在运行！"), _T("错误"), MB_OK|MB_ICONERROR);
		return FALSE;
	}

#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif

	InitNetwork("logintest.conf");

	CClientTryLoginDlg dlg;
	m_pMainWnd = &dlg;
	int nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with OK
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with Cancel
	}

	Finalize();
	::CloseHandle(hSingleApp);
	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}
