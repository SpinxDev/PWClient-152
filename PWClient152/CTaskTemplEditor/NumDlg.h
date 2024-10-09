#if !defined(AFX_NUMDLG_H__116D56B7_31D6_4D03_BF7B_91783B7251DD__INCLUDED_)
#define AFX_NUMDLG_H__116D56B7_31D6_4D03_BF7B_91783B7251DD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// NumDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CNumDlg dialog

class CNumDlg : public CDialog
{
// Construction
public:
	CNumDlg(LPCTSTR szTitle = NULL, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CNumDlg)
	enum { IDD = IDD_NUMBER };
	int		m_nNum;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CNumDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	LPCTSTR m_szTitle;

	// Generated message map functions
	//{{AFX_MSG(CNumDlg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_NUMDLG_H__116D56B7_31D6_4D03_BF7B_91783B7251DD__INCLUDED_)
