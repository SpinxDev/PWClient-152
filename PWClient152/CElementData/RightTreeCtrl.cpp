// RightTreeCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "elementdata.h"
#include "RightTreeCtrl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CRightTreeCtrl

CRightTreeCtrl::CRightTreeCtrl()
{
}

CRightTreeCtrl::~CRightTreeCtrl()
{
}


BEGIN_MESSAGE_MAP(CRightTreeCtrl, CTreeCtrl)
	//{{AFX_MSG_MAP(CRightTreeCtrl)
	ON_WM_RBUTTONDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRightTreeCtrl message handlers

void CRightTreeCtrl::OnRButtonDown(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	UINT nHF;
	HTREEITEM hItem = HitTest(point, &nHF);
	if (hItem) SelectItem(hItem);
	
	CTreeCtrl::OnRButtonDown(nFlags, point);
}
