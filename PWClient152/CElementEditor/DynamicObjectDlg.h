#if !defined(AFX_DYNAMICOBJECTDLG_H__CA748155_9A39_4712_9BF2_2E17E3F142B2__INCLUDED_)
#define AFX_DYNAMICOBJECTDLG_H__CA748155_9A39_4712_9BF2_2E17E3F142B2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DynamicObjectDlg.h : header file
//

#include "DynamicObjectManDlg.h"

/////////////////////////////////////////////////////////////////////////////
// CDynamicObjectDlg dialog

class CDynamicObjectDlg : public CDialog
{
// Construction
public:
	CDynamicObjectDlg(CWnd* pParent = NULL);   // standard constructor
	DYNAMIC_OBJECT m_Object;
	bool m_bReadOnly;
// Dialog Data
	//{{AFX_DATA(CDynamicObjectDlg)
	enum { IDD = IDD_DIALOG_DYNAMIC_OBJECT };
	CString	m_strPath;
	CString	m_strID;
	CString	m_strName;
	//}}AFX_DATA
	

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDynamicObjectDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnOK();
	//}}AFX_VIRTUAL

// Implementation
protected:
	
	// Generated message map functions
	//{{AFX_MSG(CDynamicObjectDlg)
	afx_msg void OnButtonBrowse();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DYNAMICOBJECTDLG_H__CA748155_9A39_4712_9BF2_2E17E3F142B2__INCLUDED_)
