#if !defined(AFX_PROPERTYCOLORBOX_H__A8E4B5B2_E145_4095_A80F_53EF10DEB432__INCLUDED_)
#define AFX_PROPERTYCOLORBOX_H__A8E4B5B2_E145_4095_A80F_53EF10DEB432__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PropertyColorBox.h : header file
//
#include "PropertyBox.h"
/////////////////////////////////////////////////////////////////////////////
// CPropertyColorBox window

class CPropertyColorBox : public CPropertyBox
{
// Construction
public:
	CPropertyColorBox();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPropertyColorBox)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	//}}AFX_VIRTUAL

// Implementation
public:

	virtual BOOL IsModified();
	virtual void DrawSelf(CDC * pDC);
	virtual ~CPropertyColorBox();
	// Generated message map functions
protected:

	afx_msg void OnBnClicked();
	CButton		m_wnd_button;
	BOOL		m_bool_modified;
	DWORD		m_color_value;
	// Generated message map functions
protected:
	//{{AFX_MSG(CPropertyColorBox)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PROPERTYCOLORBOX_H__A8E4B5B2_E145_4095_A80F_53EF10DEB432__INCLUDED_)
