#if !defined(AFX_PROCTYPEMASKDLG_H__B4735A56_EF42_4288_A710_F5F9758E2256__INCLUDED_)
#define AFX_PROCTYPEMASKDLG_H__B4735A56_EF42_4288_A710_F5F9758E2256__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ProcTypeMaskDlg.h : header file
//

#include "AObject.h"

struct ProcTypeMask : public CUSTOM_FUNCS
{
	~ProcTypeMask() {}
	virtual BOOL CALLBACK OnActivate(void);
	virtual LPCTSTR CALLBACK OnGetShowString(void) const;
	virtual AVariant CALLBACK OnGetValue(void) const;
	virtual void CALLBACK OnSetValue(const AVariant& var);
	AVariant m_var;
	mutable AString m_strShow;
};

/////////////////////////////////////////////////////////////////////////////
// CProcTypeMaskDlg dialog

class CProcTypeMaskDlg : public CDialog
{
// Construction
public:
	CProcTypeMaskDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CProcTypeMaskDlg)
	enum { IDD = IDD_PROC_TYPE_MASK };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

	unsigned long m_ulProcTypeMask;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CProcTypeMaskDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CProcTypeMaskDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PROCTYPEMASKDLG_H__B4735A56_EF42_4288_A710_F5F9758E2256__INCLUDED_)
