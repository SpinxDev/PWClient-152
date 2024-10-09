// DlgNewHeiMap.cpp : implementation file
//

#include "Global.h"
#include "DlgNewHeiMap.h"

//#define new A_DEBUG_NEW

/////////////////////////////////////////////////////////////////////////////
// CDlgNewHeiMap dialog


CDlgNewHeiMap::CDlgNewHeiMap(bool bLoadFlag, float fTerrainSize, CWnd* pParent /*=NULL*/)
	: CDialog(CDlgNewHeiMap::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgNewHeiMap)
	m_strFileName = _T("");
	m_strScale = _T("");
	m_iHMType = -1;
	//}}AFX_DATA_INIT

	m_bLoadFlag		= bLoadFlag;
	m_iWidth		= 0;
	m_iHeight		= 0;
	m_fTerrainSize	= fTerrainSize;
}


void CDlgNewHeiMap::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgNewHeiMap)
	DDX_Control(pDX, IDC_COMBO_SIZE, m_SizeCombo);
	DDX_Text(pDX, IDC_EDIT_FILENAME, m_strFileName);
	DDX_Text(pDX, IDC_STATIC_SCALE, m_strScale);
	DDX_Radio(pDX, IDC_RADIO1, m_iHMType);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgNewHeiMap, CDialog)
	//{{AFX_MSG_MAP(CDlgNewHeiMap)
	ON_BN_CLICKED(IDC_BTN_BROWSE, OnBtnBrowse)
	ON_CBN_SELCHANGE(IDC_COMBO_SIZE, OnSelchangeComboSize)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgNewHeiMap message handlers

BOOL CDlgNewHeiMap::OnInitDialog() 
{
	CDialog::OnInitDialog();

	if (!m_bLoadFlag)
		GetDlgItem(IDC_EDIT_FILENAME)->EnableWindow(FALSE);

	//	Initialize size array
	m_aSizes[SIZE_64]	= 65;
	m_aSizes[SIZE_128]	= 129;
	m_aSizes[SIZE_256]	= 257;
	m_aSizes[SIZE_512]	= 513;
	m_aSizes[SIZE_1024]	= 1025;
	m_aSizes[SIZE_2048]	= 2049;
	m_aSizes[SIZE_4096]	= 4097;
	m_aSizes[SIZE_8192]	= 8193;
	
	//	Initialize size combo
	m_SizeCombo.InsertString(-1, "65 x 65");
	m_SizeCombo.InsertString(-1, "129 x 129");
	m_SizeCombo.InsertString(-1, "257 x 257");
	m_SizeCombo.InsertString(-1, "513 x 513");
	m_SizeCombo.InsertString(-1, "1025 x 1025");
	m_SizeCombo.InsertString(-1, "2049 x 2049");
	m_SizeCombo.InsertString(-1, "4097 x 4097");
	m_SizeCombo.InsertString(-1, "8193 x 8193");

	m_SizeCombo.SetCurSel(3);

	OnSelchangeComboSize();

	//	Default 16-bit
	m_iHMType = 1;

	UpdateData(FALSE);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgNewHeiMap::OnOK() 
{
	UpdateData(TRUE);

	if (m_bLoadFlag && m_strFileName == "")
	{
		MessageBox("请输入高度图文件名。");
		return;
	}

	int iCur = m_SizeCombo.GetCurSel();
	m_iWidth = m_iHeight = m_aSizes[iCur];

	CDialog::OnOK();
}

void CDlgNewHeiMap::OnBtnBrowse() 
{
	char szFilter[] = "所有文件(*.*)|*.*||";
	DWORD dwFlags = OFN_FILEMUSTEXIST | OFN_EXPLORER | OFN_NOCHANGEDIR;
	CFileDialog	FileDia(TRUE, "*.*", NULL, dwFlags, szFilter, NULL);
		
	if (FileDia.DoModal() == IDCANCEL)
		return;

	m_strFileName = FileDia.GetPathName();

	CDataExchange dx(this, FALSE);
	DDX_Text(&dx, IDC_EDIT_FILENAME, m_strFileName);
}

//	Size changed
void CDlgNewHeiMap::OnSelchangeComboSize() 
{
	int iSize = m_aSizes[m_SizeCombo.GetCurSel()];
	float fScale = m_fTerrainSize / iSize;

	m_strScale.Format("1 像素 = %.4f 米", fScale);

	CDataExchange dx(this, FALSE);
	DDX_Text(&dx, IDC_STATIC_SCALE, m_strScale);
}


