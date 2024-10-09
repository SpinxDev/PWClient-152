/*
 * FILE: EC_Friend.cpp
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Duyuxin, 2005/7/12
 *
 * HISTORY: 
 *
 * Copyright (c) 2005 Archosaur Studio, All Rights Reserved.
 */

#pragma warning (disable: 4284)

#include "A3DMacros.h"

#include "EC_Friend.h"
#include "EC_Global.h"
#include "EC_Game.h"
#include "EC_FixedMsg.h"
#include "EC_GameRun.h"
#include "EC_ManPlayer.h"
#include "EC_World.h"
#include "gnet.h"
#include "EC_UIManager.h"
#include "EC_GameUIMan.h"
#include "DlgMailToFriends.h"
#include <algorithm>
using namespace std;

#define new A_DEBUG_NEW

///////////////////////////////////////////////////////////////////////////
//	
//	Define and Macro
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Reference to External variables and functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Local Types and Variables and Global variables
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Local functions
//	
///////////////////////////////////////////////////////////////////////////

bool CECFriendMan::FRIEND::IsGameOnline(char s)
{
	return (s & (char)GAME_ONLINE) != 0;
}

bool CECFriendMan::FRIEND::IsGTOnline(char s)
{
	//	除了游戏在线、GT隐身，其它状态一律视为GT在线
	return (s & ~(char)GAME_ONLINE & ~(char)GT_INVISIBLE) != 0;
}

///////////////////////////////////////////////////////////////////////////
//	
//	Implement CECFriendMan
//	
///////////////////////////////////////////////////////////////////////////

CECFriendMan::CECFriendMan() : m_FriendTab(64)
{
}

CECFriendMan::~CECFriendMan()
{
	CECFriendMan::RemoveAllFriends();
	CECFriendMan::RemoveAllOfflineMsgs();

	unsigned int i(0);
	for (i = 0;i < m_aFriendEx.size();++i)
	{
		delete m_aFriendEx[i];
	}
	m_aFriendEx.clear();

	for (i = 0;i < m_aSendInfo.GetSize();++i)
	{
		delete m_aSendInfo[i];
	}
	m_aSendInfo.RemoveAll();
}

//	Reset friend list
bool CECFriendMan::ResetFriends(GNET::GetFriends_Re& Data)
{
	using namespace GNET;

	//	Remove all exiting friends
	RemoveAllFriends();

	//	Build groups ...
	//	Create group 0 at first
	AddGroup(0, g_pGame->GetFixedMsgTab()->GetWideString(FIXMSG_MYFRIEND));

	//	Create other groups
	int i, iNumGroup = Data.groups.size();
	for (i=0; i < iNumGroup; i++)
	{
		const GGroupInfo& gi = Data.groups[i];
		if (gi.name.size())
		{
			ACString strName((const ACHAR*)gi.name.begin(), gi.name.size() / sizeof (ACHAR));
			AddGroup(gi.gid, strName);
		}
		else
		{
			AddGroup(gi.gid, NULL);
		}
	}

	//	Build friend list ...
	int iNumFriend = Data.friends.size();
	CECPlayerMan* pPlayerMan = g_pGame->GetGameRun()->GetWorld()->GetPlayerMan();
	APtrArray<FRIEND*> aFriends;
	aFriends.SetSize(iNumFriend, 10);

	for (i=0; i < iNumFriend; i++)
	{
		const GFriendInfo& fi = Data.friends[i];
		FRIEND* pFriend = AddFriend(fi, 0);
		aFriends[i] = pFriend;
	}

	//	Change friend status
	if ((Data.status.size() %2) == 0)
	{
		for (i=0; i < (int)Data.status.size(); i+=2)
		{
			int n = Data.status[i];
			char s = Data.status[i+1];
			
			if (n >= 0 && n < iNumFriend)
			{
				FRIEND* pFriend = aFriends[n];
				if (pFriend)
					pFriend->status = s;
			}
		}
	}
	else
	{
		a_LogOutput(1, "CECFriendMan::ResetFriends, Failed to init friend status!");
		ASSERT(false);
	}

	return true;
}

inline bool LoginTimeCompare(CECFriendMan::FRIEND_EX* lhs,CECFriendMan::FRIEND_EX* rhs)
{
	return lhs->last_logintime > rhs->last_logintime;	
}

bool CECFriendMan::ResetFriendsEx(GNET::FriendExtList& Data)
{
	unsigned int i(0);
	for (i = 0;i < m_aFriendEx.size();++i)
	{
		delete m_aFriendEx[i];
	}
	m_aFriendEx.clear();

	for (i = 0;i < m_aSendInfo.GetSize();++i)
	{
		delete m_aSendInfo[i];
	}
	m_aSendInfo.RemoveAll();

	for (i = 0;i < Data.extra_info.size();++i)
	{
		FRIEND_EX* pFriendEx = new FRIEND_EX;
		if (!pFriendEx)
			continue;
		pFriendEx->newFriendPlaceHolder = false;
		pFriendEx->rid = Data.extra_info[i].rid;
		pFriendEx->uid = Data.extra_info[i].uid;
		pFriendEx->level = Data.extra_info[i].level;
		pFriendEx->last_logintime = Data.extra_info[i].last_logintime;
		pFriendEx->update_time = Data.extra_info[i].update_time;
		pFriendEx->reincarnation_count = Data.extra_info[i].reincarnation_times;
		pFriendEx->strRemarks = ACString((const ACHAR*)Data.extra_info[i].remarks.begin(), Data.extra_info[i].remarks.size() / sizeof (ACHAR));
		m_aFriendEx.push_back(pFriendEx);
	}

	sort(m_aFriendEx.begin(),m_aFriendEx.end(),LoginTimeCompare);

	for (i = 0;i < Data.send_info.size();++i)
	{
		SEND_INFO* pSendInfo = new SEND_INFO;
		if (!pSendInfo)
			continue;
		pSendInfo->rid = Data.send_info[i].rid;
		pSendInfo->sendmail_time = Data.send_info[i].sendmail_time;
		m_aSendInfo.Add(pSendInfo);
	}

	CECGameUIMan* pGameUI = g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan();
	if (pGameUI)
	{
		pGameUI->BuildFriendListEx();
	}

	return true;
}

//	Add a friend
CECFriendMan::FRIEND* CECFriendMan::AddFriend(const GNET::GFriendInfo& fi, char status)
{
	if (!CheckInit())
		return NULL;

	//	In friend list, one player ID may have two names, so check the case here !
	FRIEND* pFriend = GetFriendByID(fi.rid);
	if (pFriend)
	{
		//	Remove the friend
		RemoveFriend(fi.rid);
	}

	if (!(pFriend = new FRIEND))
		return NULL;

	pFriend->id				= fi.rid;
	pFriend->iGroup			= fi.gid;
	pFriend->iProfession = fi.cls;
	pFriend->status			= status;
	pFriend->nLevel			= -1;
	pFriend->idArea			= -1;

	if (fi.name.size())
	{
		pFriend->strName = ACString((const ACHAR*)fi.name.begin(), fi.name.size() / sizeof (ACHAR));

		//	Cache friend name if this function isn't called by ResetFriends(), because
		//	player name in friend list may not be right when ResetFriends()
		if (pFriend->IsGameOnline())
		{
			ACString strName((const ACHAR*)fi.name.begin(), fi.name.size() / sizeof (ACHAR));
			g_pGame->GetGameRun()->AddPlayerName(fi.rid, strName);
		}
	}

	//	Add friend to proper group
	int i(0);
	for (i=0; i < m_aGroups.GetSize(); i++)
	{
		if (m_aGroups[i]->iGroup == pFriend->iGroup)
		{
			m_aGroups[i]->aFriends.Add(pFriend);
			break;
		}
	}

	if (i >= m_aGroups.GetSize())
	{
		ASSERT(i < m_aGroups.GetSize());
		delete pFriend;
		return NULL;
	}

	//	Add friend to table
	m_FriendTab.put(fi.rid, pFriend);
	
	FRIEND_EX* pFriendEx = new FRIEND_EX;
	pFriendEx->newFriendPlaceHolder = true;
	pFriendEx->rid = pFriend->id;
	pFriendEx->uid = 0;
	pFriendEx->level = 0;
	pFriendEx->last_logintime = 0;
	pFriendEx->update_time = 0;
	pFriendEx->reincarnation_count = 0;
	m_aFriendEx.push_back(pFriendEx);

	return pFriend;
}

//	Remove a friend
void CECFriendMan::RemoveFriend(int idFriend)
{
	if (!CheckInit())
		return;

	//	Friend exists ?
	FriendTable::pair_type Pair = m_FriendTab.get(idFriend);
	if (!Pair.second)
		return;		//	Counldn't find this friend
	
	FRIEND* pFriend = *Pair.first;

	//	Remove from group
	unsigned int i(0);
	for (i=0; i < m_aGroups.GetSize(); i++)
	{
		GROUP* pGroup = m_aGroups[i];
		if (pGroup->iGroup == pFriend->iGroup)
		{
			for (int j=0; j < pGroup->aFriends.GetSize(); j++)
			{
				if (pFriend == pGroup->aFriends[j])
				{
					pGroup->aFriends.RemoveAt(j);
					break;
				}
			}

			break;
		}
	}

	ASSERT(i < m_aGroups.GetSize());
	for (i = 0; i < m_aFriendEx.size();++i)
	{
		if (m_aFriendEx[i]->rid == idFriend)
		{
			delete m_aFriendEx[i];
			m_aFriendEx.erase(&m_aFriendEx[i],&m_aFriendEx[i + 1]);
			break;
		}
	}
	//	Remove from table
	m_FriendTab.erase(pFriend->id);

	delete pFriend;
}

//	Get friend by ID
CECFriendMan::FRIEND* CECFriendMan::GetFriendByID(int idFriend)
{
	//	Friend exists ?
	FriendTable::pair_type Pair = m_FriendTab.get(idFriend);
	if (Pair.second)
		return *Pair.first;

	return NULL;
}

//	Get friend by name
CECFriendMan::FRIEND* CECFriendMan::GetFriendByName(const ACHAR* szName)
{
	//	Friend exists ?
	FriendTable::iterator it = m_FriendTab.begin();
	for(; it != m_FriendTab.end(); ++it)
	{
		FRIEND* pFriend = (FRIEND*)(*it.value());
		if (a_strcmp(pFriend->strName, szName) == 0)
			return pFriend;
	}

	return NULL;
}

//	Remove all friends
void CECFriendMan::RemoveAllFriends()
{
	if (!CheckInit())
		return;

	//	Release friend table
	FriendTable::iterator it = m_FriendTab.begin();
	for (; it != m_FriendTab.end(); ++it)
	{
		FRIEND* pFriend = *it.value();
		delete pFriend;
	}

	m_FriendTab.clear();

	//	Release groups
	for (int i=0; i < m_aGroups.GetSize(); i++)
	{
		delete m_aGroups[i];
	}

	m_aGroups.RemoveAll();
}

//	Change friend status
void CECFriendMan::ChangeFriendStatus(int idFriend, char status)
{
	FriendTable::pair_type Pair = m_FriendTab.get(idFriend);
	if (Pair.second)
	{
		FRIEND* pFriend = *Pair.first;
		pFriend->status = status;
	}
}

//	Change friend's group
void CECFriendMan::ChangeFriendGroup(int idFriend, int idGroup)
{
	if (!CheckInit())
		return;

	FriendTable::pair_type Pair = m_FriendTab.get(idFriend);
	if (!Pair.second)
		return;
	
	FRIEND* pFriend = *Pair.first;
	if (pFriend->iGroup == idGroup)
		return;

	//	Remove friend from old group
	GROUP* pGroup = GetGroupByID(pFriend->iGroup);
	if (pGroup)
	{
		for (int i=0; i < pGroup->aFriends.GetSize(); i++)
		{
			if (pFriend == pGroup->aFriends[i])
			{
				pGroup->aFriends.RemoveAt(i);
				break;
			}
		}
	}

	if (!(pGroup = GetGroupByID(idGroup)))
	{
		//	This shouldn't happen
		ASSERT(pGroup);
		delete pFriend;
		return;
	}

	//	Add to new group
	pFriend->iGroup = idGroup;
	pGroup->aFriends.Add(pFriend);
}

//	Add group
bool CECFriendMan::AddGroup(int idGroup, const ACHAR* szName)
{
	if (idGroup && !CheckInit())
		return false;

	GROUP* pGroup = new GROUP;
	if (!pGroup)
		return false;

	pGroup->strName = szName ? szName : _AL("Group");
	pGroup->iGroup	= idGroup;
	pGroup->color	= A3DCOLORRGB(255, 204, 0);

	m_aGroups.Add(pGroup);

	return true;
}

//	Remove group
void CECFriendMan::RemoveGroup(int idGroup)
{
	if (!CheckInit())
		return;

	if (!idGroup)
	{
		ASSERT(idGroup != 0);
		return;
	}

	//	Get group 0
	GROUP* pGroup0 = m_aGroups[0];
	if (!pGroup0)
		return;

	ASSERT(pGroup0->iGroup == 0);

	int i, j;
	for (i=0; i < m_aGroups.GetSize(); i++)
	{
		if (m_aGroups[i]->iGroup == idGroup)
			break;
	}

	if (i >= m_aGroups.GetSize())
		return;

	//	Remove all friends in this group to group 0
	GROUP* pGroup = m_aGroups[i];
	for (j=0; j < pGroup->aFriends.GetSize(); j++)
	{
		FRIEND* pFriend = pGroup->aFriends[j];
		pFriend->iGroup = 0;
		pGroup0->aFriends.Add(pFriend);
	}

	//	Delete group
	m_aGroups.RemoveAt(i);
	delete pGroup;
}

//	Change group name
void CECFriendMan::ChangeGroupName(int idGroup, const ACHAR* szName)
{
	if (!CheckInit())
		return;

	if (!szName || !szName[0] || !idGroup)
		return;

	for (int i=0; i < m_aGroups.GetSize(); i++)
	{
		if (m_aGroups[i]->iGroup == idGroup)
		{
			m_aGroups[i]->strName = szName;
			break;
		}
	}
}

//	Set group color
void CECFriendMan::SetGroupColor(int idGroup, A3DCOLOR color)
{
	if (!CheckInit())
		return;

	if (!idGroup)
		return;		//	Cann't change default group color

	for (int i=0; i < m_aGroups.GetSize(); i++)
	{
		if (m_aGroups[i]->iGroup == idGroup)
		{
			m_aGroups[i]->color = color;
			break;
		}
	}
}

//	Get group by id
CECFriendMan::GROUP* CECFriendMan::GetGroupByID(int id)
{
	for (int i=0; i < m_aGroups.GetSize(); i++)
	{
		if (m_aGroups[i]->iGroup == id)
			return m_aGroups[i];
	}

	return NULL;
}

//	Reset offline message list
bool CECFriendMan::ResetOfflineMsgs(GNET::GetSavedMsg_Re& p)
{
	using namespace GNET;

	//	因为GT客户端的离线消息会由服务器主动推到，不能保证之前的离线消息已经处理，因此不能删除已有离线消息
	//	RemoveAllOfflineMsgs();

	int i, iNumMsg = p.messages.size();
	for (i=0; i < iNumMsg; i++)
	{
		Message& src = p.messages[i];

		MESSAGE* pdst = new MESSAGE;
		if (!pdst)
			return true;

		if (!src.src_name.size() || !src.msg.size())
			continue;

		ACString strName((const ACHAR*)src.src_name.begin(), src.src_name.size() / sizeof (ACHAR));
		ACString strMsg((const ACHAR*)src.msg.begin(), src.msg.size() / sizeof (ACHAR));

		pdst->strName	= strName;
		pdst->strMsg	= strMsg;
		pdst->idSender	= src.srcroleid;
		pdst->byFlag	= src.channel;

		m_aOfflineMsgs.Add(pdst);
	}

	return true;
}

//	Remove all offline messages
void CECFriendMan::RemoveAllOfflineMsgs()
{
	for (int i=0; i < m_aOfflineMsgs.GetSize(); i++)
	{
		delete m_aOfflineMsgs[i];
	}

	m_aOfflineMsgs.RemoveAll();
}

// Set friend level
void CECFriendMan::SetFriendLevel(int idFriend, int nLevel)
{
	FriendTable::pair_type Pair = m_FriendTab.get(idFriend);
	if (Pair.second)
	{
		FRIEND* pFriend = *Pair.first;
		pFriend->nLevel = nLevel;
	}
}

// Set friend area
void CECFriendMan::SetFriendArea(int idFriend, int idArea)
{
	FriendTable::pair_type Pair = m_FriendTab.get(idFriend);
	if (Pair.second)
	{
		FRIEND* pFriend = *Pair.first;
		pFriend->idArea = idArea;
	}
}

CECFriendMan::FRIEND_EX* CECFriendMan::GetFriendExByID(int idFriend)
{
	size_t count = m_aFriendEx.size();
	for ( size_t i = 0; i < count; ++i) {
		if (m_aFriendEx[i]->rid == idFriend)
			return m_aFriendEx[i];
	}
	return NULL;
}

CECFriendMan::FRIEND_EX* CECFriendMan::GetValidFriendExByID(int idFriend){
	if (CECFriendMan::FRIEND_EX* result = GetFriendExByID(idFriend)){
		if (!result->newFriendPlaceHolder){
			return result;
		}
	}
	return NULL;
}

bool CECFriendMan::CanSetFriendRemarks(int idFriend){
	return GetFriendExByID(idFriend) != NULL;
}

bool CECFriendMan::GetFriendRemarks(int idFriend, ACString &strRemarks){
	bool result(false);
	if (FRIEND_EX *pFriendEx = GetFriendExByID(idFriend)){
		strRemarks = pFriendEx->strRemarks;
		result = true;
	}
	return result;
}

bool CECFriendMan::SetFriendRemarks(int idFriend, const ACHAR *szRemarks){
	bool result(false);
	if (FRIEND_EX *pFriendEx = GetFriendExByID(idFriend)){
		pFriendEx->strRemarks = szRemarks;
		result = true;
	}
	return result;
}