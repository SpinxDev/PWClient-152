#include "stdafx.h"
#include "WndLayerRight.h"
#include "FEventLayer.h"
#include "WndSoundInstance.h"
#include "FSoundInstanceTemplate.h"
#include "FEffect.h"
#include "EffectProp.h"
#include "EffectValueChanger.h"
#include "FPropCurves.h"
#include "Engine.h"
#include "Project.h"
#include "WndLabelNoPic.h"
#include "WndDefine.h"
#include "CheckSoundInsProp.h"

using AudioEngine::CVPOINT;

#define TIP_WINDOW_HEIGHT 25
#define TIP_WINDOW_WIDTH 120

const COLORREF c_clrArray[5] = 
{
	RGB(255, 0, 0),
	RGB(0, 255, 0),
	RGB(0, 0, 255),
	RGB(255, 255, 0),
	RGB(255, 0, 255)
};

BEGIN_MESSAGE_MAP(CWndLayerRight, CWndBase)
	ON_WM_SIZE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_MESSAGE(WM_RESIZE_SOUNDINS_LEFT, OnResizeLeftSoundIns)
	ON_MESSAGE(WM_RESIZE_SOUNDINS_RIGHT, OnResizeRightSoundIns)
	ON_MESSAGE(WM_MOVE_SOUNDINS, OnMoveSoundIns)
END_MESSAGE_MAP()

CWndLayerRight::CWndLayerRight(void)
: m_pEventLayer(0)
, m_pEffect(0)
{
	m_clBK = RGB(128,128,128);
	m_bLButtonDown = false;
	m_pEditingPropCurves = 0;
	m_bEditingCurve = false;
	m_bEditingDot = false;
	m_iEditIndex = -1;
	m_pWndEffectCurveLabel = 0;
	m_iMoveOverDotIndex = -1;
	m_pMoveOverPropCurves = 0;
}

CWndLayerRight::~CWndLayerRight(void)
{
	release();
}

void CWndLayerRight::SetEventLayer(EventLayer* pEventLayer)
{
	if(!pEventLayer)
		return;
	m_pEventLayer = pEventLayer;
	UpdateWindows();
}

void CWndLayerRight::DrawRealize(CDC* pDC)
{	
}

void CWndLayerRight::EndDraw()
{

}

bool CWndLayerRight::UpdateWindows()
{
	if(!m_pEventLayer)
		return false;
	release();

	m_pWndEffectCurveLabel = new CWndLabelNoPic;
	if(!m_pWndEffectCurveLabel->Create(this, CRect(0,0,0,0)))
	{
		delete m_pWndEffectCurveLabel;
		return false;
	}

	if(m_pEffect)
	{
		int iParamNum = m_pEffect->GetNumParameters();
		for (int i=0; i<iParamNum; ++i)
		{
			PropCurves* pPropCurves = m_pEffect->GetPropCurve(i);
			if(!pPropCurves)
				return false;
			CEffectProp* pEffectProp = new CEffectProp;
			if(!pEffectProp->Init(m_pEventLayer->GetParameter(), pPropCurves, c_clrArray[i]))
			{
				delete pEffectProp;
				return false;
			}
			if(!pEffectProp->Create(this, CRect(0,0,0,0)))
			{
				delete pEffectProp;
				return false;
			}
			m_arrEffectProp.push_back(pEffectProp);
		}
	}	

	int iSoundInsNum = m_pEventLayer->GetSoundInstanceTemplateNum();
	for (int i=0; i<iSoundInsNum; ++i)
	{
		SoundInstanceTemplate* pSoundInstanceTemplate = m_pEventLayer->GetSoundInstanceTemplateByIndex(i);
		if(!pSoundInstanceTemplate)
			continue;
		CWndSoundInstance* pWndSoundIns = new CWndSoundInstance;
		pWndSoundIns->SetSoundInstance(pSoundInstanceTemplate);
		if(!pWndSoundIns->Create(this, CRect(0,0,0,0)))
		{
			delete pWndSoundIns;
			return false;
		}
		m_arrWndSoundIns.push_back(pWndSoundIns);
	}	
	return true;
}

void CWndLayerRight::release()
{
	delete m_pWndEffectCurveLabel;
	m_pWndEffectCurveLabel = 0;

	for (size_t i=0; i<m_arrWndSoundIns.size(); ++i)
	{
		m_arrWndSoundIns[i]->DestroyWindow();
		delete m_arrWndSoundIns[i];
	}
	m_arrWndSoundIns.clear();

	for (size_t i=0; i<m_arrEffectProp.size(); ++i)
	{
		delete m_arrEffectProp[i];		
	}
	m_arrEffectProp.clear();
}

void CWndLayerRight::OnSize(UINT nType, int cx, int cy)
{	
	CWndBase::OnSize(nType, cx, cy);

	if(!m_pEventLayer)
		return;

	CRect rc;	
	GetClientRect(&rc);

	for (size_t i=0; i<m_arrEffectProp.size(); ++i)
	{
		m_arrEffectProp[i]->MoveWindow(&rc);
	}

	rc.top += 3;
	rc.bottom -= 3;
	int size = (int)m_arrWndSoundIns.size();
	for (int i=0; i<size; ++i)
	{
		SoundInstanceTemplate* pSoundInstanceTemplate = m_pEventLayer->GetSoundInstanceTemplateByIndex(i);
		if(!pSoundInstanceTemplate)
			continue;
		SOUND_INSTANCE_PROPERTY prop;
		pSoundInstanceTemplate->GetProperty(prop);
		rc.left = (int)(prop.fStartPosition*(m_fMaxValue-m_fMinValue)/m_fPerPixelValue+0.5f);
		rc.right = rc.left + (int)(prop.fLength*(m_fMaxValue-m_fMinValue)/m_fPerPixelValue+0.5f);
		m_arrWndSoundIns[i]->MoveWindow(&rc);
	}
}

void CWndLayerRight::SetData(float fMinValue, float fMaxValue, float fPerPixelValue, float fInterval)
{
	m_fMinValue = fMinValue;
	m_fMaxValue = fMaxValue;
	m_fPerPixelValue = fPerPixelValue;
	m_fInterval = fInterval;
	Update();
}

void CWndLayerRight::EditEffect(Effect* pEffect)
{
	m_pEffect = pEffect;
	UpdateWindows();
	SendMessage(WM_SIZE);
}

void CWndLayerRight::UpdateCurves(bool bRecreate)
{
	if(bRecreate)
		UpdateWindows();
	UpdateWindow();
	SendMessage(WM_SIZE);
}

void CWndLayerRight::OnLButtonDown(UINT nFlags, CPoint point)
{
	SetCapture();
	m_bLButtonDown = true;
	m_ptLast.x = point.x;
	m_ptLast.y = point.y;
	CWndBase::OnLButtonDown(nFlags, point);
}

void CWndLayerRight::OnLButtonUp(UINT nFlags, CPoint point)
{
	if(GetCapture() == this)
		ReleaseCapture();
	m_bLButtonDown = false;
	m_pEditingPropCurves = 0;
	m_bEditingCurve = false;
	m_bEditingDot = false;
	m_iEditIndex = -1;
	m_ptLast.x = 0;
	m_ptLast.y = 0;
	CWndBase::OnLButtonUp(nFlags, point);
}

void CWndLayerRight::OnMouseMove(UINT nFlags, CPoint point)
{
	if(m_pMoveOverPropCurves && abs(point.x-m_ptMoveOver.x)<10 && abs(point.y-m_ptMoveOver.y)<10)
	{
		CRect rc;
		rc.left = point.x;
		rc.right = rc.left + TIP_WINDOW_WIDTH;
		rc.top = point.y;
		rc.bottom = rc.top + TIP_WINDOW_HEIGHT;

		CRect rect;
		GetClientRect(rect);
		if(rect.right - point.x < TIP_WINDOW_WIDTH+10)
		{
			rc.left -= (TIP_WINDOW_WIDTH+10);
			rc.right -= (TIP_WINDOW_WIDTH+10);
		}
		else
		{
			rc.left += 10;
			rc.right += 10;
		}

		if(rect.bottom - point.y < TIP_WINDOW_HEIGHT)
		{
			rc.top -= TIP_WINDOW_HEIGHT;
			rc.bottom -= TIP_WINDOW_HEIGHT;
		}
		CEffectValueChanger ec(m_pMoveOverPropCurves, this);
		CVPOINT pt;
		ec.ScreenToLogical(point.x, point.y, pt.x, pt.y);
		CString csLabel;
		csLabel.Format("%s: %.3f %s", m_pMoveOverPropCurves->GetName(), m_pMoveOverPropCurves->GetValue(pt.x), m_pMoveOverPropCurves->GetUnit());
		m_pWndEffectCurveLabel->SetLabel(csLabel);
		rc.right = rc.left + csLabel.GetLength()*8;
		m_pWndEffectCurveLabel->MoveWindow(&rc);
		m_pWndEffectCurveLabel->ShowWindow(SW_NORMAL);
	}
	else
	{
		m_pMoveOverPropCurves = 0;
		m_pWndEffectCurveLabel->ShowWindow(SW_HIDE);
	}
	if(!m_bLButtonDown || !m_pEditingPropCurves || (!m_bEditingCurve && !m_bEditingDot))
		return;
	CWndBase::OnMouseMove(nFlags, point);
	CEffectValueChanger ec(m_pEditingPropCurves, this);	
	if(m_bEditingDot)
	{
		CVPOINT pt;
		ec.ScreenToLogical(point.x, point.y, pt.x, pt.y);
		int iPointNum = m_pEditingPropCurves->GetPointNum();
		if(m_iEditIndex>0)
		{
			CVPOINT prePt = m_pEditingPropCurves->GetPointByIndex(m_iEditIndex-1);
			if(pt.x < prePt.x)
				pt.x = prePt.x;
		}

		if(m_iEditIndex < iPointNum-1)
		{
			CVPOINT nextPt = m_pEditingPropCurves->GetPointByIndex(m_iEditIndex+1);
			if(pt.x > nextPt.x)
				pt.x = nextPt.x;
		}
		m_pEditingPropCurves->SetPoint(m_iEditIndex, pt);
		UpdateCurves(false);
	}
	else if(m_bEditingCurve)
	{
		float fx;
		float fy0;
		float fy1;
		ec.ScreenToLogical(0, m_ptLast.y, fx, fy0);
		ec.ScreenToLogical(0, point.y, fx, fy1);
		if(m_iEditIndex != -1)
		{
			m_pEditingPropCurves->MoveCurve(m_iEditIndex, fy1-fy0);
		}
		else
		{
			m_pEditingPropCurves->MoveWholeCurve(fy1-fy0);
		}		
		UpdateCurves(false);
		m_ptLast.x = point.x;
		m_ptLast.y = point.y;
	}
	Engine::GetInstance().GetCurProject()->SetModified(true);
}

void CWndLayerRight::EditCurve(PropCurves* pPropCurves, int idx)
{
	m_pEditingPropCurves = pPropCurves;
	m_iEditIndex = idx;
	m_bEditingCurve = true;
	m_bEditingDot = false;
}

void CWndLayerRight::EditDot(PropCurves* pPropCurves, int idx)
{
	m_pEditingPropCurves = pPropCurves;
	m_iEditIndex = idx;
	m_bEditingCurve = false;
	m_bEditingDot = true;
}

void CWndLayerRight::MoveOver(PropCurves* pPropCurves, CPoint pt)
{
	m_pMoveOverPropCurves = pPropCurves;
	m_ptMoveOver = pt;
}

BOOL CWndLayerRight::PreTranslateMessage(MSG* pMsg)
{
	switch(pMsg->message)
	{
	case WM_LBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_MOUSEMOVE:
		{
			::POINT pt;
			GetCursorPos(&pt);
			ScreenToClient(&pt);
			pMsg->lParam = MAKELPARAM(pt.x, pt.y);
			SendMessage(pMsg->message,pMsg->wParam,pMsg->lParam);
		}
		break;
	}
	return CWndBase::PreTranslateMessage(pMsg);
}

LRESULT CWndLayerRight::OnMoveSoundIns(WPARAM wParam, LPARAM lParam)
{
	CWndSoundInstance* pWndSoundInstance = (CWndSoundInstance*)wParam;
	CRect rc;
	pWndSoundInstance->GetWindowRect(&rc);
	ScreenToClient(&rc);
	CRect rcOld = rc;
	rc.left += (LONG)lParam;
	rc.right += (LONG)lParam;
	SoundInstanceTemplate* pSoundInstanceTemplate = pWndSoundInstance->GetSoundInstance();
	SOUND_INSTANCE_PROPERTY prop;
	pSoundInstanceTemplate->GetProperty(prop);
	if(rc.left < 0)
	{
		rc.right = rc.Width();
		rc.left = 0;
		prop.fStartPosition = 0;
	}
	else
	{
		int iMostRight = (int)((m_fMaxValue-m_fMinValue)/m_fPerPixelValue+0.5f);
		if(rc.right > iMostRight)
		{
			rc.left = iMostRight - rc.Width();
			rc.right = iMostRight;
			prop.fStartPosition = 1.0f - prop.fLength;
		}
		else
		{
			prop.fStartPosition = rc.left*m_fPerPixelValue/(m_fMaxValue-m_fMinValue);
		}
	}	

	CCheckSoundInsProp check;
	float fConflictStart;
	float fConflictLength;
	if(!check.IsLegal(prop, pSoundInstanceTemplate, fConflictStart, fConflictLength))
	{
		if(lParam<0)
		{
			prop.fStartPosition = fConflictStart+fConflictLength;
			rc.left = (int)(prop.fStartPosition*(m_fMaxValue-m_fMinValue)/m_fPerPixelValue+0.5f);
			rc.right = rc.left + (int)(prop.fLength*(m_fMaxValue-m_fMinValue)/m_fPerPixelValue+0.5f);
		}
		else if(lParam>0)
		{
			prop.fStartPosition = fConflictStart - prop.fLength;
			rc.left = (int)(prop.fStartPosition*(m_fMaxValue-m_fMinValue)/m_fPerPixelValue+0.5f);
			rc.right = rc.left + (int)(prop.fLength*(m_fMaxValue-m_fMinValue)/m_fPerPixelValue+0.5f);
		}
	}
	pSoundInstanceTemplate->SetProperty(prop);
	Engine::GetInstance().GetCurProject()->SetModified(true);
	pWndSoundInstance->MoveWindow(rc);
	pWndSoundInstance->UpdateWindow();
	return 0;
}

LRESULT CWndLayerRight::OnResizeLeftSoundIns(WPARAM wParam, LPARAM lParam)
{
	CWndSoundInstance* pWndSoundInstance = (CWndSoundInstance*)wParam;
	CRect rc;
	pWndSoundInstance->GetWindowRect(&rc);
	ScreenToClient(&rc);
	rc.left += (LONG)lParam;
	if(rc.left < 0)
		rc.left = 0;
	if(rc.Width() < 10)
		rc.left = rc.right - 10;	
	SoundInstanceTemplate* pSoundInstanceTemplate = pWndSoundInstance->GetSoundInstance();
	SOUND_INSTANCE_PROPERTY prop;
	pSoundInstanceTemplate->GetProperty(prop);
	float fEndPosition = prop.fStartPosition + prop.fLength;
	prop.fStartPosition = rc.left*m_fPerPixelValue/(m_fMaxValue-m_fMinValue);
	prop.fLength = fEndPosition - prop.fStartPosition;
	CCheckSoundInsProp check;
	float fConflictStart;
	float fConflictLength;
	if(!check.IsLegal(prop, pSoundInstanceTemplate, fConflictStart, fConflictLength))
	{
		prop.fLength = prop.fStartPosition+prop.fLength-fConflictStart-fConflictLength;
		prop.fStartPosition = fConflictStart+fConflictLength;
		rc.left = (int)(prop.fStartPosition*(m_fMaxValue-m_fMinValue)/m_fPerPixelValue+0.5f);		
		rc.right = rc.left + (int)(prop.fLength*(m_fMaxValue-m_fMinValue)/m_fPerPixelValue+0.5f);
	}
	pSoundInstanceTemplate->SetProperty(prop);
	Engine::GetInstance().GetCurProject()->SetModified(true);
	pWndSoundInstance->MoveWindow(rc);
	pWndSoundInstance->UpdateWindow();
	return 0;
}

LRESULT CWndLayerRight::OnResizeRightSoundIns(WPARAM wParam, LPARAM lParam)
{
	CWndSoundInstance* pWndSoundInstance = (CWndSoundInstance*)wParam;
	CRect rc;
	pWndSoundInstance->GetWindowRect(&rc);
	ScreenToClient(&rc);
	rc.right += (LONG)lParam;
	int iMostRight = (int)((m_fMaxValue-m_fMinValue)/m_fPerPixelValue+0.5f);
	if(rc.right > iMostRight)
		rc.right = iMostRight;
	if(rc.Width() < 10)
		rc.right = rc.left + 10;

	SoundInstanceTemplate* pSoundInstanceTemplate = pWndSoundInstance->GetSoundInstance();
	SOUND_INSTANCE_PROPERTY prop;
	pSoundInstanceTemplate->GetProperty(prop);
	prop.fLength = (rc.right-rc.left)*m_fPerPixelValue/(m_fMaxValue-m_fMinValue);
	if (prop.fStartPosition + prop.fLength > 1.0f)
	{
		prop.fLength = 1.0f - prop.fStartPosition;
	}
	CCheckSoundInsProp check;
	float fConflictStart;
	float fConflictLength;
	if(!check.IsLegal(prop, pSoundInstanceTemplate, fConflictStart, fConflictLength))
	{
		prop.fLength = fConflictStart - prop.fStartPosition;
		rc.right = rc.left + (int)(prop.fLength*(m_fMaxValue-m_fMinValue)/m_fPerPixelValue+0.5f);
	}
	pSoundInstanceTemplate->SetProperty(prop);
	Engine::GetInstance().GetCurProject()->SetModified(true);
	pWndSoundInstance->MoveWindow(rc);
	pWndSoundInstance->UpdateWindow();
	return 0;
}