// Filename	: DlgInputName.h
// Creator	: Tom Zhou
// Date		: October 13, 2005

#pragma once

#include "DlgBase.h"
#include "AUIEditBox.h"

class CDlgInputName : public CDlgBase  
{
	friend class CDlgGuildMan;
	friend class CDlgMiniMap;
	friend class CDlgWorldMap;
	friend class CDlgFriendList;
	friend class CDlgFriendOptionGroup;
	friend class CDlgFriendOptionNormal;
	friend class CDlgChannelChat;
	friend class CDlgFriendOptionName;

	AUI_DECLARE_COMMAND_MAP()

public:
	CDlgInputName();
	virtual ~CDlgInputName();

	void OnCommand_confirm(const char * szCommand);
	void OnCommand_CANCEL(const char * szCommand);

	enum enumInputNameUsage
	{
		INPUTNAME_NULL = 0,
		INPUTNAME_MAKE_MARK,
		INPUTNAME_RENAME_MARK,
		INPUTNAME_RENAME_WORLD_MARK,
		INPUTNAME_ADD_FRIEND,
		INPUTNAME_ADD_GROUP,
		INPUTNAME_ADD_GUILD,
		INPUTNAME_RENAME_GROUP,
		INPUTNAME_FACTION_TITLE,
		INPUTNAME_CHANNEL_INVITE,
		INPUTNAME_FRIEND_CHANGE_REMARKS,
	};
	void ShowForUsage(enumInputNameUsage usage, const ACHAR *defaultText=NULL, int nMaxLength=-1, void *pvData=NULL, const char *pvDataName=NULL);
	
protected:
	virtual bool OnInitDialog();
	
	PAUIEDITBOX	m_pTxtInput;
	PAUIOBJECT	m_pTxtTitle;
};
