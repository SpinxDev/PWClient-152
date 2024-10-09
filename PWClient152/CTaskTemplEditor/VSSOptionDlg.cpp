// VSSOptionDlg.cpp : implementation file
//

#include "stdafx.h"
#include "TaskTemplEditor.h"
#include "VSSOptionDlg.h"
#include "vssoperation.h"
#include "AVariant.h"

#ifdef _DEBUG
#define new A_DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

char		g_strUser[MAX_PATH]		= "";
char		g_strPwd[MAX_PATH]		= "";
char		g_strVSSPath[MAX_PATH]	= "";
BOOL		g_bAutoUpdate			= TRUE;
BOOL		g_bCfgInit				= FALSE;

/////////////////////////////////////////////////////////////////////////////
// CVSSOptionDlg dialog


CVSSOptionDlg::CVSSOptionDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CVSSOptionDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CVSSOptionDlg)
	m_strUser = g_strUser;
	m_strPwd = g_strPwd;
	m_strVSSPath = g_strVSSPath;
	//}}AFX_DATA_INIT
}


void CVSSOptionDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CVSSOptionDlg)
	DDX_Text(pDX, IDC_USER, m_strUser);
	DDX_Text(pDX, IDC_PWD, m_strPwd);
	DDX_Text(pDX, IDC_VSS_PATH, m_strVSSPath);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CVSSOptionDlg, CDialog)
	//{{AFX_MSG_MAP(CVSSOptionDlg)
	ON_BN_CLICKED(IDC_CHECK_AUTO_UPDATE, OnCheckAutoUpdate)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CVSSOptionDlg message handlers

BOOL CVSSOptionDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	((CButton*)GetDlgItem(IDC_CHECK_AUTO_UPDATE))->SetCheck(g_bAutoUpdate);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CVSSOptionDlg::OnCheckAutoUpdate() 
{
	g_bAutoUpdate = !g_bAutoUpdate;	
}

void CVSSOptionDlg::OnOK()
{
	UpdateData();

	strcpy(g_strUser, CSafeString(m_strUser));
	strcpy(g_strPwd, CSafeString(m_strPwd));
	strcpy(g_strVSSPath, CSafeString(m_strVSSPath));
	CDialog::OnOK();
}
