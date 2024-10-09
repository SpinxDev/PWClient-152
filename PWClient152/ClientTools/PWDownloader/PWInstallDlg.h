#if !defined(AFX_PWINSTALLDLG_H__12A7F145_35C6_428B_B043_A187FEDDE1EB__INCLUDED_)
#define AFX_PWINSTALLDLG_H__12A7F145_35C6_428B_B043_A187FEDDE1EB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PWInstallDlg.h : header file
//

#include "EC_Archive7Z.h"

/////////////////////////////////////////////////////////////////////////////
// CPWInstallDlg dialog

class CPWInstallDlg : public CDialog
{
// Construction
public:
	CPWInstallDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CPWInstallDlg();

// Dialog Data
	//{{AFX_DATA(CPWInstallDlg)
	enum { IDD = IDD_DLG_INSTALL };
	CProgressCtrl	m_ctlProgress;
	CString	m_szCurFile;
	CString	m_szProgress;
	//}}AFX_DATA

	// ½âÑ¹Ä¿Â¼
	CString m_sExtractDir;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPWInstallDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

	UINT64 m_nTotal;
	UINT64 m_nCompleted;
	CRITICAL_SECTION m_csLock;
	bool m_bInstallOK;

	void OnExtractComplete();
	
public:
	static CPWInstallDlg* s_pThis;
	static void OnExtractCallback(int op, IArchive7Z::PROPERTY& prop);

// Implementation
protected:
	HICON m_hIcon;
	Gdiplus::Image* m_pBG;

	bool LoadBGImage();

	// Generated message map functions
	//{{AFX_MSG(CPWInstallDlg)
	afx_msg void OnTimer(UINT nIDEvent);
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	virtual void OnCancel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PWINSTALLDLG_H__12A7F145_35C6_428B_B043_A187FEDDE1EB__INCLUDED_)
