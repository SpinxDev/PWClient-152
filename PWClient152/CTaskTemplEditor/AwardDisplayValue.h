#if !defined(AFX_AWARDDISPLAYVALUE_H__3D8F1CA2_53C2_4A41_A5D3_CB087AC2E20B__INCLUDED_)
#define AFX_AWARDDISPLAYVALUE_H__3D8F1CA2_53C2_4A41_A5D3_CB087AC2E20B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AwardDisplayValue.h : header file
//
#include "TaskTempl.h"
#include "EditList.h"
/////////////////////////////////////////////////////////////////////////////
// CAwardDisplayValue dialog

class CAwardDisplayValue : public CDialog
{
// Construction
public:
	CAwardDisplayValue(AWARD_DATA* pAward, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CAwardDisplayValue)
	enum { IDD = IDD_AWARD_DISPLAY_VALUE };
	CEditList	m_StringList;
	CEditList	m_ValueList;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAwardDisplayValue)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	AWARD_DATA* m_pAward;
	unsigned long m_ulExpCnt;
	char m_szExp[TASK_AWARD_MAX_DISPLAY_EXP_CNT][TASK_AWARD_MAX_DISPLAY_CHAR_LEN];
	unsigned long m_ulTaskCharCnt;
	task_char m_TaskChar[TASK_AWARD_MAX_DISPLAY_EXP_CNT][TASK_AWARD_MAX_DISPLAY_CHAR_LEN];
// 	unsigned long *m_pulDisplayKeyCnt;
// 	long **m_pplDisplayKey;
// 	unsigned long m_ulDisplayKeyCnt;
// 	long m_lDisplayKey[TASK_AWARD_MAX_DISPLAY_VALUE];
	// Generated message map functions
	//{{AFX_MSG(CAwardDisplayValue)
	afx_msg void OnAdd();
	afx_msg void OnDel();
	virtual void OnOK();
	virtual void OnCancel();
	virtual BOOL OnInitDialog();
	afx_msg void OnClickAwardDisplayValue(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnAdd2();
	afx_msg void OnDel2();
	afx_msg void OnClickAwardDisplayString(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_AWARDDISPLAYVALUE_H__3D8F1CA2_53C2_4A41_A5D3_CB087AC2E20B__INCLUDED_)
