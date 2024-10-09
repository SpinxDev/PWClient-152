/*
 * FILE: GameObject.cpp
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Shizhenhua, 2012/8/28
 *
 * HISTORY: 
 *
 * Copyright (c) 2011 ACE Studio, All Rights Reserved.
 */


#include "Common.h"
#include "GameObject.h"
#include "SessionManager.h"
#include "GameSession.h"
#include "EC_GPDataType.h"
#include "EC_SendC2SCmds.h"
#include "EC_Skill.h"
#include "MoveAgent.h"
#include "elementdataman.h"
#include <algorithm>


///////////////////////////////////////////////////////////////////////////
//  
//  Define and Macro
//  
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//  
//  Types and Global variables
//  
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//  
//  Implement class GameObject
//  
///////////////////////////////////////////////////////////////////////////

// 构造函数
GameObject::GameObject( GameSession* pSession )
	: m_pSession(pSession)
{
	// 创建主玩家对象
	if( !(m_pHostPlayer = new PlayerObject(this)) )
	{
		ASSERT(m_pHostPlayer);
		a_LogOutput(1, "GameObject::GameObject(), Create host player failed!");
	}
}

// 析构函数
GameObject::~GameObject()
{
	if( m_pHostPlayer )
	{
		delete m_pHostPlayer;
		m_pHostPlayer = NULL;
	}
}

// 更新游戏对象
void GameObject::Tick()
{
	if( m_pHostPlayer )
		m_pHostPlayer->Tick();
}

// 在指定的范围内寻找一个目标
int GameObject::GatherTarget( int target_mask, float fRange, char bForceAttack )
{
	enum { TARGET_NUM = 3 };
	struct 
	{
		int id;
		float dist;
	} target_list[TARGET_NUM];

	int count = 0;
	int max_dis_index;
	float max_dist;
	fRange *= fRange;

	ObjectTable::iterator it;
	for( it=m_Objects.begin();it!=m_Objects.end();++it )
	{
		ObjectInfo& obj = it->second;
		if( ISPLAYERID(obj.id) && (!(target_mask & TargetElsePlayer) || !bForceAttack) )
			continue;
		if( ISNPCID(obj.id) && !(target_mask & TargetNPC) )
			continue;
		if( ISMATTERID(obj.id) && !(target_mask & TargetMatter) )
			continue;
		if( m_BadMatters.find(obj.id) != m_BadMatters.end() )
			continue;

		// 只选取范围内的物品
		float fDist = (obj.pos - m_pHostPlayer->GetCurPos()).SquaredMagnitude();
		if( fDist > fRange ) continue;

		if( count < TARGET_NUM )
		{
			target_list[count].id = obj.id;
			target_list[count].dist = fDist;
			count++;
		}
		else if( fDist < max_dist )
		{
			target_list[max_dis_index].id = obj.id;
			target_list[max_dis_index].dist = fDist;
		}
		else continue;

		if( count == TARGET_NUM )
		{
			max_dis_index = -1;
			max_dist = -1.0f;
			for( int i=0;i<TARGET_NUM;i++ )
			{
				if( target_list[i].dist > max_dist )
				{
					max_dis_index = i;
					max_dist = target_list[i].dist;
				}
			}
		}
	}

	if( count == 0 )
		return -1;
	else
	{
		int r = a_Random(0, TARGET_NUM-1);
		return target_list[r].id;
	}
}

// 添加到无效列表
void GameObject::AddInvalidMatter(int id)
{
	m_BadMatters.insert(id);
}

// 根据TID查找指定的NPC
int GameObject::SearchNPC(int tid)
{
	ObjectTable::iterator it;
	for( it=m_Objects.begin();it!=m_Objects.end();++it )
	{
		if( it->second.tid == tid )
			return it->second.id;
	}

	return -1;
}

// 获取指定物品信息
GameObject::ObjectInfo* GameObject::GetEntity(int id)
{
	ObjectTable::iterator it = m_Objects.find(id);
	return it != m_Objects.end() ? &it->second : NULL;
}

// 获取指定玩家信息
GameObject::ObjectInfo* GameObject::GetPlayer(int id)
{
	return ISPLAYERID(id) ? GetEntity(id) : NULL;
}

// 获取指定怪物
GameObject::ObjectInfo* GameObject::GetNPC(int id)
{
	return ISNPCID(id) ? GetEntity(id) : NULL;
}

// 获取指定物品
GameObject::ObjectInfo* GameObject::GetMatter(int id)
{
	return ISMATTERID(id) ? GetEntity(id) : NULL;
}

// 响应服务器的S2C协议
void GameObject::OnS2CCommand(WORD cmd, void* pBuf, size_t size)
{
	using namespace S2C;

	switch (cmd)
	{
	case SELF_INFO_1:
		{
			cmd_self_info_1* pCmd = (cmd_self_info_1*)pBuf;
			m_pSession->DoOvertimeCheck(false, GameSession::OT_ENTERGAME, 0);
			m_pHostPlayer->Init(pCmd);

			// 玩家EnterWorld成功，开始获取数据
			SessionManager::GetSingleton().SetTheSession(m_pSession);
			c2s_SendCmdGetAllData(true, true, false);
		}
		break;

	case PLAYER_INFO_1_LIST:
		{
			cmd_player_info_1_list* pCmd = (cmd_player_info_1_list*)pBuf;

			BYTE* pInfoBuf = &pCmd->placeholder;
			BYTE* pEnd = (BYTE*)pBuf + size;
			for( size_t i=0;i<pCmd->count;i++ )
			{
				const info_player_1& Info = *(const info_player_1*)pInfoBuf;
				pInfoBuf += sizeof(info_player_1);

				if( Info.state & GP_STATE_ADV_MODE ) 		pInfoBuf += 2*sizeof(int);
				if( Info.state & GP_STATE_SHAPE ) 			pInfoBuf += sizeof(BYTE);
				if( Info.state & GP_STATE_EMOTE ) 			pInfoBuf += sizeof(BYTE);
				if( Info.state & GP_STATE_EXTEND_PROPERTY )	pInfoBuf += 4*sizeof(unsigned int);
				if( Info.state & GP_STATE_FACTION ) 		pInfoBuf += sizeof(int)+sizeof(BYTE);
				if( Info.state & GP_STATE_BOOTH ) 			pInfoBuf += sizeof(BYTE);
				if( Info.state & GP_STATE_EFFECT )
				{
					BYTE count = *(BYTE*)pInfoBuf;
					pInfoBuf += sizeof(BYTE) + count * sizeof(short);
				}
				if( Info.state & GP_STATE_PARIAH ) 			pInfoBuf += sizeof(BYTE);
				if( Info.state & GP_STATE_IN_MOUNT ) 		pInfoBuf += sizeof(short)+sizeof(int);
				if( Info.state & GP_STATE_IN_BIND ) 		pInfoBuf += sizeof(char)+sizeof(int);
				if( Info.state & GP_STATE_SPOUSE ) 			pInfoBuf += sizeof(int);
				if( Info.state & GP_STATE_EQUIPDISABLED ) 	pInfoBuf += sizeof(int);
				if( Info.state & GP_STATE_PLAYERFORCE )		pInfoBuf += sizeof(int);
				if( Info.state & GP_STATE_MULTIOBJ_EFFECT )
				{
					int effectNum = *(int*)pInfoBuf;
					pInfoBuf += sizeof(int);
					if( effectNum ) pInfoBuf += effectNum * (sizeof(int) + sizeof(char));	
				}
				if( Info.state & GP_STATE_COUNTRY )			pInfoBuf += sizeof(int);

				if( pInfoBuf > pEnd )
				{
					a_LogOutput(1, "PLAYER_INFO_1_LIST size error");
					break;
				}

				if( Info.cid != m_pSession->GetCharacterID() )
				{
					ObjectInfo obj;
					obj.id = Info.cid;
					obj.pos = Info.pos;
					obj.dead = (Info.state & GP_STATE_CORPSE) ? true : false;
					m_Objects[obj.id] = obj;
				}
			}
		}
		break;

	case PLAYER_INFO_1:
	case PLAYER_ENTER_WORLD:
	case PLAYER_ENTER_SLICE:
		{
			info_player_1* pCmd = (info_player_1*)pBuf;
			if( pCmd->cid != m_pSession->GetCharacterID() )
			{
				ObjectInfo obj;
				obj.id = pCmd->cid;
				obj.pos = pCmd->pos;
				obj.dead = (pCmd->state & GP_STATE_CORPSE) ? true : false;
				m_Objects[obj.id] = obj;
			}
		}
		break;

	case PLAYER_LEAVE_WORLD:
		{
			cmd_player_leave_world* pCmd = (cmd_player_leave_world*)pBuf;
			m_Objects.erase(pCmd->id);
		}
		break;

	case NPC_INFO_LIST:
		{
			cmd_npc_info_list* pCmd = (cmd_npc_info_list*)pBuf;
			BYTE* pInfoBuf = &pCmd->placeholder;
			BYTE* pEnd = (BYTE*)pCmd + size;

			for( size_t i=0;i<pCmd->count;i++ )
			{
				const info_npc& Info = *(info_npc*)pInfoBuf;
				pInfoBuf += sizeof(info_npc);

				if( Info.state & GP_STATE_EXTEND_PROPERTY )	pInfoBuf += sizeof(DWORD) * 4;
				if( Info.state & GP_STATE_NPC_PET )			pInfoBuf += sizeof(int);
				if( Info.state & GP_STATE_NPC_NAME )
				{
					BYTE len = *(BYTE*)pInfoBuf;
					pInfoBuf += sizeof(BYTE) + len;
				}

				if( pInfoBuf > pEnd )
				{
					a_LogOutput(1, "NPC_INFO_LIST size error!");
					break;
				}

				ObjectInfo obj;
				obj.id = Info.nid;
				obj.tid = Info.tid;
				obj.pos = Info.pos;
				m_Objects[obj.id] = obj;
			}
		}
		break;

	case NPC_ENTER_WORLD:
	case NPC_ENTER_SLICE:
		{
			info_npc* pCmd = (info_npc*)pBuf;
			ObjectInfo obj;
			obj.id = pCmd->nid;
			obj.tid = pCmd->tid;
			obj.pos = pCmd->pos;
			m_Objects[obj.id] = obj;
		}
		break;

	case NPC_DIED:			// NPC死后无尸体
		{
			cmd_npc_died* pCmd = (cmd_npc_died*)pBuf;
			m_Objects.erase(pCmd->id);
		}
		break;

	case NPC_DIED2:			// NPC死后有尸体
		{
			cmd_npc_died2* pCmd = (cmd_npc_died2*)pBuf;
			m_Objects.erase(pCmd->id);
		}
		break;

	case MATTER_INFO_LIST:
		{
			ObjectInfo obj;
			cmd_matter_info_list* pCmd = (cmd_matter_info_list*)pBuf;

			for( size_t i=0;i<pCmd->count;i++ )
			{
				obj.id = pCmd->list[i].mid;
				obj.tid = pCmd->list[i].tid & 0xffff;
				obj.pos = pCmd->list[i].pos;
				m_Objects[obj.id] = obj;
			}
		}
		break;

	case MATTER_ENTER_WORLD:
		{
			info_matter* pCmd = (info_matter*)pBuf;
			ObjectInfo obj;
			obj.id = pCmd->mid;
			obj.pos = pCmd->pos;
			obj.tid = pCmd->tid & 0xffff;
			m_Objects[obj.id] = obj;

			// 如果距离够近，直接捡起来
			size_t pile_limit = 0;
			A3DVECTOR3 vCurPos = m_pHostPlayer->GetCurPos();
			if( (obj.pos - vCurPos).SquaredMagnitude() <= 9.5f*9.5f &&
				m_pHostPlayer->MatterCanPickup(obj.tid,pile_limit))
				m_pHostPlayer->Pickup(obj.id, obj.tid);
		}
		break;

	case MATTER_PICKUP:
		{
			cmd_matter_pickup* pCmd = (cmd_matter_pickup*)pBuf;
			m_Objects.erase(pCmd->matter_id);

			m_BadMatters.erase(pCmd->matter_id);
		}
		break;

	case OBJECT_DISAPPEAR:
		{
			cmd_object_disappear* pCmd = (cmd_object_disappear*)pBuf;
			m_Objects.erase(pCmd->id);

			m_BadMatters.erase(pCmd->id);
		}
		break;

	case OBJECT_LEAVE_SLICE:
		{
			cmd_leave_slice* pCmd = (cmd_leave_slice*)pBuf;
			if( ISPLAYERID(pCmd->id) || ISNPCID(pCmd->id) )
				m_Objects.erase(pCmd->id);
		}
		break;

	case OUT_OF_SIGHT_LIST:
		{
			cmd_out_of_sight_list* pCmd = (cmd_out_of_sight_list*)pBuf;
			for( size_t i=0;i<pCmd->uCount;i++ )
			{
				m_Objects.erase(pCmd->idList[i]);

				m_BadMatters.erase(pCmd->idList[i]);
			}
		}
		break;
		
	case NPC_INFO_00:
		{
			cmd_npc_info_00* pCmd = (cmd_npc_info_00*)pBuf;
			m_Objects[pCmd->idNPC].hp = pCmd->iHP;
		}
		break;

	case OBJECT_MOVE:
		{
			cmd_object_move* pCmd = (cmd_object_move*)pBuf;
			m_Objects[pCmd->id].pos = pCmd->dest;
			if( pCmd->move_mode & (GP_MOVE_AIR|GP_MOVE_WATER) ) break;
			BYTE mode = pCmd->move_mode & 0x0f;
			if( mode != GP_MOVE_WALK && mode != GP_MOVE_RUN ) break;
			MoveAgent::GetSingleton().Learn(pCmd->dest);
		}
		break;
	case OBJECT_STOP_MOVE:
		{
			cmd_object_stop_move* pCmd = (cmd_object_stop_move*)pBuf;
			m_Objects[pCmd->id].pos = pCmd->dest;
			if( pCmd->move_mode & (GP_MOVE_AIR|GP_MOVE_WATER) ) break;
			BYTE mode = pCmd->move_mode & 0x0f;
			if( mode != GP_MOVE_WALK && mode != GP_MOVE_RUN ) break;
			MoveAgent::GetSingleton().Learn(pCmd->dest);
		}
		break;
	case PLAYER_DIED:
		{
			cmd_player_died* pCmd = (cmd_player_died*)pBuf;
			m_Objects[pCmd->idPlayer].dead = true;
		}
		break;
	case PLAYER_REVIVE:
		{
			cmd_player_revive* pCmd = (cmd_player_revive*)pBuf;
			if( pCmd->idPlayer == m_pSession->GetCharacterID() )
			{
				if( pCmd->sReviveType == 0 || pCmd->sReviveType == 2 )
				{
					m_pHostPlayer->SetDead(false);
					m_pHostPlayer->ResetPos(pCmd->pos);
					m_pHostPlayer->ClearTask();
				}
			}
			else
			{
				if( pCmd->sReviveType == 0 || pCmd->sReviveType == 2 )
				{
					m_Objects[pCmd->idPlayer].dead = false;
				}
			}
		}
		break;

	// 交给玩家处理的协议
	case TASK_DATA:
	case TASK_VAR_DATA:
	case OWN_EXT_PROP:
	case SELF_INFO_00:
	case PLAYER_EXT_PROP_MOVE:
	case OWN_IVTR_DETAIL_DATA:
	case GET_OWN_MONEY:
	case PICKUP_MONEY:
	case PICKUP_ITEM:
	case EQUIP_ITEM:
	case PLAYER_DROP_ITEM:
	case TRASHBOX_PWD_STATE:
	case SKILL_DATA:
	case HOST_CORRECT_POS:
	case SET_MOVE_STAMP:
	case NOTIFY_HOSTPOS:
	case SELECT_TARGET:
	case UNSELECT:
	case HOST_START_ATTACK:
	case ATTACK_ONCE:
	case HOST_STOPATTACK:
	case OBJECT_CAST_SKILL:
	case HOST_STOP_SKILL:
	case ENCHANT_RESULT:
	case HOST_DIED:
	case BE_HURT:
	case COOLTIME_DATA:
	case COST_SKILL_POINT:
	case LEARN_SKILL:
	case PLAYER_ENABLE_EFFECT:
	case PLAYER_DISABLE_EFFECT:
	case PLAYER_ENABLE_FASHION:
	case PET_ROOM:
	case PLAYER_BIND_REQUEST:
	case PLAYER_BIND_INVITE:
	case PLAYER_BIND_REQUEST_REPLY:
	case PLAYER_BIND_INVITE_REPLY:
	case PLAYER_BIND_STOP:
	case PLAYER_BIND_START:
		m_pHostPlayer->OnS2CCommand(cmd, pBuf, size);
		break;

	// 下面是未被处理的协议
	case PLAYER_INFO_2:
	case PLAYER_INFO_3:
	case PLAYER_INFO_4:
	case PLAYER_INFO_2_LIST:
	case PLAYER_INFO_3_LIST:
	case PLAYER_INFO_23_LIST:
		break;

	case PLAYER_INFO_00:
		break;

	case OBJECT_FORBID_BE_SELECTED:
		break;

	case HOST_ATTACKRESULT:
		break;
		
	case PLAYER_HP_STEAL:
		break;		
		
	case MULTI_EXP_INFO:
		break;

	case CHANGE_MULTI_EXP_STATE:
		break;

	case SECURITY_PASSWD_CHECKED:
		break;
		
	case ERROR_MESSAGE:
		{
			cmd_error_msg* pCmd = (cmd_error_msg*)pBuf;
			a_LogOutput(1, "Error: %s, User:%s", m_pSession->GetErrorMessage(pCmd->iMessage).c_str(), m_pSession->GetUserName());
		}
		break;

	case HOST_ATTACKED:
		break;

	case HOST_OBTAIN_ITEM:
	case PRODUCE_ONCE:
	case TASK_DELIVER_ITEM:
		break;

	case OBJECT_START_PLAY_ACTION:
		break;

	case OBJECT_STOP_PLAY_ACTION:
		break;

	case RECEIVE_EXP:
		break;

	case LEVEL_UP:
		break;

	case GOBLIN_LEVEL_UP:
		break;
	
	case PLAYER_PROPERTY:
		break;

	case OWN_ITEM_INFO:
	case EMPTY_ITEM_SLOT:
		break;

	case OWN_IVTR_DATA:
	case CHANGE_IVTR_SIZE:
		break;

	case EXG_IVTR_ITEM:
	case MOVE_IVTR_ITEM:
	case EXG_EQUIP_ITEM:
	case MOVE_EQUIP_ITEM:
	case UNFREEZE_IVTR_SLOT:
//	case PLAYER_EQUIP_FASHION_ITEM:
		break;

	case OBJECT_INVISIBLE:
		break;

	case PLAYER_EXT_PROP_BASE:
	case PLAYER_EXT_PROP_ATK:
	case PLAYER_EXT_PROP_DEF:
		break;

	case ADD_STATUS_POINT:
		break;

	case TEAM_LEADER_INVITE:
		break;

	case TEAM_REJECT_INVITE:
		break;

	case TEAM_JOIN_TEAM:
		break;

	case TEAM_MEMBER_LEAVE:
		break;

	case TEAM_LEADER_CACEL_PARTY:
		
		break;

	case TEAM_LEAVE_PARTY:
		break;

	case TEAM_NEW_MEMBER:
		break;
		
	case TEAM_MEMBER_DATA:
		break;

	case TEAM_MEMBER_POS:
		break;

	case TEAM_ASK_TO_JOIN:
		break;

	case EQUIP_DATA:
	case EQUIP_DATA_CHANGED:
		break;

	case EQUIP_DAMAGED:
		break;
		
	case TEAM_MEMBER_PICKUP:
		break;

	case NPC_GREETING:
		break;

	case REPAIR_ALL:
	case REPAIR:
		break;

	case PURCHASE_ITEM:
		break;

	case ITEM_TO_MONEY:
		break;

	case SPEND_MONEY:
		break;

	case GAIN_MONEY_IN_TRADE:
	case GAIN_ITEM_IN_TRADE:
	case GAIN_MONEY_AFTER_TRADE:
	case GAIN_ITEM_AFTER_TRADE:
		break;

	case HOST_USE_ITEM:
		break;

	case PLAYER_USE_ITEM_WITH_ARG:
		break;

	case EMBED_ITEM:
		break;

	case CLEAR_TESSERA:
		break;

	case OBJECT_TAKEOFF:
		break;

	case OBJECT_LANDING:
		break;

	case HOST_RUSH_FLY:
		break;

	case FLYSWORD_TIME:
		break;

	case OBJECT_CAST_INSTANT_SKILL:
	case OBJECT_CAST_POS_SKILL:
		break;

	case SKILL_INTERRUPTED:
		break;

	case SELF_SKILL_INTERRUPTED:
	case SKILL_PERFORM:
		break;

	case PLAYER_CAST_RUNE_SKILL:
	case PLAYER_CAST_RUNE_INSTANT_SKILL:
		break;

	case NPC_SERVICE_CONTENT:
	case RENEW:
		break;

	case PRODUCE_START:
	case PRODUCE_END:
	case PRODUCE_NULL:
	case PRODUCE4_ITEM_INFO:
		break;

	case DECOMPOSE_START:
	case DECOMPOSE_END:
		break;


	case OBJECT_START_USE:
	case OBJECT_CANCEL_USE:
	case OBJECT_USE_ITEM:
	case OBJECT_START_USE_T:
		break;

	case OBJECT_USE_ITEM_WITH_ARG:
		break;

	case OBJECT_SIT_DOWN:
	case OBJECT_STAND_UP:
		break;

	case OBJECT_DO_EMOTE:
	case OBJECT_EMOTE_RESTORE:
		break;

	case SERVER_TIME:
		break;

	case OBJECT_ROOT:
		break;

	case HOST_NOTIFY_ROOT:	
	case HOST_DISPEL_ROOT:
		break;

	case INVADER_RISE:
	case PARIAH_RISE:
	case INVADER_FADE:
		break;

	case OBJECT_ATTACK_RESULT:
		break;

	case HURT_RESULT:
		break;

	case UPDATE_EXT_STATE:
		break;

	case ICON_STATE_NOTIFY:
		break;

	case PLAYER_GATHER_START:
	case PLAYER_GATHER_STOP:
		break;

	case TRASHBOX_PWD_CHANGED:
	case TRASHBOX_OPEN:
	case TRASHBOX_CLOSE:
	case TRASHBOX_WEALTH:
	case EXG_TRASH_MONEY:
	case EXG_TRASHBOX_ITEM:
	case MOVE_TRASHBOX_ITEM:
	case EXG_TRASHBOX_IVTR:
	case IVTR_ITEM_TO_TRASH:
	case TRASH_ITEM_TO_IVTR:
	case TRASHBOX_SIZE:
		break;

	case OBJECT_DO_ACTION:
		break;

	case HOST_SKILL_ATTACK_RESULT:
		break;
		
	case OBJECT_SKILL_ATTACK_RESULT:
		break;
	case HOST_SKILL_ATTACKED:
		break;

	case PLAYER_SET_ADV_DATA:
	case PLAYER_CLR_ADV_DATA:
		break;
		
	case PLAYER_IN_TEAM:
		break;

	case CON_EMOTE_REQUEST:
		break;

	case DO_CONCURRENT_EMOTE:
		break;

	case MAFIA_INFO_NOTIFY:
	case MAFIA_TRADE_START:
	case MAFIA_TRADE_END:
		break;

	case TASK_DELIVER_REP:
	case TASK_DELIVER_EXP:
	case TASK_DELIVER_MONEY:
		break;

	case TASK_DELIVER_LEVEL2:
		break;

	case HOST_REPUTATION:
		break;

	case ITEM_IDENTIFY:
		break;

	case PLAYER_CHGSHAPE:
		break;

	case ENTER_SANCTUARY:
	case LEAVE_SANCTUARY:
		break;

	case SELF_OPEN_BOOTH:
	case PLAYER_OPEN_BOOTH:
	case PLAYER_CLOSE_BOOTH:
	case PLAYER_BOOTH_INFO:
	case BOOTH_TRADE_SUCCESS:
	case BOOTH_NAME:
	case OPEN_BOOTH_TEST:
	case BOOTH_SELL_ITEM:
		break;

	case PLAYER_START_TRAVEL:
	case HOST_START_TRAVEL:
	case PLAYER_END_TRAVEL:
		break;

	case GM_INVINCIBLE:
	case GM_INVISIBLE:
		break;

	case PLAYER_CHANGE_SPOUSE:
		break;

	case ACTIVATE_WAYPOINT:
	case WAYPOINT_LIST:
		break;

	case TEAM_INVITE_TIMEOUT:
		break;

	case PLAYER_ENABLE_PVP:
	case PLAYER_DISABLE_PVP:
	case HOST_PVP_COOLDOWN:
	case HOST_ENABLE_FREEPVP:
	case PVP_COMBAT:
		break;

	case SKILL_ABILITY:
		break;

	case BREATH_DATA:
	case HOST_STOP_DIVE:
		break;

	case INVALID_OBJECT:
		break;

	case REVIVAL_INQUIRE:
		break;

	case SET_COOLDOWN:
		break;

	case CHANGE_TEAM_LEADER:
		break;

	case EXIT_INSTANCE:
		break;

	case CHANGE_FACE_START:
	case CHANGE_FACE_END:
		break;

	case PLAYER_CHG_FACE:
		break;

	case INST_DATA_CHECKOUT:
		break;

	case MALL_ITEM_PRICE:
	case MALL_ITEM_BUY_FAILED:
		break;
		

	case DIVIDEND_MALL_ITEM_PRICE:
	case DIVIDEND_MALL_ITEM_BUY_FAILED:
		break;
		
	case DOUBLE_EXP_TIME:
	case AVAILABLE_DOUBLE_EXP_TIME:
		break;
		
	case COMMON_DATA_NOTIFY:
	case COMMON_DATA_LIST:
		break;

	case PUBLIC_QUEST_INFO:
	case PUBLIC_QUEST_RANKS:
		break;

	case WORLD_LIFE_TIME:
		break;

	case CALC_NETWORK_DELAY_RE:
		break;

	case PLAYER_SPEC_ITEM_LIST:
		break;

	case PLAYER_KNOCKBACK:
		break;

	case WEDDING_BOOK_LIST:
		break;

	case WEDDING_BOOK_SUCCESS:
		break;

	case DUEL_RECV_REQUEST:
	case DUEL_REJECT_REQUEST:
	case DUEL_PREPARE:
	case DUEL_CANCEL:
	case HOST_DUEL_START:
	case DUEL_STOP:
	case PLAYER_DUEL_START:
		break;

	case DUEL_RESULT:
		break;

	case PLAYER_MOUNTING:
		break;

	case PLAYER_EQUIP_DETAIL:
		break;

	case PARIAH_TIME:
		break;

	case GAIN_PET:
	case FREE_PET:
	case SUMMON_PET:
	case RECALL_PET:
	case PLAYER_START_PET_OP:
	case PLAYER_STOP_PET_OP:
	case PET_RECEIVE_EXP:
	case PET_LEVELUP:
	case PET_ROOM_CAPACITY:
	case PET_HONOR_POINT:
	case PET_HUNGER_GAUGE:
	case PET_DEAD:
	case PET_REVIVE:
	case PET_HP_NOTIFY:
	case PET_AI_STATE:
	case PET_SET_COOLDOWN:
	case SUMMON_PLANT_PET:
	case PLANT_PET_DISAPPEAR:
	case PLANT_PET_HP_NOTIFY:
	case PET_PROPERTY:
		break;

	case HOST_ENTER_BATTLE:
	case BATTLE_RESULT:
	case BATTLE_SCORE:
		break;

	case TANK_LEADER_NOTIFY:
		break;

	case REFINE_RESULT:
		break;

	case PLAYER_CASH:
		break;

 	case PLAYER_DIVIDEND:
		break;

	case PLAYER_BIND_SUCCESS:
		break;

	case PVP_MODE:
		break;

	case PLAYER_WALLOW_INFO:
		break;

	case NOTIFY_SAFE_LOCK:
		break;

	case ELF_VIGOR:
	case ELF_ENHANCE:
	case ELF_STAMINA:
	case ELF_EXP:
		break;

	case ELF_REFINE_ACTIVATE:
	case CAST_ELF_SKILL:
	case ELF_CMD_RESULT:
		break;

	case FACTION_CONTRIB_NOTIFY:
		break;

	case FACTION_FORTRESS_INFO:
		break;

	case ENTER_FACTIONFORTRESS:
		break;

	case FACTION_RELATION_NOTIFY:
		break;

	case PLAYER_EQUIP_DISABLED:
		break;

	case CONGREGATE_REQUEST:
	case REJECT_CONGREGATE:
	case CONGREGATE_START:
	case CANCEL_CONGREGATE:
		break;		

	case ENGRAVE_START:
	case ENGRAVE_END:
	case ENGRAVE_RESULT:
		break;

	case DPS_DPH_RANK:
		break;

	case ADDONREGEN_START:
	case ADDONREGEN_END:
	case ADDONREGEN_RESULT:
		break;

	case INVISIBLE_OBJ_LIST:
		break;

	case SET_PLAYER_LIMIT:
		break;

	case PLAYER_TELEPORT:
		break;

	case PLAYER_FORCE_DATA:
	case PLAYER_FORCE_DATA_UPDATE:
	case FORCE_GLOBAL_DATA:
		break;

	case PLAYER_FORCE_CHANGED:
		break;

	case PLAYER_INVENTORY_DETAIL:
		break;
	case ADD_MULTIOBJECT_EFFECT:
	case REMOVE_MULTIOBJECT_EFFECT:
		break;
	case ENTER_WEDDING_SCENE:
		break;

	case ONLINE_AWARD_DATA:
	case TOGGLE_ONLINE_AWARD:
		break;

	case PLAYER_PROFIT_TIME:
		break;

	case ENTER_NONPENALTY_PVP_STATE:
		break;

	case SELF_COUNTRY_NOTIFY:
		break;

	case PLAYER_COUNTRY_CHANGED:
		break;

	case ENTER_COUNTRYBATTLE:
		break;
		
	case COUNTRYBATTLE_RESULT:
		break;
		
	case COUNTRYBATTLE_SCORE:
		
	case COUNTRYBATTLE_RESURRECT_REST_TIMES:
		break;
		
	case COUNTRYBATTLE_FLAG_CARRIER_NOTIFY:
		break;
		
	case COUNTRYBATTLE_BECAME_FLAG_CARRIER:
		break;

	case COUNTRYBATTLE_PERSONAL_SCORE:
		break;

	case COUNTRYBATTLE_FLAG_MSG_NOTIFY:
		break;

	default:
		a_LogOutput(1, "Unknown S2C command: %d received! (Session: %s)", cmd, m_pSession->GetUserName());
		break;
	}
}