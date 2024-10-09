#if !defined(AFX_AWARDRANKING_H__BD06E17E_289B_47F7_95BE_43926B904A23__INCLUDED_)
#define AFX_AWARDRANKING_H__BD06E17E_289B_47F7_95BE_43926B904A23__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AwardRanking.h : header file
//

#include "TaskTempl.h"
/////////////////////////////////////////////////////////////////////////////
// CAwardRanking dialog

class CAwardRanking : public CDialog
{
// Construction
public:
	CAwardRanking(AWARD_DATA* pAward, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CAwardRanking)
	enum { IDD = IDD_PQ_AWARD_RANKING};
	CListCtrl	m_RankingList;
	BOOL	m_bAwardByProf;
	//}}AFX_DATA

	int m_ulPQRankingAwardCnt;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAwardRanking)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	AWARD_DATA* m_pAward;

	void SortRankingList();

	// Generated message map functions
	//{{AFX_MSG(CAwardRanking)
	afx_msg void OnAdd();
	afx_msg void OnEdit();
	afx_msg void OnDel();
	virtual BOOL OnInitDialog();
	afx_msg void OnClose();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_AWARDRANKING_H__BD06E17E_289B_47F7_95BE_43926B904A23__INCLUDED_)
