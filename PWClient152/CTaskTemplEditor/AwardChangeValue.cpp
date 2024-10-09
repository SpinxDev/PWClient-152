// AwardChangeValue.cpp : implementation file
//

#include "stdafx.h"
#include "tasktempleditor.h"
#include "AwardChangeValue.h"
#include "TaskTempl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAwardChangeValue dialog


CAwardChangeValue::CAwardChangeValue(unsigned long* pulChangeKeyCnt, long **pplChangeKey, long **pplChangeKeyValue, bool **ppbChangeType, CWnd* pParent /*=NULL*/)
	: CDialog(CAwardChangeValue::IDD, pParent), m_pulChangeKeyCnt(pulChangeKeyCnt),
												m_pplChangeKey(pplChangeKey),
												m_pplChangeKeyValue(pplChangeKeyValue),
												m_ppbChangeType(ppbChangeType)
{
	//{{AFX_DATA_INIT(CAwardChangeValue)
		// NOTE: the ClassWizard will add member initialization here
// 	memset(m_lChangeKey, 0, sizeof(long)*TASK_AWARD_MAX_CHANGE_VALUE);
// 	memset(m_lChangeKeyValue, 0, sizeof(long)*TASK_AWARD_MAX_CHANGE_VALUE);
// 	memset(m_bChangeType, 0, sizeof(bool)*TASK_AWARD_MAX_CHANGE_VALUE);
	//}}AFX_DATA_INIT
}


void CAwardChangeValue::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAwardChangeValue)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	DDX_Control(pDX, IDC_AWARD_CHANGE_VALUE, m_ValueList);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CAwardChangeValue, CDialog)
	//{{AFX_MSG_MAP(CAwardChangeValue)
	ON_BN_CLICKED(IDC_ADD, OnAdd)
	ON_BN_CLICKED(IDC_DEL, OnDel)
	ON_NOTIFY(NM_CLICK, IDC_AWARD_CHANGE_VALUE, OnClickAwardChangeValue)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAwardChangeValue message handlers

BOOL CAwardChangeValue::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	m_ValueList.SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);
	m_ValueList.InsertColumn(0, _T("KEY"),  LVCFMT_LEFT, 60);
	m_ValueList.InsertColumn(1, _T("改变方式"), LVCFMT_LEFT, 100);
	m_ValueList.InsertColumn(2, _T("改变值"),   LVCFMT_LEFT, 80);

	m_strList.AddTail("设置变量");
	m_strList.AddTail("修改变量");

	for (unsigned long i = 0; i < *m_pulChangeKeyCnt; i++)
	{
		CString strKey;
		strKey.Format("%d", (*m_pplChangeKey)[i]);
		m_ValueList.InsertItem(i, strKey);
		
		m_ValueList.SetItemText(i, 1, (*m_ppbChangeType)[i] ? "修改变量" : "设置变量");
		
		CString strKeyValue;
		strKeyValue.Format("%d", (*m_pplChangeKeyValue)[i]);
		m_ValueList.SetItemText(i, 2, strKeyValue);
	}
	

	m_ulChangeKeyCnt = *m_pulChangeKeyCnt;
	memcpy(m_lChangeKey, *m_pplChangeKey, m_ulChangeKeyCnt*sizeof(long));
	memcpy(m_lChangeKeyValue, *m_pplChangeKeyValue, m_ulChangeKeyCnt*sizeof(long));
	memcpy(m_bChangeType, *m_ppbChangeType, m_ulChangeKeyCnt*sizeof(bool));

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CAwardChangeValue::OnAdd() 
{
	// TODO: Add your control notification handler code here
	if (*m_pulChangeKeyCnt >= TASK_AWARD_MAX_CHANGE_VALUE)
	{
		AfxMessageBox("已到达上限");
		return;
	}

	if (*m_pulChangeKeyCnt == 0)
	{
		*m_pplChangeKey = new long[TASK_AWARD_MAX_CHANGE_VALUE];
		memset(*m_pplChangeKey, 0, sizeof(long)* TASK_AWARD_MAX_CHANGE_VALUE);

		*m_pplChangeKeyValue= new long[TASK_AWARD_MAX_CHANGE_VALUE];
		memset(*m_pplChangeKeyValue, 0, sizeof(long)* TASK_AWARD_MAX_CHANGE_VALUE);

		*m_ppbChangeType = new bool[TASK_AWARD_MAX_CHANGE_VALUE];
		memset(*m_ppbChangeType, 0, sizeof(bool)* TASK_AWARD_MAX_CHANGE_VALUE);
	}	

	(*m_pplChangeKey)[*m_pulChangeKeyCnt] = 0;
 	(*m_pplChangeKeyValue)[*m_pulChangeKeyCnt] = 0;
 	(*m_ppbChangeType)[*m_pulChangeKeyCnt] = 0;
	

//	CString strKey;
//	strKey.Format("%d", (*m_pplChangeKey)[*m_pulChangeKeyCnt]);
	m_ValueList.InsertItem(*m_pulChangeKeyCnt, "...");

	m_ValueList.SetItemText(*m_pulChangeKeyCnt, 1, (*m_ppbChangeType)[*m_pulChangeKeyCnt] ? "修改变量" : "设置变量");

//	CString strKeyValue;
//	strKeyValue.Format("%d", (*m_pplChangeKeyValue)[*m_pulChangeKeyCnt]);
	m_ValueList.SetItemText(*m_pulChangeKeyCnt, 2, "...");

	(*m_pulChangeKeyCnt)++;
}

void CAwardChangeValue::OnDel() 
{
	// TODO: Add your control notification handler code here
	POSITION pos = m_ValueList.GetFirstSelectedItemPosition();
	if (!pos) return;
	
	unsigned long uItem = (unsigned long)m_ValueList.GetNextSelectedItem(pos);
	
	for (unsigned long i = uItem+1; i < *m_pulChangeKeyCnt; i++)
	{
		(*m_pplChangeKey)[i-1] = (*m_pplChangeKey)[i];
		(*m_pplChangeKeyValue)[i-1] = (*m_pplChangeKeyValue)[i];
		(*m_ppbChangeType)[i-1] = (*m_ppbChangeType)[i];
	}
	
	(*m_pulChangeKeyCnt)--;
	m_ValueList.DeleteItem(uItem);
}

void CAwardChangeValue::OnOK() 
{
	// TODO: Add extra validation here
	for (unsigned long i = 0; i < *m_pulChangeKeyCnt; i++)
	{	
		CString StrNum = m_ValueList.GetItemText(i, 0);
		sscanf((LPCSTR)StrNum, "%d", &(*m_pplChangeKey)[i]);
				
		CString StrType = m_ValueList.GetItemText(i, 1);
		StrType.Compare("修改变量") ? ((*m_ppbChangeType)[i] = 0) : ((*m_ppbChangeType)[i] = 1);
		
		CString StrValue = m_ValueList.GetItemText(i, 2);
		sscanf((LPCSTR)StrValue, "%d", &(*m_pplChangeKeyValue)[i]);		
	}	
	
	CDialog::OnOK();
}

void CAwardChangeValue::OnCancel() 
{
	// TODO: Add extra cleanup here
	*m_pulChangeKeyCnt = m_ulChangeKeyCnt;
	memcpy(*m_pplChangeKey, m_lChangeKey, m_ulChangeKeyCnt*sizeof(long));
	memcpy(*m_pplChangeKeyValue, m_lChangeKeyValue, m_ulChangeKeyCnt*sizeof(long));
	memcpy(*m_ppbChangeType, m_bChangeType, m_ulChangeKeyCnt*sizeof(long));

	CDialog::OnCancel();
}

void CAwardChangeValue::OnClickAwardChangeValue(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	if (pNMListView->iSubItem == 0)
		m_ValueList.EditItem(pNMListView->iItem, pNMListView->iSubItem);
	else if (pNMListView->iSubItem == 1)
		m_ValueList.ComboItem(pNMListView->iItem, pNMListView->iSubItem, m_strList);
	else if (pNMListView->iSubItem == 2)
		m_ValueList.EditItem(pNMListView->iItem, pNMListView->iSubItem);
	
	*pResult = 0;
}
