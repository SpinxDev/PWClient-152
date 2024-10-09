// ComboItem.cpp : implementation file
//

#include "stdafx.h"
#include "ComboItem.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CComboItem

CComboItem::CComboItem( int nItem, int nSubItem, CStringList *psList )
{
	m_nItem		= nItem;
	m_nSubItem	= nSubItem;

	if ( psList )
		m_sList.AddTail( psList );

	m_bVK_ESCAPE = FALSE;

}

CComboItem::~CComboItem()
{
}


BEGIN_MESSAGE_MAP(CComboItem, CComboBox)
	//{{AFX_MSG_MAP(CComboItem)
	ON_WM_NCDESTROY()
	ON_WM_CHAR()
	ON_WM_KILLFOCUS()
	ON_CONTROL_REFLECT(CBN_CLOSEUP, OnCloseup)
	ON_WM_CREATE()
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CComboItem message handlers

BOOL CComboItem::PreTranslateMessage(MSG* pMsg) 
{
	// TODO: Add your specialized code here and/or call the base class
	if( pMsg->message == WM_KEYDOWN )	
	{		
		if(pMsg->wParam == VK_RETURN || pMsg->wParam == VK_ESCAPE)	
		{
			::TranslateMessage(pMsg);
			::DispatchMessage(pMsg);			
			return 1;
		}	
	}	
	
	return CComboBox::PreTranslateMessage(pMsg);
}

void CComboItem::OnNcDestroy() 
{
	CComboBox::OnNcDestroy();
	
	// TODO: Add your message handler code here
	delete this;	
}

void CComboItem::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	// TODO: Add your message handler code here and/or call default
	if(nChar == VK_ESCAPE || nChar == VK_RETURN)	
	{		
		if( nChar == VK_ESCAPE)
			m_bVK_ESCAPE = 1;
		GetParent()->SetFocus();		
		return;	
	}	
	
	CComboBox::OnChar(nChar, nRepCnt, nFlags);
}

void CComboItem::OnKillFocus(CWnd* pNewWnd) 
{
	int nIndex = GetCurSel();

	CComboBox::OnKillFocus(pNewWnd);

	CString str;	
	GetWindowText(str);
	// Send Notification to parent of ListView ctrl	
	LV_DISPINFO lvDispinfo;
	lvDispinfo.hdr.hwndFrom = GetParent()->m_hWnd;
	lvDispinfo.hdr.idFrom = GetDlgCtrlID();//that's us
	lvDispinfo.hdr.code = LVN_ENDLABELEDIT;
	lvDispinfo.item.mask = LVIF_TEXT | LVIF_PARAM;	
	lvDispinfo.item.iItem = m_nItem;
	lvDispinfo.item.iSubItem = m_nSubItem;
	lvDispinfo.item.pszText = m_bVK_ESCAPE ? NULL : LPTSTR((LPCTSTR)str);
	lvDispinfo.item.cchTextMax = str.GetLength();
	lvDispinfo.item.lParam = GetItemData(GetCurSel());
	if(nIndex!=CB_ERR)
		GetParent()->GetParent()->SendMessage(
			WM_NOTIFY,
			GetParent()->GetDlgCtrlID(),
			(LPARAM)&lvDispinfo);
	PostMessage(WM_CLOSE);
}

void CComboItem::OnCloseup() 
{
	GetParent()->SetFocus();	
}

int CComboItem::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CComboBox::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	CFont* font = GetParent()->GetFont();	
	SetFont(font);
	//add the items from CStringlist
	POSITION pos = m_sList.GetHeadPosition();
	while(pos != NULL)
		AddString((LPCTSTR)(m_sList.GetNext(pos)));	
	SetFocus();	
	
	return 0;
}

void CComboItem::OnSize(UINT nType, int cx, int cy) 
{
	CComboBox::OnSize(nType, cx, cy);
	
	// TODO: Add your message handler code here
	
}
