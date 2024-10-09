#if !defined(AFX_PRECINCTIMPDLG_H__1BF42392_C25B_4FE0_B207_0FD7C6B2FB98__INCLUDED_)
#define AFX_PRECINCTIMPDLG_H__1BF42392_C25B_4FE0_B207_0FD7C6B2FB98__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PrecinctImpDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CPrecinctImpDlg dialog

class CPrecinctImpDlg : public CDialog
{
// Construction
public:
	CPrecinctImpDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CPrecinctImpDlg)
	enum { IDD = IDD_PRECINCT_NPC_IMPORT };
	CString	m_strName;
	float	m_fX;
	float	m_fY;
	float	m_fZ;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPrecinctImpDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnOK();
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CPrecinctImpDlg)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PRECINCTIMPDLG_H__1BF42392_C25B_4FE0_B207_0FD7C6B2FB98__INCLUDED_)
