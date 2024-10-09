// EquipMaskDlg.cpp : implementation file
//

#include "stdafx.h"
#include "elementdata.h"
#include "EquipMaskDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

BOOL CALLBACK EquipMask::OnActivate(void)
{
	CEquipMaskDlg dlg;
	dlg.m_ulEquipMask = m_var;
	dlg.m_pMaskNames = m_pMaskNames;
	dlg.m_nNameCount = m_nNameCount;
	if (dlg.DoModal() != IDOK) return FALSE;
	m_var = dlg.m_ulEquipMask;
	return TRUE;
}

LPCTSTR CALLBACK EquipMask::OnGetShowString(void) const
{
	m_strShow.Empty();
	AVariant var = m_var;
	unsigned long ulMask = var;
	for ( int i = 0; i < m_nNameCount; i++)
		if (ulMask & (1 << i))
		{
			m_strShow += m_pMaskNames[i];
			m_strShow += ";";
		}
	return m_strShow;
}

AVariant CALLBACK EquipMask::OnGetValue(void) const
{
	return m_var;
}

void CALLBACK EquipMask::OnSetValue(const AVariant& var)
{
	m_var = var;
}

/////////////////////////////////////////////////////////////////////////////
// CEquipMaskDlg dialog


CEquipMaskDlg::CEquipMaskDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CEquipMaskDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CEquipMaskDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CEquipMaskDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEquipMaskDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CEquipMaskDlg, CDialog)
	//{{AFX_MSG_MAP(CEquipMaskDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEquipMaskDlg message handlers

BOOL CEquipMaskDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	for (int i = 0; i < 32; i++)
	{
		CButton* pButton = (CButton*)GetDlgItem(IDC_CHECK1 + i);
		if (i >= m_nNameCount)
			pButton->ShowWindow(SW_HIDE);
		else
		{
			pButton->SetWindowText(m_pMaskNames[i]);
			if (m_ulEquipMask & (1 << i))
				pButton->SetCheck(BST_CHECKED);
		}
	}
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CEquipMaskDlg::OnOK()
{
	UpdateData();
	m_ulEquipMask = 0;

	for (int i = 0; i < 32; i++)
	{
		if (i >= m_nNameCount) break;
		CButton* pButton = (CButton*)GetDlgItem(IDC_CHECK1 + i);
		if (pButton->GetCheck() == BST_CHECKED)
			m_ulEquipMask |= 1 << i;
	}

	CDialog::OnOK();
}
