#if !defined(AFX_POLICYOPERATIONDLG_H__D8CA3C64_1C3A_4072_9484_4FE7AE7312C1__INCLUDED_)
#define AFX_POLICYOPERATIONDLG_H__D8CA3C64_1C3A_4072_9484_4FE7AE7312C1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PolicyOperationDlg.h : header file
//
class CPropertyList;
class ADynPropertyObject;
class COperationParam;
#include "Policy.h"
/////////////////////////////////////////////////////////////////////////////
// CPolicyOperationDlg dialog

class CPolicyOperationDlg : public CDialog
{
// Construction
public:
	CPolicyOperationDlg(CWnd* pParent = NULL);   // standard constructor
	void UpdateProperty( ADynPropertyObject *pProObj );
	COperationParam *GetOperationModifier(){ return m_pOperationModifier; };
	void ReleaseOperation();

	CPolicyData *m_pPolicyData;
	bool m_bModify;
	CTriggerData::_s_operation *m_pOperation;// modify
// Dialog Data
	//{{AFX_DATA(CPolicyOperationDlg)
	enum { IDD = IDD_DIALOG_POLICY_OPERATION };
	CComboBox	m_cbTargetType;
	CComboBox	m_cbOperationType;
	BOOL	m_bWuXia;
	BOOL	m_bYaoShou;
	BOOL	m_bYuLin;
	BOOL	m_bYuMang;
	BOOL	m_bFaShi;
	BOOL	m_bCiKe;
	BOOL	m_bWuShi;
	BOOL	m_bYaoJing;
	BOOL	m_bJianLing;
	BOOL	m_bMeiLing;
	BOOL	m_bYeYing;
	BOOL	m_bYueXian;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPolicyOperationDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnOK();
	virtual void OnCancel();
	//}}AFX_VIRTUAL

// Implementation
protected:
	void ChangeProperty(int iOpType);
	void FreshTargetList();
	void EnableCheckBox(bool bEnable);
	void InitOperation();

	void ReleaseList();

	ADynPropertyObject *m_pTemp;
	COperationParam *m_pOperationModifier;
	CPropertyList *m_pList;
	// Generated message map functions
	//{{AFX_MSG(CPolicyOperationDlg)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();
	afx_msg void OnSelchangeComboOperationType();
	afx_msg void OnSelchangeComboTargetType();
	afx_msg void OnButtonAddTarget();
	afx_msg void OnButtonDelTarget();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_POLICYOPERATIONDLG_H__D8CA3C64_1C3A_4072_9484_4FE7AE7312C1__INCLUDED_)
