#if !defined(AFX_TIMESETDLG_H__C6125D60_C7DE_4DBC_82DC_3038F71A2129__INCLUDED_)
#define AFX_TIMESETDLG_H__C6125D60_C7DE_4DBC_82DC_3038F71A2129__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TimeSetDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CTimeSetDlg dialog

class CTimeSetDlg : public CDialog
{
// Construction
public:
	CTimeSetDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CTimeSetDlg)
	enum { IDD = IDD_DIALOG_TIME };
	UINT	m_nDay;
	UINT	m_nHours;
	UINT	m_nMinu;
	UINT	m_nMouth;
	UINT	m_nSec;
	UINT	m_nYear;
	//}}AFX_DATA
	CTime m_Time;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTimeSetDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnOK();
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CTimeSetDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSetToNull();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TIMESETDLG_H__C6125D60_C7DE_4DBC_82DC_3038F71A2129__INCLUDED_)
