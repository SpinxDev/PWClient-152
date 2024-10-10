// DlgLayerProp.cpp : 实现文件
//

#include "stdafx.h"
#include "AudioEditor.h"
#include "DlgLayerProp.h"
#include "FEvent.h"
#include "FEventLayer.h"
#include "FEventParameter.h"
#include "Global.h"


// CDlgLayerProp 对话框

IMPLEMENT_DYNAMIC(CDlgLayerProp, CBCGPDialog)

CDlgLayerProp::CDlgLayerProp(Event* pEvent, CWnd* pParent /*=NULL*/)
	: CBCGPDialog(CDlgLayerProp::IDD, pParent)
	, m_strName(_T(""))
	, m_strParamName(_T(""))
	, m_pEvent(pEvent)
	, m_pEventLayer(0)
{

}

CDlgLayerProp::~CDlgLayerProp()
{
}

void CDlgLayerProp::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_LAYER_NAME, m_strName);
	DDX_CBString(pDX, IDC_PARAM_COMBO, m_strParamName);
	DDX_Control(pDX, IDC_PARAM_COMBO, m_wndParamCombo);
}


BEGIN_MESSAGE_MAP(CDlgLayerProp, CBCGPDialog)
	ON_BN_CLICKED(IDOK, &CDlgLayerProp::OnBnClickedOk)
END_MESSAGE_MAP()


// CDlgLayerProp 消息处理程序

BOOL CDlgLayerProp::OnInitDialog()
{
	CBCGPDialog::OnInitDialog();
	EnableVisualManagerStyle();

	int iParamNum = m_pEvent->GetParameterNum();
	for (int i=0; i<iParamNum; ++i)
	{
		m_wndParamCombo.AddString(m_pEvent->GetParameterByIndex(i)->GetName());
	}
	m_wndParamCombo.SetCurSel(0);
	if(m_pEventLayer)
	{
		m_strName = m_pEventLayer->GetName();
		m_strParamName = m_pEventLayer->GetParameterName();
		UpdateData(FALSE);
	}
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CDlgLayerProp::OnBnClickedOk()
{
	UpdateData(TRUE);
	if(!GF_CheckName(m_strName, GetSafeHwnd()))
		return;
	if(m_strParamName.GetLength() == 0)
	{
		MessageBox("请选择一个参数", "警告", MB_ICONWARNING);
		return;
	}	
	CBCGPDialog::OnOK();
}
