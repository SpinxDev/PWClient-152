#if !defined(AFX_LOGDLG_H__9A0A29E8_5410_4EF1_9B7E_29B98A603CC3__INCLUDED_)
#define AFX_LOGDLG_H__9A0A29E8_5410_4EF1_9B7E_29B98A603CC3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// LogDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CLogDlg dialog

class CLogDlg : public CDialog
{
// Construction
public:
	CLogDlg(CWnd* pParent = NULL);   // standard constructor
	void Log(CString str);
	void Clear();

// Dialog Data
	//{{AFX_DATA(CLogDlg)
	enum { IDD = IDD_LOG_DLG };
	CString	m_strEditor;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLogDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	CString m_strMsg;
	
	// Generated message map functions
	//{{AFX_MSG(CLogDlg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LOGDLG_H__9A0A29E8_5410_4EF1_9B7E_29B98A603CC3__INCLUDED_)
