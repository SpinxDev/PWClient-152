// MainFrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "AudioEditor.h"
#include "DlgCreateProject.h"
#include "DlgOpenProject.h"
#include "Project.h"
#include "Engine.h"
#include "Global.h"
#include "MainFrm.h"
#include "DlgReverb.h"
#include "AudioEditorView.h"
#include "DlgUploadRes.h"
#include "DlgDownloadRes.h"
#include "DlgSettings.h"

using AudioEngine::EventMap;
using AudioEngine::EventGroup;
//#include <vld.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CBCGPFrameWnd)

const int  iMaxUserToolbars		= 10;
const UINT uiFirstUserToolBarId	= AFX_IDW_CONTROLBAR_FIRST + 40;
const UINT uiLastUserToolBarId	= uiFirstUserToolBarId + iMaxUserToolbars - 1;

BEGIN_MESSAGE_MAP(CMainFrame, CBCGPFrameWnd)
	ON_WM_CREATE()
	ON_COMMAND(ID_VIEW_CUSTOMIZE, OnViewCustomize)
	ON_REGISTERED_MESSAGE(BCGM_RESETTOOLBAR, OnToolbarReset)
	ON_COMMAND_RANGE(ID_VIEW_APPLOOK_2000, ID_VIEW_APPLOOK_VS2008, OnAppLook)
	ON_UPDATE_COMMAND_UI_RANGE(ID_VIEW_APPLOOK_2000, ID_VIEW_APPLOOK_VS2008, OnUpdateAppLook)
	ON_COMMAND(ID_FILE_NEW, &CMainFrame::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, &CMainFrame::OnFileOpen)
	ON_COMMAND(ID_MY_FILE_SAVE, &CMainFrame::OnFileSave)
	ON_UPDATE_COMMAND_UI(ID_MY_FILE_SAVE, &CMainFrame::OnUpdateFileSave)
	ON_WM_CLOSE()
	ON_COMMAND(ID_REVERB_EDITOR, &CMainFrame::OnReverbEditor)
	ON_COMMAND(ID_UPLOAD_RES, &CMainFrame::OnUploadRes)
	ON_COMMAND(ID_DOWNLOAD_RES, &CMainFrame::OnDownloadRes)
	ON_COMMAND(ID_SETTINGS, &CMainFrame::OnSettings)
	ON_COMMAND(ID_EXPORT_TEXT, &CMainFrame::OnExportText)
	ON_COMMAND(ID_EXPORT_BINARY_PROJECT, &CMainFrame::OnExportBinaryProject)
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

// CMainFrame construction/destruction

CMainFrame::CMainFrame()
{

	m_nAppLook = theApp.GetInt (_T("ApplicationLook"), ID_VIEW_APPLOOK_2003);

	m_bOldModifyState = false;
}

CMainFrame::~CMainFrame()
{
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CBCGPFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	OnAppLook (m_nAppLook);

	// VISUAL_MANAGER
	
	if (CBCGPToolBar::GetUserImages () == NULL)
	{
		// Load toolbar user images:
		if (!m_UserImages.Load (_T(".\\UserImages.bmp")))
		{
			TRACE(_T("Failed to load user images\n"));
		}
		else
		{
			CBCGPToolBar::SetUserImages (&m_UserImages);
		}
	}

	CBCGPToolBar::EnableQuickCustomization ();


	// TODO: Define your own basic commands. Be sure, that each pulldown 
	// menu have at least one basic command.

	CList<UINT, UINT>	lstBasicCommands;

	lstBasicCommands.AddTail (ID_VIEW_TOOLBARS);
	lstBasicCommands.AddTail (ID_FILE_NEW);
	lstBasicCommands.AddTail (ID_FILE_OPEN);
	lstBasicCommands.AddTail (ID_FILE_SAVE);
	lstBasicCommands.AddTail (ID_FILE_PRINT);
	lstBasicCommands.AddTail (ID_APP_EXIT);
	lstBasicCommands.AddTail (ID_EDIT_CUT);
	lstBasicCommands.AddTail (ID_EDIT_PASTE);
	lstBasicCommands.AddTail (ID_EDIT_UNDO);
	lstBasicCommands.AddTail (ID_RECORD_NEXT);
	lstBasicCommands.AddTail (ID_RECORD_LAST);
	lstBasicCommands.AddTail (ID_APP_ABOUT);
	lstBasicCommands.AddTail (ID_VIEW_TOOLBAR);
	lstBasicCommands.AddTail (ID_VIEW_CUSTOMIZE);
	lstBasicCommands.AddTail (ID_VIEW_APPLOOK_2000);
	lstBasicCommands.AddTail (ID_VIEW_APPLOOK_XP);
	lstBasicCommands.AddTail (ID_VIEW_APPLOOK_2003);
	lstBasicCommands.AddTail (ID_VIEW_APPLOOK_2007);
	lstBasicCommands.AddTail (ID_VIEW_APPLOOK_VS2005);
	lstBasicCommands.AddTail (ID_VIEW_APPLOOK_WIN_XP);
	lstBasicCommands.AddTail (ID_VIEW_APPLOOK_2007_1);
	lstBasicCommands.AddTail (ID_VIEW_APPLOOK_2007_2);
	lstBasicCommands.AddTail (ID_VIEW_APPLOOK_2007_3);
	lstBasicCommands.AddTail (ID_VIEW_APPLOOK_VS2008);

	CBCGPToolBar::SetBasicCommands (lstBasicCommands);	

	if (!m_wndMenuBar.Create (this))
	{
		TRACE0("Failed to create menubar\n");
		return -1;      // fail to create
	}

	m_wndMenuBar.SetBarStyle(m_wndMenuBar.GetBarStyle() | CBRS_SIZE_DYNAMIC);

	// Detect color depth. 256 color toolbars can be used in the
	// high or true color modes only (bits per pixel is > 8):
	CClientDC dc (this);
	BOOL bIsHighColor = dc.GetDeviceCaps (BITSPIXEL) > 8;

	UINT uiToolbarHotID = bIsHighColor ? IDB_TOOLBAR256 : 0;

	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndToolBar.LoadToolBar(IDR_MAINFRAME, 0, 0, FALSE, 0, 0, uiToolbarHotID))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}

	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators,
		  sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}

	// Load control bar icons:
	CBCGPToolBarImages imagesWorkspace;
	imagesWorkspace.SetImageSize (CSize (16, 16));
	imagesWorkspace.SetTransparentColor (RGB (255, 0, 255));
	imagesWorkspace.Load (IDB_WORKSPACE);
	
	if (!m_wndEventBar.Create (_T("Events"), this, CRect (0, 0, 200, 200),
		TRUE, ID_VIEW_EVENT,
		WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_LEFT))
	{
		TRACE0("Failed to create event bar\n");
		return -1;      // fail to create
	}

	m_wndEventBar.SetIcon (imagesWorkspace.ExtractIcon (0), FALSE);

	if (!m_wndSoundDefBar.Create (_T("Sound Def"), this, CRect (0, 0, 200, 200),
		TRUE, ID_VIEW_SOUNDDEF,
		WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_LEFT | CBRS_FLOAT_MULTI))
	{
		TRACE0("Failed to create sound def bar\n");
		return -1;      // fail to create
	}

	m_wndSoundDefBar.SetIcon (imagesWorkspace.ExtractIcon (1), FALSE);

	if (!m_wndBankBar.Create (_T("Bank"), this, CRect (0, 0, 200, 200),
		TRUE, ID_VIEW_BANK,
		WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_LEFT | CBRS_FLOAT_MULTI))
	{
		TRACE0("Failed to create bank bar\n");
		return -1;      // fail to create
	}

	m_wndBankBar.SetIcon (imagesWorkspace.ExtractIcon (2), FALSE);

	if (!m_wndProperty.Create (_T("属性"), this, CRect (0, 0, 200, 200),
		TRUE, ID_VIEW_PROPERTY,
		WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_RIGHT | CBRS_FLOAT_MULTI))
	{
		TRACE0("Failed to create property bar\n");
		return -1;      // fail to create
	}

	m_wndProperty.SetIcon (imagesWorkspace.ExtractIcon (3), FALSE);


	if (!m_wndOutput.Create (_T("Output"), this, CSize (150, 150),
		TRUE /* Has gripper */, ID_VIEW_OUTPUT,
		WS_CHILD | WS_VISIBLE | CBRS_BOTTOM))
	{
		TRACE0("Failed to create output bar\n");
		return -1;      // fail to create
	}

	CString strMainToolbarTitle;
	strMainToolbarTitle.LoadString (IDS_MAIN_TOOLBAR);
	m_wndToolBar.SetWindowText (strMainToolbarTitle);

	// TODO: Delete these three lines if you don't want the toolbar to be dockable
	m_wndMenuBar.EnableDocking(CBRS_ALIGN_ANY);
	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	m_wndEventBar.EnableDocking(CBRS_ALIGN_ANY);
	m_wndSoundDefBar.EnableDocking(CBRS_ALIGN_ANY);
	m_wndBankBar.EnableDocking(CBRS_ALIGN_ANY);
	m_wndOutput.EnableDocking(CBRS_ALIGN_ANY);
	m_wndProperty.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);
	EnableAutoHideBars(CBRS_ALIGN_ANY);
	DockControlBar(&m_wndMenuBar);
	DockControlBar(&m_wndToolBar);
	DockControlBar(&m_wndEventBar);
	m_wndSoundDefBar.AttachToTabWnd (&m_wndEventBar, BCGP_DM_STANDARD, FALSE, NULL);
	m_wndBankBar.AttachToTabWnd (&m_wndEventBar, BCGP_DM_STANDARD, FALSE, NULL);
	DockControlBar(&m_wndOutput);
	DockControlBar(&m_wndProperty);


	m_wndToolBar.EnableCustomizeButton (TRUE, ID_VIEW_CUSTOMIZE, _T("Customize..."));

	// Allow user-defined toolbars operations:
	InitUserToobars (NULL,
					uiFirstUserToolBarId,
					uiLastUserToolBarId);

	// Enable control bar context menu (list of bars + customize command):
	EnableControlBarMenu (	
		TRUE,				// Enable
		ID_VIEW_CUSTOMIZE, 	// Customize command ID
		_T("Customize..."),	// Customize command text
		ID_VIEW_TOOLBARS);	// Menu items with this ID will be replaced by
							// toolbars menu

	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CBCGPFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return TRUE;
}


// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CBCGPFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CBCGPFrameWnd::Dump(dc);
}

#endif //_DEBUG


// CMainFrame message handlers




void CMainFrame::OnViewCustomize()
{
	//------------------------------------
	// Create a customize toolbars dialog:
	//------------------------------------
	CBCGPToolbarCustomize* pDlgCust = new CBCGPToolbarCustomize (this,
		TRUE /* Automatic menus scaning */);

	pDlgCust->EnableUserDefinedToolbars ();
	pDlgCust->Create ();
}

afx_msg LRESULT CMainFrame::OnToolbarReset(WPARAM /*wp*/,LPARAM)
{
	// TODO: reset toolbar with id = (UINT) wp to its initial state:
	//
	// UINT uiToolBarId = (UINT) wp;
	// if (uiToolBarId == IDR_MAINFRAME)
	// {
	//		do something with m_wndToolBar
	// }

	return 0;
}

void CMainFrame::OnAppLook(UINT id)
{
	CBCGPDockManager::SetDockMode (BCGP_DT_SMART);

	m_nAppLook = id;

	CBCGPTabbedControlBar::m_StyleTabWnd = CBCGPTabWnd::STYLE_3D;

	switch (m_nAppLook)
	{
	case ID_VIEW_APPLOOK_2000:
		// enable Office 2000 look:
		CBCGPVisualManager::SetDefaultManager (RUNTIME_CLASS (CBCGPVisualManager));
		break;

	case ID_VIEW_APPLOOK_XP:
		// enable Office XP look:
		CBCGPVisualManager::SetDefaultManager (RUNTIME_CLASS (CBCGPVisualManagerXP));
		break;

	case ID_VIEW_APPLOOK_WIN_XP:
		// enable Windows XP look (in other OS Office XP look will be used):
		CBCGPWinXPVisualManager::m_b3DTabsXPTheme = TRUE;
		CBCGPVisualManager::SetDefaultManager (RUNTIME_CLASS (CBCGPWinXPVisualManager));
		break;

	case ID_VIEW_APPLOOK_2003:
		// enable Office 2003 look:
		CBCGPVisualManager::SetDefaultManager (RUNTIME_CLASS (CBCGPVisualManager2003));
		CBCGPDockManager::SetDockMode (BCGP_DT_SMART);
		break;

	case ID_VIEW_APPLOOK_2007:
	case ID_VIEW_APPLOOK_2007_1:
	case ID_VIEW_APPLOOK_2007_2:
	case ID_VIEW_APPLOOK_2007_3:
		// enable Office 2007 look:
		switch (m_nAppLook)
		{
		case ID_VIEW_APPLOOK_2007:
			CBCGPVisualManager2007::SetStyle (CBCGPVisualManager2007::VS2007_LunaBlue);
			break;

		case ID_VIEW_APPLOOK_2007_1:
			CBCGPVisualManager2007::SetStyle (CBCGPVisualManager2007::VS2007_ObsidianBlack);
			break;

		case ID_VIEW_APPLOOK_2007_2:
			CBCGPVisualManager2007::SetStyle (CBCGPVisualManager2007::VS2007_Silver);
			break;

		case ID_VIEW_APPLOOK_2007_3:
			CBCGPVisualManager2007::SetStyle (CBCGPVisualManager2007::VS2007_Aqua);
			break;
		}

		CBCGPVisualManager::SetDefaultManager (RUNTIME_CLASS (CBCGPVisualManager2007));
		CBCGPDockManager::SetDockMode (BCGP_DT_SMART);
		break;

	case ID_VIEW_APPLOOK_VS2005:
		// enable VS 2005 look:
		CBCGPVisualManager::SetDefaultManager (RUNTIME_CLASS (CBCGPVisualManagerVS2005));
		CBCGPDockManager::SetDockMode (BCGP_DT_SMART);
		break;

	case ID_VIEW_APPLOOK_VS2008:
		// enable VS 2008 look:
		CBCGPVisualManager::SetDefaultManager (RUNTIME_CLASS (CBCGPVisualManagerVS2008));
		CBCGPDockManager::SetDockMode (BCGP_DT_SMART);
		break;
	}

	CBCGPDockManager* pDockManager = GetDockManager ();
	if (pDockManager != NULL)
	{
		ASSERT_VALID (pDockManager);
		pDockManager->AdjustBarFrames ();
	}

	CBCGPTabbedControlBar::ResetTabs ();

	RecalcLayout ();
	RedrawWindow (NULL, NULL, RDW_ALLCHILDREN | RDW_INVALIDATE | RDW_UPDATENOW | RDW_ERASE);

	theApp.WriteInt (_T("ApplicationLook"), m_nAppLook);
}

void CMainFrame::OnUpdateAppLook(CCmdUI* pCmdUI)
{
	pCmdUI->SetRadio (m_nAppLook == pCmdUI->m_nID);
}
 // RIBBON_APP

void CMainFrame::OnFileNew()
{
	if(!closeProject())
		return;
	CDlgCreateProject dlg;
	if(IDOK!=dlg.DoModal())
		return;
	CString csTitle = "音频编辑器 - ";
	csTitle += Engine::GetInstance().GetCurProject()->GetEventProject()->GetName();
	SetWindowText(csTitle);
	CString csTips;
	csTips.Format("创建工程“%s”成功。", Engine::GetInstance().GetCurProject()->GetEventProject()->GetName());
	MessageBox(csTips, "创建成功", MB_ICONINFORMATION);
	GF_Log(LOG_NORMAL, csTips);
}

void CMainFrame::OnFileOpen()
{
	if(!closeProject())
		return;
	CDlgOpenProject dlg;
	if(IDOK!=dlg.DoModal())
		return;
	CString csName = Engine::GetInstance().GetCurProject()->GetEventProject()->GetName();
	CString csTitle = "音频编辑器 - ";
	csTitle += csName;
	SetWindowText(csTitle);
	GF_Log(LOG_NORMAL, "成功打开工程“%s”", csName);
}

void CMainFrame::OnFileSave()
{
	Project* pProject = Engine::GetInstance().GetCurProject();
	if(!pProject)
		return;
	CString csName = pProject->GetEventProject()->GetName();
	if(!pProject->Save())
	{
		GF_Log(LOG_ERROR, "保存工程“%s”失败", csName);
		return;
	}
	GF_Log(LOG_NORMAL, "成功保存工程“%s”", csName);
}

void CMainFrame::OnUpdateFileSave(CCmdUI *pCmdUI)
{
	if(!Engine::GetInstance().GetCurProject())
	{
		pCmdUI->Enable(FALSE);
		return;
	}
	bool bModified = Engine::GetInstance().GetCurProject()->IsModified();
	if(m_bOldModifyState != bModified)
	{
		CString csTitle = "音频编辑器 - ";
		csTitle += Engine::GetInstance().GetCurProject()->GetEventProject()->GetName();
		if(bModified)
			csTitle += " *";
		SetWindowText(csTitle);
		m_bOldModifyState = bModified;
	}
	if(bModified)
		pCmdUI->Enable(TRUE);
	else
		pCmdUI->Enable(FALSE);
}

void CMainFrame::OnClose()
{
	if(!closeProject())
		return;

	CBCGPFrameWnd::OnClose();
}

bool CMainFrame::closeProject()
{
	if(Engine::GetInstance().GetCurProject())
	{
		if(Engine::GetInstance().GetCurProject()->IsModified())
		{
			int id = MessageBox("工程有修改，是否保存？", "警告", MB_ICONWARNING|MB_YESNOCANCEL);
			if(IDYES==id)
			{
				if(!Engine::GetInstance().GetCurProject()->Save())
				{
					GF_Log(LOG_ERROR, "保存工程失败");
					return false;
				}
				goto _success;
			}
			else if(IDCANCEL == id)
			{
				return false;
			}
		}
		GF_Log(LOG_NORMAL, "成功关闭工程");
	}
_success:
	SetWindowText("音频编辑器");
	Engine::GetInstance().CloseProject();
	GetBankBar()->UpdateTree();
	GetSoundDefBar()->UpdateTree();
	GetEventBar()->UpdateTree();
	GF_GetView()->GetEventView()->SetEvent(0);
	GF_GetView()->GetSoundDefView()->SetTreeItem(0);
	GF_GetView()->GetBankView()->SetTreeItem(0);
	return true;
}

void CMainFrame::OnReverbEditor()
{
	CDlgReverb dlg;
	dlg.DoModal();
}

void CMainFrame::OnUploadRes()
{
	CDlgUploadRes dlg;
	dlg.DoModal();
}

void CMainFrame::OnDownloadRes()
{
	CDlgDownloadRes dlg;
	dlg.DoModal();
}

void CMainFrame::OnSettings()
{
	CDlgSettings dlg;
	dlg.DoModal();
}

void CMainFrame::OnExportText()
{
	Project* pProject = Engine::GetInstance().GetCurProject();
	if(!pProject)
	{
		MessageBox("请先打开工程", "提示", MB_ICONINFORMATION);
		return;
	}
	EventManager* pEventManager = pProject->GetEventManager();
	if(!pEventManager)
		return;
	char szName[MAX_PATH] = {0};
	char szFullPath[MAX_PATH] = {0};
	sprintf(szName, "%s.txt", pProject->GetName());
	static char BASED_CODE szFilter[] = "Text Files (*.txt)|*.txt|All Files (*.*)|*.*||";
	CFileDialog dlg(FALSE, "txt", szName, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, szFilter);
	if(IDOK != dlg.DoModal())
		return;

	FILE* pFile = fopen(dlg.GetPathName(), "w");
	int iEventGroupNum = pEventManager->GetEventGroupNum();
	for (int i=0; i<iEventGroupNum; ++i)
	{
		exportEventGroup(pEventManager->GetEventGroupByIndex(i), pFile);
	}
	fclose(pFile);
	MessageBox("导出成功", "提示", MB_ICONINFORMATION);
}

void CMainFrame::exportEventGroup(AudioEngine::EventGroup* pEventGroup, FILE* pFile)
{
	if(!pEventGroup || !pFile)
		return;
	int iEventNum = pEventGroup->GetEventNum();
	if(iEventNum > 0)
	{
		char szLine[2048] = {0};
		for (int i=0; i<iEventNum; ++i)
		{
			Event* pEvent = pEventGroup->GetEventByIndex(i);
			sprintf(szLine, "%s\t%s\r\n", pEvent->GetGuid().GetString(), pEvent->GetFullPath());
			fwrite(szLine, strlen(szLine), 1, pFile);
		}
	}
	int iGroupNum = pEventGroup->GetEventGroupNum();
	for (int i=0; i<iGroupNum; ++i)
	{
		exportEventGroup(pEventGroup->GetEventGroupByIndex(i), pFile);
	}
}

int CALLBACK BrowseCallbackProc(HWND   hwnd,   UINT   msg,   LPARAM   lp,   LPARAM   pData)   
{
	if (msg == BFFM_INITIALIZED)   
	{
		::SendMessage(hwnd,BFFM_SETSELECTIONA,TRUE,(LPARAM)g_Configs.szExportBinaryPath);
	}
	return 0;
}

void CMainFrame::OnExportBinaryProject()
{
	closeProject();
	BROWSEINFOA Info;
	memset(&Info, 0, sizeof (Info));

	static char szPath[MAX_PATH] = "\0";

	Info.hwndOwner		= m_hWnd;
	Info.pidlRoot		= NULL;
	Info.pszDisplayName	= 0;
	Info.lpszTitle		= "选择导出目录 :";
	Info.lpfn			= BrowseCallbackProc;
	Info.ulFlags		= BIF_RETURNONLYFSDIRS | BIF_STATUSTEXT | BIF_DONTGOBELOWDOMAIN;	

	LPITEMIDLIST pPIDL;
	if (!(pPIDL = SHBrowseForFolderA(&Info)))
		return;

	SHGetPathFromIDListA(pPIDL, szPath);
	strcpy(g_Configs.szExportBinaryPath, szPath);

	char szAudioPath[MAX_PATH] = {0};
	sprintf_s(szAudioPath, MAX_PATH, "%s\\Audio", szPath);
	GF_CreateDirectory(szAudioPath);

	char szProjectsPath[MAX_PATH] = {0};
	sprintf_s(szProjectsPath, MAX_PATH, "%s\\Projects", szAudioPath);
	GF_CreateDirectory(szProjectsPath);

	char szReverbsPath[MAX_PATH] = {0};
	sprintf_s(szReverbsPath, MAX_PATH, "%s\\Reverbs", szAudioPath);
	GF_CreateDirectory(szReverbsPath);


	char szNewPrjListFile[MAX_PATH] = {0};
	sprintf_s(szNewPrjListFile, MAX_PATH, "%s\\projectlist.txt", szProjectsPath);
	if(!::CopyFile(g_Configs.szProjectListFile, szNewPrjListFile, FALSE))
	{
		MessageBox("拷贝工程文件失败", "提示", MB_ICONERROR);
		return;
	}

	char szNewReverbsFile[MAX_PATH] = {0};
	sprintf_s(szNewReverbsFile, MAX_PATH, "%s\\reverbs.xml", szReverbsPath);
	char szOldReverbsFile[MAX_PATH] = {0};
	sprintf_s(szOldReverbsFile, MAX_PATH, "%s\\reverbs.xml", g_Configs.szReverbsPath);
	if(!::CopyFile(szOldReverbsFile, szNewReverbsFile, FALSE))
	{
		MessageBox("拷贝Reverb文件失败", "提示", MB_ICONERROR);
		return;
	}

	Engine::GetInstance().GetEventSystem()->SetRootFolder("Audio");
	Engine::GetInstance().GetEventSystem()->LoadProjects();
	int iPrjNum = Engine::GetInstance().GetEventSystem()->GetProjectNum();
	for (int i=0; i<iPrjNum; ++i)
	{
		AudioEngine::EventProject* pEventProject = Engine::GetInstance().GetEventSystem()->GetProjectByIndex(i);
		if(!pEventProject)
			continue;
		char szNewProjectPath[MAX_PATH] = {0};
		sprintf_s(szNewProjectPath, MAX_PATH, "%s\\%s", szProjectsPath, pEventProject->GetName());
		GF_CreateDirectory(szNewProjectPath);
		char szNewProjectFile[MAX_PATH] = {0};
		sprintf_s(szNewProjectFile, MAX_PATH, "%s\\%s.data", szNewProjectPath, pEventProject->GetName());
		if(!pEventProject->Save(szNewProjectFile))
		{
			MessageBox("保存二进制工程失败", "提示", MB_ICONERROR);
			return;
		}
	}
	MessageBox("导出成功", "提示", MB_ICONINFORMATION);
}

BOOL CMainFrame::PreTranslateMessage(MSG* pMsg)
{
	if(pMsg->message == WM_KEYDOWN)
	{
		CAudioEditorView* pView = dynamic_cast<CAudioEditorView*>(GetActiveView());
		if(pView && pView->GetEventView()->IsWindowVisible())
		{
			CEventView* pEventView = pView->GetEventView();
			switch(pMsg->wParam)
			{
			case '1':
				pEventView->OnClickPlayBtn(0, 0);
				break;
			case '2':
				pEventView->OnClickPauseBtn(0, 0);
				break;
			case '3':
				pEventView->OnClickStopBtn(0, 0);
				break;
			}
		}		
	}

	return CBCGPFrameWnd::PreTranslateMessage(pMsg);
}
