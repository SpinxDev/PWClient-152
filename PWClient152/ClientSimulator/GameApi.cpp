/*
 * FILE: GameApi.cpp
 *
 * DESCRIPTION: 本文件定义了一组API到LUA脚本
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
#include "MoveAgent.h"
#include "EC_Skill.h"


///////////////////////////////////////////////////////////////////////////
//  
//  Define and Macro
//  
///////////////////////////////////////////////////////////////////////////

#define GET_LUA_STRING(dest, val) AString dest; val.RetrieveAString(dest)
#define GET_LUA_WSTRING(dest, val) AWString dest; val.RetrieveAWString(dest)
#define SET_LUA_TABLE(key, value) val.SetVal(key); keyvec.push_back(val); val.SetVal(value); valvec.push_back(val);

#define CHECK_ARGNUM(n, func) if( args.size() != n ) { a_LogOutput(1, func ", 脚本调用错误 - 应该包含%d个参数！", (n)); return; }


///////////////////////////////////////////////////////////////////////////
//  
//  Types and Global functions
//  
///////////////////////////////////////////////////////////////////////////

static GameObject* _GetGameObject(const char* username)
{
	GameSession* pSession = SessionManager::GetSingleton().GetSessionByName(username);
	return pSession ? pSession->GetGameObject() : NULL;
}


///////////////////////////////////////////////////////////////////////////
//  
//  Lua API Functions
//  
///////////////////////////////////////////////////////////////////////////
namespace LuaBind
{

/** 
param: username
param: int   iTargetType 
param: float fRange 
*/
void GatherTarget(abase::vector<CScriptValue>& args, abase::vector<CScriptValue>& ret)
{
	int iret = -1;
	CHECK_ARGNUM(3, "GameAPI.GatherTarget");

	GET_LUA_STRING(username, args[0]);
	GameObject* pGame = _GetGameObject(username);
	if( pGame )
		iret = pGame->GatherTarget(args[1].GetInt(), (float)args[2].GetDouble(), pGame->GetHostPlayer()->GetForceAttack());

	ret.push_back((double)(iret));
}

/** 
param: username
param: float fRange
*/
void GatherTargetAll(abase::vector<CScriptValue>& args, abase::vector<CScriptValue>& ret)
{
	int iret = -1;
	CHECK_ARGNUM(2, "GameAPI.GatherTargetAll");

	GET_LUA_STRING(username, args[0]);
	GameObject*pGame = _GetGameObject(username);
	if( pGame )
		iret = pGame->GatherTarget((float)args[1].GetDouble(), pGame->GetHostPlayer()->GetForceAttack());

	ret.push_back((double)(iret));
}

/** 地上物品不可捡，加入列表
param: username
param: matter id
*/
void AddInvalidMatter(abase::vector<CScriptValue>& args, abase::vector<CScriptValue>& ret)
{
	CHECK_ARGNUM(2, "GameAPI.AddInvalidMatter");

	GET_LUA_STRING(username, args[0]);
	GameObject* pGame = _GetGameObject(username);
	if( pGame )
	{
		pGame->AddInvalidMatter(args[1].GetInt());
	}
}

/** 获取世界中指定对象的的属性(ElsePlayer, NPC, Matter)
*/
void GetEntity(abase::vector<CScriptValue>& args, abase::vector<CScriptValue>& ret)
{
	CHECK_ARGNUM(2, "GameAPI.GetEntity");

	GET_LUA_STRING(username, args[0]);
	GameObject* pGame = _GetGameObject(username);
	if( pGame )
	{
		GameObject::ObjectInfo* pObj = pGame->GetEntity(args[1].GetInt());
		if( pObj )
		{
			CScriptValue val, objInfo;
			abase::vector<CScriptValue> keyvec, valvec;

			SET_LUA_TABLE("id", pObj->id);
			SET_LUA_TABLE("hp", pObj->hp);
			SET_LUA_TABLE("dead", pObj->dead);
			SET_LUA_TABLE("tid", pObj->tid);
			SET_LUA_TABLE("pos_x", pObj->pos.x);
			SET_LUA_TABLE("pos_y", pObj->pos.y);
			SET_LUA_TABLE("pos_z", pObj->pos.z);

			objInfo.SetArray(valvec, keyvec);
			ret.push_back(objInfo);
		}
	}
}

/** 根据TID，当前视野中是否存在该NPC，并返回ID
*/
void SearchNPC(abase::vector<CScriptValue>& args, abase::vector<CScriptValue>& ret)
{
	int iret = -1;
	CHECK_ARGNUM(2, "GameAPI.SearchNPC");

	GET_LUA_STRING(username, args[0]);
	GameObject* pGame = _GetGameObject(username);
	if( pGame )
		iret = pGame->SearchNPC(args[1].GetInt());

	ret.push_back((double)iret);
}

/** 获取玩家可到达的区域
*/
void GetRegion(abase::vector<CScriptValue>& args, abase::vector<CScriptValue>& ret)
{
	A3DVECTOR3 vMin, vMax;
	MoveAgent::GetSingleton().GetRegion(vMin, vMax);

	CScriptValue val, pos;
	abase::vector<CScriptValue> keyvec, valvec;

	SET_LUA_TABLE("min_x", vMin.x);
	SET_LUA_TABLE("min_z", vMin.z);
	SET_LUA_TABLE("max_x", vMax.x);
	SET_LUA_TABLE("max_z", vMax.z);
	pos.SetArray(valvec, keyvec);

	ret.push_back(pos);
}

/** 获取指定职业的技能列表
*/
void GetSkillList(abase::vector<CScriptValue>& args, abase::vector<CScriptValue>& ret)
{
	CHECK_ARGNUM(1, "GameAPI.GetSkillList");

	abase::vector<int> skills;
	int iProfession = args[0].GetInt();
	if( iProfession < PROF_WARRIOR || iProfession >= NUM_PROFESSION )
	{
		a_LogOutput(1, "GameAPI.GetSkillList, 错误的职业类型！");
		return;
	}

	CScriptValue tabSkill;
	abase::vector<CScriptValue> skillArray;
	EnumSkillList(iProfession, skills);
	for( size_t i=0;i<skills.size();i++ )
		skillArray.push_back((double)skills[i]);
	tabSkill.SetArray(skillArray);
	ret.push_back(tabSkill);
}

// 获取指定物体的类型，0:Player, 1:NPC, 2:Matter
void GetIDType(abase::vector<CScriptValue>& args, abase::vector<CScriptValue>& ret)
{
	CHECK_ARGNUM(1, "GameAPI.GetIDType");

	int id = args[0].GetInt();
	if( ISPLAYERID(id) )
		ret.push_back(0.0);
	else if( ISNPCID(id) )
		ret.push_back(1.0);
	else if( ISMATTERID(id) )
		ret.push_back(2.0);
}

// 获取技能的最大等级
void GetSkillMaxLevel(abase::vector<CScriptValue>& args, abase::vector<CScriptValue>& ret)
{
	CHECK_ARGNUM(1, "GameAPI.GetSkillMaxLevel");

	CECSkill skill(args[0].GetInt(), 1);
	ret.push_back((double)skill.GetMaxLevel());
}


IMPLEMENT_SCRIPT_API(GatherTarget)
IMPLEMENT_SCRIPT_API(GatherTargetAll)
IMPLEMENT_SCRIPT_API(AddInvalidMatter)
IMPLEMENT_SCRIPT_API(GetEntity)
IMPLEMENT_SCRIPT_API(SearchNPC)
IMPLEMENT_SCRIPT_API(GetRegion)
IMPLEMENT_SCRIPT_API(GetSkillList)
IMPLEMENT_SCRIPT_API(GetIDType)
IMPLEMENT_SCRIPT_API(GetSkillMaxLevel)

struct GameApi : public LuaAPIBase
{
	GameApi()
	{
		LuaFuncFactory::GetSingleton().AddAPI(this);
	}

	virtual void Register()
	{
		const char*pnamespace = "GameAPI";
		LUA_CS_REGISTER_API(pnamespace, GatherTarget,		"在指定范围内寻找一个目标(ElsePlayer 0, NPC 1, Matter 2)。参数(username,targetMask,fRange)"   );
		LUA_CS_REGISTER_API(pnamespace, GatherTargetAll,	"在指定范围内寻找一个目标(ElsePlayer, NPC, Matter)。参数(username,fRange)"   );
		LUA_CS_REGISTER_API(pnamespace, AddInvalidMatter,	"物品不可捡取，将其记录。参数(username, matter id)");
		LUA_CS_REGISTER_API(pnamespace, GetEntity,			"获取指定物体的信息(其他玩家，NPC，地上的物品)。参数(username, object id)");
		LUA_CS_REGISTER_API(pnamespace, SearchNPC,			"根据TID查找指定的NPC。参数(username, tid)");
		LUA_CS_REGISTER_API(pnamespace, GetRegion,			"获取玩家可以活动的区域。返回值(minx, minz, maxx, maxz)");
		LUA_CS_REGISTER_API(pnamespace, GetSkillList,		"获取指定职业的技能列表。参数(职业ID) 返回值(技能列表)");
		LUA_CS_REGISTER_API(pnamespace, GetIDType,			"获取指定物体ID的类型。参数(物体ID) 返回值(0:Player, 1:NPC, 2:Matter)");
		LUA_CS_REGISTER_API(pnamespace, GetSkillMaxLevel,	"获取指定技能的最大等级。返回值(指定技能的最大等级)");
		
		// 全部一起注册
		LuaFuncFactory::GetSingleton().Flash();
	}
};

// 产生一个实例，以便能够注册API
static GameApi g_GameApi;

}