// FindTaskDlg.cpp : implementation file
//

#include "stdafx.h"
#include "TaskTemplEditor.h"
#include "FindTaskDlg.h"

#ifdef _DEBUG
#define new A_DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFindTaskDlg dialog


CFindTaskDlg::CFindTaskDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CFindTaskDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CFindTaskDlg)
	m_strTask = _T("");
	//}}AFX_DATA_INIT
}


void CFindTaskDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFindTaskDlg)
	DDX_Text(pDX, IDC_TASK, m_strTask);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CFindTaskDlg, CDialog)
	//{{AFX_MSG_MAP(CFindTaskDlg)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFindTaskDlg message handlers
