// StatusProgress.cpp : implementation file
//

#include "stdafx.h"
#include "StatusControl.h"
#include "StatusProgress.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CStatusProgress

CStatusProgress::CStatusProgress()
{
}

CStatusProgress::~CStatusProgress()
{
}


BEGIN_MESSAGE_MAP(CStatusProgress, CProgressCtrl)
	//{{AFX_MSG_MAP(CStatusProgress)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CStatusProgress message handlers

/****************************************************************************
*                              CStatusProgress::Create
* Inputs:
*	CStatusBar * parent: Parent window, the status bar
*	UINT id: Control id
*	DWORD style: Style flags
* Result: BOOL
*       TRUE if success
*	FALSE if error
* Effect:
*       Creates a static control. Sets the pane text to the empty string.
****************************************************************************/

BOOL CStatusProgress::Create(CStatusBar * parent, UINT id, DWORD style)
    {
     CRect r;

     CStatusControl::setup(parent, id, r);

     return CProgressCtrl::Create(style | WS_CHILD, r, parent, id);
    }
