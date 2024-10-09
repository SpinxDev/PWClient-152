// FloatDlg.cpp : implementation file
//

#include "stdafx.h"
#include "TaskTemplEditor.h"
#include "FloatDlg.h"

#ifdef _DEBUG
#define new A_DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// FloatDlg dialog


FloatDlg::FloatDlg(CWnd* pParent /*=NULL*/)
	: CDialog(FloatDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(FloatDlg)
	m_fVal = 0.0f;
	//}}AFX_DATA_INIT
}


void FloatDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(FloatDlg)
	DDX_Text(pDX, IDC_NUM, m_fVal);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(FloatDlg, CDialog)
	//{{AFX_MSG_MAP(FloatDlg)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// FloatDlg message handlers
