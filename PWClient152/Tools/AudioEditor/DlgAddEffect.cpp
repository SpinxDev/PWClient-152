// DlgAddEffect.cpp : 实现文件
//

#include "stdafx.h"
#include "AudioEditor.h"
#include "DlgAddEffect.h"
#include "FAudioEngine.h"


// CDlgAddEffect 对话框

IMPLEMENT_DYNAMIC(CDlgAddEffect, CBCGPDialog)

CDlgAddEffect::CDlgAddEffect(CWnd* pParent /*=NULL*/)
	: CBCGPDialog(CDlgAddEffect::IDD, pParent)
{

}

CDlgAddEffect::~CDlgAddEffect()
{
}

void CDlgAddEffect::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_EFFECT, m_wndListBox);
}


BEGIN_MESSAGE_MAP(CDlgAddEffect, CBCGPDialog)
	ON_BN_CLICKED(IDOK, &CDlgAddEffect::OnBnClickedOk)
END_MESSAGE_MAP()


// CDlgAddEffect 消息处理程序

void CDlgAddEffect::OnBnClickedOk()
{
	int idx = m_wndListBox.GetCurSel();
	if(idx == -1)
		m_EffectType = AudioEngine::EFFECT_TYPE_NONE;
	else
		m_EffectType = (EFFECT_TYPE)m_wndListBox.GetItemData(idx);
	CBCGPDialog::OnOK();
}

BOOL CDlgAddEffect::OnInitDialog()
{
	CBCGPDialog::OnInitDialog();

	EnableVisualManagerStyle();
	int idx = m_wndListBox.AddString("Pan");
	m_wndListBox.SetItemData(idx, (DWORD_PTR)AudioEngine::EFFECT_TYPE_PAN);

	idx = m_wndListBox.AddString("Surround Pan");
	m_wndListBox.SetItemData(idx, (DWORD_PTR)AudioEngine::EFFECT_TYPE_SURROUND_PAN);

	idx = m_wndListBox.AddString("Volume");
	m_wndListBox.SetItemData(idx, (DWORD_PTR)AudioEngine::EFFECT_TYPE_VOLUME);

	idx = m_wndListBox.AddString("Pitch");
	m_wndListBox.SetItemData(idx, (DWORD_PTR)AudioEngine::EFFECT_TYPE_PITCH);

	idx = m_wndListBox.AddString("ParamEQ");
	m_wndListBox.SetItemData(idx, (DWORD_PTR)AudioEngine::EFFECT_TYPE_PARAMEQ);

	idx = m_wndListBox.AddString("Compressor");
	m_wndListBox.SetItemData(idx, (DWORD_PTR)AudioEngine::EFFECT_TYPE_COMPRESSOR);

	idx = m_wndListBox.AddString("Echo");
	m_wndListBox.SetItemData(idx, (DWORD_PTR)AudioEngine::EFFECT_TYPE_ECHO);

	idx = m_wndListBox.AddString("Occlusion");
	m_wndListBox.SetItemData(idx, (DWORD_PTR)AudioEngine::EFFECT_TYPE_OCCLUSION);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}
