#pragma once

#include "resource.h"


// CDlgEditName �Ի���

class CDlgEditName : public CBCGPDialog
{
	DECLARE_DYNAMIC(CDlgEditName)

public:
	CDlgEditName(const char* szTitle, CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDlgEditName();

// �Ի�������
	enum { IDD = IDD_EDIT_NAME };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();

	DECLARE_MESSAGE_MAP()
	CString m_strName;
	CString m_strTitle;
public:
	const CString&		GetName() const { return m_strName; }
	virtual BOOL OnInitDialog();
};
