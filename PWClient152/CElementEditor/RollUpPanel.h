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

///////////////////////////////////////////////////////////////////////////
//
//	Define and Macro
//
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//
//	Types and Global variables
//
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//
//	Declare of Global functions
//
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//
//	Class CRollUpTitleBtn
//
///////////////////////////////////////////////////////////////////////////

class CRollUpTitleBtn : public CButton
{
public:		//	Types

public:		//	Construction and Destructions

public:		//	Attributes

public:		//	Operations

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRollUpTitleBtn)
	public:
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	//}}AFX_VIRTUAL

// Implementation
public:

	// Generated message map functions

protected:	//	Attributes

protected:	//	Operations

	//{{AFX_MSG(CRollUpTitleBtn)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

///////////////////////////////////////////////////////////////////////////
//
//	Class CRollUpPanel
//
///////////////////////////////////////////////////////////////////////////

class CRollUpPanel : public CWnd
{
public:		//	Types

	//	Panel styles
	enum
	{
		STYLE_NOCLOSE	= 0x0001,	//	If this flag is set, this group counld be closed
		STYLE_NOTITLE	= 0x0002,	//	No title, If this flag is set, STYLE_NOCLOSE will also be set
	};

	//	Panel flags
	enum
	{
		STATE_VISIBLE	= 0x0001,	//	Visible flag
		STATE_OPEN		= 0x0002,	//	Open flag
	};

	//	Some size
	enum
	{
		SIZE_TITLEHEI	= 20,		//	Title button height
		SIZE_BORDER		= 5,		//	Broder size
	};

	friend class CRollUpWnd;

public:		//	Construction and Destructions

	CRollUpPanel();
	virtual ~CRollUpPanel();

public:		//	Attributes

public:		//	Operations

	//	Create pancel
	bool Create(const char* szName, DWORD dwStyles, CWnd* pContentWnd, CWnd* pParent, const RECT& rc, DWORD dwID);

	//	Show / Hide pancel
	bool Show(bool bShow);
	//	Open / Close panel
	bool Open(bool bOpen);

	//	Get panel height in open state
	int GetOpenHeight()
	{ 
		if (HasTitle())
			return SIZE_TITLEHEI + m_iDlgHeight + SIZE_BORDER;
		else
			return m_iDlgHeight + SIZE_BORDER * 3;
	}
	//	Get panel height in close state
	int GetCloseHeight() { return SIZE_TITLEHEI; }
	//	Get panel current height
	int GetCurHeight() { return IsOpen() ? GetOpenHeight() : GetCloseHeight(); }
	//	Get panel name
	const char* GetPanelName() { return m_strName; }
	//	Set panel name
	void SetPanelName(const char* szName);

	//	Get panel styles
	DWORD GetPanelStyles() { return m_dwStyles; }
	//	Get panel states
	DWORD GetPanelStates() { return m_dwStates; }
	//	Panel is open ?
	bool IsOpen() { return (m_dwStates & STATE_OPEN) ? true : false; }
	//	Panel is visible ?
	bool IsVisible() { return (m_dwStates & STATE_VISIBLE) ? true : false; }
	//	Panel is closeable ? 
	bool IsCloseable() { return !(m_dwStyles & STYLE_NOCLOSE) ? true : false; }
	//	Has title button ?
	bool HasTitle() { return !(m_dwStyles & STYLE_NOTITLE) ? true : false; }
	//	Get content window object
	CWnd* GetContentWnd() { return m_pContentWnd; }

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRollUpPanel)
	protected:
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
public:

	// Generated message map functions

protected:	//	Attributes

	static CString	m_strClassName;		//	Panel window class name

	CWnd*			m_pContentWnd;	//	Content window
	CRollUpTitleBtn	m_TitleBtn;		//	Title button
	CButton			m_BorderBtn;	//	Border button

	CString		m_strName;			//	Name of panel
	bool		m_bCreated;			//	Initialize flag
	DWORD		m_dwStyles;			//	Panel styles
	DWORD		m_dwStates;			//	Panel states
	int			m_iDlgHeight;		//	Dialog height

protected:	//	Operations

	//	Arrange child window's position and size
	void ArrangeChildrenPos();

	//{{AFX_MSG(CRollUpPanel)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

