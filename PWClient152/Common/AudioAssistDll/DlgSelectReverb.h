#pragma once
#include "afxwin.h"
#include "resource.h"

namespace AudioEngine
{
	class EventSystem;
	struct REVERB;
	class ReverbDataManager;
}

using AudioEngine::EventSystem;
using AudioEngine::REVERB;
using AudioEngine::ReverbDataManager;

class ReverbDataManager;

// CDlgSelectReverb dialog

class CDlgSelectReverb : public CDialog
{
	DECLARE_DYNAMIC(CDlgSelectReverb)

public:
	CDlgSelectReverb(EventSystem* pEventSystem, CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgSelectReverb();

// Dialog Data
	enum { IDD = IDD_SELECT_REVERB };
public:
	bool Load(const char* szPath);
	REVERB* GetSelectedReverb() const;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
protected:
	CListBox m_wndReverbListBox;
	ReverbDataManager* m_pReverbDataManager;
	REVERB* m_pSelectedReverb;
public:
	afx_msg void OnBnClickedOk();
};
