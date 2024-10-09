#if !defined(AFX_MOUNSTERFALLLISTDLG_H__E39AFD81_4B0C_4104_94C3_A023E18222C6__INCLUDED_)
#define AFX_MOUNSTERFALLLISTDLG_H__E39AFD81_4B0C_4104_94C3_A023E18222C6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MounsterFallListDlg.h : header file
//

#include "MounsterResetFallList.h"

/////////////////////////////////////////////////////////////////////////////
// CMounsterFallListDlg dialog

class CMounsterFallListDlg : public CDialog
{
// Construction
public:
	CMounsterFallListDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CMounsterFallListDlg)
	enum { IDD = IDD_DIALOG_MOUNSTER_FALL_LIST };
	CListBox	m_listItem;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMounsterFallListDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	void FreshList();
	CMounsterResetFallListData *m_pData;
	bool m_bReadOnly;
	bool m_bChanged;
	// Generated message map functions
	//{{AFX_MSG(CMounsterFallListDlg)
	afx_msg void OnAdd();
	afx_msg void OnDel();
	afx_msg void OnDblclkListItem();
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();
	afx_msg void OnRadioCheckIn();
	afx_msg void OnRadioCheckOut();
	afx_msg void OnExport();
	afx_msg void OnExportTestTxt();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MOUNSTERFALLLISTDLG_H__E39AFD81_4B0C_4104_94C3_A023E18222C6__INCLUDED_)
