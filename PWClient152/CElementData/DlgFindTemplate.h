#if !defined(AFX_DLGFINDTEMPLDLG_H__271F9429_74E8_49A6_A69D_F235A6113455__INCLUDED_)
#define AFX_DLGFINDTEMPLDLG_H__271F9429_74E8_49A6_A69D_F235A6113455__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgFindTemplate.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgFindTemplate dialog

class CDlgFindTemplate : public CDialog
{
// Construction
public:
	CDlgFindTemplate(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgFindTemplate)
	enum { IDD = IDD_FIND_TEMPLATE };
	CComboBox	m_comboType;
	CEdit	m_editText;
	CString	m_strText;
	//}}AFX_DATA

	static CString m_strLast;

	enum	FIND_TYPE
	{
		FT_TEMPL = 0,
		FT_EXTDATA,
		FT_RECIPE,
		FT_CONFIG,
		FT_FACE,
		FT_NUM,
	};
	static FIND_TYPE	m_typeLast;


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgFindTemplate)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgFindTemplate)
	afx_msg void OnFind();
	virtual BOOL OnInitDialog();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnButton1();
	//}}AFX_MSG

	virtual void OnCancel();

	void	RememberLast();
	void	RestoreLast();
	
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGFINDTEMPLDLG_H__271F9429_74E8_49A6_A69D_F235A6113455__INCLUDED_)
