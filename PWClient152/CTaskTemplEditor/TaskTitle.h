#if !defined(AFX_TASKTITLE_H__C3333BB6_E53F_413C_89B1_3DE1519F8717__INCLUDED_)
#define AFX_TASKTITLE_H__C3333BB6_E53F_413C_89B1_3DE1519F8717__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TaskTitle.h : header file
//
#include "TaskTempl.h"
#include "EditList.h"
/////////////////////////////////////////////////////////////////////////////
// CTaskTitle dialog

class CTaskTitle : public CDialog
{
// Construction
public:
	CTaskTitle(ATaskTempl* pTempl, int type, CWnd* pParent = NULL);   // standard constructor

	enum{
		TITLE_FOR_PREM,
		TITLE_FOR_AWARD_SUCCED,
		TITLE_FOR_AWARD_FAILED,
	};
// Dialog Data
	//{{AFX_DATA(CTaskTitle)
	enum { IDD = IDD_PREM_TITLE };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTaskTitle)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	CEditList	m_lstTitle;
	ATaskTempl* m_pTask;
	int			m_iType;
	unsigned long*	m_pCount;
	int*		m_pPremTitles;
	TITLE_AWARD* m_AwardTitles;
	CStringList m_strList;
	void UpdateList();
	// Generated message map functions
	//{{AFX_MSG(CTaskTitle)
	afx_msg void OnAdd();
	afx_msg void OnDel();
	virtual void OnCancel();
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg void OnPeriod();
	afx_msg void OnClickChangeDateType(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TASKTITLE_H__C3333BB6_E53F_413C_89B1_3DE1519F8717__INCLUDED_)
