// ExpPathSelDlg.cpp : implementation file
//

#include "global.h"
#include "elementeditor.h"
#include "ExpPathSelDlg.h"

//#define new A_DEBUG_NEW

/////////////////////////////////////////////////////////////////////////////
// CExpPathSelDlg dialog


CExpPathSelDlg::CExpPathSelDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CExpPathSelDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CExpPathSelDlg)
	m_strPath = g_Configs.szExpPath;
	//}}AFX_DATA_INIT
}


void CExpPathSelDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CExpPathSelDlg)
	DDX_Text(pDX, IDC_EDIT_PATH, m_strPath);
	DDV_MaxChars(pDX, m_strPath, 256);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CExpPathSelDlg, CDialog)
	//{{AFX_MSG_MAP(CExpPathSelDlg)
	ON_BN_CLICKED(IDC_BUTTON_BROWSE, OnButtonBrowse)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CExpPathSelDlg message handlers

void CExpPathSelDlg::OnButtonBrowse() 
{
	// TODO: Add your control notification handler code here
	//	Let user select a place to store exporting data
	BROWSEINFO Info;
	memset(&Info, 0, sizeof (Info));

	static char szPath[MAX_PATH] = "\0";
	
	Info.hwndOwner		= m_hWnd;
	Info.pidlRoot		= NULL;
	Info.pszDisplayName	= szPath;
	Info.lpszTitle		= "请选择导出数据的存放路径：";
	Info.ulFlags		= BIF_RETURNONLYFSDIRS | BIF_STATUSTEXT | BIF_DONTGOBELOWDOMAIN;

	LPITEMIDLIST pPIDL;
	if (!(pPIDL = SHBrowseForFolder(&Info)))
		return;

	SHGetPathFromIDList(pPIDL, szPath);
	char* szName = strrchr(szPath, '\\');
	if (!szName || *(szName+1) == '\0')
	{
		MessageBox("无法导出数据到此路径。");
		return;
	}

	m_strPath = szPath;
	UpdateData(false);
}

void CExpPathSelDlg::OnOK()
{
	UpdateData(true);
	if(m_strPath.IsEmpty())
		return;
	sprintf(g_Configs.szExpPath,"%s",m_strPath);
	CDialog::OnOK();
}
