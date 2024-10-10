// DlgParamProp.cpp : 实现文件
//

#include "stdafx.h"
#include "AudioEditor.h"
#include "DlgParamProp.h"
#include "Global.h"


// CDlgParamProp 对话框

IMPLEMENT_DYNAMIC(CDlgParamProp, CBCGPDialog)

CDlgParamProp::CDlgParamProp(CWnd* pParent /*=NULL*/)
	: CBCGPDialog(CDlgParamProp::IDD, pParent)
	, m_strName(_T(""))
	, m_fMinValue(0)
	, m_fMaxValue(1)
	, m_fSeekSpeed(0)
{

}

CDlgParamProp::~CDlgParamProp()
{
}

void CDlgParamProp::DoDataExchange(CDataExchange* pDX)
{
	CBCGPDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_PARAM_NAME, m_strName);
	DDX_Text(pDX, IDC_MIN_VALUE, m_fMinValue);
	DDX_Text(pDX, IDC_MAX_VALUE, m_fMaxValue);
	DDX_Text(pDX, IDC_PARAM_VEL, m_fVelocity);
	DDX_Text(pDX, IDC_MAX_VALUE, m_fMaxValue);
	DDX_Control(pDX, IDC_COMBO_PARAM_LOOP_TYPE, m_wmdCombo);
	DDX_Text(pDX, IDC_PARAM_SEEKSPEED, m_fSeekSpeed);
}


BEGIN_MESSAGE_MAP(CDlgParamProp, CBCGPDialog)
	ON_BN_CLICKED(IDOK, &CDlgParamProp::OnBnClickedOk)
END_MESSAGE_MAP()


// CDlgParamProp 消息处理程序

BOOL CDlgParamProp::OnInitDialog()
{
	CBCGPDialog::OnInitDialog();

	EnableVisualManagerStyle();
	UpdateData(FALSE);
	m_wmdCombo.AddString("播放一次");
	m_wmdCombo.AddString("播放一次并停止事件");
	m_wmdCombo.AddString("循环");
	m_wmdCombo.SetCurSel(m_iLoopMode);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CDlgParamProp::SetProp(const char* szName, float fMinValue, float fMaxValue, float fVelocity, int iLoopMode, float fSeekSpeed)
{
	m_strName = szName;
	m_fMinValue = fMinValue;
	m_fMaxValue = fMaxValue;
	m_fVelocity = fVelocity;
	m_iLoopMode = iLoopMode;
	m_fSeekSpeed = fSeekSpeed;
}

void CDlgParamProp::GetProp(CString& strName, float& fMinValue, float& fMaxValue, float& fVelocity, int& iLoopMode, float& fSeekSpeed)
{
	strName = m_strName;
	fMinValue = m_fMinValue;
	fMaxValue = m_fMaxValue;
	fVelocity = m_fVelocity;
	iLoopMode = m_iLoopMode;
	fSeekSpeed = m_fSeekSpeed;
}

void CDlgParamProp::OnBnClickedOk()
{
	UpdateData(TRUE);
	if(!GF_CheckName(m_strName, GetSafeHwnd()))
		return;
	m_iLoopMode = m_wmdCombo.GetCurSel();
	CBCGPDialog::OnOK();
}
