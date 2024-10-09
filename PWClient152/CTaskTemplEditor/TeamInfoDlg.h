#if !defined(AFX_TEAMINFODLG_H__6BAE2A6B_9AC1_4336_92BE_A6B5B89DE8D8__INCLUDED_)
#define AFX_TEAMINFODLG_H__6BAE2A6B_9AC1_4336_92BE_A6B5B89DE8D8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TeamInfoDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CTeamInfoDlg dialog

class ATaskTempl;
struct TEAM_MEM_WANTED;
class CTeamInfoDlg : public CDialog
{
// Construction
public:
	CTeamInfoDlg(ATaskTempl* pTempl, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CTeamInfoDlg)
	enum { IDD = IDD_TEAM_INFO };
	CListCtrl	m_MemList;
	BOOL	m_bRcvByTeam;
	BOOL	m_bSharedTask;
	BOOL	m_bSharedAchieved;
	BOOL	m_bAllFail;
	BOOL	m_bCapFail;
	BOOL	m_bDismAsSelfFail;
	BOOL	m_bCheckTeammate;
	float	m_fDist;
	BOOL	m_bCntByMemPos;
	BOOL	m_bRcvChckMemPos;
	float	m_fCntByMemDist;
	float	m_fRcvChckMemDist;
	BOOL	m_bCapSuccess;
	float	m_fSuccDist;
	BOOL	m_bAllSuccess;
	BOOL	m_bCoupleOnly;
	BOOL	m_bDistinguishedOcc;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTeamInfoDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	ATaskTempl* m_pTempl;

	void UpdateRow(int nRow, const TEAM_MEM_WANTED* pInfo);

	// Generated message map functions
	//{{AFX_MSG(CTeamInfoDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnAdd();
	afx_msg void OnDelete();
	afx_msg void OnEdit();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TEAMINFODLG_H__6BAE2A6B_9AC1_4336_92BE_A6B5B89DE8D8__INCLUDED_)
