#if !defined(AFX_DLGFINDTEMPLRESULT_H__7972E6BD_2F1E_4B22_A3D0_05DADED18F01__INCLUDED_)
#define AFX_DLGFINDTEMPLRESULT_H__7972E6BD_2F1E_4B22_A3D0_05DADED18F01__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgFindTemplResult.h : header file
//
#include <vector>

/////////////////////////////////////////////////////////////////////////////
// CDlgFindTemplResult dialog

class CDlgFindTemplResult : public CDialog
{
// Construction
public:
	CDlgFindTemplResult(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgFindTemplResult)
	enum { IDD = IDD_DIALOG_TEMPL_LIST };
	CListCtrl	m_Lst;
	//}}AFX_DATA

	struct FindItem 
	{
		unsigned long uID;
		CString	strName;
		CString strPath;

		FindItem() : uID(0) {}

		FindItem(unsigned long id, const CString &name, const CString &path)
			: uID(id), strName(name), strPath(path)
		{}
	};
	typedef std::vector<FindItem> FindResult;
	FindResult	m_result;

	typedef std::vector<unsigned long> Selection;
	Selection	m_selection;

	CString		m_strColumn2Name;


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgFindTemplResult)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	void UpdateView();

	// Generated message map functions
	//{{AFX_MSG(CDlgFindTemplResult)
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	virtual BOOL OnInitDialog();
	afx_msg void OnDblclkList1(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnButton1();
	afx_msg void OnButton2();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGFINDTEMPLRESULT_H__7972E6BD_2F1E_4B22_A3D0_05DADED18F01__INCLUDED_)
