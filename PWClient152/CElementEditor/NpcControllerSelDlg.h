#ifndef _NPC_CONTOROLLER_SEL_DLG_
#define _NPC_CONTOROLLER_SEL_DLG_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DynamicObjectSelDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDynamicObjectSelDlg dialog

class CNpcContollerSelDlg : public CDialog
{
// Construction
public:
	CNpcContollerSelDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDynamicObjectSelDlg)
	enum { IDD = IDD_DIALOG_DYNAMIC_OBJECT_SEL };
	CListBox	m_listObj;
	//}}AFX_DATA
	unsigned int m_nDyObjID;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDynamicObjectSelDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDynamicObjectSelDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnSetNull();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedBtnSearch();
	afx_msg void OnBnClickedBtnListAll();
protected:
	CString m_strSearchText;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DYNAMICOBJECTSELDLG_H__5A9EB046_42F7_4AD1_992B_16ADB4977566__INCLUDED_)
