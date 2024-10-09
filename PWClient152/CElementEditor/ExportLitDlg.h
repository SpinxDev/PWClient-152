#if !defined(AFX_EXPORTLITDLG_H__9083364A_2392_4DAA_AD8E_64E1E822F1C6__INCLUDED_)
#define AFX_EXPORTLITDLG_H__9083364A_2392_4DAA_AD8E_64E1E822F1C6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ExportLitDlg.h : header file
//
#include "af.h"
#include "SceneLightObject.h"
/////////////////////////////////////////////////////////////////////////////
// CExportLitDlg dialog

class CExportLitDlg : public CDialog
{
// Construction
public:
	CExportLitDlg(CWnd* pParent = NULL);   // standard constructor
	bool ExpLitData();
	CProgressCtrl *m_pProgress;
// Dialog Data
	//{{AFX_DATA(CExportLitDlg)
	enum { IDD = IDD_DIALOG_EXP_LIT };
	CProgressCtrl	m_Progress;
	CString	m_strExpPath;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CExportLitDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnOK();
	virtual void OnCancel();
	//}}AFX_VIRTUAL

// Implementation
protected:
	bool ExpLitData(AFile *pFile,LIGHTDATA dat,A3DVECTOR3 pos);
	// Generated message map functions
	//{{AFX_MSG(CExportLitDlg)
	afx_msg void OnButtonBrowse();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_EXPORTLITDLG_H__9083364A_2392_4DAA_AD8E_64E1E822F1C6__INCLUDED_)
