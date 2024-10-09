// ColorShow.cpp : implementation file
//

#include "stdafx.h"
#include "SelColorWnd.h"

/////////////////////////////////////////////////////////////////////////////
// CSelColorWnd

CSelColorWnd::CSelColorWnd()
{
	m_pNotifyWnd	= NULL;
	m_rgbColor		= RGB(0, 0, 0);
}

CSelColorWnd::~CSelColorWnd()
{
}


BEGIN_MESSAGE_MAP(CSelColorWnd, CStatic)
	//{{AFX_MSG_MAP(CSelColorWnd)
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSelColorWnd message handlers

void CSelColorWnd::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	Update(dc);
	// Do not call CStatic::OnPaint() for painting messages
}

void CSelColorWnd::Redraw()
{
	CClientDC dc(this);
	Update(dc);
}

void CSelColorWnd::Update(CDC& dc)
{
	CRect rc;
	GetClientRect(rc);

	CBrush brush;
	brush.CreateSolidBrush(m_rgbColor);
	CBrush *pOldBrush = dc.SelectObject(&brush);

	dc.PatBlt(rc.left, rc.top, rc.Width(), rc.Height(), PATCOPY);

	dc.SelectObject(pOldBrush);
}

void CSelColorWnd::OnLButtonDown(UINT nFlags, CPoint point) 
{
	CColorDialog ColorDia(m_rgbColor, CC_FULLOPEN);
	if (ColorDia.DoModal() == IDOK)
	{
		m_rgbColor = ColorDia.GetColor();
		Redraw();

		m_pNotifyWnd->SendMessage(WM_SELCOLCHANGED, (WPARAM)m_hWnd, (LPARAM)m_rgbColor);
	}
	
	CStatic::OnLButtonDown(nFlags, point);
}
