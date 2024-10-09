// Filename	: EC_GameTalk.cpp
// Creator	: Xu Wenbin
// Date		: 2011/01/28

#include "EC_GameTalk.h"
#include "EC_Global.h"
#include "EC_Game.h"
#include "EC_GameRun.h"
#include "EC_HostPlayer.h"

CECGameTalk CECGameTalk::s_instance;

CECGameTalk & CECGameTalk::Instance()
{
	return s_instance;
}

CECGameTalk::CECGameTalk()
{
}

void CECGameTalk::Set(int id, GNET::PRIVATE_CHANNEL channel)
{
	Record & r = m_records[id];
	r.channel = channel;
}

void CECGameTalk::Del(int id)
{
	m_records.erase(id);
}

void CECGameTalk::Clear()
{
	m_records.clear();
}

bool CECGameTalk::Get(int id, GNET::PRIVATE_CHANNEL &channel)const
{
	bool bRet = false;
	RecordList::const_iterator it = m_records.find(id);
	if (it != m_records.end())
	{
		channel = it->second.channel;
		bRet = true;
	}
	return bRet;
}

GNET::PRIVATE_CHANNEL CECGameTalk::GetChannelToSend(int id, bool bForAutoReply, bool bFriendWhisper)const
{
	//	根据发言频道记录，查找我应该向哪个频道发送（自动回复）消息
	//	id:	对方玩家 id
	//	bForAutoReply:	为 true 时查询的是自动回复频道，即玩家最近一次发来消息的频道是 CHANNEL_FRIEND 时，游戏客户端自动回复给他的频道应该是 CHANNEL_FRIEND_RE
	//								如果 bForAutoReply 为 true，但无法查询玩家之前的发言频道，则报错，并返回默认频道
	//	bFriendWhisper:	为 true 时表示希望对客户端的好友也发送密语消息 CHANNEL_NORMAL 而不是好友消息 CHANNEL_FRIEND，这取决于发送好友消息的界面
	//								Win_Chat、Win_WhisperChat 界面可以发送游戏客户端好友及非好友密语频道、GT客户端消息；
	//								Win_FriendChat 可以发送游戏客户端好友消息、GT客户端消息，特殊情况下也向非好友发送密语消息、GT客户端消息
	//

	GNET::PRIVATE_CHANNEL channel = CHANNEL_NORMAL;		//	默认向游戏客户端发送密语消息

	while (true)
	{
		GNET::PRIVATE_CHANNEL lastChannel = CHANNEL_NORMAL;
		if (Get(id, lastChannel))
		{
			//	能够查询到最近一次聊天频道（对应到某类客户端）

			if (bForAutoReply)
			{
				//	查询的是如何自动回复
				//	策略：严格执行对应关系，不考虑在线状态，
				//				即 CHANNEL_FRIEND 回复 CHANNEL_FRIEND_RE
				//				CHANNEL_NORMAL 回复 CHANNEL_NORMALRE
				//				CHANNEL_GAMETALK 回复 CHANNEL_GAMETALK
				channel = GetChannelToAutoReply(lastChannel);
				break;
			}
			else
			{
				//	查询的是非自动回复
				//	策略：对好友，如果上次聊天客户端仍然在线，则仍与此客户端聊天；
				//								或者如果有其它客户端在线，则按在线优先级发往其它客户端；
				//								全不在线情况，也回复到上次客户端，对方再上线时能收到离线消息
				//								以上操作均改由服务器判断，以正确处理单向好友问题。客户端只根据界面及上次聊天频道做出初步判断，以维持与游戏客户端聊天兼容性
				//				对非好友，由于无法查询在线状态，因此将直接回复上次聊天客户端
				
				//	默认使用上次聊天频道
				channel = DirectReplyChannel(lastChannel);

				FRIEND *pFriend = GetFriend(id);
				if (pFriend)
				{
					//	是好友，应用好友策略

					if ((channel == CHANNEL_NORMAL || channel == CHANNEL_FRIEND))
					{
						//	处理上次从 Win_Chat 等发送好友密语、这次从 Win_FriendChat 发送及相反情况
						channel = bFriendWhisper ? CHANNEL_NORMAL : CHANNEL_FRIEND;
					}
					//	其它情况使用默认处理
				}
				else
				{
					if (channel == CHANNEL_FRIEND)
					{
						//	单向好友回复频道修改
						channel = CHANNEL_NORMAL;
					}
				}
				break;
			}
			//	已处理完能够查询最近聊天客户端的情况
		}

		if (bForAutoReply)
		{
			//	自动回复应能够查到上次聊天频道信息
			ASSERT(false);
		}

		//	无法查询最近一次聊天客户端
		//	策略：由服务器根据在线状态转发
		FRIEND *pFriend = GetFriend(id);
		if (pFriend)
		{
			channel = bFriendWhisper ? CHANNEL_GT_NORMAL : CHANNEL_GT_FRIEND;
			break;
		}
		else
		{
			channel = CHANNEL_GT_NORMAL;
		}
		break;
	}
	
	return channel;
}

GNET::PRIVATE_CHANNEL CECGameTalk::GetChannelToSend(const ACString &strName, bool bForAutoReply, bool bFriendWhisper, int &id)const
{
	//	strName:	对方玩家名称
	//	bForAutoReply、bFriendWhisper 同上
	//	id:	通过玩家名称 strName 查找到的玩家 id（CHANNEL_GAMETALK 频道的消息，需要知道准确的 id，此处的 id 可能通过 CECGameRun::GetPlayerID 查找不到）

	//	先查找玩家 id
	id = 0;

	//	先在好友列表中查找（游戏客户端缓存的只有在线好友）
	FRIEND *pFriend = GetFriend(strName);
	if (pFriend)
	{
		//	成功查到好友
		id = pFriend->id;
	}

	//	好友列表中未查到的，在游戏客户端中查找
	if (!id)
	{
		CECGameRun *pGameRun = g_pGame->GetGameRun();
		if (pGameRun)
			id = pGameRun->GetPlayerID(strName);
	}

	//	根据 id （包括非法 id ）查找回复频道
	return GetChannelToSend(id, bForAutoReply, bFriendWhisper);
}

CECGameTalk::FRIEND * CECGameTalk::GetFriend(int id)
{
	CECFriendMan::FRIEND *pFriend = NULL;
	
	while (true)
	{
		CECGameRun *pGameRun = g_pGame->GetGameRun();
		if (!pGameRun)
			break;

		CECHostPlayer *pHost = pGameRun->GetHostPlayer();
		if (!pHost)
			break;

		CECFriendMan *pFriendMan = pHost->GetFriendMan();
		if (!pFriendMan)
			break;

		pFriend = pFriendMan->GetFriendByID(id);
		break;
	}

	return pFriend;
}

CECGameTalk::FRIEND * CECGameTalk::GetFriend(const ACString &name)
{
	CECFriendMan::FRIEND *pFriend = NULL;
	
	while (true)
	{
		CECGameRun *pGameRun = g_pGame->GetGameRun();
		if (!pGameRun)
			break;

		CECHostPlayer *pHost = pGameRun->GetHostPlayer();
		if (!pHost)
			break;

		CECFriendMan *pFriendMan = pHost->GetFriendMan();
		if (!pFriendMan)
			break;

		pFriend = pFriendMan->GetFriendByName(name);
		break;
	}

	return pFriend;
}

GNET::PRIVATE_CHANNEL CECGameTalk::GetChannelToAutoReply(GNET::PRIVATE_CHANNEL lastChannel)
{
	//	根据上次聊天的频道，查询应该使用哪个频道进行自动回复
	//
	GNET::PRIVATE_CHANNEL autoReplyChannel = lastChannel;
	while (true)
	{
		if (lastChannel == CHANNEL_NORMAL || lastChannel == CHANNEL_NORMALRE)
		{
			//	注：lastChannel 为 CHANNEL_NORMALRE 时，不应该再有自动回复，因此也不应该有查询此函数的需要
			//			从逻辑完整性上仍然返回到 CHANNEL_NORMALRE 频道
			if (lastChannel == CHANNEL_NORMALRE)
				ASSERT(false);
			
			autoReplyChannel = CHANNEL_NORMALRE;
			break;
		}
		
		if (lastChannel == CHANNEL_FRIEND || lastChannel == CHANNEL_FRIEND_RE || lastChannel == CHANNEL_USERINFO)
		{
			//	注：lastChannel 为 CHANNEL_FRIEND_RE 时，不应该再有自动回复，因此也不应该有查询此函数的需要
			//			从逻辑完整性上仍然返回到 CHANNEL_FRIEND_RE 频道
			//	注：lastChannel 为 CHANNEL_USERINFO 时也不应该再有自动回复
			if (lastChannel == CHANNEL_FRIEND_RE || lastChannel == CHANNEL_USERINFO)
				ASSERT(false);
			
			autoReplyChannel = CHANNEL_FRIEND_RE;
			break;
		}
		
		//	注：其它频道时，不应该有自动回复，因为自动回复应该使用单独的频道，以避免自动回复死循环
		//			从逻辑完整性上仍然返回原频道
		ASSERT(false);
		break;
	}
	return autoReplyChannel;
}

GNET::PRIVATE_CHANNEL CECGameTalk::DirectReplyChannel(GNET::PRIVATE_CHANNEL lastChannel)
{
	//	将上次聊天的频道，转换为回复聊天时应当使用的频道
	//
	GNET::PRIVATE_CHANNEL replyChannel = lastChannel;
	while (true)
	{
		if (lastChannel == CHANNEL_NORMAL || lastChannel == CHANNEL_NORMALRE)
		{
			//	与好友通过密语频道交谈，返回密语频道
			replyChannel = CHANNEL_NORMAL;
			break;
		}
		
		if (lastChannel == CHANNEL_FRIEND || lastChannel == CHANNEL_FRIEND_RE || lastChannel == CHANNEL_USERINFO)
		{
			//	CHANNEL_USERINFO 频道的信息更新，不应该记录入上次聊天频道
			if (lastChannel == CHANNEL_USERINFO)
				ASSERT(false);

			//	与好友通过好友频道交谈，返回好友频道
			replyChannel = CHANNEL_FRIEND;
			break;
		}

		//	其它频道（如 GT ）时，使用原频道回复
		replyChannel = lastChannel;
		break;
	}
	return replyChannel;
}