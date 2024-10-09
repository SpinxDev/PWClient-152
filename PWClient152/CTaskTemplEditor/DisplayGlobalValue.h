#if !defined(AFX_DISPLAYGLOBALVALUE_H__00986078_0B94_4FE7_B6AF_ED8B1446442C__INCLUDED_)
#define AFX_DISPLAYGLOBALVALUE_H__00986078_0B94_4FE7_B6AF_ED8B1446442C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DisplayGlobalValue.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDisplayGlobalValue dialog
#include "TaskTempl.h"
#include "EditList.h"

class CDisplayGlobalValue : public CDialog
{
// Construction
public:
	CDisplayGlobalValue(ATaskTempl* pTempl, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDisplayGlobalValue)
	enum { IDD = IDD_DISPLAY_GLOBAL_VALUE };
	CEditList	m_StringList;
	CEditList	m_ValueList;
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDisplayGlobalValue)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	ATaskTempl* m_pTempl;
	unsigned long m_ulExpCnt;
	char m_szExp[TASK_AWARD_MAX_DISPLAY_EXP_CNT][TASK_AWARD_MAX_DISPLAY_CHAR_LEN];
	unsigned long m_ulTaskCharCnt;
	task_char m_TaskChar[TASK_AWARD_MAX_DISPLAY_EXP_CNT][TASK_AWARD_MAX_DISPLAY_CHAR_LEN];
	// Generated message map functions
	//{{AFX_MSG(CDisplayGlobalValue)
	afx_msg void OnAdd();
	afx_msg void OnDel();
	afx_msg void OnAdd2();
	afx_msg void OnDel2();
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg void OnClickAwardDisplayValue(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnClickAwardDisplayString(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DISPLAYGLOBALVALUE_H__00986078_0B94_4FE7_B6AF_ED8B1446442C__INCLUDED_)
