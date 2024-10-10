#include "stdafx.h"
#include "WndDot.h"
#include "resource.h"
#include "FPropCurves.h"
#include "Engine.h"
#include "Project.h"
#include "WndLayerRight.h"
#include "Global.h"
#include "EffectValueChanger.h"
#include "DlgSetValue.h"

using AudioEngine::EVENT_PARAM_PROPERTY;
using AudioEngine::CVPOINT;

#define MENU_DEL_POINT	0x3201
#define MENU_SET_VALUE	0x3202

const int c_iDotEdge = 4;

BEGIN_MESSAGE_MAP(CWndDot, CWndBase)
	ON_WM_LBUTTONDOWN()
	ON_WM_CREATE()
	ON_WM_MOUSEMOVE()
	ON_COMMAND(MENU_DEL_POINT, OnDelPoint)
	ON_COMMAND(MENU_SET_VALUE, OnSetValue)
ON_WM_RBUTTONDOWN()
ON_WM_LBUTTONUP()
END_MESSAGE_MAP()

CWndDot::CWndDot(void)
{
	m_clBK = RGB(0,255,0);
	m_hHandCursor = AfxGetApp()->LoadCursor(IDC_HAND_CURSOR);
	m_pPropCurves = 0;
	m_iIndex = -1;
	m_pEventParameter = 0;
}

CWndDot::~CWndDot(void)
{
}

bool CWndDot::Init(EventParameter* pEventParameter, PropCurves* pPropCurves, int idx)
{
	if(!pPropCurves || !pEventParameter)
		return false;
	m_pPropCurves = pPropCurves;
	m_pEventParameter = pEventParameter;
	m_iIndex = idx;
	return true;
}

void CWndDot::SetIndex(int idx)
{
	m_iIndex = idx;
}

void CWndDot::DrawRealize(CDC* pDC)
{
	if(!pDC || !m_pPropCurves || m_iIndex == -1)
		return;	
	::BeginPath(pDC->GetSafeHdc());
	::SetBkMode(pDC->m_hDC, TRANSPARENT);

	CVPOINT pt = m_pPropCurves->GetPointByIndex(m_iIndex);
	int x,y;
	CEffectValueChanger ec(m_pPropCurves, this);
	ec.LogicalToScreen(pt.x, pt.y, x, y);

	pDC->MoveTo(x-c_iDotEdge, y-c_iDotEdge);
	pDC->LineTo(x+c_iDotEdge, y-c_iDotEdge);
	pDC->LineTo(x+c_iDotEdge, y+c_iDotEdge);
	pDC->LineTo(x-c_iDotEdge, y+c_iDotEdge);
	pDC->LineTo(x-c_iDotEdge, y-c_iDotEdge);

	::EndPath(pDC->GetSafeHdc());
	CRgn rgn;
	rgn.CreateFromPath(pDC);
	SetWindowRgn((HRGN)rgn, TRUE);
}

void CWndDot::OnLButtonDown(UINT nFlags, CPoint point)
{
	((CWndLayerRight*)GetParent())->EditDot(m_pPropCurves, m_iIndex);
	CWndBase::OnLButtonDown(nFlags, point);
}

void CWndDot::OnRButtonDown(UINT nFlags, CPoint point)
{
	m_hOldCursor = ::SetCursor(m_hHandCursor);

	CPoint pos;
	GetCursorPos(&pos);
	CMenu menu;
	menu.CreatePopupMenu();
	menu.AppendMenu(MF_STRING, MENU_SET_VALUE, "ÉèÖµ");
	menu.AppendMenu(MF_STRING, MENU_DEL_POINT, "É¾³ýµã");
	

	menu.TrackPopupMenu(TPM_RIGHTALIGN, pos.x, pos.y, this);
	CWndBase::OnRButtonDown(nFlags, point);
}

void CWndDot::OnDelPoint()
{
	m_pPropCurves->DeletePoint(m_iIndex);
	Engine::GetInstance().GetCurProject()->SetModified(true);
	((CWndLayerRight*)GetParent())->UpdateCurves(true);
}

void CWndDot::OnMouseMove(UINT nFlags, CPoint point)
{
	((CWndLayerRight*)GetParent())->MoveOver(m_pPropCurves, point);
	m_hOldCursor = ::SetCursor(m_hHandCursor);
	CWndBase::OnMouseMove(nFlags, point);
}

void CWndDot::OnSetValue()
{
	CVPOINT pt = m_pPropCurves->GetPointByIndex(m_iIndex);
	EVENT_PARAM_PROPERTY prop;
	m_pEventParameter->GetProperty(prop);
	CDlgSetValue dlg;
	float fParam = pt.x * (prop.fMaxValue - prop.fMinValue) + prop.fMinValue;
	dlg.Init(pt.y, m_pPropCurves->GetMin(), m_pPropCurves->GetMax(), fParam);
	if(IDOK!=dlg.DoModal())
		return;
	pt.x = dlg.GetParam();
	pt.x = (pt.x-prop.fMinValue)/(prop.fMaxValue - prop.fMinValue);
	pt.y = dlg.GetValue();
	m_pPropCurves->SetPoint(m_iIndex, pt);
	((CWndLayerRight*)GetParent())->UpdateCurves(false);
	Engine::GetInstance().GetCurProject()->SetModified(true);
}