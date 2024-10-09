#if !defined(AFX_TASKAWARDPAGE_H__9CE2EF14_66AD_4FEB_80CE_DAD5F71AAD87__INCLUDED_)
#define AFX_TASKAWARDPAGE_H__9CE2EF14_66AD_4FEB_80CE_DAD5F71AAD87__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TaskAwardPage.h : header file
//

#include "TaskTempl.h"

/////////////////////////////////////////////////////////////////////////////
// CTaskAwardPage dialog

class CTaskAwardPage : public CPropertyPage
{
	DECLARE_DYNCREATE(CTaskAwardPage)

// Construction
public:
	CTaskAwardPage();
	~CTaskAwardPage();

protected:
	ATaskTempl*	m_pTask;
	BOOL m_bReadOnly;

public:
	BOOL UpdatePage(ATaskTempl* pTask, BOOL bUpdateData, BOOL bReadOnly = FALSE);

// Dialog Data
	//{{AFX_DATA(CTaskAwardPage)
	enum { IDD = IDD_TASK_AWARD };
	int		m_nAwardSel;
	int		m_nAwardType;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CTaskAwardPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CTaskAwardPage)
	afx_msg void OnEdit();
	afx_msg void OnAwardFailure();
	afx_msg void OnAwardSuccess();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TASKAWARDPAGE_H__9CE2EF14_66AD_4FEB_80CE_DAD5F71AAD87__INCLUDED_)
