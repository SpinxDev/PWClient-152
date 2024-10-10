#pragma once
#include "bcgplistctrl.h"

typedef std::set<std::string>	ResPathSet;

// CDlgUploadRes dialog

class CDlgUploadRes : public CBCGPDialog
{
	DECLARE_DYNAMIC(CDlgUploadRes)

public:
	CDlgUploadRes(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgUploadRes();

// Dialog Data
	enum { IDD = IDD_UPLOAD_RES };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedBtnAddFile();
	afx_msg void OnBnClickedOk();

	DECLARE_MESSAGE_MAP()
protected:
	bool createDirs(const char* szSrcFilePath, const char* szDestRootPath);
protected:
	CBCGPListCtrl m_wndListCtrl;
	ResPathSet m_setResPath;
};
