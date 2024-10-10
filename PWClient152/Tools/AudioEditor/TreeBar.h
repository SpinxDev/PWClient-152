#pragma once

#include "TreeCtrlEx.h"

class CTreeBar : public CBCGPDockingControlBar
{
public:
	CTreeBar(void);
	virtual ~CTreeBar(void);
protected:
	//{{AFX_MSG(CTreeBar)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnPaint();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	virtual afx_msg void OnTreeSelectChanged(NMHDR* pNMHDR, LRESULT* pResult);
	virtual afx_msg void OnTreeRClick(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnTreeEndLabelEdit(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnTreeBeginDrag(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnNewFolder();
	afx_msg void OnNewFile();
	afx_msg void OnDelete();	
	afx_msg void OnRename();
	afx_msg void OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
protected:
	void				OnTreeLButtonDown(UINT nFlags, CPoint point);
	virtual void		OnTreeLButtonUp(UINT nFlags, CPoint point);
	void				OnTreeMouseMove(UINT nFlags, CPoint point);
	virtual BOOL		PreTranslateMessage(MSG* pMsg);	
protected:
	CTreeCtrlEx			m_wndTree;
	UINT				m_TimerTicks;		// 处理滚动的定时器所经过的时间
	UINT_PTR			m_nScrollTimerID;	// 处理滚动的定时器
	CPoint				m_HoverPoint;		// 鼠标位置
	UINT_PTR			m_nHoverTimerID;	// 鼠标敏感定时器
	DWORD				m_dwDragStart;		// 按下鼠标左键那一刻的时间
	bool				m_bDragging;		// 标识是否正在拖动过程中
	CImageList*			m_pDragImage;		// 拖动时显示的图像列表
	HTREEITEM			m_hItemDragSrc;		// 被拖动的标签
	HTREEITEM			m_hItemDragDest;	// 接受拖动的标签
protected:
	CImageList			m_ImageList;
public:
	CTreeCtrlEx*		GetTree() { return &m_wndTree; }
	virtual BOOL		SetItemText(HTREEITEM hItem, LPCTSTR lpszText);
};
