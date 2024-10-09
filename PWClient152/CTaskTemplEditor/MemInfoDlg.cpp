// MemInfoDlg.cpp : implementation file
//

#include "stdafx.h"
#include "TaskTemplEditor.h"
#include "TaskTempl.h"
#include "MemInfoDlg.h"
#include "TaskIDSelDlg.h"
#include "NumDlg.h"
#include "BaseDataIDMan.h"

#ifdef _DEBUG
#define new A_DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMemInfoDlg dialog

static const int FORCE_ID[] = {
	0,
	1004,
	1005,
	1006,
	-1,
};

int find_force_index(int iForce)
{
	for (int i = 0; i < sizeof(FORCE_ID) / sizeof(unsigned int); i++)
		if (FORCE_ID[i] == iForce)
			return i;
	
	return 0;
}

CMemInfoDlg::CMemInfoDlg(TEAM_MEM_WANTED* pMem, CWnd* pParent /*=NULL*/)
	: m_pMem(pMem), CDialog(CMemInfoDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CMemInfoDlg)
	m_nGender = 0;
	m_dwLevMax = 0;
	m_dwLevMin = 0;
	m_dwMemMax = 0;
	m_dwMemMin = 0;
	m_strMemTask = _T("");
	m_nOccup = 0;
	m_nRace = 0;
	m_nForce = 0;
	//}}AFX_DATA_INIT
}


void CMemInfoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMemInfoDlg)
	DDX_CBIndex(pDX, IDC_GENDER, m_nGender);
	DDX_Text(pDX, IDC_LEV_MAX, m_dwLevMax);
	DDX_Text(pDX, IDC_LEV_MIN, m_dwLevMin);
	DDX_Text(pDX, IDC_MEM_MAX, m_dwMemMax);
	DDX_Text(pDX, IDC_MEM_MIN, m_dwMemMin);
	DDX_Text(pDX, IDC_MEM_TASK, m_strMemTask);
	DDX_CBIndex(pDX, IDC_OCCUPATION, m_nOccup);
	DDX_CBIndex(pDX, IDC_RACE, m_nRace);
	DDX_CBIndex(pDX, IDC_TEAM_FORCE, m_nForce);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CMemInfoDlg, CDialog)
	//{{AFX_MSG_MAP(CMemInfoDlg)
	ON_BN_CLICKED(IDC_SEL_MEM_TASK, OnSelMemTask)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMemInfoDlg message handlers

BOOL CMemInfoDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_nRace		= m_pMem->m_ulRace;
	m_nGender	= m_pMem->m_ulGender;
	m_nOccup	= m_pMem->m_ulOccupation + 1;
	m_dwLevMin	= m_pMem->m_ulLevelMin;
	m_dwLevMax	= m_pMem->m_ulLevelMax;
	m_dwMemMin	= m_pMem->m_ulMinCount;
	m_dwMemMax	= m_pMem->m_ulMaxCount;
	m_nForce	= find_force_index(m_pMem->m_iForce);
	if (m_pMem->m_ulTask) m_strMemTask= GetTaskNameByID(m_pMem->m_ulTask);

	UpdateData(FALSE);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CMemInfoDlg::OnSelMemTask()
{
	CNumDlg dlgNum;
	dlgNum.m_nNum = m_pMem->m_ulTask;

	if(dlgNum.DoModal() == IDOK)
	{
		if(dlgNum.m_nNum <= 0)
		{
			m_pMem->m_ulTask = 0;
			GetDlgItem(IDC_MEM_TASK)->SetWindowText("");
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

		CString strName = pTask->GetName();

		m_pMem->m_ulTask = dlgNum.m_nNum;
		GetDlgItem(IDC_MEM_TASK)->SetWindowText(strName);

		delete pTask;
	}

	/*
	CTaskIDSelDlg dlg;
	dlg.m_bShowSub = false;
	dlg.m_ulID = m_pMem->m_ulTask;

	if (dlg.DoModal() == IDOK)
	{
		m_pMem->m_ulTask = dlg.m_ulID;
		GetDlgItem(IDC_MEM_TASK)->SetWindowText(dlg.m_strName);
	}
	*/
}

void CMemInfoDlg::OnOK() 
{
	UpdateData();

	m_pMem->m_ulRace		= m_nRace;
	m_pMem->m_ulGender		= m_nGender;
	m_pMem->m_ulOccupation	= m_nOccup - 1;
	m_pMem->m_ulLevelMin	= m_dwLevMin;
	m_pMem->m_ulLevelMax	= m_dwLevMax;
	m_pMem->m_ulMinCount	= m_dwMemMin;
	m_pMem->m_ulMaxCount	= m_dwMemMax;
	m_pMem->m_iForce		= FORCE_ID[m_nForce];

	CDialog::OnOK();
}
