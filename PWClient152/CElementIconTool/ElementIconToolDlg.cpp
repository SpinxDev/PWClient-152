// ElementIconToolDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ElementIconTool.h"
#include "ElementIconToolDlg.h"
#include "EC_Global.h"
#include "../CCommon/itemdataman.h"
#include "../CCommon/elementdataman.h"
#include <A3D.h>
#include <AFI.h>
#include <ALog.h>
#include <afxdlgs.h>
#include <AFileImage.h>
#include <png.h>
#include "ElementSkill.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define new A_DEBUG_NEW


//	Equipment endurance scale
#define ENDURANCE_SCALE		100

void set_to_classid(DATA_TYPE type, item_data * data, int major_type)
{
}
int addon_generate_arg(DATA_TYPE type, addon_data & data, int arg_num/*初始的参数个数*/)
{
	return arg_num;
} 
int addon_update_ess_data(const addon_data & data, void * essence,size_t ess_size, prerequisition * require)
{
	return 0;
}
void update_require_data(prerequisition *require)
{
	require->durability *= ENDURANCE_SCALE;
	require->max_durability *= ENDURANCE_SCALE;
}
void get_item_guid(int id, int& g1, int& g2)
{
	g1 = 0;
	g2 = 1;
}

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
// CElementIconToolDlg dialog

CElementIconToolDlg::CElementIconToolDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CElementIconToolDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CElementIconToolDlg)
	m_path = _T("");
	m_path2 = _T("");
	m_path3 = _T("");
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	m_pA3DDevice = NULL;
	m_pA3DEngine = NULL;
	m_pElementDataMan = NULL;
	m_pItemDataMan = NULL;
	m_dtTemp = DT_INVALID;
	m_idTemp = 0;
	m_idSkill = 0;
}

void CElementIconToolDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CElementIconToolDlg)
	DDX_Text(pDX, IDC_EDIT1, m_path);
	DDX_Text(pDX, IDC_EDIT2, m_path2);
	DDX_Text(pDX, IDC_EDIT3, m_path3);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CElementIconToolDlg, CDialog)
	//{{AFX_MSG_MAP(CElementIconToolDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_BUTTON2, OnButton2)
	ON_BN_CLICKED(IDC_BUTTON1, OnButton1)
	ON_EN_CHANGE(IDC_EDIT1, OnChangeEdit1)
	ON_BN_CLICKED(IDC_BUTTON3, OnButton3)
	ON_BN_CLICKED(IDC_BUTTON4, OnButton4)
	ON_BN_CLICKED(IDC_BUTTON5, OnButton5)
	ON_BN_CLICKED(IDC_BUTTON6, OnButton6)
	//}}AFX_MSG_MAP
	ON_EN_CHANGE(IDC_EDIT3, OnEnChangeEdit3)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CElementIconToolDlg message handlers

BOOL CElementIconToolDlg::OnInitDialog()
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
	
	// TODO: Add extra initialization here
	if (!Init())
		return FALSE;
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CElementIconToolDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CElementIconToolDlg::OnPaint() 
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
HCURSOR CElementIconToolDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

bool CElementIconToolDlg::InitA3DEngine()
{
	//	Init Engine
	if (!(m_pA3DEngine = new A3DEngine()))
	{
		a_LogOutput(1, "InitA3DEngine: Not enough memory!");
		return false;
	}

	HINSTANCE hInst = AfxGetInstanceHandle();
	HWND hWnd = GetSafeHwnd();
	
	RECT Rect;
	::GetClientRect(hWnd, &Rect);
		
	A3DDEVFMT devFmt;
	devFmt.bWindowed	= true;
	devFmt.nWidth		= Rect.right;
	devFmt.nHeight		= Rect.bottom;
	devFmt.fmtTarget	= A3DFMT_UNKNOWN;
	devFmt.fmtDepth		= A3DFMT_UNKNOWN;
	
	DWORD dwDevFlags = A3DDEV_ALLOWMULTITHREAD | A3DDEV_FORCESOFTWARETL;	//	Force software T&L
	
	if (!m_pA3DEngine->Init(hInst, hWnd, &devFmt, dwDevFlags))
	{
		A3DRELEASE(m_pA3DEngine);
		a_LogOutput(1, "InitA3DEngine: Failed to initialize A3D engine.");
		return false;
	}
	
	m_pA3DDevice = m_pA3DEngine->GetA3DDevice();
	return true;
}

void CElementIconToolDlg::ReleaseA3DEngine()
{
	A3DRELEASE(m_pA3DEngine);
}

bool CElementIconToolDlg::Init()
{
	af_Initialize();
	glb_InitLogSystem(GetLogFileName());
	char szWorkDir[MAX_PATH];
	GetCurrentDirectoryA(MAX_PATH, szWorkDir);
	af_SetBaseDir(szWorkDir);

	if (!InitA3DEngine())
		return false;
	return true;
}

void CElementIconToolDlg::Release()
{
	ReleaseA3DEngine();
	glb_CloseLogSystem();	
	af_Finalize();
}

void CElementIconToolDlg::OnDestroy() 
{
	Release();
	CDialog::OnDestroy();
}

void CElementIconToolDlg::OnChangeEdit1() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	
}

int CALLBACK BrowseCallback(HWND hWnd, UINT uMsg, LPARAM lParam, LPARAM lpData)
{
	switch (uMsg)
	{
	case BFFM_INITIALIZED:
		::SendMessage(hWnd, BFFM_SETSELECTION, 1, lpData);
		break;
	}
	return 0;
}

void CElementIconToolDlg::OnButton1() 
{
	// 浏览原图标文件
	CFileDialog dlg(true, _T("*.dds"), (LPCTSTR)m_path, OFN_FILEMUSTEXIST, _T("DDS Files(*.dds)|*.dds|All Files(*.*)|*.*||"));
	if (dlg.DoModal() != IDOK)
		return;
	m_path = dlg.GetPathName();
	UpdateData(FALSE);
}

void CElementIconToolDlg::OnButton2() 
{
	if (m_path.IsEmpty())
	{
		CEdit *pEdit = (CEdit*)GetDlgItem(IDC_EDIT1);
		pEdit->SetFocus();
		pEdit->SetSel(0, -1);
		return;
	}
	if (m_path2.IsEmpty())
	{
		CEdit *pEdit = (CEdit*)GetDlgItem(IDC_EDIT2);
		pEdit->SetFocus();
		pEdit->SetSel(0, -1);
		return;
	}

	ACString strPath(m_path);
	AString strOutputDir;
	if (ConvertItemIcon(strOutputDir))
	{
		AfxMessageBox(_T("转换结束!"), MB_ICONINFORMATION | MB_OK);

		// 打开输出目录用于查看
		ShellExecuteA(NULL, "open", strOutputDir, NULL, NULL, SW_SHOWNORMAL);
	}
	else
	{
		AfxMessageBox(_T("转换失败，请查看日志文件！"), MB_ICONERROR | MB_OK);
	}
	
	// 打开日志文件用于查看
	OpenLogFile();
}


bool CElementIconToolDlg::ConvertItemIcon(AString &strOutputDir)
{
	bool ret(false);

	// 验证输入
	if (!m_pA3DEngine)
		return ret;

	AString strPath = AC2AS(m_path);
	AString strPathDDS = strPath;
	if (!af_IsFileExist(strPath) || !af_CheckFileExt(strPath, ".dds"))
	{
		a_LogOutput(1, "CElementIconToolDlg::ConvertItemIcon, Invalid icon file %s", (const char *)strPath);
		return ret;
	}
	if (!af_ChangeFileExt(strPath, ".txt") || !af_IsFileExist(strPath))
	{
		a_LogOutput(1, "CElementIconToolDlg::ConvertItemIcon, Cannot find icon description file %s", (const char *)strPath);
		return ret;
	}
	AString strPathTXT = strPath;
	
	// 创建输出目录
	AString strDirOut = strPathDDS;
	strDirOut.CutRight(4); // 除去后缀名，形成跟文件相关的目录
	BOOL bCreate = ::CreateDirectoryA(strDirOut, NULL);
	if (bCreate == 0)
	{
		 DWORD dwError = GetLastError();
		 if (dwError != ERROR_ALREADY_EXISTS)
		 {
			 a_LogOutput(1, "CElementIconToolDlg::ConvertItemIcon, Failed to create output directory %s", (const char *)strDirOut);
			 return ret;
		 }
	}

	// 读取图标描述文件 iconlist_ivtrm.txt
	AFileImage fi;
	if (!fi.Open(strPathTXT, AFILE_OPENEXIST | AFILE_TEXT))
	{
		a_LogOutput(1, "CElementIconToolDlg::ConvertItemIcon, Cannot open icon description file %s", (const char *)strPathTXT);
		return ret;
	}
	typedef abase::hash_map<AString, int> IconMap;
	IconMap iconMap;
	int W(0), H(0), nCountX(0), nCountY(0);
	char szLine[AFILE_LINEMAXLEN];
	DWORD dwRead;
	fi.ReadLine(szLine, sizeof(szLine), &dwRead);
	W = atoi(szLine);
	
	fi.ReadLine(szLine, sizeof(szLine), &dwRead);
	H = atoi(szLine);
	
	fi.ReadLine(szLine, sizeof(szLine), &dwRead);
	nCountY = atoi(szLine);
	
	fi.ReadLine(szLine, sizeof(szLine), &dwRead);
	nCountX = atoi(szLine);
	AString strLine;
	for (int i = 0; i < nCountY; i++ )
	{
		for(int j = 0; j < nCountX; j++ )
		{
			int nIndex = i * nCountX + j;
			fi.ReadLine(szLine, sizeof(szLine), &dwRead);
			if (dwRead > 0 && strlen(szLine) > 0)
			{
				strLine = szLine;
				strLine.MakeLower();
				iconMap[strLine] = nIndex;
			}
		}
	}	
	fi.Close();

	// 读取图标文件 iconlist_ivtrm.dds
	A3DSurfaceMan *pSurfaceMan = m_pA3DEngine->GetA3DSurfaceMan();
	A3DSurface *pSurfaceSrc = NULL;
	if (!pSurfaceMan->LoadSurfaceFromFile(strPathDDS, 0, &pSurfaceSrc))
	{
		a_LogOutput(1, "CElementIconToolDlg::ConvertItemIcon, Cannot open icon file %s", (const char *)strPathDDS);
		return ret;
	}
#ifdef _DEBUG
	{
		AString strTemp = strPathDDS;
		strTemp.CutRight(3);
		strTemp += "bmp";
		D3DXSaveSurfaceToFile(AS2AC(strTemp), D3DXIFF_BMP, pSurfaceSrc->GetDXSurface(), NULL, NULL);
	}
#endif

	// 逐个文件转换为 PNG 格式
	A3DSurface *pSurfaceDst = new A3DSurface;
	while (true)
	{
		if (!pSurfaceDst->Create(m_pA3DEngine->GetA3DDevice(), W, H, A3DFMT_X8R8G8B8))
		{
			a_LogOutput(1, "CElementIconToolDlg::ConvertItemIcon, Cannot create temp surface for small icons");
			break;
		}	
		
		// 读取 elements.data 文件
		if (!LoadElementData())
		{
			break;
		}

		AString strIconIndex, strIconIndexTitle, strIconOut;
		if (GetFirstItemIconFilePath(strIconIndex, strIconOut))
		{
			do
			{
				// 获取下标
				af_GetFileTitle(strIconIndex, strIconIndexTitle);
				strIconIndexTitle.MakeLower();
				IconMap::iterator it = iconMap.find(strIconIndexTitle);
				int nIndex = 0;
				if (it != iconMap.end())
				{
					nIndex = it.value()->second;
				} 
				else
				{
					// 无法查到下标，使用默认项
					a_LogOutput(1, "CElementIconToolDlg::ConvertItemIcon, Invalid icon path in elements.data: id = %d, icon = %s", m_idTemp, strIconIndex);
					nIndex = 0;
				}

				// 根据下标计算小图片在大图片中的位置
				int i = nIndex/nCountX;
				int j = nIndex%nCountX;
				RECT rect;			
				rect.left = j * W;
				rect.top =  i * H;
				rect.right = j * W + W;
				rect.bottom = i * H + H;

				// 计算
				strIconOut = strDirOut + "\\" + strIconOut;
				POINT pt = {0, 0};
				if (D3D_OK != D3DXLoadSurfaceFromSurface(pSurfaceDst->GetDXSurface(), NULL, NULL, pSurfaceSrc->GetDXSurface(), NULL, &rect, D3DX_DEFAULT, 0))
				{
					a_LogOutput(1, "CElementIconToolDlg::ConvertItemIcon, Failed to make icon image");
					break;
				}
				if (!SaveSurfaceToFile(strIconOut, pSurfaceDst))
				{
					a_LogOutput(1, "CElementIconToolDlg::ConvertItemIcon, Failed to save icon to file %s", (const char *)strIconOut);
					break;
				}

			} while(GetNextItemIconFilePath(strIconIndex, strIconOut));
		}

		ret = true;
		break;
	}

	pSurfaceMan->ReleaseSurface(pSurfaceDst);
	pSurfaceMan->ReleaseSurface(pSurfaceSrc);
	ClearElementData();

	if (ret)
		strOutputDir = strDirOut;
	return ret;
}

bool CElementIconToolDlg::ConvertSkillIcon(AString &strOutputDir)
{
	bool ret(false);

	// 验证输入
	if (!m_pA3DEngine)
		return ret;

	AString strPath = AC2AS(m_path3);
	AString strPathDDS = strPath;
	if (!af_IsFileExist(strPath) || !af_CheckFileExt(strPath, ".dds"))
	{
		a_LogOutput(1, "CElementIconToolDlg::ConvertSkillIcon, Invalid icon file %s", (const char *)strPath);
		return ret;
	}
	if (!af_ChangeFileExt(strPath, ".txt") || !af_IsFileExist(strPath))
	{
		a_LogOutput(1, "CElementIconToolDlg::ConvertSkillIcon, Cannot find icon description file %s", (const char *)strPath);
		return ret;
	}
	AString strPathTXT = strPath;
	
	// 创建输出目录
	AString strDirOut = strPathDDS;
	strDirOut.CutRight(4); // 除去后缀名，形成跟文件相关的目录
	BOOL bCreate = ::CreateDirectoryA(strDirOut, NULL);
	if (bCreate == 0)
	{
		 DWORD dwError = GetLastError();
		 if (dwError != ERROR_ALREADY_EXISTS)
		 {
			 a_LogOutput(1, "CElementIconToolDlg::ConvertSkillIcon, Failed to create output directory %s", (const char *)strDirOut);
			 return ret;
		 }
	}

	// 读取图标描述文件 iconlist_skill.txt
	AFileImage fi;
	if (!fi.Open(strPathTXT, AFILE_OPENEXIST | AFILE_TEXT))
	{
		a_LogOutput(1, "CElementIconToolDlg::ConvertSkillIcon, Cannot open icon description file %s", (const char *)strPathTXT);
		return ret;
	}
	typedef abase::hash_map<AString, int> IconMap;
	IconMap iconMap;
	int W(0), H(0), nCountX(0), nCountY(0);
	char szLine[AFILE_LINEMAXLEN];
	DWORD dwRead;
	fi.ReadLine(szLine, sizeof(szLine), &dwRead);
	W = atoi(szLine);
	
	fi.ReadLine(szLine, sizeof(szLine), &dwRead);
	H = atoi(szLine);
	
	fi.ReadLine(szLine, sizeof(szLine), &dwRead);
	nCountY = atoi(szLine);
	
	fi.ReadLine(szLine, sizeof(szLine), &dwRead);
	nCountX = atoi(szLine);
	AString strLine;
	for (int i = 0; i < nCountY; i++ )
	{
		for(int j = 0; j < nCountX; j++ )
		{
			int nIndex = i * nCountX + j;
			fi.ReadLine(szLine, sizeof(szLine), &dwRead);
			if (dwRead > 0 && strlen(szLine) > 0)
			{
				strLine = szLine;
				strLine.MakeLower();
				iconMap[strLine] = nIndex;
			}
		}
	}	
	fi.Close();

	// 读取图标文件 iconlist_ivtrm.dds
	A3DSurfaceMan *pSurfaceMan = m_pA3DEngine->GetA3DSurfaceMan();
	A3DSurface *pSurfaceSrc = NULL;
	if (!pSurfaceMan->LoadSurfaceFromFile(strPathDDS, 0, &pSurfaceSrc))
	{
		a_LogOutput(1, "CElementIconToolDlg::ConvertSkillIcon, Cannot open icon file %s", (const char *)strPathDDS);
		return ret;
	}
#ifdef _DEBUG
	{
		AString strTemp = strPathDDS;
		strTemp.CutRight(3);
		strTemp += "bmp";
		D3DXSaveSurfaceToFile(AS2AC(strTemp), D3DXIFF_BMP, pSurfaceSrc->GetDXSurface(), NULL, NULL);
	}
#endif

	// 逐个文件转换为 PNG 格式
	A3DSurface *pSurfaceDst = new A3DSurface;
	while (true)
	{
		if (!pSurfaceDst->Create(m_pA3DEngine->GetA3DDevice(), W, H, A3DFMT_X8R8G8B8))
		{
			a_LogOutput(1, "CElementIconToolDlg::ConvertSkillIcon, Cannot create temp surface for small icons");
			break;
		}
		
		// 技能数据存于dll中，已经加载

		AString strIconIndex, strIconIndexTitle, strIconOut;
		if (GetFirstSkillIconFilePath(strIconIndex, strIconOut))
		{
			do
			{
				// 获取下标
				af_GetFileTitle(strIconIndex, strIconIndexTitle);
				strIconIndexTitle.MakeLower();
				IconMap::iterator it = iconMap.find(strIconIndexTitle);
				int nIndex = 0;
				if (it != iconMap.end())
				{
					nIndex = it.value()->second;
				} 
				else
				{
					// 无法查到下标，使用默认项
					a_LogOutput(1, "CElementIconToolDlg::ConvertSkillIcon, Invalid icon path in skill: id = %d, icon = %s", m_idSkill, strIconIndex);
					nIndex = 0;
				}

				// 根据下标计算小图片在大图片中的位置
				int i = nIndex/nCountX;
				int j = nIndex%nCountX;
				RECT rect;			
				rect.left = j * W;
				rect.top =  i * H;
				rect.right = j * W + W;
				rect.bottom = i * H + H;

				// 计算
				strIconOut = strDirOut + "\\" + strIconOut;
				POINT pt = {0, 0};
				if (D3D_OK != D3DXLoadSurfaceFromSurface(pSurfaceDst->GetDXSurface(), NULL, NULL, pSurfaceSrc->GetDXSurface(), NULL, &rect, D3DX_DEFAULT, 0))
				{
					a_LogOutput(1, "CElementIconToolDlg::ConvertSkillIcon, Failed to make icon image");
					break;
				}
				if (!SaveSurfaceToFile(strIconOut, pSurfaceDst))
				{
					a_LogOutput(1, "CElementIconToolDlg::ConvertSkillIcon, Failed to save icon to file %s", (const char *)strIconOut);
					break;
				}

			} while(GetNextSkillIconFilePath(strIconIndex, strIconOut));
		}

		ret = true;
		break;
	}

	pSurfaceMan->ReleaseSurface(pSurfaceDst);
	pSurfaceMan->ReleaseSurface(pSurfaceSrc);

	if (ret)
		strOutputDir = strDirOut;
	return ret;
}

bool CElementIconToolDlg::SaveSurfaceToFile(const char *szIconFile, A3DSurface *pSurfaceDst)
{
	bool ret(false);

	png_bytep * row_pointers = NULL;
	while (pSurfaceDst)
	{
		// Lock reloaded bmp files and get image data out
		void *pBits = NULL;
		int iPitch = 0;
		if (!pSurfaceDst->LockRect(NULL, &pBits, &iPitch, D3DLOCK_READONLY))
			break;

		int W = pSurfaceDst->GetWidth();
		int H = pSurfaceDst->GetHeight();

		png_bytep* raw_pointers = new png_bytep[H];
		for (int y = 0; y < H; ++y)
		{
			png_bytep pDst = new png_byte[W*3];
			raw_pointers[y] = pDst;
			char * pSrc = (char *)pBits + y*iPitch;
			for (int x = 0; x < W; ++x)
			{
				pDst[x*3] = pSrc[x*4+2];
				pDst[x*3+1] = pSrc[x*4+1];
				pDst[x*3+2] = pSrc[x*4];
			}
		}
		pSurfaceDst->UnlockRect();

		// Write image data to PNG using libpng
		bool bWrite(false);
		while (true)
		{			
			/* create file */
			FILE *fp = fopen(szIconFile, "wb");
			if (!fp)
			{
				a_LogOutput(1, "CElementIconToolDlg::SaveSurfaceToFile, File %s could not be opened for writing", szIconFile);
				break;
			}			
			
			bool temp(false);
			png_structp png_ptr = NULL;
			png_infop info_ptr = NULL;
			while (true)
			{
				/* initialize stuff */
				png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
				
				if (!png_ptr)
					break;
				
				info_ptr = png_create_info_struct(png_ptr);
				if (!info_ptr)
					break;
				
				if (setjmp(png_jmpbuf(png_ptr)))
					break;
				
				png_init_io(png_ptr, fp);				
				
				/* write header */
				if (setjmp(png_jmpbuf(png_ptr)))
					break;
				
				png_set_IHDR(png_ptr, info_ptr,
					W, H, 8, PNG_COLOR_TYPE_RGB, PNG_INTERLACE_NONE,
					PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);
				
				png_write_info(png_ptr, info_ptr);			
				
				/* write bytes */
				if (setjmp(png_jmpbuf(png_ptr)))
					break;
				
				png_write_image(png_ptr, raw_pointers);			
				
				/* end write */
				if (setjmp(png_jmpbuf(png_ptr)))
					break;
				
				png_write_end(png_ptr, NULL);
				temp = true;
				break;
			}
			if (png_ptr)
			{
				png_destroy_write_struct(&png_ptr, &info_ptr);
			}
			fclose(fp);

			if (!temp)
			{
				a_LogOutput(1, "CElementIconToolDlg::SaveSurfaceToFile, File %s writing error", szIconFile);
				break;
			}
			bWrite = true;
			break;
		}
		
		for (int x = 0; x < W; ++ x)
			delete []raw_pointers[x];
		delete []raw_pointers;

		if (!bWrite)
			break;

		ret = true;
		break;
	}

	return ret;
}

void CElementIconToolDlg::OnButton3() 
{
	// 浏览原图标文件
	CFileDialog dlg(true, _T("*.data"), (LPCTSTR)m_path2, OFN_FILEMUSTEXIST, _T("DATA Files(*.data)|*.data|All Files(*.*)|*.*||"));
	if (dlg.DoModal() != IDOK)
		return;
	m_path2 = dlg.GetPathName();
	UpdateData(FALSE);
}

bool CElementIconToolDlg::LoadElementData()
{
	bool bLoad = false;

	// 删除旧的elements.data
	ClearElementData();

	// 尝试加载新的

	CString strElement = m_path2;
	if (!strElement.IsEmpty())
	{
		if (af_IsFileExist(AC2AS(strElement)))
		{
			m_pItemDataMan = new itemdataman();
			//	now load templates from file
			if (0 != m_pItemDataMan->load_data(AC2AS((LPCTSTR)strElement)))
			{
				// 加载失败
				a_LogOutput(1, "CElementIconToolDlg::LoadElementData, Cannot load elements.data file %s", AC2AS(strElement));
				delete m_pItemDataMan;
			}
			else
			{
				// 加载成功
				m_pElementDataMan = m_pItemDataMan->GetElementDataMan();
				bLoad = true;
			}
		}
	}

	return bLoad;
}

void CElementIconToolDlg::ClearElementData()
{	
	if( m_pItemDataMan )
	{
		delete m_pItemDataMan;
		m_pItemDataMan = NULL;
		m_pElementDataMan = NULL;
	}
}

bool CElementIconToolDlg::GetFirstItemIconFilePath(AString &strIconIndex, AString &strIconOut)
{
	m_dtTemp = DT_INVALID;
	m_idTemp = m_pElementDataMan->get_first_data_id(ID_SPACE_ESSENCE, m_dtTemp);
	while (m_idTemp)
	{
		GetItemIconFilePath(strIconIndex, strIconOut);
		if (!strIconOut.IsEmpty())
		{
			// 查找到正确的
			break;
		}
		// 查找的不是有效物品，查找下一个
		m_idTemp = m_pElementDataMan->get_next_data_id(ID_SPACE_ESSENCE, m_dtTemp);
	}
	return m_idTemp != 0 && m_dtTemp != DT_INVALID;
}

bool CElementIconToolDlg::GetNextItemIconFilePath(AString &strIconIndex, AString &strIconOut)
{
	m_idTemp = m_pElementDataMan->get_next_data_id(ID_SPACE_ESSENCE, m_dtTemp);	
	while (m_idTemp)
	{
		GetItemIconFilePath(strIconIndex, strIconOut);
		if (!strIconOut.IsEmpty())
		{
			// 查找到正确的
			break;
		}
		// 查找的不是有效物品，查找下一个
		m_idTemp = m_pElementDataMan->get_next_data_id(ID_SPACE_ESSENCE, m_dtTemp);
	}
	return m_idTemp != 0 && m_dtTemp != DT_INVALID;
}

void CElementIconToolDlg::GetItemIconFilePath(AString &strIconIndex, AString &strIconOut)
{
#undef GET_ITEM_INFO
#undef GET_ITEM_INFO_1

#define GET_ITEM_INFO(essence) \
	case DT_##essence: \
	{ \
		const essence * pEssence = (const essence *)m_pElementDataMan->get_data_ptr(m_idTemp, ID_SPACE_ESSENCE, m_dtTemp); \
		if (pEssence) \
		{ \
		strIconIndex = pEssence->file_icon; \
		strIconOut.Format("%d.png", pEssence->id);	\
		} \
	}\
	break;


#define GET_ITEM_INFO_1(essence) \
	case DT_##essence: \
	{ \
		const essence * pEssence = (const essence *)m_pElementDataMan->get_data_ptr(m_idTemp, ID_SPACE_ESSENCE, m_dtTemp); \
		if (pEssence) \
		{ \
		strIconIndex = pEssence->file_icon1; \
		strIconOut.Format("%d.png", pEssence->id);	\
		} \
	}\
	break;

	strIconIndex.Empty();
	strIconOut.Empty();
	if (m_idTemp != 0 && m_dtTemp != DT_INVALID)
	{
		switch(m_dtTemp)
		{
			GET_ITEM_INFO(WEAPON_ESSENCE)
			GET_ITEM_INFO(ARMOR_ESSENCE)
			GET_ITEM_INFO(DECORATION_ESSENCE)
			GET_ITEM_INFO(MEDICINE_ESSENCE)
			GET_ITEM_INFO(MATERIAL_ESSENCE)
			GET_ITEM_INFO(DAMAGERUNE_ESSENCE)
			GET_ITEM_INFO(ARMORRUNE_ESSENCE)
			GET_ITEM_INFO(SKILLTOME_ESSENCE)
			GET_ITEM_INFO(FLYSWORD_ESSENCE)
			GET_ITEM_INFO(WINGMANWING_ESSENCE)
			GET_ITEM_INFO(TOWNSCROLL_ESSENCE)
			GET_ITEM_INFO(UNIONSCROLL_ESSENCE)
			GET_ITEM_INFO(REVIVESCROLL_ESSENCE)
			GET_ITEM_INFO(ELEMENT_ESSENCE)
			GET_ITEM_INFO(TASKMATTER_ESSENCE)
			GET_ITEM_INFO(TOSSMATTER_ESSENCE)
			GET_ITEM_INFO(PROJECTILE_ESSENCE)
			GET_ITEM_INFO(QUIVER_ESSENCE)
			GET_ITEM_INFO(STONE_ESSENCE)
			GET_ITEM_INFO(TASKDICE_ESSENCE)
			GET_ITEM_INFO(TASKNORMALMATTER_ESSENCE)
			GET_ITEM_INFO(FASHION_ESSENCE)
			GET_ITEM_INFO(FACETICKET_ESSENCE)
			GET_ITEM_INFO(FACEPILL_ESSENCE)
			GET_ITEM_INFO(GM_GENERATOR_ESSENCE)
			GET_ITEM_INFO(PET_ESSENCE)		//	角色拍卖时显示宠物图标时需要
			GET_ITEM_INFO(PET_EGG_ESSENCE)
			GET_ITEM_INFO(PET_FOOD_ESSENCE)
			GET_ITEM_INFO(PET_FACETICKET_ESSENCE)
			GET_ITEM_INFO(FIREWORKS_ESSENCE)
			GET_ITEM_INFO(WAR_TANKCALLIN_ESSENCE)
			GET_ITEM_INFO(SKILLMATTER_ESSENCE)
			GET_ITEM_INFO(REFINE_TICKET_ESSENCE)
			GET_ITEM_INFO(DESTROYING_ESSENCE)
			GET_ITEM_INFO(BIBLE_ESSENCE)
			GET_ITEM_INFO(SPEAKER_ESSENCE)
			GET_ITEM_INFO(AUTOHP_ESSENCE)
			GET_ITEM_INFO(AUTOMP_ESSENCE)
			GET_ITEM_INFO(DOUBLE_EXP_ESSENCE)
			GET_ITEM_INFO(TRANSMITSCROLL_ESSENCE)
			GET_ITEM_INFO(DYE_TICKET_ESSENCE)			
			GET_ITEM_INFO_1(GOBLIN_ESSENCE)
			GET_ITEM_INFO(GOBLIN_EQUIP_ESSENCE)
			GET_ITEM_INFO(GOBLIN_EXPPILL_ESSENCE)
			GET_ITEM_INFO(SELL_CERTIFICATE_ESSENCE)
			GET_ITEM_INFO(TARGET_ITEM_ESSENCE)
			GET_ITEM_INFO(LOOK_INFO_ESSENCE)
			GET_ITEM_INFO(INC_SKILL_ABILITY_ESSENCE)
			GET_ITEM_INFO(WEDDING_BOOKCARD_ESSENCE)
			GET_ITEM_INFO(WEDDING_INVITECARD_ESSENCE)
			GET_ITEM_INFO(SHARPENER_ESSENCE)
			GET_ITEM_INFO(FACTION_MATERIAL_ESSENCE)
			GET_ITEM_INFO(CONGREGATE_ESSENCE)
			GET_ITEM_INFO(FORCE_TOKEN_ESSENCE)
			GET_ITEM_INFO(DYNSKILLEQUIP_ESSENCE)
			GET_ITEM_INFO(MONEY_CONVERTIBLE_ESSENCE)
			GET_ITEM_INFO(MONSTER_SPIRIT_ESSENCE)
			GET_ITEM_INFO(POKER_DICE_ESSENCE)
			GET_ITEM_INFO(POKER_ESSENCE)
			GET_ITEM_INFO(SHOP_TOKEN_ESSENCE)
			GET_ITEM_INFO(UNIVERSAL_TOKEN_ESSENCE)
		}
	}
#undef GET_ITEM_INFO
#undef GET_ITEM_INFO_1
}

void CElementIconToolDlg::OnButton4() 
{
	// 浏览原图标文件
	CFileDialog dlg(true, _T("*.dds"), (LPCTSTR)m_path3, OFN_FILEMUSTEXIST, _T("DDS Files(*.dds)|*.dds|All Files(*.*)|*.*||"));
	if (dlg.DoModal() != IDOK)
		return;
	m_path3 = dlg.GetPathName();
	UpdateData(FALSE);
}

void CElementIconToolDlg::OnButton5() 
{
	if (m_path3.IsEmpty())
	{
		CEdit *pEdit = (CEdit*)GetDlgItem(IDC_EDIT3);
		pEdit->SetFocus();
		pEdit->SetSel(0, -1);
		return;
	}

	ACString strPath(m_path3);
	AString strOutputDir;
	if (ConvertSkillIcon(strOutputDir))
	{
		AfxMessageBox(_T("转换结束!"), MB_ICONINFORMATION | MB_OK);

		// 打开输出目录用于查看
		ShellExecuteA(NULL, "open", strOutputDir, NULL, NULL, SW_SHOWNORMAL);
	}
	else
	{
		AfxMessageBox(_T("转换失败，请查看日志文件！"), MB_ICONERROR | MB_OK);
	}
	
	// 打开日志文件用于查看
	OpenLogFile();
}

void CElementIconToolDlg::OpenLogFile()
{
	AString strFile = af_GetBaseDir();
	strFile += "\\";
	strFile += g_Log.GetLogDir();
	strFile += "\\" + GetLogFileName();
	ShellExecuteA(NULL, "open", strFile, NULL, NULL, SW_SHOWNORMAL);	
}

bool CElementIconToolDlg::GetFirstSkillIconFilePath(AString &strIconIndex, AString &strIconOut)
{
	bool bRet(false);

	m_idSkill = 1;
	GNET::ElementSkill *pSkill = NULL;
	while (m_idSkill < 10000)
	{
		pSkill = GNET::ElementSkill::Create(m_idSkill, 1);
		if (pSkill)
			break;
		++m_idSkill;
	}

	if (pSkill)
	{
		pSkill->Destroy();
		GetSkillIconFilePath(strIconIndex, strIconOut);
		bRet = true;
	}

	return bRet;
}

bool CElementIconToolDlg::GetNextSkillIconFilePath(AString &strIconIndex, AString &strIconOut)
{
	bool bRet(false);

	GNET::ElementSkill *pSkill = NULL;
	while (m_idSkill < 10000)
	{
		pSkill = GNET::ElementSkill::Create(++m_idSkill, 1);
		if (pSkill)
			break;
	}

	if (pSkill)
	{
		pSkill->Destroy();
		GetSkillIconFilePath(strIconIndex, strIconOut);
		bRet = true;
	}

	return bRet;
}

void CElementIconToolDlg::GetSkillIconFilePath(AString &strIconIndex, AString &strIconOut)
{
	strIconIndex = GNET::ElementSkill::GetIcon(m_idSkill);
	strIconOut.Format("%d.png", m_idSkill);
}

bool CElementIconToolDlg::GetFirstItemIDName(unsigned int &id, AString &strName)
{
	m_dtTemp = DT_INVALID;
	m_idTemp = m_pElementDataMan->get_first_data_id(ID_SPACE_ESSENCE, m_dtTemp);
	while (m_idTemp)
	{
		GetItemIDName(id, strName);
		if (!strName.IsEmpty())
		{
			// 查找到正确的
			break;
		}
		// 查找的不是有效物品，查找下一个
		m_idTemp = m_pElementDataMan->get_next_data_id(ID_SPACE_ESSENCE, m_dtTemp);
	}
	return m_idTemp != 0 && m_dtTemp != DT_INVALID;
}

bool CElementIconToolDlg::GetNextItemIDName(unsigned int &id, AString &strName)
{
	m_idTemp = m_pElementDataMan->get_next_data_id(ID_SPACE_ESSENCE, m_dtTemp);	
	while (m_idTemp)
	{
		GetItemIDName(id, strName);
		if (!strName.IsEmpty())
		{
			// 查找到正确的
			break;
		}
		// 查找的不是有效物品，查找下一个
		m_idTemp = m_pElementDataMan->get_next_data_id(ID_SPACE_ESSENCE, m_dtTemp);
	}
	return m_idTemp != 0 && m_dtTemp != DT_INVALID;
}

void CElementIconToolDlg::GetItemIDName(unsigned int &id, AString &strName)
{
#undef GET_ITEM_INFO

#define GET_ITEM_INFO(essence) \
	case DT_##essence: \
	{ \
		const essence * pEssence = (const essence *)m_pElementDataMan->get_data_ptr(m_idTemp, ID_SPACE_ESSENCE, m_dtTemp); \
		if (pEssence) \
		{ \
		id = pEssence->id;\
		strName = WC2AS(AWString(pEssence->name));\
		} \
	}\
	break;

	id = 0;
	strName.Empty();
	if (m_idTemp != 0 && m_dtTemp != DT_INVALID)
	{
		switch(m_dtTemp)
		{
			GET_ITEM_INFO(WEAPON_ESSENCE)
			GET_ITEM_INFO(ARMOR_ESSENCE)
			GET_ITEM_INFO(DECORATION_ESSENCE)
			GET_ITEM_INFO(MEDICINE_ESSENCE)
			GET_ITEM_INFO(MATERIAL_ESSENCE)
			GET_ITEM_INFO(DAMAGERUNE_ESSENCE)
			GET_ITEM_INFO(ARMORRUNE_ESSENCE)
			GET_ITEM_INFO(SKILLTOME_ESSENCE)
			GET_ITEM_INFO(FLYSWORD_ESSENCE)
			GET_ITEM_INFO(WINGMANWING_ESSENCE)
			GET_ITEM_INFO(TOWNSCROLL_ESSENCE)
			GET_ITEM_INFO(UNIONSCROLL_ESSENCE)
			GET_ITEM_INFO(REVIVESCROLL_ESSENCE)
			GET_ITEM_INFO(ELEMENT_ESSENCE)
			GET_ITEM_INFO(TASKMATTER_ESSENCE)
			GET_ITEM_INFO(TOSSMATTER_ESSENCE)
			GET_ITEM_INFO(PROJECTILE_ESSENCE)
			GET_ITEM_INFO(QUIVER_ESSENCE)
			GET_ITEM_INFO(STONE_ESSENCE)
			GET_ITEM_INFO(TASKDICE_ESSENCE)
			GET_ITEM_INFO(TASKNORMALMATTER_ESSENCE)
			GET_ITEM_INFO(FASHION_ESSENCE)
			GET_ITEM_INFO(FACETICKET_ESSENCE)
			GET_ITEM_INFO(FACEPILL_ESSENCE)
			GET_ITEM_INFO(GM_GENERATOR_ESSENCE)
			GET_ITEM_INFO(PET_ESSENCE)		//	角色拍卖时显示宠物图标时需要
			GET_ITEM_INFO(PET_EGG_ESSENCE)
			GET_ITEM_INFO(PET_FOOD_ESSENCE)
			GET_ITEM_INFO(PET_FACETICKET_ESSENCE)
			GET_ITEM_INFO(FIREWORKS_ESSENCE)
			GET_ITEM_INFO(WAR_TANKCALLIN_ESSENCE)
			GET_ITEM_INFO(SKILLMATTER_ESSENCE)
			GET_ITEM_INFO(REFINE_TICKET_ESSENCE)
			GET_ITEM_INFO(DESTROYING_ESSENCE)
			GET_ITEM_INFO(BIBLE_ESSENCE)
			GET_ITEM_INFO(SPEAKER_ESSENCE)
			GET_ITEM_INFO(AUTOHP_ESSENCE)
			GET_ITEM_INFO(AUTOMP_ESSENCE)
			GET_ITEM_INFO(DOUBLE_EXP_ESSENCE)
			GET_ITEM_INFO(TRANSMITSCROLL_ESSENCE)
			GET_ITEM_INFO(DYE_TICKET_ESSENCE)			
			GET_ITEM_INFO(GOBLIN_ESSENCE)
			GET_ITEM_INFO(GOBLIN_EQUIP_ESSENCE)
			GET_ITEM_INFO(GOBLIN_EXPPILL_ESSENCE)
			GET_ITEM_INFO(SELL_CERTIFICATE_ESSENCE)
			GET_ITEM_INFO(TARGET_ITEM_ESSENCE)
			GET_ITEM_INFO(LOOK_INFO_ESSENCE)
			GET_ITEM_INFO(INC_SKILL_ABILITY_ESSENCE)
			GET_ITEM_INFO(WEDDING_BOOKCARD_ESSENCE)
			GET_ITEM_INFO(WEDDING_INVITECARD_ESSENCE)
			GET_ITEM_INFO(SHARPENER_ESSENCE)
			GET_ITEM_INFO(FACTION_MATERIAL_ESSENCE)
			GET_ITEM_INFO(CONGREGATE_ESSENCE)
			GET_ITEM_INFO(FORCE_TOKEN_ESSENCE)
			GET_ITEM_INFO(DYNSKILLEQUIP_ESSENCE)
			GET_ITEM_INFO(MONEY_CONVERTIBLE_ESSENCE)
			GET_ITEM_INFO(MONSTER_SPIRIT_ESSENCE)
			GET_ITEM_INFO(POKER_DICE_ESSENCE)
			GET_ITEM_INFO(POKER_ESSENCE)
			GET_ITEM_INFO(SHOP_TOKEN_ESSENCE)
			GET_ITEM_INFO(UNIVERSAL_TOKEN_ESSENCE)
		}
	}
#undef GET_ITEM_INFO
}

AString MakeTempFilePath()
{
	//	获取临时文件名称
	AString strFullPath;

	while (true)
	{
		char szBuffer1[MAX_PATH];
		DWORD dwRetVal = GetTempPathA(MAX_PATH, szBuffer1);
		if (dwRetVal > MAX_PATH || dwRetVal==0)
			break;
		
		char szBuffer2[MAX_PATH];
		UINT bCreate = GetTempFileNameA(szBuffer1, NULL, 0, szBuffer2);
		if (!bCreate || bCreate==ERROR_BUFFER_OVERFLOW)
			break;

		strFullPath = szBuffer2;
		break;
	}

	return strFullPath;
}

void OpenInNotePad(const AString &strPath)
{
	SHELLEXECUTEINFOA si;
	ZeroMemory(&si, sizeof(si));

	si.cbSize = sizeof(si);
	si.fMask = SEE_MASK_NOCLOSEPROCESS;
	si.lpVerb = "open";
	si.lpFile = "notepad";
	si.lpParameters = strPath;
	si.nShow = SW_SHOW;
	ShellExecuteExA(&si);
	
	// 解压完成后才返回
	WaitForSingleObject(si.hProcess, INFINITE);
	CloseHandle(si.hProcess);
}

void CElementIconToolDlg::OnButton6() 
{
	//	导出 ID -> 物品名称对应表
	
	if (m_path2.IsEmpty())
	{
		CEdit *pEdit = (CEdit*)GetDlgItem(IDC_EDIT2);
		pEdit->SetFocus();
		pEdit->SetSel(0, -1);
		return;
	}

	AString strTempPath = MakeTempFilePath();
	if (strTempPath.IsEmpty())
	{
		MessageBoxA(NULL, "生成存放结果的临时文件失败", NULL, MB_OK);
		return;
	}
	
	AFile file;
	if (!file.Open(strTempPath, AFILE_NOHEAD | AFILE_CREATENEW | AFILE_TEXT))
	{
		MessageBoxA(NULL, "打开存放结果的临时文件失败", NULL, MB_OK);
		return;
	}
	
	// 读取 elements.data 文件
	if (!LoadElementData())
	{
		MessageBoxA(NULL, "加载elements.data失败.", NULL, MB_OK);
		file.Close();
		return;
	}
	
	AString strFormat = "%d	%s";
	AString strNewLine;
	
	unsigned int id(0);
	AString strName;
	if (GetFirstItemIDName(id, strName))
	{
		do
		{
			strNewLine.Format(strFormat, id, strName);
			file.WriteLine(strNewLine);			
		} while(GetNextItemIDName(id, strName));
	}
	
	ClearElementData();
	
	file.Flush();
	file.Close();
	
	OpenInNotePad(strTempPath);
}

void CElementIconToolDlg::OnEnChangeEdit3()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	UpdateData(TRUE);
}
