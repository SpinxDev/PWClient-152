#if !defined(AFX_EXTTEMPLATEIDSELDLG_H__D7652802_97CB_4C0E_B77A_DF7B59099682__INCLUDED_)
#define AFX_EXTTEMPLATEIDSELDLG_H__D7652802_97CB_4C0E_B77A_DF7B59099682__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ExtTemplateIDSelDlg.h : header file
//


#include "AObject.h"

struct SEL_EXT_FUNCS : public CUSTOM_FUNCS
{
	~SEL_EXT_FUNCS() {}
	virtual BOOL CALLBACK OnActivate(void);
	virtual LPCTSTR CALLBACK OnGetShowString(void) const;
	virtual AVariant CALLBACK OnGetValue(void) const;
	virtual void CALLBACK OnSetValue(const AVariant& var);
	
	AVariant	m_var;
	CString		m_strText;
	void Resume() { m_strText.Empty(); }
};


class CExtTemplIDSel
{
public:
	CExtTemplIDSel() {}
	virtual ~CExtTemplIDSel() {}

protected:
	SEL_EXT_FUNCS m_SelIDFuncs;

public:
	void SetValue(const AVariant& var) { m_SelIDFuncs.OnSetValue(var); }
	void* GetFuncsPtr() { return (void*)&m_SelIDFuncs; }
	
};

/////////////////////////////////////////////////////////////////////////////
// ExtTemplateIDSelDlg dialog

class ExtTemplateIDSelDlg : public CDialog
{
// Construction
public:
	ExtTemplateIDSelDlg(CWnd* pParent = NULL);   // standard constructor
	int m_nExtID;
// Dialog Data
	//{{AFX_DATA(ExtTemplateIDSelDlg)
	enum { IDD = IDD_DIALOG_EXT_SEL };
	CListBox	m_listExt;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(ExtTemplateIDSelDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnOK();
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(ExtTemplateIDSelDlg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_EXTTEMPLATEIDSELDLG_H__D7652802_97CB_4C0E_B77A_DF7B59099682__INCLUDED_)
