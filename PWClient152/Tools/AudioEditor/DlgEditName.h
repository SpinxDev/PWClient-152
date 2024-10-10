#pragma once

#include "resource.h"


// CDlgEditName 对话框

class CDlgEditName : public CBCGPDialog
{
	DECLARE_DYNAMIC(CDlgEditName)

public:
	CDlgEditName(const char* szTitle, CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgEditName();

// 对话框数据
	enum { IDD = IDD_EDIT_NAME };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();

	DECLARE_MESSAGE_MAP()
	CString m_strName;
	CString m_strTitle;
public:
	const CString&		GetName() const { return m_strName; }
	virtual BOOL OnInitDialog();
};
