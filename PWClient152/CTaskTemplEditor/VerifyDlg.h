#if !defined(AFX_VERIFYDLG_H__21D0AC2A_6523_4A51_8A24_D594DC3589EC__INCLUDED_)
#define AFX_VERIFYDLG_H__21D0AC2A_6523_4A51_8A24_D594DC3589EC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// VerifyDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CVerifyDlg dialog

class CVerifyDlg : public CDialog
{
// Construction
public:
	CVerifyDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CVerifyDlg)
	enum { IDD = IDD_VERIFY };
	CListCtrl	m_Report;
	CString	m_strDescript;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CVerifyDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	void wrong_npc_check(bool bDelv);

public:
	void DeleteAllCol();

	// Generated message map functions
	//{{AFX_MSG(CVerifyDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnNoDeliverTalk();
	afx_msg void OnDestroy();
	afx_msg void OnDupMonster();
	afx_msg void OnWrongNpc();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VERIFYDLG_H__21D0AC2A_6523_4A51_8A24_D594DC3589EC__INCLUDED_)
