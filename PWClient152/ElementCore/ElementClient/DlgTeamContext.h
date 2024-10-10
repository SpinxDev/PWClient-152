// Filename	: DlgTeamContext.h
// Creator	: Tom Zhou
// Date		: September 1, 2005

#pragma once

#include "DlgBase.h"

class CDlgTeamContext : public CDlgBase  
{
	AUI_DECLARE_COMMAND_MAP()

public:
	CDlgTeamContext();
	virtual ~CDlgTeamContext();

	void OnCommand_OpenT(const char * szCommand);
	void OnCommand_GroupLeave(const char * szCommand);
	void OnCommand_Whisper(const char * szCommand);
	void OnCommand_MakeLeader(const char * szCommand);
	void OnCommand_Kick(const char * szCommand);
	void OnCommand_Trade(const char * szCommand);
	void OnCommand_Follow(const char * szCommand);
	void OnCommand_Assist(const char * szCommand);
	void OnCommand_Summon(const char * szCommand);
};
