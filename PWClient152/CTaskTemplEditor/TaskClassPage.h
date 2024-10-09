#if !defined(AFX_TASKCLASSPAGE_H__F4D0DA6C_5A5F_4E13_9806_B0B0BC351AA2__INCLUDED_)
#define AFX_TASKCLASSPAGE_H__F4D0DA6C_5A5F_4E13_9806_B0B0BC351AA2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TaskClassPage.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CTaskClassPage dialog

class ATaskTempl;
class CTaskClassPage : public CPropertyPage
{
	DECLARE_DYNCREATE(CTaskClassPage)

// Construction
public:
	CTaskClassPage();
	~CTaskClassPage();

// Dialog Data
	//{{AFX_DATA(CTaskClassPage)
	enum { IDD = IDD_TASK_CLASS_FORM };
	CString	m_strName;
	CString	m_strID;
	DWORD	m_dwSuitLev;
	DWORD	m_dwTimeLimit;
	CString	m_strDelvNoText;
	CString	m_strDelvNPCText;
	CString	m_strDelvYesText;
	CString	m_strDescript;
	CString	m_strFinishNPCText;
	CString	m_strFinishPlayerText;
	DWORD	m_dwLevMax;
	DWORD	m_dwLevMin;
	CString	m_strUnfinishedNPCText;
	CString	m_strUnfinishedPlayerText;
	CString	m_strDelvNPC;
	CString	m_strAwardNPC;
	CString	m_strPreTask;
	int		m_nType;
	BOOL	m_bTeamwork;
	DWORD	m_dwSpecialAward;
	BOOL	m_bCanSeekOut;
	int		m_nPeriod;
	BOOL	m_bAutoDeliver;
	BOOL	m_bShowDirection;
	//}}AFX_DATA

	ATaskTempl* m_pTask;

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CTaskClassPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CTaskClassPage)
	afx_msg void OnSelMonster();
	afx_msg void OnEditItemsWanted();
	afx_msg void OnAward();
	afx_msg void OnSelDelvNpc();
	afx_msg void OnSelAwardNpc();
	afx_msg void OnEditGivenItems();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	BOOL UpdatePage(ATaskTempl* pTask, BOOL bUpdateData, BOOL bReadOnly);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TASKCLASSPAGE_H__F4D0DA6C_5A5F_4E13_9806_B0B0BC351AA2__INCLUDED_)
