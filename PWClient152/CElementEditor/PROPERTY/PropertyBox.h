#if !defined(AFX_PROPERTYBOX_H__0FF76CA7_E9FD_44A0_9D0E_6E75E1DC20FA__INCLUDED_)
#define AFX_PROPERTYBOX_H__0FF76CA7_E9FD_44A0_9D0E_6E75E1DC20FA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PropertyBox.h : header file
//

#include "A3DEngine.h"

/////////////////////////////////////////////////////////////////////////////
// CPropertyBox window
class APropertyObject;
class AProperty;
class CPropertyBox : public CWnd
{
// Construction
public:
	CPropertyBox();

// Attributes
public:
	static A3DEngine* g_pA3DEngine;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPropertyBox)
	public:
	//}}AFX_VIRTUAL
	int		m_defaultState;
	int		m_property_index;
	AProperty *	m_ptr_property;
	APropertyObject *m_ptr_data;

// Implementation
public:
	static CPropertyBox * CreatePropertyBox(CWnd * pParent, UINT nID, CRect & rc, APropertyObject * pData, AProperty * pProperty);

	virtual BOOL IsModified();
	virtual void DrawSelf(CDC * pDC);
	virtual ~CPropertyBox();

	BOOL Create(LPCSTR title, DWORD style, CRect & rect, CWnd * pParent, UINT nID, APropertyObject * pData, AProperty * pProperty);
	// Generated message map functions
protected:
	afx_msg LRESULT OnGetFont(WPARAM wParam, LPARAM lParam);
	CFont		m_gdi_font;

	//{{AFX_MSG(CPropertyBox)
	afx_msg void OnPaint();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PROPERTYBOX_H__0FF76CA7_E9FD_44A0_9D0E_6E75E1DC20FA__INCLUDED_)
