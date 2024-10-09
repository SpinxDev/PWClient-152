#if !defined(AFX_DLGCOPYSEL_H__F149AA6C_3E74_4B76_B1EC_8E9FF001D5A4__INCLUDED_)
#define AFX_DLGCOPYSEL_H__F149AA6C_3E74_4B76_B1EC_8E9FF001D5A4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgCopySel.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgCopySel dialog

class CDlgCopySel : public CDialog
{
// Construction
public:
	CDlgCopySel(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgCopySel)
	enum { IDD = IDD_DIALOG_EXP_COPY };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

	
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgCopySel)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnOK();
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CDlgCopySel)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGCOPYSEL_H__F149AA6C_3E74_4B76_B1EC_8E9FF001D5A4__INCLUDED_)
