// EventBar.h : interface of the CEventBar class
//
/////////////////////////////////////////////////////////////////////////////

#pragma once

#include "TreeBar.h"
#include "EventDisplay.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CEventBar : public CTreeBar
{
public:
	CEventBar();
	virtual ~CEventBar();

// Implementation
public:
	bool				UpdateTree();
	virtual BOOL		SetItemText(HTREEITEM hItem, LPCTSTR lpszText);
protected:
	//{{AFX_MSG(CEventBar)
	virtual afx_msg void OnTreeRClick(NMHDR* pNMHDR, LRESULT* pResult);	
	afx_msg void OnNewGroup();
	afx_msg void OnNewEvent();
	afx_msg void OnDeleteGroup();
	afx_msg void OnDeleteEvent();
	afx_msg void OnBatchDeleteEvent();
	afx_msg void OnRefresh();
	afx_msg void OnSavePreset();
	afx_msg void OnLoadPreset();
	afx_msg void OnBatchNewEvent();
	virtual afx_msg void OnTreeSelectChanged(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnTreeKeyDown(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
protected:
	virtual void		OnTreeLButtonUp(UINT nFlags, CPoint point);
	void				newGroup(HTREEITEM hItem);
	bool				deleteEvent(Event* pEvent);
protected:
	HTREEITEM			m_hRClickItem;
	CEventDisplay		m_eventDisplay;
public:
	afx_msg void OnPaint();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
};

/////////////////////////////////////////////////////////////////////////////
