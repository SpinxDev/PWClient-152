/********************************************************************
	created:	2005/08/11
	created:	11:8:2005   17:49
	file name:	DlgSettingVideo.h
	author:		yaojun
	
	purpose:	
*********************************************************************/

#pragma once

#include "DlgSetting.h"
#include "EC_Configs.h"

class AUISlider;
class AUICheckBox;

class CDlgSettingVideo : public CDlgSetting  
{
	AUI_DECLARE_COMMAND_MAP()
protected:
	AUISlider * m_pSldModeLimit;
	AUISlider * m_pSldSpEffect;
	AUICheckBox * m_pChk_Personize;

	// member for DDX
	EC_VIDEO_SETTING m_setting;

protected:

	virtual bool OnInitDialog();
	virtual void OnShowDialog();
	void DoDataExchange(bool bSave);
public:
	void OnCommandSpEffect(const char * szCommand);
	void OnCommandModeLimit(const char * szCommand);
	CDlgSettingVideo();
	virtual ~CDlgSettingVideo();

	virtual void SetToDefault();
	virtual void Apply();
	virtual void UpdateView();
};

