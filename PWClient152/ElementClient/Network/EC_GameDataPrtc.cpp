/*
 * FILE: EC_GameDataPrtc.cpp
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Duyuxin, 2004/12/22
 *
 * HISTORY: 
 *
 * Copyright (c) 2004 Archosaur Studio, All Rights Reserved.
 */

#include <AFile.h>

#include "EC_Global.h"
#include "EC_GameSession.h"
#include "EC_Game.h"
#include "EC_GPDataType.h"
#include "EC_GameRun.h"
#include "EC_RTDebug.h"
#include "EC_MsgDef.h"
#include "EC_Manager.h"
#include "EC_HostPlayer.h"
#include "EC_Utility.h"
#include "EC_Faction.h"
#include "EC_FixedMsg.h"
#include "EC_UIManager.h"
#include "EC_GameUIMan.h"
#include "EC_OfflineShopCtrl.h"
#include "EC_AutoPolicy.h"
#include "EC_PlayerWrapper.h"
#include "EC_UIHelper.h"
#include "EC_HostSkillModel.h"

#include "gnet.h"

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

DWORD l_totalCmdSize = 0;

///////////////////////////////////////////////////////////////////////////
//	
//	Local functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Implement CECGameSession
//	
///////////////////////////////////////////////////////////////////////////

/*	Calculate S2C command data size

	Return values:
	
		(DWORD)(-1): means iCmd is a unknown command.
		(DWORD)(-2): command size is hard to calculate, so we ignore it.
*/
DWORD CECGameSession::CalcS2CCmdDataSize(int iCmd, BYTE* pDataBuf, DWORD dwDataSize)
{
	using namespace S2C;

#define CHECK_VALID(type) \
{\
	if (!reinterpret_cast<type*>(pDataBuf)->CheckValid(dwDataSize, sz) || dwDataSize != sz) {\
		assert(0);}\
	else\
		dwSize = (DWORD)(-2);\
}

	DWORD dwSize = (DWORD)(-1);
	size_t sz;

	switch (iCmd)
	{
	case PLAYER_INFO_1:
	case PLAYER_ENTER_WORLD:
	case PLAYER_ENTER_SLICE:

		CHECK_VALID(info_player_1)
		break;

	case PLAYER_INFO_1_LIST:

		CHECK_VALID(cmd_player_info_1_list)
		break;

	case NPC_INFO_LIST:

		CHECK_VALID(cmd_npc_info_list)
		break;

	case PLAYER_INFO_2:				break;
	case PLAYER_INFO_3:				break;
	case PLAYER_INFO_4:				break;
	case PLAYER_INFO_2_LIST:		break;
	case PLAYER_INFO_3_LIST:		break;
	case PLAYER_INFO_23_LIST:		break;

	case SELF_INFO_1:

		CHECK_VALID(cmd_self_info_1)
		break;

	case SELF_INFO_00:				dwSize = sizeof (cmd_self_info_00);			break;

	case MATTER_INFO_LIST:
		CHECK_VALID(cmd_matter_info_list);
		break;

	case NPC_ENTER_WORLD:
	case NPC_ENTER_SLICE:

		CHECK_VALID(info_npc)
		break;

	case OBJECT_LEAVE_SLICE:		dwSize = sizeof (cmd_leave_slice);			break;
	case NOTIFY_HOSTPOS:			dwSize = sizeof (cmd_notify_hostpos);		break;
	case OBJECT_MOVE:				dwSize = sizeof (cmd_object_move);			break;
	case MATTER_ENTER_WORLD:		dwSize = sizeof (cmd_matter_enter_world);	break;
	case PLAYER_LEAVE_WORLD:		dwSize = sizeof (cmd_player_leave_world);	break;
	case NPC_DIED:					dwSize = sizeof (cmd_npc_died);				break;
	case OBJECT_DISAPPEAR:			dwSize = sizeof (cmd_object_disappear);		break;
	case HOST_STOPATTACK:			dwSize = sizeof (cmd_host_stop_attack);		break;
	case HOST_ATTACKRESULT:			dwSize = sizeof (cmd_host_attack_result);	break;
	case ERROR_MESSAGE:				dwSize = sizeof (cmd_error_msg);			break;
	case HOST_ATTACKED:				dwSize = sizeof (cmd_host_attacked);		break;
	case PLAYER_DIED:				dwSize = sizeof (cmd_player_died);			break;
	case HOST_DIED:					dwSize = sizeof (cmd_host_died);			break;
	case PLAYER_REVIVE:				dwSize = sizeof (cmd_player_revive);		break;
	case PICKUP_MONEY:				dwSize = sizeof (cmd_pickup_money);			break;
	case PICKUP_ITEM:				dwSize = sizeof (cmd_pickup_item);			break;
	case PLAYER_INFO_00:			dwSize = sizeof (cmd_player_info_00);		break;
	case NPC_INFO_00:				dwSize = sizeof (cmd_npc_info_00);			break;

	case OUT_OF_SIGHT_LIST:

		CHECK_VALID(cmd_out_of_sight_list);
		break;

	case OBJECT_STOP_MOVE:			dwSize = sizeof (cmd_object_stop_move);		break;
	case RECEIVE_EXP:				dwSize = sizeof (cmd_receive_exp);			break;
	case LEVEL_UP:					dwSize = sizeof (cmd_level_up);				break;
	case UNSELECT:					dwSize = 0;									break;
	case OWN_ITEM_INFO:

		CHECK_VALID(cmd_own_item_info)
		break;

	case EMPTY_ITEM_SLOT:			dwSize = sizeof (cmd_empty_item_slot);		break;
	case OWN_IVTR_DATA:

		CHECK_VALID(cmd_own_ivtr_info)
		break;

	case OWN_IVTR_DETAIL_DATA:

		CHECK_VALID(cmd_own_ivtr_detail_info)
		break;

	case EXG_IVTR_ITEM:				dwSize = sizeof (cmd_exg_ivtr_item);		break;
	case MOVE_IVTR_ITEM:			dwSize = sizeof (cmd_move_ivtr_item);		break;
	case PLAYER_DROP_ITEM:			dwSize = sizeof (cmd_player_drop_item);		break;
	case EXG_EQUIP_ITEM:			dwSize = sizeof (cmd_exg_equip_item);		break;
	case EQUIP_ITEM:				dwSize = sizeof (cmd_equip_item);			break;
	case MOVE_EQUIP_ITEM:			dwSize = sizeof (cmd_move_equip_item);		break;
	case OWN_EXT_PROP:				dwSize = sizeof (cmd_own_ext_prop);			break;
	case ADD_STATUS_POINT:			dwSize = sizeof (cmd_add_status_point);		break;
	case SELECT_TARGET:				dwSize = sizeof (cmd_select_target);		break;
	case PLAYER_EXT_PROP_BASE:		dwSize = sizeof (cmd_pep_base);				break;
	case PLAYER_EXT_PROP_MOVE:		dwSize = sizeof (cmd_pep_move);				break;
	case PLAYER_EXT_PROP_ATK:		dwSize = sizeof (cmd_pep_attack);			break;
	case PLAYER_EXT_PROP_DEF:		dwSize = sizeof (cmd_pep_def);				break;
	case TEAM_LEADER_INVITE:		dwSize = sizeof (cmd_team_leader_invite);	break;
	case TEAM_REJECT_INVITE:		dwSize = sizeof (cmd_team_reject_invite);	break;
	case TEAM_JOIN_TEAM:			dwSize = sizeof (cmd_team_join_team);		break;
	case TEAM_MEMBER_LEAVE:			dwSize = sizeof (cmd_team_member_leave);	break;
	case TEAM_LEAVE_PARTY:			dwSize = sizeof (cmd_team_leave_party);		break;
	case TEAM_NEW_MEMBER:			dwSize = sizeof (cmd_team_new_member);		break;
	case TEAM_LEADER_CACEL_PARTY:	dwSize = sizeof (cmd_team_leader_cancel_party);	break;

	case TEAM_MEMBER_DATA:

		CHECK_VALID(cmd_team_member_data);
		break;

	case TEAM_MEMBER_POS:			dwSize = sizeof (cmd_team_member_pos);		break;
	case EQUIP_DATA:

		CHECK_VALID(cmd_equip_data);
		break;

	case EQUIP_DATA_CHANGED:

		CHECK_VALID(cmd_equip_data_changed);
		break;

	case EQUIP_DAMAGED:				dwSize = sizeof (cmd_equip_damaged);		break;
	case TEAM_MEMBER_PICKUP:		dwSize = sizeof (cmd_team_member_pickup);	break;
	case NPC_GREETING:				dwSize = sizeof (cmd_npc_greeting);			break;

	case NPC_SERVICE_CONTENT:

		CHECK_VALID(cmd_npc_sev_content)
		break;

	case PURCHASE_ITEM:

		CHECK_VALID(cmd_purchase_item)
		break;

	case ITEM_TO_MONEY:				dwSize = sizeof (cmd_item_to_money);		break;
	case REPAIR_ALL:				dwSize = sizeof (cmd_repair_all);			break;
	case REPAIR:					dwSize = sizeof (cmd_repair);				break;

	case RENEW:

		break;

	case SPEND_MONEY:				dwSize = sizeof (cmd_spend_money);			break;
	case GAIN_MONEY_IN_TRADE:		dwSize = sizeof (cmd_gain_money_in_trade);	break;
	case GAIN_ITEM_IN_TRADE:		dwSize = sizeof (cmd_gain_item_in_trade);	break;
	case GAIN_MONEY_AFTER_TRADE:	dwSize = sizeof (cmd_gain_money_after_trade);	break;
	case GAIN_ITEM_AFTER_TRADE:		dwSize = sizeof (cmd_gain_item_after_trade);	break;
	case GET_OWN_MONEY:				dwSize = sizeof (cmd_get_own_money);		break;
	case ATTACK_ONCE:				dwSize = sizeof (cmd_attack_once);			break;
	case HOST_START_ATTACK:			dwSize = sizeof (cmd_host_start_attack);	break;
	case OBJECT_CAST_SKILL:			dwSize = sizeof (cmd_object_cast_skill);	break;
	case PLAYER_CAST_RUNE_SKILL:	dwSize = sizeof (cmd_player_cast_rune_skill);	break;
	case PLAYER_CAST_RUNE_INSTANT_SKILL:	dwSize = sizeof (cmd_player_cast_rune_instant_skill);	break;
	case SKILL_INTERRUPTED:			dwSize = sizeof (cmd_skill_interrupted);	break;
	case SKILL_PERFORM:				dwSize = 0;									break;
	case SELF_SKILL_INTERRUPTED:	dwSize = sizeof (cmd_self_skill_interrupted);	break;

	case SKILL_DATA:

		CHECK_VALID(cmd_skill_data)
		break;

	case HOST_USE_ITEM:				dwSize = sizeof (cmd_host_use_item);		break;
	case PLAYER_USE_ITEM_WITH_ARG:	dwSize = (DWORD)-2;							break;
	case EMBED_ITEM:				dwSize = sizeof (cmd_embed_item);			break;
	case CLEAR_TESSERA:				dwSize = sizeof (cmd_clear_tessera);		break;
	case COST_SKILL_POINT:			dwSize = sizeof (cmd_cost_skill_point);		break;
	case LEARN_SKILL:				dwSize = sizeof (cmd_learn_skill);			break;
	case OBJECT_TAKEOFF:			dwSize = sizeof (cmd_object_takeoff);		break;
	case OBJECT_LANDING:			dwSize = sizeof (cmd_object_landing);		break;
	case FLYSWORD_TIME:				dwSize = sizeof (cmd_flysword_time);		break;
	case HOST_OBTAIN_ITEM:			dwSize = sizeof (cmd_host_obtain_item);		break;
	case PRODUCE_START:				dwSize = sizeof (cmd_produce_start);		break;
	case PRODUCE_ONCE:				dwSize = sizeof (cmd_produce_once);			break;
	case PRODUCE_END:				dwSize = 0;									break;
	case DECOMPOSE_START:			dwSize = sizeof (cmd_decompose_start);		break;
	case DECOMPOSE_END:				dwSize = 0;									break;
	case TASK_DATA:

		CHECK_VALID(cmd_task_data)
		break;

	case TASK_VAR_DATA:

		CHECK_VALID(cmd_task_var_data)
		break;

	case OBJECT_START_USE:			dwSize = sizeof (cmd_object_start_use);		break;
	case OBJECT_CANCEL_USE:			dwSize = sizeof (cmd_object_cancel_use);	break;
	case OBJECT_USE_ITEM:			dwSize = sizeof (cmd_object_use_item);		break;
	case OBJECT_START_USE_T:		dwSize = sizeof (cmd_object_start_use_t);	break;
	case OBJECT_USE_ITEM_WITH_ARG:	dwSize = (DWORD)-2;							break;
	case OBJECT_SIT_DOWN:			dwSize = sizeof (cmd_object_sit_down);		break;
	case OBJECT_STAND_UP:			dwSize = sizeof (cmd_object_stand_up);		break;
	case OBJECT_DO_EMOTE:			dwSize = sizeof (cmd_object_do_emote);		break;
	case SERVER_TIME:				dwSize = sizeof (cmd_server_time);			break;
	case OBJECT_ROOT:				dwSize = sizeof (cmd_object_root);			break;
	case HOST_DISPEL_ROOT:			dwSize = sizeof (cmd_host_dispel_root);		break;
	case INVADER_RISE:				dwSize = sizeof (cmd_invader_rise);			break;
	case PARIAH_RISE:				dwSize = sizeof (cmd_pariah_rise);			break;
	case INVADER_FADE:				dwSize = sizeof (cmd_invader_fade);			break;
	case OBJECT_ATTACK_RESULT:		dwSize = sizeof (cmd_object_atk_result);	break;
	case BE_HURT:					dwSize = sizeof (cmd_be_hurt);				break;
	case HURT_RESULT:				dwSize = sizeof (cmd_hurt_result);			break;
	case HOST_STOP_SKILL:			dwSize = 0;									break;
	case UPDATE_EXT_STATE:			dwSize = sizeof (cmd_update_ext_state);		break;

	case ICON_STATE_NOTIFY:
		{
			cmd_icon_state_notify cmd;
			if (!cmd.Initialize(pDataBuf, dwDataSize, &dwSize))
				dwSize = (DWORD)-1;
			break;
		}
		break;

	case PLAYER_GATHER_START:		dwSize = sizeof (cmd_player_gather_start);	break;
	case PLAYER_GATHER_STOP:		dwSize = sizeof (cmd_player_gather_stop);	break;
	case TRASHBOX_PWD_CHANGED:		dwSize = sizeof (cmd_trashbox_pwd_changed);	break;
	case TRASHBOX_PWD_STATE:		dwSize = sizeof (cmd_trashbox_pwd_state);	break;
	case TRASHBOX_OPEN:				dwSize = sizeof (cmd_trashbox_open);		break;
	case TRASHBOX_CLOSE:			dwSize = sizeof(cmd_trashbox_close);		break;
	case TRASHBOX_WEALTH:			dwSize = sizeof (cmd_trashbox_wealth);		break;
	case EXG_TRASHBOX_ITEM:			dwSize = sizeof (cmd_exg_trashbox_item);	break;	
	case MOVE_TRASHBOX_ITEM:		dwSize = sizeof (cmd_move_trashbox_item);	break;
	case EXG_TRASHBOX_IVTR:			dwSize = sizeof (cmd_exg_trashbox_ivtr);	break;
	case IVTR_ITEM_TO_TRASH:		dwSize = sizeof (cmd_trash_item_to_ivtr);	break;
	case TRASH_ITEM_TO_IVTR:		dwSize = sizeof (cmd_ivty_item_to_trash);	break;
	case EXG_TRASH_MONEY:			dwSize = sizeof (cmd_exg_trash_money);		break;
	case ENCHANT_RESULT:			dwSize = sizeof (cmd_enchant_result);		break;
	case HOST_NOTIFY_ROOT:			dwSize = sizeof (cmd_host_notify_root);		break;
	case OBJECT_DO_ACTION:			dwSize = sizeof (cmd_obj_do_action);		break;
	case HOST_SKILL_ATTACK_RESULT:	dwSize = sizeof (cmd_host_skill_attack_result); break;
	case OBJECT_SKILL_ATTACK_RESULT:dwSize = sizeof (cmd_object_skill_attack_result); break;
	case HOST_SKILL_ATTACKED:		dwSize = sizeof (cmd_host_skill_attacked);	break;
	case PLAYER_SET_ADV_DATA:		dwSize = sizeof (cmd_player_set_adv_data);	break;
	case PLAYER_CLR_ADV_DATA:		dwSize = sizeof (cmd_player_clr_adv_data);	break;
	case PLAYER_IN_TEAM:			dwSize = sizeof (cmd_player_in_team);		break;
	case TEAM_ASK_TO_JOIN:			dwSize = sizeof (cmd_team_ask_join);		break;
	case OBJECT_EMOTE_RESTORE:		dwSize = sizeof (cmd_object_emote_restore);	break;
	case CON_EMOTE_REQUEST:			dwSize = sizeof (cmd_con_emote_request);	break;
	case DO_CONCURRENT_EMOTE:		dwSize = sizeof (cmd_do_concurrent_emote);	break;
	case MATTER_PICKUP:				dwSize = sizeof (cmd_matter_pickup);		break;
	case MAFIA_INFO_NOTIFY:			dwSize = sizeof (cmd_mafia_info_notify);	break;
	case MAFIA_TRADE_START:			dwSize = 0;									break;
	case MAFIA_TRADE_END:			dwSize = 0;									break;
	case TASK_DELIVER_ITEM:			dwSize = sizeof (cmd_task_deliver_item);	break;
	case TASK_DELIVER_REP:			dwSize = sizeof (cmd_task_deliver_rep);		break;
	case TASK_DELIVER_EXP:			dwSize = sizeof (cmd_task_deliver_exp);		break;
	case TASK_DELIVER_MONEY:		dwSize = sizeof (cmd_task_deliver_money);	break;
	case TASK_DELIVER_LEVEL2:		dwSize = sizeof (cmd_task_deliver_level2);	break;
	case HOST_REPUTATION:			dwSize = sizeof (cmd_host_reputation);		break;
	case ITEM_IDENTIFY:				dwSize = sizeof (cmd_item_identify);		break;
	case PLAYER_CHGSHAPE:			dwSize = sizeof (cmd_player_chgshape);		break;
	case ELF_REFINE_ACTIVATE:		dwSize = sizeof (cmd_elf_refine_activate);	break;
	case CAST_ELF_SKILL:			dwSize = sizeof (cmd_cast_elf_skill);		break;
	case ENTER_SANCTUARY:			dwSize = sizeof (cmd_object_enter_sanctuary); break;
	case LEAVE_SANCTUARY:			dwSize = sizeof (cmd_object_leave_sanctuary); break;

	case PLAYER_OPEN_BOOTH:

		CHECK_VALID(cmd_player_open_booth)
		break;

	case SELF_OPEN_BOOTH:

		CHECK_VALID(cmd_self_open_booth)
		break;

	case PLAYER_CLOSE_BOOTH:		dwSize = sizeof (cmd_player_close_booth);	break;
	case PLAYER_BOOTH_INFO:

		CHECK_VALID(cmd_player_booth_info)
		break;

	case BOOTH_TRADE_SUCCESS:		dwSize = sizeof (cmd_booth_trade_success);	break;

	case BOOTH_NAME:

		CHECK_VALID(cmd_booth_name)
		break;

	case PLAYER_START_TRAVEL:		dwSize = sizeof (cmd_player_start_travel);	break;
	case HOST_START_TRAVEL:			dwSize = sizeof (cmd_host_start_travel);	break;
	case PLAYER_END_TRAVEL:			dwSize = sizeof (cmd_player_end_travel);	break;
	case PLAYER_CHANGE_SPOUSE:		dwSize = sizeof (player_change_spouse);		break;
	case GM_INVINCIBLE:				dwSize = sizeof (gm_cmd_invincible);		break;
	case GM_INVISIBLE:				dwSize = sizeof (gm_cmd_invisible);			break;
	case HOST_CORRECT_POS:			dwSize = sizeof (cmd_host_correct_pos);		break;
	case OBJECT_CAST_INSTANT_SKILL:	dwSize = sizeof (cmd_object_cast_instant_skill); break;
	case ACTIVATE_WAYPOINT:			dwSize = sizeof (cmd_activate_waypoint);	break;

	case WAYPOINT_LIST:

		CHECK_VALID(cmd_waypoint_list)
		break;

	case UNFREEZE_IVTR_SLOT:		dwSize = sizeof (cmd_unfreeze_ivtr_slot);	break;
	case TEAM_INVITE_TIMEOUT:		dwSize = sizeof (cmd_team_invite_timeout);	break;
	case PLAYER_ENABLE_PVP:			dwSize = sizeof (cmd_player_enable_pvp);	break;
	case PLAYER_DISABLE_PVP:		dwSize = sizeof (cmd_player_disable_pvp);	break;
	case HOST_PVP_COOLDOWN:			dwSize = sizeof (cmd_host_pvp_cooldown);	break;

	case COOLTIME_DATA:

		CHECK_VALID(cmd_cooltime_data)
		break;

	case SKILL_ABILITY:				dwSize = sizeof (cmd_skill_ability);		break;
	case OPEN_BOOTH_TEST:			dwSize = 0;									break;
	case BREATH_DATA:				dwSize = sizeof (cmd_breath_data);			break;
	case HOST_STOP_DIVE:			dwSize = 0;									break;
	case BOOTH_SELL_ITEM:			dwSize = sizeof (cmd_booth_sell_item);		break;
	case PLAYER_ENABLE_FASHION:		dwSize = sizeof (cmd_player_enable_fashion);break;
	case HOST_ENABLE_FREEPVP:		dwSize = sizeof (cmd_host_enable_freepvp);	break;
	case INVALID_OBJECT:			dwSize = sizeof (cmd_invalid_object);		break;
	case PLAYER_ENABLE_EFFECT:		dwSize = sizeof (cmd_player_enable_effect);	break;
	case PLAYER_DISABLE_EFFECT:		dwSize = sizeof (cmd_player_disable_effect);break;
	case REVIVAL_INQUIRE:			dwSize = sizeof (cmd_revival_inquire);		break;
	case SET_COOLDOWN:				dwSize = sizeof (cmd_set_cooldown);			break;
	case CHANGE_TEAM_LEADER:		dwSize = sizeof (cmd_change_team_leader);	break;
	case EXIT_INSTANCE:				dwSize = sizeof (cmd_exit_instance);		break;
	case CHANGE_FACE_START:			dwSize = sizeof (cmd_change_face_start);	break;
	case CHANGE_FACE_END:			dwSize = sizeof (cmd_change_face_end);		break;
	case PLAYER_CHG_FACE:			dwSize = sizeof (cmd_player_chg_face);		break;
	case OBJECT_CAST_POS_SKILL:		dwSize = sizeof (cmd_object_cast_pos_skill);break;
	case SET_MOVE_STAMP:			dwSize = sizeof (cmd_set_move_stamp);		break;
	case INST_DATA_CHECKOUT:		dwSize = sizeof (cmd_inst_data_checkout);	break;
	case HOST_RUSH_FLY:				dwSize = sizeof (cmd_host_rush_fly);		break;
	case TRASHBOX_SIZE:				dwSize = sizeof (cmd_trashbox_size);		break;
	case NPC_DIED2:					dwSize = sizeof (cmd_npc_died2);			break;
	case PRODUCE_NULL:				dwSize = sizeof (cmd_produce_null);			break;
	case PVP_COMBAT:				dwSize = sizeof (cmd_pvp_combat);			break;
	case DOUBLE_EXP_TIME:			dwSize = sizeof (cmd_double_exp_time);		break;
	case AVAILABLE_DOUBLE_EXP_TIME:	dwSize = sizeof (cmd_available_double_exp_time); break;
	case DUEL_RECV_REQUEST:			dwSize = sizeof (cmd_duel_recv_request);	break;
	case DUEL_REJECT_REQUEST:		dwSize = sizeof (cmd_duel_reject_request);	break;
	case DUEL_PREPARE:				dwSize = sizeof (cmd_duel_prepare);			break;
	case DUEL_CANCEL:				dwSize = sizeof (cmd_duel_cancel);			break;
	case HOST_DUEL_START:			dwSize = sizeof (cmd_host_duel_start);		break;
	case DUEL_STOP:					dwSize = sizeof (cmd_duel_stop);			break;
	case DUEL_RESULT:				dwSize = sizeof (cmd_duel_result);			break;
	case PLAYER_BIND_REQUEST:		dwSize = sizeof (cmd_player_bind_request);	break;
	case PLAYER_BIND_INVITE:		dwSize = sizeof (cmd_player_bind_invite);	break;
	case PLAYER_BIND_REQUEST_REPLY:	dwSize = sizeof (cmd_player_bind_request_reply);	break;
	case PLAYER_BIND_INVITE_REPLY:	dwSize = sizeof (cmd_player_bind_invite_reply);		break;
	case PLAYER_BIND_START:			dwSize = sizeof (cmd_player_bind_start);	break;
	case PLAYER_BIND_STOP:			dwSize = sizeof (cmd_player_bind_stop);		break;
	case PLAYER_MOUNTING:			dwSize = sizeof (cmd_player_mounting);		break;
	case PLAYER_EQUIP_DETAIL:		dwSize = (DWORD)(-2);						break;
	case PLAYER_DUEL_START:			dwSize = sizeof (cmd_player_duel_start);	break;
	case PARIAH_TIME:				dwSize = sizeof (cmd_pariah_time);			break;
	case GAIN_PET:					dwSize = sizeof (cmd_gain_pet);				break;
	case FREE_PET:					dwSize = sizeof (cmd_free_pet);				break;
	case SUMMON_PET:				dwSize = sizeof (cmd_summon_pet);			break;
	case RECALL_PET:				dwSize = sizeof (cmd_recall_pet);			break;
	case PLAYER_START_PET_OP:		dwSize = sizeof (cmd_player_start_pet_op);	break;
	case PLAYER_STOP_PET_OP:		dwSize = 0;									break;
	case PET_RECEIVE_EXP:			dwSize = sizeof (cmd_pet_receive_exp);		break;
	case PET_LEVELUP:				dwSize = sizeof (cmd_pet_levelup);			break;

	case PET_ROOM:
	{
		const cmd_pet_room* pCmd = (const cmd_pet_room*)pDataBuf;
		dwSize = sizeof (cmd_pet_room) + pCmd->count * (sizeof (int) + sizeof (info_pet));
		break;
	}
	case PET_ROOM_CAPACITY:			dwSize = sizeof (cmd_pet_room_capacity);	break;
	case PET_HONOR_POINT:			dwSize = sizeof (cmd_pet_honor_point);		break;
	case PET_HUNGER_GAUGE:			dwSize = sizeof (cmd_pet_hunger_gauge);		break;
	case HOST_ENTER_BATTLE:			dwSize = sizeof (cmd_host_enter_battle);	break;
	case TANK_LEADER_NOTIFY:		dwSize = sizeof (cmd_tank_leader_notify);	break;
	case BATTLE_RESULT:				dwSize = sizeof (cmd_battle_result);		break;
	case BATTLE_SCORE:				dwSize = sizeof (cmd_battle_score);			break;
	case PET_DEAD:					dwSize = sizeof (cmd_pet_dead);				break;
	case PET_REVIVE:				dwSize = sizeof (cmd_pet_revive);			break;
	case PET_HP_NOTIFY:				dwSize = sizeof (cmd_pet_hp_notify);		break;
	case PET_AI_STATE:				dwSize = sizeof (cmd_pet_ai_state);			break;
	case REFINE_RESULT:				dwSize = sizeof (cmd_refine_result);		break;
	case PET_SET_COOLDOWN:			dwSize = sizeof (cmd_pet_set_cooldown);		break;
	case PLAYER_CASH:				dwSize = sizeof	(player_cash);				break;
	case PLAYER_DIVIDEND:			dwSize = sizeof (cmd_player_dividend);		break;
	case PLAYER_BIND_SUCCESS:		dwSize = sizeof (cmd_player_bind_success);	break;
	case CHANGE_IVTR_SIZE:			dwSize = sizeof (cmd_change_ivtr_size);		break;
	case PVP_MODE:					dwSize = sizeof (pvp_mode);					break;
	case PLAYER_WALLOW_INFO:		dwSize = sizeof (player_wallow_info);		break;
	case NOTIFY_SAFE_LOCK:			dwSize = sizeof (notify_safe_lock);			break;
	case ELF_VIGOR:					dwSize = sizeof (cmd_elf_vigor);			break;
	case ELF_ENHANCE:				dwSize = sizeof (cmd_elf_enhance);			break;
	case ELF_STAMINA:				dwSize = sizeof (cmd_elf_stamina);			break;
	case ELF_EXP:					dwSize = sizeof (cmd_elf_exp);				break;
	case ELF_CMD_RESULT:			dwSize = sizeof (cmd_elf_result);			break;
	case COMMON_DATA_NOTIFY:		dwSize = (DWORD)(-2);						break;
	case COMMON_DATA_LIST:			dwSize = (DWORD)(-2);						break;
	case MALL_ITEM_PRICE:			dwSize = (DWORD)(-2);						break;
	case MALL_ITEM_BUY_FAILED:		dwSize = sizeof (cmd_mall_item_buy_failed);	break; 
	case GOBLIN_LEVEL_UP:			dwSize = sizeof (cmd_goblin_level_up);		break;
	case PLAYER_PROPERTY:			dwSize = sizeof (cmd_player_property);		break;
	case PLAYER_EQUIP_TRASHBOX_ITEM: dwSize = sizeof (cmd_player_equip_trashbox_item); break;
	case OBJECT_INVISIBLE:			dwSize = sizeof (cmd_object_invisible);		break;
	case SECURITY_PASSWD_CHECKED:	dwSize = 0;									break;
	case PLAYER_HP_STEAL:			dwSize = sizeof (cmd_player_hp_steal);		break;
	case DIVIDEND_MALL_ITEM_PRICE:	dwSize = (DWORD)(-2);						break;
	case DIVIDEND_MALL_ITEM_BUY_FAILED: dwSize = sizeof (cmd_dividend_mall_item_buy_failed); break;
	case PUBLIC_QUEST_INFO:			dwSize = sizeof (cmd_public_quest_info);	break;
	case PUBLIC_QUEST_RANKS:		dwSize = (DWORD)(-2);						break;
	case MULTI_EXP_INFO:            dwSize = sizeof(cmd_multi_exp_info);        break;
	case CHANGE_MULTI_EXP_STATE:    dwSize = sizeof(cmd_change_multi_exp_state);break;
	case WORLD_LIFE_TIME:			dwSize = sizeof(cmd_world_life_time);		break;
	case WEDDING_BOOK_LIST:			dwSize = (DWORD)(-2);						break;
	case WEDDING_BOOK_SUCCESS:		dwSize = sizeof(cmd_wedding_book_success);	break;
	case CALC_NETWORK_DELAY_RE:		dwSize = sizeof(cmd_network_delay_re);		break;
	case PLAYER_KNOCKBACK:			dwSize = sizeof(cmd_player_knockback);		break;
	case SUMMON_PLANT_PET:			dwSize = sizeof(cmd_summon_plant_pet);		break;
	case PLANT_PET_DISAPPEAR:		dwSize = sizeof(cmd_plant_pet_disapper);	break;
	case PLANT_PET_HP_NOTIFY:		dwSize = sizeof(cmd_plant_pet_hp_notify);	break;
	case PET_PROPERTY:				dwSize = sizeof(cmd_pet_property);			break;
	case FACTION_CONTRIB_NOTIFY:	dwSize = sizeof(cmd_faction_contrib_notify);break;
	case FACTION_FORTRESS_INFO:
		{
			cmd_faction_fortress_info cmd;
			if (!cmd.Initialize(pDataBuf, dwDataSize, &dwSize))
				dwSize = (DWORD)-1;
			break;
		}
	case ENTER_FACTIONFORTRESS:		dwSize = sizeof(cmd_enter_factionfortress);	break;
	case FACTION_RELATION_NOTIFY:
		{
			cmd_faction_relation_notify cmd;
			if (!cmd.Initialize(pDataBuf, dwDataSize, &dwSize))
				dwSize = (DWORD)-1;
			break;
		}
	case PLAYER_EQUIP_DISABLED:		dwSize = sizeof(cmd_player_equip_disabled);		break;
	case PLAYER_SPEC_ITEM_LIST:		CHECK_VALID(player_spec_item_list);	break;

	case OBJECT_START_PLAY_ACTION:

		CHECK_VALID(cmd_object_start_play_action);
		break;

	case OBJECT_STOP_PLAY_ACTION:	dwSize = sizeof(cmd_object_stop_play_action);break;

	case CONGREGATE_REQUEST:		dwSize = sizeof(cmd_congregate_request);break;
	case REJECT_CONGREGATE:			dwSize = sizeof(cmd_reject_congregate);break;
	case CONGREGATE_START:			dwSize = sizeof(cmd_congregate_start);break;
	case CANCEL_CONGREGATE:			dwSize = sizeof(cmd_cancel_congregate);break;
	case ENGRAVE_START:					dwSize = sizeof(cmd_engrave_start);	break;
	case ENGRAVE_END:					dwSize = 0;	break;
	case ENGRAVE_RESULT:				dwSize = sizeof(cmd_engrave_result);	break;

	case DPS_DPH_RANK:

		CHECK_VALID(cmd_dps_dph_rank);
		break;

	case ADDONREGEN_START:				dwSize = sizeof(cmd_addonregen_start);	break;
	case ADDONREGEN_END:				dwSize = 0;	break;
	case ADDONREGEN_RESULT:				dwSize = sizeof(cmd_addonregen_result);	break;
	case INVISIBLE_OBJ_LIST:
		CHECK_VALID(cmd_invisible_obj_list);
		break;

	case SET_PLAYER_LIMIT:		dwSize = sizeof(cmd_set_player_limit);	break;
	case PLAYER_TELEPORT:		dwSize = sizeof(cmd_player_teleport);	break;
	case OBJECT_FORBID_BE_SELECTED:	dwSize = sizeof (cmd_object_forbid_be_selected);	break;

	case PLAYER_INVENTORY_DETAIL:
		CHECK_VALID(cmd_player_inventory_detail);
		break;

	case PLAYER_FORCE_DATA:
		CHECK_VALID(cmd_player_force_data);
		break;

	case PLAYER_FORCE_CHANGED:		dwSize = sizeof (cmd_player_force_changed);	break;
	case PLAYER_FORCE_DATA_UPDATE:	dwSize = sizeof (cmd_player_force_data_update);	break;

	case FORCE_GLOBAL_DATA:
		CHECK_VALID(cmd_force_global_data);
		break;
	case ADD_MULTIOBJECT_EFFECT:
	case REMOVE_MULTIOBJECT_EFFECT:
		dwSize = sizeof(cmd_multiobj_effect); 
		break;
	case ENTER_WEDDING_SCENE:
		dwSize = sizeof(enter_wedding_scene);
		break;
	case PRODUCE4_ITEM_INFO:
		CHECK_VALID(produce4_item_info);
		break;
	case ONLINE_AWARD_DATA:
		CHECK_VALID(cmd_online_award_data);
		break;
	case TOGGLE_ONLINE_AWARD:
		dwSize = sizeof(cmd_toggle_online_award);
		break;
	case PLAYER_PROFIT_TIME:		dwSize = sizeof(cmd_player_profit_time); break;
	case ENTER_NONPENALTY_PVP_STATE: dwSize = sizeof(cmd_nonpenalty_pvp_state); break;
	case SELF_COUNTRY_NOTIFY:		dwSize = sizeof(cmd_self_country_notify); break;
	case PLAYER_COUNTRY_CHANGED: dwSize = sizeof(cmd_player_country_changed); break;
	case ENTER_COUNTRYBATTLE:	dwSize = sizeof(cmd_enter_countrybattle); break;
	case COUNTRYBATTLE_RESULT:	dwSize = sizeof(cmd_countrybattle_result); break;
	case COUNTRYBATTLE_SCORE:	dwSize = sizeof(cmd_countrybattle_score); break;
	case COUNTRYBATTLE_RESURRECT_REST_TIMES:
		dwSize = sizeof(cmd_countrybattle_resurrect_rest_times); break;
	case COUNTRYBATTLE_FLAG_CARRIER_NOTIFY:
		dwSize = sizeof(cmd_countrybattle_flag_carrier_notify); break;
	case COUNTRYBATTLE_BECAME_FLAG_CARRIER:
		dwSize = sizeof(cmd_countrybattle_became_flag_carrier); break;
	case COUNTRYBATTLE_PERSONAL_SCORE:
		dwSize = sizeof(cmd_countrybattle_personal_score); break;
	case COUNTRYBATTLE_FLAG_MSG_NOTIFY:
		dwSize = sizeof(cmd_countrybattle_flag_msg_notify); break;
	case DEFENSE_RUNE_ENABLED:
		dwSize = sizeof(cmd_defense_rune_enable); break;
	case COUNTRYBATTLE_INFO:
		dwSize = sizeof(cmd_countrybattle_info); break;
	case SET_PROFIT_TIME:
		dwSize = sizeof(cmd_set_profit_time); break;
	case CASH_MONEY_EXCHG_RATE:	dwSize = sizeof(cmd_cash_money_exchg_rate);	break;
	case PET_REBUILD_INHERIT_START:
		dwSize = sizeof(cmd_pet_rebuild_inherit_start); break;
	case PET_REBUILD_INHERIT_INFO:
		dwSize = sizeof(cmd_pet_rebuild_inherit_info); break;
	case PET_REBUILD_INHERIT_END:
		dwSize = sizeof(cmd_pet_rebuild_inherit_end); break;
	case PET_EVOLUTION_DONE:
		dwSize = sizeof(cmd_pet_evolution_done); break;
	case PET_REBUILD_NATURE_START:
		dwSize = sizeof(cmd_pet_rebuild_nature_start); break;
	case PET_REBUILD_NATURE_INFO:
		dwSize = sizeof(cmd_pet_rebuild_nature_info); break;
	case PET_REBUILD_NATURE_END:
		dwSize = sizeof(cmd_pet_rebuild_nature_end); break;
	case MERIDIANS_NOTIFY:			
		dwSize = sizeof (cmd_notify_meridian_data);		break;
	case MERIDIANS_RESULT:			
		dwSize = sizeof (cmd_try_refine_meridian_result);		break;
	case EQUIP_ADDON_UPDATE_NOTIFY:
		dwSize = sizeof(cmd_equip_addon_update_notify);break;
	case SELF_KING_NOTIFY:
		dwSize = sizeof(cmd_self_king_notify); break;
	case PLAYER_KING_CHANGED:
		dwSize = sizeof(cmd_player_king_changed); break;
	case COUNTRYBATTLE_STRONGHOND_STATE_NOTIFY:
		CHECK_VALID(cmd_stronghold_state_notify);
		break;
	case QUERY_TOUCH_POINT:		dwSize = sizeof(cmd_query_touch_point); break;
	case SPEND_TOUCH_POINT:		dwSize = sizeof(cmd_spend_touch_point); break;
	case TOTAL_RECHARGE:		dwSize = sizeof(cmd_total_recharge); break;
	case QUERY_TITLE_RE:		CHECK_VALID(cmd_query_title_re); break;
	case CHANGE_CURR_TITLE_RE:	dwSize = sizeof(cmd_change_curr_title_re); break;
	case MODIFY_TITLE_NOFIFY:	dwSize = sizeof(cmd_modify_title_notify); break;
	case REFRESH_SIGNIN:		dwSize = sizeof(cmd_refresh_signin); break;
	case PARALLEL_WORLD_INFO:	CHECK_VALID(cmd_parallel_world_info); break;
	case PLAYER_REINCARNATION:	dwSize = sizeof(cmd_player_reincarnation); break;
	case REINCARNATION_TOME_INFO: CHECK_VALID(cmd_reincarnation_tome_info); break;
	case ACTIVATE_REINCARNATION_TOME: dwSize = sizeof(cmd_activate_reincarnation_tome); break;
	case UNIQUE_DATA_NOTIFY:	
		{
			cmd_unique_data_notify cmd;
			if (!cmd.Initialize((const void*)pDataBuf, dwDataSize, &dwSize))
				dwSize = (DWORD)-1;
		}
		break;
	case USE_GIFTCARD_RESULT:	dwSize = sizeof(cmd_use_giftcard_result); break;
	case REALM_EXP:				dwSize = sizeof(cmd_realm_exp); break;
	case REALM_LEVEL:			dwSize = sizeof(cmd_realm_level); break;
	case ENTER_TRICKBATTLE:		dwSize = sizeof(cmd_enter_trickbattle); break;
	case TRICKBATTLE_CHARIOT_INFO: dwSize = sizeof(cmd_trickbattle_chariot_info) ;break;
	case TRICKBATTLE_PERSONAL_SCORE: dwSize = sizeof(cmd_trickbattle_personal_score); break;
	case PLAYER_LEADERSHIP:		dwSize = sizeof(cmd_player_leadership); break;
	case GENERALCARD_COLLECTION_DATA: CHECK_VALID(cmd_generalcard_collection_data); break;
	case ADD_GENERALCARD_COLLECTION: dwSize = sizeof(cmd_add_generalcard_collection); break;
	case REFRESH_MONSTERSPIRIT_LEVEL: CHECK_VALID(cmd_refresh_monsterspirit_level); break;
	case MINE_GATHERED: dwSize = sizeof(cmd_mine_gathered); break;
	case PLAYER_IN_OUT_BATTLE: dwSize = sizeof(cmd_player_in_out_battle); break;
	case PLAYER_QUERY_CHARIOTS: CHECK_VALID(cmd_player_query_chariots); break;
	case COUNTRYBATTLE_LIVE_SHOW_RESULT: 
		{
			cmd_countrybattle_live_show_result cmd;
			if (!cmd.Initialize(pDataBuf, dwDataSize, &dwSize))
				dwSize = (DWORD)-1;
		}
		break;
	case RANDOM_MALL_SHOPPING_RES:	dwSize = sizeof(cmd_rand_mall_shopping_res); break;
	case FACTION_PVP_MASK_MODIFY:	dwSize = sizeof(cmd_faction_pvp_mask_modify); break;
	case PLAYER_WORLD_CONTRIBUTION: dwSize = sizeof(cmd_player_world_contribution); break;
	case SCENE_SERVICE_NPC_LIST: CHECK_VALID(cmd_scene_service_npc_list); break;
	case RANDOM_MAP_ORDER: CHECK_VALID(cmd_random_map_order);break;
	case NPC_VISIBLE_TID_NOTIFY: dwSize = sizeof(cmd_npc_visible_tid_notify); break;
	case CLIENT_SCREEN_EFFECT: dwSize = sizeof(cmd_client_screen_effect);break;
	case EQUIP_CAN_INHERIT_ADDONS: CHECK_VALID(cmd_equip_can_inherit_addons);break;
	case COMBO_SKILL_PREPARE: dwSize = sizeof(cmd_combo_skill_prepare); break;
	case INSTANCE_REENTER_NOTIFY:	dwSize = sizeof(cmd_instance_reenter_notify); break;
	case PRAY_DISTANCE_CHANGE: dwSize = sizeof(cmd_pray_distance_change); break;
	default:
		break;
	}

	return dwSize;
}

//	Game data handler
void CECGameSession::ProcessGameData(const GNET::Octets& Data)
{
	using namespace S2C;

	DWORD dwDataSize = Data.size();

	if (dwDataSize < sizeof(cmd_header))
	{
		assert(0);
		return;
	}

	dwDataSize -= sizeof(cmd_header);

	//	Get common command header
	BYTE* pDataBuf = (BYTE*)Data.begin();
	cmd_header* pCmdHeader = (cmd_header*)pDataBuf;
	pDataBuf += sizeof (cmd_header);

	// test....
/*
	static int xx = 0;

	if (++xx > 100)
	{
		switch (pCmdHeader->cmd)
		{
		case PLAYER_INFO_1:
		case PLAYER_ENTER_WORLD:
		case PLAYER_ENTER_SLICE:
		case SELF_INFO_1:
		case SELF_INFO_00:
		case PLAYER_INFO_1_LIST:
		case NOTIFY_HOSTPOS:
		case HOST_CORRECT_POS:
		case OBJECT_MOVE:
		case HOST_STOPATTACK:
		case OBJECT_STOP_MOVE:
		case PLAYER_EXT_PROP_MOVE:
			break;
		default:{
			BYTE* pXXX = pDataBuf;
			for (DWORD x = 0; x < dwDataSize; x++)
			*pXXX++ = (rand() % 256);
			}
		}
	}
	*/

	//	Check command size
	DWORD dwCmdSize = CalcS2CCmdDataSize(pCmdHeader->cmd, pDataBuf, dwDataSize);

	// log the command and data
	// LogS2CCmdData(pCmdHeader->cmd, (BYTE *)Data.begin(), Data.size());

	//	Size == -1 means we haven't handled this command
	if (dwCmdSize == (DWORD)(-1))
	{
		g_pGame->RuntimeDebugInfo(RTDCOL_WARNING, _AL("SERVER - Unknown %s"), AS2AC(g_pGame->GetRTDebug()->GetGamedataSendName(pCmdHeader->cmd, true)));
		return;
	}
	else if (dwCmdSize != (DWORD)(-2))
	{
		ASSERT(dwCmdSize == dwDataSize);
		if (dwCmdSize != dwDataSize)
		{
			g_pGame->RuntimeDebugInfo(RTDCOL_ERROR, _AL("SERVER - Invalid %s size(Network:%d, Client:%d)"), AS2AC(g_pGame->GetRTDebug()->GetGamedataSendName(pCmdHeader->cmd, true)), dwDataSize, dwCmdSize);
			return;
		}
	}

#ifdef LOG_PROTOCOL
	if( !g_pGame->GetRTDebug()->IsGameDataHide(pCmdHeader->cmd, true) )
		g_pGame->RuntimeDebugInfo(RTDCOL_NETWORK, _AL("SERVER - %s"), AS2AC(g_pGame->GetRTDebug()->GetGamedataSendName(pCmdHeader->cmd, true)));
#endif

	CECGameRun* pGameRun = g_pGame->GetGameRun();

	switch (pCmdHeader->cmd)
	{
	case PLAYER_INFO_2:
	case PLAYER_INFO_3:
	case PLAYER_INFO_4:
	case PLAYER_INFO_2_LIST:
	case PLAYER_INFO_3_LIST:
	case PLAYER_INFO_23_LIST:

		break;

	case PLAYER_INFO_1:
	case PLAYER_ENTER_WORLD:
	case PLAYER_ENTER_SLICE:
	case PLAYER_INFO_1_LIST:
	case PLAYER_INFO_00:
		
		pGameRun->PostMessage(MSG_PM_PLAYERINFO, MAN_PLAYER, -1, (DWORD)pDataBuf, pCmdHeader->cmd, dwDataSize);
		break;

	case SELF_INFO_1:

		DoOvertimeCheck(false, OT_ENTERGAME, 0);
		pGameRun->PostMessage(MSG_PM_PLAYERINFO, MAN_PLAYER, -1, (DWORD)pDataBuf, pCmdHeader->cmd, dwDataSize);
		break;

	case SELF_INFO_00:

		pGameRun->PostMessage(MSG_HST_INFO00, MAN_PLAYER, 0, (DWORD)pDataBuf, pCmdHeader->cmd);
		break;

	case NPC_ENTER_SLICE:
	case NPC_INFO_LIST:
	case NPC_INFO_00:
	case NPC_ENTER_WORLD:
	case NPC_VISIBLE_TID_NOTIFY:
		pGameRun->PostMessage(MSG_NM_NPCINFO, MAN_NPC, 0, (DWORD)pDataBuf, pCmdHeader->cmd, dwDataSize);
		break;
	
	case MATTER_INFO_LIST:
	
		pGameRun->PostMessage(MSG_MM_MATTERINFO, MAN_MATTER, 0, (DWORD)pDataBuf, pCmdHeader->cmd);
		break;
	
	case NOTIFY_HOSTPOS:

		pGameRun->PostMessage(MSG_HST_GOTO, MAN_PLAYER, 0, (DWORD)pDataBuf, pCmdHeader->cmd);
		break;

	case OBJECT_MOVE:
	{
		cmd_object_move* pCmd = (cmd_object_move*)pDataBuf;
		if (ISPLAYERID(pCmd->id))
			pGameRun->PostMessage(MSG_PM_PLAYERMOVE, MAN_PLAYER, -1, (DWORD)pDataBuf, pCmdHeader->cmd);
		else if (ISNPCID(pCmd->id))
			pGameRun->PostMessage(MSG_NM_NPCMOVE, MAN_NPC, 0, (DWORD)pDataBuf, pCmdHeader->cmd);

		break;
	}

	case OBJECT_FORBID_BE_SELECTED:
	{
		cmd_object_forbid_be_selected* pCmd = (cmd_object_forbid_be_selected*)pDataBuf;
		if (ISPLAYERID(pCmd->id))
			pGameRun->PostMessage(MSG_PM_FORBIDBESELECTED, MAN_PLAYER, -1, (DWORD)pDataBuf, pCmdHeader->cmd);
		else if (ISNPCID(pCmd->id))
			pGameRun->PostMessage(MSG_NM_FORBIDBESELECTED, MAN_NPC, 0, (DWORD)pDataBuf, pCmdHeader->cmd);

		break;
	}

	case OBJECT_LEAVE_SLICE:
	{
		cmd_object_move* pCmd = (cmd_object_move*)pDataBuf;
		if (ISPLAYERID(pCmd->id))
			pGameRun->PostMessage(MSG_PM_PLAYERRUNOUT, MAN_PLAYER, -1, (int)pDataBuf, pCmdHeader->cmd);
		else if (ISNPCID(pCmd->id))
			pGameRun->PostMessage(MSG_NM_NPCRUNOUT, MAN_NPC, 0, (DWORD)pDataBuf, pCmdHeader->cmd);

		break;
	}

	case MATTER_ENTER_WORLD:
		
		pGameRun->PostMessage(MSG_MM_MATTERENTWORLD, MAN_MATTER, 0, (DWORD)pDataBuf, pCmdHeader->cmd);
		break;

	case PLAYER_LEAVE_WORLD:
	{
		cmd_player_leave_world* pCmd = (cmd_player_leave_world*)pDataBuf;
		if (ISPLAYERID(pCmd->id))
			pGameRun->PostMessage(MSG_PM_PLAYEREXIT, MAN_PLAYER, -1, (int)pDataBuf, pCmdHeader->cmd);

		break;
	}
	case NPC_DIED:
	case NPC_DIED2:

		pGameRun->PostMessage(MSG_NM_NPCDIED, MAN_NPC, 0, (DWORD)pDataBuf, pCmdHeader->cmd);
		break;
	
	case OBJECT_DISAPPEAR:
	{
		cmd_object_disappear* pCmd = (cmd_object_disappear*)pDataBuf;
		if (ISPLAYERID(pCmd->id))
			pGameRun->PostMessage(MSG_PM_PLAYERDISAPPEAR, MAN_PLAYER, -1, (int)pDataBuf, pCmdHeader->cmd);
		else if (ISNPCID(pCmd->id))
			pGameRun->PostMessage(MSG_NM_NPCDISAPPEAR, MAN_NPC, 0, (DWORD)pDataBuf, pCmdHeader->cmd);
		else if (ISMATTERID(pCmd->id))
			pGameRun->PostMessage(MSG_MM_MATTERDISAPPEAR, MAN_MATTER, 0, (DWORD)pDataBuf, pCmdHeader->cmd);

		break;
	}
	case HOST_START_ATTACK:

		pGameRun->PostMessage(MSG_HST_STARTATTACK, MAN_PLAYER, 0, (DWORD)pDataBuf, pCmdHeader->cmd);
		break;

	case HOST_STOPATTACK:

		pGameRun->PostMessage(MSG_HST_STOPATTACK, MAN_PLAYER, 0, (DWORD)pDataBuf, pCmdHeader->cmd);
		break;

	case HOST_ATTACKRESULT:
		
		pGameRun->PostMessage(MSG_HST_ATKRESULT, MAN_PLAYER, 0, (DWORD)pDataBuf, pCmdHeader->cmd);
		break;
		
	case PLAYER_HP_STEAL:

		pGameRun->PostMessage(MSG_HST_HPSTEAL, MAN_PLAYER, 0, (DWORD)pDataBuf, pCmdHeader->cmd);
		break;		
		
	case MULTI_EXP_INFO:
		
		pGameRun->PostMessage(MSG_HST_MULTI_EXP_INFO, MAN_PLAYER, 0, (DWORD)pDataBuf, pCmdHeader->cmd);
		break;

	case CHANGE_MULTI_EXP_STATE:
		
		pGameRun->PostMessage(MSG_HST_MULTI_EXP_STATE, MAN_PLAYER, 0, (DWORD)pDataBuf, pCmdHeader->cmd);
		break;

	case SECURITY_PASSWD_CHECKED:

		pGameRun->PostMessage(MSG_HST_PASSWDCHECKED, MAN_PLAYER, 0, (DWORD)pDataBuf, pCmdHeader->cmd);
		break;
		
	case ERROR_MESSAGE:
	{
		cmd_error_msg* pCmd = (cmd_error_msg*)pDataBuf;
		if (pCmd->iMessage)
		{
			const wchar_t* szMsg = m_ErrorMsgs.GetWideString(pCmd->iMessage);
			if (!szMsg)
				g_pGame->RuntimeDebugInfo(RTDCOL_WARNING, _AL("SERVER - unknown error !"));
			else if (pCmd->iMessage != 2)
				g_pGame->GetGameRun()->AddChatMessage(szMsg, GP_CHAT_MISC);
		}

		if (pCmd->iMessage == 2)
		{
			//	Attack target is too far
			pGameRun->PostMessage(MSG_HST_TARGETISFAR, MAN_PLAYER, 0, (DWORD)pDataBuf, pCmdHeader->cmd);
		}
		else if (pCmd->iMessage == 20)
		{
			//	Failed to cast skill
			pGameRun->PostMessage(MSG_PM_CASTSKILL, MAN_PLAYER, 0, (DWORD)pDataBuf, pCmdHeader->cmd);
		}
		else if (pCmd->iMessage == 133 || pCmd->iMessage == 134)
		{
			//  deal failed 
			pGameRun->PostMessage(MSG_HST_BUY_SELL_FAIL, MAN_PLAYER, 0, (DWORD)pDataBuf, pCmdHeader->cmd);			
		}
		else if (pCmd->iMessage == 158)
		{
			// 当前汇率不对，重新取汇率
			c2s_CmdGetCashMoneyRate();
		}
		else if (pCmd->iMessage == 108 && pGameRun->GetHostPlayer()->IsInKingService())
		{
			CECGameUIMan* pGameUI = pGameRun->GetUIManager()->GetInGameUIMan();
			if( pGameUI )
				pGameUI->EndNPCService();
		}
		else if (pCmd->iMessage == 108 && pGameRun->GetHostPlayer()->GetOfflineShopCtrl()->GetNPCSevFlag()!= COfflineShopCtrl::NPCSEV_NULL)
		{
			CECGameUIMan* pGameUI = pGameRun->GetUIManager()->GetInGameUIMan();
			if( pGameUI )
				pGameUI->EndNPCService();
		}else if (pCmd->iMessage == 175){
			c2s_CmdQueryParallelWorld();
		}
		else if (pCmd->iMessage == 6)
		{
			AP_ActionEvent(AP_EVENT_CANNOTPICKUP);
		}

		break;
	}
	case HOST_ATTACKED:

		pGameRun->PostMessage(MSG_HST_ATTACKED, MAN_PLAYER, 0, (DWORD)pDataBuf, pCmdHeader->cmd);
		break;
		
	case PLAYER_DIED:

		pGameRun->PostMessage(MSG_PM_PLAYERDIED, MAN_PLAYER, -1, (DWORD)pDataBuf, pCmdHeader->cmd);
		break;

	case HOST_DIED:

		pGameRun->PostMessage(MSG_HST_DIED, MAN_PLAYER, 0, (DWORD)pDataBuf, pCmdHeader->cmd);
		break;

	case PLAYER_REVIVE:

		pGameRun->PostMessage(MSG_PM_PLAYERREVIVE, MAN_PLAYER, -1, (DWORD)pDataBuf, pCmdHeader->cmd);
		break;

	case PICKUP_MONEY:

		pGameRun->PostMessage(MSG_HST_PICKUPMONEY, MAN_PLAYER, 0, (DWORD)pDataBuf, pCmdHeader->cmd);
		break;

	case PICKUP_ITEM:
	case HOST_OBTAIN_ITEM:
	case PRODUCE_ONCE:
	case TASK_DELIVER_ITEM:

		pGameRun->PostMessage(MSG_HST_PICKUPITEM, MAN_PLAYER, 0, (DWORD)pDataBuf, pCmdHeader->cmd);
		break;

	case OUT_OF_SIGHT_LIST:
	{
		cmd_out_of_sight_list* pCmd = (cmd_out_of_sight_list*)pDataBuf;

		for (DWORD n=0; n < pCmd->uCount; n++)
		{
			if (ISPLAYERID(pCmd->idList[n]))
				pGameRun->PostMessage(MSG_PM_PLAYEROUTOFVIEW, MAN_PLAYER, -1, pCmd->idList[n], pCmdHeader->cmd);
			else if (ISNPCID(pCmd->idList[n]))
				pGameRun->PostMessage(MSG_NM_NPCOUTOFVIEW, MAN_NPC, 0, pCmd->idList[n], pCmdHeader->cmd);
			else if (ISMATTERID(pCmd->idList[n]))
				pGameRun->PostMessage(MSG_MM_MATTEROUTOFVIEW, MAN_MATTER, 0, pCmd->idList[n], pCmdHeader->cmd);
		}

		break;
	}
	case OBJECT_STOP_MOVE:
	{
		cmd_object_stop_move* pCmd = (cmd_object_stop_move*)pDataBuf;
		if (ISPLAYERID(pCmd->id))
			pGameRun->PostMessage(MSG_PM_PLAYERSTOPMOVE, MAN_PLAYER, -1, (DWORD)pDataBuf, pCmdHeader->cmd);
		else if (ISNPCID(pCmd->id))
			pGameRun->PostMessage(MSG_NM_NPCSTOPMOVE, MAN_NPC, 0, (DWORD)pDataBuf, pCmdHeader->cmd);
	
		break;
	}

	case OBJECT_START_PLAY_ACTION:
		{
			cmd_object_start_play_action *pCmd = (cmd_object_start_play_action *)pDataBuf;
			if (ISNPCID(pCmd->id))
				pGameRun->PostMessage(MSG_NM_NPCSTARTPLAYACTION, MAN_NPC, 0, (DWORD)pDataBuf, pCmdHeader->cmd);
			break;
		}

	case OBJECT_STOP_PLAY_ACTION:
		{
			cmd_object_stop_play_action *pCmd = (cmd_object_stop_play_action *)pDataBuf;
			if (ISNPCID(pCmd->id))
				pGameRun->PostMessage(MSG_NM_NPCSTOPPLAYACTION, MAN_NPC, 0, (DWORD)pDataBuf, pCmdHeader->cmd);
			break;
		}

	case RECEIVE_EXP:

		pGameRun->PostMessage(MSG_HST_RECEIVEEXP, MAN_PLAYER, 0, (DWORD)pDataBuf, pCmdHeader->cmd);
		break;

	case LEVEL_UP:
	{
		cmd_level_up* pCmd = (cmd_level_up*)pDataBuf;
		if (ISPLAYERID(pCmd->id))
			pGameRun->PostMessage(MSG_PM_PLAYERLEVELUP, MAN_PLAYER, -1, (DWORD)pDataBuf, pCmdHeader->cmd);
		else if (ISNPCID(pCmd->id))
			pGameRun->PostMessage(MSG_NM_NPCLEVELUP, MAN_NPC, 0, (DWORD)pDataBuf, pCmdHeader->cmd);

		break;
	}

	case GOBLIN_LEVEL_UP:
	{
		cmd_goblin_level_up* pCmd = (cmd_goblin_level_up*)pDataBuf;
		if(ISPLAYERID(pCmd->player_id))
			pGameRun->PostMessage(MSG_PM_GOBLINLEVELUP, MAN_PLAYER, -1, (DWORD)pDataBuf, pCmdHeader->cmd);

		break;
	}
	
	case PLAYER_PROPERTY:
	{
		cmd_player_property* pCmd = (cmd_player_property*)pDataBuf;
		if(ISPLAYERID(pCmd->id))
			pGameRun->PostMessage(MSG_HST_PLAYERPROPERTY, MAN_PLAYER, 0, (DWORD)pDataBuf, pCmdHeader->cmd, dwDataSize);
		
		break;
	}
	case SELECT_TARGET:
	case UNSELECT:

		pGameRun->PostMessage(MSG_HST_SELTARGET, MAN_PLAYER, 0, (DWORD)pDataBuf, pCmdHeader->cmd);
		break;

	case OWN_ITEM_INFO:
	case EMPTY_ITEM_SLOT:

		pGameRun->PostMessage(MSG_HST_OWNITEMINFO, MAN_PLAYER, 0, (DWORD)pDataBuf, pCmdHeader->cmd, dwDataSize);
		break;

	case OWN_IVTR_DATA:
	case OWN_IVTR_DETAIL_DATA:
	case GET_OWN_MONEY:
	case CHANGE_IVTR_SIZE:

		pGameRun->PostMessage(MSG_HST_IVTRINFO, MAN_PLAYER, 0, (DWORD)pDataBuf, pCmdHeader->cmd, dwDataSize);
		break;

	case EXG_IVTR_ITEM:
	case MOVE_IVTR_ITEM:
	case PLAYER_DROP_ITEM:
	case EXG_EQUIP_ITEM:
	case EQUIP_ITEM:
	case MOVE_EQUIP_ITEM:
	case UNFREEZE_IVTR_SLOT:
	case PLAYER_EQUIP_TRASHBOX_ITEM:

		pGameRun->PostMessage(MSG_HST_ITEMOPERATION, MAN_PLAYER, 0, (DWORD)pDataBuf, pCmdHeader->cmd);
		break;

	case OBJECT_INVISIBLE:
	{
		cmd_object_invisible* pCmd = (cmd_object_invisible*)pDataBuf;
		if (ISPLAYERID(pCmd->id))
			pGameRun->PostMessage(MSG_PM_PLAYERINVISIBLE, MAN_PLAYER, -1, (DWORD)pDataBuf, pCmdHeader->cmd);
		else if (ISNPCID(pCmd->id))
			pGameRun->PostMessage(MSG_NM_NPCINVISIBLE, MAN_NPC, 0, (DWORD)pDataBuf, pCmdHeader->cmd);

		break;
	}

	case OWN_EXT_PROP:

		pGameRun->PostMessage(MSG_HST_OWNEXTPROP, MAN_PLAYER, 0, (DWORD)pDataBuf, pCmdHeader->cmd);
		break;

	case PLAYER_EXT_PROP_BASE:
	case PLAYER_EXT_PROP_MOVE:
	case PLAYER_EXT_PROP_ATK:
	case PLAYER_EXT_PROP_DEF:

		pGameRun->PostMessage(MSG_PM_PLAYEREXTPROP, MAN_PLAYER, -1, (DWORD)pDataBuf, pCmdHeader->cmd);
		break;
	case MERIDIANS_NOTIFY:

		pGameRun->PostMessage(MSG_HST_MERIDIANS_NOTIFY, MAN_PLAYER, 0, (DWORD)pDataBuf, pCmdHeader->cmd);
		break;
	case MERIDIANS_RESULT:
		
		pGameRun->PostMessage(MSG_HST_MERIDIANS_RESULT, MAN_PLAYER, 0, (DWORD)pDataBuf, pCmdHeader->cmd);
		break;

	case ADD_STATUS_POINT:

		pGameRun->PostMessage(MSG_HST_ADDSTATUSPT, MAN_PLAYER, 0, (DWORD)pDataBuf, pCmdHeader->cmd);
		break;

	case TEAM_LEADER_INVITE:

		pGameRun->PostMessage(MSG_HST_TEAMINVITE, MAN_PLAYER, 0, (DWORD)pDataBuf, pCmdHeader->cmd);
		break;

	case TEAM_REJECT_INVITE:

		pGameRun->PostMessage(MSG_HST_TEAMREJECT, MAN_PLAYER, 0, (DWORD)pDataBuf, pCmdHeader->cmd);
		break;

	case TEAM_JOIN_TEAM:

		pGameRun->PostMessage(MSG_HST_JOINTEAM, MAN_PLAYER, 0, (DWORD)pDataBuf, pCmdHeader->cmd);
		break;

	case TEAM_MEMBER_LEAVE:

		pGameRun->PostMessage(MSG_PM_LEAVETEAM, MAN_PLAYER, -1, (DWORD)pDataBuf, pCmdHeader->cmd);
		break;

	case TEAM_LEADER_CACEL_PARTY:
		
		break;

	case TEAM_LEAVE_PARTY:

		pGameRun->PostMessage(MSG_HST_LEAVETEAM, MAN_PLAYER, 0, (DWORD)pDataBuf, pCmdHeader->cmd);
		break;

	case TEAM_NEW_MEMBER:

		pGameRun->PostMessage(MSG_HST_NEWTEAMMEM, MAN_PLAYER, 0, (DWORD)pDataBuf, pCmdHeader->cmd);
		break;
		
	case TEAM_MEMBER_DATA:

		pGameRun->PostMessage(MSG_HST_TEAMMEMBERDATA, MAN_PLAYER, 0, (DWORD)pDataBuf, pCmdHeader->cmd);
		break;

	case TEAM_MEMBER_POS:

		pGameRun->PostMessage(MSG_HST_TEAMMEMPOS, MAN_PLAYER, 0, (DWORD)pDataBuf, pCmdHeader->cmd);
		break;

	case TEAM_ASK_TO_JOIN:
		
		pGameRun->PostMessage(MSG_HST_ASKTOJOINTEAM, MAN_PLAYER, 0, (DWORD)pDataBuf, pCmdHeader->cmd);
		break;

	case EQUIP_DATA:
	case EQUIP_DATA_CHANGED:

		pGameRun->PostMessage(MSG_PM_PLAYEREQUIPDATA, MAN_PLAYER, -1, (DWORD)pDataBuf, pCmdHeader->cmd, dwDataSize);
		break;

	case EQUIP_DAMAGED:

		pGameRun->PostMessage(MSG_HST_EQUIPDAMAGED, MAN_PLAYER, 0, (DWORD)pDataBuf, pCmdHeader->cmd);
		break;
		
	case TEAM_MEMBER_PICKUP:

		pGameRun->PostMessage(MSG_HST_TEAMMEMPICKUP, MAN_PLAYER, 0, (DWORD)pDataBuf, pCmdHeader->cmd);
		break;

	case NPC_GREETING:

		pGameRun->PostMessage(MSG_HST_NPCGREETING, MAN_PLAYER, 0, (DWORD)pDataBuf, pCmdHeader->cmd);
		break;

	case REPAIR_ALL:
	case REPAIR:

		pGameRun->PostMessage(MSG_HST_REPAIR, MAN_PLAYER, 0, (DWORD)pDataBuf, pCmdHeader->cmd);
		break;

	case PURCHASE_ITEM:

		pGameRun->PostMessage(MSG_HST_PURCHASEITEMS, MAN_PLAYER, 0, (DWORD)pDataBuf, pCmdHeader->cmd);
		break;

	case ITEM_TO_MONEY:

		pGameRun->PostMessage(MSG_HST_ITEMTOMONEY, MAN_PLAYER, 0, (DWORD)pDataBuf, pCmdHeader->cmd);
		break;

	case SPEND_MONEY:

		pGameRun->PostMessage(MSG_HST_SPENDMONEY, MAN_PLAYER, 0, (DWORD)pDataBuf, pCmdHeader->cmd);
		break;

	case GAIN_MONEY_IN_TRADE:
	case GAIN_ITEM_IN_TRADE:
	case GAIN_MONEY_AFTER_TRADE:
	case GAIN_ITEM_AFTER_TRADE:

		pGameRun->PostMessage(MSG_HST_GAINITEM, MAN_PLAYER, 0, (DWORD)pDataBuf, pCmdHeader->cmd);
		break;

	case HOST_USE_ITEM:

		pGameRun->PostMessage(MSG_HST_USEITEM, MAN_PLAYER, 0, (DWORD)pDataBuf, pCmdHeader->cmd);
		break;

	case PLAYER_USE_ITEM_WITH_ARG:

		pGameRun->PostMessage(MSG_HST_USEITEMWITHDATA, MAN_PLAYER, 0, (DWORD)pDataBuf, pCmdHeader->cmd);
		break;

	case SKILL_DATA:

		pGameRun->PostMessage(MSG_HST_SKILLDATA, MAN_PLAYER, 0, (DWORD)pDataBuf, pCmdHeader->cmd);
		break;

	case EMBED_ITEM:

		pGameRun->PostMessage(MSG_HST_EMBEDITEM, MAN_PLAYER, 0, (DWORD)pDataBuf, pCmdHeader->cmd);
		break;

	case CLEAR_TESSERA:

		pGameRun->PostMessage(MSG_HST_CLEARTESSERA, MAN_PLAYER, 0, (DWORD)pDataBuf, pCmdHeader->cmd);
		break;

	case COST_SKILL_POINT:

		pGameRun->PostMessage(MSG_HST_COSTSKILLPT, MAN_PLAYER, 0, (DWORD)pDataBuf, pCmdHeader->cmd);
		break;

	case LEARN_SKILL:
		
		pGameRun->PostMessage(MSG_HST_LEARNSKILL, MAN_PLAYER, 0, (DWORD)pDataBuf, pCmdHeader->cmd);
		break;

	case OBJECT_TAKEOFF:
	{
		cmd_object_takeoff* pCmd = (cmd_object_takeoff*)pDataBuf;
		if (ISPLAYERID(pCmd->object_id))
			pGameRun->PostMessage(MSG_PM_PLAYERFLY, MAN_PLAYER, -1, (DWORD)pDataBuf, pCmdHeader->cmd);
		
		break;
	}
	case OBJECT_LANDING:
	{
		cmd_object_landing* pCmd = (cmd_object_landing*)pDataBuf;
		if (ISPLAYERID(pCmd->object_id))
			pGameRun->PostMessage(MSG_PM_PLAYERFLY, MAN_PLAYER, -1, (DWORD)pDataBuf, pCmdHeader->cmd);
		
		break;
	}
	case HOST_RUSH_FLY:
		
		pGameRun->PostMessage(MSG_PM_PLAYERFLY, MAN_PLAYER, 0, (DWORD)pDataBuf, pCmdHeader->cmd);
		break;

	case FLYSWORD_TIME:
	
		pGameRun->PostMessage(MSG_HST_FLYSWORDTIME, MAN_PLAYER, 0, (DWORD)pDataBuf, pCmdHeader->cmd);
		break;

	case OBJECT_CAST_SKILL:
	case OBJECT_CAST_INSTANT_SKILL:
	case OBJECT_CAST_POS_SKILL:
	{
		cmd_object_cast_skill* pCmd = (cmd_object_cast_skill*)pDataBuf;
		if (ISPLAYERID(pCmd->caster))
			pGameRun->PostMessage(MSG_PM_CASTSKILL, MAN_PLAYER, -1, (DWORD)pDataBuf, pCmdHeader->cmd);
		else if (ISNPCID(pCmd->caster))
			pGameRun->PostMessage(MSG_NM_NPCCASTSKILL, MAN_NPC, 0, (DWORD)pDataBuf, pCmdHeader->cmd);

		break;
	}
	case SKILL_INTERRUPTED:
	{
		cmd_skill_interrupted* pCmd = (cmd_skill_interrupted*)pDataBuf;
		if (ISPLAYERID(pCmd->caster))
			pGameRun->PostMessage(MSG_PM_CASTSKILL, MAN_PLAYER, -1, (DWORD)pDataBuf, pCmdHeader->cmd);
		else if (ISNPCID(pCmd->caster))
			pGameRun->PostMessage(MSG_NM_NPCCASTSKILL, MAN_NPC, 0, (DWORD)pDataBuf, pCmdHeader->cmd);

		break;
	}
	case HOST_STOP_SKILL:
	case SELF_SKILL_INTERRUPTED:
	case SKILL_PERFORM:

		pGameRun->PostMessage(MSG_PM_CASTSKILL, MAN_PLAYER, 0, (DWORD)pDataBuf, pCmdHeader->cmd);
		break;

	case PLAYER_CAST_RUNE_SKILL:
	case PLAYER_CAST_RUNE_INSTANT_SKILL:

		pGameRun->PostMessage(MSG_PM_CASTSKILL, MAN_PLAYER, -1, (DWORD)pDataBuf, pCmdHeader->cmd);
		break;

	case NPC_SERVICE_CONTENT:
	case RENEW:

		break;

	case PRODUCE_START:
	case PRODUCE_END:
	case PRODUCE_NULL:
	case PRODUCE4_ITEM_INFO:

		pGameRun->PostMessage(MSG_HST_PRODUCEITEM, MAN_PLAYER, 0, (DWORD)pDataBuf, pCmdHeader->cmd);
		break;

	case DECOMPOSE_START:
	case DECOMPOSE_END:

		pGameRun->PostMessage(MSG_HST_BREAKITEM, MAN_PLAYER, 0, (DWORD)pDataBuf, pCmdHeader->cmd);
		break;

	case TASK_DATA:
	case TASK_VAR_DATA:

		pGameRun->PostMessage(MSG_HST_TASKDATA, MAN_PLAYER, 0, (DWORD)pDataBuf, pCmdHeader->cmd);
		break;

	case OBJECT_START_USE:
	case OBJECT_CANCEL_USE:
	case OBJECT_USE_ITEM:
	case OBJECT_START_USE_T:
		
		pGameRun->PostMessage(MSG_PM_PLAYERUSEITEM, MAN_PLAYER, -1, (DWORD)pDataBuf, pCmdHeader->cmd);
		break;

	case OBJECT_USE_ITEM_WITH_ARG:

		pGameRun->PostMessage(MSG_PM_USEITEMWITHDATA, MAN_PLAYER, -1, (DWORD)pDataBuf, pCmdHeader->cmd);
		break;

	case OBJECT_SIT_DOWN:
	case OBJECT_STAND_UP:

		pGameRun->PostMessage(MSG_PM_PLAYERSITDOWN, MAN_PLAYER, -1, (DWORD)pDataBuf, pCmdHeader->cmd);
		break;

	case OBJECT_DO_EMOTE:
	case OBJECT_EMOTE_RESTORE:

		pGameRun->PostMessage(MSG_PM_PLAYERDOEMOTE, MAN_PLAYER, -1, (DWORD)pDataBuf, pCmdHeader->cmd);
		break;

	case SERVER_TIME:
	{
		cmd_server_time* pCmd = (cmd_server_time*)pDataBuf;
		pGameRun->PostMessage(MSG_SERVERTIME, -1, 0, pCmd->time, pCmd->timebias);

		AFileImage luaFile;
		if( !luaFile.Open("interfaces\\script\\config\\global_api.lua", AFILE_OPENEXIST | AFILE_BINARY | AFILE_TEMPMEMORY) )
		{
			g_dwFatalErrorFlag = FATAL_ERROR_WRONG_CONFIGDATA;
			break;
		}

		char szLine[1024];
		DWORD dwRead;
		luaFile.ReadLine(szLine, 1024, &dwRead);
		int version;
		sscanf(szLine, "--%d", &version);

		if( version != pCmd->lua_version )
		{
			g_dwFatalErrorFlag = FATAL_ERROR_WRONG_CONFIGDATA;	
		}

		luaFile.Close();
		break;
	}
	case OBJECT_ROOT:
	{
		cmd_object_root* pCmd = (cmd_object_root*)pDataBuf;
		if (ISPLAYERID(pCmd->id))
			pGameRun->PostMessage(MSG_PM_PLAYERROOT, MAN_PLAYER, -1, (DWORD)pDataBuf, pCmdHeader->cmd);
		else if (ISNPCID(pCmd->id))
			pGameRun->PostMessage(MSG_NM_NPCROOT, MAN_NPC, 0, (DWORD)pDataBuf, pCmdHeader->cmd);

		break;
	}
	case HOST_NOTIFY_ROOT:	
	case HOST_DISPEL_ROOT:

		pGameRun->PostMessage(MSG_HST_ROOTNOTIFY, MAN_PLAYER, 0, (DWORD)pDataBuf, pCmdHeader->cmd);
		break;

	case INVADER_RISE:
	case PARIAH_RISE:
	case INVADER_FADE:

		pGameRun->PostMessage(MSG_PM_CHANGENAMECOLOR, MAN_PLAYER, -1, (DWORD)pDataBuf, pCmdHeader->cmd);
		break;

	case OBJECT_ATTACK_RESULT:
	{
		cmd_object_atk_result* pCmd = (cmd_object_atk_result*)pDataBuf;
		if (ISPLAYERID(pCmd->attacker_id))
			pGameRun->PostMessage(MSG_PM_PLAYERATKRESULT, MAN_PLAYER, -1, (DWORD)pDataBuf, pCmdHeader->cmd);
		else if (ISNPCID(pCmd->attacker_id))
			pGameRun->PostMessage(MSG_NM_NPCATKRESULT, MAN_NPC, 0, (DWORD)pDataBuf, pCmdHeader->cmd);

		break;
	}
	case ENCHANT_RESULT:
	{
		cmd_enchant_result* pCmd = (cmd_enchant_result*)pDataBuf;
		if (ISPLAYERID(pCmd->caster))
			pGameRun->PostMessage(MSG_PM_ENCHANTRESULT, MAN_PLAYER, -1, (DWORD)pDataBuf, pCmdHeader->cmd);
		else if (ISNPCID(pCmd->caster))
			pGameRun->PostMessage(MSG_NM_ENCHANTRESULT, MAN_NPC, 0, (DWORD)pDataBuf, pCmdHeader->cmd);

		break;
	}
	case BE_HURT:
	case HURT_RESULT:

		pGameRun->PostMessage(MSG_HST_HURTRESULT, MAN_PLAYER, 0, (DWORD)pDataBuf, pCmdHeader->cmd);
		break;

	case UPDATE_EXT_STATE:
	{
		cmd_update_ext_state* pCmd = (cmd_update_ext_state*)pDataBuf;
		if (ISPLAYERID(pCmd->id))
			pGameRun->PostMessage(MSG_PM_PLAYEREXTSTATE, MAN_PLAYER, -1, (DWORD)pDataBuf, pCmdHeader->cmd);
		else if (ISNPCID(pCmd->id))
			pGameRun->PostMessage(MSG_NM_NPCEXTSTATE, MAN_NPC, 0, (DWORD)pDataBuf, pCmdHeader->cmd);

		break;
	}
	case ICON_STATE_NOTIFY:
	{
		cmd_icon_state_notify* pCmd = (cmd_icon_state_notify*)pDataBuf;
		if (ISPLAYERID(pCmd->id))
			pGameRun->PostMessage(MSG_PM_PLAYEREXTSTATE, MAN_PLAYER, -1, (DWORD)pDataBuf, pCmdHeader->cmd, dwDataSize);
		else if (ISNPCID(pCmd->id))
			pGameRun->PostMessage(MSG_NM_NPCEXTSTATE, MAN_NPC, 0, (DWORD)pDataBuf, pCmdHeader->cmd, dwDataSize);

		break;
	}
	case ATTACK_ONCE:

		pGameRun->PostMessage(MSG_HST_ATTACKONCE, MAN_PLAYER, 0, (DWORD)pDataBuf, pCmdHeader->cmd);
		break;

	case PLAYER_GATHER_START:
	case PLAYER_GATHER_STOP:
	case MINE_GATHERED:

		pGameRun->PostMessage(MSG_PM_PLAYERGATHER, MAN_PLAYER, -1, (DWORD)pDataBuf, pCmdHeader->cmd);
		break;

	case TRASHBOX_PWD_CHANGED:
	case TRASHBOX_PWD_STATE:
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

		pGameRun->PostMessage(MSG_HST_TRASHBOXOP, MAN_PLAYER, 0, (DWORD)pDataBuf, pCmdHeader->cmd);
		break;

	case OBJECT_DO_ACTION:
	{
		cmd_obj_do_action* pCmd = (cmd_obj_do_action*)pDataBuf;
		if (ISPLAYERID(pCmd->id))
			pGameRun->PostMessage(MSG_PM_PLAYERDOACTION, MAN_PLAYER, -1, (DWORD)pDataBuf, pCmdHeader->cmd);

		break;
	}

	case HOST_SKILL_ATTACK_RESULT:
		pGameRun->PostMessage(MSG_HST_SKILLRESULT, MAN_PLAYER, 0, (DWORD)pDataBuf, pCmdHeader->cmd);
		break;
		
	case OBJECT_SKILL_ATTACK_RESULT:
	{
		cmd_object_skill_attack_result* pCmd = (cmd_object_skill_attack_result*)pDataBuf;
		if (ISPLAYERID(pCmd->attacker_id))
			pGameRun->PostMessage(MSG_PM_PLAYERSKILLRESULT, MAN_PLAYER, -1, (DWORD)pDataBuf, pCmdHeader->cmd);
		else if (ISNPCID(pCmd->attacker_id))
			pGameRun->PostMessage(MSG_NM_NPCSKILLRESULT, MAN_NPC, 0, (DWORD)pDataBuf, pCmdHeader->cmd);

		break;
	}
	case HOST_SKILL_ATTACKED:

		pGameRun->PostMessage(MSG_HST_SKILLATTACKED, MAN_PLAYER, 0, (DWORD)pDataBuf, pCmdHeader->cmd);
		break;

	case PLAYER_SET_ADV_DATA:
	case PLAYER_CLR_ADV_DATA:
		
		pGameRun->PostMessage(MSG_PM_PLAYERADVDATA, MAN_PLAYER, -1, (DWORD)pDataBuf, pCmdHeader->cmd);
		break;
		
	case PLAYER_IN_TEAM:
		
		pGameRun->PostMessage(MSG_PM_PLAYERINTEAM, MAN_PLAYER, -1, (DWORD)pDataBuf, pCmdHeader->cmd);
		break;

	case CON_EMOTE_REQUEST:
		
		pGameRun->PostMessage(MSG_PM_DOCONEMOTE, MAN_PLAYER, 0, (DWORD)pDataBuf, pCmdHeader->cmd);
		break;

	case DO_CONCURRENT_EMOTE:
		
		pGameRun->PostMessage(MSG_PM_DOCONEMOTE, MAN_PLAYER, -1, (DWORD)pDataBuf, pCmdHeader->cmd);
		break;

	case MATTER_PICKUP:

		pGameRun->PostMessage(MSG_PM_PICKUPMATTER, MAN_PLAYER, -1, (DWORD)pDataBuf, pCmdHeader->cmd);
		break;

	case MAFIA_INFO_NOTIFY:
	case MAFIA_TRADE_START:
	case MAFIA_TRADE_END:

		g_pGame->GetGameRun()->PostMessage(MSG_HST_FACTION, MAN_PLAYER, 0, (DWORD)pDataBuf, pCmdHeader->cmd);
		break;

	case TASK_DELIVER_REP:
	case TASK_DELIVER_EXP:
	case TASK_DELIVER_MONEY:
		
		pGameRun->PostMessage(MSG_HST_TASKDELIVER, MAN_PLAYER, 0, (DWORD)pDataBuf, pCmdHeader->cmd);
		break;

	case TASK_DELIVER_LEVEL2:

		pGameRun->PostMessage(MSG_PM_PLAYERLEVEL2, MAN_PLAYER, -1, (DWORD)pDataBuf, pCmdHeader->cmd);
		break;

	case HOST_REPUTATION:

		pGameRun->PostMessage(MSG_HST_REPUTATION, MAN_PLAYER, 0, (DWORD)pDataBuf, pCmdHeader->cmd);
		break;

	case ITEM_IDENTIFY:
		
		pGameRun->PostMessage(MSG_HST_ITEMIDENTIFY, MAN_PLAYER, 0, (DWORD)pDataBuf, pCmdHeader->cmd);
		break;

	case PLAYER_CHGSHAPE:
		
		pGameRun->PostMessage(MSG_PM_PLAYERCHGSHAPE, MAN_PLAYER, -1, (DWORD)pDataBuf, pCmdHeader->cmd);
		break;

	case ENTER_SANCTUARY:
	case LEAVE_SANCTUARY:

		pGameRun->PostMessage(MSG_HST_SANCTUARY, MAN_PLAYER, 0, (DWORD)pDataBuf, pCmdHeader->cmd);
		break;

	case SELF_OPEN_BOOTH:
	case PLAYER_OPEN_BOOTH:
	case PLAYER_CLOSE_BOOTH:
	case PLAYER_BOOTH_INFO:
	case BOOTH_TRADE_SUCCESS:
	case BOOTH_NAME:
	case OPEN_BOOTH_TEST:
	case BOOTH_SELL_ITEM:

		pGameRun->PostMessage(MSG_PM_BOOTHOPT, MAN_PLAYER, -1, (DWORD)pDataBuf, pCmdHeader->cmd);
		break;

	case PLAYER_START_TRAVEL:
	case HOST_START_TRAVEL:
	case PLAYER_END_TRAVEL:

		pGameRun->PostMessage(MSG_PM_PLAYERTRAVEL, MAN_PLAYER, -1, (DWORD)pDataBuf, pCmdHeader->cmd);
		break;

	case GM_INVINCIBLE:
	case GM_INVISIBLE:

		pGameRun->PostMessage(MSG_HST_GMOPT, MAN_PLAYER, 0, (DWORD)pDataBuf, pCmdHeader->cmd);
		break;

	case HOST_CORRECT_POS:

		pGameRun->PostMessage(MSG_HST_CORRECTPOS, MAN_PLAYER, 0, (DWORD)pDataBuf, pCmdHeader->cmd);
		break;

	case PLAYER_CHANGE_SPOUSE:
		pGameRun->PostMessage(MSG_PM_PLAYERCHANGESPOUSE, MAN_PLAYER, -1, (DWORD)pDataBuf, pCmdHeader->cmd);
		break;

	case ACTIVATE_WAYPOINT:
	case WAYPOINT_LIST:	

		pGameRun->PostMessage(MSG_HST_WAYPOINT, MAN_PLAYER, 0, (DWORD)pDataBuf, pCmdHeader->cmd);
		break;

	case TEAM_INVITE_TIMEOUT:

	//	pGameRun->PostMessage(MSG_HST_TEAMINVITETO, MAN_PLAYER, 0, (DWORD)pDataBuf, pCmdHeader->cmd);
		break;

	case PLAYER_ENABLE_PVP:
	case PLAYER_DISABLE_PVP:
	case HOST_PVP_COOLDOWN:
	case HOST_ENABLE_FREEPVP:
	case PVP_COMBAT:

		pGameRun->PostMessage(MSG_PM_PLAYERPVP, MAN_PLAYER, -1, (DWORD)pDataBuf, pCmdHeader->cmd);
		break;

	case COOLTIME_DATA:

		pGameRun->PostMessage(MSG_HST_COOLTIMEDATA, MAN_PLAYER, 0, (DWORD)pDataBuf, pCmdHeader->cmd);
		break;

	case SKILL_ABILITY:

		pGameRun->PostMessage(MSG_HST_SKILLABILITY, MAN_PLAYER, 0, (DWORD)pDataBuf, pCmdHeader->cmd);
		break;

	case BREATH_DATA:
	case HOST_STOP_DIVE:

		pGameRun->PostMessage(MSG_HST_BREATHDATA, MAN_PLAYER, 0, (DWORD)pDataBuf, pCmdHeader->cmd);
		break;

	case PLAYER_ENABLE_FASHION:

		pGameRun->PostMessage(MSG_PM_FASHIONENABLE, MAN_PLAYER, -1, (DWORD)pDataBuf, pCmdHeader->cmd);
		break;

	case INVALID_OBJECT:
	{
		cmd_invalid_object* pCmd = (cmd_invalid_object*)pDataBuf;
		if (ISPLAYERID(pCmd->id))
			pGameRun->PostMessage(MSG_PM_INVALIDOBJECT, MAN_PLAYER, -1, (DWORD)pDataBuf, pCmdHeader->cmd);
		else if (ISNPCID(pCmd->id))
			pGameRun->PostMessage(MSG_NM_INVALIDOBJECT, MAN_NPC, 0, (DWORD)pDataBuf, pCmdHeader->cmd);
		else if (ISMATTERID(pCmd->id))
			pGameRun->PostMessage(MSG_MM_INVALIDOBJECT, MAN_MATTER, 0, (DWORD)pDataBuf, pCmdHeader->cmd);

		break;
	}
	case PLAYER_ENABLE_EFFECT:
	case PLAYER_DISABLE_EFFECT:

		pGameRun->PostMessage(MSG_PM_PLAYEREFFECT, MAN_PLAYER, -1, (DWORD)pDataBuf, pCmdHeader->cmd);
		break;

	case REVIVAL_INQUIRE:

		pGameRun->PostMessage(MSG_HST_REVIVALINQUIRE, MAN_PLAYER, 0, (DWORD)pDataBuf, pCmdHeader->cmd);
		break;

	case SET_COOLDOWN:

		pGameRun->PostMessage(MSG_HST_SETCOOLTIME, MAN_PLAYER, 0, (DWORD)pDataBuf, pCmdHeader->cmd);
		break;

	case CHANGE_TEAM_LEADER:

		pGameRun->PostMessage(MSG_HST_CHGTEAMLEADER, MAN_PLAYER, 0, (DWORD)pDataBuf, pCmdHeader->cmd);
		break;

	case EXIT_INSTANCE:

		pGameRun->PostMessage(MSG_HST_EXITINSTANCE, MAN_PLAYER, 0, (DWORD)pDataBuf, pCmdHeader->cmd);
		break;

	case CHANGE_FACE_START:
	case CHANGE_FACE_END:
		
		pGameRun->PostMessage(MSG_HST_CHANGEFACE, MAN_PLAYER, 0, (DWORD)pDataBuf, pCmdHeader->cmd);
		break;

	case PLAYER_CHG_FACE:

		pGameRun->PostMessage(MSG_PM_CHANGEFACE, MAN_PLAYER, -1, (DWORD)pDataBuf, pCmdHeader->cmd);
		break;

	case SET_MOVE_STAMP:
		
		pGameRun->PostMessage(MSG_HST_SETMOVESTAMP, MAN_PLAYER, 0, (DWORD)pDataBuf, pCmdHeader->cmd);
		break;

	case INST_DATA_CHECKOUT:
		
		pGameRun->PostMessage(MSG_INSTCHECKOUT, -1, 0, (DWORD)pDataBuf, pCmdHeader->cmd);
		break;

	case MALL_ITEM_PRICE:
	case MALL_ITEM_BUY_FAILED:
		
		pGameRun->PostMessage(MSG_MALLITEMINFO, -1, 0, (DWORD)pDataBuf, pCmdHeader->cmd);
		break;
		

	case DIVIDEND_MALL_ITEM_PRICE:
	case DIVIDEND_MALL_ITEM_BUY_FAILED:

		pGameRun->PostMessage(MSG_DIVIDENDMALLITEMINFO, -1, 0, (DWORD)pDataBuf, pCmdHeader->cmd);
		break;
		
	case DOUBLE_EXP_TIME:
	case AVAILABLE_DOUBLE_EXP_TIME:
		
		pGameRun->PostMessage(MSG_DOUBLETIME, -1, 0, (DWORD)pDataBuf, pCmdHeader->cmd);
		break;
		
	case COMMON_DATA_NOTIFY:
	case COMMON_DATA_LIST:

		pGameRun->PostMessage(MSG_COMMONDATA, -1, 0, (DWORD)pDataBuf, pCmdHeader->cmd, dwDataSize);
		break;
	case UNIQUE_DATA_NOTIFY:
		{
			pGameRun->PostMessage(MSG_UNIQUE_DATA,-1,0,(DWORD)pDataBuf,pCmdHeader->cmd,dwDataSize);
		}
		break;

	case PUBLIC_QUEST_INFO:
	case PUBLIC_QUEST_RANKS:

		pGameRun->PostMessage(MSG_PUBLICQUESTINFO, -1, 0, (DWORD)pDataBuf, pCmdHeader->cmd, dwDataSize);
		break;

	case WORLD_LIFE_TIME:

		pGameRun->PostMessage(MSG_WORLDLIFETIME, -1, 0, (DWORD)pDataBuf, pCmdHeader->cmd, dwDataSize);
		break;

	case CALC_NETWORK_DELAY_RE:
		pGameRun->PostMessage(MSG_CALCNETWORKDELAY, -1, 0, (DWORD)pDataBuf, pCmdHeader->cmd, dwDataSize);
		break;

	case PLAYER_SPEC_ITEM_LIST:
		pGameRun->PostMessage(MSG_PLAYERSPECITEMLIST, -1, 0, (DWORD)pDataBuf, pCmdHeader->cmd, dwDataSize);
		break;

	case PLAYER_KNOCKBACK:
		pGameRun->PostMessage(MSG_PM_PLAYERKNOCKBACK, MAN_PLAYER, -1, (DWORD)pDataBuf, pCmdHeader->cmd, dwDataSize);
		break;

	case WEDDING_BOOK_LIST:
		pGameRun->PostMessage(MSG_HST_WEDDINGBOOKLIST, MAN_PLAYER, 0, (DWORD)pDataBuf, pCmdHeader->cmd, dwDataSize);
		break;

	case WEDDING_BOOK_SUCCESS:
		pGameRun->PostMessage(MSG_HST_WEDDINGBOOKSUCCESS, MAN_PLAYER, 0, (DWORD)pDataBuf, pCmdHeader->cmd, dwDataSize);
		break;

	case DUEL_RECV_REQUEST:
	case DUEL_REJECT_REQUEST:
	case DUEL_PREPARE:
	case DUEL_CANCEL:
	case HOST_DUEL_START:
	case DUEL_STOP:
	case PLAYER_DUEL_START:
		
		pGameRun->PostMessage(MSG_PM_PLAYERDUELOPT, MAN_PLAYER, -1, (DWORD)pDataBuf, pCmdHeader->cmd);
		break;

	case DUEL_RESULT:

		pGameRun->PostMessage(MSG_PM_PLAYERDUELRLT, MAN_PLAYER, -1, (DWORD)pDataBuf, pCmdHeader->cmd);
		break;

	case PLAYER_BIND_REQUEST:
	case PLAYER_BIND_INVITE:
	case PLAYER_BIND_REQUEST_REPLY:
	case PLAYER_BIND_INVITE_REPLY:
	case PLAYER_BIND_STOP:
		
		pGameRun->PostMessage(MSG_PM_PLAYERBINDOPT, MAN_PLAYER, -1, (DWORD)pDataBuf, pCmdHeader->cmd);
		break;

	case PLAYER_BIND_START:

		pGameRun->PostMessage(MSG_PM_PLAYERBINDSTART, MAN_PLAYER, -1, (DWORD)pDataBuf, pCmdHeader->cmd);
		break;

	case PLAYER_MOUNTING:
		
		pGameRun->PostMessage(MSG_PM_PLAYERMOUNT, MAN_PLAYER, -1, (DWORD)pDataBuf, pCmdHeader->cmd);
		break;

	case PLAYER_EQUIP_DETAIL:
		
		pGameRun->PostMessage(MSG_HST_VIEWOTHEREQUIP, MAN_PLAYER, 0, (DWORD)pDataBuf, pCmdHeader->cmd);
		break;

	case PARIAH_TIME:

		pGameRun->PostMessage(MSG_HST_PARIAHTIME, MAN_PLAYER, 0, (DWORD)pDataBuf, pCmdHeader->cmd);
		break;

	case GAIN_PET:
	case FREE_PET:
	case SUMMON_PET:
	case RECALL_PET:
	case PLAYER_START_PET_OP:
	case PLAYER_STOP_PET_OP:
	case PET_RECEIVE_EXP:
	case PET_LEVELUP:
	case PET_ROOM:
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
	case PET_REBUILD_INHERIT_START:
	case PET_REBUILD_INHERIT_INFO:
	case PET_REBUILD_INHERIT_END:
	case PET_EVOLUTION_DONE:
	case PET_REBUILD_NATURE_START:
	case PET_REBUILD_NATURE_INFO:
	case PET_REBUILD_NATURE_END:
		pGameRun->PostMessage(MSG_HST_PETOPT, MAN_PLAYER, 0, (DWORD)pDataBuf, pCmdHeader->cmd);
		break;

	case HOST_ENTER_BATTLE:
	case BATTLE_RESULT:
	case BATTLE_SCORE:

		pGameRun->PostMessage(MSG_HST_BATTLEOPT, MAN_PLAYER, 0, (DWORD)pDataBuf, pCmdHeader->cmd);
		break;

	case TANK_LEADER_NOTIFY:
		
		pGameRun->PostMessage(MSG_PM_TANKLEADER, MAN_PLAYER, -1, (DWORD)pDataBuf, pCmdHeader->cmd);
		break;

	case REFINE_RESULT:

		pGameRun->PostMessage(MSG_HST_REFINEOPT, MAN_PLAYER, 0, (DWORD)pDataBuf, pCmdHeader->cmd);
		break;

	case PLAYER_CASH:
		{
			player_cash* pCash = (player_cash*)pDataBuf;
			CECHostPlayer* pHost = pGameRun->GetHostPlayer();

			if (pHost)
			{
				int cash_before = pHost->GetCash();
				pHost->SetCash(pCash->cash_amount);
				// 收到该协议时进行批量购买，区分一下充值还是消费
				if (cash_before > pCash->cash_amount) 
					CECUIHelper::QShopBatchBuy();
			}
		}

		break;

 	case PLAYER_DIVIDEND:
		{
 			cmd_player_dividend* pCash = (cmd_player_dividend*)pDataBuf;
 			CECHostPlayer* pHost = pGameRun->GetHostPlayer();
 
			if (pHost)
			{
				if (pHost->IsDividentInitialized())
				{
					int delta = pCash->dividend - pHost->GetDividend();
					if (delta > 0)
						g_pGame->GetGameRun()->AddFixedMessage(FIXMSG_DIVIDEND_INCREASE, delta);
					else if (delta < 0)
					{
						g_pGame->GetGameRun()->AddFixedMessage(FIXMSG_DIVIDEND_DECREASE, -delta);
							CECUIHelper::QShopBatchBuy();
					}
				}
				pHost->SetDividend(pCash->dividend);
			}
		}

		break;

	case PLAYER_BIND_SUCCESS:

		pGameRun->PostMessage(MSG_HST_ITEMBOUND, MAN_PLAYER, 0, (DWORD)pDataBuf, pCmdHeader->cmd);
		break;

	case PVP_MODE:

		pGameRun->SetPVPMode(((pvp_mode*)pDataBuf)->mode != 0);
		break;

	case PLAYER_WALLOW_INFO:

		((player_wallow_info*)pDataBuf)->play_time = g_pGame->GetServerGMTTime() - ((player_wallow_info*)pDataBuf)->play_time;
		{
			player_wallow_info temp = *(player_wallow_info*)pDataBuf;
			temp.anti_wallow_active = 1;	//	受海外需求影响，此标志的含义由客户端调整为是否收到游戏服务器转发的来自AU的防沉迷协议
			pGameRun->SetWallowInfo(temp);
		}
		break;

	case NOTIFY_SAFE_LOCK:

		pGameRun->PostMessage(MSG_HST_SAFELOCK, MAN_PLAYER, 0, (DWORD)pDataBuf, pCmdHeader->cmd);
		break;

	case ELF_VIGOR:
	case ELF_ENHANCE:
	case ELF_STAMINA:
	case ELF_EXP:

		pGameRun->PostMessage(MSG_HST_GOBLININFO, MAN_PLAYER, 0, (DWORD)pDataBuf, pCmdHeader->cmd);
		break;

	case ELF_REFINE_ACTIVATE:
	case CAST_ELF_SKILL:
	case ELF_CMD_RESULT:

		pGameRun->PostMessage(MSG_PM_GOBLINOPT, MAN_PLAYER, -1, (DWORD)pDataBuf, pCmdHeader->cmd);
		break;

	case FACTION_CONTRIB_NOTIFY:

		pGameRun->PostMessage(MSG_HST_FACTIONCONTRIB, MAN_PLAYER, 0, (DWORD)pDataBuf, pCmdHeader->cmd);
		break;

	case FACTION_FORTRESS_INFO:

		pGameRun->PostMessage(MSG_HST_FACTIONFORTRESSINFO, MAN_PLAYER, 0, (DWORD)pDataBuf, pCmdHeader->cmd, dwDataSize);
		break;

	case ENTER_FACTIONFORTRESS:

		pGameRun->PostMessage(MSG_HST_ENTERFACTIONFORTRESS, MAN_PLAYER, 0, (DWORD)pDataBuf, pCmdHeader->cmd, dwDataSize);
		break;

	case FACTION_RELATION_NOTIFY:
		{
			CECFactionMan *pFMan = g_pGame->GetFactionMan();
			if (pFMan)
			{
				cmd_faction_relation_notify cmd;				
				if (cmd.Initialize(pDataBuf, dwDataSize))
					pFMan->SetHostFactionRelation(cmd.alliance, cmd.hostile);
			}
		}
		break;

	case PLAYER_EQUIP_DISABLED:
		
		pGameRun->PostMessage(MSG_PM_PLAYEREQUIPDISABLED, MAN_PLAYER, -1, (DWORD)pDataBuf, pCmdHeader->cmd);
		break;

	case CONGREGATE_REQUEST:
	case REJECT_CONGREGATE:
	case CONGREGATE_START:
	case CANCEL_CONGREGATE:

		pGameRun->PostMessage(MSG_PM_CONGREGATE, MAN_PLAYER, -1, (DWORD)pDataBuf, pCmdHeader->cmd);
		break;		

	case ENGRAVE_START:
	case ENGRAVE_END:
	case ENGRAVE_RESULT:

		pGameRun->PostMessage(MSG_HST_ENGRAVEITEM, MAN_PLAYER, 0, (DWORD)pDataBuf, pCmdHeader->cmd);
		break;

	case DPS_DPH_RANK:
		pGameRun->PostMessage(MSG_HST_DPS_DPH_RANK, MAN_PLAYER, 0, (DWORD)pDataBuf, pCmdHeader->cmd);
		break;

	case ADDONREGEN_START:
	case ADDONREGEN_END:
	case ADDONREGEN_RESULT:
		
		pGameRun->PostMessage(MSG_HST_ADDONREGEN, MAN_PLAYER, 0, (DWORD)pDataBuf, pCmdHeader->cmd);
		break;

	case INVISIBLE_OBJ_LIST:
		pGameRun->PostMessage(MSG_HST_INVISIBLEOBJLIST, MAN_PLAYER, 0, (DWORD)pDataBuf, pCmdHeader->cmd);
		break;

	case SET_PLAYER_LIMIT:
		pGameRun->PostMessage(MSG_HST_SETPLAYERLIMIT, MAN_PLAYER, 0, (DWORD)pDataBuf, pCmdHeader->cmd);
		break;

	case PLAYER_TELEPORT:
		pGameRun->PostMessage(MSG_PM_TELEPORT, MAN_PLAYER, -1, (DWORD)pDataBuf, pCmdHeader->cmd);
		break;

	case PLAYER_FORCE_DATA:
	case PLAYER_FORCE_DATA_UPDATE:
	case FORCE_GLOBAL_DATA:
		pGameRun->PostMessage(MSG_HST_FORCE_DATA, MAN_PLAYER, 0, (DWORD)pDataBuf, pCmdHeader->cmd);
		break;

	case PLAYER_FORCE_CHANGED:
		pGameRun->PostMessage(MSG_PM_FORCE_CHANGED, MAN_PLAYER, -1, (DWORD)pDataBuf, pCmdHeader->cmd);
		break;

	case PLAYER_INVENTORY_DETAIL:
		pGameRun->PostMessage(MSG_HST_INVENTORY_DETAIL, MAN_PLAYER, 0, (DWORD)pDataBuf, pCmdHeader->cmd);
		break;
	case ADD_MULTIOBJECT_EFFECT:
	case REMOVE_MULTIOBJECT_EFFECT:
		{
			cmd_multiobj_effect* pCmd = (cmd_multiobj_effect*)pDataBuf;
			if (ISPLAYERID(pCmd->id))
				pGameRun->PostMessage(MSG_PM_MULTIOBJ_EFFECT, MAN_PLAYER, -1, (DWORD)pDataBuf, pCmdHeader->cmd);
			else if (ISNPCID(pCmd->id))
				pGameRun->PostMessage(MSG_NM_MULTIOBJECT_EFFECT, MAN_NPC, 0, (DWORD)pDataBuf, pCmdHeader->cmd);
		}
		break;
	case ENTER_WEDDING_SCENE:
		pGameRun->PostMessage( MSG_HST_WEDDINGSCENE_INFO, MAN_PLAYER, 0, (DWORD)pDataBuf, pCmdHeader->cmd );
		break;

	case ONLINE_AWARD_DATA:
	case TOGGLE_ONLINE_AWARD:
			pGameRun->PostMessage(MSG_HST_ONLINEAWARD,MAN_PLAYER,0,(DWORD)pDataBuf,pCmdHeader->cmd);
		break;

	case PLAYER_PROFIT_TIME:
	case SET_PROFIT_TIME:
		pGameRun->PostMessage(MSG_HST_PROFITTIME, MAN_PLAYER, 0, (DWORD)pDataBuf, pCmdHeader->cmd);
		break;
	case ENTER_NONPENALTY_PVP_STATE:	
		pGameRun->PostMessage(MSG_HST_PVPNOPENALTY, MAN_PLAYER, 0, (DWORD)pDataBuf, pCmdHeader->cmd);
		break;

	case SELF_COUNTRY_NOTIFY:
		pGameRun->PostMessage(MSG_HST_COUNTRY_NOTIFY, MAN_PLAYER, 0, (DWORD)pDataBuf, pCmdHeader->cmd);
		break;

	case PLAYER_COUNTRY_CHANGED:
		pGameRun->PostMessage(MSG_PM_COUNTRY_CHANGED, MAN_PLAYER, -1, (DWORD)pDataBuf, pCmdHeader->cmd);
		break;

	case ENTER_COUNTRYBATTLE:
		pGameRun->PostMessage(MSG_HST_ENTER_COUNTRYBATTLE, MAN_PLAYER, 0, (DWORD)pDataBuf, pCmdHeader->cmd);
		break;
	case TRICKBATTLE_PERSONAL_SCORE:
	case TRICKBATTLE_CHARIOT_INFO:
	case ENTER_TRICKBATTLE:
	case PLAYER_QUERY_CHARIOTS:
		pGameRun->PostMessage(MSG_HST_TRICKBATTLE, MAN_PLAYER, 0, (DWORD)pDataBuf, pCmdHeader->cmd);
		break;
		
	case COUNTRYBATTLE_RESULT:
		pGameRun->PostMessage(MSG_HST_COUNTRYBATTLE_RESULT, MAN_PLAYER, 0, (DWORD)pDataBuf, pCmdHeader->cmd);
		break;
		
	case COUNTRYBATTLE_SCORE:
		pGameRun->PostMessage(MSG_HST_COUNTRYBATTLE_SCORE, MAN_PLAYER, 0, (DWORD)pDataBuf, pCmdHeader->cmd);
		
	case COUNTRYBATTLE_RESURRECT_REST_TIMES:
		pGameRun->PostMessage(MSG_HST_COUNTRYBATTLE_REVIVE_TIMES, MAN_PLAYER, 0, (DWORD)pDataBuf, pCmdHeader->cmd);
		break;
		
	case COUNTRYBATTLE_FLAG_CARRIER_NOTIFY:
		pGameRun->PostMessage(MSG_HST_COUNTRYBATTLE_CARRIER_NOTIFY, MAN_PLAYER, 0, (DWORD)pDataBuf, pCmdHeader->cmd);
		break;
		
	case COUNTRYBATTLE_BECAME_FLAG_CARRIER:
		pGameRun->PostMessage(MSG_HST_COUNTRYBATTLE_BECOME_CARRIER, MAN_PLAYER, 0, (DWORD)pDataBuf, pCmdHeader->cmd);
		break;

	case COUNTRYBATTLE_PERSONAL_SCORE:
		pGameRun->PostMessage(MSG_HST_COUNTRYBATTLE_PERSONAL_SCORE, MAN_PLAYER, 0, (DWORD)pDataBuf, pCmdHeader->cmd);
		break;

	case COUNTRYBATTLE_FLAG_MSG_NOTIFY:
		pGameRun->PostMessage(MSG_HST_COUNTRYBATTLE_FLAG_MSG_NOTIFY, MAN_PLAYER, 0, (DWORD)pDataBuf, pCmdHeader->cmd);
		break;

	case DEFENSE_RUNE_ENABLED:
		pGameRun->PostMessage(MSG_HST_DEFENSE_RUNE_ENABLE, MAN_PLAYER, 0, (DWORD)pDataBuf, pCmdHeader->cmd);
		break;
	case COUNTRYBATTLE_INFO:
		pGameRun->PostMessage(MSG_HST_COUNTRYBATTLE_INFO, MAN_PLAYER, 0, (DWORD)pDataBuf, pCmdHeader->cmd);
		break;
	case CASH_MONEY_EXCHG_RATE:
		pGameRun->PostMessage(MSG_HST_CASHMONEYRATE, MAN_PLAYER, 0, (DWORD)pDataBuf, pCmdHeader->cmd);
	case EQUIP_ADDON_UPDATE_NOTIFY:
		pGameRun->PostMessage(MSG_HST_STONECHANGEEND,MAN_PLAYER,0,(DWORD)pDataBuf,pCmdHeader->cmd);
		break;

	case SELF_KING_NOTIFY:
		pGameRun->PostMessage(MSG_HST_KINGNOTIFY, MAN_PLAYER, 0, (DWORD)pDataBuf, pCmdHeader->cmd);
		break;
	case PLAYER_KING_CHANGED:
		pGameRun->PostMessage(MSG_PM_KINGCHANGED, MAN_PLAYER, -1, (DWORD)pDataBuf, pCmdHeader->cmd);
		break;
	case COUNTRYBATTLE_STRONGHOND_STATE_NOTIFY:
		pGameRun->PostMessage(MSG_HST_COUNTRYBATTLE_STRONGHOLD_STATE, MAN_PLAYER, 0, (DWORD)pDataBuf, pCmdHeader->cmd);
		break;
	case QUERY_TOUCH_POINT:
	case SPEND_TOUCH_POINT:
	case TOTAL_RECHARGE:
		pGameRun->PostMessage(MSG_HST_TOUCHPOINT, MAN_PLAYER, 0, (DWORD)pDataBuf, pCmdHeader->cmd);
		break;
	case QUERY_TITLE_RE:
	case CHANGE_CURR_TITLE_RE:
		pGameRun->PostMessage(MSG_PM_TITLE, MAN_PLAYER, -1, (DWORD)pDataBuf, pCmdHeader->cmd);
		break;
	case MODIFY_TITLE_NOFIFY:
		pGameRun->PostMessage(MSG_HST_TITLE, MAN_PLAYER, 0, (DWORD)pDataBuf, pCmdHeader->cmd);
		break;
	case REFRESH_SIGNIN:
		pGameRun->PostMessage(MSG_HST_SIGNIN, MAN_PLAYER, 0, (DWORD)pDataBuf, pCmdHeader->cmd);
		break;
	case PARALLEL_WORLD_INFO:
		pGameRun->PostMessage(MSG_PARALLEL_WORLD_INFO, -1, 0, (DWORD)pDataBuf, pCmdHeader->cmd);
		break;
	case PLAYER_REINCARNATION:
		pGameRun->PostMessage(MSG_PM_REINCARNATION, MAN_PLAYER, -1, (DWORD)pDataBuf, pCmdHeader->cmd);
		break;
	case REINCARNATION_TOME_INFO:
		pGameRun->PostMessage(MSG_HST_REINCARNATION_TOME_INFO, MAN_PLAYER, 0, (DWORD)pDataBuf, pCmdHeader->cmd);
		break;
	case ACTIVATE_REINCARNATION_TOME:
		pGameRun->PostMessage(MSG_HST_REINCARNATION_TOME_ACTIVATE, MAN_PLAYER, 0, (DWORD)pDataBuf, pCmdHeader->cmd);
		break;
	case USE_GIFTCARD_RESULT:
		pGameRun->PostMessage(MSG_HST_USE_GIFTCARD, MAN_PLAYER, 0, (DWORD)pDataBuf, pCmdHeader->cmd);
		break;
	case REALM_EXP:
		pGameRun->PostMessage(MSG_HST_REALM_EXP, MAN_PLAYER, 0, (DWORD)pDataBuf, pCmdHeader->cmd);
		break;
	case REALM_LEVEL:
		pGameRun->PostMessage(MSG_PM_REALMLEVEL, MAN_PLAYER, -1, (DWORD)pDataBuf, pCmdHeader->cmd);
		break;
	case PLAYER_LEADERSHIP:
	case GENERALCARD_COLLECTION_DATA:
	case ADD_GENERALCARD_COLLECTION:
		pGameRun->PostMessage(MSG_HST_GENERALCARD, MAN_PLAYER, 0, (DWORD)pDataBuf, pCmdHeader->cmd);
		break;
	case REFRESH_MONSTERSPIRIT_LEVEL:
		pGameRun->PostMessage(MSG_HST_MONSTERSPIRIT_LEVEL, MAN_PLAYER, 0, (DWORD)pDataBuf, pCmdHeader->cmd);
		break;
	case PLAYER_IN_OUT_BATTLE:
		pGameRun->PostMessage(MSG_PM_PLAYER_IN_OUT_BATTLE, MAN_PLAYER, -1, (DWORD)pDataBuf, pCmdHeader->cmd);
		break;
	case COUNTRYBATTLE_LIVE_SHOW_RESULT:
		pGameRun->PostMessage(MSG_HST_COUNTRYBATTLE_LIVESHOW, MAN_PLAYER, 0, (DWORD)pDataBuf, pCmdHeader->cmd, dwDataSize);
		break;
	case RANDOM_MALL_SHOPPING_RES:
		pGameRun->PostMessage(MSG_HST_RAND_MALL_SHOPPING_RES, MAN_PLAYER, 0,( DWORD)pDataBuf, pCmdHeader->cmd, dwDataSize);
		break;
	case FACTION_PVP_MASK_MODIFY:
		pGameRun->PostMessage(MSG_PM_FACTION_PVP_MASK_MODIFY, MAN_PLAYER, -1, (DWORD)pDataBuf, pCmdHeader->cmd, dwDataSize);
		break;
	case PLAYER_WORLD_CONTRIBUTION:
		pGameRun->PostMessage(MSG_HST_WORLD_CONTRIBUTION, MAN_PLAYER, 0, (DWORD)pDataBuf, pCmdHeader->cmd, dwDataSize);
		break;
	case SCENE_SERVICE_NPC_LIST:
		{
			cmd_scene_service_npc_list* npcList = (cmd_scene_service_npc_list*)pDataBuf;
			CECHostSkillModel::Instance().RecvNPCServiceList(Data);
			break;
		}
	case RANDOM_MAP_ORDER:
		pGameRun->PostMessage(MSG_RANDOM_MAP_ORDER,-1,0,(DWORD)pDataBuf,pCmdHeader->cmd,dwDataSize);
		break;
	case EQUIP_CAN_INHERIT_ADDONS:
		pGameRun->PostMessage(MSG_HST_CANINHERIT_ADDONS, MAN_PLAYER, 0, (DWORD)pDataBuf,pCmdHeader->cmd,dwDataSize);
        break;
	case CLIENT_SCREEN_EFFECT:
		pGameRun->PostMessage(MSG_HST_CLIENT_SCREENEFFECT, MAN_PLAYER, 0, (DWORD)pDataBuf, pCmdHeader->cmd, dwDataSize);
		break;
	case COMBO_SKILL_PREPARE:
		pGameRun->PostMessage(MSG_HST_COMBO_SKILL_PREPARE, MAN_PLAYER, 0, (DWORD)pDataBuf, pCmdHeader->cmd, dwDataSize);
		break;
	case INSTANCE_REENTER_NOTIFY:
		pGameRun->PostMessage(MSG_HST_INSTANCE_REENTER_NOTIFY, MAN_PLAYER, 0, (DWORD)pDataBuf, pCmdHeader->cmd, dwDataSize);
		break;
	case PRAY_DISTANCE_CHANGE:
		pGameRun->PostMessage(MSG_HST_PRAY_DISTANCE_CHANGE, MAN_PLAYER, 0, (DWORD)pDataBuf, pCmdHeader->cmd, dwDataSize);
		break;
	default:
		break;
	}
}

namespace 
{
static 	z_stream xin;
static 	z_stream xout;
static int deflate_size =0;
static int inflate_size =0;
static bool deflate_init_flag = false;

class ttttt
{
public:
	void Init()
	{
		deflate_init_flag = true;
		deflate_size =0;
		inflate_size =0;
		memset(&xin,0,sizeof(xin));
		memset(&xout,0,sizeof(xout));
		int err = deflateInit2(&xin,9,Z_DEFLATED,-15,9,0);
		assert(err ==Z_OK);
		err = inflateInit2(&xout,-15);
		assert(err ==Z_OK);
	}

	void Final()
	{
		deflate_init_flag = false;
		deflateEnd(&xin);		
		inflateEnd(&xout);
	}

	ttttt()
	{
	}
	~ttttt()
	{
		if(deflate_init_flag)	Final();
	}
	void SInit()
	{
		if(deflate_init_flag)	Final();		
		Init();
	}
};

void InitZStream()
{
	static ttttt t1;
	t1.SInit();
}

void TestStreamCompress(BYTE *pDataBuf,int nSize,int & compr_size,int &total_size)
{
	static ttttt t1;
	unsigned char bufin[32768];
	unsigned char bufout[32768];
	xin.next_in = pDataBuf;
	xin.avail_in = nSize;
	xin.next_out = bufin;
	xin.avail_out = 32768;
	int err = deflate(&xin, Z_SYNC_FLUSH);
	assert(err == Z_OK);
	int in_size = xin.total_out - deflate_size;
	deflate_size = xin.total_out;

	xout.next_in = bufin;
	xout.avail_in = in_size;
	xout.next_out = bufout;
	xout.avail_out = 32768;
	err = inflate(&xout, Z_SYNC_FLUSH);
	assert(err == Z_OK);
	int out_size = xout.total_out - inflate_size;
	inflate_size = xout.total_out;
	assert(out_size == nSize);
	assert(memcmp(bufout,pDataBuf,nSize) == 0);
	compr_size = in_size;
	total_size = xin.total_out;
}
}

															   
//	Log S2C command data
void CECGameSession::LogS2CCmdData(int iCmd, BYTE* pDataBuf, int nSize)
{
	if( !m_pS2CCmdLog )
		return;

	/*
	using namespace S2C;
	m_nS2CCmdSize += nSize;

	int block_c_size = nSize;
	int total_c_size = m_nS2CCmdSize;

#ifndef _NOTEST_COMPRESS
	if(m_nS2CCmdSize == nSize) InitZStream();
	TestStreamCompress(pDataBuf,nSize,block_c_size,total_c_size);
#endif

	int iPrintSize = nSize;
	a_ClampRoof(iPrintSize, 600);

	char* szStr = new char [iPrintSize * 3 + 1];
	int iCount = 0;
	for(int i=0; i < iPrintSize; i++)
	{
		sprintf(&szStr[iCount], "%02x ", pDataBuf[i]);
		iCount += 3;
	}

	m_pS2CCmdLog->Log("total: %d(%d), size: %d(%d), cmd: %d, data: %s", m_nS2CCmdSize,total_c_size, nSize,block_c_size, iCmd, szStr);

	delete [] szStr;
	*/

	// output to binary log
	if( !m_pS2CCmdBinLog )
		return;

	DWORD dwWriteLen;
	m_pS2CCmdBinLog->Write(&nSize, sizeof(nSize), &dwWriteLen);
	SYSTEMTIME sysTime;
	GetLocalTime(&sysTime);
	m_pS2CCmdBinLog->Write(&sysTime, sizeof(sysTime), &dwWriteLen);
	m_pS2CCmdBinLog->Write(pDataBuf, nSize, &dwWriteLen);
	m_pS2CCmdBinLog->Flush();
	return;
}

