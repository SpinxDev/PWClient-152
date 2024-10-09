#if !defined(AFX_FACTIONSSEL_H__D7EDAA5C_71DB_4D76_B9EE_72091625B786__INCLUDED_)
#define AFX_FACTIONSSEL_H__D7EDAA5C_71DB_4D76_B9EE_72091625B786__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// FactionsSel.h : header file
//

#include "AObject.h"

struct FactionSel : public CUSTOM_FUNCS
{
	~FactionSel() {}
	virtual BOOL CALLBACK OnActivate(void);
	virtual LPCTSTR CALLBACK OnGetShowString(void) const;
	virtual AVariant CALLBACK OnGetValue(void) const;
	virtual void CALLBACK OnSetValue(const AVariant& var);
	AVariant m_var;
	mutable AString  m_strShow;
};

/////////////////////////////////////////////////////////////////////////////
// CFactionsSel dialog

class CFactionsSel : public CDialog
{
// Construction
public:
	CFactionsSel(CWnd* pParent = NULL);   // standard constructor

	unsigned long m_ulFactionsMask;

// Dialog Data
	//{{AFX_DATA(CFactionsSel)
	enum { IDD = IDD_FACTIONS_SEL };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFactionsSel)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CFactionsSel)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FACTIONSSEL_H__D7EDAA5C_71DB_4D76_B9EE_72091625B786__INCLUDED_)
