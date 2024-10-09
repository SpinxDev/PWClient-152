#if !defined(AFX_PQGOBALVALUE_H__74797494_80CD_4D04_92B2_386D979BDFBC__INCLUDED_)
#define AFX_PQGOBALVALUE_H__74797494_80CD_4D04_92B2_386D979BDFBC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PQGobalValue.h : header file
//
#include "TaskTempl.h"
#include "EditList.h"

/////////////////////////////////////////////////////////////////////////////
// CPQGobalValue dialog

class CPQGobalValue : public CDialog
{
// Construction
public:
	CPQGobalValue(ATaskTempl* pTempl, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CPQGobalValue)
	enum { IDD = IDD_PQ_GLOBAL_VALUE };
	CEditList	m_PQValueList;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPQGobalValue)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	ATaskTempl* m_pTempl;
	unsigned long m_ulPQExpCnt;
	char m_szPQExp[TASK_AWARD_MAX_DISPLAY_EXP_CNT][TASK_AWARD_MAX_DISPLAY_CHAR_LEN];

	// Generated message map functions
	//{{AFX_MSG(CPQGobalValue)
	afx_msg void OnAdd();
	afx_msg void OnDel();
	virtual BOOL OnInitDialog();
	afx_msg void OnClickPQDisplayValue(NMHDR* pNMHDR, LRESULT* pResult);
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PQGOBALVALUE_H__74797494_80CD_4D04_92B2_386D979BDFBC__INCLUDED_)
