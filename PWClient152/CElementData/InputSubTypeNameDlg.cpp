// InputSubTypeNameDlg.cpp : implementation file
//

#include "stdafx.h"
#include "elementdata.h"
#include "InputSubTypeNameDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CInputSubTypeNameDlg dialog


CInputSubTypeNameDlg::CInputSubTypeNameDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CInputSubTypeNameDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CInputSubTypeNameDlg)
	m_strSubTypeName = _T("");
	//}}AFX_DATA_INIT
}


void CInputSubTypeNameDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CInputSubTypeNameDlg)
	DDX_Text(pDX, IDC_EDIT_SUB_NAME, m_strSubTypeName);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CInputSubTypeNameDlg, CDialog)
	//{{AFX_MSG_MAP(CInputSubTypeNameDlg)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CInputSubTypeNameDlg message handlers
