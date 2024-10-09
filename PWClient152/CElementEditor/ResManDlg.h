#if !defined(AFX_RESMANDLG_H__65F2711C_BC97_4878_8262_EA74D37F62C7__INCLUDED_)
#define AFX_RESMANDLG_H__65F2711C_BC97_4878_8262_EA74D37F62C7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ResManDlg.h : header file
//

#include "ElementResMan.h"
#include "VssOperation.h"

class CElementMap;
/////////////////////////////////////////////////////////////////////////////
// CResManDlg dialog

enum
{
	REF_TYPE_ALL = 0,
	REF_TYPE_NO_REF,
	REF_TYPE_ONLY_REF,
	REF_TYPE_COUNT,
};

class CResManDlg : public CDialog
{
// Construction
public:
	CResManDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CResManDlg)
	enum { IDD = IDD_DIALOG_RES_MAN };
	CListCtrl	m_listRes;
	CListCtrl	m_listRef;
	CComboBox	m_cbResType;
	CComboBox	m_cbResRefType;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CResManDlg)
	public:
	virtual BOOL DestroyWindow();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL
protected:
	CString			mTypeList[ElementResMan::RES_TYPE_COUNT];
	CString			mRefTypeList[REF_TYPE_COUNT];
	ElementResMan   mResMan;

	int  nResType;
	int  nRefType;

	void UpdateList();
	void UpdateRefList(APtrList<char *>*pRefList);
	void TransMap(CElementMap *pMap);
	void ScrollItem();
	
// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CResManDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();
	afx_msg void OnItemchangedListRes(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSelchangeComboResType();
	afx_msg void OnSelchangeComboResRefType();
	afx_msg void OnButtonAddRes();
	afx_msg void OnButtonDelRes();
	afx_msg void OnButtonReplaceRes();
	afx_msg void OnButtonTransOldMap();
	afx_msg void OnButtonUpdateRes();
	afx_msg void OnButtonResFind();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_RESMANDLG_H__65F2711C_BC97_4878_8262_EA74D37F62C7__INCLUDED_)
