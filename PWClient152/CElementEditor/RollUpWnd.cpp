/*
 * FILE: RollUpWnd.cpp
 *
 * DESCRIPTION: 3DMAX style roll up window
 *
 * CREATED BY: Duyuxin, 2004/6/9
 *
 * HISTORY:
 *
 * Copyright (c) 2004 Archosaur Studio, All Rights Reserved.	
 */

#include "stdafx.h"
#include "RollUpPanel.h"
#include "RollUpWnd.h"
#include "AMemory.h"

//#define new A_DEBUG_NEW

///////////////////////////////////////////////////////////////////////////
//
//	Reference to External variables and functions
//
///////////////////////////////////////////////////////////////////////////

//	Command IDs
#define MENUCMD_EXPANDALL	1000		//	Expand all panels
#define MENUCMD_COLLAPSEALL	1001		//	Collapse all panels
#define MENUCMD_PANEL0		1002		//	The first visible panel

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
//	Implement CRollUpScrollBar
//	
///////////////////////////////////////////////////////////////////////////

CString	CRollUpScrollBar::m_strClassName = "";

CRollUpScrollBar::CRollUpScrollBar()
{
	m_iMin			= 0;
	m_iMax			= 100;
	m_iRangeSize	= m_iMax - m_iMin + 1;
	m_iPageSize		= 10;
	m_iCurPos		= 0;
	m_bDrag			= false;
}

BEGIN_MESSAGE_MAP(CRollUpScrollBar, CBackBufWnd)
	//{{AFX_MSG_MAP(CRollUpScrollBar)
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//	Create scroll bar
bool CRollUpScrollBar::Create(CWnd* pParent, const RECT& rc, DWORD dwID)
{
	if (m_strClassName == "")
	{
		m_strClassName = AfxRegisterWndClass(CS_VREDRAW | CS_HREDRAW | CS_DBLCLKS,
						LoadCursor(NULL, IDC_ARROW), 
						(HBRUSH)::GetStockObject(BLACK_BRUSH));//(HBRUSH)COLOR_WINDOW);
	}

	if (!CBackBufWnd::Create(m_strClassName, "RollUpScrollBar", 
						WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS,
						rc, pParent, dwID))
		return false;

	return true;
}

//	Set range
void CRollUpScrollBar::SetRange(int iMin, int iMax, int iPageSize)
{
	ASSERT(iMin <= iMax);

	m_iMin			= iMin;
	m_iMax			= iMax;
	m_iRangeSize	= iMax - iMin;
	m_iPageSize		= iPageSize;

	if (m_iPageSize > m_iRangeSize)
		m_iPageSize = m_iRangeSize;

	ClampPosition();
}

//	Set current position
void CRollUpScrollBar::SetPos(int iPos)
{
	m_iCurPos = iPos;
	ClampPosition();
}

//	Set page size
void CRollUpScrollBar::SetPageSize(int iPageSize)
{
	m_iPageSize = iPageSize;
	if (m_iPageSize > m_iRangeSize)
		m_iPageSize = m_iRangeSize;

	ClampPosition();
}

//	Clamp position
void CRollUpScrollBar::ClampPosition()
{
	if (m_iCurPos < m_iMin)
		m_iCurPos = m_iMin;
	else if (m_iCurPos + m_iPageSize > m_iMax)
		m_iCurPos = m_iMax - m_iPageSize;
}

//	Calculate scroll box's position and size
void CRollUpScrollBar::CalcScrollBox(RECT& rc)
{
	RECT rcClient;
	GetClientRect(&rcClient);

	rc.left		= 0;
	rc.right	= rcClient.right;

	if (m_iCurPos == m_iMin)
		rc.top = 0;
	else
		rc.top = (int)((float)m_iCurPos / m_iRangeSize * rcClient.bottom);

	if (m_iCurPos + m_iPageSize >= m_iMax)
		rc.bottom = rcClient.bottom;
	else
		rc.bottom = (int)((float)(m_iCurPos + m_iPageSize) / m_iRangeSize * rcClient.bottom);

	if (rc.bottom - rc.top < 10)
	{
		//	TODO: handle the case in which the thumb is too small
	}
}

//	Convert pixel unit to logic unit
int CRollUpScrollBar::PixelToLogic(int iPixel)
{
	RECT rcClient;
	GetClientRect(&rcClient);
	return (int)((float)iPixel * m_iRangeSize / rcClient.bottom);
}

//	Draw back buffer
bool CRollUpScrollBar::DrawBackBuffer(RECT* lprc)
{
	//	Clear background
	ClearBackBuffer(RGB(0, 0, 64));

	//	Calculate scroll box position and size
	RECT rc;
	CalcScrollBox(rc);

	//	Draw scroll box
	HBRUSH hOldBrush, hBrush;
	LOGBRUSH logBrush = {BS_SOLID, RGB(255, 255, 255), 0};
	hBrush = ::CreateBrushIndirect(&logBrush);
	hOldBrush = (HBRUSH)::SelectObject(m_hMemDC, hBrush);

	HPEN hOldPen, hPen;
	hPen = ::CreatePen(PS_SOLID, 1, RGB(0, 0, 64));
	hOldPen = (HPEN)::SelectObject(m_hMemDC, hPen);

	::Rectangle(m_hMemDC, rc.left, rc.top, rc.right, rc.bottom);

	//	Release resources
	::SelectObject(m_hMemDC, hOldPen);
	::SelectObject(m_hMemDC, hOldBrush);
	::DeleteObject(hBrush);
	::DeleteObject(hPen);

	return true;
}

void CRollUpScrollBar::OnPaint() 
{
	CPaintDC dc(this); // device context for painting

	DrawBackBuffer(NULL);
	Flip(dc.m_hDC, NULL);
	
	// Do not call CBackBufWnd::OnPaint() for painting messages
}

void CRollUpScrollBar::OnLButtonDown(UINT nFlags, CPoint point) 
{
	//	Get scroll box size
	RECT rcBox;
	CalcScrollBox(rcBox);

	if (PtInRect(&rcBox, point))
	{
		m_bDrag = true;
		m_ptLastMou = point;
		SetCapture();
	}
	else if (point.y < rcBox.top)
	{
		//	Roll up half page
		RollDown(-m_iPageSize / 2, true);
	}
	else if (point.y > rcBox.bottom)
	{
		//	Roll down half page
		RollDown(m_iPageSize / 2, true);
	}
	
	CBackBufWnd::OnLButtonDown(nFlags, point);
}

void CRollUpScrollBar::OnLButtonUp(UINT nFlags, CPoint point) 
{
	if (m_bDrag)
	{
		m_bDrag = false;
		ReleaseCapture();
	}

	CBackBufWnd::OnLButtonUp(nFlags, point);
}

void CRollUpScrollBar::OnMouseMove(UINT nFlags, CPoint point) 
{
	if (!m_bDrag)
	{
		CBackBufWnd::OnMouseMove(nFlags, point);
		return;
	}

	int iDelta = point.y - m_ptLastMou.y;
	RollDown(iDelta, false);

	m_ptLastMou = point;

	CBackBufWnd::OnMouseMove(nFlags, point);
}

//	Roll up / down
//	iDelta: roll delta in screen pixels or logic unit
//	bLogicUnit: true, iDelta is logic unit; false, iDelta is pixel delta
void CRollUpScrollBar::RollDown(int iDelta, bool bLogicUnit)
{
	if (!bLogicUnit)
		iDelta = PixelToLogic(iDelta);

	int iOldPos = m_iCurPos;
	m_iCurPos += iDelta;
	ClampPosition();

	if (iOldPos == m_iCurPos)
		return;

	iDelta = m_iCurPos - iOldPos;

	//	Notify parent to roll up or down
	CWnd* pParent = GetParent();
	ASSERT(pParent);
	pParent->SendMessage(WM_ROLLUPROLLDOWN, iDelta, 0);
}

///////////////////////////////////////////////////////////////////////////
//
//	Implement CRollUpWnd
//
///////////////////////////////////////////////////////////////////////////

CRollUpWnd::CRollUpWnd()
{
	m_bDrag		= false;
	m_iTotalHei	= 0;
	m_iOffset	= 0;

	m_iNumPanelOnMenu	= 0;
}

CRollUpWnd::~CRollUpWnd()
{
	//	Release all panels
	for (int i=0; i < m_aPanels.GetSize(); i++)
	{
		delete m_aPanels[i];
	}

	m_aPanels.RemoveAll();
}

BEGIN_MESSAGE_MAP(CRollUpWnd, CWnd)
	//{{AFX_MSG_MAP(CRollUpWnd)
	ON_WM_SIZE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_DESTROY()
	ON_WM_RBUTTONDOWN()
	//}}AFX_MSG_MAP

	ON_MESSAGE(WM_ROLLUPTITLECLICK, OnRollUpTitleClick)
	ON_MESSAGE(WM_ROLLUPBEGINDRAG, OnRollUpBeginDrag)
	ON_MESSAGE(WM_ROLLUPROLLDOWN, OnRollUpRollDown)
	ON_MESSAGE(WM_ROLLUPPOPUPMENU, OnRollUpPopupMenu)

END_MESSAGE_MAP()

BOOL CRollUpWnd::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext) 
{
	dwStyle |= (WS_CLIPCHILDREN | WS_CLIPSIBLINGS);

	if (!CWnd::CreateEx(0, AfxRegisterWndClass(CS_VREDRAW | CS_HREDRAW | CS_DBLCLKS,
						LoadCursor(NULL, IDC_SIZEALL),
						/*(HBRUSH)::GetStockObject(WHITE_BRUSH)),/*/HBRUSH(COLOR_BTNFACE+1)),
						lpszWindowName, 
						dwStyle,
						rect, 
						pParentWnd, 
						nID, 
						pContext))
		return FALSE;

	RECT rcWnd, rcClient;
	GetClientRect(&rcClient);

	//	Create scroll bar
	SetRect(&rcWnd, rcClient.right-SIZE_SCROLLBAR, 0, rcClient.right, rcClient.bottom);
	if (!m_ScrollBar.Create(this, rcWnd, 101))
		return FALSE;

	//	Create right button popup menu
	m_PopupMenu.CreatePopupMenu();
	m_PopupMenu.AppendMenu(MF_STRING, MENUCMD_EXPANDALL, "全部展开");
	m_PopupMenu.AppendMenu(MF_STRING, MENUCMD_COLLAPSEALL, "全部关闭");

	m_iNumPanelOnMenu = 0;

	return TRUE;
}

void CRollUpWnd::OnDestroy() 
{
	CWnd::OnDestroy();
}

void CRollUpWnd::OnSize(UINT nType, int cx, int cy) 
{
	CWnd::OnSize(nType, cx, cy);

	if (!m_ScrollBar.m_hWnd)
		return;

	RECT rcWnd, rcClient;
	GetClientRect(&rcClient);

	//	Resize scroll bar
	SetRect(&rcWnd, rcClient.right-SIZE_SCROLLBAR, 0, rcClient.right, rcClient.bottom);
	m_ScrollBar.MoveWindow(&rcWnd);

	ArrangeVisiblePanels();
}

/*	Create a panel

	Return panel's index for success, otherwise return -1 for false
*/
int CRollUpWnd::CreatePanel(const char* szName, DWORD dwStyles, CDialog* pDialog)
{
	//	Check name collision
	if (SearchPanel(szName, false) >= 0)
		return false;

	CRollUpPanel* pPanel = new CRollUpPanel;
	if (!pPanel)
		return false;

	RECT rc = {0, 0, 100, 100};
	if (!pPanel->Create(szName, dwStyles, pDialog, this, rc, 0))
		return false;

	//	Add to panel array
	return m_aPanels.Add(pPanel);
}

//	Show / Hide a panel
bool CRollUpWnd::ShowPanel(int iIndex, bool bOpen)
{
	CRollUpPanel* pPanel = m_aPanels[iIndex];
	if (pPanel->IsVisible())
	{
		//	Panels has been visible, check open state
		if (!pPanel->HasTitle())
			bOpen = true;

		if (pPanel->IsOpen() == bOpen)
			return true;

		//	Panel has been shown, so only change it's open state
		if (bOpen)
			OpenPanel(iIndex);
		else if (pPanel->IsCloseable())
			ClosePanel(iIndex);

		return true;
	}

	//	Change panel open state
	if (!pPanel->HasTitle())
		bOpen = true;

	pPanel->Open(bOpen);
	pPanel->Show(true);

	//	Rearrange visible panels
	ArrangeVisiblePanels();

	return true;
}

//	Hide a panel
bool CRollUpWnd::HidePanel(int iIndex)
{
	CRollUpPanel* pPanel = m_aPanels[iIndex];
	if (!pPanel->IsVisible())
		return true;	//	Panel has been hidden

	pPanel->Show(false);
	ArrangeVisiblePanels();

	return true;
}

//	Open / Close a panel
bool CRollUpWnd::OpenPanel(int iIndex)
{
	CRollUpPanel* pPanel = m_aPanels[iIndex];
	if (!pPanel->IsVisible())
		return true;	//	Panel is hidden

	pPanel->Open(true);
	ArrangeVisiblePanels();

	return true;
}

//	Close a panel
bool CRollUpWnd::ClosePanel(int iIndex)
{
	CRollUpPanel* pPanel = m_aPanels[iIndex];
	if (!pPanel->IsVisible())
		return true;	//	Panel is hidden

	pPanel->Open(false);
	ArrangeVisiblePanels();

	return true;
}

//	Open all visible panels
void CRollUpWnd::OpenAllPanels()
{
	for (int i=0; i < m_aPanels.GetSize(); i++)
	{
		CRollUpPanel* pPanel = m_aPanels[i];
		if (pPanel->IsVisible())
			pPanel->Open(true);
	}

	ArrangeVisiblePanels();
}

//	Close all visible panels
void CRollUpWnd::CloseAllPanels()
{
	for (int i=0; i < m_aPanels.GetSize(); i++)
	{
		CRollUpPanel* pPanel = m_aPanels[i];
		if (pPanel->IsVisible())
			pPanel->Open(false);
	}

	ArrangeVisiblePanels();
}

//	Hide all visible panels
void CRollUpWnd::HideAllPanels()
{
	for (int i=0; i < m_aPanels.GetSize(); i++)
	{
		CRollUpPanel* pPanel = m_aPanels[i];
		if (pPanel->IsVisible())
			pPanel->Show(false);
	}

	ArrangeVisiblePanels();
}

//	Search a panel
//	bVisOnly: true, only do search in visible panels
int CRollUpWnd::SearchPanel(const char* szName, bool bVisOnly)
{
	if (bVisOnly)
	{
		for (int i=0; i < m_aPanels.GetSize(); i++)
		{
			CRollUpPanel* pPanel = m_aPanels[i];
			if (pPanel->IsVisible() && !stricmp(szName, pPanel->GetPanelName()))
				return i;
		}
	}
	else
	{
		for (int i=0; i < m_aPanels.GetSize(); i++)
		{
			CRollUpPanel* pPanel = m_aPanels[i];
			if (!stricmp(szName, pPanel->GetPanelName()))
				return i;
		}
	}

	return -1;
}

//	Arrange visible panels' position and size
void CRollUpWnd::ArrangeVisiblePanels()
{
	RECT rc, rcClient;
	GetClientRect(&rcClient);

	int iFrameWid = rcClient.right - SIZE_RIGHTSPACE;
	int iFrameHei = rcClient.bottom;
	int iOldOff	= m_iOffset;

	rc.left	 = 0;
	rc.right = iFrameWid;

	m_iTotalHei	= 0;

	//	Scroll window to origin
	if (m_iOffset)
		ScrollSlide(-m_iOffset);

	//	Calculate all panels' position
	for (int i=0; i < m_aPanels.GetSize(); i++)
	{
		CRollUpPanel* pPanel = m_aPanels[i];
		if (!pPanel->IsVisible())
			continue;	//	Panel is hidden
		
		m_iTotalHei += SIZE_PANELSPACE;

		int iHei = pPanel->GetCurHeight();

		rc.top		= m_iTotalHei;
		rc.bottom	= rc.top + iHei;

		pPanel->MoveWindow(&rc);

		m_iTotalHei += iHei;
	}

	//	Add bottom border
	if (m_iTotalHei)
		m_iTotalHei += SIZE_PANELSPACE;

	if (m_iTotalHei <= iFrameHei)
	{
		//	m_iOffset = 0;
	}
	else if (m_iTotalHei + iOldOff < iFrameHei)
	{
		ScrollSlide(iFrameHei - m_iTotalHei);
	}
	else if (iOldOff)
	{
		ScrollSlide(iOldOff);
	}

	//	Update scroll bar
	UpdateScrollBar();
}

//	Show a group of panels
bool CRollUpWnd::ShowPanels(int* aIndices, bool* aOpenFlags, int iNumPanel)
{
	ASSERT(aIndices && aOpenFlags && iNumPanel >= 0);

	for (int i=0; i < iNumPanel; i++)
	{
		CRollUpPanel* pPanel = m_aPanels[aIndices[i]];
		bool bOpen = aOpenFlags[i];

		if (pPanel->IsVisible())
		{
			//	Panels has been visible, check open state
			if (!pPanel->HasTitle())
				bOpen = true;

			if (pPanel->IsOpen() == bOpen)
				continue;

			//	Panel has been shown, so only change it's open state
			pPanel->Open(bOpen);
		}
		else
		{
			//	Change panel open state
			if (!pPanel->HasTitle())
				bOpen = true;

			pPanel->Open(bOpen);
			pPanel->Show(true);
		}
	}

	//	Arrange all visible panels
	ArrangeVisiblePanels();

	return true;
}

//	Hide a group of panels
bool CRollUpWnd::HidePanels(int* aIndices, int iNumPanel)
{
	ASSERT(aIndices && iNumPanel >= 0);

	for (int i=0; i < iNumPanel; i++)
	{
		CRollUpPanel* pPanel = m_aPanels[aIndices[i]];

		if (!pPanel->IsVisible())
			continue;	//	Panel has been hidden

		pPanel->Show(false);
	}

	//	Arrange all visible panels
	ArrangeVisiblePanels();

	return true;
}

//	WM_ROLLUPTITLECLICK handler
LRESULT CRollUpWnd::OnRollUpTitleClick(WPARAM wParam, LPARAM lParam)
{
	//	Arrange all visible panels
	CRollUpPanel* pPanel = (CRollUpPanel*)wParam;
	ASSERT(pPanel);

	int i;
	
	//	Get panel's index
	for (i=0; i < m_aPanels.GetSize(); i++)
	{
		if (m_aPanels[i] == pPanel)
			break;
	}

	ASSERT(i < m_aPanels.GetSize());

	if (!pPanel->IsOpen())
		OpenPanel(i);
	else
		ClosePanel(i);

	return TRUE;
}

//	WM_ROLLUPBEGINDRAG handler
LRESULT CRollUpWnd::OnRollUpBeginDrag(WPARAM wParam, LPARAM lParam)
{
	//	Begin drag slide
	BeginDrag();
	return TRUE;
}

//	WM_ROLLUPROLLDOWN handler
LRESULT CRollUpWnd::OnRollUpRollDown(WPARAM wParam, LPARAM lParam)
{
	RollDown(-(int)wParam);
	return TRUE;
}

//	Begin drag slide
void CRollUpWnd::BeginDrag()
{
	m_bDrag	= true;
	GetCursorPos(&m_ptLastMou);
	SetCapture();
}

void CRollUpWnd::OnLButtonDown(UINT nFlags, CPoint point) 
{
	if (!m_bDrag)
	{
		//	Begin drag slide
		BeginDrag();
	}
	
	CWnd::OnLButtonDown(nFlags, point);
}

void CRollUpWnd::OnLButtonUp(UINT nFlags, CPoint point) 
{
	if (m_bDrag)
	{
		m_bDrag = false;
		ReleaseCapture();
	}
	
	CWnd::OnLButtonUp(nFlags, point);
}

void CRollUpWnd::OnMouseMove(UINT nFlags, CPoint point) 
{
	if (!m_bDrag)
	{
		CWnd::OnMouseMove(nFlags, point);
		return;
	}

	POINT pt = {point.x, point.y};
	ClientToScreen(&pt);

	int iDelta = pt.y - m_ptLastMou.y;
	if (iDelta)
		RollDown(iDelta);

	m_ptLastMou = pt;

	CWnd::OnMouseMove(nFlags, point);
}

//	Roll up and down. iDelta > 0: roll up; iDelta < 0: roll down
void CRollUpWnd::RollDown(int iDelta)
{
	RECT rc;
	GetClientRect(&rc);
	int iFrameHei = rc.bottom - rc.top;

	if (m_iTotalHei <= iFrameHei)
		return;

	int iOffset = iDelta + m_iOffset;

	if (iOffset > 0)
		iOffset = 0;
	else if (m_iTotalHei + iOffset < iFrameHei)
		iOffset = iFrameHei - m_iTotalHei;

	if (iOffset == m_iOffset)
		return;

	ScrollSlideTo(iOffset);

	//	Update scroll bar
	UpdateScrollBar();
}

//	Scroll slide to specified offset
void CRollUpWnd::ScrollSlideTo(int iNewOff)
{ 
	ScrollSlide(iNewOff - m_iOffset);
}

//	Scroll slide
void CRollUpWnd::ScrollSlide(int iDelta)
{
	ScrollWindow(0, iDelta);
	m_iOffset += iDelta;

	RECT rc;

	//	Remove scroll bar's position.
	m_ScrollBar.GetWindowRect(&rc);
	ScreenToClient(&rc);
	OffsetRect(&rc, 0, -iDelta);
	m_ScrollBar.MoveWindow(&rc);
}

//	Update scroll bar
void CRollUpWnd::UpdateScrollBar()
{
	RECT rc;
	GetClientRect(&rc);

	if (m_iTotalHei)
	{
		m_ScrollBar.SetRange(0, m_iTotalHei-1, rc.bottom);
		m_ScrollBar.SetPos(-m_iOffset);
	}
	else
	{
		m_ScrollBar.SetRange(0, 0, rc.bottom);
		m_ScrollBar.SetPos(0);
	}

	m_ScrollBar.Update();
}

//	Get visible panel number
int CRollUpWnd::GetVisiblePanelNum()
{
	int i, iCount=0;

	for (i=0; i < m_aPanels.GetSize(); i++)
	{
		if (m_aPanels[i]->IsVisible())
			iCount++;
	}

	return iCount;
}

void CRollUpWnd::OnRButtonDown(UINT nFlags, CPoint point) 
{
	//	Popup menu
	POINT pt = {point.x, point.y};
	ClientToScreen(&pt);
	PopupMenu(pt.x, pt.y);

	CWnd::OnRButtonDown(nFlags, point);
}

BOOL CRollUpWnd::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	if (!(wParam & 0xffff0000))
	{
		//	Is from menu item
		WORD wID = wParam & 0x0000ffff;

		if (wID == MENUCMD_EXPANDALL)
			OpenAllPanels();
		else if (wID == MENUCMD_COLLAPSEALL)
			CloseAllPanels();
		else if (wID >= MENUCMD_PANEL0 && wID < MENUCMD_PANEL0 + m_iNumPanelOnMenu)
		{
			int iIndex = wID - MENUCMD_PANEL0;
			bool bOpen = !m_aPanels[iIndex]->IsOpen();
			if (bOpen)
				OpenPanel(iIndex);
			else
				ClosePanel(iIndex);
		}
	}
	
	return CWnd::OnCommand(wParam, lParam);
}

//	WM_ROLLUPPOPUPMENU handler
LRESULT CRollUpWnd::OnRollUpPopupMenu(WPARAM wParam, LPARAM lParam)
{
	PopupMenu((int)wParam, (int)lParam);
	return TRUE;
}

//	Popup menu
void CRollUpWnd::PopupMenu(int x, int y)
{
	int i;

	//	Delete old menu items
	if (m_iNumPanelOnMenu)
	{
		for (i=0; i < m_iNumPanelOnMenu+1; i++)
			m_PopupMenu.DeleteMenu(2, MF_BYPOSITION);
	}

	m_iNumPanelOnMenu = 0;

	//	Add new popup menu items
	for (i=0; i < m_aPanels.GetSize(); i++)
	{
		CRollUpPanel* pPanel = m_aPanels[i];
		if (!pPanel->IsVisible())
			continue;

		if (!m_iNumPanelOnMenu)
			m_PopupMenu.AppendMenu(MF_SEPARATOR, 0);

		UINT uFlags = MF_STRING;
		if (pPanel->IsOpen())
			uFlags |= MF_CHECKED;

		if (!pPanel->HasTitle() || !pPanel->IsCloseable())
			uFlags |= MF_GRAYED;

		m_PopupMenu.AppendMenu(uFlags, MENUCMD_PANEL0+m_iNumPanelOnMenu, pPanel->GetPanelName());

		m_iNumPanelOnMenu++;
	}

	if (m_iNumPanelOnMenu)
	{
		m_PopupMenu.EnableMenuItem(MENUCMD_EXPANDALL, MF_BYCOMMAND | MF_ENABLED);
		m_PopupMenu.EnableMenuItem(MENUCMD_COLLAPSEALL, MF_BYCOMMAND | MF_ENABLED);
	}
	else
	{
		m_PopupMenu.EnableMenuItem(MENUCMD_EXPANDALL, MF_BYCOMMAND | MF_GRAYED);
		m_PopupMenu.EnableMenuItem(MENUCMD_COLLAPSEALL, MF_BYCOMMAND | MF_GRAYED);
	}

	//	Popup menu
	m_PopupMenu.TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON, x, y, this);
}

