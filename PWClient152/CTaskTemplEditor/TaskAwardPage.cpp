// TaskAwardPage.cpp : implementation file
//

#include "stdafx.h"
#include "TaskTemplEditor.h"
#include "TaskAwardPage.h"
#include "TaskTemplEditorView.h"
#include "TaskIDSelDlg.h"
#include "TemplIDSelDlg.h"
#include "ItemsWantedDlg.h"
#include "AwardDlg.h"
#include "AwardByRatioDlg.h"
#include "AwardByItemsDlg.h"

#ifdef _DEBUG
#define new A_DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern LPCSTR g_strTaskIdSvrPath;

/////////////////////////////////////////////////////////////////////////////
// CTaskAwardPage property page

IMPLEMENT_DYNCREATE(CTaskAwardPage, CPropertyPage)

CTaskAwardPage::CTaskAwardPage() : CPropertyPage(CTaskAwardPage::IDD), m_pTask(0)
{
	//{{AFX_DATA_INIT(CTaskAwardPage)
	m_nAwardSel = 0;
	m_nAwardType = 0;
	//}}AFX_DATA_INIT
}

CTaskAwardPage::~CTaskAwardPage()
{
}

void CTaskAwardPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTaskAwardPage)
	DDX_Radio(pDX, IDC_AWARD_SUCCESS, m_nAwardSel);
	DDX_Radio(pDX, IDC_COMMON, m_nAwardType);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CTaskAwardPage, CPropertyPage)
	//{{AFX_MSG_MAP(CTaskAwardPage)
	ON_BN_CLICKED(IDC_EDIT, OnEdit)
	ON_BN_CLICKED(IDC_AWARD_FAILURE, OnAwardFailure)
	ON_BN_CLICKED(IDC_AWARD_SUCCESS, OnAwardSuccess)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTaskAwardPage message handlers

BOOL CTaskAwardPage::UpdatePage(ATaskTempl* pTask, BOOL bUpdateData, BOOL bReadOnly)
{
	if (bUpdateData)
	{
		UpdateData();

		if (m_nAwardSel == 0)
			pTask->m_ulAwardType_S = m_nAwardType;
		else
			pTask->m_ulAwardType_F = m_nAwardType;
	}
	else
	{
		
		if (((CButton*)GetDlgItem(IDC_AWARD_SUCCESS))->GetCheck() == BST_CHECKED)
			m_nAwardType = pTask->m_ulAwardType_S;
		else
			m_nAwardType = pTask->m_ulAwardType_F;
		m_pTask = pTask;

		CWnd* pFirst = GetWindow(GW_CHILD);
		while (pFirst)
		{
			if (pFirst->GetDlgCtrlID() != IDC_AWARD_SUCCESS
			 &&	pFirst->GetDlgCtrlID() != IDC_AWARD_FAILURE
			 && pFirst->GetDlgCtrlID() != 0xFFFF)
				pFirst->EnableWindow(!bReadOnly);
			pFirst = pFirst->GetNextWindow();
		}
		//Begin: PQ task [Yongdong,2010-1-21]
		if (pTask->m_bPQTask || pTask->m_bPQSubTask)
		{
			GetDlgItem(IDC_AWARD_FAILURE)->EnableWindow(FALSE);
		}
		else
			GetDlgItem(IDC_AWARD_FAILURE)->EnableWindow(TRUE);

		//End.

		m_bReadOnly = bReadOnly;
		UpdateData(FALSE);
	}

	return TRUE;
}

#define EDIT_AWARD(Result) \
{\
	if (m_nAwardType == 0 || m_nAwardType == 1)\
	{\
		CAwardDlg dlg(m_pTask->m_Award##Result);\
		dlg.DoModal();\
	}\
	else if (m_nAwardType == 2)\
	{\
		CAwardByRatioDlg dlg(m_pTask->m_AwByRatio##Result);\
		dlg.DoModal();\
	}\
	else if (m_nAwardType == 3)\
	{\
		CAwardByItemsDlg dlg(m_pTask->m_AwByItems##Result);\
		dlg.DoModal();\
	}\
}

void CTaskAwardPage::OnEdit() 
{
	if (!m_pTask) return;

	UpdateData();

	if (m_nAwardSel == 0)
		EDIT_AWARD(_S)
	else
		EDIT_AWARD(_F)
}

void CTaskAwardPage::OnAwardSuccess()
{
	if (!m_pTask) return;

	m_nAwardSel = 0;
	m_nAwardType = m_pTask->m_ulAwardType_S;
	UpdateData(FALSE);
}

void CTaskAwardPage::OnAwardFailure()
{
	if (!m_pTask) return;

	m_nAwardSel = 1;
	m_nAwardType = m_pTask->m_ulAwardType_F;
	UpdateData(FALSE);
}
