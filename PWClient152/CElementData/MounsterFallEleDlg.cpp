// MounsterFallEleDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ElementData.h"
#include "MounsterFallEleDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMounsterFallEleDlg dialog


CMounsterFallEleDlg::CMounsterFallEleDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CMounsterFallEleDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CMounsterFallEleDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CMounsterFallEleDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMounsterFallEleDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CMounsterFallEleDlg, CDialog)
	//{{AFX_MSG_MAP(CMounsterFallEleDlg)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMounsterFallEleDlg message handlers
