#ifndef _NPC_CONTOROLLER_DLG_
#define _NPC_CONTOROLLER_DLG_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// NpcControllerManDlg.h : header file
//

#include "NpcControllerManDlg.h"

/////////////////////////////////////////////////////////////////////////////
// CNpcControllerManDlg dialog

class CNpcControllerDlg : public CDialog
{
// Construction
public:
	CNpcControllerDlg(CWnd* pParent = NULL);   // standard constructor
	CONTROLLER_OBJECT m_Object;
	bool m_bReadOnly;
// Dialog Data
	//{{AFX_DATA(CNpcControllerDlg)
	enum { IDD = IDD_DIALOG_CONTROLLER_OBJECT };
	CComboBox	m_cb2Year;
	CComboBox	m_cb2Week;
	CComboBox	m_cb2Mouth;
	CComboBox	m_cb2Minute;
	CComboBox	m_cb2Hours;
	CComboBox	m_cb2Day;
	CComboBox	m_cb1Year;
	CComboBox	m_cb1Week;
	CComboBox	m_cb1Mouth;
	CComboBox	m_cb1Hours;
	CComboBox	m_cb1Minute;
	CComboBox	m_cb1Day;
	CString	m_strID;
	CString	m_strName;
	int		m_nStopTime;
	int		m_nActiveTime;
	BOOL	m_bIsInitActive;
	BOOL	m_bTime1Invailid;
	BOOL	m_bTime2Invailid;
	int		m_nControllerID;
	int		m_nActiveTimeRange;
	//}}AFX_DATA
	

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CNpcControllerDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnOK();
	//}}AFX_VIRTUAL

// Implementation
protected:
	void InitCombox();
	void EnableCombox1(bool bEnable);
	void EnableCombox2(bool bEnable);
	// Generated message map functions
	//{{AFX_MSG(CNpcControllerDlg)
	afx_msg void OnButtonBrowse();
	virtual BOOL OnInitDialog();
	afx_msg void OnCheck1Timeinvailid();
	afx_msg void OnCheck2Timeinvailid();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DYNAMICOBJECTDLG_H__CA748155_9A39_4712_9BF2_2E17E3F142B2__INCLUDED_)
