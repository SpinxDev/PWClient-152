#pragma once
#include "wndbase.h"
#include <vector>

using AudioEngine::PropCurves;

typedef std::vector<POINT> PointArray;

class CWndCurve : public CWndBase
{
public:
	CWndCurve(void);
	~CWndCurve(void);
public:
	bool				Init(PropCurves* pPropCurves);
protected:
	DECLARE_MESSAGE_MAP()
	virtual void		DrawRealize(CDC* pDC);	
	afx_msg void		OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void		OnAddPoint();
	afx_msg void		OnChangeCurveType(UINT uID);
	afx_msg void		OnRButtonDown(UINT nFlags, CPoint point);
protected:	
	void				drawPath(CDC* pDC);
protected:
	HCURSOR				m_hHandCursor;
	HCURSOR				m_hOldCursor;
	PointArray			m_arrPoints;
	PropCurves*			m_pPropCurves;
	float				m_fRClickX;
	float				m_fRClickY;
public:
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
};
