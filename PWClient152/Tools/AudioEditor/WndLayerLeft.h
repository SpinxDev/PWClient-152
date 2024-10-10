#pragma once
#include "wndbase.h"
#include "BounceButton.h"
#include <vector>

using AudioEngine::EventLayer;
using AudioEngine::EventLayerInstance;

class CWndEffect;
typedef std::vector<CWndEffect*> WndEffectArrary;

class CWndLayerLeft : public CWndBase
{
public:
public:
	CWndLayerLeft(void);
	~CWndLayerLeft(void);
public:
	void SetCheck(bool bCheck) { m_btnCheckBox.SetButtonState(bCheck); }
	bool GetLayerCheckState() const { return !m_btnCheckBox.IsButtonDown(); }
	bool GetEffectCheckState(int idx) const;
	void SetEventLayer(EventLayer* pEventLayer);
	void SetEventLayerInstance(EventLayerInstance* pEventLayerInstance);
	void Reset();
protected:
	virtual void DrawRealize(CDC* pDC);
	virtual void EndDraw();
	bool UpdateWindows();
	void release();
protected:
	DECLARE_MESSAGE_MAP()
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnClickCheck();
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnLayerProp();
	afx_msg void OnAddEffect();	
	afx_msg void OnDeleteLayer();
	afx_msg void OnAddSoundDef();
protected:
	CBounceButton	m_btnCheckBox;
	EventLayer* m_pEventLayer;
	EventLayerInstance* m_pEventLayerInstance;
	WndEffectArrary m_arrWndEffect;
	bool m_bEnable;
};
