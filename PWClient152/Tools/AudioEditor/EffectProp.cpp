#include "stdafx.h"
#include "FPropCurves.h"
#include "WndCurve.h"
#include "WndDot.h"
#include "EffectProp.h"

CEffectProp::CEffectProp(void)
: m_pWndCurve(0)
, m_pPropCurves(0)
{
}

CEffectProp::~CEffectProp(void)
{
	release();
}

bool CEffectProp::Init(EventParameter* pEventParameter, PropCurves* pPropCurves, COLORREF clr)
{
	if(!pPropCurves || !pEventParameter)
		return false;
	m_pPropCurves = pPropCurves;
	m_bkColor = clr;
	m_pWndCurve = new CWndCurve;
	if(!m_pWndCurve->Init(m_pPropCurves))
	{
		delete m_pWndCurve;
		m_pWndCurve = 0;
		return false;
	}
	int iPointNum = m_pPropCurves->GetPointNum();
	for (int i=0; i<iPointNum; ++i)
	{
		CWndDot* pWndDot = new CWndDot;
		if(!pWndDot->Init(pEventParameter, m_pPropCurves, i))
		{
			delete pWndDot;
			return false;
		}
		m_arrDot.push_back(pWndDot);
	}
	return true;
}

void CEffectProp::release()
{
	if(m_pWndCurve && m_pWndCurve->GetSafeHwnd())
		m_pWndCurve->DestroyWindow();
	delete m_pWndCurve;
	for (size_t i=0; i<m_arrDot.size(); ++i)
	{
		if(m_arrDot[i] && m_arrDot[i]->GetSafeHwnd())
			m_arrDot[i]->DestroyWindow();
		delete m_arrDot[i];
	}
	m_arrDot.clear();
}

bool CEffectProp::Create(CWnd* pParentWnd, const RECT& rect)
{
	for (size_t i=0; i<m_arrDot.size(); ++i)
	{
		if(!m_arrDot[i]->Create(pParentWnd, rect))
			return false;
	}

	if(!m_pWndCurve->Create(pParentWnd, rect))
		return false;	
	return true;
}

void CEffectProp::MoveWindow(LPCRECT lpRect)
{
	m_pWndCurve->MoveWindow(lpRect);
	m_pWndCurve->SetBKColor(m_bkColor);
	for (size_t i=0; i<m_arrDot.size(); ++i)
	{
		m_arrDot[i]->MoveWindow(lpRect);
		m_arrDot[i]->SetBKColor(m_bkColor);
	}
}