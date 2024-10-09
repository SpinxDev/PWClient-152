#if !defined(AFX_DLGEXPORTPATHINFO_H__0478B20F_B113_4E57_A003_C7A9AD1EE729__INCLUDED_)
#define AFX_DLGEXPORTPATHINFO_H__0478B20F_B113_4E57_A003_C7A9AD1EE729__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgExportPathInfo.h : header file
//
#include "DlgMapGrid.h"
#include "ProjList.h"

class CSeperateMapGridSelectPolicy;

/////////////////////////////////////////////////////////////////////////////
// CDlgExportPathInfo dialog

class CDlgExportPathInfo : public CDialog
{
// Construction
public:
	CDlgExportPathInfo(CWnd* pParent = NULL);   // standard constructor
	~CDlgExportPathInfo();

// Dialog Data
	//{{AFX_DATA(CDlgExportPathInfo)
	enum { IDD = IDD_EXPORT_PATH_INFO };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgExportPathInfo)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:	
	CMapGrid	m_mapGrid;
	CSeperateMapGridSelectPolicy* m_pMapGridSelectPolicy;	
	ProjList	m_projList;

	// Generated message map functions
	//{{AFX_MSG(CDlgExportPathInfo)
	virtual BOOL OnInitDialog();
	afx_msg void OnUnselectAll();
	afx_msg void OnSelectAll();
	afx_msg void OnExport();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGEXPORTPATHINFO_H__0478B20F_B113_4E57_A003_C7A9AD1EE729__INCLUDED_)
