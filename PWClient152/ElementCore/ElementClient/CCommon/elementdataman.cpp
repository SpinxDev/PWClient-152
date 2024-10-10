#include "elementdataman.h"
using namespace abase;

#include <stdio.h>
#include <math.h>
#include <time.h>

const char * DataTypeName[DT_MAX+1] =
{
	"DT_INVALID",				// 0
	"DT_EQUIPMENT_ADDON",
	"DT_WEAPON_MAJOR_TYPE",
	"DT_WEAPON_SUB_TYPE",
	"DT_WEAPON_ESSENCE",

	"DT_ARMOR_MAJOR_TYPE",	
	"DT_ARMOR_SUB_TYPE",
	"DT_ARMOR_ESSENCE",
	"DT_DECORATION_MAJOR_TYPE",
	"DT_DECORATION_SUB_TYPE",

	"DT_DECORATION_ESSENCE",	// 10
	"DT_MEDICINE_MAJOR_TYPE",
	"DT_MEDICINE_SUB_TYPE",
	"DT_MEDICINE_ESSENCE",
	"DT_MATERIAL_MAJOR_TYPE",

	"DT_MATERIAL_SUB_TYPE",
	"DT_MATERIAL_ESSENCE",
	"DT_DAMAGERUNE_SUB_TYPE",
	"DT_DAMAGERUNE_ESSENCE",
	"DT_ARMORRUNE_SUB_TYPE",
	
	"DT_ARMORRUNE_ESSENCE",		// 20
	"DT_SKILLTOME_SUB_TYPE",
	"DT_SKILLTOME_ESSENCE",		
	"DT_FLYSWORD_ESSENCE",
	"DT_WINGMANWING_ESSENCE",

	"DT_TOWNSCROLL_ESSENCE",
	"DT_UNIONSCROLL_ESSENCE",
	"DT_REVIVESCROLL_ESSENCE",
	"DT_ELEMENT_ESSENCE",		
	"DT_TASKMATTER_ESSENCE",

	"DT_TOSSMATTER_ESSENCE",	// 30
	"DT_PROJECTILE_TYPE",		
	"DT_PROJECTILE_ESSENCE",
	"DT_QUIVER_SUB_TYPE",
	"DT_QUIVER_ESSENCE",

	"DT_STONE_SUB_TYPE",
	"DT_STONE_ESSENCE",
	"DT_MONSTER_ADDON",
	"DT_MONSTER_TYPE",
	"DT_MONSTER_ESSENCE",	

	"DT_NPC_TALK_SERVICE",	// 40
	"DT_NPC_SELL_SERVICE",
	"DT_NPC_BUY_SERVICE",
	"DT_NPC_REPAIR_SERVICE",
	"DT_NPC_INSTALL_SERVICE",

	"DT_NPC_UNINSTALL_SERVICE",
	"DT_NPC_TASK_IN_SERVICE",
	"DT_NPC_TASK_OUT_SERVICE",
	"DT_NPC_TASK_MATTER_SERVICE",
	"DT_NPC_SKILL_SERVICE",

	"DT_NPC_HEAL_SERVICE",		// 50
	"DT_NPC_TRANSMIT_SERVICE",	
	"DT_NPC_TRANSPORT_SERVICE",
	"DT_NPC_PROXY_SERVICE",
	"DT_NPC_STORAGE_SERVICE",
	
	"DT_NPC_MAKE_SERVICE",
	"DT_NPC_DECOMPOSE_SERVICE",	
	"DT_NPC_TYPE",
	"DT_NPC_ESSENCE",
	"DT_TALK_PROC",

	"DT_FACE_TEXTURE_ESSENCE",	// 60
	"DT_FACE_SHAPE_ESSENCE",	
	"DT_FACE_EMOTION_TYPE",
	"DT_FACE_EXPRESSION_ESSENCE",
	"DT_FACE_HAIR_ESSENCE",

	"DT_FACE_MOUSTACHE_ESSENCE",
	"DT_COLORPICKER_ESSENCE",
	"DT_CUSTOMIZEDATA_ESSENCE",
	"DT_RECIPE_MAJOR_TYPE",
	"DT_RECIPE_SUB_TYPE",

	"DT_RECIPE_ESSENCE",		// 70
	"DT_ENEMY_FACTION_CONFIG",	
	"DT_CHARRACTER_CLASS_CONFIG",
	"DT_PARAM_ADJUST_CONFIG",
	"DT_PLAYER_ACTION_INFO_CONFIG",

	"DT_TASKDICE_ESSENCE",		// 75
	"DT_TASKNORMALMATTER_ESSENCE",
	"DT_FACE_FALING_ESSENCE",
	"DT_PLAYER_LEVELEXP_CONFIG",
	"DT_MINE_TYPE",

	"DT_MINE_ESSENCE",			// 80
	"DT_NPC_IDENTIFY_SERVICE",
	"DT_FASHION_MAJOR_TYPE",	
	"DT_FASHION_SUB_TYPE",
	"DT_FASHION_ESSENCE",

	"DT_FACETICKET_MAJOR_TYPE",	// 85
	"DT_FACETICKET_SUB_TYPE",
	"DT_FACETICKET_ESSENCE",
	"DT_FACEPILL_MAJOR_TYPE",
	"DT_FACEPILL_SUB_TYPE",

	"DT_FACEPILL_ESSENCE",		// 90
	"DT_SUITE_ESSENCE",
	"DT_GM_GENERATOR_TYPE",
	"DT_GM_GENERATOR_ESSENCE",
	"DT_PET_TYPE",

	"DT_PET_ESSENCE",			// 95
	"DT_PET_EGG_ESSENCE",
	"DT_PET_FOOD_ESSENCE",
	"DT_PET_FACETICKET_ESSENCE",
	"DT_FIREWORKS_ESSENCE",

	"DT_WAR_TANKCALLIN_ESSENCE",	// 100
	"DT_NPC_WAR_TOWERBUILD_SERVICE",
	"DT_PLAYER_SECONDLEVEL_CONFIG",
	"DT_NPC_RESETPROP_SERVICE",
	"DT_NPC_PETNAME_SERVICE",

	"DT_NPC_PETLEARNSKILL_SERVICE",	//	105
	"DT_NPC_PETFORGETSKILL_SERVICE",
	"DT_SKILLMATTER_ESSENCE",
	"DT_REFINE_TICKET_ESSENCE",
	"DT_DESTROYING_ESSENCE",

	"DT_NPC_EQUIPBIND_SERVICE",		//	110
	"DT_NPC_EQUIPDESTROY_SERVICE",
	"DT_NPC_EQUIPUNDESTROY_SERVICE",
	"DT_BIBLE_ESSENCE",
	"DT_SPEAKER_ESSENCE",

	"DT_AUTOHP_ESSENCE",		//	115
	"DT_AUTOMP_ESSENCE",
	"DT_DOUBLE_EXP_ESSENCE",
	"DT_TRANSMITSCROLL_ESSENCE",
	"DT_DYE_TICKET_ESSENCE", 

	"DT_GOBLIN_ESSENCE",		//	120
	"DT_GOBLIN_EQUIP_TYPE",
	"DT_GOBLIN_EQUIP_ESSENCE",
	"DT_GOBLIN_EXPPILL_ESSENCE",
	"DT_SELL_CERTIFICATE_ESSENCE",

	"DT_TARGET_ITEM_ESSENCE",	//	125
	"DT_LOOK_INFO_ESSENCE",
	"DT_UPGRADE_PRODUCTION_CONFIG",
	"DT_ACC_STORAGE_BLACKLIST_CONFIG",
	"DT_FACE_HAIR_TEXTURE_MAP",
	
	"DT_MULTI_EXP_CONFIG",         // 130
	"DT_INC_SKILL_ABILITY_ESSENCE",
	"DT_GOD_EVIL_CONVERT_CONFIG",
	"DT_WEDDING_CONFIG",
	"DT_WEDDING_BOOKCARD_ESSENCE",

	"DT_WEDDING_INVITECARD_ESSENCE",	//	135
	"DT_SHARPENER_ESSENCE",
	"DT_FACE_THIRDEYE_ESSENCE",
	"DT_FACTION_FORTRESS_CONFIG",
	"DT_FACTION_BUILDING_SUB_TYPE",

	"DT_FACTION_BUILDING_ESSENCE",	// 140
	"DT_FACTION_MATERIAL_ESSENCE",
	"DT_CONGREGATE_ESSENCE",
	"DT_ENGRAVE_MAJOR_TYPE",
	"DT_ENGRAVE_SUB_TYPE",

	"DT_ENGRAVE_ESSENCE",		// 145
	"DT_NPC_ENGRAVE_SERVICE",
	"DT_NPC_RANDPROP_SERVICE",
	"DT_RANDPROP_TYPE",
	"DT_RANDPROP_ESSENCE",
	
	"DT_WIKI_TABOO_CONFIG",		//	150
	"DT_FORCE_CONFIG",
	"DT_FORCE_TOKEN_ESSENCE",
	"DT_NPC_FORCE_SERVICE",
	"DT_PLAYER_DEATH_DROP_CONFIG",

	"DT_DYNSKILLEQUIP_ESSENCE",	//	155
	"DT_CONSUME_POINTS_CONFIG",
	"DT_ONLINE_AWARDS_CONFIG",	
	"DT_COUNTRY_CONFIG",
	"DT_GM_ACTIVITY_CONFIG",

	"DT_FASHION_WEAPON_CONFIG",
	"DT_PET_EVOLVE_CONFIG",
	"DT_PET_EVOLVED_SKILL_CONFIG",
	"DT_MONEY_CONVERTIBLE_ESSENCE",
	"DT_STONE_CHANGE_RECIPE_TYPE",

	"DT_STONE_CHANGE_RECIPE",
	"DT_MERIDIAN_CONFIG",
	"DT_PET_EVOLVED_SKILL_RAND_CONFIG",
	"DT_AUTOTASK_DISPLAY_CONFIG",
	"DT_TOUCH_SHOP_CONFIG",
	
	"DT_TITLE_CONFIG",
	"DT_COMPLEX_TITLE_CONFIG",
	"DT_MONSTER_SPIRIT_ESSENCE",
	"DT_PLAYER_SPIRIT_CONFIG",
	"DT_PLAYER_REINCARNATION_CONFIG",

	"DT_HISTORY_STAGE_CONFIG",
	"DT_HISTORY_ADVANCE_CONFIG",
	"DT_AUTOTEAM_CONFIG",
	"DT_PLAYER_REALM_CONFIG",
	"DT_CHARIOT_CONFIG",

	"DT_CHARIOT_WAR_CONFIG",
	"DT_POKER_LEVELEXP_CONFIG",
	"DT_POKER_SUITE_ESSENCE",
	"DT_POKER_DICE_ESSENCE",
	"DT_POKER_SUB_TYPE",
	
	"DT_POKER_ESSENCE",
	"DT_TOKEN_SHOP_CONFIG",
	"DT_SHOP_TOKEN_ESSENCE",
	"DT_GT_CONFIG",
	"DT_RAND_SHOP_CONFIG",

	"DT_PROFIT_TIME_CONFIG",
	"DT_FACTION_PVP_CONFIG",
	"DT_UNIVERSAL_TOKEN_ESSENCE",
	"DT_TASK_LIST_CONFIG",
	"DT_TASK_DICE_BY_WEIGHT_CONFIG",

	"DT_FASHION_SUITE_ESSENCE",
	"DT_FASHION_BEST_COLOR_CONFIG",
	"DT_SIGN_AWARD_CONFIG",

	"DT_MAX",
};

#define		ARRAY_SIZE(array)		(sizeof(array)/sizeof(array[0]))

void eliminate_zero_item(unsigned char * data, size_t stride, size_t count)
{
	unsigned char * d = data;
	unsigned char * p = data;

	for(unsigned int i=0; i<count; i++)
	{
		int flag = *(int *)d;
		if( flag == 0 )
		{
			d += stride;
			continue;
		}

		if( p != d )
			memcpy(p, d, stride);

		d += stride;
		p += stride;
	}

	if( data + stride * count - p )
		memset(p, 0, data + stride * count - p);
}

void elementdataman::NormalizeRandom(float* r, int n)
{
	double t=0;
	float * slider = r;
	for(int i=0; i<n; i++)
	{
		t += *slider;
		slider++;
	}
	if(t>0.00001)
	{
		slider = r;
		for(int i=0; i<n; i++)
		{
			*slider /= t;
			slider++;
		}
	}
	else
	{
		memset(r, 0, sizeof(float) * n);
		*r = 1.f;
	}
}

elementdataman::elementdataman()
{
	unsigned int i;
	for(i=0; i<DT_MAX; i++)
	{
		start_ptr_array.push_back(NULL);
	}
	type_size_array.push_back(0);
	type_size_array.push_back(sizeof(EQUIPMENT_ADDON));
	type_size_array.push_back(sizeof(WEAPON_MAJOR_TYPE));
	type_size_array.push_back(sizeof(WEAPON_SUB_TYPE));
	type_size_array.push_back(sizeof(WEAPON_ESSENCE));

	type_size_array.push_back(sizeof(ARMOR_MAJOR_TYPE));	
	type_size_array.push_back(sizeof(ARMOR_SUB_TYPE));
	type_size_array.push_back(sizeof(ARMOR_ESSENCE));
	type_size_array.push_back(sizeof(DECORATION_MAJOR_TYPE));
	type_size_array.push_back(sizeof(DECORATION_SUB_TYPE));

	type_size_array.push_back(sizeof(DECORATION_ESSENCE));
	type_size_array.push_back(sizeof(MEDICINE_MAJOR_TYPE));
	type_size_array.push_back(sizeof(MEDICINE_SUB_TYPE));
	type_size_array.push_back(sizeof(MEDICINE_ESSENCE));
	type_size_array.push_back(sizeof(MATERIAL_MAJOR_TYPE));

	type_size_array.push_back(sizeof(MATERIAL_SUB_TYPE));
	type_size_array.push_back(sizeof(MATERIAL_ESSENCE));
	type_size_array.push_back(sizeof(DAMAGERUNE_SUB_TYPE));
	type_size_array.push_back(sizeof(DAMAGERUNE_ESSENCE));
	type_size_array.push_back(sizeof(ARMORRUNE_SUB_TYPE));
	
	type_size_array.push_back(sizeof(ARMORRUNE_ESSENCE));
	type_size_array.push_back(sizeof(SKILLTOME_SUB_TYPE));
	type_size_array.push_back(sizeof(SKILLTOME_ESSENCE));		
	type_size_array.push_back(sizeof(FLYSWORD_ESSENCE));
	type_size_array.push_back(sizeof(WINGMANWING_ESSENCE));

	type_size_array.push_back(sizeof(TOWNSCROLL_ESSENCE));
	type_size_array.push_back(sizeof(UNIONSCROLL_ESSENCE));
	type_size_array.push_back(sizeof(REVIVESCROLL_ESSENCE));
	type_size_array.push_back(sizeof(ELEMENT_ESSENCE));		
	type_size_array.push_back(sizeof(TASKMATTER_ESSENCE));

	type_size_array.push_back(sizeof(TOSSMATTER_ESSENCE));
	type_size_array.push_back(sizeof(PROJECTILE_TYPE));
	type_size_array.push_back(sizeof(PROJECTILE_ESSENCE));
	type_size_array.push_back(sizeof(QUIVER_SUB_TYPE));
	type_size_array.push_back(sizeof(QUIVER_ESSENCE));
	type_size_array.push_back(sizeof(STONE_SUB_TYPE));

	type_size_array.push_back(sizeof(STONE_ESSENCE));
	type_size_array.push_back(sizeof(MONSTER_ADDON));		
	type_size_array.push_back(sizeof(MONSTER_TYPE));
	type_size_array.push_back(sizeof(MONSTER_ESSENCE));	
	type_size_array.push_back(sizeof(NPC_TALK_SERVICE));

	type_size_array.push_back(sizeof(NPC_SELL_SERVICE));
	type_size_array.push_back(sizeof(NPC_BUY_SERVICE));
	type_size_array.push_back(sizeof(NPC_REPAIR_SERVICE));
	type_size_array.push_back(sizeof(NPC_INSTALL_SERVICE));
	type_size_array.push_back(sizeof(NPC_UNINSTALL_SERVICE));

	type_size_array.push_back(sizeof(NPC_TASK_IN_SERVICE));
	type_size_array.push_back(sizeof(NPC_TASK_OUT_SERVICE));
	type_size_array.push_back(sizeof(NPC_TASK_MATTER_SERVICE));
	type_size_array.push_back(sizeof(NPC_SKILL_SERVICE));
	type_size_array.push_back(sizeof(NPC_HEAL_SERVICE));
	
	type_size_array.push_back(sizeof(NPC_TRANSMIT_SERVICE));
	type_size_array.push_back(sizeof(NPC_TRANSPORT_SERVICE));
	type_size_array.push_back(sizeof(NPC_PROXY_SERVICE));
	type_size_array.push_back(sizeof(NPC_STORAGE_SERVICE));
	type_size_array.push_back(sizeof(NPC_MAKE_SERVICE));

	type_size_array.push_back(sizeof(NPC_DECOMPOSE_SERVICE));
	type_size_array.push_back(sizeof(NPC_TYPE));	
	type_size_array.push_back(sizeof(NPC_ESSENCE));
	type_size_array.push_back(sizeof(talk_proc*));

	type_size_array.push_back(sizeof(FACE_TEXTURE_ESSENCE));
	type_size_array.push_back(sizeof(FACE_SHAPE_ESSENCE));
	type_size_array.push_back(sizeof(FACE_EMOTION_TYPE));
	type_size_array.push_back(sizeof(FACE_EXPRESSION_ESSENCE));
	type_size_array.push_back(sizeof(FACE_HAIR_ESSENCE));

	type_size_array.push_back(sizeof(FACE_MOUSTACHE_ESSENCE));
	type_size_array.push_back(sizeof(COLORPICKER_ESSENCE));
	type_size_array.push_back(sizeof(CUSTOMIZEDATA_ESSENCE));
	type_size_array.push_back(sizeof(RECIPE_MAJOR_TYPE));
	type_size_array.push_back(sizeof(RECIPE_SUB_TYPE));
	type_size_array.push_back(sizeof(RECIPE_ESSENCE));

	type_size_array.push_back(sizeof(ENEMY_FACTION_CONFIG));
	type_size_array.push_back(sizeof(CHARRACTER_CLASS_CONFIG));
	type_size_array.push_back(sizeof(PARAM_ADJUST_CONFIG));
	type_size_array.push_back(sizeof(PLAYER_ACTION_INFO_CONFIG));
	type_size_array.push_back(sizeof(TASKDICE_ESSENCE));

	type_size_array.push_back(sizeof(TASKNORMALMATTER_ESSENCE));
	type_size_array.push_back(sizeof(FACE_FALING_ESSENCE));
	type_size_array.push_back(sizeof(PLAYER_LEVELEXP_CONFIG));
	type_size_array.push_back(sizeof(MINE_TYPE));
	type_size_array.push_back(sizeof(MINE_ESSENCE));

	type_size_array.push_back(sizeof(NPC_IDENTIFY_SERVICE));
	type_size_array.push_back(sizeof(FASHION_MAJOR_TYPE));	
	type_size_array.push_back(sizeof(FASHION_SUB_TYPE));
	type_size_array.push_back(sizeof(FASHION_ESSENCE));

	type_size_array.push_back(sizeof(FACETICKET_MAJOR_TYPE));	
	type_size_array.push_back(sizeof(FACETICKET_SUB_TYPE));
	type_size_array.push_back(sizeof(FACETICKET_ESSENCE));
	type_size_array.push_back(sizeof(FACEPILL_MAJOR_TYPE));	
	type_size_array.push_back(sizeof(FACEPILL_SUB_TYPE));

	type_size_array.push_back(sizeof(FACEPILL_ESSENCE));
	type_size_array.push_back(sizeof(SUITE_ESSENCE));
	type_size_array.push_back(sizeof(GM_GENERATOR_TYPE));
	type_size_array.push_back(sizeof(GM_GENERATOR_ESSENCE));
	type_size_array.push_back(sizeof(PET_TYPE));

	type_size_array.push_back(sizeof(PET_ESSENCE));
	type_size_array.push_back(sizeof(PET_EGG_ESSENCE));
	type_size_array.push_back(sizeof(PET_FOOD_ESSENCE));
	type_size_array.push_back(sizeof(PET_FACETICKET_ESSENCE));
	type_size_array.push_back(sizeof(FIREWORKS_ESSENCE));

	type_size_array.push_back(sizeof(WAR_TANKCALLIN_ESSENCE));
	type_size_array.push_back(sizeof(NPC_WAR_TOWERBUILD_SERVICE));
	type_size_array.push_back(sizeof(PLAYER_SECONDLEVEL_CONFIG));
	type_size_array.push_back(sizeof(NPC_RESETPROP_SERVICE));
	type_size_array.push_back(sizeof(NPC_PETNAME_SERVICE));

	type_size_array.push_back(sizeof(NPC_PETLEARNSKILL_SERVICE));
	type_size_array.push_back(sizeof(NPC_PETFORGETSKILL_SERVICE));
	type_size_array.push_back(sizeof(SKILLMATTER_ESSENCE));		
	type_size_array.push_back(sizeof(REFINE_TICKET_ESSENCE));
	type_size_array.push_back(sizeof(DESTROYING_ESSENCE));

	type_size_array.push_back(sizeof(NPC_EQUIPBIND_SERVICE));
	type_size_array.push_back(sizeof(NPC_EQUIPDESTROY_SERVICE));
	type_size_array.push_back(sizeof(NPC_EQUIPUNDESTROY_SERVICE));
	type_size_array.push_back(sizeof(BIBLE_ESSENCE));
	type_size_array.push_back(sizeof(SPEAKER_ESSENCE));

	type_size_array.push_back(sizeof(AUTOHP_ESSENCE));
	type_size_array.push_back(sizeof(AUTOMP_ESSENCE));
	type_size_array.push_back(sizeof(DOUBLE_EXP_ESSENCE));
	type_size_array.push_back(sizeof(TRANSMITSCROLL_ESSENCE));
	type_size_array.push_back(sizeof(DYE_TICKET_ESSENCE));

	type_size_array.push_back(sizeof(GOBLIN_ESSENCE));
	type_size_array.push_back(sizeof(GOBLIN_EQUIP_TYPE));
	type_size_array.push_back(sizeof(GOBLIN_EQUIP_ESSENCE));
	type_size_array.push_back(sizeof(GOBLIN_EXPPILL_ESSENCE));
	type_size_array.push_back(sizeof(SELL_CERTIFICATE_ESSENCE));

	type_size_array.push_back(sizeof(TARGET_ITEM_ESSENCE));
	type_size_array.push_back(sizeof(LOOK_INFO_ESSENCE));
	type_size_array.push_back(sizeof(UPGRADE_PRODUCTION_CONFIG));
	type_size_array.push_back(sizeof(ACC_STORAGE_BLACKLIST_CONFIG));
	type_size_array.push_back(sizeof(FACE_HAIR_TEXTURE_MAP));

	type_size_array.push_back(sizeof(MULTI_EXP_CONFIG));
	type_size_array.push_back(sizeof(INC_SKILL_ABILITY_ESSENCE));
	type_size_array.push_back(sizeof(GOD_EVIL_CONVERT_CONFIG));
	type_size_array.push_back(sizeof(WEDDING_CONFIG));
	type_size_array.push_back(sizeof(WEDDING_BOOKCARD_ESSENCE));

	type_size_array.push_back(sizeof(WEDDING_INVITECARD_ESSENCE));
	type_size_array.push_back(sizeof(SHARPENER_ESSENCE));
	type_size_array.push_back(sizeof(FACE_THIRDEYE_ESSENCE));
	type_size_array.push_back(sizeof(FACTION_FORTRESS_CONFIG));
	type_size_array.push_back(sizeof(FACTION_BUILDING_SUB_TYPE));
	
	type_size_array.push_back(sizeof(FACTION_BUILDING_ESSENCE));
	type_size_array.push_back(sizeof(FACTION_MATERIAL_ESSENCE));
	type_size_array.push_back(sizeof(CONGREGATE_ESSENCE));
	type_size_array.push_back(sizeof(ENGRAVE_MAJOR_TYPE));
	type_size_array.push_back(sizeof(ENGRAVE_SUB_TYPE));

	type_size_array.push_back(sizeof(ENGRAVE_ESSENCE));
	type_size_array.push_back(sizeof(NPC_ENGRAVE_SERVICE));
	type_size_array.push_back(sizeof(NPC_RANDPROP_SERVICE));
	type_size_array.push_back(sizeof(RANDPROP_TYPE));
	type_size_array.push_back(sizeof(RANDPROP_ESSENCE));

	type_size_array.push_back(sizeof(WIKI_TABOO_CONFIG));
	type_size_array.push_back(sizeof(FORCE_CONFIG));
	type_size_array.push_back(sizeof(FORCE_TOKEN_ESSENCE));
	type_size_array.push_back(sizeof(NPC_FORCE_SERVICE));
	type_size_array.push_back(sizeof(PLAYER_DEATH_DROP_CONFIG));

	type_size_array.push_back(sizeof(DYNSKILLEQUIP_ESSENCE));
	type_size_array.push_back(sizeof(CONSUME_POINTS_CONFIG));
	type_size_array.push_back(sizeof(ONLINE_AWARDS_CONFIG));
	type_size_array.push_back(sizeof(COUNTRY_CONFIG));
	type_size_array.push_back(sizeof(GM_ACTIVITY_CONFIG));

	type_size_array.push_back(sizeof(FASHION_WEAPON_CONFIG));
	type_size_array.push_back(sizeof(PET_EVOLVE_CONFIG));
	type_size_array.push_back(sizeof(PET_EVOLVED_SKILL_CONFIG));
	type_size_array.push_back(sizeof(MONEY_CONVERTIBLE_ESSENCE));
	type_size_array.push_back(sizeof(STONE_CHANGE_RECIPE_TYPE));

	type_size_array.push_back(sizeof(STONE_CHANGE_RECIPE));
	type_size_array.push_back(sizeof(MERIDIAN_CONFIG));
	type_size_array.push_back(sizeof(PET_EVOLVED_SKILL_RAND_CONFIG));
	type_size_array.push_back(sizeof(AUTOTASK_DISPLAY_CONFIG));
	type_size_array.push_back(sizeof(TOUCH_SHOP_CONFIG));
	
	type_size_array.push_back(sizeof(TITLE_CONFIG));
	type_size_array.push_back(sizeof(COMPLEX_TITLE_CONFIG));
	type_size_array.push_back(sizeof(MONSTER_SPIRIT_ESSENCE));
	type_size_array.push_back(sizeof(PLAYER_SPIRIT_CONFIG));
	type_size_array.push_back(sizeof(PLAYER_REINCARNATION_CONFIG));

	type_size_array.push_back(sizeof(HISTORY_STAGE_CONFIG));
	type_size_array.push_back(sizeof(HISTORY_ADVANCE_CONFIG));
	type_size_array.push_back(sizeof(AUTOTEAM_CONFIG));
	type_size_array.push_back(sizeof(PLAYER_REALM_CONFIG));
	type_size_array.push_back(sizeof(CHARIOT_CONFIG));

	type_size_array.push_back(sizeof(CHARIOT_WAR_CONFIG));
	type_size_array.push_back(sizeof(POKER_LEVELEXP_CONFIG));
	type_size_array.push_back(sizeof(POKER_SUITE_ESSENCE));
	type_size_array.push_back(sizeof(POKER_DICE_ESSENCE));
	type_size_array.push_back(sizeof(POKER_SUB_TYPE));

	type_size_array.push_back(sizeof(POKER_ESSENCE));
	type_size_array.push_back(sizeof(TOKEN_SHOP_CONFIG));
	type_size_array.push_back(sizeof(SHOP_TOKEN_ESSENCE));
	type_size_array.push_back(sizeof(GT_CONFIG));
	type_size_array.push_back(sizeof(RAND_SHOP_CONFIG));

	type_size_array.push_back(sizeof(PROFIT_TIME_CONFIG));
	type_size_array.push_back(sizeof(FACTION_PVP_CONFIG));
	type_size_array.push_back(sizeof(UNIVERSAL_TOKEN_ESSENCE));
	type_size_array.push_back(sizeof(TASK_LIST_CONFIG));
	type_size_array.push_back(sizeof(TASK_DICE_BY_WEIGHT_CONFIG));

	type_size_array.push_back(sizeof(FASHION_SUITE_ESSENCE));
	type_size_array.push_back(sizeof(FASHION_BEST_COLOR_CONFIG));
	type_size_array.push_back(sizeof(SIGN_AWARD_CONFIG));

	type_size_array.push_back(sizeof(0));	//DT_MAX
}

elementdataman::~elementdataman()
{
	for(unsigned int i=0; i<talk_proc_array.size(); i++)
	{
		delete talk_proc_array[i];
	}
}

void elementdataman::add_structure(unsigned int id, EQUIPMENT_ADDON & data)
{
	equipment_addon_array.push_back(data);

	unsigned int pos = equipment_addon_array.size()-1;
	add_id_index(ID_SPACE_ADDON, id, DT_EQUIPMENT_ADDON, pos, &(equipment_addon_array[0]));
}

void elementdataman::add_structure(unsigned int id, WEAPON_MAJOR_TYPE & data)
{
	weapon_major_type_array.push_back(data);
	
	unsigned int pos = weapon_major_type_array.size()-1;
	add_id_index(ID_SPACE_ESSENCE, id, DT_WEAPON_MAJOR_TYPE, pos, &(weapon_major_type_array[0]));
}

void elementdataman::add_structure(unsigned int id, WEAPON_SUB_TYPE & data)
{
	NormalizeRandom(&(data.probability_fastest), 5);

	weapon_sub_type_array.push_back(data);
	unsigned int pos = weapon_sub_type_array.size()-1;
	add_id_index(ID_SPACE_ESSENCE, id, DT_WEAPON_SUB_TYPE, pos, &(weapon_sub_type_array[0]));
}

void elementdataman::add_structure(unsigned int id, WEAPON_ESSENCE & data)
{
	NormalizeRandom(&(data.drop_probability_socket0), 3);
	NormalizeRandom(&(data.make_probability_socket0), 3);
	NormalizeRandom(&(data.probability_addon_num0), 6);

	eliminate_zero_item((unsigned char*)data.addons, sizeof(unsigned int)+sizeof(float), 32);
	eliminate_zero_item((unsigned char*)data.rands, sizeof(unsigned int)+sizeof(float), 32);
	eliminate_zero_item((unsigned char*)data.uniques, sizeof(unsigned int)+sizeof(float), 16);

	float r[32];
	int i=0;
	for(i=0; i<32; i++)		r[i] = data.addons[i].probability_addon;
	NormalizeRandom(r, 32);
	for(i=0; i<32; i++)		data.addons[i].probability_addon = r[i];

	for(i=0; i<32; i++)		r[i] = data.rands[i].probability_rand;
	NormalizeRandom(r, 32);
	for(i=0; i<32; i++)		data.rands[i].probability_rand = r[i];

	for(i=0; i<16; i++)		r[i] = data.uniques[i].probability_unique;
	NormalizeRandom(r, 16);
	for(i=0; i<16; i++)		data.uniques[i].probability_unique = r[i];
	
	if(data.pile_num_max == 0)
		data.pile_num_max = 1;

	weapon_essence_array.push_back(data);
	unsigned int pos = weapon_essence_array.size()-1;
	add_id_index(ID_SPACE_ESSENCE, id, DT_WEAPON_ESSENCE, pos, &(weapon_essence_array[0]));
}

void elementdataman::add_structure(unsigned int id, ARMOR_MAJOR_TYPE & data)
{
	armor_major_type_array.push_back(data);

	unsigned int pos = armor_major_type_array.size()-1;
	add_id_index(ID_SPACE_ESSENCE, id, DT_ARMOR_MAJOR_TYPE, pos, &(armor_major_type_array[0]));
}
	
void elementdataman::add_structure(unsigned int id, ARMOR_SUB_TYPE & data)
{
	armor_sub_type_array.push_back(data);

	unsigned int pos = armor_sub_type_array.size()-1;
	add_id_index(ID_SPACE_ESSENCE, id, DT_ARMOR_SUB_TYPE, pos, &(armor_sub_type_array[0]));
}

void elementdataman::add_structure(unsigned int id, ARMOR_ESSENCE & data)
{
	NormalizeRandom(&(data.drop_probability_socket0), 5);
	NormalizeRandom(&(data.make_probability_socket0), 5);
	NormalizeRandom(&(data.probability_addon_num0), 5);

	eliminate_zero_item((unsigned char*)data.addons, sizeof(unsigned int)+sizeof(float), 32);

	float r[32];
	int i=0;
	for(i=0; i<32; i++)		r[i] = data.addons[i].probability_addon;
	NormalizeRandom(r, 32);
	for(i=0; i<32; i++)		data.addons[i].probability_addon = r[i];

	for(i=0; i<32; i++)		r[i] = data.rands[i].probability_rand;
	NormalizeRandom(r, 32);
	for(i=0; i<32; i++)		data.rands[i].probability_rand = r[i];

	if(data.pile_num_max == 0)		data.pile_num_max = 1;

	armor_essence_array.push_back(data);
	
	unsigned int pos = armor_essence_array.size()-1;
	add_id_index(ID_SPACE_ESSENCE, id, DT_ARMOR_ESSENCE, pos, &(armor_essence_array[0]));
}

void elementdataman::add_structure(unsigned int id, DECORATION_MAJOR_TYPE & data)
{
	decoration_major_type_array.push_back(data);

	unsigned int pos = decoration_major_type_array.size()-1;		
	add_id_index(ID_SPACE_ESSENCE, id, DT_DECORATION_MAJOR_TYPE, pos, &(decoration_major_type_array[0]));
}

void elementdataman::add_structure(unsigned int id, DECORATION_SUB_TYPE & data)
{
	decoration_sub_type_array.push_back(data);

	unsigned int pos = decoration_sub_type_array.size()-1;
	add_id_index(ID_SPACE_ESSENCE, id, DT_DECORATION_SUB_TYPE, pos, &(decoration_sub_type_array[0]));
}

void elementdataman::add_structure(unsigned int id, DECORATION_ESSENCE & data)
{
	NormalizeRandom(&(data.probability_addon_num0), 5);
	eliminate_zero_item((unsigned char*)data.addons, sizeof(unsigned int)+sizeof(float), 32);

	float r[32];
	int i=0;
	for(i=0; i<32; i++)		r[i] = data.addons[i].probability_addon;
	NormalizeRandom(r, 32);
	for(i=0; i<32; i++)		data.addons[i].probability_addon = r[i];

	for(i=0; i<32; i++)		r[i] = data.rands[i].probability_rand;
	NormalizeRandom(r, 32);
	for(i=0; i<32; i++)		data.rands[i].probability_rand = r[i];

	if(data.pile_num_max == 0)		data.pile_num_max = 1;

	decoration_essence_array.push_back(data);

	unsigned int pos = decoration_essence_array.size()-1;
	add_id_index(ID_SPACE_ESSENCE, id, DT_DECORATION_ESSENCE, pos, &(decoration_essence_array[0]));
}

void elementdataman::add_structure(unsigned int id, MEDICINE_MAJOR_TYPE & data)
{
	medicine_major_type_array.push_back(data);

	unsigned int pos = medicine_major_type_array.size()-1;
	add_id_index(ID_SPACE_ESSENCE, id, DT_MEDICINE_MAJOR_TYPE, pos, &(medicine_major_type_array[0]));
}

void elementdataman::add_structure(unsigned int id, MEDICINE_SUB_TYPE & data)
{
	medicine_sub_type_array.push_back(data);

	unsigned int pos = medicine_sub_type_array.size()-1;
	add_id_index(ID_SPACE_ESSENCE, id, DT_MEDICINE_SUB_TYPE, pos, &(medicine_sub_type_array[0]));
}

void elementdataman::add_structure(unsigned int id, MEDICINE_ESSENCE & data)
{
	if(data.pile_num_max == 0)		data.pile_num_max = 1;

	medicine_essence_array.push_back(data);

	unsigned int pos = medicine_essence_array.size()-1;
	add_id_index(ID_SPACE_ESSENCE, id, DT_MEDICINE_ESSENCE, pos, &(medicine_essence_array[0]));
}

void elementdataman::add_structure(unsigned int id, MATERIAL_MAJOR_TYPE & data)
{
	material_major_type_array.push_back(data);

	unsigned int pos = material_major_type_array.size()-1;
	add_id_index(ID_SPACE_ESSENCE, id, DT_MATERIAL_MAJOR_TYPE, pos, &(material_major_type_array[0]));
}

void elementdataman::add_structure(unsigned int id, MATERIAL_SUB_TYPE & data)
{
	material_sub_type_array.push_back(data);

	unsigned int pos = material_sub_type_array.size()-1;
	add_id_index(ID_SPACE_ESSENCE, id, DT_MATERIAL_SUB_TYPE, pos, &(material_sub_type_array[0]));
}

void elementdataman::add_structure(unsigned int id, MATERIAL_ESSENCE & data)
{
	if(data.pile_num_max == 0)		data.pile_num_max = 1;

	material_essence_array.push_back(data);

	unsigned int pos = material_essence_array.size()-1;
	add_id_index(ID_SPACE_ESSENCE, id, DT_MATERIAL_ESSENCE, pos, &(material_essence_array[0]));
}

void elementdataman::add_structure(unsigned int id, DAMAGERUNE_SUB_TYPE & data)
{
	damagerune_sub_type_array.push_back(data);

	unsigned int pos = damagerune_sub_type_array.size()-1;
	add_id_index(ID_SPACE_ESSENCE, id, DT_DAMAGERUNE_SUB_TYPE, pos, &(damagerune_sub_type_array[0]));
}

void elementdataman::add_structure(unsigned int id, DAMAGERUNE_ESSENCE & data)
{
	if(data.pile_num_max == 0)		data.pile_num_max = 1;

	damagerune_essence_array.push_back(data);

	unsigned int pos = damagerune_essence_array.size()-1;
	add_id_index(ID_SPACE_ESSENCE, id, DT_DAMAGERUNE_ESSENCE, pos, &(damagerune_essence_array[0]));
}

void elementdataman::add_structure(unsigned int id, ARMORRUNE_SUB_TYPE & data)
{
	armorrune_sub_type_array.push_back(data);

	unsigned int pos = armorrune_sub_type_array.size()-1;
	add_id_index(ID_SPACE_ESSENCE, id, DT_ARMORRUNE_SUB_TYPE, pos, &(armorrune_sub_type_array[0]));
}

void elementdataman::add_structure(unsigned int id, ARMORRUNE_ESSENCE & data)
{
	if(data.pile_num_max == 0)		data.pile_num_max = 1;

	armorrune_essence_array.push_back(data);

	unsigned int pos = armorrune_essence_array.size()-1;
	add_id_index(ID_SPACE_ESSENCE, id, DT_ARMORRUNE_ESSENCE, pos, &(armorrune_essence_array[0]));
}

void elementdataman::add_structure(unsigned int id, SKILLTOME_SUB_TYPE & data)
{
	skilltome_sub_type_array.push_back(data);
	
	unsigned int pos = skilltome_sub_type_array.size()-1;
	add_id_index(ID_SPACE_ESSENCE, id, DT_SKILLTOME_SUB_TYPE, pos, &(skilltome_sub_type_array[0]));
}

void elementdataman::add_structure(unsigned int id, SKILLTOME_ESSENCE & data)
{
	if(data.pile_num_max == 0)		data.pile_num_max = 1;

	skilltome_essence_array.push_back(data);
	
	unsigned int pos = skilltome_essence_array.size()-1;
	add_id_index(ID_SPACE_ESSENCE, id, DT_SKILLTOME_ESSENCE, pos, &(skilltome_essence_array[0]));
}

void elementdataman::add_structure(unsigned int id, SHOP_TOKEN_ESSENCE & data)
{
	if(data.pile_num_max == 0)		data.pile_num_max = 1;	
	shop_token_essence_array.push_back(data);	
	unsigned int pos = shop_token_essence_array.size()-1;
	add_id_index(ID_SPACE_ESSENCE, id, DT_SHOP_TOKEN_ESSENCE, pos, &(shop_token_essence_array[0]));
}

void elementdataman::add_structure(unsigned int id, UNIVERSAL_TOKEN_ESSENCE & data)
{
	if(data.pile_num_max == 0)		data.pile_num_max = 1;	
	universal_token_essence_array.push_back(data);	
	unsigned int pos = universal_token_essence_array.size()-1;
	add_id_index(ID_SPACE_ESSENCE, id, DT_UNIVERSAL_TOKEN_ESSENCE, pos, &(universal_token_essence_array[0]));
}

void elementdataman::add_structure(unsigned int id, FASHION_SUITE_ESSENCE & data)
{
	fashion_suite_essence_array.push_back(data);	
	unsigned int pos = fashion_suite_essence_array.size()-1;
	add_id_index(ID_SPACE_ESSENCE, id, DT_FASHION_SUITE_ESSENCE, pos, &(fashion_suite_essence_array[0]));
}

void elementdataman::add_structure(unsigned int id, FACTION_BUILDING_SUB_TYPE & data)
{
	faction_building_sub_type_array.push_back(data);
	
	unsigned int pos = faction_building_sub_type_array.size()-1;
	add_id_index(ID_SPACE_ESSENCE, id, DT_FACTION_BUILDING_SUB_TYPE, pos, &(faction_building_sub_type_array[0]));
}

void elementdataman::add_structure(unsigned int id, FACTION_BUILDING_ESSENCE & data)
{
	faction_building_essence_array.push_back(data);
	
	unsigned int pos = faction_building_essence_array.size()-1;
	add_id_index(ID_SPACE_ESSENCE, id, DT_FACTION_BUILDING_ESSENCE, pos, &(faction_building_essence_array[0]));
}

void elementdataman::add_structure(unsigned int id, FACTION_MATERIAL_ESSENCE & data)
{
	faction_material_essence_array.push_back(data);
	
	unsigned int pos = faction_material_essence_array.size()-1;
	add_id_index(ID_SPACE_ESSENCE, id, DT_FACTION_MATERIAL_ESSENCE, pos, &(faction_material_essence_array[0]));
}
	
void elementdataman::add_structure(unsigned int id, FLYSWORD_ESSENCE & data)
{

	if(data.pile_num_max == 0)		data.pile_num_max = 1;

	if(data.time_increase_per_element < 0.05f) data.time_increase_per_element = 1.0f;

	flysword_essence_array.push_back(data);
	
	unsigned int pos = flysword_essence_array.size()-1;
	add_id_index(ID_SPACE_ESSENCE, id, DT_FLYSWORD_ESSENCE, pos, &(flysword_essence_array[0]));
}

void elementdataman::add_structure(unsigned int id, WINGMANWING_ESSENCE & data)
{
	if(data.pile_num_max == 0)		data.pile_num_max = 1;
	
	wingmanwing_essence_array.push_back(data);
	unsigned int pos = wingmanwing_essence_array.size()-1;
	add_id_index(ID_SPACE_ESSENCE, id, DT_WINGMANWING_ESSENCE, pos, &(wingmanwing_essence_array[0]));
}

void elementdataman::add_structure(unsigned int id, TOWNSCROLL_ESSENCE & data)
{
	if(data.pile_num_max == 0)		data.pile_num_max = 1;

	townscroll_essence_array.push_back(data);

	unsigned int pos = townscroll_essence_array.size()-1;
	add_id_index(ID_SPACE_ESSENCE, id, DT_TOWNSCROLL_ESSENCE, pos, &(townscroll_essence_array[0]));
}

void elementdataman::add_structure(unsigned int id, TRANSMITSCROLL_ESSENCE & data)
{
	if(data.pile_num_max == 0)		data.pile_num_max = 1;

	transmitscroll_essence_array.push_back(data);

	unsigned int pos = transmitscroll_essence_array.size()-1;
	add_id_index(ID_SPACE_ESSENCE, id, DT_TRANSMITSCROLL_ESSENCE, pos, &(transmitscroll_essence_array[0]));
}

void elementdataman::add_structure(unsigned int id, UNIONSCROLL_ESSENCE & data)
{

	if(data.pile_num_max == 0)		data.pile_num_max = 1;

	unionscroll_essence_array.push_back(data);

	unsigned int pos = unionscroll_essence_array.size()-1;
	add_id_index(ID_SPACE_ESSENCE, id, DT_UNIONSCROLL_ESSENCE, pos, &(unionscroll_essence_array[0]));
}

void elementdataman::add_structure(unsigned int id, REVIVESCROLL_ESSENCE & data)
{
	if(data.pile_num_max == 0)		data.pile_num_max = 1;

	revivescroll_essence_array.push_back(data);

	unsigned int pos = revivescroll_essence_array.size()-1;
	add_id_index(ID_SPACE_ESSENCE, id, DT_REVIVESCROLL_ESSENCE, pos, &(revivescroll_essence_array[0]));
}

void elementdataman::add_structure(unsigned int id, ELEMENT_ESSENCE & data)
{
	if(data.pile_num_max == 0)		data.pile_num_max = 1;

	element_essence_array.push_back(data);

	unsigned int pos = element_essence_array.size()-1;
	add_id_index(ID_SPACE_ESSENCE, id, DT_ELEMENT_ESSENCE, pos, &(element_essence_array[0]));
}

void elementdataman::add_structure(unsigned int id, TASKMATTER_ESSENCE & data)
{
	if(data.pile_num_max == 0)		data.pile_num_max = 1;

	taskmatter_essence_array.push_back(data);

	unsigned int pos = taskmatter_essence_array.size()-1;
	add_id_index(ID_SPACE_ESSENCE, id, DT_TASKMATTER_ESSENCE, pos, &(taskmatter_essence_array[0]));
}

void elementdataman::add_structure(unsigned int id, TOSSMATTER_ESSENCE & data)
{
	if(data.pile_num_max == 0)		data.pile_num_max = 1;

	tossmatter_essence_array.push_back(data);

	unsigned int pos = tossmatter_essence_array.size()-1;
	add_id_index(ID_SPACE_ESSENCE, id, DT_TOSSMATTER_ESSENCE, pos, &(tossmatter_essence_array[0]));
}

void elementdataman::add_structure(unsigned int id, PROJECTILE_TYPE & data)
{
	projectile_type_array.push_back(data);

	unsigned int pos = projectile_type_array.size()-1;
	add_id_index(ID_SPACE_ESSENCE, id, DT_PROJECTILE_TYPE, pos, &(projectile_type_array[0]));
}

void elementdataman::add_structure(unsigned int id, PROJECTILE_ESSENCE & data)
{
	if(data.pile_num_max == 0)		data.pile_num_max = 1;

	projectile_essence_array.push_back(data);

	unsigned int pos = projectile_essence_array.size()-1;
	add_id_index(ID_SPACE_ESSENCE, id, DT_PROJECTILE_ESSENCE, pos, &(projectile_essence_array[0]));
}

void elementdataman::add_structure(unsigned int id, QUIVER_SUB_TYPE & data)
{
	quiver_sub_type_array.push_back(data);

	unsigned int pos = quiver_sub_type_array.size()-1;
	add_id_index(ID_SPACE_ESSENCE, id, DT_QUIVER_SUB_TYPE, pos, &(quiver_sub_type_array[0]));
}

void elementdataman::add_structure(unsigned int id, QUIVER_ESSENCE & data)
{
	quiver_essence_array.push_back(data);

	unsigned int pos = quiver_essence_array.size()-1;
	add_id_index(ID_SPACE_ESSENCE, id, DT_QUIVER_ESSENCE, pos, &(quiver_essence_array[0]));
}

void elementdataman::add_structure(unsigned int id, STONE_SUB_TYPE & data)
{
	stone_sub_type_array.push_back(data);

	unsigned int pos = stone_sub_type_array.size()-1;
	add_id_index(ID_SPACE_ESSENCE, id, DT_STONE_SUB_TYPE, pos, &(stone_sub_type_array[0]));
}

void elementdataman::add_structure(unsigned int id, STONE_ESSENCE & data)
{
	if(data.pile_num_max == 0)		data.pile_num_max = 1;

	stone_essence_array.push_back(data);


	unsigned int pos = stone_essence_array.size()-1;
	add_id_index(ID_SPACE_ESSENCE, id, DT_STONE_ESSENCE, pos, &(stone_essence_array[0]));
}

void elementdataman::add_structure(unsigned int id, MONSTER_ADDON & data)
{
	monster_addon_array.push_back(data);
	
	unsigned int pos = monster_addon_array.size()-1;
	add_id_index(ID_SPACE_ADDON, id, DT_MONSTER_ADDON, pos, &(monster_addon_array[0]));
}

void elementdataman::add_structure(unsigned int id, MONSTER_TYPE & data)
{
	int i=0;
	float r[16];
	for(i=0; i<16; i++)		r[i] = data.addons[i].probability_addon;
	NormalizeRandom(r, 16);
	for(i=0; i<16; i++)		data.addons[i].probability_addon = r[i];
	
	monster_type_array.push_back(data);

	unsigned int pos = monster_type_array.size()-1;
	add_id_index(ID_SPACE_ESSENCE, id, DT_MONSTER_TYPE, pos, &(monster_type_array[0]));
}

void elementdataman::add_structure(unsigned int id, MONSTER_ESSENCE & data)
{
	int i=0;
	float r[32];

	eliminate_zero_item((unsigned char*) data.skills, sizeof(unsigned int)+sizeof(int), 32);

	for(i=0; i<4; i++)	r[i] = data.aggro_strategy[i].probability;
	NormalizeRandom(r, 4);
	for(i=0; i<4; i++)	data.aggro_strategy[i].probability = r[i];

	for(i=0; i<5; i++)	r[i] = data.skill_hp75[i].probability;
	NormalizeRandom(r, 5);
	for(i=0; i<5; i++)	data.skill_hp75[i].probability = r[i];

	for(i=0; i<5; i++)	r[i] = data.skill_hp50[i].probability;
	NormalizeRandom(r, 5);
	for(i=0; i<5; i++)	data.skill_hp50[i].probability = r[i];

	for(i=0; i<5; i++)	r[i] = data.skill_hp25[i].probability;
	NormalizeRandom(r, 5);
	for(i=0; i<5; i++)	data.skill_hp25[i].probability = r[i];
	
	NormalizeRandom(&(data.probability_drop_num0), 4);
	
	if( data.drop_times < 0 ) data.drop_times = 0;
	if( data.drop_times > 10 ) data.drop_times = 10;

	// 对于多次掉落的列表，由于除第一次以外的掉落在随机到后16个时要取消，所以不能进行去零操作。
	if( data.drop_times == 0 || data.drop_times == 1 )
		eliminate_zero_item((unsigned char*) data.drop_matters, sizeof(unsigned int)+sizeof(float), 32);
	else
	{
		for(i=0; i<32; i++)
			if( data.drop_matters[i].id == 0 ) data.drop_matters[i].probability = 0.0f;
	}

	for(i=0; i<32; i++)		r[i] = data.drop_matters[i].probability;
	NormalizeRandom(r, 32);
	for(i=0; i<32; i++)		data.drop_matters[i].probability = r[i];
	
	eliminate_zero_item((unsigned char*) data.drop_mines, sizeof(data.drop_mines[0]), ARRAY_SIZE(data.drop_mines));
	for(i=0; i<ARRAY_SIZE(data.drop_mines); i++)		r[i] = data.drop_mines[i].probability;
	NormalizeRandom(r, ARRAY_SIZE(data.drop_mines));
	for(i=0; i<ARRAY_SIZE(data.drop_mines); i++)		data.drop_mines[i].probability = r[i];

	monster_essence_array.push_back(data);

	unsigned int pos = monster_essence_array.size()-1;
	add_id_index(ID_SPACE_ESSENCE, id, DT_MONSTER_ESSENCE, pos, &(monster_essence_array[0]));
}

void elementdataman::add_structure(unsigned int id, NPC_TALK_SERVICE & data)
{
	npc_talk_service_array.push_back(data);

	unsigned int pos = npc_talk_service_array.size()-1;
	add_id_index(ID_SPACE_ESSENCE, id, DT_NPC_TALK_SERVICE, pos, &(npc_talk_service_array[0]));
}

void elementdataman::add_structure(unsigned int id, NPC_SELL_SERVICE & data)
{
	npc_sell_service_array.push_back(data);
	unsigned int pos = npc_sell_service_array.size()-1;
	add_id_index(ID_SPACE_ESSENCE, id, DT_NPC_SELL_SERVICE, pos, &(npc_sell_service_array[0]));
}

void elementdataman::add_structure(unsigned int id, NPC_BUY_SERVICE & data)
{
	npc_buy_service_array.push_back(data);

	unsigned int pos = npc_buy_service_array.size()-1;
	add_id_index(ID_SPACE_ESSENCE, id, DT_NPC_BUY_SERVICE, pos, &(npc_buy_service_array[0]));
}

void elementdataman::add_structure(unsigned int id, NPC_REPAIR_SERVICE & data)
{
	npc_repair_service_array.push_back(data);

	unsigned int pos = npc_repair_service_array.size()-1;
	add_id_index(ID_SPACE_ESSENCE, id, DT_NPC_REPAIR_SERVICE, pos, &(npc_repair_service_array[0]));
}

void elementdataman::add_structure(unsigned int id, NPC_INSTALL_SERVICE & data)
{
	eliminate_zero_item((unsigned char*)data.id_goods, sizeof(int), 32);
	npc_install_service_array.push_back(data);

	unsigned int pos = npc_install_service_array.size()-1;
	add_id_index(ID_SPACE_ESSENCE, id, DT_NPC_INSTALL_SERVICE, pos, &(npc_install_service_array[0]));
}

void elementdataman::add_structure(unsigned int id, NPC_UNINSTALL_SERVICE & data)
{
	eliminate_zero_item((unsigned char*)data.id_goods, sizeof(int), 32);
	npc_uninstall_service_array.push_back(data);

	unsigned int pos = npc_uninstall_service_array.size()-1;
	add_id_index(ID_SPACE_ESSENCE, id, DT_NPC_UNINSTALL_SERVICE, pos, &(npc_uninstall_service_array[0]));
}

void elementdataman::add_structure(unsigned int id, NPC_TASK_IN_SERVICE & data)
{
	eliminate_zero_item((unsigned char*) data.id_tasks, sizeof(int), 256);

	npc_task_in_service_array.push_back(data);

	unsigned int pos = npc_task_in_service_array.size()-1;
	add_id_index(ID_SPACE_ESSENCE, id, DT_NPC_TASK_IN_SERVICE, pos, &(npc_task_in_service_array[0]));
}

void elementdataman::add_structure(unsigned int id, NPC_TASK_OUT_SERVICE & data)
{
	eliminate_zero_item((unsigned char*) data.id_tasks, sizeof(int), 256);
	
	npc_task_out_service_array.push_back(data);

	unsigned int pos = npc_task_out_service_array.size()-1;
	add_id_index(ID_SPACE_ESSENCE, id, DT_NPC_TASK_OUT_SERVICE, pos, &(npc_task_out_service_array[0]));
}

void elementdataman::add_structure(unsigned int id, NPC_TASK_MATTER_SERVICE & data)
{
	eliminate_zero_item((unsigned char*) data.tasks, 4*(sizeof(unsigned int)+sizeof(int))+sizeof(int), 16);
	npc_task_matter_service_array.push_back(data);

	unsigned int pos = npc_task_matter_service_array.size()-1;
	add_id_index(ID_SPACE_ESSENCE, id, DT_NPC_TASK_MATTER_SERVICE, pos, &(npc_task_matter_service_array[0]));
}

void elementdataman::add_structure(unsigned int id, NPC_SKILL_SERVICE & data)
{
	eliminate_zero_item((unsigned char*) data.id_skills, sizeof(int), ARRAY_SIZE(data.id_skills));

	npc_skill_service_array.push_back(data);

	unsigned int pos = npc_skill_service_array.size()-1;
	add_id_index(ID_SPACE_ESSENCE, id, DT_NPC_SKILL_SERVICE, pos, &(npc_skill_service_array[0]));
}

void elementdataman::add_structure(unsigned int id, NPC_HEAL_SERVICE & data)
{
	npc_heal_service_array.push_back(data);

	unsigned int pos = npc_heal_service_array.size()-1;
	add_id_index(ID_SPACE_ESSENCE, id, DT_NPC_HEAL_SERVICE, pos, &(npc_heal_service_array[0]));
}

void elementdataman::add_structure(unsigned int id, NPC_TRANSMIT_SERVICE & data)
{
	npc_transmit_service_array.push_back(data);

	unsigned int pos = npc_transmit_service_array.size()-1;
	add_id_index(ID_SPACE_ESSENCE, id, DT_NPC_TRANSMIT_SERVICE, pos, &(npc_transmit_service_array[0]));
}

void elementdataman::add_structure(unsigned int id, NPC_TRANSPORT_SERVICE & data)
{
	eliminate_zero_item((unsigned char*) data.routes, sizeof(unsigned int)+sizeof(unsigned int), 32);
	npc_transport_service_array.push_back(data);

	unsigned int pos = npc_transport_service_array.size()-1;
	add_id_index(ID_SPACE_ESSENCE, id, DT_NPC_TRANSPORT_SERVICE, pos, &(npc_transport_service_array[0]));
}

void elementdataman::add_structure(unsigned int id, NPC_PROXY_SERVICE & data)
{
	npc_proxy_service_array.push_back(data);

	unsigned int pos = npc_proxy_service_array.size()-1;
	add_id_index(ID_SPACE_ESSENCE, id, DT_NPC_PROXY_SERVICE, pos, &(npc_proxy_service_array[0]));
}

void elementdataman::add_structure(unsigned int id, NPC_STORAGE_SERVICE & data)
{
	npc_storage_service_array.push_back(data);

	unsigned int pos = npc_storage_service_array.size()-1;
	add_id_index(ID_SPACE_ESSENCE, id, DT_NPC_STORAGE_SERVICE, pos, &(npc_storage_service_array[0]));
}

void elementdataman::add_structure(unsigned int id, NPC_MAKE_SERVICE & data)
{
	npc_make_service_array.push_back(data);

	unsigned int pos = npc_make_service_array.size()-1;
	add_id_index(ID_SPACE_ESSENCE, id, DT_NPC_MAKE_SERVICE, pos, &(npc_make_service_array[0]));
}

void elementdataman::add_structure(unsigned int id, NPC_DECOMPOSE_SERVICE & data)
{
	npc_decompose_service_array.push_back(data);

	unsigned int pos = npc_decompose_service_array.size()-1;
	add_id_index(ID_SPACE_ESSENCE, id, DT_NPC_DECOMPOSE_SERVICE, pos, &(npc_decompose_service_array[0]));
}

void elementdataman::add_structure(unsigned int id, NPC_TYPE & data)
{
	npc_type_array.push_back(data);

	unsigned int pos = npc_type_array.size()-1;
	add_id_index(ID_SPACE_ESSENCE, id, DT_NPC_TYPE, pos, &(npc_type_array[0]));
}

void elementdataman::add_structure(unsigned int id, NPC_ESSENCE & data)
{
	npc_essence_array.push_back(data);

	unsigned int pos = npc_essence_array.size()-1;
	add_id_index(ID_SPACE_ESSENCE, id, DT_NPC_ESSENCE, pos, &(npc_essence_array[0]));
}

void elementdataman::add_structure(unsigned int id, talk_proc * data)
{
	talk_proc_array.push_back(data);

	unsigned int pos = talk_proc_array.size()-1;
	add_id_index(ID_SPACE_TALK, id, DT_TALK_PROC, pos, &(talk_proc_array[0]));
}

void elementdataman::add_structure(unsigned int id,  FACE_TEXTURE_ESSENCE & data)
{
	face_texture_essence_array.push_back(data);

	unsigned int pos = face_texture_essence_array.size()-1;
	add_id_index(ID_SPACE_FACE, id, DT_FACE_TEXTURE_ESSENCE, pos, &(face_texture_essence_array[0]));
}

void elementdataman::add_structure(unsigned int id,  FACE_SHAPE_ESSENCE & data)
{
	face_shape_essence_array.push_back(data);

	unsigned int pos = face_shape_essence_array.size()-1;
	add_id_index(ID_SPACE_FACE, id, DT_FACE_SHAPE_ESSENCE, pos, &(face_shape_essence_array[0]));
}

void elementdataman::add_structure(unsigned int id,  FACE_EMOTION_TYPE & data)
{
	face_emotion_type_array.push_back(data);

	unsigned int pos = face_emotion_type_array.size()-1;
	add_id_index(ID_SPACE_FACE, id, DT_FACE_EMOTION_TYPE, pos, &(face_emotion_type_array[0]));
}

void elementdataman::add_structure(unsigned int id,  FACE_EXPRESSION_ESSENCE & data)
{
	face_expression_essence_array.push_back(data);

	unsigned int pos = face_expression_essence_array.size()-1;
	add_id_index(ID_SPACE_FACE, id, DT_FACE_EXPRESSION_ESSENCE, pos, &(face_expression_essence_array[0]));
}

void elementdataman::add_structure(unsigned int id,  FACE_HAIR_ESSENCE & data)
{
	face_hair_essence_array.push_back(data);

	unsigned int pos = face_hair_essence_array.size()-1;
	add_id_index(ID_SPACE_FACE, id, DT_FACE_HAIR_ESSENCE, pos, &(face_hair_essence_array[0]));
}

void elementdataman::add_structure(unsigned int id,  FACE_MOUSTACHE_ESSENCE & data)
{
	face_moustache_essence_array.push_back(data);

	unsigned int pos = face_moustache_essence_array.size()-1;
	add_id_index(ID_SPACE_FACE, id, DT_FACE_MOUSTACHE_ESSENCE, pos, &(face_moustache_essence_array[0]));	
}

void elementdataman::add_structure(unsigned int id,  COLORPICKER_ESSENCE & data)
{
	colorpicker_essence_array.push_back(data);

	unsigned int pos = colorpicker_essence_array.size()-1;
	add_id_index(ID_SPACE_FACE, id, DT_COLORPICKER_ESSENCE, pos, &(colorpicker_essence_array[0]));	
}

void elementdataman::add_structure(unsigned int id,  CUSTOMIZEDATA_ESSENCE & data)
{
	customizedata_essence_array.push_back(data);

	unsigned int pos = customizedata_essence_array.size()-1;
	add_id_index(ID_SPACE_FACE, id, DT_CUSTOMIZEDATA_ESSENCE, pos, &(customizedata_essence_array[0]));	
}

void elementdataman::add_structure(unsigned int id,  RECIPE_MAJOR_TYPE & data)
{
	recipe_major_type_array.push_back(data);

	unsigned int pos = recipe_major_type_array.size()-1;
	add_id_index(ID_SPACE_RECIPE, id, DT_RECIPE_MAJOR_TYPE, pos, &(recipe_major_type_array[0]));
}

void elementdataman::add_structure(unsigned int id,  RECIPE_SUB_TYPE & data)
{
	recipe_sub_type_array.push_back(data);

	unsigned int pos = recipe_sub_type_array.size()-1;
	add_id_index(ID_SPACE_RECIPE, id, DT_RECIPE_SUB_TYPE, pos, &(recipe_sub_type_array[0]));
}

void elementdataman::add_structure(unsigned int id,  RECIPE_ESSENCE & data)
{
	int i=0;
	float r[32];

	eliminate_zero_item((unsigned char*) data.targets, sizeof(unsigned int)+sizeof(float), 4);
	
	for(i=0; i<4; i++)	r[i] = data.targets[i].probability;
	NormalizeRandom(r, 4);
	for(i=0; i<4; i++)	data.targets[i].probability = r[i];

	recipe_essence_array.push_back(data);

	unsigned int pos = recipe_essence_array.size()-1;
	add_id_index(ID_SPACE_RECIPE, id, DT_RECIPE_ESSENCE, pos, &(recipe_essence_array[0]));
}

void elementdataman::add_structure(unsigned int id,  ENGRAVE_MAJOR_TYPE & data)
{
	engrave_major_type_array.push_back(data);

	unsigned int pos = engrave_major_type_array.size()-1;
	add_id_index(ID_SPACE_RECIPE, id, DT_ENGRAVE_MAJOR_TYPE, pos, &(engrave_major_type_array[0]));
}

void elementdataman::add_structure(unsigned int id,  ENGRAVE_SUB_TYPE & data)
{
	engrave_sub_type_array.push_back(data);

	unsigned int pos = engrave_sub_type_array.size()-1;
	add_id_index(ID_SPACE_RECIPE, id, DT_ENGRAVE_SUB_TYPE, pos, &(engrave_sub_type_array[0]));
}

void elementdataman::add_structure(unsigned int id,  ENGRAVE_ESSENCE & data)
{
	int i=0;
	const int NUM_ADDON = ARRAY_SIZE(data.addons);
	float r[NUM_ADDON];

	eliminate_zero_item((unsigned char*)data.materials, sizeof(data.materials[0]), ARRAY_SIZE(data.materials));
	NormalizeRandom(data.probability_addon_num, ARRAY_SIZE(data.probability_addon_num));
	
	for(i=0; i<NUM_ADDON; i++)		r[i] = data.addons[i].probability;
	NormalizeRandom(r, NUM_ADDON);
	for(i=0; i<NUM_ADDON; i++)		data.addons[i].probability = r[i];
	eliminate_zero_item((unsigned char *)data.addons, sizeof(data.addons[0]), ARRAY_SIZE(data.addons));

	engrave_essence_array.push_back(data);

	unsigned int pos = engrave_essence_array.size()-1;
	add_id_index(ID_SPACE_RECIPE, id, DT_ENGRAVE_ESSENCE, pos, &(engrave_essence_array[0]));
}

void elementdataman::add_structure(unsigned int id,  RANDPROP_TYPE & data)
{
	randprop_type_array.push_back(data);

	unsigned int pos = randprop_type_array.size()-1;
	add_id_index(ID_SPACE_RECIPE, id, DT_RANDPROP_TYPE, pos, &(randprop_type_array[0]));
}

void elementdataman::add_structure(unsigned int id,  RANDPROP_ESSENCE & data)
{
	eliminate_zero_item((unsigned char *)data.equip_id, sizeof(data.equip_id[0]), ARRAY_SIZE(data.equip_id));
	eliminate_zero_item((unsigned char *)data.materials, sizeof(data.materials[0]), ARRAY_SIZE(data.materials));
	
	randprop_essence_array.push_back(data);

	unsigned int pos = randprop_essence_array.size()-1;
	add_id_index(ID_SPACE_RECIPE, id, DT_RANDPROP_ESSENCE, pos, &(randprop_essence_array[0]));
}

void elementdataman::add_structure(unsigned int id,  STONE_CHANGE_RECIPE_TYPE & data)
{
	stone_change_recipe_type_array.push_back(data);
	
	unsigned int pos = stone_change_recipe_type_array.size()-1;
	add_id_index(ID_SPACE_RECIPE, id, DT_STONE_CHANGE_RECIPE_TYPE, pos, &(stone_change_recipe_type_array[0]));
}

void elementdataman::add_structure(unsigned int id,  STONE_CHANGE_RECIPE & data)
{
	eliminate_zero_item((unsigned char *)data.materials, sizeof(data.materials[0]), ARRAY_SIZE(data.materials));

	stone_change_recipe_array.push_back(data);
	
	unsigned int pos = stone_change_recipe_array.size()-1;
	add_id_index(ID_SPACE_RECIPE, id, DT_STONE_CHANGE_RECIPE, pos, &(stone_change_recipe_array[0]));
}

void elementdataman::add_structure(unsigned int id, ENEMY_FACTION_CONFIG & data)
{
	enemy_faction_config_array.push_back(data);

	unsigned int pos = enemy_faction_config_array.size()-1;
	add_id_index(ID_SPACE_CONFIG, id, DT_ENEMY_FACTION_CONFIG, pos, &(enemy_faction_config_array[0]));
}

void elementdataman::add_structure(unsigned int id,  CHARRACTER_CLASS_CONFIG & data)
{
	character_class_config_array.push_back(data);

	unsigned int pos = character_class_config_array.size()-1;
	add_id_index(ID_SPACE_CONFIG, id, DT_CHARRACTER_CLASS_CONFIG, pos, &(character_class_config_array[0]));

}

void elementdataman::add_structure(unsigned int id,  PARAM_ADJUST_CONFIG & data)
{
	eliminate_zero_item((unsigned char*) data.level_diff_adjust, sizeof(int)+sizeof(float)*5, 16);
	param_adjust_config_array.push_back(data);

	unsigned int pos = param_adjust_config_array.size()-1;
	add_id_index(ID_SPACE_CONFIG, id, DT_PARAM_ADJUST_CONFIG, pos, &(param_adjust_config_array[0]));
}

void elementdataman::add_structure(unsigned int id,  PLAYER_ACTION_INFO_CONFIG & data)
{
	player_action_info_config_array.push_back(data);

	unsigned int pos = player_action_info_config_array.size()-1;
	add_id_index(ID_SPACE_CONFIG, id, DT_PLAYER_ACTION_INFO_CONFIG, pos, &(player_action_info_config_array[0]));
}

void elementdataman::add_structure(unsigned int id,  UPGRADE_PRODUCTION_CONFIG & data)
{
	upgrade_production_config_array.push_back(data);

	unsigned int pos = upgrade_production_config_array.size()-1;
	add_id_index(ID_SPACE_CONFIG, id, DT_UPGRADE_PRODUCTION_CONFIG, pos, &(upgrade_production_config_array[0]));
}

void elementdataman::add_structure(unsigned int id,  ACC_STORAGE_BLACKLIST_CONFIG & data)
{
	acc_storage_blacklist_config_array.push_back(data);

	unsigned int pos = acc_storage_blacklist_config_array.size()-1;
	add_id_index(ID_SPACE_CONFIG, id, DT_ACC_STORAGE_BLACKLIST_CONFIG, pos, &(acc_storage_blacklist_config_array[0]));
}

void elementdataman::add_structure(unsigned int id,  PLAYER_DEATH_DROP_CONFIG & data)
{
	eliminate_zero_item((unsigned char*)data.itemlist, sizeof(data.itemlist[0]), ARRAY_SIZE(data.itemlist));

	player_death_drop_config_array.push_back(data);

	unsigned int pos = player_death_drop_config_array.size()-1;
	add_id_index(ID_SPACE_CONFIG, id, DT_PLAYER_DEATH_DROP_CONFIG, pos, &(player_death_drop_config_array[0]));
}

void elementdataman::add_structure(unsigned int id,  CONSUME_POINTS_CONFIG & data)
{
	eliminate_zero_item((unsigned char*)data.list, sizeof(data.list[0]), ARRAY_SIZE(data.list));
	
	consume_points_config_array.push_back(data);
	
	unsigned int pos = consume_points_config_array.size()-1;
	add_id_index(ID_SPACE_CONFIG, id, DT_CONSUME_POINTS_CONFIG, pos, &(consume_points_config_array[0]));
}

void elementdataman::add_structure(unsigned int id,  ONLINE_AWARDS_CONFIG & data)
{	
	online_awards_config_array.push_back(data);
	
	unsigned int pos = online_awards_config_array.size()-1;
	add_id_index(ID_SPACE_CONFIG, id, DT_ONLINE_AWARDS_CONFIG, pos, &(online_awards_config_array[0]));
}

void elementdataman::add_structure(unsigned int id,  PET_EVOLVE_CONFIG & data)
{
	pet_evolve_config_array.push_back(data);
	
	unsigned int pos = pet_evolve_config_array.size()-1;
	add_id_index(ID_SPACE_CONFIG, id, DT_PET_EVOLVE_CONFIG, pos, &(pet_evolve_config_array[0]));
}

void elementdataman::add_structure(unsigned int id,  PET_EVOLVED_SKILL_CONFIG & data)
{	
	pet_evolved_skill_config_array.push_back(data);
	
	unsigned int pos = pet_evolved_skill_config_array.size()-1;
	add_id_index(ID_SPACE_CONFIG, id, DT_PET_EVOLVED_SKILL_CONFIG, pos, &(pet_evolved_skill_config_array[0]));
}

void elementdataman::add_structure(unsigned int id,  PET_EVOLVED_SKILL_RAND_CONFIG & data)
{
	const int size = ARRAY_SIZE(data.rand_skill_group);
	float r[size];
	int i(0);
	for(i=0; i<ARRAY_SIZE(data.rand_skill_group); i++)
		r[i] = data.rand_skill_group[i].probability;
	NormalizeRandom(r, size);
	for(i=0; i<size; i++)
		data.rand_skill_group[i].probability = r[i];

	pet_evolved_skill_rand_config_array.push_back(data);
	
	unsigned int pos = pet_evolved_skill_rand_config_array.size()-1;
	add_id_index(ID_SPACE_CONFIG, id, DT_PET_EVOLVED_SKILL_RAND_CONFIG, pos, &(pet_evolved_skill_rand_config_array[0]));
}

void elementdataman::add_structure(unsigned int id,  AUTOTASK_DISPLAY_CONFIG & data)
{	
	autotask_display_config_array.push_back(data);
	
	unsigned int pos = autotask_display_config_array.size()-1;
	add_id_index(ID_SPACE_CONFIG, id, DT_AUTOTASK_DISPLAY_CONFIG, pos, &(autotask_display_config_array[0]));
}

void elementdataman::add_structure(unsigned int id,  PLAYER_SPIRIT_CONFIG & data)
{	
	player_spirit_config_array.push_back(data);
	
	unsigned int pos = player_spirit_config_array.size()-1;
	add_id_index(ID_SPACE_CONFIG, id, DT_PLAYER_SPIRIT_CONFIG, pos, &(player_spirit_config_array[0]));
}

void elementdataman::add_structure(unsigned int id,  HISTORY_STAGE_CONFIG & data)
{	
	history_stage_config_array.push_back(data);
	
	unsigned int pos = history_stage_config_array.size()-1;
	add_id_index(ID_SPACE_CONFIG, id, DT_HISTORY_STAGE_CONFIG, pos, &(history_stage_config_array[0]));
}

void elementdataman::add_structure(unsigned int id,  HISTORY_ADVANCE_CONFIG & data)
{	
	history_advance_config_array.push_back(data);
	
	unsigned int pos = history_advance_config_array.size()-1;
	add_id_index(ID_SPACE_CONFIG, id, DT_HISTORY_ADVANCE_CONFIG, pos, &(history_advance_config_array[0]));
}

void elementdataman::add_structure(unsigned int id,  AUTOTEAM_CONFIG & data)
{
	eliminate_zero_item((unsigned char*) data.worldtag_from, sizeof(data.worldtag_from[0]), ARRAY_SIZE(data.worldtag_from));
	eliminate_zero_item((unsigned char*) data.task, sizeof(data.task[0]), ARRAY_SIZE(data.task));
	autoteam_config_array.push_back(data);
	
	unsigned int pos = autoteam_config_array.size()-1;
	add_id_index(ID_SPACE_CONFIG, id, DT_AUTOTEAM_CONFIG, pos, &(autoteam_config_array[0]));
}

void elementdataman::add_structure(unsigned int id,  CHARIOT_CONFIG & data)
{	
	chariot_config_array.push_back(data);
	
	unsigned int pos = chariot_config_array.size()-1;
	add_id_index(ID_SPACE_CONFIG, id, DT_CHARIOT_CONFIG, pos, &(chariot_config_array[0]));
}

void elementdataman::add_structure(unsigned int id,  CHARIOT_WAR_CONFIG & data)
{	
	chariot_war_config_array.push_back(data);
	
	unsigned int pos = chariot_war_config_array.size()-1;
	add_id_index(ID_SPACE_CONFIG, id, DT_CHARIOT_WAR_CONFIG, pos, &(chariot_war_config_array[0]));
}

void elementdataman::add_structure(unsigned int id,  POKER_LEVELEXP_CONFIG & data)
{	
	poker_levelexp_config_array.push_back(data);
	
	unsigned int pos = poker_levelexp_config_array.size()-1;
	add_id_index(ID_SPACE_CONFIG, id, DT_POKER_LEVELEXP_CONFIG, pos, &(poker_levelexp_config_array[0]));
}

void elementdataman::add_structure(unsigned int id,  GT_CONFIG & data)
{	
	gt_config_array.push_back(data);
	
	unsigned int pos = gt_config_array.size()-1;
	add_id_index(ID_SPACE_CONFIG, id, DT_GT_CONFIG, pos, &(gt_config_array[0]));
}

void elementdataman::add_structure(unsigned int id,  MERIDIAN_CONFIG & data)
{	
	meridian_config_array.push_back(data);
	
	unsigned int pos = meridian_config_array.size()-1;
	add_id_index(ID_SPACE_CONFIG, id, DT_MERIDIAN_CONFIG, pos, &(meridian_config_array[0]));
}

void elementdataman::add_structure(unsigned int id,  MONEY_CONVERTIBLE_ESSENCE & data)
{	
	money_convertible_essence_array.push_back(data);
	
	unsigned int pos = money_convertible_essence_array.size()-1;
	add_id_index(ID_SPACE_ESSENCE, id, DT_MONEY_CONVERTIBLE_ESSENCE, pos, &(money_convertible_essence_array[0]));
}

void elementdataman::add_structure(unsigned int id,  FASHION_WEAPON_CONFIG & data)
{	
	fashion_weapon_config_array.push_back(data);
	
	unsigned int pos = fashion_weapon_config_array.size()-1;
	add_id_index(ID_SPACE_CONFIG, id, DT_FASHION_WEAPON_CONFIG, pos, &(fashion_weapon_config_array[0]));
}

void elementdataman::add_structure(unsigned int id,  MULTI_EXP_CONFIG & data)
{
	multi_exp_config_array.push_back(data);
	
	unsigned int pos = multi_exp_config_array.size()-1;
	add_id_index(ID_SPACE_CONFIG, id, DT_MULTI_EXP_CONFIG, pos, &(multi_exp_config_array[0]));
}

void elementdataman::add_structure(unsigned int id,  WEDDING_CONFIG & data)
{
	wedding_config_array.push_back(data);
	
	unsigned int pos = wedding_config_array.size()-1;
	add_id_index(ID_SPACE_CONFIG, id, DT_WEDDING_CONFIG, pos, &(wedding_config_array[0]));
}

void elementdataman::add_structure(unsigned int id,  GOD_EVIL_CONVERT_CONFIG & data)
{
	god_evil_convert_config_array.push_back(data);
	
	unsigned int pos = god_evil_convert_config_array.size()-1;
	add_id_index(ID_SPACE_CONFIG, id, DT_GOD_EVIL_CONVERT_CONFIG, pos, &(god_evil_convert_config_array[0]));
}

void elementdataman::add_structure(unsigned int id,  WIKI_TABOO_CONFIG & data)
{	
	eliminate_zero_item((unsigned char*) data.essence, sizeof(data.essence[0]), ARRAY_SIZE(data.essence));
	eliminate_zero_item((unsigned char*) data.recipe, sizeof(data.recipe[0]), ARRAY_SIZE(data.recipe));
	eliminate_zero_item((unsigned char*) data.task, sizeof(data.task[0]), ARRAY_SIZE(data.task));
	eliminate_zero_item((unsigned char*) data.skill, sizeof(data.skill[0]), ARRAY_SIZE(data.skill));
	
	wiki_taboo_config_array.push_back(data);
	
	unsigned int pos = wiki_taboo_config_array.size()-1;
	add_id_index(ID_SPACE_CONFIG, id, DT_WIKI_TABOO_CONFIG, pos, &(wiki_taboo_config_array[0]));
}

void elementdataman::add_structure(unsigned int id,  TASKDICE_ESSENCE & data)
{
	if(data.pile_num_max == 0)		data.pile_num_max = 1;
	int i;
	float r[ARRAY_SIZE(data.task_lists)];
	for(i=0; i<ARRAY_SIZE(data.task_lists); i++)	r[i] = data.task_lists[i].probability;
	NormalizeRandom(r, ARRAY_SIZE(data.task_lists));
	for(i=0; i<ARRAY_SIZE(data.task_lists); i++)	data.task_lists[i].probability = r[i];

	taskdice_essence_array.push_back(data);

	unsigned int pos = taskdice_essence_array.size()-1;
	add_id_index(ID_SPACE_ESSENCE, id, DT_TASKDICE_ESSENCE, pos, &(taskdice_essence_array[0]));
}

void elementdataman::add_structure(unsigned int id,  POKER_DICE_ESSENCE & data)
{
	if(data.pile_num_max == 0)		data.pile_num_max = 1;
	int i;
	float r[ARRAY_SIZE(data.list)];
	for(i=0; i<ARRAY_SIZE(data.list); i++)	r[i] = data.list[i].probability;
	NormalizeRandom(r, ARRAY_SIZE(data.list));
	for(i=0; i<ARRAY_SIZE(data.list); i++)	data.list[i].probability = r[i];
	
	poker_dice_essence_array.push_back(data);
	
	unsigned int pos = poker_dice_essence_array.size()-1;
	add_id_index(ID_SPACE_ESSENCE, id, DT_POKER_DICE_ESSENCE, pos, &(poker_dice_essence_array[0]));
}

void elementdataman::add_structure(unsigned int id,  TASKNORMALMATTER_ESSENCE & data)
{
	if(data.pile_num_max == 0)		data.pile_num_max = 1;
	tasknormalmatter_essence_array.push_back(data);

	unsigned int pos = tasknormalmatter_essence_array.size()-1;
	add_id_index(ID_SPACE_ESSENCE, id, DT_TASKNORMALMATTER_ESSENCE, pos, &(tasknormalmatter_essence_array[0]));
}

void elementdataman::add_structure(unsigned int id,	 FACE_FALING_ESSENCE & data)
{
	face_faling_essence_array.push_back(data);

	unsigned int pos = face_faling_essence_array.size()-1;
	add_id_index(ID_SPACE_FACE, id, DT_FACE_FALING_ESSENCE, pos, &(face_faling_essence_array[0]));
}

void elementdataman::add_structure(unsigned int id,	 FACE_THIRDEYE_ESSENCE & data)
{
	face_thirdeye_essence_array.push_back(data);

	unsigned int pos = face_thirdeye_essence_array.size()-1;
	add_id_index(ID_SPACE_FACE, id, DT_FACE_THIRDEYE_ESSENCE, pos, &(face_thirdeye_essence_array[0]));
}

void elementdataman::add_structure(unsigned int id,	 PLAYER_LEVELEXP_CONFIG & data)
{
	player_levelexp_config_array.push_back(data);

	unsigned int pos = player_levelexp_config_array.size()-1;
	add_id_index(ID_SPACE_CONFIG, id, DT_PLAYER_LEVELEXP_CONFIG, pos, &(player_levelexp_config_array[0]));
}

void elementdataman::add_structure(unsigned int id,	 FACTION_FORTRESS_CONFIG & data)
{
	faction_fortress_config_array.push_back(data);

	unsigned int pos = faction_fortress_config_array.size()-1;
	add_id_index(ID_SPACE_CONFIG, id, DT_FACTION_FORTRESS_CONFIG, pos, &(faction_fortress_config_array[0]));
}

void elementdataman::add_structure(unsigned int id,	 FORCE_CONFIG & data)
{
	force_config_array.push_back(data);

	unsigned int pos = force_config_array.size()-1;
	add_id_index(ID_SPACE_CONFIG, id, DT_FORCE_CONFIG, pos, &(force_config_array[0]));
}

void elementdataman::add_structure(unsigned int id,	 COUNTRY_CONFIG & data)
{
	country_config_array.push_back(data);
	
	unsigned int pos = country_config_array.size()-1;
	add_id_index(ID_SPACE_CONFIG, id, DT_COUNTRY_CONFIG, pos, &(country_config_array[0]));
}

void elementdataman::add_structure(unsigned int id,	 GM_ACTIVITY_CONFIG & data)
{	
	eliminate_zero_item((unsigned char*) data.openlist, sizeof(data.openlist[0]), ARRAY_SIZE(data.openlist));
	eliminate_zero_item((unsigned char*) data.closelist, sizeof(data.closelist[0]), ARRAY_SIZE(data.closelist));

	gm_activity_config_array.push_back(data);
	
	unsigned int pos = gm_activity_config_array.size()-1;
	add_id_index(ID_SPACE_CONFIG, id, DT_GM_ACTIVITY_CONFIG, pos, &(gm_activity_config_array[0]));
}

void elementdataman::add_structure(unsigned int id,  TOUCH_SHOP_CONFIG & data)
{	
	touch_shop_config_array.push_back(data);
	
	unsigned int pos = touch_shop_config_array.size()-1;
	add_id_index(ID_SPACE_CONFIG, id, DT_TOUCH_SHOP_CONFIG, pos, &(touch_shop_config_array[0]));
}

void elementdataman::add_structure(unsigned int id,  TOKEN_SHOP_CONFIG & data)
{	
	eliminate_zero_item((unsigned char*) data.item, sizeof(data.item[0]), ARRAY_SIZE(data.item));
	token_shop_config_array.push_back(data);
	
	unsigned int pos = token_shop_config_array.size()-1;
	add_id_index(ID_SPACE_CONFIG, id, DT_TOKEN_SHOP_CONFIG, pos, &(token_shop_config_array[0]));
}

void elementdataman::add_structure(unsigned int id,  RAND_SHOP_CONFIG & data)
{	
	int i;
	float r[ARRAY_SIZE(data.list)];
	for(i=0; i<ARRAY_SIZE(data.list); i++)	r[i] = data.list[i].probability;
	NormalizeRandom(r, ARRAY_SIZE(data.list));
	for(i=0; i<ARRAY_SIZE(data.list); i++)	data.list[i].probability = r[i];

	eliminate_zero_item((unsigned char*) data.list, sizeof(data.list[0]), ARRAY_SIZE(data.list));

	rand_shop_config_array.push_back(data);
	
	unsigned int pos = rand_shop_config_array.size()-1;
	add_id_index(ID_SPACE_CONFIG, id, DT_RAND_SHOP_CONFIG, pos, &(rand_shop_config_array[0]));
}

void elementdataman::add_structure(unsigned int id,  PROFIT_TIME_CONFIG & data)
{
	profit_time_config_array.push_back(data);
	
	unsigned int pos = profit_time_config_array.size()-1;
	add_id_index(ID_SPACE_CONFIG, id, DT_PROFIT_TIME_CONFIG, pos, &(profit_time_config_array[0]));
}

void elementdataman::add_structure(unsigned int id,  FACTION_PVP_CONFIG & data)
{
	faction_pvp_config_array.push_back(data);
	
	unsigned int pos = faction_pvp_config_array.size()-1;
	add_id_index(ID_SPACE_CONFIG, id, DT_FACTION_PVP_CONFIG, pos, &(faction_pvp_config_array[0]));
}

void elementdataman::add_structure(unsigned int id,  TASK_LIST_CONFIG & data)
{
	task_list_config_array.push_back(data);
	
	unsigned int pos = task_list_config_array.size()-1;
	add_id_index(ID_SPACE_CONFIG, id, DT_TASK_LIST_CONFIG, pos, &(task_list_config_array[0]));
}

void elementdataman::add_structure(unsigned int id,  TASK_DICE_BY_WEIGHT_CONFIG & data)
{
	task_dice_by_weight_config_array.push_back(data);
	
	unsigned int pos = task_dice_by_weight_config_array.size()-1;
	add_id_index(ID_SPACE_CONFIG, id, DT_TASK_DICE_BY_WEIGHT_CONFIG, pos, &(task_dice_by_weight_config_array[0]));
}

void elementdataman::add_structure(unsigned int id,  FASHION_BEST_COLOR_CONFIG & data)
{
	fashion_best_color_config_array.push_back(data);
	
	unsigned int pos = fashion_best_color_config_array.size()-1;
	add_id_index(ID_SPACE_CONFIG, id, DT_FASHION_BEST_COLOR_CONFIG, pos, &(fashion_best_color_config_array[0]));
}

void elementdataman::add_structure(unsigned int id,  SIGN_AWARD_CONFIG & data)
{
	sign_award_config_array.push_back(data);
	
	unsigned int pos = sign_award_config_array.size()-1;
	add_id_index(ID_SPACE_CONFIG, id, DT_SIGN_AWARD_CONFIG, pos, &(sign_award_config_array[0]));
}

void elementdataman::add_structure(unsigned int id,  TITLE_CONFIG & data)
{
	title_config_array.push_back(data);
	
	unsigned int pos = title_config_array.size()-1;
	add_id_index(ID_SPACE_CONFIG, id, DT_TITLE_CONFIG, pos, &(title_config_array[0]));
}

void elementdataman::add_structure(unsigned int id,  COMPLEX_TITLE_CONFIG & data)
{	
	eliminate_zero_item((unsigned char*)data.sub_titles, sizeof(data.sub_titles[0]), ARRAY_SIZE(data.sub_titles));

	complex_title_config_array.push_back(data);
	
	unsigned int pos = complex_title_config_array.size()-1;
	add_id_index(ID_SPACE_CONFIG, id, DT_COMPLEX_TITLE_CONFIG, pos, &(complex_title_config_array[0]));
}

void elementdataman::add_structure(unsigned int id,	 MINE_TYPE & data)
{
	mine_type_array.push_back(data);

	unsigned int pos = mine_type_array.size()-1;
	add_id_index(ID_SPACE_ESSENCE, id, DT_MINE_TYPE, pos, &(mine_type_array[0]));
}

void elementdataman::add_structure(unsigned int id,	 MINE_ESSENCE & data)
{
	int		i;

	float r[16];
	for(i=0; i<16; i++)	r[i] = data.materials[i].probability;
	NormalizeRandom(r, 16);
	for(i=0; i<16; i++)	data.materials[i].probability = r[i];
	
	r[0] = data.probability1;
	r[1] = data.probability2;
	NormalizeRandom(r, 2);
	data.probability1 = r[0];
	data.probability2 = r[1];

	eliminate_zero_item((unsigned char*) data.npcgen, sizeof(data.npcgen[0]), ARRAY_SIZE(data.npcgen));
	eliminate_zero_item((unsigned char*) data.aggros, sizeof(data.aggros[0]), ARRAY_SIZE(data.aggros));

	mine_essence_array.push_back(data);

	unsigned int pos = mine_essence_array.size()-1;
	add_id_index(ID_SPACE_ESSENCE, id, DT_MINE_ESSENCE, pos, &(mine_essence_array[0]));
}

void elementdataman::add_structure(unsigned int id, NPC_IDENTIFY_SERVICE & data)
{
	npc_identify_service_array.push_back(data);

	unsigned int pos = npc_identify_service_array.size()-1;
	add_id_index(ID_SPACE_ESSENCE, id, DT_NPC_IDENTIFY_SERVICE, pos, &(npc_identify_service_array[0]));
}

void elementdataman::add_structure(unsigned int id, FASHION_MAJOR_TYPE & data)
{
	fashion_major_type_array.push_back(data);

	unsigned int pos = fashion_major_type_array.size()-1;
	add_id_index(ID_SPACE_ESSENCE, id, DT_FASHION_MAJOR_TYPE, pos, &(fashion_major_type_array[0]));
}
	
void elementdataman::add_structure(unsigned int id, FASHION_SUB_TYPE & data)
{
	fashion_sub_type_array.push_back(data);

	unsigned int pos = fashion_sub_type_array.size()-1;
	add_id_index(ID_SPACE_ESSENCE, id, DT_FASHION_SUB_TYPE, pos, &(fashion_sub_type_array[0]));
}

void elementdataman::add_structure(unsigned int id, FASHION_ESSENCE & data)
{
	if(data.pile_num_max == 0)		data.pile_num_max = 1;

	fashion_essence_array.push_back(data);
	
	unsigned int pos = fashion_essence_array.size()-1;
	add_id_index(ID_SPACE_ESSENCE, id, DT_FASHION_ESSENCE, pos, &(fashion_essence_array[0]));
}

void elementdataman::add_structure(unsigned int id, POKER_SUB_TYPE & data)
{
	poker_sub_type_array.push_back(data);
	
	unsigned int pos = poker_sub_type_array.size()-1;
	add_id_index(ID_SPACE_ESSENCE, id, DT_POKER_SUB_TYPE, pos, &(poker_sub_type_array[0]));
}

void elementdataman::add_structure(unsigned int id, POKER_ESSENCE & data)
{
	if(data.pile_num_max == 0)		data.pile_num_max = 1;
	
	poker_essence_array.push_back(data);
	
	unsigned int pos = poker_essence_array.size()-1;
	add_id_index(ID_SPACE_ESSENCE, id, DT_POKER_ESSENCE, pos, &(poker_essence_array[0]));
}

void elementdataman::add_structure(unsigned int id, FACETICKET_MAJOR_TYPE & data)
{
	faceticket_major_type_array.push_back(data);

	unsigned int pos = faceticket_major_type_array.size()-1;
	add_id_index(ID_SPACE_ESSENCE, id, DT_FACETICKET_MAJOR_TYPE, pos, &(faceticket_major_type_array[0]));
}

void elementdataman::add_structure(unsigned int id, FACETICKET_SUB_TYPE & data)
{
	faceticket_sub_type_array.push_back(data);

	unsigned int pos = faceticket_sub_type_array.size()-1;
	add_id_index(ID_SPACE_ESSENCE, id, DT_FACETICKET_SUB_TYPE, pos, &(faceticket_sub_type_array[0]));
}

void elementdataman::add_structure(unsigned int id, FACETICKET_ESSENCE & data)
{
	if(data.pile_num_max == 0)		data.pile_num_max = 1;

	faceticket_essence_array.push_back(data);

	unsigned int pos = faceticket_essence_array.size()-1;
	add_id_index(ID_SPACE_ESSENCE, id, DT_FACETICKET_ESSENCE, pos, &(faceticket_essence_array[0]));
}

void elementdataman::add_structure(unsigned int id, FACEPILL_MAJOR_TYPE & data)
{
	facepill_major_type_array.push_back(data);

	unsigned int pos = facepill_major_type_array.size()-1;
	add_id_index(ID_SPACE_ESSENCE, id, DT_FACEPILL_MAJOR_TYPE, pos, &(facepill_major_type_array[0]));
}

void elementdataman::add_structure(unsigned int id, FACEPILL_SUB_TYPE & data)
{
	facepill_sub_type_array.push_back(data);

	unsigned int pos = facepill_sub_type_array.size()-1;
	add_id_index(ID_SPACE_ESSENCE, id, DT_FACEPILL_SUB_TYPE, pos, &(facepill_sub_type_array[0]));
}

void elementdataman::add_structure(unsigned int id, FACEPILL_ESSENCE & data)
{
	if(data.pile_num_max == 0)		data.pile_num_max = 1;

	facepill_essence_array.push_back(data);

	unsigned int pos = facepill_essence_array.size()-1;
	add_id_index(ID_SPACE_ESSENCE, id, DT_FACEPILL_ESSENCE, pos, &(facepill_essence_array[0]));
}

void elementdataman::add_structure(unsigned int id, SUITE_ESSENCE & data)
{
	eliminate_zero_item((unsigned char*) data.equipments, sizeof(unsigned int), 12);
	
	suite_essence_array.push_back(data);

	unsigned int pos = suite_essence_array.size()-1;
	add_id_index(ID_SPACE_ESSENCE, id, DT_SUITE_ESSENCE, pos, &(suite_essence_array[0]));
}

void elementdataman::add_structure(unsigned int id, POKER_SUITE_ESSENCE & data)
{
	eliminate_zero_item((unsigned char*) data.list, sizeof(unsigned int), ARRAY_SIZE(data.list));
	
	poker_suite_essence_array.push_back(data);
	
	unsigned int pos = poker_suite_essence_array.size()-1;
	add_id_index(ID_SPACE_ESSENCE, id, DT_POKER_SUITE_ESSENCE, pos, &(poker_suite_essence_array[0]));
}

void elementdataman::add_structure(unsigned int id, GM_GENERATOR_TYPE & data)
{
	gm_generator_type_array.push_back(data);

	unsigned int pos = gm_generator_type_array.size()-1;
	add_id_index(ID_SPACE_ESSENCE, id, DT_GM_GENERATOR_TYPE, pos, &(gm_generator_type_array[0]));
}

void elementdataman::add_structure(unsigned int id, GM_GENERATOR_ESSENCE & data)
{
	gm_generator_essence_array.push_back(data);

	unsigned int pos = gm_generator_essence_array.size()-1;
	add_id_index(ID_SPACE_ESSENCE, id, DT_GM_GENERATOR_ESSENCE, pos, &(gm_generator_essence_array[0]));
}

void elementdataman::add_structure(unsigned int id, PET_TYPE & data)
{
	pet_type_array.push_back(data);

	unsigned int pos = pet_type_array.size()-1;
	add_id_index(ID_SPACE_ESSENCE, id, DT_PET_TYPE, pos, &(pet_type_array[0]));
}

void elementdataman::add_structure(unsigned int id, PET_ESSENCE & data)
{
	pet_essence_array.push_back(data);

	unsigned int pos = pet_essence_array.size()-1;
	add_id_index(ID_SPACE_ESSENCE, id, DT_PET_ESSENCE, pos, &(pet_essence_array[0]));
}

void elementdataman::add_structure(unsigned int id, PET_EGG_ESSENCE & data)
{
	if(data.pile_num_max == 0)		data.pile_num_max = 1;

	pet_egg_essence_array.push_back(data);

	unsigned int pos = pet_egg_essence_array.size()-1;
	add_id_index(ID_SPACE_ESSENCE, id, DT_PET_EGG_ESSENCE, pos, &(pet_egg_essence_array[0]));
}

void elementdataman::add_structure(unsigned int id, PET_FOOD_ESSENCE & data)
{
	if(data.pile_num_max == 0)		data.pile_num_max = 1;

	pet_food_essence_array.push_back(data);

	unsigned int pos = pet_food_essence_array.size()-1;
	add_id_index(ID_SPACE_ESSENCE, id, DT_PET_FOOD_ESSENCE, pos, &(pet_food_essence_array[0]));
}

void elementdataman::add_structure(unsigned int id, PET_FACETICKET_ESSENCE & data)
{
	if(data.pile_num_max == 0)		data.pile_num_max = 1;

	pet_faceticket_essence_array.push_back(data);

	unsigned int pos = pet_faceticket_essence_array.size()-1;
	add_id_index(ID_SPACE_ESSENCE, id, DT_PET_FACETICKET_ESSENCE, pos, &(pet_faceticket_essence_array[0]));
}

void elementdataman::add_structure(unsigned int id, FIREWORKS_ESSENCE & data)
{
	if(data.pile_num_max == 0)		data.pile_num_max = 1;

	fireworks_essence_array.push_back(data);

	unsigned int pos = fireworks_essence_array.size()-1;
	add_id_index(ID_SPACE_ESSENCE, id, DT_FIREWORKS_ESSENCE, pos, &(fireworks_essence_array[0]));
}

void elementdataman::add_structure(unsigned int id, GOBLIN_ESSENCE & data)
{
	goblin_essence_array.push_back(data);

	unsigned int pos = goblin_essence_array.size()-1;
	add_id_index(ID_SPACE_ESSENCE, id, DT_GOBLIN_ESSENCE, pos, &(goblin_essence_array[0]));
}

void elementdataman::add_structure(unsigned int id, GOBLIN_EQUIP_TYPE & data)
{
	goblin_equip_type_array.push_back(data);

	unsigned int pos = goblin_equip_type_array.size()-1;
	add_id_index(ID_SPACE_ESSENCE, id, DT_GOBLIN_EQUIP_TYPE, pos, &(goblin_equip_type_array[0]));
}

void elementdataman::add_structure(unsigned int id, GOBLIN_EQUIP_ESSENCE & data)
{
	if(data.pile_num_max == 0)		data.pile_num_max = 1;
	
	goblin_equip_essence_array.push_back(data);

	unsigned int pos = goblin_equip_essence_array.size()-1;
	add_id_index(ID_SPACE_ESSENCE, id, DT_GOBLIN_EQUIP_ESSENCE, pos, &(goblin_equip_essence_array[0]));
}

void elementdataman::add_structure(unsigned int id, GOBLIN_EXPPILL_ESSENCE & data)
{
	if(data.pile_num_max == 0)		data.pile_num_max = 1;

	goblin_exppill_essence_array.push_back(data);

	unsigned int pos = goblin_exppill_essence_array.size()-1;
	add_id_index(ID_SPACE_ESSENCE, id, DT_GOBLIN_EXPPILL_ESSENCE, pos, &(goblin_exppill_essence_array[0]));
}

void elementdataman::add_structure(unsigned int id, SELL_CERTIFICATE_ESSENCE & data)
{
	if(data.pile_num_max == 0)		data.pile_num_max = 1;

	sell_certificate_essence_array.push_back(data);

	unsigned int pos = sell_certificate_essence_array.size()-1;
	add_id_index(ID_SPACE_ESSENCE, id, DT_SELL_CERTIFICATE_ESSENCE, pos, &(sell_certificate_essence_array[0]));
}

void elementdataman::add_structure(unsigned int id, TARGET_ITEM_ESSENCE & data)
{
	if(data.pile_num_max == 0)		data.pile_num_max = 1;

	eliminate_zero_item((unsigned char*)data.target_id_for_pop, sizeof(data.target_id_for_pop[0]), ARRAY_SIZE(data.target_id_for_pop));

	target_item_essence_array.push_back(data);

	unsigned int pos = target_item_essence_array.size()-1;
	add_id_index(ID_SPACE_ESSENCE, id, DT_TARGET_ITEM_ESSENCE, pos, &(target_item_essence_array[0]));
}

void elementdataman::add_structure(unsigned int id, LOOK_INFO_ESSENCE & data)
{
	if(data.pile_num_max == 0)		data.pile_num_max = 1;

	look_info_essence_array.push_back(data);

	unsigned int pos = look_info_essence_array.size()-1;
	add_id_index(ID_SPACE_ESSENCE, id, DT_LOOK_INFO_ESSENCE, pos, &(look_info_essence_array[0]));
}

void elementdataman::add_structure(unsigned int id, WAR_TANKCALLIN_ESSENCE & data)
{
	if(data.pile_num_max == 0)		data.pile_num_max = 1;

	war_tankcallin_essence_array.push_back(data);

	unsigned int pos = war_tankcallin_essence_array.size()-1;
	add_id_index(ID_SPACE_ESSENCE, id, DT_WAR_TANKCALLIN_ESSENCE, pos, &(war_tankcallin_essence_array[0]));
}

void elementdataman::add_structure(unsigned int id, NPC_WAR_TOWERBUILD_SERVICE & data)
{
	eliminate_zero_item((unsigned char*)data.build_info, sizeof(int)+sizeof(int)*4, 4);

	npc_war_towerbuild_service_array.push_back(data);

	unsigned int pos = npc_war_towerbuild_service_array.size()-1;
	add_id_index(ID_SPACE_ESSENCE, id, DT_NPC_WAR_TOWERBUILD_SERVICE, pos, &(npc_war_towerbuild_service_array[0]));
}

void elementdataman::add_structure(unsigned int id,	 PLAYER_SECONDLEVEL_CONFIG & data)
{
	player_secondlevel_config_array.push_back(data);

	unsigned int pos = player_secondlevel_config_array.size()-1;
	add_id_index(ID_SPACE_CONFIG, id, DT_PLAYER_SECONDLEVEL_CONFIG, pos, &(player_secondlevel_config_array[0]));
}

void elementdataman::add_structure(unsigned int id,	 PLAYER_REINCARNATION_CONFIG & data)
{
	player_reincarnation_config_array.push_back(data);
	
	unsigned int pos = player_reincarnation_config_array.size()-1;
	add_id_index(ID_SPACE_CONFIG, id, DT_PLAYER_REINCARNATION_CONFIG, pos, &(player_reincarnation_config_array[0]));
}

void elementdataman::add_structure(unsigned int id,	 PLAYER_REALM_CONFIG & data)
{
	player_realm_config_array.push_back(data);
	
	unsigned int pos = player_realm_config_array.size()-1;
	add_id_index(ID_SPACE_CONFIG, id, DT_PLAYER_REALM_CONFIG, pos, &(player_realm_config_array[0]));
}

void elementdataman::add_structure(unsigned int id, NPC_RESETPROP_SERVICE & data)
{
	eliminate_zero_item((unsigned char*)data.prop_entry, sizeof(int)+sizeof(int)*4, 15);

	npc_resetprop_service_array.push_back(data);

	unsigned int pos = npc_resetprop_service_array.size()-1;
	add_id_index(ID_SPACE_ESSENCE, id, DT_NPC_RESETPROP_SERVICE, pos, &(npc_resetprop_service_array[0]));
}

void elementdataman::add_structure(unsigned int id, NPC_PETNAME_SERVICE & data)
{
	npc_petname_service_array.push_back(data);

	unsigned int pos = npc_petname_service_array.size()-1;
	add_id_index(ID_SPACE_ESSENCE, id, DT_NPC_PETNAME_SERVICE, pos, &(npc_petname_service_array[0]));
}

void elementdataman::add_structure(unsigned int id, NPC_PETLEARNSKILL_SERVICE & data)
{
	npc_petlearnskill_service_array.push_back(data);

	unsigned int pos = npc_petlearnskill_service_array.size()-1;
	add_id_index(ID_SPACE_ESSENCE, id, DT_NPC_PETLEARNSKILL_SERVICE, pos, &(npc_petlearnskill_service_array[0]));
}

void elementdataman::add_structure(unsigned int id, NPC_PETFORGETSKILL_SERVICE & data)
{
	npc_petforgetskill_service_array.push_back(data);

	unsigned int pos = npc_petforgetskill_service_array.size()-1;
	add_id_index(ID_SPACE_ESSENCE, id, DT_NPC_PETFORGETSKILL_SERVICE, pos, &(npc_petforgetskill_service_array[0]));
}

void elementdataman::add_structure(unsigned int id, SKILLMATTER_ESSENCE & data)
{
	if(data.pile_num_max == 0)		data.pile_num_max = 1;

	skillmatter_essence_array.push_back(data);
	
	unsigned int pos = skillmatter_essence_array.size()-1;
	add_id_index(ID_SPACE_ESSENCE, id, DT_SKILLMATTER_ESSENCE, pos, &(skillmatter_essence_array[0]));
}

void elementdataman::add_structure(unsigned int id, DYNSKILLEQUIP_ESSENCE & data)
{
	if(data.pile_num_max == 0)		data.pile_num_max = 1;

	dynskillequip_essence_array.push_back(data);

	unsigned int pos = dynskillequip_essence_array.size()-1;
	add_id_index(ID_SPACE_ESSENCE, id, DT_DYNSKILLEQUIP_ESSENCE, pos, &(dynskillequip_essence_array[0]));
}

void elementdataman::add_structure(unsigned int id, INC_SKILL_ABILITY_ESSENCE & data)
{
	if(data.pile_num_max == 0)		data.pile_num_max = 1;
	
	inc_skill_ability_essence_array.push_back(data);
	
	unsigned int pos = inc_skill_ability_essence_array.size()-1;
	add_id_index(ID_SPACE_ESSENCE, id, DT_INC_SKILL_ABILITY_ESSENCE, pos, &(inc_skill_ability_essence_array[0]));
}

void elementdataman::add_structure(unsigned int id, WEDDING_BOOKCARD_ESSENCE & data)
{
	if(data.pile_num_max == 0)		data.pile_num_max = 1;
	
	wedding_bookcard_essence_array.push_back(data);
	
	unsigned int pos = wedding_bookcard_essence_array.size()-1;
	add_id_index(ID_SPACE_ESSENCE, id, DT_WEDDING_BOOKCARD_ESSENCE, pos, &(wedding_bookcard_essence_array[0]));
}

void elementdataman::add_structure(unsigned int id, WEDDING_INVITECARD_ESSENCE & data)
{
	if(data.pile_num_max == 0)		data.pile_num_max = 1;
	
	wedding_invitecard_essence_array.push_back(data);
	
	unsigned int pos = wedding_invitecard_essence_array.size()-1;
	add_id_index(ID_SPACE_ESSENCE, id, DT_WEDDING_INVITECARD_ESSENCE, pos, &(wedding_invitecard_essence_array[0]));
}

void elementdataman::add_structure(unsigned int id, SHARPENER_ESSENCE & data)
{
	if(data.pile_num_max == 0)		data.pile_num_max = 1;
	
	sharpener_essence_array.push_back(data);
	
	unsigned int pos = sharpener_essence_array.size()-1;
	add_id_index(ID_SPACE_ESSENCE, id, DT_SHARPENER_ESSENCE, pos, &(sharpener_essence_array[0]));
}

void elementdataman::add_structure(unsigned int id, CONGREGATE_ESSENCE & data)
{
	if(data.pile_num_max == 0)		data.pile_num_max = 1;	

	eliminate_zero_item((unsigned char*)data.area, sizeof(data.area[0]), ARRAY_SIZE(data.area));

	congregate_essence_array.push_back(data);
	
	unsigned int pos = congregate_essence_array.size()-1;
	add_id_index(ID_SPACE_ESSENCE, id, DT_CONGREGATE_ESSENCE, pos, &(congregate_essence_array[0]));
}

void elementdataman::add_structure(unsigned int id, MONSTER_SPIRIT_ESSENCE & data)
{	
	if(data.pile_num_max == 0)		data.pile_num_max = 1;	
	monster_spirit_essence_array.push_back(data);
	
	unsigned int pos = monster_spirit_essence_array.size()-1;
	add_id_index(ID_SPACE_ESSENCE, id, DT_MONSTER_SPIRIT_ESSENCE, pos, &(monster_spirit_essence_array[0]));
}

void elementdataman::add_structure(unsigned int id, DESTROYING_ESSENCE & data)
{
	if(data.pile_num_max == 0)		data.pile_num_max = 1;

	destroying_essence_array.push_back(data);
	
	unsigned int pos = destroying_essence_array.size()-1;
	add_id_index(ID_SPACE_ESSENCE, id, DT_DESTROYING_ESSENCE, pos, &(destroying_essence_array[0]));
}

void elementdataman::add_structure(unsigned int id, DOUBLE_EXP_ESSENCE & data)
{
	if(data.pile_num_max == 0)		data.pile_num_max = 1;
	
	double_exp_essence_array.push_back(data);

	unsigned int pos = double_exp_essence_array.size()-1;
	add_id_index(ID_SPACE_ESSENCE, id, DT_DOUBLE_EXP_ESSENCE, pos, &(double_exp_essence_array[0]));
}

void elementdataman::add_structure(unsigned int id, DYE_TICKET_ESSENCE & data)
{
	if(data.pile_num_max == 0)		data.pile_num_max = 1;
	if(data.h_min < 0.0f)			data.h_min = 0.0f;
	if(data.h_max > 1.0f)			data.h_max = 1.0f;
	if(data.s_min < 0.0f)			data.s_min = 0.0f;
	if(data.s_max > 1.0f)			data.s_max = 1.0f;
	if(data.v_min < 0.0f)			data.v_min = 0.0f;
	if(data.v_max > 1.0f)			data.v_max = 1.0f;

	dye_ticket_essence_array.push_back(data);

	unsigned int pos = dye_ticket_essence_array.size()-1;
	add_id_index(ID_SPACE_ESSENCE, id, DT_DYE_TICKET_ESSENCE, pos, &(dye_ticket_essence_array[0]));
}

void elementdataman::add_structure(unsigned int id, REFINE_TICKET_ESSENCE & data)
{
	if(data.pile_num_max == 0)		data.pile_num_max = 1;

	refine_ticket_essence_array.push_back(data);
	
	unsigned int pos = refine_ticket_essence_array.size()-1;
	add_id_index(ID_SPACE_ESSENCE, id, DT_REFINE_TICKET_ESSENCE, pos, &(refine_ticket_essence_array[0]));
}

void elementdataman::add_structure(unsigned int id, NPC_EQUIPBIND_SERVICE & data)
{
	eliminate_zero_item((unsigned char*)data.id_object_need, sizeof(data.id_object_need[0]), ARRAY_SIZE(data.id_object_need));

	npc_equipbind_service_array.push_back(data);
	
	unsigned int pos = npc_equipbind_service_array.size()-1;
	add_id_index(ID_SPACE_ESSENCE, id, DT_NPC_EQUIPBIND_SERVICE, pos, &(npc_equipbind_service_array[0]));
}

void elementdataman::add_structure(unsigned int id, NPC_EQUIPDESTROY_SERVICE & data)
{
	npc_equipdestroy_service_array.push_back(data);
	
	unsigned int pos = npc_equipdestroy_service_array.size()-1;
	add_id_index(ID_SPACE_ESSENCE, id, DT_NPC_EQUIPDESTROY_SERVICE, pos, &(npc_equipdestroy_service_array[0]));
}

void elementdataman::add_structure(unsigned int id, NPC_EQUIPUNDESTROY_SERVICE & data)
{
	npc_equipundestroy_service_array.push_back(data);
	
	unsigned int pos = npc_equipundestroy_service_array.size()-1;
	add_id_index(ID_SPACE_ESSENCE, id, DT_NPC_EQUIPUNDESTROY_SERVICE, pos, &(npc_equipundestroy_service_array[0]));
}

void elementdataman::add_structure(unsigned int id, NPC_ENGRAVE_SERVICE & data)
{
	eliminate_zero_item((unsigned char*)data.id_engrave, sizeof(data.id_engrave[0]), ARRAY_SIZE(data.id_engrave));

	npc_engrave_service_array.push_back(data);
	
	unsigned int pos = npc_engrave_service_array.size()-1;
	add_id_index(ID_SPACE_ESSENCE, id, DT_NPC_ENGRAVE_SERVICE, pos, &(npc_engrave_service_array[0]));
}

void elementdataman::add_structure(unsigned int id, NPC_RANDPROP_SERVICE & data)
{
	npc_randprop_service_array.push_back(data);
	
	unsigned int pos = npc_randprop_service_array.size()-1;
	add_id_index(ID_SPACE_ESSENCE, id, DT_NPC_RANDPROP_SERVICE, pos, &(npc_randprop_service_array[0]));
}

void elementdataman::add_structure(unsigned int id, NPC_FORCE_SERVICE & data)
{
	npc_force_service_array.push_back(data);

	unsigned int pos = npc_force_service_array.size()-1;
	add_id_index(ID_SPACE_ESSENCE, id, DT_NPC_FORCE_SERVICE, pos, &(npc_force_service_array[0]));
}

void elementdataman::add_structure(unsigned int id, BIBLE_ESSENCE & data)
{
	data.pile_num_max = 1;
	bible_essence_array.push_back(data);
	
	unsigned int pos = bible_essence_array.size()-1;
	add_id_index(ID_SPACE_ESSENCE, id, DT_BIBLE_ESSENCE, pos, &(bible_essence_array[0]));
}

void elementdataman::add_structure(unsigned int id, SPEAKER_ESSENCE & data)
{
	data.pile_num_max = 1;
	speaker_essence_array.push_back(data);
	
	unsigned int pos = speaker_essence_array.size()-1;
	add_id_index(ID_SPACE_ESSENCE, id, DT_SPEAKER_ESSENCE, pos, &(speaker_essence_array[0]));
}

void elementdataman::add_structure(unsigned int id, AUTOHP_ESSENCE & data)
{
	data.pile_num_max = 1;
	autohp_essence_array.push_back(data);
	
	unsigned int pos = autohp_essence_array.size()-1;
	add_id_index(ID_SPACE_ESSENCE, id, DT_AUTOHP_ESSENCE, pos, &(autohp_essence_array[0]));
}

void elementdataman::add_structure(unsigned int id, AUTOMP_ESSENCE & data)
{
	data.pile_num_max = 1;
	automp_essence_array.push_back(data);
	
	unsigned int pos = automp_essence_array.size()-1;
	add_id_index(ID_SPACE_ESSENCE, id, DT_AUTOMP_ESSENCE, pos, &(automp_essence_array[0]));
}

void elementdataman::add_structure(unsigned int id, FORCE_TOKEN_ESSENCE & data)
{
	data.pile_num_max = 1;
	force_token_essence_array.push_back(data);

	unsigned int pos = force_token_essence_array.size()-1;
	add_id_index(ID_SPACE_ESSENCE, id, DT_FORCE_TOKEN_ESSENCE, pos, &(force_token_essence_array[0]));
}

void elementdataman::add_structure(unsigned int id, FACE_HAIR_TEXTURE_MAP & data)
{
	face_hair_texture_map_array.push_back(data);
	
	unsigned int pos = face_hair_texture_map_array.size()-1;
	add_id_index(ID_SPACE_FACE, id, DT_FACE_HAIR_TEXTURE_MAP, pos, &(face_hair_texture_map_array[0]));
}

void elementdataman::add_id_index_essence(unsigned int id, DATA_TYPE type, unsigned int pos, void * start)
{
	LOCATION p;
	p.type = type;
	p.pos = pos;
	essence_id_index_map[id] = p;
	start_ptr_array[type] = start;
}


void elementdataman::add_id_index_addon(unsigned int id, DATA_TYPE type, unsigned int pos, void * start)
{
	LOCATION p;
	p.type = type;
	p.pos = pos;
	addon_id_index_map[id] = p;
	start_ptr_array[type] = start;
}

void elementdataman::add_id_index_talk(unsigned int id, DATA_TYPE type, unsigned int pos, void * start)
{
	LOCATION p;
	p.type = type;
	p.pos = pos;
	talk_id_index_map[id] = p;
	start_ptr_array[type] = start;
}

void elementdataman::add_id_index_face(unsigned int id, DATA_TYPE type, unsigned int pos, void * start)
{
	LOCATION p;
	p.type = type;
	p.pos = pos;
	face_id_index_map[id] = p;
	start_ptr_array[type] = start;
}

void elementdataman::add_id_index_recipe(unsigned int id, DATA_TYPE type, unsigned int pos, void * start)
{
	LOCATION p;
	p.type = type;
	p.pos = pos;
	recipe_id_index_map[id] = p;
	start_ptr_array[type] = start;
}

void elementdataman::add_id_index_config(unsigned int id, DATA_TYPE type, unsigned int pos, void * start)
{
	LOCATION p;
	p.type = type;
	p.pos = pos;
	config_id_index_map[id] = p;
	start_ptr_array[type] = start;
}

void elementdataman::add_id_index(ID_SPACE idspace, unsigned int id, DATA_TYPE type, unsigned int pos, void * start)
{
	switch(idspace)
	{
	case ID_SPACE_ESSENCE:
		add_id_index_essence(id, type, pos, start);	
		break;
	case ID_SPACE_ADDON:
		add_id_index_addon(id, type, pos, start);
		break;
	case ID_SPACE_TALK:
		add_id_index_talk(id, type, pos, start);
		break;
	case ID_SPACE_FACE:
		add_id_index_face(id, type, pos, start);
		break;
	case ID_SPACE_RECIPE:
		add_id_index_recipe(id, type, pos, start);
	case ID_SPACE_CONFIG:
		add_id_index_config(id, type, pos, start);			
	default:
		break;
	}
}

unsigned int elementdataman::get_data_id(ID_SPACE idspace, unsigned int index, DATA_TYPE& datatype)
{
	unsigned int id = 0;
	unsigned int i;
	IDToLOCATIONMap::iterator itr;
	switch(idspace)
	{
	case ID_SPACE_ESSENCE:
		itr = essence_id_index_map.begin();
 		for(i=0; i<index; i++)	++itr;
		id = itr->first;
		datatype = (itr->second).type;
		break;
		
	case ID_SPACE_ADDON:
		itr = addon_id_index_map.begin();
		for(i=0; i<index; i++)	++itr;
		id = itr->first;
		datatype = (itr->second).type;
		break;
		
	case ID_SPACE_TALK:
		itr = talk_id_index_map.begin();
		for(i=0; i<index; i++)	++itr;
		id = itr->first;
		datatype = (itr->second).type;
		break;

	case ID_SPACE_FACE:
		itr = face_id_index_map.begin();
		for(i=0; i<index; i++)	++itr;
		id = itr->first;
		datatype = (itr->second).type;
		break;

	case ID_SPACE_RECIPE:
		itr = recipe_id_index_map.begin();
		for(i=0; i<index; i++)	++itr;
		id = itr->first;
		datatype = (itr->second).type;
		break;

	case ID_SPACE_CONFIG:
		itr = config_id_index_map.begin();
		for(i=0; i<index; i++)	++itr;
		id = itr->first;
		datatype = (itr->second).type;
		break;

	default:
		id = 0;
		datatype = DT_INVALID;
		break;
	}
	return id;
}

unsigned int elementdataman::get_first_data_id(ID_SPACE idspace, DATA_TYPE& datatype)
{
	unsigned int id = 0;
	switch(idspace)
	{
	case ID_SPACE_ESSENCE:
		esssence_id_index_itr = essence_id_index_map.begin();
		if(esssence_id_index_itr == essence_id_index_map.end())
		{
			id = 0;
			datatype = DT_INVALID;
		}
		else
		{
			id = esssence_id_index_itr->first;
			datatype = (esssence_id_index_itr->second).type;
		}
		break;
		
	case ID_SPACE_ADDON:
		addon_id_index_itr = addon_id_index_map.begin();
		if(addon_id_index_itr == addon_id_index_map.end())
		{
			id = 0;
			datatype = DT_INVALID;
		}
		else
		{
			id = addon_id_index_itr->first;
			datatype = (addon_id_index_itr->second).type;
		}
		break;
		
	case ID_SPACE_TALK:
		talk_id_index_itr = talk_id_index_map.begin();
		if(talk_id_index_itr == talk_id_index_map.end())
		{
			id = 0;
			datatype = DT_INVALID;
		}
		else
		{
			id = talk_id_index_itr->first;
			datatype = (talk_id_index_itr->second).type;
		}
		break;

	case ID_SPACE_FACE:
		face_id_index_itr = face_id_index_map.begin();
		if(face_id_index_itr == face_id_index_map.end())
		{
			id = 0;
			datatype = DT_INVALID;
		}
		else
		{
			id = face_id_index_itr->first;
			datatype = (face_id_index_itr->second).type;
		}
		break;

	case ID_SPACE_RECIPE:
		recipe_id_index_itr = recipe_id_index_map.begin();
		if(recipe_id_index_itr == recipe_id_index_map.end())
		{
			id = 0;
			datatype = DT_INVALID;
		}
		else
		{
			id = recipe_id_index_itr->first;
			datatype = (recipe_id_index_itr->second).type;
		}
		break;

	case ID_SPACE_CONFIG:
		config_id_index_itr = config_id_index_map.begin();
		if(config_id_index_itr == config_id_index_map.end())
		{
			id = 0;
			datatype = DT_INVALID;
		}
		else
		{
			id = config_id_index_itr->first;
			datatype = (config_id_index_itr->second).type;
		}
		break;

	default:
		id = 0;
		datatype = DT_INVALID;
		break;
	}
	return id;
}

unsigned int elementdataman::get_cur_data_id(ID_SPACE idspace, DATA_TYPE& datatype)
{
	unsigned int id;
	switch(idspace)
	{
	case ID_SPACE_ESSENCE:
		if(esssence_id_index_itr == essence_id_index_map.end())
		{
			id = 0;
			datatype = DT_INVALID;
		}
		else
		{
			id = esssence_id_index_itr->first;
			datatype = (esssence_id_index_itr->second).type;
		}
		break;
		
	case ID_SPACE_ADDON:
		if(addon_id_index_itr == addon_id_index_map.end())
		{
			id = 0;
			datatype = DT_INVALID;
		}
		else
		{
			id = addon_id_index_itr->first;
			datatype = (addon_id_index_itr->second).type;
		}
		break;
		
	case ID_SPACE_TALK:
		if(talk_id_index_itr == talk_id_index_map.end())
		{
			id = 0;
			datatype = DT_INVALID;
		}
		else
		{
			id = talk_id_index_itr->first;
			datatype = (talk_id_index_itr->second).type;
		}
		break;
		
	case ID_SPACE_FACE:
		if(face_id_index_itr == face_id_index_map.end())
		{
			id = 0;
			datatype = DT_INVALID;
		}
		else
		{
			id = face_id_index_itr->first;
			datatype = (face_id_index_itr->second).type;
		}
		break;
		
	case ID_SPACE_RECIPE:
		if(recipe_id_index_itr == recipe_id_index_map.end())
		{
			id = 0;
			datatype = DT_INVALID;
		}
		else
		{
			id = recipe_id_index_itr->first;
			datatype = (recipe_id_index_itr->second).type;
		}
		break;
		
	case ID_SPACE_CONFIG:
		if(config_id_index_itr == config_id_index_map.end())
		{
			id = 0;
			datatype = DT_INVALID;
		}
		else
		{
			id = config_id_index_itr->first;
			datatype = (config_id_index_itr->second).type;
		}
		break;
		
	default:
		datatype = DT_INVALID;
		break;
	}
	return id;	
}

unsigned int elementdataman::get_next_data_id(ID_SPACE idspace, DATA_TYPE& datatype)
{
	unsigned int id;
	switch(idspace)
	{
	case ID_SPACE_ESSENCE:
		++esssence_id_index_itr;
		if(esssence_id_index_itr == essence_id_index_map.end())
		{
			id = 0;
			datatype = DT_INVALID;
		}
		else
		{
			id = esssence_id_index_itr->first;
			datatype = (esssence_id_index_itr->second).type;
		}
		break;
		
	case ID_SPACE_ADDON:
		++addon_id_index_itr;
		if(addon_id_index_itr == addon_id_index_map.end())
		{
			id = 0;
			datatype = DT_INVALID;
		}
		else
		{
			id = addon_id_index_itr->first;
			datatype = (addon_id_index_itr->second).type;
		}
		break;
		
	case ID_SPACE_TALK:
		++talk_id_index_itr;
		if(talk_id_index_itr == talk_id_index_map.end())
		{
			id = 0;
			datatype = DT_INVALID;
		}
		else
		{
			id = talk_id_index_itr->first;
			datatype = (talk_id_index_itr->second).type;
		}
		break;

	case ID_SPACE_FACE:
		++face_id_index_itr;
		if(face_id_index_itr == face_id_index_map.end())
		{
			id = 0;
			datatype = DT_INVALID;
		}
		else
		{
			id = face_id_index_itr->first;
			datatype = (face_id_index_itr->second).type;
		}
		break;

	case ID_SPACE_RECIPE:
		++recipe_id_index_itr;
		if(recipe_id_index_itr == recipe_id_index_map.end())
		{
			id = 0;
			datatype = DT_INVALID;
		}
		else
		{
			id = recipe_id_index_itr->first;
			datatype = (recipe_id_index_itr->second).type;
		}
		break;

	case ID_SPACE_CONFIG:
		++config_id_index_itr;
		if(config_id_index_itr == config_id_index_map.end())
		{
			id = 0;
			datatype = DT_INVALID;
		}
		else
		{
			id = config_id_index_itr->first;
			datatype = (config_id_index_itr->second).type;
		}
		break;

	default:
		datatype = DT_INVALID;
		break;
	}
	return id;	
}

unsigned int elementdataman::get_data_num(ID_SPACE idspace)
{
	switch(idspace)
	{
	case ID_SPACE_ESSENCE:
		return essence_id_index_map.size();

	case ID_SPACE_ADDON:
		return addon_id_index_map.size();

	case ID_SPACE_TALK:
		return talk_id_index_map.size();

	case ID_SPACE_FACE:
		return face_id_index_map.size();
	
	case ID_SPACE_RECIPE:
		return recipe_id_index_map.size();

	case ID_SPACE_CONFIG:
		return config_id_index_map.size();

	default:
		return 0;
	}
	return 0;
}

DATA_TYPE elementdataman::get_data_type(unsigned int id, ID_SPACE idspace)
{
	IDToLOCATIONMap::iterator itr;
	switch(idspace)
	{
	case ID_SPACE_ESSENCE:
		itr = essence_id_index_map.find(id);
		if(itr != essence_id_index_map.end())
		{
			return (itr->second).type;
		}
		else
			return DT_INVALID;			
		break;

	case ID_SPACE_ADDON:
		itr = addon_id_index_map.find(id);
		if(itr != addon_id_index_map.end())
		{
			return (itr->second).type;
		}
		else
			return DT_INVALID;
		break;		

	case ID_SPACE_TALK:
		itr = talk_id_index_map.find(id);
		if(itr != talk_id_index_map.end())
		{
			return (itr->second).type;
		}
		else
			return DT_INVALID;
		break;

	case ID_SPACE_FACE:
		itr = face_id_index_map.find(id);
		if(itr != face_id_index_map.end())
		{
			return (itr->second).type;
		}
		else
			return DT_INVALID;
		break;

	case ID_SPACE_RECIPE:
		itr = recipe_id_index_map.find(id);
		if(itr != recipe_id_index_map.end())
		{
			return (itr->second).type;
		}
		else
			return DT_INVALID;
		break;

	case ID_SPACE_CONFIG:
		itr = config_id_index_map.find(id);
		if(itr != config_id_index_map.end())
		{
			return (itr->second).type;
		}
		else
			return DT_INVALID;
		break;

	default:
		return DT_INVALID;
		break;
	}
}

const void * elementdataman::get_data_ptr(unsigned int id, ID_SPACE idspace, DATA_TYPE& datatype)
{
	datatype = DT_INVALID;
	int pos;
	IDToLOCATIONMap::iterator itr;
	switch(idspace)
	{
	case ID_SPACE_ESSENCE:
		itr = essence_id_index_map.find(id);
		if(itr != essence_id_index_map.end())
		{
			datatype = (itr->second).type;
			pos = (itr->second).pos;
		}
		else
			datatype = DT_INVALID;			
		break;

	case ID_SPACE_ADDON:
		itr = addon_id_index_map.find(id);
		if(itr != addon_id_index_map.end())
		{
			datatype = (itr->second).type;
			pos = (itr->second).pos;
		}
		else
			datatype = DT_INVALID;
		break;
		
	case ID_SPACE_TALK:
		itr = talk_id_index_map.find(id);
		if(itr != talk_id_index_map.end())
		{
			datatype = (itr->second).type;
			pos = (itr->second).pos;
		}
		else
			datatype = DT_INVALID;
		break;

	case ID_SPACE_FACE:
		itr = face_id_index_map.find(id);
		if(itr != face_id_index_map.end())
		{
			datatype = (itr->second).type;
			pos = (itr->second).pos;
		}
		else
			datatype = DT_INVALID;
		break;

	case ID_SPACE_RECIPE:
		itr = recipe_id_index_map.find(id);
		if(itr != recipe_id_index_map.end())
		{
			datatype = (itr->second).type;
			pos = (itr->second).pos;
		}
		else
			datatype = DT_INVALID;
		break;

	case ID_SPACE_CONFIG:
		itr = config_id_index_map.find(id);
		if(itr != config_id_index_map.end())
		{
			datatype = (itr->second).type;
			pos = (itr->second).pos;
		}
		else
			datatype = DT_INVALID;
		break;

	default:
		datatype = DT_INVALID;
		break;
	}

	if(datatype != DT_INVALID)
	{
		char * start = (char *) start_ptr_array[datatype];
		if(datatype == DT_TALK_PROC)
			return (const void*) *(int *)(start+pos*type_size_array[datatype]);
		else
			return start+pos*type_size_array[datatype];
	}
	else
		return NULL;
}

void elementdataman::setup_hash_map()
{
#define ADD_HASH_MAP(ids, typ, arr) \
p.type = typ;\
for(i=0; i<arr.size(); i++)\
{\
	start_ptr_array[typ] = &(arr[0]);\
	p.pos = i;\
	ids##_id_index_map[arr[i].id] = p;\
}

	unsigned int i;
	LOCATION p;

	ADD_HASH_MAP(addon, DT_EQUIPMENT_ADDON, equipment_addon_array)
	ADD_HASH_MAP(essence, DT_WEAPON_MAJOR_TYPE, weapon_major_type_array)
	ADD_HASH_MAP(essence, DT_WEAPON_SUB_TYPE, weapon_sub_type_array)
	ADD_HASH_MAP(essence, DT_WEAPON_ESSENCE, weapon_essence_array)
	ADD_HASH_MAP(essence, DT_ARMOR_MAJOR_TYPE, armor_major_type_array)

	ADD_HASH_MAP(essence, DT_ARMOR_SUB_TYPE, armor_sub_type_array)
	ADD_HASH_MAP(essence, DT_ARMOR_ESSENCE, armor_essence_array)
	ADD_HASH_MAP(essence, DT_DECORATION_MAJOR_TYPE, decoration_major_type_array)
	ADD_HASH_MAP(essence, DT_DECORATION_SUB_TYPE, decoration_sub_type_array)
	ADD_HASH_MAP(essence, DT_DECORATION_ESSENCE, decoration_essence_array)

	ADD_HASH_MAP(essence, DT_MEDICINE_MAJOR_TYPE, medicine_major_type_array)
	ADD_HASH_MAP(essence, DT_MEDICINE_SUB_TYPE, medicine_sub_type_array)
	ADD_HASH_MAP(essence, DT_MEDICINE_ESSENCE, medicine_essence_array)
	ADD_HASH_MAP(essence, DT_MATERIAL_MAJOR_TYPE, material_major_type_array)
	ADD_HASH_MAP(essence, DT_MATERIAL_SUB_TYPE, material_sub_type_array)

	ADD_HASH_MAP(essence, DT_MATERIAL_ESSENCE, material_essence_array)
	ADD_HASH_MAP(essence, DT_DAMAGERUNE_SUB_TYPE, damagerune_sub_type_array)
	ADD_HASH_MAP(essence, DT_DAMAGERUNE_ESSENCE, damagerune_essence_array)
	ADD_HASH_MAP(essence, DT_ARMORRUNE_SUB_TYPE, armorrune_sub_type_array)
	ADD_HASH_MAP(essence, DT_ARMORRUNE_ESSENCE, armorrune_essence_array)	//20

	ADD_HASH_MAP(essence, DT_SKILLTOME_SUB_TYPE, skilltome_sub_type_array)
	ADD_HASH_MAP(essence, DT_SKILLTOME_ESSENCE, skilltome_essence_array)
	ADD_HASH_MAP(essence, DT_FLYSWORD_ESSENCE, flysword_essence_array)
	ADD_HASH_MAP(essence, DT_WINGMANWING_ESSENCE, wingmanwing_essence_array)
	ADD_HASH_MAP(essence, DT_TOWNSCROLL_ESSENCE, townscroll_essence_array)
	
	ADD_HASH_MAP(essence, DT_UNIONSCROLL_ESSENCE, unionscroll_essence_array)
	ADD_HASH_MAP(essence, DT_REVIVESCROLL_ESSENCE, revivescroll_essence_array)
	ADD_HASH_MAP(essence, DT_ELEMENT_ESSENCE, element_essence_array)
	ADD_HASH_MAP(essence, DT_TASKMATTER_ESSENCE, taskmatter_essence_array)
	ADD_HASH_MAP(essence, DT_TOSSMATTER_ESSENCE, tossmatter_essence_array)

	ADD_HASH_MAP(essence, DT_PROJECTILE_TYPE, projectile_type_array)
	ADD_HASH_MAP(essence, DT_PROJECTILE_ESSENCE, projectile_essence_array)
	ADD_HASH_MAP(essence, DT_QUIVER_SUB_TYPE, quiver_sub_type_array)
	ADD_HASH_MAP(essence, DT_QUIVER_ESSENCE, quiver_essence_array)
	ADD_HASH_MAP(essence, DT_STONE_SUB_TYPE, stone_sub_type_array)

	ADD_HASH_MAP(essence, DT_STONE_ESSENCE, stone_essence_array)
	ADD_HASH_MAP(addon, DT_MONSTER_ADDON, monster_addon_array)
	ADD_HASH_MAP(essence, DT_MONSTER_TYPE, monster_type_array)
	ADD_HASH_MAP(essence, DT_MONSTER_ESSENCE, monster_essence_array)
	ADD_HASH_MAP(essence, DT_NPC_TALK_SERVICE, npc_talk_service_array)

	ADD_HASH_MAP(essence, DT_NPC_SELL_SERVICE, npc_sell_service_array)
	ADD_HASH_MAP(essence, DT_NPC_BUY_SERVICE, npc_buy_service_array)
	ADD_HASH_MAP(essence, DT_NPC_REPAIR_SERVICE, npc_repair_service_array)
	ADD_HASH_MAP(essence, DT_NPC_INSTALL_SERVICE, npc_install_service_array)
	ADD_HASH_MAP(essence, DT_NPC_UNINSTALL_SERVICE, npc_uninstall_service_array)

	ADD_HASH_MAP(essence, DT_NPC_TASK_IN_SERVICE, npc_task_in_service_array)
	ADD_HASH_MAP(essence, DT_NPC_TASK_OUT_SERVICE, npc_task_out_service_array)
	ADD_HASH_MAP(essence, DT_NPC_TASK_MATTER_SERVICE, npc_task_matter_service_array)
	ADD_HASH_MAP(essence, DT_NPC_SKILL_SERVICE, npc_skill_service_array)
	ADD_HASH_MAP(essence, DT_NPC_HEAL_SERVICE, npc_heal_service_array)

	ADD_HASH_MAP(essence, DT_NPC_TRANSMIT_SERVICE, npc_transmit_service_array)
	ADD_HASH_MAP(essence, DT_NPC_TRANSPORT_SERVICE, npc_transport_service_array)
	ADD_HASH_MAP(essence, DT_NPC_PROXY_SERVICE, npc_proxy_service_array)
	ADD_HASH_MAP(essence, DT_NPC_STORAGE_SERVICE, npc_storage_service_array)
	ADD_HASH_MAP(essence, DT_NPC_MAKE_SERVICE, npc_make_service_array)
	
	ADD_HASH_MAP(essence, DT_NPC_DECOMPOSE_SERVICE, npc_decompose_service_array)
	ADD_HASH_MAP(essence, DT_NPC_TYPE, npc_type_array)
	ADD_HASH_MAP(essence, DT_NPC_ESSENCE, npc_essence_array)
	
//	talk_proc hashmap has been set up	
	
	ADD_HASH_MAP(face, DT_FACE_TEXTURE_ESSENCE, face_texture_essence_array)
	ADD_HASH_MAP(face, DT_FACE_SHAPE_ESSENCE, face_shape_essence_array)
	ADD_HASH_MAP(face, DT_FACE_EMOTION_TYPE, face_emotion_type_array)
	ADD_HASH_MAP(face, DT_FACE_EXPRESSION_ESSENCE, face_expression_essence_array)
	ADD_HASH_MAP(face, DT_FACE_HAIR_ESSENCE, face_hair_essence_array)
	ADD_HASH_MAP(face, DT_FACE_MOUSTACHE_ESSENCE, face_moustache_essence_array)
	ADD_HASH_MAP(face, DT_COLORPICKER_ESSENCE, colorpicker_essence_array)
	ADD_HASH_MAP(face, DT_CUSTOMIZEDATA_ESSENCE, customizedata_essence_array)

	ADD_HASH_MAP(recipe, DT_RECIPE_MAJOR_TYPE, recipe_major_type_array)
	ADD_HASH_MAP(recipe, DT_RECIPE_SUB_TYPE, recipe_sub_type_array)
	ADD_HASH_MAP(recipe, DT_RECIPE_ESSENCE, recipe_essence_array)

	ADD_HASH_MAP(config, DT_ENEMY_FACTION_CONFIG, enemy_faction_config_array)
	ADD_HASH_MAP(config, DT_CHARRACTER_CLASS_CONFIG, character_class_config_array)
	ADD_HASH_MAP(config, DT_PARAM_ADJUST_CONFIG, param_adjust_config_array)
	ADD_HASH_MAP(config, DT_PLAYER_ACTION_INFO_CONFIG, player_action_info_config_array)
	ADD_HASH_MAP(essence, DT_TASKDICE_ESSENCE, taskdice_essence_array)

	ADD_HASH_MAP(essence, DT_TASKNORMALMATTER_ESSENCE, tasknormalmatter_essence_array);
	ADD_HASH_MAP(face, DT_FACE_FALING_ESSENCE, face_faling_essence_array)
	ADD_HASH_MAP(config, DT_PLAYER_LEVELEXP_CONFIG, player_levelexp_config_array)
	ADD_HASH_MAP(essence, DT_MINE_TYPE, mine_type_array)
	ADD_HASH_MAP(essence, DT_MINE_ESSENCE, mine_essence_array)

	ADD_HASH_MAP(essence, DT_NPC_IDENTIFY_SERVICE, npc_identify_service_array)
	ADD_HASH_MAP(essence, DT_FASHION_MAJOR_TYPE, fashion_major_type_array)
	ADD_HASH_MAP(essence, DT_FASHION_SUB_TYPE, fashion_sub_type_array)
	ADD_HASH_MAP(essence, DT_FASHION_ESSENCE, fashion_essence_array)

	ADD_HASH_MAP(essence, DT_FACETICKET_MAJOR_TYPE, faceticket_major_type_array)
	ADD_HASH_MAP(essence, DT_FACETICKET_SUB_TYPE, faceticket_sub_type_array)
	ADD_HASH_MAP(essence, DT_FACETICKET_ESSENCE, faceticket_essence_array)
	ADD_HASH_MAP(essence, DT_FACEPILL_MAJOR_TYPE, facepill_major_type_array)
	ADD_HASH_MAP(essence, DT_FACEPILL_SUB_TYPE, facepill_sub_type_array)

	ADD_HASH_MAP(essence, DT_FACEPILL_ESSENCE, facepill_essence_array)
	ADD_HASH_MAP(essence, DT_SUITE_ESSENCE, suite_essence_array);
	ADD_HASH_MAP(essence, DT_GM_GENERATOR_TYPE, gm_generator_type_array);
	ADD_HASH_MAP(essence, DT_GM_GENERATOR_ESSENCE, gm_generator_essence_array);
	ADD_HASH_MAP(essence, DT_PET_TYPE, pet_type_array);

	ADD_HASH_MAP(essence, DT_PET_ESSENCE, pet_essence_array);
	ADD_HASH_MAP(essence, DT_PET_EGG_ESSENCE, pet_egg_essence_array);
	ADD_HASH_MAP(essence, DT_PET_FOOD_ESSENCE, pet_food_essence_array);
	ADD_HASH_MAP(essence, DT_PET_FACETICKET_ESSENCE, pet_faceticket_essence_array);
	ADD_HASH_MAP(essence, DT_FIREWORKS_ESSENCE, fireworks_essence_array);

	ADD_HASH_MAP(essence, DT_WAR_TANKCALLIN_ESSENCE, war_tankcallin_essence_array);
	ADD_HASH_MAP(essence, DT_NPC_WAR_TOWERBUILD_SERVICE, npc_war_towerbuild_service_array);
	ADD_HASH_MAP(config, DT_PLAYER_SECONDLEVEL_CONFIG, player_secondlevel_config_array);
	ADD_HASH_MAP(essence, DT_NPC_RESETPROP_SERVICE, npc_resetprop_service_array);
	ADD_HASH_MAP(essence, DT_NPC_PETNAME_SERVICE, npc_petname_service_array);

	ADD_HASH_MAP(essence, DT_NPC_PETLEARNSKILL_SERVICE, npc_petlearnskill_service_array);
	ADD_HASH_MAP(essence, DT_NPC_PETFORGETSKILL_SERVICE, npc_petforgetskill_service_array);
	ADD_HASH_MAP(essence, DT_SKILLMATTER_ESSENCE, skillmatter_essence_array);
	ADD_HASH_MAP(essence, DT_REFINE_TICKET_ESSENCE, refine_ticket_essence_array);
	ADD_HASH_MAP(essence, DT_DESTROYING_ESSENCE, destroying_essence_array);

	ADD_HASH_MAP(essence, DT_NPC_EQUIPBIND_SERVICE, npc_equipbind_service_array);
	ADD_HASH_MAP(essence, DT_NPC_EQUIPDESTROY_SERVICE, npc_equipdestroy_service_array);
	ADD_HASH_MAP(essence, DT_NPC_EQUIPUNDESTROY_SERVICE, npc_equipundestroy_service_array);
	ADD_HASH_MAP(essence, DT_BIBLE_ESSENCE, bible_essence_array);
	ADD_HASH_MAP(essence, DT_SPEAKER_ESSENCE, speaker_essence_array);

	ADD_HASH_MAP(essence, DT_AUTOHP_ESSENCE, autohp_essence_array);
	ADD_HASH_MAP(essence, DT_AUTOMP_ESSENCE, automp_essence_array);
	ADD_HASH_MAP(essence, DT_DOUBLE_EXP_ESSENCE, double_exp_essence_array);
	ADD_HASH_MAP(essence, DT_TRANSMITSCROLL_ESSENCE, transmitscroll_essence_array);
	ADD_HASH_MAP(essence, DT_DYE_TICKET_ESSENCE, dye_ticket_essence_array);

	ADD_HASH_MAP(essence, DT_GOBLIN_ESSENCE, goblin_essence_array);
	ADD_HASH_MAP(essence, DT_GOBLIN_EQUIP_TYPE, goblin_equip_type_array);
	ADD_HASH_MAP(essence, DT_GOBLIN_EQUIP_ESSENCE, goblin_equip_essence_array);
	ADD_HASH_MAP(essence, DT_GOBLIN_EXPPILL_ESSENCE, goblin_exppill_essence_array);
	ADD_HASH_MAP(essence, DT_SELL_CERTIFICATE_ESSENCE, sell_certificate_essence_array);

	ADD_HASH_MAP(essence, DT_TARGET_ITEM_ESSENCE, target_item_essence_array);
	ADD_HASH_MAP(essence, DT_LOOK_INFO_ESSENCE, look_info_essence_array);
	ADD_HASH_MAP(config, DT_UPGRADE_PRODUCTION_CONFIG, upgrade_production_config_array);
	ADD_HASH_MAP(config, DT_ACC_STORAGE_BLACKLIST_CONFIG, acc_storage_blacklist_config_array);
	ADD_HASH_MAP(face,   DT_FACE_HAIR_TEXTURE_MAP, face_hair_texture_map_array);

	ADD_HASH_MAP(config, DT_MULTI_EXP_CONFIG, multi_exp_config_array);
	ADD_HASH_MAP(essence, DT_INC_SKILL_ABILITY_ESSENCE, inc_skill_ability_essence_array);
	ADD_HASH_MAP(config, DT_GOD_EVIL_CONVERT_CONFIG, god_evil_convert_config_array);
	ADD_HASH_MAP(config, DT_WEDDING_CONFIG, wedding_config_array);
	ADD_HASH_MAP(essence, DT_WEDDING_BOOKCARD_ESSENCE, wedding_bookcard_essence_array);

	ADD_HASH_MAP(essence, DT_WEDDING_INVITECARD_ESSENCE, wedding_invitecard_essence_array);
	ADD_HASH_MAP(essence, DT_SHARPENER_ESSENCE, sharpener_essence_array);
	ADD_HASH_MAP(face,	DT_FACE_THIRDEYE_ESSENCE, face_thirdeye_essence_array);
	ADD_HASH_MAP(config, DT_FACTION_FORTRESS_CONFIG, faction_fortress_config_array);	
	ADD_HASH_MAP(essence, DT_FACTION_BUILDING_SUB_TYPE, faction_building_sub_type_array);

	ADD_HASH_MAP(essence, DT_FACTION_BUILDING_ESSENCE, faction_building_essence_array);
	ADD_HASH_MAP(essence, DT_FACTION_MATERIAL_ESSENCE, faction_material_essence_array);
	ADD_HASH_MAP(essence, DT_CONGREGATE_ESSENCE, congregate_essence_array);

	ADD_HASH_MAP(recipe, DT_ENGRAVE_MAJOR_TYPE, engrave_major_type_array)
	ADD_HASH_MAP(recipe, DT_ENGRAVE_SUB_TYPE, engrave_sub_type_array)
	ADD_HASH_MAP(recipe, DT_ENGRAVE_ESSENCE, engrave_essence_array)
	ADD_HASH_MAP(essence, DT_NPC_ENGRAVE_SERVICE, npc_engrave_service_array);
	ADD_HASH_MAP(essence, DT_NPC_RANDPROP_SERVICE, npc_randprop_service_array);

	ADD_HASH_MAP(recipe, DT_RANDPROP_TYPE, randprop_type_array);
	ADD_HASH_MAP(recipe, DT_RANDPROP_ESSENCE, randprop_essence_array);
	ADD_HASH_MAP(config, DT_WIKI_TABOO_CONFIG, wiki_taboo_config_array);
	ADD_HASH_MAP(config, DT_FORCE_CONFIG, force_config_array);
	ADD_HASH_MAP(essence, DT_FORCE_TOKEN_ESSENCE, force_token_essence_array);

	ADD_HASH_MAP(essence, DT_NPC_FORCE_SERVICE, npc_force_service_array);
	ADD_HASH_MAP(config, DT_PLAYER_DEATH_DROP_CONFIG, player_death_drop_config_array);
	ADD_HASH_MAP(essence, DT_DYNSKILLEQUIP_ESSENCE, dynskillequip_essence_array);
	ADD_HASH_MAP(config, DT_CONSUME_POINTS_CONFIG, consume_points_config_array);
	ADD_HASH_MAP(config, DT_ONLINE_AWARDS_CONFIG, online_awards_config_array);
	
	ADD_HASH_MAP(config, DT_COUNTRY_CONFIG, country_config_array);
	ADD_HASH_MAP(config, DT_GM_ACTIVITY_CONFIG, gm_activity_config_array);
	ADD_HASH_MAP(config, DT_FASHION_WEAPON_CONFIG, fashion_weapon_config_array);
	ADD_HASH_MAP(config, DT_PET_EVOLVE_CONFIG, pet_evolve_config_array);
	ADD_HASH_MAP(config, DT_PET_EVOLVED_SKILL_CONFIG, pet_evolved_skill_config_array);

	ADD_HASH_MAP(essence, DT_MONEY_CONVERTIBLE_ESSENCE, money_convertible_essence_array);
	ADD_HASH_MAP(recipe, DT_STONE_CHANGE_RECIPE_TYPE, stone_change_recipe_type_array);
	ADD_HASH_MAP(recipe, DT_STONE_CHANGE_RECIPE, stone_change_recipe_array);
	ADD_HASH_MAP(config, DT_MERIDIAN_CONFIG, meridian_config_array);
	ADD_HASH_MAP(config, DT_PET_EVOLVED_SKILL_RAND_CONFIG, pet_evolved_skill_rand_config_array);

	ADD_HASH_MAP(config, DT_AUTOTASK_DISPLAY_CONFIG, autotask_display_config_array);
	ADD_HASH_MAP(config, DT_TOUCH_SHOP_CONFIG, touch_shop_config_array);
	ADD_HASH_MAP(config, DT_TITLE_CONFIG, title_config_array);
	ADD_HASH_MAP(config, DT_COMPLEX_TITLE_CONFIG, complex_title_config_array);
	ADD_HASH_MAP(essence, DT_MONSTER_SPIRIT_ESSENCE, monster_spirit_essence_array);

	ADD_HASH_MAP(config, DT_PLAYER_SPIRIT_CONFIG, player_spirit_config_array);
	ADD_HASH_MAP(config, DT_PLAYER_REINCARNATION_CONFIG, player_reincarnation_config_array);
	ADD_HASH_MAP(config, DT_HISTORY_STAGE_CONFIG, history_stage_config_array);	
	ADD_HASH_MAP(config, DT_HISTORY_ADVANCE_CONFIG, history_advance_config_array);
	ADD_HASH_MAP(config, DT_AUTOTEAM_CONFIG, autoteam_config_array);
	
	ADD_HASH_MAP(config, DT_PLAYER_REALM_CONFIG, player_realm_config_array);
	ADD_HASH_MAP(config, DT_CHARIOT_CONFIG, chariot_config_array);
	ADD_HASH_MAP(config, DT_CHARIOT_WAR_CONFIG, chariot_war_config_array);
	ADD_HASH_MAP(config, DT_POKER_LEVELEXP_CONFIG, poker_levelexp_config_array);
	ADD_HASH_MAP(essence, DT_POKER_SUITE_ESSENCE, poker_suite_essence_array);

	ADD_HASH_MAP(essence, DT_POKER_DICE_ESSENCE, poker_dice_essence_array);
	ADD_HASH_MAP(essence, DT_POKER_SUB_TYPE, poker_sub_type_array);
	ADD_HASH_MAP(essence, DT_POKER_ESSENCE, poker_essence_array);
	ADD_HASH_MAP(config, DT_TOKEN_SHOP_CONFIG, token_shop_config_array);
	ADD_HASH_MAP(essence, DT_SHOP_TOKEN_ESSENCE, shop_token_essence_array);

	ADD_HASH_MAP(config, DT_GT_CONFIG, gt_config_array);
	ADD_HASH_MAP(config, DT_RAND_SHOP_CONFIG, rand_shop_config_array);
	ADD_HASH_MAP(config, DT_PROFIT_TIME_CONFIG, profit_time_config_array);
	ADD_HASH_MAP(config, DT_FACTION_PVP_CONFIG, faction_pvp_config_array);
	ADD_HASH_MAP(essence, DT_UNIVERSAL_TOKEN_ESSENCE, universal_token_essence_array);
	
	ADD_HASH_MAP(config, DT_TASK_LIST_CONFIG, task_list_config_array);
	ADD_HASH_MAP(config, DT_TASK_DICE_BY_WEIGHT_CONFIG, task_dice_by_weight_config_array);
	ADD_HASH_MAP(essence, DT_FASHION_SUITE_ESSENCE, fashion_suite_essence_array);
	ADD_HASH_MAP(config, DT_FASHION_BEST_COLOR_CONFIG, fashion_best_color_config_array);
	ADD_HASH_MAP(config, DT_SIGN_AWARD_CONFIG, sign_award_config_array);

#undef ADD_HASH_MAP
}

int elementdataman::save_data(const char * pathname)
{
	FILE * file;
	file = fopen(pathname, "wb");
	if(file == NULL)		return -1;

	unsigned int version = ELEMENTDATA_VERSION;
	fwrite(&version, sizeof(unsigned int), 1, file);

	time_t t = time(NULL);
	fwrite(&t, sizeof(time_t), 1, file);

	equipment_addon_array.save(file);
	weapon_major_type_array.save(file);
	weapon_sub_type_array.save(file);
	weapon_essence_array.save(file);
	armor_major_type_array.save(file);
	
	armor_sub_type_array.save(file);
	armor_essence_array.save(file);
	decoration_major_type_array.save(file);
	decoration_sub_type_array.save(file);
	decoration_essence_array.save(file);
	
	medicine_major_type_array.save(file);
	medicine_sub_type_array.save(file);
	medicine_essence_array.save(file);
	material_major_type_array.save(file);
	material_sub_type_array.save(file);

	material_essence_array.save(file);
	damagerune_sub_type_array.save(file);
	damagerune_essence_array.save(file);
	armorrune_sub_type_array.save(file);
	armorrune_essence_array.save(file);

#ifdef _WINDOWS 
#ifdef _MD5_CHECK
	extern void MyGetComputerName(char * szBuffer, unsigned long * pLen);
	extern void MySleep();
	// Get the computer's name of the exporter
	unsigned int tag = 0xab7689dd;
	fwrite(&tag, sizeof(unsigned int), 1, file);
	char cname[256];
	unsigned long len = 256;
	MyGetComputerName(cname, &len);
	fwrite(&len, sizeof(unsigned int), 1, file);
	char c[4] = {(char)0xab, (char)0x76, (char)0x89, (char)0xdd};
	for(unsigned int l=0; l<len; l++)
		cname[l] = cname[l] ^ c[l % 4];
	fwrite(cname, len, 1, file);
	MySleep();
	time_t ttt = time(NULL);
	fwrite(&ttt, sizeof(time_t), 1, file);
#endif
#endif

	skilltome_sub_type_array.save(file);
	skilltome_essence_array.save(file);
	flysword_essence_array.save(file);
	wingmanwing_essence_array.save(file);
	townscroll_essence_array.save(file);

	unionscroll_essence_array.save(file);
	revivescroll_essence_array.save(file);
	element_essence_array.save(file);	
	taskmatter_essence_array.save(file);
	tossmatter_essence_array.save(file);

	projectile_type_array.save(file);
	projectile_essence_array.save(file);
	quiver_sub_type_array.save(file);
	quiver_essence_array.save(file);
	stone_sub_type_array.save(file);

	stone_essence_array.save(file);
	monster_addon_array.save(file);	
	monster_type_array.save(file);
	monster_essence_array.save(file);
	
	npc_talk_service_array.save(file);
	npc_sell_service_array.save(file);
	npc_buy_service_array.save(file);
	npc_repair_service_array.save(file);
	npc_install_service_array.save(file);
	npc_uninstall_service_array.save(file);
	npc_task_in_service_array.save(file);
	npc_task_out_service_array.save(file);
	npc_task_matter_service_array.save(file);
	npc_skill_service_array.save(file);
	npc_heal_service_array.save(file);
	npc_transmit_service_array.save(file);
	npc_transport_service_array.save(file);
	npc_proxy_service_array.save(file);
	npc_storage_service_array.save(file);
	npc_make_service_array.save(file);
	npc_decompose_service_array.save(file);

	npc_type_array.save(file);
	npc_essence_array.save(file);

	size_t sz = talk_proc_array.size();
	fwrite(&sz, sizeof(size_t), 1, file);
	for(size_t i=0; i<sz; i++)
		talk_proc_array[i]->save(file);
	
	face_texture_essence_array.save(file);
	face_shape_essence_array.save(file);
	face_emotion_type_array.save(file);
	face_expression_essence_array.save(file);
	face_hair_essence_array.save(file);
	face_moustache_essence_array.save(file);
	colorpicker_essence_array.save(file);
	customizedata_essence_array.save(file);

	recipe_major_type_array.save(file);
	recipe_sub_type_array.save(file);
	recipe_essence_array.save(file);

	enemy_faction_config_array.save(file);
	character_class_config_array.save(file);
	param_adjust_config_array.save(file);
	player_action_info_config_array.save(file);
	taskdice_essence_array.save(file);

	tasknormalmatter_essence_array.save(file);
	face_faling_essence_array.save(file);
	player_levelexp_config_array.save(file);
	mine_type_array.save(file);
	mine_essence_array.save(file);

	npc_identify_service_array.save(file);
	fashion_major_type_array.save(file);
	fashion_sub_type_array.save(file);
	fashion_essence_array.save(file);

	faceticket_major_type_array.save(file);
	faceticket_sub_type_array.save(file);
	faceticket_essence_array.save(file);
	facepill_major_type_array.save(file);
	facepill_sub_type_array.save(file);
	facepill_essence_array.save(file);

	suite_essence_array.save(file);
	gm_generator_type_array.save(file);
	gm_generator_essence_array.save(file);
	pet_type_array.save(file);
	pet_essence_array.save(file);

	pet_egg_essence_array.save(file);
	pet_food_essence_array.save(file);
	pet_faceticket_essence_array.save(file);
	fireworks_essence_array.save(file);
	war_tankcallin_essence_array.save(file);

#ifdef _WINDOWS 
#ifdef _MD5_CHECK
	extern void MyGetHardInfo(unsigned char * buffer, unsigned long * len);
	unsigned int tag2 = 0xee35679f;
	fwrite(&tag2, sizeof(unsigned int), 1, file);
	unsigned char buffer[1024];
	unsigned long buffer_len = 1024;
	MyGetHardInfo(buffer, &buffer_len);
	fwrite(&buffer_len, sizeof(unsigned int), 1, file);
	fwrite(buffer, buffer_len, 1, file);
#endif
#endif
	
	npc_war_towerbuild_service_array.save(file);
	player_secondlevel_config_array.save(file);
	npc_resetprop_service_array.save(file);
	npc_petname_service_array.save(file);
	npc_petlearnskill_service_array.save(file);

	npc_petforgetskill_service_array.save(file);
	skillmatter_essence_array.save(file);
	refine_ticket_essence_array.save(file);
	destroying_essence_array.save(file);
	npc_equipbind_service_array.save(file);

	npc_equipdestroy_service_array.save(file);
	npc_equipundestroy_service_array.save(file);
	bible_essence_array.save(file);
	speaker_essence_array.save(file);
	autohp_essence_array.save(file);

	automp_essence_array.save(file);
	double_exp_essence_array.save(file);
	transmitscroll_essence_array.save(file);
	dye_ticket_essence_array.save(file);

	goblin_essence_array.save(file);
	goblin_equip_type_array.save(file);
	goblin_equip_essence_array.save(file);
	goblin_exppill_essence_array.save(file);
	sell_certificate_essence_array.save(file);

	target_item_essence_array.save(file);
	look_info_essence_array.save(file);
	upgrade_production_config_array.save(file);
	acc_storage_blacklist_config_array.save(file);
	face_hair_texture_map_array.save(file);

	multi_exp_config_array.save(file);	
	inc_skill_ability_essence_array.save(file);
	god_evil_convert_config_array.save(file);
	wedding_config_array.save(file);		
	wedding_bookcard_essence_array.save(file);

	wedding_invitecard_essence_array.save(file);
	sharpener_essence_array.save(file);
	face_thirdeye_essence_array.save(file);
	faction_fortress_config_array.save(file);
	faction_building_sub_type_array.save(file);

	faction_building_essence_array.save(file);
	faction_material_essence_array.save(file);
	congregate_essence_array.save(file);
	
	engrave_major_type_array.save(file);
	engrave_sub_type_array.save(file);
	engrave_essence_array.save(file);
	npc_engrave_service_array.save(file);
	npc_randprop_service_array.save(file);
	
	randprop_type_array.save(file);
	randprop_essence_array.save(file);
	wiki_taboo_config_array.save(file);
	force_config_array.save(file);
	force_token_essence_array.save(file);

	npc_force_service_array.save(file);
	player_death_drop_config_array.save(file);
	dynskillequip_essence_array.save(file);
	consume_points_config_array.save(file);	
	online_awards_config_array.save(file);

	country_config_array.save(file);
	gm_activity_config_array.save(file);
	fashion_weapon_config_array.save(file);
	pet_evolve_config_array.save(file);
	pet_evolved_skill_config_array.save(file);

	money_convertible_essence_array.save(file);
	stone_change_recipe_type_array.save(file);
	stone_change_recipe_array.save(file);
	meridian_config_array.save(file);
	pet_evolved_skill_rand_config_array.save(file);

	autotask_display_config_array.save(file);
	touch_shop_config_array.save(file);
	title_config_array.save(file);
	complex_title_config_array.save(file);
	monster_spirit_essence_array.save(file);

	player_spirit_config_array.save(file);
	player_reincarnation_config_array.save(file);
	history_stage_config_array.save(file);
	history_advance_config_array.save(file);
	autoteam_config_array.save(file);

	player_realm_config_array.save(file);
	chariot_config_array.save(file);
	chariot_war_config_array.save(file);
	poker_levelexp_config_array.save(file);
	poker_suite_essence_array.save(file);

	poker_dice_essence_array.save(file);
	poker_sub_type_array.save(file);
	poker_essence_array.save(file);	
	token_shop_config_array.save(file);
	shop_token_essence_array.save(file);

	gt_config_array.save(file);
	rand_shop_config_array.save(file);
	profit_time_config_array.save(file);
	faction_pvp_config_array.save(file);
	universal_token_essence_array.save(file);

	task_list_config_array.save(file);	
	task_dice_by_weight_config_array.save(file);
	fashion_suite_essence_array.save(file);
	fashion_best_color_config_array.save(file);
	sign_award_config_array.save(file);

	fclose(file);
	return 0;
}

int elementdataman::load_data(const char * pathname)
{
	FILE * file;
	file = fopen(pathname, "rb");
	if(file == NULL)		return -1;

	unsigned int version = 0;
	fread(&version, sizeof(unsigned int), 1, file);
	if( version != ELEMENTDATA_VERSION )
		return -1;

	time_t t;
	fread(&t, sizeof(time_t), 1, file);

	if(equipment_addon_array.load(file) != 0) return -1;
	if(weapon_major_type_array.load(file) != 0) return -1;
	if(weapon_sub_type_array.load(file) != 0) return -1;
	if(weapon_essence_array.load(file) != 0) return -1;
	if(armor_major_type_array.load(file) != 0) return -1;
	
	if(armor_sub_type_array.load(file) != 0) return -1;
	if(armor_essence_array.load(file) != 0) return -1;
	if(decoration_major_type_array.load(file) != 0) return -1;
	if(decoration_sub_type_array.load(file) != 0) return -1;
	if(decoration_essence_array.load(file) != 0) return -1;
	
	if(medicine_major_type_array.load(file) != 0) return -1;
	if(medicine_sub_type_array.load(file) != 0) return -1;
	if(medicine_essence_array.load(file) != 0) return -1;
	if(material_major_type_array.load(file) != 0) return -1;
	if(material_sub_type_array.load(file) != 0) return -1;

	if(material_essence_array.load(file) != 0) return -1;
	if(damagerune_sub_type_array.load(file) != 0) return -1;
	if(damagerune_essence_array.load(file) != 0) return -1;
	if(armorrune_sub_type_array.load(file) != 0) return -1;
	if(armorrune_essence_array.load(file) != 0) return -1;

	// skip the computer's name of the exporter
	unsigned int tag;
	fread(&tag, sizeof(unsigned int), 1, file);
	char buffer[1024];
	unsigned int len;
	fread(&len, sizeof(unsigned int), 1, file);
	fread(buffer, len, 1, file);
	fread(&t, sizeof(time_t), 1, file);
	
	if(skilltome_sub_type_array.load(file) != 0) return -1;
	if(skilltome_essence_array.load(file) != 0) return -1;
	if(flysword_essence_array.load(file) != 0) return -1;
	if(wingmanwing_essence_array.load(file) != 0) return -1;
	if(townscroll_essence_array.load(file) != 0) return -1;

	if(unionscroll_essence_array.load(file) != 0) return -1;
	if(revivescroll_essence_array.load(file) != 0) return -1;
	if(element_essence_array.load(file) != 0) return -1;	
	if(taskmatter_essence_array.load(file) != 0) return -1;
	if(tossmatter_essence_array.load(file) != 0) return -1;

	if(projectile_type_array.load(file) != 0) return -1;
	if(projectile_essence_array.load(file) != 0) return -1;
	if(quiver_sub_type_array.load(file) != 0) return -1;
	if(quiver_essence_array.load(file) != 0) return -1;
	if(stone_sub_type_array.load(file) != 0) return -1;

	if(stone_essence_array.load(file) != 0) return -1;
	if(monster_addon_array.load(file) != 0) return -1;	
	if(monster_type_array.load(file) != 0) return -1;
	if(monster_essence_array.load(file) != 0) return -1;
	
	if(npc_talk_service_array.load(file) != 0) return -1;
	if(npc_sell_service_array.load(file) != 0) return -1;
	if(npc_buy_service_array.load(file) != 0) return -1;
	if(npc_repair_service_array.load(file) != 0) return -1;
	if(npc_install_service_array.load(file) != 0) return -1;
	if(npc_uninstall_service_array.load(file) != 0) return -1;
	if(npc_task_in_service_array.load(file) != 0) return -1;
	if(npc_task_out_service_array.load(file) != 0) return -1;
	if(npc_task_matter_service_array.load(file) != 0) return -1;
	if(npc_skill_service_array.load(file) != 0) return -1;
	if(npc_heal_service_array.load(file) != 0) return -1;
	if(npc_transmit_service_array.load(file) != 0) return -1;

	if(npc_transport_service_array.load(file) != 0) return -1;
	if(npc_proxy_service_array.load(file) != 0) return -1;
	if(npc_storage_service_array.load(file) != 0) return -1;
	if(npc_make_service_array.load(file) != 0) return -1;
	if(npc_decompose_service_array.load(file) != 0) return -1;
	
	if(npc_type_array.load(file) != 0) return -1;
	if(npc_essence_array.load(file) != 0) return -1;

	size_t sz = 0;
	if(fread(&sz, sizeof(size_t), 1, file) != 1)	return -1;
	for(size_t i=0; i<sz; i++)
	{
		talk_proc * tp = new talk_proc;
		tp->load(file);
		add_structure(tp->id_talk, tp);
	}
	
	if(face_texture_essence_array.load(file) != 0) return -1;
	if(face_shape_essence_array.load(file) != 0) return -1;
	if(face_emotion_type_array.load(file) != 0) return -1;
	if(face_expression_essence_array.load(file) != 0) return -1;
	if(face_hair_essence_array.load(file) != 0) return -1;
	if(face_moustache_essence_array.load(file) != 0) return -1;
	if(colorpicker_essence_array.load(file) != 0) return -1;
	if(customizedata_essence_array.load(file) != 0) return -1;

	if(recipe_major_type_array.load(file) != 0) return -1;
	if(recipe_sub_type_array.load(file) != 0) return -1;
	if(recipe_essence_array.load(file) != 0) return -1;

	if(enemy_faction_config_array.load(file) != 0) return -1;
	if(character_class_config_array.load(file) != 0) return -1;
	if(param_adjust_config_array.load(file) != 0) return -1;
	if(player_action_info_config_array.load(file) != 0) return -1;
	if(taskdice_essence_array.load(file) != 0) return -1;

	if(tasknormalmatter_essence_array.load(file) != 0) return -1;
	if(face_faling_essence_array.load(file) != 0) return -1;
	if(player_levelexp_config_array.load(file) != 0) return -1;
	if(mine_type_array.load(file) != 0) return -1;
	if(mine_essence_array.load(file) != 0) return -1;

	if(npc_identify_service_array.load(file) != 0) return -1;
	if(fashion_major_type_array.load(file) != 0) return -1;
	if(fashion_sub_type_array.load(file) != 0) return -1;
	if(fashion_essence_array.load(file) != 0) return -1;

	if(faceticket_major_type_array.load(file) != 0) return -1;
	if(faceticket_sub_type_array.load(file) != 0) return -1;
	if(faceticket_essence_array.load(file) != 0) return -1;
	if(facepill_major_type_array.load(file) != 0) return -1;
	if(facepill_sub_type_array.load(file) != 0) return -1;

	if(facepill_essence_array.load(file) != 0) return -1;
	if(suite_essence_array.load(file) != 0) return -1;
	if(gm_generator_type_array.load(file) != 0) return -1;
	if(gm_generator_essence_array.load(file) != 0) return -1;
	if(pet_type_array.load(file) != 0 ) return -1;

	if(pet_essence_array.load(file) != 0) return -1;
	if(pet_egg_essence_array.load(file) != 0) return -1;
	if(pet_food_essence_array.load(file) != 0) return -1;
	if(pet_faceticket_essence_array.load(file) != 0) return -1;
	if(fireworks_essence_array.load(file) != 0) return -1;

	if(war_tankcallin_essence_array.load(file) != 0) return -1;

	fread(&tag, sizeof(unsigned int), 1, file);
	fread(&len, sizeof(unsigned int), 1, file);
	fread(buffer, len, 1, file);

	if(npc_war_towerbuild_service_array.load(file) != 0) return -1;
	if(player_secondlevel_config_array.load(file) != 0) return -1;
	if(npc_resetprop_service_array.load(file) != 0) return -1;
	if(npc_petname_service_array.load(file) != 0) return -1;

	if(npc_petlearnskill_service_array.load(file) != 0) return -1;
	if(npc_petforgetskill_service_array.load(file) != 0) return -1;
	if(skillmatter_essence_array.load(file) != 0) return -1;
	if(refine_ticket_essence_array.load(file) != 0) return -1;
	if(destroying_essence_array.load(file) != 0) return -1;

	if(npc_equipbind_service_array.load(file) != 0) return -1;
	if(npc_equipdestroy_service_array.load(file) != 0) return -1;
	if(npc_equipundestroy_service_array.load(file) != 0) return -1;
	if(bible_essence_array.load(file) != 0) return -1;
	if(speaker_essence_array.load(file) != 0) return -1;

	if(autohp_essence_array.load(file) != 0) return -1;
	if(automp_essence_array.load(file) != 0) return -1;
	if(double_exp_essence_array.load(file) != 0) return -1;
	if(transmitscroll_essence_array.load(file) != 0) return -1;
	if(dye_ticket_essence_array.load(file) != 0) return -1;

	if(goblin_essence_array.load(file) != 0) return -1;
	if(goblin_equip_type_array.load(file) != 0) return -1;
	if(goblin_equip_essence_array.load(file) != 0) return -1;
	if(goblin_exppill_essence_array.load(file) != 0) return -1;
	if(sell_certificate_essence_array.load(file) != 0) return -1;

	if(target_item_essence_array.load(file) != 0) return -1;
	if(look_info_essence_array.load(file) != 0) return -1;
	if(upgrade_production_config_array.load(file) != 0) return -1;
	if(acc_storage_blacklist_config_array.load(file) != 0) return -1;
	if(face_hair_texture_map_array.load(file) != 0) return -1;

	if(multi_exp_config_array.load(file) != 0) return -1;
	if(inc_skill_ability_essence_array.load(file) != 0) return -1;
	if(god_evil_convert_config_array.load(file) != 0) return -1;
	if(wedding_config_array.load(file) != 0) return -1;
	if(wedding_bookcard_essence_array.load(file) != 0) return -1;

	if(wedding_invitecard_essence_array.load(file) != 0) return -1;
	if(sharpener_essence_array.load(file) != 0) return -1;
	if(face_thirdeye_essence_array.load(file) != 0) return -1;
	if(faction_fortress_config_array.load(file) != 0) return -1;
	if(faction_building_sub_type_array.load(file) != 0) return -1;

	if(faction_building_essence_array.load(file) != 0) return -1;
	if(faction_material_essence_array.load(file) != 0) return -1;
	if(congregate_essence_array.load(file) != 0) return -1;	
	
	if(engrave_major_type_array.load(file) != 0) return -1;
	if(engrave_sub_type_array.load(file) != 0) return -1;
	if(engrave_essence_array.load(file) != 0) return -1;
	if(npc_engrave_service_array.load(file) != 0) return -1;
	if(npc_randprop_service_array.load(file) != 0) return -1;
	
	if(randprop_type_array.load(file) != 0) return -1;
	if(randprop_essence_array.load(file) != 0) return -1;
	if(wiki_taboo_config_array.load(file) != 0) return -1;
	if(force_config_array.load(file) != 0) return -1;
	if(force_token_essence_array.load(file) != 0) return -1;

	if(npc_force_service_array.load(file) != 0) return -1;
	if(player_death_drop_config_array.load(file) != 0) return -1;
	if(dynskillequip_essence_array.load(file) != 0) return -1;
	if(consume_points_config_array.load(file) != 0) return -1;
	if(online_awards_config_array.load(file) != 0) return -1;
	
	if(country_config_array.load(file) != 0) return -1;
	if(gm_activity_config_array.load(file) != 0) return -1;
	if(fashion_weapon_config_array.load(file) != 0) return -1;
	if(pet_evolve_config_array.load(file) != 0) return -1;
	if(pet_evolved_skill_config_array.load(file) != 0) return -1;

	if(money_convertible_essence_array.load(file) != 0) return -1;
	if(stone_change_recipe_type_array.load(file) != 0) return -1;
	if(stone_change_recipe_array.load(file) != 0) return -1;
	if(meridian_config_array.load(file) != 0) return -1;
	if(pet_evolved_skill_rand_config_array.load(file) != 0) return -1;

	if(autotask_display_config_array.load(file) != 0) return -1;
	if(touch_shop_config_array.load(file) != 0) return -1;
	if(title_config_array.load(file) != 0) return -1;
	if(complex_title_config_array.load(file) != 0) return -1;
	if(monster_spirit_essence_array.load(file) != 0) return -1;

	if(player_spirit_config_array.load(file) != 0) return -1;
	if(player_reincarnation_config_array.load(file) != 0) return -1;
	if(history_stage_config_array.load(file) != 0) return -1;
	if(history_advance_config_array.load(file) != 0) return -1;
	if(autoteam_config_array.load(file) != 0) return -1;

	if(player_realm_config_array.load(file) != 0) return -1;
	if(chariot_config_array.load(file) != 0) return -1;
	if(chariot_war_config_array.load(file) != 0) return -1;
	if(poker_levelexp_config_array.load(file) != 0) return -1;
	if(poker_suite_essence_array.load(file) != 0) return -1;

	if(poker_dice_essence_array.load(file) != 0) return -1;
	if(poker_sub_type_array.load(file) != 0) return -1;
	if(poker_essence_array.load(file) != 0) return -1;
	if(token_shop_config_array.load(file) != 0) return -1;
	if(shop_token_essence_array.load(file) != 0) return -1;

	if(gt_config_array.load(file) != 0) return -1;
	if(rand_shop_config_array.load(file) != 0) return -1;
	if(profit_time_config_array.load(file) != 0) return -1;
	if(faction_pvp_config_array.load(file) != 0) return -1;
	if(universal_token_essence_array.load(file) != 0) return -1;

	if(task_list_config_array.load(file) != 0) return -1;
	if(task_dice_by_weight_config_array.load(file) != 0) return -1;
	if(fashion_suite_essence_array.load(file) != 0) return -1;
	if(fashion_best_color_config_array.load(file)!=0) return -1;
	if(sign_award_config_array.load(file)!=0) return -1;

	setup_hash_map();

	fclose(file);	
	return 0;	
}

