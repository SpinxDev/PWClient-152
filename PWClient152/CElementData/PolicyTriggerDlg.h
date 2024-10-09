#if !defined(AFX_POLICYTRIGGERDLG_H__137B0DEE_B62A_4F28_AC1E_CDC0DC6D8D47__INCLUDED_)
#define AFX_POLICYTRIGGERDLG_H__137B0DEE_B62A_4F28_AC1E_CDC0DC6D8D47__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PolicyTriggerDlg.h : header file
//
#include "Policy.h"
CString GetTargetStr(void *pTarget);

//该树结构是为了保证触发器的恢复
struct TRIGGER_TREE
{
	TRIGGER_TREE(){ pTrigger = 0; }
	~TRIGGER_TREE()
	{ 
		for( size_t i=0; i < listChild.size(); ++i)
			if(listChild[i]) delete listChild[i];
		if(pTrigger) 
		{
			pTrigger->Release();
			delete pTrigger;
		}
		listChild.clear();
	}
	
	CTriggerData *pTrigger;
	abase::vector<TRIGGER_TREE *> listChild;
};

/////////////////////////////////////////////////////////////////////////////
// CPolicyTriggerDlg dialog
class CPolicyTriggerDlg : public CDialog
{
// Construction
public:
	CPolicyTriggerDlg(CWnd* pParent = NULL);   // standard constructor
	CTriggerData *m_pTriggerData;
	CPolicyData *m_pCurrentPolicy;
	bool m_bModifyed;
	DWORD m_dwTriggerID;
	bool m_bIsChanged;//是否已经修改
	bool m_bAttackValid;
	CString TraverselTree( void *pTree);
	CString GetTriggerName( unsigned int id);
// Dialog Data
	//{{AFX_DATA(CPolicyTriggerDlg)
	enum { IDD = IDD_DIALOG_POLICY_TRIGGER };
	CListBox	m_listCondition;
	CListBox	m_listOperation;
	CString	m_strProgram;
	CString	m_strTriggerName;
	BOOL	m_bActive;
	UINT	m_uTriggerID;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPolicyTriggerDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnOK(){};
	virtual void OnCancel();
	//}}AFX_VIRTUAL

// Implementation
protected:
	void FreshList();
	void FreshCondition();
	void SaveOldTree(TRIGGER_TREE *pTreeRoot,CTriggerData *pTrigger);
	void StartRecord();
	void RestoreTrigger(TRIGGER_TREE *pTreeRoot);
	TRIGGER_TREE *m_pOldTree;
	// Generated message map functions
	//{{AFX_MSG(CPolicyTriggerDlg)
	afx_msg void OnAddOperation();
	afx_msg void OnModify();
	virtual BOOL OnInitDialog();
	afx_msg void OnDelOperation();
	afx_msg void OnMoveUp();
	afx_msg void OnMoveDown();
	afx_msg void OnAddCondition();
	afx_msg void OnDelCondition();
	afx_msg void OnMoveUpCondition();
	afx_msg void OnMoveDownCondition();
	afx_msg void OnCheckProgramError();
	afx_msg void OnChangeEditTriggerName();
	afx_msg void OnCheckActive();
	afx_msg void OnDblclkListboxOperation();
	afx_msg void OnRadioAttackEffect();
	afx_msg void OnRadioAttactNoeffect();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_POLICYTRIGGERDLG_H__137B0DEE_B62A_4F28_AC1E_CDC0DC6D8D47__INCLUDED_)
