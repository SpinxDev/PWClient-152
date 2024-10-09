#if !defined(AFX_COPYSUBTASKDLG_H__A4523BD7_B64E_46FA_BBA3_93DA0F673118__INCLUDED_)
#define AFX_COPYSUBTASKDLG_H__A4523BD7_B64E_46FA_BBA3_93DA0F673118__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CopySubTaskDlg.h : header file
//
class ATaskTempl;
/////////////////////////////////////////////////////////////////////////////
// CCopySubTaskDlg dialog

class CCopySubTaskDlg : public CDialog
{
// Construction
public:
	CCopySubTaskDlg(ATaskTempl* pSrcTask, ATaskTempl* pDestTask, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CCopySubTaskDlg)
	enum { IDD = IDD_COPY_SUB_TASK };
	CTreeCtrl	m_TreeSouceTask;
	BOOL	m_bCopySubTask;
	CString	m_strNewName;
	CString	m_strOldName;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCopySubTaskDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	ATaskTempl* m_pSrcTask;
	ATaskTempl* m_pDestTask;

	CImageList m_ImageList;
	// Generated message map functions
	//{{AFX_MSG(CCopySubTaskDlg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_COPYSUBTASKDLG_H__A4523BD7_B64E_46FA_BBA3_93DA0F673118__INCLUDED_)
