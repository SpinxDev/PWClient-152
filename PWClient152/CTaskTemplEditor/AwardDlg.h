#if !defined(AFX_AWARDDLG_H__D02B47F0_AAFC_4A49_B34A_5F4D076581D0__INCLUDED_)
#define AFX_AWARDDLG_H__D02B47F0_AAFC_4A49_B34A_5F4D076581D0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AwardDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CAwardDlg dialog

struct AWARD_DATA;
class CAwardDlg : public CDialog
{
// Construction
public:
	CAwardDlg(AWARD_DATA* pAward, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CAwardDlg)
	enum { IDD = IDD_AWARD };
	DWORD	m_dwExp;
	DWORD	m_dwGold;
	CString	m_strNewTask;
	long	m_lRepu;
	DWORD	m_dwSP;
	int		m_iFactionContrib;
	int		m_iFactionExpContrib;
	int		m_nPeriod;
	DWORD	m_dwStation;
	DWORD	m_dwStorehouse;
	DWORD	m_dwFury;
	DWORD	m_dwTransWld;
	DWORD	m_dwPetNum;
	long	m_lMonsCtrl;
	BOOL	m_bTrigCtrl;
	long	m_lInventorySize;
	BOOL	m_bUseLevCo;
	BOOL	m_bDivorce;
	BOOL	m_bSendMsg;
	int		m_nMsgChannel;
	DWORD	m_dwStorehouse2;
	DWORD	m_dwStorehouse3;
	BOOL    m_bMulti;
	int     m_nNumType;
	long    m_lNum;
	BOOL	m_bDeath;
	int		m_nDeathType;
	long	m_lSpecifyTaskID;
	long	m_lSpecifySubTaskID;
	long	m_lSpecifyTaskContrib;
	long	m_lRandContrib;
	long	m_lLowestContrib;
	long	m_iContrib;
	DWORD	m_dwStorehouse4;
	long	m_lDividend;
	
	BOOL	m_bAwardSkill;
	int		m_iAwardSkillID;
	int		m_iAwardSkillLevel;

	int		m_iForceActivity;
	int		m_iForceContrib;
	int		m_iForceRepu;
	int		m_iForceSetRepu;
	
	BOOL	m_bTaskLimit;
	int		m_iRealmExp;
	BOOL	m_bExpandRealmLevelMax;
	int		m_iLeaderShip;
	int		m_iWorldContribution;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAwardDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	AWARD_DATA* m_pAward;

	// Generated message map functions
	//{{AFX_MSG(CAwardDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelNewTask();
	afx_msg void OnEditItemsWanted();
	virtual void OnOK();
	afx_msg void OnTransPt();
	afx_msg void OnAwardChangeValue();
	afx_msg void OnAwardHistoryChangeValue();
	afx_msg void OnAwardDisplayValue();
	afx_msg void OnAwardSummonMonster();
	afx_msg void OnPQRankingAward();
	afx_msg void OnTitleAward();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_AWARDDLG_H__D02B47F0_AAFC_4A49_B34A_5F4D076581D0__INCLUDED_)
