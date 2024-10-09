// TaskTemplEditor.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "TaskTemplEditor.h"

#include "MainFrm.h"
#include "TaskTemplEditorDoc.h"
#include "TaskTemplEditorView.h"
#include <shlwapi.h>
#include "VSSOptionDlg.h"
#include "vssoperation.h"
#include "LogoDlg.h"
#include "BaseDataIDMan.h"
#include "TemplIDSelDlg.h"
#include "TaskTemplMan.h"
#include "AMemory.h"

#ifdef _DEBUG
#define new A_DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

bool g_bLinkVss = true;

bool FileIsReadOnly(AString szFilePathName)
{
	DWORD flag = GetFileAttributes(szFilePathName);
	if(flag==-1)
	{
		AfxMessageBox("不正确的文件附加属性,不能判断文件是否是只读");
		return true;
	}
	if (!(flag & FILE_ATTRIBUTE_READONLY)) 
	{
		return false;
	}
	return true;
}

/////////////////////////////////////////////////////////////////////////////
// CTaskTemplEditorApp

BEGIN_MESSAGE_MAP(CTaskTemplEditorApp, CWinApp)
	//{{AFX_MSG_MAP(CTaskTemplEditorApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	//}}AFX_MSG_MAP
	// Standard file based document commands
	ON_COMMAND(ID_FILE_NEW, CWinApp::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, CWinApp::OnFileOpen)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTaskTemplEditorApp construction

CTaskTemplEditorApp::CTaskTemplEditorApp()
{
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CTaskTemplEditorApp object

CTaskTemplEditorApp theApp;

BaseDataIDManBase g_TemplIDMan;

/////////////////////////////////////////////////////////////////////////////
// CTaskTemplEditorApp initialization

BOOL RefreshBaseDataSettings()
{
	FILE* fp = fopen(_org_config_path, "rb");
	if (!fp) return FALSE;

	_org_paths.clear();
	char line[TASK_MAX_LINE_LEN];
	while (ReadLine(fp, line) && strlen(line) != 0)
	{
		int nLevel = -1;
		while (true)
		{
			CString strOrg;
			strOrg.Format("%s\\%d.org", line, nLevel + 1);
			if (GetFileAttributes(strOrg) == (DWORD)-1)
				break;
			nLevel++;
		}
		_org_paths[AString(line)] = nLevel;
	}
	fclose(fp);

	g_TaskIDMan.Release();
	if (g_TaskIDMan.Load("BaseData\\TaskID.dat") != 0)
	{
		AfxMessageBox("打开TaskID文件失败", MB_ICONSTOP);
		return FALSE;
	}

	g_TemplIDMan.Release();
	if (g_TemplIDMan.Load("BaseData\\TemplID.dat") != 0)
	{
		AfxMessageBox("打开TemplID文件失败", MB_ICONSTOP);
		return FALSE;
	}

	/*
	g_TalkIDMan.Release();
	if (g_TalkIDMan.Load("TalkData\\TalkID.dat") != 0)
	{
		AfxMessageBox("打开TalkID文件失败", MB_ICONSTOP);
		return FALSE;
	}
	*/

	return TRUE;
}

extern void _start_up();
extern void _set_init();

char g_szWorkDir[MAX_PATH];

BOOL CTaskTemplEditorApp::InitInstance()
{
	AfxEnableControlContainer();

	// Standard initialization

#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif

	// Change the registry key under which our settings are stored.
	SetRegistryKey(_T("CTaskEditor"));

	LoadStdProfileSettings();  // Load standard INI file options (including MRU)

	GetModuleFileName(NULL, g_szWorkDir, MAX_PATH);
	LPTSTR lpPath = PathFindFileName(g_szWorkDir);
	lpPath[0] = _T('\0');

	// Register document templates

	CSingleDocTemplate* pDocTemplate;
	pDocTemplate = new CSingleDocTemplate(
		IDR_MAINFRAME,
		RUNTIME_CLASS(CTaskTemplEditorDoc),
		RUNTIME_CLASS(CMainFrame),       // main SDI frame window
		RUNTIME_CLASS(CTaskTemplEditorView));
	AddDocTemplate(pDocTemplate);

	// Enable DDE Execute open
	/*
	EnableShellOpen();
	RegisterShellFileTypes(TRUE);
	*/

	// Parse command line for standard shell commands, DDE, file open
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);

	// Dispatch commands specified on the command line
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;

	m_pMainWnd->SetWindowText("完美国际 任务编辑器");
	m_pMainWnd->ShowWindow(SW_SHOWMAXIMIZED);
	m_pMainWnd->UpdateWindow();

	// Enable drag/drop open
	m_pMainWnd->DragAcceptFiles();

	LoadConfigs();

	if(!InitVss()) return FALSE;

	CLogoDlg dlg;
	dlg.DoModal();

	if (!RefreshBaseDataSettings())
	{
		AfxMessageBox("读取配置文件失败！", MB_ICONSTOP);
		return FALSE;
	}

	//AfxGetEditorView()->UpdateTaskTree();
	// use new method: we load one template, get what we want, then release. and then we load next.
	AfxGetEditorView()->InitTaskTree();
	AfxGetEditorView()->UpdateTemplClassTree();
	
	return TRUE;
}

bool CTaskTemplEditorApp::InitVss()
{
	while(1)
	{
		if (!SetEnvironmentVariable("SSDIR", g_strVSSPath)
			||	!SetEnvironmentVariable("SSUSER", g_strUser)
			|| !SetEnvironmentVariable("SSPWD", g_strPwd))
			return false;
		
		g_VSS.InitVss(NULL);
		g_VSS.SetUserName(g_strUser);
		g_VSS.SetPassword(g_strPwd);
		g_VSS.SetServerPath(g_strVSSPath);
		
		CString _fileType = g_VSS.GetFileTypes("s_d_project.ini");
		if(!g_VSS.TestVssConnected(_fileType))
		{
			AfxMessageBox("No VSS database!");
			CVSSOptionDlg dlg;
			if(IDOK!=dlg.DoModal()) return FALSE;
		}else
		{
			//user name or password is not right.
			if( _fileType.Find(_T("Username"))>=0 && _fileType.Find(_T("Password"))>=0 )
			{
				AfxMessageBox("User name or Password is wrong!");
				CVSSOptionDlg dlg;
				if(IDOK!=dlg.DoModal()) return FALSE;	
			}else return true;
		}
	}
	
	return true;
}


/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
		// No message handlers
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// App command to run the dialog
void CTaskTemplEditorApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

/////////////////////////////////////////////////////////////////////////////
// CTaskTemplEditorApp message handlers


int CTaskTemplEditorApp::ExitInstance() 
{
	// TODO: Add your specialized code here and/or call the base class
	SaveConfigs();
	return CWinApp::ExitInstance();
}
