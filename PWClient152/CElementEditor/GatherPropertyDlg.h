#if !defined(AFX_GATHERPROPERTYDLG_H__4383A01D_9574_4195_9FAE_82A6C7165CE9__INCLUDED_)
#define AFX_GATHERPROPERTYDLG_H__4383A01D_9574_4195_9FAE_82A6C7165CE9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// GatherPropertyDlg.h : header file
//
#include "SceneGatherObject.h"
/////////////////////////////////////////////////////////////////////////////
// CGatherPropertyDlg dialog

class CGatherPropertyDlg : public CDialog
{
// Construction
public:
	CGatherPropertyDlg(CWnd* pParent = NULL);   // standard constructor
	GATHER m_data;
	bool m_bModified;
// Dialog Data
	//{{AFX_DATA(CGatherPropertyDlg)
	enum { IDD = IDD_GATHER_PROPERTY_DLG };
	CListBox	m_listObject;
	CComboBox	m_cbType;
	UINT	m_nFresh;
	UINT	m_nNum;
	float	m_fOffset;
	//}}AFX_DATA

	void FreshObjectList();


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGatherPropertyDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnOK();
	//}}AFX_VIRTUAL
	
// Implementation
protected:
	DWORD                m_nType;
	
	// Generated message map functions
	//{{AFX_MSG(CGatherPropertyDlg)
	afx_msg void OnSelchangeComboGatherType();
	virtual BOOL OnInitDialog();
	afx_msg void OnButtonFinder();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GATHERPROPERTYDLG_H__4383A01D_9574_4195_9FAE_82A6C7165CE9__INCLUDED_)
