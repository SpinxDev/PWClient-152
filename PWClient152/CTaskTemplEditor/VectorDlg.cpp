// VectorDlg.cpp : implementation file
//

#include "stdafx.h"
#include "TaskTemplEditor.h"
#include "VectorDlg.h"

#ifdef _DEBUG
#define new A_DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CVectorDlg dialog


CVectorDlg::CVectorDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CVectorDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CVectorDlg)
	m_fX = 0.0f;
	m_fY = 0.0f;
	m_fZ = 0.0f;
	//}}AFX_DATA_INIT
}


void CVectorDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CVectorDlg)
	DDX_Text(pDX, IDC_X, m_fX);
	DDX_Text(pDX, IDC_Y, m_fY);
	DDX_Text(pDX, IDC_Z, m_fZ);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CVectorDlg, CDialog)
	//{{AFX_MSG_MAP(CVectorDlg)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CVectorDlg message handlers
