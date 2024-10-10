#pragma once

#include "resource.h"


// CDlgSetParamValue �Ի���

class CDlgSetParamValue : public CBCGPDialog
{
	DECLARE_DYNAMIC(CDlgSetParamValue)

public:
	CDlgSetParamValue(float fValue, CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDlgSetParamValue();

// �Ի�������
	enum { IDD = IDD_SET_PARAM_VALUE };
public:
	float GetValue() const { return m_fValue; }

protected:
	virtual BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()

	
protected:
	float m_fValue;
public:
	afx_msg void OnBnClickedOk();
};
