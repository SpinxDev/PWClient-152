// CreateWorkspaceDlg.cpp : implementation file
//

#include "global.h"
#include "elementeditor.h"
#include "CreateWorkspaceDlg.h"
#include "AF.h"

//#define new A_DEBUG_NEW

/////////////////////////////////////////////////////////////////////////////
// CCreateWorkspaceDlg dialog


CCreateWorkspaceDlg::CCreateWorkspaceDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CCreateWorkspaceDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CCreateWorkspaceDlg)
	m_strNewWorkspace = _T("");
	m_bRandomMap = FALSE;
	//}}AFX_DATA_INIT
}


void CCreateWorkspaceDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCreateWorkspaceDlg)
	DDX_Text(pDX, IDC_EDIT_WORKSPACE_NAME, m_strNewWorkspace);
	DDX_Check(pDX,IDC_CHK_RANDOM_MAP, m_bRandomMap);
	DDV_MaxChars(pDX, m_strNewWorkspace, 128);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CCreateWorkspaceDlg, CDialog)
	//{{AFX_MSG_MAP(CCreateWorkspaceDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCreateWorkspaceDlg message handlers

void CCreateWorkspaceDlg::OnOK() 
{
	// TODO: Add extra validation here
	UpdateData(true);
	if(m_strNewWorkspace.IsEmpty())
	{
		MessageBox("不能创建名字为空的工作空间，请重新指定名字");
		return;
	}

	bool bExist = false;
	AScriptFile File;
	if (File.Open("EditMaps\\workspace.ws"))
	{//Clear current project list
		while (File.GetNextToken(true))
		{
			if(stricmp(File.m_szToken,m_strNewWorkspace)==0)
			{
				bExist = true;
				break;
			}
		}
		File.Close();
	}
	if(bExist) 
	{
		MessageBox("相同名字的工作空间已经存在，请重新指定名字");
		return;
	}
	CDialog::OnOK();
}
