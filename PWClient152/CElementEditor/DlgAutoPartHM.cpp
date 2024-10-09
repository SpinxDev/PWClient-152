// DlgAutoPartHM.cpp : implementation file
//

#include "Global.h"
#include "DlgAutoPartHM.h"
#include "AMemory.h"

//#define new A_DEBUG_NEW

/////////////////////////////////////////////////////////////////////////////
// CDlgAutoPartHM dialog


CDlgAutoPartHM::CDlgAutoPartHM(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgAutoPartHM::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgAutoPartHM)
	m_strFileName = _T("");
	m_iHeight = 0;
	m_iWidth = 0;
	m_iNumRow = 0;
	m_iNumCol = 0;
	//}}AFX_DATA_INIT

	m_pSrcData	= NULL;
	m_pDstData	= NULL;
}

void CDlgAutoPartHM::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgAutoPartHM)
	DDX_Text(pDX, IDC_EDIT_FILENAME, m_strFileName);
	DDX_Text(pDX, IDC_EDIT_HEIGHT, m_iHeight);
	DDX_Text(pDX, IDC_EDIT_WIDTH, m_iWidth);
	DDX_Text(pDX, IDC_EDIT_ROW, m_iNumRow);
	DDX_Text(pDX, IDC_EDIT_COL, m_iNumCol);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CDlgAutoPartHM, CDialog)
	//{{AFX_MSG_MAP(CDlgAutoPartHM)
	ON_BN_CLICKED(IDC_BROWSE, OnBrowse)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgAutoPartHM message handlers

BOOL CDlgAutoPartHM::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	
	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgAutoPartHM::OnBrowse() 
{
	char szFilter[] = "Raw Files (*.raw)|*.raw||";
	DWORD dwFlags = OFN_FILEMUSTEXIST | OFN_EXPLORER | OFN_NOCHANGEDIR;
	CFileDialog	FileDia(TRUE, "*.raw", NULL, dwFlags, szFilter, NULL);
		
	if (FileDia.DoModal() == IDCANCEL)
		return;

	m_strFileName = FileDia.GetPathName();

	CDataExchange dx(this, FALSE);
	DDX_Text(&dx, IDC_EDIT_FILENAME, m_strFileName);
}

void CDlgAutoPartHM::OnOK() 
{
	if (!UpdateData(TRUE))
		return;

	if (m_iWidth <= 0 || m_iHeight <= 0 ||
		m_iNumCol <= 0 || m_iNumRow <= 0)
	{
		MessageBox("Invalid input number");
		return;
	}

	if (m_iNumCol >= m_iWidth || m_iNumRow >= m_iHeight ||
		(m_iWidth-1) % m_iNumCol || (m_iHeight-1) % m_iNumRow)
	{
		MessageBox("Invalid input number");
		return;
	}

	//	Let user select exporting path
	BROWSEINFO Info;
	memset(&Info, 0, sizeof (Info));

	Info.hwndOwner	= m_hWnd;
	Info.pidlRoot	= NULL;
	Info.lpszTitle	= "Please select exporting path";
	Info.ulFlags	= BIF_RETURNONLYFSDIRS | BIF_STATUSTEXT | BIF_DONTGOBELOWDOMAIN;

	LPITEMIDLIST pPIDL;
	if (!(pPIDL = SHBrowseForFolder(&Info)))
		return;

	SHGetPathFromIDList(pPIDL, m_szDstPath);
	char* pTemp = strrchr(m_szDstPath, '\\');
	if (!pTemp || *(pTemp+1) == '\0')
	{
		MessageBox("Invalid path");
		return;
	}

	if (!m_strFileName.GetLength())
	{
		MessageBox("Please select a height map file");
		return;
	}

	//	Load source height map
	if (!LoadSourceHeightMap())
	{
		MessageBox("Failed to load data from height map file");
		return;
	}

	//	Create height map buffer
	m_iSubWid	= (m_iWidth-1) / m_iNumCol + 1;
	m_iSubHei	= (m_iHeight-1) / m_iNumRow + 1;

	if (!(m_pDstData = (WORD*)a_malloc(m_iSubWid * m_iSubHei * sizeof (WORD))))
	{
		ReleaseSourceHeightMap();
		MessageBox("Not enough memory !");
		return;
	}

	int iMapCount = 0;

	for (int r=0; r < m_iNumRow; r++)
	{
		for (int c=0; c < m_iNumCol; c++)
		{
			ExportSubHeightMap(r, c, iMapCount++);
		}
	}

	a_free(m_pDstData);
	m_pDstData = NULL;

//	CDialog::OnOK();
}

void CDlgAutoPartHM::OnCancel() 
{
	ReleaseSourceHeightMap();

	if (m_pDstData)
	{
		a_free(m_pDstData);
		m_pDstData = NULL;
	}

	CDialog::OnCancel();
}

//	Load source height map from file
bool CDlgAutoPartHM::LoadSourceHeightMap()
{
	//	Open height map file
	FILE* fp = fopen(m_strFileName, "rb");
	if (!fp)
	{
		g_Log.Log("CDlgAutoPartHM::LoadSourceHeightMap, Falied to open file %s !", m_strFileName);
		return false;
	}

	int iSize = m_iWidth * m_iHeight * sizeof (WORD);
	if (!(m_pSrcData = (WORD*)a_malloc(iSize)))
	{
		fclose(fp);
		g_Log.Log("CDlgAutoPartHM::LoadSourceHeightMap, Not enough memory !");
		return false;
	}

	if (fread(m_pSrcData, 1, iSize, fp) != (DWORD)iSize)
	{
		fclose(fp);
		delete [] m_pSrcData;
		g_Log.Log("CDlgAutoPartHM::LoadSourceHeightMap, Failed to read height map data. %s !", m_strFileName);
		return false;
	}

	fclose(fp);

	return true;
}

//	Release source height map data
void CDlgAutoPartHM::ReleaseSourceHeightMap()
{
	if (m_pSrcData)
	{
		a_free(m_pSrcData);
		m_pSrcData = NULL;
	}
}

//	Export a sub height map
bool CDlgAutoPartHM::ExportSubHeightMap(int r, int c, int iMapCount)
{
	//	Create destination file
	char szFileName[MAX_PATH];
	sprintf(szFileName, "%s\\heightmap_%03d.raw", m_szDstPath, iMapCount+1);
	FILE* fp = fopen(szFileName, "wb+");
	if (!fp)
	{
		g_Log.Log("CDlgAutoPartHM::LoadSourceHeightMap, Failed to create file %s !", szFileName);
		return false;
	}

	WORD* pSrcData = m_pSrcData + r * (m_iSubHei - 1) * m_iWidth + c * (m_iSubWid - 1);
	WORD* pDstData = m_pDstData;

	for (int i=0; i < m_iSubHei; i++)
	{
		memcpy(pDstData, pSrcData, m_iSubWid * sizeof (WORD));
		pDstData += m_iSubWid;
		pSrcData += m_iWidth;
	}

	//	Save data
	fwrite(m_pDstData, 1, m_iSubWid * m_iSubHei * sizeof (WORD), fp);

	fclose(fp);

	return true;
}


