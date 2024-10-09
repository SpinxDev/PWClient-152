// TaskIDSelDlg.cpp : implementation file
//

#include "stdafx.h"
#ifdef TASK_TEMPL_EDITOR
	#include "../../CTaskTemplEditor/TaskTemplEditor.h"
	#include "../../CTaskTemplEditor/TaskTemplEditorView.h"
	#include "TaskTemplMan.h"
#else
	#include "ElementData.h"
#endif
#include "AFile.h"
#include "TaskIDSelDlg.h"
#include "BaseDataIDMan.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CString GetTaskNameByID(unsigned long ulID)
{
	return g_TaskIDMan.GetPathByID(ulID);
}

BOOL CALLBACK TASK_ID_FUNCS::OnActivate(void)
{
	CTaskIDSelDlg dlg;
	dlg.m_ulID = m_var;
	if (dlg.DoModal() != IDOK) return FALSE;
	m_strText = dlg.m_strName;
	m_var = dlg.m_ulID;
	return TRUE;
}

LPCTSTR CALLBACK TASK_ID_FUNCS::OnGetShowString(void) const
{
	return m_strText;
}

AVariant CALLBACK TASK_ID_FUNCS::OnGetValue(void) const
{
	return m_var;
}

void CALLBACK TASK_ID_FUNCS::OnSetValue(const AVariant& var)
{
	m_var = var;
	m_strText = GetTaskNameByID(m_var);
}

/////////////////////////////////////////////////////////////////////////////
// CTaskIDSelDlg dialog


CTaskIDSelDlg::CTaskIDSelDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTaskIDSelDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CTaskIDSelDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_bShowSub = true;
	m_bOneOnly = false;
	m_ulTopTaskToSel = 0;
}


void CTaskIDSelDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTaskIDSelDlg)
	DDX_Control(pDX, IDC_TREE_TASK, m_IDTree);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTaskIDSelDlg, CDialog)
	//{{AFX_MSG_MAP(CTaskIDSelDlg)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_TASK, OnSelchangedTreeTask)
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTaskIDSelDlg message handlers

BOOL CTaskIDSelDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	m_ImageList.Create(IDB_LISTTAG, 16, 3, RGB(255, 0, 255));
	m_IDTree.SetImageList(&m_ImageList, TVSIL_NORMAL);

	HTREEITEM hNone = m_IDTree.InsertItem("£¨ÎÞ£©", TVI_ROOT);
	m_IDTree.SetItemImage(hNone, 4, 4);

	m_strInitFile = g_TaskIDMan.GetPathByID(m_ulID);

	UpdateTaskTree();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CTaskIDSelDlg::OnOK()
{
	m_ulID = 0;
	HTREEITEM hItem = m_IDTree.GetSelectedItem();

#ifdef TASK_TEMPL_EDITOR
	
	CMainFrame* pMain = (CMainFrame*)AfxGetApp()->m_pMainWnd;
	CTaskTemplEditorView* pView = (CTaskTemplEditorView*)pMain->GetActiveView();
	if (hItem)
	{
		AString strPath = m_IDTree.GetItemText(hItem);
		m_ulID = g_TaskIDMan.GetIDByPath(strPath);
		m_strName = m_IDTree.GetItemText(hItem);
		
		TreeItemMap::iterator iter = GetTaskTemplMan()->GetTopTreeItemMap().find(m_ulID);
		if (iter != GetTaskTemplMan()->GetTopTreeItemMap().end())
		{
			HTREEITEM hViewTreeItem = (iter->second).m_hItem;
			pView->LoadTaskTempl(strPath,hViewTreeItem);
		}
		
	}
#else //TASK_TEMPL_EDITOR

	if (hItem)
	{
		ATaskTempl* p = (ATaskTempl*)m_IDTree.GetItemData(hItem);
		if (p)
		{
			m_strName = m_IDTree.GetItemText(hItem);
			m_ulID = p->m_ID;
		}
		else
		{
			m_strName.Empty();
			m_ulID = 0;
		}
	}

#endif //TASK_TEMPL_EDITOR

	CDialog::OnOK();
}

void CTaskIDSelDlg::OnSelchangedTreeTask(NMHDR* pNMHDR, LRESULT* pResult)
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	GetDlgItem(IDOK)->EnableWindow();
	*pResult = 0;
}
#ifdef TASK_TEMPL_EDITOR

void CopySubTree(CTreeCtrl& TDest,CTreeCtrl& TSrc,HTREEITEM hDest,HTREEITEM hSrc)
{
	if (TSrc.ItemHasChildren(hSrc))
	{
		HTREEITEM hParent = TSrc.GetChildItem(hSrc);
		
		while(hParent != NULL)
		{
			HTREEITEM hDestItem = TDest.InsertItem(TSrc.GetItemText(hParent),hDest,TVI_LAST);
			CopySubTree(TDest,TSrc,hDestItem,hParent);
			hParent = TSrc.GetNextSiblingItem(hParent);
		}
	}
}
void CopyTree(CTreeCtrl& TDest,CTreeCtrl& TSrc)
{
	HTREEITEM hSubTree = TSrc.GetChildItem(TVI_ROOT);
	
	while (hSubTree != NULL)
	{
		HTREEITEM hDestSub = TDest.InsertItem(TSrc.GetItemText(hSubTree),TVI_ROOT,TVI_LAST);
		CopySubTree(TDest,TSrc,hDestSub,hSubTree);
		hSubTree = TSrc.GetNextSiblingItem(hSubTree);
	}
}

#endif
void CTaskIDSelDlg::UpdateTaskTree()
{
#ifdef TASK_TEMPL_EDITOR
	CMainFrame* pMain = (CMainFrame*)AfxGetApp()->m_pMainWnd;
	CTaskTemplEditorView* pView = (CTaskTemplEditorView*)pMain->GetActiveView();
	
	CTreeCtrl& TreeSrc = pView->m_TaskTree;
	
	HTREEITEM hItem = TreeSrc.GetChildItem(TVI_ROOT);
	
	if (m_bShowSub)
	{
		CopyTree(m_IDTree,TreeSrc);
	}
	else
	{
		HTREEITEM hItem = TreeSrc.GetChildItem(TVI_ROOT);
		
		while (hItem != NULL)
		{
			m_IDTree.InsertItem(TreeSrc.GetItemText(hItem),TVI_ROOT,TVI_LAST);
			hItem = TreeSrc.GetNextSiblingItem(hItem);
		}
	}
	
#else //TASK_TEMPL_EDITOR
	ATaskTempl* pTask;

	if (m_bOneOnly)
	{
		CString strPath = g_TaskIDMan.GetPathByID(m_ulTopTaskToSel);
		strPath = "BaseData\\TaskTemplate\\" + strPath + ".tkt";
		pTask = new ATaskTempl;

		if (!pTask->LoadFromTextFile(strPath))
		{
			delete pTask;
			return;
		}

		HTREEITEM hItem = UpdateTaskTree(TVI_ROOT, pTask);
		m_IDTree.SetItemImage(hItem, 4, 4);
	}
	else
	{
		abase::vector<AString> PathArray;
		g_TaskIDMan.GeneratePathArray(PathArray);

		for (size_t i = 0; i < PathArray.size(); i++)
		{
			CString strFile = "BaseData\\TaskTemplate\\" + PathArray[i] + ".tkt";
			pTask = new ATaskTempl;

			if (!pTask->LoadFromTextFile(strFile))
			{
				delete pTask;
				continue;
			}

			HTREEITEM hItem = UpdateTaskTree(TVI_ROOT, pTask);
			m_IDTree.SetItemImage(hItem, 4, 4);
		}
	}
#endif //TASK_TEMPL_EDITOR
}

HTREEITEM CTaskIDSelDlg::UpdateTaskTree(HTREEITEM hParent, ATaskTempl* pTask)
{
	CString strName = GetTaskNameByID(pTask->m_ID);
	HTREEITEM hItem = m_IDTree.InsertItem(strName, hParent, hParent == TVI_ROOT ? TVI_SORT : TVI_LAST);
	m_IDTree.SetItemData(hItem, (DWORD)pTask);
	m_IDTree.SetItemImage(hItem, 4, 4);

	if (m_strInitFile.CompareNoCase(strName) == 0)
		m_IDTree.Select(hItem, TVGN_CARET);

	if (m_bShowSub)
	{
		ATaskTempl* pSub = pTask->m_pFirstChild;

		while (pSub)
		{
			UpdateTaskTree(hItem, pSub);
			pSub = pSub->m_pNextSibling;
		}
	}

	return hItem;
}

void CTaskIDSelDlg::OnDestroy() 
{
	HTREEITEM hItem = m_IDTree.GetChildItem(TVI_ROOT);
	while (hItem)
	{
		delete (ATaskTempl*)m_IDTree.GetItemData(hItem);
		hItem = m_IDTree.GetNextSiblingItem(hItem);
	}

	CDialog::OnDestroy();
}
