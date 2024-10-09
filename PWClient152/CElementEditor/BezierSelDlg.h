#if !defined(AFX_BEZIERSELDLG_H__D16B54D9_CDF3_4ACB_8E64_16714E10FAFC__INCLUDED_)
#define AFX_BEZIERSELDLG_H__D16B54D9_CDF3_4ACB_8E64_16714E10FAFC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// BezierSelDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CBezierSelDlg dialog

class CBezierSelDlg : public CDialog
{
// Construction
public:
	CBezierSelDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CBezierSelDlg)
	enum { IDD = IDD_BEZIER_ID_SEL };
	CListBox	m_listBezier;
	//}}AFX_DATA
	CString m_strBezierName;
	int   m_nBezierID;
	int   m_nObjectType;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBezierSelDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	void FreshList();
	// Generated message map functions
	//{{AFX_MSG(CBezierSelDlg)
	afx_msg void OnSelchangeListBezier();
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg void OnButtonNull();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BEZIERSELDLG_H__D16B54D9_CDF3_4ACB_8E64_16714E10FAFC__INCLUDED_)
