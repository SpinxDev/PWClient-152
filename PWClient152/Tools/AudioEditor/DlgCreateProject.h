#pragma once


// CDlgCreateProject �Ի���

class CDlgCreateProject : public CBCGPDialog
{
	DECLARE_DYNAMIC(CDlgCreateProject)

public:
	CDlgCreateProject(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDlgCreateProject();

// �Ի�������
	enum { IDD = IDD_CREATE_PROJECT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	virtual BOOL OnInitDialog();
protected:
	CString m_strProjectName;
};
