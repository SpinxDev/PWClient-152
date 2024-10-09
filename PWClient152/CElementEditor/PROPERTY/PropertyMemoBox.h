#pragma once
// PropertyMemoBox.h : header file
//
#include "PropertyBox.h"
/////////////////////////////////////////////////////////////////////////////
// CPropertyMemoBox window

class CPropertyMemoBox : public CPropertyBox
{
// Construction
public:
	CPropertyMemoBox();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPropertyMemoBox)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual BOOL IsModified();
	virtual void DrawSelf(CDC * pDC);
	virtual ~CPropertyMemoBox();

	// Generated message map functions
protected:
	afx_msg void OnBnClicked();
	afx_msg void OnEditChanged();
	CString m_str_value;
	CEdit m_wnd_edit;
	CButton m_wnd_button;
	bool m_bool_modified;
	//{{AFX_MSG(CPropertyMemoBox)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

