#if !defined(AFX_PROPERTYDIALOG_H__F1DC9FB5_EEBF_4C2B_85C6_1CFD7EBA2154__INCLUDED_)
#define AFX_PROPERTYDIALOG_H__F1DC9FB5_EEBF_4C2B_85C6_1CFD7EBA2154__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PropertyDialog.h : header file
//

#include "A3DEngine.h"

/////////////////////////////////////////////////////////////////////////////
// CPropertyDialog dialog

class CPropertyList;
class APropertyObject;
class CPropertyDialog : public CDialog
{
// Construction
public:
	CPropertyDialog(A3DEngine* pEngine, APropertyObject * pData,CWnd* pParent = NULL);   // standard constructor
	
	CPropertyList * m_pList;
	APropertyObject * m_pData;
// Dialog Data
	//{{AFX_DATA(CPropertyDialog)
	enum { IDD = IDD_PROPERTY_DIALOG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPropertyDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CPropertyDialog)
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PROPERTYDIALOG_H__F1DC9FB5_EEBF_4C2B_85C6_1CFD7EBA2154__INCLUDED_)
