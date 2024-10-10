/********************************************************************
	created:	2005/08/11
	created:	11:8:2005   17:03
	file name:	DlgSetting.h
	author:		yaojun
	
	purpose:	
*********************************************************************/

#include "DlgBase.h"

#pragma once

class AUIStillImageButton;

class CDlgSetting : public CDlgBase
{
	AUI_DECLARE_COMMAND_MAP()
	AUI_DECLARE_EVENT_MAP()
protected:
	AUIStillImageButton * m_pBtnSystem;
	AUIStillImageButton * m_pBtnVideo;
	AUIStillImageButton * m_pBtnGame;
	AUIStillImageButton * m_pBtnQuickKey;
	AUIStillImageButton * m_pBtnDefault;

	bool m_bValidateSucceed;

protected:
	virtual void DoDataExchange(bool bSave);
public:
	void OnCommandCancel(const char * szCommand);
	void OnCommandConfirm(const char * szCommand);
	void OnCommandApply(const char * szCommand);
	void OnCommandDefault(const char * szCommand);
	void OnCommandSetting(const char * szCommand);

	// display default set
	virtual void SetToDefault() {}
	// update display with current globle setting
	virtual void UpdateView() {}
	// apply change
	virtual void Apply() {}
	// validate
	virtual bool Validate() { return true; }

	CDlgSetting();
	~CDlgSetting();
};