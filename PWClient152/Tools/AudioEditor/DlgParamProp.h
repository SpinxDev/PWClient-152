#pragma once
#include "afxwin.h"

// CDlgParamProp �Ի���

class CDlgParamProp : public CBCGPDialog
{
	DECLARE_DYNAMIC(CDlgParamProp)

public:
	CDlgParamProp(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDlgParamProp();

// �Ի�������
	enum { IDD = IDD_PARAM_PROP };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();

	DECLARE_MESSAGE_MAP()
public:
	void SetProp(const char* szName, float fMinValue, float fMaxValue, float fVelocity, int iLoopMode, float fSeekSpeed);
	void GetProp(CString& strName, float& fMinValue, float& fMaxValue, float& fVelocity, int& iLoopMode, float& fSeekSpeed);
	
protected:
	CString m_strName;
	float m_fMinValue;
	float m_fMaxValue;
	float m_fVelocity;
	int m_iLoopMode;
	CComboBox m_wmdCombo;
	float m_fSeekSpeed;
};
