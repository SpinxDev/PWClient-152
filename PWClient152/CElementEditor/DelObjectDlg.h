#if !defined(AFX_DELOBJECTDLG_H__E11A8C96_F50F_46B6_972C_4B9CA5F5815D__INCLUDED_)
#define AFX_DELOBJECTDLG_H__E11A8C96_F50F_46B6_972C_4B9CA5F5815D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DelObjectDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDelObjectDlg dialog

class CDelObjectDlg : public CDialog
{
// Construction
public:
	CDelObjectDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDelObjectDlg)
	enum { IDD = IDD_DIALOG_DEL_LIST };
	CListBox	m_listObj;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDelObjectDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDelObjectDlg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DELOBJECTDLG_H__E11A8C96_F50F_46B6_972C_4B9CA5F5815D__INCLUDED_)
