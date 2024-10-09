#if !defined(AFX_DLGMERGEPLAYERPASSMAP_H__1B1D4037_A009_4FE7_B6DA_A6C0E8E4E5F6__INCLUDED_)
#define AFX_DLGMERGEPLAYERPASSMAP_H__1B1D4037_A009_4FE7_B6DA_A6C0E8E4E5F6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgMergePlayerPassMap.h : header file
//
#include "DlgMapGrid.h"
#include "ProjList.h"

/////////////////////////////////////////////////////////////////////////////
// CDlgMergePlayerPassMap dialog

class CDlgMergePlayerPassMap : public CDialog
{
// Construction
public:
	CDlgMergePlayerPassMap(CWnd* pParent = NULL);   // standard constructor
	~CDlgMergePlayerPassMap();

// Dialog Data
	//{{AFX_DATA(CDlgMergePlayerPassMap)
	enum { IDD = IDD_MERGE_PLAYERPASSMAP };
	CComboBox	m_cbSelPPassMapLayer;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgMergePlayerPassMap)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	CMapGrid	m_mapGrid;
	CRangedMapGridSelectPolicy	* m_pMapGridSelectPolicy;	
	ProjList	m_projList;

	// Generated message map functions
	//{{AFX_MSG(CDlgMergePlayerPassMap)
	afx_msg void OnMerge();
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeComboLayer();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGMERGEPLAYERPASSMAP_H__1B1D4037_A009_4FE7_B6DA_A6C0E8E4E5F6__INCLUDED_)
