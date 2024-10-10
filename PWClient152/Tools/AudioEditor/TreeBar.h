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
	UINT				m_TimerTicks;		// ��������Ķ�ʱ����������ʱ��
	UINT_PTR			m_nScrollTimerID;	// ��������Ķ�ʱ��
	CPoint				m_HoverPoint;		// ���λ��
	UINT_PTR			m_nHoverTimerID;	// ������ж�ʱ��
	DWORD				m_dwDragStart;		// ������������һ�̵�ʱ��
	bool				m_bDragging;		// ��ʶ�Ƿ������϶�������
	CImageList*			m_pDragImage;		// �϶�ʱ��ʾ��ͼ���б�
	HTREEITEM			m_hItemDragSrc;		// ���϶��ı�ǩ
	HTREEITEM			m_hItemDragDest;	// �����϶��ı�ǩ
protected:
	CImageList			m_ImageList;
public:
	CTreeCtrlEx*		GetTree() { return &m_wndTree; }
	virtual BOOL		SetItemText(HTREEITEM hItem, LPCTSTR lpszText);
};
