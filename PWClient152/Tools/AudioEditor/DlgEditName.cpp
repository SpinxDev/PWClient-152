// DlgEditName.cpp : 实现文件
//

#include "stdafx.h"
#include "AudioEditor.h"
#include "DlgEditName.h"
#include "Global.h"


// CDlgEditName 对话框

IMPLEMENT_DYNAMIC(CDlgEditName, CBCGPDialog)

CDlgEditName::CDlgEditName(const char* szTitle, CWnd* pParent /*=NULL*/)
	: CBCGPDialog(CDlgEditName::IDD, pParent)
	, m_strName(_T(""))
	, m_strTitle(szTitle)
{

}

CDlgEditName::~CDlgEditName()
{
}

void CDlgEditName::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_NAME, m_strName);
	DDV_MaxChars(pDX, m_strName, 32);
}


BEGIN_MESSAGE_MAP(CDlgEditName, CBCGPDialog)
	ON_BN_CLICKED(IDOK, &CDlgEditName::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CDlgEditName::OnBnClickedCancel)
END_MESSAGE_MAP()


// CDlgEditName 消息处理程序

void CDlgEditName::OnBnClickedOk()
{
	UpdateData(true);
	if(!GF_CheckName(m_strName, GetSafeHwnd()))
		return;
	CBCGPDialog::OnOK();
}

void CDlgEditName::OnBnClickedCancel()
{
	CBCGPDialog::OnCancel();
}

BOOL CDlgEditName::OnInitDialog()
{
	CBCGPDialog::OnInitDialog();
	EnableVisualManagerStyle();
	SetWindowText(m_strTitle);
	GetDlgItem(IDC_EDIT_NAME)->SetFocus();
	return FALSE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}
