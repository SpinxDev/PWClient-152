// ListDlgBar.cpp : implementation file
//

#include "stdafx.h"
#include "ElementData.h"
#include "ListDlgBar.h"



#define new A_DEBUG_NEW

/////////////////////////////////////////////////////////////////////////////
// CListDlgBar dialog


CListDlgBar::CListDlgBar(CWnd* pParent /*=NULL*/)
{
	//{{AFX_DATA_INIT(CListDlgBar)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CListDlgBar::DoDataExchange(CDataExchange* pDX)
{
	CDialogBar::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CListDlgBar)
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CListDlgBar, CDialogBar)
	//{{AFX_MSG_MAP(CListDlgBar)
	ON_WM_SIZE()
	ON_WM_CREATE()
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CListDlgBar message handlers

void CListDlgBar::OnSize(UINT nType, int cx, int cy) 
{
	CDialogBar::OnSize(nType, cx, cy);
	
	// TODO: Add your message handler code here
	
	CRect rect;
	GetClientRect(&rect);
	
	m_ListDlg.SetWindowPos(NULL,2,10,rect.Width()-2,rect.Height()-12,0);	
	
}

int CListDlgBar::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CDialogBar::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// TODO: Add your specialized creation code here
	m_ListDlg.Create(IDD_DIALOG_LIST,this); 
	m_ListDlg.ShowWindow(SW_SHOW); 
	return 0;
}

/*
void CListDlgBar::CreateBaseTreeList()
{

	CTreeCtrl *pTree = (CTreeCtrl*)GetDlgItem(IDC_TREE_ITEM);
	if(pTree)
	{
		pTree->DeleteAllItems();
		pTree->SetImageList(&m_ImageList,TVSIL_NORMAL);
		for(int i = 0; i<SORT1_NUM; i++)
		{
			m_tvSort[i].hParent = NULL;
			m_tvSort[i].hInsertAfter = NULL;
			m_tvSort[i].item.pszText = m_strSort1Name[i].GetBuffer(0);
			m_strSort1Name[i].ReleaseBuffer();
			HTREEITEM hItem = pTree->InsertItem(&m_tvSort[i]);
			pTree->SetItemImage(hItem,0,1);
		}
	}
}
*/
BOOL CListDlgBar::DestroyWindow() 
{
	// TODO: Add your specialized code here and/or call the base class
	
	return CDialogBar::DestroyWindow();
}

void CListDlgBar::OnFinalRelease() 
{
	// TODO: Add your specialized code here and/or call the base class
	
	CDialogBar::OnFinalRelease();
}

void CListDlgBar::OnDestroy() 
{
	CDialogBar::OnDestroy();
	
	// TODO: Add your message handler code here
}
