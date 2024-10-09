// ClientTryLoginDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ClientTryLogin.h"
#include "ClientTryLoginDlg.h"

#include <fstream>
#include <stdio.h>
#include "Base64.h"

#include "Common.h"
#include "EC_CrossServerList.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


std::wstring StringToWString(const std::string&str)
{
	int n = MultiByteToWideChar(CP_ACP,0,str.c_str(),-1,0,0);
	wchar_t*buf = new wchar_t[n];
	
	MultiByteToWideChar(CP_ACP,0,str.c_str(),-1,buf,n);
	std::wstring wstr(buf,n-1);
	delete[]buf;
	return wstr;
}

std::string WStringToString(const std::wstring&wstr)
{
	int n = WideCharToMultiByte(CP_ACP,0,wstr.c_str(),-1,0,0,0,0);
	char*buf = new char[n];
	
	WideCharToMultiByte(CP_ACP,0,wstr.c_str(),-1,buf,n,0,0);
	std::string str(buf,n-1);
	delete[]buf;
	return str;
}

static CString s_LastFolder;
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
// CClientTryLoginDlg dialog

CClientTryLoginDlg::CClientTryLoginDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CClientTryLoginDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CClientTryLoginDlg)
	m_serverList = _T("");
	m_userName = _T("");
	m_pwd = _T("");
	m_serverNum = _T("");
	m_TestCount = 0;
	m_version = _T("");
	m_nPortTestLoopCount = 0;
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CClientTryLoginDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CClientTryLoginDlg)
	DDX_Control(pDX, IDC_BUTTON_RELOGIN, m_btnReStart);
	DDX_Control(pDX, IDC_LIST_RESULT_ACCOUNT, m_accountCtrl);
	DDX_Control(pDX, IDC_BUTTON_LOGIN, m_btnStart);
	DDX_Control(pDX, IDC_PROGRESS_LOGIN, m_progressCtrl);
	DDX_Control(pDX, IDC_LIST_RESULT_SUCCESS, m_successInfoCtrl);
	DDX_Control(pDX, IDC_LIST_RESULT_FAIL, m_failInfoCtrl);
	DDX_Control(pDX, IDC_NORMAL_TEST, m_btnNormal);
	DDX_Control(pDX, IDC_CROSS_TEST, m_btnCrossServer);
	DDX_Control(pDX, IDC_WAIT_WARNING, m_Warning);
	DDX_Text(pDX, IDC_EDIT_SERVERLIST, m_serverList);
	DDX_Text(pDX, IDC_EDIT_USERNAME, m_userName);
	DDX_Text(pDX, IDC_EDIT_PWD, m_pwd);
	DDX_Text(pDX, IDC_STATIC_ServerName, m_serverNum);
	DDX_Text(pDX, IDC_STATIC_VERSION, m_version);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CClientTryLoginDlg, CDialog)
	//{{AFX_MSG_MAP(CClientTryLoginDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_FIND_SERVERLIST, OnButtonFindServerlist)
	ON_BN_CLICKED(IDC_BUTTON_LOGIN, OnButtonLogin)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_LIST_RESULT_SUCCESS, OnDrawListResultSuccess)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_LIST_RESULT_FAIL, OnFailDrawListResultFail)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BUTTON_OPEN_ERROR, OnButtonOpenError)
	ON_BN_CLICKED(IDC_BUTTON_ADDACCOUNT, OnButtonAddaccount)
	ON_BN_CLICKED(IDC_BUTTON_DELACCOUNT, OnButtonDelaccount)
	ON_BN_CLICKED(IDC_BUTTON_RELOGIN, OnButtonRelogin)
	ON_WM_CTLCOLOR()
	ON_WM_CANCELMODE()
	ON_BN_CLICKED(IDC_NORMAL_TEST, OnNormalTest)
	ON_BN_CLICKED(IDC_CROSS_TEST, OnCrossTest)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CClientTryLoginDlg message handlers

// 重新load账号
void CClientTryLoginDlg::ReLoadAccount()
{
	m_accountInfos.clear();
	m_accountCtrl.DeleteAllItems();

	TCHAR info[MAX_PATH];
	TCHAR temp[MAX_PATH];
	unsigned int userCount = ::GetPrivateProfileInt(_T("info"),_T("userCount"),0,_T("info.ini"));
	::GetPrivateProfileString(_T("info"),_T("serverList"),_T(""),info,MAX_PATH,_T("info.ini"));
	m_serverList = info;

	AccountInfo ainfo;
	Base64Code bc64;
	for(int k = 0; k <= (int)userCount;k++)
	{
		sprintf( temp, "userName%d", k);
		::GetPrivateProfileString(_T("info"),temp,_T(""),info,MAX_PATH,_T("info.ini"));
		ainfo.userName = info;
		if(ainfo.userName.IsEmpty()) continue;
		sprintf( temp, "password%d", k);
		::GetPrivateProfileString(_T("info"),temp,_T(""),info,MAX_PATH,_T("info.ini"));
		bc64.Decode(info,temp);
		for(int i = 0; i < MAX_PATH;i++)
			temp[i] = ~temp[i];
		ainfo.pwd = temp;
		if(!ainfo.userName.IsEmpty() && !ainfo.pwd.IsEmpty())
		{
			m_accountInfos.push_back(ainfo);
			// 插入用户账号
			int n = m_accountCtrl.GetItemCount();
			m_accountCtrl.InsertItem(n,ainfo.userName,0);
			// 设置用户ini配置名称
			sprintf( temp, "userName%d", k);
			m_accountCtrl.SetItemText(n,1,temp);
		}
	}
	
	UpdateData(false);
}

BOOL CClientTryLoginDlg::OnInitDialog()
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
	
	DWORD dwStyle = m_successInfoCtrl.GetExtendedStyle();
	m_successInfoCtrl.SetExtendedStyle(dwStyle | LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);

	dwStyle = m_accountCtrl.GetExtendedStyle();
	m_accountCtrl.SetExtendedStyle(dwStyle | LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);

	dwStyle = m_failInfoCtrl.GetExtendedStyle();
	m_failInfoCtrl.SetExtendedStyle(dwStyle | LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);

	// 初始化listCtrl
	const int GROUP_NAME_LEN(120), IP_LEN(120), PORT_LEN(100), INFO_LEN(300);
	m_successInfoCtrl.InsertColumn(0,_T("服务器组名"),LVCFMT_LEFT,GROUP_NAME_LEN);
	m_successInfoCtrl.InsertColumn(1,_T("服务器地址"),LVCFMT_LEFT,IP_LEN);
	m_successInfoCtrl.InsertColumn(2,_T("服务器端口"),LVCFMT_LEFT,PORT_LEN);
	m_successInfoCtrl.InsertColumn(3,_T("服务器信息"),LVCFMT_LEFT,INFO_LEN);
	       
	m_failInfoCtrl.InsertColumn(0,_T("服务器组名"),LVCFMT_LEFT,GROUP_NAME_LEN);
	m_failInfoCtrl.InsertColumn(1,_T("服务器地址"),LVCFMT_LEFT,IP_LEN);
	m_failInfoCtrl.InsertColumn(2,_T("服务器端口"),LVCFMT_LEFT,PORT_LEN);
	m_failInfoCtrl.InsertColumn(3,_T("服务器信息"),LVCFMT_LEFT,INFO_LEN);

	m_accountCtrl.InsertColumn(0,_T("账号名称"),LVCFMT_LEFT,260);
	m_accountCtrl.InsertColumn(1,_T("user"),LVCFMT_LEFT,0);

	m_btnNormal.SetCheck(BST_CHECKED);
	m_Warning.ShowWindow(SW_HIDE);

	TCHAR temp[MAX_PATH];
	sprintf( temp, _T("客户端大版本：%s,小版本：%s"),  BigVersionStr(GAME_VERSION), GAME_VERSION_STRING);

	m_version = temp;
	UpdateData(false);
	ReLoadAccount();
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CClientTryLoginDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CClientTryLoginDlg::OnPaint() 
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
HCURSOR CClientTryLoginDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

int CALLBACK BrowseCallbackProc(HWND hWnd, UINT uMsg, LPARAM lParam, LPARAM lpData)
{
	if (uMsg == BFFM_INITIALIZED)
	{
		// 将目录设置为初始化目录
		SendMessage(hWnd, BFFM_SETSELECTION, TRUE, (LPARAM)(LPCTSTR)s_LastFolder);
	}
	return 0;
}

// 输入serverlist.txt的路径
void CClientTryLoginDlg::OnButtonFindServerlist() 
{
	std::string root = m_serverList;
	int pos = root.rfind("/");
	if(pos == -1)
		pos = root.rfind("\\");
	if(pos != -1)
	{
		root = root.substr(0,pos);
		s_LastFolder = root.c_str();
	}
	else
	{
		TCHAR info[MAX_PATH];
		::GetCurrentDirectory(MAX_PATH,info);
		s_LastFolder = info;
	}
	
	BROWSEINFO bi;
	memset(&bi,0,sizeof(BROWSEINFO));
	bi.hwndOwner = GetSafeHwnd();
	bi.ulFlags = BIF_NEWDIALOGSTYLE | BIF_STATUSTEXT | BIF_BROWSEINCLUDEFILES | BIF_VALIDATE;;
	bi.lpszTitle = _T("选择Serverlist.txt");
	bi.lpfn = BrowseCallbackProc;
	std::string s = m_serverList;
	bi.pszDisplayName = (char*)s.c_str();

	LPITEMIDLIST idl = ::SHBrowseForFolder(&bi);
	if(idl == NULL) return;
	::SHGetPathFromIDList(idl,m_serverList.GetBuffer(MAX_PATH));
	m_serverList.ReleaseBuffer();

	UpdateData(false);
}

void CClientTryLoginDlg::StartTest()
{
	m_TestCount = 0;
	DisablePanel();

	// 读取列表
	ReadServerList();
	// 设置timer
	m_progressCtrl.SetPos(0);
	SetTimer(1,1000,NULL);
}


void CClientTryLoginDlg::SplitString(const AWString& str,SplitStrings&splits,const wchar_t*sep,bool bOnce)
{
	splits.clear();
	AWString temp = str;
	size_t pos;
	
	while(true)
	{
		pos = temp.Find(sep);
		if(pos == -1)
		{
			splits.push_back(temp);
			break;
		}
		splits.push_back(temp.Left(pos));
		temp.CutLeft(pos + wcslen(sep));
		while(true)
		{
			pos = temp.Find(sep);
			if(pos != 0) break;
			temp.CutLeft(pos + wcslen(sep));
		}
		
		if(bOnce)
		{
			splits.push_back(temp);
			break;
		}
	}
}

void CClientTryLoginDlg::ReadServerList()
{
	FILE *fStream;

	const AWString HOT_SERVER = L"&1";
	const AWString CROSS_SERVER = L"&2";

	SplitStrings splits;
	SplitStrings splitIPs;
	SplitStrings splitPorts;
	AWString groupName;

	m_LoginVector.clear();
	CECCrossServerList::GetSingleton().Clear();
	m_nPortTestLoopCount = 0;

	if(!m_serverList.IsEmpty() && (fStream = fopen(m_serverList, "rb")) != NULL )
	{
		m_serverInfo.clear();
		m_serverName2Group.clear();
		m_TestCount = 0;

		wchar_t szLine[256];
		szLine[0] = fgetwc(fStream);
		if( szLine[0] != 0xfeff )
		{
			MessageBox(_T("不是有效的完美国际serverlist.txt格式！"), _T("错误"), MB_OK|MB_ICONERROR);
			KillTimer(1);
			EnablePanel();
			return;
		}

		while(true)
		{
			memset((char*)szLine,0,sizeof(szLine));
			if( !fgetws(szLine, 256, fStream) )
				break;
			if( wcslen(szLine) > 0)
			{
				int n = wcslen(szLine) - 1;
				while( n >= 0 && (szLine[n] == L'\r' || szLine[n] == L'\n') )
					n--;
				szLine[n + 1] = L'\0';

				SplitString(szLine,splits,L"\t");

				if( splits.size() >= 2 )	// server
				{
					SplitString(splits[1],splitIPs,L":");
					if( splitIPs.size() >= 2 )
					{
						OneServerInfo info;
						info.serverName = splits[0];
						info.serverIP = splitIPs[1];
						info.serverIP.TrimLeft();
						info.serverIP.TrimRight();
						info.serverPortStart = 0;
						info.serverPortEnd   = 0;
						info.serverLine = splitIPs.size() > 2 ? splitIPs[2].ToInt() : 0;
						info.bHaveFail = false;
						info.bHaveSuccess = false;

						if( splits.size() >= 3 )
							info.serverZoneID = splits[2].ToInt();
						else
							info.serverZoneID = 0;

						SplitString(splitIPs[0],splitPorts,L"-");
						if (!splitPorts.empty())
						{
							AString strPort = WC2AS(splitPorts[0]);
							strPort.TrimLeft();
							strPort.TrimRight();
							int pmin = strPort.ToInt();
							int pmax = pmin;
							
							if (splitPorts.size()>1)
							{
								strPort = WC2AS(splitPorts[1]);
								strPort.TrimLeft();
								strPort.TrimRight();
								pmax = strPort.ToInt();
							}
							
							if (pmin > pmax)
								std::swap(pmin, pmax);
							
							info.serverPortStart = pmin;
							info.serverPortEnd = pmax + 1;// 最后一个作为结束条件
						}

						// 跨服服务器
						if( wcsstr(szLine, CROSS_SERVER) )
						{
							CECCrossServerList::ServerInfo crossInfo;
							crossInfo.port		= a_Random(info.serverPortStart, info.serverPortEnd - 1);
							crossInfo.port_max	= info.serverPortEnd - 1;
							crossInfo.port_min	= info.serverPortStart;
							crossInfo.address	= WC2AS(info.serverIP);
							crossInfo.zoneid	= info.serverZoneID;
							crossInfo.server_name = WC2AS(info.serverName);
							crossInfo.line		= info.serverLine;
							CECCrossServerList::GetSingleton().PutServerAllPort(crossInfo);
							continue;
						}
						
						m_serverInfo[groupName].push_back(info);
						m_serverName2Group[info.serverIP] = groupName;

						// 加入测试集
						if( !IsTestCrossServer() )
						{
							for(int iPort = info.serverPortStart;iPort < info.serverPortEnd; ++iPort)
							{
								++m_TestCount;

								ConnectResult cr;
								cr.zone_id = info.serverZoneID;
								cr.address = WC2AS(info.serverIP);
								cr.port = iPort;
								cr.line = info.serverLine;
								m_LoginVector.push_back(cr);
							}
						}
						else
						{
							++m_TestCount;

							ConnectResult cr;
							cr.zone_id = info.serverZoneID;
							cr.address = WC2AS(info.serverIP);
							cr.port = info.serverPortStart;
							cr.line = info.serverLine;
							m_LoginVector.push_back(cr);
						}
					}
				}
				else // group
				{
					if(!splits[0].IsEmpty())
					{
						groupName = splits[0];
						AreaServers as;
						m_serverInfo[groupName] = as;
					}
				}
			}

		}

		fclose(fStream);
	}

	int acountSize = (int)m_accountInfos.size();
	if(acountSize * MAX_LOGINTEST_COUNT < m_TestCount)
	{
		int leastAccount = m_TestCount / MAX_LOGINTEST_COUNT;
		if( m_TestCount % MAX_LOGINTEST_COUNT ) leastAccount++;

		if( !IsTestCrossServer() )
		{
			CString strMsg;
			strMsg.Format(_T("测试服务器数量为%d, 测试账号不足，至少需要%d个！\n请点击确认继续测试。点击取消则取消本次测试！"),m_TestCount,leastAccount);
			int ret = MessageBox(strMsg,_T("自动登陆"),MB_OKCANCEL);
			if(ret >= 2)
			{
				KillTimer(1);
				EnablePanel();
				return;
			}
		}
	}

	for(int i = 0;i < (int)m_LoginVector.size();++i)
	{
		LoginServer(m_LoginVector[i].zone_id, m_LoginVector[i].line, m_LoginVector[i].address,m_LoginVector[i].port);
	}
}

// 自动登陆测试
void CClientTryLoginDlg::OnButtonLogin() 
{
	UpdateData();
	
	if(m_serverList.IsEmpty())
	{
		MessageBox(_T("没有ServerList.txt,请先输入！"),_T("自动登陆"));
		return;
	}

	if(m_accountInfos.empty())
	{
		MessageBox(_T("没有可用的账号，请先输入！"), _T("自动登录"));
		return;
	}

	// 先保存到ini文件中。
	::WritePrivateProfileString(_T("info"),_T("serverList"),m_serverList,_T("info.ini"));
	// 设置测试账号
	ClearTestUser();
	for(int i = 0;i < (int)m_accountInfos.size();++i)
	{
		AddTestUser(m_accountInfos[i].userName,m_accountInfos[i].pwd);
	}
	// 设置结果收集
	SetResultCollector(&m_collector);
	// 开始测试
	StartTest();
}

// 重新测试
void CClientTryLoginDlg::OnButtonRelogin() 
{
	UpdateData();

	if(m_accountInfos.empty())
	{
		MessageBox(_T("没有可用的账号，请先输入！"), _T("自动登录"));
		return;
	}
	
	// 设置测试账号
	ClearTestUser();
	for(int i = 0;i < (int)m_accountInfos.size();++i)
	{
		AddTestUser(m_accountInfos[i].userName,m_accountInfos[i].pwd);
	}
	// 设置结果收集
	SetResultCollector(&m_collector);
	// 开始失败测试
	StartFailTest();
}

void CClientTryLoginDlg::StartFailTest()
{
	DisablePanel();

	// 读取失败列表
	ReadFailServerList();
	// 清空失败列表
	m_failInfoCtrl.DeleteAllItems();
	// 设置timer
	m_progressCtrl.SetPos(0);
	SetTimer(1,1000,NULL);
}

void CClientTryLoginDlg::ReadFailServerList()
{
	// 清空信息
	ServerInfo::iterator it = m_serverInfo.begin(), itEnd = m_serverInfo.end();
	for(;it != itEnd;++it)
	{
		AreaServers& as = it->second;

		for(int i = 0;i < (int)as.size();++i)
		{
			as[i].bHaveSuccess = false;
			as[i].bHaveFail    = false;
			as[i].cResults.clear();
		}
	}

	CString groupName;
	CString serverName;
	CString serverIP;
	CString serverPort;

	m_TestCount = 0;
	// 直接从失败服务器列表中读取
	int nCount = m_failInfoCtrl.GetItemCount();
	for(int n = 0;n < nCount;n++)
	{
		int iItemData = (int)m_failInfoCtrl.GetItemData(n);
		int iZoneID = iItemData & 0xffff;
		int iLine = iItemData >> 16;
		serverName = m_failInfoCtrl.GetItemText(n,0);
		serverIP   = m_failInfoCtrl.GetItemText(n,1);
		serverPort = m_failInfoCtrl.GetItemText(n,2);

		if(serverIP.IsEmpty() || serverPort.IsEmpty())
		{
			groupName = serverName;
			continue;
		}

		++m_TestCount;
		LoginServer(iZoneID, iLine, serverIP, atoi(serverPort));
	}
}

void CClientTryLoginDlg::OnDrawListResultSuccess(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LPNMLVCUSTOMDRAW lplvcd = (LPNMLVCUSTOMDRAW)pNMHDR;
	*pResult = CDRF_DODEFAULT;
	switch(lplvcd->nmcd.dwDrawStage)
	{
	case CDDS_PREPAINT:
		{
			*pResult = CDRF_NOTIFYITEMDRAW;
			return;
		}
	case CDDS_ITEMPREPAINT:
		{
			int nItem = static_cast<int>(lplvcd->nmcd.dwItemSpec);
			COLORREF clrNewTextColor = RGB(0,155,0);
			COLORREF clrNewBkColor = RGB(255,255,255);
			lplvcd->clrText = clrNewTextColor;
			lplvcd->clrTextBk = clrNewBkColor;
			*pResult = CDRF_DODEFAULT;
			return;
		}
	}
}

void CClientTryLoginDlg::OnFailDrawListResultFail(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LPNMLVCUSTOMDRAW lplvcd = (LPNMLVCUSTOMDRAW)pNMHDR;
	*pResult = CDRF_DODEFAULT;
	switch(lplvcd->nmcd.dwDrawStage)
	{
	case CDDS_PREPAINT:
		{
			*pResult = CDRF_NOTIFYITEMDRAW;
			return;
		}
	case CDDS_ITEMPREPAINT:
		{
			int nItem = static_cast<int>(lplvcd->nmcd.dwItemSpec);
			COLORREF clrNewTextColor = RGB(255,0,0);
			if(m_lineInfos.find(nItem) != m_lineInfos.end())
			{
				if(m_lineInfos[nItem])
					clrNewTextColor = RGB(0,0,255);
				else
					clrNewTextColor = RGB(255,0,255);
			}
			COLORREF clrNewBkColor = RGB(255,255,255);
			lplvcd->clrText = clrNewTextColor;
			lplvcd->clrTextBk = clrNewBkColor;
			*pResult = CDRF_DODEFAULT;
			return;
		}
	}
}

void CClientTryLoginDlg::OnTimer(UINT nIDEvent) 
{
	if(m_TestCount == 0)
	{
		KillTimer(1);
		MessageBox(_T("没有任何服务器需要测试！"),_T("自动登陆"));
		EnablePanel();
		CDialog::OnTimer(nIDEvent);
		return;
	}

	if( IsAllUserCooldown() )
		m_Warning.ShowWindow(SW_SHOW);
	else
		m_Warning.ShowWindow(SW_HIDE);

	size_t count = m_collector.GetResultCount();
	if(count)
	{
		std::ofstream f("server_error_report.txt");
		f << _T("服务器组名	服务器地址	服务器端口	服务器信息\n");

		bool bFind;
		size_t i;
		// 收集结果。
		for(i = 0;i < count;++i)
		{
			ConnectResult cr;
			m_collector.GetResult(i, cr);
			AWString addressIP = AS2WC(cr.address);

			bFind = false;
			ServerInfo::iterator it = m_serverInfo.begin(), itEnd = m_serverInfo.end();
			for(;it != itEnd;++it)
			{
				AreaServers& as = it->second;
			
				for(size_t a = 0;a < as.size();++a)
				{
					// m_nPortTestLoopCount大于0时表示已进入到端口测试阶段，
					// 为了将ConnectResult添加到虚拟的服务器组"跨服端口测试循环%d"中而不是真实服务器组中，
					// 需要判断ConnectResult的dest_port与AreaServers的destPort是否相同
					// 
					bool bDestServerPortValid = m_nPortTestLoopCount ? as[a].destPort == cr.dest_port : true; 
					// 当受到result不是跨服状态时，需要判断登陆端口是否合法
					bool bSourceServerPortValid = (cr.is_on_cross || m_nPortTestLoopCount) ? 
						true : cr.port >= as[a].serverPortStart && cr.port < as[a].serverPortEnd;
					if(addressIP.CompareNoCase(as[a].serverIP) == 0 && bDestServerPortValid && bSourceServerPortValid)
					{
						as[a].cResults.push_back(cr);
						
						if(IsTestAllRight(cr.result))
							as[a].bHaveSuccess = true;
						else
							as[a].bHaveFail = true;
						bFind = true;
						break;
					}
				}

				if(bFind) break;
			}
		}
		m_collector.ClearResults(0, count);	

		int progressCount = 0;
		// 显示结果
		// 清空
		m_successInfoCtrl.DeleteAllItems();
		m_failInfoCtrl.DeleteAllItems();
		m_lineInfos.clear();

		char buf[100];
		bool bHaveSuccess;
		bool bHaveFail;
		ServerInfo::iterator it = m_serverInfo.begin(), itEnd = m_serverInfo.end();
		for(;it != itEnd;++it)
		{
			AreaServers& as = it->second;
			bHaveSuccess = false;			
			bHaveFail = false;
			for(i = 0;i < as.size();++i)
			{
				if(as[i].bHaveSuccess) bHaveSuccess = true;
				if(as[i].bHaveFail) bHaveFail = true;
				if(bHaveSuccess && bHaveFail) break;
			}

			if(bHaveSuccess)
			{
				CListCtrl& listCtrl = m_successInfoCtrl;

				int n = listCtrl.GetItemCount();
				// 插入Group
				AString groupName = WC2AS(it->first);
				listCtrl.InsertItem(n,groupName,0);
				// 插入服务器测试信息

				for(i = 0;i < as.size();++i)
				{
					if(as[i].bHaveSuccess)
					{
						for(size_t a = 0;a < as[i].cResults.size();++a)
						{
							const ConnectResult & cr = as[i].cResults[a];
							if(IsTestAllRight(cr.result))
							{
								// 插入服务器名称
								n = listCtrl.GetItemCount();
								AString temp = WC2AS(as[i].serverName);
								listCtrl.InsertItem(n,temp,0);
								// 设置服务器IP
								temp = WC2AS(as[i].serverIP);
								listCtrl.SetItemText(n,1,temp);
								// 设置服务器Port
								sprintf( buf, "%d", cr.port);
								listCtrl.SetItemText(n,2,buf);
								// 设置服务器成功信息
								listCtrl.SetItemText(n,3,_T("登陆成功！")+cr.queryResult);
								++progressCount;
							}
						}
						
					}
				}
			}

			if(bHaveFail)
			{
				CListCtrl& listCtrl = m_failInfoCtrl;
				
				int n = listCtrl.GetItemCount();
				// 插入Group
				AString groupName = WC2AS(it->first);
				listCtrl.InsertItem(n,groupName,0);
				// 插入服务器测试信息
				
				for(i = 0;i < as.size();++i)
				{
					if(as[i].bHaveFail)
					{
						for(size_t a = 0;a < as[i].cResults.size();++a)
						{
							const ConnectResult & cr = as[i].cResults[a];
							if(!IsTestAllRight(cr.result))
							{
								// 插入服务器名称
								n = listCtrl.GetItemCount();
								if(IsTestError(cr.result))
								{
									m_lineInfos[n] = true;
								}
								else if(IsTestInWorldWithError(cr.result))
								{
									m_lineInfos[n] = false;
								}

								AString temp = WC2AS(as[i].serverName);
								listCtrl.InsertItem(n,temp,0);
								std::string s(temp);
								f << s.c_str() << "	";
								// 设置服务器IP
								temp = WC2AS(as[i].serverIP);
								listCtrl.SetItemText(n,1,temp);
								s = temp;
								f << s.c_str() << "	";
								// 设置服务器Port
								sprintf( buf, "%d", cr.port);
								listCtrl.SetItemText(n,2,buf);
								s = buf;
								f << s.c_str() << "	";
								// 设置服务器失败信息
								AString fInfo = cr.failReason;
								if( IsTestInWorld(cr.result))
									fInfo = _T("登陆成功！") + cr.queryResult;
								if(IsTestVersionErr(cr.result))
									fInfo = cr.versionErr + _T(",") + fInfo;
								listCtrl.SetItemText(n,3,fInfo);
								listCtrl.SetItemData(n, (as[i].serverZoneID | as[i].serverLine << 16));
								s = fInfo;
								f << s.c_str() << std::endl;
								++progressCount;
							}
						}
						
					}
				}
			}

		}
		f.close();
		// 设置进度条
		m_progressCtrl.SetPos(progressCount * 100 / m_TestCount);
		
		sprintf( buf, "%d/%d", progressCount,m_TestCount);
		m_serverNum = buf;

		UpdateData(false);

		if (IsTestCrossServer())
		{
			if(progressCount >= m_TestCount)
			{
				TestCrossServerLeft();
			}
		}
		else
		{	
			// 全部完成测试
			if(progressCount >= m_TestCount)
			{
				KillTimer(1);
				MessageBox(_T("所有服务器测试完成！"),_T("自动登陆"));
				EnablePanel();
			}
		}
	}

	
	CDialog::OnTimer(nIDEvent);
}

void CClientTryLoginDlg::OnButtonOpenError() 
{
	::ShellExecute(this->GetSafeHwnd(),"open","server_error_report.txt",0,0,SW_SHOWNORMAL);
}

void CClientTryLoginDlg::OnButtonAddaccount() 
{
	UpdateData();
	TCHAR info[MAX_PATH];
	TCHAR temp[MAX_PATH];
	// 先保存到ini文件中。

	if(m_userName.IsEmpty() || m_pwd.IsEmpty())
	{
		MessageBox(_T("账号或者密码不能为空！"),_T("账号"),MB_ICONERROR);
		return;
	}

	if( IsTestCrossServer() )
	{
		// 这里使用GM账号，为了安全不保存
		AccountInfo acc;
		acc.userName = m_userName;
		acc.pwd = m_pwd;
		m_accountInfos.push_back(acc);

		// 插入用户账号
		int n = m_accountCtrl.GetItemCount();
		m_accountCtrl.InsertItem(n, acc.userName, 0);
	}
	else
	{
		// username and password
		unsigned int userCount = ::GetPrivateProfileInt(_T("info"),_T("userCount"),0,_T("info.ini"));
		userCount++;
		sprintf( temp, "%d", userCount);
		::WritePrivateProfileString(_T("info"),_T("userCount"),temp,_T("info.ini"));
		sprintf( temp, "userName%d", userCount);
		::WritePrivateProfileString(_T("info"),temp,m_userName,_T("info.ini"));
		sprintf( temp, "password%d", userCount);
		strcpy(info,m_pwd);
		for(int i = 0; i < MAX_PATH;i++)
			info[i] = ~info[i];

		CString pkey = temp;
		Base64Code bc64;
		bc64.Encode(info,temp);
		::WritePrivateProfileString(_T("info"),pkey,temp,_T("info.ini"));
		
		ReLoadAccount();
	}
}

void CClientTryLoginDlg::OnButtonDelaccount() 
{
	if(m_accountCtrl.GetSelectedCount() == 0)
	{
		MessageBox(_T("请在账号列表中选择需要删除的账号！"),_T("账号"),MB_ICONERROR);
		return;
	}
	
	POSITION pos = m_accountCtrl.GetFirstSelectedItemPosition();

	if( !IsTestCrossServer() )
	{
		CString userNameIni;

		while(pos)
		{
			int nItem   = m_accountCtrl.GetNextSelectedItem(pos);
			userNameIni = m_accountCtrl.GetItemText(nItem,1);

			// 直接置空
			::WritePrivateProfileString(_T("info"),userNameIni,"",_T("info.ini"));
		}
		
		ReLoadAccount();
	}
	else
	{
		while(pos)
		{
			int nItem = m_accountCtrl.GetNextSelectedItem(pos);
			m_accountInfos.erase(m_accountInfos.begin() + nItem);
			m_accountCtrl.DeleteItem(nItem);
		}
	}
}

HBRUSH CClientTryLoginDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
	
	if( pWnd->GetDlgCtrlID() == IDC_STATIC_HINT ||
		pWnd->GetDlgCtrlID() == IDC_WAIT_WARNING )
	{
		pDC->SetTextColor(RGB(255,0,0));
	}
	else if( pWnd->GetDlgCtrlID() == IDC_STATIC_VERSION)
	{
		pDC->SetTextColor(RGB(0,0,255));
	}

	// TODO: Return a different brush if the default is not desired
	return hbr;
}

void CClientTryLoginDlg::OnCancelMode() 
{
	CDialog::OnCancelMode();
	
	// TODO: Add your message handler code here
	
}

void CClientTryLoginDlg::OnNormalTest() 
{
	// TODO: Add your control notification handler code here
	ReLoadAccount();
	SetTestCrossServer(false);
}

void CClientTryLoginDlg::OnCrossTest() 
{
	// TODO: Add your control notification handler code here
	m_accountInfos.clear();
	m_accountCtrl.DeleteAllItems();
	SetTestCrossServer(true);
}

bool CClientTryLoginDlg::IsTestCrossServer()
{
	if( m_btnNormal.GetCheck() == BST_CHECKED )
		return false;
	else
		return true;
}

void CClientTryLoginDlg::DisablePanel()
{
	m_btnStart.EnableWindow(false);
	m_btnReStart.EnableWindow(false);
	m_btnNormal.EnableWindow(false);
	m_btnCrossServer.EnableWindow(false);
}

void CClientTryLoginDlg::EnablePanel()
{
	m_btnStart.EnableWindow(true);
	m_btnReStart.EnableWindow(true);
	m_btnNormal.EnableWindow(true);
	m_btnCrossServer.EnableWindow(true);
}

void CClientTryLoginDlg::TestCrossServerLeft()
{
	CECCrossServerList::OriServerInfoContainer vecOrgServerToTest;
	CECCrossServerList::GetSingleton().GetOrgAddressToTest(vecOrgServerToTest);
	if (!vecOrgServerToTest.empty())
	{
		m_nPortTestLoopCount++;
		AWString strGroup;
		strGroup.Format(L"跨服端口测试循环%d", m_nPortTestLoopCount);
		// 添加一组虚拟的服务器组，用来测试跨服服务器剩余端口
		AreaServers as;
		m_serverInfo[strGroup] = as;
		
		// 查找能跨到待测中央服的原服信息，并添加到虚拟服务器组中
		CECCrossServerList::OriServerInfoContainer::iterator iter = vecOrgServerToTest.begin();
		for (; iter != vecOrgServerToTest.end(); ++iter)
		{
			ServerInfo::iterator iterServerInfo = m_serverInfo.begin();
			for (; iterServerInfo != m_serverInfo.end(); ++iterServerInfo)
			{
				AreaServers& as = iterServerInfo->second;
				bool bFind = false;
				if (iterServerInfo->first != strGroup)
				{
					for (AreaServers::iterator it = as.begin(); it != as.end(); ++it)
					{
						AString strIP = WC2AS(it->serverIP);
						if (strIP == iter->address && it->serverLine == iter->line)
						{
							OneServerInfo info;
							info.bHaveFail = false;
							info.bHaveSuccess = false;
							info.serverIP = it->serverIP;
							info.serverLine = it->serverLine;
							info.serverName = it->serverName;
							info.serverPortEnd = it->serverPortEnd;
							info.serverPortStart = it->serverPortStart;
							info.serverZoneID = it->serverZoneID;
							info.destPort = iter->dest_port;
							m_serverInfo[strGroup].push_back(info);
							++m_TestCount;
							bFind = true;
							break;
						}
					}
				}
				if (bFind) break;
			}
		}
					
		for (iter = vecOrgServerToTest.begin(); iter != vecOrgServerToTest.end(); ++iter)
		{
			for(int i = 0;i < (int)m_LoginVector.size();++i)
			{
				if(iter->address == m_LoginVector[i].address && iter->line == m_LoginVector[i].line)
				{
					LoginServer(m_LoginVector[i].zone_id, m_LoginVector[i].line, m_LoginVector[i].address,m_LoginVector[i].port);
					break;
				}
			}
		}
	}
	else
	{
		KillTimer(1);
		MessageBox(_T("所有服务器测试完成！"),_T("自动登陆"));
		EnablePanel();
	}
}
