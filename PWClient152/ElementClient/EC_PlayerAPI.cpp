/*
* FILE: EC_PlayerAPI.cpp
*
* DESCRIPTION: 
*
* CREATED BY: Shizhenhua, 2013/8/21
*
* HISTORY: 
*
* Copyright (c) 2011 ACE Studio, All Rights Reserved.
*/


#include <ALog.h>
#include "EC_PlayerWrapper.h"
#include "EC_AutoPolicy.h"

#include <ScriptValue.h>
#include <LuaAPI.h>
#include <LuaState.h>


using namespace LuaBind;

///////////////////////////////////////////////////////////////////////////
//  
//  Define and Macro
//  
///////////////////////////////////////////////////////////////////////////

#define CHECK_PLAYERWRAPPER() \
	CECPlayerWrapper* pPlayer = _GetPlayerWrapper(); \
	if( !pPlayer ) { ASSERT(0); a_LogOutput(1, "Invalid player wrapper, failed to call lua api!"); return; }

#define CHECK_ARGNUM(func, n) \
	if( args.size() < n ) { ASSERT(0); a_LogOutput(1, #func ": lua call error - should contain at least %d parameters!", (n)); return; }

#define BEGIN_LUA_TABLE(table) CScriptValue table, _tempval; abase::vector<CScriptValue> _keyvec, _valvec;
#define SET_TABLE_ITEM(key, val) _tempval.SetVal(key); _keyvec.push_back(_tempval); _tempval.SetVal(val); _valvec.push_back(_tempval);
#define SET_TABLE_ITEM_TAB(key, tab) _tempval.SetVal(key); _keyvec.push_back(_tempval); _valvec.push_back(tab);
#define END_LUA_TABLE(table) table.SetArray(_valvec, _keyvec);


///////////////////////////////////////////////////////////////////////////
//  
//  Local functions
//  
///////////////////////////////////////////////////////////////////////////

static CECPlayerWrapper* _GetPlayerWrapper()
{
	return CECAutoPolicy::GetInstance().GetPlayerWrapper();
}


///////////////////////////////////////////////////////////////////////////
//  
//  Implement all the API function
//  
///////////////////////////////////////////////////////////////////////////

/** 停止策略
*/
void StopPolicy(abase::vector<CScriptValue>& args, abase::vector<CScriptValue>& ret)
{
	CHECK_PLAYERWRAPPER();
	pPlayer->StopPolicy();
}
IMPLEMENT_SCRIPT_API(StopPolicy)

/** 玩家行为是否已经处理完
*/
void HaveAction(abase::vector<CScriptValue>& args, abase::vector<CScriptValue>& ret)
{
	CHECK_PLAYERWRAPPER();
	ret.push_back(pPlayer->HaveAction());
}
IMPLEMENT_SCRIPT_API(HaveAction)

/** 添加一个空闲行为
@param 持续时间
*/
void AddIdleAction(abase::vector<CScriptValue>& args, abase::vector<CScriptValue>& ret)
{
	CHECK_PLAYERWRAPPER();
	CHECK_ARGNUM(AddIdleAction, 1);

	int iTime = args[0].GetInt();
	pPlayer->AddIdleAction(iTime);
}
IMPLEMENT_SCRIPT_API(AddIdleAction)

/** 获取物品索引
@param 包裹的ID，包裹、装备包裹、任务包裹
@param 物品的TID
*/
void GetItemIndex(abase::vector<CScriptValue>& args, abase::vector<CScriptValue>& ret)
{
	CHECK_PLAYERWRAPPER();
	CHECK_ARGNUM(GetItemIndex, 2);

	int iPack = args[0].GetInt();
	int tid = args[1].GetInt();
	ret.push_back((double)pPlayer->GetItemIndex(iPack, tid));
}
IMPLEMENT_SCRIPT_API(GetItemIndex)

/** 获取物品的个数
@param 包裹的ID，包裹、装备包裹、任务包裹
@param 物品的TID
*/
void GetItemCount(abase::vector<CScriptValue>& args, abase::vector<CScriptValue>& ret)
{
	CHECK_PLAYERWRAPPER();
	CHECK_ARGNUM(GetItemCount, 2);

	int iPack = args[0].GetInt();
	int tid = args[1].GetInt();
	ret.push_back((double)pPlayer->GetItemCount(iPack, tid));
}
IMPLEMENT_SCRIPT_API(GetItemCount)

/** 获取玩家的位置
*/
void GetPos(abase::vector<CScriptValue>& args, abase::vector<CScriptValue>& ret)
{
	CHECK_PLAYERWRAPPER();
	A3DVECTOR3 vPos = pPlayer->GetPos();

	BEGIN_LUA_TABLE(pos);
	SET_TABLE_ITEM("x", vPos.x);
	SET_TABLE_ITEM("y", vPos.y);
	SET_TABLE_ITEM("z", vPos.z);
	END_LUA_TABLE(pos);
	ret.push_back(pos);
}
IMPLEMENT_SCRIPT_API(GetPos)

/** 移动到指定位置
@Param X
@Param Z
*/
void MoveTo(abase::vector<CScriptValue>& args, abase::vector<CScriptValue>& ret)
{
	CHECK_PLAYERWRAPPER();
	CHECK_ARGNUM(MoveTo, 2);

	float x = (float)args[0].GetDouble();
	float z = (float)args[1].GetDouble();
	pPlayer->MoveTo(x, z);
}
IMPLEMENT_SCRIPT_API(MoveTo)

/** 取消当前行为
*/
void CancelAction(abase::vector<CScriptValue>& args, abase::vector<CScriptValue>& ret)
{
	CHECK_PLAYERWRAPPER();
	pPlayer->CancelAction();
}
IMPLEMENT_SCRIPT_API(CancelAction)

/** 搜寻一个目标
*/
void SearchTarget(abase::vector<CScriptValue>& args, abase::vector<CScriptValue>& ret)
{
	CHECK_PLAYERWRAPPER();
	int id = 0, tid = 0;
	pPlayer->SearchTarget(id, tid);
	ret.push_back((double)id);
	ret.push_back((double)tid);
}
IMPLEMENT_SCRIPT_API(SearchTarget)

/** 获取当前选中的目标
*/
void GetSelectedTarget(abase::vector<CScriptValue>& args, abase::vector<CScriptValue>& ret)
{
	CHECK_PLAYERWRAPPER();
	ret.push_back((double)pPlayer->GetSelectedTarget());
}
IMPLEMENT_SCRIPT_API(GetSelectedTarget)

/** 选取取指定目标
@param 目标ID
*/
void SelectTarget(abase::vector<CScriptValue>& args, abase::vector<CScriptValue>& ret)
{
	CHECK_PLAYERWRAPPER();
	CHECK_ARGNUM(SelectTarget, 1);

	int iTarget = args[0].GetInt();
	pPlayer->SelectTarget(iTarget);
}
IMPLEMENT_SCRIPT_API(SelectTarget)

/** 取消选中
*/
void Unselect(abase::vector<CScriptValue>& args, abase::vector<CScriptValue>& ret)
{
	CHECK_PLAYERWRAPPER();
	pPlayer->Unselect();
}
IMPLEMENT_SCRIPT_API(Unselect)

/** 判断选取对象能否被攻击
@param 怪物ID
*/
void NpcCanAttack(abase::vector<CScriptValue>& args, abase::vector<CScriptValue>& ret)
{
	CHECK_PLAYERWRAPPER();
	CHECK_ARGNUM(NpcCanAttack, 1);

	int iMonster = args[0].GetInt();
	ret.push_back(pPlayer->NpcCanAttack(iMonster));
}
IMPLEMENT_SCRIPT_API(NpcCanAttack)

/** 施放组合技能
@param 组合技能ID
@Param 是否忽略普攻和循环的标记
@Param 是否延迟，本参数仅用于辅助技能的施放
*/
void CastComboSkill(abase::vector<CScriptValue>& args, abase::vector<CScriptValue>& ret)
{
	CHECK_PLAYERWRAPPER();
	CHECK_ARGNUM(CastComboSkill, 1);

	int iSkill = args[0].GetInt();
	bool bIgnoreAtkLoop = args.size() >= 2 ? args[1].GetBool() : false;
	bool bDelay = args.size() >= 3 ? args[2].GetBool() : false;
	pPlayer->CastComboSkill(iSkill, bIgnoreAtkLoop, bDelay);
}
IMPLEMENT_SCRIPT_API(CastComboSkill)

/** 施放技能
@Param 技能ID
@Param 是否延迟，本参数仅用于辅助技能的施放
*/
void CastSkill(abase::vector<CScriptValue>& args, abase::vector<CScriptValue>& ret)
{
	CHECK_PLAYERWRAPPER();
	CHECK_ARGNUM(CastSkill, 1);

	int iSkill = args[0].GetInt();
	bool bDelay = args.size() >= 2 ? args[1].GetBool() : false;
	ret.push_back(pPlayer->CastSkill(iSkill, bDelay));
}
IMPLEMENT_SCRIPT_API(CastSkill)

/** 发起普攻
*/
void NormalAttack(abase::vector<CScriptValue>& args, abase::vector<CScriptValue>& ret)
{
	CHECK_PLAYERWRAPPER();
	ret.push_back(pPlayer->NormalAttack());
}
IMPLEMENT_SCRIPT_API(NormalAttack)

/** 使用物品
@param 物品在包裹中的索引
@param 物品的TID
*/
void UseItem(abase::vector<CScriptValue>& args, abase::vector<CScriptValue>& ret)
{
	CHECK_PLAYERWRAPPER();
	CHECK_ARGNUM(UseItem, 1);

	int iSlot = args[0].GetInt();
	pPlayer->UseItem(iSlot);
}
IMPLEMENT_SCRIPT_API(UseItem)

/** 判断物品是否可以捡取
@param 物品的ID
@param 物品的TID
*/
void MatterCanPickup(abase::vector<CScriptValue>& args, abase::vector<CScriptValue>& ret)
{
	CHECK_PLAYERWRAPPER();
	CHECK_ARGNUM(MatterCanPickup, 2);

	int matter_id = args[0].GetInt();
	int tid = args[1].GetInt();
	ret.push_back(pPlayer->MatterCanPickup(matter_id, tid));
}
IMPLEMENT_SCRIPT_API(MatterCanPickup)

/** 捡取物品
@param 物品的ID
*/
void Pickup(abase::vector<CScriptValue>& args, abase::vector<CScriptValue>& ret)
{
	CHECK_PLAYERWRAPPER();
	CHECK_ARGNUM(Pickup, 1);

	int tid = args[0].GetInt();
	pPlayer->Pickup(tid);
}
IMPLEMENT_SCRIPT_API(Pickup)

/** 判断玩家是否离开视野
@param 玩家ID
*/
void IsPlayerInSlice(abase::vector<CScriptValue>& args, abase::vector<CScriptValue>& ret)
{
	CHECK_PLAYERWRAPPER();
	CHECK_ARGNUM(IsPlayerInSlice, 1);

	int idPlayer = args[0].GetInt();
	ret.push_back(pPlayer->IsPlayerInSlice(idPlayer));
}
IMPLEMENT_SCRIPT_API(IsPlayerInSlice)

/** 获取武器持久
*/
void GetWeaponEndurance(abase::vector<CScriptValue>& args, abase::vector<CScriptValue>& ret)
{
	CHECK_PLAYERWRAPPER();
	ret.push_back((double)pPlayer->GetWeaponEndurance());
}
IMPLEMENT_SCRIPT_API(GetWeaponEndurance)

/** 玩家是否已死
*/
void IsDead(abase::vector<CScriptValue>& args, abase::vector<CScriptValue>& ret)
{
	CHECK_PLAYERWRAPPER();
	ret.push_back(pPlayer->IsDead());
}
IMPLEMENT_SCRIPT_API(IsDead)

/** 是否被其他玩家施法相救
*/
void IsRevivedByOther(abase::vector<CScriptValue>& args, abase::vector<CScriptValue>& ret)
{
	CHECK_PLAYERWRAPPER();
	ret.push_back(pPlayer->IsRevivedByOther());
}
IMPLEMENT_SCRIPT_API(IsRevivedByOther)

/** 接受他人施法复活
*/
void AcceptRevive(abase::vector<CScriptValue>& args, abase::vector<CScriptValue>& ret)
{
	CHECK_PLAYERWRAPPER();
	pPlayer->AcceptRevive();
}
IMPLEMENT_SCRIPT_API(AcceptRevive)

/** 使用复活卷轴复活
*/
void ReviveByItem(abase::vector<CScriptValue>& args, abase::vector<CScriptValue>& ret)
{
	CHECK_PLAYERWRAPPER();
	ret.push_back(pPlayer->ReviveByItem());
}
IMPLEMENT_SCRIPT_API(ReviveByItem)

/** 回城复活
*/
void ReviveInTown(abase::vector<CScriptValue>& args, abase::vector<CScriptValue>& ret)
{
	CHECK_PLAYERWRAPPER();
	pPlayer->ReviveInTown();
}
IMPLEMENT_SCRIPT_API(ReviveInTown)

/** 获取自动打怪策略的配置数据
*/
void GetConfigData(abase::vector<CScriptValue>& args, abase::vector<CScriptValue>& ret)
{
	const CECAutoPolicy::CONFIG& cfgData = CECAutoPolicy::GetInstance().GetConfigData();

	BEGIN_LUA_TABLE(config);
	
	CScriptValue attack_skill;
	{
		_valvec.clear(); _keyvec.clear();
		SET_TABLE_ITEM("id", cfgData.attack_skill);
		SET_TABLE_ITEM("is_combo", cfgData.attack_iscombo);
		attack_skill.SetArray(_valvec, _keyvec);
	}

	CScriptValue assist_skill;
	{
		_valvec.clear(); _keyvec.clear();
		SET_TABLE_ITEM("id", cfgData.assist_skill);
		SET_TABLE_ITEM("is_combo", cfgData.assist_iscombo);
		assist_skill.SetArray(_valvec, _keyvec);
	}

	_valvec.clear(); _keyvec.clear();
	SET_TABLE_ITEM_TAB("attack_skill", attack_skill);
	SET_TABLE_ITEM_TAB("assist_skill", assist_skill);
	SET_TABLE_ITEM("assist_interval", cfgData.nAssistInterval);
	SET_TABLE_ITEM("petrol_radius", cfgData.nPetrolRadius);
	SET_TABLE_ITEM("keeping_time", cfgData.nTime);
	SET_TABLE_ITEM("autopick", cfgData.iAutoPickMode);
	END_LUA_TABLE(config);

	ret.push_back(config);
}
IMPLEMENT_SCRIPT_API(GetConfigData)

/** 设置强行攻击标记
*/
void SetForceAttack(abase::vector<CScriptValue>& args, abase::vector<CScriptValue>& ret)
{
	CHECK_PLAYERWRAPPER();
	CHECK_ARGNUM(SetForceAttack, 1);

	bool bFlag = args[0].GetBool();
	pPlayer->SetForceAttack(bFlag);
}
IMPLEMENT_SCRIPT_API(SetForceAttack)

/** 获取玩家初始位置
*/
void GetHostOrigPos(abase::vector<CScriptValue>& args, abase::vector<CScriptValue>& ret)
{
	CHECK_PLAYERWRAPPER();
	A3DVECTOR3 vPos = pPlayer->GetOrigPos();

	BEGIN_LUA_TABLE(pos);
	SET_TABLE_ITEM("x", vPos.x);
	SET_TABLE_ITEM("y", vPos.y);
	SET_TABLE_ITEM("z", vPos.z);
	END_LUA_TABLE(pos);
	ret.push_back(pos);
}
IMPLEMENT_SCRIPT_API(GetHostOrigPos)

/** 获取指定物体的位置
*/
void GetObjectPos(abase::vector<CScriptValue>& args, abase::vector<CScriptValue>& ret)
{
	CHECK_PLAYERWRAPPER();
	CHECK_ARGNUM(GetObjectPos, 1);

	A3DVECTOR3 vPos = pPlayer->GetObjectPos(args[0].GetInt());
	BEGIN_LUA_TABLE(pos);
	SET_TABLE_ITEM("x", vPos.x);
	SET_TABLE_ITEM("y", vPos.y);
	SET_TABLE_ITEM("z", vPos.z);
	END_LUA_TABLE(pos);
	ret.push_back(pos);
}
IMPLEMENT_SCRIPT_API(GetObjectPos)

/** 设置指定的物品不可攻击或捡取
*/
void SetInvalidObject(abase::vector<CScriptValue>& args, abase::vector<CScriptValue>& ret)
{
	CHECK_PLAYERWRAPPER();
	CHECK_ARGNUM(SetInvalidObject, 1);

	int object_id = args[0].GetInt();
	pPlayer->SetInvalidObject(object_id);
}
IMPLEMENT_SCRIPT_API(SetInvalidObject)

/** 是否在安全区内
*/
void IsInSanctuary(abase::vector<CScriptValue>& args, abase::vector<CScriptValue>& ret)
{
	CHECK_PLAYERWRAPPER();
	ret.push_back(pPlayer->IsInSanctuary());
}
IMPLEMENT_SCRIPT_API(IsInSanctuary)

/** 是否有怪物正在攻击我
*/
void IsMonsterAttackMe(abase::vector<CScriptValue>& args, abase::vector<CScriptValue>& ret)
{
	CHECK_PLAYERWRAPPER();
	ret.push_back(pPlayer->IsMonsterAttackMe());
}
IMPLEMENT_SCRIPT_API(IsMonsterAttackMe)


///////////////////////////////////////////////////////////////////////////

// 注册所有API
void _InitPlayerAPI(CLuaState* pState)
{
	REGISTER_SCRIPT_API(PlayerAPI, StopPolicy);
	REGISTER_SCRIPT_API(PlayerAPI, HaveAction);
	REGISTER_SCRIPT_API(PlayerAPI, AddIdleAction);
	REGISTER_SCRIPT_API(PlayerAPI, GetItemIndex);
	REGISTER_SCRIPT_API(PlayerAPI, GetItemCount);
	REGISTER_SCRIPT_API(PlayerAPI, GetPos);
	REGISTER_SCRIPT_API(PlayerAPI, MoveTo);
	REGISTER_SCRIPT_API(PlayerAPI, CancelAction);
	REGISTER_SCRIPT_API(PlayerAPI, SearchTarget);
	REGISTER_SCRIPT_API(PlayerAPI, GetSelectedTarget);
	REGISTER_SCRIPT_API(PlayerAPI, SelectTarget);
	REGISTER_SCRIPT_API(PlayerAPI, Unselect);
	REGISTER_SCRIPT_API(PlayerAPI, NpcCanAttack);
	REGISTER_SCRIPT_API(PlayerAPI, NormalAttack);
	REGISTER_SCRIPT_API(PlayerAPI, CastComboSkill);
	REGISTER_SCRIPT_API(PlayerAPI, CastSkill);
	REGISTER_SCRIPT_API(PlayerAPI, UseItem);
	REGISTER_SCRIPT_API(PlayerAPI, MatterCanPickup);
	REGISTER_SCRIPT_API(PlayerAPI, Pickup);
	REGISTER_SCRIPT_API(PlayerAPI, IsPlayerInSlice);
	REGISTER_SCRIPT_API(PlayerAPI, GetWeaponEndurance);
	REGISTER_SCRIPT_API(PlayerAPI, IsDead);
	REGISTER_SCRIPT_API(PlayerAPI, IsRevivedByOther);
	REGISTER_SCRIPT_API(PlayerAPI, AcceptRevive);
	REGISTER_SCRIPT_API(PlayerAPI, ReviveByItem);
	REGISTER_SCRIPT_API(PlayerAPI, ReviveInTown);
	REGISTER_SCRIPT_API(PlayerAPI, GetConfigData);
	REGISTER_SCRIPT_API(PlayerAPI, SetForceAttack);
	REGISTER_SCRIPT_API(PlayerAPI, GetHostOrigPos);
	REGISTER_SCRIPT_API(PlayerAPI, GetObjectPos);
	REGISTER_SCRIPT_API(PlayerAPI, SetInvalidObject);
	REGISTER_SCRIPT_API(PlayerAPI, IsInSanctuary);
	REGISTER_SCRIPT_API(PlayerAPI, IsMonsterAttackMe);

	pState->RegisterLibApi("PlayerAPI");
}