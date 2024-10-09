// TextInputDlg.cpp : implementation file
//

#include "stdafx.h"
#include "elementdata.h"
#include "TextInputDlg.h"
#include "BaseDataTemplate.h"
#include "ExtendDataTempl.h"
#include "ExpTypes.h"
#include "BaseDataExp.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

BOOL CALLBACK TextInputFuncs::OnActivate(void)
{
	CTextInputDlg dlg;
	dlg.m_strText = AString(m_var);
	ConvertMultilineText((LPSTR)(LPCTSTR)dlg.m_strText, true);
	if (dlg.DoModal() != IDOK) return FALSE;
	m_strText = dlg.m_strText;
	ConvertMultilineText((LPSTR)(LPCTSTR)dlg.m_strText, false);
	m_var = AString(dlg.m_strText);

	return TRUE;
}

LPCTSTR CALLBACK TextInputFuncs::OnGetShowString(void) const
{
	return m_strText;
}

AVariant CALLBACK TextInputFuncs::OnGetValue(void) const
{
	return m_var;
}

void CALLBACK TextInputFuncs::OnSetValue(const AVariant& var)
{
	m_var = var;
	m_strText = (const char*)AString(const_cast<AVariant&>(var));
	ConvertMultilineText((LPSTR)(LPCTSTR)m_strText, true);
}

/////////////////////////////////////////////////////////////////////////////
// CTextInputDlg dialog


CTextInputDlg::CTextInputDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTextInputDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CTextInputDlg)
	m_strText = _T("");
	//}}AFX_DATA_INIT
}


void CTextInputDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTextInputDlg)
	DDX_Text(pDX, IDC_TEXT_INPUT, m_strText);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTextInputDlg, CDialog)
	//{{AFX_MSG_MAP(CTextInputDlg)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTextInputDlg message handlers
