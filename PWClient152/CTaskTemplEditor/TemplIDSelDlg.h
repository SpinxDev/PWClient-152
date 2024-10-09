#if !defined(AFX_TEMPLIDSELDLG_H__CB3251B2_3AF2_4735_A35C_DDC7DAF5DA85__INCLUDED_)
#define AFX_TEMPLIDSELDLG_H__CB3251B2_3AF2_4735_A35C_DDC7DAF5DA85__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TemplIDSelDlg.h : header file
//

#include "AString.h"
#include "hashmap.h"

#define MAX_READ_LEN	1024
typedef abase::hash_map<AString, int> ORG_PATHS;
extern ORG_PATHS _org_paths;
extern const char* _org_config_path;

inline bool ReadLine(FILE* fp, char* szLine)
{
	if (fgets(szLine, MAX_READ_LEN, fp) == NULL) return false;
	szLine[strcspn(szLine, "\r\n")] = '\0';
	return true;
}

extern CString GetBaseDataNameByID(unsigned long ulID);
struct _tree_node;

/////////////////////////////////////////////////////////////////////////////
// CTemplIDSelDlg dialog

class CTemplIDSelDlg : public CDialog
{
// Construction
public:

	CTemplIDSelDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CTemplIDSelDlg)
	enum { IDD = IDD_TEMPL_ID_SEL };
	CTreeCtrl	m_IDTree;
	CString	m_strToFind;
	//}}AFX_DATA

	void InitTree();

	CImageList		m_ImageList;
	CString			m_strRoot;
	CString			m_strExt;
	unsigned long	m_ulID;
	CString			m_strName;
	CString			m_strInitPath;
	CString			m_strSelPath;
	
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTemplIDSelDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	void EnumTempl(_tree_node* node, HTREEITEM hRoot);
	BOOL Find(HTREEITEM hItem, const CString& strToFind);
	BOOL Find2(HTREEITEM hItem, const CString& strToFind, int iID);

	// Generated message map functions
	//{{AFX_MSG(CTemplIDSelDlg)
	afx_msg void OnSelchangedTreeTempl(NMHDR* pNMHDR, LRESULT* pResult);
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnFind();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TEMPLIDSELDLG_H__CB3251B2_3AF2_4735_A35C_DDC7DAF5DA85__INCLUDED_)
