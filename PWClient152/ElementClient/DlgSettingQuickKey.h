// Filename	: DlgSettingQuickKey.h
// Creator	: zhangyitian
// Date		: 2014/07/22

#ifndef _ELEMENTCLIENT_DLGSETTINGQUICKKEY_H_
#define _ELEMENTCLIENT_DLGSETTINGQUICKKEY_H_

// 快捷栏界面，放入设置界面中，代码多来自原先的DlgQuickKey

#include "DlgSetting.h"

#include "AUIListBox.h"

class CDlgSettingQuickKey : public CDlgSetting  
{
	AUI_DECLARE_EVENT_MAP()
	AUI_DECLARE_COMMAND_MAP()

public:
	CDlgSettingQuickKey();
	
	virtual void SetToDefault();
	virtual void Apply();

	// 确认操作与基类的流程可能不同，故重新写一个
	void OnCommandConfirm(const char *szCommand);

	void OnCommandKeyboard(const char *szCommand);
	
	void OnEventRButtonUp_KeyList(WPARAM wParam, LPARAM lParam, AUIObject *pObj);
	
	bool CustomizeHotKey(int iUsageCustomize, int nHotKey, DWORD dwModifier);
	void CustomizeHotKey(int iUsageCustomize, int iUsage);;
	
	void ConfirmHotKeyCustomize(bool hideAfterConfirm=false);
	
protected:
	virtual bool OnInitDialog();
	virtual void OnShowDialog();
	virtual void OnHideDialog();
	virtual void OnTick();
	
	void UpdateHotKeyDisplay(int nSel=-1);
	int  FindItemWithUsage(int iUsage);
	
private:
	PAUILISTBOX m_pLst_Keylist;
	int m_nLastSelected;
	
	// Temporary for customize usage
	int m_iUsageCustomize;
	int m_iUsage;
	int m_nHotKey;
	DWORD m_dwModifier;
};
	
#endif