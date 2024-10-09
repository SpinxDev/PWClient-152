// File		: DlgChannelOption.h
// Creator	: Xiao Zhou
// Date		: 2005/10/25

#pragma once

#include "DlgBase.h"

class CDlgChannelOption : public CDlgBase  
{
	AUI_DECLARE_COMMAND_MAP()
public:
	void OnCommandWhisper(const char *szCommand);
	void OnCommandFriendInvite(const char *szCommand);
	void OnCommandGroupInvite(const char *szCommand);
	void OnCommandKick(const char *szCommand);

	CDlgChannelOption();
	virtual ~CDlgChannelOption();
};
