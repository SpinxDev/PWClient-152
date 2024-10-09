#if !defined(AFX_TASKMETHODPAGE_H__4A5806F3_3DDD_4561_838F_E88E06807907__INCLUDED_)
#define AFX_TASKMETHODPAGE_H__4A5806F3_3DDD_4561_838F_E88E06807907__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TaskMethodPage.h : header file
//

#include "TaskTempl.h"

/////////////////////////////////////////////////////////////////////////////
// CTaskMethodPage dialog

class CTaskMethodPage : public CPropertyPage
{
	DECLARE_DYNCREATE(CTaskMethodPage)

// Construction
public:
	CTaskMethodPage();
	~CTaskMethodPage();

protected:
	ATaskTempl*	m_pTask;
	unsigned long	m_ulNPCToProtect;
	unsigned long	m_ulNPCMoving;

public:
	BOOL UpdatePage(ATaskTempl* pTask, BOOL bUpdateData, BOOL bReadOnly = FALSE);

// Dialog Data
	//{{AFX_DATA(CTaskMethodPage)
	enum { IDD = IDD_TASK_METHOD };
	CComboBox	m_cmbEmotinAction;
	CComboBox	m_ItemsBox;
	int		m_nFinishType;
	int		m_nMethod;
	DWORD	m_dwTimeLen;
	CString	m_strNPCMoving;
	CString	m_strNPCToProtect;
	DWORD	m_dwWaitTime;
	DWORD	m_dwGoldWanted;
	DWORD	m_dwFactionContribWanted;
	DWORD	m_dwFactionExpContribWanted;
	DWORD	m_dwSiteId;
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
	DWORD	m_dwLeaveSiteId;
	int		m_iZoneStartX;
	int		m_iZoneStartZ;
	int		m_iZonesNumX;
	int		m_iZonesNumZ;
	int		m_iZoneSide;
	int		m_iTransformed;
	int		m_iReachLevel;
	int		m_iReachReincarnation;
	int		m_iReachRealmLevel;
	int		m_iEmotion;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CTaskMethodPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CTaskMethodPage)
	afx_msg void OnSelMonster();
	afx_msg void OnSelNpcMoving();
	afx_msg void OnSelNpcToProtect();
	afx_msg void OnEditItemsWanted();
	afx_msg void OnSelMaxPoint();
	afx_msg void OnShowTaskchar();
	afx_msg void OnSelMaxPoint2();
	afx_msg void OnSelPlayer();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	void ShowItemsWanted();

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TASKMETHODPAGE_H__4A5806F3_3DDD_4561_838F_E88E06807907__INCLUDED_)
