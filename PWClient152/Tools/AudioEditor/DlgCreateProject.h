#pragma once


// CDlgCreateProject 对话框

class CDlgCreateProject : public CBCGPDialog
{
	DECLARE_DYNAMIC(CDlgCreateProject)

public:
	CDlgCreateProject(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgCreateProject();

// 对话框数据
	enum { IDD = IDD_CREATE_PROJECT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	virtual BOOL OnInitDialog();
protected:
	CString m_strProjectName;
};
