#if !defined(AFX_MOUNSTERFALLITEM_H__DFDB672A_88D2_4D4B_8C6E_ABE1485FA8CA__INCLUDED_)
#define AFX_MOUNSTERFALLITEM_H__DFDB672A_88D2_4D4B_8C6E_ABE1485FA8CA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MounsterFallItemDlg.h : header file
//
#include "MounsterResetFallList.h"

class CPropertyList;
class ADynPropertyObject;
class CTemplIDSel;
/////////////////////////////////////////////////////////////////////////////
// CMounsterFallItemDlg dialog

class CMounsterFallItemDlg : public CDialog
{
// Construction
public:
	CMounsterFallItemDlg(CWnd* pParent = NULL);   // standard constructor
	void UpdateItemData(bool bGet);

	CMounsterFallItem *m_pItem;
	CMounsterFallItem *m_pOldItem;
	bool m_bModified;
	bool m_bChanged;
	bool m_bReadOnly;
// Dialog Data
	//{{AFX_DATA(CMounsterFallItemDlg)
	enum { IDD = IDD_DIALOG_MOUNSTER_FALL_ITEM };
	CComboBox	m_cbType;
	CListBox	m_listMounster;
	CString	m_strName;
	//}}AFX_DATA

	
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMounsterFallItemDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnOK();
	virtual void OnCancel();
	//}}AFX_VIRTUAL

// Implementation
protected:
	void FreshMounsterList();
	CString GetMounsterName( unsigned int id);

	CPropertyList *m_pList;
	ADynPropertyObject *m_pProperty;
	CTemplIDSel* m_pListIDSel[256];
	// Generated message map functions
	//{{AFX_MSG(CMounsterFallItemDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();
	afx_msg void OnAdd();
	afx_msg void OnDel();
	afx_msg void OnModified();
	afx_msg void OnDelAll();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MOUNSTERFALLITEM_H__DFDB672A_88D2_4D4B_8C6E_ABE1485FA8CA__INCLUDED_)
