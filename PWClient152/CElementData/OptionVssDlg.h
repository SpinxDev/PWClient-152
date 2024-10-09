#if !defined(AFX_OPTIONVSSDLG_H__BF4B231E_E652_4A32_B359_0A62FC23DDB6__INCLUDED_)
#define AFX_OPTIONVSSDLG_H__BF4B231E_E652_4A32_B359_0A62FC23DDB6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// OptionVssDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// COptionVssDlg dialog

class COptionVssDlg : public CDialog
{
// Construction
public:
	COptionVssDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(COptionVssDlg)
	enum { IDD = IDD_DIALOG_OPTION_VSS };
	CString	m_strName;
	CString	m_strPassword;
	CString	m_strServerPath;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(COptionVssDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	bool m_bLink;
	bool m_bAutoUpdate;

	// Generated message map functions
	//{{AFX_MSG(COptionVssDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnCheckAutoUpdate();
	afx_msg void OnCheckCutLinkVss();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_OPTIONVSSDLG_H__BF4B231E_E652_4A32_B359_0A62FC23DDB6__INCLUDED_)
