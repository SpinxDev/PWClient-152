#if !defined(AFX_TREETYPESADDDLG_H__E42591E0_FDF3_42B7_A380_1BB945FF01FA__INCLUDED_)
#define AFX_TREETYPESADDDLG_H__E42591E0_FDF3_42B7_A380_1BB945FF01FA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TreeTypesAddDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CTreeTypesAddDlg dialog

class CTreeTypesAddDlg : public CDialog
{
// Construction
public:
	CTreeTypesAddDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CTreeTypesAddDlg)
	enum { IDD = IDD_TREE_TYPE_ADD };
	CString	m_strSpeedTreeName;
	CString	m_strDDSName;
	CString	m_strTreeName;
	//}}AFX_DATA

	bool m_bIsChange;
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTreeTypesAddDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual void OnOK();
	// Generated message map functions
	//{{AFX_MSG(CTreeTypesAddDlg)
	afx_msg void OnButtonSpeedtree();
	afx_msg void OnButtonDds();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TREETYPESADDDLG_H__E42591E0_FDF3_42B7_A380_1BB945FF01FA__INCLUDED_)
