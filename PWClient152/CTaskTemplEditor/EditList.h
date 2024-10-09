#if !defined(AFX_EDITLIST_H__E050A554_2071_4B17_ABC8_AEBC6F61B31C__INCLUDED_)
#define AFX_EDITLIST_H__E050A554_2071_4B17_ABC8_AEBC6F61B31C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// EditList.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CEditList window

class CEditList : public CListCtrl
{
// Construction
public:
	CEditList();

// Attributes
public:
	
// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEditList)
	protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CEditList();

	CComboBox * ComboItem( int nItem,  int nSubItem, CStringList& m_strList);
	CEdit * EditItem( int nItem, int nSubItem);

	// Generated message map functions
protected:
	//{{AFX_MSG(CEditList)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnEndlabeledit(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_EDITLIST_H__E050A554_2071_4B17_ABC8_AEBC6F61B31C__INCLUDED_)
