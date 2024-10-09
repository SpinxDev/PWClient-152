#if !defined(AFX_TEMPLIDSELDLGEX_H__2E96B584_6E34_46BE_9B1E_8C9057B64681__INCLUDED_)
#define AFX_TEMPLIDSELDLGEX_H__2E96B584_6E34_46BE_9B1E_8C9057B64681__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TemplIDSelDlgEx.h : header file
//

#include "vector.h"
#include "MltiTree.h"

/////////////////////////////////////////////////////////////////////////////
// CTemplIDSelDlgEx dialog

class CTemplIDSelDlgEx : public CDialog
{
// Construction
public:
	CTemplIDSelDlgEx(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CTemplIDSelDlgEx)
	enum { IDD = IDD_TEMPL_ID_SEL_EX };
	CMultiTree m_IDTree;
	CString	m_strTarget;
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

	CImageList		m_ImageList;
	CString			m_strRoot;
	CString			m_strExt;
	//unsigned long	m_ulID;
	CString			m_strName;
	CString			m_strFileName;
	CString			m_strInitPath;
	HTREEITEM		m_hNull;
	abase::vector<unsigned long> listID;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTemplIDSelDlgEx)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	void EnumTempl(const CString& strPath, HTREEITEM hRoot);
	HTREEITEM SearchItem(const char* szItem, HTREEITEM hRoot = TVI_ROOT);
	unsigned long GetTemplID(HTREEITEM hLeaf);
	void GetAllSelTemplID(HTREEITEM hRoot, bool bAppend);
	void SetItemCheck( HTREEITEM hRoot ,bool bCheck);
	// Generated message map functions
	//{{AFX_MSG(CTemplIDSelDlgEx)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangedTreeTempl(NMHDR* pNMHDR, LRESULT* pResult);
	virtual void OnOK();
	afx_msg void OnSearch();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TEMPLIDSELDLGEX_H__2E96B584_6E34_46BE_9B1E_8C9057B64681__INCLUDED_)
