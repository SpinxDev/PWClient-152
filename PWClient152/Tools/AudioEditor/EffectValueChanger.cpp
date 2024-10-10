#include "stdafx.h"
#include "EffectValueChanger.h"
#include "FPropCurves.h"
#include "Global.h"

CEffectValueChanger::CEffectValueChanger(PropCurves* pPropCurves, CWnd* pWnd)
: m_pPropCurves(pPropCurves)
, m_pWnd(pWnd)
{
}

CEffectValueChanger::~CEffectValueChanger(void)
{
}

void CEffectValueChanger::ScreenToLogical(int x, int y, float& fx, float& fy)
{	
	if(!m_pPropCurves || !m_pWnd)
		return;
	CRect rc;
	m_pWnd->GetClientRect(&rc);
	if(rc.Width()==0 || rc.Height()==0)
		return;
	fx = (float)(x-rc.left)/(rc.right-rc.left);
	fy = (float)(y-rc.bottom)/(rc.top-rc.bottom);
	float fMin = m_pPropCurves->GetMin();
	float fMax = m_pPropCurves->GetMax();
	fy = fy*(fMax-fMin)+fMin;

	fx = GF_Clamp(0.0f, 1.0f, fx);
	fy = GF_Clamp(m_pPropCurves->GetMin(), m_pPropCurves->GetMax(), fy);
}

void CEffectValueChanger::LogicalToScreen(float fx, float fy, int& x, int& y)
{
	if(!m_pPropCurves || !m_pWnd)
		return;
	CRect rc;
	m_pWnd->GetClientRect(&rc);
	float fMin = m_pPropCurves->GetMin();
	float fMax = m_pPropCurves->GetMax();
	fy = (fy-fMin)/(fMax-fMin);
	x = rc.left + (int)((rc.right-rc.left)*fx+0.5f);
	y = rc.bottom + (int)((rc.top-rc.bottom)*fy-0.5f);
	x = GF_Clamp((int)rc.left, (int)rc.right, x);
	y = GF_Clamp((int)rc.top, (int)rc.bottom, y);
}