/*
 * FILE: EC_ManNPC.cpp
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
#include "EC_ManNPC.h"
#include "EC_GameSession.h"
#include "EC_Game.h"
#include "EC_NPC.h"
#include "EC_Monster.h"
#include "EC_Pet.h"
#include "EC_NPCServer.h"
#include "EC_CDS.h"
#include "EC_ManPlayer.h"
#include "EC_GameRun.h"
#include "EC_HostPlayer.h"
#include "EC_World.h"
#include "EC_Utility.h"
#include "EC_Configs.h"
#include "EC_Model.h"
#include "EC_SceneLoader.h"
#include "EC_UIManager.h"
#include "EC_InputCtrl.h"
#include "ECScriptMan.h"
#include "ECScriptContext.h"
#include "ECSCriptCheckState.h"
#include "ElementSkill.h"
#include "EC_AutoPolicy.h"
#include "EC_PlayerWrapper.h"
#include "EC_Faction.h"

#include "A3DCollision.h"
#include "A3DMacros.h"
#include "A3DSkin.h"
#include "elementdataman.h"
#include "ACSWrapper.h"

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
//	Implement CECNPCMan
//	
///////////////////////////////////////////////////////////////////////////

CECNPCMan::CECNPCMan(CECGameRun* pGameRun) :
CECManager(pGameRun),
m_NPCTab(512),
m_UkNPCTab(32)
{
	m_iManagerID = MAN_NPC;

	m_QueryCnt.SetPeriod(10000);
	
	InitializeCriticalSection(&m_csLoad);
}

CECNPCMan::~CECNPCMan()
{
	DeleteCriticalSection(&m_csLoad);
}

//	Release manager
void CECNPCMan::Release()
{
	OnLeaveGameWorld();
}

//	Tick routine
bool CECNPCMan::Tick(DWORD dwDeltaTime)
{
	//	Deliver loaded NPC models
	DeliverLoadedNPCModels();

	enum
	{
		SIZE_REMOVETAB = 64
	};

	//	Every tick maximum 64 NPCs can be removed
	CECNPC* aRemove[SIZE_REMOVETAB];
	int i, iRemoveCnt=0;

	//	Tick all NPCs
	NPCTable::iterator it = m_NPCTab.begin();
	for (; it != m_NPCTab.end(); ++it)
	{
		CECNPC* pNPC = *it.value();

		if (pNPC->ShouldDisappear())
		{
			if (iRemoveCnt < SIZE_REMOVETAB)
				aRemove[iRemoveCnt++] = pNPC;
		}
		else
		{
			pNPC->Tick(dwDeltaTime);
		}
	}

	for (i=0; i < iRemoveCnt; i++)
		NPCLeave(aRemove[i]->GetNPCID());

	//	Tick all NPCs who are in disappear table
	iRemoveCnt = 0;

	for (i=0; i < m_aDisappearNPCs.GetSize(); i++)
	{
		CECNPC* pNPC = m_aDisappearNPCs[i];
		if (pNPC->ShouldDisappear())
		{
			if (iRemoveCnt < SIZE_REMOVETAB)
			{
				aRemove[iRemoveCnt++] = pNPC;
				m_aDisappearNPCs.RemoveAtQuickly(i);
				i--;
			}
		}
		else
			pNPC->Tick(dwDeltaTime);
	}

	for (i=0; i < iRemoveCnt; i++)
		ReleaseNPC(aRemove[i]);

	//	Update NPCs in various ranges (Active, visible, mini-map etc.)
	UpdateNPCInRanges(dwDeltaTime);

	//	Udpate unknown NPC table
	UpdateUnknownNPCs();

	return true; 
}

//	Render routine
bool CECNPCMan::Render(CECViewport* pViewport)
{
	CECNPC* pHHNPC = NULL;	//	The NPC will be high-light rendered
	int idHoverObject = 0;

	CECHostPlayer* pHost = g_pGame->GetGameRun()->GetHostPlayer();
	if (pHost)
	{
		if (pHost->IsChangingFace())
			return true;

		idHoverObject = pHost->GetCursorHoverObject();
	}
	
	//	Render all NPCs in active table
	NPCTable::iterator it = m_NPCTab.begin();
	for (; it != m_NPCTab.end(); ++it)
	{
		CECNPC* pNPC = *it.value();
		if (idHoverObject == pNPC->GetNPCID())
			pHHNPC = pNPC;
		else
			pNPC->Render(pViewport);
	}

	//	Render high-lighted NPC
	if (pHHNPC)
		pHHNPC->RenderHighLight(pViewport);

	//	Render all NPCs in disappear table
	for (int i=0; i < m_aDisappearNPCs.GetSize(); i++)
	{
		CECNPC* pNPC = m_aDisappearNPCs[i];
		pNPC->Render(pViewport);
	}

	return true; 
}

//	RenderForReflected routine
bool CECNPCMan::RenderForReflect(CECViewport * pViewport)
{
	// all npc are reflect visible, but no gfx should be register again;
	NPCTable::iterator it = m_NPCTab.begin();
	for (; it != m_NPCTab.end(); ++it)
	{
		CECNPC* pNPC = *it.value();
		pNPC->Render(pViewport, CECObject::RD_REFLECT);
	}

	return true;
}

//	RenderForRefract routine
bool CECNPCMan::RenderForRefract(CECViewport * pViewport)
{
	// all npc are refract visible
	NPCTable::iterator it = m_NPCTab.begin();
	for (; it != m_NPCTab.end(); ++it)
	{
		CECNPC* pNPC = *it.value();
		pNPC->Render(pViewport, CECObject::RD_REFRACT);
	}

	return true;
}

//	On entering game world
bool CECNPCMan::OnEnterGameWorld()
{
	return true; 
}

//	On leaving game world
bool CECNPCMan::OnLeaveGameWorld()
{
	//	Release all NPCs in active table
	NPCTable::iterator it = m_NPCTab.begin();
	for (; it != m_NPCTab.end(); ++it)
	{
		CECNPC* pNPC = *it.value();
		ReleaseNPC(pNPC);
	}

	m_NPCTab.clear();

	//	Release all NPCs in disappear table
	int i;
	for (i=0; i < m_aDisappearNPCs.GetSize(); i++)
		ReleaseNPC(m_aDisappearNPCs[i]);
	
	m_aDisappearNPCs.RemoveAll();

	//	Release all loaded models
	ACSWrapper csa(&m_csLoad);

	for (i=0; i < m_aLoadedModels.GetSize(); i++)
	{
		NPCMODEL* pInfo = m_aLoadedModels[i];
		CECNPC::ReleaseNPCModel(pInfo->Ret);
		delete pInfo;
	}

	m_aLoadedModels.RemoveAll();
	m_aMMNPCs.RemoveAll(false);
	m_aTabSels.RemoveAll(false);

	return true;
}

//	Process message
bool CECNPCMan::ProcessMessage(const ECMSG& Msg)
{
	ASSERT(Msg.iManager == MAN_NPC);

	using namespace S2C;

	if (!Msg.iSubID)
	{
		switch (Msg.dwMsg)
		{
		case MSG_NM_NPCINFO:		OnMsgNPCInfo(Msg);			break;
		case MSG_NM_NPCMOVE:		OnMsgNPCMove(Msg);			break;
		case MSG_NM_NPCRUNOUT:		OnMsgNPCRunOut(Msg);		break;
		case MSG_NM_NPCDIED:		OnMsgNPCDied(Msg);			break;
		case MSG_NM_NPCDISAPPEAR:	OnMsgNPCDisappear(Msg);		break;
		case MSG_NM_NPCOUTOFVIEW:	OnMsgNPCOutOfView(Msg);		break;
		case MSG_NM_NPCSTOPMOVE:	OnMsgNPCStopMove(Msg);		break;
		case MSG_NM_INVALIDOBJECT:	OnMsgInvalidObject(Msg);	break;
		case MSG_NM_FORBIDBESELECTED:	OnMsgForbidBeSelected(Msg);	break;

		case MSG_NM_NPCATKRESULT:
		case MSG_NM_NPCEXTSTATE:
		case MSG_NM_NPCCASTSKILL:
		case MSG_NM_ENCHANTRESULT:
		case MSG_NM_NPCROOT:
		case MSG_NM_NPCSKILLRESULT:
		case MSG_NM_NPCLEVELUP:
		case MSG_NM_NPCINVISIBLE:

		case MSG_NM_NPCSTARTPLAYACTION:
		case MSG_NM_NPCSTOPPLAYACTION:
		case MSG_NM_MULTIOBJECT_EFFECT:

			TransmitMessage(Msg);
			break;
		}
	}

	return true; 
}

//	Transmit message
bool CECNPCMan::TransmitMessage(const ECMSG& Msg)
{
	using namespace S2C;

	int nid = 0;

	switch (Msg.dwMsg)
	{
	case MSG_NM_NPCATKRESULT:

		ASSERT(Msg.dwParam2 == OBJECT_ATTACK_RESULT);
		nid = ((cmd_object_atk_result*)Msg.dwParam1)->attacker_id;
		break;

	case MSG_NM_NPCEXTSTATE:

		nid = ((cmd_update_ext_state*)Msg.dwParam1)->id;
		break;

	case MSG_NM_NPCCASTSKILL:

		nid = ((cmd_object_cast_skill*)Msg.dwParam1)->caster;
		break;

	case MSG_NM_ENCHANTRESULT:

		nid = ((cmd_enchant_result*)Msg.dwParam1)->caster;
		break;

	case MSG_NM_NPCROOT:

		nid = ((cmd_object_root*)Msg.dwParam1)->id;
		break;

	case MSG_NM_NPCSKILLRESULT:

		nid = ((cmd_object_skill_attack_result*)Msg.dwParam1)->attacker_id;
		break;

	case MSG_NM_NPCLEVELUP:

		nid = ((cmd_level_up*)Msg.dwParam1)->id;
		break;

	case MSG_NM_NPCINVISIBLE:
		
		nid = ((cmd_object_invisible*)Msg.dwParam1)->id;
		break;

	case MSG_NM_NPCSTARTPLAYACTION:
		nid = ((cmd_object_start_play_action*)Msg.dwParam1)->id;
		break;

	case MSG_NM_NPCSTOPPLAYACTION:
		nid = ((cmd_object_stop_play_action*)Msg.dwParam1)->id;
		break;
	case MSG_NM_MULTIOBJECT_EFFECT:
		nid = ((cmd_multiobj_effect*)Msg.dwParam1)->id;
		break;

	default:
		ASSERT(0);
		return false;
	}

	CECNPC* pNPC = SeekOutNPC(nid);
	if (pNPC)
		pNPC->ProcessMessage(Msg);
	else
	{
		//	补充处理攻击方、被攻击方其中一者为玩家宠物时的伤害显示
		//	宠物受伤、或者被宠物攻击的对象受伤，都是玩家关心的内容
		//

		CECNPC *pAttacker = GetNPCFromAll(nid);
		CECHostPlayer* pHost = g_pGame->GetGameRun()->GetHostPlayer();
		CECPlayerMan *pPlayerMan = g_pGame->GetGameRun()->GetWorld()->GetPlayerMan();

		int idTarget(0);
		int dwModifier(0);
		int nDamage(0);
		switch (Msg.dwMsg)
		{
		case MSG_NM_NPCATKRESULT:
			{
				cmd_object_atk_result *pCmd = (cmd_object_atk_result*)Msg.dwParam1;
				idTarget = pCmd->target_id;
				dwModifier = pCmd->attack_flag;
				nDamage = pCmd->damage;
			}
			break;
			
		case MSG_NM_ENCHANTRESULT:
			{
				cmd_enchant_result * pCmd = (cmd_enchant_result *)Msg.dwParam1;
				idTarget = pCmd->target;
				dwModifier = pCmd->attack_flag;
				nDamage = (GNET::ElementSkill::GetType(pCmd->skill) == GNET::TYPE_ATTACK) ? -1 : -2;
			}
			break;

		case MSG_NM_NPCSKILLRESULT:
			{
				cmd_object_skill_attack_result *pCmd = (cmd_object_skill_attack_result*)Msg.dwParam1;
				idTarget = pCmd->target_id;
				dwModifier = pCmd->attack_flag;
				nDamage = pCmd->attack_flag;
			}
			break;
		}
		
		while (idTarget)
		{
			if (ISNPCID(idTarget))
			{
				//	被攻击者是 NPC 时

				CECNPC* pTarget = GetNPCFromAll(idTarget);
				if (!pTarget)
				{
					//	无法获取被攻击者，不再需要处理
					break;
				}
				if (pTarget->IsPetNPC() && pTarget->GetMasterID() == pHost->GetCharacterID())
				{
					//	被攻击者为玩家宠物，直接显示伤害信息即可
					pTarget->Damaged(nDamage, dwModifier);
					break;
				}
				//	被攻击者不是玩家宠物，检查攻击者是否为玩家宠物
				if (!pAttacker)
				{
					//	无法查询攻击者
					break;
				}
				if (pAttacker->IsPetNPC() && pAttacker->GetMasterID() == pHost->GetCharacterID())
				{
					//	攻击者是玩家宠物，显示被攻击者信息
					pTarget->Damaged(nDamage, dwModifier);
				}
				break;
			}
			if (ISPLAYERID(idTarget))
			{
				//	被攻击者是其它玩家时
				
				CECPlayer *pTarget = pPlayerMan->GetPlayer(idTarget);
				if (!pTarget)
				{
					break;
				}
				if (!pAttacker)
				{
					break;
				}
				if (pAttacker->IsPetNPC() && pAttacker->GetMasterID() == pHost->GetCharacterID())
				{
					pTarget->Damaged(nDamage, dwModifier);
				}
				break;
			}
			break;
		}
	}

	return true;
}

//	Messages handlers
bool CECNPCMan::OnMsgNPCInfo(const ECMSG& Msg)
{
	using namespace S2C;

	switch (Msg.dwParam2)
	{
	case NPC_INFO_LIST:
	{
		cmd_npc_info_list* pCmd = (cmd_npc_info_list*)Msg.dwParam1;
		BYTE* pDataBuf = &pCmd->placeholder;

		for (int i=0; i < pCmd->count; i++)
		{
			const info_npc& Info = *(const info_npc*)pDataBuf;

			//	Calculate npc info data size and skip it
			int iSize = sizeof (info_npc);
			if (Info.state & GP_STATE_EXTEND_PROPERTY)
				iSize += sizeof (DWORD) * OBJECT_EXT_STATE_COUNT;

			if (Info.state & GP_STATE_NPC_PET)
				iSize += sizeof (int);

			if (Info.state & GP_STATE_NPC_NAME)
			{
				BYTE len = *(BYTE*)(pDataBuf + iSize);
				iSize += sizeof (BYTE) + len;
			}
			if (Info.state & GP_STATE_MULTIOBJ_EFFECT){
				int count = *(int*)(pDataBuf + iSize);
				iSize += sizeof (int);
				iSize += count * (sizeof(int) + sizeof(char));
			}
			if (Info.state & GP_STATE_NPC_MAFIA){
				iSize += sizeof(int);
			}

			NPCEnter(Info, false);
			pDataBuf += iSize;
		}

		break;
	}
	case NPC_ENTER_SLICE:
		NPCEnter(*(info_npc*)Msg.dwParam1, false);
		break;

	case NPC_ENTER_WORLD:
		NPCEnter(*(info_npc*)Msg.dwParam1, true);
		break;
	
	case NPC_INFO_00:
	{
		cmd_npc_info_00* pCmd = (cmd_npc_info_00*)Msg.dwParam1;
		CECNPC* pNPC = SeekOutNPC(pCmd->idNPC);
		if (pNPC)
		{
			ROLEBASICPROP& bp = pNPC->GetBasicProps();
			ROLEEXTPROP& ep = pNPC->GetExtendProps();

			bp.iCurHP		= pCmd->iHP;
			ep.bs.max_hp	= pCmd->iMaxHP;
			pNPC->SetSelectedTarget(pCmd->iTargetID);
		}

		break;
	}
	case NPC_VISIBLE_TID_NOTIFY:
		{
			cmd_npc_visible_tid_notify* pCmd = (cmd_npc_visible_tid_notify*)Msg.dwParam1;
			CECNPC* pNPC = SeekOutNPC(pCmd->nid);
			if (pNPC)
				pNPC->TransformShape(pCmd->vis_tid);
			break;
		}
	}

	return true;
}

//	Message MSG_NM_NPCMOVE handler
bool CECNPCMan::OnMsgNPCMove(const ECMSG& Msg)
{
	using namespace S2C;

	cmd_object_move* pCmd = (cmd_object_move*)Msg.dwParam1;
	ASSERT(pCmd);

	if (!pCmd->use_time)
		return true;

	CECNPC* pNPC = SeekOutNPC(pCmd->id);
	if (pNPC)
		pNPC->MoveTo(*pCmd);

	return true;
}

//	Message MSG_NM_NPCRUNOUT handler
bool CECNPCMan::OnMsgNPCRunOut(const ECMSG& Msg)
{
	using namespace S2C;

	cmd_leave_slice* pCmd = (cmd_leave_slice*)Msg.dwParam1;
	ASSERT(pCmd);

	NPCLeave(pCmd->id);

	return true;
}

bool CECNPCMan::OnMsgNPCDied(const ECMSG& Msg)
{
	using namespace S2C;

	int nid=0, idKiller=0;
	bool bDelay = false;

	if (Msg.dwParam2 == NPC_DIED)
	{
		cmd_npc_died* pCmd = (cmd_npc_died*)Msg.dwParam1;
		nid = pCmd->id;
		idKiller = pCmd->idKiller;
	}
	else if (Msg.dwParam2 == NPC_DIED2)
	{
		cmd_npc_died2* pCmd = (cmd_npc_died2*)Msg.dwParam1;
		nid = pCmd->id;
		idKiller = pCmd->idKiller;
		bDelay = true;
	}

	if (!ISNPCID(nid))
		return false;

	CECNPC* pNPC = GetNPC(nid);
	if (pNPC && !pNPC->IsAboutToDie())
	{
		pNPC->Killed(bDelay);

	//	Below codes may case the last damaged bubble number before
	//	npc died couldn't popup
	//	if (!bDelay)
	//		NPCDisappear(nid);
	}
	
	CECGameRun* pGameRun = g_pGame->GetGameRun();
	if (pGameRun->GetHostPlayer())
	{
		if( idKiller == pGameRun->GetHostPlayer()->GetCharacterID() )
		{
			// we need to notify help system that the user has killed some npc, to keep the check state uptodate.
			CECScriptMan * pScriptMan = pGameRun->GetUIManager()->GetScriptMan();
			if( pScriptMan )
			{
				pScriptMan->GetContext()->GetCheckState()->SetKillMonster(true);
			}
		}
	}

	return true;
}

bool CECNPCMan::OnMsgNPCDisappear(const ECMSG& Msg)
{
	using namespace S2C;
	cmd_object_disappear* pCmd = (cmd_object_disappear*)Msg.dwParam1;
	ASSERT(pCmd && ISNPCID(pCmd->id));

	NPCDisappear(pCmd->id);
	return true;
}

bool CECNPCMan::OnMsgNPCOutOfView(const ECMSG& Msg)
{
	NPCLeave((int)Msg.dwParam1);
	return true;
}

bool CECNPCMan::OnMsgNPCStopMove(const ECMSG& Msg)
{
	using namespace S2C;

	cmd_object_stop_move* pCmd = (cmd_object_stop_move*)Msg.dwParam1;
	ASSERT(pCmd);

	CECNPC* pNPC = SeekOutNPC(pCmd->id);
	if (pNPC)
		pNPC->StopMoveTo(*pCmd);

	return true;
}

bool CECNPCMan::OnMsgInvalidObject(const ECMSG& Msg)
{
	using namespace S2C;
	cmd_invalid_object* pCmd = (cmd_invalid_object*)Msg.dwParam1;
	ASSERT(pCmd);

	//	Remove the NPC if it exists
	CECNPC* pNPC = GetNPC(pCmd->id);
	if (pNPC)
	{
		NPCLeave(pCmd->id);
	}

	return true;
}

bool CECNPCMan::OnMsgForbidBeSelected(const ECMSG& Msg)
{
	using namespace S2C;
	cmd_object_forbid_be_selected* pCmd = (cmd_object_forbid_be_selected*)Msg.dwParam1;
	ASSERT(pCmd);

	//	Remove the NPC if it exists
	CECNPC* pNPC = GetNPC(pCmd->id);
	if (pNPC)
	{
		pNPC->SetSelectable(pCmd->b == 0);
	}

	return true;
}

//	Create a NPC
CECNPC* CECNPCMan::CreateNPC(const S2C::info_npc& Info, bool bBornInSight)
{
	CECNPC* pNPC = NULL;

	int tid = Info.tid;
	bool bPet = (Info.state & GP_STATE_NPC_PET) ? true : false;

	//	Get data type from database
	DATA_TYPE DataType = g_pGame->GetElementDataMan()->get_data_type(tid, ID_SPACE_ESSENCE);
	if (DataType != DT_NPC_ESSENCE && DataType != DT_MONSTER_ESSENCE && DataType != DT_PET_ESSENCE)
	{
		//	Try to create a default npc
		tid = 4249;
		DataType = g_pGame->GetElementDataMan()->get_data_type(tid, ID_SPACE_ESSENCE);
	}

	if (bPet)
	{
		pNPC = new CECPet(this);
	}
	else
	{
		switch (DataType)
		{
		case DT_NPC_ESSENCE:		pNPC = new CECNPCServer(this);	break;
		case DT_MONSTER_ESSENCE:	pNPC = new CECMonster(this);	break;
		case DT_PET_ESSENCE:		pNPC = new CECPet(this);		break;
		default:
		{
			ASSERT(0);
			return NULL;
		}
		}
	}

	//	Set born stamp
	pNPC->SetBornStamp(g_pGame->GetGameRun()->GetWorld()->GetBornStamp());
	pNPC->SetBornInSight(bBornInSight);

	if (!pNPC || !pNPC->Init(tid, Info))
	{
		delete pNPC;
		glb_ErrorOutput(ECERR_FAILEDTOCALL, "CECNPCMan::CreateNPC", __LINE__);
		return false;
	}

	return pNPC;
}

//	Release a NPC
void CECNPCMan::ReleaseNPC(CECNPC* pNPC)
{
	if (pNPC)
	{
		//	Remove tab-selected array
		CECHostPlayer* pHost = m_pGameRun->GetHostPlayer();
		if (pHost)
			pHost->RemoveObjectFromTabSels(pNPC);

		pNPC->Release();
		delete pNPC;
		pNPC = NULL;
	}
}

//	Put NPC into disappear table
void CECNPCMan::NPCDisappear(int nid)
{
	CECNPC* pNPC = GetNPC(nid);
	if (pNPC)
	{
		if (!pNPC->IsDead())
		{
			//	NPC 消失时（可能身上还有相关特效，需要触发，如自爆时爆炸特效）
			//	虽然在 CECNPC::Killed 中已做处理，但客户端可能会直接收到 disappear 消息（如自爆技能）
			//	因此这里需要增加触发调用
			//	若之前 NPC 已死亡，则说明已调用过
			pNPC->ClearComActFlag(true);
		}

		pNPC->Disappear();

		//	From npc from active table and add it to disappear table
		NPCLeave(nid, true, false);
		m_aDisappearNPCs.Add(pNPC);
	}
}

//	One NPC enter view area
bool CECNPCMan::NPCEnter(const S2C::info_npc& Info, bool bBornInSight)
{
	//	If NPC with specified id exists, delete it
	CECNPC* pNPC = GetNPC(Info.nid);

	//	NPC 立即死亡、并且复活时间小于5秒时，有机会指针还在 m_NPCTab 里 及 m_aMMNPCs 里
	//	因此，立即复活时，会单独删除 m_NPCTab 里的 NPC 但在 m_aMMNPCs 里留下无效指针，
	//	而恰巧“同时”（即同一 Tick 里）有周围其它 NPC 消失的消息需要处理，而又恰巧无效指针是 m_aMMNPCs 里的最后一个，
	//	这种情况下，会在从 m_aMMNPCs 里删除这个“其它NPC”时写访问无效指针、从而导致客户端崩溃
	if (pNPC) RemoveNPCFromMiniMap(pNPC);

	ReleaseNPC(pNPC);
	m_NPCTab.erase(Info.nid);

	//	If this NPC id is in unknown table, remove it because this NPC
	//	won't be unknown anymore
	UkNPCTable::pair_type Pair = m_UkNPCTab.get(Info.nid);
	if (Pair.second)
		m_UkNPCTab.erase(Info.nid);

	//	Create a new NPC
	if (!(pNPC = CreateNPC(Info, bBornInSight)))
	{
		a_LogOutput(1, "CECNPCMan::NPCEnter, Failed to create NPC (%d)", Info.tid);
		return false;
	}

//	pNPC->SetTransparence(1.0f, 0.0f, 3000);
	
	//	Add NPC to table
	m_NPCTab.put(Info.nid, pNPC);

	return true;
}

//	One NPC leave view area
void CECNPCMan::NPCLeave(int nid, bool bUpdateMMArray/* true */, bool bRelease/* true */)
{
	//	Release NPC
	CECNPC* pNPC = GetNPC(nid);
	if (!pNPC)
		return;

	if (bUpdateMMArray)
		RemoveNPCFromMiniMap(pNPC);

	pNPC->m_iMMIndex = -1;

	//	If this NPC is selected by host, cancel the selection
	if (pNPC->GetNPCID() == g_pGame->GetGameRun()->GetHostPlayer()->GetSelectedTarget())
		m_pGameRun->GetHostPlayer()->SelectTarget(0);

	//	Remove it from active NPC table
	m_NPCTab.erase(nid);

	//	Forbid reloading npc's resources
	QueueNPCUndoLoad(nid, pNPC->GetBornStamp());

	//	Release NPC resource
	if (bRelease)
		ReleaseNPC(pNPC);
	else
	{
		CECHostPlayer* pHost = m_pGameRun->GetHostPlayer();
		if (pHost)
			pHost->RemoveObjectFromTabSels(pNPC);
	}

	CECPlayerWrapper* pWrapper = CECAutoPolicy::GetInstance().GetPlayerWrapper();
	if( pWrapper ) pWrapper->OnObjectDisappear(nid);
}

//	Get NPC of speicified ID
CECNPC* CECNPCMan::GetNPC(int nid, DWORD dwBornStamp/* 0 */)
{
	//	NPC exists ?
	NPCTable::pair_type Pair = m_NPCTab.get(nid);
	if (!Pair.second)
		return NULL;	//	Counldn't find this NPC

	if (dwBornStamp)
	{
		CECNPC* pNPC = *Pair.first;
		if (pNPC->GetBornStamp() != dwBornStamp)
			return NULL;
	}

	return *Pair.first;
}

CECNPC* CECNPCMan::GetNPCFromAll(int nid)
{
	CECNPC* pNPC = GetNPC(nid);
	if (pNPC)
		return pNPC;

	//	Search from disappear array ?
	for (int i=0; i < m_aDisappearNPCs.GetSize(); i++)
	{
		CECNPC* pNPC = m_aDisappearNPCs[i];
		if (pNPC->GetNPCID() == nid)
			return pNPC;
	}

	return NULL;
}

//	Get NPC's name
const wchar_t* CECNPCMan::GetNPCName(int nid)
{
	CECNPC* pNPC = GetNPC(nid);
	if (pNPC)
		return pNPC->GetName();
	else
		return L"Unknown NPC";
}

//	Seek out NPC with specified id, if NPC doesn't exist, try to get from server
CECNPC* CECNPCMan::SeekOutNPC(int nid)
{
	NPCTable::pair_type Pair = m_NPCTab.get(nid);
	if (!Pair.second)
	{
		//	Counldn't find this NPC, put it into unkonwn NPC table
		m_UkNPCTab.put(nid, nid);
		return NULL;
	}

	return *Pair.first;
}

//	Ray trace
bool CECNPCMan::RayTrace(ECRAYTRACE* pTraceInfo)
{
	//	If alt is pressed, ignore all npc and monsters
	//	If shift is pressed, ignore all pets
	bool bIgnoreNPC = (pTraceInfo->dwKeyStates & EC_KSF_ALT) ? true : false;
	bool bIgnorePet = (pTraceInfo->dwKeyStates & EC_KSF_SHIFT) ? true : false;
	if (bIgnoreNPC && bIgnorePet)
		return false;

	bool bRet = false;

	//	Trace all NPCs
	NPCTable::iterator it = m_NPCTab.begin();
	for (; it != m_NPCTab.end(); ++it)
	{
		CECNPC* pNPC = *it.value();

		//	TRACEOBJ_LBTNCLICK is caused by player pressed left button
		if (pNPC->IsDead() && (pTraceInfo->iTraceObj == TRACEOBJ_LBTNCLICK || 
			pTraceInfo->iTraceObj == TRACEOBJ_TESTRAY))
			continue;

		if ((pNPC->IsPetNPC() && bIgnorePet) || (!pNPC->IsPetNPC() && bIgnoreNPC))
			continue;

		if (pNPC->RayTrace(pTraceInfo))
		{
			bRet = true;
		}
	}

	return bRet;
}

// Trace for CD
bool CECNPCMan::TraceWithBrush(BrushTraceInfo * pInfo)
{
	bool bCollide=false;

	//save original result
	bool		bStartSolid = pInfo->bStartSolid;	//	Collide something at start point
	bool		bAllSolid = pInfo->bAllSolid;		//	All in something
	int			iClipPlane = pInfo->iClipPlane;		//	Clip plane's index
	float		fFraction = 100.0f;		//	Fraction
	A3DVECTOR3  vNormal = pInfo->ClipPlane.GetNormal(); //clip plane normal
	float       fDist = pInfo->ClipPlane.GetDist();	//clip plane dist

	// now see if collide with dynamic scene building
	NPCTable::iterator it = m_NPCTab.begin();
	for (; it != m_NPCTab.end(); ++it)
	{
		CECNPC* pNPC = *it.value();
		if ((fFraction > 0.0f) && pNPC && pNPC->TraceWithBrush(pInfo) 
			&& (pInfo->fFraction < fFraction ) ) 
		{
			fFraction = pInfo->fFraction;
			bAllSolid = pInfo->bAllSolid;
			bStartSolid = pInfo->bStartSolid;
			iClipPlane = pInfo->iClipPlane;
			vNormal = pInfo->ClipPlane.GetNormal();
			fDist = pInfo->ClipPlane.GetDist();
			bCollide=true;
		}
	}		

	//set back
	pInfo->fFraction = fFraction;
	pInfo->bStartSolid = bStartSolid;
	pInfo->bAllSolid = bAllSolid;
	pInfo->iClipPlane = iClipPlane;
	pInfo->ClipPlane.SetNormal(vNormal);
	pInfo->ClipPlane.SetD(fDist);
	return bCollide;
}

//	Update NPCs in various ranges (Active, visible, mini-map etc.)
void CECNPCMan::UpdateNPCInRanges(DWORD dwDeltaTime)
{
	CECHostPlayer* pHost = m_pGameRun->GetHostPlayer();
	if (!pHost || !pHost->IsAllResReady())
		return;

	CECConfigs* pConfigs = g_pGame->GetConfigs();
	int idHoverObject = pHost->GetCursorHoverObject();
	int idSelected = pHost->GetSelectedTarget();

	m_aMMNPCs.RemoveAll(false);

	//	Check all NPCs
	NPCTable::iterator it = m_NPCTab.begin();
	for (; it != m_NPCTab.end(); )
	{
		CECNPC* pNPC = *it.value();
		float fDistToCamera = pNPC->GetDistToCamera();
		float fDistToHost = pNPC->GetDistToHost();
		float fDistToHostH = pNPC->GetDistToHostH();

		//	Check whether name is visible
		if (fDistToHost <= 20.0f || pNPC->GetNPCID() == idHoverObject ||
			pNPC->GetNPCID() == idSelected)
			pNPC->SetRenderNameFlag(true);
		else
			pNPC->SetRenderNameFlag(false);

		//	Set bars visible flags
		pNPC->SetRenderBarFlag(pNPC->GetNPCID() == idSelected);

		//	Check whether NPC is in mini-map
		if (fDistToHostH > pConfigs->GetSevActiveRadius() && !pHost->IsDeferedUpdateSlice())
		{
			++it;
			NPCLeave(pNPC->GetNPCID(), false);
			continue;
		}
		else if (fDistToHostH <= pConfigs->GetSevActiveRadius())
			pNPC->m_iMMIndex = m_aMMNPCs.Add(pNPC);

		//	Check NPC's visiblity
		pNPC->SetVisible(fDistToCamera <= pConfigs->GetSevActiveRadius() ? true : false);

		//	Show quest icon ?
		if (pNPC->IsServerNPC())
			((CECNPCServer*)pNPC)->SetQuestIconFlag(fDistToHost <= 60.0f ? true : false);

		++it;
	}
}

//	Remove NPC from m_aMMNPCs array
void CECNPCMan::RemoveNPCFromMiniMap(CECNPC* pNPC)
{
	ASSERT(pNPC);
	int iIndex = pNPC->m_iMMIndex;
	if (iIndex < 0 || iIndex >= m_aMMNPCs.GetSize())
		return;

	CECNPC* pTempNPC = m_aMMNPCs[iIndex];
	if (pTempNPC != pNPC)
	{
		ASSERT(pTempNPC == pNPC);
		return;
	}

	//	Exchange with the last layer in array
	int iLastIdx = m_aMMNPCs.GetSize() - 1;
	if (m_aMMNPCs.GetSize() > 1 && iIndex != iLastIdx)
	{
		pTempNPC = m_aMMNPCs[iLastIdx];
		m_aMMNPCs[iIndex] = pTempNPC;
		pTempNPC->m_iMMIndex = iIndex;
		m_aMMNPCs.RemoveAt(iLastIdx);
	}
	else
		m_aMMNPCs.RemoveAtQuickly(iIndex);
}

//	When world's loading center changed
void CECNPCMan::OnLoadCenterChanged()
{
	m_aMMNPCs.RemoveAll(false);
}

//	Update unknown NPC table
void CECNPCMan::UpdateUnknownNPCs()
{
	DWORD dwRealTime = g_pGame->GetRealTickTime();
	if (!m_QueryCnt.IncCounter(dwRealTime))
		return;

	m_QueryCnt.Reset();

	if (!m_UkNPCTab.size())
		return;

	AArray<int, int> aIDs;

	UkNPCTable::iterator it = m_UkNPCTab.begin();
	for (; it != m_UkNPCTab.end(); ++it)
		aIDs.Add(*it.value());

	g_pGame->GetGameSession()->c2s_CmdQueryNPCInfo1(aIDs.GetSize(), aIDs.GetData());

	m_UkNPCTab.clear();
}

bool CECNPCMan::ISNPCModelNeedLoad(int nid, DWORD dwBornStamp)
{
	bool bNeedLoad(false);

	// 查看 NPC 是否已经加载
	//
	CECNPC *pNPC = GetNPC(nid, dwBornStamp);
	if (pNPC)
	{
		// NPC 存在，且尚未交付
		if (!pNPC->HasModel())
		{
			ACSWrapper csa(&m_csLoad);

			int i(0);
			int nLoaded = m_aLoadedModels.GetSize();
			for (i=0; i < nLoaded; i++)
			{
				const NPCMODEL* pInfo = m_aLoadedModels[i];
				if (pInfo && pInfo->nid == nid &&
					pInfo->dwBornStamp == dwBornStamp)
				{
					// NPC 即将交付，因此不需再加载
					break;
				}
			}

			if (i >= nLoaded)
			{
				// NPC 现在还未交付，因此需要继续加载
				bNeedLoad = true;
			}
		}
	}

	return bNeedLoad;
}

//	Load NPC model in loading thread
bool CECNPCMan::ThreadLoadNPCModel(int nid, DWORD dwBornStamp, int tid, const char* szFile)
{
	NPCMODEL* pInfo = new NPCMODEL;
	pInfo->nid = nid;
	pInfo->dwBornStamp = dwBornStamp;

	if (!CECNPC::LoadNPCModel(tid, szFile, pInfo->Ret))
	{
		delete pInfo;
		return false;
	}

	ACSWrapper csa(&m_csLoad);
	m_aLoadedModels.Add(pInfo);

	return true;
}

//	Deliver loaded NPC models
void CECNPCMan::DeliverLoadedNPCModels()
{
	ACSWrapper csa(&m_csLoad);

	for (int i=0; i < m_aLoadedModels.GetSize(); i++)
	{
		NPCMODEL* pInfo = m_aLoadedModels[i];
		CECNPC* pNPC = GetNPC(pInfo->nid, pInfo->dwBornStamp);
		if (pNPC)
			pNPC->SetNPCLoadedResult(pInfo->Ret);
		else
			CECNPC::ReleaseNPCModel(pInfo->Ret);
	
		delete pInfo;
	}

	m_aLoadedModels.RemoveAll(false);
}

//	Get pet of specified nid
CECPet* CECNPCMan::GetPetByID(int nid)
{
	if (!ISNPCID(nid))
		return NULL;
	
	CECNPC* pNPC = GetNPC(nid);
	if (!pNPC || !pNPC->IsPetNPC())
		return NULL;

	return (CECPet*)pNPC;
}

//	Remove npc from tab-selected array
void CECNPCMan::RemoveNPCFromTabSels(CECNPC* pNPC)
{
	for (int i=0; i < m_aTabSels.GetSize(); i++)
	{
		if (m_aTabSels[i] == pNPC)
		{
			m_aTabSels.RemoveAtQuickly(i);
			break;
		}
	}
}

//	Get npc candidates whom can be auto-selected by 'TAB' key
void CECNPCMan::TabSelectCandidates(int idCurSel, APtrArray<CECNPC*>& aCands)
{
	CECHostPlayer* pHost = g_pGame->GetGameRun()->GetHostPlayer();
	if (!pHost->IsSkeletonReady()){
		//	只有 IsSkeletonReady() 为 true 时 GetDistToHost() 才有效
		return;
	}

	//	Trace all NPCs
	NPCTable::iterator it = m_NPCTab.begin();
	for (; it != m_NPCTab.end(); ++it)
	{
		CECNPC* pNPC = *it.value();
		if (!pNPC->IsSelectable() ||
			!pNPC->IsMonsterNPC() ||
			pNPC->IsDead() || pNPC->GetNPCID() == idCurSel ||
			pHost->AttackableJudge(pNPC->GetNPCID(), false) != 1)
			continue;
		
		float fDist = pNPC->GetDistToHost();
		
		if (fDist > EC_TABSEL_DIST || !CECHostPlayer::CanSafelySelectWith(fDist))
		{
			continue;
		}

		aCands.Add(pNPC);
	}
}

void CECNPCMan::OptimizeShowExtendStates()
{
	//	根据当前优化设置，更新 NPC 上的 Extend States 光效显示
	NPCTable::iterator it = m_NPCTab.begin();
	for (; it != m_NPCTab.end(); ++it)
	{
		CECNPC* pNPC = *it.value();
		pNPC->OptimizeShowExtendStates();
	}
}

void CECNPCMan::OptimizeWeaponStoneGfx()
{
	NPCTable::iterator it = m_NPCTab.begin();
	for (; it != m_NPCTab.end(); ++it)
	{
		CECNPC* pNPC = *it.value();
		pNPC->OptimizeWeaponStoneGfx();
	}
}

void CECNPCMan::OptimizeArmorStoneGfx()
{
	NPCTable::iterator it = m_NPCTab.begin();
	for (; it != m_NPCTab.end(); ++it)
	{
		CECNPC* pNPC = *it.value();
		pNPC->OptimizeArmorStoneGfx();
	}
}

void CECNPCMan::OptimizeSuiteGfx()
{
	NPCTable::iterator it = m_NPCTab.begin();
	for (; it != m_NPCTab.end(); ++it)
	{
		CECNPC* pNPC = *it.value();
		pNPC->OptimizeSuiteGfx();
	}
}

CECNPC* CECNPCMan::FindNPCByTempleID(int templeId)
{
	NPCTable::iterator it = m_NPCTab.begin();
	for (; it != m_NPCTab.end(); ){
		CECNPC* pNPC = *it.value();
		if (pNPC->GetTemplateID() == templeId)
			return pNPC;
		++it;
	}
	return NULL;
}

CECNPC* CECNPCMan::GetMouseOnPateTextNPC( int x, int y )
{
	CECNPC* result = NULL;
	NPCTable::iterator it = m_NPCTab.begin();
	for (; it != m_NPCTab.end(); ++it)
	{
		CECNPC* pNPC = *it.value();
		if (pNPC && pNPC->IsServerNPC()){
			if (pNPC->IsClickNamePateText(x, y)){
				if(result == NULL)
					result = pNPC;
				if (result->GetDistToCamera() > pNPC->GetDistToCamera()){
					result = pNPC;
				}
			}
		}
	}
	return result;
}

void CECNPCMan::OnModelChange(const CECFactionMan *p, const CECObservableChange *q){
	const CECFactionManChange *pChange = dynamic_cast<const CECFactionManChange *>(q);
	if (!pChange){
		ASSERT(false);
	}else if (pChange->ChangeMask() != CECFactionManChange::FACTION_RENAMEFACTION){
		return;
	}
	const CECFactionManFactionChange *pFactionChange = dynamic_cast<const CECFactionManFactionChange *>(pChange);
	for (NPCTable::iterator it = m_NPCTab.begin(); it != m_NPCTab.end(); ++it){
		CECNPC* pNPC = *it.value();
		if (pNPC && pNPC->IsPetNPC()){
			if (CECPlayer *pClonedMaster = pNPC->GetClonedMaster()){
				if (pClonedMaster->GetFactionID() == pFactionChange->FactionID()){
					pClonedMaster->OnFactionNameChange();
				}
			}
		}
	}
}