// PWDownloader.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "PWDownloader.h"
#include "PWDownloaderDlg.h"
#include "DlgPreInstall.h"
#include "PWInstallDlg.h"
#include "LogFile.h"
#include <TlHelp32.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPWDownloaderApp

BEGIN_MESSAGE_MAP(CPWDownloaderApp, CWinApp)
	//{{AFX_MSG_MAP(CPWDownloaderApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPWDownloaderApp construction

CPWDownloaderApp::CPWDownloaderApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
	m_iParentProcessID = 0;
	m_hWndParent = NULL;
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CPWDownloaderApp object

CPWDownloaderApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CPWDownloaderApp initialization

BOOL CPWDownloaderApp::InitInstance()
{
	AfxEnableControlContainer();

	// ��ֹ����࿪
	HANDLE hSingleApp = CreateMutex(NULL, TRUE, "CPW_MiniDownloader");
	if( GetLastError() == ERROR_ALREADY_EXISTS )
	{
		CloseHandle(hSingleApp);
		return FALSE;
	}

	// ��ȡ�ͻ��˵Ĵ��ھ��
	HWND hClientWnd = NULL;
	_stscanf(GetCommandLine(), _T("%d %x"), &m_iParentProcessID, &m_hWndParent);
	if( !m_iParentProcessID || !IsWindow(m_hWndParent) ) return FALSE;

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

	ULONG_PTR gdiplusToken;
	Gdiplus::GdiplusStartupInput gdiplusStartupInput;
	Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif

	// ������־�ļ�
	CLogFile::GetInstance().CreateLog("Logs\\PWDownloader.log");

	// ��ʼ����
	CPWDownloaderDlg dlg;
	m_pMainWnd = &dlg;
	int nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with OK

		// ��ʼ��װ�����ͻ���
		if( dlg.IsDownloadOK() )
		{
			// ����߳���Ϣ�����е�WM_QUIT
			MSG msg;
			PeekMessage(&msg, NULL, 0, 0, PM_REMOVE);

			CDlgPreInstall preDlg;
			if( preDlg.DoModal() == IDOK )
			{
				CPWInstallDlg installDlg;
				installDlg.m_sExtractDir = preDlg.m_szExtractDir;
				installDlg.DoModal();
			}
		}
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with Cancel
	}

	Gdiplus::GdiplusShutdown(gdiplusToken);
	CloseHandle(hSingleApp);
	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}

// ���ͻ��˽����Ƿ���������
bool CPWDownloaderApp::IsClientRunning()
{
	HANDLE hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
	if( hProcessSnap == INVALID_HANDLE_VALUE )
		return FALSE;

	PROCESSENTRY32 pe32;
	memset(&pe32, 0, sizeof(PROCESSENTRY32));
	pe32.dwSize = sizeof(pe32);
	BOOL bMore = Process32First(hProcessSnap, &pe32);
	while(bMore)
	{
		if( (int)pe32.th32ProcessID == m_iParentProcessID )
			return TRUE;

		bMore = Process32Next(hProcessSnap, &pe32);
	}

	CloseHandle(hProcessSnap);
	return FALSE;
}
