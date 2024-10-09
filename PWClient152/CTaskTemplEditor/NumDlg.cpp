// NumDlg.cpp : implementation file
//

#include "stdafx.h"
#include "TaskTemplEditor.h"
#include "NumDlg.h"

#ifdef _DEBUG
#define new A_DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CNumDlg dialog


CNumDlg::CNumDlg(LPCTSTR szTitle, CWnd* pParent /*=NULL*/)
	: m_szTitle(szTitle), CDialog(CNumDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CNumDlg)
	m_nNum = 0;
	//}}AFX_DATA_INIT
}


void CNumDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CNumDlg)
	DDX_Text(pDX, IDC_NUM, m_nNum);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CNumDlg, CDialog)
	//{{AFX_MSG_MAP(CNumDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CNumDlg message handlers

BOOL CNumDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	if (m_szTitle) SetWindowText(m_szTitle);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
