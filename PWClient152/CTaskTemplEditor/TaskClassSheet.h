#if !defined(AFX_TASKCLASSSHEET_H__2F4E049E_2BA4_4816_BB1E_6501DF06ADAC__INCLUDED_)
#define AFX_TASKCLASSSHEET_H__2F4E049E_2BA4_4816_BB1E_6501DF06ADAC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TaskClassSheet.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CTaskClassSheet

class CTaskClassSheet : public CPropertySheet
{
	DECLARE_DYNAMIC(CTaskClassSheet)

// Construction
public:
	CTaskClassSheet(UINT nIDCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	CTaskClassSheet(LPCTSTR pszCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTaskClassSheet)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CTaskClassSheet();

	// Generated message map functions
protected:
	//{{AFX_MSG(CTaskClassSheet)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TASKCLASSSHEET_H__2F4E049E_2BA4_4816_BB1E_6501DF06ADAC__INCLUDED_)
