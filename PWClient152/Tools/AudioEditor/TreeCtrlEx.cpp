#include "StdAfx.h"
#include "TreeCtrlEx.h"

#define TCEX_EDITLABEL 1		// Edit label timer event 

BEGIN_MESSAGE_MAP(CTreeCtrlEx, CTreeCtrl)
	ON_WM_LBUTTONDOWN() 
	ON_WM_LBUTTONUP() 
	ON_WM_MOUSEMOVE() 
	ON_WM_KEYDOWN() 
	ON_NOTIFY_REFLECT_EX(TVN_ITEMEXPANDING, OnItemexpanding) 
	ON_NOTIFY_REFLECT_EX(NM_SETFOCUS, OnSetfocus) 
	ON_NOTIFY_REFLECT_EX(NM_KILLFOCUS, OnKillfocus) 
	ON_NOTIFY_REFLECT_EX(TVN_SELCHANGED, OnSelchanged) 
	ON_WM_RBUTTONDOWN() 
	ON_WM_LBUTTONDBLCLK() 
	ON_WM_TIMER()	 
END_MESSAGE_MAP()

CTreeCtrlEx::CTreeCtrlEx(void)
{
	m_bMultiSelect = false;
	m_bSelectPending = FALSE;
	m_hClickedItem = NULL;
	m_hFirstSelectedItem = NULL;
	m_bSelectionComplete = TRUE;
	m_bEditLabelPending = FALSE;
}

CTreeCtrlEx::~CTreeCtrlEx(void)
{
}

BOOL CTreeCtrlEx::Create(DWORD dwStyle, DWORD dwExStyle, const RECT& rect, CWnd* pParentWnd, UINT nID)   
{    
	return CTreeCtrl::CreateEx( dwExStyle, dwStyle, rect, pParentWnd, nID );     
}   

BOOL CTreeCtrlEx::Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID)   
{   
	return CTreeCtrl::Create(dwStyle, rect, pParentWnd, nID);   
}

void CTreeCtrlEx::DeleteChildren(HTREEITEM hItem)
{
	if(!hItem)
		return;
	HTREEITEM hChildItem = GetChildItem(hItem);
	while (hChildItem)
	{		
		HTREEITEM hNextItem = GetNextItem(hChildItem, TVGN_NEXT);
		DeleteItem(hChildItem);
		hChildItem = hNextItem;
	}
}

bool CTreeCtrlEx::IsChildItem(HTREEITEM hItem, HTREEITEM hUnknownItem)
{
	if(!hUnknownItem || !hItem)
		return false;
	HTREEITEM hChildItem = GetChildItem(hItem);
	while (hChildItem)
	{
		if(hChildItem == hUnknownItem)
			return true;
		hChildItem = GetNextItem(hChildItem, TVGN_NEXT);
	}
	return false;
}

// void CTreeCtrlEx::OnLButtonDown(UINT nFlags, CPoint point)
// {
// 	if(!m_bMultiSelect)
// 	{
// 		clearSelection();
// 		CTreeCtrl::OnLButtonDown(nFlags, point);
// 		m_arrSelectItem.push_back(GetSelectedItem());
// 		return;
// 	}
// 	unsigned short keyState = GetKeyState(VK_CONTROL);
// 	keyState >>= 15;
// 	if(keyState != 1)
// 	{
// 		clearSelection();
// 		CTreeCtrl::OnLButtonDown(nFlags, point);
// 		m_arrSelectItem.push_back(GetSelectedItem());
// 		return;
// 	}
// 	CPoint pt;
// 	GetCursorPos(&pt);
// 	ScreenToClient(&pt);
// 	CRect rcItem;
// 	HTREEITEM hItem = GetRootItem();
// 	while(hItem)
// 	{
// 		GetItemRect(hItem, &rcItem, FALSE);
// 		if(rcItem.PtInRect(pt))
// 			break;
// 		hItem = GetNextVisibleItem(hItem);
// 	}
// 	if(!hItem)
// 		return;
// 	if((GetItemState(hItem, TVIS_SELECTED) & TVIS_SELECTED) != 0)
// 	{
// 		SetItemState(hItem, 0, TVIS_SELECTED);
// 		for (size_t i=0; i<m_arrSelectItem.size(); ++i)
// 		{
// 			if(hItem == m_arrSelectItem[i])
// 			{
// 				m_arrSelectItem.erase(m_arrSelectItem.begin() + i);
// 				break;
// 			}
// 		}
// 	}
// 	else
// 	{
// 		SetItemState(hItem, TVIS_SELECTED, TVIS_SELECTED);
// 		m_arrSelectItem.push_back(hItem);
// 	}
// }
// 
// void CTreeCtrlEx::clearSelection()
// {
// 	for (size_t i=0; i<m_arrSelectItem.size(); ++i)
// 	{
// 		SetItemState(m_arrSelectItem[i], 0, TVIS_SELECTED);
// 	}
// 	m_arrSelectItem.clear();
// }


void CTreeCtrlEx::OnLButtonDown( UINT nFlags, CPoint point )   
{
 	if(!m_bMultiSelect)
 	{
 		ClearSelection();  
 		CTreeCtrl::OnLButtonDown(nFlags, point);
		m_arrSelectItem.push_back(GetSelectedItem());
 		return;
 	}

	UINT nHitFlags = 0;   
	HTREEITEM hClickedItem = HitTest( point, &nHitFlags );   

	// Must invoke label editing explicitly. The base class OnLButtonDown would normally    
	// do this, but we can't call it here because of the multiple selection...    
	if( !( nFlags&( MK_CONTROL|MK_SHIFT ) ) && ( GetStyle() & TVS_EDITLABELS ) && ( nHitFlags & TVHT_ONITEMLABEL ) )   
		if ( hClickedItem == GetSelectedItem() )   
		{   
			// Clear multple selection before label editing    
			ClearSelection();   
			SelectItem( hClickedItem );   

			// Invoke label editing    
			m_bEditLabelPending = TRUE;   
			m_idTimer = (UINT)SetTimer(TCEX_EDITLABEL, GetDoubleClickTime(), NULL);   

			return;   
		}   

		m_bEditLabelPending = FALSE;   

		if( nHitFlags & TVHT_ONITEM )   
		{   
			SetFocus();   

			m_hClickedItem = hClickedItem;   

			// Is the clicked item already selected ?    
			BOOL bIsClickedItemSelected = GetItemState( hClickedItem, TVIS_SELECTED ) & TVIS_SELECTED;   

			if ( bIsClickedItemSelected )   
			{   
				// Maybe user wants to drag/drop multiple items!    
				// So, wait until OnLButtonUp() to do the selection stuff.     
				m_bSelectPending=TRUE;   
			}   
			else   
			{   
				SelectMultiple( hClickedItem, nFlags, point );   
				m_bSelectPending=FALSE;   
			}   

			m_ptClick=point;   
		}   
		else   
			CTreeCtrl::OnLButtonDown( nFlags, point );
}   

void CTreeCtrlEx::OnLButtonUp( UINT nFlags, CPoint point )   
{   
	if ( m_bSelectPending )   
	{   
		// A select has been waiting to be performed here    
		SelectMultiple( m_hClickedItem, nFlags, point );   
		m_bSelectPending=FALSE;   
	}   

	m_hClickedItem=NULL;   

	CTreeCtrl::OnLButtonUp( nFlags, point );   
}   


void CTreeCtrlEx::OnMouseMove( UINT nFlags, CPoint point )   
{   
	// If there is a select pending, check if cursor has moved so much away from the     
	// down-click point that we should cancel the pending select and initiate    
	// a drag/drop operation instead!    

	if ( m_hClickedItem )   
	{   
		CSize sizeMoved = m_ptClick-point;   

		if ( abs(sizeMoved.cx) > GetSystemMetrics( SM_CXDRAG ) || abs(sizeMoved.cy) > GetSystemMetrics( SM_CYDRAG ) )   
		{   
			m_bSelectPending=FALSE;   

			// Notify parent that he may begin drag operation    
			// Since we have taken over OnLButtonDown(), the default handler doesn't    
			// do the normal work when clicking an item, so we must provide our own    
			// TVN_BEGINDRAG notification for the parent!    

			CWnd* pWnd = GetParent();   
			if ( pWnd && !( GetStyle() & TVS_DISABLEDRAGDROP ) )   
			{   
				NM_TREEVIEW tv;   

				tv.hdr.hwndFrom = GetSafeHwnd();   
				tv.hdr.idFrom = GetWindowLong( GetSafeHwnd(), GWL_ID );   
				tv.hdr.code = TVN_BEGINDRAG;   

				tv.itemNew.hItem = m_hClickedItem;   
				tv.itemNew.state = GetItemState( m_hClickedItem, 0xffffffff );   
				tv.itemNew.lParam = GetItemData( m_hClickedItem );   

				tv.ptDrag.x = point.x;   
				tv.ptDrag.y = point.y;   

				pWnd->SendMessage( WM_NOTIFY, tv.hdr.idFrom, (LPARAM)&tv );   
			}   

			m_hClickedItem=NULL;   
		}   
	}   

	CTreeCtrl::OnMouseMove( nFlags, point );   
}   


void CTreeCtrlEx::SelectMultiple( HTREEITEM hClickedItem, UINT nFlags, CPoint point )   
{   
	// Start preparing an NM_TREEVIEW struct to send a notification after selection is done    
	NM_TREEVIEW tv;   
	memset(&tv.itemOld, 0, sizeof(tv.itemOld));   

	CWnd* pWnd = GetParent();   

	HTREEITEM hOldItem = GetSelectedItem();   

	if ( hOldItem )   
	{   
		tv.itemOld.hItem = hOldItem;   
		tv.itemOld.state = GetItemState( hOldItem, 0xffffffff );   
		tv.itemOld.lParam = GetItemData( hOldItem );   
		tv.itemOld.mask = TVIF_HANDLE|TVIF_STATE|TVIF_PARAM;   
	}   

	// Flag signaling that selection process is NOT complete.    
	// (Will prohibit TVN_SELCHANGED from being sent to parent)    
	m_bSelectionComplete = FALSE;   

	// Action depends on whether the user holds down the Shift or Ctrl key    
	if ( nFlags & MK_SHIFT )   
	{   
		// Select from first selected item to the clicked item    
		if ( !m_hFirstSelectedItem )   
			m_hFirstSelectedItem = GetSelectedItem();   

		SelectItems( m_hFirstSelectedItem, hClickedItem );   
	}   
	else if ( nFlags & MK_CONTROL )   
	{   
		// Find which item is currently selected    
		HTREEITEM hSelectedItem = GetSelectedItem();   

		// Is the clicked item already selected ?    
		BOOL bIsClickedItemSelected = GetItemState( hClickedItem, TVIS_SELECTED ) & TVIS_SELECTED;   
		BOOL bIsSelectedItemSelected = FALSE;   
		if ( hSelectedItem )   
			bIsSelectedItemSelected = GetItemState( hSelectedItem, TVIS_SELECTED ) & TVIS_SELECTED;   

		// Must synthesize a TVN_SELCHANGING notification    
		if ( pWnd )   
		{   
			tv.hdr.hwndFrom = GetSafeHwnd();   
			tv.hdr.idFrom = GetWindowLong( GetSafeHwnd(), GWL_ID );   
			tv.hdr.code = TVN_SELCHANGING;   

			tv.itemNew.hItem = hClickedItem;   
			tv.itemNew.state = GetItemState( hClickedItem, 0xffffffff );   
			tv.itemNew.lParam = GetItemData( hClickedItem );   

			tv.itemOld.hItem = NULL;   
			tv.itemOld.mask = 0;   

			tv.action = TVC_BYMOUSE;   

			tv.ptDrag.x = point.x;   
			tv.ptDrag.y = point.y;   

			pWnd->SendMessage( WM_NOTIFY, tv.hdr.idFrom, (LPARAM)&tv );   
		}   

		// If the previously selected item was selected, re-select it    
		if ( bIsSelectedItemSelected )   
			SetItemState( hSelectedItem, TVIS_SELECTED, TVIS_SELECTED );   

		// We want the newly selected item to toggle its selected state,    
		// so unselect now if it was already selected before    
		if ( bIsClickedItemSelected )   
			SetItemState( hClickedItem, 0, TVIS_SELECTED );   
		else   
		{   
			SelectItem(hClickedItem);   
			SetItemState( hClickedItem, TVIS_SELECTED, TVIS_SELECTED );   
		}   

		// If the previously selected item was selected, re-select it    
		if ( bIsSelectedItemSelected && hSelectedItem != hClickedItem )   
			SetItemState( hSelectedItem, TVIS_SELECTED, TVIS_SELECTED );   

		// Store as first selected item (if not already stored)    
		if ( m_hFirstSelectedItem==NULL )   
			m_hFirstSelectedItem = hClickedItem;   
	}   
	else   
	{   
		// Clear selection of all "multiple selected" items first    
		ClearSelection();   

		// Then select the clicked item    
		SelectItem( hClickedItem );   
		SetItemState( hClickedItem, TVIS_SELECTED, TVIS_SELECTED );   

		// Store as first selected item    
		m_hFirstSelectedItem = hClickedItem;   
	}   

	// Selection process is now complete. Since we have 'eaten' the TVN_SELCHANGED     
	// notification provided by Windows' treectrl, we must now produce one ourselves,    
	// so that our parent gets to know about the change of selection.    
	m_bSelectionComplete = TRUE;   

	if ( pWnd )   
	{   
		tv.hdr.hwndFrom = GetSafeHwnd();   
		tv.hdr.idFrom = GetWindowLong( GetSafeHwnd(), GWL_ID );   
		tv.hdr.code = TVN_SELCHANGED;   

		tv.itemNew.hItem = m_hClickedItem;   
		tv.itemNew.state = GetItemState( m_hClickedItem, 0xffffffff );   
		tv.itemNew.lParam = GetItemData( m_hClickedItem );   
		tv.itemNew.mask = TVIF_HANDLE|TVIF_STATE|TVIF_PARAM;   

		tv.action = TVC_UNKNOWN;   

		pWnd->SendMessage( WM_NOTIFY, tv.hdr.idFrom, (LPARAM)&tv );   
	}
	m_arrSelectItem.clear();
	int cnt = GetSelectedCount();
	if(cnt == 0)
		return;
	HTREEITEM hItem = GetFirstSelectedItem();
	m_arrSelectItem.push_back(hItem);
	hItem = GetNextVisibleItem(hItem);
	while(hItem)
	{
		if((GetItemState( hItem, TVIS_SELECTED ) & TVIS_SELECTED ))
			m_arrSelectItem.push_back(hItem);
		hItem = GetNextVisibleItem(hItem);
	}
}

void CTreeCtrlEx::OnKeyDown( UINT nChar, UINT nRepCnt, UINT nFlags )    
{   
	CWnd* pWnd = GetParent();   

	if ( nChar==VK_NEXT || nChar==VK_PRIOR )   
	{   
		if ( !( GetKeyState( VK_SHIFT )&0x8000 ) )   
		{   
			// User pressed Pg key without holding 'Shift':    
			// Clear multiple selection (if multiple) and let base class do     
			// normal selection work!    
			if ( GetSelectedCount()>1 )   
				ClearSelection( TRUE );   

			CTreeCtrl::OnKeyDown( nChar, nRepCnt, nFlags );   
			m_hFirstSelectedItem = GetSelectedItem();   
			return;   
		}   

		// Flag signaling that selection process is NOT complete.    
		// (Will prohibit TVN_SELCHANGED from being sent to parent)    
		m_bSelectionComplete = FALSE;   

		// Let base class select the item    
		CTreeCtrl::OnKeyDown( nChar, nRepCnt, nFlags );   
		HTREEITEM hSelectedItem = GetSelectedItem();   

		// Then select items in between    
		SelectItems( m_hFirstSelectedItem, hSelectedItem );   

		// Selection process is now complete. Since we have 'eaten' the TVN_SELCHANGED     
		// notification provided by Windows' treectrl, we must now produce one ourselves,    
		// so that our parent gets to know about the change of selection.    
		m_bSelectionComplete = TRUE;   

		if (pWnd)   
		{   
			NM_TREEVIEW tv;   
			memset(&tv.itemOld, 0, sizeof(tv.itemOld));   

			tv.hdr.hwndFrom = GetSafeHwnd();   
			tv.hdr.idFrom = GetWindowLong(GetSafeHwnd(), GWL_ID);   
			tv.hdr.code = TVN_SELCHANGED;   

			tv.itemNew.hItem = hSelectedItem;   
			tv.itemNew.state = GetItemState(hSelectedItem, 0xffffffff);   
			tv.itemNew.lParam = GetItemData(hSelectedItem);   
			tv.itemNew.mask = TVIF_HANDLE|TVIF_STATE|TVIF_PARAM;   

			tv.action = TVC_UNKNOWN;   

			pWnd->SendMessage(WM_NOTIFY, tv.hdr.idFrom, (LPARAM)&tv);   
		}   
	}   
	else if ( nChar==VK_UP || nChar==VK_DOWN )   
	{   
		// Find which item is currently selected    
		HTREEITEM hSelectedItem = GetSelectedItem();   

		HTREEITEM hNextItem;   
		if ( nChar==VK_UP )   
			hNextItem = GetPrevVisibleItem( hSelectedItem );   
		else   
			hNextItem = GetNextVisibleItem( hSelectedItem );   

		if ( !( GetKeyState( VK_SHIFT )&0x8000 ) )   
		{   
			// User pressed arrow key without holding 'Shift':    
			// Clear multiple selection (if multiple) and let base class do     
			// normal selection work!    
			if ( GetSelectedCount()>1 )   
				ClearSelection( TRUE );   

			if ( hNextItem )   
				CTreeCtrl::OnKeyDown( nChar, nRepCnt, nFlags );   
			m_hFirstSelectedItem = GetSelectedItem();   
			return;   
		}   

		if ( hNextItem )   
		{   
			// Flag signaling that selection process is NOT complete.    
			// (Will prohibit TVN_SELCHANGED from being sent to parent)    
			m_bSelectionComplete = FALSE;   

			// If the next item is already selected, we assume user is    
			// "moving back" in the selection, and thus we should clear     
			// selection on the previous one    
			BOOL bSelect = !( GetItemState( hNextItem, TVIS_SELECTED ) & TVIS_SELECTED );   

			// Select the next item (this will also deselect the previous one!)    
			SelectItem( hNextItem );   

			// Now, re-select the previously selected item    
			if ( bSelect || ( !( GetItemState( hSelectedItem, TVIS_SELECTED ) & TVIS_SELECTED ) ) )   
				SelectItems( m_hFirstSelectedItem, hNextItem );   

			// Selection process is now complete. Since we have 'eaten' the TVN_SELCHANGED     
			// notification provided by Windows' treectrl, we must now produce one ourselves,    
			// so that our parent gets to know about the change of selection.    
			m_bSelectionComplete = TRUE;   

			if (pWnd)   
			{   
				NM_TREEVIEW tv;   
				memset(&tv.itemOld, 0, sizeof(tv.itemOld));   

				tv.hdr.hwndFrom = GetSafeHwnd();   
				tv.hdr.idFrom = GetWindowLong(GetSafeHwnd(), GWL_ID);   
				tv.hdr.code = TVN_SELCHANGED;   

				tv.itemNew.hItem = hNextItem;   
				tv.itemNew.state = GetItemState(hNextItem, 0xffffffff);   
				tv.itemNew.lParam = GetItemData(hNextItem);   
				tv.itemNew.mask = TVIF_HANDLE|TVIF_STATE|TVIF_PARAM;   

				tv.action = TVC_UNKNOWN;   

				pWnd->SendMessage(WM_NOTIFY, tv.hdr.idFrom, (LPARAM)&tv);   
			}   
		}   

		// Since the base class' OnKeyDown() isn't called in this case,    
		// we must provide our own TVN_KEYDOWN notification to the parent    

		CWnd* pWnd = GetParent();   
		if ( pWnd )   
		{   
			NMTVKEYDOWN tvk;   

			tvk.hdr.hwndFrom = GetSafeHwnd();   
			tvk.hdr.idFrom = GetWindowLong( GetSafeHwnd(), GWL_ID );   
			tvk.hdr.code = TVN_KEYDOWN;   

			tvk.wVKey = nChar;   
			tvk.flags = 0;   

			pWnd->SendMessage( WM_NOTIFY, tvk.hdr.idFrom, (LPARAM)&tvk );   
		}   
	}   
	else   
		// Behave normally    
		CTreeCtrl::OnKeyDown( nChar, nRepCnt, nFlags );   
}   


///////////////////////////////////////////////////////////////////////////////    
// I want clicking on an item with the right mouse button to select the item,    
// but not if there is currently a multiple selection    

void CTreeCtrlEx::OnRButtonDown( UINT nFlags, CPoint point )   
{   
	UINT nHitFlags = 0;   
	HTREEITEM hClickedItem = HitTest( point, &nHitFlags );   

	if( nHitFlags&TVHT_ONITEM )   
		if ( GetSelectedCount()<2 )   
			SelectItem( hClickedItem );   

	CTreeCtrl::OnRButtonDown( nFlags, point );   
}   


///////////////////////////////////////////////////////////////////////////////    
// Get number of selected items    

UINT CTreeCtrlEx::GetSelectedCount() const   
{   
	// Only visible items should be selected!    
	UINT uCount=0;   
	for ( HTREEITEM hItem = GetRootItem(); hItem!=NULL; hItem = GetNextVisibleItem( hItem ) )   
		if ( GetItemState( hItem, TVIS_SELECTED ) & TVIS_SELECTED )   
			uCount++;   

	return uCount;   
}   


///////////////////////////////////////////////////////////////////////////////    
// Overloaded to catch our own special code    

HTREEITEM CTreeCtrlEx::GetNextItem(HTREEITEM hItem, UINT nCode)   
{   
	if (nCode==TVGN_EX_ALL)   
	{   
		// This special code lets us iterate through ALL tree items regardless     
		// of their parent/child relationship (very handy)    
		HTREEITEM hNextItem;   

		// If it has a child node, this will be the next item    
		hNextItem = GetChildItem( hItem );   
		if (hNextItem)   
			return hNextItem;   

		// Otherwise, see if it has a next sibling item    
		hNextItem = GetNextSiblingItem(hItem);   
		if (hNextItem)   
			return hNextItem;   

		// Finally, look for next sibling to the parent item    
		HTREEITEM hParentItem=hItem;   
		while (!hNextItem && hParentItem)   
		{   
			// No more children: Get next sibling to parent    
			hParentItem = GetParentItem(hParentItem);   
			hNextItem = GetNextSiblingItem(hParentItem);   
		}   

		return hNextItem; // will return NULL if no more parents    
	}   
	else   
		return CTreeCtrl::GetNextItem(hItem, nCode);    // standard processing    
}   

///////////////////////////////////////////////////////////////////////////////    
// Helpers to list out selected items. (Use similar to GetFirstVisibleItem(),     
// GetNextVisibleItem() and GetPrevVisibleItem()!)    

HTREEITEM CTreeCtrlEx::GetFirstSelectedItem()   
{   
	for ( HTREEITEM hItem = GetRootItem(); hItem!=NULL; hItem = GetNextVisibleItem( hItem ) )   
		if ( GetItemState( hItem, TVIS_SELECTED ) & TVIS_SELECTED )   
			return hItem;   

	return NULL;   
}   

HTREEITEM CTreeCtrlEx::GetNextSelectedItem( HTREEITEM hItem )   
{   
	for ( hItem = GetNextVisibleItem( hItem ); hItem!=NULL; hItem = GetNextVisibleItem( hItem ) )   
		if ( GetItemState( hItem, TVIS_SELECTED ) & TVIS_SELECTED )   
			return hItem;   

	return NULL;   
}   

HTREEITEM CTreeCtrlEx::GetPrevSelectedItem( HTREEITEM hItem )   
{   
	for ( hItem = GetPrevVisibleItem( hItem ); hItem!=NULL; hItem = GetPrevVisibleItem( hItem ) )   
		if ( GetItemState( hItem, TVIS_SELECTED ) & TVIS_SELECTED )   
			return hItem;   

	return NULL;   
}   


///////////////////////////////////////////////////////////////////////////////    
// Select/unselect item without unselecting other items    

BOOL CTreeCtrlEx::SelectItemEx(HTREEITEM hItem, BOOL bSelect/*=TRUE*/)   
{   
	HTREEITEM hSelItem = GetSelectedItem();   

	if ( hItem==hSelItem )   
	{   
		if ( !bSelect )   
		{   
			SelectItem( NULL );   
			return TRUE;   
		}   

		return FALSE;   
	}   

	SelectItem( hItem );   
	m_hFirstSelectedItem=hItem;   

	// Reselect previous "real" selected item which was unselected byt SelectItem()    
	if ( hSelItem )   
		SetItemState( hSelItem, TVIS_SELECTED, TVIS_SELECTED );   

	return TRUE;   
}   

///////////////////////////////////////////////////////////////////////////////    
// Select visible items between specified 'from' and 'to' item (including these!)    
// If the 'to' item is above the 'from' item, it traverses the tree in reverse     
// direction. Selection on other items is cleared!    

BOOL CTreeCtrlEx::SelectItems( HTREEITEM hFromItem, HTREEITEM hToItem )   
{   
	// Determine direction of selection     
	// (see what item comes first in the tree)    
	HTREEITEM hItem = GetRootItem();   

	while ( hItem && hItem!=hFromItem && hItem!=hToItem )   
		hItem = GetNextVisibleItem( hItem );   

	if ( !hItem )   
		return FALSE;   // Items not visible in tree    

	BOOL bReverse = hItem==hToItem;   

	// "Really" select the 'to' item (which will deselect     
	// the previously selected item)    

	SelectItem( hToItem );   

	// Go through all visible items again and select/unselect    

	hItem = GetRootItem();   
	BOOL bSelect = FALSE;   

	while ( hItem )   
	{   
		if ( hItem == ( bReverse ? hToItem : hFromItem ) )   
			bSelect = TRUE;   

		if ( bSelect )   
		{   
			if ( !( GetItemState( hItem, TVIS_SELECTED ) & TVIS_SELECTED ) )   
				SetItemState( hItem, TVIS_SELECTED, TVIS_SELECTED );   
		}   
		else   
		{   
			if ( GetItemState( hItem, TVIS_SELECTED ) & TVIS_SELECTED )   
				SetItemState( hItem, 0, TVIS_SELECTED );   
		}   

		if ( hItem == ( bReverse ? hFromItem : hToItem ) )   
			bSelect = FALSE;   

		hItem = GetNextVisibleItem( hItem );   
	}   

	return TRUE;   
}   


///////////////////////////////////////////////////////////////////////////////    
// Clear selected state on all visible items    

void CTreeCtrlEx::ClearSelection(BOOL bMultiOnly/*=FALSE*/)   
{   
	//  if ( !bMultiOnly )    
	//      SelectItem( NULL );    

	for ( HTREEITEM hItem=GetRootItem(); hItem!=NULL; hItem=GetNextVisibleItem( hItem ) )
	{
		if ( GetItemState( hItem, TVIS_SELECTED ) & TVIS_SELECTED )   
			SetItemState( hItem, 0, TVIS_SELECTED );
		if ( GetItemState( hItem, TVIS_DROPHILITED ) & TVIS_DROPHILITED )   
			SetItemState( hItem, 0, TVIS_DROPHILITED );
	}
	m_arrSelectItem.clear();
}   


///////////////////////////////////////////////////////////////////////////////    
// If a node is collapsed, we should clear selections of its child items     

BOOL CTreeCtrlEx::OnItemexpanding(NMHDR* pNMHDR, LRESULT* pResult)    
{   
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;   

	if ( pNMTreeView->action == TVE_COLLAPSE )   
	{   
		HTREEITEM hItem = GetChildItem( pNMTreeView->itemNew.hItem );   

		while ( hItem )   
		{   
			if ( GetItemState( hItem, TVIS_SELECTED ) & TVIS_SELECTED )   
				SetItemState( hItem, 0, TVIS_SELECTED );   

			// Get the next node: First see if current node has a child    
			HTREEITEM hNextItem = GetChildItem( hItem );   
			if ( !hNextItem )   
			{   
				// No child: Get next sibling item    
				if ( !( hNextItem = GetNextSiblingItem( hItem ) ) )   
				{   
					HTREEITEM hParentItem = hItem;   
					while ( !hNextItem )   
					{   
						// No more children: Get parent    
						if ( !( hParentItem = GetParentItem( hParentItem ) ) )   
							break;   

						// Quit when parent is the collapsed node    
						// (Don't do anything to siblings of this)    
						if ( hParentItem == pNMTreeView->itemNew.hItem )   
							break;   

						// Get next sibling to parent    
						hNextItem = GetNextSiblingItem( hParentItem );   
					}   

					// Quit when parent is the collapsed node    
					if ( hParentItem == pNMTreeView->itemNew.hItem )   
						break;   
				}   
			}   

			hItem = hNextItem;   
		}   
	}   

	*pResult = 0;   
	return FALSE;   // Allow parent to handle this notification as well    
}   


///////////////////////////////////////////////////////////////////////////////    
// Intercept TVN_SELCHANGED and pass it only to the parent window of the    
// selection process is finished    

BOOL CTreeCtrlEx::OnSelchanged(NMHDR* pNMHDR, LRESULT* pResult)   
{   
	// Return TRUE if selection is not complete. This will prevent the     
	// notification from being sent to parent.    
	return !m_bSelectionComplete;      
}   


///////////////////////////////////////////////////////////////////////////////    
// Ensure the multiple selected items are drawn correctly when loosing/getting    
// the focus    

BOOL CTreeCtrlEx::OnSetfocus(NMHDR* pNMHDR, LRESULT* pResult)    
{   
	Invalidate();   
	*pResult = 0;   
	return FALSE;   
}   

BOOL CTreeCtrlEx::OnKillfocus(NMHDR* pNMHDR, LRESULT* pResult)    
{   
	Invalidate();   
	*pResult = 0;   
	return FALSE;   
}   

void CTreeCtrlEx::OnLButtonDblClk(UINT nFlags, CPoint point)   
{   
	// We stop label editing.    
	m_bEditLabelPending = FALSE;   
	CTreeCtrl::OnLButtonDblClk(nFlags, point);   
}   

void CTreeCtrlEx::OnTimer(UINT_PTR nIDEvent)   
{   
	if (nIDEvent == TCEX_EDITLABEL)   
	{   
		// Stop the timer.    
		KillTimer(m_idTimer);   

		// Invoke label editing.    
		if (m_bEditLabelPending)   
			EditLabel(GetSelectedItem());   

		m_bEditLabelPending = FALSE;   
		return;   
	}   

	CTreeCtrl::OnTimer(nIDEvent);   
}   

///////////////////////////////////////////////////////////////////////////////    
// Retreives a tree ctrl item given the item's data    

HTREEITEM CTreeCtrlEx::ItemFromData(DWORD dwData, HTREEITEM hStartAtItem/*=NULL*/) const   
{   
	// Traverse all items in tree control    
	HTREEITEM hItem;   
	if ( hStartAtItem )   
		hItem = hStartAtItem;   
	else   
		hItem = GetRootItem();   

	while ( hItem )   
	{   
		if ( dwData == (DWORD)GetItemData( hItem ) )   
			return hItem;   

		// Get first child node    
		HTREEITEM hNextItem = GetChildItem( hItem );   

		if ( !hNextItem )   
		{   
			// Get next sibling child    
			hNextItem = GetNextSiblingItem( hItem );   

			if ( !hNextItem )   
			{   
				HTREEITEM hParentItem=hItem;   
				while ( !hNextItem && hParentItem )   
				{   
					// No more children: Get next sibling to parent    
					hParentItem = GetParentItem( hParentItem );   
					hNextItem = GetNextSiblingItem( hParentItem );   
				}   
			}   
		}   

		hItem = hNextItem;   
	}   

	return NULL;   
}   


/////////////////////////////////////////////////////////////////////////////    
// Global function to retreive a HTREEITEM from a tree control, given the     
// item's itemdata.    

HTREEITEM GetTreeItemFromData(CTreeCtrl& treeCtrl, DWORD dwData, HTREEITEM hStartAtItem /*=NULL*/)   
{   
	// Traverse from given item (or all items if hFromItem is NULL)    
	HTREEITEM hItem;   
	if ( hStartAtItem )   
		hItem=hStartAtItem;   
	else   
		hItem = treeCtrl.GetRootItem();   

	while ( hItem )   
	{   
		if ( dwData == (DWORD)treeCtrl.GetItemData( hItem ) )   
			return hItem;   

		// Get first child node    
		HTREEITEM hNextItem = treeCtrl.GetChildItem( hItem );   

		if ( !hNextItem )   
		{   
			// Get next sibling child    
			hNextItem = treeCtrl.GetNextSiblingItem( hItem );   

			if ( !hNextItem )   
			{   
				HTREEITEM hParentItem=hItem;   
				while ( !hNextItem && hParentItem )   
				{   
					// No more children: Get next sibling to parent    
					hParentItem = treeCtrl.GetParentItem( hParentItem );   
					hNextItem = treeCtrl.GetNextSiblingItem( hParentItem );   
				}   
			}   
		}   
		hItem = hNextItem;   
	}   
	return NULL;   
}   