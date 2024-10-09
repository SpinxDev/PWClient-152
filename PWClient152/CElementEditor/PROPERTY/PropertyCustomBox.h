// PropertyCustomBox.h: interface for the CPropertyCustomBox class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PROPERTYCUSTOMBOX_H__B28A6698_C69D_4636_BE83_F63E47D7F0F7__INCLUDED_)
#define AFX_PROPERTYCUSTOMBOX_H__B28A6698_C69D_4636_BE83_F63E47D7F0F7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "PropertyBox.h"
#include "AObject.h"

class CPropertyCustomBox : public CPropertyBox  
{
public:
	CPropertyCustomBox();
	virtual ~CPropertyCustomBox();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPropertyCustomBox)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	//}}AFX_VIRTUAL

// Implementation
public:

	virtual BOOL IsModified();
	virtual void DrawSelf(CDC * pDC);

protected:
	CButton			m_wndButton;
	BOOL			m_bModified;
	CString			m_strText;
	CUSTOM_FUNCS*	m_pCustomFuncs;

	//{{AFX_MSG(CPropertyCustomBox)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG
	afx_msg void OnBnClicked();
	DECLARE_MESSAGE_MAP()
};

#endif // !defined(AFX_PROPERTYCUSTOMBOX_H__B28A6698_C69D_4636_BE83_F63E47D7F0F7__INCLUDED_)
