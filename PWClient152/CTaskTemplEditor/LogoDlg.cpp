// LogoDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Resource.h"
#include "LogoDlg.h"
#include "VssOperation.h"
#include "VSSOptionDlg.h"

#ifdef _DEBUG
#define new A_DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CLogoDlg dialog


CLogoDlg::CLogoDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CLogoDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CLogoDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CLogoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CLogoDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

extern void RefreshTaskData();

void CLogoDlg::UpdateFiles()
{
	g_VSS.InitVss(NULL);
	g_VSS.SetUserName(g_strUser);
	g_VSS.SetPassword(g_strPwd);
	g_VSS.SetServerPath(g_strVSSPath);
	char szPath[MAX_PATH];
	GetCurrentDirectory(MAX_PATH, szPath);
	strcat(szPath, "\\BaseData");
	g_VSS.SetWorkFolder(szPath);
	if(g_bAutoUpdate) RefreshTaskData();
	g_VSS.SetProjectPath("");
	int result = 0;
	KillTimer(0);
	EndDialog(result);
}


BEGIN_MESSAGE_MAP(CLogoDlg, CDialog)
	//{{AFX_MSG_MAP(CLogoDlg)
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLogoDlg message handlers

BOOL CLogoDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	SetTimer(0,1000,NULL);
	// TODO: Add extra initialization here
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CLogoDlg::OnTimer(UINT nIDEvent) 
{
	// TODO: Add your message handler code here and/or call default
	UpdateFiles();
	CDialog::OnTimer(nIDEvent);
}
