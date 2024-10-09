#if !defined(AFX_PRETASKDLG_H__83193BEE_BC50_450B_9C6E_DF8E54415CC4__INCLUDED_)
#define AFX_PRETASKDLG_H__83193BEE_BC50_450B_9C6E_DF8E54415CC4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PreTaskDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CPreTaskDlg dialog

class ATaskTempl;
class CPreTaskDlg : public CDialog
{
// Construction
public:
	CPreTaskDlg(ATaskTempl* pTempl, bool bPre, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CPreTaskDlg)
	enum { IDD = IDD_PRETASK };
	CListCtrl	m_PreTask;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPreTaskDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	ATaskTempl* m_pTempl;
	bool m_bPre;
	unsigned long m_ulAtLeastTaskNumFinished;

	void UpdateList();

	// Generated message map functions
	//{{AFX_MSG(CPreTaskDlg)
	afx_msg void OnAdd();
	afx_msg void OnDel();
	afx_msg void OnEdit();
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PRETASKDLG_H__83193BEE_BC50_450B_9C6E_DF8E54415CC4__INCLUDED_)
