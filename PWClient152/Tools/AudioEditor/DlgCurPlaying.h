#pragma once
#include "afxcmn.h"
#include "resource.h"
#include "FAudioInstance.h"
#include "DynSizeCtrl.h"

using AudioEngine::AudioInstance;
using AudioEngine::EventInstance;

// CDlgCurPlaying dialog

class CDlgCurPlaying : public CBCGPDialog, AudioInstance::AudioInstanceListener
{
	DECLARE_DYNAMIC(CDlgCurPlaying)

public:
	CDlgCurPlaying(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgCurPlaying();

// Dialog Data
	enum { IDD = IDD_CUR_PLAYING };
public:
	void SetEventInstance(EventInstance* pEventInstance);
	virtual bool OnPlay(AudioInstance* pAudioInstance);
	virtual bool OnStop(AudioInstance* pAudioInstance);

protected:
	afx_msg void OnSize(UINT nType, int cx, int cy);
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()	
protected:
	CBCGPListCtrl m_wndListCtrl;
	DynSizeCtrlMan m_dscMan;
	EventInstance* m_pEventInstance;
};
