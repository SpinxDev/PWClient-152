#pragma once
#include "afxwin.h"


// CDlgDownloadRes dialog

class CDlgDownloadRes : public CBCGPDialog
{
	DECLARE_DYNAMIC(CDlgDownloadRes)

public:
	CDlgDownloadRes(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgDownloadRes();

// Dialog Data
	enum { IDD = IDD_DOWNLOAD_RES };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedBtnDownload();
	DECLARE_MESSAGE_MAP()

protected:
	CCheckListBox m_wndListBox;
};
