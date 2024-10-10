#pragma once
#include "wndbase.h"
#include "BounceButton.h"

using AudioEngine::Effect;
using AudioEngine::EventLayerInstance;
using AudioEngine::EventLayer;

class CWndEffect : public CWndBase
{
public:
	CWndEffect(void);
	~CWndEffect(void);
public:
	void SetEffect(Effect* pEffect, EventLayer* pEventLayer);
	void SetEventLayerInstance(EventLayerInstance* pEventLayerInstance);
	void Reset();
	bool GetEffectCheckState() const { return !m_btnCheckBox.IsButtonDown(); }
protected:
	DECLARE_MESSAGE_MAP()
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnClickProp();
	afx_msg void OnClickEdit();
	afx_msg void OnDeleteEffect();
	afx_msg void OnClickCheck();
	afx_msg void OnSavePreset();
	afx_msg void OnLoadPreset();
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
protected:
	virtual void DrawRealize(CDC* pDC);
	virtual void EndDraw();
protected:
	CBCGPButton m_btnProp;
	CBCGPButton m_btnEdit;
	CBounceButton m_btnCheckBox;
	Effect* m_pEffect;
	EventLayerInstance* m_pEventLayerInstance;
	EventLayer* m_pEventLayer;
	bool m_bEnable;
	bool m_bEditing;
};
