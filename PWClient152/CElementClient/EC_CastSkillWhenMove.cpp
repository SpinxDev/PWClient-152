// Filename	: EC_CastSkillWhenMove.cpp
// Creator	: Xu Wenbin
// Date		: 2014/8/1

#include "EC_CastSkillWhenMove.h"
#include "EC_RoleTypes.h"
#include "ElementSkill.h"
#include "EC_Player.h"
#include "EC_Global.h"
#include "EC_Resource.h"

#include <algorithm>

//	static

//	胧族
//	标准男：UpperBody
static const char *s_szOboroMaleStandardUpperBodyBonesName[] = {"Bip01 shangshen",
"Bip01 Spine",
"Bip01 Spine1",
"Bip01 Spine2",
"Bip01 Neck",
"Bip01 Head",
"Bip01 L Clavicle",
"Bip01 L UpperArm",
"Bip01 L Forearm",
"Bip01 L Hand",
"Bip01 L Finger0",
"Bip01 L Finger01",
"Bip01 L Finger1",
"Bip01 L Finger11",
"Bip01 R Clavicle",
"Bip01 R UpperArm",
"Bip01 R Forearm",
"Bip01 R Hand",
"Bip01 R Finger0",
"Bip01 R Finger01",
"Bip01 R Finger1",
"Bip01 R Finger11",
"Bone05",
};
//	标准男：LowerBody
static const char *s_szOboroMaleStandardLowerBodyBonesName[] = {"Bip01",
"Bip01 Pelvis",
"Bip01 xiashen",
"Bip01 L Thigh",
"Bip01 L Calf",
"Bip01 L Foot",
"Bip01 L Toe0",
"Bip01 R Thigh",
"Bip01 R Calf",
"Bip01 R Foot",
"Bip01 R Toe0",
"Bone01",
"Bone02",
"Bone03",
"Bone04",
};

//	职业变身男：UpperBody
static const char *s_szOboroMaleShape1UpperBodyBonesName[] = {"Bip01 shangshen",
"Bip01 Spine",
"Bip01 Spine1",
"Bip01 Spine2",
"Bip01 Neck",
"Bip01 Head",
"Bip01 L Clavicle",
"Bip01 L UpperArm",
"Bip01 L Forearm",
"Bip01 L Hand",
"Bip01 L Finger0",
"Bip01 L Finger01",
"Bip01 L Finger1",
"Bip01 L Finger11",
"Bip01 R Clavicle",
"Bip01 R UpperArm",
"Bip01 R Forearm",
"Bip01 R Hand",
"Bip01 R Finger0",
"Bip01 R Finger01",
"Bip01 R Finger1",
"Bip01 R Finger11",
"Bone05",
"Bone13",
"Bone14",
"Bone15",
"Bone16",
"Bone17",
"Bone18",
};
//	职业变身男：LowerBody
static const char *s_szOboroMaleShape1LowerBodyBonesName[] = {"Bip01",
"Bip01 Pelvis",
"Bip01 xiashen",
"Bip01 L Thigh",
"Bip01 L Calf",
"Bip01 L Foot",
"Bip01 L Toe0",
"Bip01 R Thigh",
"Bip01 R Calf",
"Bip01 R Foot",
"Bip01 R Toe0",
"Bone06",
"Bone07",
"Bone08",
"Bone09",
"Bone10",
"Bone11",
"Bone12",
"Bone19",
"Bone20",
"Bone21",
};

//	标准女：UpperBody
static const char *s_szOboroFemaleStandardUpperBodyBonesName[] = {"Bip01 Spine shangshen",
"Bip01 Spine",
"Bip01 Spine1",
"Bip01 Spine2",
"Bip01 Neck",
"Bip01 Head",
"Bip01 Ponytail1",
"Bip01 Ponytail11",
"Bip01 Ponytail12",
"Bip01 L Clavicle",
"Bip01 L UpperArm",
"Bip01 L Forearm",
"Bip01 L Hand",
"Bip01 L Finger0",
"Bip01 L Finger01",
"Bip01 L Finger1",
"Bip01 L Finger11",
"Bip01 R Clavicle",
"Bip01 R UpperArm",
"Bip01 R Forearm",
"Bip01 R Hand",
"Bip01 R Finger0",
"Bip01 R Finger01",
"Bip01 R Finger1",
"Bip01 R Finger11",
"Bone05",
"Bone01",
};
//	标准女：LowerBody
static const char *s_szOboroFemaleStandardLowerBodyBonesName[] = {"Bip01",
"Bip01 Pelvis",
"Bip01 Spine xiashen",
"Bip01 L Thigh",
"Bip01 L Calf",
"Bip01 L Foot",
"Bip01 L Toe0",
"Bip01 R Thigh",
"Bip01 R Calf",
"Bip01 R Foot",
"Bip01 R Toe0",
"Bone08",
"Bone09",
"Bone10",
"Bone02",
"Bone03",
"Bone04",
};

//	职业变身女：UpperBody
static const char *s_szOboroFemaleShape1UpperBodyBonesName[] = {"Bip01 Spine shangshen",
"Bip01 Spine",
"Bip01 Spine1",
"Bip01 Spine2",
"Bip01 Neck",
"Bip01 Head",
"Bip01 Ponytail1",
"Bip01 Ponytail11",
"Bip01 Ponytail12",
"Bip01 L Clavicle",
"Bip01 L UpperArm",
"Bip01 L Forearm",
"Bip01 L Hand",
"Bip01 L Finger0",
"Bip01 L Finger01",
"Bip01 L Finger1",
"Bip01 L Finger11",
"Bip01 R Clavicle",
"Bip01 R UpperArm",
"Bip01 R Forearm",
"Bip01 R Hand",
"Bip01 R Finger0",
"Bip01 R Finger01",
"Bip01 R Finger1",
"Bip01 R Finger11",
"Bone05",
"Bone01",
"Bone14",
"Bone15",
"Bone16",
"Bone17",
"Bone18",
"Bone19",
"Bone20",
"Bone21",
"Bone22",
"Bone23",
"Bone24",
};
//	职业变身女：LowerBody
static const char *s_szOboroFemaleShape1LowerBodyBonesName[] = {"Bip01",
"Bip01 Pelvis",
"Bip01 Spine xiashen",
"Bip01 L Thigh",
"Bip01 L Calf",
"Bip01 L Foot",
"Bip01 L Toe0",
"Bip01 R Thigh",
"Bip01 R Calf",
"Bip01 R Foot",
"Bip01 R Toe0",
"Bone08",
"Bone09",
"Bone10",
"Bone02",
"Bone03",
"Bone04",
"Bone06",
"Bone07",
"Bone11",
"Bone12",
"Bone13",
};

//	class CECCastSkillWhenMove::PlayerInfo
CECCastSkillWhenMove::PlayerInfo::PlayerInfo(int profession, int gender, int shapeType, int shapeID)
: profession(profession)
, gender(gender)
, shapeType(shapeType)
, shapeID(shapeID)
{
}

CECCastSkillWhenMove::PlayerInfo::PlayerInfo(CECPlayer *pPlayer)
{
	profession = pPlayer->GetProfession();
	gender = pPlayer->GetGender();
	shapeType = pPlayer->GetShapeType();
	shapeID = pPlayer->GetShapeID();
}

//	class CECCastSkillWhenMove
CECCastSkillWhenMove::CECCastSkillWhenMove()
{
}

CECCastSkillWhenMove & CECCastSkillWhenMove::Instance(){
	static CECCastSkillWhenMove s_instance;
	return s_instance;
}

bool CECCastSkillWhenMove::HasModelSupport(const PlayerInfo &player)const{
	if (player.profession == PROF_YUEXIAN){
		switch (player.shapeType){
		case PLAYERMODEL_MAJOR:			//	基本模型
			return true;
		case PLAYERMODEL_PROFESSION:	//	职业变身模型
			if (player.gender == GENDER_MALE){
				return player.shapeID == RES_MOD_YUEXIAN_RESHAPE_M;	//	男职业变身
			}else{
				return player.shapeID == RES_MOD_YUEXIAN_RESHAPE_F;	//	女职业变身
			}
		}
	}
	//	支持移动施法的ecm模型
	//	注：夜影 PROF_YEYING 模型也支持，但由于没有任何移动施法技能，故此处屏蔽，减少意外情况的发生
	return false;
}

bool CECCastSkillWhenMove::IsSkillSupported(int idSkill, const PlayerInfo &player)const{	//	支持移动施法的技能ID
	return GNET::ElementSkill::IsMovingSkill(idSkill);
}

bool CECCastSkillWhenMove::HasActionSupport(int idSkill, const PlayerInfo &player)const{	//	可移动施法的技能动作（即可叠加移动与施法动作）
	if (HasModelSupport(player)){
		const int SKILL_COUNT = 19;
		static const int s_skills[SKILL_COUNT] = {2571, 2572, 2579, 2580, 2588, 2591,		//	2571朔光咏 2572云起咏 2579月诗引 2580冰轮引 2588霜雷术 2591月华乱舞 
			2627, 2619, 2620, 2621, 2622, 2787, 2788, 2789, 2790, 2805, 2806, 2811, 2812,	//	2627祭月礼 2619真·朔光咏 2620狂·朔光咏 2621真·云起咏 2622狂·云起咏 2787真·月诗引 2788狂·月诗引 2789真·冰轮引 2790狂·冰轮引 2805真·霜雷术 2806狂·霜雷术 2811真·月华乱舞 2812狂·月华乱舞
		};
		const int *begin = s_skills;
		const int *end = s_skills + SKILL_COUNT;
		return std::find(begin, end, idSkill) != end;
	}
	return false;
}

bool CECCastSkillWhenMove::GetUpperBodyBonesName(const PlayerInfo &player, int &bonesNameCount, const char ** & szBonesName)const{
	bool result(false);
	if (HasModelSupport(player)){
		switch (player.shapeType){
		case PLAYERMODEL_MAJOR:
			if (GENDER_MALE == player.gender){
				szBonesName = s_szOboroMaleStandardUpperBodyBonesName;
				bonesNameCount = ARRAY_SIZE(s_szOboroMaleStandardUpperBodyBonesName);
			}else{
				szBonesName = s_szOboroFemaleStandardUpperBodyBonesName;
				bonesNameCount = ARRAY_SIZE(s_szOboroFemaleStandardUpperBodyBonesName);
			}
			break;
		case PLAYERMODEL_PROFESSION:
			if (GENDER_MALE == player.gender){
				if (RES_MOD_YUEXIAN_RESHAPE_M == player.shapeID){
					szBonesName = s_szOboroMaleShape1UpperBodyBonesName;
					bonesNameCount = ARRAY_SIZE(s_szOboroMaleShape1UpperBodyBonesName);
				}
			}else{
				if (RES_MOD_YUEXIAN_RESHAPE_F == player.shapeID){
					szBonesName = s_szOboroFemaleShape1UpperBodyBonesName;
					bonesNameCount = ARRAY_SIZE(s_szOboroFemaleShape1UpperBodyBonesName);
				}
			}
			break;
		default:
			ASSERT(false);
			break;
		}
		result = true;
	}
	return true;
}

bool CECCastSkillWhenMove::GetLowerBodyBonesName(const PlayerInfo &player, int &bonesNameCount, const char ** & szBonesName)const{
	bool result(false);
	if (HasModelSupport(player)){
		switch (player.shapeType){
		case PLAYERMODEL_MAJOR:
			if (GENDER_MALE == player.gender){
				szBonesName = s_szOboroMaleStandardLowerBodyBonesName;
				bonesNameCount = ARRAY_SIZE(s_szOboroMaleStandardLowerBodyBonesName);
			}else{
				szBonesName = s_szOboroFemaleStandardLowerBodyBonesName;
				bonesNameCount = ARRAY_SIZE(s_szOboroFemaleStandardLowerBodyBonesName);
			}
			break;
		case PLAYERMODEL_PROFESSION:
			if (GENDER_MALE == player.gender){
				if (RES_MOD_YUEXIAN_RESHAPE_M == player.shapeID){
					szBonesName = s_szOboroMaleShape1LowerBodyBonesName;
					bonesNameCount = ARRAY_SIZE(s_szOboroMaleShape1LowerBodyBonesName);
				}
			}else{
				if (RES_MOD_YUEXIAN_RESHAPE_F == player.shapeID){
					szBonesName = s_szOboroFemaleShape1LowerBodyBonesName;
					bonesNameCount = ARRAY_SIZE(s_szOboroFemaleShape1LowerBodyBonesName);
				}
			}
			break;
		default:
			ASSERT(false);
			break;
		}
		result = true;
	}
	return result;
}

bool CECCastSkillWhenMove::HasModelSupport(CECPlayer *pPlayer)const{
	return pPlayer != NULL && HasModelSupport(PlayerInfo(pPlayer));
}
bool CECCastSkillWhenMove::IsSkillSupported(int idSkill, CECPlayer *pPlayer)const{
	return pPlayer != NULL && IsSkillSupported(idSkill, PlayerInfo(pPlayer));
}
bool CECCastSkillWhenMove::HasActionSupport(int idSkill, CECPlayer *pPlayer)const{
	return pPlayer != NULL && HasActionSupport(idSkill, PlayerInfo(pPlayer));
}
bool CECCastSkillWhenMove::GetUpperBodyBonesName(CECPlayer *pPlayer, int &bonesNameCount, const char ** & szBonesName)const{
	return pPlayer != NULL && GetUpperBodyBonesName(PlayerInfo(pPlayer), bonesNameCount, szBonesName);
}
bool CECCastSkillWhenMove::GetLowerBodyBonesName(CECPlayer *pPlayer, int &bonesNameCount, const char ** & szBonesName)const{
	return pPlayer != NULL && GetLowerBodyBonesName(PlayerInfo(pPlayer), bonesNameCount, szBonesName);
}