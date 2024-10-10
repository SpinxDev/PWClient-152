// DlgSetParamValue.cpp : 实现文件
//

#include "stdafx.h"
#include "AudioEditor.h"
#include "DlgSetParamValue.h"


// CDlgSetParamValue 对话框

IMPLEMENT_DYNAMIC(CDlgSetParamValue, CBCGPDialog)

CDlgSetParamValue::CDlgSetParamValue(float fValue, CWnd* pParent /*=NULL*/)
	: CBCGPDialog(CDlgSetParamValue::IDD, pParent)
	, m_fValue(fValue)
{

}

CDlgSetParamValue::~CDlgSetParamValue()
{
}

void CDlgSetParamValue::DoDataExchange(CDataExchange* pDX)
{
	CBCGPDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_PARAM_VALUE, m_fValue);
}


BEGIN_MESSAGE_MAP(CDlgSetParamValue, CBCGPDialog)
	ON_BN_CLICKED(IDOK, &CDlgSetParamValue::OnBnClickedOk)
END_MESSAGE_MAP()


// CDlgSetParamValue 消息处理程序

BOOL CDlgSetParamValue::OnInitDialog()
{
	CBCGPDialog::OnInitDialog();

	EnableVisualManagerStyle();
	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CDlgSetParamValue::OnBnClickedOk()
{
	UpdateData(TRUE);
	CBCGPDialog::OnOK();
}
