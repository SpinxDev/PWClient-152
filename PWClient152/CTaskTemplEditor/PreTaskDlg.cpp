// PreTaskDlg.cpp : implementation file
//

#include "stdafx.h"
#include "TaskTemplEditor.h"
#include "PreTaskDlg.h"
#include "TaskTempl.h"
#include "TaskIDSelDlg.h"
#include "NumDlg.h"
#include "BaseDataIDMan.h"

#ifdef _DEBUG
#define new A_DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPreTaskDlg dialog


CPreTaskDlg::CPreTaskDlg(ATaskTempl* pTempl, bool bPre, CWnd* pParent /*=NULL*/)
	: CDialog(CPreTaskDlg::IDD, pParent), m_pTempl(pTempl), m_bPre(bPre) ,m_ulAtLeastTaskNumFinished(0)
{
	//{{AFX_DATA_INIT(CPreTaskDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CPreTaskDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPreTaskDlg)
	DDX_Control(pDX, IDC_PRETASK, m_PreTask);
	DDX_Text(pDX, IDC_LEAST_PREM_TASK_NUM, m_ulAtLeastTaskNumFinished);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPreTaskDlg, CDialog)
	//{{AFX_MSG_MAP(CPreTaskDlg)
	ON_BN_CLICKED(IDC_ADD, OnAdd)
	ON_BN_CLICKED(IDC_DEL, OnDel)
	ON_BN_CLICKED(IDC_EDIT, OnEdit)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPreTaskDlg message handlers

BOOL CPreTaskDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	m_PreTask.SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);
	m_PreTask.InsertColumn(0, _T("任务名称"), LVCFMT_LEFT, 180);

	UpdateList();

	if (m_bPre)
		SetWindowText("前提任务");
	else
	{
		GetDlgItem(IDC_LEAST_PREM_TASK_NUM)->EnableWindow(FALSE);
		SetWindowText("互斥任务");
	}

	m_ulAtLeastTaskNumFinished = m_pTempl->m_ulPremise_Task_Least_Num;
	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CPreTaskDlg::UpdateList()
{
	m_PreTask.DeleteAllItems();
	unsigned long i;

	if (m_bPre)
	{
		for (i = 0; i < m_pTempl->m_ulPremise_Task_Count; i++)
		{
			m_PreTask.InsertItem(
				i,
				GetTaskNameByID(m_pTempl->m_ulPremise_Tasks[i]));
		}
	}
	else
	{
		for (i = 0; i < m_pTempl->m_ulMutexTaskCount; i++)
		{
			m_PreTask.InsertItem(
				i,
				GetTaskNameByID(m_pTempl->m_ulMutexTasks[i]));
		}
	}
}

void CPreTaskDlg::OnAdd()
{
	unsigned long ulCount, ulMaxCount;

	if (m_bPre)
	{
		ulCount = m_pTempl->m_ulPremise_Task_Count;
		ulMaxCount = MAX_PREM_TASK_COUNT;
	}
	else
	{
		ulCount = m_pTempl->m_ulMutexTaskCount;
		ulMaxCount = MAX_MUTEX_TASK_COUNT;
	}

	if (ulCount >= ulMaxCount)
	{
		AfxMessageBox("已到达上限", MB_ICONSTOP);
		return;
	}

	unsigned long ulTask = 0;

	CNumDlg dlgNum;
	dlgNum.m_nNum = 0;

	if(dlgNum.DoModal() == IDOK)
	{
		if(dlgNum.m_nNum <= 0)
		{
			ulTask = 0;
			return;
		}
		ATaskTempl* pTask = new ATaskTempl;
		
		CString strPath = g_TaskIDMan.GetPathByID(dlgNum.m_nNum);
		strPath = "BaseData\\TaskTemplate\\" + strPath + ".tkt";

		if(!pTask->LoadFromTextFile(strPath))
		{
			AfxMessageBox("找不到此任务，请输入正确的任务ID。");
			delete pTask;
			return;
		}
		
		if (!pTask->m_bNeedRecord)
		{
			AfxMessageBox("此任务不需要记录完成结果，你需要让它记录完成结果。");
			delete pTask;
			return;
		}
		ulTask = dlgNum.m_nNum;
		delete pTask;		
	}

	/*
	CTaskIDSelDlg dlg;
	dlg.m_bShowSub = false;
	dlg.m_ulID = 0;

	if (dlg.DoModal() != IDOK) return;
	ulTask = dlg.m_ulID;
	*/

	if (m_bPre)
	{
		m_pTempl->m_ulPremise_Tasks[ulCount] = ulTask;
		m_pTempl->m_ulPremise_Task_Count = ulCount + 1;
	}
	else
	{
		m_pTempl->m_ulMutexTasks[ulCount] = ulTask;
		m_pTempl->m_ulMutexTaskCount = ulCount + 1;
	}

	UpdateList();
}

void CPreTaskDlg::OnDel()
{
	POSITION pos = m_PreTask.GetFirstSelectedItemPosition();
	if (!pos || AfxMessageBox("确定吗？", MB_YESNO) != IDYES) return;

	unsigned long ulItem = m_PreTask.GetNextSelectedItem(pos);
	unsigned long i;

	if (m_bPre)
	{
		for (i = ulItem+1; i < m_pTempl->m_ulPremise_Task_Count; i++)
			m_pTempl->m_ulPremise_Tasks[i-1] = m_pTempl->m_ulPremise_Tasks[i];

		m_pTempl->m_ulPremise_Task_Count--;
	}
	else
	{
		for (i = ulItem+1; i < m_pTempl->m_ulMutexTaskCount; i++)
			m_pTempl->m_ulMutexTasks[i-1] = m_pTempl->m_ulMutexTasks[i];

		m_pTempl->m_ulMutexTaskCount--;
	}

	UpdateList();
}

void CPreTaskDlg::OnEdit()
{
	POSITION pos = m_PreTask.GetFirstSelectedItemPosition();
	if (!pos) return;
	unsigned long ulItem = m_PreTask.GetNextSelectedItem(pos);

	unsigned long ulTask;

	CTaskIDSelDlg dlg;
	dlg.m_bShowSub = false;

	if (m_bPre)
		dlg.m_ulID = m_pTempl->m_ulPremise_Tasks[ulItem];
	else
		dlg.m_ulID = m_pTempl->m_ulMutexTasks[ulItem];

	if (dlg.DoModal() != IDOK) return;
	ulTask = dlg.m_ulID;

	if (m_bPre)
		m_pTempl->m_ulPremise_Tasks[ulItem] = ulTask;
	else
		m_pTempl->m_ulMutexTasks[ulItem] = ulTask;

	UpdateList();
}

void CPreTaskDlg::OnOK() 
{
	// TODO: Add extra validation here
	UpdateData();

	if (m_ulAtLeastTaskNumFinished > m_pTempl->m_ulPremise_Task_Count)
	{
		AfxMessageBox(CString("数字不对，不能多于前提任务总数！"));
		return;
	}
	else
	{
		m_pTempl->m_ulPremise_Task_Least_Num = m_ulAtLeastTaskNumFinished;
	}
	
	CDialog::OnOK();
}
