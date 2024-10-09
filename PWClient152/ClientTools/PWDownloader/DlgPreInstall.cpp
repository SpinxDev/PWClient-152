// DlgPreInstall.cpp : implementation file
//

#include "stdafx.h"
#include "PWDownloader.h"
#include "DlgPreInstall.h"
#include "PWInstallDlg.h"
#include <string>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

char g_szGameDir[MAX_PATH];		// 迷你客户端的安装目录
char g_szInstallDir[MAX_PATH];	// 解压安装文件的目录

/////////////////////////////////////////////////////////////////////////////
// CDlgPreInstall dialog

CDlgPreInstall::CDlgPreInstall(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgPreInstall::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgPreInstall)
	m_szExtractDir = _T("");
	//}}AFX_DATA_INIT
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}


void CDlgPreInstall::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgPreInstall)
	DDX_Text(pDX, IDC_EXTRACT_DIR, m_szExtractDir);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgPreInstall, CDialog)
	//{{AFX_MSG_MAP(CDlgPreInstall)
	ON_BN_CLICKED(IDC_SEL_DIR, OnSelDir)
	ON_WM_TIMER()
	ON_WM_NCPAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgPreInstall message handlers

BOOL CDlgPreInstall::OnInitDialog() 
{
	CDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
	char szWorkPath[MAX_PATH];
	GetCurrentDirectoryA(MAX_PATH, szWorkPath);
	std::string strDestDir(szWorkPath);
	size_t nPos = strDestDir.find_last_of('\\');
	strDestDir = strDestDir.substr(0, nPos);
	strncpy(g_szGameDir, strDestDir.c_str(), MAX_PATH);
	strDestDir += "\\config\\element\\";
	m_szExtractDir = strDestDir.c_str();
	strncpy(g_szInstallDir, strDestDir.c_str(), MAX_PATH);
	UpdateData(FALSE);

	GetDlgItem(IDOK)->EnableWindow(FALSE);
	SetTimer(0, 200, NULL);		// 检测客户端进程是否彻底退出，彻底退出才能开始安装
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgPreInstall::OnSelDir() 
{
	// TODO: Add your control notification handler code here
	char szPath[MAX_PATH] = {0};

	BROWSEINFOA bi;
	bi.hwndOwner = GetSafeHwnd();
	bi.pidlRoot = NULL;
	bi.pszDisplayName = szPath;
	bi.lpszTitle = "请选择要解压到的目录：";
	bi.ulFlags = 0;
	bi.lpfn = NULL;
	bi.iImage = 0;

	LPITEMIDLIST lp = SHBrowseForFolder(&bi);
	if( !(lp && SHGetPathFromIDList(lp, szPath)) )
		return;

	m_szExtractDir = szPath;
	strncpy(g_szInstallDir, szPath, MAX_PATH);
	UpdateData(FALSE);
}

void CDlgPreInstall::OnOK()
{
	UpdateData();
	m_szExtractDir.TrimLeft();
	m_szExtractDir.TrimRight();
	if( m_szExtractDir.IsEmpty() )
	{
		AfxMessageBox(_T("请选择要解压的目录"));
		return;
	}

	// 开始解压
	CDialog::OnOK();
}

void CDlgPreInstall::OnCancel()
{
	if( MessageBox("确定要取消本次安装吗？", "提示", MB_YESNO|MB_ICONQUESTION) != IDYES )
		return;

	CDialog::OnCancel();
}

void CDlgPreInstall::OnTimer(UINT nIDEvent) 
{
	// TODO: Add your message handler code here and/or call default
	if( nIDEvent == 0 )
	{
		if( !((CPWDownloaderApp*)AfxGetApp())->IsClientRunning() )
		{
			KillTimer(1);
			GetDlgItem(IDOK)->EnableWindow(TRUE);
			ShowWindow(SW_SHOW);
		}
	}

	CDialog::OnTimer(nIDEvent);
}

void CDlgPreInstall::OnNcPaint()
{
	// 首次显示窗口时，系统发送WM_NCPAINT消息，此时隐藏窗口可解决闪烁问题
	static int i = 1;
	if( i > 0 )
	{
		i--;
		ShowWindow(SW_HIDE);
	}
	else
		CDialog::OnNcPaint();
}