#if !defined(AFX_TEXTINPUTDLG_H__A02E6215_C8B2_4ACD_9844_29B82876B69E__INCLUDED_)
#define AFX_TEXTINPUTDLG_H__A02E6215_C8B2_4ACD_9844_29B82876B69E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TextInputDlg.h : header file
//

#include "AObject.h"

struct TextInputFuncs : public CUSTOM_FUNCS
{
	~TextInputFuncs() {}
	virtual BOOL CALLBACK OnActivate(void);
	virtual LPCTSTR CALLBACK OnGetShowString(void) const;
	virtual AVariant CALLBACK OnGetValue(void) const;
	virtual void CALLBACK OnSetValue(const AVariant& var);

	AVariant	m_var;
	AString		m_strText;
};

/////////////////////////////////////////////////////////////////////////////
// CTextInputDlg dialog

class CTextInputDlg : public CDialog
{
// Construction
public:
	CTextInputDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CTextInputDlg)
	enum { IDD = IDD_TEXT_INPUT };
	CString	m_strText;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTextInputDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CTextInputDlg)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TEXTINPUTDLG_H__A02E6215_C8B2_4ACD_9844_29B82876B69E__INCLUDED_)
