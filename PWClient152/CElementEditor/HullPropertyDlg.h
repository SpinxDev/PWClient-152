#if !defined(AFX_HULLPROPERTYDLG_H__CB8A4E93_FEB1_4DA8_A93D_C5CB6710A8B5__INCLUDED_)
#define AFX_HULLPROPERTYDLG_H__CB8A4E93_FEB1_4DA8_A93D_C5CB6710A8B5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// HullPropertyDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CHullPropertyDlg dialog

class CHullPropertyDlg : public CDialog
{
// Construction
public:
	CHullPropertyDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CHullPropertyDlg)
	enum { IDD = IDD_DIALOG_HULL_PROPERTY };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA
	bool m_bC;
	bool m_bT;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CHullPropertyDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CHullPropertyDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnRadioSkipCo();
	afx_msg void OnRadioSkipTa();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_HULLPROPERTYDLG_H__CB8A4E93_FEB1_4DA8_A93D_C5CB6710A8B5__INCLUDED_)
