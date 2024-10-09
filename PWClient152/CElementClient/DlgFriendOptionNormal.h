// File		: DlgFriendOptionNormal.h
// Creator	: Xiao Zhou
// Date		: 2005/8/18

#pragma once

#include "DlgBase.h"

class CDlgFriendOptionNormal : public CDlgBase  
{
	AUI_DECLARE_COMMAND_MAP()
public:
	void OnCommandCANCAL(const char *szCommand);
	void OnCommandAddFriend(const char *szCommand);
	void OnCommandAddGroup(const char *szCommand);
	void OnCommandSetAfk(const char *szCommand);
	void OnCommandOnOffline(const char *szCommand);

	CDlgFriendOptionNormal();
	virtual ~CDlgFriendOptionNormal();
};
