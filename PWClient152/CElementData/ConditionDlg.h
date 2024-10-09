#if !defined(AFX_CONDITIONDLG_H__893E4200_D466_473E_A200_8EAF35F9F23D__INCLUDED_)
#define AFX_CONDITIONDLG_H__893E4200_D466_473E_A200_8EAF35F9F23D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ConditionDlg.h : header file
//

#include "Policy.h"

/////////////////////////////////////////////////////////////////////////////
// CConditionDlg dialog

class CConditionDlg : public CDialog
{
// Construction
public:
	CConditionDlg(CWnd* pParent = NULL);   // standard constructor
	unsigned int m_uConditionFunc;
	CTriggerData::_s_tree_item *m_pRoot;
	bool bModify;
// Dialog Data
	//{{AFX_DATA(CConditionDlg)
	enum { IDD = IDD_DIALOG_CONDITION };
	CComboBox	m_cbFunc;
	float	m_fConditionParam;
	int		m_iConditionParam;
	CString	m_strLeft;
	CString	m_strRight;
	//}}AFX_DATA
	bool m_bIsRoot;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CConditionDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnOK();
	virtual void OnCancel();
	//}}AFX_VIRTUAL

// Implementation
protected:
	void DisableConditon2(bool bDisable = true);
	void FreshInterface();

	bool IsCondition(CTriggerData::_e_condition c)const;
	void ShowIntParam(int valueOnShow);
	void HideIntParam();
	void ShowFloatParam(float valueOnShow);
	void HideFloatParam();

	void FreeTree();
	void AllocParameter(size_t size);

	CString TraverselTree( void *pTree);
	bool Check(CTriggerData::_s_tree_item* left, CTriggerData::_s_tree_item* right);
	// Generated message map functions
	//{{AFX_MSG(CConditionDlg)
	afx_msg void OnSelchangeComboFunc();
	virtual BOOL OnInitDialog();
	afx_msg void OnButtonSub1();
	afx_msg void OnButtonSub2();
	afx_msg void OnChangeEditParamFloat();
	afx_msg void OnChangeEditParamInt();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CONDITIONDLG_H__893E4200_D466_473E_A200_8EAF35F9F23D__INCLUDED_)
