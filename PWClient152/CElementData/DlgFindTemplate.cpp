// DlgFindTemplate.cpp : implementation file
//

#include "stdafx.h"
#include "elementdata.h"
#include "DlgFindTemplate.h"
#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CString	CDlgFindTemplate::m_strLast;
CDlgFindTemplate::FIND_TYPE	CDlgFindTemplate::m_typeLast = FT_TEMPL;

/////////////////////////////////////////////////////////////////////////////
// CDlgFindTemplate dialog


CDlgFindTemplate::CDlgFindTemplate(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgFindTemplate::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgFindTemplate)
	m_strText = _T("");
	//}}AFX_DATA_INIT
}


void CDlgFindTemplate::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgFindTemplate)
	DDX_Control(pDX, IDC_COMBO1, m_comboType);
	DDX_Control(pDX, IDC_EDIT1, m_editText);
	DDX_Text(pDX, IDC_EDIT1, m_strText);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgFindTemplate, CDialog)
	//{{AFX_MSG_MAP(CDlgFindTemplate)
	ON_BN_CLICKED(IDOK, OnFind)
	ON_WM_SHOWWINDOW()
	ON_WM_CANCELMODE()
	ON_BN_CLICKED(IDC_BUTTON1, OnButton1)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgFindTemplate message handlers

void CDlgFindTemplate::OnFind() 
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);

	if (m_strText.IsEmpty())
	{
		m_editText.SetFocus();
		return ;
	}

	RememberLast();

	OnOK();
}

LPCTSTR	_combo_type[] = 
{
	_T("普通"),
	_T("附加属性"),
	_T("配方"),
	_T("Config"),
	_T("Face")
};

BOOL CDlgFindTemplate::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	for (int i = 0; i < sizeof(_combo_type)/sizeof(_combo_type[0]); ++ i)
	{
		m_comboType.AddString(_combo_type[i]);
	}
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgFindTemplate::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	CDialog::OnShowWindow(bShow, nStatus);
	RestoreLast();
	m_editText.SetFocus();
}

void CDlgFindTemplate::OnCancel()
{
	RememberLast();
	CDialog::OnCancel();
}

void CDlgFindTemplate::RememberLast()
{
	//	保存本次输入内容到静态变量
	m_editText.GetWindowText(m_strLast);
	m_typeLast = static_cast<FIND_TYPE>(m_comboType.GetCurSel());
}

void CDlgFindTemplate::RestoreLast()
{
	//	恢复上次输入内容
	m_editText.SetWindowText(m_strLast);
	m_editText.SetSel(0, -1);
	m_comboType.SetCurSel(m_typeLast);
}

void CDlgFindTemplate::OnButton1() 
{
	CMainFrame* pMain = (CMainFrame*)AfxGetApp()->m_pMainWnd;
	pMain->UpdateSearchFile();
}
