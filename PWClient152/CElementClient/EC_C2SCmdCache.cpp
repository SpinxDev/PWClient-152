/*
 * FILE: EC_C2SCmdCache.cpp
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Duyuxin, 2005/1/20
 *
 * HISTORY: 
 *
 * Copyright (c) 2005 Archosaur Studio, All Rights Reserved.
 */

#include "EC_Global.h"
#include "EC_C2SCmdCache.h"
#include "EC_GameSession.h"
#include "EC_Game.h"
#include "EC_GameRun.h"
#include "EC_World.h"
#include "EC_HostPlayer.h"
#include "EC_IvtrItem.h"
#include "EC_Inventory.h"
#include "defence/stackchecker.h"
#include "callid.hxx"

#include "factionresourcebattlegetmap.hpp"
#include "factionresourcebattlegetrecord.hpp"

#include "AAssist.h"

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


///////////////////////////////////////////////////////////////////////////
//	
//	Implement CECC2SCmdCache
//	
///////////////////////////////////////////////////////////////////////////

CECC2SCmdCache::CECC2SCmdCache()
{
	m_idLastPickUpItem	= 0;
	m_idLastSelTarget	= 0;
	m_bGetExpProps		= false;
	m_bEnterSanctuary	= false;

	CECC2SCmdCache::InitCounters();
}

CECC2SCmdCache::~CECC2SCmdCache()
{
	//	Release all counters
	{
		CounterTable::iterator it = m_CounterMap.begin();
		for (; it != m_CounterMap.end(); ++it)
			delete it->second;
	}
	{
		CounterTable::iterator it = m_CounterMap2.begin();
		for (; it != m_CounterMap2.end(); ++it)
			delete it->second;
	}
}

//	Initialize counters
bool CECC2SCmdCache::InitCounters()
{
	using namespace C2S;

	//	'Use item' command time counter
	CECCounter* pCnt = new CECCounter;
	pCnt->SetPeriod(200);
	pCnt->Reset(true);
	m_CounterMap[USE_ITEM] = pCnt;

	//	'Pickup item' command time counter
	pCnt = new CECCounter;
	pCnt->SetPeriod(500);
	m_CounterMap[PICKUP] = pCnt;

	//	'Select target' command time counter
	pCnt = new CECCounter;
	pCnt->SetPeriod(250);
	m_CounterMap[SELECT_TARGET] = pCnt;

	//	'Get extend properties' command time counter
	pCnt = new CECCounter;
	pCnt->SetPeriod(2000);
	pCnt->Reset(true);
	m_CounterMap[GET_EXT_PROP] = pCnt;

	//	'Cast skill' command time counter
	pCnt = new CECCounter;
	pCnt->SetPeriod(200);
	pCnt->Reset(true);
	m_CounterMap[CAST_SKILL] = pCnt;

	//	'Revive ask' command time counter
	pCnt = new CECCounter;
	pCnt->SetPeriod(500);
	pCnt->Reset(true);
	m_CounterMap[REVIVE_VILLAGE] = pCnt;
	
	//	'Enter sanctuary' command time counter
	pCnt = new CECCounter;
	pCnt->SetPeriod(2000);
	m_CounterMap[ENTER_SANCTUARY] = pCnt;
	
	//	'Enter instance' command time counter
	pCnt = new CECCounter;
	pCnt->SetPeriod(2000);
	pCnt->Reset(true);
	m_CounterMap[ENTER_INSTANCE] = pCnt;

	//	'Rush fly' command time counter
	pCnt = new CECCounter;
	pCnt->SetPeriod(500);
	pCnt->Reset(true);
	m_CounterMap[ACTIVE_RUSH_FLY] = pCnt;

	//	'Cancel action' command time counter
	pCnt = new CECCounter;
	pCnt->SetPeriod(200);
	pCnt->Reset(true);
	m_CounterMap[CANCEL_ACTION] = pCnt;

	//	'Control pet' command time counter
	pCnt = new CECCounter;
	pCnt->SetPeriod(400);
	pCnt->Reset(true);
	m_CounterMap[PET_CTRL] = pCnt;

	//	'Hello' command time counter
	pCnt = new CECCounter;
	pCnt->SetPeriod(2000);
	pCnt->Reset(true);
	m_CounterMap[SEVNPC_HELLO] = pCnt;

	//	'Present' command time counter
	pCnt = new CECCounter;
	pCnt->SetPeriod(1000);
	pCnt->Reset(true);
	m_CounterMap[PLAYER_GIVE_PRESENT] = pCnt;

	//	'GetPlayerBriefInfo' time counter
	pCnt = new CECCounter;
	pCnt->SetPeriod(2000);
	pCnt->Reset(true);
	m_CounterMap2[GNET::PROTOCOL_GETPLAYERBRIEFINFO] = pCnt;

	//  'PlayerBaseInfo' time counter
	pCnt = new CECCounter;
	pCnt->SetPeriod(2000);
	pCnt->Reset(true);
	m_CounterMap2[GNET::PROTOCOL_PLAYERBASEINFO] = pCnt;
	
	pCnt = new CECCounter;
	pCnt->SetPeriod(10*1000);
	pCnt->Reset(true);
	m_CounterMap2[GNET::PROTOCOL_FACTIONRESOURCEBATTLEGETMAP] = pCnt;

	pCnt = new CECCounter;
	pCnt->SetPeriod(10*1000);
	pCnt->Reset(true);
	m_CounterMap2[GNET::PROTOCOL_FACTIONRESOURCEBATTLEGETRECORD] = pCnt;
	return true;
}

//	Remove all un-sent commands in cache
void CECC2SCmdCache::RemoveAllCmds()
{
	m_UseItemCmdList.RemoveAll();

	//	重置 C2S 命令计时器
	m_CounterMap[C2S::USE_ITEM]->Reset(true);
	
	m_EnterSanctuaryList.RemoveAll();
	m_CounterMap[C2S::ENTER_SANCTUARY]->Reset(true);
	m_bEnterSanctuary = false;
	
	m_PresentInfoList.RemoveAll();
	m_CounterMap[C2S::PLAYER_GIVE_PRESENT]->Reset(true);
	
	//	重置协议计时器
	m_GetPlayerBriefInfoList.RemoveAll();
	m_CounterMap2[GNET::PROTOCOL_GETPLAYERBRIEFINFO]->Reset(true);

	m_PlayerBaseInfoList.RemoveAll();
	m_CounterMap2[GNET::PROTOCOL_PLAYERBASEINFO]->Reset(true);
	
	m_CounterMap2[GNET::PROTOCOL_FACTIONRESOURCEBATTLEGETMAP]->Reset(true);
	m_CounterMap2[GNET::PROTOCOL_FACTIONRESOURCEBATTLEGETRECORD]->Reset(true);
}

//	Tick routine
bool CECC2SCmdCache::Tick(DWORD dwDeltaTime)
{
	DWORD dwRealTime = g_pGame->GetRealTickTime();

	{
		CounterTable::iterator it = m_CounterMap.begin();
		for (; it != m_CounterMap.end(); ++it)
			((CECCounter*)it->second)->IncCounter(dwRealTime);
	}
	{
		CounterTable::iterator it = m_CounterMap2.begin();
		for (; it != m_CounterMap2.end(); ++it)
			((CECCounter*)it->second)->IncCounter(dwRealTime);
	}

	//	Try to send 'use item' command in cache
	SendCachedCmdUseItem();

	//	Try to send 'get extend properties' command
	SendCachedCmdGetExtProp();

	//	Try to send 'GetPlayerBriefInfo'
	SendCachedGetPlayerBriefInfo();
	//  Try to send 'PlayerBaseInfo'
	SendCachedPlayerBaseInfo();
	//  Try to send 'PresentInfo'
	SendCachedPresentInfo();

	if (m_bEnterSanctuary)
	{
		CECCounter* pCnt = m_CounterMap[C2S::ENTER_SANCTUARY];
		if (pCnt->IsFull())
		{
			ALISTPOSITION pos = m_EnterSanctuaryList.GetHeadPosition();
			while(pos)
			{
				int id = m_EnterSanctuaryList.GetNext(pos);
				c2s_SendCmdEnterSanctuary(id);
			}

			m_EnterSanctuaryList.RemoveAll();
			m_bEnterSanctuary = false;
		}
	}

	return true;
}

//	Send 'use item' command
void CECC2SCmdCache::SendCachedCmdUseItem()
{
	CECCounter* pCnt = m_CounterMap[C2S::USE_ITEM];
	if (!pCnt->IsFull() || !m_UseItemCmdList.GetCount())
		return;

	pCnt->Reset();

	//	Send the first item
	const C2S::cmd_use_item& Cmd = m_UseItemCmdList.GetHead();
	c2s_SendCmdUseItem(Cmd.where, (BYTE)Cmd.index, Cmd.item_id, Cmd.byCount);
	m_UseItemCmdList.RemoveHead();
}

//	Send cached 'get extend properties' command
void CECC2SCmdCache::SendCachedCmdGetExtProp()
{
	CECCounter* pCnt = m_CounterMap[C2S::GET_EXT_PROP];
	if (!m_bGetExpProps || !pCnt->IsFull())
		return;

	pCnt->Reset();

	m_bGetExpProps = false;
	c2s_SendCmdGetExtProps();
}

//	Send cached 'GetPlayerBriefInfo' command
void CECC2SCmdCache::SendCachedGetPlayerBriefInfo()
{
	using namespace GNET;

	CECCounter* pCnt = m_CounterMap2[PROTOCOL_GETPLAYERBRIEFINFO];
	if (!pCnt->IsFull() || !m_GetPlayerBriefInfoList.GetCount())
		return;

	pCnt->Reset();

	GetPlayerBriefInfo & p = m_GetPlayerBriefInfoList.GetHead();
	if (p.playerlist.size() != 0)
	{
		//	获取第一个玩家id并向服务器发送协议
		GetPlayerBriefInfo temp = p;
		temp.playerlist.resize(0);
		temp.playerlist.add(p.playerlist[0]);
		g_pGame->GetGameSession()->SendNetData(temp);
		
		//	从列表中清除
		p.playerlist.erase(p.playerlist.begin());
	}

	if (p.playerlist.size() == 0)
		m_GetPlayerBriefInfoList.RemoveHead();
}

//  Remove the cached 'PlayerBaseInfo' request
void CECC2SCmdCache::RemovePlayerBaseInfo(int iRoleID)
{
	ALISTPOSITION pos = m_PlayerBaseInfoList.Find(iRoleID);
	if( pos )
	{
		m_PlayerBaseInfoList.RemoveAt(pos);
	}
}

//  Send cached 'PlayerBaseInfo' protocol
void CECC2SCmdCache::SendCachedPlayerBaseInfo()
{
	using namespace GNET;

	CECCounter* pCnt = m_CounterMap2[PROTOCOL_PLAYERBASEINFO];
	if( !pCnt->IsFull() || !m_PlayerBaseInfoList.GetCount() )
		return;

	pCnt->Reset();

	AArray<int, int> aRoles;
	ALISTPOSITION pos = m_PlayerBaseInfoList.GetHeadPosition();
	while (pos)
	{
		aRoles.Add(m_PlayerBaseInfoList.GetAt(pos));
		m_PlayerBaseInfoList.GetNext(pos);
	}

	g_pGame->GetGameSession()->GetRoleBaseInfo(aRoles.GetSize(), aRoles.GetData());
}

/*	Send use item command

	The strategy to send 'use item' command: 

	1. if enough time has passed since last using time, send command directly.
	2. if 1 wasn't met and command cache is empty, put command into cache
	3. if 1, 2 weren't met, look through command cache, if found a command using
	   the item in the same slot, increase counter of item will be used and 
	   jump out of function.
    4. if 1, 2, 3 weren't met, add command to cache.
*/
void CECC2SCmdCache::SendCmdUseItem(BYTE byPackage, BYTE bySlot, int tid, BYTE byCount)
{
	using namespace C2S;

	CECCounter* pCnt = m_CounterMap[USE_ITEM];
	if (!m_UseItemCmdList.GetCount() && pCnt->IsFull())
	{
		pCnt->Reset();
		c2s_SendCmdUseItem(byPackage, bySlot, tid, byCount);
		return;
	}

	if (!m_UseItemCmdList.GetCount())
	{
		cmd_use_item Cmd;
		Cmd.where	= byPackage;
		Cmd.index	= bySlot;
		Cmd.item_id	= tid;
		Cmd.byCount	= byCount;
		m_UseItemCmdList.AddTail(Cmd);
		return;
	}

	ALISTPOSITION pos = m_UseItemCmdList.GetHeadPosition();
	while (pos)
	{
		cmd_use_item& Cmd = m_UseItemCmdList.GetAt(pos);

		if (Cmd.where == byPackage && Cmd.index == bySlot)
		{
			CECHostPlayer* pHost = g_pGame->GetGameRun()->GetHostPlayer();
			CECInventory* pPack = pHost->GetPack(byPackage);
			if (!pPack) return;
			CECIvtrItem* pItem = pPack->GetItem(bySlot);
			if (!pItem || !pItem->CheckUseCondition())
				return;

			int iTotal = Cmd.byCount + byCount;
			if (iTotal >= pItem->GetCount())
				iTotal = pItem->GetCount();

			a_ClampRoof(iTotal, 255);

			Cmd.byCount = (BYTE)iTotal;
			break;
		}

		m_UseItemCmdList.GetNext(pos);
	}

	if (!pos)
	{
		cmd_use_item Cmd;
		Cmd.where	= byPackage;
		Cmd.index	= bySlot;
		Cmd.item_id	= tid;
		Cmd.byCount	= byCount;
		m_UseItemCmdList.AddTail(Cmd);
	}

	//	Try to send command in cache
	SendCachedCmdUseItem();
}

/*	Send 'pick up item' command

	The strategy to send 'pick up item' command: 

	1. if the item is just the one which was to be picked up, check whether
	   enough time has passed since last command was sent. If true, send
	   command again, otherwise just throw command
    2. if the item isn't the one which was to be picked up, send command
	   directly.
*/
void CECC2SCmdCache::SendCmdPickUp(int idItem, int tid)
{
	StackChecker::ACTrace(5);

	CECCounter* pCnt = m_CounterMap[C2S::PICKUP];
	if (m_idLastPickUpItem != idItem || pCnt->IsFull())
	{
		pCnt->Reset();
		c2s_SendCmdPickup(idItem, tid);
		m_idLastPickUpItem = idItem;
	}
}

//	Send 'select target' command
//	The strategy to send 'select target' command is like as SendCmdPickUp()
void CECC2SCmdCache::SendCmdSelectTarget(int id)
{
	// Set selection first before server returns, so as to reduce the player waiting time.
	CECHostPlayer* pHost = g_pGame->GetGameRun()->GetHostPlayer();
	pHost->SetSelectedTarget(id);
	
	CECCounter* pCnt = m_CounterMap[C2S::SELECT_TARGET];
	if (m_idLastSelTarget != id || pCnt->IsFull())
	{
		pCnt->Reset();
		c2s_SendCmdSelectTarget(id);
		m_idLastSelTarget = id;
	}
}

//	Send 'get extend properties' command
void CECC2SCmdCache::SendCmdExtProps()
{
	//	If there is already a request waiting, return directly
	if (m_bGetExpProps)
		return;

	CECCounter* pCnt = m_CounterMap[C2S::GET_EXT_PROP];
	if (pCnt->IsFull())
	{
		pCnt->Reset();
		c2s_SendCmdGetExtProps();
		return;
	}

	m_bGetExpProps = true;
}

//	Send 'revive ask' command
void CECC2SCmdCache::SendCmdReviveVillage(int param)
{
	CECCounter* pCnt = m_CounterMap[C2S::REVIVE_VILLAGE];
	if (pCnt->IsFull())
	{
		pCnt->Reset();
		c2s_SendCmdReviveVillage(param);
	}
}

//	Send 'revive ask' command
void CECC2SCmdCache::SendCmdReviveItem(int param)
{
	//	This command merge time counter with REVIVE_VILLAGE
	CECCounter* pCnt = m_CounterMap[C2S::REVIVE_VILLAGE];
	if (pCnt->IsFull())
	{
		pCnt->Reset();
		c2s_SendCmdReviveItem(param);
	}
}

//	Send 'cast skill' command
void CECC2SCmdCache::SendCmdCastSkill(int idSkill, BYTE byPVPMask, int iNumTarget, int* aTargets)
{
	CECCounter* pCnt = m_CounterMap[C2S::CAST_SKILL];
	if (pCnt->IsFull())
	{
		pCnt->Reset();
		c2s_SendCmdCastSkill(idSkill, byPVPMask, iNumTarget, aTargets);
	}
}

//	Send 'cast instant skill' command
void CECC2SCmdCache::SendCmdCastInstantSkill(int idSkill, BYTE byPVPMask, int iNumTarget, int* aTargets)
{
	CECCounter* pCnt = m_CounterMap[C2S::CAST_SKILL];
	if (pCnt->IsFull())
	{
		pCnt->Reset();
		c2s_SendCmdCastInstantSkill(idSkill, byPVPMask, iNumTarget, aTargets);
	}
}

//	Send 'enter sanctuary' command
void CECC2SCmdCache::SendCmdEnterSanctuary(int id)
{
	ALISTPOSITION pos = m_EnterSanctuaryList.GetHeadPosition();
	while( pos )
	{
		int cid = m_EnterSanctuaryList.GetNext(pos);
		if( cid == id ) return;
	}

	//	Delay this command some time
	m_bEnterSanctuary = true;
	CECCounter* pCnt = m_CounterMap[C2S::ENTER_SANCTUARY];
	pCnt->Reset();
	m_EnterSanctuaryList.AddTail(id);
}

//	Send 'enter instance' commnad
void CECC2SCmdCache::SendCmdEnterInstance(int iTransIdx, int idInst)
{
	CECCounter* pCnt = m_CounterMap[C2S::ENTER_INSTANCE];
	if (pCnt->IsFull())
	{
		pCnt->Reset();
		c2s_SendCmdEnterInstance(iTransIdx, idInst);
	}
}

void CECC2SCmdCache::SendCmdActiveRushFly(bool bActive)
{
	CECCounter* pCnt = m_CounterMap[C2S::ACTIVE_RUSH_FLY];
	if (pCnt->IsFull())
	{
		pCnt->Reset();
		c2s_SendCmdActiveRushFly(bActive);
	}
}

void CECC2SCmdCache::SendCmdCancelAction()
{
	CECCounter* pCnt = m_CounterMap[C2S::CANCEL_ACTION];
	if (pCnt->IsFull())
	{
		pCnt->Reset();
		c2s_SendCmdCancelAction();
	}
}

void CECC2SCmdCache::SendCmdPetCtrl(int idTarget, int cmd, void* pParamBuf, int iParamLen)
{
	CECCounter* pCnt = m_CounterMap[C2S::PET_CTRL];
	if (pCnt->IsFull())
	{
		pCnt->Reset();
		c2s_SendCmdPetCtrl(idTarget, cmd, pParamBuf, iParamLen);
	}
}

void CECC2SCmdCache::SendCmdNPCSevHello(int nid)
{
	CECCounter* pCnt = m_CounterMap[C2S::SEVNPC_HELLO];
	if (pCnt->IsFull())
	{
		pCnt->Reset();
		c2s_SendCmdNPCSevHello(nid);
	}
}

void CECC2SCmdCache::SendCmdFactionPVPQueryInfo(int idFaction)
{
	CECHostPlayer* pHost = g_pGame->GetGameRun()->GetHostPlayer();
	if (!pHost->GetCoolTime(GP_CT_QUERY_MAFIA_PVP_INFO)){
		::c2s_SendCmdQueryFactionPVPInfo(idFaction);
	}
}

void CECC2SCmdCache::SendFactionPVPGetMap()
{
	using namespace GNET;
	CECCounter *pCnt = m_CounterMap2[PROTOCOL_FACTIONRESOURCEBATTLEGETMAP];
	if (pCnt->IsFull()){
		pCnt->Reset();
		FactionResourceBattleGetMap p;
		CECGameSession *pGameSession = g_pGame->GetGameSession();
		p.roleid = pGameSession->GetCharacterID();	
		pGameSession->SendNetData(p);
	}
}

void CECC2SCmdCache::SendFactionPVPGetRank()
{
	using namespace GNET;
	CECCounter *pCnt = m_CounterMap2[PROTOCOL_FACTIONRESOURCEBATTLEGETRECORD];
	if (pCnt->IsFull()){
		pCnt->Reset();
		FactionResourceBattleGetRecord p;
		CECGameSession *pGameSession = g_pGame->GetGameSession();
		p.roleid = pGameSession->GetCharacterID();	
		pGameSession->SendNetData(p);
	}
}

void CECC2SCmdCache::SendGetPlayerBriefInfo(int iNumPlayer, int* aIDs, int iReason)
{
	using namespace C2S;

	if (!iNumPlayer || !aIDs)
		return;

	//	1.合并添加到列表
	GNET::GetPlayerBriefInfo p;
	p.roleid = g_pGame->GetGameSession()->GetCharacterID();
	p.reason = (BYTE)iReason;
	for (int i = 0; i < iNumPlayer; ++ i)
	{
		ASSERT(aIDs[i]);
		if (aIDs[i])
			p.playerlist.add(aIDs[i]);
	}
	if (p.playerlist.size())
		m_GetPlayerBriefInfoList.AddTail(p);

	//	2.检查并发送
	SendCachedGetPlayerBriefInfo();
}
void CECC2SCmdCache::SendGetPlayerBaseInfo(int iNumRole, const int* aRoleIDs)
{
	using namespace C2S;

	if( !iNumRole || !aRoleIDs )
		return;

	for( int i=0; i<iNumRole; i++ )
	{
		if( aRoleIDs[i] )
			m_PlayerBaseInfoList.AddTail(aRoleIDs[i]);
	}

	SendCachedPlayerBaseInfo();
}

void CECC2SCmdCache::SendGivePresentProtocol(int roleid, int mailid, int itemid, int index, int slot)
{
	presentInfo info;
	info.roleid = roleid;
	info.mailid = mailid;
	info.itemid = itemid;
	info.index  = index;
	info.slot   = slot;
	m_PresentInfoList.AddTail(info);
}

void CECC2SCmdCache::SendCachedPresentInfo()
{
	CECCounter* pCnt = m_CounterMap[C2S::PLAYER_GIVE_PRESENT];
	if (!pCnt->IsFull() || !m_PresentInfoList.GetCount())
		return;
	
	pCnt->Reset();

	presentInfo& info = m_PresentInfoList.GetHead();
	c2s_SendCmdGivePresent(info.roleid, info.mailid, info.itemid, info.index, info.slot);
	m_PresentInfoList.RemoveHead();
}