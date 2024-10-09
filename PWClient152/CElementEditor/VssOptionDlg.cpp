// VssOptionDlg.cpp : implementation file
//

#include "global.h"
#include "elementeditor.h"
#include "VssOptionDlg.h"

//#define new A_DEBUG_NEW

/////////////////////////////////////////////////////////////////////////////
// CVssOptionDlg dialog


CVssOptionDlg::CVssOptionDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CVssOptionDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CVssOptionDlg)
	m_editVssPassword = _T("");
	m_editVssServerPath = _T("");
	m_editVssUserName = _T("");
	//}}AFX_DATA_INIT
}


void CVssOptionDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CVssOptionDlg)
	DDX_Text(pDX, IDC_EDIT_PASSWORD, m_editVssPassword);
	DDV_MaxChars(pDX, m_editVssPassword, 60);
	DDX_Text(pDX, IDC_EDIT_SERVER_PATH, m_editVssServerPath);
	DDV_MaxChars(pDX, m_editVssServerPath, 60);
	DDX_Text(pDX, IDC_EDIT_USER_NAME, m_editVssUserName);
	DDV_MaxChars(pDX, m_editVssUserName, 128);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CVssOptionDlg, CDialog)
	//{{AFX_MSG_MAP(CVssOptionDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CVssOptionDlg message handlers

BOOL CVssOptionDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	m_editVssUserName = g_Configs.szUserName;
	m_editVssPassword = g_Configs.szUserPassword;
	m_editVssServerPath = g_Configs.szServerPath;
	UpdateData(false);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CVssOptionDlg::OnOK()
{
	UpdateData(true);

	strcpy(g_Configs.szUserName,m_editVssUserName);
	strcpy(g_Configs.szUserPassword,m_editVssPassword);
	strcpy(g_Configs.szServerPath,m_editVssServerPath);

	CDialog::OnOK();
}
