#if !defined(AFX_PROPERTYFILEBOX_H__0CB093DD_9746_458B_AA2E_B19CA06EDE99__INCLUDED_)
#define AFX_PROPERTYFILEBOX_H__0CB093DD_9746_458B_AA2E_B19CA06EDE99__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PropertyFileBox.h : header file
//
#include "PropertyBox.h"
/////////////////////////////////////////////////////////////////////////////
// CPropertyFileBox window

class CPropertyFileBox : public CPropertyBox
{
// Construction
public:
	CPropertyFileBox();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPropertyFileBox)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual BOOL IsModified();
	virtual void DrawSelf(CDC * pDC);
	virtual ~CPropertyFileBox();

	// Generated message map functions
protected:
	afx_msg void OnBnClicked();
	afx_msg void OnEditChanged();
	CButton m_wnd_button;
	CString m_str_file;
	CString m_str_path;
	CEdit m_wnd_edit;
	//{{AFX_MSG(CPropertyFileBox)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PROPERTYFILEBOX_H__0CB093DD_9746_458B_AA2E_B19CA06EDE99__INCLUDED_)
