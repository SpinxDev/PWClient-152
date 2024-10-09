#if !defined(AFX_AWARDBYITEMSDLG_H__F02E8549_BD07_4756_AE78_A1832934D229__INCLUDED_)
#define AFX_AWARDBYITEMSDLG_H__F02E8549_BD07_4756_AE78_A1832934D229__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AwardByItemsDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CAwardByItemsDlg dialog

struct AWARD_ITEMS_SCALE;
class CAwardByItemsDlg : public CDialog
{
// Construction
public:
	CAwardByItemsDlg(AWARD_ITEMS_SCALE* pAward, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CAwardByItemsDlg)
	enum { IDD = IDD_AWARD_BY_ITEMS };
	CListCtrl	m_Award;
	CString	m_strItem;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAwardByItemsDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	
	AWARD_ITEMS_SCALE* m_pAward;
	void UpdateList();

	// Generated message map functions
	//{{AFX_MSG(CAwardByItemsDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnAdd();
	afx_msg void OnDel();
	afx_msg void OnEditAward();
	afx_msg void OnEditNum();
	afx_msg void OnEditItem();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_AWARDBYITEMSDLG_H__F02E8549_BD07_4756_AE78_A1832934D229__INCLUDED_)
