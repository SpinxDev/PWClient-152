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
	//	���ݷ���Ƶ����¼��������Ӧ�����ĸ�Ƶ�����ͣ��Զ��ظ�����Ϣ
	//	id:	�Է���� id
	//	bForAutoReply:	Ϊ true ʱ��ѯ�����Զ��ظ�Ƶ������������һ�η�����Ϣ��Ƶ���� CHANNEL_FRIEND ʱ����Ϸ�ͻ����Զ��ظ�������Ƶ��Ӧ���� CHANNEL_FRIEND_RE
	//								��� bForAutoReply Ϊ true�����޷���ѯ���֮ǰ�ķ���Ƶ�����򱨴�������Ĭ��Ƶ��
	//	bFriendWhisper:	Ϊ true ʱ��ʾϣ���Կͻ��˵ĺ���Ҳ����������Ϣ CHANNEL_NORMAL �����Ǻ�����Ϣ CHANNEL_FRIEND����ȡ���ڷ��ͺ�����Ϣ�Ľ���
	//								Win_Chat��Win_WhisperChat ������Է�����Ϸ�ͻ��˺��Ѽ��Ǻ�������Ƶ����GT�ͻ�����Ϣ��
	//								Win_FriendChat ���Է�����Ϸ�ͻ��˺�����Ϣ��GT�ͻ�����Ϣ�����������Ҳ��Ǻ��ѷ���������Ϣ��GT�ͻ�����Ϣ
	//

	GNET::PRIVATE_CHANNEL channel = CHANNEL_NORMAL;		//	Ĭ������Ϸ�ͻ��˷���������Ϣ

	while (true)
	{
		GNET::PRIVATE_CHANNEL lastChannel = CHANNEL_NORMAL;
		if (Get(id, lastChannel))
		{
			//	�ܹ���ѯ�����һ������Ƶ������Ӧ��ĳ��ͻ��ˣ�

			if (bForAutoReply)
			{
				//	��ѯ��������Զ��ظ�
				//	���ԣ��ϸ�ִ�ж�Ӧ��ϵ������������״̬��
				//				�� CHANNEL_FRIEND �ظ� CHANNEL_FRIEND_RE
				//				CHANNEL_NORMAL �ظ� CHANNEL_NORMALRE
				//				CHANNEL_GAMETALK �ظ� CHANNEL_GAMETALK
				channel = GetChannelToAutoReply(lastChannel);
				break;
			}
			else
			{
				//	��ѯ���Ƿ��Զ��ظ�
				//	���ԣ��Ժ��ѣ�����ϴ�����ͻ�����Ȼ���ߣ�������˿ͻ������죻
				//								��������������ͻ������ߣ����������ȼ����������ͻ��ˣ�
				//								ȫ�����������Ҳ�ظ����ϴοͻ��ˣ��Է�������ʱ���յ�������Ϣ
				//								���ϲ��������ɷ������жϣ�����ȷ������������⡣�ͻ���ֻ���ݽ��漰�ϴ�����Ƶ�����������жϣ���ά������Ϸ�ͻ������������
				//				�ԷǺ��ѣ������޷���ѯ����״̬����˽�ֱ�ӻظ��ϴ�����ͻ���
				
				//	Ĭ��ʹ���ϴ�����Ƶ��
				channel = DirectReplyChannel(lastChannel);

				FRIEND *pFriend = GetFriend(id);
				if (pFriend)
				{
					//	�Ǻ��ѣ�Ӧ�ú��Ѳ���

					if ((channel == CHANNEL_NORMAL || channel == CHANNEL_FRIEND))
					{
						//	�����ϴδ� Win_Chat �ȷ��ͺ��������δ� Win_FriendChat ���ͼ��෴���
						channel = bFriendWhisper ? CHANNEL_NORMAL : CHANNEL_FRIEND;
					}
					//	�������ʹ��Ĭ�ϴ���
				}
				else
				{
					if (channel == CHANNEL_FRIEND)
					{
						//	������ѻظ�Ƶ���޸�
						channel = CHANNEL_NORMAL;
					}
				}
				break;
			}
			//	�Ѵ������ܹ���ѯ�������ͻ��˵����
		}

		if (bForAutoReply)
		{
			//	�Զ��ظ�Ӧ�ܹ��鵽�ϴ�����Ƶ����Ϣ
			ASSERT(false);
		}

		//	�޷���ѯ���һ������ͻ���
		//	���ԣ��ɷ�������������״̬ת��
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
	//	strName:	�Է��������
	//	bForAutoReply��bFriendWhisper ͬ��
	//	id:	ͨ��������� strName ���ҵ������ id��CHANNEL_GAMETALK Ƶ������Ϣ����Ҫ֪��׼ȷ�� id���˴��� id ����ͨ�� CECGameRun::GetPlayerID ���Ҳ�����

	//	�Ȳ������ id
	id = 0;

	//	���ں����б��в��ң���Ϸ�ͻ��˻����ֻ�����ߺ��ѣ�
	FRIEND *pFriend = GetFriend(strName);
	if (pFriend)
	{
		//	�ɹ��鵽����
		id = pFriend->id;
	}

	//	�����б���δ�鵽�ģ�����Ϸ�ͻ����в���
	if (!id)
	{
		CECGameRun *pGameRun = g_pGame->GetGameRun();
		if (pGameRun)
			id = pGameRun->GetPlayerID(strName);
	}

	//	���� id �������Ƿ� id �����һظ�Ƶ��
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
	//	�����ϴ������Ƶ������ѯӦ��ʹ���ĸ�Ƶ�������Զ��ظ�
	//
	GNET::PRIVATE_CHANNEL autoReplyChannel = lastChannel;
	while (true)
	{
		if (lastChannel == CHANNEL_NORMAL || lastChannel == CHANNEL_NORMALRE)
		{
			//	ע��lastChannel Ϊ CHANNEL_NORMALRE ʱ����Ӧ�������Զ��ظ������Ҳ��Ӧ���в�ѯ�˺�������Ҫ
			//			���߼�����������Ȼ���ص� CHANNEL_NORMALRE Ƶ��
			if (lastChannel == CHANNEL_NORMALRE)
				ASSERT(false);
			
			autoReplyChannel = CHANNEL_NORMALRE;
			break;
		}
		
		if (lastChannel == CHANNEL_FRIEND || lastChannel == CHANNEL_FRIEND_RE || lastChannel == CHANNEL_USERINFO)
		{
			//	ע��lastChannel Ϊ CHANNEL_FRIEND_RE ʱ����Ӧ�������Զ��ظ������Ҳ��Ӧ���в�ѯ�˺�������Ҫ
			//			���߼�����������Ȼ���ص� CHANNEL_FRIEND_RE Ƶ��
			//	ע��lastChannel Ϊ CHANNEL_USERINFO ʱҲ��Ӧ�������Զ��ظ�
			if (lastChannel == CHANNEL_FRIEND_RE || lastChannel == CHANNEL_USERINFO)
				ASSERT(false);
			
			autoReplyChannel = CHANNEL_FRIEND_RE;
			break;
		}
		
		//	ע������Ƶ��ʱ����Ӧ�����Զ��ظ�����Ϊ�Զ��ظ�Ӧ��ʹ�õ�����Ƶ�����Ա����Զ��ظ���ѭ��
		//			���߼�����������Ȼ����ԭƵ��
		ASSERT(false);
		break;
	}
	return autoReplyChannel;
}

GNET::PRIVATE_CHANNEL CECGameTalk::DirectReplyChannel(GNET::PRIVATE_CHANNEL lastChannel)
{
	//	���ϴ������Ƶ����ת��Ϊ�ظ�����ʱӦ��ʹ�õ�Ƶ��
	//
	GNET::PRIVATE_CHANNEL replyChannel = lastChannel;
	while (true)
	{
		if (lastChannel == CHANNEL_NORMAL || lastChannel == CHANNEL_NORMALRE)
		{
			//	�����ͨ������Ƶ����̸����������Ƶ��
			replyChannel = CHANNEL_NORMAL;
			break;
		}
		
		if (lastChannel == CHANNEL_FRIEND || lastChannel == CHANNEL_FRIEND_RE || lastChannel == CHANNEL_USERINFO)
		{
			//	CHANNEL_USERINFO Ƶ������Ϣ���£���Ӧ�ü�¼���ϴ�����Ƶ��
			if (lastChannel == CHANNEL_USERINFO)
				ASSERT(false);

			//	�����ͨ������Ƶ����̸�����غ���Ƶ��
			replyChannel = CHANNEL_FRIEND;
			break;
		}

		//	����Ƶ������ GT ��ʱ��ʹ��ԭƵ���ظ�
		replyChannel = lastChannel;
		break;
	}
	return replyChannel;
}