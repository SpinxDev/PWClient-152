// PWDownloaderDlg.cpp : implementation file
//

#include "stdafx.h"
#include "PWDownloader.h"
#include "PWDownloaderDlg.h"
#include "DlgSetSpeed.h"
#include "EC_PWDownload.h"
#include "EC_Pipe.h"
#include "LogFile.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// 响应托盘图标的操作
#define WM_TRAYICONNOTIFY (WM_USER + 101)
#define TRAYICON_TIPS _T("完美世界国际版完整版正在下载")

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPWDownloaderDlg dialog

CPWDownloaderDlg::CPWDownloaderDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CPWDownloaderDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPWDownloaderDlg)
	m_szProgress = _T("");
	m_szDownSpeed = _T("");
	m_szUpSpeed = _T("");
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_bDownloadOK = false;
}

CPWDownloaderDlg::~CPWDownloaderDlg()
{
}

void CPWDownloaderDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPWDownloaderDlg)
	DDX_Control(pDX, IDC_DOWN_PROGRESS, m_ctlProgress);
	DDX_Text(pDX, IDC_CUR_PROGRESS, m_szProgress);
	DDX_Text(pDX, IDC_DOWN_SPEED, m_szDownSpeed);
	DDX_Text(pDX, IDC_UP_SPEED, m_szUpSpeed);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CPWDownloaderDlg, CDialog)
	//{{AFX_MSG_MAP(CPWDownloaderDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_NCPAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_TIMER()
	ON_MESSAGE(WM_SENDPIPEDATA, OnPipeData)
	ON_WM_CLOSE()
	ON_MESSAGE(WM_TRAYICONNOTIFY, OnTrayIconNotify)
	ON_COMMAND(IDM_SETSPEED, OnSpeedSetting)
	ON_COMMAND(IDM_QUIT, OnMenuQuit)
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPWDownloaderDlg message handlers

BOOL CPWDownloaderDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	// TODO: Add extra initialization here
	memset(&m_nid, 0, sizeof(NOTIFYICONDATA));
	m_nid.cbSize = sizeof(NOTIFYICONDATA);
	m_nid.hWnd = GetSafeHwnd();
	m_nid.uID = IDR_MAINFRAME;
	m_nid.uFlags = NIF_MESSAGE|NIF_ICON|NIF_TIP;
	m_nid.uCallbackMessage = WM_TRAYICONNOTIFY;
	m_nid.hIcon = LoadIcon(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDR_MAINFRAME));
	strcpy(m_nid.szTip, TRAYICON_TIPS);
	PostMessage(WM_CLOSE, 0, 0);

	SetTimer(0, 100, NULL);		// 下载器Tick
	SetTimer(1, 500, NULL);		// 更新当前下载信息
//	SetTimer(2, 1000, NULL);	// 检测客户端是否退出
	m_ctlProgress.SetRange(0, 1000);

	// 初始化下载器
	CECPWDownload& downloader = CECPWDownload::GetInstance();
	if( !downloader.Init(((CPWDownloaderApp*)AfxGetApp())->m_hWndParent) )
	{
		AfxMessageBox(_T("初始化下载器失败！"));
		return FALSE;
	}

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CPWDownloaderDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CPWDownloaderDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CPWDownloaderDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CPWDownloaderDlg::OnTimer(UINT nIDEvent) 
{
	// TODO: Add your message handler code here and/or call default
	CECPWDownload& dl = CECPWDownload::GetInstance();

	if( nIDEvent == 0 )
	{
		dl.Tick();
	}
	else if( nIDEvent == 1 )
	{
		float fProgress, fDSpeed, fUSpeed;
		dl.GetTaskInfo(fProgress, fDSpeed, fUSpeed);
		m_ctlProgress.SetPos(int(1000 * fProgress));
		m_szProgress.Format(_T("%.1f%%"), fProgress*100);
		m_szDownSpeed.Format(_T("%.1fKB"), fDSpeed);
		m_szUpSpeed.Format(_T("%.1fKB"), fUSpeed);
		UpdateData(FALSE);

		// 下载完成
		m_bDownloadOK = dl.IsDownloadOK();
		if( m_bDownloadOK )
		{
			KillTimer(1);

			// 将下载完成通知客户端，删除该定时器
			if( ((CPWDownloaderApp*)AfxGetApp())->IsClientRunning() )
				dl.GetPipe()->SendDownloadOK();
			else
			{
				if( MessageBox(_T("完美国际完整版客户端已下载完毕，要开始安装吗？"), _T("提示"), MB_YESNO|MB_ICONQUESTION)
					== IDYES )
				{
					dl.Install();
				}
			}

			LogOutput("Download OK!!!!");
			return;
		}

		// 下载出错
		int iErrCode = dl.GetErrorCode();
		if( iErrCode > 0 )
		{
			KillTimer(1);
			CString strMsg;
			strMsg.Format(iErrCode == P2PE_TIMEOUT ? IDS_TIMEOUT : IDS_P2PE_OK + iErrCode);
			MessageBox(strMsg, "错误", MB_OK|MB_ICONERROR);
		}
	}
// 	else if( nIDEvent == 2 )
// 	{
// 		if( !((CPWDownloaderApp*)AfxGetApp())->IsClientRunning() )
// 			dl.Quit();
// 	}
	
	CDialog::OnTimer(nIDEvent);
}

void CPWDownloaderDlg::OnPipeData(WPARAM wParam, LPARAM lParam)
{
	if( wParam == 2 )
		CECPWDownload::GetInstance().GetPipe()->OnSendedDataProcessed();
	else
		CECPWDownload::GetInstance().GetPipe()->OnPipeData((int)lParam);
}

BOOL CPWDownloaderDlg::PreTranslateMessage(MSG* pMsg) 
{
	// TODO: Add your specialized code here and/or call the base class
	// 屏蔽ESC导致程序退出
 	if( pMsg->message == WM_KEYDOWN && (pMsg->wParam == VK_ESCAPE || pMsg->wParam == VK_CANCEL) )
	{
		OnClose();
 		return TRUE;
	}

	return CDialog::PreTranslateMessage(pMsg);
}

void CPWDownloaderDlg::OnClose() 
{
	// TODO: Add your message handler code here and/or call default
	ShowWindow(SW_HIDE);
	Shell_NotifyIcon(NIM_ADD, &m_nid);
}

void CPWDownloaderDlg::OnTrayIconNotify(WPARAM wParam, LPARAM lParam)
{
	if( wParam != IDR_MAINFRAME )
		return;

	if( lParam == WM_LBUTTONDOWN )
	{
		ShowWindow(SW_SHOWNORMAL);
		Shell_NotifyIcon(NIM_DELETE, &m_nid);	
	}
	else if( lParam == WM_RBUTTONDOWN )
	{
		CMenu menu;
		menu.LoadMenu(IDR_TRAYMENU);

		CPoint pt;
		GetCursorPos(&pt);
		CMenu* pSubMenu = menu.GetSubMenu(0);
		pSubMenu->TrackPopupMenu(TPM_LEFTALIGN|TPM_RIGHTBUTTON, pt.x, pt.y, this, TPM_LEFTALIGN);
	}
}

void CPWDownloaderDlg::OnSpeedSetting()
{
	CDlgSetSpeed dlg;
	dlg.DoModal();
}

void CPWDownloaderDlg::OnMenuQuit()
{
	CECPWDownload::GetInstance().Quit();
}

void CPWDownloaderDlg::OnDestroy() 
{
	CDialog::OnDestroy();
	
	// TODO: Add your message handler code here
	CECPWDownload::GetInstance().Release();
	Shell_NotifyIcon(NIM_DELETE, &m_nid);
}

void CPWDownloaderDlg::OnNcPaint()
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