// MonsterFaction.cpp : implementation file
//

#include "stdafx.h"
#include "elementdata.h"
#include "MonsterFaction.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

abase::vector<AString> _monster_faction;

BOOL CALLBACK MonsterFaction::OnActivate(void)
{
	CMonsterFaction dlg;
	dlg.m_ulFactionsMask = m_var;
	if (dlg.DoModal() != IDOK) return FALSE;
	m_var = dlg.m_ulFactionsMask;
	return TRUE;
}

LPCTSTR CALLBACK MonsterFaction::OnGetShowString(void) const
{
	m_strShow.Empty();
	AVariant var = m_var;
	unsigned long ulMask = var;
	for (unsigned long i = 0; i < 32; i++)
		if (ulMask & (1 << i))
			m_strShow += _monster_faction[i] + ";";
	return m_strShow;
}

AVariant CALLBACK MonsterFaction::OnGetValue(void) const
{
	return m_var;
}

void CALLBACK MonsterFaction::OnSetValue(const AVariant& var)
{
	m_var = var;
}

/////////////////////////////////////////////////////////////////////////////
// CMonsterFaction dialog


CMonsterFaction::CMonsterFaction(CWnd* pParent /*=NULL*/)
	: CDialog(CMonsterFaction::IDD, pParent)
{
	//{{AFX_DATA_INIT(CMonsterFaction)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CMonsterFaction::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMonsterFaction)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CMonsterFaction, CDialog)
	//{{AFX_MSG_MAP(CMonsterFaction)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMonsterFaction message handlers

BOOL CMonsterFaction::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	for (size_t i = 0; i < _monster_faction.size(); i++)
	{
		CButton* pButton = (CButton*)GetDlgItem(IDC_CHECK1 + i);
		pButton->SetWindowText(_monster_faction[i]);
		if (m_ulFactionsMask & (1 << i))
			pButton->SetCheck(BST_CHECKED);
	}

	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CMonsterFaction::OnOK() 
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
