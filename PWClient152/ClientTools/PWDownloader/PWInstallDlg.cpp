// PWInstallDlg.cpp : implementation file
//

#include "stdafx.h"
#include "PWDownloader.h"
#include "PWInstallDlg.h"
#include "LogFile.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern char g_szGameDir[];
extern char g_szClientCompleted[];
extern char g_szInstallDir[];
extern void UDeleteFile(const char* filename);

/////////////////////////////////////////////////////////////////////////////
// CPWInstallDlg dialog

CPWInstallDlg* CPWInstallDlg::s_pThis;

CPWInstallDlg::CPWInstallDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CPWInstallDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPWInstallDlg)
	m_szCurFile = _T("");
	m_szProgress = _T("");
	//}}AFX_DATA_INIT

	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_pBG = NULL;
	m_bInstallOK = false;
	m_nTotal = 100;
	m_nCompleted = 0;
	::InitializeCriticalSection(&m_csLock);
	s_pThis = this;
}

CPWInstallDlg::~CPWInstallDlg()
{
	if( m_pBG )
	{
		delete m_pBG;
		m_pBG = NULL;
	}

	::DeleteCriticalSection(&m_csLock);
	s_pThis = NULL;
}

void CPWInstallDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPWInstallDlg)
	DDX_Control(pDX, IDC_CTL_PROGRESS, m_ctlProgress);
	DDX_Text(pDX, IDC_CUR_FILE, m_szCurFile);
	DDX_Text(pDX, IDC_PROGRESS_TEXT, m_szProgress);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPWInstallDlg, CDialog)
	//{{AFX_MSG_MAP(CPWInstallDlg)
	ON_WM_TIMER()
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPWInstallDlg message handlers

void CPWInstallDlg::OnTimer(UINT nIDEvent) 
{
	// TODO: Add your message handler code here and/or call default
	if( nIDEvent == 0 )
	{
		::EnterCriticalSection(&m_csLock);
		int iProgress = (int)(1000 * ((double)(INT64)m_nCompleted / (INT64)m_nTotal));
		m_ctlProgress.SetPos(iProgress);
		m_szProgress.Format(_T("%.1f%%"), (double)(INT64)m_nCompleted / (INT64)m_nTotal * 100);
		UpdateData(FALSE);
		if( m_nCompleted == m_nTotal )
			OnExtractComplete();
		::LeaveCriticalSection(&m_csLock);
	}

	CDialog::OnTimer(nIDEvent);
}

// 安装客户端的线程
DWORD WINAPI ThreadProc(LPVOID lpParameter)
{
	CPWInstallDlg* pDlg = (CPWInstallDlg*)lpParameter;
	IArchive7Z* pArchive = CreateArchive7Z();
	pArchive->SetExtractCB(CPWInstallDlg::OnExtractCallback);
	if( pArchive->LoadPack(g_szClientCompleted, "") )
	{
		if( !pArchive->ExtractTo(pDlg->m_sExtractDir) )
		{
			MessageBox(NULL, "请清理磁盘空间，然后运行迷你客户端可重新进行解压!", "错误", MB_OK|MB_ICONERROR);
			LogOutput("ThreadProc, Extract the full client pack failed!");
		}
	}
	else
	{
		MessageBox(NULL, "打开安装包失败！", "错误", MB_OK|MB_ICONERROR);
		LogOutput("ThreadProc, Load the full client pack failed!");
	}
	pArchive->Release();
	return 0;
}

// 响应解压完成
void CPWInstallDlg::OnExtractComplete()
{
	KillTimer(0);
	m_bInstallOK = true;

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if( pSysMenu ) pSysMenu->EnableMenuItem(SC_CLOSE, MF_BYCOMMAND|MF_ENABLED);

	if( MessageBox(_T("解压完成，是否要删除压缩包以节省磁盘空间？"), "提示", MB_YESNO|MB_ICONQUESTION) == IDYES )
		UDeleteFile(g_szClientCompleted);

	// 启动安装程序
	std::string strWorkDir = std::string(g_szInstallDir) + "\\完美世界国际版";
	std::string strPath = strWorkDir + "\\install.exe";
	ShellExecuteA(NULL, "open", strPath.c_str(), NULL, strWorkDir.c_str(), SW_SHOWNORMAL);
	CDialog::OnOK();
}

void CPWInstallDlg::OnExtractCallback(int op, IArchive7Z::PROPERTY& prop)
{
	CPWInstallDlg* pDlg = CPWInstallDlg::s_pThis;

	::EnterCriticalSection(&pDlg->m_csLock);
	if( op == IArchive7Z::OP_GETTOTAL )
	{
		pDlg->m_nTotal = prop.pi;
	}
	else if( op == IArchive7Z::OP_GETCOMPLETED )
	{
		pDlg->m_nCompleted = prop.pi;
	}
	else if( op == IArchive7Z::OP_GETCURFILE )
	{
		pDlg->m_szCurFile = prop.ps;
	}
	::LeaveCriticalSection(&pDlg->m_csLock);
}

BOOL CPWInstallDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	// Load the background image
	LoadBGImage();
	
	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
	m_ctlProgress.SetRange(0, 1000);

	// 开始解压文件
	HANDLE hThread = ::CreateThread(NULL, 0, ThreadProc, this, 0, NULL);
	::CloseHandle(hThread);
	SetTimer(0, 50, NULL);
	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if( pSysMenu ) pSysMenu->EnableMenuItem(SC_CLOSE, MF_BYCOMMAND|MF_GRAYED);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

bool CPWInstallDlg::LoadBGImage()
{
	HINSTANCE hInst = AfxGetInstanceHandle();
	HRSRC hResource = FindResource(hInst, MAKEINTRESOURCE(IDR_INSTALLBG), "PNG");
	if( !hResource ) return false;
	DWORD imageSize = SizeofResource(hInst, hResource);
	if( !imageSize ) return false;

	const void* pResData = LockResource(LoadResource(hInst, hResource));
	if( !pResData ) return false;
	HGLOBAL hBuffer = GlobalAlloc(GMEM_FIXED, imageSize);
	if( hBuffer )
	{
		void* pBuffer = GlobalLock(hBuffer);
		if( pBuffer )
		{
			memcpy(pBuffer, pResData, imageSize);
			GlobalUnlock(hBuffer);

			IStream* pStream = NULL;
			if( SUCCEEDED(CreateStreamOnHGlobal(hBuffer, FALSE, &pStream)) )
			{
				m_pBG = Gdiplus::Image::FromStream(pStream);
				pStream->Release();
			}
		}

		GlobalFree(hBuffer);
	}

	return true;
}

void CPWInstallDlg::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	// TODO: Add your message handler code here
	Gdiplus::Graphics gph(dc);
	gph.DrawImage(m_pBG, 7, 7, m_pBG->GetWidth(), m_pBG->GetHeight());

	// Do not call CDialog::OnPaint() for painting messages
}

void CPWInstallDlg::OnCancel()
{
	// 什么也不做，防止ESC退出
}