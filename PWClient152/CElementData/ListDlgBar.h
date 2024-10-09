#if !defined(AFX_LISTDLGBAR_H__A8E94C49_E573_40CE_9972_14C9136649EB__INCLUDED_)
#define AFX_LISTDLGBAR_H__A8E94C49_E573_40CE_9972_14C9136649EB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ListDlgBar.h : header file
//

#include "AString.h"
#include "ListDlg.h"

#define SORT1_NUM	5

/////////////////////////////////////////////////////////////////////////////
// CListDlgBar dialog


class CListDlgBar : public CDialogBar
{
// Construction
public:
	CListDlgBar(CWnd* pParent = NULL);   // standard constructor
	CListDlg m_ListDlg;
// Dialog Data
	//{{AFX_DATA(CListDlgBar)
	enum { IDD = IDD_DIALOG_LIST_BAR };
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CListDlgBar)
	public:
	virtual BOOL DestroyWindow();
	virtual void OnFinalRelease();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL
	
// Implementation
protected:
	
	// Generated message map functions
	//{{AFX_MSG(CListDlgBar)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	virtual void OnOK(){};
	virtual void OnCancel(){};
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LISTDLGBAR_H__A8E94C49_E573_40CE_9972_14C9136649EB__INCLUDED_)
