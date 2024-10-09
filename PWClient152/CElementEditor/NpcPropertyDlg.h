#if !defined(AFX_NPCPROPERTYDLG_H__BC062E22_C122_4E03_8497_3B03DE8B6092__INCLUDED_)
#define AFX_NPCPROPERTYDLG_H__BC062E22_C122_4E03_8497_3B03DE8B6092__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// NpcPropertyDlg.h : header file
//

#include "elementdataman.h"
#include "NumEdit.h"
#include "SceneAIGenerator.h"

/////////////////////////////////////////////////////////////////////////////
// CNpcPropertyDlg dialog

class CNpcPropertyDlg : public CDialog
{
// Construction
public:
	CNpcPropertyDlg(CWnd* pParent = NULL);   // standard constructor
	void Init(elementdataman *pData){ m_pDataMan = pData; };
// Dialog Data
	//{{AFX_DATA(CNpcPropertyDlg)
	enum { IDD = IDD_AI_PROPERTY };
	CNumEdit	m_editDisapearTime;
	CComboBox	m_cbSpeedType;
	CNumEdit	m_editPathID;
	CComboBox	m_cbLoopType;
	CNumEdit	m_editID;
	CEdit	m_editFresh;
	CEdit   m_editFreshMin;
	CNumEdit	m_editOffsetT;
	CNumEdit	m_editOffsetW;
	CNumEdit	m_editNum;
	CNumEdit	m_editDieNum;
	CComboBox	m_comboNpc;
	//}}AFX_DATA
	DWORD        m_dwInitiactive;
	NPC_PROPERTY m_npcData;
	bool  m_bModified;
	int          m_nLoopType;
	int          m_nSpeedType;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CNpcPropertyDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	DWORD GetMonsterID(AString strName);
	void  EnableNeedHelp(bool bNeed = true);
	elementdataman* m_pDataMan;

	// Generated message map functions
	//{{AFX_MSG(CNpcPropertyDlg)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg void OnRadioInitiactive();
	afx_msg void OnRadioNoactive();
	afx_msg void OnRadioNormal();
	afx_msg void OnSelchangeComboNpc();
	afx_msg void OnMonsterType();
	afx_msg void OnMonsterHelp();
	afx_msg void OnMonsterFrend();
	afx_msg void OnRadioHelpNeed();
	afx_msg void OnRadioHelpNo();
	afx_msg void OnMonsterDefault1();
	afx_msg void OnMonsterDefault2();
	afx_msg void OnMonsterDefault3();
	afx_msg void OnMonsterFind();
	afx_msg void OnSelchangeComboPathLoopType();
	afx_msg void OnSelPathBrowse();
	afx_msg void OnSelchangeComboPathSpeedType();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_NPCPROPERTYDLG_H__BC062E22_C122_4E03_8497_3B03DE8B6092__INCLUDED_)
