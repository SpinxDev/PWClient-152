/*
 * FILE: GameApi.cpp
 *
 * DESCRIPTION: ���ļ�������һ��API��LUA�ű�
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

#define CHECK_ARGNUM(n, func) if( args.size() != n ) { a_LogOutput(1, func ", �ű����ô��� - Ӧ�ð���%d��������", (n)); return; }


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

/** ������Ʒ���ɼ񣬼����б�
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

/** ��ȡ������ָ������ĵ�����(ElsePlayer, NPC, Matter)
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

/** ����TID����ǰ��Ұ���Ƿ���ڸ�NPC��������ID
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

/** ��ȡ��ҿɵ��������
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

/** ��ȡָ��ְҵ�ļ����б�
*/
void GetSkillList(abase::vector<CScriptValue>& args, abase::vector<CScriptValue>& ret)
{
	CHECK_ARGNUM(1, "GameAPI.GetSkillList");

	abase::vector<int> skills;
	int iProfession = args[0].GetInt();
	if( iProfession < PROF_WARRIOR || iProfession >= NUM_PROFESSION )
	{
		a_LogOutput(1, "GameAPI.GetSkillList, �����ְҵ���ͣ�");
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

// ��ȡָ����������ͣ�0:Player, 1:NPC, 2:Matter
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

// ��ȡ���ܵ����ȼ�
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
		LUA_CS_REGISTER_API(pnamespace, GatherTarget,		"��ָ����Χ��Ѱ��һ��Ŀ��(ElsePlayer 0, NPC 1, Matter 2)������(username,targetMask,fRange)"   );
		LUA_CS_REGISTER_API(pnamespace, GatherTargetAll,	"��ָ����Χ��Ѱ��һ��Ŀ��(ElsePlayer, NPC, Matter)������(username,fRange)"   );
		LUA_CS_REGISTER_API(pnamespace, AddInvalidMatter,	"��Ʒ���ɼ�ȡ�������¼������(username, matter id)");
		LUA_CS_REGISTER_API(pnamespace, GetEntity,			"��ȡָ���������Ϣ(������ң�NPC�����ϵ���Ʒ)������(username, object id)");
		LUA_CS_REGISTER_API(pnamespace, SearchNPC,			"����TID����ָ����NPC������(username, tid)");
		LUA_CS_REGISTER_API(pnamespace, GetRegion,			"��ȡ��ҿ��Ի�����򡣷���ֵ(minx, minz, maxx, maxz)");
		LUA_CS_REGISTER_API(pnamespace, GetSkillList,		"��ȡָ��ְҵ�ļ����б�����(ְҵID) ����ֵ(�����б�)");
		LUA_CS_REGISTER_API(pnamespace, GetIDType,			"��ȡָ������ID�����͡�����(����ID) ����ֵ(0:Player, 1:NPC, 2:Matter)");
		LUA_CS_REGISTER_API(pnamespace, GetSkillMaxLevel,	"��ȡָ�����ܵ����ȼ�������ֵ(ָ�����ܵ����ȼ�)");
		
		// ȫ��һ��ע��
		LuaFuncFactory::GetSingleton().Flash();
	}
};

// ����һ��ʵ�����Ա��ܹ�ע��API
static GameApi g_GameApi;

}