#if !defined(AFX_TASKPROPERTYSHEET_H__0ABA4B48_3840_45B9_BFAA_D3A544282EF3__INCLUDED_)
#define AFX_TASKPROPERTYSHEET_H__0ABA4B48_3840_45B9_BFAA_D3A544282EF3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TaskPropertySheet.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CTaskPropertySheet

class CTaskPropertySheet : public CPropertySheet
{
	DECLARE_DYNAMIC(CTaskPropertySheet)

// Construction
public:
	CTaskPropertySheet(UINT nIDCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	CTaskPropertySheet(LPCTSTR pszCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTaskPropertySheet)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CTaskPropertySheet();

	// Generated message map functions
protected:
	//{{AFX_MSG(CTaskPropertySheet)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	afx_msg void OnSelchangedPage(NMHDR* pNMHDR, LRESULT* pResult);		
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TASKPROPERTYSHEET_H__0ABA4B48_3840_45B9_BFAA_D3A544282EF3__INCLUDED_)
