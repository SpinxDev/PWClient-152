#include "TaskTempl.h"
#include "TaskTemplMan.h"

/* Version Info */
unsigned long _task_templ_cur_version = 121;

static const char _format_version[]			= "MOXTVersion: %u";
static const char _format_task_count[]		= "TaskCount: %d";

/* 任务对象的属性 */
static const char _format_id[]				= "ID: %u";
static const char _format_type[]			= "Type: %u";
static const char _format_dyn[]				= "Dyn: %d";
static const char _format_has_sign[]		= "HasSign: %d";
static const char _format_time_limit[]		= "TimeLimit: %u";

static const char _format_offline_fail[]	= "OffLineFail: %d";
static const char _format_abs_fail_time[]	= "AbsFailTime: %d";
static const char _format_item_not_take_off[]	= "ItemNotTakeOff: %d";

static const char _format_abs_time[]		= "AbsTime: %d";
static const char _format_child_in_order[]	= "ChildInOrder: %u";
static const char _format_choose_one[]		= "ChooseOne: %u";
static const char _format_rand_one[]		= "RandOne: %u";
static const char _format_parent_fail[]		= "ParentAlsoFail: %d";
static const char _format_parent_succ[]		= "ParentAlsoSuccess: %d";
static const char _format_can_give_up[]		= "CanGiveUp: %d";
static const char _format_die_as_fail[]		= "DieAsFail: %d";
static const char _format_can_redo[]		= "CanRedo: %d";
static const char _format_redo_after_fail[]	= "CanRedoAfterFailure: %d";
static const char _format_clear_as_giveup[]	= "ClearAsGiveUp: %d";
static const char _format_need_record[]		= "NeedRecord: %d";
static const char _format_max_receiver[]	= "MaxReceiver: %u";
static const char _format_must_in_zone[]	= "MustInZone: %u";
static const char _format_enter_region_fail[]	= "EnterRegionFail: %u";
static const char _format_leave_region_fail[]	= "LeaveRegionFail: %u";
static const char _format_trans_to[]		= "TransTo: %u";
static const char _format_world_id[]		= "WorldId: %u";
static const char _format_zone_vert[]		= "ZoneVert: %f, %f, %f";
static const char _format_controller[]		= "Controller: %d";
static const char _format_trig_flag[]		= "TrigFlag: %d";
static const char _format_auto_deliver[]	= "AutoDeliver: %d";
static const char _format_auto_deliver_prompt[] = "IsPromptWhenAutoDeliver: %d"; 
static const char _format_death_trig[]		= "DeathTrig: %d";
static const char _format_clear_acquired[]	= "ClearAcquired: %d";
static const char _format_suit_lev[]		= "SuitLev: %d";
static const char _format_show_prompt[]		= "ShowPrompt: %d";
static const char _format_key_task[]		= "KeyTask: %d";
static const char _format_npc[]				= "NPC: %d";
static const char _format_skill_task[]		= "SkillTask: %d";
static const char _format_seek_out[]		= "SeekOut: %d";
static const char _format_direction[]		= "Direction: %d";
static const char _format_avail_frequency[]	= "AvailFrequency: %d";
static const char _format_period_limit[]	= "PeriodLimit: %d";
static const char _format_marriage[]		= "%d";
static const char _format_switch_fail[]     = "SwitchSceneFail: %d";
static const char _format_hidden[]          = "HiddenTask: %d";
static const char _format_pq_task[]			= "PQTask: %d";
static const char _format_pq_task_exp_cnt[]	= "PQTaskExpCnt: %d";
static const char _format_pq_sub_task[]		= "PQSubTask: %d";
static const char _format_pq_clear_contrib[]	= "PQClearContrib: %d";
static const char _format_pq_contrib_cnt[]	= "PQContribCnt: %d";
static const char _format_pq_monster_contrib[] = "MonsterID: %d WholeContrib: %d ShareContrib: %d";
static const char _format_pq_monster_contrib_personal[] = "PersonalWholeContrib: %d"; // version 82
static const char _format_account_limit[]	= "AccountTaskLimit: %d";
static const char _format_role_limit[]	= "RoleTaskLimit: %d";
static const char _format_account_limit_cnt[] = "AccountTaskLimitCnt: %d";

static const char _format_deliverySkill[] = "Delivery Skill: %d";   // version 77 
static const char _format_deliveredSkillID[] = "Delivered Skill ID: %d";
static const char _format_deliveredSkillLevel[] = "Delivered Skill Level: %d";

static const char _format_showGfxWhenFinished[] = "Show Gfx: %d"; // version 78
static const char _format_changePQRanking[] = "Change PQ Ranking: %d"; // version 79

static const char _format_taskRegionCnt[] = "taskRegionCnt: %u"; // version 80

static const char _format_compareItemAndInventory[] = "compare item and inventory: %d"; // version 81
static const char _format_needInventorySlotNum[] = "Need inventory slot num: %u";

static const char _format_is_library_task[] = "Is library task or not: %d";//version 83
static const char _format_library_task_probability[] = "Probability of Library task: %f";
static const char _format_unique_storage_task[] = "Is unique storage task: %d";
static const char _format_leave_force_fail[] = "Leave Force Fail: %d";
static const char _format_leave_faction_fail[] = "Leave faction Fail: %d";
static const char _format_failed_not_take_item[] = "TeamFailedNotTakeItem: %d";
static const char _format_display_in_title_ui[] = "DisplayInTitleUI: %d";
static const char _format_failed_not_inc_count[] = "NotAddCountWhenFailedTask: %d";
static const char _format_token_shop_task[] = "TokenShopTask: %d";
static const char _format_worldcontrib[] = "WorldContrib: %d";

/* 任务的开启条件及方式 */
static const char _format_prem_items_any_one[] = "PremItemsAnyOne: %d";
static const char _format_prem_transformed_mask[] = "PremiseTransformedMask: %c";
static const char _format_show_by_transformed[] = "ShowByTransformed: %d";
static const char _format_prem_record_tasks_num[] = "PremiseRecordTasksNum: %d";
static const char _format_prem_faction_contrib[] = "PremiseFactionContrib: %d";
static const char _format_prem_level[]		= "PremiseLevel: %u";
static const char _format_prem_deposit[]	= "PremiseDeposit: %u";
static const char _format_prem_reputation[]	= "PremiseReputation: %d";
static const char _format_prem_task_count[]	= "PremiseTaskCount: %u";
static const char _format_prem_task[]		= "PremiseTask: %u";
static const char _format_prem_task_leastnum[] = "AtLeastNumTaskFinished: %u";
static const char _format_prem_period[]		= "PremisePeriod: %u";
static const char _format_prem_npc[]		= "PremiseNPC: %u";
static const char _format_prem_is_team[]	= "PremiseIsTeam: %d";
static const char _format_prem_is_spouse[]	= "PremiseIsSpouse: %d";
static const char _format_prem_is_wedding_owner[] = "WeddingOwner: %d";
static const char _format_prem_is_gm[]		= "PremiseIsGM: %d";
static const char _format_prem_is_shielduser[] = "PremiseIsShieldUser: %d";
static const char _format_prem_clan[]		= "PremiseClan: %u";
static const char _format_prem_clan_role[]  = "PremiseClanRole: %d";
static const char _format_prem_gender[]		= "PremiseGender: %u";
static const char _format_prem_occupation[]	= "PremiseOccupation: %u";
static const char _format_prem_cotask[]		= "PremiseCotask: %u";
static const char _format_cotask_cond[]		= "CotaskCond: %u";
static const char _format_mutex_task_count[]= "MutexTaskCount: %u";
static const char _format_mutex_task[]		= "MutexTask: %u";
static const char _format_rcv_by_team[]		= "RcvByTeam: %d";
static const char _format_shared[]			= "SharedWithNew: %d";
static const char _format_share_acquire[]	= "ShareAcquired: %d";
static const char _format_check_tmmate[]	= "CheckTeammate: %d";
static const char _format_rcv_chck_mem[]	= "RcvChckMem: %d";
static const char _format_cnt_by_mem_pos[]	= "CntByMemPos: %d";
static const char _format_all_fail[]		= "TeamAllFail: %d";
static const char _format_cap_fail[]		= "TeamCapFail: %d";
static const char _format_cap_succ[]		= "TeamCapSucc: %d";
static const char _format_all_succ[]		= "TeamAllSucc: %d";
static const char _format_dism_self_fail[]	= "DismissAsSelfFail: %d";
static const char _format_dism_all_fail[]	= "DismissAsAllFail: %d";
static const char _format_couple_only[]		= "CoupleOnly: %d";
static const char _format_distinguished_occ[]="AllOccDifferent: %d";

static const char _format_mem_num_wanted[]	= "MemNumWanted: %d";
static const char _format_race[]			= "Race: %d";
static const char _format_occupation[]		= "Occupation: %X";
static const char _format_task[]			= "Task: %d";
static const char _format_level[]			= "Level: %d";
static const char _format_gender[]			= "Gender: %d";
static const char _format_count[]			= "Count: %d";
static const char _format_full_tm[]			= "Y: %d, M: %d, D: %d, H: %d, M: %d, W: %d";
static const char _format_full_tm_old[]		= "Y: %d, M: %d, D: %d, H: %d, M: %d";
static const char _format_dist[]			= "Dist: %f";
static const char _format_given_items_num[]	= "GivenItemsNum: %d";
static const char _format_proficiency[]		= "Proficiency: %d";
static const char _format_special_award[]	= "SpeAward: %d";
static const char _format_tm_type[]			= "TimeType: %d";

static const char _format_by_deposit[]		= "ShowByDeposit: %d";
static const char _format_by_gender[]		= "ShowByGender: %d";
static const char _format_by_items[]		= "ShowByItems: %d";
static const char _format_by_record_tasks_num[] = "ShowByRecordTasksNum: %d";
static const char _format_by_faction_contrib[]= "ShowByFactionContrib: %d";
static const char _format_by_lev[]			= "ShowByLev: %d";
static const char _format_by_occup[]		= "ShowByOccup: %d";
static const char _format_by_preTask[]		= "ShowByPreTask: %d";
static const char _format_by_repu[]			= "ShowByRepu: %d";
static const char _format_by_team[]			= "ShowByTeam: %d";
static const char _format_by_faction[]		= "ShowByFaction: %d";
static const char _format_by_period[]		= "ShowByPeriod: %d";
static const char _format_by_spouse[]		= "ShowBySpouse: %d";
static const char _format_by_wedding_owner[]= "ShowByWeddingOwner: %d";
static const char _format_by_force[]		= "ShowByForce: %d";
static const char _format_by_force_contrib[]= "ShowByForceContrib: %d";
static const char _format_by_force_repu[]	= "ShowByForceReputation: %d";
static const char _format_by_force_exp[]    = "ShowByForceExp: %d";
static const char _format_by_force_sp[]		= "ShowByForceSP: %d";
static const char _format_by_force_al[]		= "ShowByForceActivityLevel: %d";
static const char _format_by_king[]			= "ShowByKing: %d";
static const char _format_by_not_in_team[]  = "ShowByDoNotAllowInTeam: %d";
static const char _format_by_title[]		= "ShowByTitle: %d";
static const char _format_by_reincarnation[]= "ShowByReincarnation: %d";
static const char _format_by_realmlevel[] = "ShowByRealmLevel: %d";
static const char _format_by_generalcard_cnt[]	= "ShowByGerneralCardCount: %d";
static const char _format_by_generalcard_rankcnt[]	= "ShowByGerneralCardRankCount: %d";

static const char _format_need_comp[]       = "NeedCompare: %d";
static const char _format_and_or[]          = "Exp1AndOrExp2: %d"; 
static const char _format_left_type[]       = "LeftNumType: %d";
static const char _format_left_num[]        = "LeftNum: %d";
static const char _format_comp_oper[]       = "CompareOper: %d";
static const char _format_right_type[]      = "RightNumType %d";
static const char _format_right_num[]       = "RightNum: %d";

static const char _format_prem_key_cnt[]    = "ChangeKeyCnt: %d";
static const char _format_prem_change_key[] = "ChangeKey: %d";
static const char _format_prem_change_kv[]  = "ChangeKeyValue: %d";
static const char _format_prem_change_typ[] = "ChangeType: %d";

static const char _format_prem_rmb_min[]	= "AccountRMBMin: %d";
static const char _format_prem_rmb_max[]	= "AccountRMBMax: %d";

static const char _format_char_time[]		= "CharacterTime: %d";
static const char _format_by_char_time[]	= "ShowByCharTime: %d";
static const char _format_char_start_time[]	= "CharacterStartTime: %d";
static const char _format_char_end_time[]	= "CharacterEndTime: %d";
static const char _format_char_time_greater[]= "CharacterTimeGreaterThan: %d";
static const char _format_by_rmb[]			= "ShowByRMB: %d";
static const char _format_team_force[]      = "TeamForce: %d";
static const char _format_check_force[]     = "CheckForce: %d";
static const char _format_force[]			= "Force: %d";
static const char _format_prem_force_contrib[] = "ForcePremContrib: %d";
static const char _format_prem_force_repu[]   = "ForcePremReputation: %d";
static const char _format_prem_force_exp[]	= "ForcePremExp: %d";
static const char _format_prem_force_sp[]		= "ForcePremSP: %d";
static const char _format_prem_force_al[]	= "ForceActivityLevel: %d";
static const char _format_prem_is_king[]	= "CheckKing: %d";
static const char _format_prem_not_in_team[] = "DonotAllowInTeam: %d";
static const char _format_prem_title_total[] = "TitleTotal: %d";
static const char _format_prem_title_required[] = "TitleRequired: %d";
static const char _format_prem_title_id[] = "Title ID: %d";
static const char _format_prem_historyIndex[] = "HistoryStage index: %d, %d";
static const char _format_prem_check_historymaxlevel[] = "MaxHistoryLevel: %d";
static const char _format_prem_check_reincarnation[] = "CheckReincarnation: %d";
static const char _format_prem_reincarnation_min[] = "MinReincarnationCount: %u";
static const char _format_prem_reincarnation_max[] = "MaxReincarnationCount: %u";
static const char _format_prem_check_realmlevel[] = "CheckRealmLevel: %d";
static const char _format_prem_realmlevel_min[] = "RealmLevelMin: %u";
static const char _format_prem_realmlevel_max[] = "RealmLevelMax: %u";
static const char _format_prem_realmexp_full[] = "RealmExpFull: %d";
static const char _format_prem_cardcollection[] = "CardCollection Count: %d";
static const char _format_prem_cardrank[] = "CardRank: %d";
static const char _format_prem_cardrankcount[] = "CardRankCount: %u";
static const char _format_by_historyStage[]			= "ShowByHistoryStage: %d";

/* 任务完成的方式及条件 */
static const char _format_transformed_mask[] = "TransformedMask: %c";
static const char _format_treasure_zones_s[]= "TreasureZonesStart: %f, %f, %f";
static const char _format_treasure_zones_x[]= "TreasureZonesX: %d";
static const char _format_treasure_zones_z[]= "TreasureZonesZ: %d";
static const char _format_treasure_zone_l[] = "TreasureZoneLen: %d";
static const char _format_method[]			= "Method: %u";
static const char _format_finish_type[]		= "FinishType: %u";
static const char _format_emotion[] = "EmotionAction: %d";


static const char _format_player_wanted[]   = "PlayerWanted: %u";
static const char _format_player_num[]		= "PlayerNum: %u";
static const char _format_player_require_occ[]= "KillPlayerRequiredOccupations: %u";
static const char _format_player_require_level_min[] = "KillPlayerRequiredMinLevel: %d";
static const char _format_player_require_level_max[] = "KillPlayerRequiredMaxLevel: %d";
static const char _format_player_require_gender[] = "KillPlayerRequiredGender: %d";
static const char _format_player_require_force[] = "KillPlayerRequiredForce: %d";
static const char _format_monster_wanted[]	= "MonsterWanted: %u";
static const char _format_monster_templ[]	= "MonsterTempl: %u";
static const char _format_monster_num[]		= "MonsterNum: %u";
static const char _format_item_drop_prob[]	= "ItemDropProb: %f";
static const char _format_killer_lev[]		= "KillerLev: %d";
static const char _format_dps[]				= "DPS: %d";
static const char _format_dph[]				= "DPH: %d";
static const char _format_item_wanted_num[]	= "ItemWantedNum: %u";
static const char _format_item_templ[]		= "ItemTempl: %u";
static const char _format_item_is_cmn[]		= "IsCommon: %d";
static const char _format_item_num[]		= "ItemNum: %u";
static const char _format_npc_to_protect[]	= "NPCToProtect: %u";
static const char _format_protect_tm_len[]	= "ProtectTimeLen: %u";
static const char _format_waittime[]		= "WaitTime: %u";
static const char _format_gold[]			= "GoldNum: %u";
static const char _format_faction_contrib[]	= "FactionConsumeContrib: %u";
static const char _format_faction_exp_contrib[]	= "FactionConsumeContrib: %u";
static const char _format_dis_expcnt[]= "DisplayExpCnt: %d";
static const char _format_discharcnt[]= "DisplayTaskCharCnt: %d";
static const char _format_reach_level[] = "ReachLevel: %u";
static const char _format_reach_reincarnation_count[] = "ReachReincarnationCount: %u";
static const char _format_reach_realm_level[] = "ReachRealmLevel: %u";

/* 任务结束后的奖励 */
static const char _format_award_faction_contrib[] = "FactionContrib: %d";
static const char _format_award_faction_exp_contrib[] = "FactionExpContrib: %d";
static const char _format_award_type[]		= "AwardType: %u";
static const char _format_award_gold_num[]	= "GoldNum: %u";
static const char _format_award_exp_num[]	= "ExpNum: %u";
static const char _format_award_realm_exp[] = "RealmExp: %u";
static const char _format_award_realmlevel_max[] = "RealmLevelMax: %d";
static const char _format_award_cmn_item[]	= "CommonItem: %u";
static const char _format_award_task_item[]	= "TaskItem: %u";
static const char _format_award_sp_num[]	= "SPNum: %u";
static const char _format_award_repu[]		= "Reputation: %d";
static const char _format_award_period[]	= "Period: %u";
static const char _format_award_station[]	= "Station: %u";
static const char _format_award_store[]		= "StoreSize: %u";
static const char _format_award_invent[]	= "InventSize: %d";
static const char _format_award_pet[]		= "PetNum: %u";
static const char _format_award_fury[]		= "Fury: %u";
static const char _format_award_new_task[]	= "NewTask: %u";
static const char _format_ratio[]			= "Ratio: %f";
static const char _format_award_cands_num[]	= "CandsNum: %d";
static const char _format_prob[]			= "Prob: %f";
static const char _format_period[]          = "Period: %d";
static const char _format_use_lev_co[]		= "UseLevCo: %d";
static const char _format_award_divorce[]	= "Divorce: %d";
static const char _format_award_msg[]		= "SendMsg: %d";
static const char _format_award_channel[]	= "Channel: %d";

static const char _format_award_death[]		= "AwardDeath: %d";
static const char _format_award_death_loss[]= "DeathWithLoss: %d";

static const char _format_award_dividend[]	= "AwardDividend: %d";

static const char _format_award_key_cnt[]   = "ChangeKeyCnt: %d";
static const char _format_award_change_key[]= "ChangeKey: %d";
static const char _format_award_change_kv[] = "ChangeKeyValue: %d";
static const char _format_award_change_typ[]= "ChangeType: %d";

static const char _format_award_multi[]     = "CalMulti: %d";
static const char _format_award_num_type[]  = "MultiNumType: %d";
static const char _format_award_num[]       = "MultiNum: %d";

static const char _format_award_dis_keycnt[]= "DisplayKeyCnt: %d";
static const char _format_award_dis_key[]   = "DisplayKey: %d";

static const char _format_award_dis_expcnt[]= "DisplayExpCnt: %d";
static const char _format_award_discharcnt[]= "DisplayTaskCharCnt: %d";

static const char _format_award_monster_cand[] = "MonsterCand: %d";
static const char _format_award_rand_monster[] = "MonsterRand: %u";
static const char _format_award_summon_radius[] = "SummonRadius: %d";
static const char _format_award_death_disappear[] = "DeathDisappear: %u";

static const char _format_award_monster_id[] = "MonsterID: %d";
static const char _format_award_monster_num[] = "MonsterNum: %d";
static const char _format_award_monster_prob[] = "MonsterProb: %f";
static const char _format_award_monster_period[] = "MonsterPeriod: %d";

static const char _format_award_pq_by_prof[] = "PQAwardByProf: %u";
static const char _format_award_pq_specify_id[] = "PQSpecifyTaskID: %d";
static const char _format_award_pq_specify_contrib[] = "PQSpecifyContrib: %d";
static const char _format_award_pq_contrib[] = "PQContrib: %d";
static const char _format_award_pq_rand_contrib[] = "PQRandContrib: %d";
static const char _format_award_pq_lowest_contrib[] = "PQLowestContrib: %d";
static const char _format_award_pq_ranking_cnt[] = "PQRankCnt: %d";
static const char _format_award_pq_ranking[] = "RankStart: %d RankEnd: %d ItemID: %d IsCommonItem: %d ItemNum: %d Period %d";

static const char _format_award_pq_specify_sub_taskid[] = "PQSpecifySubTaskID: %d"; // 76

static const char _format_award_DeliverySkill[] = "Award delivery skill: %d"; // version 77
static const char _format_award_DeliverySkillID[] = "Award delivered skill ID: %d";
static const char _format_award_DeliverySkillLevel[] = "Award delivered skill level: %d";

static const char _format_award_force_activity[]	= "Award delivered force activity: %d";
static const char _format_award_force_contrib[]		= "Award delivered force contribution: %d";
static const char _format_award_force_repu[]		= "Award delivered force reputation: %d";
static const char _format_award_force_set_repu[]	= "Award set force reputation: %d";

static const char _format_award_set_task_limit[]	= "Award set task limit: %d";
static const char _format_award_title_num[]	= "Award title num: %d";
static const char _format_award_title_id[]	= "Award title id: %d";
static const char _format_award_title_period[]	= "Award title period: %d";

static const char _format_award_historykey_cnt[]   = "HistoryChangeKeyCnt: %d";
static const char _format_award_historychange_key[]= "HistoryChangeKey: %d";
static const char _format_award_historychange_kv[] = "HistoryChangeKeyValue: %d";
static const char _format_award_historychange_typ[]= "HistoryChangeType: %d";

static const char _format_award_leadership[] = "LeaderShip: %d";
static const char _format_award_worldcontrib[] = "AwardWorldContrib: %d";

/* 层次关系 */
static const char _format_parent[]			= "ParentId: %u";
static const char _format_next_sibling[]	= "NextSibling: %u";
static const char _format_prev_sibling[]	= "PrevSibling: %u";
static const char _format_first_child[]		= "FirstChild: %u";

static const char _format_descpt_len[]		= "DescriptLen: %u";
static const char _format_tribute_len[]		= "TributeLen: %u";

const unsigned long _race_occ_map[MAX_OCCUPATIONS] =
{
	1,	//武侠
	1,	//法师
	3,	//巫师
	2,	//妖精
	2,	//妖兽
	3,	//刺客
	4,	//羽芒
	4,	//羽灵
	5,	//剑灵
	5,	//魅灵
	6,	//夜影
	6,	//月仙
};

unsigned long _living_skill_ids[MAX_LIVING_SKILLS] =
{
	158, // 武器制造
	159, // 护具制造
	160, // 饰品制造
	161	 // 药品、符箓制造
};

unsigned long g_ulNewCount = 0;
unsigned long g_ulDelCount = 0;

#define VERT_ZONE(pos)		(pos).x, (pos).y, (pos).z
#define VERT_ZONE_ADDR(pos)	&(pos).x, &(pos).y, &(pos).z

#ifdef _TASK_CLIENT

inline int marshal_str(char* data, const task_char* s)
{
	char* p = data;
	size_t len = s ? wcslen(s) : 0;

	*(int*)p = len;
	p += sizeof(int);

	if (len)
	{
		len *= sizeof(task_char);
		memcpy(p, s, len);
		p += len;
	}

	return p - data;
}

#endif

inline int unmarshal_str(const char* data, task_char*& s)
{
	const char* p = data;

	size_t len = *(int*)p;
	p += sizeof(int);

	s = new task_char[len + 1];
	g_ulNewCount++;
	s[len] = 0;

	if (len)
	{
		len *= sizeof(task_char);
		memcpy(s, p, len);
		p += len;
	}

	return p - data;
}

#ifdef _WINDOWS

	int marshal_option(const talk_proc::option* opt, char* data)
	{
		char* p = data;

		*(int*)p = opt->id;
		p += sizeof(int);

		*(int*)p = opt->param;
		p += sizeof(int);

		size_t sz = wcslen(opt->text) * sizeof(namechar);
		*(int*)p = sz;
		p += sizeof(int);

		if (sz)
		{
			memcpy(p, opt->text, sz);
			p += sz;
		}

		return p - data;
	}

	int marshal_window(const talk_proc::window* win, char* data)
	{
		char* p = data;

		*p = (char)win->id;
		p++;

		*p = (char)win->id_parent;
		p++;

		*(int*)p = win->talk_text_len;
		p += sizeof(int);

		if (win->talk_text_len)
		{
			size_t sz = sizeof(namechar) * win->talk_text_len;
			memcpy(p, win->talk_text, sz);
			p += sz;
		}

		*p = (char)win->num_option;
		p++;

		for (char i = 0; i < (char)win->num_option; i++)
			p += marshal_option(&win->options[i], p);

		return p - data;
	}

	int marshal_talk_proc(const talk_proc* talk, char* data)
	{
		char* p = data;

		size_t sz = wcslen(talk->text) * sizeof(namechar);
		*(int*)p = sz;
		p += sizeof(int);

		if (sz)
		{
			memcpy(p, talk->text, sz);
			p += sz;
		}

		*p = (char)talk->num_window;
		p++;

		for (char i = 0; i < (char)talk->num_window; i++)
			p += marshal_window(&talk->windows[i], p);

		return p - data;
	}

#endif

int unmarshal_option(talk_proc::option* opt, const char* data)
{
	const char* p = data;

	opt->id = *(int*)p;
	p += sizeof(int);

	opt->param = *(int*)p;
	p += sizeof(int);

	size_t sz = *(int*)p;
	p += sizeof(int);

	if (sz)
	{
		memcpy(opt->text, p, sz);
		p += sz;
	}

	return p - data;
}

int unmarshal_window(talk_proc::window* win, const char* data)
{
	const char* p = data;

	win->id = *p;
	p++;

	win->id_parent = *p;
	p++;

	win->talk_text_len = *(int*)p;
	p += sizeof(int);

	if (win->talk_text_len)
	{
		win->talk_text = new namechar[win->talk_text_len];
		size_t sz = sizeof(namechar) * win->talk_text_len;
		memcpy(win->talk_text, p, sz);
		p += sz;
		g_ulNewCount++;
	}

	win->num_option = *p;
	p++;

	if (win->num_option)
	{
		win->options = new talk_proc::option[win->num_option];
		memset(win->options, 0, sizeof(talk_proc::option) * win->num_option);
		g_ulNewCount++;

		for (int i = 0; i < win->num_option; i++)
			p += unmarshal_option(&win->options[i], p);
	}

	return p - data;
}

int unmarshal_talk_proc(talk_proc* talk, const char* data)
{
	const char* p = data;

	size_t sz = *(int*)p;
	p += sizeof(int);

	if (sz)
	{
		memcpy(talk->text, p, sz);
		p += sz;
	}

	talk->num_window = *p;
	p++;

	if (talk->num_window)
	{
		talk->windows = new talk_proc::window[talk->num_window];
		memset(talk->windows, 0, sizeof(talk_proc::window) * talk->num_window);
		g_ulNewCount++;

		for (int i = 0; i < talk->num_window; i++)
			p += unmarshal_window(&talk->windows[i], p);
	}

	return p - data;
}

inline void convert_txt(namechar* str, namechar code)
{
	int i = 0;

	while (str[i])
	{
		str[i] ^= code;
		i++;
	}
}

inline void convert_txt(namechar* str, int len, namechar code)
{
	int i = 0;

	for (; i < len; i++)
		str[i] ^= code;
}

inline void convert_talk_text(talk_proc* talk, namechar code)
{
	int i, j;

	convert_txt(talk->text, sizeof(talk->text) / sizeof(namechar), code);

	for (i = 0; i < talk->num_window; i++)
	{
		talk_proc::window* win = &talk->windows[i];
		convert_txt(win->talk_text, win->talk_text_len, code);

		for (j = 0; j < win->num_option; j++)
		{
			talk_proc::option* o = &win->options[j];
			convert_txt(o->text, sizeof(o->text) / sizeof(namechar), code);
		}
	}
}

static bool ReadLine(FILE* fp, char* szLine)
{
	if (fgets(szLine, TASK_MAX_LINE_LEN, fp) == NULL) return false;
	szLine[strcspn(szLine, "\r\n")] = '\0';
	return true;
}

inline bool LoadAwardCand(FILE* fp, AWARD_ITEMS_CAND& ic, unsigned long ulVersion)
{
	char line[TASK_MAX_LINE_LEN];
	unsigned long i;
	int nRead;

	ic.m_bRandChoose = false;
	if (ulVersion >= 26)
	{
		if (!ReadLine(fp, line)) return false;
		sscanf(line, _format_rand_one, &nRead);
		ic.m_bRandChoose = (nRead != 0);
	}

	ic.m_ulAwardItems = 0;
	ic.m_ulAwardCmnItems = 0;
	ic.m_ulAwardTskItems = 0;

	if (!ReadLine(fp, line)) return false;
	sscanf(line, _format_item_wanted_num, &ic.m_ulAwardItems);

#ifndef TASK_TEMPL_EDITOR

	if (ic.m_ulAwardItems)
	{
		ic.m_AwardItems = new ITEM_WANTED[ic.m_ulAwardItems];
		g_ulNewCount++;
	}

#endif

	for (i = 0; i < ic.m_ulAwardItems; i++)
	{
		ITEM_WANTED& iw = ic.m_AwardItems[i];
		int nBool = 0;

		if (!ReadLine(fp, line)) return false;
		sscanf(line, _format_item_templ, &iw.m_ulItemTemplId);

		if (!ReadLine(fp, line)) return false;
		sscanf(line, _format_item_is_cmn, &nBool);
		iw.m_bCommonItem = (nBool != 0);

		if (nBool) ic.m_ulAwardCmnItems++;
		else ic.m_ulAwardTskItems++;

		if (!ReadLine(fp, line)) return false;
		sscanf(line, _format_item_num, &iw.m_ulItemNum);

		if (ulVersion >= 8)
		{
			if (!ReadLine(fp, line)) return false;
			sscanf(line, _format_prob, &iw.m_fProb);
		}
		else
			iw.m_fProb = 1.0f;

		iw.m_lPeriod = 0;

		if (ulVersion >= 60)
		{
			if (!ReadLine(fp, line)) return false;
			sscanf(line, _format_period, &iw.m_lPeriod);
		}
	}

	return true;
}

inline bool LoadAwardCandBin(FILE* fp, AWARD_ITEMS_CAND& ic, unsigned long ulVersion)
{
	unsigned long i;

	ic.m_ulAwardItems = 0;
	ic.m_ulAwardCmnItems = 0;
	ic.m_ulAwardTskItems = 0;

	fread(&ic.m_bRandChoose, sizeof(ic.m_bRandChoose), 1, fp);
	fread(&ic.m_ulAwardItems, sizeof(ic.m_ulAwardItems), 1, fp);

	LOG_DELETE_ARR(ic.m_AwardItems);
	ic.m_AwardItems = NULL;

	if (ic.m_ulAwardItems)
	{
		ic.m_AwardItems = new ITEM_WANTED[ic.m_ulAwardItems];
		g_ulNewCount++;
	}

	for (i = 0; i < ic.m_ulAwardItems; i++)
	{
		ITEM_WANTED& iw = ic.m_AwardItems[i];
		fread(&iw, sizeof(iw), 1, fp);

		if (iw.m_bCommonItem) ic.m_ulAwardCmnItems++;
		else ic.m_ulAwardTskItems++;
	}

	return true;
}

inline bool LoadAwardMonsters(FILE* fp, AWARD_MONSTERS_SUMMONED& ms, unsigned long ulVersion)
{
	char line[TASK_MAX_LINE_LEN];
	unsigned long i;
	int nRead;

	ms.m_bRandChoose = false;
	if (ulVersion >= 64)
	{
		if (!ReadLine(fp, line)) return false;
		sscanf(line, _format_award_rand_monster, &nRead);
		ms.m_bRandChoose = (nRead != 0);

		if (!ReadLine(fp, line)) return false;
		sscanf(line, _format_award_summon_radius, &nRead);
		ms.m_ulSummonRadius = nRead;

		if (!ReadLine(fp, line)) return false;
		sscanf(line, _format_award_death_disappear, &nRead);
		ms.m_bDeathDisappear = (nRead != 0);

#ifndef TASK_TEMPL_EDITOR

		if (ms.m_ulMonsterNum)
		{
			ms.m_Monsters = new MONSTERS_SUMMONED[ms.m_ulMonsterNum];
			g_ulNewCount++;
		}
#endif
		for(i=0; i< ms.m_ulMonsterNum; i++)
		{
			MONSTERS_SUMMONED& monster = ms.m_Monsters[i];
			int nBool = 0;

			if (!ReadLine(fp, line)) return false;
			sscanf(line, _format_award_monster_id, &monster.m_ulMonsterTemplId);
			
			if (!ReadLine(fp, line)) return false;
			sscanf(line, _format_award_monster_num, &monster.m_ulMonsterNum);

			if (!ReadLine(fp, line)) return false;
			sscanf(line, _format_award_monster_prob, &monster.m_fSummonProb);

			if (!ReadLine(fp, line)) return false;
			sscanf(line, _format_award_monster_period, &monster.m_lPeriod);
		}
	}

	return true;
}

inline bool LoadAwardMonstersBin(FILE* fp, AWARD_MONSTERS_SUMMONED& ms, unsigned long ulVersion)
{
	unsigned long i;
	
	fread(&ms.m_bRandChoose, sizeof(ms.m_bRandChoose), 1, fp);
	fread(&ms.m_ulSummonRadius, sizeof(ms.m_ulSummonRadius), 1, fp);
	fread(&ms.m_bDeathDisappear, sizeof(ms.m_bDeathDisappear), 1, fp);

	LOG_DELETE_ARR(ms.m_Monsters);
	ms.m_Monsters = NULL;

	if(ms.m_ulMonsterNum)
	{
		ms.m_Monsters = new MONSTERS_SUMMONED[ms.m_ulMonsterNum];
		g_ulNewCount++;
	}

	for (i=0; i < ms.m_ulMonsterNum; i++)
	{
		MONSTERS_SUMMONED& monster = ms.m_Monsters[i];
		fread(&monster, sizeof(monster), 1, fp);
	}

	return true;
}

inline bool LoadAwardPQRanking(FILE* fp, AWARD_PQ_RANKING& pr, unsigned long ulVersion)
{
	char line[TASK_MAX_LINE_LEN];
	unsigned long i;
	int nRead;

	pr.m_bAwardByProf = false;
	if (ulVersion >= 71)
	{
		if (!ReadLine(fp, line)) return false;
		sscanf(line, _format_award_pq_by_prof, &nRead);
		pr.m_bAwardByProf = (nRead != 0);

#ifndef TASK_TEMPL_EDITOR

		if (pr.m_ulRankingAwardNum)
		{
			pr.m_RankingAward = new RANKING_AWARD[pr.m_ulRankingAwardNum];
			g_ulNewCount++;
		}
#endif
		for(i=0; i< pr.m_ulRankingAwardNum; i++)
		{
			RANKING_AWARD& rAward = pr.m_RankingAward[i];
			int nBool = 0;

			if (!ReadLine(fp, line)) return false;
			sscanf(line, _format_award_pq_ranking, &rAward.m_iRankingStart, &rAward.m_iRankingEnd, &rAward.m_ulAwardItemId, &nRead,  &rAward.m_ulAwardItemNum, &rAward.m_lPeriod);
			rAward.m_bCommonItem = (nRead != 0);
		}
	}

	return true;
}

inline bool LoadAwardPQRankingBin(FILE* fp, AWARD_PQ_RANKING& pr, unsigned long ulVersion)
{
	unsigned long i;
	
	fread(&pr.m_bAwardByProf, sizeof(pr.m_bAwardByProf), 1, fp);

	LOG_DELETE_ARR(pr.m_RankingAward);
	pr.m_RankingAward = NULL;

	if(pr.m_ulRankingAwardNum)
	{
		pr.m_RankingAward = new RANKING_AWARD[pr.m_ulRankingAwardNum];
		g_ulNewCount++;
	}

	for (i=0; i < pr.m_ulRankingAwardNum; i++)
	{
		RANKING_AWARD& rAward = pr.m_RankingAward[i];
		fread(&rAward, sizeof(rAward), 1, fp);
	}

	return true;
}


inline bool LoadAwardData(FILE* fp, AWARD_DATA& ad, unsigned long ulVersion)
{
	char line[TASK_MAX_LINE_LEN];
	unsigned long i;

	if (ulVersion >= 97)
	{
		if (!ReadLine(fp,line)) return false;
		sscanf(line, _format_award_force_activity, &ad.m_iForceActivity);
		if (!ReadLine(fp,line)) return false;
		sscanf(line, _format_award_force_contrib, &ad.m_iForceContribution);
		if (!ReadLine(fp,line)) return false;
		sscanf(line, _format_award_force_repu, &ad.m_iForceReputation);
	}

	if (ulVersion >= 100)
	{
		if (!ReadLine(fp,line)) return false;
		sscanf(line, _format_award_force_set_repu, &ad.m_iForceSetRepu);
	}

	if (ulVersion >= 101)
	{
		if (!ReadLine(fp,line)) return false;
		sscanf(line, _format_award_set_task_limit, &ad.m_iTaskLimit);
	}

	if (ulVersion >= 109)
	{
		if (!ReadLine(fp,line)) return false;
		sscanf(line, _format_award_title_num, &ad.m_ulTitleNum);
#ifndef TASK_TEMPL_EDITOR
		if (ad.m_ulTitleNum)
		{
			ad.m_pTitleAward = new TITLE_AWARD[ad.m_ulTitleNum];
			g_ulNewCount++;
		}
#endif
		for (i = 0; i < ad.m_ulTitleNum; i++)
		{
			if (!ReadLine(fp,line)) return false;
			sscanf(line, _format_award_title_id, &ad.m_pTitleAward[i].m_ulTitleID);
			if (!ReadLine(fp,line)) return false;
			sscanf(line, _format_award_title_period, &ad.m_pTitleAward[i].m_lPeriod);
		}
	}
	if (ulVersion >= 115)
	{
		if (!ReadLine(fp,line)) return false;
		sscanf(line, _format_award_leadership, &ad.m_iLeaderShip);
	}
	if (ulVersion >= 119)
	{
		if (!ReadLine(fp,line)) return false;
		sscanf(line, _format_award_worldcontrib, &ad.m_iWorldContribution);
	}
	if (ulVersion >= 87)
	{
		if (!ReadLine(fp,line)) return false;
		sscanf(line, _format_award_faction_contrib, &ad.m_iFactionContrib);
	}

	if (ulVersion >= 89)
	{
		if (!ReadLine(fp,line)) return false;
		sscanf(line, _format_award_faction_exp_contrib, &ad.m_iFactionExpContrib);
	}

	if (!ReadLine(fp, line)) return false;
	sscanf(line, _format_award_gold_num, &ad.m_ulGoldNum);

	if (!ReadLine(fp, line)) return false;
	sscanf(line, _format_award_exp_num, &ad.m_ulExp);

	if (ulVersion >= 113) {
		if (!ReadLine(fp, line)) return false;
		sscanf(line, _format_award_realm_exp, &ad.m_ulRealmExp);		
		int nBool = 0;
		if (!ReadLine(fp, line)) return false;
		sscanf(line, _format_award_realmlevel_max, &nBool);
		ad.m_bExpandRealmLevelMax = (nBool != 0);
	}


	if (ulVersion < 7)
		ad.m_ulCandItems = 1;
	else
	{
		if (!ReadLine(fp, line)) return false;
		sscanf(line, _format_award_cands_num, &ad.m_ulCandItems);
	}

#ifndef TASK_TEMPL_EDITOR

	if (ad.m_ulCandItems)
	{
		ad.m_CandItems = new AWARD_ITEMS_CAND[ad.m_ulCandItems];
		g_ulNewCount++;
	}
	
#endif

	for (i = 0; i < ad.m_ulCandItems; i++)
		LoadAwardCand(
			fp,
			ad.m_CandItems[i],
			ulVersion);


	if (ulVersion < 64)
		ad.m_ulSummonedMonsters = 0;
	else
	{
		if (!ReadLine(fp, line)) return false;
		sscanf(line, _format_award_monster_cand, &ad.m_ulSummonedMonsters);
	}

#ifndef TASK_TEMPL_EDITOR

	if (ad.m_ulSummonedMonsters)
	{
		ad.m_SummonedMonsters = new AWARD_MONSTERS_SUMMONED;
		g_ulNewCount++;
	}

#endif
	if (ulVersion >= 64 )
	{
		if( ad.m_ulSummonedMonsters)
		{
			ad.m_SummonedMonsters->m_ulMonsterNum = ad.m_ulSummonedMonsters;
			LoadAwardMonsters(
				fp,
				*ad.m_SummonedMonsters,
				ulVersion);
		}
	}

	if (ulVersion < 71)
		ad.m_ulPQRankingAwardCnt = 0;
	else
	{
		if (!ReadLine(fp, line)) return false;
		sscanf(line, _format_award_pq_ranking_cnt, &ad.m_ulPQRankingAwardCnt);
	}

#ifndef TASK_TEMPL_EDITOR

	if (ad.m_ulPQRankingAwardCnt)
	{
		ad.m_PQRankingAward = new AWARD_PQ_RANKING;
		g_ulNewCount++;
	}

#endif
	if (ulVersion >= 71 )
	{
		if( ad.m_ulPQRankingAwardCnt)
		{
			ad.m_PQRankingAward->m_ulRankingAwardNum = ad.m_ulPQRankingAwardCnt;
			LoadAwardPQRanking(
				fp,
				*ad.m_PQRankingAward,
				ulVersion);
		}
	}

	if (ulVersion >= 58)
	{
		if (!ReadLine(fp, line)) return false;
		sscanf(line, _format_award_key_cnt, &ad.m_ulChangeKeyCnt);
		
		if (ad.m_ulChangeKeyCnt)
		{
			ad.m_plChangeKey      =	new long[TASK_AWARD_MAX_CHANGE_VALUE];
			ad.m_plChangeKeyValue = new long[TASK_AWARD_MAX_CHANGE_VALUE];
			ad.m_pbChangeType     = new bool[TASK_AWARD_MAX_CHANGE_VALUE];
		}
		
		for (i=0; i<ad.m_ulChangeKeyCnt; i++)
		{
			if (!ReadLine(fp, line)) return false;
			sscanf(line, _format_award_change_key, &ad.m_plChangeKey[i]);
			
			if (!ReadLine(fp, line)) return false;
			sscanf(line, _format_award_change_kv, &ad.m_plChangeKeyValue[i]);
			
			int nBool = 0;
			if (!ReadLine(fp, line)) return false;
			sscanf(line, _format_award_change_typ, &nBool);
			ad.m_pbChangeType[i] = (nBool != 0);
		}

		int nBool = 0;
		if (!ReadLine(fp, line)) return false;
		sscanf(line, _format_award_multi, &nBool);
		ad.m_bMulti = (nBool != 0);
		
		if (!ReadLine(fp, line)) return false;
		sscanf(line, _format_award_num_type, &ad.m_nNumType);
		
		if (!ReadLine(fp, line)) return false;
		sscanf(line, _format_award_num, &ad.m_lNum);
	}

	if (ulVersion >= 112)
	{
		if (!ReadLine(fp, line)) return false;
		sscanf(line, _format_award_historykey_cnt, &ad.m_ulHistoryChangeCnt);
		
		if (ad.m_ulHistoryChangeCnt)
		{
			ad.m_plHistoryChangeKey      =	new long[TASK_AWARD_MAX_CHANGE_VALUE];
			ad.m_plHistoryChangeKeyValue = new long[TASK_AWARD_MAX_CHANGE_VALUE];
			ad.m_pbHistoryChangeType     = new bool[TASK_AWARD_MAX_CHANGE_VALUE];
		}
		
		for (i=0; i<ad.m_ulHistoryChangeCnt; i++)
		{
			if (!ReadLine(fp, line)) return false;
			sscanf(line, _format_award_historychange_key, &ad.m_plHistoryChangeKey[i]);
			
			if (!ReadLine(fp, line)) return false;
			sscanf(line, _format_award_historychange_kv, &ad.m_plHistoryChangeKeyValue[i]);
			
			int nBool = 0;
			if (!ReadLine(fp, line)) return false;
			sscanf(line, _format_award_historychange_typ, &nBool);
			ad.m_pbHistoryChangeType[i] = (nBool != 0);
		}
	}


	if (ulVersion >= 59)
	{		
		if (!ReadLine(fp, line)) return false;
		sscanf(line, _format_award_dis_keycnt, &ad.m_ulDisplayKeyCnt);
		
		if (ad.m_ulDisplayKeyCnt)
		{
			ad.m_plDisplayKey = new long[TASK_AWARD_MAX_DISPLAY_VALUE];
		}
		
		for (i=0; i<ad.m_ulDisplayKeyCnt; i++)
		{
			if (!ReadLine(fp, line)) return false;
			sscanf(line, _format_award_dis_key, &ad.m_plDisplayKey[i]);
		}
	
	}

	if (ulVersion >= 61)
	{
		if (!ReadLine(fp, line)) return false;
		sscanf(line, _format_award_dis_expcnt, &ad.m_ulExpCnt);

		if (ad.m_ulExpCnt)
		{
			ad.m_pszExp = new char[TASK_AWARD_MAX_DISPLAY_EXP_CNT][TASK_AWARD_MAX_DISPLAY_CHAR_LEN];
			memset(ad.m_pszExp, 0, TASK_AWARD_MAX_DISPLAY_EXP_CNT*TASK_AWARD_MAX_DISPLAY_CHAR_LEN);

			ad.m_pExpArr = new TASK_EXPRESSION[TASK_AWARD_MAX_DISPLAY_EXP_CNT][TASK_AWARD_MAX_DISPLAY_CHAR_LEN];
			memset(ad.m_pExpArr, 0, sizeof(TASK_EXPRESSION)*TASK_AWARD_MAX_DISPLAY_EXP_CNT*TASK_AWARD_MAX_DISPLAY_CHAR_LEN);
		}

		for (i=0; i<ad.m_ulExpCnt; i++)
		{
			fread(ad.m_pszExp[i], 1, TASK_AWARD_MAX_DISPLAY_CHAR_LEN, fp);
			if (!ReadLine(fp, line)) return false;
		}

		if (!ReadLine(fp, line)) return false;
		sscanf(line, _format_award_discharcnt, &ad.m_ulTaskCharCnt);
		
		if (ad.m_ulTaskCharCnt)
		{
			ad.m_pTaskChar = new task_char[TASK_AWARD_MAX_DISPLAY_EXP_CNT][TASK_AWARD_MAX_DISPLAY_CHAR_LEN];
			memset(ad.m_pTaskChar, 0, sizeof(task_char)*TASK_AWARD_MAX_DISPLAY_EXP_CNT*TASK_AWARD_MAX_DISPLAY_CHAR_LEN);
		}

		for (i=0; i<ad.m_ulTaskCharCnt; i++)
		{
			fread(ad.m_pTaskChar[i], sizeof(task_char), TASK_AWARD_MAX_DISPLAY_CHAR_LEN, fp);
			if (!ReadLine(fp, line)) return false;
		}		
	}

	if (!ReadLine(fp, line)) return false;
	sscanf(line, _format_award_sp_num, &ad.m_ulSP);

	if (!ReadLine(fp, line)) return false;
	sscanf(line, _format_award_repu, &ad.m_lReputation);

	if (ulVersion >= 25)
	{
		if (!ReadLine(fp, line)) return false;
		sscanf(line, _format_award_period, &ad.m_ulNewPeriod);
	}

	if (ulVersion >= 33)
	{
		if (!ReadLine(fp, line)) return false;
		sscanf(line, _format_award_station, &ad.m_ulNewRelayStation);
	}

	if (ulVersion >= 36)
	{
		if (!ReadLine(fp, line)) return false;
		sscanf(line, _format_award_store, &ad.m_ulStorehouseSize);
	}

	if (ulVersion >= 57)
	{
		if (!ReadLine(fp, line)) return false;
		sscanf(line, _format_award_store, &ad.m_ulStorehouseSize2);

		if (!ReadLine(fp, line)) return false;
		sscanf(line, _format_award_store, &ad.m_ulStorehouseSize3);
	}

	if (ulVersion >= 72)
	{
		if (!ReadLine(fp, line)) return false;
		sscanf(line, _format_award_store, &ad.m_ulStorehouseSize4);

		if (!ReadLine(fp, line)) return false;
		sscanf(line, _format_award_pq_specify_id, &ad.m_ulSpecifyContribTaskID);

		if (!ReadLine(fp, line)) return false;
		sscanf(line, _format_award_pq_specify_contrib, &ad.m_ulSpecifyContrib);

		if (!ReadLine(fp, line)) return false;
		sscanf(line, _format_award_pq_contrib, &ad.m_ulContrib);

		if (!ReadLine(fp, line)) return false;
		sscanf(line, _format_award_pq_rand_contrib, &ad.m_ulRandContrib);

		if (!ReadLine(fp, line)) return false;
		sscanf(line, _format_award_pq_lowest_contrib, &ad.m_ulLowestcontrib);
	}

	if (ulVersion >= 52)
	{
		if (!ReadLine(fp, line)) return false;
		sscanf(line, _format_award_invent, &ad.m_lInventorySize);
	}

	if (ulVersion >= 45)
	{
		if (!ReadLine(fp, line)) return false;
		sscanf(line, _format_award_pet, &ad.m_ulPetInventorySize);
	}

	if (ulVersion >= 37)
	{
		if (!ReadLine(fp, line)) return false;
		sscanf(line, _format_award_fury, &ad.m_ulFuryULimit);
	}

	if (ulVersion >= 38)
	{
		if (!ReadLine(fp, line)) return false;
		sscanf(line, _format_world_id, &ad.m_ulTransWldId);

		if (!ReadLine(fp, line)) return false;
		sscanf(line, _format_zone_vert, VERT_ZONE_ADDR(ad.m_TransPt));
	}

	if (ulVersion >= 48)
	{
		if (!ReadLine(fp, line)) return false;
		sscanf(line, _format_controller, &ad.m_lMonsCtrl);

		int nRead = 0;
		if (!ReadLine(fp, line)) return false;
		sscanf(line, _format_trig_flag, &nRead);
		ad.m_bTrigCtrl = (nRead != 0);
	}

	if (!ReadLine(fp, line)) return false;
	sscanf(line, _format_award_new_task, &ad.m_ulNewTask);

	if (ulVersion >= 54)
	{
		int nRead = 0;
		if (!ReadLine(fp, line)) return false;
		sscanf(line, _format_use_lev_co, &nRead);
		ad.m_bUseLevCo = (nRead != 0);
	}

	if (ulVersion >= 55)
	{
		int nBool = 0;
		if (!ReadLine(fp, line)) return false;
		sscanf(line, _format_award_divorce, &nBool);
		ad.m_bDivorce = (nBool != 0);
	}

	if (ulVersion >= 56)
	{
		int nBool;
		if (!ReadLine(fp, line)) return false;
		sscanf(line, _format_award_msg, &nBool);
		ad.m_bSendMsg = (nBool != 0);

		if (!ReadLine(fp, line)) return false;
		sscanf(line, _format_award_channel, &ad.m_nMsgChannel);
	}

	if (ulVersion >= 69)
	{
		int nBool;
		if (!ReadLine(fp, line)) return false;
		sscanf(line, _format_award_death, &nBool);
		ad.m_bAwardDeath = (nBool != 0);

		if (!ReadLine(fp, line)) return false;
		sscanf(line, _format_award_death_loss, &nBool);
		ad.m_bAwardDeathWithLoss = (nBool != 0);
	}

	if (ulVersion >= 74)
	{
		if (!ReadLine(fp, line)) return false;
		sscanf(line, _format_award_dividend, &ad.m_ulDividend);
	}

	if (ulVersion >=76)
	{
		if(!ReadLine(fp,line)) return false;
		sscanf(line,_format_award_pq_specify_sub_taskid,&ad.m_ulSpecifyContribSubTaskID);
	}

	if (ulVersion >= 77)
	{
		int nRead =0;
		if(!ReadLine(fp,line)) return false;
		sscanf(line,_format_award_DeliverySkill,&nRead);
		ad.m_bAwardSkill = (nRead != 0);

		if(!ReadLine(fp,line)) return false;
		sscanf(line,_format_award_DeliverySkillID,&ad.m_iAwardSkillID);

		if(!ReadLine(fp,line)) return false;
		sscanf(line,_format_award_DeliverySkillLevel,&ad.m_iAwardSkillLevel);
	}

	return true;
}

inline bool LoadAwardDataBin(FILE* fp, AWARD_DATA& ad, unsigned long ulVersion)
{
	unsigned long i;

	LOG_DELETE_ARR(ad.m_CandItems);
	ad.m_CandItems = NULL;

	LOG_DELETE_ARR(ad.m_pTitleAward);
	ad.m_pTitleAward = NULL;

	LOG_DELETE(ad.m_SummonedMonsters);
	ad.m_SummonedMonsters = NULL;

	LOG_DELETE(ad.m_PQRankingAward);
	ad.m_PQRankingAward= NULL;


	LOG_DELETE_ARR(ad.m_plChangeKey);
	ad.m_plChangeKey = NULL;
	
	LOG_DELETE_ARR(ad.m_plChangeKeyValue);
	ad.m_plChangeKeyValue = NULL;
	
	LOG_DELETE_ARR(ad.m_pbChangeType);
	ad.m_pbChangeType = NULL;

	LOG_DELETE_ARR(ad.m_plHistoryChangeKey);
	ad.m_plHistoryChangeKey = NULL;
	
	LOG_DELETE_ARR(ad.m_plHistoryChangeKeyValue);
	ad.m_plHistoryChangeKeyValue = NULL;
	
	LOG_DELETE_ARR(ad.m_pbHistoryChangeType);
	ad.m_pbHistoryChangeType = NULL;

	LOG_DELETE_ARR(ad.m_plDisplayKey);
	ad.m_plDisplayKey = NULL;
		
	LOG_DELETE_ARR(ad.m_pszExp);
	ad.m_pszExp = NULL;
	
	LOG_DELETE_ARR(ad.m_pExpArr);
	ad.m_pExpArr = NULL;

	LOG_DELETE_ARR(ad.m_pTaskChar);
	ad.m_pTaskChar = NULL;
	
	fread(&ad, sizeof(ad), 1, fp);

	if (ad.m_ulCandItems)
	{
		ad.m_CandItems = new AWARD_ITEMS_CAND[ad.m_ulCandItems];
		g_ulNewCount++;
	}
	else
		ad.m_CandItems = NULL;

	if (ad.m_ulTitleNum)
	{
		ad.m_pTitleAward = new TITLE_AWARD[ad.m_ulTitleNum];
		g_ulNewCount++;
	}
	else ad.m_pTitleAward = NULL;

	if (ad.m_ulSummonedMonsters)
	{
		ad.m_SummonedMonsters = new AWARD_MONSTERS_SUMMONED;
		g_ulNewCount++;
	}
	else
		ad.m_SummonedMonsters = NULL;

	if (ad.m_ulPQRankingAwardCnt)
	{
		ad.m_PQRankingAward = new AWARD_PQ_RANKING;
		g_ulNewCount++;
	}
	else
		ad.m_PQRankingAward = NULL;


	for (i = 0; i < ad.m_ulCandItems; i++)
		LoadAwardCandBin(
			fp,
			ad.m_CandItems[i],
			ulVersion);

	if(ad.m_ulSummonedMonsters)
	{
		ad.m_SummonedMonsters->m_ulMonsterNum = ad.m_ulSummonedMonsters;
		LoadAwardMonstersBin(
			fp,
			*ad.m_SummonedMonsters,
			ulVersion);
	}

	if(ad.m_ulPQRankingAwardCnt)
	{
		ad.m_PQRankingAward->m_ulRankingAwardNum = ad.m_ulPQRankingAwardCnt;
		LoadAwardPQRankingBin(
			fp,
			*ad.m_PQRankingAward,
			ulVersion);
	}
	
	for (i = 0; i < ad.m_ulTitleNum; ++i)
	{
		fread(&ad.m_pTitleAward[i], sizeof(TITLE_AWARD), 1, fp);
	}
	if (ad.m_ulChangeKeyCnt)
	{
		ad.m_plChangeKey      =	new long[ad.m_ulChangeKeyCnt];
		ad.m_plChangeKeyValue = new long[ad.m_ulChangeKeyCnt];
		ad.m_pbChangeType     = new bool[ad.m_ulChangeKeyCnt];
	}
	
	for (i=0; i<ad.m_ulChangeKeyCnt; i++)
	{
		fread(&ad.m_plChangeKey[i], sizeof(long), 1, fp);
		fread(&ad.m_plChangeKeyValue[i], sizeof(long), 1, fp);
		fread(&ad.m_pbChangeType[i], sizeof(bool), 1, fp);
	}

	if (ad.m_ulHistoryChangeCnt)
	{
		ad.m_plHistoryChangeKey      =	new long[ad.m_ulHistoryChangeCnt];
		ad.m_plHistoryChangeKeyValue = new long[ad.m_ulHistoryChangeCnt];
		ad.m_pbHistoryChangeType     = new bool[ad.m_ulHistoryChangeCnt];
	}
	for (i=0; i<ad.m_ulHistoryChangeCnt; i++)
	{
		fread(&ad.m_plHistoryChangeKey[i], sizeof(long), 1, fp);
		fread(&ad.m_plHistoryChangeKeyValue[i], sizeof(long), 1, fp);
		fread(&ad.m_pbHistoryChangeType[i], sizeof(bool), 1, fp);
	}

	if (ad.m_ulDisplayKeyCnt)
	{
		ad.m_plDisplayKey = new long[ad.m_ulDisplayKeyCnt];		
		
		for (i=0; i<ad.m_ulDisplayKeyCnt; i++)
		{
			fread(&ad.m_plDisplayKey[i], sizeof(long), 1, fp);
		}
	}
	
	if (ad.m_ulExpCnt)
	{
		ad.m_pszExp = new char[ad.m_ulExpCnt][TASK_AWARD_MAX_DISPLAY_CHAR_LEN];
		memset(ad.m_pszExp, 0, ad.m_ulExpCnt*TASK_AWARD_MAX_DISPLAY_CHAR_LEN);

		ad.m_pExpArr = new TASK_EXPRESSION[ad.m_ulExpCnt][TASK_AWARD_MAX_DISPLAY_CHAR_LEN];
		memset(ad.m_pExpArr, 0, sizeof(TASK_EXPRESSION)*ad.m_ulExpCnt*TASK_AWARD_MAX_DISPLAY_CHAR_LEN);
		
		for (i=0; i<ad.m_ulExpCnt; i++)
		{	
			fread(ad.m_pszExp[i], 1, TASK_AWARD_MAX_DISPLAY_CHAR_LEN, fp);
			fread(ad.m_pExpArr[i], sizeof(TASK_EXPRESSION), TASK_AWARD_MAX_DISPLAY_CHAR_LEN, fp);
		}
	}

	if (ad.m_ulTaskCharCnt)
	{
		ad.m_pTaskChar = new task_char[ad.m_ulTaskCharCnt][TASK_AWARD_MAX_DISPLAY_CHAR_LEN];
		memset(ad.m_pTaskChar, 0, sizeof(task_char)*ad.m_ulTaskCharCnt*TASK_AWARD_MAX_DISPLAY_CHAR_LEN);

		for (i=0; i<ad.m_ulTaskCharCnt; i++)
		{	
			fread(ad.m_pTaskChar[i], sizeof(task_char), TASK_AWARD_MAX_DISPLAY_CHAR_LEN, fp);
		}
	}
	
	return true;
}

inline bool LoadAwardDataRatioScale(FILE* fp, AWARD_RATIO_SCALE& ad, unsigned long ulVersion)
{
	fread(&ad.m_ulScales, sizeof(ad.m_ulScales), 1, fp);
	fread(ad.m_Ratios, sizeof(ad.m_Ratios), 1, fp);

	LOG_DELETE_ARR(ad.m_Awards);
	ad.m_Awards = NULL;

	if (ad.m_ulScales)
	{
		ad.m_Awards = new AWARD_DATA[ad.m_ulScales];
		g_ulNewCount++;
	}

	for (unsigned long i = 0; i < ad.m_ulScales; i++)
		LoadAwardDataBin(fp, ad.m_Awards[i], ulVersion);

	return true;
}

inline bool LoadAwardDataItemsScale(FILE* fp, AWARD_ITEMS_SCALE& ad, unsigned long ulVersion)
{
	fread(&ad.m_ulScales, sizeof(ad.m_ulScales), 1, fp);
	fread(&ad.m_ulItemId, sizeof(ad.m_ulItemId), 1, fp);
	fread(ad.m_Counts, sizeof(ad.m_Counts), 1, fp);

	LOG_DELETE_ARR(ad.m_Awards);
	ad.m_Awards = NULL;

	if (ad.m_ulScales)
	{
		ad.m_Awards = new AWARD_DATA[ad.m_ulScales];
		g_ulNewCount++;
	}

	for (unsigned long i = 0; i < ad.m_ulScales; i++)
		LoadAwardDataBin(fp, ad.m_Awards[i], ulVersion);

	return true;
}

bool ATaskTemplFixedData::LoadFixedDataFromTextFile(FILE* fp, unsigned long ulVersion)
{
	char line[TASK_MAX_LINE_LEN];
	int nRead;
	unsigned long i;

	/* 任务对象的属性 */

	if (!ReadLine(fp, line)) return false;
	sscanf(line, _format_id, &m_ID);

	fread(m_szName, sizeof(task_char), MAX_TASK_NAME_LEN, fp);
	if (!ReadLine(fp, line)) return false;

	if (ulVersion >= 43)
	{
		if (!ReadLine(fp, line)) return false;
		sscanf(line, _format_dyn, &nRead);
		m_DynTaskType = nRead;
	}

	if (ulVersion >= 41)
	{
		if (!ReadLine(fp, line)) return false;
		sscanf(line, _format_type, &m_ulType);
	}

	if (ulVersion >= 40)
	{
		if (!ReadLine(fp, line)) return false;
		sscanf(line, _format_has_sign, &nRead);
		m_bHasSign = (nRead != 0);

		if (m_bHasSign)
		{
			if (!m_pszSignature)
			{
				m_pszSignature = new task_char[MAX_TASK_NAME_LEN];
				g_ulNewCount++;
			}

			fread(m_pszSignature, sizeof(task_char), MAX_TASK_NAME_LEN, fp);
			if (!ReadLine(fp, line)) return false;
		}
	}

	if (!ReadLine(fp, line)) return false;
	sscanf(line, _format_time_limit, &m_ulTimeLimit);

	if (ulVersion >= 68)
	{
		if (!ReadLine(fp, line)) return false;
		sscanf(line, _format_offline_fail,  &m_bOfflineFail);

		if (!ReadLine(fp, line)) return false;
		sscanf(line, _format_abs_fail_time,  &m_bAbsFail);

		if (!ReadLine(fp, line)) return false;
		sscanf(line, _format_full_tm_old, &m_tmAbsFailTime.year, &m_tmAbsFailTime.month, &m_tmAbsFailTime.day, &m_tmAbsFailTime.hour, &m_tmAbsFailTime.min);

		if (!ReadLine(fp, line)) return false;
		sscanf(line, _format_item_not_take_off,  &m_bItemNotTakeOff);
	}

	if (ulVersion >= 52)
	{
		if (!ReadLine(fp, line)) return false;
		sscanf(line, _format_avail_frequency, &m_lAvailFrequency);
	}

	if (ulVersion >= 50)
	{
		if (!ReadLine(fp, line)) return false;
		sscanf(line, _format_abs_time, &nRead);
		m_bAbsTime = (nRead != 0);
	}

	if (ulVersion >= 2)
	{
		if (!ReadLine(fp, line)) return false;
		sscanf(line, _format_count, &m_ulTimetable);

#ifndef TASK_TEMPL_EDITOR
		if (m_ulTimetable)
		{
			m_tmStart	= new task_tm[m_ulTimetable];
			g_ulNewCount++;
			m_tmEnd		= new task_tm[m_ulTimetable];
			g_ulNewCount++;
		}
#endif

		for (i = 0; i < m_ulTimetable; i++)
		{
			task_tm& s = m_tmStart[i];
			task_tm& e = m_tmEnd[i];

			if (ulVersion >= 47)
			{
				int n;

				if (!ReadLine(fp, line)) return false;
				sscanf(line, _format_tm_type, &n);
				m_tmType[i] = (char)n;

				if (!ReadLine(fp, line)) return false;
				sscanf(line, _format_full_tm, &s.year, &s.month, &s.day, &s.hour, &s.min, &s.wday);

				if (!ReadLine(fp, line)) return false;
				sscanf(line, _format_full_tm, &e.year, &e.month, &e.day, &e.hour, &e.min, &e.wday);
			}
			else
			{
				m_tmType[i] = (char)enumTaskTimeDate;

				if (!ReadLine(fp, line)) return false;
				sscanf(line, _format_full_tm_old, &s.year, &s.month, &s.day, &s.hour, &s.min);
				s.wday = 0;

				if (!ReadLine(fp, line)) return false;
				sscanf(line, _format_full_tm_old, &e.year, &e.month, &e.day, &e.hour, &e.min);
				e.wday = 0;
			}
		}
	}

	if (!ReadLine(fp, line)) return false;
	sscanf(line, _format_child_in_order, &nRead);
	m_bExeChildInOrder = (nRead != 0);

	if (ulVersion >= 10)
	{
		if (!ReadLine(fp, line)) return false;
		sscanf(line, _format_choose_one, &nRead);
		m_bChooseOne = (nRead != 0);
	}

	if (ulVersion >= 17)
	{
		if (!ReadLine(fp, line)) return false;
		sscanf(line, _format_rand_one, &nRead);
		m_bRandOne = (nRead != 0);
	}

	if (!ReadLine(fp, line)) return false;
	sscanf(line, _format_parent_fail, &nRead);
	m_bParentAlsoFail = (nRead != 0);

	if (!ReadLine(fp, line)) return false;
	sscanf(line, _format_parent_succ, &nRead);
	m_bParentAlsoSucc = (nRead != 0);

	if (!ReadLine(fp, line)) return false;
	sscanf(line, _format_can_give_up, &nRead);
	m_bCanGiveUp = (nRead != 0);

	if (!ReadLine(fp, line)) return false;
	sscanf(line, _format_die_as_fail, &nRead);
	m_bFailAsPlayerDie = (nRead != 0);

	if (!ReadLine(fp, line)) return false;
	sscanf(line, _format_can_redo, &nRead);
	m_bCanRedo = (nRead != 0);

	if (!ReadLine(fp, line)) return false;
	sscanf(line, _format_redo_after_fail, &nRead);
	m_bCanRedoAfterFailure = (nRead != 0);

	if (!ReadLine(fp, line)) return false;
	sscanf(line, _format_clear_as_giveup, &nRead);
	m_bClearAsGiveUp = (nRead != 0);

	if (!ReadLine(fp, line)) return false;
	sscanf(line, _format_need_record, &nRead);
	m_bNeedRecord = (nRead != 0);

	if (!ReadLine(fp, line)) return false;
	sscanf(line, _format_max_receiver, &m_ulMaxReceiver);

	if (ulVersion >= 11)
	{
		if (!ReadLine(fp, line)) return false;
		sscanf(line, _format_must_in_zone, &nRead);
		m_bDelvInZone = (nRead != 0);

		if (ulVersion >= 35)
		{
			if (!ReadLine(fp, line)) return false;
			sscanf(line, _format_world_id, &m_ulDelvWorld);
		}

		// version 80
		if (ulVersion >=80)
		{
			if (!ReadLine(fp, line)) return false;
			sscanf(line, _format_taskRegionCnt, &m_ulDelvRegionCnt);
		}
		else
		{
			if(m_bDelvInZone)
				m_ulDelvRegionCnt = 1;
			else
			{
				// dummy read
				Task_Region t;
				
				if (!ReadLine(fp, line)) return false;
				sscanf(line, _format_zone_vert, VERT_ZONE_ADDR(t.zvMin));
				
				if (!ReadLine(fp, line)) return false;
				sscanf(line, _format_zone_vert, VERT_ZONE_ADDR(t.zvMax));
			}
		}
		
#ifndef TASK_TEMPL_EDITOR
		if(m_ulDelvRegionCnt>0)
			m_pDelvRegion = new Task_Region[m_ulDelvRegionCnt];
#endif

		for (i=0;i<m_ulDelvRegionCnt;i++)
		{
			Task_Region& t = m_pDelvRegion[i];
			
			if (!ReadLine(fp, line)) return false;
			sscanf(line, _format_zone_vert, VERT_ZONE_ADDR(t.zvMin));
			
			if (!ReadLine(fp, line)) return false;
			sscanf(line, _format_zone_vert, VERT_ZONE_ADDR(t.zvMax));
		}
	}
	
	if (ulVersion >= 67)
	{
		if (!ReadLine(fp, line)) return false;
		sscanf(line, _format_enter_region_fail, &nRead);
		m_bEnterRegionFail = (nRead != 0);

		if (!ReadLine(fp, line)) return false;
		sscanf(line, _format_world_id, &m_ulEnterRegionWorld);

		if (ulVersion >= 80)
		{
			if (!ReadLine(fp, line)) return false;
			sscanf(line, _format_taskRegionCnt, &m_ulEnterRegionCnt);
		}
		else
		{
			if(m_bEnterRegionFail)
				m_ulEnterRegionCnt = 1;
			else
			{
				Task_Region t;
				
				if (!ReadLine(fp, line)) return false;
				sscanf(line, _format_zone_vert, VERT_ZONE_ADDR(t.zvMin));
				
				if (!ReadLine(fp, line)) return false;
				sscanf(line, _format_zone_vert, VERT_ZONE_ADDR(t.zvMax));
			}
		}
#ifndef TASK_TEMPL_EDITOR
		if(m_ulEnterRegionCnt>0) m_pEnterRegion = new Task_Region[m_ulEnterRegionCnt];
#endif
		for (i=0;i<m_ulEnterRegionCnt;i++)
		{
			Task_Region& t = m_pEnterRegion[i];

			if (!ReadLine(fp, line)) return false;
			sscanf(line, _format_zone_vert, VERT_ZONE_ADDR(t.zvMin));
			
			if (!ReadLine(fp, line)) return false;
			sscanf(line, _format_zone_vert, VERT_ZONE_ADDR(t.zvMax));
		}

		if (!ReadLine(fp, line)) return false;
		sscanf(line, _format_leave_region_fail, &nRead);
		m_bLeaveRegionFail = (nRead != 0);

		if (!ReadLine(fp, line)) return false;
		sscanf(line, _format_world_id, &m_ulLeaveRegionWorld);

		if (ulVersion>=80)
		{
			if (!ReadLine(fp, line)) return false;
			sscanf(line, _format_taskRegionCnt, &m_ulLeaveRegionCnt);
		}
		else
		{
			if (m_bLeaveRegionFail)
				m_ulLeaveRegionCnt = 1;
			else
			{
				Task_Region t;
				
				if (!ReadLine(fp, line)) return false;
				sscanf(line, _format_zone_vert, VERT_ZONE_ADDR(t.zvMin));
				
				if (!ReadLine(fp, line)) return false;
				sscanf(line, _format_zone_vert, VERT_ZONE_ADDR(t.zvMax));
			}
		}
#ifndef TASK_TEMPL_EDITOR
		if(m_ulLeaveRegionCnt>0) m_pLeaveRegion = new Task_Region[m_ulLeaveRegionCnt];
#endif
		for (i=0;i<m_ulLeaveRegionCnt;i++)
		{
			Task_Region& t = m_pLeaveRegion[i];
			
			if (!ReadLine(fp, line)) return false;
			sscanf(line, _format_zone_vert, VERT_ZONE_ADDR(t.zvMin));
			
			if (!ReadLine(fp, line)) return false;
			sscanf(line, _format_zone_vert, VERT_ZONE_ADDR(t.zvMax));
		}		
	}

	if (ulVersion >= 97)
	{
		if (!ReadLine(fp, line)) return false;
		sscanf(line, _format_leave_force_fail, &nRead);
		m_bLeaveForceFail = (nRead != 0);
	}
	if (ulVersion >= 105)
	{
		if (!ReadLine(fp, line)) return false;
		sscanf(line, _format_leave_faction_fail, &nRead);
		m_bLeaveFactionFail = (nRead != 0);
	}
	if (ulVersion >= 106)
	{
		if (!ReadLine(fp, line)) return false;
		sscanf(line, _format_prem_is_king, &nRead);
		m_bPremIsKing = (nRead != 0);

		if (!ReadLine(fp, line)) return false;
		sscanf(line, _format_by_king, &nRead);
		m_bShowByKing = (nRead != 0);
	}
	if (ulVersion >= 107)
	{
		if (!ReadLine(fp, line)) return false;
		sscanf(line, _format_prem_not_in_team, &nRead);
		m_bPremNotInTeam = (nRead != 0);
		
		if (!ReadLine(fp, line)) return false;
		sscanf(line, _format_by_not_in_team, &nRead);
		m_bShowByNotInTeam = (nRead != 0);
	}
	if (ulVersion >= 108)
	{
		if (!ReadLine(fp, line)) return false;
		sscanf(line, _format_failed_not_take_item, &nRead);
		m_bNotClearItemWhenFailed = (nRead != 0);
	}
	if (ulVersion >= 109)
	{
		if (!ReadLine(fp, line)) return false;
		sscanf(line, _format_prem_title_total, &m_iPremTitleNumTotal);

		if (!ReadLine(fp, line)) return false;
		sscanf(line, _format_prem_title_required, &m_iPremTitleNumRequired);

		if (!ReadLine(fp, line)) return false;
		sscanf(line, _format_by_title, &nRead);
		m_bShowByTitle = (nRead != 0);
#ifndef TASK_TEMPL_EDITOR
		m_PremTitles = new int[m_iPremTitleNumTotal];
		g_ulNewCount++;
#endif
		for (unsigned int i = 0; i < m_iPremTitleNumTotal; i++)
		{
			if (!ReadLine(fp, line)) return false;
			sscanf(line, _format_prem_title_id, &m_PremTitles[i]);			
		}
	}
	if (ulVersion >= 110)
	{
		if (!ReadLine(fp, line)) return false;
		sscanf(line, _format_display_in_title_ui, &nRead);
		m_bDisplayInTitleTaskUI = (nRead != 0);
	}
	if (ulVersion >= 111)
	{
		if (!ReadLine(fp, line)) return false;
		sscanf(line, _format_failed_not_inc_count, &nRead);
		m_bNotIncCntWhenFailed = (nRead != 0);
	}
	if (ulVersion >= 38)
	{
		if (!ReadLine(fp, line)) return false;
		sscanf(line, _format_trans_to, &nRead);
		m_bTransTo = (nRead != 0);

		if (!ReadLine(fp, line)) return false;
		sscanf(line, _format_world_id, &m_ulTransWldId);

		if (!ReadLine(fp, line)) return false;
		sscanf(line, _format_zone_vert, VERT_ZONE_ADDR(m_TransPt));
	}

	if (ulVersion >= 48)
	{
		if (!ReadLine(fp, line)) return false;
		sscanf(line, _format_controller, &m_lMonsCtrl);

		if (!ReadLine(fp, line)) return false;
		sscanf(line, _format_trig_flag, &nRead);
		m_bTrigCtrl = (nRead != 0);
	}

	if (ulVersion >= 16)
	{
		if (!ReadLine(fp, line)) return false;
		sscanf(line, _format_auto_deliver, &nRead);
		m_bAutoDeliver = (nRead != 0);
	}

	if (ulVersion >= 20)
	{
		if (!ReadLine(fp, line)) return false;
		sscanf(line, _format_death_trig, &nRead);
		m_bDeathTrig = (nRead != 0);
	}

	if (ulVersion >= 24)
	{
		if (!ReadLine(fp, line)) return false;
		sscanf(line, _format_clear_acquired, &nRead);
		m_bClearAcquired = (nRead != 0);
	}

	if (ulVersion >= 27)
	{
		if (!ReadLine(fp, line)) return false;
		sscanf(line, _format_suit_lev, &m_ulSuitableLevel);

		if (!ReadLine(fp, line)) return false;
		sscanf(line, _format_show_prompt, &nRead);
		m_bShowPrompt = (nRead != 0);

		if (!ReadLine(fp, line)) return false;
		sscanf(line, _format_key_task, &nRead);
		m_bKeyTask = (nRead != 0);
	}

	if (ulVersion >= 29)
	{
		if (!ReadLine(fp, line)) return false;
		sscanf(line, _format_npc, &m_ulDelvNPC);

		if (!ReadLine(fp, line)) return false;
		sscanf(line, _format_npc, &m_ulAwardNPC);
	}

	if (ulVersion >= 31)
	{
		if (!ReadLine(fp, line)) return false;
		sscanf(line, _format_skill_task, &nRead);
		m_bSkillTask = (nRead != 0);
	}

	if (ulVersion >= 44)
	{
		nRead = 0;
		if (!ReadLine(fp, line)) return false;
		sscanf(line, _format_seek_out, &nRead);
		m_bCanSeekOut = (nRead != 0);
	}
	else
		m_bCanSeekOut = true;

	if (ulVersion >= 46)
	{
		nRead = 0;
		if (!ReadLine(fp, line)) return false;
		sscanf(line, _format_direction, &nRead);
		m_bShowDirection = (nRead != 0);
	}
	else
		m_bShowDirection = true;

	if (ulVersion >= 55)
	{
		nRead = 0;
		if (!ReadLine(fp, line)) return false;
		sscanf(line, _format_marriage, &nRead);
		m_bMarriage = (nRead != 0);
	}
	
	if (ulVersion >= 58)
	{
		if (!ReadLine(fp, line)) return false;
		sscanf(line, _format_prem_key_cnt, &m_ulChangeKeyCnt);
		
		if (m_ulChangeKeyCnt)
		{
			m_plChangeKey      = new long[TASK_AWARD_MAX_CHANGE_VALUE];
			m_plChangeKeyValue = new long[TASK_AWARD_MAX_CHANGE_VALUE];
			m_pbChangeType     = new bool[TASK_AWARD_MAX_CHANGE_VALUE];
		}
		
		for (i=0; i<m_ulChangeKeyCnt; i++)
		{
			if (!ReadLine(fp, line)) return false;
			sscanf(line, _format_prem_change_key, &m_plChangeKey[i]);
			
			if (!ReadLine(fp, line)) return false;
			sscanf(line, _format_prem_change_kv, &m_plChangeKeyValue[i]);
			
			int nBool = 0;
			if (!ReadLine(fp, line)) return false;
			sscanf(line, _format_prem_change_typ, &nBool);
			m_pbChangeType[i] = (nBool != 0);
		}
	}

	if (ulVersion >= 63)
	{
		nRead = 0;		
		if (!ReadLine(fp, line)) return false;
		sscanf(line, _format_switch_fail, &nRead);
		m_bSwitchSceneFail = (nRead != 0);

		if (!ReadLine(fp, line)) return false;
		sscanf(line, _format_hidden, &nRead);
		m_bHidden = (nRead != 0);
	}
	
	if (ulVersion >= 70)
	{
		nRead = 0;
		if (!ReadLine(fp, line)) return false;
		sscanf(line, _format_pq_task, &nRead);
		m_bPQTask = (nRead != 0);

		if (!ReadLine(fp, line)) return false;
		sscanf(line, _format_pq_task_exp_cnt, &nRead);
		m_ulPQExpCnt = nRead;

		if(m_ulPQExpCnt)
		{
			m_pszPQExp = new char[TASK_AWARD_MAX_DISPLAY_EXP_CNT][TASK_AWARD_MAX_DISPLAY_CHAR_LEN];
			memset(m_pszPQExp, 0, TASK_AWARD_MAX_DISPLAY_EXP_CNT*TASK_AWARD_MAX_DISPLAY_CHAR_LEN);

			m_pPQExpArr = new TASK_EXPRESSION[TASK_AWARD_MAX_DISPLAY_EXP_CNT][TASK_AWARD_MAX_DISPLAY_CHAR_LEN];
			memset(m_pPQExpArr, 0, sizeof(TASK_EXPRESSION)*TASK_AWARD_MAX_DISPLAY_EXP_CNT*TASK_AWARD_MAX_DISPLAY_CHAR_LEN);
		}

		for (i=0;i<m_ulPQExpCnt; i++)
		{
			fread(m_pszPQExp[i], 1, TASK_AWARD_MAX_DISPLAY_CHAR_LEN, fp);
			if (!ReadLine(fp, line)) return false;
		}


		nRead = 0;
		if (!ReadLine(fp, line)) return false;
		sscanf(line, _format_pq_sub_task, &nRead);
		m_bPQSubTask = (nRead != 0);

		if (!ReadLine(fp, line)) return false;
		sscanf(line, _format_pq_clear_contrib, &nRead);
		m_bClearContrib = (nRead != 0);

		if (!ReadLine(fp, line)) return false;
		sscanf(line, _format_pq_contrib_cnt, &nRead);
		m_ulMonsterContribCnt = nRead;

#ifndef TASK_TEMPL_EDITOR

		if(m_ulMonsterContribCnt)
		{
			m_MonstersContrib = new MONSTERS_CONTRIB[m_ulMonsterContribCnt];
			g_ulNewCount++;
		}

#endif

		for(i=0; i<m_ulMonsterContribCnt; i++)
		{
			int id, whole, share;
			if(!ReadLine(fp, line)) return false;
			sscanf(line, _format_pq_monster_contrib, &id, &whole, &share);
			m_MonstersContrib[i].m_ulMonsterTemplId = id;
			m_MonstersContrib[i].m_iWholeContrib = whole;
			m_MonstersContrib[i].m_iShareContrib = share;
			if (ulVersion >=82)
			{
				int personal;
				if(!ReadLine(fp,line)) return false;
				sscanf(line,_format_pq_monster_contrib_personal,&personal);
				m_MonstersContrib[i].m_iPersonalWholeContrib = personal;
			}
		}
	}

	if(ulVersion >= 74)
	{
		if (!ReadLine(fp, line)) return false;
		sscanf(line, _format_account_limit, &nRead);
		m_bAccountTaskLimit = (nRead != 0);

		if (!ReadLine(fp, line)) return false;
		sscanf(line, _format_account_limit_cnt, &nRead);
		m_ulAccountTaskLimitCnt = nRead;
	}

	if (ulVersion >= 115)
	{
		if (!ReadLine(fp, line)) return false;
		sscanf(line, _format_role_limit, &nRead);
		m_bRoleTaskLimit = (nRead != 0);	
	}
	if (ulVersion >= 77)
	{
		if(!ReadLine(fp,line)) return false;
		sscanf(line,_format_deliverySkill,&nRead);
		m_bDeliverySkill = (nRead !=0);

		if(!ReadLine(fp,line)) return false;
		sscanf(line,_format_deliveredSkillID,&m_iDeliveredSkillID);

		if(!ReadLine(fp,line)) return false;
		sscanf(line,_format_deliveredSkillLevel,&m_iDeliveredSkillLevel);
	}

	if (ulVersion >=78)
	{
		if(!ReadLine(fp,line)) return false;
		sscanf(line,_format_showGfxWhenFinished,&nRead);
		m_bShowGfxFinished = (nRead!=0);
	}

	if (ulVersion >=79)
	{
		if(!ReadLine(fp,line)) return false;
		sscanf(line,_format_changePQRanking,&nRead);
		m_bChangePQRanking = (nRead!=0);
	}

	if (ulVersion >=81)
	{
		if(!ReadLine(fp,line)) return false;
		sscanf(line,_format_compareItemAndInventory,&nRead);
		m_bCompareItemAndInventory = (nRead!=0);

		if(!ReadLine(fp,line)) return false;
		sscanf(line,_format_needInventorySlotNum,&m_ulInventorySlotNum);
	}

	if (ulVersion >=83)
	{
		if(!ReadLine(fp,line)) return false;
		sscanf(line,_format_is_library_task,&nRead);
		m_bIsLibraryTask = (nRead!=0);
		
		if(!ReadLine(fp,line)) return false;
		sscanf(line,_format_library_task_probability,&m_fLibraryTasksProbability);
	}
	if (ulVersion >= 95)
	{
		if(!ReadLine(fp,line)) return false;
		sscanf(line,_format_unique_storage_task,&nRead);
		m_bIsUniqueStorageTask = (nRead!=0);
	}
	if (ulVersion >= 119)
	{
		if(!ReadLine(fp,line)) return false;
		sscanf(line,_format_worldcontrib,&m_iWorldContribution);
	}
	if (ulVersion >= 84)
	{
		if (!ReadLine(fp, line)) return false;
		sscanf(line, _format_auto_deliver_prompt, &nRead);
		m_bDisplayInExclusiveUI = (nRead != 0);
	}

	if (ulVersion >= 116)
	{
		if (!ReadLine(fp, line)) return false;
		sscanf(line, _format_token_shop_task, &nRead);
		m_bUsedInTokenShop = (nRead != 0);
	}

	if (ulVersion >= 85)
	{
		if (!ReadLine(fp, line)) return false;
		sscanf(line, _format_period_limit, &m_lPeriodLimit);
	}
	/* 任务的开启条件及方式 */

	if (ulVersion >= 96)
	{
		if (!ReadLine(fp, line)) return false;
		sscanf(line,_format_prem_transformed_mask,&m_ucPremiseTransformedForm);

		if (!ReadLine(fp, line)) return false;
		sscanf(line, _format_show_by_transformed, &nRead);
		m_bShowByTransformed = (nRead != 0);
	}

	if (ulVersion >= 91)
	{
		if (!ReadLine(fp, line)) return false;
		sscanf(line,_format_prem_record_tasks_num,&m_iPremNeedRecordTasksNum);
		
		if (!ReadLine(fp, line)) return false;
		sscanf(line, _format_by_record_tasks_num, &nRead);
		m_bShowByNeedRecordTasksNum = (nRead != 0);
		
	}

	if (ulVersion >= 87)
	{
		if (!ReadLine(fp, line)) return false;
		sscanf(line,_format_prem_faction_contrib,&m_iPremiseFactionContrib);

		if (!ReadLine(fp, line)) return false;
		sscanf(line, _format_by_faction_contrib, &nRead);
		m_bShowByFactionContrib = (nRead != 0);
		
	}

	if (ulVersion >= 42)
	{
		if (!ReadLine(fp, line)) return false;
		sscanf(line, _format_special_award, &m_ulSpecialAward);
	}

	if (!ReadLine(fp, line)) return false;
	sscanf(line, _format_prem_level, &m_ulPremise_Lev_Min);

	if (!ReadLine(fp, line)) return false;
	sscanf(line, _format_prem_level, &m_ulPremise_Lev_Max);

	if (ulVersion >= 113)
	{
		if (!ReadLine(fp, line)) return false;
		sscanf(line, _format_prem_check_historymaxlevel, &nRead);
		m_bPremCheckMaxHistoryLevel = (nRead != 0);

		if (!ReadLine(fp, line)) return false;
		sscanf(line, _format_prem_check_reincarnation, &nRead);
		m_bPremCheckReincarnation = (nRead != 0);
		
		if (!ReadLine(fp, line)) return false;
		sscanf(line, _format_prem_reincarnation_min, &m_ulPremReincarnationMin);
		
		if (!ReadLine(fp, line)) return false;
		sscanf(line, _format_prem_reincarnation_max, &m_ulPremReincarnationMax);
		
		if (!ReadLine(fp, line)) return false;
		sscanf(line, _format_by_reincarnation, &nRead);
		m_bShowByReincarnation = (nRead != 0);
		
		if (!ReadLine(fp, line)) return false;
		sscanf(line, _format_prem_check_realmlevel, &nRead);
		m_bPremCheckRealmLevel = (nRead != 0);
		
		if (!ReadLine(fp, line)) return false;
		sscanf(line, _format_prem_realmlevel_min, &m_ulPremRealmLevelMin);
		
		if (!ReadLine(fp, line)) return false;
		sscanf(line, _format_prem_realmlevel_max, &m_ulPremRealmLevelMax);

		if (!ReadLine(fp, line)) return false;
		sscanf(line, _format_prem_realmexp_full, &nRead);
		m_bPremCheckRealmExpFull = (nRead != 0);

		if (!ReadLine(fp, line)) return false;
		sscanf(line, _format_by_realmlevel, &nRead);
		m_bShowByRealmLevel = (nRead != 0);
	}

	if(ulVersion >= 112)
	{
		if (!ReadLine(fp, line)) return false;
		sscanf(line, _format_prem_historyIndex, &m_iPremHistoryStageIndex[0],&m_iPremHistoryStageIndex[1]);
	}
	if (ulVersion >= 118)
	{
		if (!ReadLine(fp, line)) return false;
		sscanf(line, _format_by_historyStage, &nRead);
		m_bShowByHistoryStage = (nRead != 0);
	}

	if (!ReadLine(fp, line)) return false;
	sscanf(line, _format_item_wanted_num, &m_ulPremItems);

	if (ulVersion >= 98)
	{
		if (!ReadLine(fp, line)) return false;
		sscanf(line, _format_prem_items_any_one, &nRead);
		m_bPremItemsAnyOne = (nRead != 0);
	}

#ifndef TASK_TEMPL_EDITOR

	if (m_ulPremItems)
	{
		m_PremItems = new ITEM_WANTED[m_ulPremItems];
		g_ulNewCount++;
	}

#endif

	for (i = 0; i < m_ulPremItems; i++)
	{
		ITEM_WANTED& iw = m_PremItems[i];
		int nBool;

		if (!ReadLine(fp, line)) return false;
		sscanf(line, _format_item_templ, &iw.m_ulItemTemplId);

		if (!ReadLine(fp, line)) return false;
		sscanf(line, _format_item_is_cmn, &nBool);
		iw.m_bCommonItem = (nBool != 0);

		if (!ReadLine(fp, line)) return false;
		sscanf(line, _format_item_num, &iw.m_ulItemNum);

		if(ulVersion >= 73)
		{
			if (!ReadLine(fp, line)) return false;
			sscanf(line, _format_period, &iw.m_lPeriod);
		}
	}

	if (ulVersion >= 5)
	{
		if (!ReadLine(fp, line)) return false;
		sscanf(line, _format_given_items_num, &m_ulGivenItems);

#ifndef TASK_TEMPL_EDITOR

		if (m_ulGivenItems)
		{
			m_GivenItems = new ITEM_WANTED[m_ulGivenItems];
			g_ulNewCount++;
		}

#endif

		for (i = 0; i < m_ulGivenItems; i++)
		{
			ITEM_WANTED& iw = m_GivenItems[i];
			
			iw.m_fProb = 1.0f;
			iw.m_lPeriod = 0;

			if (!ReadLine(fp, line)) return false;
			sscanf(line, _format_item_templ, &iw.m_ulItemTemplId);

			if (!ReadLine(fp, line)) return false;
			sscanf(line, _format_item_num, &iw.m_ulItemNum);

			if (ulVersion >= 21)
			{
				if (!ReadLine(fp, line)) return false;
				sscanf(line, _format_item_is_cmn, &nRead);
				iw.m_bCommonItem = (nRead != 0);
			}
			else
				iw.m_bCommonItem = false;

			if (iw.m_bCommonItem) m_ulGivenCmnCount++;
			else m_ulGivenTskCount++;

			if(ulVersion >= 73)
			{
				if (!ReadLine(fp, line)) return false;
				sscanf(line, _format_period,  &iw.m_lPeriod);
			}
		}
	}

	if (!ReadLine(fp, line)) return false;
	sscanf(line, _format_prem_deposit, &m_ulPremise_Deposit);

	if (!ReadLine(fp, line)) return false;
	sscanf(line, _format_prem_reputation, &m_lPremise_Reputation);

	if (ulVersion >= 53)
	{
		if (!ReadLine(fp, line)) return false;
		sscanf(line, _format_prem_reputation, &m_lPremise_RepuMax);
	}

	if (!ReadLine(fp, line)) return false;
	sscanf(line, _format_prem_cotask, &m_ulPremise_Cotask);

	if (ulVersion >= 18)
	{
		if (!ReadLine(fp, line)) return false;
		sscanf(line, _format_cotask_cond, &m_ulCoTaskCond);

		if (ulVersion >= 30)
		{
			if (!ReadLine(fp, line)) return false;
			sscanf(line, _format_mutex_task_count, &m_ulMutexTaskCount);

			for (i = 0; i < m_ulMutexTaskCount; i++)
			{
				if (!ReadLine(fp, line)) return false;
				sscanf(line, _format_mutex_task, &m_ulMutexTasks[i]);
			}
		}
		else
		{
			if (!ReadLine(fp, line)) return false;
			sscanf(line, _format_mutex_task, &m_ulMutexTasks[0]);
			if (m_ulMutexTasks[0]) m_ulMutexTaskCount++;
		}
	}

	if (!ReadLine(fp, line)) return false;
	sscanf(line, _format_prem_task_count, &m_ulPremise_Task_Count);

	for (i = 0; i < m_ulPremise_Task_Count; i++)
	{
		if (!ReadLine(fp, line)) return false;
		sscanf(line, _format_prem_task, &m_ulPremise_Tasks[i]);

		if (ulVersion < 32) ReadLine(fp, line);
	}

	if (ulVersion >= 90)
	{
		if (!ReadLine(fp, line)) return false;
		sscanf(line, _format_prem_task_leastnum, &m_ulPremise_Task_Least_Num);
	}

	if (!ReadLine(fp, line)) return false;
	sscanf(line, _format_prem_period, &m_ulPremise_Period);

	if (!ReadLine(fp, line)) return false;
	sscanf(line, _format_prem_is_team, &nRead);
	m_bTeamwork = (nRead != 0);

	if (ulVersion >= 2 && m_bTeamwork)
	{
		nRead = 0;
		if (!ReadLine(fp, line)) return false;
		sscanf(line, _format_rcv_by_team, &nRead);
		m_bRcvByTeam = (nRead != 0);

		nRead = 0;
		if (!ReadLine(fp, line)) return false;
		sscanf(line, _format_shared, &nRead);
		m_bSharedTask = (nRead != 0);

		nRead = 0;
		if (!ReadLine(fp, line)) return false;
		sscanf(line, _format_share_acquire, &nRead);
		m_bSharedAchieved = (nRead != 0);

		if (ulVersion >= 4)
		{
			if (!ReadLine(fp, line)) return false;
			sscanf(line, _format_check_tmmate, &nRead);
			m_bCheckTeammate = (nRead != 0);

			if (!ReadLine(fp, line)) return false;
			sscanf(line, _format_dist, &m_fTeammateDist);
		}

		if (ulVersion >= 12)
		{
			if (!ReadLine(fp, line)) return false;
			sscanf(line, _format_rcv_chck_mem, &nRead);
			m_bRcvChckMem = (nRead != 0);

			if (!ReadLine(fp, line)) return false;
			sscanf(line, _format_dist, &m_fRcvMemDist);

			if (!ReadLine(fp, line)) return false;
			sscanf(line, _format_cnt_by_mem_pos, &nRead);
			m_bCntByMemPos = (nRead != 0);

			if (!ReadLine(fp, line)) return false;
			sscanf(line, _format_dist, &m_fCntMemDist);
		}

		nRead = 0;
		if (!ReadLine(fp, line)) return false;
		sscanf(line, _format_all_fail, &nRead);
		m_bAllFail = (nRead != 0);

		nRead = 0;
		if (!ReadLine(fp, line)) return false;
		sscanf(line, _format_cap_fail, &nRead);
		m_bCapFail = (nRead != 0);

		if (ulVersion >= 19)
		{
			nRead = 0;
			if (!ReadLine(fp, line)) return false;
			sscanf(line, _format_cap_succ, &nRead);
			m_bCapSucc = (nRead != 0);

			if (!ReadLine(fp, line)) return false;
			sscanf(line, _format_dist, &m_fSuccDist);
		}
		
		if (ulVersion >= 66 )
		{
			nRead = 0;
			if (!ReadLine(fp, line)) return false;
			sscanf(line, _format_all_succ, &nRead);
			m_bAllSucc = (nRead != 0);
		}

		if (ulVersion >= 74)
		{
			nRead = 0;
			if (!ReadLine(fp, line)) return false;
			sscanf(line, _format_couple_only, &nRead);
			m_bCoupleOnly = (nRead != 0);
		}

		if (ulVersion >= 86)
		{
			nRead = 0;
			if (!ReadLine(fp, line)) return false;
			sscanf(line, _format_distinguished_occ, &nRead);
			m_bDistinguishedOcc = (nRead != 0);
		}

		if (ulVersion < 2) ReadLine(fp, line);

		nRead = 0;
		if (!ReadLine(fp, line)) return false;
		sscanf(line, _format_dism_self_fail, &nRead);
		m_bDismAsSelfFail = (nRead != 0);

		m_ulTeamMemsWanted = 0;
		if (!ReadLine(fp, line)) return false;
		sscanf(line, _format_mem_num_wanted, &m_ulTeamMemsWanted);

#ifndef TASK_TEMPL_EDITOR

		if (m_ulTeamMemsWanted)
		{
			m_TeamMemsWanted = new TEAM_MEM_WANTED[m_ulTeamMemsWanted];
			g_ulNewCount++;
		}

#endif

		for (i = 0; i < m_ulTeamMemsWanted; i++)
		{
			const TEAM_MEM_WANTED& tmw = m_TeamMemsWanted[i];
			
			if (!ReadLine(fp, line)) return false;
			sscanf(line, _format_level, &tmw.m_ulLevelMin);

			if (!ReadLine(fp, line)) return false;
			sscanf(line, _format_level, &tmw.m_ulLevelMax);

			if (!ReadLine(fp, line)) return false;
			sscanf(line, _format_occupation, &tmw.m_ulOccupation);

			if (ulVersion >= 22)
			{
				if (!ReadLine(fp, line)) return false;
				sscanf(line, _format_race, &tmw.m_ulRace);
			}
			
			if (!ReadLine(fp, line)) return false;
			sscanf(line, _format_gender, &tmw.m_ulGender);
			
			if (!ReadLine(fp, line)) return false;
			sscanf(line, _format_count, &tmw.m_ulMinCount);

			if (!ReadLine(fp, line)) return false;
			sscanf(line, _format_count, &tmw.m_ulMaxCount);

			if (!ReadLine(fp, line)) return false;
			sscanf(line, _format_task, &tmw.m_ulTask);

			if (ulVersion >= 97)
			{
				if (!ReadLine(fp, line)) return false;
				sscanf(line, _format_team_force, &tmw.m_iForce);
			}
		}
	}

	if (!ReadLine(fp, line)) return false;
	sscanf(line, _format_prem_is_spouse, &nRead);
	m_bPremise_Spouse = (nRead != 0);

	if (ulVersion >= 102)
	{
		if (!ReadLine(fp, line)) return false;
		sscanf(line, _format_prem_is_wedding_owner, &nRead);
		m_bPremiseWeddingOwner = (nRead != 0);

		if (!ReadLine(fp, line)) return false;
		sscanf(line, _format_by_wedding_owner, &nRead);
		m_bShowByWeddingOwner = (nRead != 0);
	}

	if (ulVersion >= 49)
	{
		if (!ReadLine(fp, line)) return false;
		sscanf(line, _format_prem_is_gm, &nRead);
		m_bGM = (nRead != 0);
	}
	
	if (ulVersion >= 65)
	{
		if (!ReadLine(fp, line)) return false;
		sscanf(line, _format_prem_is_shielduser, &nRead);
		m_bShieldUser = (nRead != 0);
	}

	if (!ReadLine(fp, line)) return false;
	sscanf(line, _format_prem_clan, &m_ulPremise_Faction);

	if (ulVersion >= 93)
	{
		if (!ReadLine(fp, line)) return false;
		sscanf(line, _format_prem_clan_role, &m_iPremise_FactionRole);
	}

	if (ulVersion < 2)
	{
		ReadLine(fp, line); // old race

		if (!ReadLine(fp, line)) return false;
		sscanf(line, _format_prem_occupation, &m_Occupations[0]);
		if (m_Occupations[0] != INVALID_VAL) m_ulOccupations++;
	}
	else
	{
		if (!ReadLine(fp, line)) return false;
		sscanf(line, _format_prem_gender, &m_ulGender);

		if (!ReadLine(fp, line)) return false;
		sscanf(line, _format_count, &m_ulOccupations);

		for (i = 0; i < m_ulOccupations; i++)
		{
			if (!ReadLine(fp, line)) return false;
			sscanf(line, _format_prem_occupation, &m_Occupations[i]);
		}
	}

	if (ulVersion >= 30)
	{
		for (i = 0; i < MAX_LIVING_SKILLS; i++)
		{
			if (!ReadLine(fp, line)) return false;
			sscanf(line, _format_proficiency, &m_lSkillLev[i]);
		}
	}

	if (ulVersion < 2)
	{
		if (!ReadLine(fp, line)) return false;
		if (!ReadLine(fp, line)) return false;
	}

	if (ulVersion >= 14)
	{
		if (!ReadLine(fp, line)) return false;
		sscanf(line, _format_by_deposit, &nRead);
		m_bShowByDeposit = (nRead != 0);

		if (!ReadLine(fp, line)) return false;
		sscanf(line, _format_by_gender, &nRead);
		m_bShowByGender = (nRead != 0);

		if (!ReadLine(fp, line)) return false;
		sscanf(line, _format_by_items, &nRead);
		m_bShowByItems = (nRead != 0);

		if (!ReadLine(fp, line)) return false;
		sscanf(line, _format_by_lev, &nRead);
		m_bShowByLev = (nRead != 0);

		if (!ReadLine(fp, line)) return false;
		sscanf(line, _format_by_occup, &nRead);
		m_bShowByOccup = (nRead != 0);

		if (!ReadLine(fp, line)) return false;
		sscanf(line, _format_by_preTask, &nRead);
		m_bShowByPreTask = (nRead != 0);

		if (!ReadLine(fp, line)) return false;
		sscanf(line, _format_by_repu, &nRead);
		m_bShowByRepu = (nRead != 0);

		if (!ReadLine(fp, line)) return false;
		sscanf(line, _format_by_team, &nRead);
		m_bShowByTeam = (nRead != 0);

		if (!ReadLine(fp, line)) return false;
		sscanf(line, _format_by_faction, &nRead);
		m_bShowByFaction = (nRead != 0);

		if (!ReadLine(fp, line)) return false;
		sscanf(line, _format_by_period, &nRead);
		m_bShowByPeriod = (nRead != 0);

		if (!ReadLine(fp, line)) return false;
		sscanf(line, _format_by_spouse, &nRead);
		m_bShowBySpouse = (nRead != 0);
	}

	if (ulVersion >= 58)
	{
		nRead = 0;
		if (!ReadLine(fp, line)) return false;
		sscanf(line, _format_need_comp, &nRead);
		m_bPremNeedComp = (nRead != 0);

		if (!ReadLine(fp, line)) return false;
		sscanf(line, _format_and_or, &m_nPremExp1AndOrExp2);
		
		if (!ReadLine(fp, line)) return false;
		sscanf(line, _format_left_type, &m_Prem1KeyValue.nLeftType);

		if (!ReadLine(fp, line)) return false;
		sscanf(line, _format_left_num, &m_Prem1KeyValue.lLeftNum);

		if (!ReadLine(fp, line)) return false;
		sscanf(line, _format_comp_oper, &m_Prem1KeyValue.nCompOper);

		if (!ReadLine(fp, line)) return false;
		sscanf(line, _format_right_type, &m_Prem1KeyValue.nRightType);

		if (!ReadLine(fp, line)) return false;
		sscanf(line, _format_right_num, &m_Prem1KeyValue.lRightNum);
		
		if (!ReadLine(fp, line)) return false;
		sscanf(line, _format_left_type, &m_Prem2KeyValue.nLeftType);
		
		if (!ReadLine(fp, line)) return false;
		sscanf(line, _format_left_num, &m_Prem2KeyValue.lLeftNum);
		
		if (!ReadLine(fp, line)) return false;
		sscanf(line, _format_comp_oper, &m_Prem2KeyValue.nCompOper);
		
		if (!ReadLine(fp, line)) return false;
		sscanf(line, _format_right_type, &m_Prem2KeyValue.nRightType);
		
		if (!ReadLine(fp, line)) return false;
		sscanf(line, _format_right_num, &m_Prem2KeyValue.lRightNum);

	}

	if (ulVersion >= 74)
	{
		nRead = 0;
		if (!ReadLine(fp, line)) return false;
		sscanf(line, _format_prem_rmb_min, &m_ulPremRMBMin);

		if (!ReadLine(fp, line)) return false;
		sscanf(line, _format_prem_rmb_max, &m_ulPremRMBMax);


		if (!ReadLine(fp, line)) return false;
		sscanf(line, _format_char_time, &nRead);
		m_bCharTime = (nRead != 0);

		if (!ReadLine(fp, line)) return false;
		sscanf(line, _format_by_char_time, &nRead);
		m_bShowByCharTime = (nRead != 0);

		if (!ReadLine(fp, line)) return false;
		sscanf(line, _format_char_start_time, &m_iCharStartTime);

		if (!ReadLine(fp, line)) return false;
		sscanf(line, _format_char_end_time, &m_iCharEndTime);

		if (!ReadLine(fp, line)) return false;
		sscanf(line, _format_full_tm_old, &m_tmCharEndTime.year, &m_tmCharEndTime.month, &m_tmCharEndTime.day, &m_tmCharEndTime.hour, &m_tmCharEndTime.min);

		if (!ReadLine(fp, line)) return false;
		sscanf(line, _format_char_time_greater, &m_ulCharTimeGreaterThan);
	}

	if (ulVersion >= 75)
	{
		nRead = 0;
		if (!ReadLine(fp, line)) return false;
		sscanf(line, _format_by_rmb, &nRead);
		m_bShowByRMB = (nRead != 0);
	}

	if (ulVersion >= 97)
	{
		nRead = 0;
		if (!ReadLine(fp, line)) return false;
		sscanf(line, _format_by_force, &nRead);
		m_bShowByForce = (nRead != 0);

		nRead = 0;
		if (!ReadLine(fp, line)) return false;
		sscanf(line, _format_by_force_contrib, &nRead);
		m_bShowByForceContribution = (nRead != 0);

		nRead = 0;
		if (!ReadLine(fp, line)) return false;
		sscanf(line, _format_by_force_repu, &nRead);
		m_bShowByForceReputation = (nRead != 0);

		nRead = 0;
		if (!ReadLine(fp, line)) return false;
		sscanf(line, _format_by_force_exp, &nRead);
		m_bShowByForceExp = (nRead != 0);

		nRead = 0;
		if (!ReadLine(fp, line)) return false;
		sscanf(line, _format_by_force_sp, &nRead);
		m_bShowByForceSP = (nRead != 0);

		nRead = 0;
		if (!ReadLine(fp, line)) return false;
		sscanf(line, _format_check_force, &nRead);
		m_bPremCheckForce = (nRead != 0);

		if (!ReadLine(fp, line)) return false;
		sscanf(line, _format_force, &m_iPremForce);
		if (!ReadLine(fp, line)) return false;
		sscanf(line, _format_prem_force_contrib, &m_iPremForceContribution);
		if (!ReadLine(fp, line)) return false;
		sscanf(line, _format_prem_force_repu, &m_iPremForceReputation);
		if (!ReadLine(fp, line)) return false;
		sscanf(line, _format_prem_force_exp, &m_iPremForceExp);
		if (!ReadLine(fp, line)) return false;
		sscanf(line, _format_prem_force_sp, &m_iPremForceSP);

	}

	if (ulVersion >= 99)
	{
		nRead = 0;
		if (!ReadLine(fp, line)) return false;
		sscanf(line, _format_by_force_al, &nRead);
		m_bShowByForceActivityLevel = (nRead != 0);

		if (!ReadLine(fp, line)) return false;
		sscanf(line, _format_prem_force_al, &m_iPremForceActivityLevel);
	}

	if(ulVersion >= 114)
	{
		if (!ReadLine(fp, line)) return false;
		sscanf(line, _format_prem_cardcollection, &m_ulPremGeneralCardCount);

		if (!ReadLine(fp, line)) return false;
		sscanf(line, _format_by_generalcard_cnt, &nRead);
		m_bShowByGeneralCard = (nRead != 0);
	}
	if(ulVersion >= 117)
	{
		if (!ReadLine(fp, line)) return false;
		sscanf(line, _format_prem_cardrank, &m_iPremGeneralCardRank);

		if (!ReadLine(fp, line)) return false;
		sscanf(line, _format_prem_cardrankcount, &m_ulPremGeneralCardRankCount);
		
		if (!ReadLine(fp, line)) return false;
		sscanf(line, _format_by_generalcard_rankcnt, &nRead);
		m_bShowByGeneralCardRank = (nRead != 0);
	}

	/* 任务完成的方式及条件 */

	if (!ReadLine(fp, line)) return false;
	sscanf(line, _format_method, &m_enumMethod);

	if (!ReadLine(fp, line)) return false;
	sscanf(line, _format_finish_type, &m_enumFinishType);

	if (m_enumMethod == enumTMKillNumMonster)
	{
		if (ulVersion < 28)
			m_ulMonsterWanted = 1;
		else
		{
			m_ulMonsterWanted = 0;

			if (!ReadLine(fp, line)) return false;
			sscanf(line, _format_monster_wanted, &m_ulMonsterWanted);
		}

#ifndef TASK_TEMPL_EDITOR

		if (m_ulMonsterWanted)
		{
			m_MonsterWanted = new MONSTER_WANTED[m_ulMonsterWanted];
			g_ulNewCount++;
		}

#endif

		for (i = 0; i < m_ulMonsterWanted; i++)
		{
			MONSTER_WANTED& Monster = m_MonsterWanted[i];

			if (!ReadLine(fp, line)) return false;
			sscanf(line, _format_monster_templ, &Monster.m_ulMonsterTemplId);

			if (!ReadLine(fp, line)) return false;
			sscanf(line, _format_monster_num, &Monster.m_ulMonsterNum);

			if (!ReadLine(fp, line)) return false;
			sscanf(line, _format_item_templ, &Monster.m_ulDropItemId);

			if (!ReadLine(fp, line)) return false;
			sscanf(line, _format_item_wanted_num, &Monster.m_ulDropItemCount);

			if (ulVersion >= 23)
			{
				if (!ReadLine(fp, line)) return false;
				sscanf(line, _format_item_is_cmn, &nRead);
				Monster.m_bDropCmnItem = (nRead != 0);
			}

			if (!ReadLine(fp, line)) return false;
			sscanf(line, _format_item_drop_prob, &Monster.m_fDropProb);

			if (ulVersion >= 34)
			{
				if (!ReadLine(fp, line)) return false;
				sscanf(line, _format_killer_lev, &nRead);
				Monster.m_bKillerLev = (nRead != 0);
			}
			else
				Monster.m_bKillerLev = false;

			if (ulVersion >= 92)
			{
				if (!ReadLine(fp, line)) return false;
				sscanf(line, _format_dps, &Monster.m_iDPS);

				if (!ReadLine(fp, line)) return false;
				sscanf(line, _format_dph, &Monster.m_iDPH);
			}
			else
			{
				Monster.m_iDPS = 0;
				Monster.m_iDPH = 0;
			}
		}
	}
	else if (m_enumMethod == enumTMKillPlayer)
	{
		if (ulVersion >= 103)
		{
			m_ulPlayerWanted = 0;
			if (!ReadLine(fp, line)) return false;
			sscanf(line, _format_player_wanted, &m_ulPlayerWanted);

#ifndef TASK_TEMPL_EDITOR
			if (m_ulPlayerWanted)
			{
				m_PlayerWanted = new PLAYER_WANTED[m_ulPlayerWanted];
				g_ulNewCount++;
			}
#endif

			for (i = 0;i < m_ulPlayerWanted;++i)
			{
				PLAYER_WANTED& Player = m_PlayerWanted[i];

				if (!ReadLine(fp, line)) return false;
				sscanf(line, _format_player_num, &Player.m_ulPlayerNum);

				if (!ReadLine(fp, line)) return false;
				sscanf(line, _format_item_templ, &Player.m_ulDropItemId);
				
				if (!ReadLine(fp, line)) return false;
				sscanf(line, _format_item_wanted_num, &Player.m_ulDropItemCount);

				if (!ReadLine(fp, line)) return false;
				sscanf(line, _format_item_is_cmn, &nRead);
				Player.m_bDropCmnItem = (nRead != 0);
				
				if (!ReadLine(fp, line)) return false;
				sscanf(line, _format_item_drop_prob, &Player.m_fDropProb);

				if (!ReadLine(fp, line)) return false;
				sscanf(line, _format_player_require_occ, &Player.m_Requirements.m_ulOccupations);

				if (!ReadLine(fp, line)) return false;
				sscanf(line, _format_player_require_level_min, &Player.m_Requirements.m_iMinLevel);

				if (!ReadLine(fp, line)) return false;
				sscanf(line, _format_player_require_level_max, &Player.m_Requirements.m_iMaxLevel);

				if (!ReadLine(fp, line)) return false;
				sscanf(line, _format_player_require_gender, &Player.m_Requirements.m_iGender);

				if (!ReadLine(fp, line)) return false;
				sscanf(line, _format_player_require_force, &Player.m_Requirements.m_iForce);
			}
		}
	}
	else if (m_enumMethod == enumTMCollectNumArticle)
	{
		if (ulVersion >= 39)
		{
			if (!ReadLine(fp, line)) return false;
			sscanf(line, _format_gold, &m_ulGoldWanted);
		}

		if (ulVersion >= 88)
		{
			if (!ReadLine(fp, line)) return false;
			sscanf(line, _format_faction_contrib, &m_iFactionContribWanted);
		}

		if (ulVersion >= 89)
		{
			if (!ReadLine(fp, line)) return false;
			sscanf(line, _format_faction_exp_contrib, &m_iFactionExpContribWanted);
		}

		m_ulItemsWanted = 0;

		if (!ReadLine(fp, line)) return false;
		sscanf(line, _format_item_wanted_num, &m_ulItemsWanted);

#ifndef TASK_TEMPL_EDITOR

		if (m_ulItemsWanted)
		{
			m_ItemsWanted = new ITEM_WANTED[m_ulItemsWanted];
			g_ulNewCount++;
		}
#endif

		for (i = 0; i < m_ulItemsWanted; i++)
		{
			ITEM_WANTED& iw = m_ItemsWanted[i];
			int nBool = 0;
			iw.m_fProb = 1.0f;
			iw.m_lPeriod = 0;

			if (!ReadLine(fp, line)) return false;
			sscanf(line, _format_item_templ, &iw.m_ulItemTemplId);

			if (!ReadLine(fp, line)) return false;
			sscanf(line, _format_item_is_cmn, &nBool);
			iw.m_bCommonItem = (nBool != 0);

			if (!ReadLine(fp, line)) return false;
			sscanf(line, _format_item_num, &iw.m_ulItemNum);
		}
	}
	else if (m_enumMethod == enumTMTalkToNPC)
	{
		if (ulVersion < 13) ReadLine(fp, line);
	}
	else if (m_enumMethod == enumTMProtectNPC)
	{
		if (!ReadLine(fp, line)) return false;
		sscanf(line, _format_npc_to_protect, &m_ulNPCToProtect);

		if (!ReadLine(fp, line)) return false;
		sscanf(line, _format_protect_tm_len, &m_ulProtectTimeLen);
	}
	else if (m_enumMethod == enumTMWaitTime)
	{
		if (!ReadLine(fp, line)) return false;
		sscanf(line, _format_waittime, &m_ulWaitTime);
	}
	else if (m_enumMethod == enumTMReachTreasureZone)
	{
		if (ulVersion>=94)
		{
			if (!ReadLine(fp, line)) return false;
			sscanf(line, _format_treasure_zones_s, VERT_ZONE_ADDR(m_TreasureStartZone));
			if (!ReadLine(fp, line)) return false;
			int iRead;
			sscanf(line, _format_treasure_zones_x, &iRead);
			m_ucZonesNumX = (unsigned char)iRead;
			if (!ReadLine(fp, line)) return false;
			sscanf(line, _format_treasure_zones_z, &iRead);
			m_ucZonesNumZ = (unsigned char)iRead;
			if (!ReadLine(fp, line)) return false;
			sscanf(line, _format_treasure_zone_l, &iRead);
			m_ucZoneSide = (unsigned char)iRead;
		}	
	}
	else if (m_enumMethod == enumTMTransform)
	{
		if (ulVersion >= 104)
		{
			if (!ReadLine(fp, line)) return false;
			sscanf(line,_format_transformed_mask,&m_ucTransformedForm);
		}
	}
	else if (m_enumMethod == enumTMSimpleClientTask)
	{
		if (ulVersion >= 113)
		{
			if (!ReadLine(fp, line)) return false;
			sscanf(line,_format_emotion,&m_uiEmotion);
		}
	}
	else if (m_enumMethod == enumTMReachLevel)
	{
		if (ulVersion >= 113)
		{
			if (!ReadLine(fp, line)) return false;
			sscanf(line, _format_reach_level, &m_ulReachLevel);
			if (!ReadLine(fp, line)) return false;
			sscanf(line, _format_reach_reincarnation_count, &m_ulReachReincarnationCount);
			if (!ReadLine(fp, line)) return false;
			sscanf(line, _format_reach_realm_level, &m_ulReachRealmLevel);
		}
	}
	else if (m_enumMethod == enumTMReachSite)
	{

		if (ulVersion>=80)
		{
			if (!ReadLine(fp, line)) return false;
			sscanf(line, _format_taskRegionCnt, &m_ulReachSiteCnt);
		}
		else 
		{
			m_ulReachSiteCnt = 1;
		}

#ifndef TASK_TEMPL_EDITOR
		m_pReachSite = new Task_Region[m_ulReachSiteCnt];
#endif
		for (i=0;i<m_ulReachSiteCnt;i++)
		{
			Task_Region& t = m_pReachSite[i];

			if (!ReadLine(fp, line)) return false;
			sscanf(line, _format_zone_vert, VERT_ZONE_ADDR(t.zvMin));
			
			if (!ReadLine(fp, line)) return false;
			sscanf(line, _format_zone_vert, VERT_ZONE_ADDR(t.zvMax));
		}

		if (ulVersion >= 39)
		{
			if (!ReadLine(fp, line)) return false;
			sscanf(line, _format_world_id, &m_ulReachSiteId);
		}
	}
	else if (m_enumMethod == enumTMLeaveSite)
	{
		if (ulVersion>=80)
		{
			if (!ReadLine(fp, line)) return false;
			sscanf(line, _format_taskRegionCnt, &m_ulLeaveSiteCnt);
		}
		else m_ulLeaveSiteCnt = 1;
		
#ifndef TASK_TEMPL_EDITOR
		m_pLeaveSite = new Task_Region[m_ulLeaveSiteCnt];
#endif
		for (i=0;i<m_ulLeaveSiteCnt;i++)
		{
			Task_Region& t = m_pLeaveSite[i];
			
			if (!ReadLine(fp, line)) return false;
			sscanf(line, _format_zone_vert, VERT_ZONE_ADDR(t.zvMin));
			
			if (!ReadLine(fp, line)) return false;
			sscanf(line, _format_zone_vert, VERT_ZONE_ADDR(t.zvMax));
		}

		if (!ReadLine(fp, line)) return false;
		sscanf(line, _format_world_id, &m_ulLeaveSiteId);
	}

	if (ulVersion >= 58)
	{
		nRead = 0;
		if (!ReadLine(fp, line)) return false;
		sscanf(line, _format_need_comp, &nRead);
		m_bFinNeedComp = (nRead != 0);

		if (!ReadLine(fp, line)) return false;
		sscanf(line, _format_and_or, &m_nFinExp1AndOrExp2);
		
		if (!ReadLine(fp, line)) return false;
		sscanf(line, _format_left_type, &m_Fin1KeyValue.nLeftType);

		if (!ReadLine(fp, line)) return false;
		sscanf(line, _format_left_num, &m_Fin1KeyValue.lLeftNum);

		if (!ReadLine(fp, line)) return false;
		sscanf(line, _format_comp_oper, &m_Fin1KeyValue.nCompOper);

		if (!ReadLine(fp, line)) return false;
		sscanf(line, _format_right_type, &m_Fin1KeyValue.nRightType);

		if (!ReadLine(fp, line)) return false;
		sscanf(line, _format_right_num, &m_Fin1KeyValue.lRightNum);
		
		if (!ReadLine(fp, line)) return false;
		sscanf(line, _format_left_type, &m_Fin2KeyValue.nLeftType);
		
		if (!ReadLine(fp, line)) return false;
		sscanf(line, _format_left_num, &m_Fin2KeyValue.lLeftNum);
		
		if (!ReadLine(fp, line)) return false;
		sscanf(line, _format_comp_oper, &m_Fin2KeyValue.nCompOper);
		
		if (!ReadLine(fp, line)) return false;
		sscanf(line, _format_right_type, &m_Fin2KeyValue.nRightType);
		
		if (!ReadLine(fp, line)) return false;
		sscanf(line, _format_right_num, &m_Fin2KeyValue.lRightNum);
	}

	if (ulVersion >= 62)
	{
		if (!ReadLine(fp, line)) return false;
		sscanf(line, _format_dis_expcnt, &m_ulExpCnt);

		if (m_ulExpCnt)
		{
			m_pszExp = new char[TASK_AWARD_MAX_DISPLAY_EXP_CNT][TASK_AWARD_MAX_DISPLAY_CHAR_LEN];
			memset(m_pszExp, 0, TASK_AWARD_MAX_DISPLAY_EXP_CNT*TASK_AWARD_MAX_DISPLAY_CHAR_LEN);

			m_pExpArr = new TASK_EXPRESSION[TASK_AWARD_MAX_DISPLAY_EXP_CNT][TASK_AWARD_MAX_DISPLAY_CHAR_LEN];
			memset(m_pExpArr, 0, sizeof(TASK_EXPRESSION)*TASK_AWARD_MAX_DISPLAY_EXP_CNT*TASK_AWARD_MAX_DISPLAY_CHAR_LEN);
		}

		for (i=0; i<m_ulExpCnt; i++)
		{
			fread(m_pszExp[i], 1, TASK_AWARD_MAX_DISPLAY_CHAR_LEN, fp);
			if (!ReadLine(fp, line)) return false;
		}

		if (!ReadLine(fp, line)) return false;
		sscanf(line, _format_discharcnt, &m_ulTaskCharCnt);
		
		if (m_ulTaskCharCnt)
		{
			m_pTaskChar = new task_char[TASK_AWARD_MAX_DISPLAY_EXP_CNT][TASK_AWARD_MAX_DISPLAY_CHAR_LEN];
			memset(m_pTaskChar, 0, sizeof(task_char)*TASK_AWARD_MAX_DISPLAY_EXP_CNT*TASK_AWARD_MAX_DISPLAY_CHAR_LEN);
		}

		for (i=0; i<m_ulTaskCharCnt; i++)
		{
			fread(m_pTaskChar[i], sizeof(task_char), TASK_AWARD_MAX_DISPLAY_CHAR_LEN, fp);
			if (!ReadLine(fp, line)) return false;
		}		
	}

	/* 任务结束后的奖励 */
#define LOAD_AWARD_BY_RESULT(Result, editor) \
{\
	unsigned long i;\
\
	if (ulVersion >= 3)\
	{\
		if (!ReadLine(fp, line)) return false;\
		sscanf(line, _format_award_type, &m_ulAwardType##Result);\
	}\
	else\
		m_ulAwardType##Result = enumTATNormal;\
\
	switch (m_ulAwardType##Result)\
	{\
	case enumTATNormal:\
	case enumTATEach:\
		LoadAwardData(fp, *m_Award##Result, ulVersion);\
		break;\
	case enumTATRatio:\
		if (!ReadLine(fp, line)) return false;\
		sscanf(line, _format_count, &m_AwByRatio##Result->m_ulScales);\
		if (!editor && m_AwByRatio##Result->m_ulScales)\
		{\
			m_AwByRatio##Result->m_Awards = new AWARD_DATA[m_AwByRatio##Result->m_ulScales];\
			g_ulNewCount++;\
		}\
\
		for (i = 0; i < m_AwByRatio##Result->m_ulScales; i++)\
		{\
			if (!ReadLine(fp, line)) return false;\
			sscanf(line, _format_ratio, &m_AwByRatio##Result->m_Ratios[i]);\
\
			LoadAwardData(fp, m_AwByRatio##Result->m_Awards[i], ulVersion);\
		}\
		break;\
	case enumTATItemCount:\
		if (!ReadLine(fp, line)) return false;\
		sscanf(line, _format_count, &m_AwByItems##Result->m_ulScales);\
\
		if (!ReadLine(fp, line)) return false;\
		sscanf(line, _format_item_templ, &m_AwByItems##Result->m_ulItemId);\
		if (!editor && m_AwByItems##Result->m_ulScales)\
		{\
			m_AwByItems##Result->m_Awards = new AWARD_DATA[m_AwByItems##Result->m_ulScales];\
			g_ulNewCount++;\
		}\
\
		for (i = 0; i < m_AwByItems##Result->m_ulScales; i++)\
		{\
			if (!ReadLine(fp, line)) return false;\
			sscanf(line, _format_item_num, &m_AwByItems##Result->m_Counts[i]);\
\
			LoadAwardData(fp, m_AwByItems##Result->m_Awards[i], ulVersion);\
		}\
		break;\
	}\
}

	bool bEditor;
#ifdef TASK_TEMPL_EDITOR
	bEditor = true;
#else
	bEditor = false;
#endif

	LOAD_AWARD_BY_RESULT(_S, bEditor)

	LOAD_AWARD_BY_RESULT(_F, bEditor)

	/* 层次关系 */

	if (!ReadLine(fp, line)) return false;
	sscanf(line, _format_parent, &m_ulParent);

	if (!ReadLine(fp, line)) return false;
	sscanf(line, _format_next_sibling, &m_ulNextSibling);

	if (!ReadLine(fp, line)) return false;
	sscanf(line, _format_prev_sibling, &m_ulPrevSibling);

	if (!ReadLine(fp, line)) return false;
	sscanf(line, _format_first_child, &m_ulFirstChild);

	return true;
}

bool ATaskTemplFixedData::operator ==(const ATaskTemplFixedData& src) const
{
		
		if (!CompareTwoPointer(m_pDelvRegion,src.m_pDelvRegion))
		{
			return false;
		}
		if (!CompareTwoPointer(m_pEnterRegion,src.m_pEnterRegion))
		{
			return false;
		}
		if (!CompareTwoPointer(m_pLeaveRegion,src.m_pLeaveRegion))
		{
			return false;
		}
		if (m_ulChangeKeyCnt != src.m_ulChangeKeyCnt)
		{
			return false;
		}
		unsigned int i = 0;
		for (i = 0;i < m_ulChangeKeyCnt;++i)
		{
			if (m_plChangeKey[i] != src.m_plChangeKey[i] || m_plChangeKeyValue[i] != src.m_plChangeKeyValue[i] || m_pbChangeType[i] != src.m_pbChangeType[i])
			{
				return false;
			}
		}
		if (m_ulMonsterContribCnt != src.m_ulMonsterContribCnt)
		{
			return false;
		}
		for (i = 0;i < m_ulMonsterContribCnt;++i)
		{
			if (!(m_MonstersContrib[i] == src.m_MonstersContrib[i]))
			{
				return false;
			}
		}
		if (m_ulPremItems != src.m_ulPremItems)
		{
			return false;
		}
		for (i = 0;i < m_ulPremItems;++i)
		{
			if (!(m_PremItems[i] == src.m_PremItems[i]))
			{
				return false;
			}
		}
		if (m_ulGivenItems != src.m_ulGivenItems)
		{
			return false;
		}
		for (i = 0;i < m_ulGivenItems;++i)
		{
			if (!(m_GivenItems[i] == src.m_GivenItems[i]))
			{
				return false;
			}
		}
		if (m_ulPremise_Task_Count != src.m_ulPremise_Task_Count)
		{
			return false;
		}
		for (i = 0;i < m_ulPremise_Task_Count;++i)
		{
			if (m_ulPremise_Tasks[i] != src.m_ulPremise_Tasks[i])
			{
				return false;
			}
		}
		if (m_ulOccupations != src.m_ulOccupations)
		{
			return false;
		}
		for (i = 0;i < m_ulOccupations;++i)
		{
			if (m_Occupations[i] != src.m_Occupations[i])
			{
				return false;
			}
		}
		if (m_ulMutexTaskCount != src.m_ulMutexTaskCount)
		{
			return false;
		}
		for (i = 0;i < m_ulMutexTaskCount;++i)
		{
			if (m_ulMutexTasks[i] != src.m_ulMutexTasks[i])
			{
				return false;
			}
		}
		for (i = 0;i < MAX_LIVING_SKILLS;++i)
		{
			if (m_lSkillLev[i] != src.m_lSkillLev[i])
			{
				return false;
			}
		}
		if (m_ulTeamMemsWanted != src.m_ulTeamMemsWanted)
		{
			return false;
		}
		for (i = 0;i < m_ulTeamMemsWanted;++i)
		{
			if (!(m_TeamMemsWanted[i] == src.m_TeamMemsWanted[i]))
			{
				return false;
			}
		}
		if (m_ulMonsterWanted != src.m_ulMonsterWanted)
		{
			return false;
		}
		for (i = 0;i < m_ulMonsterWanted;++i)
		{
			if (!(m_MonsterWanted[i] == src.m_MonsterWanted[i]))
			{
				return false;
			}
		}
		if (m_ulItemsWanted != src.m_ulItemsWanted)
		{
			return false;
		}
		for (i = 0;i < m_ulItemsWanted;++i)
		{
			if (!(m_ItemsWanted[i] == src.m_ItemsWanted[i]))
			{
				return false;
			}
		}
		if (m_ulReachSiteCnt != src.m_ulReachSiteCnt)
		{
			return false;
		}
		for (i = 0;i < m_ulReachSiteCnt;++i)
		{
			if (!(m_pReachSite[i] == src.m_pReachSite[i]))
			{
				return false;
			}
		}
		if (m_ulLeaveSiteCnt != src.m_ulLeaveSiteCnt)
		{
			return false;
		}
		for (i = 0;i < m_ulLeaveSiteCnt;++i)
		{
			if (!(m_pLeaveSite[i] == src.m_pLeaveSite[i]))
			{
				return false;
			}
		}
		if (m_ulTimetable != src.m_ulTimetable)
		{
			return false;
		}
		for (i = 0;i < m_ulTimetable;++i)
		{
			if (m_tmType[i] != src.m_tmType[i] || !(m_tmStart[i] == src.m_tmStart[i]) || !(m_tmEnd[i] == src.m_tmEnd[i]))
			{
				return false;
			}
		}
		if (!CompareTwoPointer(m_Award_F,src.m_Award_F))
		{
			return false;
		}
		if (!CompareTwoPointer(m_Award_S,src.m_Award_S))
		{
			return false;
		}
		if (!CompareTwoPointer(m_AwByRatio_S,src.m_AwByRatio_S))
		{
			return false;
		}
		if (!CompareTwoPointer(m_AwByRatio_F,src.m_AwByRatio_F))
		{
			return false;
		}
		if (!CompareTwoPointer(m_AwByItems_F,src.m_AwByItems_F))
		{
			return false;
		}
		if (!CompareTwoPointer(m_AwByItems_S,src.m_AwByItems_S))
		{
			return false;
		}

		return ( m_ID						== src.m_ID								&&
				 m_ulType					== src.m_ulType							&&
				 m_ulTimeLimit				== src.m_ulTimeLimit					&&
				 m_bOfflineFail				== src.m_bOfflineFail					&&
				 m_bAbsFail					== src.m_bAbsFail						&&
				 m_tmAbsFailTime			== src.m_tmAbsFailTime					&&
				 m_bItemNotTakeOff			== src.m_bItemNotTakeOff				&&
				 m_bAbsTime					== src.m_bAbsTime						&&
				 m_ulTimetable				== src.m_ulTimetable					&&
				 m_lAvailFrequency			== src.m_lAvailFrequency				&&
				 m_lPeriodLimit				== src.m_lPeriodLimit					&&
				 m_bChooseOne				== src.m_bChooseOne						&&
				 m_bRandOne					== src.m_bRandOne						&&
				 m_bExeChildInOrder			== src.m_bExeChildInOrder				&&
				 m_bParentAlsoFail			== src.m_bParentAlsoFail				&&
				 m_bParentAlsoSucc			== src.m_bParentAlsoSucc				&&
				 m_bCanGiveUp				== src.m_bCanGiveUp						&&
				 m_bCanRedo					== src.m_bCanRedo						&&
				 m_bCanRedoAfterFailure		== src.m_bCanRedoAfterFailure			&&
				 m_bClearAsGiveUp			== src.m_bClearAsGiveUp					&&
				 m_bNeedRecord				== src.m_bNeedRecord					&&
				 m_bFailAsPlayerDie			== src.m_bFailAsPlayerDie				&&
				 m_ulMaxReceiver			== src.m_ulMaxReceiver					&&
				 m_bDelvInZone				== src.m_bDelvInZone					&&
				 m_ulDelvWorld				== src.m_ulDelvWorld					&&
				 m_ulDelvRegionCnt			== src.m_ulDelvRegionCnt				&&
				 m_bEnterRegionFail			== src.m_bEnterRegionFail				&&
				 m_ulEnterRegionWorld		== src.m_ulEnterRegionWorld				&&
				 m_ulEnterRegionCnt			== src.m_ulEnterRegionCnt				&&
				 m_bLeaveRegionFail			== src.m_bLeaveRegionFail				&&
				 m_ulLeaveRegionWorld		== src.m_ulLeaveRegionWorld				&&
				 m_ulLeaveRegionCnt			== src.m_ulLeaveRegionCnt				&&
				 m_bTransTo					== src.m_bTransTo						&&
				 m_ulTransWldId				== src.m_ulTransWldId					&&
				 m_TransPt					== src.m_TransPt						&&
				 m_lMonsCtrl				== src.m_lMonsCtrl						&&
				 m_bTrigCtrl				== src.m_bTrigCtrl						&&
				 m_bAutoDeliver				== src.m_bAutoDeliver					&&
				 m_bDisplayInExclusiveUI	== src.m_bDisplayInExclusiveUI			&&
				 m_bReadyToNotifyServer		== src.m_bReadyToNotifyServer			&&
				 m_bDeathTrig				== src.m_bDeathTrig						&&
				 m_bClearAcquired			== src.m_bClearAcquired					&&
				 m_ulSuitableLevel			== src.m_ulSuitableLevel				&&
				 m_bShowPrompt				== src.m_bShowPrompt					&&
				 m_bKeyTask					== src.m_bKeyTask						&&
				 m_ulDelvNPC				== src.m_ulDelvNPC						&&
				 m_ulAwardNPC				== src.m_ulAwardNPC						&&
				 m_bSkillTask				== src.m_bSkillTask						&&
				 m_bCanSeekOut				== src.m_bCanSeekOut					&&
				 m_bShowDirection			== src.m_bShowDirection					&&
				 m_bMarriage				== src.m_bMarriage						&&
				 m_bSwitchSceneFail			== src.m_bSwitchSceneFail				&&
				 m_bHidden					== src.m_bHidden						&&
				 m_bDeliverySkill			== src.m_bDeliverySkill					&&
				 m_iDeliveredSkillID		== src.m_iDeliveredSkillID				&&
				 m_iDeliveredSkillLevel		== src.m_iDeliveredSkillLevel			&&
				 m_bShowGfxFinished			== src.m_bShowGfxFinished				&&
				 m_bChangePQRanking			== src.m_bChangePQRanking				&&
				 m_bCompareItemAndInventory == src.m_bCompareItemAndInventory		&&
				 m_ulInventorySlotNum		== src.m_ulInventorySlotNum				&&
				 m_bPQTask					== src.m_bPQTask						&&
				 m_ulPQExpCnt				== src.m_ulPQExpCnt						&&
				 m_bPQSubTask				== src.m_bPQSubTask						&&
				 m_bClearContrib			== src.m_bClearContrib					&&
				 m_iPremiseFactionContrib	== src.m_iPremiseFactionContrib			&&
				 m_bShowByFactionContrib	== src.m_bShowByFactionContrib			&&
				 m_iPremNeedRecordTasksNum  == src.m_iPremNeedRecordTasksNum		&&
				 m_bShowByNeedRecordTasksNum== src.m_bShowByNeedRecordTasksNum		&&
				 m_bAccountTaskLimit		== src.m_bAccountTaskLimit				&&
				 m_ulAccountTaskLimitCnt	== src.m_ulAccountTaskLimitCnt			&&
				 m_ulPremise_Lev_Min		== src.m_ulPremise_Lev_Min				&&
				 m_ulPremise_Lev_Max		== src.m_ulPremise_Lev_Max				&&
				 m_iPremHistoryStageIndex[0]== src.m_iPremHistoryStageIndex[0]		&&
				 m_iPremHistoryStageIndex[1]== src.m_iPremHistoryStageIndex[1]		&&
				 m_ulPremGeneralCardCount	== src.m_ulPremGeneralCardCount			&&
				 m_bShowByLev				== src.m_bShowByLev						&&
				 m_bShowByItems				== src.m_bShowByItems					&&
				 m_ulGivenCmnCount			== src.m_ulGivenCmnCount				&&
				 m_ulGivenTskCount			== src.m_ulGivenTskCount				&&
				 m_ulPremise_Deposit		== src.m_ulPremise_Deposit				&&
				 m_bShowByDeposit			== src.m_bShowByDeposit					&&
				 m_lPremise_Reputation		== src.m_lPremise_Reputation			&&
				 m_lPremise_RepuMax			== src.m_lPremise_RepuMax				&&
				 m_bShowByRepu				== src.m_bShowByRepu					&&
				 m_bShowByPreTask			== src.m_bShowByPreTask					&&
				 m_ulPremise_Task_Least_Num == src.m_ulPremise_Task_Least_Num		&&
				 m_ulPremise_Period			== src.m_ulPremise_Period				&&
				 m_bShowByPeriod			== src.m_bShowByPeriod					&&
				 m_ulPremise_Faction		== src.m_ulPremise_Faction				&&
				 m_bShowByFaction			== src.m_bShowByFaction					&&
				 m_ulGender					== src.m_ulGender						&&
				 m_bShowByGender			== src.m_bShowByGender					&&
				 m_bShowByOccup				== src.m_bShowByOccup					&&
				 m_bPremise_Spouse			== src.m_bPremise_Spouse				&&
				 m_bShowBySpouse			== src.m_bShowBySpouse					&&
				 m_bGM						== src.m_bGM							&&
				 m_bShieldUser				== src.m_bShieldUser					&&
				 m_bShowByRMB				== src.m_bShowByRMB						&&
				 m_ulPremRMBMin				== src.m_ulPremRMBMin					&&
				 m_ulPremRMBMax				== src.m_ulPremRMBMax					&&
				 m_bCharTime				== src.m_bCharTime						&&
				 m_bShowByCharTime			== src.m_bShowByCharTime				&&
				 m_iCharStartTime			== src.m_iCharStartTime					&&
				 m_iCharEndTime				== src.m_iCharEndTime					&&
				 m_tmCharEndTime			== src.m_tmCharEndTime					&&
				 m_ulCharTimeGreaterThan	== src.m_ulCharTimeGreaterThan			&&
				 m_ulPremise_Cotask			== src.m_ulPremise_Cotask				&&
				 m_ulCoTaskCond				== src.m_ulCoTaskCond					&&
				 m_DynTaskType				== src.m_DynTaskType					&&
				 m_ulSpecialAward			== src.m_ulSpecialAward					&&
				 m_bTeamwork				== src.m_bTeamwork						&&
				 m_bRcvByTeam				== src.m_bRcvByTeam						&&
				 m_bSharedTask				== src.m_bSharedTask					&&
				 m_bSharedAchieved			== src.m_bSharedAchieved				&&
				 m_bCheckTeammate			== src.m_bCheckTeammate					&&
				 m_fTeammateDist			== src.m_fTeammateDist					&&
				 m_bAllFail					== src.m_bAllFail						&&
				 m_bCapFail					== src.m_bCapFail						&&
				 m_bCapSucc					== src.m_bCapSucc						&&
				 m_fSuccDist				== src.m_fSuccDist						&&
				 m_bDismAsSelfFail			== src.m_bDismAsSelfFail				&&
				 m_bRcvChckMem				== src.m_bRcvChckMem					&&
				 m_fRcvMemDist				== src.m_fRcvMemDist					&&
				 m_bCntByMemPos				== src.m_bCntByMemPos					&&
				 m_fCntMemDist				== src.m_fCntMemDist					&&
				 m_bAllSucc					== src.m_bAllSucc						&&
				 m_bCoupleOnly				== src.m_bCoupleOnly					&&
				 m_bDistinguishedOcc		== src.m_bDistinguishedOcc				&&
				 m_bShowByTeam				== src.m_bShowByTeam					&&
				 m_bPremNeedComp			== src.m_bPremNeedComp					&&
				 m_nPremExp1AndOrExp2		== src.m_nPremExp1AndOrExp2				&&
				 m_Prem1KeyValue			== src.m_Prem1KeyValue					&&
				 m_Prem2KeyValue			== src.m_Prem2KeyValue					&&
				 m_enumMethod				== src.m_enumMethod						&&
				 m_enumFinishType			== src.m_enumFinishType					&&
				 m_ulGoldWanted				== src.m_ulGoldWanted					&&
				 m_iFactionContribWanted	== src.m_iFactionContribWanted			&&
				 m_iFactionExpContribWanted	== src.m_iFactionExpContribWanted		&&
				 m_ulNPCToProtect			== src.m_ulNPCToProtect					&&
				 m_ulProtectTimeLen			== src.m_ulProtectTimeLen				&&
				 m_ulNPCMoving				== src.m_ulNPCMoving					&&
				 m_ulNPCDestSite			== src.m_ulNPCDestSite					&&
				 m_ulReachSiteId			== src.m_ulReachSiteId					&&
				 m_ulWaitTime				== src.m_ulWaitTime						&&
				 m_ulLeaveSiteId			== src.m_ulLeaveSiteId					&&
				 m_bFinNeedComp				== src.m_bFinNeedComp					&&
				 m_nFinExp1AndOrExp2		== src.m_nFinExp1AndOrExp2				&&
				 m_Fin1KeyValue				== src.m_Fin1KeyValue					&&
				 m_Fin2KeyValue				== src.m_Fin2KeyValue					&&
				 m_ulAwardType_S			== src.m_ulAwardType_S					&&
				 m_ulAwardType_F			== src.m_ulAwardType_F					&&
				 m_bIsLibraryTask			== src.m_bIsLibraryTask					&&
				 m_fLibraryTasksProbability	== src.m_fLibraryTasksProbability);
}
bool ATaskTemplFixedData::LoadFixedDataFromBinFile(FILE* fp)
{
	LOG_DELETE(m_Award_S);
	LOG_DELETE(m_Award_F);
	LOG_DELETE(m_AwByRatio_S);
	LOG_DELETE(m_AwByRatio_F);
	LOG_DELETE(m_AwByItems_S);
	LOG_DELETE(m_AwByItems_F);
	LOG_DELETE_ARR(m_tmStart);
	LOG_DELETE_ARR(m_tmEnd);
	LOG_DELETE_ARR(m_plChangeKey);
	LOG_DELETE_ARR(m_plChangeKeyValue);
	LOG_DELETE_ARR(m_pbChangeType);
	LOG_DELETE_ARR(m_PremItems);
	LOG_DELETE_ARR(m_GivenItems);
	LOG_DELETE_ARR(m_TeamMemsWanted);
	LOG_DELETE_ARR(m_ItemsWanted);
	LOG_DELETE_ARR(m_PlayerWanted);
	LOG_DELETE_ARR(m_MonsterWanted);
	LOG_DELETE_ARR(m_pszSignature);
	LOG_DELETE_ARR(m_pszExp);
	LOG_DELETE_ARR(m_pExpArr);
	LOG_DELETE_ARR(m_pTaskChar);
	LOG_DELETE_ARR(m_pszPQExp);
	LOG_DELETE_ARR(m_pPQExpArr);
	LOG_DELETE_ARR(m_MonstersContrib);

	LOG_DELETE_ARR(m_pDelvRegion);
	LOG_DELETE_ARR(m_pEnterRegion);
	LOG_DELETE_ARR(m_pLeaveRegion);
	LOG_DELETE_ARR(m_pReachSite);
	LOG_DELETE_ARR(m_pLeaveSite);
	LOG_DELETE_ARR(m_PremTitles);

	unsigned long i;
	fread(this, sizeof(*this), 1, fp);

	convert_txt(m_szName, MAX_TASK_NAME_LEN, (namechar)m_ID);

	m_Award_S		= new AWARD_DATA;
	g_ulNewCount++;
	m_Award_F		= new AWARD_DATA;
	g_ulNewCount++;
	m_AwByRatio_S	= new AWARD_RATIO_SCALE;
	g_ulNewCount++;
	m_AwByRatio_F	= new AWARD_RATIO_SCALE;
	g_ulNewCount++;
	m_AwByItems_S	= new AWARD_ITEMS_SCALE;
	g_ulNewCount++;
	m_AwByItems_F	= new AWARD_ITEMS_SCALE;
	g_ulNewCount++;
	m_tmStart		= NULL;
	m_tmEnd			= NULL;
	m_plChangeKey   = NULL;
	m_plChangeKeyValue = NULL;
	m_pbChangeType  = NULL;
	m_PremItems		= NULL;
	m_GivenItems	= NULL;
	m_TeamMemsWanted= NULL;
	m_ItemsWanted	= NULL;
	m_PlayerWanted  = NULL;
	m_MonsterWanted	= NULL;
	m_pszSignature	= NULL;
	m_pszExp        = NULL;
	m_pExpArr       = NULL;
	m_pTaskChar     = NULL;
	m_pszPQExp      = NULL;
	m_pPQExpArr     = NULL;
	m_MonstersContrib = NULL;
	m_PremTitles	= NULL;

	if (m_bHasSign)
	{
		m_pszSignature = new task_char[MAX_TASK_NAME_LEN];
		g_ulNewCount++;
		fread(m_pszSignature, sizeof(task_char), MAX_TASK_NAME_LEN, fp);
		convert_txt(m_pszSignature, MAX_TASK_NAME_LEN, (namechar)m_ID);
	}

	if (m_ulTimetable)
	{
		m_tmStart	= new task_tm[m_ulTimetable];
		g_ulNewCount++;
		m_tmEnd		= new task_tm[m_ulTimetable];
		g_ulNewCount++;
	}

	for (i = 0; i < m_ulTimetable; i++)
	{
		fread(&m_tmStart[i], sizeof(task_tm), 1, fp);
		fread(&m_tmEnd[i], sizeof(task_tm), 1, fp);
	}

	if (m_ulChangeKeyCnt)
	{
		m_plChangeKey      = new long[m_ulChangeKeyCnt];
		m_plChangeKeyValue = new long[m_ulChangeKeyCnt];
		m_pbChangeType     = new bool[m_ulChangeKeyCnt];
	}

	for (i=0; i<m_ulChangeKeyCnt; i++)
	{
		fread(&m_plChangeKey[i], sizeof(long), 1, fp);
		fread(&m_plChangeKeyValue[i], sizeof(long), 1, fp);
		fread(&m_pbChangeType[i], sizeof(bool), 1, fp);
	}

	if (m_ulPQExpCnt)
	{
		m_pszPQExp = new char[m_ulPQExpCnt][TASK_AWARD_MAX_DISPLAY_CHAR_LEN];
		memset(m_pszPQExp, 0, m_ulPQExpCnt*TASK_AWARD_MAX_DISPLAY_CHAR_LEN);

		m_pPQExpArr = new TASK_EXPRESSION[m_ulPQExpCnt][TASK_AWARD_MAX_DISPLAY_CHAR_LEN];
		memset(m_pPQExpArr, 0, sizeof(TASK_EXPRESSION)*m_ulPQExpCnt*TASK_AWARD_MAX_DISPLAY_CHAR_LEN);
		
		for (i=0; i<m_ulPQExpCnt; i++)
		{	
			fread(m_pszPQExp[i], 1, TASK_AWARD_MAX_DISPLAY_CHAR_LEN, fp);
			fread(m_pPQExpArr[i], sizeof(TASK_EXPRESSION), TASK_AWARD_MAX_DISPLAY_CHAR_LEN, fp);
		}
	}

	if (m_ulMonsterContribCnt)
	{
		m_MonstersContrib = new MONSTERS_CONTRIB[m_ulMonsterContribCnt];
		g_ulNewCount++;
	}

	for (i = 0; i < m_ulMonsterContribCnt; i++)
	{
		MONSTERS_CONTRIB& mc = m_MonstersContrib[i];
		fread(&mc, sizeof(mc), 1, fp);
	}

	// version 80
	if (m_ulDelvRegionCnt>0)
	{
		m_pDelvRegion = new Task_Region[m_ulDelvRegionCnt];
		g_ulNewCount++;
	}
	else m_pDelvRegion = NULL;

	for (i=0;i<m_ulDelvRegionCnt;i++)
	{
		Task_Region& t = m_pDelvRegion[i];
		fread(&t,sizeof(Task_Region),1,fp);
	}
	if (m_ulEnterRegionCnt>0)
	{
		m_pEnterRegion = new Task_Region[m_ulEnterRegionCnt];
		g_ulNewCount++;
	}
	else m_pEnterRegion = NULL;

	for (i=0;i<m_ulEnterRegionCnt;i++)
	{
		Task_Region& t = m_pEnterRegion[i];
		fread(&t,sizeof(Task_Region),1,fp);
	}
	if (m_ulLeaveRegionCnt>0)
	{
		m_pLeaveRegion = new Task_Region[m_ulLeaveRegionCnt];
		g_ulNewCount++;
	}
	else m_pLeaveRegion = NULL;

	for (i=0;i<m_ulLeaveRegionCnt;i++)
	{
		Task_Region& t = m_pLeaveRegion[i];
		fread(&t,sizeof(Task_Region),1,fp);
	}

	// 任务开启条件及方式
	if (m_ulPremItems)
	{
		m_PremItems = new ITEM_WANTED[m_ulPremItems];
		g_ulNewCount++;
	}

	for (i = 0; i < m_ulPremItems; i++)
	{
		ITEM_WANTED& iw = m_PremItems[i];
		fread(&iw, sizeof(iw), 1, fp);
	}

	m_ulGivenCmnCount = 0;
	m_ulGivenTskCount = 0;

	if (m_ulGivenItems)
	{
		m_GivenItems = new ITEM_WANTED[m_ulGivenItems];
		g_ulNewCount++;
	}

	for (i = 0; i < m_ulGivenItems; i++)
	{
		ITEM_WANTED& iw = m_GivenItems[i];
		fread(&iw, sizeof(iw), 1, fp);

		if (iw.m_bCommonItem) m_ulGivenCmnCount++;
		else m_ulGivenTskCount++;
	}

	if (m_bTeamwork)
	{
		if (m_ulTeamMemsWanted)
		{
			m_TeamMemsWanted = new TEAM_MEM_WANTED[m_ulTeamMemsWanted];
			g_ulNewCount++;
		}

		for (i = 0; i < m_ulTeamMemsWanted; i++)
		{
			TEAM_MEM_WANTED& tmw = m_TeamMemsWanted[i];
			fread(&tmw, sizeof(tmw), 1, fp);
		}
	}
	if (m_iPremTitleNumTotal)
	{
		m_PremTitles = new int[m_iPremTitleNumTotal];
		g_ulNewCount++;
		for (i = 0; i < m_iPremTitleNumTotal; i++)
		{
			fread(&m_PremTitles[i], sizeof(int), 1, fp);
		}
	}

	/* 任务完成的方式及条件 */

	if (m_ulMonsterWanted)
	{
		m_MonsterWanted = new MONSTER_WANTED[m_ulMonsterWanted];
		g_ulNewCount++;
	}

	for (i = 0; i < m_ulMonsterWanted; i++)
	{
		MONSTER_WANTED& mw = m_MonsterWanted[i];
		fread(&mw, sizeof(mw), 1, fp);
	}

	if (m_ulPlayerWanted)
	{
		m_PlayerWanted = new PLAYER_WANTED[m_ulPlayerWanted];
		g_ulNewCount++;
	}
	for (i = 0; i < m_ulPlayerWanted; ++i)
	{
		PLAYER_WANTED& pw = m_PlayerWanted[i];
		fread(&pw, sizeof(pw), 1, fp);
	}

	if (m_ulItemsWanted)
	{
		m_ItemsWanted = new ITEM_WANTED[m_ulItemsWanted];
		g_ulNewCount++;
	}

	for (i = 0; i < m_ulItemsWanted; i++)
	{
		ITEM_WANTED& iw = m_ItemsWanted[i];
		fread(&iw, sizeof(iw), 1, fp);
	}

	if (m_ulExpCnt)
	{
		m_pszExp = new char[m_ulExpCnt][TASK_AWARD_MAX_DISPLAY_CHAR_LEN];
		memset(m_pszExp, 0, m_ulExpCnt*TASK_AWARD_MAX_DISPLAY_CHAR_LEN);

		m_pExpArr = new TASK_EXPRESSION[m_ulExpCnt][TASK_AWARD_MAX_DISPLAY_CHAR_LEN];
		memset(m_pExpArr, 0, sizeof(TASK_EXPRESSION)*m_ulExpCnt*TASK_AWARD_MAX_DISPLAY_CHAR_LEN);
		
		for (i=0; i<m_ulExpCnt; i++)
		{	
			fread(m_pszExp[i], 1, TASK_AWARD_MAX_DISPLAY_CHAR_LEN, fp);
			fread(m_pExpArr[i], sizeof(TASK_EXPRESSION), TASK_AWARD_MAX_DISPLAY_CHAR_LEN, fp);
		}
	}

	if (m_ulTaskCharCnt)
	{
		m_pTaskChar = new task_char[m_ulTaskCharCnt][TASK_AWARD_MAX_DISPLAY_CHAR_LEN];
		memset(m_pTaskChar, 0, sizeof(task_char)*m_ulTaskCharCnt*TASK_AWARD_MAX_DISPLAY_CHAR_LEN);

		for (i=0; i<m_ulTaskCharCnt; i++)
		{	
			fread(m_pTaskChar[i], sizeof(task_char), TASK_AWARD_MAX_DISPLAY_CHAR_LEN, fp);
		}
	}

	// version 80
	if (m_ulReachSiteCnt>0)
	{
		m_pReachSite = new Task_Region[m_ulReachSiteCnt];
		g_ulNewCount++;
	}
	else m_pReachSite = NULL;

	for (i=0;i<m_ulReachSiteCnt;i++)
	{
		Task_Region& t = m_pReachSite[i];
		fread(&t,sizeof(Task_Region),1,fp);
	}
	if (m_ulLeaveSiteCnt>0)
	{
		m_pLeaveSite = new Task_Region[m_ulLeaveSiteCnt];
		g_ulNewCount++;
	}
	else m_pLeaveSite = NULL;

	for (i=0;i<m_ulLeaveSiteCnt;i++)
	{
		Task_Region& t = m_pLeaveSite[i];
		fread(&t,sizeof(Task_Region),1,fp);
	}

	LoadAwardDataBin(fp, *m_Award_S, _task_templ_cur_version);
	LoadAwardDataBin(fp, *m_Award_F, _task_templ_cur_version);

	LoadAwardDataRatioScale(fp, *m_AwByRatio_S, _task_templ_cur_version);
	LoadAwardDataRatioScale(fp, *m_AwByRatio_F, _task_templ_cur_version);

	LoadAwardDataItemsScale(fp, *m_AwByItems_S, _task_templ_cur_version);
	LoadAwardDataItemsScale(fp, *m_AwByItems_F, _task_templ_cur_version);

	return true;
}

ATaskTempl* ATaskTempl::CreateEmptyTaskTempl()
{
	ATaskTempl* pTempl = new ATaskTempl;
	g_ulNewCount++;
	pTempl->Init();
	return pTempl;
}

void ATaskTempl::AddSubTaskTempl(ATaskTempl* pSub)
{
	pSub->m_pParent = this;

	if (m_pFirstChild == NULL)
		m_pFirstChild = pSub;
	else
	{
		ATaskTempl* pSibling = m_pFirstChild;
		while (pSibling->m_pNextSibling) pSibling = pSibling->m_pNextSibling;
		pSibling->m_pNextSibling = pSub;
		pSub->m_pPrevSibling = pSibling;
	}
}

void ATaskTempl::InsertTaskTemplBefore(ATaskTempl* pInsert)
{
	pInsert->m_pParent = m_pParent;
	if (m_pParent && m_pParent->m_pFirstChild == this)
		m_pParent->m_pFirstChild = pInsert;
	pInsert->m_pPrevSibling = m_pPrevSibling;
	pInsert->m_pNextSibling = this;
	if (m_pPrevSibling) m_pPrevSibling->m_pNextSibling = pInsert;
	m_pPrevSibling = pInsert;
}

bool ATaskTempl::LoadFromTextFile(FILE* fp, bool bLoadDescript)
{
	unsigned long ulVersion;
	char line[TASK_MAX_LINE_LEN];

	if (!ReadLine(fp, line)) return false;
	sscanf(line, _format_version, &ulVersion);

	if (ulVersion > _task_templ_cur_version) return false;

	if (!Load(fp, ulVersion, true)) return false;

#ifndef _TASK_CLIENT
	// 如果有接受者上限，则记录任务的接受者id和起始时间
	if (m_ulMaxReceiver)
	{
#if DEBUG_LOG == 1
		if (!m_ulTimeLimit)
			TaskInterface::WriteLog(0, m_ID, 0, "MaxRcv Dont Have TimeLimit");
#endif
	}
#endif

	CheckDepth();
	return true;
}

bool ATaskTempl::LoadFromTextFile(const char* szPath, bool bLoadDescript)
{
	FILE* fp = fopen(szPath, "rb");
	if (fp == NULL) return false;
	if (!LoadFromTextFile(fp, bLoadDescript))
	{
		fclose(fp);
		return false;
	}

	fclose(fp);
	strcpy(m_szFilePath, szPath);
	return true;
}

bool ATaskTempl::LoadFromBinFile(FILE* fp)
{
	LoadBinary(fp);
	CheckDepth();
	return true;
}

unsigned long ATaskTempl::CheckSpecialAward(TaskInterface* pTask) const
{
	if (m_DynTaskType != enumDTTSpecialAward) return 0;
	if (!m_ulSpecialAward) return TASK_PREREQU_FAIL_SPECIAL_AWARD;

	special_award sa;
	pTask->GetSpecailAwardInfo(&sa);

	if (m_ulSpecialAward == sa.id1 || m_ulSpecialAward == sa.id2)
	{
		return 0;
	}
	else
	{
		return TASK_PREREQU_FAIL_SPECIAL_AWARD;
	}
}

int ATaskTempl::UnmarshalKillMonster(const char* pData)
{
	const char* p = pData;

	m_ulMonsterWanted = *p;
	p++;

#ifndef TASK_TEMPL_EDITOR

	if (m_ulMonsterWanted)
	{
		m_MonsterWanted = new MONSTER_WANTED[m_ulMonsterWanted];
		g_ulNewCount++;
	}

#endif

	size_t sz = m_ulMonsterWanted * sizeof(MONSTER_WANTED);
	if (sz)
	{
		memcpy(m_MonsterWanted, p, sz);
		p += sz;
	}

	return p - pData;
}

int ATaskTempl::UnmarshalCollectItems(const char* pData)
{
	const char* p = pData;

	m_ulItemsWanted = *(unsigned char*)p;
	p++;

	if (m_ulItemsWanted)
	{
#ifndef TASK_TEMPL_EDITOR
		m_ItemsWanted = new ITEM_WANTED[m_ulItemsWanted];
		g_ulNewCount++;
#endif

		size_t sz = m_ulItemsWanted * sizeof(ITEM_WANTED);
		memcpy(m_ItemsWanted, p, sz);
		p += sz;
	}

	m_ulGoldWanted = *(long*)p;
	p += sizeof(long);

	m_iFactionContribWanted = *(long*)p;
	p += sizeof(long);

	m_iFactionExpContribWanted = *(long*)p;
	p += sizeof(long);

	return p - pData;
}

int ATaskTempl::UnmarshalDynTask(const char* pData)
{
	Init();

	const char* p = pData;

	unsigned long token_mask1, token_mask2;

	token_mask1 = *(long*)p;
	p += sizeof(long);

	token_mask2 = *(long*)p;
	p += sizeof(long);

	int token_count = 0;

	// dyn type
	m_DynTaskType = *p;
	p++;

	// top task
	if (!m_pParent)
	{
		switch (m_DynTaskType)
		{
		case enumDTTSpecialAward:
		case enumDTTGiftCard:
			p += UnmarshalSpecialAwardData(p);
			break;
		default:
			break;
		}
	}

	// id
	m_ID = *(long*)p;
	p += sizeof(long);

	// name
	char len = *p; p++;
	len *= sizeof(task_char);
	memcpy(m_szName, p, len);
	p += len;

	// choose one
	m_bChooseOne = (*p != 0);
	p++;

	// rand one
	m_bRandOne = (*p != 0);
	p++;

	// in order
	m_bExeChildInOrder = (*p != 0);
	p++;

	// parent fail
	m_bParentAlsoFail = (*p != 0);
	p++;

	// parent succ
	m_bParentAlsoSucc = (*p != 0);
	p++;

	// give up
	m_bCanGiveUp = (*p != 0);
	p++;

	// redo
	m_bCanRedo = (*p != 0);
	p++;

	// redo after fail
	m_bCanRedoAfterFailure = (*p != 0);
	p++;

	// clear as give up
	m_bClearAsGiveUp = (*p != 0);
	p++;

	// record
	m_bNeedRecord = (*p != 0);
	p++;

	// die
	m_bFailAsPlayerDie = (*p != 0);
	p++;

	// auto deliver
	m_bAutoDeliver = (*p != 0);
	p++;

	// death trig
	m_bDeathTrig = (*p != 0);
	p++;

	// clear acquired
	m_bClearAcquired = (*p != 0);
	p++;

	// spouse
	m_bPremise_Spouse = (*p != 0);
	p++;

	// teamwork
	m_bTeamwork = (*p != 0);
	p++;

	// direction
	m_bShowDirection = (*p != 0);
	p++;

	// level
	m_ulPremise_Lev_Min = *(unsigned char*)p; p++;
	m_ulPremise_Lev_Max = *(unsigned char*)p; p++;

	// 0: time limit
	if ((1 << token_count++) & token_mask1)
	{
		m_ulTimeLimit = *(long*)p;
		p += sizeof(long);
	}

	// 1: reputation
	if ((1 << token_count++) & token_mask1)
	{
		m_lPremise_Reputation = *(long*)p;
		p += sizeof(long);
	}

	// 2: period
	if ((1 << token_count++) & token_mask1)
	{
		m_ulPremise_Period = *(unsigned short*)p;
		p += sizeof(unsigned short);
	}

	// 3: prem items
	if ((1 << token_count++) & token_mask1)
	{
		m_ulPremItems = *(unsigned char*)p;
		p++;

#ifndef TASK_TEMPL_EDITOR
		m_PremItems = new ITEM_WANTED[m_ulPremItems];
		g_ulNewCount++;
#endif

		size_t sz = m_ulPremItems * sizeof(ITEM_WANTED);
		memcpy(m_PremItems, p, sz);
		p += sz;
	}

	// 4: delv in zone
	if ((1 << token_count++) & token_mask1)
	{
		m_bDelvInZone = (*p != 0);
		p++;

		m_ulDelvWorld = *(long*)p;
		p += sizeof(long);

		m_ulDelvRegionCnt = *(unsigned long*)p;
		p += sizeof(unsigned long);

		unsigned long i;
		for (i=0;i<m_ulDelvRegionCnt;i++)
		{
			m_pDelvRegion[i] = *(Task_Region*)p;
			p += sizeof(Task_Region);
		}
/*		m_DelvMinVert = *(ZONE_VERT*)p;
		p += sizeof(ZONE_VERT);

		m_DelvMaxVert = *(ZONE_VERT*)p;
		p += sizeof(ZONE_VERT);*/
	}

	// 5: trans to
	if ((1 << token_count++) & token_mask1)
	{
		m_bTransTo = (*p != 0);
		p++;

		m_ulTransWldId = *(long*)p;
		p += sizeof(long);

		m_TransPt = *(ZONE_VERT*)p;
		p += sizeof(ZONE_VERT);
	}

	// 6: given items
	if ((1 << token_count++) & token_mask1)
	{
		m_ulGivenItems = *p;
		p++;

		m_ulGivenCmnCount = *p;
		p++;

		m_ulGivenTskCount = *p;
		p++;

#ifndef TASK_TEMPL_EDITOR
		m_GivenItems = new ITEM_WANTED[m_ulGivenItems];
		g_ulNewCount++;
#endif

		size_t sz = m_ulGivenItems * sizeof(ITEM_WANTED);
		memcpy(m_GivenItems, p, sz);
		p += sz;
	}

	// 7: deposit
	if ((1 << token_count++) & token_mask1)
	{
		m_ulPremise_Deposit = *(long*)p;
		p += sizeof(long);
	}

	// 8: pre task
	if ((1 << token_count++) & token_mask1)
	{
		m_ulPremise_Task_Count = *p;
		p++;

		m_ulPremise_Task_Least_Num = *p;
		p++;

		size_t sz = sizeof(long) * m_ulPremise_Task_Count;
		memcpy(m_ulPremise_Tasks, p, sz);
		p += sz;
	}

	// 9: gender
	if ((1 << token_count++) & token_mask1)
	{
		m_ulGender = *p;
		p++;
	}

	// 10: occupation
	if ((1 << token_count++) & token_mask1)
	{
		m_ulOccupations = *p;
		p++;

		size_t sz = sizeof(long) * m_ulOccupations;
		memcpy(m_Occupations, p, sz);
		p += sz;
	}

	// 11: mutex task
	if ((1 << token_count++) & token_mask1)
	{
		m_ulMutexTaskCount = *p;
		p++;

		size_t sz = sizeof(long) * m_ulMutexTaskCount;
		memcpy(m_ulMutexTasks, p, sz);
		p += sz;
	}

	// 12: time table
	if ((1 << token_count++) & token_mask1)
	{
		m_ulTimetable = *p;
		p++;

#ifndef TASK_TEMPL_EDITOR
		m_tmStart = new task_tm[m_ulTimetable];
		g_ulNewCount++;

		m_tmEnd = new task_tm[m_ulTimetable];
		g_ulNewCount++;
#endif

		size_t sz;

		sz = sizeof(char) * m_ulTimetable;

		memcpy(m_tmType, p, sz);
		p += sz;

		sz = sizeof(task_tm) * m_ulTimetable;

		memcpy(m_tmStart, p, sz);
		p += sz;

		memcpy(m_tmEnd, p, sz);
		p += sz;
	}

	// method
	m_enumMethod = *(unsigned char*)p;
	p++;

	switch (m_enumMethod)
	{
	case enumTMKillNumMonster:
		p += UnmarshalKillMonster(p);
		break;
	case enumTMCollectNumArticle:
		p += UnmarshalCollectItems(p);
		break;
	case enumTMReachSite:
		m_ulReachSiteId = *(long*)p;
		p += sizeof(long);
		memcpy(m_pReachSite, p, sizeof(Task_Region)*m_ulReachSiteCnt);
		p += sizeof(Task_Region)*m_ulReachSiteCnt;
//		memcpy(&m_ReachSiteMax, p, sizeof(ZONE_VERT));
//		p += sizeof(ZONE_VERT);
		break;
	case enumTMLeaveSite:
		m_ulLeaveSiteId = *(long*)p;
		p += sizeof(long);
		memcpy(m_pLeaveSite, p, sizeof(Task_Region)*m_ulLeaveSiteCnt);
		p += sizeof(ZONE_VERT);
//		memcpy(&m_LeaveSiteMax, p, sizeof(ZONE_VERT));
//		p += sizeof(ZONE_VERT);
		break;
	case enumTMWaitTime:
		m_ulWaitTime = *(long*)p;
		p += sizeof(long);
		break;
	}

	// finish type
	m_enumFinishType = *(unsigned char*)p;
	p++;

	// award
	p += m_Award_S->UnmarshalBasicData(p);

	// talks
	p += unmarshal_str(p, m_pwstrDescript);
	p += unmarshal_str(p, m_pwstrOkText);
	p += unmarshal_str(p, m_pwstrNoText);
	p += unmarshal_talk_proc(&m_DelvTaskTalk, p);
	p += unmarshal_talk_proc(&m_UnqualifiedTalk, p);
	p += unmarshal_talk_proc(&m_DelvItemTalk, p);
	p += unmarshal_talk_proc(&m_ExeTalk, p);
	p += unmarshal_talk_proc(&m_AwardTalk, p);

#ifndef _TASK_CLIENT
	CheckMask();
#else
	SyncTaskType();
#endif

	m_nSubCount = *(int*)p;
	p += sizeof(int);

	for (int i = 0; i < m_nSubCount; i++)
	{
		ATaskTempl* pSub = new ATaskTempl;
		g_ulNewCount++;
		AddSubTaskTempl(pSub);
		p += pSub->UnmarshalDynTask(p);
	}

	SynchID();
	if (!m_pParent) CheckDepth();

	return p - pData;
}

int ATaskTempl::UnmarshalSpecialAwardData(const char* pData)
{
	const char* p = pData;
	m_ulSpecialAward = *(long*)p;
	p += sizeof(long);

	return p - pData;
}

bool ATaskTempl::Load(FILE* fp, unsigned long ulVersion, bool bTextFile)
{
	char line[TASK_MAX_LINE_LEN];

	Init();

	if (!LoadFixedDataFromTextFile(fp, ulVersion)
	 || !LoadDescription(fp)) return false;

	if (ulVersion >= 9 && !LoadTribute(fp))
		return false;

#ifndef _TASK_CLIENT
	CheckMask();
#else
	SyncTaskType();
#endif

	if (ulVersion >= 6)
	{
		if (m_DelvTaskTalk.load(fp) != 0)
			return false;

		if (ulVersion >= 15 && m_UnqualifiedTalk.load(fp) != 0)
			return false;

		if (m_DelvItemTalk.load(fp) != 0)
			return false;

		if (m_ExeTalk.load(fp) != 0)
			return false;

		if (m_AwardTalk.load(fp) != 0)
			return false;
	}

	if (!ReadLine(fp, line)) return false;
	sscanf(line, _format_task_count, &m_nSubCount);

	for (int i = 0; i < m_nSubCount; i++)
	{
		ATaskTempl* pSub = new ATaskTempl;
		g_ulNewCount++;
		AddSubTaskTempl(pSub);
		if (!pSub->Load(fp, ulVersion, bTextFile))
		{
			LOG_DELETE(pSub);
			return false;
		}
	}

	SynchID();
	return true;
}

bool ATaskTempl::LoadBinary(FILE* fp)
{
	LoadFixedDataFromBinFile(fp);
	LoadDescriptionBin(fp);
	LoadTributeBin(fp);

#ifndef _TASK_CLIENT
	CheckMask();
#else
	SyncTaskType();
#endif


	namechar code = (namechar)m_ID;

	m_DelvTaskTalk.load(fp);
	convert_talk_text(&m_DelvTaskTalk, code);

	m_UnqualifiedTalk.load(fp);
	convert_talk_text(&m_UnqualifiedTalk, code);

	m_DelvItemTalk.load(fp);
	convert_talk_text(&m_DelvItemTalk, code);

	m_ExeTalk.load(fp);
	convert_talk_text(&m_ExeTalk, code);

	m_AwardTalk.load(fp);
	convert_talk_text(&m_AwardTalk, code);

	fread(&m_nSubCount, sizeof(m_nSubCount), 1, fp);

	for (int i = 0; i < m_nSubCount; i++)
	{
		ATaskTempl* pSub = new ATaskTempl;
		g_ulNewCount++;
		AddSubTaskTempl(pSub);
		pSub->LoadBinary(fp);
	}

	SynchID();
	return true;
}

bool ATaskTempl::LoadDescription(FILE* fp)
{
	size_t len = 0;
	char line[TASK_MAX_LINE_LEN];

	if (!ReadLine(fp, line)) return false;
	sscanf(line, _format_descpt_len, &len);

	m_pwstrDescript = new task_char[len+1];
	m_pwstrDescript[len] = L'\0';
	g_ulNewCount++;

	if (len)
	{
		fread(m_pwstrDescript, sizeof(task_char), len, fp);
		ReadLine(fp, line);
	}

	len = 0;
	if (!ReadLine(fp, line)) return false;
	sscanf(line, _format_descpt_len, &len);

	m_pwstrOkText = new task_char[len+1];
	m_pwstrOkText[len] = L'\0';
	g_ulNewCount++;

	if (len)
	{
		fread(m_pwstrOkText, sizeof(task_char), len, fp);
		ReadLine(fp, line);
	}

	len = 0;
	if (!ReadLine(fp, line)) return false;
	sscanf(line, _format_descpt_len, &len);

	m_pwstrNoText = new task_char[len+1];
	m_pwstrNoText[len] = L'\0';
	g_ulNewCount++;

	if (len)
	{
		fread(m_pwstrNoText, sizeof(task_char), len, fp);
		ReadLine(fp, line);
	}

	return true;
}

bool ATaskTempl::LoadDescriptionBin(FILE* fp)
{
	size_t len = 0;

	fread(&len, sizeof(len), 1, fp);
	m_pwstrDescript = new task_char[len+1];
	m_pwstrDescript[len] = L'\0';

	if (len)
	{
		fread(m_pwstrDescript, sizeof(task_char), len, fp);
		convert_txt(m_pwstrDescript, len, (namechar)m_ID);
	}

	g_ulNewCount++;

	len = 0;
	fread(&len, sizeof(len), 1, fp);
	m_pwstrOkText = new task_char[len+1];
	m_pwstrOkText[len] = L'\0';

	if (len)
	{
		fread(m_pwstrOkText, sizeof(task_char), len, fp);
		convert_txt(m_pwstrOkText, len, (namechar)m_ID);
	}

	g_ulNewCount++;

	len = 0;
	fread(&len, sizeof(len), 1, fp);
	m_pwstrNoText = new task_char[len+1];
	m_pwstrNoText[len] = L'\0';

	if (len)
	{
		fread(m_pwstrNoText, sizeof(task_char), len, fp);
		convert_txt(m_pwstrNoText, len, (namechar)m_ID);
	}

	g_ulNewCount++;

	return true;
}

bool ATaskTempl::LoadTribute(FILE* fp)
{
	size_t len;
	char line[TASK_MAX_LINE_LEN];

	len = 0;
	if (!ReadLine(fp, line)) return false;
	sscanf(line, _format_tribute_len, &len);

	m_pwstrTribute = new task_char[len+1];
	m_pwstrTribute[len] = L'\0';
	g_ulNewCount++;

	if (len)
	{
		fread(m_pwstrTribute, sizeof(task_char), len, fp);
		ReadLine(fp, line);
	}

	return true;
}

bool ATaskTempl::LoadTributeBin(FILE* fp)
{
	size_t len;
	
	len = 0;
	fread(&len, sizeof(len), 1, fp);
	m_pwstrTribute = new task_char[len+1];
	m_pwstrTribute[len] = L'\0';

	if (len)
	{
		fread(m_pwstrTribute, sizeof(task_char), len, fp);
		convert_txt(m_pwstrTribute, len, (namechar)m_ID);
	}

	g_ulNewCount++;

	return true;
}

#ifdef _TASK_CLIENT

void ATaskTempl::SaveDescription(FILE* fp)
{
	size_t len;

	len = m_pwstrDescript ? wcslen(m_pwstrDescript) : 0;
	fprintf(fp, _format_descpt_len, len);
	fprintf(fp, "\r\n");

	if (len)
	{
		fwrite(m_pwstrDescript, sizeof(task_char), len, fp);
		fprintf(fp, "\r\n");
	}

	len = m_pwstrOkText ? wcslen(m_pwstrOkText) : 0;
	fprintf(fp, _format_descpt_len, len);
	fprintf(fp, "\r\n");

	if (len)
	{
		fwrite(m_pwstrOkText, sizeof(task_char), len, fp);
		fprintf(fp, "\r\n");
	}

	len = m_pwstrNoText ? wcslen(m_pwstrNoText) : 0;
	fprintf(fp, _format_descpt_len, len);
	fprintf(fp, "\r\n");

	if (len)
	{
		fwrite(m_pwstrNoText, sizeof(task_char), len, fp);
		fprintf(fp, "\r\n");
	}
}

void ATaskTempl::SaveDescriptionBin(FILE* fp)
{
	size_t len;

	len = m_pwstrDescript ? wcslen(m_pwstrDescript) : 0;
	fwrite(&len, sizeof(len), 1, fp);

	if (len)
	{
		convert_txt(m_pwstrDescript, len, (namechar)m_ID);
		fwrite(m_pwstrDescript, sizeof(task_char), len, fp);
		convert_txt(m_pwstrDescript, len, (namechar)m_ID);
	}

	len = m_pwstrOkText ? wcslen(m_pwstrOkText) : 0;
	fwrite(&len, sizeof(len), 1, fp);

	if (len)
	{
		convert_txt(m_pwstrOkText, len, (namechar)m_ID);
		fwrite(m_pwstrOkText, sizeof(task_char), len, fp);
		convert_txt(m_pwstrOkText, len, (namechar)m_ID);
	}

	len = m_pwstrNoText ? wcslen(m_pwstrNoText) : 0;
	fwrite(&len, sizeof(len), 1, fp);

	if (len)
	{
		convert_txt(m_pwstrNoText, len, (namechar)m_ID);
		fwrite(m_pwstrNoText, sizeof(task_char), len, fp);
		convert_txt(m_pwstrNoText, len, (namechar)m_ID);
	}
}

void ATaskTempl::SaveTribute(FILE* fp)
{
	size_t len;

	len = m_pwstrTribute ? wcslen(m_pwstrTribute) : 0;
	fprintf(fp, _format_tribute_len, len);
	fprintf(fp, "\r\n");

	if (len)
	{
		fwrite(m_pwstrTribute, sizeof(task_char), len, fp);
		fprintf(fp, "\r\n");
	}
}

void ATaskTempl::SaveTributeBin(FILE* fp)
{
	size_t len;

	len = m_pwstrTribute ? wcslen(m_pwstrTribute) : 0;
	fwrite(&len, sizeof(len), 1, fp);

	if (len)
	{
		convert_txt(m_pwstrTribute, len, (namechar)m_ID);
		fwrite(m_pwstrTribute, sizeof(task_char), len, fp);
		convert_txt(m_pwstrTribute, len, (namechar)m_ID);
	}
}

#include "windows.h"
#define CODE_PAGE CP_ACP

class CSafeString
{
	LPSTR m_szBuf;
	LPWSTR m_wszBuf;

public:
	CSafeString(LPCSTR lpsz, int nLen) : m_wszBuf(NULL)
	{
		assert(lpsz);
		if (nLen < 0) nLen = strlen(lpsz);
		m_szBuf = new char[nLen+1];
		strncpy(m_szBuf, lpsz, nLen);
		m_szBuf[nLen] = '\0';
	}

	CSafeString(LPCSTR lpsz) : m_wszBuf(NULL)
	{
		assert(lpsz);
		int n = strlen(lpsz);
		m_szBuf = new char[n+1];
		strcpy(m_szBuf, lpsz);
	}
	
	CSafeString(LPCWSTR lpwsz, int nLen) : m_szBuf(NULL)
	{
		assert(lpwsz);
		if (nLen < 0) nLen = wcslen(lpwsz);
		m_wszBuf = new wchar_t[nLen+1];
		wcsncpy(m_wszBuf, lpwsz, nLen);
		m_wszBuf[nLen] = L'\0';
	}

	CSafeString(LPCWSTR lpwsz) : m_szBuf(NULL)
	{
		assert(lpwsz);
		int n = wcslen(lpwsz);
		m_wszBuf = new wchar_t[n+1];
		wcscpy(m_wszBuf, lpwsz);
	}

	operator LPCSTR() { return GetAnsi();}
	operator LPCWSTR() { return GetUnicode(); }
	
	CSafeString& operator= (const CSafeString& str)
	{
		delete[] m_szBuf;
		delete[] m_wszBuf;

		m_szBuf = NULL;
		m_wszBuf = NULL;

		if (str.m_szBuf)
		{
			m_szBuf = new char[strlen(str.m_szBuf)+1];
			strcpy(m_szBuf, str.m_szBuf);
		}

		if (str.m_wszBuf)
		{
			m_wszBuf = new wchar_t[wcslen(str.m_wszBuf)+1];
			wcscpy(m_wszBuf, str.m_wszBuf);
		}
		return *this;
	}

	LPCSTR GetAnsi()
	{
		if (m_szBuf) return m_szBuf;
		assert(m_wszBuf);
		
		int nCount = WideCharToMultiByte(
			CODE_PAGE,
			0,
			m_wszBuf,
			-1,
			NULL,
			0,
			NULL,
			NULL);
	
		m_szBuf = new char[nCount];

		WideCharToMultiByte(
			CODE_PAGE,
			0,
			m_wszBuf,
			-1,
			m_szBuf,
			nCount,
			NULL,
			NULL);
		return m_szBuf;
	}
	
	LPCWSTR GetUnicode()
	{
		if (m_wszBuf) return m_wszBuf;
		assert(m_szBuf);
		
		int nCount = MultiByteToWideChar(
			CODE_PAGE,
			0,
			m_szBuf,
			-1,
			NULL,
			0);

		m_wszBuf = new wchar_t[nCount];

		MultiByteToWideChar(
			CODE_PAGE,
			0,
			m_szBuf,
			-1,
			m_wszBuf,
			nCount);

		return m_wszBuf;
	}

	virtual ~CSafeString()
	{
		delete[] m_szBuf;
		delete[] m_wszBuf;
	}
};

void save_talk_text(FILE* fp, const talk_proc* pTalk)
{
	for (int i = 0; i < pTalk->num_window; i++)
	{
		talk_proc::window* win = &pTalk->windows[i];

		fprintf(fp, "【窗口提示文字】\r\n");
		fprintf(fp, CSafeString(win->talk_text));
		fprintf(fp, "\r\n");

		for (int j = 0; j < win->num_option; j++)
		{
			fprintf(fp, "【选项提示文字】\r\n");
			fprintf(fp, CSafeString(win->options[j].text));
			fprintf(fp, "\r\n");
		}
	}
}

void ATaskTempl::SaveAllText(FILE* fp)
{
	// Begin: export task ID into description txt file. [Yongdong,2010-1-4]
	char szID[MAX_PATH] = {0};
//	ultoa(m_ID,szID,sizeof(char)*MAX_PATH);
	sprintf(szID,"%lu",m_ID);

	fprintf(fp,"【任务ID】\r\n");
	fprintf(fp,szID);
	fprintf(fp,"\r\n");
	// End

	fprintf(fp, "【任务名称】\r\n");
	fprintf(fp, CSafeString(m_szName));
	fprintf(fp, "\r\n");

	fprintf(fp, "【任务描述】\r\n");
	fprintf(fp, CSafeString(m_pwstrDescript));
	fprintf(fp, "\r\n");

	fprintf(fp, "【发放对话】\r\n");
	save_talk_text(fp, &m_DelvTaskTalk);
	fprintf(fp, "\r\n");

	fprintf(fp, "【不符合条件对话】\r\n");
	save_talk_text(fp, &m_UnqualifiedTalk);
	fprintf(fp, "\r\n");

	fprintf(fp, "【未完成对话】\r\n");
	save_talk_text(fp, &m_ExeTalk);
	fprintf(fp, "\r\n");

	fprintf(fp, "【完成对话】\r\n");
	save_talk_text(fp, &m_AwardTalk);
	fprintf(fp, "\r\n");

	ATaskTempl* pChild = m_pFirstChild;
	while (pChild)
	{
		pChild->SaveAllText(fp);
		pChild = pChild->m_pNextSibling;
	}
}

inline void SaveAwardCand(const AWARD_ITEMS_CAND& ic, FILE* fp)
{
	unsigned long i;

	fprintf(fp, _format_rand_one, ic.m_bRandChoose);
	fprintf(fp, "\r\n");

	fprintf(fp, _format_item_wanted_num, ic.m_ulAwardItems);
	fprintf(fp, "\r\n");

	for (i = 0; i < ic.m_ulAwardItems; i++)
	{
		const ITEM_WANTED& iw = ic.m_AwardItems[i];

		fprintf(fp, _format_item_templ, iw.m_ulItemTemplId);
		fprintf(fp, "\r\n");

		fprintf(fp, _format_item_is_cmn, iw.m_bCommonItem);
		fprintf(fp, "\r\n");

		fprintf(fp, _format_item_num, iw.m_ulItemNum);
		fprintf(fp, "\r\n");

		fprintf(fp, _format_prob, iw.m_fProb);
		fprintf(fp, "\r\n");

		fprintf(fp, _format_period, iw.m_lPeriod);
		fprintf(fp, "\r\n");
	}
}

inline void SaveAwardCandBin(const AWARD_ITEMS_CAND& ic, FILE* fp)
{
	unsigned long i;

	fwrite(&ic.m_bRandChoose, sizeof(ic.m_bRandChoose), 1, fp);
	fwrite(&ic.m_ulAwardItems, sizeof(ic.m_AwardItems), 1, fp);

	for (i = 0; i < ic.m_ulAwardItems; i++)
	{
		const ITEM_WANTED& iw = ic.m_AwardItems[i];
		fwrite(&iw, sizeof(iw), 1, fp);
	}
}
inline void SaveAwardMonsters(const AWARD_MONSTERS_SUMMONED& ms, FILE* fp)
{
	unsigned long i;

	fprintf(fp, _format_award_rand_monster, ms.m_bRandChoose);
	fprintf(fp, "\r\n");

	fprintf(fp, _format_award_summon_radius, ms.m_ulSummonRadius);
	fprintf(fp, "\r\n");

	fprintf(fp, _format_award_death_disappear, ms.m_bDeathDisappear);
	fprintf(fp, "\r\n");

	for(i = 0; i < ms.m_ulMonsterNum; i++)
	{
		const MONSTERS_SUMMONED& monster = ms.m_Monsters[i];
		
		fprintf(fp, _format_award_monster_id, monster.m_ulMonsterTemplId);
		fprintf(fp, "\r\n");

		fprintf(fp, _format_award_monster_num, monster.m_ulMonsterNum);
		fprintf(fp, "\r\n");

		fprintf(fp, _format_award_monster_prob, monster.m_fSummonProb);
		fprintf(fp, "\r\n");

		fprintf(fp, _format_award_monster_period, monster.m_lPeriod);
		fprintf(fp, "\r\n");
	}
}

inline void SaveAwardMonstersBin(const AWARD_MONSTERS_SUMMONED& ms, FILE* fp)
{
	unsigned long i;

	fwrite(&ms.m_bRandChoose, sizeof(ms.m_bRandChoose), 1, fp);
	fwrite(&ms.m_ulSummonRadius, sizeof(ms.m_ulSummonRadius), 1, fp);
	fwrite(&ms.m_bDeathDisappear, sizeof(ms.m_bDeathDisappear), 1, fp);

	for (i = 0; i < ms.m_ulMonsterNum; i++)
	{
		const MONSTERS_SUMMONED& monster = ms.m_Monsters[i];
		fwrite(&monster, sizeof(monster), 1, fp);
	}
}

inline void SaveAwardPQRanking(const AWARD_PQ_RANKING& pr, FILE* fp)
{
	unsigned long i;

	fprintf(fp, _format_award_pq_by_prof, pr.m_bAwardByProf);
	fprintf(fp, "\r\n");

	for(i = 0; i < pr.m_ulRankingAwardNum; i++)
	{
		const RANKING_AWARD& rAward = pr.m_RankingAward[i];
		
		fprintf(fp, _format_award_pq_ranking, rAward.m_iRankingStart, rAward.m_iRankingEnd, rAward.m_ulAwardItemId, rAward.m_bCommonItem, rAward.m_ulAwardItemNum, rAward.m_lPeriod);
		fprintf(fp, "\r\n");
	}
}

inline void SaveAwardPQRankingBin(const AWARD_PQ_RANKING& pr, FILE* fp)
{
	unsigned long i;

	fwrite(&pr.m_bAwardByProf, sizeof(pr.m_bAwardByProf), 1, fp);

	for (i = 0; i < pr.m_ulRankingAwardNum; i++)
	{
		const RANKING_AWARD& rAward = pr.m_RankingAward[i];
		fwrite(&rAward, sizeof(rAward), 1, fp);
	}
}

inline void SaveAwardData(const AWARD_DATA& ad, FILE* fp)
{
	unsigned long i;

	fprintf(fp, _format_award_force_activity, ad.m_iForceActivity);
	fprintf(fp, "\r\n");
	
	fprintf(fp, _format_award_force_contrib, ad.m_iForceContribution);
	fprintf(fp, "\r\n");
	
	fprintf(fp, _format_award_force_repu, ad.m_iForceReputation);
	fprintf(fp, "\r\n");

	fprintf(fp, _format_award_force_set_repu, ad.m_iForceSetRepu);
	fprintf(fp, "\r\n");

	fprintf(fp, _format_award_set_task_limit, ad.m_iTaskLimit);
	fprintf(fp, "\r\n");

	fprintf(fp, _format_award_title_num, ad.m_ulTitleNum);
	fprintf(fp, "\r\n");

	for (i = 0; i < ad.m_ulTitleNum; ++i)
	{
		fprintf(fp, _format_award_title_id, ad.m_pTitleAward[i].m_ulTitleID);
		fprintf(fp, "\r\n");
		fprintf(fp, _format_award_title_period, ad.m_pTitleAward[i].m_lPeriod);
		fprintf(fp, "\r\n");
	}

	fprintf(fp, _format_award_leadership, ad.m_iLeaderShip);
	fprintf(fp, "\r\n");

	fprintf(fp, _format_award_worldcontrib, ad.m_iWorldContribution);
	fprintf(fp, "\r\n");	

	fprintf(fp, _format_award_faction_contrib, ad.m_iFactionContrib);
	fprintf(fp, "\r\n");

	fprintf(fp, _format_award_faction_exp_contrib, ad.m_iFactionExpContrib);
	fprintf(fp, "\r\n");

	fprintf(fp, _format_award_gold_num, ad.m_ulGoldNum);
	fprintf(fp, "\r\n");

	fprintf(fp, _format_award_exp_num, ad.m_ulExp);
	fprintf(fp, "\r\n");

	fprintf(fp, _format_award_realm_exp, ad.m_ulRealmExp);
	fprintf(fp, "\r\n");

	fprintf(fp, _format_award_realmlevel_max, ad.m_bExpandRealmLevelMax);
	fprintf(fp, "\r\n");

	fprintf(fp, _format_award_cands_num, ad.m_ulCandItems);
	fprintf(fp, "\r\n");

	for (i = 0; i < ad.m_ulCandItems; i++)
	{
		SaveAwardCand(
			ad.m_CandItems[i],
			fp
		);
	}

	fprintf(fp, _format_award_monster_cand, ad.m_ulSummonedMonsters);
	fprintf(fp, "\r\n");

	if(ad.m_ulSummonedMonsters)
	{
		ad.m_SummonedMonsters->m_ulMonsterNum = ad.m_ulSummonedMonsters;
		SaveAwardMonsters(
			*ad.m_SummonedMonsters,
			fp
		);
	}

	fprintf(fp, _format_award_pq_ranking_cnt, ad.m_ulPQRankingAwardCnt);
	fprintf(fp, "\r\n");

	if(ad.m_ulPQRankingAwardCnt)
	{
		ad.m_PQRankingAward->m_ulRankingAwardNum = ad.m_ulPQRankingAwardCnt;
		SaveAwardPQRanking(
			*ad.m_PQRankingAward,
			fp
		);
	}

	fprintf(fp, _format_award_key_cnt, ad.m_ulChangeKeyCnt);
	fprintf(fp, "\r\n");
	
	for (i=0; i<ad.m_ulChangeKeyCnt; i++)
	{
		fprintf(fp, _format_award_change_key, ad.m_plChangeKey[i]);
		fprintf(fp, "\r\n");
		
		fprintf(fp, _format_award_change_kv, ad.m_plChangeKeyValue[i]);
		fprintf(fp, "\r\n");
		
		fprintf(fp, _format_award_change_typ, ad.m_pbChangeType[i]);
		fprintf(fp, "\r\n");
	}

	fprintf(fp, _format_award_multi, ad.m_bMulti);
	fprintf(fp, "\r\n");

	fprintf(fp, _format_award_num_type, ad.m_nNumType);
	fprintf(fp, "\r\n");

	fprintf(fp, _format_award_num, ad.m_lNum);
	fprintf(fp, "\r\n");

	fprintf(fp, _format_award_historykey_cnt, ad.m_ulHistoryChangeCnt);
	fprintf(fp, "\r\n");

	
	for (i=0; i<ad.m_ulHistoryChangeCnt; i++)
	{
		fprintf(fp, _format_award_historychange_key, ad.m_plHistoryChangeKey[i]);
		fprintf(fp, "\r\n");
		
		fprintf(fp, _format_award_historychange_kv, ad.m_plHistoryChangeKeyValue[i]);
		fprintf(fp, "\r\n");
		
		fprintf(fp, _format_award_historychange_typ, ad.m_pbHistoryChangeType[i]);
		fprintf(fp, "\r\n");
	}

	fprintf(fp, _format_award_dis_keycnt, ad.m_ulDisplayKeyCnt);
	fprintf(fp, "\r\n");
	
	for (i=0; i<ad.m_ulDisplayKeyCnt; i++)
	{
		fprintf(fp, _format_award_dis_key, ad.m_plDisplayKey[i]);
		fprintf(fp, "\r\n");
	}

	fprintf(fp, _format_award_dis_expcnt, ad.m_ulExpCnt);
	fprintf(fp, "\r\n");
	
	for (i=0; i<ad.m_ulExpCnt; i++)
	{		
		fwrite(ad.m_pszExp[i], 1, TASK_AWARD_MAX_DISPLAY_CHAR_LEN, fp);
		fprintf(fp, "\r\n");
	}

	fprintf(fp, _format_award_discharcnt, ad.m_ulTaskCharCnt);
	fprintf(fp, "\r\n");
	
	for (i=0; i<ad.m_ulTaskCharCnt; i++)
	{		
		fwrite(ad.m_pTaskChar[i], sizeof(task_char), TASK_AWARD_MAX_DISPLAY_CHAR_LEN, fp);
		fprintf(fp, "\r\n");
	}

	fprintf(fp, _format_award_sp_num, ad.m_ulSP);
	fprintf(fp, "\r\n");

	fprintf(fp, _format_award_repu, ad.m_lReputation);
	fprintf(fp, "\r\n");

	fprintf(fp, _format_award_period, ad.m_ulNewPeriod);
	fprintf(fp, "\r\n");

	fprintf(fp, _format_award_station, ad.m_ulNewRelayStation);
	fprintf(fp, "\r\n");

	fprintf(fp, _format_award_store, ad.m_ulStorehouseSize);
	fprintf(fp, "\r\n");

	fprintf(fp, _format_award_store, ad.m_ulStorehouseSize2);
	fprintf(fp, "\r\n");

	fprintf(fp, _format_award_store, ad.m_ulStorehouseSize3);
	fprintf(fp, "\r\n");

	fprintf(fp, _format_award_store, ad.m_ulStorehouseSize4);
	fprintf(fp, "\r\n");

	fprintf(fp, _format_award_pq_specify_id, ad.m_ulSpecifyContribTaskID);
	fprintf(fp, "\r\n");

	fprintf(fp, _format_award_pq_specify_contrib, ad.m_ulSpecifyContrib);
	fprintf(fp, "\r\n");

	fprintf(fp, _format_award_pq_contrib, ad.m_ulContrib);
	fprintf(fp, "\r\n");

	fprintf(fp, _format_award_pq_rand_contrib, ad.m_ulRandContrib);
	fprintf(fp, "\r\n");

	fprintf(fp, _format_award_pq_lowest_contrib, ad.m_ulLowestcontrib);
	fprintf(fp, "\r\n");

	fprintf(fp, _format_award_invent, ad.m_lInventorySize);
	fprintf(fp, "\r\n");

	fprintf(fp, _format_award_pet, ad.m_ulPetInventorySize);
	fprintf(fp, "\r\n");

	fprintf(fp, _format_award_fury, ad.m_ulFuryULimit);
	fprintf(fp, "\r\n");

	fprintf(fp, _format_world_id, ad.m_ulTransWldId);
	fprintf(fp, "\r\n");

	fprintf(fp, _format_zone_vert, VERT_ZONE(ad.m_TransPt));
	fprintf(fp, "\r\n");

	fprintf(fp, _format_controller, ad.m_lMonsCtrl);
	fprintf(fp, "\r\n");

	fprintf(fp, _format_trig_flag, ad.m_bTrigCtrl);
	fprintf(fp, "\r\n");

	fprintf(fp, _format_award_new_task, ad.m_ulNewTask);
	fprintf(fp, "\r\n");

	fprintf(fp, _format_use_lev_co, ad.m_bUseLevCo);
	fprintf(fp, "\r\n");

	fprintf(fp, _format_award_divorce, ad.m_bDivorce);
	fprintf(fp, "\r\n");

	fprintf(fp, _format_award_msg, ad.m_bSendMsg);
	fprintf(fp, "\r\n");

	fprintf(fp, _format_award_channel, ad.m_nMsgChannel);
	fprintf(fp, "\r\n");

	fprintf(fp, _format_award_death, ad.m_bAwardDeath);
	fprintf(fp, "\r\n");

	fprintf(fp, _format_award_death_loss, ad.m_bAwardDeathWithLoss);
	fprintf(fp, "\r\n");	

	fprintf(fp, _format_award_dividend, ad.m_ulDividend);
	fprintf(fp, "\r\n");

	fprintf(fp, _format_award_pq_specify_sub_taskid,ad.m_ulSpecifyContribSubTaskID); // version 76, add pq sub task ID contrib.
	fprintf(fp, "\r\n");

	fprintf(fp, _format_award_DeliverySkill,ad.m_bAwardSkill); // version 77.
	fprintf(fp, "\r\n");

	fprintf(fp, _format_award_DeliverySkillID,ad.m_iAwardSkillID); // version 77.
	fprintf(fp, "\r\n");

	fprintf(fp, _format_award_DeliverySkillLevel,ad.m_iAwardSkillLevel); // version 77.
	fprintf(fp, "\r\n");

}

inline void SaveAwardDataBin(const AWARD_DATA& ad, FILE* fp)
{
	unsigned long i;

	AWARD_DATA* p = (AWARD_DATA*)&ad;

	AWARD_ITEMS_CAND* tmp = p->m_CandItems;
	p->m_CandItems = NULL;

	AWARD_MONSTERS_SUMMONED* monsters = p->m_SummonedMonsters;
	p->m_SummonedMonsters = NULL;

	AWARD_PQ_RANKING* ranking = p->m_PQRankingAward;
	p->m_PQRankingAward = NULL;

	long *pKey = p->m_plChangeKey;
	long *pVal = p->m_plChangeKeyValue;
	bool *pTye = p->m_pbChangeType;
	long *pDis = p->m_plDisplayKey;
	p->m_plChangeKey = NULL;
	p->m_plChangeKeyValue = NULL;
	p->m_pbChangeType = NULL;
	p->m_plDisplayKey = NULL;

	long* pHistoryKey = p->m_plHistoryChangeKey;
	long* pHistoryVal = p->m_plHistoryChangeKeyValue;
	bool* pHistoryTye = p->m_pbHistoryChangeType;
	p->m_plHistoryChangeKey = NULL;
	p->m_plHistoryChangeKeyValue = NULL;
	p->m_pbHistoryChangeType = NULL;

	char (*pExp)[TASK_AWARD_MAX_DISPLAY_CHAR_LEN]			= p->m_pszExp;
	TASK_EXPRESSION (*pExpArr)[TASK_AWARD_MAX_DISPLAY_CHAR_LEN]		= p->m_pExpArr;
	task_char (*pTaskChar)[TASK_AWARD_MAX_DISPLAY_CHAR_LEN] = p->m_pTaskChar;
	p->m_pszExp     = NULL;
	p->m_pExpArr    = NULL;
	p->m_pTaskChar  = NULL;

	
	fwrite(&ad, sizeof(ad), 1, fp);
	
	p->m_CandItems = tmp;
	p->m_SummonedMonsters = monsters;
	p->m_PQRankingAward = ranking;

	p->m_plChangeKey = pKey;
	p->m_plChangeKeyValue = pVal;
	p->m_pbChangeType = pTye;
	p->m_plDisplayKey = pDis;

	p->m_plHistoryChangeKey = pHistoryKey;
	p->m_plHistoryChangeKeyValue = pHistoryVal;
	p->m_pbHistoryChangeType = pHistoryTye;

	p->m_pszExp    = pExp;
	p->m_pExpArr   = pExpArr;
	p->m_pTaskChar = pTaskChar;

	for (i = 0; i < ad.m_ulCandItems; i++)
		SaveAwardCandBin(
			ad.m_CandItems[i],
			fp
		);

	if(ad.m_ulSummonedMonsters)
	{
		ad.m_SummonedMonsters->m_ulMonsterNum = ad.m_ulSummonedMonsters;	
		SaveAwardMonstersBin(
			*ad.m_SummonedMonsters,
			fp
		);
	}

	if(ad.m_ulPQRankingAwardCnt)
	{
		ad.m_PQRankingAward->m_ulRankingAwardNum = ad.m_ulPQRankingAwardCnt;	
		SaveAwardPQRankingBin(
			*ad.m_PQRankingAward,
			fp
		);
	}
	
	for (i = 0; i < ad.m_ulTitleNum; ++i)
	{
		fwrite(&ad.m_pTitleAward[i], sizeof(TITLE_AWARD), 1, fp);
	}
	for (i=0; i<ad.m_ulChangeKeyCnt; i++)
	{
		fwrite(&ad.m_plChangeKey[i], sizeof(long), 1, fp);
		fwrite(&ad.m_plChangeKeyValue[i], sizeof(long), 1, fp);
		fwrite(&ad.m_pbChangeType[i], sizeof(bool), 1, fp);
	}
	for (i=0; i<ad.m_ulHistoryChangeCnt; i++)
	{
		fwrite(&ad.m_plHistoryChangeKey[i], sizeof(long), 1, fp);
		fwrite(&ad.m_plHistoryChangeKeyValue[i], sizeof(long), 1, fp);
		fwrite(&ad.m_pbHistoryChangeType[i], sizeof(bool), 1, fp);
	}

	for (i=0; i<ad.m_ulDisplayKeyCnt; i++)
	{
		fwrite(&ad.m_plDisplayKey[i], sizeof(long), 1, fp);
	}

	for (i=0; i<ad.m_ulExpCnt; i++)
	{
		fwrite(ad.m_pszExp[i], 1, TASK_AWARD_MAX_DISPLAY_CHAR_LEN, fp);

		//存vector				
		TaskExpAnalyser Analyser;
		TaskExpressionArr tmpArr;
		
		Analyser.Analyse(ad.m_pszExp[i], tmpArr);		
		size_t k(0);
		for (k = 0; k < tmpArr.size(); k++)
		{
			ad.m_pExpArr[i][k].type  = tmpArr[k].type;
			ad.m_pExpArr[i][k].value = tmpArr[k].value;
		}

		ad.m_pExpArr[i][k].type = -1;
		ad.m_pExpArr[i][k].value = -1;
		
		fwrite(ad.m_pExpArr[i], sizeof(TASK_EXPRESSION), TASK_AWARD_MAX_DISPLAY_CHAR_LEN, fp);
	}

	for (i=0; i<ad.m_ulTaskCharCnt; i++)
	{
		fwrite(ad.m_pTaskChar[i], sizeof(task_char), TASK_AWARD_MAX_DISPLAY_CHAR_LEN, fp);
	}
}

inline void SaveAwardDataByRatio(const AWARD_RATIO_SCALE& ad, FILE* fp)
{
	fwrite(&ad.m_ulScales, sizeof(ad.m_ulScales), 1, fp);
	fwrite(ad.m_Ratios, sizeof(ad.m_Ratios), 1, fp);

	for (unsigned long i = 0; i < ad.m_ulScales; i++)
		SaveAwardDataBin(ad.m_Awards[i], fp);
}

inline void SaveAwardDataByItem(const AWARD_ITEMS_SCALE& ad, FILE* fp)
{
	fwrite(&ad.m_ulScales, sizeof(ad.m_ulScales), 1, fp);
	fwrite(&ad.m_ulItemId, sizeof(ad.m_ulItemId), 1, fp);
	fwrite(ad.m_Counts, sizeof(ad.m_Counts), 1, fp);

	for (unsigned long i = 0; i < ad.m_ulScales; i++)
		SaveAwardDataBin(ad.m_Awards[i], fp);
}

void ATaskTemplFixedData::SaveFixedDataToTextFile(FILE* fp)
{
	unsigned long i;

	fprintf(fp, _format_id, m_ID);
	fprintf(fp, "\r\n");

	fwrite(m_szName, sizeof(task_char), MAX_TASK_NAME_LEN, fp);
	fprintf(fp, "\r\n");

	fprintf(fp, _format_dyn, m_DynTaskType);
	fprintf(fp, "\r\n");

	fprintf(fp, _format_type, m_ulType);
	fprintf(fp, "\r\n");

	fprintf(fp, _format_has_sign, m_bHasSign);
	fprintf(fp, "\r\n");

	if (m_bHasSign)
	{
		fwrite(m_pszSignature, sizeof(task_char), MAX_TASK_NAME_LEN, fp);
		fprintf(fp, "\r\n");
	}

	fprintf(fp, _format_time_limit, m_ulTimeLimit);
	fprintf(fp, "\r\n");

	fprintf(fp, _format_offline_fail, m_bOfflineFail);
	fprintf(fp, "\r\n");

	fprintf(fp, _format_abs_fail_time, m_bAbsFail);
	fprintf(fp, "\r\n");

	fprintf(fp, _format_full_tm_old, m_tmAbsFailTime.year, m_tmAbsFailTime.month, m_tmAbsFailTime.day, m_tmAbsFailTime.hour, m_tmAbsFailTime.min);
	fprintf(fp, "\r\n");

// 	fprintf(fp, _format_abs_fail_time, m_ulAbsFailTime);
//	fprintf(fp, "\r\n");

	fprintf(fp, _format_item_not_take_off, m_bItemNotTakeOff);
	fprintf(fp, "\r\n");


	fprintf(fp, _format_avail_frequency, m_lAvailFrequency);
	fprintf(fp, "\r\n");

	fprintf(fp, _format_abs_time, m_bAbsTime);
	fprintf(fp, "\r\n");

	fprintf(fp, _format_count, m_ulTimetable);
	fprintf(fp, "\r\n");

	for (i = 0; i < m_ulTimetable; i++)
	{
		fprintf(fp, _format_tm_type, m_tmType[i]);
		fprintf(fp, "\r\n");

		task_tm& s = m_tmStart[i];
		fprintf(fp, _format_full_tm, s.year, s.month, s.day, s.hour, s.min, s.wday);
		fprintf(fp, "\r\n");

		task_tm& e = m_tmEnd[i];
		fprintf(fp, _format_full_tm, e.year, e.month, e.day, e.hour, e.min, e.wday);
		fprintf(fp, "\r\n");
	}

	// 如果选择某个子任务，则不能顺序执行
	if (m_bChooseOne || m_bRandOne)
		m_bExeChildInOrder = false;

	fprintf(fp, _format_child_in_order, m_bExeChildInOrder);
	fprintf(fp, "\r\n");

	fprintf(fp, _format_choose_one, m_bChooseOne);
	fprintf(fp, "\r\n");

	fprintf(fp, _format_rand_one, m_bRandOne);
	fprintf(fp, "\r\n");

	fprintf(fp, _format_parent_fail, m_bParentAlsoFail);
	fprintf(fp, "\r\n");

	fprintf(fp, _format_parent_succ, m_bParentAlsoSucc);
	fprintf(fp, "\r\n");

	fprintf(fp, _format_can_give_up, m_bCanGiveUp);
	fprintf(fp, "\r\n");

	fprintf(fp, _format_die_as_fail, m_bFailAsPlayerDie);
	fprintf(fp, "\r\n");

	fprintf(fp, _format_can_redo, m_bCanRedo);
	fprintf(fp, "\r\n");

	fprintf(fp, _format_redo_after_fail, m_bCanRedoAfterFailure);
	fprintf(fp, "\r\n");

	fprintf(fp, _format_clear_as_giveup, m_bClearAsGiveUp);
	fprintf(fp, "\r\n");

	fprintf(fp, _format_need_record, m_bNeedRecord);
	fprintf(fp, "\r\n");

	fprintf(fp, _format_max_receiver, m_ulMaxReceiver);
	fprintf(fp, "\r\n");

	fprintf(fp, _format_must_in_zone, m_bDelvInZone);
	fprintf(fp, "\r\n");

	fprintf(fp, _format_world_id, m_ulDelvWorld);
	fprintf(fp, "\r\n");

	fprintf(fp,_format_taskRegionCnt,m_ulDelvRegionCnt); // version 80
	fprintf(fp, "\r\n");

	for (i=0;i<m_ulDelvRegionCnt;i++)
	{
		const Task_Region& t = m_pDelvRegion[i];

		fprintf(fp, _format_zone_vert, VERT_ZONE(t.zvMin));
		fprintf(fp, "\r\n");
		fprintf(fp, _format_zone_vert, VERT_ZONE(t.zvMax));
		fprintf(fp, "\r\n");
	}

/*	fprintf(fp, _format_zone_vert, VERT_ZONE(m_DelvMinVert));
	fprintf(fp, "\r\n");

	fprintf(fp, _format_zone_vert, VERT_ZONE(m_DelvMaxVert));
	fprintf(fp, "\r\n");
*/
	fprintf(fp, _format_enter_region_fail, m_bEnterRegionFail);
	fprintf(fp, "\r\n");

	fprintf(fp, _format_world_id, m_ulEnterRegionWorld);
	fprintf(fp, "\r\n");

	fprintf(fp,_format_taskRegionCnt,m_ulEnterRegionCnt); // version 80
	fprintf(fp, "\r\n");

	for (i=0;i<m_ulEnterRegionCnt;i++)
	{
		const Task_Region& t = m_pEnterRegion[i];
		
		fprintf(fp, _format_zone_vert, VERT_ZONE(t.zvMin));
		fprintf(fp, "\r\n");

		fprintf(fp, _format_zone_vert, VERT_ZONE(t.zvMax));
		fprintf(fp, "\r\n");
	}
/*
	fprintf(fp, _format_zone_vert, VERT_ZONE(m_EnterRegionMinVert));
	fprintf(fp, "\r\n");

	fprintf(fp, _format_zone_vert, VERT_ZONE(m_EnterRegionMaxVert));
	fprintf(fp, "\r\n");
	*/
	fprintf(fp, _format_leave_region_fail, m_bLeaveRegionFail);
	fprintf(fp, "\r\n");

	fprintf(fp, _format_world_id, m_ulLeaveRegionWorld);
	fprintf(fp, "\r\n");

	fprintf(fp,_format_taskRegionCnt,m_ulLeaveRegionCnt); // version 80
	fprintf(fp, "\r\n");

	for (i=0;i<m_ulLeaveRegionCnt;i++)
	{
		const Task_Region& t = m_pLeaveRegion[i];

		fprintf(fp, _format_zone_vert, VERT_ZONE(t.zvMin));
		fprintf(fp, "\r\n");
		
		fprintf(fp, _format_zone_vert, VERT_ZONE(t.zvMax));
		fprintf(fp, "\r\n");
	}
/*	fprintf(fp, _format_zone_vert, VERT_ZONE(m_LeaveRegionMinVert));
	fprintf(fp, "\r\n");

	fprintf(fp, _format_zone_vert, VERT_ZONE(m_LeaveRegionMaxVert));
	fprintf(fp, "\r\n");
*/
	fprintf(fp, _format_leave_force_fail, m_bLeaveForceFail);
	fprintf(fp, "\r\n");

	fprintf(fp, _format_leave_faction_fail, m_bLeaveFactionFail);
	fprintf(fp, "\r\n");

	fprintf(fp, _format_prem_is_king, m_bPremIsKing);
	fprintf(fp, "\r\n");

	fprintf(fp, _format_by_king, m_bShowByKing);
	fprintf(fp, "\r\n");

	fprintf(fp, _format_prem_not_in_team, m_bPremNotInTeam);
	fprintf(fp, "\r\n");
	
	fprintf(fp, _format_by_not_in_team, m_bShowByNotInTeam);
	fprintf(fp, "\r\n");

	fprintf(fp, _format_failed_not_take_item, m_bNotClearItemWhenFailed);
	fprintf(fp, "\r\n");

	fprintf(fp, _format_prem_title_total, m_iPremTitleNumTotal);
	fprintf(fp, "\r\n");

	fprintf(fp, _format_prem_title_required, m_iPremTitleNumRequired);
	fprintf(fp, "\r\n");

	fprintf(fp, _format_by_title, m_bShowByTitle);
	fprintf(fp, "\r\n");

	for (i = 0; i < m_iPremTitleNumTotal; i++)
	{
		fprintf(fp, _format_prem_title_id, m_PremTitles[i]);
		fprintf(fp, "\r\n");
	}

	fprintf(fp, _format_display_in_title_ui, m_bDisplayInTitleTaskUI);
	fprintf(fp, "\r\n");

	fprintf(fp, _format_failed_not_inc_count, m_bNotIncCntWhenFailed);
	fprintf(fp, "\r\n");

	fprintf(fp, _format_trans_to, m_bTransTo);
	fprintf(fp, "\r\n");

	fprintf(fp, _format_world_id, m_ulTransWldId);
	fprintf(fp, "\r\n");

	fprintf(fp, _format_zone_vert, VERT_ZONE(m_TransPt));
	fprintf(fp, "\r\n");

	fprintf(fp, _format_controller, m_lMonsCtrl);
	fprintf(fp, "\r\n");

	fprintf(fp, _format_trig_flag, m_bTrigCtrl);
	fprintf(fp, "\r\n");

	fprintf(fp, _format_auto_deliver, m_bAutoDeliver);
	fprintf(fp, "\r\n");

	fprintf(fp, _format_death_trig, m_bDeathTrig);
	fprintf(fp, "\r\n");

	fprintf(fp, _format_clear_acquired, m_bClearAcquired);
	fprintf(fp, "\r\n");

	fprintf(fp, _format_suit_lev, m_ulSuitableLevel);
	fprintf(fp, "\r\n");

	fprintf(fp, _format_show_prompt, m_bShowPrompt);
	fprintf(fp, "\r\n");

	fprintf(fp, _format_key_task, m_bKeyTask);
	fprintf(fp, "\r\n");

	fprintf(fp, _format_npc, m_ulDelvNPC);
	fprintf(fp, "\r\n");

	fprintf(fp, _format_npc, m_ulAwardNPC);
	fprintf(fp, "\r\n");

	fprintf(fp, _format_skill_task, m_bSkillTask);
	fprintf(fp, "\r\n");

	fprintf(fp, _format_seek_out, m_bCanSeekOut);
	fprintf(fp, "\r\n");

	fprintf(fp, _format_direction, m_bShowDirection);
	fprintf(fp, "\r\n");

	fprintf(fp, _format_marriage, m_bMarriage);
	fprintf(fp, "\r\n");

	fprintf(fp, _format_prem_key_cnt, m_ulChangeKeyCnt);
	fprintf(fp, "\r\n");
	
	for (i=0; i<m_ulChangeKeyCnt; i++)
	{
		fprintf(fp, _format_prem_change_key, m_plChangeKey[i]);
		fprintf(fp, "\r\n");
		
		fprintf(fp, _format_prem_change_kv, m_plChangeKeyValue[i]);
		fprintf(fp, "\r\n");
		
		fprintf(fp, _format_prem_change_typ, m_pbChangeType[i]);
		fprintf(fp, "\r\n");
	}

	fprintf(fp, _format_switch_fail, m_bSwitchSceneFail);
	fprintf(fp, "\r\n");

	fprintf(fp, _format_hidden, m_bHidden);
	fprintf(fp, "\r\n");

	fprintf(fp, _format_pq_task, m_bPQTask);
	fprintf(fp, "\r\n");

	fprintf(fp, _format_pq_task_exp_cnt, m_ulPQExpCnt);
	fprintf(fp, "\r\n");

	for (i=0;i<m_ulPQExpCnt; i++)
	{
		fwrite(m_pszPQExp[i], 1, TASK_AWARD_MAX_DISPLAY_CHAR_LEN, fp);
		fprintf(fp, "\r\n");
	}

	fprintf(fp, _format_pq_sub_task, m_bPQSubTask);
	fprintf(fp, "\r\n");

	fprintf(fp, _format_pq_clear_contrib, m_bClearContrib);
	fprintf(fp, "\r\n");

	fprintf(fp, _format_pq_contrib_cnt, m_ulMonsterContribCnt);
	fprintf(fp, "\r\n");

	for (i=0;i<m_ulMonsterContribCnt; i++)
	{
		fprintf(fp, _format_pq_monster_contrib, m_MonstersContrib[i].m_ulMonsterTemplId, m_MonstersContrib[i].m_iWholeContrib, m_MonstersContrib[i].m_iShareContrib);
		fprintf(fp, "\r\n");

		fprintf(fp,_format_pq_monster_contrib_personal,m_MonstersContrib[i].m_iPersonalWholeContrib);
		fprintf(fp,"\r\n");
	}

	fprintf(fp, _format_account_limit, m_bAccountTaskLimit);
	fprintf(fp, "\r\n");

	fprintf(fp, _format_account_limit_cnt, m_ulAccountTaskLimitCnt);
	fprintf(fp, "\r\n");

	fprintf(fp, _format_role_limit, m_bRoleTaskLimit);
	fprintf(fp, "\r\n");

	fprintf(fp, _format_deliverySkill, m_bDeliverySkill);  // version 77
	fprintf(fp, "\r\n");

	fprintf(fp, _format_deliveredSkillID, m_iDeliveredSkillID);
	fprintf(fp, "\r\n");

	fprintf(fp, _format_deliveredSkillLevel, m_iDeliveredSkillLevel);
	fprintf(fp, "\r\n");

	fprintf(fp,_format_showGfxWhenFinished,m_bShowGfxFinished); // version 78
	fprintf(fp,"\r\n");

	fprintf(fp,_format_changePQRanking,m_bChangePQRanking); // version 79
	fprintf(fp,"\r\n");

	fprintf(fp,_format_compareItemAndInventory,m_bCompareItemAndInventory); //version 81
	fprintf(fp,"\r\n");

	fprintf(fp,_format_needInventorySlotNum,m_ulInventorySlotNum);
	fprintf(fp,"\r\n");

	fprintf(fp,_format_is_library_task,m_bIsLibraryTask); //version 83
	fprintf(fp,"\r\n");

	fprintf(fp,_format_library_task_probability,m_fLibraryTasksProbability);
	fprintf(fp,"\r\n");

	fprintf(fp,_format_unique_storage_task,m_bIsUniqueStorageTask); 
	fprintf(fp,"\r\n");

	fprintf(fp,_format_worldcontrib, m_iWorldContribution); 
	fprintf(fp,"\r\n");
	
	fprintf(fp,_format_auto_deliver_prompt,m_bDisplayInExclusiveUI); //version 84
	fprintf(fp,"\r\n");

	fprintf(fp,_format_token_shop_task,m_bUsedInTokenShop);
	fprintf(fp,"\r\n");
	
	fprintf(fp,_format_period_limit,m_lPeriodLimit);//version 85
	fprintf(fp,"\r\n");

	/* 任务的开启条件及方式 */

	fprintf(fp, _format_prem_transformed_mask, m_ucPremiseTransformedForm);
	fprintf(fp, "\r\n");

	fprintf(fp,_format_show_by_transformed,m_bShowByTransformed);
	fprintf(fp, "\r\n");

	fprintf(fp,_format_prem_record_tasks_num,m_iPremNeedRecordTasksNum);
	fprintf(fp,"\r\n");
	
	fprintf(fp, _format_by_record_tasks_num, m_bShowByNeedRecordTasksNum);
	fprintf(fp, "\r\n");

	fprintf(fp,_format_prem_faction_contrib,m_iPremiseFactionContrib);
	fprintf(fp,"\r\n");

	fprintf(fp, _format_by_faction_contrib, m_bShowByFactionContrib);
	fprintf(fp, "\r\n");

	fprintf(fp, _format_special_award, m_ulSpecialAward);
	fprintf(fp, "\r\n");

	fprintf(fp, _format_prem_level, m_ulPremise_Lev_Min);
	fprintf(fp, "\r\n");

	fprintf(fp, _format_prem_level, m_ulPremise_Lev_Max);
	fprintf(fp, "\r\n");

	fprintf(fp, _format_prem_check_historymaxlevel, m_bPremCheckMaxHistoryLevel);
	fprintf(fp, "\r\n");

	fprintf(fp, _format_prem_check_reincarnation, m_bPremCheckReincarnation);
	fprintf(fp, "\r\n");

	fprintf(fp, _format_prem_reincarnation_min, m_ulPremReincarnationMin);
	fprintf(fp, "\r\n");
	
	fprintf(fp, _format_prem_reincarnation_max, m_ulPremReincarnationMax);
	fprintf(fp, "\r\n");

	fprintf(fp, _format_by_reincarnation, m_bShowByReincarnation);
	fprintf(fp, "\r\n");

	fprintf(fp, _format_prem_check_realmlevel, m_bPremCheckRealmLevel);
	fprintf(fp, "\r\n");
	
	fprintf(fp, _format_prem_realmlevel_min, m_ulPremRealmLevelMin);
	fprintf(fp, "\r\n");
	
	fprintf(fp, _format_prem_realmlevel_max, m_ulPremRealmLevelMax);
	fprintf(fp, "\r\n");

	fprintf(fp, _format_prem_realmexp_full, m_bPremCheckRealmExpFull);
	fprintf(fp, "\r\n");
	
	fprintf(fp, _format_by_realmlevel, m_bShowByRealmLevel);
	fprintf(fp, "\r\n");

	fprintf(fp, _format_prem_historyIndex, m_iPremHistoryStageIndex[0],m_iPremHistoryStageIndex[1]);
	fprintf(fp, "\r\n");

	fprintf(fp,_format_by_historyStage,m_bShowByHistoryStage);
	fprintf(fp,"\r\n");

	fprintf(fp, _format_item_wanted_num, m_ulPremItems);
	fprintf(fp, "\r\n");

	fprintf(fp, _format_prem_items_any_one,m_bPremItemsAnyOne);
	fprintf(fp,"\r\n");

	for (i = 0; i < m_ulPremItems; i++)
	{
		ITEM_WANTED& iw = m_PremItems[i];

		fprintf(fp, _format_item_templ, iw.m_ulItemTemplId);
		fprintf(fp, "\r\n");

		fprintf(fp, _format_item_is_cmn, iw.m_bCommonItem);
		fprintf(fp, "\r\n");

		fprintf(fp, _format_item_num, iw.m_ulItemNum);
		fprintf(fp, "\r\n");

		fprintf(fp, _format_period, iw.m_lPeriod);
		fprintf(fp, "\r\n");		
	}

	fprintf(fp, _format_given_items_num, m_ulGivenItems);
	fprintf(fp, "\r\n");

	for (i = 0; i < m_ulGivenItems; i++)
	{
		ITEM_WANTED& iw = m_GivenItems[i];

		fprintf(fp, _format_item_templ, iw.m_ulItemTemplId);
		fprintf(fp, "\r\n");

		fprintf(fp, _format_item_num, iw.m_ulItemNum);
		fprintf(fp, "\r\n");

		fprintf(fp, _format_item_is_cmn, iw.m_bCommonItem);
		fprintf(fp, "\r\n");
		
		fprintf(fp, _format_period, iw.m_lPeriod);
		fprintf(fp, "\r\n");
	}

	fprintf(fp, _format_prem_deposit, m_ulPremise_Deposit);
	fprintf(fp, "\r\n");

	fprintf(fp, _format_prem_reputation, m_lPremise_Reputation);
	fprintf(fp, "\r\n");

	fprintf(fp, _format_prem_reputation, m_lPremise_RepuMax);
	fprintf(fp, "\r\n");

	fprintf(fp, _format_prem_cotask, m_ulPremise_Cotask);
	fprintf(fp, "\r\n");

	fprintf(fp, _format_cotask_cond, m_ulCoTaskCond);
	fprintf(fp, "\r\n");

	fprintf(fp, _format_mutex_task_count, m_ulMutexTaskCount);
	fprintf(fp, "\r\n");

	for (i = 0; i < m_ulMutexTaskCount; i++)
	{
		fprintf(fp, _format_mutex_task, m_ulMutexTasks[i]);
		fprintf(fp, "\r\n");
	}

	fprintf(fp, _format_prem_task_count, m_ulPremise_Task_Count);
	fprintf(fp, "\r\n");

	for (i = 0; i < m_ulPremise_Task_Count; i++)
	{
		fprintf(fp, _format_prem_task, m_ulPremise_Tasks[i]);
		fprintf(fp, "\r\n");
	}

	fprintf(fp, _format_prem_task_leastnum, m_ulPremise_Task_Least_Num);
	fprintf(fp,"\r\n");

	fprintf(fp, _format_prem_period, m_ulPremise_Period);
	fprintf(fp, "\r\n");

	fprintf(fp, _format_prem_is_team, m_bTeamwork);
	fprintf(fp, "\r\n");

	if (m_bTeamwork)
	{
		fprintf(fp, _format_rcv_by_team, m_bRcvByTeam);
		fprintf(fp, "\r\n");

		fprintf(fp, _format_shared, m_bSharedTask);
		fprintf(fp, "\r\n");

		fprintf(fp, _format_share_acquire, m_bSharedAchieved);
		fprintf(fp, "\r\n");

		fprintf(fp, _format_check_tmmate, m_bCheckTeammate);
		fprintf(fp, "\r\n");

		fprintf(fp, _format_dist, m_fTeammateDist);
		fprintf(fp, "\r\n");

		fprintf(fp, _format_rcv_chck_mem, m_bRcvChckMem);
		fprintf(fp, "\r\n");

		fprintf(fp, _format_dist, m_fRcvMemDist);
		fprintf(fp, "\r\n");

		fprintf(fp, _format_cnt_by_mem_pos, m_bCntByMemPos);
		fprintf(fp, "\r\n");

		fprintf(fp, _format_dist, m_fCntMemDist);
		fprintf(fp, "\r\n");

		fprintf(fp, _format_all_fail, m_bAllFail);
		fprintf(fp, "\r\n");

		fprintf(fp, _format_cap_fail, m_bCapFail);
		fprintf(fp, "\r\n");

		fprintf(fp, _format_cap_succ, m_bCapSucc);
		fprintf(fp, "\r\n");

		fprintf(fp, _format_dist, m_fSuccDist);
		fprintf(fp, "\r\n");

		fprintf(fp, _format_all_succ, m_bAllSucc);
		fprintf(fp, "\r\n");

		fprintf(fp, _format_couple_only, m_bCoupleOnly);
		fprintf(fp, "\r\n");

		fprintf(fp, _format_distinguished_occ, m_bDistinguishedOcc);
		fprintf(fp, "\r\n");

		fprintf(fp, _format_dism_self_fail, m_bDismAsSelfFail);
		fprintf(fp, "\r\n");

		fprintf(fp, _format_mem_num_wanted, m_ulTeamMemsWanted);
		fprintf(fp, "\r\n");

		for (i = 0; i < m_ulTeamMemsWanted; i++)
		{
			const TEAM_MEM_WANTED& tmw = m_TeamMemsWanted[i];

			fprintf(fp, _format_level, tmw.m_ulLevelMin);
			fprintf(fp, "\r\n");

			fprintf(fp, _format_level, tmw.m_ulLevelMax);
			fprintf(fp, "\r\n");

			fprintf(fp, _format_occupation, tmw.m_ulOccupation);
			fprintf(fp, "\r\n");

			fprintf(fp, _format_race, tmw.m_ulRace);
			fprintf(fp, "\r\n");

			fprintf(fp, _format_gender, tmw.m_ulGender);
			fprintf(fp, "\r\n");

			fprintf(fp, _format_count, tmw.m_ulMinCount);
			fprintf(fp, "\r\n");

			fprintf(fp, _format_count, tmw.m_ulMaxCount);
			fprintf(fp, "\r\n");

			fprintf(fp, _format_task, tmw.m_ulTask);
			fprintf(fp, "\r\n");

			fprintf(fp, _format_team_force, tmw.m_iForce);
			fprintf(fp, "\r\n");
			
		}
	}

	fprintf(fp, _format_prem_is_spouse, m_bPremise_Spouse);
	fprintf(fp, "\r\n");

	fprintf(fp,_format_prem_is_wedding_owner,m_bPremiseWeddingOwner);
	fprintf(fp,"\r\n");

	fprintf(fp,_format_by_wedding_owner,m_bShowByWeddingOwner);
	fprintf(fp,"\r\n");

	fprintf(fp, _format_prem_is_gm, m_bGM);
	fprintf(fp, "\r\n");

	fprintf(fp, _format_prem_is_shielduser, m_bShieldUser);
	fprintf(fp, "\r\n");

	fprintf(fp, _format_prem_clan, m_ulPremise_Faction);
	fprintf(fp, "\r\n");

	fprintf(fp, _format_prem_clan_role, m_iPremise_FactionRole);
	fprintf(fp, "\r\n");

	fprintf(fp, _format_prem_gender, m_ulGender);
	fprintf(fp, "\r\n");

	fprintf(fp, _format_count, m_ulOccupations);
	fprintf(fp, "\r\n");

	for (i = 0; i < m_ulOccupations; i++)
	{
		fprintf(fp, _format_prem_occupation, m_Occupations[i]);
		fprintf(fp, "\r\n");
	}

	for (i = 0; i < MAX_LIVING_SKILLS; i++)
	{
		fprintf(fp, _format_proficiency, m_lSkillLev[i]);
		fprintf(fp, "\r\n");
	}

	fprintf(fp, _format_by_deposit, m_bShowByDeposit);
	fprintf(fp, "\r\n");

	fprintf(fp, _format_by_gender, m_bShowByGender);
	fprintf(fp, "\r\n");

	fprintf(fp, _format_by_items, m_bShowByItems);
	fprintf(fp, "\r\n");

	fprintf(fp, _format_by_lev, m_bShowByLev);
	fprintf(fp, "\r\n");

	fprintf(fp, _format_by_occup, m_bShowByOccup);
	fprintf(fp, "\r\n");

	fprintf(fp, _format_by_preTask, m_bShowByPreTask);
	fprintf(fp, "\r\n");

	fprintf(fp, _format_by_repu, m_bShowByRepu);
	fprintf(fp, "\r\n");

	fprintf(fp, _format_by_team, m_bShowByTeam);
	fprintf(fp, "\r\n");

	fprintf(fp, _format_by_faction, m_bShowByFaction);
	fprintf(fp, "\r\n");

	fprintf(fp, _format_by_period, m_bShowByPeriod);
	fprintf(fp, "\r\n");

	fprintf(fp, _format_by_spouse, m_bShowBySpouse);
	fprintf(fp, "\r\n");
	
	fprintf(fp, _format_need_comp, m_bPremNeedComp);
	fprintf(fp, "\r\n");

	fprintf(fp, _format_and_or, m_nPremExp1AndOrExp2);
	fprintf(fp, "\r\n");
	
	fprintf(fp, _format_left_type, m_Prem1KeyValue.nLeftType);
	fprintf(fp, "\r\n");
	
	fprintf(fp, _format_left_num, m_Prem1KeyValue.lLeftNum);
	fprintf(fp, "\r\n");	
	
	fprintf(fp, _format_comp_oper, m_Prem1KeyValue.nCompOper);
	fprintf(fp, "\r\n");
	
	fprintf(fp, _format_right_type, m_Prem1KeyValue.nRightType);
	fprintf(fp, "\r\n");
	
	fprintf(fp, _format_right_num, m_Prem1KeyValue.lRightNum);
	fprintf(fp, "\r\n");

	fprintf(fp, _format_left_type, m_Prem2KeyValue.nLeftType);
	fprintf(fp, "\r\n");
	
	fprintf(fp, _format_left_num, m_Prem2KeyValue.lLeftNum);
	fprintf(fp, "\r\n");	
	
	fprintf(fp, _format_comp_oper, m_Prem2KeyValue.nCompOper);
	fprintf(fp, "\r\n");
	
	fprintf(fp, _format_right_type, m_Prem2KeyValue.nRightType);
	fprintf(fp, "\r\n");
	
	fprintf(fp, _format_right_num, m_Prem2KeyValue.lRightNum);
	fprintf(fp, "\r\n");

	fprintf(fp, _format_prem_rmb_min, m_ulPremRMBMin);
	fprintf(fp, "\r\n");

	fprintf(fp, _format_prem_rmb_max, m_ulPremRMBMax);
	fprintf(fp, "\r\n");

	fprintf(fp, _format_char_time, m_bCharTime);
	fprintf(fp, "\r\n");

	fprintf(fp, _format_by_char_time, m_bShowByCharTime);
	fprintf(fp, "\r\n");

	fprintf(fp, _format_char_start_time, m_iCharStartTime);
	fprintf(fp, "\r\n");

	fprintf(fp, _format_char_end_time, m_iCharEndTime);
	fprintf(fp, "\r\n");

	fprintf(fp, _format_full_tm_old, m_tmCharEndTime.year, m_tmCharEndTime.month, m_tmCharEndTime.day, m_tmCharEndTime.hour, m_tmCharEndTime.min);
	fprintf(fp, "\r\n");

	fprintf(fp, _format_char_time_greater, m_ulCharTimeGreaterThan);
	fprintf(fp, "\r\n");

	fprintf(fp, _format_by_rmb, m_bShowByRMB);
	fprintf(fp, "\r\n");

	fprintf(fp, _format_by_force, m_bShowByForce);
	fprintf(fp, "\r\n");

	fprintf(fp, _format_by_force_contrib, m_bShowByForceContribution);
	fprintf(fp, "\r\n");

	fprintf(fp, _format_by_force_repu, m_bShowByForceReputation);
	fprintf(fp, "\r\n");

	fprintf(fp, _format_by_force_exp, m_bShowByForceExp);
	fprintf(fp, "\r\n");

	fprintf(fp, _format_by_force_sp, m_bShowByForceSP);
	fprintf(fp, "\r\n");

	fprintf(fp, _format_check_force, m_bPremCheckForce);
	fprintf(fp, "\r\n");

	fprintf(fp, _format_force, m_iPremForce);
	fprintf(fp, "\r\n");

	fprintf(fp, _format_prem_force_contrib, m_iPremForceContribution);
	fprintf(fp, "\r\n");

	fprintf(fp, _format_prem_force_repu, m_iPremForceReputation);
	fprintf(fp, "\r\n");

	fprintf(fp, _format_prem_force_exp, m_iPremForceExp);
	fprintf(fp, "\r\n");

	fprintf(fp, _format_prem_force_sp, m_iPremForceSP);
	fprintf(fp, "\r\n");

	fprintf(fp, _format_by_force_al, m_bShowByForceActivityLevel);
	fprintf(fp, "\r\n");

	fprintf(fp, _format_prem_force_al, m_iPremForceActivityLevel);
	fprintf(fp, "\r\n");

	fprintf(fp, _format_prem_cardcollection, m_ulPremGeneralCardCount);
	fprintf(fp, "\r\n");

	fprintf(fp, _format_by_generalcard_cnt, m_bShowByGeneralCard ? 1:0);
	fprintf(fp, "\r\n");

	fprintf(fp, _format_prem_cardrank, m_iPremGeneralCardRank);
	fprintf(fp, "\r\n");

	fprintf(fp, _format_prem_cardrankcount, m_ulPremGeneralCardRankCount);
	fprintf(fp, "\r\n");
	
	fprintf(fp, _format_by_generalcard_rankcnt, m_bShowByGeneralCardRank);
	fprintf(fp, "\r\n");
	/* 任务完成的方式及条件 */

	if (m_ulFirstChild)
		fprintf(fp, _format_method, enumTMNone);
	else
		fprintf(fp, _format_method, m_enumMethod);
	fprintf(fp, "\r\n");

	fprintf(fp, _format_finish_type, m_enumFinishType);
	fprintf(fp, "\r\n");

	if (m_enumMethod == enumTMKillNumMonster)
	{
		fprintf(fp, _format_monster_wanted, m_ulMonsterWanted);
		fprintf(fp, "\r\n");

		for (i = 0; i < m_ulMonsterWanted; i++)
		{
			MONSTER_WANTED& Monster = m_MonsterWanted[i];

			fprintf(fp, _format_monster_templ, Monster.m_ulMonsterTemplId);
			fprintf(fp, "\r\n");

			fprintf(fp, _format_monster_num, Monster.m_ulMonsterNum);
			fprintf(fp, "\r\n");

			fprintf(fp, _format_item_templ, Monster.m_ulDropItemId);
			fprintf(fp, "\r\n");

			fprintf(fp, _format_item_wanted_num, Monster.m_ulDropItemCount);
			fprintf(fp, "\r\n");

			fprintf(fp, _format_item_is_cmn, Monster.m_bDropCmnItem);
			fprintf(fp, "\r\n");

			fprintf(fp, _format_item_drop_prob, Monster.m_fDropProb);
			fprintf(fp, "\r\n");

			fprintf(fp, _format_killer_lev, Monster.m_bKillerLev);
			fprintf(fp, "\r\n");

			fprintf(fp, _format_dps, Monster.m_iDPS);
			fprintf(fp, "\r\n");

			fprintf(fp, _format_dph, Monster.m_iDPH);
			fprintf(fp, "\r\n");
		}
	}
	else if (m_enumMethod == enumTMKillPlayer)
	{
		fprintf(fp, _format_player_wanted, m_ulPlayerWanted);
		fprintf(fp, "\r\n");

		for (i = 0; i < m_ulPlayerWanted; ++i)
		{
			PLAYER_WANTED& Player = m_PlayerWanted[i];
			
			fprintf(fp, _format_player_num, Player.m_ulPlayerNum);
			fprintf(fp, "\r\n");
			
			fprintf(fp, _format_item_templ, Player.m_ulDropItemId);
			fprintf(fp, "\r\n");
			
			fprintf(fp, _format_item_wanted_num, Player.m_ulDropItemCount);
			fprintf(fp, "\r\n");
			
			fprintf(fp, _format_item_is_cmn, Player.m_bDropCmnItem);
			fprintf(fp, "\r\n");
			
			fprintf(fp, _format_item_drop_prob, Player.m_fDropProb);
			fprintf(fp, "\r\n");
			
			fprintf(fp, _format_player_require_occ, Player.m_Requirements.m_ulOccupations);
			fprintf(fp, "\r\n");

			fprintf(fp, _format_player_require_level_min, Player.m_Requirements.m_iMinLevel);
			fprintf(fp, "\r\n");

			fprintf(fp, _format_player_require_level_max, Player.m_Requirements.m_iMaxLevel);
			fprintf(fp, "\r\n");

			fprintf(fp, _format_player_require_gender, Player.m_Requirements.m_iGender);
			fprintf(fp, "\r\n");

			fprintf(fp, _format_player_require_force, Player.m_Requirements.m_iForce);
			fprintf(fp, "\r\n");
		}
	}
	else if (m_enumMethod == enumTMCollectNumArticle)
	{
		fprintf(fp, _format_gold, m_ulGoldWanted);
		fprintf(fp, "\r\n");

		fprintf(fp, _format_faction_contrib, m_iFactionContribWanted);
		fprintf(fp, "\r\n");

		fprintf(fp, _format_faction_exp_contrib, m_iFactionExpContribWanted);
		fprintf(fp, "\r\n");

		fprintf(fp, _format_item_wanted_num, m_ulItemsWanted);
		fprintf(fp, "\r\n");

		for (i = 0; i < m_ulItemsWanted; i++)
		{
			ITEM_WANTED& iw = m_ItemsWanted[i];

			fprintf(fp, _format_item_templ, iw.m_ulItemTemplId);
			fprintf(fp, "\r\n");

			fprintf(fp, _format_item_is_cmn, iw.m_bCommonItem);
			fprintf(fp, "\r\n");

			fprintf(fp, _format_item_num, iw.m_ulItemNum);
			fprintf(fp, "\r\n");
		}
	}
	else if (m_enumMethod == enumTMProtectNPC)
	{
		fprintf(fp, _format_npc_to_protect, m_ulNPCToProtect);
		fprintf(fp, "\r\n");

		fprintf(fp, _format_protect_tm_len, m_ulProtectTimeLen);
		fprintf(fp, "\r\n");
	}
	else if (m_enumMethod == enumTMWaitTime)
	{
		fprintf(fp, _format_waittime, m_ulWaitTime);
		fprintf(fp, "\r\n");
	}
	else if (m_enumMethod == enumTMReachTreasureZone)
	{
		fprintf(fp, _format_treasure_zones_s, VERT_ZONE(m_TreasureStartZone));
		fprintf(fp, "\r\n");

		int iWrite = m_ucZonesNumX;
		fprintf(fp,_format_treasure_zones_x,iWrite); 
		fprintf(fp, "\r\n");

		iWrite = m_ucZonesNumZ;
		fprintf(fp,_format_treasure_zones_z,iWrite); 
		fprintf(fp, "\r\n");

		iWrite = m_ucZoneSide;
		fprintf(fp,_format_treasure_zone_l,iWrite); 
		fprintf(fp, "\r\n");
	}
	else if (m_enumMethod == enumTMTransform)
	{
		fprintf(fp, _format_transformed_mask, m_ucTransformedForm);
		fprintf(fp, "\r\n");
	}
	else if (m_enumMethod == enumTMSimpleClientTask)
	{
		fprintf(fp, _format_emotion, m_uiEmotion);
		fprintf(fp, "\r\n");
	}
	else if (m_enumMethod == enumTMReachLevel)
	{
		fprintf(fp, _format_reach_level, m_ulReachLevel);
		fprintf(fp, "\r\n");
		
		fprintf(fp, _format_reach_reincarnation_count, m_ulReachReincarnationCount);
		fprintf(fp, "\r\n");
		
		fprintf(fp, _format_reach_realm_level, m_ulReachRealmLevel);
		fprintf(fp, "\r\n");
	}
	else if (m_enumMethod == enumTMReachSite)
	{
		fprintf(fp,_format_taskRegionCnt,m_ulReachSiteCnt); // version 80
		fprintf(fp, "\r\n");
		
		for (i=0;i<m_ulReachSiteCnt;i++)
		{
			const Task_Region& t = m_pReachSite[i];
			
			fprintf(fp, _format_zone_vert, VERT_ZONE(t.zvMin));
			fprintf(fp, "\r\n");
			
			fprintf(fp, _format_zone_vert, VERT_ZONE(t.zvMax));
			fprintf(fp, "\r\n");
		}
	/*	fprintf(fp, _format_zone_vert, VERT_ZONE(m_ReachSiteMin));
		fprintf(fp, "\r\n");

		fprintf(fp, _format_zone_vert, VERT_ZONE(m_ReachSiteMax));
		fprintf(fp, "\r\n");
*/
		fprintf(fp, _format_world_id, m_ulReachSiteId);
		fprintf(fp, "\r\n");
	}
	else if (m_enumMethod == enumTMLeaveSite)
	{
		fprintf(fp,_format_taskRegionCnt,m_ulLeaveSiteCnt); // version 80
		fprintf(fp, "\r\n");
		
		for (i=0;i<m_ulLeaveSiteCnt;i++)
		{
			const Task_Region& t = m_pLeaveSite[i];
			
			fprintf(fp, _format_zone_vert, VERT_ZONE(t.zvMin));
			fprintf(fp, "\r\n");
			
			fprintf(fp, _format_zone_vert, VERT_ZONE(t.zvMax));
			fprintf(fp, "\r\n");
		}

/*		fprintf(fp, _format_zone_vert, VERT_ZONE(m_LeaveSiteMin));
		fprintf(fp, "\r\n");

		fprintf(fp, _format_zone_vert, VERT_ZONE(m_LeaveSiteMax));
		fprintf(fp, "\r\n");
*/
		fprintf(fp, _format_world_id, m_ulLeaveSiteId);
		fprintf(fp, "\r\n");
	}

	fprintf(fp, _format_need_comp, m_bFinNeedComp);
	fprintf(fp, "\r\n");

	fprintf(fp, _format_and_or, m_nFinExp1AndOrExp2);
	fprintf(fp, "\r\n");
	
	fprintf(fp, _format_left_type, m_Fin1KeyValue.nLeftType);
	fprintf(fp, "\r\n");
	
	fprintf(fp, _format_left_num, m_Fin1KeyValue.lLeftNum);
	fprintf(fp, "\r\n");	
	
	fprintf(fp, _format_comp_oper, m_Fin1KeyValue.nCompOper);
	fprintf(fp, "\r\n");
	
	fprintf(fp, _format_right_type, m_Fin1KeyValue.nRightType);
	fprintf(fp, "\r\n");
	
	fprintf(fp, _format_right_num, m_Fin1KeyValue.lRightNum);
	fprintf(fp, "\r\n");

	fprintf(fp, _format_left_type, m_Fin2KeyValue.nLeftType);
	fprintf(fp, "\r\n");
	
	fprintf(fp, _format_left_num, m_Fin2KeyValue.lLeftNum);
	fprintf(fp, "\r\n");	
	
	fprintf(fp, _format_comp_oper, m_Fin2KeyValue.nCompOper);
	fprintf(fp, "\r\n");
	
	fprintf(fp, _format_right_type, m_Fin2KeyValue.nRightType);
	fprintf(fp, "\r\n");
	
	fprintf(fp, _format_right_num, m_Fin2KeyValue.lRightNum);
	fprintf(fp, "\r\n");

	fprintf(fp, _format_dis_expcnt, m_ulExpCnt);
	fprintf(fp, "\r\n");
	
	for (i=0; i<m_ulExpCnt; i++)
	{		
		fwrite(m_pszExp[i], 1, TASK_AWARD_MAX_DISPLAY_CHAR_LEN, fp);
		fprintf(fp, "\r\n");
	}
	
	fprintf(fp, _format_discharcnt, m_ulTaskCharCnt);
	fprintf(fp, "\r\n");
	
	for (i=0; i<m_ulTaskCharCnt; i++)
	{		
		fwrite(m_pTaskChar[i], sizeof(task_char), TASK_AWARD_MAX_DISPLAY_CHAR_LEN, fp);
		fprintf(fp, "\r\n");
	}

	/* 任务结束后的奖励 */

#define SAVE_AWARD_BY_RESULT(Result) \
{\
	unsigned long i;\
\
	fprintf(fp, _format_award_type, m_ulAwardType##Result);\
	fprintf(fp, "\r\n");\
\
	switch (m_ulAwardType##Result)\
	{\
	case enumTATNormal:\
	case enumTATEach:\
		SaveAwardData(*m_Award##Result, fp);\
		break;\
	case enumTATRatio:\
		fprintf(fp, _format_count, m_AwByRatio##Result->m_ulScales);\
		fprintf(fp, "\r\n");\
\
		for (i = 0; i < m_AwByRatio##Result->m_ulScales; i++)\
		{\
			fprintf(fp, _format_ratio, m_AwByRatio##Result->m_Ratios[i]);\
			fprintf(fp, "\r\n");\
\
			SaveAwardData(m_AwByRatio##Result->m_Awards[i], fp);\
		}\
		break;\
	case enumTATItemCount:\
		fprintf(fp, _format_count, m_AwByItems##Result->m_ulScales);\
		fprintf(fp, "\r\n");\
\
		fprintf(fp, _format_item_templ, m_AwByItems##Result->m_ulItemId);\
		fprintf(fp, "\r\n");\
\
		for (i = 0; i < m_AwByItems##Result->m_ulScales; i++)\
		{\
			fprintf(fp, _format_item_num, m_AwByItems##Result->m_Counts[i]);\
			fprintf(fp, "\r\n");\
\
			SaveAwardData(m_AwByItems##Result->m_Awards[i], fp);\
		}\
		break;\
	}\
}

	SAVE_AWARD_BY_RESULT(_S)

	SAVE_AWARD_BY_RESULT(_F)

	/* 层次关系 */

	fprintf(fp, _format_parent, m_ulParent);
	fprintf(fp, "\r\n");

	fprintf(fp, _format_next_sibling, m_ulNextSibling);
	fprintf(fp, "\r\n");

	fprintf(fp, _format_prev_sibling, m_ulPrevSibling);
	fprintf(fp, "\r\n");

	fprintf(fp, _format_first_child, m_ulFirstChild);
	fprintf(fp, "\r\n");
}

void ATaskTemplFixedData::SaveFixedDataToBinFile(FILE* fp)
{
	unsigned long i;

	// 如果选择某个子任务，则不能顺序执行
	if (m_bChooseOne || m_bRandOne)
		m_bExeChildInOrder = false;

	m_ulType = GetTaskTypeFromComboBox(GetComboBoxFromTaskType(m_ulType));

	fwrite(this, sizeof(*this), 1, fp);

	if (m_bHasSign)
	{
		convert_txt(m_pszSignature, MAX_TASK_NAME_LEN, (namechar)m_ID);
		fwrite(m_pszSignature, sizeof(task_char), MAX_TASK_NAME_LEN, fp);
		convert_txt(m_pszSignature, MAX_TASK_NAME_LEN, (namechar)m_ID);
	}

	for (i = 0; i < m_ulTimetable; i++)
	{
		fwrite(&m_tmStart[i], sizeof(m_tmStart[i]), 1, fp);
		fwrite(&m_tmEnd[i], sizeof(m_tmEnd[i]), 1, fp);
	}

	for (i=0; i<m_ulChangeKeyCnt; i++)
	{
		fwrite(&m_plChangeKey[i], sizeof(long), 1, fp);
		fwrite(&m_plChangeKeyValue[i], sizeof(long), 1, fp);
		fwrite(&m_pbChangeType[i], sizeof(bool), 1, fp);
	}	

	for (i=0; i<m_ulPQExpCnt; i++)
	{
		fwrite(m_pszPQExp[i], 1, TASK_AWARD_MAX_DISPLAY_CHAR_LEN, fp);

		//存vector				
		TaskExpAnalyser Analyser;
		TaskExpressionArr tmpArr;
		
		Analyser.Analyse(m_pszPQExp[i], tmpArr);		
		size_t k(0);
		for (k = 0; k < tmpArr.size(); k++)
		{
			m_pPQExpArr[i][k].type  = tmpArr[k].type;
			m_pPQExpArr[i][k].value = tmpArr[k].value;
		}

		m_pPQExpArr[i][k].type = -1;
		m_pPQExpArr[i][k].value = -1;
		
		fwrite(m_pPQExpArr[i], sizeof(TASK_EXPRESSION), TASK_AWARD_MAX_DISPLAY_CHAR_LEN, fp);
	}

	for (i=0; i<m_ulMonsterContribCnt; i++)
	{
		fwrite(&m_MonstersContrib[i], sizeof(m_MonstersContrib[i]), 1, fp);
	}

	for (i=0;i<m_ulDelvRegionCnt;i++)
	{
		fwrite(&m_pDelvRegion[i],sizeof(Task_Region),1,fp);
	}
	for (i=0;i<m_ulEnterRegionCnt;i++)
	{
		fwrite(&m_pEnterRegion[i],sizeof(Task_Region),1,fp);
	}
	for (i=0;i<m_ulLeaveRegionCnt;i++)
	{
		fwrite(&m_pLeaveRegion[i],sizeof(Task_Region),1,fp);
	}

	/* 任务的开启条件及方式 */
	for (i = 0; i < m_ulPremItems; i++)
		fwrite(&m_PremItems[i], sizeof(m_PremItems[i]), 1, fp);

	for (i = 0; i < m_ulGivenItems; i++)
	{
		ITEM_WANTED& iw = m_GivenItems[i];
		fwrite(&iw, sizeof(iw), 1, fp);
	}

	if (m_bTeamwork)
	{
		for (i = 0; i < m_ulTeamMemsWanted; i++)
		{
			const TEAM_MEM_WANTED& tmw = m_TeamMemsWanted[i];
			fwrite(&tmw, sizeof(tmw), 1, fp);
		}
	}
	for (i = 0; i < m_iPremTitleNumTotal; i++)
	{
		fwrite(&m_PremTitles[i], sizeof(int), 1, fp);	
	}

	/* 任务完成的方式及条件 */

	for (i = 0; i < m_ulMonsterWanted; i++)
	{
		MONSTER_WANTED& mw = m_MonsterWanted[i];
		fwrite(&mw, sizeof(mw), 1, fp);
	}

	for (i = 0; i < m_ulPlayerWanted; ++i)
	{
		PLAYER_WANTED& pw = m_PlayerWanted[i];
		fwrite(&pw, sizeof(pw), 1, fp);
	}

	for (i = 0; i < m_ulItemsWanted; i++)
	{
		ITEM_WANTED& iw = m_ItemsWanted[i];
		fwrite(&iw, sizeof(iw), 1, fp);
	}

	for (i=0; i<m_ulExpCnt; i++)
	{
		fwrite(m_pszExp[i], 1, TASK_AWARD_MAX_DISPLAY_CHAR_LEN, fp);

		//存vector				
		TaskExpAnalyser Analyser;
		TaskExpressionArr tmpArr;
		
		Analyser.Analyse(m_pszExp[i], tmpArr);		
		size_t k(0);
		for (k = 0; k < tmpArr.size(); k++)
		{
			m_pExpArr[i][k].type  = tmpArr[k].type;
			m_pExpArr[i][k].value = tmpArr[k].value;
		}

		m_pExpArr[i][k].type = -1;
		m_pExpArr[i][k].value = -1;
		
		fwrite(m_pExpArr[i], sizeof(TASK_EXPRESSION), TASK_AWARD_MAX_DISPLAY_CHAR_LEN, fp);
	}

	for (i=0; i<m_ulTaskCharCnt; i++)
	{
		fwrite(m_pTaskChar[i], sizeof(task_char), TASK_AWARD_MAX_DISPLAY_CHAR_LEN, fp);
	}

	// version 80
	for (i=0;i<m_ulReachSiteCnt;i++)
	{
		fwrite(&m_pReachSite[i],sizeof(Task_Region),1,fp);
	}
	for (i=0;i<m_ulLeaveSiteCnt;i++)
	{
		fwrite(&m_pLeaveSite[i],sizeof(Task_Region),1,fp);
	}

	/* 任务结束后的奖励 */

	SaveAwardDataBin(*m_Award_S, fp);
	SaveAwardDataBin(*m_Award_F, fp);

	SaveAwardDataByRatio(*m_AwByRatio_S, fp);
	SaveAwardDataByRatio(*m_AwByRatio_F, fp);

	SaveAwardDataByItem(*m_AwByItems_S, fp);
	SaveAwardDataByItem(*m_AwByItems_F, fp);
}

int ATaskTempl::MarshalKillMonster(char* pData)
{
	char* p = pData;

	char cnt = (char)m_ulMonsterWanted;
	*p = cnt;
	p++;

	if (cnt)
	{
		size_t sz = cnt * sizeof(MONSTER_WANTED);
		memcpy(p, m_MonsterWanted, sz);
		p += sz;
	}

	return p - pData;
}

int ATaskTempl::MarshalCollectItems(char* pData)
{
	char* p = pData;

	*p = (char)m_ulItemsWanted;
	p++;

	size_t sz = m_ulItemsWanted * sizeof(ITEM_WANTED);
	if (sz)
	{
		memcpy(p, m_ItemsWanted, sz);
		p += sz;
	}

	*(long*)p = m_ulGoldWanted;
	p += sizeof(long);

	*(long*)p = m_iFactionContribWanted;
	p += sizeof(long);

	*(long*)p = m_iFactionExpContribWanted;
	p += sizeof(long);

	return p - pData;
}

int ATaskTempl::MarshalDynTask(char* pData)
{
	char* p = pData;

	unsigned long* token_mask1 = (unsigned long*)p;
	*token_mask1 = 0;
	p += sizeof(long);

	unsigned long* token_mask2 = (unsigned long*)p;
	*token_mask2 = 0;
	p += sizeof(long);

	int token_count = 0;

	// dyn type
	*p = m_DynTaskType;
	p++;

	// top task
	if (!m_pParent)
	{
		int sz = 0;

		switch (m_DynTaskType)
		{
		case enumDTTSpecialAward:
		case enumDTTGiftCard:
			sz = MarshalSpecialAwardData(p);
			if (sz == 0) return 0;
			break;
		default:
			break;
		}

		p += sz;
	}

	// id
	*(long*)p = m_ID;
	p += sizeof(long);

	// name
	char len = (char)wcslen(m_szName);
	*p = len; p++;
	len *= sizeof(task_char);
	memcpy(p, m_szName, len);
	p += len;

	// choose one
	*p = m_bChooseOne;
	p++;

	// rand one
	*p = m_bRandOne;
	p++;

	// in order
	*p = m_bExeChildInOrder;
	p++;

	// parent fail
	*p = m_bParentAlsoFail;
	p++;

	// parent succ
	*p = m_bParentAlsoSucc;
	p++;

	// give up
	*p = m_bCanGiveUp;
	p++;

	// redo
	*p = m_bCanRedo;
	p++;

	// redo after fail
	*p = m_bCanRedoAfterFailure;
	p++;

	// clear as give up
	*p = m_bClearAsGiveUp;
	p++;

	// record
	*p = m_bNeedRecord;
	p++;

	// die
	*p = m_bFailAsPlayerDie;
	p++;

	// auto deliver
	*p = m_bAutoDeliver;
	p++;

	// death trig
	*p = m_bDeathTrig;
	p++;

	// clear acquired
	*p = m_bClearAcquired;
	p++;

	// spouse
	*p = m_bPremise_Spouse;
	p++;

	// teamwork
	*p = m_bTeamwork;
	p++;

	// direction
	*p = m_bShowDirection;
	p++;

	// lev
	*p = (char)m_ulPremise_Lev_Min; p++;
	*p = (char)m_ulPremise_Lev_Max; p++;

	// 0: time limit
	if (m_ulTimeLimit)
	{
		*(long*)p = m_ulTimeLimit;
		p += sizeof(long);

		*token_mask1 |= (1 << token_count);
	}

	token_count++;

	// 1: reputation
	if (m_lPremise_Reputation)
	{
		*(long*)p = m_lPremise_Reputation;
		p += sizeof(long);

		*token_mask1 |= (1 << token_count);
	}

	token_count++;

	// 2: period
	if (m_ulPremise_Period)
	{
		*(short*)p = (short)m_ulPremise_Period;
		p += sizeof(short);

		*token_mask1 |= (1 << token_count);
	}

	token_count++;

	// 3: prem items
	if (m_ulPremItems)
	{
		*p = (char)m_ulPremItems;
		p++;

		size_t sz = m_ulPremItems * sizeof(ITEM_WANTED);
		memcpy(p, m_PremItems, sz);
		p += sz;

		*token_mask1 |= (1 << token_count);
	}

	token_count++;

	// 4: delv in zone
	if (m_bDelvInZone)
	{
		*p = m_bDelvInZone;
		p++;

		*(long*)p = m_ulDelvWorld;
		p += sizeof(long);

		*(unsigned long*)p = m_ulDelvRegionCnt;
		p += sizeof(unsigned long);

		unsigned long i;
		for (i=0;i<m_ulDelvRegionCnt;i++)
		{
			*(Task_Region*)p = m_pDelvRegion[i];
			p += sizeof(Task_Region);
		}
/*		*(ZONE_VERT*)p = m_DelvMinVert;
		p += sizeof(ZONE_VERT);

		*(ZONE_VERT*)p = m_DelvMaxVert;
		p += sizeof(ZONE_VERT);
*/
		*token_mask1 |= (1 << token_count);
	}

	token_count++;

	// 5: trans to
	if (m_bTransTo)
	{
		*p = m_bTransTo;
		p++;

		*(long*)p = m_ulTransWldId;
		p += sizeof(long);

		*(ZONE_VERT*)p = m_TransPt;
		p += sizeof(ZONE_VERT);

		*token_mask1 |= (1 << token_count);
	}

	token_count++;

	// 6: given items
	if (m_ulGivenItems)
	{
		*p = (char)m_ulGivenItems;
		p++;

		*p = (char)m_ulGivenCmnCount;
		p++;

		*p = (char)m_ulGivenTskCount;
		p++;

		size_t sz = m_ulGivenItems * sizeof(ITEM_WANTED);
		memcpy(p, m_GivenItems, sz);
		p += sz;

		*token_mask1 |= (1 << token_count);
	}

	token_count++;

	// 7: deposit
	if (m_ulPremise_Deposit)
	{
		m_ulPremise_Deposit = *(long*)p;
		p += sizeof(long);

		*token_mask1 |= (1 << token_count);
	}

	token_count++;

	// 8: pre task
	if (m_ulPremise_Task_Count)
	{
		*p = (char)m_ulPremise_Task_Count;
		p++;

		*p = (char)m_ulPremise_Task_Least_Num;
		p++;

		size_t sz = sizeof(long) * m_ulPremise_Task_Count;
		memcpy(p, m_ulPremise_Tasks, sz);
		p += sz;

		*token_mask1 |= (1 << token_count);
	}

	token_count++;

	// 9: gender
	if (m_ulGender)
	{
		*p = (char)m_ulGender;
		p++;

		*token_mask1 |= (1 << token_count);
	}

	token_count++;

	// 10: occupation
	if (m_ulOccupations)
	{
		*p = (char)m_ulOccupations;
		p++;

		size_t sz = sizeof(long) * m_ulOccupations;
		memcpy(p, m_Occupations, sz);
		p += sz;

		*token_mask1 |= (1 << token_count);
	}

	token_count++;

	// 11: mutex task
	if (m_ulMutexTaskCount)
	{
		*p = (char)m_ulMutexTaskCount;
		p++;

		size_t sz = sizeof(long) * m_ulMutexTaskCount;
		memcpy(p, m_ulMutexTasks, sz);
		p += sz;

		*token_mask1 |= (1 << token_count);
	}

	token_count++;

	// 12: time table
	if (m_ulTimetable)
	{
		*p = (char)m_ulTimetable;
		p++;

		size_t sz;

		sz = sizeof(char) * m_ulTimetable;
		memcpy(p, m_tmType, sz);
		p += sz;

		sz = sizeof(task_tm) * m_ulTimetable;
		memcpy(p, m_tmStart, sz);
		p += sz;

		memcpy(p, m_tmEnd, sz);
		p += sz;

		*token_mask1 |= (1 << token_count);
	}

	token_count++;

	// method
	*p = (char)m_enumMethod;
	p++;

	switch (m_enumMethod)
	{
	case enumTMKillNumMonster:
		p += MarshalKillMonster(p);
		break;
	case enumTMCollectNumArticle:
		p += MarshalCollectItems(p);
		break;
	case enumTMReachSite:
		*(long*)p = m_ulReachSiteId;
		p += sizeof(long);
		*(unsigned long*)p = m_ulReachSiteCnt;
		p += sizeof(unsigned long);

		memcpy(p, m_pReachSite, sizeof(Task_Region)*m_ulReachSiteCnt);
		p += sizeof(Task_Region)*m_ulReachSiteCnt;
	//	memcpy(p, &m_ReachSiteMax, sizeof(ZONE_VERT));
	//	p += sizeof(ZONE_VERT);
		break;
	case enumTMLeaveSite:
		*(long*)p = m_ulLeaveSiteId;
		p += sizeof(long);
		memcpy(p, m_pLeaveSite, sizeof(Task_Region)*m_ulLeaveSiteCnt);
		p += sizeof(Task_Region)*m_ulLeaveSiteCnt;
	//	memcpy(p, &m_LeaveSiteMax, sizeof(ZONE_VERT));
	//	p += sizeof(ZONE_VERT);
		break;
	case enumTMWaitTime:
		*(long*)p = m_ulWaitTime;
		p += sizeof(long);
		break;
	}

	// finish type
	*p = (char)m_enumFinishType;
	p++;

	// award
	p += m_Award_S->MarshalBasicData(p);

	// talks
	p += marshal_str(p, m_pwstrDescript);
	p += marshal_str(p, m_pwstrOkText);
	p += marshal_str(p, m_pwstrNoText);
	p += marshal_talk_proc(&m_DelvTaskTalk, p);
	p += marshal_talk_proc(&m_UnqualifiedTalk, p);
	p += marshal_talk_proc(&m_DelvItemTalk, p);
	p += marshal_talk_proc(&m_ExeTalk, p);
	p += marshal_talk_proc(&m_AwardTalk, p);

	// sub tasks
	*(int*)p = GetSubCount();
	p += sizeof(int);

	ATaskTempl* pChild = m_pFirstChild;
	while (pChild)
	{
		p += pChild->MarshalDynTask(p);
		pChild = pChild->m_pNextSibling;
	}

	return p - pData;
}

int ATaskTempl::MarshalSpecialAwardData(char* pData)
{
	if (!m_ulSpecialAward)
		return 0;

	char* p = pData;
	*(long*)p = m_ulSpecialAward;
	p += sizeof(long);

	return p - pData;
}

void ATaskTempl::SaveToTextFile(FILE* fp)
{
	fprintf(fp, _format_version, _task_templ_cur_version);
	fprintf(fp, "\r\n");
	Save(fp, true);
}

bool ATaskTempl::SaveToTextFile(const char* szPath)
{
	FILE* fp = fopen(szPath, "w+b");
	if (fp == NULL) return false;
	SaveToTextFile(fp);
	fclose(fp);
	return true;
}

void ATaskTempl::Save(FILE* fp, bool bTextFile)
{
	SynchID();
	ATaskTemplFixedData::SaveFixedDataToTextFile(fp);
	SaveDescription(fp);
	SaveTribute(fp);
	m_DelvTaskTalk.save(fp);
	m_UnqualifiedTalk.save(fp);
	m_DelvItemTalk.save(fp);
	m_ExeTalk.save(fp);
	m_AwardTalk.save(fp);
	fprintf(fp, _format_task_count, GetSubCount());
	fprintf(fp, "\r\n");

	ATaskTempl* pChild = m_pFirstChild;
	while (pChild)
	{
		pChild->Save(fp, bTextFile);
		pChild = pChild->m_pNextSibling;
	}
}

void ATaskTempl::SaveBinary(FILE* fp)
{
	SynchID();

	convert_txt(m_szName, MAX_TASK_NAME_LEN, (namechar)m_ID);
	ATaskTemplFixedData::SaveFixedDataToBinFile(fp);
	convert_txt(m_szName, MAX_TASK_NAME_LEN, (namechar)m_ID);

	SaveDescriptionBin(fp);
	SaveTributeBin(fp);

	namechar code = (namechar)m_ID;

	convert_talk_text(&m_DelvTaskTalk, code);
	m_DelvTaskTalk.save(fp);
	convert_talk_text(&m_DelvTaskTalk, code);

	convert_talk_text(&m_UnqualifiedTalk, code);
	m_UnqualifiedTalk.save(fp);
	convert_talk_text(&m_UnqualifiedTalk, code);

	convert_talk_text(&m_DelvItemTalk, code);
	m_DelvItemTalk.save(fp);
	convert_talk_text(&m_DelvItemTalk, code);

	convert_talk_text(&m_ExeTalk, code);
	m_ExeTalk.save(fp);
	convert_talk_text(&m_ExeTalk, code);

	convert_talk_text(&m_AwardTalk, code);
	m_AwardTalk.save(fp);
	convert_talk_text(&m_AwardTalk, code);

	int nSubCount = GetSubCount();
	fwrite(&nSubCount, sizeof(nSubCount), 1, fp);

	ATaskTempl* pChild = m_pFirstChild;
	while (pChild)
	{
		pChild->SaveBinary(fp);
		pChild = pChild->m_pNextSibling;
	}
}

ATaskTempl& ATaskTempl::operator= (const ATaskTempl& src)
{
	int i;

	*m_Award_S = *src.m_Award_S;
	*m_Award_F = *src.m_Award_F;
	AWARD_DATA* _Award_S = m_Award_S;
	AWARD_DATA* _Award_F = m_Award_F;

	*m_AwByRatio_S = *src.m_AwByRatio_S;
	*m_AwByRatio_F = *src.m_AwByRatio_F;
	AWARD_RATIO_SCALE* _AwByRatio_S = m_AwByRatio_S;
	AWARD_RATIO_SCALE* _AwByRatio_F = m_AwByRatio_F;

	*m_AwByItems_S = *src.m_AwByItems_S;
	*m_AwByItems_F = *src.m_AwByItems_F;
	AWARD_ITEMS_SCALE* _AwByItems_S = m_AwByItems_S;
	AWARD_ITEMS_SCALE* _AwByItems_F = m_AwByItems_F;

	for (i = 0; i < MAX_TIMETABLE_SIZE; i++)
	{
		m_tmStart[i] = src.m_tmStart[i];
		m_tmEnd[i] = src.m_tmEnd[i];
	}

	task_tm* _tmStart = m_tmStart;
	task_tm* _tmEnd = m_tmEnd;

	for (i = 0; i < MAX_ITEM_WANTED; i++)
	{
		m_PremItems[i] = src.m_PremItems[i];
		m_GivenItems[i] = src.m_GivenItems[i];
		m_ItemsWanted[i] = src.m_ItemsWanted[i];
	}

	ITEM_WANTED* _PremItems = m_PremItems;
	ITEM_WANTED* _GivenItems = m_GivenItems;
	ITEM_WANTED* _ItemsWanted = m_ItemsWanted;

	for (i = 0; i < MAX_TEAM_MEM_WANTED; i++)
		m_TeamMemsWanted[i] = src.m_TeamMemsWanted[i];

	TEAM_MEM_WANTED* _TeamMemsWanted = m_TeamMemsWanted;

	for (i = 0; i < MAX_MONSTER_WANTED; i++)
		m_MonsterWanted[i] = src.m_MonsterWanted[i];

	MONSTER_WANTED* _MonsterWanted = m_MonsterWanted;

	for (i=0;i<MAX_CONTRIB_MONSTERS; i++)
		m_MonstersContrib[i] = src.m_MonstersContrib[i];
	
	MONSTERS_CONTRIB* _MonsterContrib = m_MonstersContrib;

	*(ATaskTemplFixedData*)this = *(const ATaskTemplFixedData*)&src;

	m_Award_S = _Award_S;
	m_Award_F = _Award_F;

	m_AwByRatio_S = _AwByRatio_S;
	m_AwByRatio_F = _AwByRatio_F;

	m_AwByItems_S = _AwByItems_S;
	m_AwByItems_F = _AwByItems_F;

	m_tmStart = _tmStart;
	m_tmEnd = _tmEnd;

	m_PremItems = _PremItems;
	m_GivenItems = _GivenItems;
	m_ItemsWanted = _ItemsWanted;

	m_TeamMemsWanted = _TeamMemsWanted;
	m_MonsterWanted = _MonsterWanted;
	m_MonstersContrib = _MonsterContrib;

	if (src.m_pwstrDescript)
	{
		m_pwstrDescript = new task_char[wcslen(src.m_pwstrDescript)+1];
		wcscpy(m_pwstrDescript, src.m_pwstrDescript);
		g_ulNewCount++;
	}

	if (src.m_pwstrOkText)
	{
		m_pwstrOkText = new task_char[wcslen(src.m_pwstrOkText)+1];
		wcscpy(m_pwstrOkText, src.m_pwstrOkText);
		g_ulNewCount++;
	}

	if (src.m_pwstrNoText)
	{
		m_pwstrNoText = new task_char[wcslen(src.m_pwstrNoText)+1];
		wcscpy(m_pwstrNoText, src.m_pwstrNoText);
		g_ulNewCount++;
	}

	if (src.m_pwstrTribute)
	{
		m_pwstrTribute = new task_char[wcslen(src.m_pwstrTribute)+1];
		wcscpy(m_pwstrTribute, src.m_pwstrTribute);
		g_ulNewCount++;
	}
	
	if (src.m_pszPQExp)
	{
		m_pszPQExp = new char[TASK_AWARD_MAX_DISPLAY_EXP_CNT][TASK_AWARD_MAX_DISPLAY_CHAR_LEN];
		memcpy(m_pszPQExp, src.m_pszPQExp, TASK_AWARD_MAX_DISPLAY_EXP_CNT*TASK_AWARD_MAX_DISPLAY_CHAR_LEN);
		g_ulNewCount++;
	}

	if (src.m_pPQExpArr)
	{
		m_pPQExpArr = new TASK_EXPRESSION[TASK_AWARD_MAX_DISPLAY_EXP_CNT][TASK_AWARD_MAX_DISPLAY_CHAR_LEN];
		memcpy(m_pPQExpArr, src.m_pPQExpArr, sizeof(TASK_EXPRESSION)*TASK_AWARD_MAX_DISPLAY_EXP_CNT*TASK_AWARD_MAX_DISPLAY_CHAR_LEN);
		g_ulNewCount++;
	}

	if (src.m_pszExp)
	{
		m_pszExp = new char[TASK_AWARD_MAX_DISPLAY_EXP_CNT][TASK_AWARD_MAX_DISPLAY_CHAR_LEN];
		memcpy(m_pszExp, src.m_pszExp, TASK_AWARD_MAX_DISPLAY_EXP_CNT*TASK_AWARD_MAX_DISPLAY_CHAR_LEN);
		g_ulNewCount++;
	}

	if (src.m_pExpArr)
	{
		m_pExpArr = new TASK_EXPRESSION[TASK_AWARD_MAX_DISPLAY_EXP_CNT][TASK_AWARD_MAX_DISPLAY_CHAR_LEN];
		memcpy(m_pExpArr, src.m_pExpArr, sizeof(TASK_EXPRESSION)*TASK_AWARD_MAX_DISPLAY_EXP_CNT*TASK_AWARD_MAX_DISPLAY_CHAR_LEN);
		g_ulNewCount++;
	}

	return *this;
}

int GetTaskTypeFromComboBox(int nComboBoxData)
{
	return nComboBoxData + 100;
}

int GetComboBoxFromTaskType(int nTaskType)
{
	int ret(0);
	switch(nTaskType) {
	case enumTTKeyOld:  ret = 1; break;
	case enumTTSevenKillListOld: ret = 5; break;
	case enumTTFactionOld: ret = 6; break;
		
	case enumTTCommonOld:
	case enumTTRepeatedOld:
	case enumTTBossOld:
	case enumTTInstanceOld:
	case enumTTScrollOld:
	case enumTTBusinessOld:
	case enumTTFeelingOld:
	case enumTTTacticsOld:
	case enumTTRegionOld: 
	case enumTTHintOld: ret = 3; break;
		
	case enumTTDaily:
	case enumTTLevel2:
	case enumTTMajor:
	case enumTTBranch:
	case enumTTEvent:
	case enumTTQiShaList:
	case enumTTFaction: 
	case enumTTFunction:
	case enumTTLegend:
	case enumTTQuestion:
		ret = nTaskType - 100; break;
	default:
		assert(false && "unknown task type!");
		break;
	}
	return ret;
}

#endif
