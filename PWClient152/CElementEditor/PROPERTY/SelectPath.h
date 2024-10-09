#if !defined(AFX_SELECTPATH_H__62105BEE_2691_4022_BEE2_3E0C2FB6C27B__INCLUDED_)
#define AFX_SELECTPATH_H__62105BEE_2691_4022_BEE2_3E0C2FB6C27B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SelectPath.h : header file
//

#include "PathTree.h"
/////////////////////////////////////////////////////////////////////////////
// CSelectPath dialog

class CSelectPath : public CDialog
{
// Construction
public:
	CSelectPath(CWnd* pParent = NULL);   // standard constructor

	void SetPath(CString path,CString filter);
	CString CSelectPath::GetPathName();
	CString m_path;
	CString m_filter;
	CString m_cur_dir;
// Dialog Data
	//{{AFX_DATA(CSelectPath)
	enum { IDD = IDD_PATH_DIALOG };
	CString m_filename;
	CPathTree m_pathtree;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSelectPath)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSelectPath)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangedTree1(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SELECTPATH_H__62105BEE_2691_4022_BEE2_3E0C2FB6C27B__INCLUDED_)
