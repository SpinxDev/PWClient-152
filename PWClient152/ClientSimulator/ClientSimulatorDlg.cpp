// ClientSimulatorDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ClientSimulator.h"
#include "ClientSimulatorDlg.h"
#include "Common.h"
#include "CmdManager.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

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
// CClientSimulatorDlg dialog

CClientSimulatorDlg::CClientSimulatorDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CClientSimulatorDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CClientSimulatorDlg)
	m_strCommand = _T("> ");
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CClientSimulatorDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CClientSimulatorDlg)
	DDX_Control(pDX, IDC_CONSOLE_TEXT, m_ConsoleText);
	DDX_Control(pDX, IDC_CONSOLE_CMD, m_ConsoleCmd);
	DDX_Text(pDX, IDC_CONSOLE_CMD, m_strCommand);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CClientSimulatorDlg, CDialog)
	//{{AFX_MSG_MAP(CClientSimulatorDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_EN_CHANGE(IDC_CONSOLE_CMD, OnChangeConsoleCmd)
	ON_WM_SIZE()
	ON_WM_CTLCOLOR()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CClientSimulatorDlg message handlers

BOOL CClientSimulatorDlg::OnInitDialog()
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
	LOGFONT font;
	memset( &font,0,sizeof(LOGFONT) );
	font.lfHeight = 12;
	font.lfWidth = -MulDiv( 9,GetDeviceCaps(GetDC()->m_hDC,LOGPIXELSY),72 );
	strcpy( font.lfFaceName, "宋体" );
	m_Font.CreateFontIndirect( &font );
	m_ConsoleText.SetFont( &m_Font );
	m_ConsoleText.SetBackgroundColor(FALSE, RGB(0, 0, 0));
	m_BlackBrush.CreateSolidBrush(RGB(0, 0, 0));

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CClientSimulatorDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CClientSimulatorDlg::OnPaint() 
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
HCURSOR CClientSimulatorDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

// 打印信息到输出对话框
void CClientSimulatorDlg::PrintMessage( const char* szMessage,int color )
{
	int nStart,nEnd;
	if( !szMessage ) return;
	
	CHARFORMAT cf = {0};
	cf.cbSize = sizeof(CHARFORMAT);
	cf.dwMask = CFM_COLOR;
	cf.dwEffects &= ~CFE_AUTOCOLOR;
	nStart = m_ConsoleText.GetTextLength();
	m_ConsoleText.SetSel(-1, -1);
	ACString strFinal = szMessage;
	m_ConsoleText.ReplaceSel(strFinal);
	nEnd = m_ConsoleText.GetTextLength();
	
	switch( color )
	{
	case LOGCOLOR_WHITE:	cf.crTextColor = RGB(255, 255, 255);	break;
	case LOGCOLOR_RED:		cf.crTextColor = RGB(255, 0, 0);		break;
	case LOGCOLOR_GREEN:	cf.crTextColor = RGB(0, 255, 0);		break;
	case LOGCOLOR_BLUE:		cf.crTextColor = RGB(0, 0, 255);		break;
	case LOGCOLOR_PURPLE:	cf.crTextColor = RGB(255, 0, 255);		break;
	case LOGCOLOR_YELLOW:	cf.crTextColor = RGB(255, 255, 0);		break;
	default:				cf.crTextColor = RGB(255, 255, 255);	break;
	}
	
	m_ConsoleText.SetSel(nStart, nEnd);
	m_ConsoleText.SetSelectionCharFormat(cf);
	m_ConsoleText.HideSelection(TRUE, FALSE);
	m_ConsoleText.LineScroll(1);
}

// 清除日志信息
void CClientSimulatorDlg::ClearMessage()
{
	m_ConsoleText.SetWindowText(_T(""));
}

BOOL CClientSimulatorDlg::PreTranslateMessage(MSG* pMsg) 
{
	// TODO: Add your specialized code here and/or call the base class
	if( pMsg->message == WM_KEYDOWN )
	{
		if( pMsg->wParam == VK_ESCAPE )
		{
			PostMessage(WM_CLOSE, 0, 0);
			return TRUE;
		}
		else if( pMsg->wParam == VK_RETURN )
		{
			UpdateData();
			AddConsoleText(m_strCommand + "\n", LOGCOLOR_WHITE);
			if( m_strCommand.GetLength() > 2 )
			{
				AString strCmd = m_strCommand.Mid(2);
				CmdManager::GetSingleton().AddCommand(strCmd);
			}
			m_strCommand = _T("> ");
			UpdateData(FALSE);
			m_ConsoleCmd.SetSel(2, 2);
			return TRUE;
		}
		else if( pMsg->wParam == VK_UP &&
			pMsg->hwnd == GetDlgItem(IDC_CONSOLE_CMD)->GetSafeHwnd() )
		{
			m_strCommand = _T("> ") + CmdManager::GetSingleton().GetPrevHistory();
			UpdateData(FALSE);
			return TRUE;
		}
		else if( pMsg->wParam == VK_DOWN &&
			pMsg->hwnd == GetDlgItem(IDC_CONSOLE_CMD)->GetSafeHwnd() )
		{
			m_strCommand = _T("> ") + CmdManager::GetSingleton().GetNextHistory();
			UpdateData(FALSE);
			return TRUE;
		}
		
		int iStart, iEnd;
		m_ConsoleCmd.GetSel(iStart, iEnd);
		if( iStart < 2 ) m_ConsoleCmd.SetSel(2, m_ConsoleCmd.GetWindowTextLength());
	}

	return CDialog::PreTranslateMessage(pMsg);
}

void CClientSimulatorDlg::OnChangeConsoleCmd() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	if( m_ConsoleCmd.GetWindowTextLength() < 2 )
	{
		m_ConsoleCmd.SetWindowText(_T("> "));
		m_ConsoleCmd.SetSel(2, 2);
	}
}

void CClientSimulatorDlg::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);
	
	const int iBorderSize = 1;
	
	// TODO: Add your message handler code here
	if( !m_ConsoleCmd.GetSafeHwnd() ) return;
	CRect rcCmd;
	m_ConsoleCmd.GetWindowRect(&rcCmd);
	
	m_ConsoleText.SetWindowPos(NULL, iBorderSize, iBorderSize, 
		cx - 2 * iBorderSize, cy - 3 * iBorderSize - rcCmd.Height(), SWP_NOACTIVATE | SWP_NOZORDER);
	m_ConsoleCmd.SetWindowPos(NULL, iBorderSize, cy - iBorderSize - rcCmd.Height(),
		cx - 2 * iBorderSize, rcCmd.Height(), SWP_NOACTIVATE | SWP_NOZORDER);
}

HBRUSH CClientSimulatorDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
	
	// TODO: Change any attributes of the DC here
	if( pWnd->GetDlgCtrlID() == IDC_CONSOLE_CMD )
	{
		pDC->SetBkColor(RGB(0, 0, 0));
		pDC->SetTextColor(RGB(255, 255, 255));
		return m_BlackBrush;
	}

	// TODO: Return a different brush if the default is not desired
	return hbr;
}
