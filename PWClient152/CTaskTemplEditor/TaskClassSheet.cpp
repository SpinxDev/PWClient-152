// TaskClassSheet.cpp : implementation file
//

#include "stdafx.h"
#include "TaskTemplEditor.h"
#include "TaskClassSheet.h"

#ifdef _DEBUG
#define new A_DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTaskClassSheet

IMPLEMENT_DYNAMIC(CTaskClassSheet, CPropertySheet)

CTaskClassSheet::CTaskClassSheet(UINT nIDCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(nIDCaption, pParentWnd, iSelectPage)
{
}

CTaskClassSheet::CTaskClassSheet(LPCTSTR pszCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(pszCaption, pParentWnd, iSelectPage)
{
}

CTaskClassSheet::~CTaskClassSheet()
{
}


BEGIN_MESSAGE_MAP(CTaskClassSheet, CPropertySheet)
	//{{AFX_MSG_MAP(CTaskClassSheet)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTaskClassSheet message handlers
