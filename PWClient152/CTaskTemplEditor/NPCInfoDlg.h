#if !defined(AFX_NPCINFODLG_H__30B5D0AB_03A6_42A0_A715_02D736F52709__INCLUDED_)
#define AFX_NPCINFODLG_H__30B5D0AB_03A6_42A0_A715_02D736F52709__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// NPCInfoDlg.h : header file
//

#include "TaskTemplMan.h"
#include <vector>
#include <algorithm>

struct NPC_INFO_EX : public NPC_INFO
{
	CString name;
};

typedef std::vector<NPC_INFO_EX*> NPCInfoVec;

/////////////////////////////////////////////////////////////////////////////
// CNPCInfoDlg dialog

class CNPCInfoDlg : public CDialog
{
// Construction
public:
	CNPCInfoDlg(CWnd* pParent = NULL);   // standard constructor
	~CNPCInfoDlg();
	bool m_bReadOnly;
	NPCInfoVec npc_arr;

// Dialog Data
	//{{AFX_DATA(CNPCInfoDlg)
	enum { IDD = IDD_NPC_INFO };
	CListCtrl	m_NPCList;
	int		m_nUnMatch;
	CString	m_strNPCToFind;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CNPCInfoDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

public:
	bool LoadAndUpdateNPCInfo(bool bGetName);

// Implementation
protected:
	static bool LoadNPCInfo(TaskNPCInfoMap& npc_infos);
	bool SaveNPCInfo();

	// Generated message map functions
	//{{AFX_MSG(CNPCInfoDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnDblclkNpc(NMHDR* pNMHDR, LRESULT* pResult);
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnDestroy();
	afx_msg void OnFindNpc();
	afx_msg void OnCheckIn();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_NPCINFODLG_H__30B5D0AB_03A6_42A0_A715_02D736F52709__INCLUDED_)
