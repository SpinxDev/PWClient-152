// TaskLinkDlg.cpp : implementation file
//

#include "stdafx.h"
#include "TaskTemplEditor.h"
#include "TaskLinkDlg.h"
#include "TaskTemplMan.h"
#include "AVariant.h"
#include "TaskIDSelDlg.h"
#include "VSSOperation.h"
#include "shlwapi.h"

#ifdef _DEBUG
#define new A_DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

struct TASK_LINK_NODE
{
	ATaskTempl*		pTask;
	TASK_LINK_NODE*	pPrev;
	TASK_LINK_NODE*	pNext;

	~TASK_LINK_NODE() { if (pNext) delete pNext; }
};

typedef abase::vector<TASK_LINK_NODE*> TaskLinkArray;
TaskLinkArray links;

TASK_LINK_NODE** get_head_link(ATaskTempl* pToFind)
{
	for (size_t i = 0; i < links.size(); i++)
	{
		if (links[i]->pTask == pToFind)
			return &links[i];
	}

	return NULL;
}

TASK_LINK_NODE* get_tail_link(ATaskTempl* pToFind)
{
	for (size_t i = 0; i < links.size(); i++)
	{
		TASK_LINK_NODE* pNode = links[i];
		while (pNode->pNext) pNode = pNode->pNext;

		if (pNode->pTask == pToFind)
			return pNode;
	}

	return NULL;
}

static void clear_task_links()
{
	for (size_t i = 0; i < links.size(); i++)
		delete links[i];

	links.clear();
}

static bool compare_task(TASK_LINK_NODE* p1, TASK_LINK_NODE* p2)
{
	return strcmp(CSafeString(p1->pTask->GetName()), CSafeString(p2->pTask->GetName())) <= 0;
}

static void sort_link()
{
	for (int i = 0; i < (int)links.size() - 1; i++)
	{
		for (int j = links.size() - 2; j >= i; j--)
		{
			if (!compare_task(links[j], links[j+1]))
			{
				TASK_LINK_NODE* p = links[j+1];
				links[j+1] = links[j];
				links[j] = p;
			}
		}
	}
}

static void build_task_links()
{
	clear_task_links();
	links.reserve(2048);

	TaskTemplMap& TopMap = GetTaskTemplMan()->GetTopTemplMap();

	for (TaskTemplMap::iterator it = TopMap.begin(); it != TopMap.end(); ++it)
	{
		ATaskTempl* pTempl = it->second;

		if (pTempl->m_ulPremise_Task_Count == 0)
			continue;

		for (unsigned long i = 0; i < pTempl->m_ulPremise_Task_Count; i++)
		{
			TASK_LINK_NODE** ppNode = get_head_link(pTempl);

			if (ppNode)
			{
				TaskTemplMap::iterator iter = TopMap.find(pTempl->m_ulPremise_Tasks[i]);
				ATaskTempl* pPrem = 0;	
				if (iter != TopMap.end())
				{
					pPrem = iter->second;
				}
				
				else
					continue;

				// 前提条件是否是尾部
				TASK_LINK_NODE* pPremNode = get_tail_link(pPrem);

				if (pPremNode)
				{
					pPremNode->pNext = *ppNode;
					(*ppNode)->pPrev = pPremNode;
					links.erase(ppNode);
				}
				else
				{
					TASK_LINK_NODE* pHead = new TASK_LINK_NODE;
					pHead->pTask = pPrem;
					pHead->pPrev = NULL;
					pHead->pNext = *ppNode;
					(*ppNode)->pPrev = pHead;
					*ppNode = pHead;
				}
			}
			else
			{
				TaskTemplMap::iterator it = TopMap.find(pTempl->m_ulPremise_Tasks[i]);
				if (it == TopMap.end())
					continue;

				ATaskTempl* pPrev = it->second;
				TASK_LINK_NODE* pTailNode = get_tail_link(pPrev);

				if (pTailNode)
				{
					TASK_LINK_NODE* pTail = new TASK_LINK_NODE;
					pTail->pTask = pTempl;
					pTail->pNext = NULL;
					pTail->pPrev = pTailNode;
					pTailNode->pNext = pTail;
				}
				else
				{
					TASK_LINK_NODE* pNode = new TASK_LINK_NODE;
					TASK_LINK_NODE* pHead = new TASK_LINK_NODE;
					pHead->pTask = pPrev;
					pHead->pPrev = NULL;
					pHead->pNext = pNode;
					pNode->pTask = pTempl;
					pNode->pPrev = pHead;
					pNode->pNext = NULL;
					links.push_back(pHead);
				}
			}
		}
	}

	sort_link();
}

/////////////////////////////////////////////////////////////////////////////
// CTaskLinkDlg dialog

extern BOOL IsReadOnly(LPCTSTR szFile);

CTaskLinkDlg::CTaskLinkDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTaskLinkDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CTaskLinkDlg)
	//}}AFX_DATA_INIT
	m_dwPrev = 0;
	m_dwNext = 0;
}

void CTaskLinkDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTaskLinkDlg)
	DDX_Control(pDX, IDC_TASK_LINK, m_TaskLink);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTaskLinkDlg, CDialog)
	//{{AFX_MSG_MAP(CTaskLinkDlg)
	ON_WM_DESTROY()
	ON_NOTIFY(NM_RCLICK, IDC_TASK_LINK, OnRclickTaskLink)
	ON_BN_CLICKED(IDC_NEW_LINK, OnNewLink)
	ON_BN_CLICKED(IDC_PREV, OnPrev)
	ON_BN_CLICKED(IDC_NEXT, OnNext)
	//}}AFX_MSG_MAP
	ON_COMMAND(ID_INSERT_LINK, OnInsert)
	ON_COMMAND(ID_ADD_LINK, OnAdd)
	ON_COMMAND(ID_DEL_LINK, OnDel)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTaskLinkDlg message handlers

void CTaskLinkDlg::InitTree()
{
	build_task_links();

	m_TaskLink.DeleteAllItems();

	for (size_t i = 0; i < links.size(); i++)
	{
		TASK_LINK_NODE* pNode = links[i];

		HTREEITEM hNode = m_TaskLink.InsertItem(CSafeString(pNode->pTask->GetName()));

		while (pNode)
		{
			HTREEITEM hItem = m_TaskLink.InsertItem(CSafeString(pNode->pTask->GetName()), hNode);
			m_TaskLink.SetItemData(hItem, reinterpret_cast<DWORD>(pNode));
			pNode = pNode->pNext;
		}
	}
}

BOOL CTaskLinkDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	InitTree();
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CTaskLinkDlg::OnDestroy() 
{
	CDialog::OnDestroy();
	clear_task_links();	
}

void CTaskLinkDlg::OnRclickTaskLink(NMHDR* pNMHDR, LRESULT* pResult)
{
	*pResult = 0;

	CPoint pt;
	UINT uFlag;

	GetCursorPos(&pt);
	m_TaskLink.ScreenToClient(&pt);
	HTREEITEM hItem = m_TaskLink.HitTest(pt, &uFlag);
	m_TaskLink.ClientToScreen(&pt);

	if (hItem && (TVHT_ONITEM & uFlag))
	{
		m_TaskLink.Select(hItem, TVGN_CARET);

		if (!m_TaskLink.GetParentItem(hItem))
			return;

		CMenu menu;
		menu.LoadMenu(IDR_TASK_LINK);
		CMenu* pSub = menu.GetSubMenu(0);

		pSub->TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON,
			pt.x,
			pt.y,
			this);
	}
}

void CTaskLinkDlg::OnInsert()
{
	CTaskIDSelDlg dlg;
	dlg.m_bShowSub = false;
	dlg.m_ulID = 0;

	if (dlg.DoModal() != IDOK || !dlg.m_ulID) return;

	ATaskTempl* pInsert = GetTaskTemplMan()->GetTopTaskByID(dlg.m_ulID);
	if (!pInsert) return;

	HTREEITEM hSel = m_TaskLink.GetSelectedItem();
	TASK_LINK_NODE* pNode = reinterpret_cast<TASK_LINK_NODE*>(m_TaskLink.GetItemData(hSel));
	ATaskTempl* pNext = pNode->pTask;

	if (pNext->GetID() == pInsert->GetID())
		return;

	unsigned long i(0);
	for (i = 0; i < pNext->m_ulPremise_Task_Count; i++)
	{
		if (pNext->m_ulPremise_Tasks[i] == pInsert->GetID())
		{
			AfxMessageBox("已有此前提任务");
			return;
		}
	}

	if (pNode->pPrev)
	{
		ATaskTempl* p = pNode->pPrev->pTask;

		if (p->GetID() == pInsert->GetID())
			return;

		for (i = 0; i < pInsert->m_ulPremise_Task_Count; i++)
		{
			if (pInsert->m_ulPremise_Tasks[i] == p->GetID())
			{
				AfxMessageBox("已有此前提任务");
				return;
			}
		}
	}

	bool bInsertCheckOut;

	CString strInsertPath = pInsert->GetFilePath();
	int nFind = strInsertPath.ReverseFind('\\');
	strInsertPath = strInsertPath.Left(nFind);

	if (IsReadOnly(pInsert->GetFilePath()))
	{
		g_VSS.SetProjectPath(strInsertPath);
		g_VSS.CheckOutFile(pInsert->GetFilePath());
		bInsertCheckOut = false;
	}
	else
		bInsertCheckOut = true;

	if (IsReadOnly(pInsert->GetFilePath()))
	{
		CString strErr;
		strErr.Format("CheckOut%s失败！", CSafeString(pInsert->GetName()));
		AfxMessageBox(strErr);
		return;
	}

	bool bNextCheckOut;

	CString strNextPath = pNext->GetFilePath();
	nFind = strNextPath.ReverseFind('\\');
	strNextPath = strNextPath.Left(nFind);

	if (IsReadOnly(pNext->GetFilePath()))
	{
		g_VSS.SetProjectPath(strNextPath);
		g_VSS.CheckOutFile(pNext->GetFilePath());
		bNextCheckOut = false;
	}
	else
		bNextCheckOut = true;

	if (IsReadOnly(pNext->GetFilePath()))
	{
		CString strErr;
		strErr.Format("CheckOut%s失败！", CSafeString(pNext->GetName()));
		AfxMessageBox(strErr);

		if (!bInsertCheckOut)
		{
			g_VSS.SetProjectPath(strInsertPath);
			g_VSS.CheckInFile(PathFindFileName(pInsert->GetFilePath()));
		}

		return;
	}

	if (pNode->pPrev)
	{
		if (pInsert->m_ulPremise_Task_Count < MAX_PREM_TASK_COUNT)
		{
			pInsert->m_ulPremise_Tasks[pInsert->m_ulPremise_Task_Count++] = pNode->pPrev->pTask->GetID();

			for (i = 0; i < pNext->m_ulPremise_Task_Count; i++)
			{
				if (pNext->m_ulPremise_Tasks[i] == pNode->pPrev->pTask->GetID())
				{
					pNext->m_ulPremise_Tasks[i] = pInsert->GetID();
					break;
				}
			}
		}
	}
	else if (pNext->m_ulPremise_Task_Count < MAX_PREM_TASK_COUNT)
		pNext->m_ulPremise_Tasks[pNext->m_ulPremise_Task_Count++] = pInsert->GetID();

	pInsert->SaveToTextFile(pInsert->GetFilePath());
	pNext->SaveToTextFile(pNext->GetFilePath());

	if (!bInsertCheckOut)
	{
		g_VSS.SetProjectPath(strInsertPath);
		g_VSS.CheckInFile(PathFindFileName(pInsert->GetFilePath()));
	}

	if (!bNextCheckOut)
	{
		g_VSS.SetProjectPath(strNextPath);
		g_VSS.CheckInFile(PathFindFileName(pNext->GetFilePath()));
	}

	InitTree();
}

void CTaskLinkDlg::OnAdd()
{
	CTaskIDSelDlg dlg;
	dlg.m_bShowSub = false;
	dlg.m_ulID = 0;

	if (dlg.DoModal() != IDOK || !dlg.m_ulID) return;

	ATaskTempl* pInsert = GetTaskTemplMan()->GetTopTaskByID(dlg.m_ulID);
	if (!pInsert || pInsert->m_ulPremise_Task_Count >= MAX_PREM_TASK_COUNT) return;

	HTREEITEM hSel = m_TaskLink.GetSelectedItem();
	TASK_LINK_NODE* pNode = reinterpret_cast<TASK_LINK_NODE*>(m_TaskLink.GetItemData(hSel));
	while (pNode->pNext) pNode = pNode->pNext;
	ATaskTempl* pPrev = pNode->pTask;

	if (pPrev->GetID() == pInsert->GetID())
		return;

	for (unsigned long i = 0; i < pInsert->m_ulPremise_Task_Count; i++)
	{
		if (pInsert->m_ulPremise_Tasks[i] == pPrev->GetID())
		{
			AfxMessageBox("已有此前提任务");
			return;
		}
	}

	bool bInsertCheckOut;

	CString strInsertPath = pInsert->GetFilePath();
	int nFind = strInsertPath.ReverseFind('\\');
	strInsertPath = strInsertPath.Left(nFind);

	if (IsReadOnly(pInsert->GetFilePath()))
	{
		g_VSS.SetProjectPath(strInsertPath);
		g_VSS.CheckOutFile(pInsert->GetFilePath());
		bInsertCheckOut = false;
	}
	else
		bInsertCheckOut = true;

	if (IsReadOnly(pInsert->GetFilePath()))
	{
		CString strErr;
		strErr.Format("CheckOut%s失败！", CSafeString(pInsert->GetName()));
		AfxMessageBox(strErr);
		return;
	}

	pInsert->m_ulPremise_Tasks[pInsert->m_ulPremise_Task_Count++] = pPrev->GetID();

	pInsert->SaveToTextFile(pInsert->GetFilePath());

	if (!bInsertCheckOut)
	{
		g_VSS.SetProjectPath(strInsertPath);
		g_VSS.CheckInFile(PathFindFileName(pInsert->GetFilePath()));
	}

	InitTree();
}

void CTaskLinkDlg::OnDel()
{
	HTREEITEM hSel = m_TaskLink.GetSelectedItem();
	TASK_LINK_NODE* pNode = reinterpret_cast<TASK_LINK_NODE*>(m_TaskLink.GetItemData(hSel));
	ATaskTempl* pTask = pNode->pTask;

	ATaskTempl* pNext = NULL;
	bool bNextCheckOut = true;
	CString strNextPath;

	if (pNode->pNext)
	{
		pNext = pNode->pNext->pTask;
		strNextPath = pNext->GetFilePath();
		int nFind = strNextPath.ReverseFind('\\');
		strNextPath = strNextPath.Left(nFind);

		if (IsReadOnly(pNext->GetFilePath()))
		{
			g_VSS.SetProjectPath(strNextPath);
			g_VSS.CheckOutFile(pNext->GetFilePath());
			bNextCheckOut = false;
		}

		if (IsReadOnly(pNext->GetFilePath()))
		{
			CString strErr;
			strErr.Format("CheckOut%s失败！", CSafeString(pNext->GetName()));
			AfxMessageBox(strErr);
			return;
		}
	}

	bool bCurCheckOut;
	CString strPath = pTask->GetFilePath();
	int nFind = strPath.ReverseFind('\\');
	strPath = strPath.Left(nFind);

	if (IsReadOnly(pTask->GetFilePath()))
	{
		g_VSS.SetProjectPath(strPath);
		g_VSS.CheckOutFile(pTask->GetFilePath());
		bCurCheckOut = false;
	}
	else
		bCurCheckOut = true;

	if (IsReadOnly(pTask->GetFilePath()))
	{
		CString strErr;
		strErr.Format("CheckOut%s失败！", CSafeString(pTask->GetName()));
		AfxMessageBox(strErr);

		if (!bNextCheckOut)
		{
			g_VSS.SetProjectPath(strNextPath);
			g_VSS.CheckInFile(PathFindFileName(pNext->GetFilePath()));
		}

		return;
	}

	unsigned long ulPrev = 0;

	if (pNode->pPrev)
	{
		ATaskTempl* pPrev = pNode->pPrev->pTask;
		ulPrev = pPrev->GetID();

		for (unsigned long i = 0; i < pTask->m_ulPremise_Task_Count; i++)
		{
			if (pTask->m_ulPremise_Tasks[i] == pPrev->GetID())
			{
				for (unsigned long j = i; j < pTask->m_ulPremise_Task_Count-1; j++)
					pTask->m_ulPremise_Tasks[j] = pTask->m_ulPremise_Tasks[j+1];
	
				pTask->m_ulPremise_Task_Count--;
				pTask->m_ulPremise_Tasks[pTask->m_ulPremise_Task_Count] = 0;
				break;
			}
		}
	}

	if (pNext)
	{
		for (unsigned long i = 0; i < pNext->m_ulPremise_Task_Count; i++)
		{
			if (pNext->m_ulPremise_Tasks[i] == pTask->GetID())
			{
				pNext->m_ulPremise_Tasks[i] = ulPrev;

				if (ulPrev == 0)
				{
					for (unsigned long j = i; j < pNext->m_ulPremise_Task_Count-1; j++)
						pNext->m_ulPremise_Tasks[j] = pNext->m_ulPremise_Tasks[j+1];

					pNext->m_ulPremise_Task_Count--;
					pNext->m_ulPremise_Tasks[pNext->m_ulPremise_Task_Count] = 0;
				}

				break;
			}
		}

		pNext->SaveToTextFile(pNext->GetFilePath());

		if (!bNextCheckOut)
		{
			g_VSS.SetProjectPath(strNextPath);
			g_VSS.CheckInFile(PathFindFileName(pNext->GetFilePath()));
		}
	}

	pTask->SaveToTextFile(pTask->GetFilePath());

	if (!bCurCheckOut)
	{
		g_VSS.SetProjectPath(strPath);
		g_VSS.CheckInFile(PathFindFileName(pTask->GetFilePath()));
	}

	InitTree();
}

void CTaskLinkDlg::OnNewLink()
{
	if (m_dwPrev == m_dwNext || !m_dwPrev || !m_dwNext)
		return;

	ATaskTempl* pTask = GetTaskTemplMan()->GetTopTaskByID(m_dwNext);

	if (!pTask)
	{
		AfxMessageBox("无此后续任务！");
		return;
	}

	bool bCurCheckOut;
	CString strPath = pTask->GetFilePath();
	int nFind = strPath.ReverseFind('\\');
	strPath = strPath.Left(nFind);

	if (IsReadOnly(pTask->GetFilePath()))
	{
		g_VSS.SetProjectPath(strPath);
		g_VSS.CheckOutFile(pTask->GetFilePath());
		bCurCheckOut = false;
	}
	else
		bCurCheckOut = true;

	if (IsReadOnly(pTask->GetFilePath()))
	{
		CString strErr;
		strErr.Format("CheckOut%s失败！", CSafeString(pTask->GetName()));
		AfxMessageBox(strErr);
		return;
	}

	if (pTask->m_ulPremise_Task_Count >= MAX_PREM_TASK_COUNT)
	{
		AfxMessageBox("前提任务已满！");
		return;
	}

	for (unsigned long i = 0; i < pTask->m_ulPremise_Task_Count; i++)
	{
		if (pTask->m_ulPremise_Tasks[i] == m_dwPrev)
		{
			AfxMessageBox("已由此前提任务！");
			return;
		}
	}

	pTask->m_ulPremise_Tasks[pTask->m_ulPremise_Task_Count++] = m_dwPrev;
	pTask->SaveToTextFile(pTask->GetFilePath());

	if (!bCurCheckOut)
	{
		g_VSS.SetProjectPath(strPath);
		g_VSS.CheckInFile(PathFindFileName(pTask->GetFilePath()));
	}

	InitTree();
}

void CTaskLinkDlg::OnPrev()
{
	CTaskIDSelDlg dlg;
	dlg.m_bShowSub = false;
	dlg.m_ulID = m_dwPrev;

	if (dlg.DoModal() != IDOK) return;
	m_dwPrev = dlg.m_ulID;
	GetDlgItem(IDC_PREV_NAME)->SetWindowText(GetTaskNameByID(m_dwPrev));
}

void CTaskLinkDlg::OnNext()
{
	CTaskIDSelDlg dlg;
	dlg.m_bShowSub = false;
	dlg.m_ulID = m_dwNext;

	if (dlg.DoModal() != IDOK) return;
	m_dwNext = dlg.m_ulID;
	GetDlgItem(IDC_NEXT_NAME)->SetWindowText(GetTaskNameByID(m_dwNext));
}
