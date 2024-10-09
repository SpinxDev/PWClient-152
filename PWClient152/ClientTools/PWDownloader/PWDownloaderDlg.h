// PWDownloaderDlg.h : header file
//

#if !defined(AFX_PWDOWNLOADERDLG_H__3F55B384_5B98_4AFF_A73A_6476A58618EB__INCLUDED_)
#define AFX_PWDOWNLOADERDLG_H__3F55B384_5B98_4AFF_A73A_6476A58618EB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CPWDownloaderDlg dialog

class CPWDownloaderDlg : public CDialog
{
// Construction
public:
	CPWDownloaderDlg(CWnd* pParent = NULL);	// standard constructor
	virtual ~CPWDownloaderDlg();

// Dialog Data
	//{{AFX_DATA(CPWDownloaderDlg)
	enum { IDD = IDD_PWDOWNLOADER_DIALOG };
	CProgressCtrl	m_ctlProgress;
	CString	m_szProgress;
	CString	m_szDownSpeed;
	CString	m_szUpSpeed;
	//}}AFX_DATA

	// 是否已经下载完成
	bool IsDownloadOK() { return m_bDownloadOK; }

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPWDownloaderDlg)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;
	NOTIFYICONDATA m_nid;
	bool m_bDownloadOK;

	// Generated message map functions
	//{{AFX_MSG(CPWDownloaderDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnPipeData(WPARAM wParam, LPARAM lParam);
	afx_msg void OnClose();
	afx_msg void OnTrayIconNotify(WPARAM wParam, LPARAM lParam);
	afx_msg void OnSpeedSetting();
	afx_msg void OnMenuQuit();
	afx_msg void OnDestroy();
	afx_msg void OnNcPaint();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PWDOWNLOADERDLG_H__3F55B384_5B98_4AFF_A73A_6476A58618EB__INCLUDED_)
