/*
 * FILE: ImageWnd.h
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Duyuxin, 2004/6/22
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
//	Class CImageWnd
//	
///////////////////////////////////////////////////////////////////////////

class CImageWnd : public CWnd
{
public:		//	Types

public:		//	Construction and Destructions

	CImageWnd();
	virtual ~CImageWnd();

public:		//	Attributes

public:		//	Operations

	//	Create scroll bar
	bool Create(DWORD dwStyle, CWnd* pParent, const RECT& rc, DWORD dwID);
	//	Load image from file
	bool LoadImage(const char* szFile);
	//	Set bitmap
	void SetBitmap(HBITMAP hBitmap, bool bOwnBmp);

	//	Get image file name
	const char* GetFileName() { return m_strFileName; }

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CImageWnd)
	//}}AFX_VIRTUAL

// Implementation
public:

	// Generated message map functions

protected:	//	Attributes

	IPicture*	m_pPicture;
	HBITMAP		m_hBitmap;
	bool		m_bOwnBmp;			//	true, this class can delete m_hBitmap
	CString		m_strFileName;

protected:	//	Operations

	//	Release image
	void ReleaseImage();

	//{{AFX_MSG(CImageWnd)
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

///////////////////////////////////////////////////////////////////////////
//	
//	Inline functions
//	
///////////////////////////////////////////////////////////////////////////

