/*
 * FILE: PlayerApi.cpp
 *
 * DESCRIPTION: ���ļ�������һ�������ص�API���ű�ʹ��
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

#define CHECK_ARGNUM(n, func) if( args.size() < n ) { a_LogOutput(1, func ", �ű����ô��� - Ӧ�����ٰ���%d��������", (n)); return; }

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

// ��ȡ�������ȼ�
void GetLevel2(abase::vector<CScriptValue>& args, abase::vector<CScriptValue>& ret)
{
	CHECK_ARGNUM(1, "PlayerAPI.GetLevel2");
	GET_LUA_STRING(username, args[0]);
	PlayerObject* pPlayer = _GetPlayerObject(username);
	if( pPlayer )
		ret.push_back((double)pPlayer->GetLevel2());
}
IMPLEMENT_SCRIPT_API(GetLevel2)

/** ��ȡ���ְҵ���
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

/** ����Ƿ�Ϊ����
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

// ��ȡ��ɫ������
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

// ��ȡǮ������
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

// ��ȡ��Ҿ���ֵ
void GetExp(abase::vector<CScriptValue>& args, abase::vector<CScriptValue>& ret)
{
	CHECK_ARGNUM(1, "PlayerAPI.GetExp");
	GET_LUA_STRING(username, args[0]);
	PlayerObject* pPlayer = _GetPlayerObject(username);
	if( pPlayer )
		ret.push_back((double)pPlayer->GetExp());
}
IMPLEMENT_SCRIPT_API(GetExp)

// ��ȡ��ҵļ��ܵ�
void GetSP(abase::vector<CScriptValue>& args, abase::vector<CScriptValue>& ret)
{
	CHECK_ARGNUM(1, "PlayerAPI.GetSP");
	GET_LUA_STRING(username, args[0]);
	PlayerObject* pPlayer = _GetPlayerObject(username);
	if( pPlayer )
		ret.push_back((double)pPlayer->GetSP());
}
IMPLEMENT_SCRIPT_API(GetSP)

// ��ɫ�Ƿ�����
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

// ��ɫ�Ƿ���˱�����
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

// ��ɫ�Ƿ���ʱװģʽ
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

// ��ȡָ���ļ�����Ϣ
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

// ��ȡָ���ļ�����Ϣ
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
// �������
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
// ���ָ������Ƿ��������װ��
void IsAnyEquipExist(abase::vector<CScriptValue>& args, abase::vector<CScriptValue>& ret)
{
	CHECK_ARGNUM(2, "PlayerAPI.IsAnyEquipExist");

	GET_LUA_STRING(username, args[0]);
	PlayerObject* pPlayer = _GetPlayerObject(username);

	if( pPlayer )
		ret.push_back(CScriptValue(pPlayer->IsEquipExist(args[1].GetInt())));
}
IMPLEMENT_SCRIPT_API(IsAnyEquipExist)
// ���ָ������Ƿ����ָ��װ��
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
// Ŀ�����
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

// ���˲�Ƶ���ǰ��ͼĳ��λ��
void Goto(abase::vector<CScriptValue>& args, abase::vector<CScriptValue>& ret)
{
	CHECK_ARGNUM(3, "PlayerAPI.Goto");
	GET_LUA_STRING(username, args[0]);
	PlayerObject* pPlayer = _GetPlayerObject(username);
	if( pPlayer )
		pPlayer->Goto((float)args[1].GetDouble(), (float)args[2].GetDouble());
}
IMPLEMENT_SCRIPT_API(Goto)

// ������˲��
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
		a_LogOutput(1, "PlayerAPI.SendDebugCmd, ����Ĳ���������");
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
		a_LogOutput(1, "PlayerAPI.CastSkill, ����Ĳ������ͣ�");
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

// ��Һ�NPC�Ի�
void TalkToNPC(abase::vector<CScriptValue>& args, abase::vector<CScriptValue>& ret)
{
	CHECK_ARGNUM(2, "PlayerAPI.TalkToNPC");
	GET_LUA_STRING(username, args[0]);
	PlayerObject* pPlayer = _GetPlayerObject(username);
	if( pPlayer )
		pPlayer->TalkToNPC(args[1].GetInt());
}
IMPLEMENT_SCRIPT_API(TalkToNPC)

// ���ѧ����
void LearnSkill(abase::vector<CScriptValue>& args, abase::vector<CScriptValue>& ret)
{
	CHECK_ARGNUM(2, "PlayerAPI.LearnSkill");
	GET_LUA_STRING(username, args[0]);
	PlayerObject* pPlayer = _GetPlayerObject(username);
	if( pPlayer )
		pPlayer->LearnSkill(args[1].GetInt());
}
IMPLEMENT_SCRIPT_API(LearnSkill)

// ��Ұ�̯
void Booth(abase::vector<CScriptValue>& args, abase::vector<CScriptValue>& ret)
{
	if( args.size() < 2 )
	{
		a_LogOutput(1, "PlayerAPI.Booth, ����Ĳ���������");
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

// ��ұ��ݶ���
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

// �л�ʱװ/��ͨģʽ
void SwitchFashion(abase::vector<CScriptValue>& args, abase::vector<CScriptValue>& ret)
{
	CHECK_ARGNUM(2, "PlayerAPI.SwitchFashion");
	GET_LUA_STRING(username, args[0]);
	PlayerObject* pPlayer = _GetPlayerObject(username);
	if( pPlayer )
		pPlayer->SwitchFashion(args[1].GetBool());
}
IMPLEMENT_SCRIPT_API(SwitchFashion)

// �ٻ�����
void SummonPet(abase::vector<CScriptValue>& args, abase::vector<CScriptValue>& ret)
{
	CHECK_ARGNUM(2, "PlayerAPI.SummonPet");
	GET_LUA_STRING(username, args[0]);
	PlayerObject* pPlayer = _GetPlayerObject(username);
	if( pPlayer )
		pPlayer->SummonPet(args[1].GetInt());
}
IMPLEMENT_SCRIPT_API(SummonPet)

// �ٻس���
void BanishPet(abase::vector<CScriptValue>& args, abase::vector<CScriptValue>& ret)
{
	CHECK_ARGNUM(2, "PlayerAPI.BanishPet");
	GET_LUA_STRING(username, args[0]);
	PlayerObject* pPlayer = _GetPlayerObject(username);
	if( pPlayer )
		pPlayer->BanishPet(args[1].GetInt());
}
IMPLEMENT_SCRIPT_API(BanishPet)

// ��ȡ��ҳ�������
void GetPetCount(abase::vector<CScriptValue>& args, abase::vector<CScriptValue>& ret)
{
	CHECK_ARGNUM(1, "PlayerAPI.GetPetCount");
	GET_LUA_STRING(username, args[0]);
	PlayerObject* pPlayer = _GetPlayerObject(username);
	if( pPlayer )
		ret.push_back((double)pPlayer->GetPetCount());
}
IMPLEMENT_SCRIPT_API(GetPetCount)

// ��ȡ��������
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

// ӵ��ָ�����
void AttachBuddy(abase::vector<CScriptValue>& args, abase::vector<CScriptValue>& ret)
{
	CHECK_ARGNUM(2, "PlayerAPI.AttachBuddy");
	GET_LUA_STRING(username, args[0]);
	PlayerObject* pPlayer = _GetPlayerObject(username);
	if( pPlayer )
		pPlayer->AttachBuddy(args[1].GetInt());
}
IMPLEMENT_SCRIPT_API(AttachBuddy)

// �����������
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
// ѡȡ�������Ʒ
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
// �����Ϊ���

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
		a_LogOutput(1, "PlayerAPI.AddMoveTask, ����Ĳ������ͣ�");
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
		a_LogOutput(1, "PlayerAPI.AddStopMoveTask, ����Ĳ������ͣ�");
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
		a_LogOutput(1, "PlayerAPI.MoveAgentLearn, ����Ĳ������ͣ�");
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
		a_LogOutput(1, "PlayerAPI.GetNextMovePos, ����Ĳ������ͣ�");
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
		a_LogOutput(1, "PlayerAPI.GetNextMovePos, ����Ĳ������ͣ�");
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

// ���ʹ�ð����ڵ���Ʒ
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

// �������
void TeamInvite(abase::vector<CScriptValue>& args, abase::vector<CScriptValue>& ret)
{
	CHECK_ARGNUM(2, "PlayerAPI.TeamInvite");
	GET_LUA_STRING(username, args[0]);
	PlayerObject* pPlayer = _GetPlayerObject(username);

	if( pPlayer )
		pPlayer->TeamInvite(args[1].GetInt());
}
IMPLEMENT_SCRIPT_API(TeamInvite)

// �Զ����
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
		LUA_CS_REGISTER_API(pnamespace, GetLevel,		"��ɫ�ȼ�������(username)"   );
		LUA_CS_REGISTER_API(pnamespace, GetLevel2,		"��ɫ����ȼ�������(username)");
		LUA_CS_REGISTER_API(pnamespace, GetHP,			"��ɫHP������(username)"   );
		LUA_CS_REGISTER_API(pnamespace, GetMP,			"��ɫMP������(username)"   );
		LUA_CS_REGISTER_API(pnamespace, GetMaxHP,		"��ɫMaxHP������(username)"   );
		LUA_CS_REGISTER_API(pnamespace, GetRoleID,		"��ɫID������(username)"   );
		LUA_CS_REGISTER_API(pnamespace, IsDead,			"��ɫ�Ƿ�����������(username)"   );
		LUA_CS_REGISTER_API(pnamespace, GetCurPos,		"��ɫ��ǰλ�á�����(username)������ֵ��(x,y,z)"   );
		LUA_CS_REGISTER_API(pnamespace, GetDirection,	"��ɫ���򡣲���(username)"   );
		LUA_CS_REGISTER_API(pnamespace, GetRunSpeed,	"��ɫ�ٶȡ�����(username)"   );
		LUA_CS_REGISTER_API(pnamespace, GetSelectedTarget,"��ɫѡ��Ŀ��id������(username)"   );
		LUA_CS_REGISTER_API(pnamespace, GetForceAttack,	"��ɫ�Ƿ�ǿ�ƹ�����ǡ�����(username)"   );
		LUA_CS_REGISTER_API(pnamespace, GetSkillNum,	"��ɫ������Ŀ������(username)"   );
		LUA_CS_REGISTER_API(pnamespace, GetSkill,		"��ȡ��ɫָ���ļ��ܡ�����(����)");
		LUA_CS_REGISTER_API(pnamespace, GetSkillByID,	"��ȡ��ɫָ���ļ��ܡ�����(����ID)");
		LUA_CS_REGISTER_API(pnamespace, GetProfession,	"��ɫְҵ������(username)");
		LUA_CS_REGISTER_API(pnamespace, GetRoleName,	"��ɫ���֡�����(username)");
		LUA_CS_REGISTER_API(pnamespace, GetMoney,		"��ɫ��Ǯ������(username)");
		LUA_CS_REGISTER_API(pnamespace, GetExp,			"��ɫ����ֵ������(username)");
		LUA_CS_REGISTER_API(pnamespace, GetSP,			"��ɫ���ܵ㡣����(username)");
		LUA_CS_REGISTER_API(pnamespace, IsChangingFace,	"��ɫ�Ƿ���˱����衣����(username)");
		LUA_CS_REGISTER_API(pnamespace, IsMale,			"��ɫ�Ƿ�Ϊ���ԡ�����(username)");
		LUA_CS_REGISTER_API(pnamespace, IsInFashion,	"��ɫ�Ƿ���ʱװģʽ������(username)");

		LUA_CS_REGISTER_API(pnamespace, SetDirection,     "���ý�ɫ���򡣲���(username,byDir)"   );
		LUA_CS_REGISTER_API(pnamespace, SetForceAttack,   "���ý�ɫǿ�ƹ���״̬������(username,bForce)"   );
		LUA_CS_REGISTER_API(pnamespace, SelectAttackSkill,   "���ѡȡһ�����õĹ������ܡ�����(username)������ֵ��(skillid,sqr_range)"   );

		///////////////////////////////////////////////////////////////////////////
		// �������
		LUA_CS_REGISTER_API(pnamespace, GetItemCount ,     "��ȡָ����Ʒ�ĸ���������(username,packageType,tid)"   );
		LUA_CS_REGISTER_API(pnamespace, GetItemIndex ,     "��ȡָ����Ʒ������������(username,packageType,tid)"   );
		LUA_CS_REGISTER_API(pnamespace, IsAnyEquipExist,   "������Ƿ����ĳװ��������(username, index)");
		LUA_CS_REGISTER_API(pnamespace, IsEquipExist ,     "���ָ��װ���Ƿ���ڡ�����(username,tid,index)"   );
		LUA_CS_REGISTER_API(pnamespace, IsItemExist  ,     "�����Ʒ�Ƿ���ڡ�����(username,tid)"   );
		LUA_CS_REGISTER_API(pnamespace, GetEmptySlot ,     "��ȡ�����ڿյĲ�ۡ�����(username)"   );
		
		///////////////////////////////////////////////////////////////////////////
		// Ŀ�����
		LUA_CS_REGISTER_API(pnamespace, MatterCanPickup ,     "�����Ƿ��ܱ����𡣲���(username,tid,pile_limit)"   );
		LUA_CS_REGISTER_API(pnamespace, MatterCanGather ,     "�����Ƿ��ܱ��ɼ�������(username,tid)"   );
		LUA_CS_REGISTER_API(pnamespace, NpcCanAttack    ,     "NPC�ܷ񱻹���������(username,tid)"   );
		LUA_CS_REGISTER_API(pnamespace, PlayerCanAttack ,     "����ܷ񱻹���������(username,id)"   );

		///////////////////////////////////////////////////////////////////////////
		// �������
		LUA_CS_REGISTER_API(pnamespace, GetPetCount,		"��ȡ��ҵĳ������������(username)");
		LUA_CS_REGISTER_API(pnamespace, GetPetData,			"��ȡ���ָ���ĳ������ݡ�����(username, ���������)");

		///////////////////////////////////////////////////////////////////////////
		// C2S Command
		LUA_CS_REGISTER_API(pnamespace, SelectTarget,     "���ý�ɫǿ�ƹ���״̬������(username,targetID)"   );
		LUA_CS_REGISTER_API(pnamespace, SendDebugCmd,     "����debug�������(username,cmd,param1[,param2])"   );
		LUA_CS_REGISTER_API(pnamespace, Unselect               ,     "ȡ��ѡ�С�����(username)"   );
		LUA_CS_REGISTER_API(pnamespace, NormalAttack           ,     "��ͨ����������(username,force_attack)"   );
		LUA_CS_REGISTER_API(pnamespace, CastSkill              ,     "�����ܡ�����(username,skill_id,force_attack,target_count,target1[,target2...])"   );
		LUA_CS_REGISTER_API(pnamespace, Pickup                 ,     "����������(username,id,tid)"   );
		LUA_CS_REGISTER_API(pnamespace, EquipItem              ,     "װ����Ʒ������(username,inv_index,eq_index)"   );
		LUA_CS_REGISTER_API(pnamespace, GetInventoryDetail     ,     "��ȡ��Ʒ��ϸ��Ϣ������(username,packageid)"   );
		LUA_CS_REGISTER_API(pnamespace, ResurrectByItem        ,     "ʹ�ø�����ᡣ����(username)"   );
		LUA_CS_REGISTER_API(pnamespace, ResurrectInTown        ,     "�سǸ������(username)"   );
		LUA_CS_REGISTER_API(pnamespace, CheckSecurityPassword  ,     "���ֿ����롣����(username)"   );
		LUA_CS_REGISTER_API(pnamespace, CancelAction           ,     "ȡ����һ����������(username)"   );
		LUA_CS_REGISTER_API(pnamespace, Logout                 ,     "�ǳ���ҡ�����(username)"   );
		LUA_CS_REGISTER_API(pnamespace, Speak                  ,     "��ҽ���������(username,strText)"   );
		LUA_CS_REGISTER_API(pnamespace, Sitdown                ,     "����������(username,bSitdown)"   );
		LUA_CS_REGISTER_API(pnamespace, TalkToNPC,		"��ָ��NPC�Ի�����ʼNPC���񡣲���(username, NPC��ID)");
		LUA_CS_REGISTER_API(pnamespace, LearnSkill,		"���ѧϰ���ܡ�����(username, ����ID)");
		LUA_CS_REGISTER_API(pnamespace, Booth,			"��ʼ��̯�������̯������(username, ��ʼ��̯(true or false), ̯λ����(��ѡ))");
		LUA_CS_REGISTER_API(pnamespace, PlayAction,		"��ұ��ݶ���������(username, ����ID)");
		LUA_CS_REGISTER_API(pnamespace, Goto,			"����ڵ�ǰ��ͼ˲�ơ�����(username, X, Z)");
		LUA_CS_REGISTER_API(pnamespace, RandomMove,		"����ڵ�ǰ��ͼ��Χ�����˲�ơ�����(username)");
		LUA_CS_REGISTER_API(pnamespace, UseItem,		"���ʹ�ð����ڵ���Ʒ������(username, ��Ʒ���ڰ����ڵ�Slot)");
		LUA_CS_REGISTER_API(pnamespace, SwitchFashion,	"����л�ʱװģʽ������(username, true or false)");
		LUA_CS_REGISTER_API(pnamespace, SummonPet,		"����ٻ�ָ���������(username, �������б��е�����)");
		LUA_CS_REGISTER_API(pnamespace, BanishPet,		"����ٻ�ָ���������(username, �������б��е�����)");
		LUA_CS_REGISTER_API(pnamespace, AttachBuddy,	"���ӵ��ָ����ҡ�����(username, ��ӵ���ߵ�RoleID)");
		LUA_CS_REGISTER_API(pnamespace, DetachBuddy,	"��ҽ���������ˡ�����(username)");
		LUA_CS_REGISTER_API(pnamespace, TeamInvite,		"����ָ�������ӡ�����(username, ����������ID)");
		LUA_CS_REGISTER_API(pnamespace, DoAutoTeam,		"��ʼ�Զ���ӡ�����(username, �ID, ��������(1����0ȡ��))");

		////////////////////////////////////////////////
		// ��Ϊ���
		LUA_CS_REGISTER_API(pnamespace, SelectMatter       ,     "ѡȡ�������Ʒ������(username,Matter ID)"   );
		LUA_CS_REGISTER_API(pnamespace, SetAIPolicy        ,     "�������AI���ԡ�����(username,name)"   );
		LUA_CS_REGISTER_API(pnamespace, AddIdleTask        ,     "����һ�����еȴ����񡣲���(username,timeInterval)"   );
		LUA_CS_REGISTER_API(pnamespace, AddMoveTask        ,     "����һ���ƶ����񡣲���(username,x,y,z,time,speed,mode)"   );
		LUA_CS_REGISTER_API(pnamespace, AddStopMoveTask    ,     "ֹͣ�ƶ����񡣲���(username,x,y,z,time,speed,mode)"   );
		LUA_CS_REGISTER_API(pnamespace, AddAttackTask      ,     "��ͨ�������񡣲���(username)"   );
		LUA_CS_REGISTER_API(pnamespace, AddCastSkillTask   ,     "���ܹ������񡣲���(username)"   );
		LUA_CS_REGISTER_API(pnamespace, ClearTask          ,     "������񡣲���(username)"   );
		LUA_CS_REGISTER_API(pnamespace, HasNextTask        ,     "�Ƿ����¸����񡣲���(username)"   );
		LUA_CS_REGISTER_API(pnamespace, IsCurTaskEmpty     ,     "��ǰ����Ϊ�ա�����(username)"   );
		LUA_CS_REGISTER_API(pnamespace, MoveAgentLearn     ,     "agent�����ƶ��㡣����(username,x,y,z)"   );
		LUA_CS_REGISTER_API(pnamespace, GetNextMovePos     ,     "����´��ƶ��ĵ㡣����(username,vPos,vDest,fRange) ����(vNextPos)"   );
		LUA_CS_REGISTER_API(pnamespace, GetNextMovePosByDir,     "����´��ƶ��ĵ㣨ͨ�����򣩡�����(username,vPos,dir,fRange) ����(vNextPos)"   );
		LUA_CS_REGISTER_API(pnamespace, IsMoving           ,     "�Ƿ��ƶ��С�����(username)"   );

		// ȫ��һ��ע��
		LuaFuncFactory::GetSingleton().Flash();
	}
};

// ����һ��ʵ�����Ա��ܹ�ע��API
static PlayerApi g_PlayerApi;

}