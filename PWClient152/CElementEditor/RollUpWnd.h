/*
 * FILE: RollUpWnd.h
 *
 * DESCRIPTION: 3DMAX style roll up window
 *
 * CREATED BY: Duyuxin, 2004/6/9
 *
 * HISTORY:
 *
 * Copyright (c) 2004 Archosaur Studio, All Rights Reserved.
 */

#pragma once

#include "BackBufWnd.h"

///////////////////////////////////////////////////////////////////////////
//
//	Define and Macro
//
///////////////////////////////////////////////////////////////////////////

//	Message sent to CRollUpWnd when panel's title button was clicked
//	wParam: CRollUpPanel object's address
#define WM_ROLLUPTITLECLICK		(WM_USER + 5500)

//	Message sent to CRollUpWnd when user begin to drag the slide
#define WM_ROLLUPBEGINDRAG		(WM_USER + 5501)

//	Message sent to CRollUpWnd to tell it roll up or down
//	wParam: offset to roll, > 0 - roll down; < 0 - roll up
#define WM_ROLLUPROLLDOWN		(WM_USER + 5502)

//	Message sent to CRollUpWnd to popup menu
//	wParam: x position in screen
//	lParam: y position in screen
#define WM_ROLLUPPOPUPMENU		(WM_USER + 5503)

///////////////////////////////////////////////////////////////////////////
//
//	Types and Global variables
//
///////////////////////////////////////////////////////////////////////////

class CRollUpPanel;

///////////////////////////////////////////////////////////////////////////
//
//	Declare of Global functions
//
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//
//	Class CRollUpScrollBar
//
///////////////////////////////////////////////////////////////////////////

class CRollUpScrollBar : public CBackBufWnd
{
public:		//	Types

public:		//	Construction and Destructions

	CRollUpScrollBar();
	virtual ~CRollUpScrollBar() {}

public:		//	Attributes

public:		//	Operations

	//	Create scroll bar
	bool Create(CWnd* pParent, const RECT& rc, DWORD dwID);

	//	Set range
	void SetRange(int iMin, int iMax, int iPageSize);
	//	Get minimum limit
	int GetMinLimit() { return m_iMin; }
	//	Get maximum limit
	int GetMaxLimit() { return m_iMax; }
	//	Set current position
	void SetPos(int iPos);
	//	Get current position
	int GetPos() { return m_iCurPos; }
	//	Set page size
	void SetPageSize(int iPageSize);
	//	Get page size
	int GetPageSize() { return m_iPageSize; }

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRollUpScrollBar)
	//}}AFX_VIRTUAL

// Implementation
public:

	// Generated message map functions

protected:	//	Attributes

	static CString	m_strClassName;		//	Rollup slide window class name

	int		m_iMin;			//	Minimum limit
	int		m_iMax;			//	Maximum limit
	int		m_iRangeSize;	//	Range size
	int		m_iCurPos;		//	Current position
	int		m_iPageSize;	//	Page size

	POINT	m_ptLastMou;	//	Mouse's last position
	bool	m_bDrag;		//	Drag flag

protected:	//	Operations

	//	Clamp position
	void ClampPosition();
	//	Calculate scroll box's position and size
	void CalcScrollBox(RECT& rc);
	//	Roll up / down
	void RollDown(int iDelta, bool bLogicUnit);
	//	Convert pixel unit to logic unit
	int PixelToLogic(int iPixel);

	//	Draw back buffer
	virtual bool DrawBackBuffer(RECT* lprc);

	//{{AFX_MSG(CRollUpScrollBar)
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

///////////////////////////////////////////////////////////////////////////
//
//	Class CRollUpWnd
//
///////////////////////////////////////////////////////////////////////////

class CRollUpWnd : public CWnd
{
public:		//	Types

	//	Some constants
	enum
	{
		SIZE_PANELSPACE		= 4,	//	Space between panels
		SIZE_SCROLLBAR		= 6,	//	Width of scroll bar
		SIZE_SCROLLSPACE	= 4,	//	Space between scroll bar and slide window
		SIZE_RIGHTSPACE		= SIZE_SCROLLSPACE + SIZE_SCROLLBAR,
	};

public:		//	Construction and Destructions

	CRollUpWnd();
	virtual ~CRollUpWnd();

public:		//	Attributes

public:		//	Operations

	//	Create a panel
	int CreatePanel(const char* szName, DWORD dwStyles, CDialog* pDialog);
	//	Search a panel
	int SearchPanel(const char* szName, bool bVisOnly);

	//	Show a panel
	bool ShowPanel(int iIndex, bool bOpen);
	bool ShowPanelByName(const char* szName, bool bOpen)
	{
		int iIndex = SearchPanel(szName, false);
		if (iIndex >= 0)
			return ShowPanel(iIndex, bOpen);
		else
			return false;
	}

	//	Hide a panel
	bool HidePanel(int iIndex);
	bool HidePanelByName(const char* szName)
	{
		int iIndex = SearchPanel(szName, false);
		if (iIndex >= 0)
			return HidePanel(iIndex);
		else
			return false;
	}

	//	Show a group of panels
	bool ShowPanels(int* aIndices, bool* aOpenFlags, int iNumPanel);
	//	Hide a group of panels
	bool HidePanels(int* aIndices, int iNumPanel);

	//	Open a panel
	bool OpenPanel(int iIndex);
	bool OpenPanelByName(const char* szName)
	{
		int iIndex = SearchPanel(szName, false);
		if (iIndex >= 0)
			return OpenPanel(iIndex);
		else
			return false;
	}

	//	Close a panel
	bool ClosePanel(int iIndex);
	bool ClosePanelByName(const char* szName)
	{
		int iIndex = SearchPanel(szName, false);
		if (iIndex >= 0)
			return ClosePanel(iIndex);
		else
			return false;
	}

	//	Open all visible panels
	void OpenAllPanels();
	//	Close all visible panels
	void CloseAllPanels();
	//	Hide all visible panels
	void HideAllPanels();

	//	Get visible panel number
	int GetVisiblePanelNum();
	//	Get panel total number
	int GetAllPanelNum() { return m_aPanels.GetSize(); }
	//	Get panel object
	CRollUpPanel* GetPanel(int n) { return m_aPanels[n]; }

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRollUpWnd)
	public:
	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = NULL);
	protected:
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
public:

	// Generated message map functions

protected:	//	Attributes

	CRollUpScrollBar	m_ScrollBar;	//	Scroll bar

	CMenu	m_PopupMenu;		//	Right button menu
	int		m_iNumPanelOnMenu;	//	Panel number on menu

	POINT	m_ptLastMou;		//	Mouse's last position
	bool	m_bDrag;			//	Drag flag
	int		m_iOffset;			//	Roll offset, this value always <= 0
	int		m_iTotalHei;		//	Total height of all panels

	CArray<CRollUpPanel*, CRollUpPanel*>	m_aPanels;		//	Property panels

protected:	//	Operations

	//	Arrange visible panels' position and size
	void ArrangeVisiblePanels();

	//	Begin drag slide
	void BeginDrag();
	//	Roll up and down. iDelta > 0: roll up; iDelta < 0: roll down
	void RollDown(int iDelta);
	//	Scroll slide to specified offset
	void ScrollSlideTo(int iNewOff);
	//	Scroll slide
	void ScrollSlide(int iDelta);

	//	Update scroll bar
	void UpdateScrollBar();
	//	Popup menu
	void PopupMenu(int x, int y);

	//{{AFX_MSG(CRollUpWnd)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnDestroy();
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	//}}AFX_MSG

	//	WM_ROLLUPTITLECLICK handler
	afx_msg LRESULT OnRollUpTitleClick(WPARAM wParam, LPARAM lParam);
	//	WM_ROLLUPBEGINDRAG handler
	afx_msg LRESULT OnRollUpBeginDrag(WPARAM wParam, LPARAM lParam);
	//	WM_ROLLUPROLLDOWN handler
	afx_msg LRESULT OnRollUpRollDown(WPARAM wParam, LPARAM lParam);
	//	WM_ROLLUPPOPUPMENU handler
	afx_msg LRESULT OnRollUpPopupMenu(WPARAM wParam, LPARAM lParam);
	
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous li
