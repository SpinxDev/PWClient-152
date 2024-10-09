/*
 * FILE: OptionsPage.h
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Duyuxin, 2004/6/18
 *
 * HISTORY: 
 *
 * Copyright (c) 2004 Archosaur Studio, All Rights Reserved.
 */

#pragma once

#include "RollUpWnd.h"
#include "DlgOptions.h"
#include "DlgRenderOpt.h"
#include "BrushSetPanel.h"

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
//	Class COptionsPage
//	
///////////////////////////////////////////////////////////////////////////

class COptionsPage : public CRollUpWnd
{
public:		//	Types

	//	Index of panels
	enum
	{
		PANEL_OPTION = 0,
		PANEL_RENDER = 1,
		PANEL_BRUSH  = 2,
		NUM_PANEL,
	};

public:		//	Constructor and Destructor

	COptionsPage();
	virtual ~COptionsPage();

public:		//	Attributes

public:		//	Operations

	//	Initlaize device objects
	bool InitDeviceObjects();
	//	Release resources
	void Release();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(COptionsPage)
	public:
	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = NULL);
	//}}AFX_VIRTUAL

// Implementation
public:

	// Generated message map functions

protected:	//	Attributes

	CDlgOptions		m_OptionPanel;
	CDlgRenderOpt	m_RenderPanel;
	CBrushSetPanel  m_BrushSetPanel;

protected:	//	Operations

	//{{AFX_MSG(COptionsPage)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

///////////////////////////////////////////////////////////////////////////
//	
//	Inline functions
//	
///////////////////////////////////////////////////////////////////////////

