#if !defined(AFX_RANKINGAWARD_H__84ED2232_18E9_44E5_8A69_E5746E532725__INCLUDED_)
#define AFX_RANKINGAWARD_H__84ED2232_18E9_44E5_8A69_E5746E532725__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// RankingAward.h : header file
//
#include "TaskTempl.h"

/////////////////////////////////////////////////////////////////////////////
// CRankingAward dialog

class CRankingAward : public CDialog
{
// Construction
public:
	CRankingAward(RANKING_AWARD* rAward = NULL,  CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CRankingAward)
	enum { IDD = IDD_RANKING_AWARD_EDIT };
	UINT	m_iRankStart;
	UINT	m_iRankEnd;
	long	m_lPeriod;
	UINT	m_iItemNum;
	UINT	m_ulItemID;
	//}}AFX_DATA

	bool m_bCommonItem;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRankingAward)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	RANKING_AWARD* m_pRankAward;
	// Generated message map functions
	//{{AFX_MSG(CRankingAward)
	afx_msg void OnSelItem();
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_RANKINGAWARD_H__84ED2232_18E9_44E5_8A69_E5746E532725__INCLUDED_)
