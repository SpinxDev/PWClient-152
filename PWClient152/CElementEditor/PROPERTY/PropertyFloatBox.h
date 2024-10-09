#if !defined(AFX_PROPERTYFLOATBOX_H__2F96ACFD_EB59_40F6_917E_9A6B8E0EF383__INCLUDED_)
#define AFX_PROPERTYFLOATBOX_H__2F96ACFD_EB59_40F6_917E_9A6B8E0EF383__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PropertyFloatBox.h : header file
//
#include "PropertyBox.h"
/////////////////////////////////////////////////////////////////////////////
// CPropertyFloatBox window

class CPropertyFloatBox : public CPropertyBox
{
// Construction
public:
	CPropertyFloatBox();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPropertyFloatBox)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual void DrawSelf(CDC * pDC);
	virtual BOOL IsModified();
	virtual ~CPropertyFloatBox();

	// Generated message map functions
protected:

	afx_msg void OnEditChanged();
	CEdit m_wnd_edit;
	//{{AFX_MSG(CPropertyFloatBox)
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PROPERTYFLOATBOX_H__2F96ACFD_EB59_40F6_917E_9A6B8E0EF383__INCLUDED_)
