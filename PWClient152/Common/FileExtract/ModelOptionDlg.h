#if !defined(AFX_MODELOPTIONDLG_H__FAEA633E_A409_45F8_86B4_371599C50567__INCLUDED_)
#define AFX_MODELOPTIONDLG_H__FAEA633E_A409_45F8_86B4_371599C50567__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ModelOptionDlg.h : header file
//

#include <AAssist.h>

/////////////////////////////////////////////////////////////////////////////
// CModelOptionDlg dialog

class CModelOptionDlg : public CDialog
{
// Construction
public:
	CModelOptionDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CModelOptionDlg)
	enum { IDD = IDD_MODEL_OPTION };
	CListBox	m_Actions;
	CString	m_szModelName;
	BOOL	m_bEquipGfx;
	//}}AFX_DATA

	AString m_szModelPath;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CModelOptionDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CModelOptionDlg)
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnBtnSelectall();
	afx_msg void OnBtnNoselect();
	afx_msg void OnBtnInvertsel();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MODELOPTIONDLG_H__FAEA633E_A409_45F8_86B4_371599C50567__INCLUDED_)
