#if !defined(AFX_MEMOEDITDLG_H__C4A87978_10AC_40A2_A85D_FB84A8BE038B__INCLUDED_)
#define AFX_MEMOEDITDLG_H__C4A87978_10AC_40A2_A85D_FB84A8BE038B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MemoEditDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CMemoEditDlg dialog

class CMemoEditDlg : public CDialog
{
// Construction
public:
	CMemoEditDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CMemoEditDlg)
	enum { IDD = IDD_MEMOEDIT_DIALOG };
	CString	m_Memo;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMemoEditDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CMemoEditDlg)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MEMOEDITDLG_H__C4A87978_10AC_40A2_A85D_FB84A8BE038B__INCLUDED_)
