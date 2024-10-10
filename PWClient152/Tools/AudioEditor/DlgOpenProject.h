#pragma once
#include "afxwin.h"


// CDlgOpenProject �Ի���

class CDlgOpenProject : public CBCGPDialog
{
	DECLARE_DYNAMIC(CDlgOpenProject)

public:
	CDlgOpenProject(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDlgOpenProject();

// �Ի�������
	enum { IDD = IDD_OPEN_PROJECT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedDelProject();
	afx_msg void OnLbnDblclkListProjNames();
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
protected:
	CListBox m_wndListbox;
};
