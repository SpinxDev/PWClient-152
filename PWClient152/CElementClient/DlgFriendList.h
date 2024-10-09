// File		: DlgFriendList.h
// Creator	: Xiao Zhou
// Date		: 2005/8/18

#pragma once

#include "DlgBase.h"
#include "EC_Friend.h"

#include "AUITreeView.h"

class CDlgFriendList : public CDlgBase  
{
	friend class CDlgFriendColor;
	friend class CDlgFriendOptionGroup;
	friend class CDlgFriendOptionName;
	
	AUI_DECLARE_COMMAND_MAP()
	AUI_DECLARE_EVENT_MAP()

public:
	void OnCommandCANCAL(const char *szCommand);
	void OnCommandBlackList(const char *szCommand);
	void OnCommandAdd(const char *szCommand);
	void OnCommandRemove(const char *szCommand);
	void OnCommandAction(const char *szCommand);

	void OnEventButtonDBCLK(WPARAM wParam, LPARAM lParam, AUIObject * pObj);
	void OnEventRButtonUp(WPARAM wParam, LPARAM lParam, AUIObject * pObj);

	CDlgFriendList();
	virtual ~CDlgFriendList();

	void BuildFriendList(PAUIDIALOG pDlg = NULL);
	void FriendAction(int idPlayer, int idGroup, int idAction, int nCode);
	
	//	根据好友 id，生成此玩家在好友列表中的显示信息
	bool GetFriendToShow(int idFriend, ACString &strFriendName, ACString &strNameToShow, A3DCOLOR &clrShow, char &status);

	bool GetSelectedFriendID(int &idPlayer);
	bool GetSelectedFriendName(ACString &strFriendName);
	bool GetSelectedFriendGroup(int &idGroup);
	bool GetSelectedFriendRemarks(ACString &strRemarks);

	bool GetFriendStatus(P_AUITREEVIEW_ITEM pItem, char &status);
	bool SetFriendStatus(P_AUITREEVIEW_ITEM pItem, char status);

	int  GetFriendID(P_AUITREEVIEW_ITEM pItem);

	enum
	{
		FRIEND_ACTION_FRIEND_ADD = 0,
		FRIEND_ACTION_FRIEND_DELETE,
		FRIEND_ACTION_FRIEND_UPDATE,
		FRIEND_ACTION_GROUP_ADD,
		FRIEND_ACTION_GROUP_DELETE,
		FRIEND_ACTION_GROUP_RENAME,
		FRIEND_ACTION_INFO_REFRESH,
		FRIEND_ACTION_INFO_LEVEL,
		FRIEND_ACTION_INFO_AREA,
		FRIEND_ACTION_FRIEND_UPDATE_INPLACE,
	};

protected:
	virtual bool OnInitDialog();
	virtual void OnChangeLayoutEnd(bool bAllDone);

	P_AUITREEVIEW_ITEM SearchItem(int id, bool bSearchPlayer);

	//	根据玩家 id 在好友列表及缓冲区中查找玩家姓名
	bool GetFriendNameByID(int idFriend, ACString &strFriendName);

	//	根据好友列表的显示情况，查找玩家名称，用于聊天等
	bool GetFriendName(P_AUITREEVIEW_ITEM pItem, ACString &strFriendName);

	//	特殊聊天状态图标
	enum
	{
		IMAGE_GT_ONLINE = 0,
		IMAGE_GT_OFFLINE,
		IMAGE_NUM,
	};
	bool LoadImages();
	void ReleaseImages();
	bool IsImageReady() { return !m_pA2DSpriteImage.empty(); }
	ACString GetFriendInfoStr(int idFriend);
	abase::vector<A2DSprite*>	m_pA2DSpriteImage;
	
	AUITreeView *	m_pTvFriendList;
};
