#if !defined(AFX_TASKAREADLG_H__12153F8C_93C1_4842_B3B3_58F1F207AE08__INCLUDED_)
#define AFX_TASKAREADLG_H__12153F8C_93C1_4842_B3B3_58F1F207AE08__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TaskAreaDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CTaskAreaDlg dialog

class CTaskAreaDlg : public CDialog
{
// Construction
public:
	CTaskAreaDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CTaskAreaDlg)
	enum { IDD = IDD_AREA };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTaskAreaDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CTaskAreaDlg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TASKAREADLG_H__12153F8C_93C1_4842_B3B3_58F1F207AE08__INCLUDED_)
