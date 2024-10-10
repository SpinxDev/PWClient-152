#pragma once
#include "afxwin.h"
#include <map>
#include "WndPlay.h"
#include "FSoundInstance.h"

using AudioEngine::AudioBankArchive;
using AudioEngine::Audio;
using AudioEngine::SoundInstance;
using AudioEngine::SoundInstanceTemplate;
using AudioEngine::SoundDef;

typedef std::map<int, Audio*> ItemAudioMap;

class CBankView : public CWnd, public SoundInstance::SoundInstanceListener
{
public:
	CBankView(void);
	~CBankView(void);
public:
	void SetTreeItem(HTREEITEM hItem);
protected:
	afx_msg void OnPaint();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnListCtrlRClick(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg LRESULT OnClickPlayBtn(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnClickStopBtn(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()
protected:
	void listItems(AudioBankArchive* pArchive);
	virtual bool OnStop(SoundInstance* pSoundInstance);
	void insertItem(Audio* pAudio);
protected:
	CBCGPListCtrl m_wndListCtrl;
	ItemAudioMap m_mapItemAudio;
	int m_iRClickItem;
	CWndPlay* m_pWndPlay;
	SoundInstance* m_pSoundInstance;
	SoundInstanceTemplate* m_pSoundInstanceTemplate;
	SoundDef* m_pSoundDef;
	bool m_bPlaying;
};
