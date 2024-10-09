#if !defined(AFX_WORKSPACESETDLG_H__E8DB6E9B_9601_4672_93E0_BCE57A540262__INCLUDED_)
#define AFX_WORKSPACESETDLG_H__E8DB6E9B_9601_4672_93E0_BCE57A540262__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// WorkSpaceSetDlg.h : header file
//
extern const int g_randMapVersion;

/////////////////////////////////////////////////////////////////////////////
// CWorkSpaceSetDlg dialog

class CWorkSpaceSetDlg : public CDialog
{
// Construction
public:
	CWorkSpaceSetDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CWorkSpaceSetDlg)
	enum { IDD = IDD_DIALOG_SET_WORKSPACE };
	CListBox	m_listWorkSpace;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWorkSpaceSetDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	void FreshList();
	// Generated message map functions
	//{{AFX_MSG(CWorkSpaceSetDlg)
	afx_msg void OnButtonNewWorkspace();
	afx_msg void OnButtonDelWorkspace();
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg void OnDblclkListWorkspace();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WORKSPACESETDLG_H__E8DB6E9B_9601_4672_93E0_BCE57A540262__INCLUDED_)
