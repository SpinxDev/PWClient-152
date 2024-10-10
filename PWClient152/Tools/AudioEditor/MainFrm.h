// MainFrm.h : interface of the CMainFrame class
//


#pragma once
#include "EventBar.h"
#include "SoundDefBar.h"
#include "BankBar.h"
#include "PropertyBar.h"
#include "OutputBar.h"

namespace AudioEngine
{
	class EventGroup;
}

class CMainFrame : public CBCGPFrameWnd
{
	
protected: // create from serialization only
	CMainFrame();
	DECLARE_DYNCREATE(CMainFrame)

// Attributes
public:

// Operations
public:

// Overrides
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

// Implementation
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // control bar embedded members
	CBCGPStatusBar			m_wndStatusBar;
	CBCGPMenuBar			m_wndMenuBar;
	CBCGPToolBar			m_wndToolBar;
	CEventBar				m_wndEventBar;
	CSoundDefBar			m_wndSoundDefBar;
	CBankBar				m_wndBankBar;
	CPropertyBar			m_wndProperty;
	COutputBar				m_wndOutput;
	bool					m_bOldModifyState;

	CBCGPToolBarImages	m_UserImages;
// Generated message map functions
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnViewCustomize();
	afx_msg LRESULT OnToolbarReset(WPARAM,LPARAM);
	void OnToolsViewUserToolbar (UINT id);
	void OnUpdateToolsViewUserToolbar (CCmdUI* pCmdUI);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnAppLook(UINT id);
	afx_msg void OnUpdateAppLook(CCmdUI* pCmdUI);
	afx_msg void OnFileNew();
	afx_msg void OnFileOpen();
	afx_msg void OnFileSave();
	afx_msg void OnUpdateFileSave(CCmdUI *pCmdUI);
	afx_msg void OnClose();
	afx_msg void OnReverbEditor();
	afx_msg void OnUploadRes();
	afx_msg void OnDownloadRes();
	afx_msg void OnSettings();
	afx_msg void OnExportText();
	afx_msg void OnExportBinaryProject();
	DECLARE_MESSAGE_MAP()

	UINT	m_nAppLook;
protected:
	bool closeProject();
	void exportEventGroup(AudioEngine::EventGroup* pEventGroup, FILE* pFile);
public:
	CEventBar*				GetEventBar() { return &m_wndEventBar; }
	CSoundDefBar*			GetSoundDefBar() { return &m_wndSoundDefBar; }
	CBankBar*				GetBankBar() { return &m_wndBankBar; }
	CPropertyBar*			GetPropertyBar() { return &m_wndProperty; }	
};