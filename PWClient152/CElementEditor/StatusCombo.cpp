// StatusCombo.cpp : implementation file
//

#include "stdafx.h"
#include "StatusControl.h"
#include "StatusCombo.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CStatusCombo

CStatusCombo::CStatusCombo()
{
 maxlen = 0;
}

CStatusCombo::~CStatusCombo()
{
}


BEGIN_MESSAGE_MAP(CStatusCombo, CComboBox)
	//{{AFX_MSG_MAP(CStatusCombo)
        ON_CONTROL_REFLECT(CBN_DROPDOWN, OnDropdown)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CStatusCombo message handlers

/****************************************************************************
*                              CStatusCombo::Create
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

BOOL CStatusCombo::Create(CStatusBar * parent, UINT id, DWORD style)
   {
    CRect r;

    CStatusControl::setup(parent, id, r);

    BOOL result = CComboBox::Create(style | WS_CHILD, r, parent, id);
    if(!result)
       return FALSE;

    CFont * f = parent->GetFont();
    SetFont(f);
    return TRUE;
   }

/****************************************************************************
*                          CStatusCombo::OnDropdown
* Result: void
*       
* Effect: 
*       Adjusts the combo box to be the right size for the number of items
****************************************************************************/

void CStatusCombo::OnDropdown() 
   {
    int n = GetCount();
    n = max(n, 2);

    int ht = GetItemHeight(0);
    CRect r;
    GetWindowRect(&r);

    if(maxlen > 0)
       n = max(maxlen, 2);

    CSize sz;
    sz.cx = r.Width();
    sz.cy = ht * (n + 2);

    if(maxlen == 0)
       { /* screen limit */
	if(r.top - sz.cy < 0 || r.bottom + sz.cy > ::GetSystemMetrics(SM_CYSCREEN))
	   { /* invoke limit */
	 // Compute the largest distance the dropdown can appear, 
	 // relative to the screen (not the window!)

	    int k = max( (r.top / ht), 
			 (::GetSystemMetrics(SM_CYSCREEN) - r.bottom) / ht);

	 // compute new space. Note that we don't really fill the screen.
	 // We only have to use this size if it is smaller than the max size
	 // actually required
	    int ht2 = ht * k;
	    sz.cy = min(ht2, sz.cy);
	   } /* invoke limit */
       } /* screen limit */

    SetWindowPos(NULL, 0, 0, sz.cx, sz.cy, SWP_NOMOVE | SWP_NOZORDER);

   }
