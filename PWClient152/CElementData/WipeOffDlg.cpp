// WipeOffDlg.cpp : implementation file
//

#include "stdafx.h"
#include "elementdata.h"
#include "WipeOffDlg.h"
#include "BaseDataIDMan.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CWipeOffDlg dialog


CWipeOffDlg::CWipeOffDlg(CStringArray* pArray, CUIntArray* pIDArray, CWnd* pParent /*=NULL*/)
	: m_pWipeOffArray(pArray), m_pIDArray(pIDArray), CDialog(CWipeOffDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CWipeOffDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CWipeOffDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CWipeOffDlg)
	DDX_Control(pDX, IDC_WIPEOFF_LIST, m_WipeOffLst);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CWipeOffDlg, CDialog)
	//{{AFX_MSG_MAP(CWipeOffDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWipeOffDlg message handlers

BOOL CWipeOffDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	m_WipeOffLst.SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);
	m_WipeOffLst.InsertColumn(0, "ID", LVCFMT_LEFT, 50);
	m_WipeOffLst.InsertColumn(1, "Â·¾¶", LVCFMT_LEFT, 420);

	int nCount = 0;
	for (int i = 0; i < m_pWipeOffArray->GetSize(); i++)
	{
		CString strId;
		strId.Format("%d", m_pIDArray->GetAt(i));
		m_WipeOffLst.InsertItem(nCount, strId);
		m_WipeOffLst.SetItemText(nCount, 1, m_pWipeOffArray->GetAt(i));
		nCount++;
	}
		
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CWipeOffDlg::OnOK() 
{
	if (AfxMessageBox("È·¶¨Âð£¿", MB_YESNO) == IDYES)
		CDialog::OnOK();
}
