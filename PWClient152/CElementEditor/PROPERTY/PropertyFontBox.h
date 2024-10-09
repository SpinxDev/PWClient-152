#ifndef		_PROPERTYFONTBOX_H__
#define		_PROPERTYFONTBOX_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PropertyFontBox.h : header file
//
#include "PropertyBox.h"
#include <A3DVector.h>
#include <A3DFontMan.h>

/////////////////////////////////////////////////////////////////////////////
// CPropertyFontBox window

class CPropertyFontBox : public CPropertyBox
{
// Construction
public:
	CPropertyFontBox();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPropertyFontBox)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	//}}AFX_VIRTUAL

// Implementation
public:

	virtual BOOL IsModified();
	virtual void DrawSelf(CDC * pDC);
	virtual ~CPropertyFontBox();

	// Generated message map functions
protected:

	afx_msg void OnBnClicked();
	CButton m_wnd_button;
	BOOL		m_bool_modified;
	A3DFontMan::FONTTYPE m_font;

	//{{AFX_MSG(CPropertyFontBox)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

#endif