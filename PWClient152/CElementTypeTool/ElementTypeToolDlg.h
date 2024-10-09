// ElementTypeToolDlg.h : header file
//

#if !defined(AFX_ELEMENTTYPETOOLDLG_H__9AAF833B_B027_42F3_A204_E136DEE53F80__INCLUDED_)
#define AFX_ELEMENTTYPETOOLDLG_H__9AAF833B_B027_42F3_A204_E136DEE53F80__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CElementTypeToolDlg dialog

class CElementTypeToolDlg : public CDialog
{
// Construction
public:
	CElementTypeToolDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CElementTypeToolDlg)
	enum { IDD = IDD_ELEMENTTYPETOOL_DIALOG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CElementTypeToolDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CElementTypeToolDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnButton1();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ELEMENTTYPETOOLDLG_H__9AAF833B_B027_42F3_A204_E136DEE53F80__INCLUDED_)
