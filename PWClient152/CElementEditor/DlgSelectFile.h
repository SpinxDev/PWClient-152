#if !defined(AFX_DLGSELECTFILE_H__2B30DDAB_E966_4E20_9E74_C679F072E59A__INCLUDED_)
#define AFX_DLGSELECTFILE_H__2B30DDAB_E966_4E20_9E74_C679F072E59A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgSelectFile.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgSelectFile dialog

class CDlgSelectFile : public CDialog
{
public:	//	Types

	//	Initialize parameters
	struct PARAMS
	{
		const char*	szRootDir;		//	Root directory
		const char*	szFileFilter;	//	File filter, (*.* etc)
		bool		bMultiSelect;	//	true, enable multi-selection
		const char*	szPrompt;		//	Prompt string
	};

	//	Path tree item type
	enum
	{
		PTIT_DRIVER = 0,		//	Driver
		PTIT_FOLDER,			//	Folder
	};

	//	Image index
	enum
	{
		IMG_FOLDER = 0,			//	Closed folder
		IMG_OPENEDFOLDER,		//	Opened folder
	};

// Construction
public:
	CDlgSelectFile(const PARAMS& Params, CWnd* pParent = NULL);   // standard constructor

public:		//	Attributes

public:		//	Operations

	//	For single-selected style ------------------
	//	Get selected file's name
	CString GetFileName();
	//	Get selected file's relative path name
	CString GetRelativePathName();
	//	Get selected file's full path name
	CString GetFullPathName();

	//	For multi-selected style --------------------
	//	Get start position of file list
	DWORD GetStartPosition();
	//	Get next file name in file list
	CString GetNextFileName(DWORD* pdwPos);
	//	Get next relative path and name in file list
	CString GetNextRelativePathName(DWORD* pdwPos);
	//	Get next full path name in file list
	CString GetNextFullPathName(DWORD* pdwPos);

	//	Get number of selected file
	int GetSelectedFileNum() { return m_iNumSelFile; }

// Dialog Data
	//{{AFX_DATA(CDlgSelectFile)
	enum { IDD = IDD_SELECTFILE };
	CTreeCtrl	m_DirTree;
	CListCtrl	m_FileList;
	CString	m_strPrompt;
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgSelectFile)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

protected:	//	Attributes

	PARAMS		m_Params;
	CString		m_strRootDir;			//	Root directory
	CString		m_strFilter;			//	File filter
	char		m_szCurPath[MAX_PATH];	//	Current path backup
	CImageList	m_Images;

	CString		m_strFullPath;			//	Store full path
	CString		m_strRelativePath;		//	Store relative path
	CString		m_strFileList;			//	File list
	int			m_iNumSelFile;			//	Number of selected file

protected:	//	Operations

	//	Initialize image list
	void InitImageList();
	//	Set root path
	void SetRootPath();
	//	Recursively travel all sub directoies
	void RecursiveDirectories(const char* szPath, HTREEITEM hParent);
	//	Insert a path to directory tree
	HTREEITEM InsertPathItem(HTREEITEM hParent, const char* szPath);
	//	Get item path
	void GetItemPath(CString& strPath, HTREEITEM hItem, bool bFull);

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgSelectFile)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnDestroy();
	afx_msg void OnSelchangedTreeDirectory(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDblclkListFile(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnButtonCheckOut();
	afx_msg void OnButtonCheckIn();
	afx_msg void OnDblclkTreeDirectory(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGSELECTFILE_H__2B30DDAB_E966_4E20_9E74_C679F072E59A__INCLUDED_)
