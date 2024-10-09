#if !defined(AFX_DLGEXPORTBHT_H__FC89EDE6_D52B_4951_9172_705CCA700A32__INCLUDED_)
#define AFX_DLGEXPORTBHT_H__FC89EDE6_D52B_4951_9172_705CCA700A32__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgExportBHT.h : header file
//
#include "DlgMapGrid.h"
#include "ProjList.h"

class CSeperateMapGridSelectPolicy;

/////////////////////////////////////////////////////////////////////////////
// CDlgExportBHT dialog

class CDlgExportBHT : public CDialog
{
// Construction
public:
	CDlgExportBHT(CWnd* pParent = NULL);   // standard constructor
	~CDlgExportBHT();

// Dialog Data
	//{{AFX_DATA(CDlgExportBHT)
	enum { IDD = IDD_EXPORT_BHT };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

	void SetExportDir(const CString &strExportDir){
		m_strExportDir = strExportDir;
	}

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgExportBHT)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	
	CString		m_strExportDir;
	CMapGrid	m_mapGrid;
	CSeperateMapGridSelectPolicy* m_pMapGridSelectPolicy;	
	ProjList	m_projList;
	
	// Generated message map functions
	//{{AFX_MSG(CDlgExportBHT)
	virtual BOOL OnInitDialog();
	afx_msg void OnExport();
	afx_msg void OnSelectAll();
	afx_msg void OnUnselectAll();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGEXPORTBHT_H__FC89EDE6_D52B_4951_9172_705CCA700A32__INCLUDED_)
