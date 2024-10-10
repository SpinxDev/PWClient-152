#include "stdafx.h"
#include "WndEvent.h"
#include "WndParam.h"
#include "FEvent.h"
#include "WndScale.h"
#include "WndDefine.h"
#include "WndLayers.h"
#include "FEventInstance.h"
#include "WndLayerLeft.h"

#define MENU_ADD_LAYER		0x2001

using AudioEngine::EventLayer;

BEGIN_MESSAGE_MAP(CWndEvent, CWndBase)
	ON_WM_SIZE()	
	ON_MESSAGE(WM_UPDATE_WINDOW, OnUpdateWindow)
	ON_WM_RBUTTONUP()
	ON_COMMAND(MENU_ADD_LAYER, &CWndEvent::OnAddLayer)
END_MESSAGE_MAP()

CWndEvent::CWndEvent(void)
: m_pEvent(0)
, m_pWndScale(0)
, m_pActiveWndParam(0)
, m_pWndLayers(0)
{
}

CWndEvent::~CWndEvent(void)
{
	release();
}

void CWndEvent::release()
{
	for (size_t i=0; i<m_arrParam.size(); ++i)
	{
		m_arrParam[i]->DestroyWindow();
		delete m_arrParam[i];
	}
	m_arrParam.clear();

	if(m_pWndScale)
		m_pWndScale->DestroyWindow();
	delete m_pWndScale;
	m_pWndScale = 0;

	if(m_pWndLayers)
		m_pWndLayers->DestroyWindow();
	delete m_pWndLayers;
	m_pWndLayers = 0;
}

void CWndEvent::SetEvent(Event* pEvent)
{
	if(!pEvent)
	{
		release();
	}
	if(m_pEvent == pEvent)
		return;
	m_pActiveWndParam = 0;
	m_pEvent = pEvent;
	if(!m_pEvent)
		return;
	if(!UpdateWindows())
		return;
}

bool CWndEvent::UpdateWindows()
{
	release();
	int iParamNum = m_pEvent->GetParameterNum();
	for (int i=0; i<iParamNum; ++i)
	{
		CWndParam* pWndParam = new CWndParam;
		if(!pWndParam->Create(this, CRect(0,0,0,0)))
			return false;
		if(i == 0)
		{
			pWndParam->SetActive(true);
			m_pActiveWndParam = pWndParam;
		}
		else
			pWndParam->SetActive(false);
		pWndParam->SetEventParem(m_pEvent->GetParameterByIndex(i), 0);
		m_arrParam.push_back(pWndParam);
	}

	m_pWndScale = new CWndScale;
	if(!m_pWndScale->Create(this, CRect(0,0,0,0)))
		return false;

	m_pWndLayers = new CWndLayers;
	if(!m_pWndLayers->Create(this, CRect(0,0,0,0)))
		return false;
	m_pWndLayers->SetEvent(m_pEvent);
	m_pWndLayers->SetEventWindow(this);
	SendMessage(WM_SIZE);
	return true;
}

void CWndEvent::Reset()
{
	m_pWndLayers->Reset();
}

void CWndEvent::OnSize(UINT nType, int cx, int cy)
{
	CWndBase::OnSize(nType, cx, cy);
	if(!m_pEvent)
		return;
	int y = 0;
	CRect rcClient;
	GetClientRect(&rcClient);
	CRect rc;
	for (size_t i=0; i<m_arrParam.size(); ++i)
	{
		rc.left = WND_LAYER_LEFT;
		rc.right = rcClient.right;
		rc.top = y;
		rc.bottom = WND_PARAM_HEIGHT + rc.top;
		y = rc.bottom;
		m_arrParam[i]->MoveWindow(&rc, TRUE);
	}

	rc.left = WND_LAYER_LEFT;
	rc.right = rcClient.right;
	rc.top = y;
	rc.bottom = rc.top + WND_SCALE_HEIGHT;
	y = rc.bottom;
	if(m_pWndScale)
	{
		m_pWndScale->MoveWindow(&rc, TRUE);
		WND_PARAM_DATA data;
		m_pActiveWndParam->GetParamData(data);
		m_pWndScale->SetData(data.fMinValue, data.fMaxValue, data.fPerPixelValue, data.fInterval);
	}

	rc.left = rcClient.left;
	rc.right = rcClient.right;
	rc.top = y;
	rc.bottom = rcClient.bottom;
	m_pWndLayers->MoveWindow(&rc, TRUE);
}

bool CWndEvent::GetLayerParamData(int idx, WND_PARAM_DATA& data)
{
	if(!m_pEvent)
		return false;
	EventLayer* pEventLayer = m_pEvent->GetLayerByIndex(idx);
	if(!pEventLayer)
		return false;
	for (size_t i=0; i<m_arrParam.size(); ++i)
	{
		if(m_arrParam[i]->GetEventParam() == pEventLayer->GetParameter())
		{
			m_arrParam[i]->GetParamData(data);
			return true;
		}
	}
	return false;
}

BOOL CWndEvent::PreTranslateMessage(MSG* pMsg)
{
	switch (pMsg->message)
	{
	case WM_LBUTTONDOWN:
		CWndParam* pOld = m_pActiveWndParam;
		for (size_t i=0; i<m_arrParam.size(); ++i)
		{
			if(m_arrParam[i]->GetSafeHwnd() == pMsg->hwnd)
			{
				m_arrParam[i]->SetActive(true);
				m_pActiveWndParam = m_arrParam[i];
			}
			else
			{
				m_arrParam[i]->SetActive(false);
			}
		}
		if(m_pActiveWndParam)
			m_pActiveWndParam->SetActive(true);
		if(m_pActiveWndParam != pOld)
		{
			WND_PARAM_DATA data;
			m_pActiveWndParam->GetParamData(data);
			m_pWndScale->SetData(data.fMinValue, data.fMaxValue, data.fPerPixelValue, data.fInterval);
			return TRUE;
		}
		break;
	}

	return CWndBase::PreTranslateMessage(pMsg);
}

LRESULT CWndEvent::OnUpdateWindow(WPARAM wParam, LPARAM lParam)
{
	UpdateWindows();
	return 0;
}

void CWndEvent::SetEventInstance(EventInstance* pEventInstance)
{
	m_pWndLayers->SetEventInstance(pEventInstance);
	int size = (int)m_arrParam.size();
	for (int i=0; i<size; ++i)
	{
		EventInstanceParameter* pEventInstanceParameter = 0;
		if(pEventInstance)
			pEventInstanceParameter = pEventInstance->GetInstanceParameterByIndex(i);
		m_arrParam[i]->SetEventParem(m_pEvent->GetParameterByIndex(i), pEventInstanceParameter);
	}
}

void CWndEvent::OnRButtonUp(UINT nFlags, CPoint point)
{
	if(!m_pEvent)
		return;
	ClientToScreen(&point);
	CMenu menu;
	menu.CreatePopupMenu();
	menu.AppendMenu(MF_STRING, MENU_ADD_LAYER, "Ìí¼Ó²ã");
	menu.TrackPopupMenu(TPM_RIGHTALIGN, point.x, point.y, this);

	CWndBase::OnRButtonUp(nFlags, point);
}

void CWndEvent::OnAddLayer()
{
	if(!m_pWndLayers || !m_pEvent)
		return;
	m_pWndLayers->OnAddLayer();
}

bool CWndEvent::GetLayerCheckState(int idx) const
{
	return m_pWndLayers->GetWndLayerLeft(idx)->GetLayerCheckState();
}

bool CWndEvent::GetEffectCheckState(int iLayerIdx, int iEffectIdx) const
{
	return m_pWndLayers->GetWndLayerLeft(iLayerIdx)->GetEffectCheckState(iEffectIdx);
}