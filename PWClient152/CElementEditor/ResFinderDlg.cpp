// ResFinderDlg.cpp : implementation file
//

#include "stdafx.h"
#include "elementeditor.h"
#include "ResFinderDlg.h"

//#define new A_DEBUG_NEW

/////////////////////////////////////////////////////////////////////////////
// CResFinderDlg dialog


CResFinderDlg::CResFinderDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CResFinderDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CResFinderDlg)
	m_strKeyWord = _T("");
	//}}AFX_DATA_INIT
	m_pResMan = 0;
}


void CResFinderDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CResFinderDlg)
	DDX_Control(pDX, IDC_LIST_SERTCH_RESULT, m_listResult);
	DDX_Text(pDX, IDC_EDIT_KEYWORD, m_strKeyWord);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CResFinderDlg, CDialog)
	//{{AFX_MSG_MAP(CResFinderDlg)
	ON_BN_CLICKED(IDC_BUTTON_FIND, OnButtonFind)
	ON_LBN_DBLCLK(IDC_LIST_SERTCH_RESULT, OnDblclkListSertchResult)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CResFinderDlg message handlers

void CResFinderDlg::OnButtonFind() 
{
	// TODO: Add your control notification handler code here
	if(m_pResMan==NULL) return;
	UpdateData(true);
	int n = m_listResult.GetCount();
	for( int m = 0; m < n; m++)
		m_listResult.DeleteString(0);

	int nResCount = m_pResMan->GetResCount(m_nResType);
	int nItem = 0;
	for(int i = 0; i < nResCount; i++)
	{
		Res_Item *pItem = m_pResMan->GetRes(m_nResType,i);
		if(pItem==NULL) continue;
		
		CString org(pItem->szOrgName);
		if(org.Find(m_strKeyWord,0)!=-1)
		{//Found item for i want
			org = pItem->szID;
			org += "--";
			org += pItem->szOrgName;
			m_listResult.AddString(org);	
		}
		nItem ++;
	}
}

void CResFinderDlg::OnOK()
{
	int index = m_listResult.GetCurSel();
	if(index!=-1)
	{
		m_listResult.GetText(index,m_strResultItem);
	}else m_strResultItem = "";

	CDialog::OnOK();
}

void CResFinderDlg::OnDblclkListSertchResult() 
{
	// TODO: Add your control notification handler code here
	int index = m_listResult.GetCurSel();
	if(index!=-1)
	{
		m_listResult.GetText(index,m_strResultItem);
		CDialog::OnOK();
	}else m_strResultItem = "";
}
