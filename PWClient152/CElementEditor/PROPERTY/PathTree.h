#if !defined(AFX_PATHTREE_H__0BA44ED9_44A3_4F83_840E_C39DC31BA936__INCLUDED_)
#define AFX_PATHTREE_H__0BA44ED9_44A3_4F83_840E_C39DC31BA936__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PathTree.h : header file
//
#include <afxtempl.h>

/////////////////////////////////////////////////////////////////////////////
// CPathTree window
enum
{
	PATHTREE_FILEICON		= 0,
	PATHTREE_FOLDERICON,
	PATHTREE_FOLDEROPENICON,
};

typedef enum
{
	PTIT_DRIVER,			//驱动器
	PTIT_FILE,				//文件
	PTIT_FOLDER,			//目录
}	PATHTREEITEMTYPE;

typedef BOOL (CALLBACK * FINDFILTER)(DWORD param, CString filepath, PATHTREEITEMTYPE type);

class CPathTree : public CTreeCtrl
{
// Construction
public:
	CPathTree();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPathTree)
	public:
	protected:
	virtual void PreSubclassWindow();
	//}}AFX_VIRTUAL

// Implementation
public:

	void EnsureVisiblePath(CString path, BOOL bPathIsFull = FALSE);
	CString GetItemPath(HTREEITEM hItem, BOOL bFull = TRUE);
	CString GetSelectedPath(BOOL bFull = TRUE, PATHTREEITEMTYPE * type = NULL);
	CString GetPath();
	virtual BOOL SetPath(CString path, FINDFILTER filterproc = NULL, DWORD param = 0);
	virtual ~CPathTree();

	// Generated message map functions
protected:

	DWORD		m_param;
	FINDFILTER	m_filterproc;
	struct FILEREGISTERICON
	{
		char	m_extendname[_MAX_FNAME];
		int		m_imageindex;
	};

	CList<FILEREGISTERICON, FILEREGISTERICON &> m_registerIcon;

	void InitImageList();
	void QueryFileIconIndex(CString filepath, PATHTREEITEMTYPE type, int & image, int & selimage);
	HTREEITEM InsertFileItem(HTREEITEM hParent, CString filepath, PATHTREEITEMTYPE type, BOOL queryicon);
	BOOL ReadPathStruct(CString path, int layer, HTREEITEM hParent = NULL, FINDFILTER filterproc = NULL);
	
	CString m_path;
	CImageList m_pImages;
	//{{AFX_MSG(CPathTree)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnItemexpanding(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnClose();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PATHTREE_H__0BA44ED9_44A3_4F83_840E_C39DC31BA936__INCLUDED_)
