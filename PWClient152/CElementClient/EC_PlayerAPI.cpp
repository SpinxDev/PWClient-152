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

/** ֹͣ����
*/
void StopPolicy(abase::vector<CScriptValue>& args, abase::vector<CScriptValue>& ret)
{
	CHECK_PLAYERWRAPPER();
	pPlayer->StopPolicy();
}
IMPLEMENT_SCRIPT_API(StopPolicy)

/** �����Ϊ�Ƿ��Ѿ�������
*/
void HaveAction(abase::vector<CScriptValue>& args, abase::vector<CScriptValue>& ret)
{
	CHECK_PLAYERWRAPPER();
	ret.push_back(pPlayer->HaveAction());
}
IMPLEMENT_SCRIPT_API(HaveAction)

/** ���һ��������Ϊ
@param ����ʱ��
*/
void AddIdleAction(abase::vector<CScriptValue>& args, abase::vector<CScriptValue>& ret)
{
	CHECK_PLAYERWRAPPER();
	CHECK_ARGNUM(AddIdleAction, 1);

	int iTime = args[0].GetInt();
	pPlayer->AddIdleAction(iTime);
}
IMPLEMENT_SCRIPT_API(AddIdleAction)

/** ��ȡ��Ʒ����
@param ������ID��������װ���������������
@param ��Ʒ��TID
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

/** ��ȡ��Ʒ�ĸ���
@param ������ID��������װ���������������
@param ��Ʒ��TID
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

/** ��ȡ��ҵ�λ��
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

/** �ƶ���ָ��λ��
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

/** ȡ����ǰ��Ϊ
*/
void CancelAction(abase::vector<CScriptValue>& args, abase::vector<CScriptValue>& ret)
{
	CHECK_PLAYERWRAPPER();
	pPlayer->CancelAction();
}
IMPLEMENT_SCRIPT_API(CancelAction)

/** ��Ѱһ��Ŀ��
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

/** ��ȡ��ǰѡ�е�Ŀ��
*/
void GetSelectedTarget(abase::vector<CScriptValue>& args, abase::vector<CScriptValue>& ret)
{
	CHECK_PLAYERWRAPPER();
	ret.push_back((double)pPlayer->GetSelectedTarget());
}
IMPLEMENT_SCRIPT_API(GetSelectedTarget)

/** ѡȡȡָ��Ŀ��
@param Ŀ��ID
*/
void SelectTarget(abase::vector<CScriptValue>& args, abase::vector<CScriptValue>& ret)
{
	CHECK_PLAYERWRAPPER();
	CHECK_ARGNUM(SelectTarget, 1);

	int iTarget = args[0].GetInt();
	pPlayer->SelectTarget(iTarget);
}
IMPLEMENT_SCRIPT_API(SelectTarget)

/** ȡ��ѡ��
*/
void Unselect(abase::vector<CScriptValue>& args, abase::vector<CScriptValue>& ret)
{
	CHECK_PLAYERWRAPPER();
	pPlayer->Unselect();
}
IMPLEMENT_SCRIPT_API(Unselect)

/** �ж�ѡȡ�����ܷ񱻹���
@param ����ID
*/
void NpcCanAttack(abase::vector<CScriptValue>& args, abase::vector<CScriptValue>& ret)
{
	CHECK_PLAYERWRAPPER();
	CHECK_ARGNUM(NpcCanAttack, 1);

	int iMonster = args[0].GetInt();
	ret.push_back(pPlayer->NpcCanAttack(iMonster));
}
IMPLEMENT_SCRIPT_API(NpcCanAttack)

/** ʩ����ϼ���
@param ��ϼ���ID
@Param �Ƿ�����չ���ѭ���ı��
@Param �Ƿ��ӳ٣������������ڸ������ܵ�ʩ��
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

/** ʩ�ż���
@Param ����ID
@Param �Ƿ��ӳ٣������������ڸ������ܵ�ʩ��
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

/** �����չ�
*/
void NormalAttack(abase::vector<CScriptValue>& args, abase::vector<CScriptValue>& ret)
{
	CHECK_PLAYERWRAPPER();
	ret.push_back(pPlayer->NormalAttack());
}
IMPLEMENT_SCRIPT_API(NormalAttack)

/** ʹ����Ʒ
@param ��Ʒ�ڰ����е�����
@param ��Ʒ��TID
*/
void UseItem(abase::vector<CScriptValue>& args, abase::vector<CScriptValue>& ret)
{
	CHECK_PLAYERWRAPPER();
	CHECK_ARGNUM(UseItem, 1);

	int iSlot = args[0].GetInt();
	pPlayer->UseItem(iSlot);
}
IMPLEMENT_SCRIPT_API(UseItem)

/** �ж���Ʒ�Ƿ���Լ�ȡ
@param ��Ʒ��ID
@param ��Ʒ��TID
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

/** ��ȡ��Ʒ
@param ��Ʒ��ID
*/
void Pickup(abase::vector<CScriptValue>& args, abase::vector<CScriptValue>& ret)
{
	CHECK_PLAYERWRAPPER();
	CHECK_ARGNUM(Pickup, 1);

	int tid = args[0].GetInt();
	pPlayer->Pickup(tid);
}
IMPLEMENT_SCRIPT_API(Pickup)

/** �ж�����Ƿ��뿪��Ұ
@param ���ID
*/
void IsPlayerInSlice(abase::vector<CScriptValue>& args, abase::vector<CScriptValue>& ret)
{
	CHECK_PLAYERWRAPPER();
	CHECK_ARGNUM(IsPlayerInSlice, 1);

	int idPlayer = args[0].GetInt();
	ret.push_back(pPlayer->IsPlayerInSlice(idPlayer));
}
IMPLEMENT_SCRIPT_API(IsPlayerInSlice)

/** ��ȡ�����־�
*/
void GetWeaponEndurance(abase::vector<CScriptValue>& args, abase::vector<CScriptValue>& ret)
{
	CHECK_PLAYERWRAPPER();
	ret.push_back((double)pPlayer->GetWeaponEndurance());
}
IMPLEMENT_SCRIPT_API(GetWeaponEndurance)

/** ����Ƿ�����
*/
void IsDead(abase::vector<CScriptValue>& args, abase::vector<CScriptValue>& ret)
{
	CHECK_PLAYERWRAPPER();
	ret.push_back(pPlayer->IsDead());
}
IMPLEMENT_SCRIPT_API(IsDead)

/** �Ƿ��������ʩ�����
*/
void IsRevivedByOther(abase::vector<CScriptValue>& args, abase::vector<CScriptValue>& ret)
{
	CHECK_PLAYERWRAPPER();
	ret.push_back(pPlayer->IsRevivedByOther());
}
IMPLEMENT_SCRIPT_API(IsRevivedByOther)

/** ��������ʩ������
*/
void AcceptRevive(abase::vector<CScriptValue>& args, abase::vector<CScriptValue>& ret)
{
	CHECK_PLAYERWRAPPER();
	pPlayer->AcceptRevive();
}
IMPLEMENT_SCRIPT_API(AcceptRevive)

/** ʹ�ø�����Ḵ��
*/
void ReviveByItem(abase::vector<CScriptValue>& args, abase::vector<CScriptValue>& ret)
{
	CHECK_PLAYERWRAPPER();
	ret.push_back(pPlayer->ReviveByItem());
}
IMPLEMENT_SCRIPT_API(ReviveByItem)

/** �سǸ���
*/
void ReviveInTown(abase::vector<CScriptValue>& args, abase::vector<CScriptValue>& ret)
{
	CHECK_PLAYERWRAPPER();
	pPlayer->ReviveInTown();
}
IMPLEMENT_SCRIPT_API(ReviveInTown)

/** ��ȡ�Զ���ֲ��Ե���������
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

/** ����ǿ�й������
*/
void SetForceAttack(abase::vector<CScriptValue>& args, abase::vector<CScriptValue>& ret)
{
	CHECK_PLAYERWRAPPER();
	CHECK_ARGNUM(SetForceAttack, 1);

	bool bFlag = args[0].GetBool();
	pPlayer->SetForceAttack(bFlag);
}
IMPLEMENT_SCRIPT_API(SetForceAttack)

/** ��ȡ��ҳ�ʼλ��
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

/** ��ȡָ�������λ��
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

/** ����ָ������Ʒ���ɹ������ȡ
*/
void SetInvalidObject(abase::vector<CScriptValue>& args, abase::vector<CScriptValue>& ret)
{
	CHECK_PLAYERWRAPPER();
	CHECK_ARGNUM(SetInvalidObject, 1);

	int object_id = args[0].GetInt();
	pPlayer->SetInvalidObject(object_id);
}
IMPLEMENT_SCRIPT_API(SetInvalidObject)

/** �Ƿ��ڰ�ȫ����
*/
void IsInSanctuary(abase::vector<CScriptValue>& args, abase::vector<CScriptValue>& ret)
{
	CHECK_PLAYERWRAPPER();
	ret.push_back(pPlayer->IsInSanctuary());
}
IMPLEMENT_SCRIPT_API(IsInSanctuary)

/** �Ƿ��й������ڹ�����
*/
void IsMonsterAttackMe(abase::vector<CScriptValue>& args, abase::vector<CScriptValue>& ret)
{
	CHECK_PLAYERWRAPPER();
	ret.push_back(pPlayer->IsMonsterAttackMe());
}
IMPLEMENT_SCRIPT_API(IsMonsterAttackMe)


///////////////////////////////////////////////////////////////////////////

// ע������API
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