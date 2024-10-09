#if !defined(AFX_MONPROPDLG_H__C130E06F_825E_4636_B395_6737BAB958BB__INCLUDED_)
#define AFX_MONPROPDLG_H__C130E06F_825E_4636_B395_6737BAB958BB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MonPropDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CMonPropDlg dialog

struct MONSTER_WANTED;
class CMonPropDlg : public CDialog
{
// Construction
public:
	CMonPropDlg(MONSTER_WANTED* pMonster, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CMonPropDlg)
	enum { IDD = IDD_MONSTER };
	CString	m_strItem;
	DWORD	m_dwDropNum;
	float	m_fDropProb;
	CString	m_strMonster;
	DWORD	m_dwMonsterNum;
	BOOL	m_bKillerLev;
	int		m_iDPS;
	int		m_iDPH;
	//}}AFX_DATA

	MONSTER_WANTED* m_pMonster;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMonPropDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CMonPropDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelItem();
	afx_msg void OnSelMonster();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MONPROPDLG_H__C130E06F_825E_4636_B395_6737BAB958BB__INCLUDED_)
