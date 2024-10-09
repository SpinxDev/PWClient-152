// AwardDisplayValue.cpp : implementation file
//

#include "stdafx.h"
#include "tasktempleditor.h"
#include "AwardDisplayValue.h"

#include "AObject.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAwardDisplayValue dialog


CAwardDisplayValue::CAwardDisplayValue(AWARD_DATA* pAward, CWnd* pParent /*=NULL*/)
	: CDialog(CAwardDisplayValue::IDD, pParent), m_pAward(pAward)
{
	//{{AFX_DATA_INIT(CAwardDisplayValue)
		// NOTE: the ClassWizard will add member initialization here
	m_ulExpCnt      = m_pAward->m_ulExpCnt;
	m_ulTaskCharCnt = m_pAward->m_ulTaskCharCnt;
	//}}AFX_DATA_INIT
}


void CAwardDisplayValue::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAwardDisplayValue)
	DDX_Control(pDX, IDC_AWARD_DISPLAY_STRING, m_StringList);
	DDX_Control(pDX, IDC_AWARD_DISPLAY_VALUE, m_ValueList);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CAwardDisplayValue, CDialog)
	//{{AFX_MSG_MAP(CAwardDisplayValue)
	ON_BN_CLICKED(IDC_ADD, OnAdd)
	ON_BN_CLICKED(IDC_DEL, OnDel)
	ON_NOTIFY(NM_CLICK, IDC_AWARD_DISPLAY_VALUE, OnClickAwardDisplayValue)
	ON_BN_CLICKED(IDC_ADD2, OnAdd2)
	ON_BN_CLICKED(IDC_DEL2, OnDel2)
	ON_NOTIFY(NM_CLICK, IDC_AWARD_DISPLAY_STRING, OnClickAwardDisplayString)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAwardDisplayValue message handlers

void CAwardDisplayValue::OnAdd() 
{
	// TODO: Add your control notification handler code here
	if (m_pAward->m_ulExpCnt >= TASK_AWARD_MAX_DISPLAY_EXP_CNT)
	{
		AfxMessageBox("已到达上限");
		return;
	}

	if (m_pAward->m_ulExpCnt == 0)
	{
		m_pAward->m_pszExp = new char[TASK_AWARD_MAX_DISPLAY_EXP_CNT][TASK_AWARD_MAX_DISPLAY_CHAR_LEN];
		memset(*m_pAward->m_pszExp, 0, sizeof(char) * TASK_AWARD_MAX_DISPLAY_EXP_CNT*TASK_AWARD_MAX_DISPLAY_CHAR_LEN);
	}	

	CString strIndex;
	strIndex.Format("%d", m_ulExpCnt+1);

	m_ValueList.InsertItem(m_ulExpCnt, strIndex);
	m_ValueList.SetItemText(m_ulExpCnt, 1, "");

	m_ulExpCnt++;
}

void CAwardDisplayValue::OnDel() 
{
	// TODO: Add your control notification handler code here
	POSITION pos = m_ValueList.GetFirstSelectedItemPosition();
	if (!pos) return;
	
	unsigned long uItem = (unsigned long)m_ValueList.GetNextSelectedItem(pos);
	
 	for (unsigned long i = uItem+1; i < m_ulExpCnt; i++)
 	{
		memcpy(m_pAward->m_pszExp[i-1], m_pAward->m_pszExp[i], TASK_AWARD_MAX_DISPLAY_CHAR_LEN);
 	}
 	
 	m_ulExpCnt--;
 	m_ValueList.DeleteItem(uItem);
}

void CAwardDisplayValue::OnOK() 
{
	// TODO: Add extra validation here
	
	TaskExpAnalyser Analyser;
	TaskExpressionArr tmpArr;

	unsigned long i;	
	for (i = 0; i < m_ulExpCnt; i++)
	{	
		CString StrExp = m_ValueList.GetItemText(i, 1);
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

		memcpy(m_pAward->m_pszExp[i], StrExp, sizeof(char)*TASK_AWARD_MAX_DISPLAY_CHAR_LEN);
	}	

	m_pAward->m_ulExpCnt = m_ulExpCnt;

	for (i = 0; i < m_ulTaskCharCnt; i++)
	{
		CString StrTaskChar = m_StringList.GetItemText(i, 1);
		if (StrTaskChar.GetLength() <= 0)
		{
			AfxMessageBox("字符串不能为空", MB_ICONSTOP);
			return;
		}
		else if (StrTaskChar.GetLength() >= TASK_AWARD_MAX_DISPLAY_CHAR_LEN)
		{
			AfxMessageBox("字符串填写超长", MB_ICONSTOP);
			return;
		}
		
//		memcpy(m_pAward->m_pTaskChar[i], StrTaskChar, sizeof(task_char)*TASK_AWARD_MAX_DISPLAY_CHAR_LEN);
		wcscpy(m_pAward->m_pTaskChar[i], CSafeString(StrTaskChar));
	}

	m_pAward->m_ulTaskCharCnt = m_ulTaskCharCnt;

	CDialog::OnOK();
}

void CAwardDisplayValue::OnCancel() 
{
	// TODO: Add extra cleanup here
// 	*m_pulDisplayKeyCnt = m_ulDisplayKeyCnt;
// 	memcpy(*m_pplDisplayKey, m_lDisplayKey, m_ulDisplayKeyCnt*sizeof(long));
 	CDialog::OnCancel();
}

BOOL CAwardDisplayValue::OnInitDialog() 
{
	CDialog::OnInitDialog();

	unsigned long i;
	// TODO: Add extra initialization here
	m_ValueList.SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);
	m_ValueList.InsertColumn(0, _T("序号"),  LVCFMT_LEFT, 40);
	m_ValueList.InsertColumn(1, _T("需处理的表达式"),  LVCFMT_LEFT, 300);

	for (i = 0; i < m_pAward->m_ulExpCnt; i++)
	{
		CString strIndex;
		strIndex.Format("%d", i+1);
		m_ValueList.InsertItem(i, strIndex);

		CString StrExp;
		StrExp.Format("%s", m_pAward->m_pszExp[i]);
		m_ValueList.SetItemText(i, 1, StrExp);
	}
	
	m_StringList.SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);
	m_StringList.InsertColumn(0, _T("序号"),  LVCFMT_LEFT, 40);
	m_StringList.InsertColumn(1, _T("需显示的文字"),  LVCFMT_LEFT, 300);

	for (i = 0; i < m_pAward->m_ulTaskCharCnt; i++)
	{
		CString strIndex;
		strIndex.Format("%d", i+1);
		m_StringList.InsertItem(i, strIndex);

		CString StrExp = m_pAward->m_pTaskChar[i];
		m_StringList.SetItemText(i, 1, StrExp);
	}
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CAwardDisplayValue::OnClickAwardDisplayValue(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	if (pNMListView->iSubItem == 1)
		m_ValueList.EditItem(pNMListView->iItem, pNMListView->iSubItem);

	*pResult = 0;
}

void CAwardDisplayValue::OnAdd2() 
{
	// TODO: Add your control notification handler code here
	if (m_pAward->m_ulTaskCharCnt >= TASK_AWARD_MAX_DISPLAY_EXP_CNT)
	{
		AfxMessageBox("已到达上限");
		return;
	}

	if (m_pAward->m_ulTaskCharCnt == 0)
	{
		m_pAward->m_pTaskChar = new task_char[TASK_AWARD_MAX_DISPLAY_EXP_CNT][TASK_AWARD_MAX_DISPLAY_CHAR_LEN];
		memset(*m_pAward->m_pTaskChar, 0, sizeof(task_char) * TASK_AWARD_MAX_DISPLAY_EXP_CNT*TASK_AWARD_MAX_DISPLAY_CHAR_LEN);
	}	

	CString strIndex;
	strIndex.Format("%d", m_ulTaskCharCnt+1);

	m_StringList.InsertItem(m_ulTaskCharCnt, strIndex);
	m_StringList.SetItemText(m_ulTaskCharCnt, 1, "");

	m_ulTaskCharCnt++;
}

void CAwardDisplayValue::OnDel2() 
{
	// TODO: Add your control notification handler code here
	POSITION pos = m_StringList.GetFirstSelectedItemPosition();
	if (!pos) return;
	
	unsigned long uItem = (unsigned long)m_StringList.GetNextSelectedItem(pos);
	
	for (unsigned long i = uItem+1; i < m_ulTaskCharCnt; i++)
	{
		memcpy(m_pAward->m_pTaskChar[i-1], m_pAward->m_pTaskChar[i], TASK_AWARD_MAX_DISPLAY_CHAR_LEN);
	}
	
	m_ulTaskCharCnt--;
	m_StringList.DeleteItem(uItem);
}

void CAwardDisplayValue::OnClickAwardDisplayString(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	if (pNMListView->iSubItem == 1)
		m_StringList.EditItem(pNMListView->iItem, pNMListView->iSubItem);
	
	*pResult = 0;
}
