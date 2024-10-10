#pragma once

// CDlgSetValue dialog

class CDlgSetValue : public CBCGPDialog
{
	DECLARE_DYNAMIC(CDlgSetValue)

public:
	CDlgSetValue(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgSetValue();

// Dialog Data
	enum { IDD = IDD_SET_VALUE };
public:
	void Init(float val, float min, float max, float param)
	{
		m_fVal = val;
		m_fMin = min;
		m_fMax = max;
		m_fParam = param;
	}

	float GetValue() const { return m_fVal; }
	float GetParam() const { return m_fParam; }

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	afx_msg void OnEnKillfocusEditValue();

	DECLARE_MESSAGE_MAP()
protected:
	float m_fVal;
	float m_fMin;
	float m_fMax;
	float m_fParam;
};
