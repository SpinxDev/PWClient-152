#pragma once

#include <vector>

using AudioEngine::PropCurves;
using AudioEngine::EventParameter;

class CWndCurve;
class CWndDot;

typedef std::vector<CWndDot*> DotArray;

class CEffectProp
{
public:
	CEffectProp(void);
	~CEffectProp(void);
public:
	bool						Init(EventParameter* pEventParameter, PropCurves* pPropCurves, COLORREF clr);
	bool						Create(CWnd* pParentWnd, const RECT& rect);
	void						MoveWindow(LPCRECT lpRect);
protected:
	void						release();
protected:
	CWndCurve*					m_pWndCurve;
	DotArray					m_arrDot;
	PropCurves*					m_pPropCurves;
	COLORREF					m_bkColor;
};
