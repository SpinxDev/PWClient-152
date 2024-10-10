// DlgSetParamValue.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "AudioEditor.h"
#include "DlgSetParamValue.h"


// CDlgSetParamValue �Ի���

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


// CDlgSetParamValue ��Ϣ�������

BOOL CDlgSetParamValue::OnInitDialog()
{
	CBCGPDialog::OnInitDialog();

	EnableVisualManagerStyle();
	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}

void CDlgSetParamValue::OnBnClickedOk()
{
	UpdateData(TRUE);
	CBCGPDialog::OnOK();
}
