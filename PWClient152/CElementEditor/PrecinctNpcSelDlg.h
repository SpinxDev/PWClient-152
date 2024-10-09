#if !defined(AFX_PRECINCTNPCSELDLG_H__64BD7AE7_F695_4F56_B4EC_8D89936C2FDB__INCLUDED_)
#define AFX_PRECINCTNPCSELDLG_H__64BD7AE7_F695_4F56_B4EC_8D89936C2FDB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PrecinctNpcSelDlg.h : header file
//

#include "ScenePrecinctObject.h"

/////////////////////////////////////////////////////////////////////////////
// CPrecinctNpcSelDlg dialog

class CPrecinctNpcSelDlg : public CDialog
{
// Construction
public:
	CPrecinctNpcSelDlg(CWnd* pParent = NULL);   // standard constructor
	void SetPrecinctObject(CScenePrecinctObject * pPrecinct){ m_pPrecinct = pPrecinct; };
// Dialog Data
	//{{AFX_DATA(CPrecinctNpcSelDlg)
	enum { IDD = IDD_PRICIENT_NPC_SEL };
	CListBox	m_listSceneNpc;
	CListBox	m_listPrecinctNpc;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPrecinctNpcSelDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

	
	
// Implementation
protected:
	CScenePrecinctObject* m_pPrecinct;
	void FreshPrecinctList();
	// Generated message map functions
	//{{AFX_MSG(CPrecinctNpcSelDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnDelPos();
	afx_msg void OnDelPos2();
	afx_msg void OnPrecinctAddPos();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PRECINCTNPCSELDLG_H__64BD7AE7_F695_4F56_B4EC_8D89936C2FDB__INCLUDED_)
