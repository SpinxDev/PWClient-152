// File		: DlgBase.h
// Creator	: Xiao Zhou
// Date		: 2005/8/12

#pragma once

#include "DlgBase.h"

class AUIObject;
class AUIComboBox;
class AUIStillImageButton;
class AUILabel;

class CDlgBBS : public CDlgBase  
{
	friend class CDlgSystem2;
	friend class ObjectList;

	AUI_DECLARE_COMMAND_MAP()
	AUI_DECLARE_EVENT_MAP()

public:
	CDlgBBS();
	virtual ~CDlgBBS();
	bool OnInitDialog();
	virtual bool Release();
	virtual bool Tick();
	void OnShowDialog();
	
	void ChangeSize(int nWidth,int nHeight);

	void OnCommand_selectchange(const char * szCommand);
	void OnCommand_goback(const char * szCommand);
	void OnCommand_goahead(const char * szCommand);
	void OnCommand_refresh(const char * szCommand);
	void OnCommand_stop(const char * szCommand);
	void OnCommand_homepage(const char * szCommand);
	void OnCommand_close(const char * szCommand);
	void OnCommand_fontsize(const char * szCommand);
	void OnEvent_mousewheel(WPARAM wParam, LPARAM lParam, AUIObject *pObj);
	void OnEvent_mousemove(WPARAM wParam, LPARAM lParam, AUIObject *pObj);
	void OnEvent_lbuttonup(WPARAM wParam, LPARAM lParam, AUIObject *pObj);
private:
	AUIStillImageButton	*	m_pGoBackButton;
	AUIStillImageButton	*	m_pGoAheadButton;
	AUIStillImageButton	*	m_pRefreshButton;
	AUIStillImageButton	*	m_pStopButton;
	AUIStillImageButton	*	m_pCloseButton;
	AUIStillImageButton	*	m_pHomePageButton;
	AUILabel *				m_pFontTipLabel;
	AUIComboBox *			m_pFontSizeCombo;
	bool					m_bUseNewFontSize;
	bool					m_bInit;
	AString					m_aInitUrl;
	bool					m_bNeedClose;
	bool					m_bNeedStop;
	bool					m_bFirstOpen;
	bool					m_bShowBBS;
};
