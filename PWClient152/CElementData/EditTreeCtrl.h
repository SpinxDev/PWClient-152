#if !defined(AFX_EDITTREECTRL_H__9B91B09A_C2CB_4E07_883D_B309FE648D4D__INCLUDED_)
#define AFX_EDITTREECTRL_H__9B91B09A_C2CB_4E07_883D_B309FE648D4D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// EditTreeCtrl.h : header file
//

#include "Treasure.h"


/////////////////////////////////////////////////////////////////////////////
// CEditTreeCtrl window

class CEditTreeCtrl : public CTreeCtrl
{
// Construction
public:
	CEditTreeCtrl();

// Attributes
public:

// Operations
public:
	void SetTreasureMan(CTreasureMan *pMan){ m_pTreasureMan = pMan; }

	void MoveUp();
	void MoveDown();

	bool SortTreasure();
	HTREEITEM CopyBranch(HTREEITEM htiBranch,HTREEITEM htiNewParent,HTREEITEM htiAfter);
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEditTreeCtrl)
	public:
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CEditTreeCtrl();
	bool  m_bReadOnly;
	bool  m_bChanged;
	// Generated message map functions
protected:
	MAIN_TYPE* FindMainType( const char *szName);
	void ReplaceSubTextName( int id, const char *szName);
	bool ObjIsEqual(TREASURE_ITEM *pObj1, TREASURE_ITEM *pObj2);
	void UpdateMenu(CMenu *pMenu);
	int  GetChildItemsCount(HTREEITEM hParent);
	
	void EnumTree(HTREEITEM hParent, abase::vector<TREASURE_ITEM*>* pList);
	HTREEITEM CopyItem(HTREEITEM hItem,HTREEITEM htiNewParent,HTREEITEM htiAfter);

	UINT          m_TimerTicks;      //��������Ķ�ʱ����������ʱ��
	UINT          m_nScrollTimerID;  //��������Ķ�ʱ��
	CPoint        m_HoverPoint;      //���λ��
	UINT          m_nHoverTimerID;   //������ж�ʱ��
	DWORD         m_dwDragStart;     //������������һ�̵�ʱ��
	BOOL          m_bDragging;       //��ʶ�Ƿ������϶�������
	BOOL          m_bEditting;       
	CImageList*   m_pDragImage;      //�϶�ʱ��ʾ��ͼ���б�
	HTREEITEM     m_hItemDragS;      //���϶��ı�ǩ
	HTREEITEM     m_hItemDragD;      //�����϶��ı�ǩ

	//{{AFX_MSG(CEditTreeCtrl)
	afx_msg void OnNcRButtonDown(UINT nHitTest, CPoint point);
	afx_msg void OnRclick(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnMenuAddsub();
	afx_msg void OnBeginlabeledit(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnEndlabeledit(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDblclk(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnMenuAddTreasure();
	afx_msg void OnMenuDelTreasure();
	afx_msg void OnMenuDelsub();

	afx_msg void OnBegindrag(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG

	CTreasureMan *m_pTreasureMan;

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_EDITTREECTRL_H__9B91B09A_C2CB_4E07_883D_B309FE648D4D__INCLUDED_)
