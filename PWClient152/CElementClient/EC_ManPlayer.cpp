/*
 * FILE: EC_ManPlayer.cpp
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Duyuxin, 2004/8/30
 *
 * HISTORY: 
 *
 * Copyright (c) 2004 Archosaur Studio, All Rights Reserved.
 */

#pragma warning (disable: 4284)

#include "EC_Global.h"
#include "EC_ManPlayer.h"
#include "EC_HostPlayer.h"
#include "EC_ElsePlayer.h"
#include "EC_LoginPlayer.h"
#include "EC_Viewport.h"
#include "EC_GPDataType.h"
#include "EC_Game.h"
#include "EC_GameRun.h"
#include "EC_UIManager.h"
#include "EC_GameUIMan.h"
#include "EC_GameSession.h"
#include "EC_World.h"
#include "EC_CDS.h"
#include "EC_Utility.h"
#include "EC_Configs.h"
#include "EC_Team.h"
#include "EC_TeamMan.h"
#include "EC_UIManager.h"
#include "EC_GameUIMan.h"
#include "EC_FixedMsg.h"
#include "EC_Resource.h"
#include "EC_SceneLoader.h"
#include "EC_Model.h"
#include "EC_Faction.h"
#include "EC_InputCtrl.h"
#include "EC_ManMatter.h"
#include "EC_MemSimplify.h"

#include "PlayerBaseInfo_Re.hpp"
#include "GetCustomData_Re.hpp"

#include "EC_HPWorkForceNavigate.h"
#include "EC_PlayerClone.h"

#include "A3DCamera.h"
#include "A2DSprite.h"
#include "A3DCollision.h"
#include "A3DTerrain2.h"
#include "A2DSpriteBuffer.h"
#include "A3DFont.h"
#include "EC_Goblin.h"

#define new A_DEBUG_NEW

///////////////////////////////////////////////////////////////////////////
//	
//	Define and Macro
//	
///////////////////////////////////////////////////////////////////////////

#define MAX_CACHEDPLAYER	1024	//	Maximum cached player number

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
//	Implement CECPlayerMan
//	
///////////////////////////////////////////////////////////////////////////

CECPlayerMan::CECPlayerMan(CECGameRun* pGameRun) :
CECManager(pGameRun),
m_PlayerTab(512),
m_UkPlayerTab(32),
m_PlayerCache(MAX_CACHEDPLAYER),
m_aMMPlayers(0, 128)
{
	m_iManagerID	= MAN_PLAYER;

	m_QueryCnt.SetPeriod(3000);
	m_CacheCnt.SetPeriod(300000);
	m_TLTimeCnt.SetPeriod(2000);

	InitializeCriticalSection(&m_csLoad);
	InitializeCriticalSection(&m_csPlayerTab);
}

CECPlayerMan::~CECPlayerMan()
{
	DeleteCriticalSection(&m_csLoad);
	DeleteCriticalSection(&m_csPlayerTab);
}

//	Release manager
void CECPlayerMan::Release()
{
	OnLeaveGameWorld();
}

//	On entering game world
bool CECPlayerMan::OnEnterGameWorld()
{
	return true;
}

//	On leaving game world
bool CECPlayerMan::OnLeaveGameWorld()
{
	//	Release all else players
	if (m_PlayerTab.size())
	{
		PlayerTable::iterator it = m_PlayerTab.begin();
		for (; it != m_PlayerTab.end(); ++it)
		{
			CECElsePlayer* pPlayer = *it.value();
			ReleaseElsePlayer(pPlayer);
		}

		ACSWrapper cspt(&m_csPlayerTab);
		m_PlayerTab.clear();
	}

	//	Release all cached player data
	if (m_PlayerCache.size())
	{
		CacheTable::iterator it = m_PlayerCache.begin();
		for (; it != m_PlayerCache.end(); ++it)
		{
			CECEPCacheData* pData = *it.value();
			delete pData;
		}

		m_PlayerCache.clear();
	}
	
	//	Release all loaded models
	ACSWrapper csa(&m_csLoad);

	int i;
	for (i=0; i < m_aLoadedModels.GetSize(); i++)
	{
		PLAYERMODEL* pInfo = m_aLoadedModels[i];
		CECPlayer::ReleasePlayerModel(pInfo->Ret);
		delete pInfo;
	}

	m_aLoadedModels.RemoveAll();

	return true;
}

//	Get host player
CECHostPlayer* CECPlayerMan::GetHostPlayer()
{
	return m_pGameRun->GetHostPlayer();
}

//	Process message
bool CECPlayerMan::ProcessMessage(const ECMSG& Msg)
{
	ASSERT(Msg.iManager == MAN_PLAYER);

	if (!Msg.iSubID)
	{
		//	Messages to host player
		GetHostPlayer()->ProcessMessage(Msg);
	}
	else if (Msg.iSubID < 0)
	{
		switch (Msg.dwMsg)
		{
		case MSG_PM_PLAYERINFO:			OnMsgPlayerInfo(Msg);			break;
		case MSG_PM_PLAYERMOVE:			OnMsgPlayerMove(Msg);			break;
		case MSG_PM_PLAYERRUNOUT:		OnMsgPlayerRunOut(Msg);			break;
		case MSG_PM_PLAYEREXIT:			OnMsgPlayerExit(Msg);			break;
		case MSG_PM_PLAYERDIED:			OnMsgPlayerDied(Msg);			break;
		case MSG_PM_PLAYERREVIVE:		OnMsgPlayerRevive(Msg);			break;
		case MSG_PM_PLAYEROUTOFVIEW:	OnMsgPlayerOutOfView(Msg);		break;
		case MSG_PM_PLAYERDISAPPEAR:	OnMsgPlayerDisappear(Msg);		break;
		case MSG_PM_PLAYERSTOPMOVE:		OnMsgPlayerStopMove(Msg);		break;
		case MSG_PM_PLAYERLEVELUP:		OnMsgPlayerLevelUp(Msg);		break;
		case MSG_PM_GOBLINLEVELUP:		OnMsgGoblinLevelUp(Msg);		break;
		case MSG_PM_PLAYEREXTPROP:		OnMsgPlayerExtProp(Msg);		break;
		case MSG_PM_LEAVETEAM:			OnMsgPlayerLeaveTeam(Msg);		break;
		case MSG_PM_DOCONEMOTE:			OnMsgDoConEmote(Msg);			break;
		case MSG_PM_INVALIDOBJECT:		OnMsgInvalidObject(Msg);		break;
		case MSG_PM_PLAYEREXTSTATE:		OnMsgPlayerExtState(Msg);		break;
		case MSG_PM_PICKUPMATTER:		OnMsgPickUpMatter(Msg);			break;
		case MSG_PM_PLAYERBINDSTART:	OnMsgPlayerBindStart(Msg);		break;
		case MSG_PM_PLAYERDUELRLT:		OnMsgPlayerDuelResult(Msg);		break;
		case MSG_PM_TANKLEADER:			OnMsgTankLeader(Msg);			break;
		case MSG_PM_PLAYERCHANGESPOUSE:	OnMsgPlayerChangeSpouse(Msg);	break;
		case MSG_PM_PLAYERINVISIBLE:	OnMsgPlayerInvisible(Msg);		break;
		case MSG_PM_FORBIDBESELECTED:	OnMsgForbidBeSelected(Msg);		break;

		case MSG_PM_PLAYEREQUIPDATA:
		case MSG_PM_PLAYERBASEINFO:
		case MSG_PM_PLAYERCUSTOM:
		case MSG_PM_PLAYERFLY:
		case MSG_PM_PLAYERSITDOWN:
		case MSG_PM_CASTSKILL:
		case MSG_PM_CHANGENAMECOLOR:
		case MSG_PM_PLAYERROOT:
		case MSG_PM_PLAYERATKRESULT:
		case MSG_PM_PLAYERDOEMOTE:
		case MSG_PM_PLAYERUSEITEM:
		case MSG_PM_ENCHANTRESULT:
		case MSG_PM_PLAYERDOACTION:
		case MSG_PM_PLAYERSKILLRESULT:
		case MSG_PM_PLAYERADVDATA:
		case MSG_PM_PLAYERGATHER:
		case MSG_PM_PLAYERINTEAM:
		case MSG_PM_PLAYERCHGSHAPE:
		case MSG_PM_GOBLINOPT:
		case MSG_PM_BOOTHOPT:
		case MSG_PM_PLAYERTRAVEL:
		case MSG_PM_PLAYERPVP:
		case MSG_PM_FASHIONENABLE:	
		case MSG_PM_PLAYEREFFECT:
		case MSG_PM_CHANGEFACE:
		case MSG_PM_PLAYERBINDOPT:
		case MSG_PM_PLAYERMOUNT:
		case MSG_PM_PLAYERDUELOPT:
		case MSG_PM_PLAYERLEVEL2:
		case MSG_PM_PLAYERKNOCKBACK:
		case MSG_PM_PLAYEREQUIPDISABLED:
		case MSG_PM_CONGREGATE:
		case MSG_PM_TELEPORT:
		case MSG_PM_FORCE_CHANGED:
		case MSG_PM_MULTIOBJ_EFFECT:
		case MSG_PM_COUNTRY_CHANGED:
		case MSG_PM_KINGCHANGED:
		case MSG_PM_TITLE:
		case MSG_PM_REINCARNATION:
		case MSG_PM_REALMLEVEL:
		case MSG_PM_PLAYER_IN_OUT_BATTLE:
		case MSG_PM_FACTION_PVP_MASK_MODIFY:
			TransmitMessage(Msg);
			break;

		default:
			ASSERT(0);
			break;
		}
	}
	else
	{
		//	Messages to specified else player
	}

	return true;
}

//	Get else player by id
CECElsePlayer* CECPlayerMan::GetElsePlayer(int cid, DWORD dwBornStamp/* 0 */)
{
	ACSWrapper cspt(&m_csPlayerTab);		// 这里加个锁，因为此函数会被后台加载线程调用到
	PlayerTable::pair_type Pair = m_PlayerTab.get(cid);
	if (!Pair.second)
		return NULL;

	if (dwBornStamp)
	{
		CECPlayer* pPlayer = *Pair.first;
		if (pPlayer->GetBornStamp() != dwBornStamp)
			return NULL;
	}

	return *Pair.first;
}

//	Get a player (may be host or else player) by id
CECPlayer* CECPlayerMan::GetPlayer(int cid, DWORD dwBornStamp/* 0 */)
{
	CECHostPlayer* pHost = GetHostPlayer();
	if (pHost && pHost->GetCharacterID() == cid)
		return pHost;
	else
		return GetElsePlayer(cid, dwBornStamp);
}

//	Seek out else player with specified id, if he doesn't exist, try to get from server
CECElsePlayer* CECPlayerMan::SeekOutElsePlayer(int cid)
{
	PlayerTable::pair_type Pair = m_PlayerTab.get(cid);
	if (!Pair.second)
	{
		//	Counldn't find this else player, put it into unkonwn player table
		m_UkPlayerTab.put(cid, cid);
		return NULL;
	}

	return *Pair.first;
}

//	Player information messages handler
bool CECPlayerMan::OnMsgPlayerInfo(const ECMSG& Msg)
{
	using namespace S2C;

	CECGameSession* pSession = g_pGame->GetGameSession();
	int iHostID = pSession->GetCharacterID();

	BYTE* pDataBuf = (BYTE*)Msg.dwParam1;
	ASSERT(pDataBuf);

	switch (Msg.dwParam2)
	{
	case PLAYER_INFO_1:
	case PLAYER_ENTER_WORLD:
	case PLAYER_ENTER_SLICE:
	{
		info_player_1* pInfo = (info_player_1*)pDataBuf;

		if (pInfo->cid != iHostID)
		{
			CECElsePlayer* pPlayer = ElsePlayerEnter(*pInfo, Msg.dwParam2);
			if (pPlayer)
			{
				//	Is custom data ready
				if (!pPlayer->IsCustomDataReady())
				{
					if (!pPlayer->IsBaseInfoReady())
						pSession->GetRoleBaseInfo(1, &pInfo->cid);
					else
						pSession->GetRoleCustomizeData(1, &pInfo->cid);
				}

				//	Is equipment data ready
				if (!pPlayer->IsEquipDataReady())
					pSession->c2s_CmdGetOtherEquip(1, &pInfo->cid);

				//	Get faction info
				if (pPlayer->GetFactionID())
					g_pGame->GetFactionMan()->GetFaction(pPlayer->GetFactionID(), true);
			}
		}

		break;
	}
	case PLAYER_INFO_2:			break;
	case PLAYER_INFO_3:			break;
	case PLAYER_INFO_4:			break;

	case PLAYER_INFO_1_LIST:
	{
		cmd_player_info_1_list* pCmd = (cmd_player_info_1_list*)pDataBuf;
		if (pCmd->count == 0) break;

		pDataBuf = &pCmd->placeholder;

		AArray<int, int> a1(0, pCmd->count);
		AArray<int, int> a2(0, pCmd->count);
		AArray<int, int> a3(0, pCmd->count);
		AArray<int, int> a4(0, pCmd->count);

		for (int i=0; i < pCmd->count; i++)
		{
			const info_player_1& Info = *(const info_player_1*)pDataBuf;

			if (Info.cid != iHostID)
			{
				CECElsePlayer* pPlayer = ElsePlayerEnter(Info, Msg.dwParam2);
				if (pPlayer)
				{
					//	Is custom data ready
					if (!pPlayer->IsCustomDataReady())
					{
						if (!pPlayer->IsBaseInfoReady())
							a2.Add(Info.cid);
						else
							a3.Add(Info.cid);
					}

					//	Is equipment data ready
					if (!pPlayer->IsEquipDataReady())
						a1.Add(Info.cid);

					// Get faction info
					if (pPlayer->GetFactionID() && !g_pGame->GetFactionMan()->GetFaction(pPlayer->GetFactionID()))
					{
						int i(0);
						for (i = 0; i < a4.GetSize(); i++)
							if (a4[i] == pPlayer->GetFactionID())
								break;

						if (i == a4.GetSize())
							a4.Add(pPlayer->GetFactionID());
					}
				}
			}

			//	Calculate player info data size and skip it
			int iSize = sizeof (info_player_1);
			if (Info.state & GP_STATE_ADV_MODE)
				iSize += sizeof (int) * 2;

			if (Info.state & GP_STATE_SHAPE)
				iSize += sizeof (BYTE);

			if (Info.state & GP_STATE_EMOTE)
				iSize += sizeof (BYTE);

			if (Info.state & GP_STATE_EXTEND_PROPERTY)
				iSize += sizeof (DWORD) * OBJECT_EXT_STATE_COUNT;

			if (Info.state & GP_STATE_FACTION)
				iSize += (sizeof (int) + sizeof (BYTE));

			if (Info.state & GP_STATE_BOOTH)
				iSize += sizeof (BYTE);
			
			//	Parse effect data
			if (Info.state & GP_STATE_EFFECT)
			{
				BYTE byNum = *(pDataBuf + iSize);
				iSize += sizeof (BYTE);

				if (byNum)
					iSize += byNum * sizeof (short);
			}

			if (Info.state & GP_STATE_PARIAH)
				iSize += sizeof (BYTE);

			if (Info.state & GP_STATE_IN_MOUNT)
				iSize += sizeof (unsigned short) + sizeof (int);

			//	Parse bind data
			if (Info.state & GP_STATE_IN_BIND)
				iSize += sizeof (char) + sizeof (int);

			//	Parse spouse data
			if (Info.state & GP_STATE_SPOUSE)
				iSize += sizeof(int);

			if (Info.state & GP_STATE_EQUIPDISABLED)
				iSize += sizeof(__int64);

			if (Info.state & GP_STATE_PLAYERFORCE)
				iSize += sizeof(int);

			if (Info.state & GP_STATE_MULTIOBJ_EFFECT)
			{
				int effectNum = *(int *)(pDataBuf + iSize);
				iSize += sizeof (int);

				if (effectNum)
					iSize += effectNum * (sizeof(int) + sizeof(char));	
			}

			if (Info.state & GP_STATE_COUNTRY)
				iSize += sizeof(int);
			if (Info.state2 & GP_STATE2_TITLE)
				iSize += sizeof(unsigned short);
			if (Info.state2 & GP_STATE2_REINCARNATION)
				iSize += sizeof(unsigned char);
			if (Info.state2 & GP_STATE2_REALM)
				iSize += sizeof(unsigned char);
			if (Info.state2 & GP_STATE2_FACTION_PVP_MASK)
				iSize += sizeof(unsigned char);
			
			//  Goblin refine data
// 			if (Info->state & GP_STATE_GOBLINREFINE)
//				iSize += sizeof(int);

			pDataBuf += iSize;
		}

		//	Get both base info and custom data
		if (a2.GetSize())
			pSession->CacheGetRoleBaseInfo(a2.GetSize(), a2.GetData());

		//	Get new equipment data
		if (a1.GetSize())
			pSession->c2s_CmdGetOtherEquip(a1.GetSize(), a1.GetData());

		//	Only get custom data
		if (a3.GetSize())
			pSession->GetRoleCustomizeData(a3.GetSize(), a3.GetData());

		//	Get factions info
		if (a4.GetSize())
			pSession->GetFactionInfo(a4.GetSize(), a4.GetData());

		break;
	}
	case PLAYER_INFO_2_LIST:	break;
	case PLAYER_INFO_3_LIST:	break;
	case PLAYER_INFO_23_LIST:	break;

	case SELF_INFO_1:

		HostPlayerInfo1(*(cmd_self_info_1*)pDataBuf);
		break;

	case PLAYER_INFO_00:
	{
		cmd_player_info_00* pCmd = (cmd_player_info_00*)pDataBuf;
		CECPlayer* pPlayer = GetPlayer(pCmd->idPlayer);
		if (pPlayer)
		{
			ROLEBASICPROP bp = pPlayer->GetBasicProps();
			ROLEEXTPROP ep = pPlayer->GetExtendProps();

			bp.iLevel		= pCmd->sLevel;
			bp.iLevel2		= pCmd->Level2;
			bp.iCurHP		= pCmd->iHP;
			bp.iCurMP		= pCmd->iMP;
			ep.bs.max_hp	= pCmd->iMaxHP;
			ep.bs.max_mp	= pCmd->iMaxMP;

			pPlayer->SetProps(&bp, &ep);
			pPlayer->SetSelectedTarget(pCmd->iTargetID);
		
		//	CuiMing told me don't use this flag, because it isn't
		//	updated in time.
		//	pPlayer->SetFightFlag(pCmd->State ? true : false);
		}
		
		break;
	}
	}

	return true;
}

//	Handler of MSG_PM_PLAYERMOVE
bool CECPlayerMan::OnMsgPlayerMove(const ECMSG& Msg)
{
	using namespace S2C;

	CECGameSession* pSession = g_pGame->GetGameSession();
	int iHostID = pSession->GetCharacterID();

	cmd_object_move* pCmd = (cmd_object_move*)Msg.dwParam1;
	ASSERT(pCmd);

	if (!pCmd->use_time)
		return true;

	if (pCmd->id != iHostID)
	{
		CECElsePlayer* pPlayer = SeekOutElsePlayer(pCmd->id);
		if (pPlayer)
			pPlayer->MoveTo(*pCmd);
	}

	return true;
}

//	Handler of MSG_PM_PLAYERRUNOUT
bool CECPlayerMan::OnMsgPlayerRunOut(const ECMSG& Msg)
{
	using namespace S2C;

	CECGameSession* pSession = g_pGame->GetGameSession();

	cmd_leave_slice* pCmd = (cmd_leave_slice*)Msg.dwParam1;
	ASSERT(pCmd && pCmd->id != pSession->GetCharacterID());

	ElsePlayerLeave(pCmd->id, false);

	return true;
}

//	Handler of MSG_PM_PLAYEREXIT
bool CECPlayerMan::OnMsgPlayerExit(const ECMSG& Msg)
{
	using namespace S2C;

	CECGameSession* pSession = g_pGame->GetGameSession();

	cmd_player_leave_world* pCmd = (cmd_player_leave_world*)Msg.dwParam1;
	ASSERT(pCmd);
	ASSERT(pCmd->id != pSession->GetCharacterID());

	ElsePlayerLeave(pCmd->id, true);

	return true;
}

//	Handler of MSG_PM_PLAYERDIED
bool CECPlayerMan::OnMsgPlayerDied(const ECMSG& Msg)
{
	using namespace S2C;

	cmd_player_died* pCmd = (cmd_player_died*)Msg.dwParam1;
	ASSERT(pCmd && ISPLAYERID(pCmd->idPlayer));

	CECElsePlayer* pPlayer = SeekOutElsePlayer(pCmd->idPlayer);
	if (pPlayer && !pPlayer->IsAboutToDie())
		pPlayer->Killed(pCmd->idKiller);
	
	return true;
}

//	Handler of MSG_PM_PLAYERREVIVE
bool CECPlayerMan::OnMsgPlayerRevive(const ECMSG& Msg)
{
	using namespace S2C;

	cmd_player_revive* pCmd = (cmd_player_revive*)Msg.dwParam1;
	ASSERT(pCmd && ISPLAYERID(pCmd->idPlayer));

	CECGameSession* pSession = g_pGame->GetGameSession();

	if (pCmd->idPlayer == pSession->GetCharacterID())
	{
		GetHostPlayer()->Revive(pCmd);
	}
	else
	{
		CECElsePlayer* pPlayer = SeekOutElsePlayer(pCmd->idPlayer);
		if (pPlayer)
			pPlayer->Revive(pCmd);
	}

	return true;
}

bool CECPlayerMan::OnMsgPlayerOutOfView(const ECMSG& Msg)
{
	using namespace S2C;

	int id = (int)Msg.dwParam1;
	CECGameSession* pSession = g_pGame->GetGameSession();
	ASSERT(id != pSession->GetCharacterID());

	ElsePlayerLeave(id, false);

	return true;
}

bool CECPlayerMan::OnMsgPlayerDisappear(const ECMSG& Msg)
{
	using namespace S2C;

	CECGameSession* pSession = g_pGame->GetGameSession();

	cmd_object_disappear* pCmd = (cmd_object_disappear*)Msg.dwParam1;
	ASSERT(pCmd && ISPLAYERID(pCmd->id));
	ASSERT(pCmd->id != pSession->GetCharacterID());

	ElsePlayerLeave(pCmd->id, false);

	return true;
}

bool CECPlayerMan::OnMsgPlayerStopMove(const ECMSG& Msg)
{
	using namespace S2C;

	CECGameSession* pSession = g_pGame->GetGameSession();

	cmd_object_stop_move* pCmd = (cmd_object_stop_move*)Msg.dwParam1;
	ASSERT(pCmd && ISPLAYERID(pCmd->id));
	ASSERT(pCmd->id != pSession->GetCharacterID());

	CECElsePlayer* pPlayer = SeekOutElsePlayer(pCmd->id);
	if (pPlayer)
		pPlayer->StopMoveTo(*pCmd);

	return true;
}

bool CECPlayerMan::OnMsgPlayerLevelUp(const ECMSG& Msg)
{
	using namespace S2C;

	cmd_level_up* pCmd = (cmd_level_up*)Msg.dwParam1;
	ASSERT(pCmd && ISPLAYERID(pCmd->id));

	CECGameSession* pSession = g_pGame->GetGameSession();

	if (pCmd->id == pSession->GetCharacterID())
	{
		GetHostPlayer()->LevelUp();
	}
	else
	{
		CECElsePlayer* pPlayer = SeekOutElsePlayer(pCmd->id);
		if (pPlayer)
			pPlayer->LevelUp();
	}

	return true;
}

bool CECPlayerMan::OnMsgGoblinLevelUp(const ECMSG& Msg)
{
	using namespace S2C;

	cmd_goblin_level_up* pCmd = (cmd_goblin_level_up*)Msg.dwParam1;
	ASSERT(pCmd && ISPLAYERID(pCmd->player_id));

	CECGameSession* pSession = g_pGame->GetGameSession();

	if (pCmd->player_id == pSession->GetCharacterID())
	{
		if(GetHostPlayer()->GetGoblinModel())
		{
			if(!GetHostPlayer()->GetRenderGoblin())
				GetHostPlayer()->SetRenderGoblin(true);
			GetHostPlayer()->SetGoblinRenderCnt(5000);
			GetHostPlayer()->GetGoblinModel()->LevelUp();
		}
	}
	else
	{
		CECElsePlayer* pPlayer = SeekOutElsePlayer(pCmd->player_id);
		if (pPlayer)
		{
			if(pPlayer->GetGoblinModel())
				pPlayer->GetGoblinModel()->LevelUp();
		}
	}

	return true;
}

bool CECPlayerMan::OnMsgPlayerInvisible(const ECMSG& Msg)
{
	using namespace S2C;

	cmd_object_invisible* pCmd = (cmd_object_invisible*)Msg.dwParam1;
	ASSERT(pCmd && ISPLAYERID(pCmd->id));

	CECGameSession* pSession = g_pGame->GetGameSession();

	if (pCmd->id == pSession->GetCharacterID())
	{
		GetHostPlayer()->ChangeInvisible(pCmd->invisible_degree);
	}
	else
	{
		CECElsePlayer* pPlayer = SeekOutElsePlayer(pCmd->id);
		if (pPlayer)
			pPlayer->ChangeInvisible(pCmd->invisible_degree);
	}

	return true;
}

bool CECPlayerMan::OnMsgForbidBeSelected(const ECMSG& Msg)
{
	using namespace S2C;

	cmd_object_forbid_be_selected* pCmd = (cmd_object_forbid_be_selected*)Msg.dwParam1;
	ASSERT(pCmd && ISPLAYERID(pCmd->id));

	CECPlayer* pPlayer = NULL;
	CECGameSession* pSession = g_pGame->GetGameSession();

	if (pCmd->id == pSession->GetCharacterID())
	{
		pPlayer = GetHostPlayer();
	}
	else
	{
		pPlayer = SeekOutElsePlayer(pCmd->id);
	}

	if(pPlayer)
	{
		pPlayer->SetSelectable(pCmd->b == 0);
	}

	return true;
}


bool CECPlayerMan::OnMsgPlayerExtProp(const ECMSG& Msg)
{
	using namespace S2C;

	void* pData;
	int idPlayer, iIndex;

	switch (Msg.dwParam2)
	{
	case PLAYER_EXT_PROP_BASE:
	{
		cmd_pep_base* pCmd = (cmd_pep_base*)Msg.dwParam1;
		idPlayer	= pCmd->idPlayer;
		pData		= &pCmd->ep_base;
		iIndex		= EXTPROPIDX_BASE;
		break;
	}
	case PLAYER_EXT_PROP_MOVE:
	{
		cmd_pep_move* pCmd = (cmd_pep_move*)Msg.dwParam1;
		idPlayer	= pCmd->idPlayer;
		pData		= &pCmd->ep_move;
		iIndex		= EXTPROPIDX_MOVE;
		break;
	}
	case PLAYER_EXT_PROP_ATK:
	{
		cmd_pep_attack* pCmd = (cmd_pep_attack*)Msg.dwParam1;
		idPlayer	= pCmd->idPlayer;
		pData		= &pCmd->ep_attack;
		iIndex		= EXTPROPIDX_ATTACK;
		break;
	}
	case PLAYER_EXT_PROP_DEF:
	{
		cmd_pep_def* pCmd = (cmd_pep_def*)Msg.dwParam1;
		idPlayer	= pCmd->idPlayer;
		pData		= &pCmd->ep_def;
		iIndex		= EXTPROPIDX_DEF;
		break;
	}
	default:
		ASSERT(0);
		return false;
	}

	if (!ISPLAYERID(idPlayer))
	{
		ASSERT(ISPLAYERID(idPlayer));
		return false;
	}

	CECGameSession* pSession = g_pGame->GetGameSession();

	if (idPlayer == pSession->GetCharacterID())
	{
		GetHostPlayer()->SetPartExtendProps(iIndex, pData);
	}
	else
	{
		CECElsePlayer* pPlayer = SeekOutElsePlayer(idPlayer);
		if (pPlayer)
			pPlayer->SetPartExtendProps(iIndex, pData);
	}

	return true;
}

bool CECPlayerMan::OnMsgPlayerLeaveTeam(const ECMSG& Msg)
{
	using namespace S2C;

	cmd_team_member_leave* pCmd = (cmd_team_member_leave*)Msg.dwParam1;
	ASSERT(pCmd);

	CECTeamMan* pTeamMan = g_pGame->GetGameRun()->GetTeamMan();
	CECTeam* pTeam = pTeamMan->GetTeam(pCmd->idLeader);
	if (!pTeam)
	{
		ASSERT(pTeam);
		return false;
	}

	CECTeamMember* pMember = pTeam->GetMemberByID(pCmd->idMember);
	if (!pMember)
	{
		ASSERT(pMember);
		return false;
	}

	//	In cmd_team_member_leave, reason won't be GP_LTR_LEADERCANCEL
	if (pCmd->reason != GP_LTR_LEADERCANCEL)
	{
		//	Display the reason
		if (pCmd->reason == GP_LTR_KICKEDOUT)
			g_pGame->GetGameRun()->AddFixedMessage(FIXMSG_TEAMMEMKICKEDOUT, pMember->GetName());
		else
			g_pGame->GetGameRun()->AddFixedMessage(FIXMSG_LEAVETEAM, pMember->GetName());
	}

	pTeam->RemoveMember(pCmd->idMember);

	//	Update team UI
	CECGameUIMan* pGameUI = g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan();
	pGameUI->UpdateTeam();

	return true;
}

bool CECPlayerMan::OnMsgDoConEmote(const ECMSG& Msg)
{
	using namespace S2C;
	cmd_do_concurrent_emote* pCmd = (cmd_do_concurrent_emote*)Msg.dwParam1;
	ASSERT(pCmd && Msg.dwParam2 == DO_CONCURRENT_EMOTE);

	CECHostPlayer* pHost = GetHostPlayer();

	if (pCmd->id1)
	{
		if (pCmd->id1 == pHost->GetCharacterID())
			pHost->ProcessMessage(Msg);
		else
		{
			CECElsePlayer* pPlayer = SeekOutElsePlayer(pCmd->id1);
			if (pPlayer)
				pPlayer->ProcessMessage(Msg);
		}
	}
	
	if (pCmd->id2)
	{
		if (pCmd->id2 == pHost->GetCharacterID())
			pHost->ProcessMessage(Msg);
		else
		{
			CECElsePlayer* pPlayer = SeekOutElsePlayer(pCmd->id2);
			if (pPlayer)
				pPlayer->ProcessMessage(Msg);
		}
	}
	
	return true;
}

bool CECPlayerMan::OnMsgInvalidObject(const ECMSG& Msg)
{
	using namespace S2C;
	cmd_invalid_object* pCmd = (cmd_invalid_object*)Msg.dwParam1;
	ASSERT(pCmd);

	//	Remove the player if it exists
	CECElsePlayer* pPlayer = GetElsePlayer(pCmd->id);
	if (pPlayer)
	{
		ElsePlayerLeave(pCmd->id, false);
	}
	
	return true;
}

bool CECPlayerMan::OnMsgPlayerExtState(const ECMSG& Msg)
{
	using namespace S2C;

	int cid = 0;

	cmd_icon_state_notify cmd;

	if (Msg.dwParam2 == UPDATE_EXT_STATE)
		cid = ((cmd_update_ext_state*)Msg.dwParam1)->id;
	else if (Msg.dwParam2 == ICON_STATE_NOTIFY)
	{
		if (!cmd.Initialize((const void *)Msg.dwParam1, Msg.dwParam3))
		{
			ASSERT(0);
			return false;
		}
		cid = cmd.id;
	}
	else
	{
		ASSERT(0);
		return false;
	}

	CECHostPlayer* pHost = g_pGame->GetGameRun()->GetHostPlayer();

	if (cid == pHost->GetCharacterID())
		pHost->ProcessMessage(Msg);
	else
	{
		CECElsePlayer* pPlayer = SeekOutElsePlayer(cid);
		if (pPlayer)
			pPlayer->ProcessMessage(Msg);

		if (Msg.dwParam2 == ICON_STATE_NOTIFY && pHost->GetTeam())
		{
			//	Update host's team member's icon state
			CECTeam* pTeam = pHost->GetTeam();
			CECTeamMember* pMember = pTeam->GetMemberByID(cid);
			if (pMember)
				pMember->ResetIconStates(cmd.states);
		}
	}

	return true;
}

void CECPlayerMan::OnMsgPickUpMatter(const ECMSG& Msg)
{
	using namespace S2C;
	cmd_matter_pickup* pCmd = (cmd_matter_pickup*)Msg.dwParam1;
	CECHostPlayer* pHost = g_pGame->GetGameRun()->GetHostPlayer();

	if (pCmd->who == pHost->GetCharacterID())
		pHost->ProcessMessage(Msg);
	else
	{
		CECElsePlayer* pPlayer = SeekOutElsePlayer(pCmd->who);
		if (pPlayer)
			pPlayer->ProcessMessage(Msg);
	}

	//	Remove matter from world
	CECMatterMan* pMatterMan = g_pGame->GetGameRun()->GetWorld()->GetMatterMan();
	pMatterMan->RemoveMatter(pCmd->matter_id);
}

void CECPlayerMan::OnMsgPlayerBindStart(const ECMSG& Msg)
{
	using namespace S2C;
	const cmd_player_bind_start* pCmd = (const cmd_player_bind_start*)Msg.dwParam1;

	CECPlayer* pPlayer = GetPlayer(pCmd->mule);
	if (pPlayer)
		pPlayer->OnStartBinding(pCmd->mule, pCmd->rider);

	pPlayer = GetPlayer(pCmd->rider);
	if (pPlayer)
		pPlayer->OnStartBinding(pCmd->mule, pCmd->rider);
}

void CECPlayerMan::OnMsgPlayerDuelResult(const ECMSG& Msg)
{
	using namespace S2C;
	const cmd_duel_result* pCmd = (const cmd_duel_result*)Msg.dwParam1;
	CECGameRun* pGameRun = g_pGame->GetGameRun();

	const ACHAR* szName1 = pGameRun->GetPlayerName(pCmd->id1, false);
	const ACHAR* szName2 = pGameRun->GetPlayerName(pCmd->id2, false);
	if (!szName1 || !szName2)
		return;

	int r1=0, r2=0;

	if (pCmd->result == 0)
	{
		r1 = r2 = 3;
		pGameRun->AddFixedMessage(FIXMSG_DUEL_DRAW, szName1, szName2);
	}
	else if (pCmd->result == 1)
	{
		r1 = 1;
		r2 = 2;
		pGameRun->AddFixedMessage(FIXMSG_DUEL_WIN, szName1, szName2);
	}

	CECPlayer* pPlayer = GetPlayer(pCmd->id1);
	if (pPlayer)
		pPlayer->SetDuelResult(r1);

	pPlayer = GetPlayer(pCmd->id2);
	if (pPlayer)
		pPlayer->SetDuelResult(r2);
}

void CECPlayerMan::OnMsgTankLeader(const ECMSG& Msg)
{
	using namespace S2C;
	const cmd_tank_leader_notify* pCmd = (const cmd_tank_leader_notify*)Msg.dwParam1;

	if (pCmd->idLeader)
	{
		CECPlayer* pPlayer = GetPlayer(pCmd->idLeader);
		if (pPlayer)
			pPlayer->ChangeTankLeader(pCmd->idTank, true);
	}
	else
	{
		CECHostPlayer* pHost = GetHostPlayer();
		if (pHost->GetBattleTankNum())
			pHost->ChangeTankLeader(pCmd->idTank, false);

		//	Handle all else players
		PlayerTable::iterator it = m_PlayerTab.begin();
		for (; it != m_PlayerTab.end(); ++it)
		{
			CECElsePlayer* pPlayer = *it.value();
			if (pPlayer->GetBattleTankNum())
				pPlayer->ChangeTankLeader(pCmd->idTank, false);
		}
	}
}

void CECPlayerMan::OnMsgPlayerChangeSpouse(const ECMSG& Msg)
{
	using namespace S2C;
	const player_change_spouse* pCmd = (const player_change_spouse*)Msg.dwParam1;

	CECPlayer* pPlayer = GetPlayer(pCmd->who);
	if (pPlayer)
	{
		// See if need add a fixed message here.
		if( pPlayer == g_pGame->GetGameRun()->GetHostPlayer() && pPlayer->GetSpouse() && pCmd->spouse == 0 )
			g_pGame->GetGameRun()->AddFixedMessage(FIXMSG_GOTDIVORCED, g_pGame->GetGameRun()->GetPlayerName(pPlayer->GetSpouse(), true));
		else if( pPlayer == g_pGame->GetGameRun()->GetHostPlayer() && pPlayer->GetSpouse() == 0 && pCmd->spouse )
			g_pGame->GetGameRun()->AddFixedMessage(FIXMSG_GOTMARRIED, g_pGame->GetGameRun()->GetPlayerName(pCmd->spouse, true));

		pPlayer->SetSpouse(pCmd->spouse);
	}
}

//	Transmit message
bool CECPlayerMan::TransmitMessage(const ECMSG& Msg)
{
	using namespace S2C;
	using namespace GNET;

	int cid = 0;
	CECHostPlayer* pHost = GetHostPlayer();

	switch (Msg.dwMsg)
	{
	case MSG_PM_PLAYEREQUIPDATA:
		
		if (Msg.dwParam2 == EQUIP_DATA)
			cid = ((cmd_equip_data*)Msg.dwParam1)->idPlayer;
		else	//	Msg.dwParam2 == EQUIP_DATA_CHANGED
			cid = ((cmd_equip_data_changed*)Msg.dwParam1)->idPlayer;

		break;

	case MSG_PM_PLAYERBASEINFO:
		{
			cid = ((PlayerBaseInfo_Re*)Msg.dwParam1)->player.id;

			// 完成PlayerBaseInfo请求，从CACHE中清除
			CECC2SCmdCache* pCache = g_pGame->GetGameSession()->GetC2SCmdCache();
			pCache->RemovePlayerBaseInfo(cid);
		}
		break;

	case MSG_PM_PLAYERCUSTOM:

		cid = ((GetCustomData_Re*)Msg.dwParam1)->cus_roleid;
		break;

	case MSG_PM_PLAYERFLY:
		
		if (Msg.dwParam2 == OBJECT_TAKEOFF)
			cid = ((cmd_object_takeoff*)Msg.dwParam1)->object_id;
		else	//	OBJECT_LANDING
			cid = ((cmd_object_landing*)Msg.dwParam1)->object_id;

		break;

	case MSG_PM_PLAYERSITDOWN:
		
		if (Msg.dwParam2 == OBJECT_SIT_DOWN)
			cid = ((cmd_object_sit_down*)Msg.dwParam1)->id;
		else	//	OBJECT_STAND_UP
			cid = ((cmd_object_stand_up*)Msg.dwParam1)->id;

		break;

	case MSG_PM_CASTSKILL:
		
		switch (Msg.dwParam2)
		{
		case OBJECT_CAST_SKILL:			cid = ((cmd_object_cast_skill*)Msg.dwParam1)->caster;			break;
		case OBJECT_CAST_INSTANT_SKILL:	cid = ((cmd_object_cast_instant_skill*)Msg.dwParam1)->caster;	break;
		case OBJECT_CAST_POS_SKILL:		cid = ((cmd_object_cast_pos_skill*)Msg.dwParam1)->caster;		break;
		case SKILL_INTERRUPTED:			cid = ((cmd_skill_interrupted*)Msg.dwParam1)->caster;			break;
		case PLAYER_CAST_RUNE_SKILL:	cid = ((cmd_player_cast_rune_skill*)Msg.dwParam1)->caster;		break;
		case PLAYER_CAST_RUNE_INSTANT_SKILL:	cid = ((cmd_player_cast_rune_instant_skill*)Msg.dwParam1)->caster;		break;
		}
		
		break;

	case MSG_PM_CHANGENAMECOLOR:

		switch (Msg.dwParam2)
		{
		case INVADER_RISE:		cid = ((cmd_invader_rise*)Msg.dwParam1)->id;	break;
		case PARIAH_RISE:		cid = ((cmd_pariah_rise*)Msg.dwParam1)->id;		break;
		case INVADER_FADE:		cid = ((cmd_invader_fade*)Msg.dwParam1)->id;	break;
		}
			
		break;

	case MSG_PM_PLAYERROOT:

		ASSERT(Msg.dwParam2 == OBJECT_ROOT);
		cid = ((cmd_object_root*)Msg.dwParam1)->id;
		break;

	case MSG_PM_PLAYERATKRESULT:

		ASSERT(Msg.dwParam2 == OBJECT_ATTACK_RESULT);
		cid = ((cmd_object_atk_result*)Msg.dwParam1)->attacker_id;
		break;

	case MSG_PM_PLAYERDOEMOTE:
		
		if (Msg.dwParam2 == OBJECT_DO_EMOTE)
			cid = ((cmd_object_do_emote*)Msg.dwParam1)->id;
		else	//	Msg.dwParam2 == OBJECT_EMOTE_RESTORE
			cid = ((cmd_object_emote_restore*)Msg.dwParam1)->id;

		break;

	case MSG_PM_PLAYERUSEITEM:

		switch (Msg.dwParam2)
		{
		case OBJECT_START_USE:		cid = ((cmd_object_start_use*)Msg.dwParam1)->user;		break;
		case OBJECT_CANCEL_USE:		cid = ((cmd_object_cancel_use*)Msg.dwParam1)->user;		break;
		case OBJECT_USE_ITEM:		cid = ((cmd_object_use_item*)Msg.dwParam1)->user;		break;
		case OBJECT_START_USE_T:	cid = ((cmd_object_start_use_t*)Msg.dwParam1)->user;	break;
		}

		break;
		
	case MSG_PM_USEITEMWITHDATA:

		cid = ((object_use_item_with_arg*)Msg.dwParam1)->user;
		break;

	case MSG_PM_ENCHANTRESULT:

		cid = ((cmd_enchant_result*)Msg.dwParam1)->caster;
		break;

	case MSG_PM_PLAYERDOACTION:

		cid = ((cmd_obj_do_action*)Msg.dwParam1)->id;
		break;

	case MSG_PM_PLAYERSKILLRESULT:

		cid = ((cmd_object_skill_attack_result*)Msg.dwParam1)->attacker_id;
		break;

	case MSG_PM_PLAYERADVDATA:

		if (Msg.dwParam2 == PLAYER_SET_ADV_DATA)
			cid = ((cmd_player_set_adv_data*)Msg.dwParam1)->id;
		else
		{
			ASSERT(Msg.dwParam2 == PLAYER_CLR_ADV_DATA);
			cid = ((cmd_player_clr_adv_data*)Msg.dwParam1)->id;
		}

		break;

	case MSG_PM_PLAYERINTEAM:
		
		cid = ((cmd_player_in_team*)Msg.dwParam1)->id;
		break;

	case MSG_PM_PLAYERGATHER:

		if (Msg.dwParam2 == PLAYER_GATHER_START)
			cid = ((cmd_player_gather_start*)Msg.dwParam1)->pid;
		else if (Msg.dwParam2 == PLAYER_GATHER_STOP)
			cid = ((cmd_player_gather_stop*)Msg.dwParam1)->pid;
		else
		{
			ASSERT(Msg.dwParam2 == MINE_GATHERED);
			cid = ((cmd_mine_gathered*)Msg.dwParam1)->player_id;
		}

		break;

	case MSG_PM_PLAYERCHGSHAPE:

		cid = ((cmd_player_chgshape*)Msg.dwParam1)->idPlayer;
		break;

	case MSG_PM_GOBLINOPT:
	{
		switch (Msg.dwParam2)
		{
		case ELF_REFINE_ACTIVATE:	cid = ((cmd_elf_refine_activate*)Msg.dwParam1)->pid;	break;
		case CAST_ELF_SKILL:		cid = ((cmd_cast_elf_skill*)Msg.dwParam1)->pid;			break;
		case ELF_CMD_RESULT:		cid = pHost->GetCharacterID();							break;			
		}
				
		break;
	}

	case MSG_PM_BOOTHOPT:
	{
		switch (Msg.dwParam2)
		{
		case SELF_OPEN_BOOTH:
		case BOOTH_TRADE_SUCCESS:
		case OPEN_BOOTH_TEST:
		case PLAYER_BOOTH_INFO:
		case BOOTH_SELL_ITEM:		cid = pHost->GetCharacterID();	break;
		case PLAYER_OPEN_BOOTH:		cid = ((cmd_player_open_booth*)Msg.dwParam1)->pid;	break;
		case PLAYER_CLOSE_BOOTH:	cid = ((cmd_player_close_booth*)Msg.dwParam1)->pid;	break;
		case BOOTH_NAME:			cid = ((cmd_booth_name*)Msg.dwParam1)->pid;	break;
		}

		break;
	}
	case MSG_PM_PLAYERTRAVEL:

		switch (Msg.dwParam2)
		{
		case PLAYER_START_TRAVEL:	cid = ((cmd_player_start_travel*)Msg.dwParam1)->pid; break;
		case HOST_START_TRAVEL:		cid = pHost->GetCharacterID(); break;
		case PLAYER_END_TRAVEL:		cid = ((cmd_player_end_travel*)Msg.dwParam1)->pid; break;
		}

		break;

	case MSG_PM_PLAYERPVP:

		switch (Msg.dwParam2)
		{
		case PLAYER_ENABLE_PVP:		cid = ((cmd_player_enable_pvp*)Msg.dwParam1)->who;	break;
		case PLAYER_DISABLE_PVP:	cid = ((cmd_player_disable_pvp*)Msg.dwParam1)->who;	break;
		case HOST_PVP_COOLDOWN:
		case HOST_ENABLE_FREEPVP:	cid = pHost->GetCharacterID();	break;
		case PVP_COMBAT:			cid = ((cmd_pvp_combat*)Msg.dwParam1)->idPlayer;	break;
		}

		break;

	case MSG_PM_FASHIONENABLE:

		cid = ((cmd_player_enable_fashion*)Msg.dwParam1)->idPlayer;
		break;

	case MSG_PM_PLAYEREFFECT:

		if (Msg.dwParam2 == PLAYER_ENABLE_EFFECT)
			cid = ((cmd_player_enable_effect*)Msg.dwParam1)->id;
		else
		{
			ASSERT(Msg.dwParam2 == PLAYER_DISABLE_EFFECT);
			cid = ((cmd_player_disable_effect*)Msg.dwParam1)->id;
		}

		break;

	case MSG_PM_CHANGEFACE:

		cid = ((cmd_player_chg_face*)Msg.dwParam1)->idPlayer;
		break;

	case MSG_PM_PLAYERBINDOPT:

		switch (Msg.dwParam2)
		{
		case PLAYER_BIND_REQUEST:
		case PLAYER_BIND_INVITE:
		case PLAYER_BIND_REQUEST_REPLY:
		case PLAYER_BIND_INVITE_REPLY:	cid = pHost->GetCharacterID();	break;
		case PLAYER_BIND_STOP:			cid = ((cmd_player_bind_stop*)Msg.dwParam1)->who;	break;
		}

		break;

	case MSG_PM_PLAYERMOUNT:

		cid = ((cmd_player_mounting*)Msg.dwParam1)->id;
		break;

	case MSG_PM_PLAYERDUELOPT:

		switch (Msg.dwParam2)
		{
		case DUEL_RECV_REQUEST:
		case DUEL_REJECT_REQUEST:
		case DUEL_PREPARE:
		case DUEL_CANCEL:
		case HOST_DUEL_START:	cid = pHost->GetCharacterID();	break;
		case PLAYER_DUEL_START:	cid = ((cmd_player_duel_start*)Msg.dwParam1)->player_id;	break;
		case DUEL_STOP:			cid = ((cmd_duel_stop*)Msg.dwParam1)->player_id;	break;
		}

		break;

	case MSG_PM_PLAYERLEVEL2:

		cid = ((cmd_task_deliver_level2*)Msg.dwParam1)->id_player;
		break;

	case MSG_PM_PLAYERINVISIBLE:
		
		cid = ((cmd_object_invisible*)Msg.dwParam1)->id;
		break;

	case MSG_PM_PLAYERKNOCKBACK:
		cid = ((cmd_player_knockback*)Msg.dwParam1)->id;
		break;

	case MSG_PM_PLAYEREQUIPDISABLED:
		cid = ((cmd_player_equip_disabled*)Msg.dwParam1)->id;
		break;

	case MSG_PM_CONGREGATE:
		
		switch (Msg.dwParam2)
		{
		case CONGREGATE_REQUEST:cid = pHost->GetCharacterID();	break;
		case REJECT_CONGREGATE:	cid = pHost->GetCharacterID();	break;
		case CONGREGATE_START:	cid = ((cmd_congregate_start*)Msg.dwParam1)->id;	break;
		case CANCEL_CONGREGATE:	cid = ((cmd_cancel_congregate*)Msg.dwParam1)->id;	break;
		}
		break;

	case MSG_PM_TELEPORT:
		cid = ((cmd_player_teleport *)Msg.dwParam1)->id;
		break;

	case MSG_PM_FORCE_CHANGED:
		cid = ((cmd_player_force_changed *)Msg.dwParam1)->id;
		break;
	case MSG_PM_MULTIOBJ_EFFECT:
		cid = ((cmd_multiobj_effect *)Msg.dwParam1)->id;
		break;

	case MSG_PM_COUNTRY_CHANGED:
		cid = ((cmd_player_country_changed *)Msg.dwParam1)->id;
		break;

	case MSG_PM_KINGCHANGED:
		cid = ((cmd_player_king_changed*)Msg.dwParam1)->id;
		break;
	case MSG_PM_TITLE:
		switch (Msg.dwParam2)
		{
		case QUERY_TITLE_RE:		cid = ((cmd_query_title_re*)Msg.dwParam1)->roleid;	break;
		case CHANGE_CURR_TITLE_RE:	cid = ((cmd_change_curr_title_re*)Msg.dwParam1)->roleid; break;
		}
		break;
	case MSG_PM_REINCARNATION:
		cid = ((cmd_player_reincarnation*)Msg.dwParam1)->id;
		break;
	case MSG_PM_REALMLEVEL:
		cid = ((cmd_realm_level*)Msg.dwParam1)->roleid;
		break;
	case MSG_PM_PLAYER_IN_OUT_BATTLE:
		cid = ((cmd_player_in_out_battle*)Msg.dwParam1)->player_id;
		break;
	case MSG_PM_FACTION_PVP_MASK_MODIFY:
		cid = ((cmd_faction_pvp_mask_modify*)Msg.dwParam1)->roleid;
		break;
	default:
		ASSERT(0);
		return false;
	}

	if (!cid)
	{
		ASSERT(cid);
		return false;
	}

	if (cid == pHost->GetCharacterID())
		pHost->ProcessMessage(Msg);
	else
	{
		CECElsePlayer* pPlayer = SeekOutElsePlayer(cid);
		if (pPlayer)
			pPlayer->ProcessMessage(Msg);
	}

	return true;
}

//	Tick host player separately
//	pViewport: viewport used by host player, can be NULL
bool CECPlayerMan::TickHostPlayer(DWORD dwDeltaTime, CECViewport* pViewport)
{
	CECHostPlayer* pHost = GetHostPlayer();
	if (!pHost)
		return true;

	pHost->Tick(dwDeltaTime);

	if (pViewport)
	{
		//	Update camera position and direction
		A3DCamera* pCamera = pViewport->GetA3DCamera();
		A3DCoordinate* pCameraCoord = pHost->GetCameraCoord();
		pCamera->SetPos(pCameraCoord->GetPos());
		pCamera->SetDirAndUp(pCameraCoord->GetDir(), pCameraCoord->GetUp());
	}

	return true;
}

//	Tick routine
bool CECPlayerMan::Tick(DWORD dwDeltaTime)
{
	DWORD dwRealTime = g_pGame->GetRealTickTime();

	//	Deliver loaded player models
	DeliverLoadedPlayerModels();

	//	Tick all players
	PlayerTable::iterator it = m_PlayerTab.begin();
	for (; it != m_PlayerTab.end(); ++it)
	{
		CECElsePlayer* pPlayer = *it.value();
		pPlayer->Tick(dwDeltaTime);
	}

	//	Update players in various ranges
	UpdatePlayerInRanges(dwDeltaTime);

	//	Tick Login Players here, if there are
	int nNumLoginPlayers = m_LoginPlayers.size();
	for(int i=0; i < nNumLoginPlayers; i++)
	{
		m_LoginPlayers[i]->Tick(dwDeltaTime);
	}

	//	Clear up player data cache
	if (m_CacheCnt.IncCounter(dwRealTime))
	{
		m_CacheCnt.Reset();
		ClearUpPlayerCache();
	}

	UpdateUnknownElsePlayers();

	//	Update player's tank leader state if host is in battle
	CECHostPlayer* pHost = GetHostPlayer();
	if (pHost && pHost->IsInBattle() && !pHost->IsInFortress() && m_TLTimeCnt.IncCounter(dwRealTime))
	{
		m_TLTimeCnt.Reset();

		if (pHost->GetBattleTankNum())
			pHost->UpdateTankLeader();

		it = m_PlayerTab.begin();
		for (; it != m_PlayerTab.end(); ++it)
		{
			CECElsePlayer* pPlayer = *it.value();
			if (pPlayer->GetBattleTankNum())
				pPlayer->UpdateTankLeader();
		}
	}

	// test code
	/*
	if (GetHostPlayer())
	{
		PlayerTable::iterator it = m_PlayerTab.begin();
		for (; it != m_PlayerTab.end(); ++it)
		{
			CECElsePlayer* pPlayer = *it.value();
			if ((pPlayer->GetPos() - GetHostPlayer()->GetPos()).Normalize() < 2.0f)
			{
				if (pPlayer->GetAttachMode() == CECPlayer::enumAttachNone)
				{
					CECPlayer* p1 = GetHostPlayer();
					CECPlayer* p2 = pPlayer;

					if (p1->GetGender() != p2->GetGender())
					{
						if (p1->GetGender() == GENDER_MALE)
							p1->AttachBuddy(p2->GetPlayerInfo().cid);
						else
							p2->AttachBuddy(p1->GetPlayerInfo().cid);
						break;
					}
				}
			}
		}
	}
	*/

	return true; 
}

//	Render routine
bool CECPlayerMan::Render(CECViewport* pViewport)
{
	bool bUseHintModel = g_pGame->GetConfigs()->GetVideoSettings().bModelLimit;

	//	Render all else players
	CECHostPlayer* pHost = GetHostPlayer();
	CECElsePlayer* pHHElsePlayer = NULL;	//	The player will be high-light rendered
	int idHoverObject = pHost ? pHost->GetCursorHoverObject() : 0;
	bool bRenderElse = pHost ? !pHost->IsChangingFace() : true;

	if (pHost && pHost->IsInForceNavigateState())
		bRenderElse = false;

	if (bRenderElse)
	{
		PlayerTable::iterator it = m_PlayerTab.begin();
		for (; it != m_PlayerTab.end(); ++it)
		{
			CECElsePlayer* pPlayer = *it.value();
			if (pPlayer->IsHangerOn() && pPlayer->GetFaceModel())
				pPlayer->GetFaceModel()->Tick(0);

			if (idHoverObject == pPlayer->GetCharacterID()
			 && pPlayer->GetAttachMode() == CECPlayer::enumAttachNone
			 && !pPlayer->IsRidingOnPet())
				pHHElsePlayer = pPlayer;
			else
			{
				if (pPlayer->GetBoothState() == 2)
					pPlayer->RenderForBooth(pViewport);
				else if (bUseHintModel && (pPlayer->GetUseHintModelFlag() || !pPlayer->IsAllResReady()))
					pPlayer->RenderForHint(pViewport);
				else
					pPlayer->Render(pViewport);
			}
		}

		if (pHHElsePlayer)
		{
			if (pHHElsePlayer->GetBoothState() == 2)
				pHHElsePlayer->RenderForBooth(pViewport, 0, true);
			else if (bUseHintModel && (pHHElsePlayer->GetUseHintModelFlag() || !pHHElsePlayer->IsAllResReady()))
				pHHElsePlayer->RenderForHint(pViewport, 0, true);
			else
				pHHElsePlayer->RenderHighLight(pViewport);
		}
	}

	//	Render host player
	if (pHost)
	{
		if (pHost->IsHangerOn())
			pHost->GetFaceModel()->Tick(0);

		if (pHost->GetBoothState() != 2)
			pHost->Render(pViewport);
		else
			pHost->RenderForBooth(pViewport);
	}

	//	Render Login Players here, if there are
	int nNumLoginPlayers = m_LoginPlayers.size();
	for(int i=0; i<nNumLoginPlayers; i++)
	{
		m_LoginPlayers[i]->Render(pViewport);
	}

	if (g_pGame->GetGameRun()->GetGameState() == CECGameRun::GS_GAME)
	{
		//	Flush title font
		A3DFont* pFont = g_pGame->GetFont(RES_FONT_TITLE);
		pFont->Flush();

		//	Flush emotion decals
		CECGameUIMan* pGameUI = g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan();
		A2DSprite* pFaceImg = NULL;
		pGameUI->GetEmotionList(&pFaceImg, NULL);
		if (pFaceImg)
			pFaceImg->GetTextures()->GetSpriteBuffer()->FlushBuffer();
	}

	return true;
}

//	RenderForReflected routine
bool CECPlayerMan::RenderForReflect(CECViewport * pViewport)
{
	//	All player should be reflect visible
	//	Render all players
	PlayerTable::iterator it = m_PlayerTab.begin();
	for (; it != m_PlayerTab.end(); ++it)
	{
		CECElsePlayer* pPlayer = *it.value();
		if (pPlayer->GetBoothState() != 2)
			pPlayer->Render(pViewport, CECObject::RD_REFLECT);
		else
			pPlayer->RenderForBooth(pViewport, CECObject::RD_REFLECT);
	}

	//	Render host player
	CECHostPlayer* pHost = GetHostPlayer();
	if (pHost)
	{
		if (pHost->GetBoothState() != 2)
			pHost->Render(pViewport, CECObject::RD_REFLECT);
		else
			pHost->RenderForBooth(pViewport, CECObject::RD_REFLECT);
	}

	//	Render Login Players here, if there are
	int nNumLoginPlayers = m_LoginPlayers.size();
	for(int i=0; i<nNumLoginPlayers; i++)
	{
		m_LoginPlayers[i]->Render(pViewport, CECObject::RD_REFLECT);
	}

	return true;
}

//	RenderForRefract routine
bool CECPlayerMan::RenderForRefract(CECViewport * pViewport)
{
	//	All player should be reflect visible
	//	Render all players
	PlayerTable::iterator it = m_PlayerTab.begin();
	for (; it != m_PlayerTab.end(); ++it)
	{
		CECElsePlayer* pPlayer = *it.value();
		pPlayer->Render(pViewport, CECObject::RD_REFRACT);
	}

	//	Render host player
	CECHostPlayer* pHost = GetHostPlayer();
	if (pHost)
		pHost->Render(pViewport, CECObject::RD_REFRACT);

	//	Render Login Players here, if there are
	int nNumLoginPlayers = m_LoginPlayers.size();
	for(int i=0; i<nNumLoginPlayers; i++)
	{
		m_LoginPlayers[i]->Render(pViewport, CECObject::RD_REFRACT);
	}

	return true;
}

//	Host information arrived
bool CECPlayerMan::HostPlayerInfo1(const S2C::cmd_self_info_1& Info)
{
	//	Set world load center
	ASSERT(m_pGameRun);
	const A3DVECTOR3& vInitLoadPos = m_pGameRun->GetWorld()->GetInitLoadPos();
	if (vInitLoadPos.x != Info.pos.x || vInitLoadPos.z != Info.pos.z)
		m_pGameRun->GetWorld()->SetLoadCenter(Info.pos);

	CECHostPlayer* pHost = GetHostPlayer();

	if (!pHost->Init(Info))
	{
		a_LogOutput(1, "CECPlayerMan::HostPlayerInfo1, Failed to initialize host player");
		return false;
	}

	int nFaction = pHost->GetFactionID();
	if (nFaction)
	{
		g_pGame->GetGameSession()->GetFactionInfo(1, &nFaction);
		// g_pGame->GetGameSession()->faction_player_info();
	}

	//	Update the camera which is associated with host player
	//	TODO: Directly get viewport here isn't a good way. Maybe we should
	//	find a way to know which camera is associated with host player.
	TickHostPlayer(0, g_pGame->GetViewport());

	return true;
}

//	One else player enter view area
CECElsePlayer* CECPlayerMan::ElsePlayerEnter(const S2C::info_player_1& Info, int iCmd)
{
	//	If this player's id is in unknown table, remove it because this player
	//	won't be unknown anymore
	UkPlayerTable::pair_type Pair = m_UkPlayerTab.get(Info.cid);
	if (Pair.second)
		m_UkPlayerTab.erase(Info.cid);

	int iAppearFlag = (iCmd == S2C::PLAYER_ENTER_WORLD) ? CECElsePlayer::APPEAR_ENTERWORLD : CECElsePlayer::APPEAR_RUNINTOVIEW;

	//	Has player been in active player table ?
	CECElsePlayer* pPlayer = GetElsePlayer(Info.cid);
	if (pPlayer)
	{
		//	This player has existed in player table, call special initial function
		pPlayer->Init(Info, iAppearFlag, true);
		return pPlayer;
	}

	//	Create a new player
	if (!(pPlayer = CreateElsePlayer(Info, iAppearFlag)))
	{
		a_LogOutput(1, "CECPlayerMan::ElsePlayerEnter, Failed to create player (%d)", Info.cid);
		return NULL;
	}

	//	Add player to table
	ACSWrapper cspt(&m_csPlayerTab);
	m_PlayerTab.put(Info.cid, pPlayer);
	g_pGame->GetGameRun()->GetMemSimplify()->OnPlayerEnter(pPlayer);

	return pPlayer;
}

//	One else player leave view area or exit game
void CECPlayerMan::ElsePlayerLeave(int cid, bool bExit, bool bUpdateMMArray/* true */)
{
	//	Player has been in active player table ?
	CECElsePlayer* pPlayer = GetElsePlayer(cid);
	CECHostPlayer* pHost = GetHostPlayer();

	//	Remove player from m_aMMPlayers array
	if (pPlayer)
	{
		if (bUpdateMMArray)
			RemovePlayerFromMiniMap(pPlayer);

		pPlayer->m_iMMIndex = -1;

		//	If this player is selected by host, cancel the selection
		if (pPlayer->GetCharacterID() == pHost->GetSelectedTarget())
			pHost->SelectTarget(0);
		
		pHost->RemoveObjectFromTabSels(pPlayer);
	}

	// 删除正在进行中的PlayerBaseInfo请求
	CECC2SCmdCache* pCache = g_pGame->GetGameSession()->GetC2SCmdCache();
	pCache->RemovePlayerBaseInfo(cid);
	if (bExit)	//	Player exit game
	{
		if (!pPlayer)
		{
			//	Release player data in cache
			CECEPCacheData* pData = PopPlayerFromCache(cid);
			if (pData)
				delete pData;
		}
		else
		{
			//	Remove from active player table
			ACSWrapper cspt(&m_csPlayerTab);
			m_PlayerTab.erase(cid);

			QueuePlayerUndoLoad(cid);
		}
	}
	else	//	Player leave view area
	{
		if (!pPlayer)
			return;		//	Counldn't find this player in view area

		//	If this player is a member of host player's team, remain it
		//	in player list !
		//	Note: there are no players in ghost mode anymore !
	/*	if (pHost && pHost->IsTeamMember(cid))
		{
			pPlayer->TurnToGhost();
			return;
		}	*/

		//	This player shouldn't be in cache
		ASSERT(!IsPlayerInCache(cid));

		//	Push player data into cache
		CECEPCacheData* pCacheData = pPlayer->CreateCacheData();
		if (pCacheData)
			PushPlayerToCache(pCacheData);

		//	Remove from active player table
		{
			ACSWrapper cspt(&m_csPlayerTab);
			m_PlayerTab.erase(cid);
		}

		QueuePlayerUndoLoad(cid);
	}

	//	Release player object
	ReleaseElsePlayer(pPlayer);
}

//	One else player's more information arrived
bool CECPlayerMan::MoreElsePlayerInfo2(int cid, const S2C::info_player_2& Info)
{
	return true;
}

//	One else player's more information arrived
bool CECPlayerMan::MoreElsePlayerInfo3(int cid, const S2C::info_player_3& Info)
{
	return true;
}

//	One else player's more information arrived
bool CECPlayerMan::MoreElsePlayerInfo4(int cid, const S2C::info_player_4& Info)
{
	return true;
}

//	Create else player
CECElsePlayer* CECPlayerMan::CreateElsePlayer(const S2C::info_player_1& Info, int iAppearFlag)
{
	CECElsePlayer* pPlayer = new CECElsePlayer(this);
	if (!pPlayer)
	{
		glb_ErrorOutput(ECERR_NOTENOUGHMEMORY, "CECPlayerMan::CreateElsePlayer", __LINE__);
		return false;
	}

	pPlayer->SetBornStamp(g_pGame->GetGameRun()->GetWorld()->GetBornStamp());

	//	Try to load player data from cache at first
	CECEPCacheData* pCacheData = PopPlayerFromCache(Info.cid);
	if (pCacheData)
	{
		if (!pPlayer->InitFromCache(Info, pCacheData, iAppearFlag))
		{
			delete pCacheData;
			a_LogOutput(1, "CECPlayerMan::CreateElsePlayer, Failed to init player from cache data.");
			return NULL;
		}

		delete pCacheData;
	}
	else
	{
		if (!pPlayer->Init(Info, iAppearFlag, false))
		{
			glb_ErrorOutput(ECERR_FAILEDTOCALL, "CECPlayerMan::CreateElsePlayer", __LINE__);
			return false;
		}
	}

	return pPlayer;
}

//	Release else player
void CECPlayerMan::ReleaseElsePlayer(CECElsePlayer* pPlayer)
{
	if (pPlayer)
	{
		//	Remove tab-selected array
		CECHostPlayer* pHost = GetHostPlayer();
		if (pHost)
		pHost->RemoveObjectFromTabSels(pPlayer);
		g_pGame->GetGameRun()->GetMemSimplify()->OnPlayerLeave(pPlayer);

		pPlayer->Release();
		delete pPlayer;
		pPlayer = NULL;
	}
}

//	Get else player cache data
CECEPCacheData* CECPlayerMan::GetPlayerCacheData(int cid)
{
	CacheTable::pair_type Pair = m_PlayerCache.get(cid);
	return Pair.second ? *Pair.first : NULL;
}

//	Pop player data from cache
CECEPCacheData* CECPlayerMan::PopPlayerFromCache(int cid)
{
	CacheTable::pair_type Pair = m_PlayerCache.get(cid);
	if (!Pair.second)
		return NULL;

	CECEPCacheData* pData = *Pair.first;
	ASSERT(pData);

	//	Remove player from cache
	m_PlayerCache.erase(cid);

	return pData;
}

//	Push player to cache
bool CECPlayerMan::PushPlayerToCache(CECEPCacheData* pData)
{
	CacheTable::pair_type Pair = m_PlayerCache.get(pData->m_cid);
	if (Pair.second)
	{
		//	This case shouldn't happen, but we still handle it.
		CECEPCacheData* pTemp = *Pair.first;
		delete pTemp;
		m_PlayerCache.erase(pData->m_cid);
	}

	pData->m_dwCacheTime = (DWORD)(a_GetTime() * 0.001f);
	m_PlayerCache.put(pData->m_cid, pData);
	return true;
}

//	Clear up player data cache
void CECPlayerMan::ClearUpPlayerCache()
{
	if (m_PlayerCache.size() < MAX_CACHEDPLAYER)
		return;

	DWORD dwCurTime = a_GetTime() / 1000;

	//	Release some old enough cached data
	int aIDs[50], iCount=0;
	
	CacheTable::iterator it = m_PlayerCache.begin();
	for (; it != m_PlayerCache.end(); ++it)
	{
		CECEPCacheData* pData = *it.value();
		if (pData->m_dwCacheTime + 240 < dwCurTime)
		{
			aIDs[iCount++] = pData->m_cid;			
			delete pData;
			if (iCount >= 50)
				break;
		}
	}

	for (int i=0; i < iCount; i++)
		m_PlayerCache.erase(aIDs[i]);
}

//	Update players in various ranges (Active, visible, mini-map etc.)
void CECPlayerMan::UpdatePlayerInRanges(DWORD dwDeltaTime)
{
	CECHostPlayer* pHost = GetHostPlayer();
	if (!pHost || !pHost->IsAllResReady())
		return;

	CECConfigs* pConfigs = g_pGame->GetConfigs();
	int idHoverObject = pHost->GetCursorHoverObject();
	int idSelected = pHost->GetSelectedTarget();

	//	Get current player visible radius
	float fPVRadius = pConfigs->GetPlayerVisRadius();
	if (!pConfigs->GetVideoSettings().bModelLimit)
		fPVRadius = pConfigs->GetSevActiveRadius();

	m_aMMPlayers.RemoveAll(false);

	//	Check all else players
	PlayerTable::iterator it = m_PlayerTab.begin();
	while(it != m_PlayerTab.end())
	{
		CECElsePlayer* pPlayer = *it.value();
		float fDistToHostH = pPlayer->GetDistToHostH();
		float fDistToHost = pPlayer->GetDistToHost();
		float fDistToCamera = pPlayer->GetDistToCamera();

		//	Check player's visiblity
		if( pConfigs->GetVideoSettings().bModelLimit )
			pPlayer->SetUseHintModelFlag(fDistToHost <= fPVRadius ? false : true);
		else
			pPlayer->SetUseHintModelFlag(false);

		//	Check whether name is visible
		if (fDistToHost <= 20.0f || pPlayer->GetCharacterID() == idHoverObject ||
			pPlayer->GetCharacterID() == idSelected)
			pPlayer->SetRenderNameFlag(true);
		else
			pPlayer->SetRenderNameFlag(false);

		//	Set bars visible flags
		pPlayer->SetRenderBarFlag(pPlayer->GetCharacterID() == idSelected);

		//	Check whether player is in mini-map
		if (fDistToHostH > pConfigs->GetSevActiveRadius() && !pHost->IsDeferedUpdateSlice())
		{		 
			// backup next item before delete it.
			++it;
			
			ElsePlayerLeave(pPlayer->GetCharacterID(), false, false);
			continue;
		}
		else if (!pPlayer->InGhostState() && fDistToHostH <= pConfigs->GetSevActiveRadius())
			pPlayer->m_iMMIndex = m_aMMPlayers.Add(pPlayer);

		 ++it;
	}
}

//	Remove else player from m_aMMPlayers array
void CECPlayerMan::RemovePlayerFromMiniMap(CECElsePlayer* pPlayer)
{
	ASSERT(pPlayer);
	int iIndex = pPlayer->m_iMMIndex;
	if (iIndex < 0 || iIndex >= m_aMMPlayers.GetSize())
		return;

	CECElsePlayer* pTempPlayer = m_aMMPlayers[iIndex];
	if (pTempPlayer != pPlayer)
	{
		ASSERT(pTempPlayer == pPlayer);
		return;
	}

	//	Exchange with the last layer in array
	int iLastIdx = m_aMMPlayers.GetSize() - 1;
	if (m_aMMPlayers.GetSize() > 1 && iIndex != iLastIdx)
	{
		pTempPlayer = m_aMMPlayers[iLastIdx];
		m_aMMPlayers[iIndex] = pTempPlayer;
		pTempPlayer->m_iMMIndex = iIndex;
		m_aMMPlayers.RemoveAt(iLastIdx);
	}
	else
		m_aMMPlayers.RemoveAtQuickly(iIndex);
}

//	When world's loading center changed
void CECPlayerMan::OnLoadCenterChanged()
{
	m_aMMPlayers.RemoveAll(false);
}

//	Ray trace
bool CECPlayerMan::RayTrace(ECRAYTRACE* pTraceInfo)
{
	//	If shift is pressed, ignore all players
	if (pTraceInfo->dwKeyStates & EC_KSF_SHIFT)
		return false;

	A3DVECTOR3 vHitPos, vNormal;
	float fFracion;
	bool bRet = false;

/*	Trace host player heself

	if (m_pHostPlayer &&
		(pTraceInfo->iTraceObj == TRACEOBJ_LBTNCLICK || 
		pTraceInfo->iTraceObj == TRACEOBJ_TESTRAY))
	{
		//	Trace host player
		const A3DAABB* paabb = &m_pHostPlayer->GetPlayerAABB();
		if (CLS_RayToAABB3(pTraceInfo->vStart, pTraceInfo->vDelta, paabb->Mins, paabb->Maxs,
					vHitPos, &fFracion, vNormal))
		{
			if (fFracion < pTraceInfo->pTraceRt->fFraction)
			{
				pTraceInfo->pTraceRt->fFraction		= fFracion;
				pTraceInfo->pTraceRt->vHitPos		= vHitPos;
				pTraceInfo->pTraceRt->vPoint		= vHitPos;
				pTraceInfo->pTraceRt->vNormal		= vNormal;

				pTraceInfo->pECTraceRt->fFraction	= fFracion;
				pTraceInfo->pECTraceRt->iEntity		= ECENT_PLAYER;
				pTraceInfo->pECTraceRt->iObjectID	= m_pHostPlayer->GetCharacterID();
				pTraceInfo->pECTraceRt->vNormal		= vNormal;

				bRet = true;
			}
		}
	}
*/
	
	//	Trace all else player
	PlayerTable::iterator it = m_PlayerTab.begin();
	for (; it != m_PlayerTab.end(); ++it)
	{
		CECElsePlayer* pPlayer = *it.value();
		if (pPlayer->InGhostState())
			continue;

		//	TRACEOBJ_LBTNCLICK is caused by player pressed left button
	//	if (pTraceInfo->iTraceObj == TRACEOBJ_LBTNCLICK && pPlayer->IsDead())
	//		continue;

		const A3DAABB* paabb;
		if (GetHostPlayer() && GetHostPlayer()->GetBattleInfo().IsChariotWar())
		{
			paabb = &pPlayer->GetPlayerPickAABB();
		}
		else
		{
			if ((pTraceInfo->iTraceObj == TRACEOBJ_LBTNCLICK ||
				pTraceInfo->iTraceObj == TRACEOBJ_RBTNCLICK) &&
				!pPlayer->IsDead())		//	When player died, use GetPlayerPickAABB()
				paabb = &pPlayer->GetPlayerAABB();
			else
				paabb = &pPlayer->GetPlayerPickAABB();
		}

		
		if (CLS_RayToAABB3(pTraceInfo->vStart, pTraceInfo->vDelta, paabb->Mins, paabb->Maxs,
					vHitPos, &fFracion, vNormal))
		{
			if (fFracion < pTraceInfo->pTraceRt->fFraction)
			{
				pTraceInfo->pTraceRt->fFraction		= fFracion;
				pTraceInfo->pTraceRt->vHitPos		= vHitPos;
				pTraceInfo->pTraceRt->vPoint		= vHitPos;
				pTraceInfo->pTraceRt->vNormal		= vNormal;

				pTraceInfo->pECTraceRt->fFraction	= fFracion;
				pTraceInfo->pECTraceRt->iEntity		= ECENT_PLAYER;
				pTraceInfo->pECTraceRt->iObjectID	= pPlayer->GetCharacterID();
				pTraceInfo->pECTraceRt->vNormal		= vNormal;

				bRet = true;
			}
		}
	}

	return bRet;
}

//	Update unknown else player table
void CECPlayerMan::UpdateUnknownElsePlayers()
{
	DWORD dwRealTime = g_pGame->GetRealTickTime();
	if (!m_QueryCnt.IncCounter(dwRealTime))
		return;

	m_QueryCnt.Reset();

	//	Query unknown player's information
	if (m_UkPlayerTab.size())
	{
		AArray<int, int> aIDs;

		UkPlayerTable::iterator it = m_UkPlayerTab.begin();
		for (; it != m_UkPlayerTab.end(); ++it)
			aIDs.Add(*it.value());

		g_pGame->GetGameSession()->c2s_CmdQueryPlayerInfo1(aIDs.GetSize(), aIDs.GetData());

		m_UkPlayerTab.clear();
	}

	//	Query booth name
	if (m_aBoothQuery.GetSize())
	{
		g_pGame->GetGameSession()->c2s_CmdQueryBoothName(m_aBoothQuery.GetSize(), m_aBoothQuery.GetData());
		m_aBoothQuery.RemoveAll(false);
	}
}

//	Load player model in loading thread
bool CECPlayerMan::ThreadLoadPlayerModel(
	int cid,
	DWORD dwBornStamp,
	int iProfession,
	int iGender,
	int iCustom,
	const int* pEquips,
	const char* szPetPath,
	bool bClient)
{
	PLAYERMODEL* pInfo = new PLAYERMODEL;
	memset(pInfo, 0, sizeof(*pInfo));
	pInfo->cid = cid;
	pInfo->dwBornStamp = dwBornStamp;
	pInfo->bClientModel = bClient;    // 纯客户端模型

	bool bSimpleFace = false, bSimpleModel = false;

	CECPlayer* pPlayer = g_pGame->GetGameRun()->GetWorld()->GetPlayerMan()->GetPlayer(cid);
	if( g_pGame->GetGameRun()->GetGameState() == CECGameRun::GS_GAME && pPlayer )
	{
		bSimpleFace = !pPlayer->ShouldUseFaceModel();
		bSimpleModel = !pPlayer->ShouldUseClothedModel();
	}

	if (!CECPlayer::LoadPlayerModel(iProfession, iGender, iCustom, pEquips, szPetPath, pInfo->Ret, bSimpleFace, bSimpleModel))
	{
		delete pInfo;
		return false;
	}

	ACSWrapper csa(&m_csLoad);
	m_aLoadedModels.Add(pInfo);

	return true;
}

bool CECPlayerMan::ThreadLoadPlayerEquips(int cid, DWORD dwBornStamp, int iProfession, int iGender, const int* pEquips, __int64 EquipMask)
{
	PLAYERMODEL* pInfo = new PLAYERMODEL;
	memset(pInfo, 0, sizeof(*pInfo));
	pInfo->cid = cid;
	pInfo->dwBornStamp = dwBornStamp;
	pInfo->Ret.dwValidMask |= PLAYERLOADRESULT_EQUIPMODEL;

	bool bSimpleModel = false;
	CECPlayer* pPlayer = g_pGame->GetGameRun()->GetWorld()->GetPlayerMan()->GetPlayer(cid);
	if( pPlayer && !pPlayer->ShouldUseClothedModel() )
		bSimpleModel = true;

	if (!CECPlayer::LoadPlayerEquips(iProfession, iGender, EquipMask, pEquips, pInfo->Ret.EquipResult, bSimpleModel))
	{
		delete pInfo;
		return false;
	}

	ACSWrapper csa(&m_csLoad);
	m_aLoadedModels.Add(pInfo);	

	return true;
}

bool CECPlayerMan::ThreadLoadPetModel(int cid, DWORD dwBornStamp, const char* szPetPath)
{
	PLAYERMODEL* pInfo = new PLAYERMODEL;
	memset(pInfo, 0, sizeof(*pInfo));
	pInfo->cid = cid;
	pInfo->dwBornStamp = dwBornStamp;
	pInfo->Ret.dwValidMask |= PLAYERLOADRESULT_PETMODEL;

	if (!CECPlayer::LoadPetModel(szPetPath, &pInfo->Ret.pPetModel))
	{
		delete pInfo;
		return false;
	}

	ACSWrapper csa(&m_csLoad);
	m_aLoadedModels.Add(pInfo);

	return true;
}

bool CECPlayerMan::ThreadLoadDummyModel(int cid, DWORD dwBornStamp, int iShape,bool bClient)
{
	PLAYERMODEL* pInfo = new PLAYERMODEL;
	memset(pInfo, 0, sizeof(*pInfo));
	pInfo->cid = cid;
	pInfo->dwBornStamp = dwBornStamp;
	pInfo->bClientModel = bClient;
	pInfo->Ret.dwValidMask |= PLAYERLOADRESULT_DUMMYMODEL;
	pInfo->Ret.iShape = iShape;
	
	if (!CECPlayer::LoadDummyModel(iShape, &pInfo->Ret.pDummyModel))
	{
		delete pInfo;
		return false;
	}
	
	ACSWrapper csa(&m_csLoad);
	m_aLoadedModels.Add(pInfo);
	
	return true;
}

bool CECPlayerMan::ThreadLoadPlayerFace(int cid, DWORD dwBornStamp, int iProfession, int iGender, int iCustom)
{
	PLAYERMODEL* pInfo = new PLAYERMODEL;
	memset(pInfo, 0, sizeof(*pInfo));
	pInfo->cid = cid;
	pInfo->dwBornStamp = dwBornStamp;
	pInfo->Ret.dwValidMask |= PLAYERLOADRESULT_PLAYERFACE;

	if (!(pInfo->Ret.pFaceModel = CECPlayer::ThreadLoadFaceModel(iProfession, iGender, iCustom)))
	{
		delete pInfo;
		return false;
	}

	ACSWrapper csa(&m_csLoad);
	m_aLoadedModels.Add(pInfo);

	return true;
}

//	Deliver loaded player models
void CECPlayerMan::DeliverLoadedPlayerModels()
{
	ACSWrapper csa(&m_csLoad);

	for (int i=0; i < m_aLoadedModels.GetSize(); i++)
	{
		PLAYERMODEL* pInfo = m_aLoadedModels[i];

		CECPlayer* pPlayer = NULL;
		if (pInfo->bClientModel && GetHostPlayer())		
			pPlayer = GetHostPlayer()->GetNavigatePlayer();			
		else		
			pPlayer = GetPlayer(pInfo->cid, pInfo->dwBornStamp);		

		if (!pPlayer || !pPlayer->SetPlayerLoadedResult(pInfo->Ret))
		{
			CECPlayer::ReleasePlayerModel(pInfo->Ret);
		}

		delete pInfo;
	}

	m_aLoadedModels.RemoveAll(false);
}


//	Get player candidates whom can be auto-selected by 'TAB' key
void CECPlayerMan::TabSelectCandidates(int idCurSel, APtrArray<CECElsePlayer*>& aCands)
{
	CECHostPlayer* pHost = GetHostPlayer();
	if (!pHost->IsSkeletonReady()){
		//	只有 IsSkeletonReady() 为 true 时 GetDistToHost() 才有效
		return;
	}

	//	Trace all Else Players
	PlayerTable::iterator it = m_PlayerTab.begin();
	for (; it != m_PlayerTab.end(); ++it)
	{
		CECElsePlayer* pPlayer = *it.value();
		if (!pPlayer->IsSelectable() ||
			pPlayer->IsDead() ||
			pPlayer->GetCharacterID() == idCurSel || 
			pHost->AttackableJudge(pPlayer->GetCharacterID(), false) != 1)
			continue;

		float fDist = pPlayer->GetDistToHost();
		if (fDist > EC_TABSEL_DIST || !CECHostPlayer::CanSafelySelectWith(fDist))
			continue;	//	Target is too far

		aCands.Add(pPlayer);
	}
}

void CECPlayerMan::OptimizeShowExtendStates()
{
	//	根据当前优化设置，更新 NPC 上的 Extend States 光效显示
	PlayerTable::iterator it = m_PlayerTab.begin();
	for (; it != m_PlayerTab.end(); ++it)
	{
		CECElsePlayer* pPlayer = *it.value();
		pPlayer->OptimizeShowExtendStates();
	}
	CECHostPlayer *pHost = GetHostPlayer();
	pHost->OptimizeShowExtendStates();
}

void CECPlayerMan::OptimizeWeaponStoneGfx()
{
	//	根据当前优化设置，更新 NPC 上的 Extend States 光效显示
	PlayerTable::iterator it = m_PlayerTab.begin();
	for (; it != m_PlayerTab.end(); ++it)
	{
		CECElsePlayer* pPlayer = *it.value();
		pPlayer->OptimizeWeaponStoneGfx();
	}
	CECHostPlayer *pHost = GetHostPlayer();
	pHost->OptimizeWeaponStoneGfx();
}


void CECPlayerMan::OptimizeArmorStoneGfx()
{
	//	根据当前优化设置，更新 NPC 上的 Extend States 光效显示
	PlayerTable::iterator it = m_PlayerTab.begin();
	for (; it != m_PlayerTab.end(); ++it)
	{
		CECElsePlayer* pPlayer = *it.value();
		pPlayer->OptimizeArmorStoneGfx();
	}
	CECHostPlayer *pHost = GetHostPlayer();
	pHost->OptimizeArmorStoneGfx();
}


void CECPlayerMan::OptimizeSuiteGfx()
{
	//	根据当前优化设置，更新 NPC 上的 Extend States 光效显示
	PlayerTable::iterator it = m_PlayerTab.begin();
	for (; it != m_PlayerTab.end(); ++it)
	{
		CECElsePlayer* pPlayer = *it.value();
		pPlayer->OptimizeSuiteGfx();
	}
	CECHostPlayer *pHost = GetHostPlayer();
	pHost->OptimizeSuiteGfx();
}

CECElsePlayer* CECPlayerMan::GetMouseOnPateTextPlayer( int x, int y )
{
	CECElsePlayer* result = NULL;
	PlayerTable::iterator it = m_PlayerTab.begin();
	for (; it != m_PlayerTab.end(); ++it)
	{
		CECElsePlayer* pPlayer = *it.value();
		if (pPlayer->IsClickBoothBar(x, y)){
			if(result == NULL)
				result = pPlayer;
			if (result->GetDistToCamera() > pPlayer->GetDistToCamera()){
				result = pPlayer;
			}
		}
	}
	return result;
}

void CECPlayerMan::OnModelChange(const CECFactionMan *p, const CECObservableChange *q){
	const CECFactionManChange *pChange = dynamic_cast<const CECFactionManChange *>(q);
	if (!pChange){
		ASSERT(false);
	}else if (pChange->ChangeMask() != CECFactionManChange::FACTION_RENAMEFACTION){
		return;
	}
	//	HostPlayer
	const CECFactionManFactionChange *pFactionChange = dynamic_cast<const CECFactionManFactionChange *>(pChange);
	if (CECHostPlayer *pHost = GetHostPlayer()){
		if (pHost->GetFactionID() == pFactionChange->FactionID()){
			pHost->OnFactionNameChange();
		}
	}
	//	ElsePlayer
	for (PlayerTable::iterator it = m_PlayerTab.begin(); it != m_PlayerTab.end(); ++it){
		CECElsePlayer* pPlayer = *it.value();
		if (pPlayer->GetFactionID() != pFactionChange->FactionID()){
			continue;
		}
		pPlayer->OnFactionNameChange();
	}
}