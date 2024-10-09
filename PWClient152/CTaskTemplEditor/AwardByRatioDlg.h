#if !defined(AFX_AWARDBYRATIODLG_H__0100FA20_DF3E_4ED6_B93C_85F6925C21E9__INCLUDED_)
#define AFX_AWARDBYRATIODLG_H__0100FA20_DF3E_4ED6_B93C_85F6925C21E9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AwardByRatioDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CAwardByRatioDlg dialog

struct AWARD_RATIO_SCALE;
class CAwardByRatioDlg : public CDialog
{
// Construction
public:
	CAwardByRatioDlg(AWARD_RATIO_SCALE* pAward, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CAwardByRatioDlg)
	enum { IDD = IDD_AWARD_BY_RATIO };
	CListCtrl	m_Award;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAwardByRatioDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	AWARD_RATIO_SCALE* m_pAward;

	void UpdateList();

	// Generated message map functions
	//{{AFX_MSG(CAwardByRatioDlg)
	afx_msg void OnAdd();
	afx_msg void OnDel();
	afx_msg void OnEditAward();
	afx_msg void OnEditRatio();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_AWARDBYRATIODLG_H__0100FA20_DF3E_4ED6_B93C_85F6925C21E9__INCLUDED_)
