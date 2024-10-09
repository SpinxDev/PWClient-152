// OptionVssDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ElementData.h"
#include "OptionVssDlg.h"
#include "Global.h"

/////////////////////////////////////////////////////////////////////////////
// COptionVssDlg dialog


COptionVssDlg::COptionVssDlg(CWnd* pParent /*=NULL*/)
	: CDialog(COptionVssDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(COptionVssDlg)
	m_strName = g_szUserName;
	m_strPassword =g_szPassword;
	m_strServerPath = g_szServerPath;
	//}}AFX_DATA_INIT
}


void COptionVssDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(COptionVssDlg)
	DDX_Text(pDX, IDC_EDIT1, m_strName);
	DDX_Text(pDX, IDC_EDIT2, m_strPassword);
	DDX_Text(pDX, IDC_EDIT3, m_strServerPath);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(COptionVssDlg, CDialog)
	//{{AFX_MSG_MAP(COptionVssDlg)
	ON_BN_CLICKED(IDC_CHECK_AUTO_UPDATE, OnCheckAutoUpdate)
	ON_BN_CLICKED(IDC_CHECK_CUT_LINK_VSS, OnCheckCutLinkVss)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COptionVssDlg message handlers

BOOL COptionVssDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	CButton *pButton = (CButton*)GetDlgItem(IDC_CHECK_AUTO_UPDATE);
	pButton->SetCheck(g_bAutoUpdate);

	pButton = (CButton*)GetDlgItem(IDC_CHECK_CUT_LINK_VSS);
	pButton->SetCheck(g_bLinkVss);

	m_bLink = g_bLinkVss;
	m_bAutoUpdate = g_bAutoUpdate;
	// TODO: Add extra initialization here
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void COptionVssDlg::OnCheckAutoUpdate() 
{
	// TODO: Add your control notification handler code here
	m_bAutoUpdate = !m_bAutoUpdate;
}

void COptionVssDlg::OnCheckCutLinkVss() 
{
	// TODO: Add your control notification handler code here
	m_bLink = !m_bLink;
}

void COptionVssDlg::OnOK()
{
	g_bLinkVss = m_bLink;
	g_bAutoUpdate = m_bAutoUpdate;
	CDialog::OnOK();
}
