// StatusControls.cpp : implementation file
//

/*-----------------------------------------------------------------------------
Note:

   The code in this class is adapted from the article "Showing progress bar in
   a status bar pane", by Brad Mann, as amended by Thoams Stadler.

   I took this code and created this MFC class that handles a static
   control (which I used for an icon). The use is illustrated in the
   accompanying example code.

   The creation call involves passing in the window reference for the
   status bar window and the ID of the pane. This creates a static
   control over the pane. 

   In the OnSize handler for CMainFrame, add the code

   	mycontrol.Reposition();

   for each such control you have created.

-----------------------------------------------------------------------------*/

#include "stdafx.h"
#include "StatusControl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CStatusControl

CStatusControl::CStatusControl()
{
}

CStatusControl::~CStatusControl()
{
}


BEGIN_MESSAGE_MAP(CStatusControl, CWnd)
	//{{AFX_MSG_MAP(CStatusControl)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CStatusControls message handlers

/****************************************************************************
*                            CStatusControl::setup
* Inputs:
*	CStatusBar * parent: Parent window (status bar)
*	UINT id: ID of pane
*	CRect & r: Place to put rectangle
* Result: BOOL
*       TRUE if successful
*	FALSE if the area is off-window
* Effect: 
*       Computes the rectangle for the pane, given the status bar and id
****************************************************************************/

BOOL CStatusControl::setup(CStatusBar * parent, UINT id, CRect & r)
    {
     int i = parent->CommandToIndex(id);

     parent->GetItemRect(i, &r);

     parent->SetPaneText(i, "");

     // If the pane was not visible, GetItemRect has returned a
     // (0, 0, 0, 0) rectangle. Attempting to create the control
     // using this rectangle creates it, possibly of zero size,
     // at the left of the status bar. We correct this by
     // forcing it to be off the visible right end of the status
     // bar. If the programmer properly handles the parent frame's
     // OnSize method, when the control becomes visible it will
     // move to the correct location.
     if(r.IsRectEmpty())
	{ /* offscreen */
	 CRect r1;
	 parent->GetWindowRect(&r1); // get parent width
	 r.left = r1.right + 1;
	 r.top =  r1.top;
	 r.right = r1.right + 2;
	 r.bottom = r1.bottom;
	 return FALSE;
	} /* offscreen */

     return TRUE;
    }

/****************************************************************************
*                         CStatusControl::reposition
* Inputs:
*       CWnd * wnd: Window to reposition
* Result: void
*       
* Effect: 
*       Repositions the control
****************************************************************************/

void CStatusControl::reposition(CWnd * wnd)
    {
     if(wnd == NULL || wnd->m_hWnd == NULL)
         return;
     UINT id = ::GetWindowLong(wnd->m_hWnd, GWL_ID);
     CRect r;

     // Note that because the control ID is the same as the
     // pane ID, this actually works well enough that
     // no special variable is required to obtain the
     // pane index.
     CStatusBar * parent = (CStatusBar *)wnd->GetParent();
     int i = parent->CommandToIndex(id);
     parent->GetItemRect(i, &r);
     wnd->SetWindowPos(&wndTop, r.left, r.top, r.Width(), r.Height(), 0);
    }

/****************************************************************************
*                              CStatusControl::Create
* Inputs:
*	LPCTSTR classname
*	CStatusBar * parent: Parent window, the status bar
*	UINT id: Control id
*	DWORD style: Style flags
* Result: BOOL
*       TRUE if success
*	FALSE if error
* Effect:
*       Creates a static control. Sets the pane text to the empty string.
****************************************************************************/

BOOL CStatusControl::Create(LPCTSTR classname, CStatusBar * parent, UINT id, DWORD style)
    {
     CRect r;

     setup(parent, id, r);

     return CWnd::Create(classname, NULL, style | WS_CHILD, r, parent, id);
    }

/****************************************************************************
*                           CStatusControl::Reposition
* Result: void
*
* Effect:
*       Repositions the status window in the status bar
****************************************************************************/

void CStatusControl::Reposition()
    {
     reposition(this);
    }

