// LogoDlg.cpp : implementation file
//

#include "stdafx.h"
#include "global.h"
#include "ElementData.h"
#include "LogoDlg.h"
#include "VssOperation.h"

#ifdef _DEBUG
#define new DEBUG_NEW
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

void CLogoDlg::UpdateFiles()
{
	CString str = CString(g_szWorkDir);
	str = str + "BaseData";
	g_VSS.SetWorkFolder(str);
	if(g_bAutoUpdate) 
	{
		g_VSS.SetProjectPath("BaseData");
		g_VSS.GetAllFiles("BaseData");
		
		g_VSS.SetProjectPath("TalkData");
		g_VSS.GetAllFiles("TalkData");
	}
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
