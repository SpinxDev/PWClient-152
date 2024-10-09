#if !defined(AFX_RIGHTTREECTRL_H__CB00CECF_66F7_4385_8927_3DBF52AD4F43__INCLUDED_)
#define AFX_RIGHTTREECTRL_H__CB00CECF_66F7_4385_8927_3DBF52AD4F43__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// RightTreeCtrl.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CRightTreeCtrl window

class CRightTreeCtrl : public CTreeCtrl
{
// Construction
public:
	CRightTreeCtrl();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRightTreeCtrl)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CRightTreeCtrl();

	// Generated message map functions
protected:
	//{{AFX_MSG(CRightTreeCtrl)
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_RIGHTTREECTRL_H__CB00CECF_66F7_4385_8927_3DBF52AD4F43__INCLUDED_)
