// HullPropertyDlg.cpp : implementation file
//

#include "stdafx.h"
#include "elementeditor.h"
#include "HullPropertyDlg.h"

//#define new A_DEBUG_NEW

/////////////////////////////////////////////////////////////////////////////
// CHullPropertyDlg dialog


CHullPropertyDlg::CHullPropertyDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CHullPropertyDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CHullPropertyDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_bT = m_bC = false;
}


void CHullPropertyDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CHullPropertyDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CHullPropertyDlg, CDialog)
	//{{AFX_MSG_MAP(CHullPropertyDlg)
	ON_BN_CLICKED(IDC_RADIO_SKIP_CO, OnRadioSkipCo)
	ON_BN_CLICKED(IDC_RADIO_SKIP_TA, OnRadioSkipTa)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CHullPropertyDlg message handlers

BOOL CHullPropertyDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	CButton *pButton = (CButton*)GetDlgItem(IDC_RADIO_SKIP_CO);
	if(pButton) pButton->SetCheck(m_bC);
	pButton = (CButton*)GetDlgItem(IDC_RADIO_SKIP_TA);
	if(pButton) pButton->SetCheck(m_bT);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CHullPropertyDlg::OnRadioSkipCo() 
{
	// TODO: Add your control notification handler code here
	m_bC = !m_bC;
}

void CHullPropertyDlg::OnRadioSkipTa() 
{
	// TODO: Add your control notification handler code here
	m_bT = !m_bT;
}
