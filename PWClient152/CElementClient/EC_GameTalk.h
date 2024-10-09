// Filename	: EC_GameTalk.h
// Creator	: Xu Wenbin
// Date		: 2011/01/28

#pragma once

#include <AAssist.h>
#include <hashmap.h>
#include "Network/gnetdef.h"
#include "EC_Friend.h"

//
//	�� CECGameTalk: ��¼���� GT ��ƽ̨����� ID�����Ƶ���������ڻظ�
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

	//	��ѯ���Ѹ�������
	typedef CECFriendMan::FRIEND FRIEND;
	static FRIEND* GetFriend(int id);
	static FRIEND* GetFriend(const ACString &name);

	//	��ѯƵ����������
	static GNET::PRIVATE_CHANNEL GetChannelToAutoReply(GNET::PRIVATE_CHANNEL lastChannel);
	static GNET::PRIVATE_CHANNEL DirectReplyChannel(GNET::PRIVATE_CHANNEL lastChannel);

	//	ÿ����Ч��¼
	struct Record
	{
		GNET::PRIVATE_CHANNEL channel;
	};

	//	�������һ�δ�GT��WEB������������Ϣ������б�
	typedef abase::hash_map<int, Record> RecordList;
	RecordList	m_records;

	static CECGameTalk s_instance;
};