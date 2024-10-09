#if !defined(AFX_PROPERTYIDBOX_H__6D7A828D_8474_43F2_953E_D048D41CB595__INCLUDED_)
#define AFX_PROPERTYIDBOX_H__6D7A828D_8474_43F2_953E_D048D41CB595__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PropertyIDBox.h : header file
//
#include "PropertyBox.h"
/////////////////////////////////////////////////////////////////////////////
// CPropertyIDBox window

class CPropertyIDBox : public CPropertyBox
{
// Construction
public:
	CPropertyIDBox(CString type);

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPropertyIDBox)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	//}}AFX_VIRTUAL


// Implementation
public:
	virtual void DrawSelf(CDC * pDC);
	virtual BOOL IsModified();
	virtual ~CPropertyIDBox();

	// Generated message map functions
protected:

	afx_msg void OnSelChanged();
	CString		m_str_type;
	CString		m_str_object;
	CComboBox	m_wnd_list;
	BOOL		m_bool_modified;
	//{{AFX_MSG(CPropertyIDBox)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PROPERTYIDBOX_H__6D7A828D_8474_43F2_953E_D048D41CB595__INCLUDED_)
