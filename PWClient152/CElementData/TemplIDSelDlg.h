#if !defined(AFX_TEMPLIDSELDLG_H__B30B74D5_AD6C_4A75_A697_D103E673F761__INCLUDED_)
#define AFX_TEMPLIDSELDLG_H__B30B74D5_AD6C_4A75_A697_D103E673F761__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TemplIDSelDlg.h : header file
//

#include "AObject.h"

enum ID_TYPE
{
	enumBaseID,
	enumRecipeID,
	enumFaceID,
	enumExtID,
	enumConfigID,
};

struct SEL_ID_FUNCS : public CUSTOM_FUNCS
{
	~SEL_ID_FUNCS() {}
	virtual BOOL CALLBACK OnActivate(void);
	virtual LPCTSTR CALLBACK OnGetShowString(void) const;
	virtual AVariant CALLBACK OnGetValue(void) const;
	virtual void CALLBACK OnSetValue(const AVariant& var);

	CString		m_strRootDir;
	CString		m_strExt;
	AVariant	m_var;
	ID_TYPE		m_IDType;
	CString		m_strText;

	void Resume() { m_strText.Empty(); }
};

class CTemplIDSel
{
public:
	CTemplIDSel() {}
	virtual ~CTemplIDSel() {}

protected:
	SEL_ID_FUNCS m_SelIDFuncs;

public:
	void SetRootPath(LPCTSTR szRoot) { m_SelIDFuncs.m_strRootDir = szRoot; }
	void SetFileExt(LPCTSTR szExt) { m_SelIDFuncs.m_strExt = szExt; }
	void SetValue(const AVariant& var) { m_SelIDFuncs.OnSetValue(var); }
	void* GetFuncsPtr() { return (void*)&m_SelIDFuncs; }
	void SetIDType(ID_TYPE id_type) { m_SelIDFuncs.m_IDType = id_type; }
	void Init(
		LPCTSTR szRoot,
		LPCTSTR szExt,
		const AVariant& var,
		ID_TYPE id_type)
	{
		m_SelIDFuncs.Resume();
		SetRootPath(szRoot);
		SetFileExt(szExt);
		SetIDType(id_type);
		SetValue(var);
	}
};

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
	CString	m_strTarget;
	//}}AFX_DATA
	
	CImageList		m_ImageList;
	CString			m_strRoot;
	CString			m_strExt;
	unsigned long	m_ulID;
	CString			m_strName;
	CString			m_strFileName;
	CString			m_strInitPath;
	HTREEITEM		m_hNull;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTemplIDSelDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	void EnumTempl(const CString& strPath, HTREEITEM hRoot);
	HTREEITEM SearchItem(const char* szItem, HTREEITEM hRoot = TVI_ROOT);

	// Generated message map functions
	//{{AFX_MSG(CTemplIDSelDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangedTreeTempl(NMHDR* pNMHDR, LRESULT* pResult);
	virtual void OnOK();
	afx_msg void OnSearch();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TEMPLIDSELDLG_H__B30B74D5_AD6C_4A75_A697_D103E673F761__INCLUDED_)
