#if !defined(AFX_FIXEDNPCSELDLG_H__D20AB8E4_20C4_43F5_AAD2_B5C73348CE64__INCLUDED_)
#define AFX_FIXEDNPCSELDLG_H__D20AB8E4_20C4_43F5_AAD2_B5C73348CE64__INCLUDED_

#include "elementdataman.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// FixedNpcSelDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CFixedNpcSelDlg dialog


class CFixedNpcSelDlg : public CDialog
{
// Construction
public:
	CFixedNpcSelDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CFixedNpcSelDlg)
	enum { IDD = IDD_FIXED_NPC_SEL };
	CComboBox	m_comboSel;
	//}}AFX_DATA
	DWORD m_dwSelId;
	CString m_strName;
	bool    m_bMonster;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFixedNpcSelDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	DWORD GetNpcID(CString name);
	void  FreshList();
	//elementdataman m_testData;
	// Generated message map functions
	//{{AFX_MSG(CFixedNpcSelDlg)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg void OnFileSortMap();
	afx_msg void OnDestroy();
	afx_msg void OnRadioNpc();
	afx_msg void OnRadioMonster();
	afx_msg void OnButtonFind();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FIXEDNPCSELDLG_H__D20AB8E4_20C4_43F5_AAD2_B5C73348CE64__INCLUDED_)
