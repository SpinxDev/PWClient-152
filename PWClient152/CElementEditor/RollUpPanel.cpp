/*
 * FILE: RollUpPanel.cpp
 *
 * DESCRIPTION: Rollup window pancel class
 *
 * CREATED BY: Duyuxin, 2004/6/9
 *
 * HISTORY: 
 *
 * Copyright (c) 2004 Archosaur Studio, All Rights Reserved.
 */

#include "StdAfx.h"
#include "RollUpWnd.h"
#include "RollUpPanel.h"
#include "AMemory.h"

//#define new A_DEBUG_NEW

///////////////////////////////////////////////////////////////////////////
//	
//	Define and Macro
//	
///////////////////////////////////////////////////////////////////////////

#define ID_PANELBTN_TITLE	1000

///////////////////////////////////////////////////////////////////////////
//	
//	Reference to External variables and functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Local Types and Variables and Global variables
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Local functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Implement CRollUpTitleBtn
//	
///////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CRollUpTitleBtn, CButton)
	//{{AFX_MSG_MAP(CRollUpTitleBtn)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CRollUpTitleBtn::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct) 
{
	//	This code only works with buttons.
	ASSERT(lpDrawItemStruct->CtlType == ODT_BUTTON);

	//	Draw background
	COLORREF colBack = RGB(185, 200, 205);//::GetSysColor(COLOR_WINDOW);
	COLORREF colFont = RGB(0, 0, 128);//::GetSysColor(COLOR_BTNTEXT);

	if ((lpDrawItemStruct->itemState & ODS_GRAYED) ||
		(lpDrawItemStruct->itemState & ODS_DISABLED))
	{	
		colBack = RGB(240, 240, 240);
		colFont = RGB(180, 180, 180);
	}
	else if (lpDrawItemStruct->itemState & ODS_SELECTED)
	{
	//	colBack = RGB(0, 0, 128);
		colFont = RGB(128, 0, 255);
	}
/*	else if (lpDrawItemStruct->itemState & ODS_HOTLIGHT)
	{	
	//	colBack = RGB(0, 0, 128);
		colFont = RGB(0, 255, 0);
	}
*/
	HBRUSH hOldBrush, hBrush;
	LOGBRUSH logBrush = {BS_SOLID, colBack, 0};
	hBrush = ::CreateBrushIndirect(&logBrush);
	hOldBrush = (HBRUSH)::SelectObject(lpDrawItemStruct->hDC, hBrush);

	HPEN hOldPen, hPen;
	hPen = ::CreatePen(PS_SOLID, 1, RGB(0, 0, 128));
	hOldPen = (HPEN)::SelectObject(lpDrawItemStruct->hDC, hPen);

	RECT rc;
	GetClientRect(&rc);
	::Rectangle(lpDrawItemStruct->hDC, 0, 0, rc.right, rc.bottom);

	//	Get the button's text.
	CString strText;
	GetWindowText(strText);

	//	Draw the button text using the text color red.
	COLORREF colOldFont = ::SetTextColor(lpDrawItemStruct->hDC, colFont);
	COLORREF colOldBack = ::SetBkColor(lpDrawItemStruct->hDC, colBack);
	::DrawText(lpDrawItemStruct->hDC, strText, strText.GetLength(), 
				&lpDrawItemStruct->rcItem, DT_SINGLELINE | DT_VCENTER | DT_CENTER);
	::SetBkColor(lpDrawItemStruct->hDC, colOldBack);
	::SetTextColor(lpDrawItemStruct->hDC, colOldFont);

	//	Release resources
	::SelectObject(lpDrawItemStruct->hDC, hOldPen);
	::SelectObject(lpDrawItemStruct->hDC, hOldBrush);
	::DeleteObject(hBrush);
	::DeleteObject(hPen);
}

///////////////////////////////////////////////////////////////////////////
//	
//	Implement CRollUpPanel
//	
///////////////////////////////////////////////////////////////////////////

CString CRollUpPanel::m_strClassName = "";

CRollUpPanel::CRollUpPanel()
{
	m_bCreated		= false;
	m_dwStyles		= 0;
	m_dwStates		= 0;
	m_iDlgHeight	= 0;
}

CRollUpPanel::~CRollUpPanel()
{
}

BEGIN_MESSAGE_MAP(CRollUpPanel, CWnd)
	//{{AFX_MSG_MAP(CRollUpPanel)
	ON_WM_SIZE()
	ON_WM_LBUTTONDOWN()
	ON_WM_RBUTTONDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//	Create panel window
bool CRollUpPanel::Create(const char* szName, DWORD dwStyles, CWnd* pContentWnd, 
						CWnd* pParent, const RECT& rc, DWORD dwID)
{
	ASSERT(pContentWnd && pParent);

	//	Register class name
	if (!m_strClassName.GetLength())
	{
		m_strClassName = AfxRegisterWndClass(CS_VREDRAW | CS_HREDRAW | CS_DBLCLKS,
						//	LoadCursor(NULL, MAKEINTRESOURCE(IDC_ARROW)),
							LoadCursor(NULL, IDC_SIZEALL),
							HBRUSH(COLOR_BTNFACE+1));
	}

	DWORD dwWndStyles = WS_CHILD | WS_CLIPSIBLINGS;
	if (!CWnd::CreateEx(0, m_strClassName, szName, dwWndStyles,	rc, pParent, dwID, NULL))
		return false;

	m_strName		= szName;
	m_dwStyles		= dwStyles;
	m_pContentWnd	= pContentWnd;

	//	Get dialog height
	RECT rcWnd;
	m_pContentWnd->GetWindowRect(&rcWnd);
	m_iDlgHeight = rcWnd.bottom - rcWnd.top;

	//	Ensure dialog has WS_CHILD style
	DWORD dwDlgStyles = ::GetWindowLong(m_pContentWnd->m_hWnd, GWL_STYLE);
	dwDlgStyles |= WS_CHILD;
	dwDlgStyles &= ~WS_POPUP;
	::SetWindowLong(m_pContentWnd->m_hWnd, GWL_STYLE, dwDlgStyles);

	//	Change dialog's parent
	m_pContentWnd->SetParent(this);
	m_pContentWnd->ShowWindow(SW_SHOW);

	//	Create title and border buttons
	SetRect(&rcWnd, 0, 0, 100, 100);

	m_TitleBtn.Create(szName, BS_FLAT | BS_OWNERDRAW | WS_VISIBLE, rcWnd, this, ID_PANELBTN_TITLE);
	m_BorderBtn.Create("", WS_VISIBLE | WS_DISABLED | BS_FLAT | BS_GROUPBOX, rcWnd, this, ID_PANELBTN_TITLE);

	//	Adjust window's z order
	m_TitleBtn.SetWindowPos(&m_BorderBtn, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
	m_pContentWnd->SetWindowPos(&m_BorderBtn, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);

	if (!HasTitle())
	{
		m_dwStyles |= STYLE_NOCLOSE;
		m_TitleBtn.ShowWindow(SW_HIDE);
	}

	if (!IsCloseable())
	{
		//	Disable title button
		m_TitleBtn.EnableWindow(FALSE);
	}

	m_bCreated = true;

	//	Arrange children window position and size
	ArrangeChildrenPos();

	return true;
}

//	Set panel name
void CRollUpPanel::SetPanelName(const char* szName)
{
	m_strName = szName;
	m_TitleBtn.SetWindowText(szName);
}

//	Arrange child window's position and size
void CRollUpPanel::ArrangeChildrenPos()
{
	RECT rc, rcClient;
	GetClientRect(&rcClient);

	if (m_dwStates & STATE_OPEN)	//	Panel is open
	{
		if (HasTitle())
		{
			//	Adjust title button
			SetRect(&rc, SIZE_BORDER, 0, rcClient.right-SIZE_BORDER, SIZE_TITLEHEI);
			m_TitleBtn.MoveWindow(&rc);

			//	Adjust border button
			rc = rcClient;
			m_BorderBtn.MoveWindow(&rc);

			//	Adjust dialog
			SetRect(&rc, SIZE_BORDER, SIZE_TITLEHEI, rcClient.right-SIZE_BORDER, rcClient.bottom-SIZE_BORDER);
			m_pContentWnd->MoveWindow(&rc);
		}
		else
		{
			//	Adjust border button
			rc = rcClient;
			m_BorderBtn.MoveWindow(&rc);

			//	Adjust dialog
			SetRect(&rc, SIZE_BORDER, SIZE_BORDER * 2, rcClient.right-SIZE_BORDER, rcClient.bottom-SIZE_BORDER);
			m_pContentWnd->MoveWindow(&rc);
		}
	}
	else if (IsCloseable())	//	Panel is close
	{
		if (HasTitle())
		{	
			//	Adjust title button
			SetRect(&rc, SIZE_BORDER, 0, rcClient.right-SIZE_BORDER, SIZE_TITLEHEI);
			m_TitleBtn.MoveWindow(&rc);

			//	Adjust border button
			rc = rcClient;
			rc.bottom -= SIZE_BORDER;
			m_BorderBtn.MoveWindow(&rc);
		}
	}
}

void CRollUpPanel::OnSize(UINT nType, int cx, int cy) 
{
	if (!m_bCreated)
	{
		CWnd::OnSize(nType, cx, cy);
		return;
	}

	cy = IsOpen() ? GetOpenHeight() : GetCloseHeight();
	CWnd::OnSize(nType, cx, cy);

	ArrangeChildrenPos();
}

//	Show / Hide pancel
bool CRollUpPanel::Show(bool bShow)
{
	if (bShow)
	{
		ShowWindow(SW_SHOW);
		m_dwStates |= STATE_VISIBLE;
	}
	else
	{
		ShowWindow(SW_HIDE);
		m_dwStates &= ~STATE_VISIBLE;
	}

	return true;
}

//	Open / Close panel
bool CRollUpPanel::Open(bool bOpen)
{
	if (bOpen)
	{
		if (m_pContentWnd)
			m_pContentWnd->ShowWindow(SW_SHOW);

		m_dwStates |= STATE_OPEN;
	}
	else
	{
		if (!IsCloseable())
			return true;

		if (m_pContentWnd)
			m_pContentWnd->ShowWindow(SW_HIDE);

		m_dwStates &= ~STATE_OPEN;
	}

	ArrangeChildrenPos();

	return true;
}

BOOL CRollUpPanel::OnCommand(WPARAM wParam, LPARAM lParam)
{
	WORD wNotify = (WORD)(wParam & 0x0000ffff);

	//	Message is from controls ?
	if (lParam == (LPARAM)m_TitleBtn.GetSafeHwnd())
	{
		CWnd* pParent = GetParent();
		ASSERT(pParent);
		pParent->SendMessage(WM_ROLLUPTITLECLICK, (WPARAM)this, 0);
	}

	return CWnd::OnCommand(wParam, lParam);
}

void CRollUpPanel::OnLButtonDown(UINT nFlags, CPoint point) 
{
	CWnd* pParent = GetParent();
	ASSERT(pParent);
	pParent->SendMessage(WM_ROLLUPBEGINDRAG, 0, 0);
	
	CWnd::OnLButtonDown(nFlags, point);
}

void CRollUpPanel::OnRButtonDown(UINT nFlags, CPoint point) 
{
	POINT pt = {point.x, point.y};
	ClientToScreen(&pt);

	CWnd* pParent = GetParent();
	ASSERT(pParent);
	pParent->SendMessage(WM_ROLLUPPOPUPMENU, pt.x, pt.y);
	
	CWnd::OnRButtonDown(nFlags, point);
}
