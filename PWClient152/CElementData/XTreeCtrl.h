#if !defined(AFX_XTREECTRL_H__3EF12526_EF66_4FD9_A572_59476441D79A__INCLUDED_)
#define AFX_XTREECTRL_H__3EF12526_EF66_4FD9_A572_59476441D79A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// XTreeCtrl.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CXTreeCtrl window

class CXTreeCtrl : public CTreeCtrl
{
// Construction
public:
	CXTreeCtrl();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CXTreeCtrl)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CXTreeCtrl();
	HTREEITEM CopyBranch(HTREEITEM htiBranch,HTREEITEM htiNewParent,HTREEITEM htiAfter);
	// Generated message map functions
protected:
	UINT          m_TimerTicks;      //��������Ķ�ʱ����������ʱ��
	UINT          m_nScrollTimerID;  //��������Ķ�ʱ��
	CPoint        m_HoverPoint;      //���λ��
	UINT          m_nHoverTimerID;   //������ж�ʱ��
	DWORD         m_dwDragStart;     //������������һ�̵�ʱ��
	BOOL          m_bDragging;       //��ʶ�Ƿ������϶�������
	CImageList*   m_pDragImage;      //�϶�ʱ��ʾ��ͼ���б�
	HTREEITEM     m_hItemDragS;      //���϶��ı�ǩ
	HTREEITEM     m_hItemDragD;      //�����϶��ı�ǩ
	//{{AFX_MSG(CXTreeCtrl)
	afx_msg void OnBegindrag(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
private:
	HTREEITEM CopyItem(HTREEITEM hItem,HTREEITEM htiNewParent,HTREEITEM htiAfter);
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_XTREECTRL_H__3EF12526_EF66_4FD9_A572_59476441D79A__INCLUDED_)
