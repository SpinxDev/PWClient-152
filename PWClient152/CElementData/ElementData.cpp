// ElementData.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "Global.h"
#include "Render.h"
#include "ElementData.h"

#include "MainFrm.h"
#include "ChildFrm.h"
#include "ElementDataDoc.h"
#include "ElementDataView.h"
#include "VssOperation.h"
#include "BaseDataIDMan.h"
#include "OptionVssDlg.h"

#include "PropertyBox.h"

#include "AF.h"
#include "A3D.h"
#include "EnumTypes.h"
#include "LogoDlg.h"

#define new A_DEBUG_NEW

/////////////////////////////////////////////////////////////////////////////
// CElementDataApp

BEGIN_MESSAGE_MAP(CElementDataApp, CWinApp)
	//{{AFX_MSG_MAP(CElementDataApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	ON_COMMAND(ID_FILE_NEW, OnFileNew)
	ON_COMMAND(ID_SAVE_ALL_DOC, OnSaveAllDoc)
	ON_UPDATE_COMMAND_UI(ID_SAVE_ALL_DOC, OnUpdateSaveAllDoc)
	//}}AFX_MSG_MAP
	// Standard file based document commands
	ON_COMMAND(ID_FILE_NEW, CWinApp::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, CWinApp::OnFileOpen)
	// Standard print setup command
	ON_COMMAND(ID_FILE_PRINT_SETUP, CWinApp::OnFilePrintSetup)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CElementDataApp construction

CElementDataApp::CElementDataApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CElementDataApp object

CElementDataApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CElementDataApp initialization

BOOL CElementDataApp::InitInstance()
{
	AfxEnableControlContainer();

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif

	// Change the registry key under which our settings are stored.
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization.
	SetRegistryKey(_T("ElementData"));

	LoadStdProfileSettings();  // Load standard INI file options (including MRU)

	if (!InitApp())
		return FALSE;

	// Register the application's document templates.  Document templates
	//  serve as the connection between documents, frame windows and views.
	pDocTemplate = new CMultiDocTemplate(
		IDR_ELEMENTYPE,
		RUNTIME_CLASS(CElementDataDoc),
		RUNTIME_CLASS(CChildFrame), // custom MDI child frame
		RUNTIME_CLASS(CElementDataView));
	AddDocTemplate(pDocTemplate);

	// create main MDI Frame window
	CMainFrame* pMainFrame = new CMainFrame;
	if (!pMainFrame->LoadFrame(IDR_MAINFRAME))
		return FALSE;
	m_pMainWnd = pMainFrame;

	// Parse command line for standard shell commands, DDE, file open
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);

	// Dispatch commands specified on the command line
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;

	//Get project's name from server
	if( !GetProjectName() )
		return FALSE;

	CString titile;
	titile.Format("ElementData-[%s]",g_szProject);
	pMainFrame->SetWindowText(titile);
	pMainFrame->SetTitle(titile);
	// The main window has been initialized, so show and update it.
	pMainFrame->ShowWindow(SW_SHOWMAXIMIZED);
	pMainFrame->UpdateWindow();
	
	//	Create A3D engine after main frame shown, because before that render window's
	//	size is zero.
	if (!g_Render.Init(m_hInstance, pMainFrame->GetSafeHwnd()))
	{
		g_Log.Log("Failed to initlaize render");
		return FALSE;
	}
	
	CPropertyBox::g_pA3DEngine = g_Render.GetA3DEngine();
	return TRUE;
}

extern abase::vector<AString> _factions;
extern abase::vector<AString> _monster_faction;

BOOL RefreshBaseDataSettings()
{
	FILE* fp = fopen(_org_config_path, "rb");
	if (!fp) 
	{
		CString err_msg;
		err_msg.Format("不能打开文件 %s ,你需要从服务器上取得最新数据吗?",_org_config_path);
		if(IDYES == AfxMessageBox(err_msg,MB_ICONQUESTION|MB_YESNO))
		{
			AfxMessageBox("程序从服务器取数据开始,该过程没有窗口,取完数据后程序恢复运行!");
			err_msg.Format("%sBaseData",g_szWorkDir);
			CreateDirectory(err_msg,NULL);
			err_msg.Format("%sTalkData",g_szWorkDir);
			CreateDirectory(err_msg,NULL);

			g_VSS.SetProjectPath("BaseData");
			g_VSS.GetAllFiles("BaseData");
			g_VSS.SetProjectPath("TalkData");
			g_VSS.GetAllFiles("TalkData");
			
			fp = fopen(_org_config_path, "rb");
			if(fp==NULL)
			{
				err_msg.Format("不能打开文件 %s ",_org_config_path);
				AfxMessageBox(err_msg,MB_ICONSTOP);
				return false;
			}
		}else return false;
	}

	_org_paths.clear();
	char line[MAX_LINE_LEN];
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

	fp = fopen("BaseData\\factions.dat", "rb");
	if (!fp) return FALSE;

	_factions.clear();
	while (ReadLine(fp, line) && strlen(line) != 0)
		_factions.push_back(AString(line));
	fclose(fp);

	fp = fopen("BaseData\\monster_faction.dat", "rb");
	if (!fp) return FALSE;

	_monster_faction.clear();
	while (ReadLine(fp, line) && strlen(line) != 0)
		_monster_faction.push_back(AString(line));
	fclose(fp);

	g_BaseIDMan.Release();
	if (g_BaseIDMan.Load("BaseData\\TemplID.dat") != 0)
	{
		AfxMessageBox("打开BaseID文件失败", MB_ICONSTOP);
		return FALSE;
	}

	g_TaskIDMan.Release();
	if (g_TaskIDMan.Load("BaseData\\TaskID.dat") != 0)
	{
		AfxMessageBox("打开TaskID文件失败", MB_ICONSTOP);
		return FALSE;
	}

	g_ExtBaseIDMan.Release();
	if (g_ExtBaseIDMan.Load("BaseData\\ExtDataID.dat") != 0)
	{
		AfxMessageBox("打开ExtID文件失败", MB_ICONSTOP);
		return FALSE;
	}

	g_TalkIDMan.Release();
	if (g_TalkIDMan.Load("TalkData\\TalkID.dat") != 0)
	{
		AfxMessageBox("打开TalkID文件失败", MB_ICONSTOP);
		return FALSE;
	}
	
	g_FaceIDMan.Release();
	if (g_FaceIDMan.Load("BaseData\\FaceID.dat") != 0)
	{
		AfxMessageBox("打开FaceID文件失败", MB_ICONSTOP);
		return FALSE;
	}

	g_RecipeIDMan.Release();
	if (g_RecipeIDMan.Load("BaseData\\RecipeID.dat") != 0)
	{
		AfxMessageBox("打开RecipeID文件失败", MB_ICONSTOP);
		return FALSE;
	}
	
	g_ConfigIDMan.Release();
	if (g_ConfigIDMan.Load("BaseData\\ConfigID.dat") != 0)
	{
		AfxMessageBox("打开ConfigID文件失败", MB_ICONSTOP);
		return FALSE;
	}
	
	return TRUE;
}

bool CElementDataApp::InitVss()
{
	while(1)
	{
		if(!g_bLinkVss) 
		{
			g_VSS.InitVss(NULL,true);
			AfxMessageBox("服务器连接被手动断开，请检查VSS设置!");
			return true;
		}

		if (!SetEnvironmentVariable("SSDIR", g_szServerPath)
			||	!SetEnvironmentVariable("SSUSER", g_szUserName)
			|| !SetEnvironmentVariable("SSPWD", g_szPassword))
			return false;
		
		g_VSS.InitVss(NULL);
		g_VSS.SetUserName(g_szUserName);
		g_VSS.SetPassword(g_szPassword);
		g_VSS.SetServerPath(g_szServerPath);
		
		CString _fileType = g_VSS.GetFileTypes("s_d_project.ini");
		if(!g_VSS.TestVssConnected(_fileType))
		{
			AfxMessageBox("No VSS database!");
			COptionVssDlg dlg;
			if(IDOK==dlg.DoModal())
			{
				strcpy(g_szUserName,dlg.m_strName);
				strcpy(g_szPassword,dlg.m_strPassword);
				strcpy(g_szServerPath,dlg.m_strServerPath);
			}else return FALSE;
		}else
		{
			//user name or password is not right.
			if( _fileType.Find(_T("Username"))>=0 && _fileType.Find(_T("Password"))>=0 )
			{
				AfxMessageBox("User name or Password is wrong!");
				COptionVssDlg dlg;
				if(IDOK==dlg.DoModal())
				{
					strcpy(g_szUserName,dlg.m_strName);
					strcpy(g_szPassword,dlg.m_strPassword);
					strcpy(g_szServerPath,dlg.m_strServerPath);
				}else return FALSE;
			}else return TRUE;
		}
	}
	
	return TRUE;
}

//	Do some initial work
bool CElementDataApp::InitApp()
{
	//	Get current directory
	char szAppPath[MAX_PATH];
	GetCurrentDirectory(MAX_PATH, szAppPath);
	strcat(szAppPath, "\\");
	CString temp(szAppPath);
	temp.Replace("\\\\","\\");
	strcpy(szAppPath,temp);
	strcpy(g_szIniFile, szAppPath);
	strcat(g_szIniFile, "ElementData.ini");
	//	Get work path
	//GetPrivateProfileString("Path", "WorkPath", szAppPath, g_szWorkDir, MAX_PATH, g_szIniFile);
	strcpy(g_szWorkDir,szAppPath);
	//	Set work path
	af_Initialize();
	af_SetBaseDir(g_szWorkDir);
	LoadConfigs();
	g_Record.Read();

	if(!InitVss()) return false;

	CLogoDlg dlg;
	dlg.DoModal();
	//	Open log file
	g_EnumTypes.ReadTypes();
	if (!RefreshBaseDataSettings())
		return false;

    g_Log.Init("ElementData.log", "Element data log ...");
	LoadAllCursor();
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
void CElementDataApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

/////////////////////////////////////////////////////////////////////////////
// CElementDataApp message handlers


int CElementDataApp::ExitInstance() 
{
	// TODO: Add your specialized code here and/or call the base class
	g_Render.Release();
	af_Finalize();
	SaveConfigs();
	g_Record.Save();
	return CWinApp::ExitInstance();
}

CDocument* CElementDataApp::OpenDocumentFile(LPCTSTR lpszFileName) 
{
	// TODO: Add your specialized code here and/or call the base class
	return CWinApp::OpenDocumentFile(lpszFileName);
}

void CElementDataApp::OnFileNew() 
{
	// TODO: Add your command handler code here
	
}

CDocument* CElementDataApp::GetDocument(CString pathName)
{
	CString path(g_szWorkDir);
	path = path + pathName;
	POSITION pos = pDocTemplate->GetFirstDocPosition();
	while(pos)
	{
		CDocument* pDoc = pDocTemplate->GetNextDoc(pos);
		if(pDoc) 
		{
			CString docPathName = pDoc->GetPathName();
			if(stricmp(docPathName,path)==0)
			{
				return pDoc;
			}
		}
	}
	return NULL;
}

void CElementDataApp::OnSaveAllDoc() 
{
	// TODO: Add your command handler code here
	POSITION pos = pDocTemplate->GetFirstDocPosition();
	while(pos)
	{
		CElementDataDoc* pDoc = (CElementDataDoc*)pDocTemplate->GetNextDoc(pos);
		if(pDoc) 
		{
			pDoc->Save();
		}
	}
}

void CElementDataApp::UpdateAllDocData()
{
	POSITION pos = pDocTemplate->GetFirstDocPosition();
	while(pos)
	{
		CElementDataDoc* pDoc = (CElementDataDoc*)pDocTemplate->GetNextDoc(pos);
		if(pDoc) 
		{
			if(pDoc->IsModifed()) 
				pDoc->SetModifiedFlag(true);
		}
	}
}

void CElementDataApp::OnUpdateSaveAllDoc(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	POSITION pos = pDocTemplate->GetFirstDocPosition();
	if(pos==NULL)
		pCmdUI->Enable(false);
	else pCmdUI->Enable(true);
}

BOOL CElementDataApp::PreTranslateMessage(MSG* pMsg) 
{
	// TODO: Add your specialized code here and/or call the base class
	return CWinApp::PreTranslateMessage(pMsg);
}

BOOL CElementDataApp::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo) 
{
	// TODO: Add your specialized code here and/or call the base class
	
	return CWinApp::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}

//  Get project's name from server
bool CElementDataApp::GetProjectName()
{
	// Get projlist 
	CString str;
	str.Format("%s",g_szWorkDir);
	g_VSS.SetProjectPath("");
	g_VSS.SetWorkFolder(str);
	g_VSS.GetFile("s_d_project.ini");
	
	strcpy(g_szProject,"");
	char szPath[256];
	sprintf(szPath,"%ss_d_project.ini",g_szWorkDir);
	GetPrivateProfileString("Data Project", "ProjectName", g_szProject, g_szProject, 256, szPath);
	CString msg(g_szProject);
	if(msg.IsEmpty())
	{
		AfxMessageBox("警告: 没项目配置文件或是项目名为空!");
		return false;
	}

	char	szVersion[32];
	szVersion[0] = '\0';
	GetPrivateProfileString("Data Project", "Version", szVersion, szVersion, 32, szPath);
	if( stricmp(g_szVersion, szVersion) != 0  && g_bLinkVss)
	{
		AfxMessageBox("错误: ElementData.exe 不是当前最新版本，请更新后再进行使用!");
		return false;
	}
	return true;
}
