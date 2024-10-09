#if !defined(AFX_INPUTSUBTYPENAMEDLG_H__B1481CB4_6170_450E_BDF9_4635CE659FFF__INCLUDED_)
#define AFX_INPUTSUBTYPENAMEDLG_H__B1481CB4_6170_450E_BDF9_4635CE659FFF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// InputSubTypeNameDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CInputSubTypeNameDlg dialog

class CInputSubTypeNameDlg : public CDialog
{
// Construction
public:
	CInputSubTypeNameDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CInputSubTypeNameDlg)
	enum { IDD = IDD_DIALOG_INPUT_SUBTYPE };
	CString	m_strSubTypeName;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CInputSubTypeNameDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CInputSubTypeNameDlg)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_INPUTSUBTYPENAMEDLG_H__B1481CB4_6170_450E_BDF9_4635CE659FFF__INCLUDED_)
