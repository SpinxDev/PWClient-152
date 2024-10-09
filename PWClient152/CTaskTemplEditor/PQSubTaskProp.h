#if !defined(AFX_PQSUBTASKPROP_H__52F5568B_6D56_4740_807F_AE3BCB80860C__INCLUDED_)
#define AFX_PQSUBTASKPROP_H__52F5568B_6D56_4740_807F_AE3BCB80860C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PQSubTaskProp.h : header file
//
#include "TaskTempl.h"

/////////////////////////////////////////////////////////////////////////////
// CPQSubTaskProp dialog

class CPQSubTaskProp : public CDialog
{
// Construction
public:
	CPQSubTaskProp(ATaskTempl* pTempl, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CPQSubTaskProp)
	enum { IDD = IDD_PQ_SUBTASK_PROP };
	CListCtrl	m_MonsterContribList;
	BOOL	m_bClearContrib;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPQSubTaskProp)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	ATaskTempl* m_pTempl;
	unsigned long m_ulContribMonsterNum;
	MONSTERS_CONTRIB* m_pMonsters;

	// Generated message map functions
	//{{AFX_MSG(CPQSubTaskProp)
	afx_msg void OnAdd();
	virtual BOOL OnInitDialog();
	afx_msg void OnDel();
	afx_msg void OnEdit();
	afx_msg void OnClose();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PQSUBTASKPROP_H__52F5568B_6D56_4740_807F_AE3BCB80860C__INCLUDED_)
