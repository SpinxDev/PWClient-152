// File		: DlgSelect.h
// Creator	: Feng Ning
// Date		: 2010/10/25

#pragma once

#include "DlgBase.h"

class CECLoginUIMan;
class CECLoginPlayer;
class CDlgSelect : public CDlgBase  
{
	AUI_DECLARE_COMMAND_MAP();
	AUI_DECLARE_EVENT_MAP();

public:
	CDlgSelect();
	virtual ~CDlgSelect();

	void Switch(bool bShow);
	void EnableCreate(bool bEnable);	
	bool CanEnterGame();
	void EnableEnterGame(bool bEnable);

	void OnCommand_Confirm(const char* szCommand);
	void OnCommand_Cancel(const char* szCommand);
	void OnCommand_Quit(const char* szCommand);
	void OnCommand_CreateAccount(const char* szCommand);
	void OnCommand_Password(const char* szCommand);
	void OnCommand_Modify(const char* szCommand);
	void OnCommand_Restore(const char* szCommand);
	void OnCommand_Delete(const char* szCommand);
	void OnCommand_CancelTrade(const char* szCommand);
	void OnCommand_PagePrev(const char *szCommand);
	void OnCommand_PageNext(const char *szCommand);

	void SelectChar(int roleIndex);
	bool LoginSelectedChar();
	void OnCommand_SelectChar(const char* szCommand);

	void OnEventKeyDown(WPARAM wParam, LPARAM lParam, AUIObject *pObj);
	void OnEventLButtonUp_RdoChar(WPARAM wParam, LPARAM lParam, AUIObject *pObj);

	void RefreshCharacterList();
	void DeleteCharacter(int roleIndex);
	void RestoreCharacter(int roleIndex);

protected:
	virtual void OnShowDialog();
	virtual void OnHideDialog();
	virtual bool OnInitDialog();
	virtual void OnTick();
	virtual bool Render();
	virtual bool OnChangeLayout(PAUIOBJECT pMine, PAUIOBJECT pTheir);
	virtual void OnChangeLayoutEnd(bool bAllDone);
	virtual bool OnEventMap(UINT uMsg, WPARAM wParam, LPARAM lParam, PAUIOBJECT pObj);

	void RenderAccountTime();
	bool IsCannotCreate();
	bool IsRoleShown(int roleIndex);
	bool HavePrevPage();
	bool HaveNextPage();

	int		m_nBaseCharIndex;
};
