#if !defined(AFX_MONSTERSUMMONEDDLG_H__3D2A3D75_0485_4339_B83F_6B9CA0BF1B8A__INCLUDED_)
#define AFX_MONSTERSUMMONEDDLG_H__3D2A3D75_0485_4339_B83F_6B9CA0BF1B8A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MonsterSummonedDlg.h : header file
//
//	Created by Sun Xuewei 2009-06-04
//

#include "TaskTempl.h"

/////////////////////////////////////////////////////////////////////////////
// CMonsterSummonedDlg dialog

class CMonsterSummonedDlg : public CDialog
{
// Construction
public:
// 	CMonsterSummonedDlg(unsigned long ulMaxCount, 
// 						MONSTERS_SUMMONED* pMonsters, 
// 						unsigned long* pCount, 
//						CWnd* pParent = NULL);   // standard constructor

	CMonsterSummonedDlg(AWARD_DATA* pAward, CWnd* pParent = NULL);

	unsigned long m_ulMaxCount;
	MONSTERS_SUMMONED* m_pMonsters;
	unsigned long* m_pCount;



// Dialog Data
	//{{AFX_DATA(CMonsterSummonedDlg)
	enum { IDD = IDD_AWARD_MONSTER_SUMMONED };
	CListCtrl	m_MonstersLst;
	BOOL	m_bRandSel;
	BOOL	m_bDisappear;
	int		m_iRadius;
	float	m_fTotalProb;
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMonsterSummonedDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	AWARD_DATA* m_pAward;

	// Generated message map functions
	//{{AFX_MSG(CMonsterSummonedDlg)
	afx_msg void OnAdd();
	afx_msg void OnDel();
	afx_msg void OnEditNum();
	afx_msg void OnRandSel();
	afx_msg void OnProb();
	afx_msg void OnPeriod();
	afx_msg void OnDisappear();
	virtual BOOL OnInitDialog();
	afx_msg void OnClose();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MONSTERSUMMONEDDLG_H__3D2A3D75_0485_4339_B83F_6B9CA0BF1B8A__INCLUDED_)
