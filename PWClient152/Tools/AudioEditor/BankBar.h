#pragma once

#include "TreeBar.h"
#include "FAudioBank.h"
#include "BankDisplay.h"

using AudioEngine::Audio;
using AudioEngine::AudioContainer;
using AudioEngine::AudioList;
using AudioEngine::BANK_TYPE;

class CBankBar : public CTreeBar
{
public:
	CBankBar();
	virtual ~CBankBar();

	// Implementation
public:
	bool				UpdateTree();
	virtual BOOL		SetItemText(HTREEITEM hItem, LPCTSTR lpszText);
protected:
	//{{AFX_MSG(CBankBar)
	virtual afx_msg void OnTreeRClick(NMHDR* pNMHDR, LRESULT* pResult);	
	afx_msg void OnNewBank();
	afx_msg void OnNewArchive();
	afx_msg void OnDeleteBank();
	afx_msg void OnDeleteArchive();	
	afx_msg void OnAddAudio();
	afx_msg void OnDeleteAudio();
	afx_msg void OnBatchDeleteAudio();
	afx_msg void OnRefresh();
	virtual afx_msg void OnTreeSelectChanged(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnPaint();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnTreeKeyDown(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
protected:
	virtual void		OnTreeLButtonUp(UINT nFlags, CPoint point);
	bool				ifCanDelete(Audio* pAudio);
	void				getSubAudio(AudioContainer* pAudioContainer, AudioList& listAudio);
	bool				deleteAudio(Audio* pAudio);
protected:
	HTREEITEM			m_hRClickItem;
	CBankDisplay		m_bankDisplay;
};
