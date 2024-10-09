/*
 * FILE: EC_C2SCmdCache.h
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Duyuxin, 2005/1/20
 *
 * HISTORY: 
 *
 * Copyright (c) 2005 Archosaur Studio, All Rights Reserved.
 */

#pragma once

#include "EC_GPDataType.h"
#include "hashmap.h"
#include "AList2.h"
#include "getplayerbriefinfo.hpp"
#include "playerbaseinfo.hpp"

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

class CECCounter;

namespace GNET
{
	class GetPlayerBriefInfo;
}

///////////////////////////////////////////////////////////////////////////
//	
//	Declare of Global functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Class CECC2SCmdCache
//	
///////////////////////////////////////////////////////////////////////////

class CECC2SCmdCache
{
public:		//	Types

	struct presentInfo
	{
		int roleid;
		int mailid;
		int itemid;
		int index;
		int slot;
	};

	typedef abase::hash_map<int, CECCounter*> CounterTable;

public:		//	Constructor and Destructor

	CECC2SCmdCache();
	virtual ~CECC2SCmdCache();

public:		//	Attributes

public:		//	Operations

	//	Remove all un-sent commands in cache
	void RemoveAllCmds();

	//	Tick routine
	bool Tick(DWORD dwDeltaTime);

	//	Send commands ...
	void SendCmdUseItem(BYTE byPackage, BYTE bySlot, int tid, BYTE byCount);
	void SendCmdPickUp(int idItem, int tid);
	void SendCmdSelectTarget(int id);
	void SendCmdExtProps();
	void SendCmdReviveVillage(int param=0);
	void SendCmdReviveItem(int param=0);
	void SendCmdCastSkill(int idSkill, BYTE byPVPMask, int iNumTarget, int* aTargets);
	void SendCmdCastInstantSkill(int idSkill, BYTE byPVPMask, int iNumTarget, int* aTargets);
	void SendCmdEnterSanctuary(int id);
	void SendCmdEnterInstance(int iTransIdx, int idInst);
	void SendCmdActiveRushFly(bool bActive);
	void SendCmdCancelAction();
	void SendCmdPetCtrl(int idTarget, int cmd, void* pParamBuf, int iParamLen);
	void SendCmdNPCSevHello(int nid);
	void SendCmdFactionPVPQueryInfo(int idFaction);

	//	Send protocols ...
	void SendGetPlayerBriefInfo(int iNumPlayer, int* aIDs, int iReason);
	void SendGetPlayerBaseInfo(int iNumRole, const int* aRoleIDs);
	void SendGivePresentProtocol(int roleid, int mailid, int itemid, int index, int slot);
	void SendFactionPVPGetMap();
	void SendFactionPVPGetRank();

	//  Remove the PlayerBaseInfo request
	void RemovePlayerBaseInfo(int iRoleID);

protected:	//	Attributes

	int		m_idLastPickUpItem;		//	ID of picked up item last time
	int		m_idLastSelTarget;		//	ID of selected item last time
	bool	m_bGetExpProps;
	bool	m_bEnterSanctuary;

	CounterTable	m_CounterMap;
	CounterTable	m_CounterMap2;

	AList2<C2S::cmd_use_item, C2S::cmd_use_item&>	m_UseItemCmdList;
	AList2<GNET::GetPlayerBriefInfo, GNET::GetPlayerBriefInfo&>	m_GetPlayerBriefInfoList;
	AList2<int,int> m_PlayerBaseInfoList;
	AList2<int,int> m_EnterSanctuaryList;
	AList2<presentInfo,presentInfo&> m_PresentInfoList;
protected:	//	Operations

	//	Initialize counters
	bool InitCounters();

	//	Send cached 'use item' command
	void SendCachedCmdUseItem();
	//	Send cached 'get extend properties' command
	void SendCachedCmdGetExtProp();
	//	Send cached 'get player brief info' command
	void SendCachedGetPlayerBriefInfo();
	//  Send cached 'PlayerBaseInfo' protocol
	void SendCachedPlayerBaseInfo();
	//  Send cached 'presentInfo' protocol
	void SendCachedPresentInfo();
};

///////////////////////////////////////////////////////////////////////////
//	
//	Inline functions
//	
///////////////////////////////////////////////////////////////////////////



