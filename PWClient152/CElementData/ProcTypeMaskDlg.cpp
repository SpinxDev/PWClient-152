// ProcTypeMaskDlg.cpp : implementation file
//

#include "stdafx.h"
#include "elementdata.h"
#include "ProcTypeMaskDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static LPCTSTR _mask_name[] = {
	_T("����ʱ���ܵ���"),
	_T("���������ڵ���"),
	_T("����������NPC"),
	_T("���������Ʒ"),
	_T("��������Ҽ佻��"),
	_T("�����������Ʒ"),
	_T("װ�����"),
	_T("�ɽ��"),
	_T("�뿪������ʧ"),
	_T("���˾���"),
	_T("�����ض�����"),
	_T("���ߵ���"),
	_T("��������"),
	_T("�����Ʒ"),
	_T("���ɷ����˺Ųֿ�"),
};

BOOL CALLBACK ProcTypeMask::OnActivate(void)
{
	CProcTypeMaskDlg dlg;
	dlg.m_ulProcTypeMask = m_var;
	if (dlg.DoModal() != IDOK) return FALSE;
	m_var = dlg.m_ulProcTypeMask;
	return TRUE;
}

LPCTSTR CALLBACK ProcTypeMask::OnGetShowString(void) const
{
	m_strShow.Empty();
	AVariant var = m_var;
	unsigned long ulMask = var;
	for (unsigned long i = 0; i < sizeof(_mask_name) / sizeof(LPCTSTR); i++)
		if (ulMask & (1 << i))
		{
			m_strShow += _mask_name[i];
			m_strShow += ";";
		}
	return m_strShow;
}

AVariant CALLBACK ProcTypeMask::OnGetValue(void) const
{
	return m_var;
}

void CALLBACK ProcTypeMask::OnSetValue(const AVariant& var)
{
	m_var = var;
}

/////////////////////////////////////////////////////////////////////////////
// CProcTypeMaskDlg dialog


CProcTypeMaskDlg::CProcTypeMaskDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CProcTypeMaskDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CProcTypeMaskDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CProcTypeMaskDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CProcTypeMaskDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CProcTypeMaskDlg, CDialog)
	//{{AFX_MSG_MAP(CProcTypeMaskDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CProcTypeMaskDlg message handlers

BOOL CProcTypeMaskDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	for (UINT i = 0; i < sizeof(_mask_name) / sizeof(LPCTSTR); i++)
	{
		CButton* pButton = (CButton*)GetDlgItem(IDC_PROC_CHECK1 + i);
		pButton->SetWindowText(_mask_name[i]);
		pButton->EnableWindow(i!=13);
		if (m_ulProcTypeMask & (1 << i))
			pButton->SetCheck(BST_CHECKED);
	}
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CProcTypeMaskDlg::OnOK() 
{
	UpdateData();
	m_ulProcTypeMask = 0;

	for (UINT i = 0; i < sizeof(_mask_name) / sizeof(LPCTSTR); i++)
	{
		CButton* pButton = (CButton*)GetDlgItem(IDC_PROC_CHECK1 + i);
		if (pButton->GetCheck() == BST_CHECKED)
			m_ulProcTypeMask |= 1 << i;
	}

	CDialog::OnOK();
}
