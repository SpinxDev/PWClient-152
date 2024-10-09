#if !defined(AFX_TEMPLATECREATEDLG_H__C8C68E0F_9FAE_4A04_8FEC_465341FC88FA__INCLUDED_)
#define AFX_TEMPLATECREATEDLG_H__C8C68E0F_9FAE_4A04_8FEC_465341FC88FA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TemplateCreateDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CTemplateCreateDlg dialog

class CTemplateCreateDlg : public CDialog
{
// Construction
public:
	CTemplateCreateDlg(CWnd* pParent = NULL);   // standard constructor

	CString m_strPath;

// Dialog Data
	//{{AFX_DATA(CTemplateCreateDlg)
	enum { IDD = IDD_DIALOG_TEMPLATE_CREATE };
	CEdit	m_EditTmplName;
	CString	m_strName;
	CString	m_strSrcTempl;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTemplateCreateDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CTemplateCreateDlg)
	afx_msg void OnSelCopy();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TEMPLATECREATEDLG_H__C8C68E0F_9FAE_4A04_8FEC_465341FC88FA__INCLUDED_)
