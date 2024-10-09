// StatusEdit.cpp : implementation file
//

#include "stdafx.h"
#include "StatusControl.h"
#include "StatusEdit.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CStatusEdit

CStatusEdit::CStatusEdit()
{
}

CStatusEdit::~CStatusEdit()
{
}


BEGIN_MESSAGE_MAP(CStatusEdit, CEdit)
	//{{AFX_MSG_MAP(CStatusEdit)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CStatusEdit message handlers

/****************************************************************************
*                              CStatusEdit::Create
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

BOOL CStatusEdit::Create(CStatusBar * parent, UINT id, DWORD style)
    {
     CRect r;

     CStatusControl::setup(parent, id, r);

     BOOL result = CEdit::Create(style | WS_CHILD, r, parent, id);
     if(!result)
	return FALSE;

     CFont * f = parent->GetFont();
     SetFont(f);
     return TRUE;
    }
