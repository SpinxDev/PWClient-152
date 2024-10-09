#if !defined(AFX_RESFINDERDLG_H__ED5EEA85_1D83_4DC5_A58D_A628D0B36ABA__INCLUDED_)
#define AFX_RESFINDERDLG_H__ED5EEA85_1D83_4DC5_A58D_A628D0B36ABA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ResFinderDlg.h : header file
//
#include "ElementResMan.h"
/////////////////////////////////////////////////////////////////////////////
// CResFinderDlg dialog

class CResFinderDlg : public CDialog
{
// Construction
public:
	CResFinderDlg(CWnd* pParent = NULL);   // standard constructor
	void Init( ElementResMan *pResMan, int nResType)
	{
		m_pResMan = pResMan;
		m_nResType = nResType;
	};
	CString m_strResultItem;
// Dialog Data
	//{{AFX_DATA(CResFinderDlg)
	enum { IDD = IDD_DIALOG_RES_FINDER };
	CListBox	m_listResult;
	CString	m_strKeyWord;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CResFinderDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnOK();
	//}}AFX_VIRTUAL

// Implementation
protected:
	ElementResMan *m_pResMan;
	int            m_nResType;
	// Generated message map functions
	//{{AFX_MSG(CResFinderDlg)
	afx_msg void OnButtonFind();
	afx_msg void OnDblclkListSertchResult();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_RESFINDERDLG_H__ED5EEA85_1D83_4DC5_A58D_A628D0B36ABA__INCLUDED_)
