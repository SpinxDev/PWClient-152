/*
 * File:	DlgSysModuleQuickBar.h
 * Created: WYD
 * Date:	2013/09/03
 */

#include "AUIObject.h"
#include "AUICheckBox.h"
#include "DlgBase.h"
#include "EC_IvtrTypes.h"
#include "EC_Shortcut.h"
#include "AUIImagePicture.h"

class CDlgSysModule: public CDlgBase
{
	AUI_DECLARE_EVENT_MAP()

public:
	~CDlgSysModule();
	void OnEventLButtonDown(WPARAM wParam, LPARAM lParam, AUIObject * pObj);
	void OnEventRButtonUp(WPARAM wParam, LPARAM lParam, AUIObject * pObj);
	
protected:
	virtual bool OnInitDialog();
	virtual void OnShowDialog();

protected:
	CECSCSysModule* m_sysmods[CECSCSysModule::FM_NUM];
	PAUIOBJECT m_pLbl_SysName[CECSCSysModule::FM_NUM];
};

//////////////////////////////////////////////////////////////////////////

class CDlgSysModQuickBar: public CDlgBase
{
	AUI_DECLARE_EVENT_MAP()
		AUI_DECLARE_COMMAND_MAP()

public:
	void OnCommand_add(const char * szCommand);
	void OnCommand_minus(const char * szCommand);
	void OnCommand_sysmod(const char* szCommand);
	void OnCommand_mini(const char * szCommand);
//	void OnCommand_CANCEL(const char * szCommand);
	
	void OnEventLButtonDown(WPARAM wParam, LPARAM lParam, AUIObject *pObj);

	PAUIOBJECT GetFirstEmptySlot();

	void SetMiniMode(bool bMini);
	bool GetMiniMode() const { return m_pChk_Mini->IsChecked();}

protected:
	void OnTick();
	virtual bool OnInitDialog();

	PAUIOBJECT m_pBtn_AddIcon;
	PAUIOBJECT m_pBtn_Add;
	PAUIOBJECT m_pBtn_Minus;
	PAUIOBJECT m_pBtn_Bg;
	PAUIIMAGEPICTURE m_pBtn_Item[SIZE_SYSMODSCSET];
	PAUICHECKBOX m_pChk_Mini;
	bool	m_bFlash;
};


//////////////////////////////////////////////////////////////////////////
