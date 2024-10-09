#if !defined(AFX_MOUSTERSTATFILTERDLG_H__F351541A_8EB8_4142_AEB7_1DB085BF68AC__INCLUDED_)
#define AFX_MOUSTERSTATFILTERDLG_H__F351541A_8EB8_4142_AEB7_1DB085BF68AC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MousterStatFilterDlg.h : header file
//
#include <vector.h>
/////////////////////////////////////////////////////////////////////////////
// CMousterStatFilterDlg dialog

class CMousterStatFilterDlg : public CDialog
{
// Construction
public:
	CMousterStatFilterDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CMousterStatFilterDlg)
	enum { IDD = IDD_DIALOG_STAT_FILTER };
	CComboBox	m_comboSet;
	CTreeCtrl	m_treeFilter;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMousterStatFilterDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnOK();
	//}}AFX_VIRTUAL

// Implementation
protected:
	void GetMapController(abase::vector<int> &listController);
	void AddMapController(abase::vector<int> &listController,int id);
	bool FindContoller(abase::vector<int> &listController,int id);
	// Generated message map functions
	//{{AFX_MSG(CMousterStatFilterDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeComboSet();
	afx_msg void OnSelectall();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MOUSTERSTATFILTERDLG_H__F351541A_8EB8_4142_AEB7_1DB085BF68AC__INCLUDED_)
