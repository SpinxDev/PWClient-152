// EditList.cpp : implementation file
//

#include "stdafx.h"
#include "EditList.h"
#include "ComboItem.h"
#include "EditItem.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CEditList

CEditList::CEditList()
{
}

CEditList::~CEditList()
{
}

CEdit *CEditList::EditItem(int nItem, int nSubItem)
{
	CRect rect;
	int offset = 0;
	if(!EnsureVisible(nItem, TRUE))
	{ 
		return NULL;	
	}

	GetSubItemRect(nItem, nSubItem, LVIR_BOUNDS, rect);
	// Now scroll if we need to expose the column
	CRect rcClient;
	GetClientRect(rcClient);
	if( offset + rect.left < 0 || offset + rect.left > rcClient.right )
	{
		CSize size(offset + rect.left,0);		
		Scroll(size);
		rect.left -= size.cx;
	}
	rect.left += offset;	
	rect.right = rect.left + GetColumnWidth(nSubItem);
	if(rect.right > rcClient.right) 
	   rect.right = rcClient.right;

	// Get Column alignment	
	LV_COLUMN lvcol;
	lvcol.mask = LVCF_FMT;
	GetColumn(nSubItem, &lvcol);

	DWORD dwStyle;
	if((lvcol.fmt & LVCFMT_JUSTIFYMASK) == LVCFMT_LEFT)
		dwStyle = ES_LEFT;
	else if((lvcol.fmt & LVCFMT_JUSTIFYMASK) == LVCFMT_RIGHT)
		dwStyle = ES_RIGHT;
	else 
		dwStyle = ES_CENTER;	

	
	dwStyle |=WS_BORDER|WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL;
	CEdit *pEdit = new CEditItem(nItem, nSubItem, GetItemText(nItem, nSubItem));

#define IDC_EDITCTRL 0x1234
	pEdit->Create(dwStyle, rect, this, IDC_EDITCTRL);	
	//	pEdit->ModifyStyleEx(0,WS_EX_CLIENTEDGE);
	//funny thing happend here, uncomment this, 
	//and then edit an item, 
	//enter a long text so that the ES_AUTOHSCROLL comes to rescue
	//yes that's look funny, ???.
	return pEdit;
}


CComboBox * CEditList::ComboItem(int nItem, int nSubItem, CStringList& m_strList)
{
#define IDC_COMBOBOXINLISTVIEW 0x1235

	CString strFind = GetItemText(nItem, nSubItem);

	//basic code start
	CRect rect;
	int offset = 0;
	// Make sure that the item is visible
	if( !EnsureVisible(nItem, TRUE)) 
		return NULL;

	GetSubItemRect(nItem, nSubItem, LVIR_BOUNDS, rect);
	// Now scroll if we need to expose the column
	CRect rcClient;
	GetClientRect(rcClient);
	if( offset + rect.left < 0 || offset + rect.left > rcClient.right )
	{
		CSize size;
		size.cx = offset + rect.left;
		size.cy = 0;
		Scroll(size);
		rect.left -= size.cx;
	}
	
	rect.left += offset;	
	rect.right = rect.left + GetColumnWidth(nSubItem);
	if(rect.right > rcClient.right) 
	   rect.right = rcClient.right;
	//basic code end

	rect.bottom += 30 * rect.Height();//dropdown area
	
	DWORD dwStyle =  WS_CHILD | WS_VISIBLE |CBS_DROPDOWNLIST | CBS_DISABLENOSCROLL;
	CComboBox *pList = new CComboItem(nItem, nSubItem, &m_strList);
	pList->Create(dwStyle, rect, this, IDC_COMBOBOXINLISTVIEW);
	pList->ModifyStyleEx(0,WS_EX_CLIENTEDGE);//can we tell at all
	
	pList->ShowDropDown();
	pList->SelectString(-1, strFind.GetBuffer(1));
	// The returned pointer should not be saved
	return pList;
}




BEGIN_MESSAGE_MAP(CEditList, CListCtrl)
	//{{AFX_MSG_MAP(CEditList)
	ON_WM_CREATE()
	ON_NOTIFY_REFLECT(LVN_ENDLABELEDIT, OnEndlabeledit)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEditList message handlers

int CEditList::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CListCtrl::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// TODO: Add your specialized creation code here
	SetExtendedStyle( GetExtendedStyle() | LVS_EX_CHECKBOXES );
	
	return 0;
}

void CEditList::OnEndlabeledit(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	LV_DISPINFO *plvDispInfo = (LV_DISPINFO*)pNMHDR;
 	LV_ITEM *plvItem = &plvDispInfo->item;

	if( plvItem->iItem != -1 &&  // valid item
		plvItem->pszText )		// valid text
	{
		SetItemText( plvItem->iItem, plvItem->iSubItem, plvItem->pszText);				
		
		// this will invoke an ItemChanged handler in parent
		if ( plvItem->iSubItem != 0 )
		{
			char szText[0x10+1];
			GetItemText( plvItem->iItem, 0, szText, 0x10);				
			SetItemText( plvItem->iItem, 0, szText);				
		}	
	}
	
	*pResult = 0;
}

BOOL CEditList::PreCreateWindow(CREATESTRUCT& cs) 
{
	// TODO: Add your specialized code here and/or call the base class
		cs.style |= WS_CHILD |
				LVS_REPORT |
				LVS_SINGLESEL |
				LVS_SHOWSELALWAYS;	
	
	return CListCtrl::PreCreateWindow(cs);
}
