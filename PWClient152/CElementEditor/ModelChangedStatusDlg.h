#if !defined(AFX_MODELCHANGEDSTATUSDLG_H__6ACE5D99_D7CB_4F2E_B849_B08576E78375__INCLUDED_)
#define AFX_MODELCHANGEDSTATUSDLG_H__6ACE5D99_D7CB_4F2E_B849_B08576E78375__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ModelChangedStatusDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CModelChangedStatusDlg dialog

class CModelChangedStatusDlg : public CDialog
{
// Construction
public:
	CModelChangedStatusDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CModelChangedStatusDlg)
	enum { IDD = IDD_DIALOG_MODEL_CHANGED };
	CProgressCtrl	m_Progress;
	int		m_nDate;
	int		m_nMouth;
	int		m_nYear;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CModelChangedStatusDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnOK();
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CModelChangedStatusDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MODELCHANGEDSTATUSDLG_H__6ACE5D99_D7CB_4F2E_B849_B08576E78375__INCLUDED_)
