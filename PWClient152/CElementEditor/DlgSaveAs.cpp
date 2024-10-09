// DlgSaveAs.cpp : implementation file
//

#include "Global.h"
#include "DlgSaveAs.h"
#include "ElementMap.h"

//#define new A_DEBUG_NEW

/////////////////////////////////////////////////////////////////////////////
// CDlgSaveAs dialog

CDlgSaveAs::CDlgSaveAs(CElementMap* pMap, CWnd* pParent /*=NULL*/)
	: CDialog(CDlgSaveAs::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgSaveAs)
	m_strProjName = _T("");
	//}}AFX_DATA_INIT

	m_pMap = pMap;
}


void CDlgSaveAs::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgSaveAs)
	DDX_Text(pDX, IDC_EDIT_PROJNAME, m_strProjName);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CDlgSaveAs, CDialog)
	//{{AFX_MSG_MAP(CDlgSaveAs)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgSaveAs message handlers

BOOL CDlgSaveAs::OnInitDialog() 
{
	CDialog::OnInitDialog();

	m_strProjName = m_pMap->GetMapName();

	UpdateData(FALSE);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgSaveAs::OnOK() 
{
	if (!UpdateData(TRUE))
		return;

	if (m_strProjName.CompareNoCase(m_pMap->GetMapName()))
	{
		//	Project name changed, create new project directory
		if (!AUX_CreateProjectDir(m_hWnd, m_strProjName))
			return;
	}
	
	CDialog::OnOK();
}


