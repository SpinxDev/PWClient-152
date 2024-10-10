#pragma once
#include "wndbase.h"

using AudioEngine::PropCurves;
using AudioEngine::EventParameter;

class CWndDot : public CWndBase
{
public:
	CWndDot(void);
	~CWndDot(void);
public:
	bool				Init(EventParameter* pEventParameter, PropCurves* pPropCurves, int idx);
	void				SetIndex(int idx);
protected:
	virtual void		DrawRealize(CDC* pDC);
protected:
	DECLARE_MESSAGE_MAP()
	afx_msg void		OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void		OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void		OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void		OnDelPoint();
	afx_msg void		OnSetValue();
protected:
	HCURSOR				m_hHandCursor;
	HCURSOR				m_hOldCursor;
	PropCurves*			m_pPropCurves;
	EventParameter*		m_pEventParameter;
	int					m_iIndex;
};
