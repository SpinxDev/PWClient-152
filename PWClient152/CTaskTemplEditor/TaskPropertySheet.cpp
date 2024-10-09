// TaskPropertySheet.cpp : implementation file
//

#include "stdafx.h"
#include "TaskTemplEditor.h"
#include "TaskPropertySheet.h"
#include "TaskTemplEditorView.h"

#ifdef _DEBUG
#define new A_DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTaskPropertySheet

IMPLEMENT_DYNAMIC(CTaskPropertySheet, CPropertySheet)

CTaskPropertySheet::CTaskPropertySheet(UINT nIDCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(nIDCaption, pParentWnd, iSelectPage)
{
}

CTaskPropertySheet::CTaskPropertySheet(LPCTSTR pszCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(pszCaption, pParentWnd, iSelectPage)
{
}

CTaskPropertySheet::~CTaskPropertySheet()
{
}


BEGIN_MESSAGE_MAP(CTaskPropertySheet, CPropertySheet)
	//{{AFX_MSG_MAP(CTaskPropertySheet)
	//}}AFX_MSG_MAP
	ON_NOTIFY(TCN_SELCHANGING, AFX_IDC_TAB_CONTROL, OnSelchangedPage)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTaskPropertySheet message handlers

extern BOOL _view_mode;
extern bool g_DataReady;

void CTaskPropertySheet::OnSelchangedPage(NMHDR* pNMHDR, LRESULT* pResult)
{
	*pResult = 0;
	if (_view_mode || !g_DataReady) return;
	if (AfxGetEditorView()->GetDlgItem(IDC_APPLY)->IsWindowEnabled())
		AfxGetEditorView()->OnApply();
}
