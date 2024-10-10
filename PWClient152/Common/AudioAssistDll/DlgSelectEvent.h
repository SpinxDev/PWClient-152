#pragma once
#include "afxcmn.h"
#include "..\AudioEditor\TreeCtrlEx.h"
#include "..\AudioEditor\EventDisplay.h"
#include "resource.h"
#include "afxwin.h"
#include "FAudioEngine.h"

namespace AudioEngine
{
	class Event;
	class EventManager;
	class EventSystem;
	class EventInstance;
}

using AudioEngine::Event;
using AudioEngine::EventManager;
using AudioEngine::EventSystem;
using AudioEngine::EventInstance;
using AudioEngine::VECTOR;

struct DynSizeCtrlMan;

// CDlgSelectEvent dialog

class CDlgSelectEvent : public CDialog
{
	DECLARE_DYNAMIC(CDlgSelectEvent)

public:
	CDlgSelectEvent(EventSystem* pEventSystem, const VECTOR& vListenerPos, CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgSelectEvent();
public:
	Event*							GetSelectEvent() const;
	const char*						GetSelectEventGuid() const;
	// return false if not exist or event not found
	bool							SetInitSelectEvent(const char* szGuid);

// Dialog Data
	enum { IDD = IDD_SELECT_EVENT };
protected:
	void release();
protected:
	virtual BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	afx_msg void OnBnClickedOk();
	afx_msg void OnNMDblclkTreeEvent(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLbnSelchangeProjList();
	afx_msg void OnBnClickedPlay();
	afx_msg void OnBnClickedStop();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedBtnNull();
	afx_msg void OnTimer(UINT_PTR nIDEvent);

	DECLARE_MESSAGE_MAP()
	CTreeCtrlEx m_wndTree;
	CListBox	m_ProjLst;
protected:
	CImageList						m_ImageList;
	Event*							m_pSelectEvent;
	EventSystem*					m_pEventSystem;
	CEventDisplay					m_EventDisplay;
	EventInstance*					m_pEventInstance;
	Event*							m_pPlayingEvent;
	DynSizeCtrlMan*					m_pDscMan;
	VECTOR							m_vListenerPos;
};
