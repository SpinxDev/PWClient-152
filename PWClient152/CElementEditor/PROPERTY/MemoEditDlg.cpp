// MemoEditDlg.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "MemoEditDlg.h"
#include "AMemory.h"

//#define new A_DEBUG_NEW

/////////////////////////////////////////////////////////////////////////////
// CMemoEditDlg dialog


CMemoEditDlg::CMemoEditDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CMemoEditDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CMemoEditDlg)
	m_Memo = _T("");
	//}}AFX_DATA_INIT
}


void CMemoEditDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMemoEditDlg)
	DDX_Text(pDX, IDC_EDITMEMO, m_Memo);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CMemoEditDlg, CDialog)
	//{{AFX_MSG_MAP(CMemoEditDlg)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMemoEditDlg message handlers
