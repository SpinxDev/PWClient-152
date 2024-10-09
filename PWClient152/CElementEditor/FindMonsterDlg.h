#if !defined(AFX_FINDMONSTERDLG_H__0477CEC4_77FA_4C66_8235_5440503CEC24__INCLUDED_)
#define AFX_FINDMONSTERDLG_H__0477CEC4_77FA_4C66_8235_5440503CEC24__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// FindMonsterDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CFindMonsterDlg dialog

class CFindMonsterDlg : public CDialog
{
// Construction
public:
	CFindMonsterDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CFindMonsterDlg)
	enum { IDD = IDD_MONSTER_FINDER };
	CListBox	m_listMonster;
	CString	m_strKey;
	BOOL	m_bExa;
	//}}AFX_DATA
	int m_nType;
	CString m_strSelectedMonster;
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFindMonsterDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL
	
// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CFindMonsterDlg)
	afx_msg void OnButtonFind();
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FINDMONSTERDLG_H__0477CEC4_77FA_4C66_8235_5440503CEC24__INCLUDED_)
