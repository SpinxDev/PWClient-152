// SingleTextDlg.cpp : implementation file
//

#include "stdafx.h"
#include "elementdata.h"
#include "SingleTextDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

BOOL CALLBACK SingleTextFuncs::OnActivate(void)
{
	CSingleTextDlg dlg;
	dlg.m_strText = AString(m_var);
	if (dlg.DoModal() != IDOK) return FALSE;
	m_strText = dlg.m_strText;
	m_var = AString(dlg.m_strText);

	return TRUE;
}

LPCTSTR CALLBACK SingleTextFuncs::OnGetShowString(void) const
{
	return m_strText;
}

AVariant CALLBACK SingleTextFuncs::OnGetValue(void) const
{
	return m_var;
}

void CALLBACK SingleTextFuncs::OnSetValue(const AVariant& var)
{
	m_var = var;
	m_strText = AString(const_cast<AVariant&>(var));
}

/////////////////////////////////////////////////////////////////////////////
// CSingleTextDlg dialog


CSingleTextDlg::CSingleTextDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSingleTextDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSingleTextDlg)
	m_strText = _T("");
	//}}AFX_DATA_INIT
}


void CSingleTextDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSingleTextDlg)
	DDX_Text(pDX, IDC_TEXT, m_strText);
	DDV_MaxChars(pDX, m_strText, 32);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSingleTextDlg, CDialog)
	//{{AFX_MSG_MAP(CSingleTextDlg)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSingleTextDlg message handlers
