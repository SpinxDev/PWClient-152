#if !defined(AFX_DATETYPEDLG_H__48351891_47CF_4C2F_BBCE_206D82CC5128__INCLUDED_)
#define AFX_DATETYPEDLG_H__48351891_47CF_4C2F_BBCE_206D82CC5128__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DateTypeDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDateTypeDlg dialog

class CDateTypeDlg : public CDialog
{
// Construction
public:
	CDateTypeDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDateTypeDlg)
	enum { IDD = IDD_DATE_TYPE };
	int		m_nDateType;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDateTypeDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDateTypeDlg)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DATETYPEDLG_H__48351891_47CF_4C2F_BBCE_206D82CC5128__INCLUDED_)
