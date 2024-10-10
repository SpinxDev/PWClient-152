#pragma once
#include "afxcmn.h"

#include <map>
#include <vector>

#define TVGN_EX_ALL			0x000F 

typedef std::map<HTREEITEM, UINT> ItemValueMap;
typedef std::map<HTREEITEM, DWORD_PTR> ItemDataMap;
typedef std::vector<HTREEITEM> ItemArray;

class CTreeCtrlEx :	public CTreeCtrl
{
public:	
	CTreeCtrlEx();
	~CTreeCtrlEx(void);
	BOOL Create(DWORD dwStyle, DWORD dwExStyle, const RECT& rect, CWnd* pParentWnd, UINT nID); 
	BOOL Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID); 
public:
	UINT				GetExtraData(HTREEITEM hItem) { return m_mapItemValue[hItem]; }
	void				SetExtraData(HTREEITEM hItem, UINT uiExtra) { m_mapItemValue[hItem] = uiExtra; }
	DWORD_PTR			GetExtraUserData(HTREEITEM hItem) { return m_mapItemData[hItem]; }
	void				SetExtraUserData(HTREEITEM hItem, DWORD_PTR lpExtraUserData) { m_mapItemData[hItem] = lpExtraUserData; }
	void				DeleteChildren(HTREEITEM hItem);
	bool				IsChildItem(HTREEITEM hItem, HTREEITEM hUnknownItem);
	void				EnableMultiSelect(bool bEnable) { m_bMultiSelect = bEnable; }
	int					GetSelectedItemNum() const { return (int)m_arrSelectItem.size(); }
	HTREEITEM			GetSelectedItemByIndex(int idx) const { return m_arrSelectItem[idx]; }	
protected:
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point); 
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point); 
	afx_msg void OnMouseMove(UINT nFlags, CPoint point); 
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags); 
	afx_msg BOOL OnItemexpanding(NMHDR* pNMHDR, LRESULT* pResult); 
	afx_msg BOOL OnSetfocus(NMHDR* pNMHDR, LRESULT* pResult); 
	afx_msg BOOL OnKillfocus(NMHDR* pNMHDR, LRESULT* pResult); 
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point); 
	afx_msg BOOL OnSelchanged(NMHDR* pNMHDR, LRESULT* pResult); 
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point); 
	afx_msg void OnTimer(UINT_PTR nIDEvent); 
	DECLARE_MESSAGE_MAP()
	//afx_msg void		OnLButtonDown(UINT nFlags, CPoint point);
	//void				clearSelection();

	UINT				GetSelectedCount() const; 
	HTREEITEM			GetNextItem(HTREEITEM hItem, UINT nCode); 
	HTREEITEM			GetFirstSelectedItem(); 
	HTREEITEM			GetNextSelectedItem(HTREEITEM hItem); 
	HTREEITEM			GetPrevSelectedItem(HTREEITEM hItem); 
	HTREEITEM			ItemFromData(DWORD dwData, HTREEITEM hStartAtItem=NULL) const; 
	BOOL				SelectItemEx(HTREEITEM hItem, BOOL bSelect=TRUE);
	BOOL				SelectItems(HTREEITEM hFromItem, HTREEITEM hToItem); 
	void				ClearSelection(BOOL bMultiOnly=FALSE);
	void				SelectMultiple( HTREEITEM hClickedItem, UINT nFlags, CPoint point );
protected:
	ItemValueMap		m_mapItemValue;
	ItemDataMap			m_mapItemData;
	ItemArray			m_arrSelectItem;
	bool				m_bMultiSelect;

	BOOL				m_bSelectPending; 
	CPoint				m_ptClick; 
	HTREEITEM			m_hClickedItem; 
	HTREEITEM			m_hFirstSelectedItem; 
	BOOL				m_bSelectionComplete; 
	BOOL				m_bEditLabelPending; 
	UINT				m_idTimer;
};

HTREEITEM GetTreeItemFromData(CTreeCtrl& treeCtrl, DWORD dwData, HTREEITEM hStartAtItem=NULL); 