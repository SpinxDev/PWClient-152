#if !defined(AFX_TASKPREMISEPAGE_H__58A6B212_805A_4A8B_B03A_A20329FCC4EA__INCLUDED_)
#define AFX_TASKPREMISEPAGE_H__58A6B212_805A_4A8B_B03A_A20329FCC4EA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TaskPremisePage.h : header file
//

#include "TaskTempl.h"

/////////////////////////////////////////////////////////////////////////////
// CTaskPremisePage dialog

class CTaskPremisePage : public CPropertyPage
{
	DECLARE_DYNCREATE(CTaskPremisePage)

// Construction
public:
	CTaskPremisePage();
	~CTaskPremisePage();

protected:
	ATaskTempl*	m_pTask;

public:
	BOOL UpdatePage(ATaskTempl* pTask, BOOL bUpdateData, BOOL bReadOnly = FALSE);

// Dialog Data
	//{{AFX_DATA(CTaskPremisePage)
	enum { IDD = IDD_TASK_PREMISE };
	CComboBox	m_ItemsBox;
	BOOL	m_bIsClan;
	int		m_nFactionRole;
	BOOL	m_bIsSpouse;
	BOOL	m_bIsTeam;
	CString	m_strPremCotask;
	DWORD	m_dwDeposit;
	long	m_lReputation;
	int		m_iFactionContrib;
	DWORD	m_dwPremLevMin;
	DWORD	m_dwPremLevMax;
	int		m_nGender;
	BOOL	m_bShowByDeposit;
	BOOL	m_bShowByGender;
	BOOL	m_bShowByItems;
	BOOL	m_bShowByLev;
	BOOL	m_bShowByOccup;
	BOOL	m_bShowByPreTask;
	BOOL	m_bShowByRepu;
	BOOL	m_bShowByFactionContrib;
	BOOL	m_bShowByTeam;
	BOOL	m_bShowByPeriod;
	BOOL	m_bShowBySpouse;
	BOOL	m_bShowByFaction;
	int		m_nCoTaskCond;
	int		m_nPeriod;
	BOOL	m_bIsGM;
	long	m_lRepuMax;
	BOOL	m_bNeedComp;
	int     m_nExp1AndOrExp2;
	int     m_nLeftType1;
	long	m_lLeftNum1;
	int     m_nCompOper1;
	int		m_nRightType1;
	long	m_lRightNum1;
	int     m_nLeftType2;
	long	m_lLeftNum2;
	int     m_nCompOper2;
	int		m_nRightType2;
	long	m_lRightNum2;
	BOOL	m_bIsShieldUser;
	DWORD	m_dwPremRMBMin;
	DWORD	m_dwPremRMBMax;
	int		m_iCharStartTime;
	BOOL	m_bShowByCharTime;
	BOOL	m_bCharTime;
	UINT	m_iCharTimeGreaterThan;
	int		m_nCurrTimeCond;
	CTime	m_tmCharEndTime;
	BOOL	m_bShowByRMB;
	BOOL	m_bIsLibraryTask;
	float		m_fLibraryTasksProbability;
	int		m_iWorldContribution;
	int		m_iNeedRecordTasksNum;
	BOOL	m_bShowByRecordTasksNum;
	BOOL	m_bIsUniqueStorageTask;
	int		m_iTransformed;
	BOOL	m_bShowByTransformed;
	BOOL	m_bCheckForce;
	int		m_iForce;
	BOOL	m_bShowByForce;
	int		m_iForceContrib;
	BOOL	m_bShowByForceContrib;
	int		m_iForceRepu;
	BOOL	m_bShowByForceRepu;
	int		m_iExp;
	BOOL	m_bShowByExp;
	int		m_iSP;
	BOOL	m_bShowBySP;
	int		m_iForceAL;
	BOOL	m_bShowByForceAL;

	BOOL	m_bEnableForce;
	BOOL	m_bEnableForcePro;

	BOOL	m_bWeddingOwner;
	BOOL	m_bShowByWeddingOwner;

	BOOL	m_bIsKing;
	BOOL	m_bShowByKing;

	BOOL	m_bNotInTeam;
	BOOL	m_bShowByNotInTeam;
	int		m_iTitleRequired;

	CComboBox m_cbForce;

	int		m_iHistoryIndexLow;
	int		m_iHistoryIndexHigh;

	BOOL	m_bCheckHistoryMaxLevel;
	BOOL	m_bCheckReincarnation;
	BOOL	m_bCheckRealmLevel;
	BOOL	m_bShowByReincarnation;
	BOOL	m_bShowByRealmLevel;
	int		m_iReincarnationMin;
	int		m_iReincarnationMax;
	int		m_iRealmLevelMin;
	int		m_iRealmLevelMax;
	BOOL	m_bCheckRealmExpFull;
	BOOL	m_bShowByCardCount;
	int		m_iGeneralCardCount;
	int		m_iGeneralCardRank;
	int		m_iGeneralCardRankCount;
	BOOL	m_bShowByCardRank;
	BOOL	m_bShowByHistoryStage;

	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CTaskPremisePage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CTaskPremisePage)
	afx_msg void OnSelCotask();
	afx_msg void OnEditItemsWanted();
	afx_msg void OnEditTeam();
	afx_msg void OnEditOccup();
	afx_msg void OnDeliverPremItem();
	afx_msg void OnSelPretask();
	afx_msg void OnSelMutexTask();
	afx_msg void OnChangeProficiency();
	afx_msg void OnCheckForce();
	afx_msg void OnCBForceSelectChange();
	afx_msg void OnSelTitle();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	void ShowItemsWanted();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TASKPREMISEPAGE_H__58A6B212_805A_4A8B_B03A_A20329FCC4EA__INCLUDED_)
