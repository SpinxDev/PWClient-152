#pragma once
#include "afxwin.h"
#include "FAudioEngine.h"

using AudioEngine::EFFECT_TYPE;

// CDlgAddEffect 对话框

class CDlgAddEffect : public CBCGPDialog
{
	DECLARE_DYNAMIC(CDlgAddEffect)

public:
	CDlgAddEffect(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgAddEffect();
public:
	EFFECT_TYPE GetEffectType() const { return m_EffectType; }

// 对话框数据
	enum { IDD = IDD_ADD_EFFECT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	afx_msg void OnBnClickedOk();
	virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()
protected:
	CBCGPListBox m_wndListBox;
	EFFECT_TYPE m_EffectType;
};
