#include "stdafx.h"
#include "WndLayers.h"
#include "WndLayerLeft.h"
#include "WndLayerRight.h"
#include "WndParam.h"
#include "FEvent.h"
#include "FEventParameter.h"
#include "WndEvent.h"
#include "WndDefine.h"
#include "DlgLayerProp.h"
#include "FEventInstance.h"
#include "Engine.h"
#include "Project.h"

#define MENU_ADD_LAYER		0x2001

#define SCROLL_LINE_SIZE	10
#define SCROLL_PAGE_SIZE	200

BEGIN_MESSAGE_MAP(CWndLayers, CWndBase)
	ON_WM_SIZE()
	ON_WM_RBUTTONUP()
	ON_WM_VSCROLL()
	ON_COMMAND(MENU_ADD_LAYER, &CWndLayers::OnAddLayer)
END_MESSAGE_MAP()

CWndLayers::CWndLayers(void)
: m_pEvent(0)
, m_pWndEvent(0)
, m_iVScrollPos(0)
, m_iVScrollPageSize(500)
{
}

CWndLayers::~CWndLayers(void)
{
	release();
}

BOOL CWndLayers::Create(CWnd* pParentWnd, const RECT& rect)
{
	return CWnd::Create(NULL,
		_T("WndBase"),
		WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_VSCROLL, rect,
		pParentWnd,
		NULL,
		NULL);
}

void CWndLayers::release()
{
	for (size_t i=0; i<m_arrLayerLeft.size(); ++i)
	{
		m_arrLayerLeft[i]->DestroyWindow();
		delete m_arrLayerLeft[i];
	}
	m_arrLayerLeft.clear();

	for (size_t i=0; i<m_arrLayerRight.size(); ++i)
	{
		m_arrLayerRight[i]->DestroyWindow();
		delete m_arrLayerRight[i];
	}
	m_arrLayerRight.clear();
}

void CWndLayers::SetEventWindow(CWndEvent* pWndEvent)
{
	m_pWndEvent = pWndEvent;
}

void CWndLayers::SetEvent(Event* pEvent)
{
	if(!pEvent)
	{
		release();
	}
	if(m_pEvent == pEvent)
		return;
	m_pEvent = pEvent;
	if(!m_pEvent)
		return;
	if(!UpdateWindows())
		return;
}

bool CWndLayers::UpdateWindows()
{
	release();

	int iLayerNum = m_pEvent->GetLayerNum();
	for (int i=0; i<iLayerNum; ++i)
	{
		EventLayer* pEventLayer = m_pEvent->GetLayerByIndex(i);

		CWndLayerRight* pWndLayerRight = new CWndLayerRight;
		if(!pWndLayerRight->Create(this, CRect(0,0,0,0)))
			return false;
		pWndLayerRight->SetEventLayer(pEventLayer);
		m_arrLayerRight.push_back(pWndLayerRight);

		CWndLayerLeft* pWndLayerLeft = new CWndLayerLeft;
		if(!pWndLayerLeft->Create(this, CRect(0,0,0,0)))
			return false;
		pWndLayerLeft->SetEventLayer(pEventLayer);
		m_arrLayerLeft.push_back(pWndLayerLeft);		
	}
	SendMessage(WM_SIZE);
	return true;
}

void CWndLayers::Reset()
{
	for (size_t i=0; i<m_arrLayerLeft.size(); ++i)
	{
		m_arrLayerLeft[i]->Reset();
	}
}

void CWndLayers::OnSize(UINT nType, int cx, int cy)
{
	CWndBase::OnSize(nType, cx, cy);
	if(!m_pEvent || !m_pWndEvent)
		return;	

	int y = -m_iVScrollPos;
	CRect rcClient;
	GetClientRect(&rcClient);
	CRect rc;	

	int size = (int)m_arrLayerLeft.size();
	for (int i=0; i<size; ++i)
	{
		int iEffectNum = m_pEvent->GetLayerByIndex(i)->GetEffectNum();
		int iHeight = iEffectNum * 25 + 30;
		if(iHeight < WND_LAYER_HEIGHT)
			iHeight = WND_LAYER_HEIGHT;
		rc.left = 0;
		rc.right = WND_LAYER_LEFT;
		rc.top = y;
		rc.bottom = rc.top + iHeight;
		m_arrLayerLeft[i]->MoveWindow(&rc, TRUE);
		
		WND_PARAM_DATA data;
		m_pWndEvent->GetLayerParamData(i, data);
		rc.left = WND_LAYER_LEFT;
		rc.right = rcClient.right;
		rc.top = y;
		m_arrLayerRight[i]->SetData(data.fMinValue, data.fMaxValue, data.fPerPixelValue, data.fInterval);
		m_arrLayerRight[i]->MoveWindow(&rc, TRUE);
		m_arrLayerRight[i]->RedrawWindow();
		y = rc.bottom;
	}
	updateVScrollBar();
}

void CWndLayers::OnRButtonUp(UINT nFlags, CPoint point)
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

void CWndLayers::OnAddLayer()
{
	if(!m_pEvent)
		return;
	CDlgLayerProp dlg(m_pEvent);
	if(IDOK != dlg.DoModal())
		return;
	EventLayer* pEventLayer = m_pEvent->CreateLayer(dlg.GetName());
	if(!pEventLayer)
		return;
	if(!pEventLayer->AttachParameter(dlg.GetParamName()))
		return;
	UpdateWindows();
	Engine::GetInstance().GetCurProject()->SetModified(true);
}

void CWndLayers::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	int iDelta = 0;
	switch (nSBCode)
	{
	case SB_LINEUP:
		iDelta = -SCROLL_LINE_SIZE;
		break;
	case SB_LINEDOWN:
		iDelta = SCROLL_LINE_SIZE;
		break;
	case SB_PAGEUP:
		iDelta = -SCROLL_PAGE_SIZE;
		break;
	case SB_PAGEDOWN:
		iDelta = SCROLL_PAGE_SIZE;
		break;
	case SB_THUMBTRACK:
		iDelta = nPos - m_iVScrollPos;
		break;
	}

	int iTotalHeight = getTotalHeight();
	int iScrollPos = m_iVScrollPos + iDelta;
	int iMaxPos = iTotalHeight - m_iVScrollPageSize;
	if(iScrollPos < 0)
	{
		iDelta = -m_iVScrollPos;
	}
	else if(iScrollPos > iMaxPos)
	{
		iDelta = iMaxPos - m_iVScrollPos;
	}
	if(iDelta != 0)
	{
		m_iVScrollPos += iDelta;
		SetScrollPos(SB_VERT, m_iVScrollPos, TRUE);
		ScrollWindow(0, -iDelta);
		SendMessage(WM_SIZE);
	}

	CWndBase::OnVScroll(nSBCode, nPos, pScrollBar);
}

void CWndLayers::updateVScrollBar()
{
	CRect rcClient;
	GetClientRect(&rcClient);
	if(rcClient.Height() <= 0)
		return;
	SCROLLINFO si;
	si.cbSize = sizeof(si);
	si.fMask = SIF_ALL;
	GetScrollInfo(SB_VERT, &si);
	si.nMin = 0;
	int iTotalHeight = getTotalHeight();	

	int cy = rcClient.Height()-1;

	if(iTotalHeight == cy)
	{
		si.nMax = 0;
		si.nPos = 0;
		si.nPage = 0;
		m_iVScrollPos = 0;
	}
	else
	{
		si.nMax = iTotalHeight;
		si.nPage = cy;
		if(m_iVScrollPos < 0)
			m_iVScrollPos = 0;
		m_iVScrollPageSize = cy;
//		if(m_iVScrollPos >= iTotalHeight - m_iVScrollPageSize)
//			m_iVScrollPos = iTotalHeight - cy;
		si.nPos = m_iVScrollPos;
	}
	SetScrollInfo(SB_VERT, &si, TRUE);
}

int CWndLayers::getTotalHeight()
{
	int iHeight = 0;
	CRect rc;
	for (size_t i=0; i<m_arrLayerLeft.size(); ++i)
	{
		m_arrLayerLeft[i]->GetClientRect(&rc);
		iHeight += rc.Height();
	}
	iHeight += 100;
	return iHeight;
}

void CWndLayers::SetEventInstance(EventInstance* pEventInstance)
{
	if(!m_pEvent)
		return;
	int iLayerNum = m_pEvent->GetLayerNum();
	for (int i=0; i<iLayerNum; ++i)
	{
		EventLayerInstance* pEventLayerInstance = 0;
		if(pEventInstance)
			pEventLayerInstance = pEventInstance->GetEventLayerInstanceByIndex(i);
		m_arrLayerLeft[i]->SetEventLayerInstance(pEventLayerInstance);
	}
}

CWndLayerLeft* CWndLayers::GetWndLayerLeft(int idx) const
{
	return m_arrLayerLeft[idx];
}

CWndLayerRight* CWndLayers::GetWndLayerRight(int idx) const
{
	return m_arrLayerRight[idx];
}