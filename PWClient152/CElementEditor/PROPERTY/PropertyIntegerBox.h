#if !defined(AFX_PROPERTYINTEGERBOX_H__2F5793EB_E7DB_49E3_AB9C_E88FE56E6D52__INCLUDED_)
#define AFX_PROPERTYINTEGERBOX_H__2F5793EB_E7DB_49E3_AB9C_E88FE56E6D52__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PropertyIntegerBox.h : header file
//
#include "PropertyBox.h"
/////////////////////////////////////////////////////////////////////////////
// CPropertyIntegerBox window

class CPropertyIntegerBox : public CPropertyBox
{
// Construction
public:
	CPropertyIntegerBox();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPropertyIntegerBox)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual BOOL IsModified();
	virtual void DrawSelf(CDC * pDC);
	virtual ~CPropertyIntegerBox();

	// Generated message map functions
protected:
	BOOL m_bool_modified;
	afx_msg void OnEditChanged();
	int	m_int_index;

	CComboBox m_wnd_list;
	CEdit	  m_wnd_edit;
	//{{AFX_MSG(CPropertyIntegerBox)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PROPERTYINTEGERBOX_H__2F5793EB_E7DB_49E3_AB9C_E88FE56E6D52__INCLUDED_)
