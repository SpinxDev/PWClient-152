#pragma once
#include "afxwin.h"
#include "WndPlay.h"
#include "FEventInstance.h"
#include "DlgCurPlaying.h"

class CWndEvent;

using AudioEngine::Event;
using AudioEngine::EventInstance;

class CEventView : public CWnd, public EventInstance::EventInstanceListener
{
public:
	CEventView(void);
	~CEventView(void);
public:
	void SetEvent(Event* pEvent);
	void RecreateEventInstance();
public:
	afx_msg LRESULT OnClickPlayBtn(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnClickPauseBtn(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnClickStopBtn(WPARAM wParam, LPARAM lParam);
protected:	
	afx_msg void OnPaint();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);	
	afx_msg LRESULT OnClickPlayingBtn(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()
protected:
	virtual bool		OnStop(EventInstance* pEventInstance);
	void				setMuteLayers();
	void				setEffectEnable();
protected:
	CWndPlay* m_pWndPlay;
	bool m_bPlaying;
	bool m_bNeedRecreateEventInstance;
	Event* m_pEvent;
	Event* m_pLastPlayEvent;
	EventInstance* m_pEventInstance;
	CWndEvent* m_pWndEvent;
	CDlgCurPlaying m_dlgCurPlaying;
};
