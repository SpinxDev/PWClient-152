/*
 * FILE: DlgBars.cpp
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Duyuxin, 2004/6/17
 *
 * HISTORY: 
 *
 * Copyright (c) 2004 Archosaur Studio, All Rights Reserved.
 */

#include "StdAfx.h"
#include <afxpriv.h>
#include "DlgBars.h"

///////////////////////////////////////////////////////////////////////////
//	
//	Define and Macro
//	
///////////////////////////////////////////////////////////////////////////


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
//	Implement CDlgToolBar
//	
///////////////////////////////////////////////////////////////////////////

CDlgToolBar::CDlgToolBar()
{
}

CDlgToolBar::~CDlgToolBar()
{
}

BEGIN_MESSAGE_MAP(CDlgToolBar, CToolBar)
	//{{AFX_MSG_MAP(CDlgToolBar)
	ON_MESSAGE(WM_IDLEUPDATECMDUI, OnIdleUpdateCmdUI)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/*	OnIdleUpdateCmdUI handles the WM_IDLEUPDATECMDUI message, which is 
    used to update the status of user-interface elements within the MFC 
	framework.

	We have to get a little tricky here: CToolBar::OnUpdateCmdUI 
    expects a CFrameWnd pointer as its first parameter.  However, it
    doesn't do anything but pass the parameter on to another function
    which only requires a CCmdTarget pointer.  We can get a CWnd pointer
    to the parent window, which is a CCmdTarget, but may not be a 
 	CFrameWnd.  So, to make CToolBar::OnUpdateCmdUI happy, we will call
    our CWnd pointer a CFrameWnd pointer temporarily.  	

	lParam of WM_IDLEUPDATECMDUI isn't used by MFC, but we use it as force
	update flag here.
*/
LRESULT CDlgToolBar::OnIdleUpdateCmdUI(WPARAM wParam, LPARAM lParam)
{
	if (lParam || IsWindowVisible()) 
	{
		CFrameWnd *pParent = (CFrameWnd*)GetParent();
		if (pParent)
			OnUpdateCmdUI(pParent, (BOOL)wParam);
	}

	return 0L;
}

///////////////////////////////////////////////////////////////////////////
//	
//	Implement CDlgStatusBar
//	
///////////////////////////////////////////////////////////////////////////

CDlgStatusBar::CDlgStatusBar()
{
}

CDlgStatusBar::~CDlgStatusBar()
{
} 

BEGIN_MESSAGE_MAP(CDlgStatusBar, CStatusBar)
	//{{AFX_MSG_MAP(CDlgStatusBar)
	ON_MESSAGE(WM_IDLEUPDATECMDUI, OnIdleUpdateCmdUI)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/*	OnIdleUpdateCmdUI handles the WM_IDLEUPDATECMDUI message, which is 
    used to update the status of user-interface elements within the MFC 
	framework.

	We have to get a little tricky here: CDlgStatusBar::OnUpdateCmdUI 
    expects a CFrameWnd pointer as its first parameter.  However, it
    doesn't do anything but pass the parameter on to another function
    which only requires a CCmdTarget pointer.  We can get a CWnd pointer
    to the parent window, which is a CCmdTarget, but may not be a 
 	CFrameWnd.  So, to make CDlgStatusBar::OnUpdateCmdUI happy, we will call
    our CWnd pointer a CFrameWnd pointer temporarily.  	
*/
LRESULT CDlgStatusBar::OnIdleUpdateCmdUI(WPARAM wParam, LPARAM lParam)
{
	if (IsWindowVisible()) 
	{
		CFrameWnd *pParent = (CFrameWnd*)GetParent();
		if (pParent)
			OnUpdateCmdUI(pParent, (BOOL)wParam);
	}

	return 0L;
}
