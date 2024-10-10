#pragma once

#include "resource.h"


// CDlgSetParamValue 对话框

class CDlgSetParamValue : public CBCGPDialog
{
	DECLARE_DYNAMIC(CDlgSetParamValue)

public:
	CDlgSetParamValue(float fValue, CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgSetParamValue();

// 对话框数据
	enum { IDD = IDD_SET_PARAM_VALUE };
public:
	float GetValue() const { return m_fValue; }

protected:
	virtual BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()

	
protected:
	float m_fValue;
public:
	afx_msg void OnBnClickedOk();
};
