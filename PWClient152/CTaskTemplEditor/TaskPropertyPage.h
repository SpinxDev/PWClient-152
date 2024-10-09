#if !defined(AFX_TASKPROPERTYPAGE_H__8F47FE42_3379_487F_8C95_89FF679CABB9__INCLUDED_)
#define AFX_TASKPROPERTYPAGE_H__8F47FE42_3379_487F_8C95_89FF679CABB9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TaskPropertyPage.h : header file
//

#include "TaskTempl.h"

/////////////////////////////////////////////////////////////////////////////
// CTaskPropertyPage dialog

struct talk_proc;
class CTaskPropertyPage : public CPropertyPage
{
	DECLARE_DYNCREATE(CTaskPropertyPage)

// Construction
public:
	CTaskPropertyPage();
	~CTaskPropertyPage();

protected:
	ATaskTempl*	m_pTask;

public:
	BOOL UpdatePage(ATaskTempl* pTask, BOOL bUpdateData, BOOL bReadOnly = FALSE);
	void UpdateTalk(talk_proc* pTalk, int ts);

// Dialog Data
	//{{AFX_DATA(CTaskPropertyPage)
	enum { IDD = IDD_TASK_PROPERTY };
	CButton	m_BtnNeedRecord;
	CButton	m_BtnCanRedoAfterFailure;
	CButton	m_BtnCanRedo;
	BOOL	m_bCanRedoAfterFailure;
	BOOL	m_bCanRedo;
	DWORD	m_dwMaxTime;
	CString	m_strName;
	BOOL	m_bNeedRecord;
	BOOL	m_bMaxReceiver;
	BOOL	m_bCanGiveUp;
	BOOL	m_bFailAsKilled;
	BOOL	m_bChildInOrder;
	BOOL	m_bParentAlsoFail;
	DWORD	m_dwRcvNum;
	BOOL	m_bParentAlsoSucc;
	BOOL	m_bClearAsGiveUp;
	CString	m_strDescript;
	CString	m_strTribute;
	BOOL	m_bChooseOne;
	BOOL	m_bHasZone;
	CString	m_strId;
	BOOL	m_bAutoDelv;
	BOOL	m_bPromptWhenAutoDelv;
	CButton m_BtnPrompt;
	BOOL	m_bRandOne;
	BOOL	m_bDeathTrig;
	BOOL	m_bClearAcquired;
	DWORD	m_dwSuitedLev;
	BOOL	m_bKeyTask;
	BOOL	m_bShowPrompt;
	CString	m_strAwardNPC;
	CString	m_strDelvNPC;
	BOOL	m_bSkillTask;
	DWORD	m_dwWorldId;
	DWORD	m_dwTransWld;
	BOOL	m_bTransTo;
	CString	m_strSignature;
	int		m_nType;
	BOOL	m_bCanSeekOut;
	BOOL	m_bShowDirection;
	long	m_lMonsCtrl;
	BOOL	m_bTrigCtrl;
	BOOL	m_bAbsTime;
	int		m_nVailFrequency;
	int		m_nPeriodLimit;
	BOOL	m_bMarriage;
	BOOL    m_bSwitchSceneFail;
	BOOL    m_bHidden;
	BOOL	m_bLeaveRegion;
	int		m_iLeaveRegionID;
	BOOL	m_bEnterRegion;
	int		m_iEnterRegionID;
	BOOL	m_bOffLineFail;
	BOOL	m_bTaskFailTime;
	BOOL	m_bItemNotTakeOff;
	BOOL	m_bIsPQTask;
	BOOL	m_bIsPQSubTask;
	BOOL	m_bAccountTaskLimit;
	BOOL	m_bRoleTaskLimit;
//	int		m_iAccountTaskLimitCnt;

	BOOL	m_bDeliverySkill;
	int		m_iDeliverySkillID;
	int		m_iDeliverySkillLevel;

	BOOL	m_bShowGfx;
	BOOL	m_bChangePQRanking;
	BOOL	m_bCompareItemAndInventory;
	unsigned long	m_ulNeedInventorySlotNum;
	BOOL	m_bLeaveForceFail;
	BOOL	m_bLeaveFactionFail;
	BOOL	m_bNotClearItemWhenFailed;
	BOOL	m_bDisplayInTitleTaskUI;
	BOOL	m_bNotAddCountWhenFailed;
	BOOL	m_bUsedInTokenShop;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CTaskPropertyPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CTaskPropertyPage)
	afx_msg void OnCanRedo();
	afx_msg void OnCanRedoAfterFailure();
	afx_msg void OnEditTimetable();
	afx_msg void OnDeliverTalk();
	afx_msg void OnDeliverItemTalk();
	afx_msg void OnAcceptedTalk();
	afx_msg void OnAwardTalk();

	afx_msg void OnSelMaxPoint();
	afx_msg void OnShowTalk();
	afx_msg void OnSelAwardNpc();
	afx_msg void OnSelDelvNpc();
	afx_msg void OnTransPoint();
	afx_msg void OnEditChangeVal();

	afx_msg void OnEditEnterBig();
	afx_msg void OnEditLeaveBig();

	afx_msg void OnEditFailTime();
	afx_msg void OnPQValueDisplay();
	afx_msg void OnPQSubTaskProp();
	afx_msg void OnAutoDelvChecked();
	afx_msg void OnAccountLimitChecked();
	afx_msg void OnRoleLimitChecked();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TASKPROPERTYPAGE_H__8F47FE42_3379_487F_8C95_89FF679CABB9__INCLUDED_)
