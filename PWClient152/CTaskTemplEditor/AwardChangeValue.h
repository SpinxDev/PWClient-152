#if !defined(AFX_AWARDCHANGEVALUE_H__CA2EEDEC_7A9E_4EC2_BC66_303663B11CFA__INCLUDED_)
#define AFX_AWARDCHANGEVALUE_H__CA2EEDEC_7A9E_4EC2_BC66_303663B11CFA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AwardChangeValue.h : header file
//
#include "TaskTempl.h"
#include "EditList.h"
/////////////////////////////////////////////////////////////////////////////
// CAwardChangeValue dialog

class CAwardChangeValue : public CDialog
{
// Construction
public:
	CAwardChangeValue(unsigned long* pulChangeKeyCnt, long **pplChangeKey, long **pplChangeKeyValue, bool **ppbChangeType, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CAwardChangeValue)
	enum { IDD = IDD_AWARD_CHANGE_VALUE };
		// NOTE: the ClassWizard will add data members here
	CEditList	m_ValueList;
	CStringList m_strList;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAwardChangeValue)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	unsigned long *m_pulChangeKeyCnt;
	long **m_pplChangeKey;
	long **m_pplChangeKeyValue;
	bool **m_ppbChangeType;

	unsigned long m_ulChangeKeyCnt;
	long m_lChangeKey[TASK_AWARD_MAX_CHANGE_VALUE];
	long m_lChangeKeyValue[TASK_AWARD_MAX_CHANGE_VALUE];
	bool m_bChangeType[TASK_AWARD_MAX_CHANGE_VALUE];
	// Generated message map functions
	//{{AFX_MSG(CAwardChangeValue)
	virtual BOOL OnInitDialog();
	afx_msg void OnAdd();
	afx_msg void OnDel();
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnClickAwardChangeValue(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_AWARDCHANGEVALUE_H__CA2EEDEC_7A9E_4EC2_BC66_303663B11CFA__INCLUDED_)
