// PQGobalValue.cpp : implementation file
//

#include "stdafx.h"
#include "tasktempleditor.h"
#include "PQGobalValue.h"

#include "AObject.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPQGobalValue dialog


CPQGobalValue::CPQGobalValue(ATaskTempl* pTempl, CWnd* pParent/*=NULL*/)
	: CDialog(CPQGobalValue::IDD, pParent), m_pTempl(pTempl)
{
	//{{AFX_DATA_INIT(CPQGobalValue)
		// NOTE: the ClassWizard will add member initialization here
	m_ulPQExpCnt      = m_pTempl->m_ulPQExpCnt;
	//}}AFX_DATA_INIT

}


void CPQGobalValue::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPQGobalValue)
	DDX_Control(pDX, IDC_PQ_DISPLAY_VALUE, m_PQValueList);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPQGobalValue, CDialog)
	//{{AFX_MSG_MAP(CPQGobalValue)
	ON_BN_CLICKED(IDC_ADD, OnAdd)
	ON_BN_CLICKED(IDC_DEL, OnDel)
	ON_NOTIFY(NM_CLICK, IDC_PQ_DISPLAY_VALUE, OnClickPQDisplayValue)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPQGobalValue message handlers

void CPQGobalValue::OnAdd() 
{
	// TODO: Add your control notification handler code here
	if (m_ulPQExpCnt >= TASK_AWARD_MAX_DISPLAY_EXP_CNT)
	{
		AfxMessageBox("已到达上限");
		return;
	}

	if (m_ulPQExpCnt == 0)
	{
		m_pTempl->m_pszPQExp = new char[TASK_AWARD_MAX_DISPLAY_EXP_CNT][TASK_AWARD_MAX_DISPLAY_CHAR_LEN];
		memset(*m_pTempl->m_pszPQExp, 0, sizeof(char) * TASK_AWARD_MAX_DISPLAY_EXP_CNT*TASK_AWARD_MAX_DISPLAY_CHAR_LEN);
	}	

	CString strIndex;
	strIndex.Format("%d", m_ulPQExpCnt+1);

	m_PQValueList.InsertItem(m_ulPQExpCnt, strIndex);
	m_PQValueList.SetItemText(m_ulPQExpCnt, 1, "");

	m_ulPQExpCnt++;
}

void CPQGobalValue::OnDel() 
{
	// TODO: Add your control notification handler code here	
	POSITION pos = m_PQValueList.GetFirstSelectedItemPosition();
	if (!pos) return;
	
	unsigned long uItem = (unsigned long)m_PQValueList.GetNextSelectedItem(pos);
	
 	for (unsigned long i = uItem+1; i < m_ulPQExpCnt; i++)
 	{
		memcpy(m_pTempl->m_pszPQExp[i-1], m_pTempl->m_pszPQExp[i], TASK_AWARD_MAX_DISPLAY_CHAR_LEN);
 	}
 	
 	m_ulPQExpCnt--;
 	m_PQValueList.DeleteItem(uItem);
}

BOOL CPQGobalValue::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	m_PQValueList.SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);
	m_PQValueList.InsertColumn(0, _T("序号"),  LVCFMT_LEFT, 40);
	m_PQValueList.InsertColumn(1, _T("全局变量ID"),  LVCFMT_LEFT, 300);

	unsigned long i;
	for (i = 0; i < m_pTempl->m_ulPQExpCnt; i++)
	{
		CString strIndex;
		strIndex.Format("%d", i+1);
		m_PQValueList.InsertItem(i, strIndex);

		CString StrExp;
		StrExp.Format("%s", m_pTempl->m_pszPQExp[i]);
		m_PQValueList.SetItemText(i, 1, StrExp);
	}
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CPQGobalValue::OnClickPQDisplayValue(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	if (pNMListView->iSubItem == 1)
		m_PQValueList.EditItem(pNMListView->iItem, pNMListView->iSubItem);

	*pResult = 0;
}

void CPQGobalValue::OnOK() 
{
	// TODO: Add extra validation here
	TaskExpAnalyser Analyser;
	TaskExpressionArr tmpArr;

	unsigned long i;	
	for (i = 0; i < m_ulPQExpCnt; i++)
	{	
		CString StrExp = m_PQValueList.GetItemText(i, 1);
		if (StrExp.GetLength() >= TASK_AWARD_MAX_DISPLAY_CHAR_LEN)
		{
			AfxMessageBox("全局表达式填写超长", MB_ICONSTOP);
			return;
		}
		
		Analyser.KickBlank((LPTSTR)(LPCTSTR)StrExp);	
		BOOL bRet = Analyser.Analyse((LPCTSTR)StrExp, tmpArr);
		
		if (!bRet)
		{
			AfxMessageBox("全局表达式填写错误", MB_ICONSTOP);
			return;
		}

		memcpy(m_pTempl->m_pszPQExp[i], StrExp, sizeof(char)*TASK_AWARD_MAX_DISPLAY_CHAR_LEN);
	}	

	m_pTempl->m_ulPQExpCnt = m_ulPQExpCnt;
	
	CDialog::OnOK();
}
