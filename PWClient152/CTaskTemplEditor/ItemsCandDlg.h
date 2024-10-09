#if !defined(AFX_ITEMSCANDDLG_H__73A23CC2_4503_45AE_9DA2_3DEBB3BE3852__INCLUDED_)
#define AFX_ITEMSCANDDLG_H__73A23CC2_4503_45AE_9DA2_3DEBB3BE3852__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ItemsCandDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CItemsCandDlg dialog

struct AWARD_DATA;
class CItemsCandDlg : public CDialog
{
// Construction
public:
	CItemsCandDlg(AWARD_DATA* pAWard, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CItemsCandDlg)
	enum { IDD = IDD_ITEMS_CAND };
	CListCtrl	m_Cands;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CItemsCandDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	AWARD_DATA* m_pAward;

	void InitList();

	// Generated message map functions
	//{{AFX_MSG(CItemsCandDlg)
	afx_msg void OnAdd();
	afx_msg void OnDel();
	afx_msg void OnEdit();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ITEMSCANDDLG_H__73A23CC2_4503_45AE_9DA2_3DEBB3BE3852__INCLUDED_)
