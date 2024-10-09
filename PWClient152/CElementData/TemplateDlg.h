#if !defined(AFX_TEMPLATEDLG_H__C587AFA4_2C00_429C_8EB6_E26D5D62D4CC__INCLUDED_)
#define AFX_TEMPLATEDLG_H__C587AFA4_2C00_429C_8EB6_E26D5D62D4CC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TemplateDlg.h : header file
//
class CPropertyList;
/////////////////////////////////////////////////////////////////////////////
// CTemplateDlg dialog

class CTemplateDlg : public CDialog
{
// Construction
public:
	CTemplateDlg(CWnd* pParent = NULL);   // standard constructor
	void SetProperty(ADynPropertyObject *pProperty);
// Dialog Data
	//{{AFX_DATA(CTemplateDlg)
	enum { IDD = IDD_TEMPLATE_DLG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTemplateDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	CPropertyList *m_pList;
	
	// Generated message map functions
	//{{AFX_MSG(CTemplateDlg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TEMPLATEDLG_H__C587AFA4_2C00_429C_8EB6_E26D5D62D4CC__INCLUDED_)
