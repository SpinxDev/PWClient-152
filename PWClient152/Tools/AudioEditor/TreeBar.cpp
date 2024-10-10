#include "StdAfx.h"
#include "TreeBar.h"
#include "Resource.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define MENU_NEW_FOLDER	4001
#define MENU_NEW_FILE	4002
#define MENU_DELETE		4003

#define ITEM_FOLDER	0x1001
#define ITEM_FILE	0x1002

const int nBorderSize = 0;

CTreeBar::CTreeBar(void)
: m_pDragImage(0)
, m_bDragging(false)
{
}

CTreeBar::~CTreeBar(void)
{
	delete m_pDragImage;
	m_pDragImage = 0;
}

BEGIN_MESSAGE_MAP(CTreeBar, CBCGPDockingControlBar)
	//{{AFX_MSG_MAP(CTreeBar)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_PAINT()
	ON_WM_TIMER()

	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_CTRL, &CTreeBar::OnTreeSelectChanged)
	ON_NOTIFY(NM_RCLICK, IDC_TREE_CTRL, &CTreeBar::OnTreeRClick)
	ON_NOTIFY(TVN_ENDLABELEDIT, IDC_TREE_CTRL, &CTreeBar::OnTreeEndLabelEdit)
	ON_NOTIFY(TVN_BEGINDRAG, IDC_TREE_CTRL, &CTreeBar::OnTreeBeginDrag)
	ON_COMMAND(MENU_NEW_FOLDER, &CTreeBar::OnNewFolder)
	ON_COMMAND(MENU_NEW_FILE, &CTreeBar::OnNewFile)
	ON_COMMAND(MENU_DELETE, &CTreeBar::OnDelete)
	//}}AFX_MSG_MAP
	ON_WM_CONTEXTMENU()
END_MESSAGE_MAP()

int CTreeBar::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CBCGPDockingControlBar::OnCreate(lpCreateStruct) == -1)
		return -1;

	CRect rectDummy;
	rectDummy.SetRectEmpty ();

	// Create tree windows.
	// TODO: create your own tab windows here:
	const DWORD dwViewStyle =	WS_CHILD | WS_VISIBLE | TVS_HASLINES | 
		TVS_LINESATROOT | TVS_HASBUTTONS | TVS_SHOWSELALWAYS;

	if (!m_wndTree.Create (dwViewStyle, rectDummy, this, IDC_TREE_CTRL))
	{
		TRACE0("Failed to create workspace view\n");
		return -1;      // fail to create
	}

	if(!m_ImageList.Create(16, 16, ILC_COLOR32|ILC_MASK, 0, 6))
		return -1;
	m_wndTree.SetImageList(&m_ImageList, TVSIL_NORMAL);
	return 0;
}

void CTreeBar::OnSize(UINT nType, int cx, int cy) 
{
	CBCGPDockingControlBar::OnSize(nType, cx, cy);

	// Tab control should cover a whole client area:
	m_wndTree.SetWindowPos (NULL, nBorderSize, nBorderSize, 
		cx - 2 * nBorderSize, cy - 2 * nBorderSize,
		SWP_NOACTIVATE | SWP_NOZORDER);
}

void CTreeBar::OnPaint() 
{
	CPaintDC dc(this); // device context for painting

	CRect rectTree;
	m_wndTree.GetWindowRect (rectTree);
	ScreenToClient (rectTree);

	rectTree.InflateRect (nBorderSize, nBorderSize);
	dc.Draw3dRect (rectTree,	::GetSysColor (COLOR_3DSHADOW), 
		::GetSysColor (COLOR_3DSHADOW));
}

void CTreeBar::OnTreeSelectChanged(NMHDR* pNMHDR, LRESULT* pResult)
{
	HTREEITEM hItem = m_wndTree.GetSelectedItem();

	*pResult = 0;
}

void CTreeBar::OnTreeRClick(NMHDR* pNMHDR, LRESULT* pResult)
{
	CPoint pos;
	GetCursorPos(&pos);
	m_wndTree.ScreenToClient(&pos);
	HTREEITEM hItem = m_wndTree.HitTest(pos);
	if(!hItem)
		return;
	if(!m_wndTree.SelectItem(hItem))
		return;
	
	//////////////////////////////////////////////////////////////////////////
	// 右键菜单代码	
	//////////////////////////////////////////////////////////////////////////
	*pResult = 0;
}

void CTreeBar::OnTreeEndLabelEdit(NMHDR* pNMHDR, LRESULT* pResult)
{
	CEdit* pEdit = m_wndTree.GetEditControl();
	if(!pEdit)
		return;
	CString strText;
	pEdit->GetWindowText(strText);
	if(strText.GetLength() == 0)
		return;
	HTREEITEM hSelItem = m_wndTree.GetSelectedItem();
	if(!hSelItem)
		return;	
	//////////////////////////////////////////////////////////////////////////
	// 处理代码
	

	//////////////////////////////////////////////////////////////////////////
	m_wndTree.SetItemText(hSelItem, strText);
	*pResult = 0;
}

void CTreeBar::OnTreeBeginDrag(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);	

	// 如果是无意拖拽，则放弃操作
	if(GetTickCount() - m_dwDragStart < 60)
		return;
	m_hItemDragSrc = pNMTreeView->itemNew.hItem;
	m_wndTree.Expand(m_hItemDragSrc, TVE_COLLAPSE);

	// 得到用于拖动时显示的图像列表
	delete m_pDragImage;
	m_pDragImage = 0;
	m_pDragImage = m_wndTree.CreateDragImage(m_hItemDragSrc);
	if(!m_pDragImage)
		return;
	m_bDragging = true;
	m_pDragImage->BeginDrag(0, CPoint(8,8));
	CPoint pt = pNMTreeView->ptDrag;
	m_pDragImage->DragEnter(&m_wndTree, pt); // 将拖拽动作限制在该窗口
	SetCapture();
	m_nScrollTimerID = SetTimer(2, 40, NULL);
	*pResult = 0;
}

void CTreeBar::OnTimer(UINT_PTR nIDEvent)
{
	// 鼠标敏感节点
	if(nIDEvent == m_nHoverTimerID)
	{
		KillTimer(m_nHoverTimerID);
		m_nHoverTimerID = 0;		
		UINT uFlag = 0;
		HTREEITEM hItem = m_wndTree.HitTest(m_HoverPoint, &uFlag);
		if(hItem && m_bDragging)
		{
			m_wndTree.SelectItem(hItem);
			m_wndTree.Expand(hItem, TVE_EXPAND);
		}
	}
	// 处理拖拽过程中的滚动问题
	else if(nIDEvent == m_nScrollTimerID)
	{
		++m_TimerTicks;
		CPoint pt;
		GetCursorPos(&pt);
		CRect rect;
		m_wndTree.GetClientRect(&rect);
		ClientToScreen(&rect);

		HTREEITEM hItem = m_wndTree.GetFirstVisibleItem();

		if(pt.y<rect.top+10)
		{
			// 向上滚动
			int slowscroll = 6-(rect.top+10-pt.y)/20;
			if(0 == (m_TimerTicks%(slowscroll>0?slowscroll:1)))
			{
				CImageList::DragShowNolock(false);
				SendMessage(WM_VSCROLL, SB_LINEUP);
				if(hItem)
					m_wndTree.SelectDropTarget(hItem);
				m_hItemDragDest = hItem;
				CImageList::DragShowNolock(true);
			}
		}
		else
		{
			// 向下滚动
			int slowscroll = 6-(pt.y-rect.bottom+10)/20;
			if(0 == (m_TimerTicks%(slowscroll>0?slowscroll:1)))
			{
				CImageList::DragShowNolock(false);
				SendMessage(WM_VSCROLL, SB_LINEDOWN);
				int iNum = m_wndTree.GetVisibleCount();
				for (int i=0; i<iNum-1; ++i)
				{
					hItem = m_wndTree.GetNextVisibleItem(hItem);
				}
				if(hItem)
					m_wndTree.SelectDropTarget(hItem);
				m_hItemDragDest = hItem;
				CImageList::DragShowNolock(true);
			}
		}
	}
	else
		CBCGPDockingControlBar::OnTimer(nIDEvent);
}

void CTreeBar::OnTreeLButtonDown(UINT nFlags, CPoint point)
{
	// 处理无意拖拽
	m_dwDragStart = GetTickCount();
}

void CTreeBar::OnTreeLButtonUp(UINT nFlags, CPoint point)
{
	if(!m_bDragging)
		return;
	m_bDragging = false;
	CImageList::DragLeave(&m_wndTree);
	CImageList::EndDrag();
	ReleaseCapture();
	delete m_pDragImage;
	m_pDragImage = 0;

	m_wndTree.SelectDropTarget(NULL);
	if(m_hItemDragSrc == m_hItemDragDest)
	{
		KillTimer(m_nScrollTimerID);
		return;
	}

	if(!m_hItemDragDest)
	{
		KillTimer(m_nScrollTimerID);
		return;
	}
	m_wndTree.Expand(m_hItemDragDest, TVE_EXPAND);

	//////////////////////////////////////////////////////////////////////////
	// 填入数据处理代码
	//////////////////////////////////////////////////////////////////////////
	m_wndTree.DeleteItem(m_hItemDragSrc);
	KillTimer(m_nScrollTimerID);
}

void CTreeBar::OnTreeMouseMove(UINT nFlags, CPoint point)
{
	HTREEITEM hItem;
	UINT flags;

	// 检测鼠标敏感定时器是否存在，如果存在则删除，删除后再定时
	if(m_nHoverTimerID)
	{
		KillTimer(m_nHoverTimerID);
		m_nHoverTimerID = 0;
	}
	
	m_nHoverTimerID = SetTimer(1, 800, NULL); // 定时为0.8秒则自动展开
	m_HoverPoint = point;
	if(!m_bDragging)
		return;
	CPoint pt = point;
	CImageList::DragMove(pt);

	// 鼠标经过时高亮显示	
	CImageList::DragShowNolock(false); // 避免鼠标经过时留下难看的痕迹

	hItem = m_wndTree.HitTest(point, &flags);
	if(NULL != hItem)
	{
		m_wndTree.SelectDropTarget(hItem);
		m_hItemDragDest = hItem;
	}

	CImageList::DragShowNolock(true);

	// 当条目被拖拽到左边缘时，将条目放在根下
	CRect rect;
	m_wndTree.GetClientRect(&rect);
	if(point.x<rect.left+20)
		m_hItemDragDest = NULL;
}

BOOL CTreeBar::PreTranslateMessage(MSG* pMsg)
{
	switch (pMsg->message)
	{
	case WM_LBUTTONDOWN:
		{
			int x = LOWORD(pMsg->lParam);
			int y = HIWORD(pMsg->lParam);
			CRect rcClient;
			m_wndTree.GetClientRect(&rcClient);
			if(pMsg->hwnd == m_wndTree.GetSafeHwnd())
			{

			}
			else if(pMsg->hwnd == GetSafeHwnd())
			{
				CRect rcWindow;
				m_wndTree.GetWindowRect(&rcWindow);
				ScreenToClient(&rcWindow);
				x -= rcWindow.left;
				y -= rcWindow.top;
			}
			CPoint pt(x, y);
			if(rcClient.PtInRect(pt))
				OnTreeLButtonDown(LOWORD(pMsg->wParam), pt);
		}
		break;
	case WM_LBUTTONUP:
		{
			int x = LOWORD(pMsg->lParam);
			int y = HIWORD(pMsg->lParam);
			CRect rcClient;
			m_wndTree.GetClientRect(&rcClient);
			if(pMsg->hwnd == m_wndTree.GetSafeHwnd())
			{

			}
			else if(pMsg->hwnd == GetSafeHwnd())
			{
				CRect rcWindow;
				m_wndTree.GetWindowRect(&rcWindow);
				ScreenToClient(&rcWindow);
				x -= rcWindow.left;
				y -= rcWindow.top;
			}
			CPoint pt(x, y);
			if(rcClient.PtInRect(pt))
				OnTreeLButtonUp(LOWORD(pMsg->wParam), pt);
		}
		break;
	case WM_MOUSEMOVE:
		{
			int x = LOWORD(pMsg->lParam);
			int y = HIWORD(pMsg->lParam);
			CRect rcClient;
			m_wndTree.GetClientRect(&rcClient);
			if(pMsg->hwnd == m_wndTree.GetSafeHwnd())
			{

			}
			else if(pMsg->hwnd == GetSafeHwnd())
			{
				CRect rcWindow;
				m_wndTree.GetWindowRect(&rcWindow);
				ScreenToClient(&rcWindow);
				x -= rcWindow.left;
				y -= rcWindow.top;
			}
			CPoint pt(x, y);
			if(rcClient.PtInRect(pt))
				OnTreeMouseMove(LOWORD(pMsg->wParam), pt);
		}
		break;
	case WM_KEYDOWN:
		{
			if(pMsg->wParam == VK_RETURN)
			{
				CEdit* pWndEdit = m_wndTree.GetEditControl();
				if(pWndEdit)
					m_wndTree.GetEditControl()->CloseWindow();
				return TRUE;
			}
		}
	}
	return CBCGPDockingControlBar::PreTranslateMessage(pMsg);
}





void CTreeBar::OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/)
{
}

void CTreeBar::OnNewFolder()
{
	HTREEITEM hItem = m_wndTree.GetSelectedItem();
	if(!hItem)
		return;
}

void CTreeBar::OnNewFile()
{
	HTREEITEM hItem = m_wndTree.GetSelectedItem();
	if(!hItem)
		return;
}

void CTreeBar::OnDelete()
{
	HTREEITEM hItem = m_wndTree.GetSelectedItem();
	if(!hItem)
		return;
}

BOOL CTreeBar::SetItemText(HTREEITEM hItem, LPCTSTR lpszText)
{
	return m_wndTree.SetItemText(hItem, lpszText);
}