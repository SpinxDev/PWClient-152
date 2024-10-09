// ElementTypeToolDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ElementTypeTool.h"
#include "ElementTypeToolDlg.h"
#include <afxdlgs.h>
#include <AAssist.h>
#include <AWScriptFile.h>
#include <AFileImage.h>
#include <AFI.h>
#include <fstream>

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
// CElementTypeToolDlg dialog

CElementTypeToolDlg::CElementTypeToolDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CElementTypeToolDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CElementTypeToolDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CElementTypeToolDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CElementTypeToolDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CElementTypeToolDlg, CDialog)
	//{{AFX_MSG_MAP(CElementTypeToolDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON1, OnButton1)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CElementTypeToolDlg message handlers

BOOL CElementTypeToolDlg::OnInitDialog()
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
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CElementTypeToolDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CElementTypeToolDlg::OnPaint() 
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
HCURSOR CElementTypeToolDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CElementTypeToolDlg::OnButton1() 
{	
	CFileDialog dlg(true, L"*.txt", NULL, OFN_FILEMUSTEXIST, L"txt Files(*.txt)|*.txt|All Files(*.*)|*.*||");
	if (dlg.DoModal() != IDOK)
		return;

	// 创建并打开输出文件
	TCHAR szBuffer1[MAX_PATH];
	DWORD dwRetVal = GetTempPath(MAX_PATH, szBuffer1);
	if (dwRetVal > MAX_PATH || dwRetVal==0)
		return;

	TCHAR szBuffer2[MAX_PATH];
	UINT bCreate = GetTempFileName(szBuffer1, NULL, 0, szBuffer2);
	if (!bCreate || bCreate==ERROR_BUFFER_OVERFLOW)
		return;

	AString strFileOut = AC2AS(szBuffer2);
	af_ChangeFileExt(strFileOut, ".txt");
	std::ofstream fileOut(strFileOut);
	if (!fileOut.is_open())
 		return;

	// 打开输入文件
	ACString strFile = dlg.GetPathName();
	AFileImage fileIn;
	if (!fileIn.Open(AC2AS(strFile), AFILE_OPENEXIST | AFILE_TYPE_BINARY))
		return;

	ACHAR *szData = new ACHAR[fileIn.GetFileLength() / sizeof(ACHAR) + 1];
	DWORD dwLen;
	fileIn.Read(szData, fileIn.GetFileLength(), &dwLen);
	fileIn.Close();
	dwLen /= sizeof(ACHAR);

	// 写XML文件头
	fileOut << "<?xml version=\"1.0\" encoding=\"GB2312\" standalone=\"no\" ?>" << std::endl;
	fileOut << "<object-array>" << std::endl;

	// 分析输入文件格式
	abase::vector<ACString> groupNames; // 当前读取的部分/全部组字符串，如武器->单剑->青铜剑
	int groupID(-1);                    // 读取的类型 ID
	bool bError(false);
	AString strTemp, strTemp2;
	ACHAR szBuffer[100];
	DWORD p(1), q(0);
	while (p < dwLen)
	{
		// 忽略所有 \r \n
		while (p<dwLen && (szData[p]=='\r' || szData[p]=='\n' || szData[p]=='\t'))
			p++;
		if (p >= dwLen)
			break;

		// 遇到第一个有效字符
		q = p+1;
		while (q<dwLen && szData[q]!='\r' && szData[q]!='\n' && szData[q]!='\t')
			q++;

		// 提取字符串
		if (q-p>=sizeof(szBuffer)/sizeof(szBuffer[0]))
		{
			bError = true;
			break;
		}

		a_strncpy(szBuffer, szData+p, q-p);
		szBuffer[q-p] = 0;

		int p1 = p-1;
		p = q;

		// 检查字符串是否为类型ID
		groupID = a_atoi(szBuffer);
		if (!groupID)
		{
			// 不是整数

			// 判断是第几层
			size_t nLevel(0);
			while (p1>=1 && szData[p1]=='\t')
			{
				++nLevel;
				--p1;
			}

			// 删除之前的层字符串
			if (nLevel<groupNames.size())
				groupNames.erase(groupNames.begin()+nLevel, groupNames.end());

			groupNames.push_back(szBuffer);
			continue;
		}

		// 是类型ID，判断是否合法文件格式
		if (groupNames.size()<2)
		{
			// 文件格式错误
			bError = true;
			break;
		}

		// 生成一行XML文件
		strTemp.Format("  <ItemType id=\"%d\" name=\"%s\"", groupID, (const char *)AC2AS(groupNames.back()));
		for (size_t i=0; i<groupNames.size(); ++i)
		{
			strTemp2.Format("group%d=\"%s\"", i, (const char *)AC2AS(groupNames[i]));
			strTemp += " ";
			strTemp += strTemp2;
		}
		strTemp += "/>";
		fileOut << (const char *)(strTemp) << std::endl;
	}
	
	if (!bError)
	{
		// 写XML文件结束
		fileOut << "</object-array>";
	}
	delete []szData;
	fileOut.close();
	
	if (bError)
	{
		AfxMessageBox(L"文件错误！", MB_ICONERROR|MB_OK);
	}

	// 打开XML文件
	ShellExecuteA(NULL, "open", strFileOut, NULL, NULL, SW_SHOWNORMAL);
}
