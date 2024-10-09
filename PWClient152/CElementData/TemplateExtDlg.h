#if !defined(AFX_TEMPLATEEXTDLG_H__542B1D00_5DAB_4AF6_AA82_A042E4DF3F4C__INCLUDED_)
#define AFX_TEMPLATEEXTDLG_H__542B1D00_5DAB_4AF6_AA82_A042E4DF3F4C__INCLUDED_

#include "ExtendDataTempl.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TemplateExtDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CTemplateExtDlg dialog

class CTemplateExtDlg : public CDialog
{
// Construction
public:
	CTemplateExtDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CTemplateExtDlg)
	enum { IDD = IDD_DIALOG_EXT_CREATE };
	CEdit	m_EditName;
	CComboBox	m_cExtItem1;
	CComboBox	m_cExtItemNum;
	CComboBox	m_cExtItem4;
	CComboBox	m_cExtItem3;
	CComboBox	m_cExtItem2;
	CString	m_strName;
	CString	m_strItemName1;
	CString	m_strItemName2;
	CString	m_strItemName3;
	CString	m_strItemName4;
	//}}AFX_DATA
	CString m_strTemplateName;
	CString m_strDir;
	int m_nItemNum;
	bool m_bModified;
	void DisableItem(int num);
	void EnumTemplates();
	void LoadExt(CString pathName);
	ExtendDataTempl m_extTmp;
	CString m_strItemType[4];
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTemplateExtDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CTemplateExtDlg)
	afx_msg void OnSelchangeComboTemplateList();
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeComboExtNum();
	afx_msg void OnSelchangeComboExtItem2();
	afx_msg void OnSelchangeComboExtItem3();
	afx_msg void OnSelchangeComboExtItem4();
	afx_msg void OnSelchangeComboExtItem5();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TEMPLATEEXTDLG_H__542B1D00_5DAB_4AF6_AA82_A042E4DF3F4C__INCLUDED_)
