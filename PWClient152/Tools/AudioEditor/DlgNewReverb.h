#pragma once
#include "afxwin.h"

namespace AudioEngine
{
	struct REVERB;
}

using AudioEngine::REVERB;

// CDlgNewReverb dialog

class CDlgNewReverb : public CBCGPDialog
{
	DECLARE_DYNAMIC(CDlgNewReverb)

public:
	CDlgNewReverb(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgNewReverb();

// Dialog Data
	enum { IDD = IDD_NEW_REVERB };

protected:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	bool isExist(const char* szName);

	DECLARE_MESSAGE_MAP()
public:
	REVERB* GetSelectReverb() const;
	const char* GetName() const;
protected:
	CString m_strName;
	REVERB* m_pSelectReverb;
	CComboBox m_wndReverbCombo;
};
