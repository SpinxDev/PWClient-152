#if !defined(AFX_EXPPATHSELDLG_H__93793C61_D695_4EF4_840D_2535DF4F579E__INCLUDED_)
#define AFX_EXPPATHSELDLG_H__93793C61_D695_4EF4_840D_2535DF4F579E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ExpPathSelDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CExpPathSelDlg dialog

class CExpPathSelDlg : public CDialog
{
// Construction
public:
	CExpPathSelDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CExpPathSelDlg)
	enum { IDD = IDD_DIALOG_EXP_PATH_SEL };
	CString	m_strPath;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CExpPathSelDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnOK();
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CExpPathSelDlg)
	afx_msg void OnButtonBrowse();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_EXPPATHSELDLG_H__93793C61_D695_4EF4_840D_2535DF4F579E__INCLUDED_)
