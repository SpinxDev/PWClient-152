#if !defined(AFX_ITEMSWANTEDDLG_H__D764B5C7_2B07_4ABF_9ADD_AE19ADB12420__INCLUDED_)
#define AFX_ITEMSWANTEDDLG_H__D764B5C7_2B07_4ABF_9ADD_AE19ADB12420__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ItemsWantedDlg.h : header file
//

#include "TaskTempl.h"
#include "EditList.h"
/////////////////////////////////////////////////////////////////////////////
// CItemsWantedDlg dialog

class CItemsWantedDlg : public CDialog
{
// Construction
public:
	CItemsWantedDlg(
		unsigned long ulMaxCount,
		ITEM_WANTED* pItems,
		unsigned long* pCount,
		BOOL bOnlyTaskItem = false,
		BOOL bFlag = false,
		CWnd* pParent = NULL,
		BOOL bPremItem = false);   // standard constructor

	unsigned long m_ulMaxCount;
	ITEM_WANTED* m_pItems;
	unsigned long* m_pCount;
	BOOL m_bOnlyTaskItem;
	BOOL m_bFlag;
	BOOL m_bPremItem;

	void SetRandOne(BOOL bRandOne) { m_bRandOne = bRandOne; }

// Dialog Data
	//{{AFX_DATA(CItemsWantedDlg)
	enum { IDD = IDD_ITEMS_WANTED };
	CEditList	m_ItemsLst;
	CStringList m_strList;
	BOOL	m_bRandOne;
	float	m_fTotalProb;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CItemsWantedDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CItemsWantedDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnAdd();
	afx_msg void OnDel();
	afx_msg void OnEditNum();
	afx_msg void OnProb();
	afx_msg void OnRandOne();
	afx_msg void OnPeriod();
	afx_msg void OnClickChangeDateType(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ITEMSWANTEDDLG_H__D764B5C7_2B07_4ABF_9ADD_AE19ADB12420__INCLUDED_)
