// Filename	: EC_GameTalk.h
// Creator	: Xu Wenbin
// Date		: 2011/01/28

#pragma once

#include <AAssist.h>
#include <hashmap.h>
#include "Network/gnetdef.h"
#include "EC_Friend.h"

//
//	类 CECGameTalk: 记录发自 GT 等平台的玩家 ID、名称等情况，用于回复
//
class CECGameTalk
{
public:

	static CECGameTalk &Instance();

	void Set(int id, GNET::PRIVATE_CHANNEL channel);
	void Del(int id);
	void Clear();

	bool Get(int id, GNET::PRIVATE_CHANNEL &channel)const;

	GNET::PRIVATE_CHANNEL GetChannelToSend(int id, bool bForAutoReply, bool bFriendWhisper)const;
	GNET::PRIVATE_CHANNEL GetChannelToSend(const ACString &strName, bool bForAutoReply, bool bFriendWhisper, int &id)const;

private:

	CECGameTalk();
	CECGameTalk(const CECGameTalk &);
	CECGameTalk & operator = (const CECGameTalk &);

	//	查询好友辅助函数
	typedef CECFriendMan::FRIEND FRIEND;
	static FRIEND* GetFriend(int id);
	static FRIEND* GetFriend(const ACString &name);

	//	查询频道辅助函数
	static GNET::PRIVATE_CHANNEL GetChannelToAutoReply(GNET::PRIVATE_CHANNEL lastChannel);
	static GNET::PRIVATE_CHANNEL DirectReplyChannel(GNET::PRIVATE_CHANNEL lastChannel);

	//	每条有效记录
	struct Record
	{
		GNET::PRIVATE_CHANNEL channel;
	};

	//	所有最近一次从GT、WEB发来过聊天信息的玩家列表
	typedef abase::hash_map<int, Record> RecordList;
	RecordList	m_records;

	static CECGameTalk s_instance;
};