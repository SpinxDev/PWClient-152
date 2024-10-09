#if !defined(AFX_MONWANTEDDLG_H__6E51E986_58C8_4E55_BB43_FDD6ABA46F04__INCLUDED_)
#define AFX_MONWANTEDDLG_H__6E51E986_58C8_4E55_BB43_FDD6ABA46F04__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MonWantedDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CMonWantedDlg dialog

struct MONSTER_WANTED;
class CMonWantedDlg : public CDialog
{
// Construction
public:
	CMonWantedDlg(MONSTER_WANTED* pMonsters, unsigned long* pCount, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CMonWantedDlg)
	enum { IDD = IDD_MONSTER_WANTED };
	CListCtrl	m_Monsters;
	//}}AFX_DATA

	MONSTER_WANTED* m_pMonsters;
	unsigned long* m_pCount;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMonWantedDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	void UpdateList();

	// Generated message map functions
	//{{AFX_MSG(CMonWantedDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnAdd();
	afx_msg void OnDel();
	afx_msg void OnEditMonster();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MONWANTEDDLG_H__6E51E986_58C8_4E55_BB43_FDD6ABA46F04__INCLUDED_)
