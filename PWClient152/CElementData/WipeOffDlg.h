#if !defined(AFX_WIPEOFFDLG_H__B0BEA904_6159_446B_B438_A99ADE5BA0DD__INCLUDED_)
#define AFX_WIPEOFFDLG_H__B0BEA904_6159_446B_B438_A99ADE5BA0DD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// WipeOffDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CWipeOffDlg dialog

class CWipeOffDlg : public CDialog
{
// Construction
public:
	CWipeOffDlg(CStringArray* pArray, CUIntArray* pIDArray, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CWipeOffDlg)
	enum { IDD = IDD_WIPE_OFF };
	CListCtrl	m_WipeOffLst;
	//}}AFX_DATA

	CStringArray* m_pWipeOffArray;
	CUIntArray* m_pIDArray;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWipeOffDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CWipeOffDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WIPEOFFDLG_H__B0BEA904_6159_446B_B438_A99ADE5BA0DD__INCLUDED_)
