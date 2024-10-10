#pragma once
#include "afxwin.h"
#include <map>
#include "WndPlay.h"
#include "FSoundInstance.h"

using AudioEngine::SoundInstance;
using AudioEngine::SoundInstanceTemplate;
using AudioEngine::SoundDef;
using AudioEngine::Audio;

class CWndPlay;

typedef std::map<int, Audio*> ItemAudioMap;

class CSoundDefView : public CWnd, public SoundInstance::SoundInstanceListener
{
public:
	CSoundDefView(void);
	~CSoundDefView(void);
public:
	void SetTreeItem(HTREEITEM hItem);
protected:	
	afx_msg void OnPaint();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg LRESULT OnClickPlayBtn(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnClickStopBtn(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()
protected:	
	virtual bool OnStop(SoundInstance* pSoundInstance);
	void insertItem(Audio* pAudio);
protected:
	CBCGPListCtrl m_wndListCtrl;
	CWndPlay* m_pWndPlay;
	int m_iRClickItem;
	SoundInstance* m_pSoundInstance;
	SoundInstanceTemplate* m_pSoundInstanceTemplate;
	SoundDef* m_pSoundDef;
	bool m_bPlaying;
	ItemAudioMap m_mapItemAudio;
};
