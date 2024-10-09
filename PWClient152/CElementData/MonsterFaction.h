#if !defined(AFX_MONSTERFACTION_H__1F6493D1_F01C_4484_BFBD_D1182A8FB206__INCLUDED_)
#define AFX_MONSTERFACTION_H__1F6493D1_F01C_4484_BFBD_D1182A8FB206__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MonsterFaction.h : header file
//

#include "AObject.h"

struct MonsterFaction : public CUSTOM_FUNCS
{
	~MonsterFaction() {}
	virtual BOOL CALLBACK OnActivate(void);
	virtual LPCTSTR CALLBACK OnGetShowString(void) const;
	virtual AVariant CALLBACK OnGetValue(void) const;
	virtual void CALLBACK OnSetValue(const AVariant& var);
	AVariant m_var;
	mutable AString  m_strShow;
};

/////////////////////////////////////////////////////////////////////////////
// CMonsterFaction dialog

class CMonsterFaction : public CDialog
{
// Construction
public:
	CMonsterFaction(CWnd* pParent = NULL);   // standard constructor

	unsigned long m_ulFactionsMask;

// Dialog Data
	//{{AFX_DATA(CMonsterFaction)
	enum { IDD = IDD_MONSTER_FACTIONS };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMonsterFaction)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CMonsterFaction)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MONSTERFACTION_H__1F6493D1_F01C_4484_BFBD_D1182A8FB206__INCLUDED_)
