#if !defined(AFX_MOUNSTERFALLELEDLG_H__4ED4E72F_E860_4A32_AF0B_B237D8496A6C__INCLUDED_)
#define AFX_MOUNSTERFALLELEDLG_H__4ED4E72F_E860_4A32_AF0B_B237D8496A6C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MounsterFallEleDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CMounsterFallEleDlg dialog

class CMounsterFallEleDlg : public CDialog
{
// Construction
public:
	CMounsterFallEleDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CMounsterFallEleDlg)
	enum { IDD = IDD_DIALOG_MOUNSTER_ELEMENT };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMounsterFallEleDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CMounsterFallEleDlg)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MOUNSTERFALLELEDLG_H__4ED4E72F_E860_4A32_AF0B_B237D8496A6C__INCLUDED_)
