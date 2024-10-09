// StatusStatic.cpp : implementation file
//

#include "stdafx.h"
#include "StatusControl.h"
#include "StatusStatic.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CStatusStatic

CStatusStatic::CStatusStatic()
{
}

CStatusStatic::~CStatusStatic()
{
}


BEGIN_MESSAGE_MAP(CStatusStatic, CStatic)
	//{{AFX_MSG_MAP(CStatusStatic)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CStatusStatic message handlers

/****************************************************************************
*                              CStatusStatic::Create
* Inputs:
*	CStatusBar * parent: Parent window, the status bar
*	UINT id: Control id, also pane id
*	DWORD style: Style flags
* Result: BOOL
*       TRUE if success
*	FALSE if error
* Effect:
*       Creates a static control. Sets the pane text to the empty string.
****************************************************************************/

BOOL CStatusStatic::Create(CStatusBar * parent, UINT id, DWORD style)
    {
     CRect r;

     CStatusControl::setup(parent, id, r);

     BOOL result = CStatic::Create(NULL, style | WS_CHILD, r, parent, id);
     if(!result)
	return FALSE;
     CFont * f = parent->GetFont();
     SetFont(f);
     return TRUE;
    }
