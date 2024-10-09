#if !defined(AFX_PROPERTYVECTORBOX_H__17EDC5B6_A2DF_486B_8D3D_27DB04F45A2A__INCLUDED_)
#define AFX_PROPERTYVECTORBOX_H__17EDC5B6_A2DF_486B_8D3D_27DB04F45A2A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PropertyVectorBox.h : header file
//
#include "PropertyBox.h"
#include <A3DVector.h>
/////////////////////////////////////////////////////////////////////////////
// CPropertyVectorBox window

class CPropertyVectorBox : public CPropertyBox
{
// Construction
public:
	CPropertyVectorBox();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPropertyVectorBox)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	//}}AFX_VIRTUAL

// Implementation
public:

	virtual BOOL IsModified();
	virtual void DrawSelf(CDC * pDC);
	virtual ~CPropertyVectorBox();

	// Generated message map functions
protected:

	afx_msg void OnBnClicked();
	CButton m_wnd_button;
	BOOL		m_bool_modified;
	A3DVECTOR3	m_av3_value;

	//{{AFX_MSG(CPropertyVectorBox)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PROPERTYVECTORBOX_H__17EDC5B6_A2DF_486B_8D3D_27DB04F45A2A__INCLUDED_)
