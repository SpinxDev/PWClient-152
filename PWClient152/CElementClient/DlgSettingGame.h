/********************************************************************
	created:	2005/08/11
	created:	11:8:2005   17:50
	file name:	DlgSettingGame.h
	author:		yaojun
	
	purpose:	
*********************************************************************/


#pragma once

#include "DlgSetting.h"
#include "EC_Configs.h"

class CDlgSettingGame : public CDlgSetting  
{
	AUI_DECLARE_COMMAND_MAP()
	AUI_DECLARE_EVENT_MAP()
protected:

	// member used for DDX
	int m_nChanel;
	EC_GAME_SETTING m_setting;
protected:
	virtual bool OnInitDialog();
	virtual void OnShowDialog();
	virtual void OnChangeLayoutEnd(bool bAllDone);
	
	void DoDataExchange(bool bSave);
public:
	void OnLButtonUpChat(WPARAM, LPARAM, AUIObject *);
	void OnCommandSet(const char *szCommand);
	void OnCommandFunction(const char *szCommand);

	CDlgSettingGame();
	virtual ~CDlgSettingGame();

	virtual void SetToDefault();
	virtual void Apply();
	virtual void UpdateView();

	

	void SwitchCountryChannel();
};

