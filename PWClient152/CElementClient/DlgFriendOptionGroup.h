// File		: DlgFriendOptionGroup.h
// Creator	: Xiao Zhou
// Date		: 2005/8/18

#pragma once

#include "DlgBase.h"

class CDlgFriendOptionGroup : public CDlgBase  
{
	AUI_DECLARE_COMMAND_MAP()
public:
	void OnCommandCANCAL(const char *szCommand);
	void OnCommandAddFriend(const char *szCommand);
	void OnCommandAdd(const char *szCommand);
	void OnCommandAddGroup(const char *szCommand);
	void OnCommandRenameGroup(const char *szCommand);
	void OnCommandGroupColor(const char *szCommand);
	void OnCommandDeleteGroup(const char *szCommand);

	void OnEventButtonDBCLK(WPARAM wParam, LPARAM lParam, AUIObject * pObj);
	void OnEventRButtonUp(WPARAM wParam, LPARAM lParam, AUIObject * pObj);

	CDlgFriendOptionGroup();
	virtual ~CDlgFriendOptionGroup();
};
