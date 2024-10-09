// FileExtractDlg.cpp : implementation file
//

#include "stdafx.h"
#include "FileExtract.h"
#include "FileExtractDlg.h"
#include "FileAnalyse.h"
#include "ProgressDlg.h"
#include <AString.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


// 工作目录
extern char g_szWorkDir[];

const TCHAR* ERR_INVALIDMODELPATH = _T("不允许加载游戏目录外的文件！");

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
// CFileExtractDlg dialog

CFileExtractDlg::CFileExtractDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CFileExtractDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CFileExtractDlg)
	m_szSearchText = _T("");
	m_nType = FileAnalyse::TYPE_NPCMODEL;
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CFileExtractDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFileExtractDlg)
	DDX_Control(pDX, IDC_FILELIST, m_ctlFileList);
	DDX_Text(pDX, IDC_SEARCHTEXT, m_szSearchText);
	DDX_CBIndex(pDX, IDC_ANALYSE_OPT, m_nType);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CFileExtractDlg, CDialog)
	//{{AFX_MSG_MAP(CFileExtractDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_ANALYSE, OnAnalyse)
	ON_BN_CLICKED(IDC_EXTRACT, OnExtract)
	ON_COMMAND(IDM_SAVEFILELIST, OnSaveFileList)
	ON_COMMAND(IDM_CLEARLIST, OnClearList)
	ON_WM_CONTEXTMENU()
	ON_CBN_SELCHANGE(IDC_ANALYSE_OPT, OnSelchangeAnalyseOpt)
	ON_BN_CLICKED(IDC_BTN_SELECT, OnBtnSelect)
	ON_WM_CTLCOLOR()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFileExtractDlg message handlers

BOOL CFileExtractDlg::OnInitDialog()
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
	if( !FileAnalyse::GetInstance().Init() )
	{
		PostMessage(WM_CLOSE, 0, 0);
		return FALSE;
	}

	if( m_TextTip.Create(this) )
	{
		CString strDesc;
		strDesc.LoadString(IDS_NPCMODEL);
		m_TextTip.AddTool(GetDlgItem(IDC_SEARCHTEXT), strDesc);
		m_TextTip.SetTipTextColor(RGB(0, 0, 0));
		m_TextTip.SetDelayTime(TTDT_INITIAL, 500);
		m_TextTip.SetMaxTipWidth(200);
	}

	m_ctlFileList.SetHorizontalExtent(500);
	GetDlgItem(IDC_BTN_SELECT)->ShowWindow(SW_HIDE);
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CFileExtractDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CFileExtractDlg::OnPaint() 
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
HCURSOR CFileExtractDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CFileExtractDlg::OnAnalyse() 
{
	// TODO: Add your control notification handler code here
	UpdateData();

	if( m_szSearchText.IsEmpty() )
	{
		AfxMessageBox(_T("请输入要提取的内容！"));
		return;
	}

	m_ctlFileList.ResetContent();
	GetDlgItem(IDC_ANALYSE)->EnableWindow(FALSE);

	if( m_nType == FileAnalyse::TYPE_SCALEDNPC )
		CProgressDlg::DoSearchScaledNPC();			// 搜索耗时较长，交后台线程执行
	else if( m_nType == FileAnalyse::TYPE_EQUIPGFXNOHANGER )
		CProgressDlg::DoSearchEquipGfxNoHanger();	// 搜索耗时较长，交后台线程执行
	else
		FileAnalyse::GetInstance().Analyse(AC2AS(m_szSearchText), (FileAnalyse::FileType)m_nType);

	// 显示结果
	FileAnalyse::FilePathList::const_iterator it;
	const FileAnalyse::FilePathList& files = FileAnalyse::GetInstance().GetResult();
	for( it=files.begin();it!=files.end();++it )
	{
		m_ctlFileList.AddString(AS2AC(*it));
	}

	GetDlgItem(IDC_ANALYSE)->EnableWindow(TRUE);
}

void CFileExtractDlg::OnExtract() 
{
	// TODO: Add your control notification handler code here
	TCHAR szPath[MAX_PATH] = {0};

	if( !FileAnalyse::GetInstance().GetResult().size() )
		return;

	BROWSEINFO bi;
	bi.hwndOwner = AfxGetMainWnd()->GetSafeHwnd();
	bi.pidlRoot = NULL;
	bi.pszDisplayName = szPath;
	bi.lpszTitle = _T("请选择要导出到的目录：");
	bi.ulFlags = 0;
	bi.lpfn = NULL;
	bi.iImage = 0;

	LPITEMIDLIST lp = SHBrowseForFolder(&bi);
	if( !(lp && SHGetPathFromIDList(lp, szPath)) )
		return;

	// 导出文件
	CProgressDlg::DoExport(AC2AS(szPath));
}

void CFileExtractDlg::OnSaveFileList()
{
	CFileDialog dlg(FALSE, NULL, NULL, OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,
		_T("Text File (*.txt)|*.txt||"));
	if( dlg.DoModal() == IDOK )
	{
		CString strFile = dlg.GetPathName();
		if( !strFile.IsEmpty() )
		{
			strFile += _T(".txt");
			FILE* pFile = fopen(AC2AS(strFile), "w");
			for( int i=0;i<m_ctlFileList.GetCount();i++ )
			{
				CString strLine;
				m_ctlFileList.GetText(i, strLine);
				fprintf(pFile, "%s\n", AC2AS(strLine));
			}
			fclose(pFile);
		}
	}
}

void CFileExtractDlg::OnClearList()
{
	FileAnalyse::GetInstance().Clear();
	m_ctlFileList.ResetContent();
}

void CFileExtractDlg::OnContextMenu(CWnd* pWnd, CPoint point) 
{
	// TODO: Add your message handler code here
	CRect rc;
	m_ctlFileList.GetWindowRect(&rc);
	
	CPoint pt;
	GetCursorPos(&pt);
	if( rc.PtInRect(pt) )
	{
		CMenu menu;
		menu.LoadMenu(IDR_LISTMENU);
		
		CMenu* pSubMenu = menu.GetSubMenu(0);
		pSubMenu->TrackPopupMenu(TPM_LEFTALIGN|TPM_RIGHTBUTTON, pt.x, pt.y, this);
	}
}

BOOL CFileExtractDlg::PreTranslateMessage(MSG* pMsg) 
{
	// TODO: Add your specialized code here and/or call the base class
	if( m_TextTip.m_hWnd )
		m_TextTip.RelayEvent(pMsg);
	
	return CDialog::PreTranslateMessage(pMsg);
}

void CFileExtractDlg::OnSelchangeAnalyseOpt() 
{
	// TODO: Add your control notification handler code here
	int nOldType = m_nType;
	UpdateData();

	if( nOldType != m_nType )
	{
		CString strDesc;
		strDesc.LoadString(IDS_NPCMODEL + m_nType);
		m_TextTip.UpdateTipText(strDesc, GetDlgItem(IDC_SEARCHTEXT));
		m_szSearchText = _T("");
		UpdateData(FALSE);

		if( m_nType == FileAnalyse::TYPE_ECMODEL ||
			m_nType == FileAnalyse::TYPE_UI )
			GetDlgItem(IDC_BTN_SELECT)->ShowWindow(SW_SHOW);
		else
			GetDlgItem(IDC_BTN_SELECT)->ShowWindow(SW_HIDE);

		if( m_nType == FileAnalyse::TYPE_SCALEDNPC )
		{
			GetDlgItem(IDC_SEARCHTEXT)->SetWindowText(_T("Scaled NPC"));
			GetDlgItem(IDC_SEARCHTEXT)->EnableWindow(FALSE);
		}
		else if( m_nType == FileAnalyse::TYPE_EQUIPGFXNOHANGER )
		{
			GetDlgItem(IDC_SEARCHTEXT)->SetWindowText(_T("Weapon FlySword Wing"));
			GetDlgItem(IDC_SEARCHTEXT)->EnableWindow(FALSE);
		}
		else
		{
			GetDlgItem(IDC_SEARCHTEXT)->SetWindowText(_T(""));
			GetDlgItem(IDC_SEARCHTEXT)->EnableWindow(TRUE);
		}
	}
}

void CFileExtractDlg::OnBtnSelect() 
{
	// TODO: Add your control notification handler code here
	CString strFilters;
	if( m_nType == FileAnalyse::TYPE_ECMODEL )
		strFilters = _T("Model File (*.ecm)|*.ecm||");
	else if( m_nType == FileAnalyse::TYPE_UI )
		strFilters = _T("UI Layout File (*.xml;*.dcf)|*.xml;*.dcf||");
	else
	{
		ASSERT(0);
		return;
	}

	UpdateData();

	CFileDialog dlg(TRUE, NULL, NULL, OFN_HIDEREADONLY, strFilters);
	if( dlg.DoModal() == IDOK )
	{
		AString strFile = AC2AS(dlg.GetPathName());
		if( strFile.Find(":") != -1 )
		{
			if( strFile.Find(g_szWorkDir) == -1 )
			{
				AfxMessageBox(ERR_INVALIDMODELPATH);
				return;
			}

			strFile.CutLeft(strlen(g_szWorkDir) + 1);
		}

		if( !m_szSearchText.IsEmpty() )
			m_szSearchText += _T(",");
		m_szSearchText += AS2AC(strFile);
		UpdateData(FALSE);
	}
}

HBRUSH CFileExtractDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
	
	// TODO: Change any attributes of the DC here
	if( pWnd->GetDlgCtrlID() == IDC_COPYRIGHT )
		pDC->SetTextColor(RGB(255, 0, 0));
	
	// TODO: Return a different brush if the default is not desired
	return hbr;
}
