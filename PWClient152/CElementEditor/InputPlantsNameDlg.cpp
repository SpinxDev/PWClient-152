// InputPlantsNameDlg.cpp : implementation file
//

#include "stdafx.h"
#include "elementeditor.h"
#include "InputPlantsNameDlg.h"

//#define new A_DEBUG_NEW

/////////////////////////////////////////////////////////////////////////////
// CInputPlantsNameDlg dialog


CInputPlantsNameDlg::CInputPlantsNameDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CInputPlantsNameDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CInputPlantsNameDlg)
	m_strName = _T("");
	//}}AFX_DATA_INIT
}


void CInputPlantsNameDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CInputPlantsNameDlg)
	DDX_Text(pDX, IDC_EDIT_PLANTS_NAME, m_strName);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CInputPlantsNameDlg, CDialog)
	//{{AFX_MSG_MAP(CInputPlantsNameDlg)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CInputPlantsNameDlg message handlers
