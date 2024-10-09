// MainFrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "Global.h"
#include "ElementData.h"
#include "MainFrm.h"
#include "OptionVssDlg.h"
#include "VssOperation.h"
#include "EnumTypes.h"
#include "BaseDataTemplate.h"
#include "ExtendDataTempl.h"
#include "ExpTypes.h"
#include "BaseDataExp.h"
#include "TalkModifyDlg.h"
#include "BaseDataIDMan.h"
#include "WipeOffDlg.h"
#include "PasswdDlg.h"
#include "PolicyDlg.h"
#include "MounsterFallListDlg.h"
#include "TreasureDlg.h"
#include "DlgFindTemplate.h"
#include "DlgFindTemplResult.h"
#include "DlgProgressNotify.h"
#include <shlwapi.h>
#include <process.h>
#define new A_DEBUG_NEW

/////////////////////////////////////////////////////////////////////////////
// CMainFrame

IMPLEMENT_DYNAMIC(CMainFrame, CMDIFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CMDIFrameWnd)
	//{{AFX_MSG_MAP(CMainFrame)
	ON_WM_CREATE()
	ON_COMMAND(ID_OPTION_VSS, OnOptionVss)
	ON_COMMAND(ID_VSS_FRESH, OnVssFresh)
	ON_COMMAND(ID_EXPORT_DATA, OnExportData)
	ON_COMMAND(ID_TEST_TALK, OnTestTalk)
	ON_COMMAND(ID_CHECK, OnCheck)
	ON_COMMAND(ID_POLICY_EDITOR, OnPolicyEditor)
	ON_COMMAND(ID_EXPORT_POLICY_DATA, OnExportPolicyData)
	ON_COMMAND(ID_TRANS_VERSION, OnTransVersion)
	ON_COMMAND(ID_GET_POLICY_DATA, OnGetPolicyData)
	ON_COMMAND(ID_MOUNSTER_FALL_ITEM_EDITOR, OnMounsterFallItemEditor)
	ON_COMMAND(ID_ITEMS_EDITOR, OnItemsEditor)
	ON_COMMAND(ID_ITEMS2_EDITOR, OnItems2Editor)
	ON_COMMAND(ID_QUICKLY_GET_ALL_DATA, OnQuicklyGetAllData)
	ON_COMMAND(ID_FIND_TEMPLATE, OnFindTemplate)
	ON_COMMAND(ID_TEST_EXPORT_DATA, OnTestExportData)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

/////////////////////////////////////////////////////////////////////////////
// CMainFrame construction/destruction

CMainFrame::CMainFrame()
{
	// TODO: add member initialization code here
	
}

CMainFrame::~CMainFrame()
{
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CMDIFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
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

	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);
	DockControlBar(&m_wndToolBar);
	
	m_wndListBar.Create(this,IDD_DIALOG_LIST_BAR,WS_CHILD | WS_VISIBLE | 
			CBRS_LEFT| CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY |
			CBRS_SIZE_DYNAMIC,IDD_DIALOG_LIST_BAR);
	// TODO: Delete these three lines if you don't want the toolbar to
	//  be dockable
	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	//cs.style |= WS_MAXIMIZE;
	if( !CMDIFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CMDIFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CMDIFrameWnd::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMainFrame message handlers




void CMainFrame::OnOptionVss() 
{
	// TODO: Add your command handler code here
	COptionVssDlg dlg;
	if(IDOK==dlg.DoModal())
	{
		strcpy(g_szUserName,dlg.m_strName);
		strcpy(g_szPassword,dlg.m_strPassword);
		strcpy(g_szServerPath,dlg.m_strServerPath);
		((CElementDataApp*)AfxGetApp())->InitVss();
	}
}

extern BOOL RefreshBaseDataSettings();
extern CEnumTypes g_EnumTypes;
static const char* _update_log = "BaseData\\update_log.txt";

void CMainFrame::OnVssFresh()
{
	if(AfxMessageBox("你确定要从服务器上取得最新数据吗？",MB_YESNO|MB_ICONQUESTION)!=IDYES)
		return;
	
	g_VSS.SetProjectPath("BaseData");
	g_VSS.CheckOutFile(_update_log);
	
	if (FileIsReadOnly(_update_log))
	{
		MessageBox("打开update记录失败，请稍后重试！");
		return;
	}
	
	FILE* fp = fopen(_update_log, "r+b");
	
	if (!fp)
	{
		g_VSS.SetProjectPath("BaseData");
		g_VSS.CheckInFile("update_log.txt");
		MessageBox("读写update记录失败！");
		return;
	}
	
	int seq = 0;
	fscanf(fp, "%d", &seq);
	fseek(fp, 0, SEEK_SET);
	fprintf(fp, "%d", seq+1);
	fclose(fp);
	
	g_VSS.SetProjectPath("BaseData");
	g_VSS.CheckInFile("update_log.txt");
	
	if (!FileIsReadOnly(_update_log))
	{
		MessageBox("Checkin update记录失败！");
		return;
	}
	
	// TODO: Add your command handler code here
	//((CElementDataApp*)AfxGetApp())->CloseAllDoc();
	m_wndListBar.m_ListDlg.UpdateRecord();

	g_VSS.SetProjectPath("BaseData");
	g_VSS.GetAllFiles("BaseData");
	
	g_VSS.SetProjectPath("TalkData");
	g_VSS.GetAllFiles("TalkData");

	g_EnumTypes.Release();
	g_EnumTypes.ReadTypes();
	RefreshBaseDataSettings();

	CString strWildcard;
	strWildcard = g_szWorkDir;
	strWildcard += "BaseData";
	m_wndListBar.m_ListDlg.DeleteAllItems();
	m_wndListBar.m_ListDlg.FreshObjectList(strWildcard,NULL,2);
	m_wndListBar.m_ListDlg.ReInitSearch();
}

struct RECORD
{
	CString op;
	CString user;
	CString version;
	CString fl;
};

typedef abase::vector<RECORD*> RecordVec;

void CMainFrame::OnQuicklyGetAllData()
{
	if (AfxMessageBox("你确定要从服务器上取得最新数据吗？", MB_YESNO | MB_ICONQUESTION) != IDYES)
		return;

	g_VSS.SetProjectPath("BaseData");
	g_VSS.CheckOutFile(_update_log);

	if (FileIsReadOnly(_update_log))
	{
		MessageBox("打开update记录失败，请稍后重试！");
		return;
	}

	FILE* fp = fopen(_update_log, "r+b");

	if (!fp)
	{
		g_VSS.SetProjectPath("BaseData");
		g_VSS.CheckInFile("update_log.txt");
		MessageBox("读写update记录失败！");
		return;
	}

	int seq = 0;
	fscanf(fp, "%d", &seq);
	fseek(fp, 0, SEEK_SET);
	fprintf(fp, "%d", seq+1);
	fclose(fp);

	g_VSS.SetProjectPath("BaseData");
	g_VSS.CheckInFile("update_log.txt");

	if (!FileIsReadOnly(_update_log))
	{
		MessageBox("Checkin update记录失败！");
		return;
	}

	CString strLog = g_szServerPath;
	strLog.TrimRight('\\');
	strLog += "\\logs.txt";

	CFile log;

	if (!log.Open(strLog, CFile::modeRead | CFile::typeBinary | CFile::shareDenyNone))
	{
		MessageBox("打开vss log失败！请务必使用“取得最新模板数据”功能全取数据");
		return;
	}

	unsigned int nLen = (unsigned int)log.GetLength();
	char* content = new char[nLen+1];
	content[nLen] = 0;
	log.Read(content, nLen);
	log.Close();
	char* end = content+nLen;
	char* line_end = NULL;

	RecordVec records;
	CString op;
	CString user;
	CString version;
	CString fl;
	bool is_find = false;
	bool is_ok = false;

	while (end >= content)
	{
		if (end == content)
		{
			if (line_end == NULL)
				break;
		}
		else if (*end == '\r')
		{
			if (line_end == NULL)
			{
				line_end = end;
				end--;
				continue;
			}
		}
		else
		{
			end--;
			continue;
		}

		CString line(end, line_end-end);
		line.TrimLeft();
		line.TrimRight();

		if (!line.IsEmpty())
		{
			if (op.IsEmpty())
				op = line;
			else if (user.IsEmpty())
				user = line;
			else if (version.IsEmpty())
				version = line;
			else
			{
				fl = line;

				char u[256];
				u[0] = 0;
				char d[256];
				d[0] = 0;
				char t[256];
				t[0] = 0;

				sscanf(user, "User: %s Date: %s Time: %s", u, d, t);
				CString uu = u;
				uu.TrimLeft();
				uu.TrimRight();
				bool is_me = (uu.CompareNoCase(g_szUserName) == 0);
				bool is_log = (fl.CompareNoCase("$/BaseData/update_log.txt") == 0);

				if (is_me)
				{
					if (is_log && op.CompareNoCase("Checked in") == 0)
					{
						if (!is_find)
						{
							is_find = true;
							op.Empty();
							user.Empty();
							version.Empty();
							fl.Empty();
							line_end = end;
							end--;
							continue;
						}
						else
						{
							is_ok = true;
							break;
						}
					}
				}

				if (is_find && !is_me && !is_log)
				{
					RECORD* rec = new RECORD;
					rec->op = op;
					rec->user = user;
					rec->version = version;
					rec->fl = fl;
					records.push_back(rec);
				}

				op.Empty();
				user.Empty();
				version.Empty();
				fl.Empty();
			}
		}

		if (end == content)
			break;

		line_end = end;
		end--;
	}

	delete[] content;

	if (!is_ok)
	{
		MessageBox("没有更新log，快速重取失败，请全部更新数据");

		for (size_t i = 0; i < records.size(); i++)
			delete records[i];

		return;
	}

	if (!records.size())
	{
		MessageBox("数据是最新的");
		return;
	}

	for (int i = records.size() - 1; i >= 0; i--)
	{
		RECORD* rec = records[i];

		if (rec->op.CompareNoCase("Checked in") == 0 || rec->op.CompareNoCase("Rolled back") == 0)
		{
			CString ff = rec->fl;
			ff.TrimLeft("$/");
			const char* path = ff;
			char* name = ::PathFindFileNameA(path);
			CString strPath(path, name-path);
			strPath.TrimRight('/');

			g_VSS.SetProjectPath(strPath);
			g_VSS.GetFile(ff);
		}
		else
		{
			char op[256];
			char fl[MAX_PATH];
			op[0] = 0;
			fl[0] = 0;

			sscanf(rec->op, "%s %s", fl, op);

			if (stricmp(op, "created") == 0)
			{
				CString strFile = fl;
				strFile.TrimLeft();
				strFile.TrimRight();

				strFile = rec->fl + "/" + strFile;
				strFile.TrimLeft("$/");
				::CreateDirectory(strFile, NULL);

				g_VSS.SetProjectPath(strFile);
				g_VSS.GetAllFiles(strFile);
			}
			else if (stricmp(op, "added") == 0)
			{
				CString strFile = fl;
				strFile.TrimLeft();
				strFile.TrimRight();
			
				rec->fl.TrimLeft("$/");
				rec->fl.TrimRight("/");
				g_VSS.SetProjectPath(rec->fl);
				g_VSS.GetFile(rec->fl + "/" + strFile);
			}
		}

		delete rec;
	}

	//((CElementDataApp*)AfxGetApp())->CloseAllDoc();
	m_wndListBar.m_ListDlg.UpdateRecord();

	g_EnumTypes.Release();
	g_EnumTypes.ReadTypes();
	RefreshBaseDataSettings();

	CString strWildcard;
	strWildcard = g_szWorkDir;
	strWildcard += "BaseData";
	m_wndListBar.m_ListDlg.DeleteAllItems();
	m_wndListBar.m_ListDlg.FreshObjectList(strWildcard,NULL,2);
	m_wndListBar.m_ListDlg.ReInitSearch();
}

static unsigned int __stdcall Thread_ExportAllBaseDataImpl(void *pParam)
{
	__try
	{
		ExportBaseData((const BaseDataExportConfig *)pParam);
	}
	__except (glb_HandleException(GetExceptionInformation()))
	{
	}

	if (CDlgProgressNotify::m_nCurProgress < CDlgProgressNotify::m_nProgressMax)
	{
		AfxMessageBox("输出失败！", MB_ICONSTOP);
		CDlgProgressNotify::m_nCurProgress = CDlgProgressNotify::m_nProgressMax;
	}

	return 0;
}

void CMainFrame::OnExportData()
{
	CFileDialog dlg(
		FALSE,
		NULL,
		"elements.data",
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_NOCHANGEDIR);

	if (dlg.DoModal() == IDOK)
	{
		BaseDataExportConfig exportConfig;
		
		BaseDataIDManBase *IDMans[BDET_NUM] = {
			&g_BaseIDMan,
			&g_ExtBaseIDMan,
			&g_TalkIDMan,
			&g_FaceIDMan,
			&g_RecipeIDMan,
			&g_ConfigIDMan,
		};
		for (int i(0); i < BDET_NUM; ++ i){
			IDMans[i]->GeneratePathArray(exportConfig.pathArray[i]);
		}

		CDlgProgressNotify dlgProgress;

		CDlgProgressNotify::m_nProgressMax = exportConfig.ExportCount();
		CDlgProgressNotify::m_nCurProgress = 0;
		CDlgProgressNotify::m_strWndText = "正在导出，请稍等...";
		
		exportConfig.exportFullPath = dlg.GetPathName();
		HANDLE hThread = (HANDLE)_beginthreadex(NULL, 0, Thread_ExportAllBaseDataImpl, &exportConfig, 0, NULL);
		dlgProgress.DoModal();
		::WaitForSingleObject((void *)hThread, INFINITE);
	}
}

void CMainFrame::OnTestExportData()
{
	CFileDialog dlg(
		FALSE,
		NULL,
		"test.elements.data",
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_NOCHANGEDIR);
	
	if (dlg.DoModal() == IDOK)
	{
		BaseDataExportConfig exportConfig;

		abase::hash_map<AString, bool> orgPathAdded;
		BaseDataIDManBase *IDMans[BDET_NUM] = {
			&g_BaseIDMan,
			&g_ExtBaseIDMan,
			NULL,
			&g_FaceIDMan,
			&g_RecipeIDMan,
			&g_ConfigIDMan,
		};
		for (int i(0); i < BDET_NUM; ++ i){
			BaseDataIDManBase *pIDMan = IDMans[i];
			if (!pIDMan){
				continue;
			}
			abase::vector<AString> pathArray;
			pIDMan->GeneratePathArray(pathArray);
			for (int j(0); j < (int)pathArray.size(); ++ j)
			{
				const AString &strTemplatePath = pathArray[j];
				AString nameInOrgConfig;
				if (!CanExport(strTemplatePath, nameInOrgConfig)){
					continue;
				}
				AString strSubPath = strTemplatePath.Right(strTemplatePath.GetLength()-nameInOrgConfig.GetLength()-1);
				int nOrgLevel(0);
				AString orgPath = BaseDataTempl::ParseOrgPath(nameInOrgConfig, strSubPath, nOrgLevel);
				if (orgPathAdded.find(orgPath) != orgPathAdded.end()){
					continue;;
				}
				orgPathAdded[orgPath] = true;
				exportConfig.pathArray[i].push_back(pathArray[j]);
			}
		}
		
		CDlgProgressNotify dlgProgress;
		
		CDlgProgressNotify::m_nProgressMax = exportConfig.ExportCount();
		CDlgProgressNotify::m_nCurProgress = 0;
		CDlgProgressNotify::m_strWndText = "正在导出，请稍等...";
		
		exportConfig.exportFullPath = dlg.GetPathName();
		HANDLE hThread = (HANDLE)_beginthreadex(NULL, 0, Thread_ExportAllBaseDataImpl, &exportConfig, 0, NULL);
		dlgProgress.DoModal();
		::WaitForSingleObject((void *)hThread, INFINITE);
	}
}

LRESULT CMainFrame::WindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	// TODO: Add your specialized code here and/or call the base class
	if(message==WM_CLOSE)
	{
		((CElementDataApp*)AfxGetApp())->UpdateAllDocData();
	}
	return CMDIFrameWnd::WindowProc(message, wParam, lParam);
}

void CMainFrame::OnTestTalk() 
{
	// TODO: Add your command handler code here
	CTalkModifyDlg dlg;
	dlg.DoModal();
}

void CMainFrame::OnCheck()
{
	CPasswdDlg dlgPasswd;
	if (dlgPasswd.DoModal() != IDOK) return;

	if (dlgPasswd.m_strPasswd != "debug")
	{
		AfxMessageBox("密码错误！");
		return;
	}

	if (AfxMessageBox("此操作前请确认更新本地数据，继续吗？", MB_YESNO) != IDYES)
		return;

	abase::vector<AString> PathArray, WOArrayBase, WOArrayFace, WOArrayRecipe, WOArrayConfig, WOArrayExt;
	CStringArray WipeOffArray;
	CUIntArray IDArray;

	g_BaseIDMan.GeneratePathArray(PathArray);
	size_t i(0);
	for (i = 0; i < PathArray.size(); i++)
		if (GetFileAttributes(PathArray[i]) == -1)
		{
			WipeOffArray.Add(PathArray[i]);
			IDArray.Add(g_BaseIDMan.GetIDByPath(PathArray[i]));
			WOArrayBase.push_back(PathArray[i]);
		}

	PathArray.clear();
	g_FaceIDMan.GeneratePathArray(PathArray);
	for (i = 0; i < PathArray.size(); i++)
		if (GetFileAttributes(PathArray[i]) == -1)
		{
			WipeOffArray.Add(PathArray[i]);
			IDArray.Add(g_FaceIDMan.GetIDByPath(PathArray[i]));
			WOArrayFace.push_back(PathArray[i]);
		}

	PathArray.clear();
	g_RecipeIDMan.GeneratePathArray(PathArray);
	for (i = 0; i < PathArray.size(); i++)
		if (GetFileAttributes(PathArray[i]) == -1)
		{
			WipeOffArray.Add(PathArray[i]);
			IDArray.Add(g_RecipeIDMan.GetIDByPath(PathArray[i]));
			WOArrayRecipe.push_back(PathArray[i]);
		}
		
	PathArray.clear();
	g_ConfigIDMan.GeneratePathArray(PathArray);
	for (i = 0; i < PathArray.size(); i++)
		if (GetFileAttributes(PathArray[i]) == -1)
		{
			WipeOffArray.Add(PathArray[i]);
			IDArray.Add(g_ConfigIDMan.GetIDByPath(PathArray[i]));
			WOArrayConfig.push_back(PathArray[i]);
		}
		
	PathArray.clear();
	g_ExtBaseIDMan.GeneratePathArray(PathArray);
	for (i = 0; i < PathArray.size(); i++)
		if (GetFileAttributes(PathArray[i]) == -1)
		{
			WipeOffArray.Add(PathArray[i]);
			IDArray.Add(g_ExtBaseIDMan.GetIDByPath(PathArray[i]));
			WOArrayExt.push_back(PathArray[i]);
		}

	if (WipeOffArray.GetSize() == 0)
	{
		AfxMessageBox("无错误模板！");
		return;
	}

	CWipeOffDlg dlg(&WipeOffArray, &IDArray);
	if (dlg.DoModal() == IDOK)
	{
		g_BaseIDMan.WipeOffByPath(WOArrayBase);
		g_FaceIDMan.WipeOffByPath(WOArrayFace);
		g_RecipeIDMan.WipeOffByPath(WOArrayRecipe);
		g_ConfigIDMan.WipeOffByPath(WOArrayConfig);
		g_ExtBaseIDMan.WipeOffByPath(WOArrayExt);
	}
}

void CMainFrame::OnPolicyEditor() 
{
	// TODO: Add your command handler code here
	CPolicyDlg dlg;
	dlg.DoModal();
}

void CMainFrame::OnExportPolicyData() 
{
	// TODO: Add your command handler code here
	CFileDialog dlg(
		FALSE,
		NULL,
		NULL,
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_NOCHANGEDIR);

	if (dlg.DoModal() == IDOK
	&& !ExportPolicyData(dlg.GetPathName()))
		AfxMessageBox("输出失败！", MB_ICONSTOP);
}

void CMainFrame::OnTransVersion() 
{	
	/*
	// TODO: Add your command handler code here
	if(IDYES==AfxMessageBox("该操作前需要手动取出所有模板文件!你确定要进行添加MD5校验码的操作吗？",MB_YESNO|MB_ICONQUESTION))
	TempTransVersion();	
	*/
	//TempExtendFullProfession();
}

void CMainFrame::OnGetPolicyData() 
{
	// TODO: Add your command handler code here
	if(AfxMessageBox("你确定要从服务器上取得最新策略数据吗？",MB_YESNO|MB_ICONQUESTION)!=IDYES)
		return;
	// TODO: Add your command handler code here
	CreateDirectory("PolicyData", NULL);
	BeginWaitCursor();
	g_VSS.SetProjectPath("PolicyData");
	g_VSS.GetAllFiles("PolicyData");
	EndWaitCursor();
}

void CMainFrame::OnMounsterFallItemEditor() 
{
	// TODO: Add your command handler code here
	CMounsterFallListDlg dlg;
	dlg.DoModal();
}

void CMainFrame::OnItemsEditor() 
{
	return;
	// TODO: Add your command handler code here
	CTreasureDlg dlg;
	dlg.Init("treasure.dat");
	dlg.DoModal();
}

void CMainFrame::OnItems2Editor() 
{
	return;
	// TODO: Add your command handler code here
	CTreasureDlg dlg;
	dlg.Init("treasure1.dat");
	dlg.DoModal();
}


void CMainFrame::OnFindTemplate() 
{
	//	打开按ID查找tmpl模板窗口
	CDlgFindTemplate dlg;
	if (dlg.DoModal() != IDOK)	return;
	
	CString strText = dlg.m_strText;
	CDlgFindTemplate::FIND_TYPE type = dlg.m_typeLast;
	strText.TrimLeft();
	strText.TrimRight();
	if (strText.IsEmpty())	return;
	
	int id = atoi(strText);
	CString strCheck;
	strCheck.Format("%d", id);
	if (strCheck == strText)
	{
		//	按 ID 查找
		m_wndListBar.m_ListDlg.Open(id, type);
	}
	else
	{
		//	按名称查找，将所有查找结果保存在查找列表中，再通过单独的对话框选择部分打开
		
		CDlgFindTemplResult dlg2;
		if (!m_wndListBar.m_ListDlg.Find(strText, type, dlg2.m_result, dlg2.m_strColumn2Name) ||
			dlg2.m_result.empty() ||
			dlg2.DoModal() != IDOK)
		{
			OnFindTemplate();
			return;
		}
		
		for (CDlgFindTemplResult::Selection::iterator it = dlg2.m_selection.begin(); it != dlg2.m_selection.end(); ++ it)
			m_wndListBar.m_ListDlg.Open(*it, type);
	}
}

void CMainFrame::UpdateSearchFile()
{
	m_wndListBar.m_ListDlg.ReInitSearchImpl(true);
	m_wndListBar.m_ListDlg.SaveSearchToFile();
}