/*
 * FILE: EC_Friend.h
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Duyuxin, 2005/7/12
 *
 * HISTORY: 
 *
 * Copyright (c) 2005 Archosaur Studio, All Rights Reserved.
 */

#pragma once

#include "hashtab.h"
#include "AArray.h"
#include "AAssist.h"
#include "A3DTypes.h"
#include "Network/gnetdef.h"

///////////////////////////////////////////////////////////////////////////
//	
//	Define and Macro
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Types and Global variables
//	
///////////////////////////////////////////////////////////////////////////

namespace GNET
{
	class GFriendInfo;
	class GetFriends_Re;
	class GetSavedMsg_Re;
	class FriendExtList;
}

///////////////////////////////////////////////////////////////////////////
//	
//	Declare of Global functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Class CECFriendMan
//	
///////////////////////////////////////////////////////////////////////////

class CECFriendMan
{
public:		//	Types

	struct FRIEND
	{
		int			id;
		int			iProfession;
		int			iGroup;			//	Group id
		char      status;
		int			nLevel;
		int			idArea;
		ACString	strName;

		const ACHAR* GetName() const{ return strName; }
		bool	IsGameOnline() const{return IsGameOnline(status); }
		bool	IsGTOnline() const{return IsGTOnline(status); }

		static bool IsGameOnline(char s);
		static bool IsGTOnline(char s);
	};

	struct FRIEND_EX
	{
		bool newFriendPlaceHolder;	//	新添加的朋友在下线再上前，没有FRIEND_EX信息，为方便设置备注，增加占位符
		int uid;
		int rid;
		int	level;
		long last_logintime;
		long update_time;
		int reincarnation_count;
		ACString strRemarks;
	};

	struct SEND_INFO
	{
		int rid;
		long sendmail_time;
	};

	struct GROUP
	{
		ACString	strName;	//	Group name
		int			iGroup;		//	Group id
		A3DCOLOR	color;		//	Group color
		
		APtrArray<FRIEND*>	aFriends;
	};

	struct MESSAGE
	{
		ACString	strName;	//	Sender name
		int			idSender;	//	Sender id
		ACString	strMsg;		//	Message
		BYTE		byFlag;		//	Flag
	};

	typedef abase::hashtab<FRIEND*, int, abase::_hash_function>	FriendTable;

public:		//	Constructor and Destructor

	CECFriendMan();
	virtual ~CECFriendMan();

public:		//	Attributes

public:		//	Operations

	//	Check whether manager has been initiailized
	bool CheckInit() { return m_aGroups.GetSize() ? true : false; }
	
	//	Reset friend list
	bool ResetFriends(GNET::GetFriends_Re& Data);
	//  Reset friend extra info list
	bool ResetFriendsEx(GNET::FriendExtList& Data);
	//	Add a friend
	FRIEND* AddFriend(const GNET::GFriendInfo& fi, char status);
	//	Remove a friend
	void RemoveFriend(int idFriend);
	//	Remove all friends
	void RemoveAllFriends();
	//	Change friend status
	void ChangeFriendStatus(int idFriend, char status);
	//	Change friend's group
	void ChangeFriendGroup(int idFriend, int idGroup);
	//	Get friend by ID
	FRIEND* GetFriendByID(int idFriend);	
	FRIEND_EX* GetValidFriendExByID(int idFriend);
	//	Get friend by name
	FRIEND* GetFriendByName(const ACHAR* szName);
	// Set friend level
	void SetFriendLevel(int idFriend, int nLevel);
	// Set friend area
	void SetFriendArea(int idFriend, int idArea);
	bool CanSetFriendRemarks(int idFriend);
	bool GetFriendRemarks(int idFriend, ACString &strRemarks);
	bool SetFriendRemarks(int idFriend, const ACHAR *szRemarks);

	//	Add group
	bool AddGroup(int idGroup, const ACHAR* szName);
	//	Remove group
	void RemoveGroup(int idGroup);
	//	Change group name
	void ChangeGroupName(int idGroup, const ACHAR* szName);
	//	Set group color
	void SetGroupColor(int idGroup, A3DCOLOR color);
	//	Get group number
	int GetGroupNum() { return m_aGroups.GetSize(); }
	//	Get group by index
	GROUP* GetGroupByIndex(int n) { return m_aGroups[n]; }
	//	Get group by id
	GROUP* GetGroupByID(int id);
	//  Get friend extra info
	abase::vector<FRIEND_EX*>& GetFriendEx(){return m_aFriendEx;}
	APtrArray<SEND_INFO*>& GetSendInfo(){return m_aSendInfo;}

	//	Reset offline message list
	bool ResetOfflineMsgs(GNET::GetSavedMsg_Re& p);
	//	Remove all offline messages
	void RemoveAllOfflineMsgs();
	//	Get offline message number
	int GetOfflineMsgNum() { return m_aOfflineMsgs.GetSize(); }
	//	Get offline message
	MESSAGE* GetOfflineMsg(int n) { return m_aOfflineMsgs[n]; }

protected:	//	Attributes

	APtrArray<GROUP*>	m_aGroups;		//	Group info
	FriendTable			m_FriendTab;	//	Friend table
	APtrArray<MESSAGE*>	m_aOfflineMsgs;	//	Offline messages
	abase::vector<FRIEND_EX*> m_aFriendEx;  //  Friend extra info
	APtrArray<SEND_INFO*> m_aSendInfo;  //  Friend send mail info

protected:	//	Operations
	FRIEND_EX* GetFriendExByID(int idFriend);
};

///////////////////////////////////////////////////////////////////////////
//	
//	Inline functions
//	
///////////////////////////////////////////////////////////////////////////



