// RegionPointDlg.cpp : implementation file
//

#include "stdafx.h"
#include "tasktempleditor.h"
#include "RegionPointDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CRegionPointDlg dialog


CRegionPointDlg::CRegionPointDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CRegionPointDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CRegionPointDlg)
	m_fX = 0.0f;
	m_fY = 0.0f;
	m_fZ = 0.0f;
	m_fX2 = 0.0f;
	m_fY2 = 0.0f;
	m_fZ2 = 0.0f;
	//}}AFX_DATA_INIT
}


void CRegionPointDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CRegionPointDlg)
	DDX_Text(pDX, IDC_EDIT_X, m_fX);
	DDX_Text(pDX, IDC_EDIT_Y, m_fY);
	DDX_Text(pDX, IDC_EDIT_Z, m_fZ);
	DDX_Text(pDX, IDC_EDIT_X2, m_fX2);
	DDX_Text(pDX, IDC_EDIT_Y2, m_fY2);
	DDX_Text(pDX, IDC_EDIT_Z2, m_fZ2);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CRegionPointDlg, CDialog)
	//{{AFX_MSG_MAP(CRegionPointDlg)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRegionPointDlg message handlers
