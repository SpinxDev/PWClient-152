#pragma once
#include "wndbase.h"
#include <vector>

using AudioEngine::EventLayer;
using AudioEngine::Effect;
using AudioEngine::PropCurves;

class CWndSoundInstance;
class CEffectProp;
class CWndLabelNoPic;

typedef std::vector<CWndSoundInstance*> WndSoundInsArray;
typedef std::vector<CEffectProp*> EffectPropArray;

class CWndLayerRight : public CWndBase
{
public:
	CWndLayerRight(void);
	~CWndLayerRight(void);
public:
	void					SetEventLayer(EventLayer* pEventLayer);
	void					SetData(float fMinValue, float fMaxValue, float fPerPixelValue, float fInterval);
	void					EditEffect(Effect* pEffect);
	void					UpdateCurves(bool bRecreate);
	void					EditCurve(PropCurves* pPropCurves, int idx);
	void					EditDot(PropCurves* pPropCurves, int idx);
	void					MoveOver(PropCurves* pPropCurves, CPoint pt);
protected:
	DECLARE_MESSAGE_MAP()
	afx_msg void			OnSize(UINT nType, int cx, int cy);
	afx_msg void			OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void			OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void			OnMouseMove(UINT nFlags, CPoint point);
	virtual BOOL			PreTranslateMessage(MSG* pMsg);
	LRESULT					OnMoveSoundIns(WPARAM wParam, LPARAM lParam);
	LRESULT					OnResizeLeftSoundIns(WPARAM wParam, LPARAM lParam);
	LRESULT					OnResizeRightSoundIns(WPARAM wParam, LPARAM lParam);
protected:
	virtual void			DrawRealize(CDC* pDC);
	virtual void			EndDraw();
	bool					UpdateWindows();
	void					release();
protected:
	EventLayer*				m_pEventLayer;
	WndSoundInsArray		m_arrWndSoundIns;
	float					m_fMinValue;
	float					m_fMaxValue;
	float					m_fPerPixelValue;
	float					m_fInterval;
	EffectPropArray			m_arrEffectProp;
	Effect*					m_pEffect;
	bool					m_bLButtonDown;
	PropCurves*				m_pEditingPropCurves;
	bool					m_bEditingCurve;
	bool					m_bEditingDot;
	int						m_iEditIndex;
	::POINT					m_ptLast;
	CWndLabelNoPic*			m_pWndEffectCurveLabel;

	CPoint					m_ptMoveOver;
	int						m_iMoveOverDotIndex;
	PropCurves*				m_pMoveOverPropCurves;
};
