// AudioEditor.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "AudioEditor.h"
#include "MainFrm.h"

#include "AudioEditorDoc.h"
#include "AudioEditorView.h"
#include "Global.h"
#include "Engine.h"
#include "WndBase.h"
#include "Render.h"
#include <fmod.hpp>
#include <AFI.h>

using AudioEngine::VECTOR;


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAudioEditorApp

BEGIN_MESSAGE_MAP(CAudioEditorApp, CWinApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	// Standard file based document commands
	ON_COMMAND(ID_FILE_NEW, CWinApp::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, CWinApp::OnFileOpen)
	// Standard print setup command
	ON_COMMAND(ID_FILE_PRINT_SETUP, CWinApp::OnFilePrintSetup)
END_MESSAGE_MAP()


// CAudioEditorApp construction

CAudioEditorApp::CAudioEditorApp() :
	CBCGPWorkspace (TRUE /* m_bResourceSmartUpdate */)
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}


// The one and only CAudioEditorApp object

CAudioEditorApp theApp;


// CAudioEditorApp initialization

BOOL CAudioEditorApp::InitInstance()
{
	
	// InitCommonControlsEx() is required on Windows XP if an application
	// manifest specifies use of ComCtl32.dll version 6 or later to enable
	// visual styles.  Otherwise, any window creation will fail.
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// Set this to include all the common control classes you want to use
	// in your application.
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();

	// Initialize OLE libraries
	if (!AfxOleInit())
	{
		AfxMessageBox(IDP_OLE_INIT_FAILED);
		return FALSE;
	}
	AfxEnableControlContainer();
	// Standard initialization
	// If you are not using these features and wish to reduce the size
	// of your final executable, you should remove from the following
	// the specific initialization routines you do not need
	// Change the registry key under which our settings are stored
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization
	SetRegistryKey(_T("BCGP AppWizard-Generated Applications"));
	LoadStdProfileSettings(4);  // Load standard INI file options (including MRU)

	m_bSaveState = FALSE;

	SetRegistryBase (_T("Settings"));	

	// Initialize all Managers for usage. They are automatically constructed
	// if not yet present
	InitContextMenuManager();
	InitKeyboardManager();

	// TODO: Remove this if you don't want extended tooltips:
	InitTooltipManager();

	CBCGPToolTipParams params;
	params.m_bVislManagerTheme = TRUE;

	theApp.GetTooltipManager ()->SetTooltipParams (
		BCGP_TOOLTIP_TYPE_ALL,
		RUNTIME_CLASS (CBCGPToolTipCtrl),
		&params);	

	// Register the application's document templates.  Document templates
	//  serve as the connection between documents, frame windows and views
	CSingleDocTemplate* pDocTemplate;
	pDocTemplate = new CSingleDocTemplate(
		IDR_MAINFRAME,
		RUNTIME_CLASS(CAudioEditorDoc),
		RUNTIME_CLASS(CMainFrame),       // main SDI frame window
		RUNTIME_CLASS(CAudioEditorView));
	if (!pDocTemplate)
		return FALSE;
	AddDocTemplate(pDocTemplate);

	
	// Parse command line for standard shell commands, DDE, file open
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);


	// Dispatch commands specified on the command line
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;	

	// The one and only window has been initialized, so show and update it
	m_pMainWnd->ShowWindow(SW_SHOW);
	m_pMainWnd->UpdateWindow();
	// call DragAcceptFiles only if there's a suffix
	//  In an SDI app, this should occur after ProcessShellCommand

	if(!initApp())
		return FALSE;
	g_Render.Init(NULL, m_pMainWnd->GetSafeHwnd());
	
	return TRUE;
}

// CAudioEditorApp message handlers

int CAudioEditorApp::ExitInstance() 
{
	BCGCBProCleanUp();


	if(!exitApp())
		return FALSE;
	g_Render.Release();

	return CWinApp::ExitInstance();
}

// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };
	CBCGPURLLinkButton m_btnURL;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMPANY_URL, m_btnURL);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()

// App command to run the dialog
void CAudioEditorApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}


// CAudioEditorApp message handlers


void CAudioEditorApp::PreLoadState ()
{

	GetContextMenuManager()->AddMenu (_T("My menu"), IDR_CONTEXT_MENU);

	// TODO: add another context menus here
}

bool CAudioEditorApp::initApp()
{
	char szWorkDir[MAX_PATH] = {0};
	GetCurrentDirectory(MAX_PATH, szWorkDir);
	af_Initialize();
	af_SetBaseDir(szWorkDir);

	if(!CWndBase::Init())
		return false;
	GF_InitConfigs();
	GF_LoadConfigs();	
	if(!GF_CreateDirectory(g_Configs.szAudioEditorPath))
		return false;
	if(!GF_CreateDirectory(g_Configs.szProjectsPath))
		return false;
	if(!GF_CreateDirectory(g_Configs.szReverbsPath))
		return false;
	if(!GF_CreateDirectory(g_Configs.szPresetsPath))
		return false;
	if(!GF_CreateDirectory(g_Configs.szEventPresetPath))
		return false;
	if(!GF_CreateDirectory(g_Configs.szSoundDefPresetPath))
		return false;
	if(!GF_CreateDirectory(g_Configs.szEffectPresetPath))
		return false;
	if(!GF_CreateDirectory(g_Configs.szSoundInsPresetPath))
		return false;
	if(!Engine::GetInstance().Init(500, FMOD_INIT_NORMAL, 0))
		return false;
	return true;
}

bool CAudioEditorApp::exitApp()
{
	Engine::GetInstance().Release();
	GF_SaveConfigs();
	CWndBase::Release();

	af_Finalize();
	return true;
}

BOOL CAudioEditorApp::OnIdle(LONG lCount)
{
	static unsigned int iTickCount = 0;
	unsigned int iCurTickCount = GetTickCount();
	if (iCurTickCount - iTickCount < 50)
	{
		return __super::OnIdle(lCount);
	}
	iTickCount = iCurTickCount;
	Engine::GetInstance().GetEventSystem()->Update();
	VECTOR pos(g_Configs.vListenerPos.x, g_Configs.vListenerPos.y, g_Configs.vListenerPos.z);
	VECTOR vel(g_Configs.vListenerVel.x, g_Configs.vListenerVel.y, g_Configs.vListenerVel.z);
	VECTOR forward(g_Configs.vListenerForward.x, g_Configs.vListenerForward.y, g_Configs.vListenerForward.z);
	VECTOR up(g_Configs.vListenerUp.x, g_Configs.vListenerUp.y, g_Configs.vListenerUp.z);
	Engine::GetInstance().GetEventSystem()->Set3DListenerAttributes(pos, vel, forward, up);
	__super::OnIdle(lCount);
	return TRUE;
}
