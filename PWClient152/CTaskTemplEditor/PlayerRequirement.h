#if !defined(AFX_PLAYERREQUIREMENT_H__99781B05_C979_4FF0_94A9_DE8E25F7FB7B__INCLUDED_)
#define AFX_PLAYERREQUIREMENT_H__99781B05_C979_4FF0_94A9_DE8E25F7FB7B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PlayerRequirement.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// PlayerRequirement dialog
class Kill_Player_Requirements;
class PlayerRequirement : public CDialog
{
// Construction
public:
	PlayerRequirement(Kill_Player_Requirements* pRequirement, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(PlayerRequirement)
	enum { IDD = IDD_PLAYER_REQUIREMENT };
	BOOL	m_bWuxia;
	BOOL	m_Jianling;
	BOOL	m_Cike;
	BOOL	m_Yuling;
	BOOL	m_Yaoshou;
	BOOL	m_Fashi;
	BOOL	m_Meiling;
	BOOL	m_Wushi;
	BOOL	m_Yumang;
	BOOL	m_Yaojing;
	BOOL	m_YeYing;
	BOOL	m_YueXian;

	BOOL	m_Anyin;
	BOOL	m_Huaguang;
	BOOL	m_Huiye;
	UINT	m_MinLevel;
	UINT	m_MaxLevel;
	int m_nGender;
	//}}AFX_DATA

	Kill_Player_Requirements* m_pRequirement;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(PlayerRequirement)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(PlayerRequirement)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PLAYERREQUIREMENT_H__99781B05_C979_4FF0_94A9_DE8E25F7FB7B__INCLUDED_)
