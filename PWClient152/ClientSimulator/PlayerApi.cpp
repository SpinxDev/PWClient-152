/*
 * FILE: PlayerApi.cpp
 *
 * DESCRIPTION: 本文件定义了一组玩家相关的API供脚本使用
 *
 * CREATED BY: Shizhenhua, 2012/9/11
 *
 * HISTORY: 
 *
 * Copyright (c) 2011 ACE Studio, All Rights Reserved.
 */


#include "Common.h"
#include "luaFunc.h"
#include "SessionManager.h"
#include "GameSession.h"
#include "GameObject.h"
#include "PlayerObject.h"


///////////////////////////////////////////////////////////////////////////
//  
//  Define and Macro
//  
///////////////////////////////////////////////////////////////////////////

#define GET_LUA_STRING(dest, val) AString dest; val.RetrieveAString(dest)
#define GET_LUA_WSTRING(dest, val) AWString dest; val.RetrieveAWString(dest)
#define SET_LUA_TABLE(key, value) val.SetVal(key); keyvec.push_back(val); val.SetVal(value); valvec.push_back(val);

#define CHECK_ARGNUM(n, func) if( args.size() < n ) { a_LogOutput(1, func ", 脚本调用错误 - 应该至少包含%d个参数！", (n)); return; }

///////////////////////////////////////////////////////////////////////////
//  
//  Types and Global functions
//  
///////////////////////////////////////////////////////////////////////////

static PlayerObject* _GetPlayerObject(const char* username)
{
	GameSession* pSession = SessionManager::GetSingleton().GetSessionByName(username);
	return pSession ? pSession->GetGameObject()->GetHostPlayer() : NULL;
}


///////////////////////////////////////////////////////////////////////////
//  
//  Lua API Functions
//  
///////////////////////////////////////////////////////////////////////////

namespace LuaBind
{

/** 
param: char* username
**
ret: int level
*/
void GetLevel(abase::vector<CScriptValue>& args, abase::vector<CScriptValue>& ret)
{
	PlayerObject*pPlayer = NULL;
	int iret = -1;
	CHECK_ARGNUM(1, "PlayerAPI.GetLevel");
	GET_LUA_STRING(username, args[0]);
	pPlayer = _GetPlayerObject(username);

	if(pPlayer)
	{
		iret = pPlayer->GetLevel();
		ret.push_back((double)(iret));
	}
}
IMPLEMENT_SCRIPT_API(GetLevel)

// 获取玩家修真等级
void GetLevel2(abase::vector<CScriptValue>& args, abase::vector<CScriptValue>& ret)
{
	CHECK_ARGNUM(1, "PlayerAPI.GetLevel2");
	GET_LUA_STRING(username, args[0]);
	PlayerObject* pPlayer = _GetPlayerObject(username);
	if( pPlayer )
		ret.push_back((double)pPlayer->GetLevel2());
}
IMPLEMENT_SCRIPT_API(GetLevel2)

/** 获取玩家职业类别
*/
void GetProfession(abase::vector<CScriptValue>& args, abase::vector<CScriptValue>& ret)
{
	CHECK_ARGNUM(1, "PlayerAPI.GetProfession");

	GET_LUA_STRING(username, args[0]);
	PlayerObject* pPlayer = _GetPlayerObject(username);

	if( pPlayer )
	{
		ret.push_back(double(pPlayer->GetProfession()));
	}
}
IMPLEMENT_SCRIPT_API(GetProfession);

/** 玩家是否为男性
*/
void IsMale(abase::vector<CScriptValue>& args, abase::vector<CScriptValue>& ret)
{
	CHECK_ARGNUM(1, "PlayerAPI.IsMale");

	GET_LUA_STRING(username, args[0]);
	PlayerObject* pPlayer = _GetPlayerObject(username);

	if( pPlayer )
		ret.push_back(pPlayer->IsMale());
}
IMPLEMENT_SCRIPT_API(IsMale)

/** 
param: char* username
**
ret: int HP
*/
void GetHP(abase::vector<CScriptValue>& args, abase::vector<CScriptValue>& ret)
{
	PlayerObject*pPlayer = NULL;
	int iret = -1;
	CHECK_ARGNUM(1, "PlayerAPI.GetHP");
	GET_LUA_STRING(username, args[0]);
	pPlayer = _GetPlayerObject(username);

	if(pPlayer)
	{
		iret = pPlayer->GetHP();
		ret.push_back((double)(iret));
	}
}
IMPLEMENT_SCRIPT_API(GetHP)

/** 
param: char* username
**
ret: int MP
*/
void GetMP(abase::vector<CScriptValue>& args, abase::vector<CScriptValue>& ret)
{
	PlayerObject*pPlayer = NULL;
	int iret = -1;
	CHECK_ARGNUM(1, "PlayerAPI.GetMP");
	GET_LUA_STRING(username, args[0]);
	pPlayer = _GetPlayerObject(username);

	if(pPlayer)
	{
		iret = pPlayer->GetMP();
		ret.push_back((double)(iret));
	}
}
IMPLEMENT_SCRIPT_API(GetMP)

/** 
param: char* username
**
ret: int MaxHP
*/
void GetMaxHP(abase::vector<CScriptValue>& args, abase::vector<CScriptValue>& ret)
{
	PlayerObject*pPlayer = NULL;
	int iret = -1;
	CHECK_ARGNUM(1, "PlayerAPI.GetMaxHP");
	GET_LUA_STRING(username, args[0]);
	pPlayer = _GetPlayerObject(username);

	if(pPlayer)
	{
		iret = pPlayer->GetMaxHP();
		ret.push_back((double)(iret));
	}
}
IMPLEMENT_SCRIPT_API(GetMaxHP)

/** 
param: char* username
**
ret: int MaxMP
*/
void GetMaxMP(abase::vector<CScriptValue>& args, abase::vector<CScriptValue>& ret)
{
	PlayerObject*pPlayer = NULL;
	int iret = -1;
	CHECK_ARGNUM(1, "PlayerAPI.GetMaxMP");
	GET_LUA_STRING(username, args[0]);
	pPlayer = _GetPlayerObject(username);

	if(pPlayer)
	{
		iret = pPlayer->GetMaxMP();
		ret.push_back((double)(iret));
	}
}
IMPLEMENT_SCRIPT_API(GetMaxMP)
//
void GetRoleID(abase::vector<CScriptValue>& args, abase::vector<CScriptValue>& ret)
{
	PlayerObject*pPlayer = NULL;
	int iret = -1;
	CHECK_ARGNUM(1, "PlayerAPI.GetRoleID");
	GET_LUA_STRING(username, args[0]);
	pPlayer = _GetPlayerObject(username);
	
	if(pPlayer)
	{
		iret = pPlayer->GetRoleID();
		ret.push_back((double)(iret));
	}
}
IMPLEMENT_SCRIPT_API(GetRoleID)

// 获取角色的名字
void GetRoleName(abase::vector<CScriptValue>& args, abase::vector<CScriptValue>& ret)
{
	CHECK_ARGNUM(1, "PlayerAPI.GetRoleName");
	GET_LUA_STRING(username, args[0]);
	PlayerObject* pPlayer = _GetPlayerObject(username);

	if( pPlayer )
	{
		CScriptValue name;
		name.SetVal(pPlayer->GetRoleName());
		ret.push_back(name);
	}
}
IMPLEMENT_SCRIPT_API(GetRoleName)

// 获取钱的数量
void GetMoney(abase::vector<CScriptValue>& args, abase::vector<CScriptValue>& ret)
{
	CHECK_ARGNUM(1, "PlayerAPI.GetMoney");
	GET_LUA_STRING(username, args[0]);
	PlayerObject* pPlayer = _GetPlayerObject(username);

	if( pPlayer )
	{
		ret.push_back((double)pPlayer->GetMoney());
	}
}
IMPLEMENT_SCRIPT_API(GetMoney)

// 获取玩家经验值
void GetExp(abase::vector<CScriptValue>& args, abase::vector<CScriptValue>& ret)
{
	CHECK_ARGNUM(1, "PlayerAPI.GetExp");
	GET_LUA_STRING(username, args[0]);
	PlayerObject* pPlayer = _GetPlayerObject(username);
	if( pPlayer )
		ret.push_back((double)pPlayer->GetExp());
}
IMPLEMENT_SCRIPT_API(GetExp)

// 获取玩家的技能点
void GetSP(abase::vector<CScriptValue>& args, abase::vector<CScriptValue>& ret)
{
	CHECK_ARGNUM(1, "PlayerAPI.GetSP");
	GET_LUA_STRING(username, args[0]);
	PlayerObject* pPlayer = _GetPlayerObject(username);
	if( pPlayer )
		ret.push_back((double)pPlayer->GetSP());
}
IMPLEMENT_SCRIPT_API(GetSP)

// 角色是否死亡
void IsDead(abase::vector<CScriptValue>& args, abase::vector<CScriptValue>& ret)
{
	PlayerObject*pPlayer = NULL;
	bool bret = false;
	CHECK_ARGNUM(1, "PlayerAPI.IsDead");
	GET_LUA_STRING(username, args[0]);
	pPlayer = _GetPlayerObject(username);
	
	if(pPlayer)
		bret = pPlayer->IsDead();
	
	ret.push_back(bret);
}
IMPLEMENT_SCRIPT_API(IsDead)

// 角色是否吃了变形丸
void IsChangingFace(abase::vector<CScriptValue>& args, abase::vector<CScriptValue>& ret)
{
	CHECK_ARGNUM(1, "PlayerAPI.IsChangingFace");
	GET_LUA_STRING(username, args[0]);
	PlayerObject* pPlayer = _GetPlayerObject(username);

	bool bret = false;
	if( pPlayer )
		bret = pPlayer->IsChangingFace();
	ret.push_back(bret);
}
IMPLEMENT_SCRIPT_API(IsChangingFace)

// 角色是否处于时装模式
void IsInFashion(abase::vector<CScriptValue>& args, abase::vector<CScriptValue>& ret)
{
	CHECK_ARGNUM(1, "PlayerAPI.IsInFashion");
	GET_LUA_STRING(username, args[0]);
	PlayerObject* pPlayer = _GetPlayerObject(username);

	bool bRet = false;
	if( pPlayer )
		bRet = pPlayer->IsInFashion();
	ret.push_back(bRet);
}
IMPLEMENT_SCRIPT_API(IsInFashion)

//
void GetCurPos(abase::vector<CScriptValue>& args, abase::vector<CScriptValue>& ret)
{
	PlayerObject* pPlayer = NULL;
	CHECK_ARGNUM(1, "PlayerAPI.GetCurPos");
	GET_LUA_STRING(username, args[0]);
	pPlayer = _GetPlayerObject(username);
	
	if(pPlayer)
	{
		A3DVECTOR3 vret = pPlayer->GetCurPos();

		CScriptValue vPos;
		abase::vector<CScriptValue> vPosValues;
		vPosValues.push_back((double)vret.x);
		vPosValues.push_back((double)vret.y);
		vPosValues.push_back((double)vret.z);
		vPos.SetArray(vPosValues);
		ret.push_back(vPos);
	}
}
IMPLEMENT_SCRIPT_API(GetCurPos)
//
void GetDirection(abase::vector<CScriptValue>& args, abase::vector<CScriptValue>& ret)
{
	PlayerObject*pPlayer = NULL;
	int iret = -1;
	CHECK_ARGNUM(1, "PlayerAPI.GetDirection");
	GET_LUA_STRING(username, args[0]);
	pPlayer = _GetPlayerObject(username);
	
	if(pPlayer)
	{
		iret = pPlayer->GetDirection();
		ret.push_back((double)(iret));
	}
}
IMPLEMENT_SCRIPT_API(GetDirection)
//
void SetDirection(abase::vector<CScriptValue>& args, abase::vector<CScriptValue>& ret)
{
	PlayerObject*pPlayer = NULL;
	CHECK_ARGNUM(2, "PlayerAPI.SetDirection");
	GET_LUA_STRING(username, args[0]);
	pPlayer = _GetPlayerObject(username);
	
	if(pPlayer) pPlayer->SetDirection(args[1].GetInt());
	
}
IMPLEMENT_SCRIPT_API(SetDirection)
//
void GetRunSpeed(abase::vector<CScriptValue>& args, abase::vector<CScriptValue>& ret)
{
	PlayerObject*pPlayer = NULL;
	float fret = -1.0;
	CHECK_ARGNUM(1, "PlayerAPI.GetRunSpeed");
	GET_LUA_STRING(username, args[0]);
	pPlayer = _GetPlayerObject(username);
	
	if(pPlayer)
	{
		fret = pPlayer->GetRunSpeed();
		ret.push_back((double)(fret));
	}
}
IMPLEMENT_SCRIPT_API(GetRunSpeed)
//
void GetSelectedTarget(abase::vector<CScriptValue>& args, abase::vector<CScriptValue>& ret)
{
	PlayerObject* pPlayer = NULL;
	CHECK_ARGNUM(1, "PlayerAPI.GetSelectedTarget");
	GET_LUA_STRING(username, args[0]);
	pPlayer = _GetPlayerObject(username);

	int iret = -1;
	if(pPlayer)
		iret = pPlayer->GetSelectedTarget();
	ret.push_back((double)(iret));
}
IMPLEMENT_SCRIPT_API(GetSelectedTarget)
//
void GetForceAttack(abase::vector<CScriptValue>& args, abase::vector<CScriptValue>& ret)
{
	PlayerObject* pPlayer = NULL;
	CHECK_ARGNUM(1, "PlayerAPI.GetForceAttack");
	GET_LUA_STRING(username, args[0]);
	pPlayer = _GetPlayerObject(username);
	
	if(pPlayer)
	{
		int iret = pPlayer->GetForceAttack();
		ret.push_back((double)(iret));
	}
}
IMPLEMENT_SCRIPT_API(GetForceAttack)
//
void SetForceAttack(abase::vector<CScriptValue>& args, abase::vector<CScriptValue>& ret)
{
	PlayerObject*pPlayer = NULL;
	CHECK_ARGNUM(2, "PlayerAPI.SetForceAttack");
	GET_LUA_STRING(username, args[0]);
	pPlayer = _GetPlayerObject(username);
	
	if(pPlayer) pPlayer->SetForceAttack(args[1].GetBool());
	
}
IMPLEMENT_SCRIPT_API(SetForceAttack)
//
void SelectAttackSkill(abase::vector<CScriptValue>& args, abase::vector<CScriptValue>& ret)
{
	PlayerObject*pPlayer = NULL;
	int iret = -1;
	float fRange = 0.0;
	CHECK_ARGNUM(1, "PlayerAPI.SelectAttackSkill");
	GET_LUA_STRING(username, args[0]);
	pPlayer = _GetPlayerObject(username);
	
	if(pPlayer) iret = pPlayer->SelectAttackSkill(fRange);
	
	ret.push_back((double)(iret));
	ret.push_back((double)(fRange));
}
IMPLEMENT_SCRIPT_API(SelectAttackSkill)
//
void GetSkillNum(abase::vector<CScriptValue>& args, abase::vector<CScriptValue>& ret)
{
	PlayerObject*pPlayer = NULL;
	int iret = -1;
	CHECK_ARGNUM(1, "PlayerAPI.GetSkillNum");
	GET_LUA_STRING(username, args[0]);
	pPlayer = _GetPlayerObject(username);
	
	if(pPlayer) iret = pPlayer->GetSkillNum();
	
	ret.push_back((double)(iret));
}
IMPLEMENT_SCRIPT_API(GetSkillNum)

// 获取指定的技能信息
void GetSkill(abase::vector<CScriptValue>& args, abase::vector<CScriptValue>& ret)
{
	CHECK_ARGNUM(2, "PlayerAPI.GetSkill");

	GET_LUA_STRING(username, args[0]);
	PlayerObject* pPlayer = _GetPlayerObject(username);

	if( pPlayer )
	{
		PlayerObject::SKILL* pSkill = pPlayer->GetSkill(args[1].GetInt());
		if( pSkill )
		{
			CScriptValue val;
			abase::vector<CScriptValue> keyvec, valvec;
			SET_LUA_TABLE("id", pSkill->id);
			SET_LUA_TABLE("level", pSkill->level);
			SET_LUA_TABLE("mp_cost", pSkill->mp_cost);
			SET_LUA_TABLE("range", pSkill->range);
			val.SetArray(valvec, keyvec);
			ret.push_back(val);
		}
	}
}
IMPLEMENT_SCRIPT_API(GetSkill)

// 获取指定的技能信息
void GetSkillByID(abase::vector<CScriptValue>& args, abase::vector<CScriptValue>& ret)
{
	CHECK_ARGNUM(2, "PlayerAPI.GetSkillByID");

	GET_LUA_STRING(username, args[0]);
	PlayerObject* pPlayer = _GetPlayerObject(username);

	if( pPlayer )
	{
		PlayerObject::SKILL* pSkill = pPlayer->GetSkillByID(args[1].GetInt());
		if( pSkill )
		{
			CScriptValue val;
			abase::vector<CScriptValue> keyvec, valvec;
			SET_LUA_TABLE("id", pSkill->id);
			SET_LUA_TABLE("level", pSkill->level);
			SET_LUA_TABLE("mp_cost", pSkill->mp_cost);
			SET_LUA_TABLE("range", pSkill->range);
			val.SetArray(valvec, keyvec);
			ret.push_back(val);
		}
	}
}
IMPLEMENT_SCRIPT_API(GetSkillByID)

///////////////////////////////////////////////////////////////////////////
// 包裹相关
//
void GetItemCount(abase::vector<CScriptValue>& args, abase::vector<CScriptValue>& ret)
{
	PlayerObject*pPlayer = NULL;
	int iret = 0;
	CHECK_ARGNUM(3, "PlayerAPI.GetItemCount");
	GET_LUA_STRING(username, args[0]);
	pPlayer = _GetPlayerObject(username);
	
	if(pPlayer)
		iret = pPlayer->GetItemCount((PlayerObject::PackType)args[1].GetInt(),args[2].GetInt());
	
	ret.push_back((double)(iret));
}
IMPLEMENT_SCRIPT_API(GetItemCount)
//
void GetItemIndex(abase::vector<CScriptValue>& args, abase::vector<CScriptValue>& ret)
{
	PlayerObject*pPlayer = NULL;
	int iret = -1;
	CHECK_ARGNUM(3, "PlayerAPI.GetItemIndex");
	GET_LUA_STRING(username, args[0]);
	pPlayer = _GetPlayerObject(username);
	
	if(pPlayer) iret = pPlayer->GetItemIndex((PlayerObject::PackType)args[1].GetInt(),args[2].GetInt());
	
	ret.push_back((double)(iret));
}
IMPLEMENT_SCRIPT_API(GetItemIndex)
// 检测指定插槽是否存在任意装备
void IsAnyEquipExist(abase::vector<CScriptValue>& args, abase::vector<CScriptValue>& ret)
{
	CHECK_ARGNUM(2, "PlayerAPI.IsAnyEquipExist");

	GET_LUA_STRING(username, args[0]);
	PlayerObject* pPlayer = _GetPlayerObject(username);

	if( pPlayer )
		ret.push_back(CScriptValue(pPlayer->IsEquipExist(args[1].GetInt())));
}
IMPLEMENT_SCRIPT_API(IsAnyEquipExist)
// 检测指定插槽是否存在指定装备
void IsEquipExist(abase::vector<CScriptValue>& args, abase::vector<CScriptValue>& ret)
{
	PlayerObject*pPlayer = NULL;
	bool bret = false;
	CHECK_ARGNUM(3, "PlayerAPI.IsEquipExist");
	GET_LUA_STRING(username, args[0]);
	pPlayer = _GetPlayerObject(username);
	
	if(pPlayer) bret = pPlayer->IsEquipExist(args[1].GetInt(),args[2].GetInt());
	
	ret.push_back(bret);
}
IMPLEMENT_SCRIPT_API(IsEquipExist)
//
void IsItemExist(abase::vector<CScriptValue>& args, abase::vector<CScriptValue>& ret)
{
	PlayerObject*pPlayer = NULL;
	bool bret = false;
	CHECK_ARGNUM(2, "PlayerAPI.IsItemExist");
	GET_LUA_STRING(username, args[0]);
	pPlayer = _GetPlayerObject(username);
	
	if(pPlayer) bret = pPlayer->IsItemExist(args[1].GetInt());
	
	ret.push_back(bret);
}
IMPLEMENT_SCRIPT_API(IsItemExist)
//
void GetEmptySlot(abase::vector<CScriptValue>& args, abase::vector<CScriptValue>& ret)
{
	PlayerObject*pPlayer = NULL;
	int iret = -1;
	CHECK_ARGNUM(1, "PlayerAPI.GetEmptySlot");
	GET_LUA_STRING(username, args[0]);
	pPlayer = _GetPlayerObject(username);
	
	if(pPlayer) iret = pPlayer->GetEmptySlot();
	
	ret.push_back((double)(iret));
}
IMPLEMENT_SCRIPT_API(GetEmptySlot)

///////////////////////////////////////////////////////////////////////////
// 目标对象
//
void MatterCanPickup(abase::vector<CScriptValue>& args, abase::vector<CScriptValue>& ret)
{
	PlayerObject*pPlayer = NULL;
	bool bret = false;
	CHECK_ARGNUM(3, "PlayerAPI.MatterCanPickup");
	GET_LUA_STRING(username, args[0]);
	pPlayer = _GetPlayerObject(username);

	size_t pile_limit = args[2].GetInt();
	
	if(pPlayer) bret = pPlayer->MatterCanPickup(args[1].GetInt(),pile_limit);
	
	ret.push_back(bret);
}
IMPLEMENT_SCRIPT_API(MatterCanPickup)
//
void MatterCanGather(abase::vector<CScriptValue>& args, abase::vector<CScriptValue>& ret)
{
	PlayerObject*pPlayer = NULL;
	bool bret = false;
	CHECK_ARGNUM(2, "PlayerAPI.MatterCanGather");
	GET_LUA_STRING(username, args[0]);
	pPlayer = _GetPlayerObject(username);
	
	if(pPlayer) bret = pPlayer->MatterCanGather(args[1].GetInt());
	
	ret.push_back(bret);
}
IMPLEMENT_SCRIPT_API(MatterCanGather)
//
void NpcCanAttack(abase::vector<CScriptValue>& args, abase::vector<CScriptValue>& ret)
{
	PlayerObject*pPlayer = NULL;
	bool bret = false;
	CHECK_ARGNUM(2, "PlayerAPI.NpcCanAttack");
	GET_LUA_STRING(username, args[0]);
	pPlayer = _GetPlayerObject(username);
	
	if(pPlayer) bret = pPlayer->NpcCanAttack(args[1].GetInt());
	
	ret.push_back(bret);
}
IMPLEMENT_SCRIPT_API(NpcCanAttack)
//
void PlayerCanAttack(abase::vector<CScriptValue>& args, abase::vector<CScriptValue>& ret)
{
	PlayerObject*pPlayer = NULL;
	bool bret = false;
	CHECK_ARGNUM(2, "PlayerAPI.PlayerCanAttack");
	GET_LUA_STRING(username, args[0]);
	pPlayer = _GetPlayerObject(username);
	
	if(pPlayer) bret = pPlayer->PlayerCanAttack(args[1].GetInt());
	
	ret.push_back(bret);
}
IMPLEMENT_SCRIPT_API(PlayerCanAttack)

///////////////////////////////////////////////////////////////////////////
// C2S Command

// 玩家瞬移到当前地图某个位置
void Goto(abase::vector<CScriptValue>& args, abase::vector<CScriptValue>& ret)
{
	CHECK_ARGNUM(3, "PlayerAPI.Goto");
	GET_LUA_STRING(username, args[0]);
	PlayerObject* pPlayer = _GetPlayerObject(username);
	if( pPlayer )
		pPlayer->Goto((float)args[1].GetDouble(), (float)args[2].GetDouble());
}
IMPLEMENT_SCRIPT_API(Goto)

// 玩家随机瞬移
void RandomMove(abase::vector<CScriptValue>& args, abase::vector<CScriptValue>& ret)
{
	CHECK_ARGNUM(1, "PlayerAPI.RandomMove");
	GET_LUA_STRING(username, args[0]);
	PlayerObject* pPlayer = _GetPlayerObject(username);
	if( pPlayer )
		pPlayer->RandomMove();
}
IMPLEMENT_SCRIPT_API(RandomMove)

//
void SelectTarget(abase::vector<CScriptValue>& args, abase::vector<CScriptValue>& ret)
{
	PlayerObject*pPlayer = NULL;
	CHECK_ARGNUM(2, "PlayerAPI.SelectTarget");
	GET_LUA_STRING(username, args[0]);
	pPlayer = _GetPlayerObject(username);
	
	if(pPlayer) pPlayer->SelectTarget(args[1].GetInt());
	
}
IMPLEMENT_SCRIPT_API(SelectTarget)
//
void SendDebugCmd(abase::vector<CScriptValue>& args, abase::vector<CScriptValue>& ret)
{
	if( args.size() < 2 )
	{
		a_LogOutput(1, "PlayerAPI.SendDebugCmd, 错误的参数个数！");
		return;
	}

	GET_LUA_STRING(username, args[0]);
	PlayerObject* pPlayer = _GetPlayerObject(username);
	
	if( pPlayer ) 
	{
		if( args.size() == 2 )
			pPlayer->SendDebugCmd(args[1].GetInt());
		else if( args.size() == 3 )
			pPlayer->SendDebugCmd(args[1].GetInt(), args[2].GetInt());
		else
			pPlayer->SendDebugCmd(args[1].GetInt(), args[2].GetInt(), args[3].GetInt());
	}
}
IMPLEMENT_SCRIPT_API(SendDebugCmd)
//
void Unselect(abase::vector<CScriptValue>& args, abase::vector<CScriptValue>& ret)
{
	PlayerObject*pPlayer = NULL;
	CHECK_ARGNUM(1, "PlayerAPI.Unselect");
	GET_LUA_STRING(username, args[0]);
	pPlayer = _GetPlayerObject(username);
	
	if(pPlayer) pPlayer->Unselect();
	
}
IMPLEMENT_SCRIPT_API(Unselect)
//
void NormalAttack(abase::vector<CScriptValue>& args, abase::vector<CScriptValue>& ret)
{
	PlayerObject* pPlayer = NULL;
	CHECK_ARGNUM(2, "PlayerAPI.NormalAttack");
	GET_LUA_STRING(username, args[0]);
	pPlayer = _GetPlayerObject(username);
	
	if(pPlayer) pPlayer->NormalAttack(args[1].GetInt());
	
}
IMPLEMENT_SCRIPT_API(NormalAttack)
//
void CastSkill(abase::vector<CScriptValue>& args, abase::vector<CScriptValue>& ret)
{
	CHECK_ARGNUM(4, "PlayerAPI.CastSkill");
	GET_LUA_STRING(username, args[0]);
	PlayerObject* pPlayer = _GetPlayerObject(username);

	if( args[3].m_Type != CScriptValue::SVT_ARRAY )
	{
		a_LogOutput(1, "PlayerAPI.CastSkill, 错误的参数类型！");
		return;
	}

	abase::vector<int> targets;
	size_t target_count = args[3].m_ArrVal.size();
	for( size_t i=0;i<target_count;i++ )
		targets.push_back(args[3].m_ArrVal[i].GetInt());

	if( pPlayer )
		pPlayer->CastSkill(args[1].GetInt(), args[2].GetInt(), target_count, &targets[0]);
}
IMPLEMENT_SCRIPT_API(CastSkill)
//
void Pickup(abase::vector<CScriptValue>& args, abase::vector<CScriptValue>& ret)
{
	PlayerObject*pPlayer = NULL;
	CHECK_ARGNUM(3, "PlayerAPI.Pickup");
	GET_LUA_STRING(username, args[0]);
	pPlayer = _GetPlayerObject(username);
	
	if(pPlayer) pPlayer->Pickup(args[1].GetInt(),args[2].GetInt());
	
}
IMPLEMENT_SCRIPT_API(Pickup)
//
void EquipItem(abase::vector<CScriptValue>& args, abase::vector<CScriptValue>& ret)
{
	PlayerObject*pPlayer = NULL;
	CHECK_ARGNUM(3, "PlayerAPI.EquipItem");
	GET_LUA_STRING(username, args[0]);
	pPlayer = _GetPlayerObject(username);
	
	if(pPlayer) pPlayer->EquipItem(args[1].GetInt(),args[2].GetInt());
	
}
IMPLEMENT_SCRIPT_API(EquipItem)
//
void GetInventoryDetail(abase::vector<CScriptValue>& args, abase::vector<CScriptValue>& ret)
{
	PlayerObject*pPlayer = NULL;
	CHECK_ARGNUM(2, "PlayerAPI.GetInventoryDetail");
	GET_LUA_STRING(username, args[0]);
	pPlayer = _GetPlayerObject(username);
	
	if(pPlayer) pPlayer->GetInventoryDetail(args[1].GetInt());
	
}
IMPLEMENT_SCRIPT_API(GetInventoryDetail)
//
void ResurrectByItem(abase::vector<CScriptValue>& args, abase::vector<CScriptValue>& ret)
{
	PlayerObject*pPlayer = NULL;
	CHECK_ARGNUM(1, "PlayerAPI.ResurrectByItem");
	GET_LUA_STRING(username, args[0]);
	pPlayer = _GetPlayerObject(username);
	
	if(pPlayer) pPlayer->ResurrectByItem();
}
IMPLEMENT_SCRIPT_API(ResurrectByItem)
//
void ResurrectInTown(abase::vector<CScriptValue>& args, abase::vector<CScriptValue>& ret)
{
	PlayerObject*pPlayer = NULL;
	CHECK_ARGNUM(1, "PlayerAPI.ResurrectInTown");
	GET_LUA_STRING(username, args[0]);
	pPlayer = _GetPlayerObject(username);
	
	if(pPlayer) pPlayer->ResurrectInTown();
}
IMPLEMENT_SCRIPT_API(ResurrectInTown)
//
void CheckSecurityPassword(abase::vector<CScriptValue>& args, abase::vector<CScriptValue>& ret)
{
	PlayerObject*pPlayer = NULL;
	CHECK_ARGNUM(1, "PlayerAPI.CheckSecurityPassword");
	GET_LUA_STRING(username, args[0]);
	pPlayer = _GetPlayerObject(username);
	
	if(pPlayer) pPlayer->CheckSecurityPassword();
}
IMPLEMENT_SCRIPT_API(CheckSecurityPassword)
//
void CancelAction(abase::vector<CScriptValue>& args, abase::vector<CScriptValue>& ret)
{
	PlayerObject*pPlayer = NULL;
	CHECK_ARGNUM(1, "PlayerAPI.CancelAction");
	GET_LUA_STRING(username, args[0]);
	pPlayer = _GetPlayerObject(username);
	
	if(pPlayer) pPlayer->CancelAction();
}
IMPLEMENT_SCRIPT_API(CancelAction)
//
void Logout(abase::vector<CScriptValue>& args, abase::vector<CScriptValue>& ret)
{
	PlayerObject*pPlayer = NULL;
	CHECK_ARGNUM(1, "PlayerAPI.Logout");
	GET_LUA_STRING(username, args[0]);
	pPlayer = _GetPlayerObject(username);
	
	if(pPlayer) pPlayer->Logout();
}
IMPLEMENT_SCRIPT_API(Logout)
//
void Speak(abase::vector<CScriptValue>& args, abase::vector<CScriptValue>& ret)
{
	PlayerObject*pPlayer = NULL;
	CHECK_ARGNUM(2, "PlayerAPI.Speak");
	GET_LUA_STRING(username, args[0]);
	pPlayer = _GetPlayerObject(username);
	GET_LUA_WSTRING(strText, args[1]);
	if(pPlayer) pPlayer->Speak(strText);
}
IMPLEMENT_SCRIPT_API(Speak)
//
void Sitdown(abase::vector<CScriptValue>& args, abase::vector<CScriptValue>& ret)
{
	PlayerObject*pPlayer = NULL;
	CHECK_ARGNUM(2, "PlayerAPI.Sitdown");
	GET_LUA_STRING(username, args[0]);
	pPlayer = _GetPlayerObject(username);
	
	if(pPlayer) pPlayer->Sitdown(args[1].GetBool());
}
IMPLEMENT_SCRIPT_API(Sitdown)

// 玩家和NPC对话
void TalkToNPC(abase::vector<CScriptValue>& args, abase::vector<CScriptValue>& ret)
{
	CHECK_ARGNUM(2, "PlayerAPI.TalkToNPC");
	GET_LUA_STRING(username, args[0]);
	PlayerObject* pPlayer = _GetPlayerObject(username);
	if( pPlayer )
		pPlayer->TalkToNPC(args[1].GetInt());
}
IMPLEMENT_SCRIPT_API(TalkToNPC)

// 玩家学技能
void LearnSkill(abase::vector<CScriptValue>& args, abase::vector<CScriptValue>& ret)
{
	CHECK_ARGNUM(2, "PlayerAPI.LearnSkill");
	GET_LUA_STRING(username, args[0]);
	PlayerObject* pPlayer = _GetPlayerObject(username);
	if( pPlayer )
		pPlayer->LearnSkill(args[1].GetInt());
}
IMPLEMENT_SCRIPT_API(LearnSkill)

// 玩家摆摊
void Booth(abase::vector<CScriptValue>& args, abase::vector<CScriptValue>& ret)
{
	if( args.size() < 2 )
	{
		a_LogOutput(1, "PlayerAPI.Booth, 错误的参数个数！");
		return;
	}

	GET_LUA_STRING(username, args[0]);
	PlayerObject* pPlayer = _GetPlayerObject(username);
	bool bOpen = args[1].GetBool();

	if( pPlayer )
	{
		if( args.size() > 2 )
		{
			GET_LUA_WSTRING(boothName, args[2]);
			pPlayer->Booth(bOpen, (const wchar_t*)boothName);
		}
		else
			pPlayer->Booth(bOpen);
	}
}
IMPLEMENT_SCRIPT_API(Booth)

// 玩家表演动作
void PlayAction(abase::vector<CScriptValue>& args, abase::vector<CScriptValue>& ret)
{
	CHECK_ARGNUM(2, "PlayerAPI.PlayAction");
	GET_LUA_STRING(username, args[0]);
	PlayerObject* pPlayer = _GetPlayerObject(username);
	if( pPlayer )
	{
		int iPose = args[1].GetInt();
		pPlayer->PlayAction(iPose);
	}
}
IMPLEMENT_SCRIPT_API(PlayAction)

// 切换时装/普通模式
void SwitchFashion(abase::vector<CScriptValue>& args, abase::vector<CScriptValue>& ret)
{
	CHECK_ARGNUM(2, "PlayerAPI.SwitchFashion");
	GET_LUA_STRING(username, args[0]);
	PlayerObject* pPlayer = _GetPlayerObject(username);
	if( pPlayer )
		pPlayer->SwitchFashion(args[1].GetBool());
}
IMPLEMENT_SCRIPT_API(SwitchFashion)

// 召唤宠物
void SummonPet(abase::vector<CScriptValue>& args, abase::vector<CScriptValue>& ret)
{
	CHECK_ARGNUM(2, "PlayerAPI.SummonPet");
	GET_LUA_STRING(username, args[0]);
	PlayerObject* pPlayer = _GetPlayerObject(username);
	if( pPlayer )
		pPlayer->SummonPet(args[1].GetInt());
}
IMPLEMENT_SCRIPT_API(SummonPet)

// 召回宠物
void BanishPet(abase::vector<CScriptValue>& args, abase::vector<CScriptValue>& ret)
{
	CHECK_ARGNUM(2, "PlayerAPI.BanishPet");
	GET_LUA_STRING(username, args[0]);
	PlayerObject* pPlayer = _GetPlayerObject(username);
	if( pPlayer )
		pPlayer->BanishPet(args[1].GetInt());
}
IMPLEMENT_SCRIPT_API(BanishPet)

// 获取玩家宠物数量
void GetPetCount(abase::vector<CScriptValue>& args, abase::vector<CScriptValue>& ret)
{
	CHECK_ARGNUM(1, "PlayerAPI.GetPetCount");
	GET_LUA_STRING(username, args[0]);
	PlayerObject* pPlayer = _GetPlayerObject(username);
	if( pPlayer )
		ret.push_back((double)pPlayer->GetPetCount());
}
IMPLEMENT_SCRIPT_API(GetPetCount)

// 获取宠物数据
void GetPetData(abase::vector<CScriptValue>& args, abase::vector<CScriptValue>& ret)
{
	CHECK_ARGNUM(2, "PlayerAPI.GetPetData");
	GET_LUA_STRING(username, args[0]);
	PlayerObject* pPlayer = _GetPlayerObject(username);
	if( pPlayer )
	{
		const PlayerObject::PET_DATA* pPet = pPlayer->GetPetData(args[1].GetInt());
		if( pPet )
		{
			CScriptValue val, objInfo;
			abase::vector<CScriptValue> keyvec, valvec;
			
			SET_LUA_TABLE("name", pPet->name);
			SET_LUA_TABLE("class", pPet->pet_class);
			SET_LUA_TABLE("level", pPet->level);

			objInfo.SetArray(valvec, keyvec);
			ret.push_back(objInfo);
		}
	}
}
IMPLEMENT_SCRIPT_API(GetPetData)

// 拥抱指定玩家
void AttachBuddy(abase::vector<CScriptValue>& args, abase::vector<CScriptValue>& ret)
{
	CHECK_ARGNUM(2, "PlayerAPI.AttachBuddy");
	GET_LUA_STRING(username, args[0]);
	PlayerObject* pPlayer = _GetPlayerObject(username);
	if( pPlayer )
		pPlayer->AttachBuddy(args[1].GetInt());
}
IMPLEMENT_SCRIPT_API(AttachBuddy)

// 解除相依相偎
void DetachBuddy(abase::vector<CScriptValue>& args, abase::vector<CScriptValue>& ret)
{
	CHECK_ARGNUM(1, "PlayerAPI.DetachBuddy");
	GET_LUA_STRING(username, args[0]);
	PlayerObject* pPlayer = _GetPlayerObject(username);
	if( pPlayer )
		pPlayer->DetachBuddy();
}
IMPLEMENT_SCRIPT_API(DetachBuddy)

////////////////////////////////////////////////////
// 选取待捡的物品
void SelectMatter(abase::vector<CScriptValue>& args, abase::vector<CScriptValue>& ret)
{
	PlayerObject*pPlayer = NULL;
	CHECK_ARGNUM(2, "PlayerAPI.SelectMatter");
	GET_LUA_STRING(username, args[0]);
	pPlayer = _GetPlayerObject(username);
	
	if(pPlayer) pPlayer->SelectMatter(args[1].GetInt());
	
}
IMPLEMENT_SCRIPT_API(SelectMatter)
///////////////////////////////////////////////////////////////////////////
// 玩家行为相关

// 
void SetAIPolicy(abase::vector<CScriptValue>& args, abase::vector<CScriptValue>& ret)
{
	PlayerObject*pPlayer = NULL;
	CHECK_ARGNUM(2, "PlayerAPI.SetAIPolicy");
	GET_LUA_STRING(username, args[0]);
	pPlayer = _GetPlayerObject(username);
	GET_LUA_STRING(policy, args[1]);
	if(pPlayer) pPlayer->SetAIPolicy(policy);
	
}
IMPLEMENT_SCRIPT_API(SetAIPolicy)
// 
void AddIdleTask(abase::vector<CScriptValue>& args, abase::vector<CScriptValue>& ret)
{
	PlayerObject*pPlayer = NULL;
	CHECK_ARGNUM(2, "PlayerAPI.AddIdleTask");
	GET_LUA_STRING(username, args[0]);
	pPlayer = _GetPlayerObject(username);
	
	if(pPlayer) pPlayer->AddIdleTask(args[1].GetInt());
	
}
IMPLEMENT_SCRIPT_API(AddIdleTask)
// 
void AddMoveTask(abase::vector<CScriptValue>& args, abase::vector<CScriptValue>& ret)
{
	PlayerObject*pPlayer = NULL;
	CHECK_ARGNUM(5, "PlayerAPI.AddMoveTask");
	GET_LUA_STRING(username, args[0]);
	pPlayer = _GetPlayerObject(username);

	if( args[1].m_Type != CScriptValue::SVT_ARRAY ||
		args[1].m_ArrVal.size() != 3 )
	{
		a_LogOutput(1, "PlayerAPI.AddMoveTask, 错误的参数类型！");
		return;
	}

	A3DVECTOR3 v3;
	v3.x = (float)args[1].m_ArrVal[0].GetDouble();
	v3.y = (float)args[1].m_ArrVal[1].GetDouble();
	v3.z = (float)args[1].m_ArrVal[2].GetDouble();

	if(pPlayer) pPlayer->AddMoveTask(v3,args[2].GetInt(),(float)args[3].GetDouble(),args[4].GetInt());
	
}
IMPLEMENT_SCRIPT_API(AddMoveTask)
// 
void AddStopMoveTask(abase::vector<CScriptValue>& args, abase::vector<CScriptValue>& ret)
{
	PlayerObject*pPlayer = NULL;
	CHECK_ARGNUM(5, "PlayerAPI.AddStopMoveTask");
	GET_LUA_STRING(username, args[0]);
	pPlayer = _GetPlayerObject(username);

	if( args[1].m_Type != CScriptValue::SVT_ARRAY ||
		args[1].m_ArrVal.size() != 3 )
	{
		a_LogOutput(1, "PlayerAPI.AddStopMoveTask, 错误的参数类型！");
		return;
	}

	A3DVECTOR3 v3;
	v3.x = (float)args[1].m_ArrVal[0].GetDouble();
	v3.y = (float)args[1].m_ArrVal[1].GetDouble();
	v3.z = (float)args[1].m_ArrVal[2].GetDouble();
	
	if(pPlayer) pPlayer->AddStopMoveTask(v3,args[2].GetInt(),(float)args[3].GetDouble(),args[4].GetInt());
	
}
IMPLEMENT_SCRIPT_API(AddStopMoveTask)
// 
void AddAttackTask(abase::vector<CScriptValue>& args, abase::vector<CScriptValue>& ret)
{
	PlayerObject*pPlayer = NULL;
	CHECK_ARGNUM(1, "PlayerAPI.AddAttackTask");
	GET_LUA_STRING(username, args[0]);
	pPlayer = _GetPlayerObject(username);
	
	if(pPlayer) pPlayer->AddAttackTask();
	
}
IMPLEMENT_SCRIPT_API(AddAttackTask)
// 
void AddCastSkillTask(abase::vector<CScriptValue>& args, abase::vector<CScriptValue>& ret)
{
	PlayerObject*pPlayer = NULL;
	CHECK_ARGNUM(1, "PlayerAPI.AddCastSkillTask");
	GET_LUA_STRING(username, args[0]);
	pPlayer = _GetPlayerObject(username);
	
	if(pPlayer) pPlayer->AddCastSkillTask();
	
}
IMPLEMENT_SCRIPT_API(AddCastSkillTask)
// 
void ClearTask(abase::vector<CScriptValue>& args, abase::vector<CScriptValue>& ret)
{
	PlayerObject*pPlayer = NULL;
	CHECK_ARGNUM(1, "PlayerAPI.ClearTask");
	GET_LUA_STRING(username, args[0]);
	pPlayer = _GetPlayerObject(username);
	
	if(pPlayer) pPlayer->ClearTask();
	
}
IMPLEMENT_SCRIPT_API(ClearTask)
// 
void HasNextTask(abase::vector<CScriptValue>& args, abase::vector<CScriptValue>& ret)
{
	bool bret = false;
	PlayerObject*pPlayer = NULL;
	CHECK_ARGNUM(1, "PlayerAPI.HasNextTask");
	GET_LUA_STRING(username, args[0]);
	pPlayer = _GetPlayerObject(username);
	
	if(pPlayer) bret = pPlayer->HasNextTask();
	ret.push_back(bret);
}
IMPLEMENT_SCRIPT_API(HasNextTask)
// 
void IsCurTaskEmpty(abase::vector<CScriptValue>& args, abase::vector<CScriptValue>& ret)
{
	bool bret = 0;
	PlayerObject*pPlayer = NULL;
	CHECK_ARGNUM(1, "PlayerAPI.IsCurTaskEmpty");
	GET_LUA_STRING(username, args[0]);
	pPlayer = _GetPlayerObject(username);
	
	if(pPlayer) bret = pPlayer->IsCurTaskEmpty();
	ret.push_back(bret);
}
IMPLEMENT_SCRIPT_API(IsCurTaskEmpty)
// 
void MoveAgentLearn(abase::vector<CScriptValue>& args, abase::vector<CScriptValue>& ret)
{
	PlayerObject*pPlayer = NULL;
	CHECK_ARGNUM(2, "PlayerAPI.MoveAgentLearn");
	GET_LUA_STRING(username, args[0]);
	pPlayer = _GetPlayerObject(username);

	if( args[1].m_Type != CScriptValue::SVT_ARRAY )
	{
		a_LogOutput(1, "PlayerAPI.MoveAgentLearn, 错误的参数类型！");
		return;
	}

	A3DVECTOR3 v3;
	v3.x = (float)args[1].m_ArrVal[0].GetDouble();
	v3.y = (float)args[1].m_ArrVal[1].GetDouble();
	v3.z = (float)args[1].m_ArrVal[2].GetDouble();
	
	if(pPlayer) pPlayer->MoveAgentLearn(v3);
	
}
IMPLEMENT_SCRIPT_API(MoveAgentLearn)
// 
void GetNextMovePos(abase::vector<CScriptValue>& args, abase::vector<CScriptValue>& ret)
{
	PlayerObject* pPlayer = NULL;
	CHECK_ARGNUM(4, "PlayerAPI.GetNextMovePos");
	GET_LUA_STRING(username, args[0]);
	pPlayer = _GetPlayerObject(username);

	if( args[1].m_Type != CScriptValue::SVT_ARRAY || args[1].m_ArrVal.size() != 3 ||
		args[2].m_Type != CScriptValue::SVT_ARRAY || args[2].m_ArrVal.size() != 3 )
	{
		a_LogOutput(1, "PlayerAPI.GetNextMovePos, 错误的参数类型！");
		return;
	}

	A3DVECTOR3 v3_1;
	v3_1.x = (float)args[1].m_ArrVal[0].GetDouble();
	v3_1.y = (float)args[1].m_ArrVal[1].GetDouble();
	v3_1.z = (float)args[1].m_ArrVal[2].GetDouble();
	
	A3DVECTOR3 v3_2;
	v3_2.x = (float)args[2].m_ArrVal[0].GetDouble();
	v3_2.y = (float)args[2].m_ArrVal[1].GetDouble();
	v3_2.z = (float)args[2].m_ArrVal[2].GetDouble();

	A3DVECTOR3 v3_3;
	if( pPlayer )
	{
		bool bRet = pPlayer->GetNextMovePos(v3_1, v3_2, (float)args[3].GetDouble(), v3_3);
		ret.push_back(bRet);

		CScriptValue vNextPos;
		abase::vector<CScriptValue> vPosValues;
		vPosValues.push_back((double)v3_3.x);
		vPosValues.push_back((double)v3_3.y);
		vPosValues.push_back((double)v3_3.z);
		vNextPos.SetArray(vPosValues);
		ret.push_back(vNextPos);
	}
}
IMPLEMENT_SCRIPT_API(GetNextMovePos)
// 
void GetNextMovePosByDir(abase::vector<CScriptValue>& args, abase::vector<CScriptValue>& ret)
{
	PlayerObject* pPlayer = NULL;
	CHECK_ARGNUM(4, "PlayerAPI.GetNextMovePosByDir");
	GET_LUA_STRING(username, args[0]);
	pPlayer = _GetPlayerObject(username);

	if( args[1].m_Type != CScriptValue::SVT_ARRAY || args[1].m_ArrVal.size() != 3 )
	{
		a_LogOutput(1, "PlayerAPI.GetNextMovePos, 错误的参数类型！");
		return;
	}

	A3DVECTOR3 v3_1;
	v3_1.x = (float)args[1].m_ArrVal[0].GetDouble();
	v3_1.y = (float)args[1].m_ArrVal[1].GetDouble();
	v3_1.z = (float)args[1].m_ArrVal[2].GetDouble();

	A3DVECTOR3 v3_2;
	if( pPlayer )
	{
		bool bRet = pPlayer->GetNextMovePos(v3_1, args[2].GetInt(), (float)args[3].GetDouble(), v3_2);
		ret.push_back(bRet);

		CScriptValue vNextPos;
		abase::vector<CScriptValue> vPosValues;
		vPosValues.push_back((double)v3_2.x);
		vPosValues.push_back((double)v3_2.y);
		vPosValues.push_back((double)v3_2.z);
		vNextPos.SetArray(vPosValues);
		ret.push_back(vNextPos);
	}
}
IMPLEMENT_SCRIPT_API(GetNextMovePosByDir)
// 
void IsMoving(abase::vector<CScriptValue>& args, abase::vector<CScriptValue>& ret)
{
	bool bMove = false;
	PlayerObject*pPlayer = NULL;
	CHECK_ARGNUM(1, "PlayerAPI.IsMoving");
	GET_LUA_STRING(username, args[0]);
	pPlayer = _GetPlayerObject(username);
	
	if(pPlayer)
		bMove = pPlayer->IsMoving();
	ret.push_back(bMove);
}
IMPLEMENT_SCRIPT_API(IsMoving)

// 玩家使用包裹内的物品
void UseItem(abase::vector<CScriptValue>& args, abase::vector<CScriptValue>& ret)
{
	CHECK_ARGNUM(2, "PlayerAPI.UseItem");
	GET_LUA_STRING(username, args[0]);
	PlayerObject* pPlayer = _GetPlayerObject(username);

	if( pPlayer )
	{
		int iSlot = args[1].GetInt();
		pPlayer->UseItem(iSlot);
	}
}
IMPLEMENT_SCRIPT_API(UseItem)

// 组队邀请
void TeamInvite(abase::vector<CScriptValue>& args, abase::vector<CScriptValue>& ret)
{
	CHECK_ARGNUM(2, "PlayerAPI.TeamInvite");
	GET_LUA_STRING(username, args[0]);
	PlayerObject* pPlayer = _GetPlayerObject(username);

	if( pPlayer )
		pPlayer->TeamInvite(args[1].GetInt());
}
IMPLEMENT_SCRIPT_API(TeamInvite)

// 自动组队
void DoAutoTeam(abase::vector<CScriptValue>& args, abase::vector<CScriptValue>& ret)
{
	CHECK_ARGNUM(4, "PlayerAPI.DoAutoTeam");
	GET_LUA_STRING(username, args[0]);
	PlayerObject* pPlayer = _GetPlayerObject(username);

	if( pPlayer )
		pPlayer->DoAutoTeam(args[1].GetInt(), args[2].GetInt(), args[3].GetInt());
}
IMPLEMENT_SCRIPT_API(DoAutoTeam)

/////////////////////////////////////////////////
//
struct PlayerApi : public LuaAPIBase
{
	PlayerApi()
	{
		LuaFuncFactory::GetSingleton().AddAPI(this);
	}

	virtual void Register()
	{
		const char*pnamespace = "PlayerAPI";
		LUA_CS_REGISTER_API(pnamespace, GetLevel,		"角色等级。参数(username)"   );
		LUA_CS_REGISTER_API(pnamespace, GetLevel2,		"角色修真等级。参数(username)");
		LUA_CS_REGISTER_API(pnamespace, GetHP,			"角色HP。参数(username)"   );
		LUA_CS_REGISTER_API(pnamespace, GetMP,			"角色MP。参数(username)"   );
		LUA_CS_REGISTER_API(pnamespace, GetMaxHP,		"角色MaxHP。参数(username)"   );
		LUA_CS_REGISTER_API(pnamespace, GetRoleID,		"角色ID。参数(username)"   );
		LUA_CS_REGISTER_API(pnamespace, IsDead,			"角色是否死亡。参数(username)"   );
		LUA_CS_REGISTER_API(pnamespace, GetCurPos,		"角色当前位置。参数(username)，返回值是(x,y,z)"   );
		LUA_CS_REGISTER_API(pnamespace, GetDirection,	"角色方向。参数(username)"   );
		LUA_CS_REGISTER_API(pnamespace, GetRunSpeed,	"角色速度。参数(username)"   );
		LUA_CS_REGISTER_API(pnamespace, GetSelectedTarget,"角色选中目标id。参数(username)"   );
		LUA_CS_REGISTER_API(pnamespace, GetForceAttack,	"角色是否强制攻击标记。参数(username)"   );
		LUA_CS_REGISTER_API(pnamespace, GetSkillNum,	"角色技能数目。参数(username)"   );
		LUA_CS_REGISTER_API(pnamespace, GetSkill,		"获取角色指定的技能。参数(索引)");
		LUA_CS_REGISTER_API(pnamespace, GetSkillByID,	"获取角色指定的技能。参数(技能ID)");
		LUA_CS_REGISTER_API(pnamespace, GetProfession,	"角色职业。参数(username)");
		LUA_CS_REGISTER_API(pnamespace, GetRoleName,	"角色名字。参数(username)");
		LUA_CS_REGISTER_API(pnamespace, GetMoney,		"角色金钱。参数(username)");
		LUA_CS_REGISTER_API(pnamespace, GetExp,			"角色经验值。参数(username)");
		LUA_CS_REGISTER_API(pnamespace, GetSP,			"角色技能点。参数(username)");
		LUA_CS_REGISTER_API(pnamespace, IsChangingFace,	"角色是否吃了变形丸。参数(username)");
		LUA_CS_REGISTER_API(pnamespace, IsMale,			"角色是否为男性。参数(username)");
		LUA_CS_REGISTER_API(pnamespace, IsInFashion,	"角色是否处于时装模式。参数(username)");

		LUA_CS_REGISTER_API(pnamespace, SetDirection,     "设置角色方向。参数(username,byDir)"   );
		LUA_CS_REGISTER_API(pnamespace, SetForceAttack,   "设置角色强制攻击状态。参数(username,bForce)"   );
		LUA_CS_REGISTER_API(pnamespace, SelectAttackSkill,   "随机选取一个可用的攻击技能。参数(username)，返回值是(skillid,sqr_range)"   );

		///////////////////////////////////////////////////////////////////////////
		// 包裹相关
		LUA_CS_REGISTER_API(pnamespace, GetItemCount ,     "获取指定物品的个数。参数(username,packageType,tid)"   );
		LUA_CS_REGISTER_API(pnamespace, GetItemIndex ,     "获取指定物品的索引。参数(username,packageType,tid)"   );
		LUA_CS_REGISTER_API(pnamespace, IsAnyEquipExist,   "检测插槽是否存在某装备。参数(username, index)");
		LUA_CS_REGISTER_API(pnamespace, IsEquipExist ,     "检测指定装备是否存在。参数(username,tid,index)"   );
		LUA_CS_REGISTER_API(pnamespace, IsItemExist  ,     "检测物品是否存在。参数(username,tid)"   );
		LUA_CS_REGISTER_API(pnamespace, GetEmptySlot ,     "获取背包内空的插槽。参数(username)"   );
		
		///////////////////////////////////////////////////////////////////////////
		// 目标对象
		LUA_CS_REGISTER_API(pnamespace, MatterCanPickup ,     "东西是否能被捡起。参数(username,tid,pile_limit)"   );
		LUA_CS_REGISTER_API(pnamespace, MatterCanGather ,     "东西是否能被采集。参数(username,tid)"   );
		LUA_CS_REGISTER_API(pnamespace, NpcCanAttack    ,     "NPC能否被攻击。参数(username,tid)"   );
		LUA_CS_REGISTER_API(pnamespace, PlayerCanAttack ,     "玩家能否被攻击。参数(username,id)"   );

		///////////////////////////////////////////////////////////////////////////
		// 宠物相关
		LUA_CS_REGISTER_API(pnamespace, GetPetCount,		"获取玩家的宠物个数。参数(username)");
		LUA_CS_REGISTER_API(pnamespace, GetPetData,			"获取玩家指定的宠物数据。参数(username, 宠物的索引)");

		///////////////////////////////////////////////////////////////////////////
		// C2S Command
		LUA_CS_REGISTER_API(pnamespace, SelectTarget,     "设置角色强制攻击状态。参数(username,targetID)"   );
		LUA_CS_REGISTER_API(pnamespace, SendDebugCmd,     "发送debug命令。参数(username,cmd,param1[,param2])"   );
		LUA_CS_REGISTER_API(pnamespace, Unselect               ,     "取消选中。参数(username)"   );
		LUA_CS_REGISTER_API(pnamespace, NormalAttack           ,     "普通攻击。参数(username,force_attack)"   );
		LUA_CS_REGISTER_API(pnamespace, CastSkill              ,     "发技能。参数(username,skill_id,force_attack,target_count,target1[,target2...])"   );
		LUA_CS_REGISTER_API(pnamespace, Pickup                 ,     "捡东西。参数(username,id,tid)"   );
		LUA_CS_REGISTER_API(pnamespace, EquipItem              ,     "装备物品。参数(username,inv_index,eq_index)"   );
		LUA_CS_REGISTER_API(pnamespace, GetInventoryDetail     ,     "获取物品详细信息。参数(username,packageid)"   );
		LUA_CS_REGISTER_API(pnamespace, ResurrectByItem        ,     "使用复活卷轴。参数(username)"   );
		LUA_CS_REGISTER_API(pnamespace, ResurrectInTown        ,     "回城复活。参数(username)"   );
		LUA_CS_REGISTER_API(pnamespace, CheckSecurityPassword  ,     "检测仓库密码。参数(username)"   );
		LUA_CS_REGISTER_API(pnamespace, CancelAction           ,     "取消上一动作。参数(username)"   );
		LUA_CS_REGISTER_API(pnamespace, Logout                 ,     "登出玩家。参数(username)"   );
		LUA_CS_REGISTER_API(pnamespace, Speak                  ,     "玩家讲话。参数(username,strText)"   );
		LUA_CS_REGISTER_API(pnamespace, Sitdown                ,     "打坐。参数(username,bSitdown)"   );
		LUA_CS_REGISTER_API(pnamespace, TalkToNPC,		"和指定NPC对话，开始NPC服务。参数(username, NPC的ID)");
		LUA_CS_REGISTER_API(pnamespace, LearnSkill,		"玩家学习技能。参数(username, 技能ID)");
		LUA_CS_REGISTER_API(pnamespace, Booth,			"开始摆摊或结束摆摊。参数(username, 开始摆摊(true or false), 摊位名字(可选))");
		LUA_CS_REGISTER_API(pnamespace, PlayAction,		"玩家表演动作。参数(username, 动作ID)");
		LUA_CS_REGISTER_API(pnamespace, Goto,			"玩家在当前地图瞬移。参数(username, X, Z)");
		LUA_CS_REGISTER_API(pnamespace, RandomMove,		"玩家在当前地图范围内随机瞬移。参数(username)");
		LUA_CS_REGISTER_API(pnamespace, UseItem,		"玩家使用包裹内的物品。参数(username, 物品所在包裹内的Slot)");
		LUA_CS_REGISTER_API(pnamespace, SwitchFashion,	"玩家切换时装模式。参数(username, true or false)");
		LUA_CS_REGISTER_API(pnamespace, SummonPet,		"玩家召唤指定宠物。参数(username, 宠物在列表中的索引)");
		LUA_CS_REGISTER_API(pnamespace, BanishPet,		"玩家召回指定宠物。参数(username, 宠物在列表中的索引)");
		LUA_CS_REGISTER_API(pnamespace, AttachBuddy,	"玩家拥抱指定玩家。参数(username, 被拥抱者的RoleID)");
		LUA_CS_REGISTER_API(pnamespace, DetachBuddy,	"玩家解除相依相偎。参数(username)");
		LUA_CS_REGISTER_API(pnamespace, TeamInvite,		"邀请指定玩家组队。参数(username, 被邀请的玩家ID)");
		LUA_CS_REGISTER_API(pnamespace, DoAutoTeam,		"开始自动组队。参数(username, 活动ID, 操作类型(1发起，0取消))");

		////////////////////////////////////////////////
		// 行为相关
		LUA_CS_REGISTER_API(pnamespace, SelectMatter       ,     "选取待捡的物品。参数(username,Matter ID)"   );
		LUA_CS_REGISTER_API(pnamespace, SetAIPolicy        ,     "更改玩家AI策略。参数(username,name)"   );
		LUA_CS_REGISTER_API(pnamespace, AddIdleTask        ,     "加入一个空闲等待任务。参数(username,timeInterval)"   );
		LUA_CS_REGISTER_API(pnamespace, AddMoveTask        ,     "加入一个移动任务。参数(username,x,y,z,time,speed,mode)"   );
		LUA_CS_REGISTER_API(pnamespace, AddStopMoveTask    ,     "停止移动任务。参数(username,x,y,z,time,speed,mode)"   );
		LUA_CS_REGISTER_API(pnamespace, AddAttackTask      ,     "普通攻击任务。参数(username)"   );
		LUA_CS_REGISTER_API(pnamespace, AddCastSkillTask   ,     "技能攻击任务。参数(username)"   );
		LUA_CS_REGISTER_API(pnamespace, ClearTask          ,     "清除任务。参数(username)"   );
		LUA_CS_REGISTER_API(pnamespace, HasNextTask        ,     "是否有下个任务。参数(username)"   );
		LUA_CS_REGISTER_API(pnamespace, IsCurTaskEmpty     ,     "当前任务为空。参数(username)"   );
		LUA_CS_REGISTER_API(pnamespace, MoveAgentLearn     ,     "agent尝试移动点。参数(username,x,y,z)"   );
		LUA_CS_REGISTER_API(pnamespace, GetNextMovePos     ,     "获得下次移动的点。参数(username,vPos,vDest,fRange) 返回(vNextPos)"   );
		LUA_CS_REGISTER_API(pnamespace, GetNextMovePosByDir,     "获得下次移动的点（通过方向）。参数(username,vPos,dir,fRange) 返回(vNextPos)"   );
		LUA_CS_REGISTER_API(pnamespace, IsMoving           ,     "是否移动中。参数(username)"   );

		// 全部一起注册
		LuaFuncFactory::GetSingleton().Flash();
	}
};

// 产生一个实例，以便能够注册API
static PlayerApi g_PlayerApi;

}