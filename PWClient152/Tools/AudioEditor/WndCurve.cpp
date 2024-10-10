#include "stdafx.h"
#include "WndCurve.h"
#include "resource.h"
#include "FPropCurves.h"
#include "WndLayerRight.h"
#include "Engine.h"
#include "Project.h"
#include "Global.h"
#include "EffectValueChanger.h"

using AudioEngine::CVPOINT;
using AudioEngine::CURVE_TYPE;

const int c_iPointNum = 200;
const int c_iPenWidth = 2;

#define MENU_ADD_POINT		0x2901
#define MENU_CURVE_LINEAR	0x2902
#define MENU_CURVE_SIN		0x2903
#define MENU_CURVE_COS		0x2904
#define MENU_CURVE_VERT_SIN	0x2905
#define MENU_CURVE_POW		0x2906

BEGIN_MESSAGE_MAP(CWndCurve, CWndBase)
	ON_WM_MOUSEMOVE()
	ON_COMMAND(MENU_ADD_POINT, OnAddPoint)
	ON_COMMAND_RANGE(MENU_CURVE_LINEAR, MENU_CURVE_POW, OnChangeCurveType)
	ON_WM_RBUTTONDOWN()
	ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()

CWndCurve::CWndCurve(void)
{
	m_clBK = RGB(0,255,0);
	m_hHandCursor = AfxGetApp()->LoadCursor(IDC_HAND_CURSOR);
	m_pPropCurves = 0;
}

CWndCurve::~CWndCurve(void)
{
}

bool CWndCurve::Init(PropCurves* pPropCurves)
{
	if(!pPropCurves)
		return false;
	m_pPropCurves = pPropCurves;
	return true;
}

void CWndCurve::DrawRealize(CDC* pDC)
{
	m_arrPoints.clear();
	int x,y;
	float fx,fy;
	::POINT pt;
	CEffectValueChanger ec(m_pPropCurves, this);
	int iPointNum = m_pPropCurves->GetPointNum();
	if(iPointNum > 1)
	{
		for (int i=0; i<iPointNum-1; ++i)
		{
			CVPOINT ptStart = m_pPropCurves->GetPointByIndex(i);
			CVPOINT ptEnd = m_pPropCurves->GetPointByIndex(i+1);
			float sx = ptStart.x;
			float ex = ptEnd.x;
			ec.LogicalToScreen(ptStart.x, ptStart.y, x, y);
			pt.x = x;
			pt.y = y;
			m_arrPoints.push_back(pt);
			for (int j=1; j<c_iPointNum; ++j)
			{
				fx = (float)j*(ex-sx)/c_iPointNum+sx;
				fy = m_pPropCurves->GetValue(fx);
				ec.LogicalToScreen(fx, fy, x, y);
				pt.x = x;
				pt.y = y;
				m_arrPoints.push_back(pt);
			}
			ec.LogicalToScreen(ptEnd.x, ptEnd.y, x, y);
			pt.x = x;
			pt.y = y;
			m_arrPoints.push_back(pt);
		}

		CVPOINT ptLast = m_pPropCurves->GetPointByIndex(iPointNum-1);
		for (int i=1; i<=c_iPointNum; ++i)
		{
			fx = (float)i*(1-ptLast.x)/c_iPointNum+ptLast.x;
			fy = m_pPropCurves->GetValue(fx);
			ec.LogicalToScreen(fx, fy, x, y);
			pt.x = x;
			pt.y = y;
			m_arrPoints.push_back(pt);
		}
	}
	else
	{
		for (int i=0; i<=c_iPointNum; ++i)
		{
			fx = (float)i/c_iPointNum;
			fy = m_pPropCurves->GetValue(fx);
			ec.LogicalToScreen(fx, fy, x, y);
			pt.x = x;
			pt.y = y;
			m_arrPoints.push_back(pt);
		}
	}
	
	::BeginPath(pDC->GetSafeHdc());
	::SetBkMode(pDC->m_hDC, TRANSPARENT);

	drawPath(pDC);

	::EndPath(pDC->GetSafeHdc());
	CRgn rgn;
	rgn.CreateFromPath(pDC);
	SetWindowRgn((HRGN)rgn, TRUE);
}

void CWndCurve::OnMouseMove(UINT nFlags, CPoint point)
{
	((CWndLayerRight*)GetParent())->MoveOver(m_pPropCurves, point);
	m_hOldCursor = ::SetCursor(m_hHandCursor);
	CWndBase::OnMouseMove(nFlags, point);
}

void CWndCurve::drawPath(CDC* pDC)
{
	int size = (int)m_arrPoints.size();
	for (int i=0; i<size; ++i)
	{
		if(m_arrPoints[i].y < c_iPenWidth)
			m_arrPoints[i].y = c_iPenWidth;
		::POINT pt = m_arrPoints[i];
		if(i == 0)
		{
			pDC->MoveTo(pt);
			continue;
		}
		pDC->LineTo(pt);
	}

	for (int i=0; i<size; ++i)
	{
		::POINT pt = m_arrPoints[size - i - 1];
		if(size-i-2 > 0)
		{
			if(m_arrPoints[size-i-2].x == pt.x)
			{
				if(m_arrPoints[size-i-2].y == pt.y)
					continue;
				pt.x += c_iPenWidth;
				m_arrPoints[size-i-2].x += c_iPenWidth;
			}
			else
			{
				pt.y -= c_iPenWidth;
			}
		}
		else
		{
			if(m_arrPoints[0].x == m_arrPoints[1].x)
			{
				if(m_arrPoints[0].y == m_arrPoints[1].y && i == size - 2)
					continue;
				pt.x += c_iPenWidth;
				m_arrPoints[0].x += c_iPenWidth;
			}
			else
			{
				pt.y -= c_iPenWidth;
			}
		}
		pDC->LineTo(pt);
	}
}

void CWndCurve::OnRButtonDown(UINT nFlags, CPoint point)
{
	m_hOldCursor = ::SetCursor(m_hHandCursor);

	CPoint pos;
	GetCursorPos(&pos);
	ScreenToClient(&pos);
	CEffectValueChanger ec(m_pPropCurves, this);
	ec.ScreenToLogical(pos.x, pos.y, m_fRClickX, m_fRClickY);
	m_fRClickY = m_pPropCurves->GetValue(m_fRClickX);
	ClientToScreen(&pos);
	CMenu menu;
	menu.CreatePopupMenu();
	menu.AppendMenu(MF_STRING, MENU_ADD_POINT, "添加点");
	if(m_pPropCurves->GetPointNum() && m_fRClickX < m_pPropCurves->GetPointByIndex(m_pPropCurves->GetPointNum()-1).x)
	{
		menu.AppendMenu(MF_SEPARATOR);
		menu.AppendMenu(MF_STRING, MENU_CURVE_LINEAR, "线性");
		menu.AppendMenu(MF_STRING, MENU_CURVE_SIN, "正弦");
		menu.AppendMenu(MF_STRING, MENU_CURVE_COS, "上凸");
		menu.AppendMenu(MF_STRING, MENU_CURVE_VERT_SIN, "反正弦");
		menu.AppendMenu(MF_STRING, MENU_CURVE_POW, "下凹");
	}
	
	menu.TrackPopupMenu(TPM_RIGHTALIGN, pos.x, pos.y, this);
	CWndBase::OnRButtonDown(nFlags, point);
}


void CWndCurve::OnAddPoint()
{
	CVPOINT pt;
	pt.x = m_fRClickX;
	pt.y = m_fRClickY;
	m_pPropCurves->AddPoint(pt);
	Engine::GetInstance().GetCurProject()->SetModified(true);
	((CWndLayerRight*)GetParent())->UpdateCurves(true);
}

void CWndCurve::OnChangeCurveType(UINT uID)
{
	int iCurveIdx = -1;
	int iPointNum = m_pPropCurves->GetPointNum();
	for(int i=0; i<iPointNum-1; ++i)
	{
		CVPOINT pt0 = m_pPropCurves->GetPointByIndex(i);
		CVPOINT pt1 = m_pPropCurves->GetPointByIndex(i+1);
		if(m_fRClickX > pt0.x && m_fRClickX < pt1.x)
		{
			iCurveIdx = i;
			break;
		}
	}
	if(iCurveIdx == -1)
		return;
	CURVE_TYPE type = (CURVE_TYPE)(uID - MENU_CURVE_LINEAR);
	if(m_pPropCurves->GetCurveByIndex(iCurveIdx) == type)
		return;
	m_pPropCurves->SetCurve(iCurveIdx, type);
	Engine::GetInstance().GetCurProject()->SetModified(true);
	((CWndLayerRight*)GetParent())->UpdateCurves(false);	
}

void CWndCurve::OnLButtonDown(UINT nFlags, CPoint point)
{
	int iPointNum = m_pPropCurves->GetPointNum();
	if(iPointNum == 0 || iPointNum == 1)
	{
		((CWndLayerRight*)GetParent())->EditCurve(m_pPropCurves, -1);
		CWndBase::OnLButtonDown(nFlags, point);
		return;
	}
	int iCurveIdx = -1;

	float fx, fy;
	CEffectValueChanger ec(m_pPropCurves, this);
	ec.ScreenToLogical(point.x, point.y, fx, fy);

	
	for(int i=0; i<iPointNum-1; ++i)
	{
		CVPOINT pt0 = m_pPropCurves->GetPointByIndex(i);
		CVPOINT pt1 = m_pPropCurves->GetPointByIndex(i+1);
		if(fx > pt0.x && fx < pt1.x)
		{
			iCurveIdx = i;
			break;
		}
	}
	if(iCurveIdx == -1)
		return;

	((CWndLayerRight*)GetParent())->EditCurve(m_pPropCurves, iCurveIdx);
	CWndBase::OnLButtonDown(nFlags, point);
}
