#if !defined(AFX_LOGODLG_H__BB9910A6_DCCB_47D2_924E_C82FF9F69249__INCLUDED_)
#define AFX_LOGODLG_H__BB9910A6_DCCB_47D2_924E_C82FF9F69249__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// LogoDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CLogoDlg dialog

class CLogoDlg : public CDialog
{
// Construction
public:
	CLogoDlg(CWnd* pParent = NULL);   // standard constructor
	void UpdateFiles();
// Dialog Data
	//{{AFX_DATA(CLogoDlg)
	enum { IDD = IDD_DIALOG_LOGO };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLogoDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual void OnOK(){};
	virtual void OnCancel(){};
	// Generated message map functions
	//{{AFX_MSG(CLogoDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LOGODLG_H__BB9910A6_DCCB_47D2_924E_C82FF9F69249__INCLUDED_)
