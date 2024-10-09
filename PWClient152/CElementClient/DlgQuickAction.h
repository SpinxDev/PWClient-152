// Filename	: DlgQuickAction.h
// Creator	: Tom Zhou
// Date		: October 10, 2005

#pragma once

#include "DlgBase.h"
#include "AUIStillImageButton.h"

class CDlgQuickAction : public CDlgBase  
{
	AUI_DECLARE_COMMAND_MAP()

public:
	CDlgQuickAction();
	virtual ~CDlgQuickAction();

	void OnCommand_SecretMsg(const char * szCommand);
	void OnCommand_Report(const char * szCommand);
	void OnCommand_Extra(const char * szCommand);
	void OnCommand_Trade(const char * szCommand);
	void OnCommand_Whisper(const char * szCommand);
	void OnCommand_AddFriend(const char * szCommand);
	void OnCommand_InviteGroup(const char * szCommand);
	void OnCommand_InviteGuild(const char * szCommand);
	void OnCommand_Follow(const char * szCommand);
	void OnCommand_JoinChannel(const char * szCommand);
	void OnCommand_InviteChannel(const char * szCommand);
	void OnCommand_Bind(const char * szCommand);
	void OnCommand_Duel(const char * szCommand);
	void OnCommand_EPEquip(const char * szCommand);
	void OnCommand_CopyName(const char * szCommand);
	void OnCommand_ProfView(const char * szCommand);
	void OnCommand_CANCEL(const char * szCommand);
	void OnCommand_shop(const char * szCommand);

protected:
	PAUISTILLIMAGEBUTTON m_pInviteGuild;
	PAUISTILLIMAGEBUTTON m_pInviteChannel;
	PAUISTILLIMAGEBUTTON m_pInviteGroup;
	PAUISTILLIMAGEBUTTON m_pTrade;
	PAUISTILLIMAGEBUTTON m_pDuel;
	PAUISTILLIMAGEBUTTON m_pBind;
	PAUISTILLIMAGEBUTTON m_pshop;	
	PAUISTILLIMAGEBUTTON m_pExtra;

	virtual void OnTick();
	virtual bool OnInitDialog();
	virtual void OnShowDialog();
};
