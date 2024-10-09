// File		: DlgCreate.h
// Creator	: Feng Ning
// Date		: 2010/10/20

#pragma once

#include "DlgBase.h"

//	选择职业
class CDlgCreateProfession : public CDlgBase 
{
	AUI_DECLARE_COMMAND_MAP();
	AUI_DECLARE_EVENT_MAP();
	
public:
	CDlgCreateProfession();

	void OnCommand_Race(const char *szCommand);
	void OnCommand_Prof(const char *szCommand);
	void OnCommand_Confirm(const char *szCommand);
	void OnCommand_Cancel(const char *szCommand);
	void OnCommand_PrevRace(const char *szCommand);
	void OnCommand_NextRace(const char *szCommand);
	
protected:
	virtual void OnShowDialog();
	virtual void OnHideDialog();
	virtual void OnTick();
	
	void OnEventLButtonDblClick_Prof(WPARAM wParam, LPARAM lParam, AUIObject *pObj);

	void SendCmdChooseProfession(int prof);
};


//	选择性别、输入角色名称
class CDlgCreateGenderName : public CDlgBase  
{
	AUI_DECLARE_COMMAND_MAP();
	AUI_DECLARE_EVENT_MAP();
	
public:
	
	void OnCommand_Gender(const char *szCommand);
	void OnCommand_Back(const char *szCommand);
	void OnCommand_Confirm(const char *szCommand);
	void OnCommand_Cancel(const char *szCommand);
	void OnCommand_SwitchEquip(const char *szCommand);

	void OnEventKeyDown(WPARAM wParam, LPARAM lParam, AUIObject *pObj);

	void OnCreateSucccess();

protected:
	virtual bool OnInitDialog();
	virtual void OnShowDialog();
	virtual void OnHideDialog();
	virtual void OnTick();

	bool		CanConfirm();
	ACString	GetCharName();
	void		SetCharName(const ACHAR *szName);
	void		FocusOnInput();
};