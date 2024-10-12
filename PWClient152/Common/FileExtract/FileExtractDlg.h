// FileExtractDlg.h : header file
//

#if !defined(AFX_FILEEXTRACTDLG_H__C6813934_D266_40AB_B40C_4F8D7CA8F21A__INCLUDED_)
#define AFX_FILEEXTRACTDLG_H__C6813934_D266_40AB_B40C_4F8D7CA8F21A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class FileAnalyse;

/////////////////////////////////////////////////////////////////////////////
// CFileExtractDlg dialog

class CFileExtractDlg : public CDialog
{
// Construction
public:
	CFileExtractDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CFileExtractDlg)
	enum { IDD = IDD_FILEEXTRACT_DIALOG };
	CListBox m_ctlFileList;
	CString	m_szSearchText;
	int		m_nType;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFileExtractDlg)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;
	CToolTipCtrl m_TextTip;

	// Generated message map functions
	//{{AFX_MSG(CFileExtractDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnAnalyse();
	afx_msg void OnExtract();
	afx_msg void OnSaveFileList();
	afx_msg void OnClearList();
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnSelchangeAnalyseOpt();
	afx_msg void OnBtnSelect();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FILEEXTRACTDLG_H__C6813934_D266_40AB_B40C_4F8D7CA8F21A__INCLUDED_)
