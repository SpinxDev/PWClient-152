// SetNPCInfoDlg.cpp : implementation file
//

#include "stdafx.h"
#include "TaskTemplEditor.h"
#include "SetNPCInfoDlg.h"

#ifdef _DEBUG
#define new A_DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSetNPCInfoDlg dialog


CSetNPCInfoDlg::CSetNPCInfoDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSetNPCInfoDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSetNPCInfoDlg)
	m_x = 0;
	m_y = 0;
	m_strName = _T("");
	m_z = 0;
	//}}AFX_DATA_INIT
}


void CSetNPCInfoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSetNPCInfoDlg)
	DDX_Text(pDX, IDC_X, m_x);
	DDX_Text(pDX, IDC_Y, m_y);
	DDX_Text(pDX, IDC_NAME, m_strName);
	DDX_Text(pDX, IDC_Z, m_z);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSetNPCInfoDlg, CDialog)
	//{{AFX_MSG_MAP(CSetNPCInfoDlg)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSetNPCInfoDlg message handlers
