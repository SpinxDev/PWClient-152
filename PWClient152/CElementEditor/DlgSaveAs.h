#if !defined(AFX_DLGSAVEAS_H__038089BE_F54E_43F6_9600_BB444338FDC4__INCLUDED_)
#define AFX_DLGSAVEAS_H__038089BE_F54E_43F6_9600_BB444338FDC4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgSaveAs.h : header file
//

class CElementMap;

/////////////////////////////////////////////////////////////////////////////
// CDlgSaveAs dialog

class CDlgSaveAs : public CDialog
{
// Construction
public:
	CDlgSaveAs(CElementMap* pMap, CWnd* pParent = NULL);   // standard constructor

	//	Get project name
	const char* GetProjectName() { return m_strProjName; }

// Dialog Data
	//{{AFX_DATA(CDlgSaveAs)
	enum { IDD = IDD_SAVEAS };
	CString	m_strProjName;
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgSaveAs)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	CElementMap*	m_pMap;

	// Generated message map functions
	//{{AFX_MSG(CDlgSaveAs)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGSAVEAS_H__038089BE_F54E_43F6_9600_BB444338FDC4__INCLUDED_)
