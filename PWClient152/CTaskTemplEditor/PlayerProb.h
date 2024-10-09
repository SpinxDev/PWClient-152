#if !defined(AFX_PLAYERPROB_H__4C28A71F_9403_446E_8490_2B6A494B6B8A__INCLUDED_)
#define AFX_PLAYERPROB_H__4C28A71F_9403_446E_8490_2B6A494B6B8A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PlayerProb.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// PlayerProb dialog
struct PLAYER_WANTED;
class PlayerProb : public CDialog
{
// Construction
public:
	PlayerProb(PLAYER_WANTED* pPlayer, unsigned long ulIndex, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(PlayerProb)
	enum { IDD = IDD_PLAYER };
	int		m_ItemNum;
	float	m_DropRate;
	CString	m_ItemName;
//	CString	m_PlayerClass;
	UINT	m_ulPlayerNum;
	//}}AFX_DATA
	
	PLAYER_WANTED* m_pPlayerWanted;
	unsigned long m_ulIndex;
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(PlayerProb)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(PlayerProb)
	afx_msg void OnSelPlayer();
	afx_msg void OnSelItem();
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PLAYERPROB_H__4C28A71F_9403_446E_8490_2B6A494B6B8A__INCLUDED_)
