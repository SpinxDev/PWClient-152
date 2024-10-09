#if !defined(AFX_SINGLETEXTDLG_H__702B45A7_1CA1_41E8_9976_47BC1EC151D0__INCLUDED_)
#define AFX_SINGLETEXTDLG_H__702B45A7_1CA1_41E8_9976_47BC1EC151D0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SingleTextDlg.h : header file
//

#include "AObject.h"

struct SingleTextFuncs : public CUSTOM_FUNCS
{
	~SingleTextFuncs() {}
	virtual BOOL CALLBACK OnActivate(void);
	virtual LPCTSTR CALLBACK OnGetShowString(void) const;
	virtual AVariant CALLBACK OnGetValue(void) const;
	virtual void CALLBACK OnSetValue(const AVariant& var);

	AVariant	m_var;
	AString		m_strText;
};

/////////////////////////////////////////////////////////////////////////////
// CSingleTextDlg dialog

class CSingleTextDlg : public CDialog
{
// Construction
public:
	CSingleTextDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CSingleTextDlg)
	enum { IDD = IDD_SINGLE_TEXT };
	CString	m_strText;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSingleTextDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSingleTextDlg)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SINGLETEXTDLG_H__702B45A7_1CA1_41E8_9976_47BC1EC151D0__INCLUDED_)
