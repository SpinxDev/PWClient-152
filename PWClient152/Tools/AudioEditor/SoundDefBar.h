#pragma once

#include "TreeBar.h"
#include "WndPlay.h"
#include "FSoundInstance.h"
#include "SoundDefDisplay.h"

using AudioEngine::SoundDef;
using AudioEngine::SoundInstance;
using AudioEngine::SoundInstanceTemplate;
using AudioEngine::SoundDefArchive;
using AudioEngine::SoundDefList;

class CSoundDefBar : public CTreeBar, public SoundInstance::SoundInstanceListener
{
public:
	CSoundDefBar();
	virtual ~CSoundDefBar();

	// Implementation
public:
	bool				UpdateTree();
	virtual BOOL		SetItemText(HTREEITEM hItem, LPCTSTR lpszText);
	void				RecreateSoundInstance();
protected:
	//{{AFX_MSG(CSoundDefBar)
	virtual afx_msg void OnTreeRClick(NMHDR* pNMHDR, LRESULT* pResult);	
	afx_msg void OnNewArchive();
	afx_msg void OnNewEmptySoundDef();
	afx_msg void OnBatchNewSoundDef();
	afx_msg void OnDeleteArchive();
	afx_msg void OnDeleteSoundDef();
	afx_msg void OnBatchDeleteSoundDef();
	afx_msg void OnDeleteAudio();
	afx_msg void OnAddAudio();
	afx_msg void OnRefresh();
	afx_msg void OnAddAudioGroup();
	afx_msg void OnDeleteAudioGroup();
	afx_msg LRESULT OnClickPlayBtn(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnClickStopBtn(WPARAM wParam, LPARAM lParam);
	virtual afx_msg void OnTreeSelectChanged(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnPaint();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSavePreset();
	afx_msg void OnLoadPreset();
	afx_msg void OnMoveUpAudio();
	afx_msg void OnMoveDownAudio();
	afx_msg void OnMoveUpAudioGroup();
	afx_msg void OnMoveDownAudioGroup();
	afx_msg void OnTreeKeyDown(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
protected:
	virtual void		OnTreeLButtonUp(UINT nFlags, CPoint point);
	virtual bool		OnStop(SoundInstance* pSoundInstance);
	bool				ifCanDelete(SoundDef* pSoundDef);
	bool				reloadEventWhenSoundDefChanged(SoundDef* pSoundDef);
	void				getSubSoundDef(SoundDefArchive* pArchive, SoundDefList& listSoundDef);
	void				newArchive(HTREEITEM hItem);
	bool				deleteSoundDef(SoundDef* pSoundDef);
protected:
	HTREEITEM			m_hRClickItem;
	CWndPlay*			m_pWndPlay;
	bool				m_bPlaying;
	bool				m_bNeedRecreateSoundInstance;
	SoundInstance*		m_pSoundInstance;
	SoundInstanceTemplate* m_pSoundInstanceTemplate;
	SoundDef*			m_pLastPlaySoundDef;
	CSoundDefDisplay	m_soundDefDisplay;
};


/////////////////////////////////////////////////////////////////////////////
