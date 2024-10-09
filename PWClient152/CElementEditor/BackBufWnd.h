/*
 * FILE: BackBufWnd.h
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Duyuxin, 2004/6/11
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
//	Class CBackBufWnd
//	
///////////////////////////////////////////////////////////////////////////

class CBackBufWnd : public CWnd
{
public:		//	Types

public:		//	Constructor and Destructor

	CBackBufWnd();
	virtual ~CBackBufWnd();

public:		//	Attributes

public:		//	Operations

	//	Update window
	bool Update();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBackBufWnd)
	//}}AFX_VIRTUAL

// Implementation
public:

	// Generated message map functions
	
protected:	//	Attributes

	HDC			m_hMemDC;		//	Memory DC used to display text
	HBITMAP		m_hBackBmp;		//	Handle of memory bitmap used by memory DC
	HBITMAP		m_hOldBmp;		//	Old bitmap used by memory DC
	LPBYTE		m_lpBackBuf;	//	Buffer used to store memory bitmap

protected:	//	Operations

	//	Create back buffer
	bool CreateBackBuffer();
	//	Release back buffer
	void ReleaseBackBuffer();
	//	Flip back buffer
	bool Flip(HDC hDC, RECT* lprc);
	//	Clear back buffer
	void ClearBackBuffer(DWORD dwCol);

	//	Draw back buffer
	virtual bool DrawBackBuffer(RECT* lprc) { return true; }

	//{{AFX_MSG(CBackBufWnd)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnDestroy();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

///////////////////////////////////////////////////////////////////////////
//	
//	Inline functions
//	
///////////////////////////////////////////////////////////////////////////

