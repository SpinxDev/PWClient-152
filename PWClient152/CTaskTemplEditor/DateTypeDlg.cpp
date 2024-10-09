// DateTypeDlg.cpp : implementation file
//

#include "stdafx.h"
#include "tasktempleditor.h"
#include "DateTypeDlg.h"

#ifdef _DEBUG
#define new A_DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDateTypeDlg dialog


CDateTypeDlg::CDateTypeDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDateTypeDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDateTypeDlg)
	m_nDateType = 0;
	//}}AFX_DATA_INIT
}


void CDateTypeDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDateTypeDlg)
	DDX_Radio(pDX, IDC_DATE_TYPE, m_nDateType);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDateTypeDlg, CDialog)
	//{{AFX_MSG_MAP(CDateTypeDlg)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDateTypeDlg message handlers
