#if !defined(AFX_MOUSTERSTATDLG_H__AFD3A09E_C3D4_46CF_BC0A_ECFE7162111E__INCLUDED_)
#define AFX_MOUSTERSTATDLG_H__AFD3A09E_C3D4_46CF_BC0A_ECFE7162111E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MousterStatDlg.h : header file
//
#include <hashmap.h>
/////////////////////////////////////////////////////////////////////////////
// CMousterStatDlg dialog

struct STAT_MOUNSETER
{
	int nId;
	int nFixed;
	int nArea;
public:
	STAT_MOUNSETER(){ nId = 0; nFixed = 0; nArea = 0; }
};

class CMousterStatDlg : public CDialog
{
// Construction
public:
	CMousterStatDlg(CWnd* pParent = NULL);   // standard constructor
	abase::hash_map<int,bool> mapFilter;
	int    m_nSet;
// Dialog Data
	//{{AFX_DATA(CMousterStatDlg)
	enum { IDD = IDD_DIALOG_STAT };
	CListCtrl	m_listReport;
	CTabCtrl	m_TabSort;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMousterStatDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	void BuildMounsterTable();
	void BuildNpcTable();
	void BuildGatherTable();
	void BuildDynamicTable();

	void StatMounster();
	void StatNpc();
	void StatGather();
	void StatDynamic();

	void AddIDMounsterTable(abase::vector<STAT_MOUNSETER>& listTable, int id, int num, bool bFixed = false);
	bool IsStat(int id);

	// Generated message map functions
	//{{AFX_MSG(CMousterStatDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeTabSort(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MOUSTERSTATDLG_H__AFD3A09E_C3D4_46CF_BC0A_ECFE7162111E__INCLUDED_)
