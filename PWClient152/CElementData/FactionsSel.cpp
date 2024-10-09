// FactionsSel.cpp : implementation file
//

#include "stdafx.h"
#include "elementdata.h"
#include "FactionsSel.h"
#include "vector.h"
#include "AString.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

abase::vector<AString> _factions;

BOOL CALLBACK FactionSel::OnActivate(void)
{
	CFactionsSel dlg;
	dlg.m_ulFactionsMask = m_var;
	if (dlg.DoModal() != IDOK) return FALSE;
	m_var = dlg.m_ulFactionsMask;
	return TRUE;
}

LPCTSTR CALLBACK FactionSel::OnGetShowString(void) const
{
	m_strShow.Empty();
	AVariant var = m_var;
	unsigned long ulMask = var;
	for (unsigned long i = 0; i < 32; i++)
		if (ulMask & (1 << i))
			m_strShow += _factions[i] + ";";
	return m_strShow;
}

AVariant CALLBACK FactionSel::OnGetValue(void) const
{
	return m_var;
}

void CALLBACK FactionSel::OnSetValue(const AVariant& var)
{
	m_var = var;
}

/////////////////////////////////////////////////////////////////////////////
// CFactionsSel dialog


CFactionsSel::CFactionsSel(CWnd* pParent /*=NULL*/)
	: CDialog(CFactionsSel::IDD, pParent)
{
	//{{AFX_DATA_INIT(CFactionsSel)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CFactionsSel::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFactionsSel)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CFactionsSel, CDialog)
	//{{AFX_MSG_MAP(CFactionsSel)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFactionsSel message handlers

BOOL CFactionsSel::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	for (size_t i = 0; i < _factions.size(); i++)
	{
		CButton* pButton = (CButton*)GetDlgItem(IDC_CHECK1 + i);
		pButton->SetWindowText(_factions[i]);
		if (m_ulFactionsMask & (1 << i))
			pButton->SetCheck(BST_CHECKED);
	}
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CFactionsSel::OnOK() 
{
	UpdateData();
	m_ulFactionsMask = 0;

	for (UINT i = 0; i < 32; i++)
	{
		CButton* pButton = (CButton*)GetDlgItem(IDC_CHECK1 + i);
		if (pButton->GetCheck() == BST_CHECKED)
			m_ulFactionsMask |= 1 << i;
	}
	CDialog::OnOK();
}
