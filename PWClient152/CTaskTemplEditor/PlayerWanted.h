#if !defined(AFX_PLAYERWANTED_H__CA751F81_F07E_4ACF_AF41_D8DB99B1953A__INCLUDED_)
#define AFX_PLAYERWANTED_H__CA751F81_F07E_4ACF_AF41_D8DB99B1953A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PlayerWanted.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// PlayerWanted dialog
struct PLAYER_WANTED;
class PlayerWanted : public CDialog
{
// Construction
public:
	PlayerWanted(PLAYER_WANTED* pPlayer, unsigned long* pCount, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(PlayerWanted)
	enum { IDD = IDD_PLAYER_WANTED };
	CListCtrl	m_PlayerList;
	//}}AFX_DATA
	PLAYER_WANTED*	m_pPlayerWanted;
	unsigned long* m_pCount;


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(PlayerWanted)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	void UpdateList();
	// Generated message map functions
	//{{AFX_MSG(PlayerWanted)
	afx_msg void OnAdd();
	afx_msg void OnDel();
	afx_msg void OnEditPlayer();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PLAYERWANTED_H__CA751F81_F07E_4ACF_AF41_D8DB99B1953A__INCLUDED_)
