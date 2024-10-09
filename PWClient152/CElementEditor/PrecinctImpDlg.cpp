// PrecinctImpDlg.cpp : implementation file
//

#include "stdafx.h"
#include "elementeditor.h"
#include "PrecinctImpDlg.h"

//#define new A_DEBUG_NEW

/////////////////////////////////////////////////////////////////////////////
// CPrecinctImpDlg dialog


CPrecinctImpDlg::CPrecinctImpDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CPrecinctImpDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPrecinctImpDlg)
	m_strName = _T("");
	m_fX = 0.0f;
	m_fY = 0.0f;
	m_fZ = 0.0f;
	//}}AFX_DATA_INIT
}


void CPrecinctImpDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPrecinctImpDlg)
	DDX_Text(pDX, IDC_EDIT_NAME, m_strName);
	DDX_Text(pDX, IDC_EDIT_X, m_fX);
	DDX_Text(pDX, IDC_EDIT_Y, m_fY);
	DDX_Text(pDX, IDC_EDIT_Z, m_fZ);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPrecinctImpDlg, CDialog)
	//{{AFX_MSG_MAP(CPrecinctImpDlg)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPrecinctImpDlg message handlers

void CPrecinctImpDlg::OnOK()
{
	UpdateData(true);
	CDialog::OnOK();
}