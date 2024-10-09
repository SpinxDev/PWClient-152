// File		: DlgFriendOptionName.h
// Creator	: Xiao Zhou
// Date		: 2005/8/18

#pragma once

#include "DlgBase.h"

class CDlgFriendOptionName : public CDlgBase  
{
	AUI_DECLARE_COMMAND_MAP()

public:
	void OnCommandWhisper(const char *szCommand);
	void OnCommandInvite(const char *szCommand);
	void OnCommandChat(const char *szCommand);
	void OnCommandHistory(const char *szCommand);
	void OnCommandUpdate(const char *szCommand);
	void OnCommandMoveGroup(const char *szCommand);
	void OnCommandDeleteFriend(const char *szCommand);
	void OnCommandInviteChannel(const char *szCommand);
	void OnCommandJoinChannel(const char *szCommand);
	void OnCommandCopyName(const char *szCommand);
	void OnCommandChangeRemarks(const char *szCommand);

private:
	bool GetSelectedFriendName(ACString &strFriendName);
	bool GetSelectedFriendID(int &idPlayer);
	bool GetSelectedFriendGroup(int &idGroup);
	bool GetSelectedFriendRemarks(ACString &strRemarks);

};
