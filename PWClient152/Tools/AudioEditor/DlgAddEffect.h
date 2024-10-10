#pragma once
#include "afxwin.h"
#include "FAudioEngine.h"

using AudioEngine::EFFECT_TYPE;

// CDlgAddEffect �Ի���

class CDlgAddEffect : public CBCGPDialog
{
	DECLARE_DYNAMIC(CDlgAddEffect)

public:
	CDlgAddEffect(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDlgAddEffect();
public:
	EFFECT_TYPE GetEffectType() const { return m_EffectType; }

// �Ի�������
	enum { IDD = IDD_ADD_EFFECT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
	afx_msg void OnBnClickedOk();
	virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()
protected:
	CBCGPListBox m_wndListBox;
	EFFECT_TYPE m_EffectType;
};
