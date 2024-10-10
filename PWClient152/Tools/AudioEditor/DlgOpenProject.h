#pragma once
#include "afxwin.h"


// CDlgOpenProject 对话框

class CDlgOpenProject : public CBCGPDialog
{
	DECLARE_DYNAMIC(CDlgOpenProject)

public:
	CDlgOpenProject(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgOpenProject();

// 对话框数据
	enum { IDD = IDD_OPEN_PROJECT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedDelProject();
	afx_msg void OnLbnDblclkListProjNames();
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
protected:
	CListBox m_wndListbox;
};
