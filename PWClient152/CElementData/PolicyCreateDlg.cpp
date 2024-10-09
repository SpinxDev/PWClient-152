// PolicyCreateDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ElementData.h"
#include "PolicyCreateDlg.h"
#include "PolicyTreeDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPolicyCreateDlg dialog


CPolicyCreateDlg::CPolicyCreateDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CPolicyCreateDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPolicyCreateDlg)
	m_szPolicyName = _T("");
	m_szPolicyCopyPath = _T("");
	//}}AFX_DATA_INIT
}


void CPolicyCreateDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPolicyCreateDlg)
	DDX_Text(pDX, IDC_EDIT_NAME, m_szPolicyName);
	DDX_Text(pDX, IDC_EDIT_POLICY_COPY_PATH, m_szPolicyCopyPath);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPolicyCreateDlg, CDialog)
	//{{AFX_MSG_MAP(CPolicyCreateDlg)
		// NOTE: the ClassWizard will add message map macros here
		ON_BN_CLICKED(IDC_BUTTON_POLICY_COPY_PATH, OnBtnSelCopy)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPolicyCreateDlg message handlers

void CPolicyCreateDlg::OnOK()
{
	UpdateData(true);
	if(m_szPolicyName.IsEmpty())
	{
		MessageBox("策略的名字不能为空!");
		return;
	}
	CDialog::OnOK();
}

void CPolicyCreateDlg::OnBtnSelCopy()
{
	UpdateData(TRUE);
	CPolicyTreeDlg dlg;
	if(IDOK != dlg.DoModal())
		return;
	
	m_szPolicyCopyPath = dlg.m_strSelPolicyPath;
	UpdateData(FALSE);
}