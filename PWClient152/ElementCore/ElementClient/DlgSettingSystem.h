/********************************************************************
	created:	2005/08/11
	created:	11:8:2005   17:49
	file name:	DlgSettingSystem.h
	author:		yaojun
	
	purpose:	
*********************************************************************/


#pragma once

#include "DlgSetting.h"
#include "EC_Configs.h"
#include <vector.h>

#include "AUIComboBox.h"
#include "AUILabel.h"
#include "AUISlider.h"
#include "AUICheckBox.h"

class CDlgSettingSystem : public CDlgSetting  
{
	AUI_DECLARE_COMMAND_MAP()
protected:
	EC_SYSTEM_SETTING m_setting;
	struct Res {int nWidth; int nHeight;};
	abase::vector<Res> m_vecRes;
	bool m_bIncludeCustomSize;

	// member for DDX
	AUIComboBox * m_pComboRes;
	AUIComboBox * m_pComboFS;
	AUILabel * m_pLabelAdvanceWater;
	AUISlider * m_pSldBright;
	AUISlider * m_pSldWater;
	AUICheckBox * m_pChkWideScreen;
	AUILabel * m_pLabelWideScreen;

protected:
	// ajdust control state (eg. enable/disable)
	// according to ddx member
	void UpdateControlState();

	// fill fs combobox
	void FillFSCombo();
	// fill res combobox and update m_bIncludeCustomSize which
	// indicating whether the current res is customized type
	void RefillResCombo();

	void ComboIndex2WidthHeight(int nIndex, int &nWidth, int &nHeight);
	void WidthHeight2ComboIndex(int &nIndex, int nWidth, int nHeight);

	virtual void DoDataExchange(bool bSave);
	virtual bool OnInitDialog();
	virtual void OnShowDialog();
public:
	void OnCommandRes(const char * szCommand);
	void OnCommandSFX(const char * szCommand);
	void OnCommandMusic(const char * szCommand);
	void OnCommandWater(const char * szCommand);
	void OnCommandBright(const char * szCommand);
	void OnCommandTotal(const char * szCommand);
	void OnCommandCancel(const char *szCommand);

	// change setting that should be changed immedialtely
	// but not after apply
	void MakeChangeNow();

	virtual void SetToDefault();
	virtual void Apply();
	virtual void UpdateView();
	virtual bool Validate();
	
	CDlgSettingSystem();
	virtual ~CDlgSettingSystem();

};

