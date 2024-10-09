#if !defined(AFX_PROPERTYBOOLEANBOX_H__4F3B7255_0CAE_48FD_9478_D5987E46C7E4__INCLUDED_)
#define AFX_PROPERTYBOOLEANBOX_H__4F3B7255_0CAE_48FD_9478_D5987E46C7E4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PropertyBooleanBox.h : header file
//
#include "PropertyBox.h"
/////////////////////////////////////////////////////////////////////////////
// CPropertyBooleanBox window

class CPropertyBooleanBox : public CPropertyBox
{
// Construction
public:
	CPropertyBooleanBox();

// Attributes
public:

// Operations
public:
	virtual void DrawSelf(CDC * pDC);
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPropertyBooleanBox)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual BOOL IsModified();
	virtual ~CPropertyBooleanBox();

	// Generated message map functions
protected:
	int m_int_sel;
	afx_msg void OnSelChanged();
	BOOL m_bool_modified;
	CComboBox m_wnd_combobox;
	//{{AFX_MSG(CPropertyBooleanBox)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PROPERTYBOOLEANBOX_H__4F3B7255_0CAE_48FD_9478_D5987E46C7E4__INCLUDED_)
