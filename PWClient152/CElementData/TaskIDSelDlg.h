#if !defined(AFX_TASKIDSELDLG_H__52508711_F406_448A_B73F_3F3355AE3AE3__INCLUDED_)
#define AFX_TASKIDSELDLG_H__52508711_F406_448A_B73F_3F3355AE3AE3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TaskIDSelDlg.h : header file
//

#include "AObject.h"
#include "..\CElementClient\Task\TaskTempl.h"

struct TASK_ID_FUNCS : public CUSTOM_FUNCS
{
	~TASK_ID_FUNCS() {}
	virtual BOOL CALLBACK OnActivate(void);
	virtual LPCTSTR CALLBACK OnGetShowString(void) const;
	virtual AVariant CALLBACK OnGetValue(void) const;
	virtual void CALLBACK OnSetValue(const AVariant& var);

	AVariant	m_var;
	CString		m_strText;
};

extern CString GetTaskNameByID(unsigned long ulID);

/////////////////////////////////////////////////////////////////////////////
// CTaskIDSelDlg dialog

class CTaskIDSelDlg : public CDialog
{
// Construction
public:
	CTaskIDSelDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CTaskIDSelDlg)
	enum { IDD = IDD_TASK_ID_SEL };
	CTreeCtrl	m_IDTree;
	//}}AFX_DATA
	
	CImageList		m_ImageList;
	unsigned long	m_ulID;
	CString			m_strName;
	CString			m_strInitFile;
	bool			m_bShowSub;
	bool			m_bOneOnly;
	unsigned long	m_ulTopTaskToSel;

	void UpdateTaskTree();
	HTREEITEM UpdateTaskTree(HTREEITEM hParent, ATaskTempl* pTask);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTaskIDSelDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CTaskIDSelDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnSelchangedTreeTask(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TASKIDSELDLG_H__52508711_F406_448A_B73F_3F3355AE3AE3__INCLUDED_)
