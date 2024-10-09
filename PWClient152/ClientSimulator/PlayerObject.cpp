/*
 * FILE: PlayerObject.cpp
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Shizhenhua, 2012/8/28
 *
 * HISTORY: 
 *
 * Copyright (c) 2011 ACE Studio, All Rights Reserved.
 */


#include "PlayerObject.h"
#include "GameObject.h"
#include "EC_GPDataType.h"
#include "GameSession.h"
#include "SessionManager.h"
#include "EC_SendC2SCmds.h"
#include "EC_Skill.h"
#include "AIObject.h"
#include "MoveAgent.h"
#include <publicchat.hpp>
#include "luaFunc.h"
#include "elementdataman.h"


///////////////////////////////////////////////////////////////////////////
//  
//  Define and Macro
//  
///////////////////////////////////////////////////////////////////////////

#define CHAT_ACHAR_LENGTH 120


///////////////////////////////////////////////////////////////////////////
//  
//  Types and Global variables
//  
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//  
//  �����Ϊ���
//  
///////////////////////////////////////////////////////////////////////////

bool task_idle::Tick(int tick)
{
	if( _imp->HasNextTask() ) return false;	
	return task_base::Tick(tick);
}

bool task_move::StartTask()
{
	// ֱ�Ӿ��ƶ�
	_imp->MoveTo(_dest);
	_imp->IncMoveStamp();
	_imp->PlayerMove(_dest,_time,_speed,_mode);
	_imp->SetMoving(true);
	return true;
}

bool task_stop_move::StartTask()
{
	// ֱ�Ӿ��ƶ�
	_imp->MoveTo(_dest);
	_imp->IncMoveStamp();
	_imp->PlayerStopMove(_dest,_time,_speed,_mode);
	_imp->SetMoving(false);
	return true;
}


///////////////////////////////////////////////////////////////////////////
//  
//  Implement class PlayerObject
//  
///////////////////////////////////////////////////////////////////////////

// ���캯��
PlayerObject::PlayerObject( GameObject* pGame ) : m_pGame(pGame), m_bIsDead(false), m_bChangingFace(false),
	m_iLevel(0), m_iLevel2(0), m_iExp(0), m_iSP(0), m_iHP(0), m_iMaxHP(0), m_iMP(0), m_iMaxMP(0), m_iRoleID(0),
	m_fAttackRange(0.0f), m_fRunSpeed(0.0f), m_nMoney(0), m_nMoneyCapacity(200000000),
	m_vPos(0,0,0), m_wMoveStamp(0), m_bIsMoving(false), m_byDir(0), m_cForceAttack(0),
	m_iSelectTarget(-1), m_pCurTask(NULL), m_iPosError(0), m_bInFashion(false)
{
	m_bInitOK = false;
	m_dwLastTime = GetTickCount();
	m_sScriptAI = "";	// ʹ��ȱʡAI

	if( !(m_pAI = new AIObject(this)) )
	{
		ASSERT(m_pAI);
		a_LogOutput(1, "PlayerObject::PlayerObject(), Create AI object failed!");
	}
}

// ��������
PlayerObject::~PlayerObject()
{
	// ���������б�
	ClearTask();

	if( m_pAI )
	{
		delete m_pAI;
		m_pAI = NULL;
	}

	// ɾ���ű�AIʵ��
	if( !m_sScriptAI.IsEmpty() )
	{
		CScriptValue val;
		abase::vector<CScriptValue> args;
		abase::vector<CScriptValue> ret;
		val.SetVal(m_pGame->GetGameSession()->GetUserName());
		args.push_back(val);
		LuaFuncFactory::GetSingleton().Call("AIManager", "OnExit", args, ret);
	}
}

// ��ʼ�������Ϣ
bool PlayerObject::Init(const S2C::cmd_self_info_1* info)
{
	m_iRoleID = info->cid;
	m_iExp = info->iExp;
	m_vPos = info->pos;
	m_iLevel2 = info->level2;

	// ��ȡ��������
	BYTE* pData = (BYTE*)info + sizeof(S2C::cmd_self_info_1);
	if( info->state & GP_STATE_ADV_MODE )
		pData += sizeof(int) * 2;

	// ��״����
	if( info->state & GP_STATE_SHAPE )
		pData += sizeof(BYTE);

	// ��������
	if( info->state & GP_STATE_EMOTE )
		pData += sizeof(BYTE);

	// ���״̬����
	if( info->state & GP_STATE_EXTEND_PROPERTY )
		pData += sizeof(DWORD) * PLAYER_STATE_COUNT;

	// ����ID
	if( info->state & GP_STATE_FACTION )
		pData += sizeof(int) + sizeof(BYTE);

	// ̯λCRC
	if( info->state & GP_STATE_BOOTH )
		pData += sizeof(BYTE);

	// Effect
	if( info->state & GP_STATE_EFFECT )
	{
		BYTE byNum = *pData;
		pData += sizeof(BYTE);
		short* pEffects = (short*)pData;
		pData += sizeof(short) * byNum;

		elementdataman* pDataMan = SessionManager::GetSingleton().GetDataMan();
		for( BYTE n=0;n<byNum;n++ )
		{
			DATA_TYPE DataType = pDataMan->get_data_type(pEffects[n], ID_SPACE_ESSENCE);
			if( DataType == DT_FACEPILL_ESSENCE )
			{
				m_bChangingFace = true;
				break;
			}
		}
	}

	// �����ȼ�
	if( info->state & GP_STATE_PARIAH )
		pData += sizeof(BYTE);

	// ���
	if( info->state & GP_STATE_IN_MOUNT )
		pData += sizeof(unsigned short) + sizeof(int);

	// ��������
	if( info->state & GP_STATE_IN_BIND )
		pData += sizeof(char) + sizeof(int);

	// ��żID
	if( info->state & GP_STATE_SPOUSE )
		pData += sizeof(int);

	// װ������
	if( info->state & GP_STATE_EQUIPDISABLED )
		pData += sizeof(int);

	// �������
	if( info->state & GP_STATE_PLAYERFORCE )
		pData += sizeof(int);

	// ����һ�������������Ч��
	if( info->state & GP_STATE_MULTIOBJ_EFFECT )
	{
		int iCount = *(int*)pData;
		pData += sizeof(int);

		for( int n=0;n<iCount;n++ )
			pData += sizeof(int) + sizeof(char);
	}

	// ����
	if( info->state & GP_STATE_COUNTRY )
		pData += sizeof(int);

	// ʱװģʽ
	if( info->state & GP_STATE_FASHION )
		m_bInFashion = true;

	// ���Ҷ϶�����������
	if( info->state & GP_STATE_CORPSE )
		m_bIsDead = true;

	return true;
}

// �������AI����
void PlayerObject::SetAIPolicy( const char* name )
{
	CScriptValue val;
	abase::vector<CScriptValue> args;
	abase::vector<CScriptValue> ret;

	if( !m_bInitOK )
	{
		a_LogOutput(1, "PlayerObject::SetAIPolicy, change AI policy failed before player is initialized OK!");
		return;
	}

	if( name )
	{
		if( m_sScriptAI == name )
			return;

		if( m_sScriptAI.IsEmpty() )
		{
			// �뿪ȱʡAI
			m_pAI->Leave();
		}
		else
		{
			// �뿪��ǰAI
			val.SetVal(m_pGame->GetGameSession()->GetUserName());
			args.push_back(val);
			LuaFuncFactory::GetSingleton().Call("AIManager", "OnLeave", args, ret);
		}

		// �����µ�AI
		args.clear(); ret.clear();
		val.SetVal(m_pGame->GetGameSession()->GetUserName());
		args.push_back(val);
		val.SetVal(name);
		args.push_back(val);
		LuaFuncFactory::GetSingleton().Call("AIManager", "OnEnter", args, ret);
		if( !ret[0].GetBool() )
		{
			SetAIPolicy(NULL);
			a_LogOutput(1, "Can't find the specified policy, we will use the default one! User:%s", m_pGame->GetGameSession()->GetUserName());
			return;
		}

		m_sScriptAI = name;
	}
	else
	{
		if( !m_sScriptAI.IsEmpty() )
		{
			// ʹ����뿪��ǰ����
			val.SetVal(m_pGame->GetGameSession()->GetUserName());
			args.push_back(val);
			LuaFuncFactory::GetSingleton().Call("AIManager", "OnLeave", args, ret);

			// ����ȱʡAI����
			m_pAI->Enter();
			m_sScriptAI.Empty();
		}
	}
}

// ִ��AI����
void PlayerObject::AICommand( StringVector& cmd )
{
	if( m_sScriptAI.IsEmpty() )
	{
		m_pAI->Command(cmd);
	}
	else
	{
		CScriptValue val;
		abase::vector<CScriptValue> args;
		abase::vector<CScriptValue> ret;

		// �û���
		val.SetVal(m_pGame->GetGameSession()->GetUserName());
		args.push_back(val);

		// ѹ���������
		CScriptValue cmdLine;
		abase::vector<CScriptValue> cmds;
		for( StringVector::iterator it=cmd.begin();it!=cmd.end();++it )
		{
			val.SetVal(*it);
			cmds.push_back(val);
		}

		cmdLine.SetArray(cmds);
		args.push_back(cmdLine);

		LuaFuncFactory::GetSingleton().Call("AIManager", "OnCommand", args, ret);
	}
}

// ��������߼�
void PlayerObject::Tick()
{
	DWORD dwCurTime = GetTickCount();
	DWORD dwDeltaTime = dwCurTime - m_dwLastTime;
	m_dwLastTime = dwCurTime;

	// ���������Ϊ�б�
	if( m_pCurTask )
	{
		if( !m_pCurTask->Tick(dwDeltaTime) )
		{
			EndCurTask();
			StartTask();
		}

		return;
	}

	if( !m_sScriptAI.IsEmpty() )
	{
		// ����LUA�ű�AI
		CScriptValue val;
		abase::vector<CScriptValue> args;
		abase::vector<CScriptValue> ret;
		val.SetVal(m_pGame->GetGameSession()->GetUserName());
		args.push_back(val);
		LuaFuncFactory::GetSingleton().Call("AIManager", "OnTick", args, ret);
	}
	else
	{
		if( m_pAI ) m_pAI->Tick();
	}
}

///////////////////////////////////////////////////////////////////////////
//  
//  �����Ϊ���
//  
///////////////////////////////////////////////////////////////////////////

// ���һ�� ���еȴ�
bool PlayerObject::AddIdleTask(int iTime)
{
	AddTask(new task_idle(this, iTime));
	return StartTask();
}

// �ƶ�
bool PlayerObject::AddMoveTask(const A3DVECTOR3& vDest, int time, float speed, char mode)
{
	AddTask(new task_move(this, vDest, time, speed, mode));
	return StartTask();
}

// ֹͣ�ƶ�
bool PlayerObject::AddStopMoveTask(const A3DVECTOR3& vDest, int time, float speed, char mode)
{
	AddTask(new task_stop_move(this, vDest, time, speed, mode));
	return StartTask();
}

bool PlayerObject::AddAttackTask()
{
	AddTask(new task_normal_attack(this));
	return StartTask();
}

bool PlayerObject::AddCastSkillTask()
{
	AddTask(new task_cast_skill(this));
	return StartTask();
}

// ���һ����Ϊ
bool PlayerObject::AddTask(task_base* pTask)
{
	if( m_Tasks.size() >= MAX_PLAYER_TASK )
	{
		std::list<task_base*>::iterator it;
		for( it=m_Tasks.begin();it!=m_Tasks.end();++it )
		{
			if( *it ) delete *it;
		}
		m_Tasks.clear();
	}

	m_Tasks.push_back(pTask);
	return m_pCurTask != NULL;
}

// ��ʼһ�������Ϊ
bool PlayerObject::StartTask()
{
	if( m_pCurTask ) return false;

	bool ret = false;
	while( m_Tasks.size() )
	{
		m_pCurTask = m_Tasks.front();
		m_Tasks.pop_front();

		if( (ret = m_pCurTask->StartTask()) )
			break;
		else
			EndCurTask();
	}

	return ret;
}

// ����һ�������Ϊ
bool PlayerObject::EndCurTask()
{
	if( !m_pCurTask )
		return false;

	m_pCurTask->EndTask();
	delete m_pCurTask;
	m_pCurTask = NULL;
	return true;
}

// ��������Ϊ
void PlayerObject::ClearTask()
{
	if( m_pCurTask )
		EndCurTask();

	std::list<task_base*>::iterator it;
	for( it=m_Tasks.begin();it!=m_Tasks.end();++it )
	{
		if( *it ) delete *it;
	}

	m_Tasks.clear();
}

// �ƶ���Ŀ��λ��
void PlayerObject::MoveTo(const A3DVECTOR3& vDest)
{
	A3DVECTOR3 direction = vDest;
	direction -= m_vPos;
	if( direction.SquaredMagnitude() > 1e-6 ) m_byDir = DirConvert(direction);
	m_vPos = vDest;
}

void PlayerObject::MoveAgentLearn(const A3DVECTOR3& vPos)
{
	MoveAgent::GetSingleton().Learn(vPos);
}

bool PlayerObject::GetNextMovePos(const A3DVECTOR3& vPos, const A3DVECTOR3& vDest, float fRange, A3DVECTOR3& vNext)
{
	return MoveAgent::GetSingleton().GetMovePos(vPos, vDest, fRange, vNext);
}

bool PlayerObject::GetNextMovePos(const A3DVECTOR3& vPos, BYTE dir, float fRange, A3DVECTOR3& vNext)
{
	return MoveAgent::GetSingleton().GetMovePos(vPos, dir, fRange, vNext);
}

// ǿ�ƹ���
void PlayerObject::SetForceAttack(bool bForce)
{
	if( bForce )
		m_cForceAttack |= GP_PVPMASK_FORCE;
	else
		m_cForceAttack = 0;
}

// ��ȡ��ҵ�ְҵ
int PlayerObject::GetProfession()
{
	return m_pGame->GetGameSession()->GetProfession();
}

// ����Ƿ�Ϊ����
bool PlayerObject::IsMale() const
{
	return m_pGame->GetGameSession()->GetGender() == GENDER_MALE;
}

// ��ȡ��ҵļ�����Ϣ
PlayerObject::SKILL* PlayerObject::GetSkillByID(int id)
{
	for( size_t i=0;i<m_Skills.size();i++ )
	{
		if( m_Skills[i].id == id )
			return &m_Skills[i];
	}

	return NULL;
}


///////////////////////////////////////////////////////////////////////////
//  
//  ��Ұ������
//  
///////////////////////////////////////////////////////////////////////////

// ���װ������Ƿ��������װ��
bool PlayerObject::IsEquipExist(size_t index)
{
	return index < m_Equips.size() && m_Equips[index].tid != 0;
}

// ���װ������Ƿ����ָ��װ��
bool PlayerObject::IsEquipExist(int tid, size_t index)
{
	return index < m_Equips.size() && m_Equips[index].tid == tid;
}

// �����Ʒ�Ƿ����
bool PlayerObject::IsItemExist(int tid)
{
	UserPackage::iterator it;
	for( it=m_Inventory.begin();it!=m_Inventory.end();++it )
	{
		if( it->tid == tid && it->count > 0 )
			return true;
	}

	return false;
}

// ��ȡ�����ڿյĲ��
int PlayerObject::GetEmptySlot()
{
	for( size_t i=0;i<m_Inventory.size();i++ )
	{
		if( m_Inventory[i].tid == 0 )
			return i;
	}

	return -1;
}

// ��ȡָ����Ʒ�ĸ���
int PlayerObject::GetItemCount(PackType pack, int tid)
{
	int count = 0;
	UserPackage* pk = (pack == PACK_INVENTORY) ? &m_Inventory : &m_Equips;
	for( size_t i=0;i<pk->size();i++ )
	{
		if( (*pk)[i].tid == tid )
			count += (*pk)[i].count;
	}
	return count;
}

// ��ȡָ����Ʒ������
int PlayerObject::GetItemIndex(PackType pack, int tid)
{
	UserPackage* pk = (pack == PACK_EQUIPMENTS) ? &m_Equips : &m_Inventory;
	for( size_t i=0;i<pk->size();i++ )
	{
		if( (*pk)[i].tid == tid )
			return i;
	}

	return -1;
}

// �����Ʒ������
bool PlayerObject::PushItem(int tid, int count, int& slot, int& slot_count)
{
	int iFirstEmpty = -1;
	int iCurCount = count;
	int pile_num = GetItemPileLimit(tid);

	for( size_t i=0;i<m_Inventory.size();i++ )
	{
		if( m_Inventory[i].tid )
		{
			if( m_Inventory[i].tid != tid || m_Inventory[i].count >= (size_t)pile_num )
				continue;

			if( m_Inventory[i].count + iCurCount > (size_t)pile_num  )
			{
				int iAddNum = pile_num - m_Inventory[i].count;
				m_Inventory[i].count += iAddNum;
				iCurCount -= iAddNum;
			}
			else
			{
				m_Inventory[i].count += iCurCount;
				iCurCount = 0;
			}

			if( !iCurCount )
			{
				slot = i;
				slot_count = m_Inventory[i].count;
				return true;
			}
		}
		else if( iFirstEmpty < 0 )
			iFirstEmpty = i;
	}

	if( iFirstEmpty < 0 || !iCurCount )
		return false;

	// ʹ��һ���µĸ���
	ASSERT(iCurCount <= pile_num);
	m_Inventory[iFirstEmpty].tid = tid;
	m_Inventory[iFirstEmpty].count = iCurCount;
	slot = iFirstEmpty;
	slot_count = iCurCount;
	return true;
}

///////////////////////////////////////////////////////////////////////////
//  
//  Ŀ��������
//  
///////////////////////////////////////////////////////////////////////////

// �����Ƿ��ܱ�����
bool PlayerObject::MatterCanPickup(int tid, size_t& pile_limit)
{
	if( tid == 3044 )
	{
		// ��ҳ����������ֵ
		if( m_nMoney >= m_nMoneyCapacity )
			return false;
	}
	else
	{
		int empty_index = -1;
		for( size_t i=0;i<m_Inventory.size();i++ )
		{
			if( m_Inventory[i].tid == 0 )
			{
				empty_index = i;
				break;
			}
		}

		if( empty_index == -1 ) return false;
	}

	return true;
}

// �����Ƿ��ܱ��ɼ�
bool PlayerObject::MatterCanGather(int tid)
{
	return false;
}

// NPC�ܷ񱻹���
bool PlayerObject::NpcCanAttack(int tid)
{
	return true;
}

// ����ܷ񱻹���
bool PlayerObject::PlayerCanAttack(int id)
{
	GameObject::ObjectInfo* pObj = m_pGame->GetPlayer(id);
	if( !pObj ) return false;
	return true;
}

// �Ƿ�Ϊ��������
bool PlayerObject::IsPositiveSkill(int id, int& mp_cost, float& range)
{
	CECSkill skill(id, 1);
	mp_cost = skill.GetRequiredMP();
	range = skill.GetCastRange(0.0f, 0.0f);
	return skill.IsPositiveSkill();
}

// ѡȡһ�����õĹ�������
int PlayerObject::SelectAttackSkill(float& sqr_range)
{
	if( a_Random(0, 2) != 0 )
	{
		// 66%����ʹ�ü���
		if( m_Skills.size() )
		{
			SKILL& sk = m_Skills[a_Random(0, m_Skills.size()-1)];

			if( m_iMP >= sk.mp_cost )
			{
				sqr_range = sk.range * sk.range;
				return sk.id;
			}
		}
	}

	//ʹ����ͨ����
	sqr_range = m_fAttackRange * m_fAttackRange;
	return 0;
}


///////////////////////////////////////////////////////////////////////////
//  
//  C2S Command Functions
//  
///////////////////////////////////////////////////////////////////////////

void PlayerObject::SendGameData(void* buf, size_t size)
{
	GameSession* pSession = m_pGame->GetGameSession();
	pSession->SendGameData(buf, size);
}

///////////////////////////////////////////////////////////////////////////
// ���͵�������

void PlayerObject::SendDebugCmd(short cmd)
{
	SessionManager::GetSingleton().SetTheSession(m_pGame->GetGameSession());
	c2s_SendDebugCmd(cmd, 0);
}

void PlayerObject::SendDebugCmd(short cmd, int param)
{
	SessionManager::GetSingleton().SetTheSession(m_pGame->GetGameSession());
	c2s_SendDebugCmd(cmd, 1, param);
}

void PlayerObject::SendDebugCmd(short cmd, int param1, int param2)
{
	SessionManager::GetSingleton().SetTheSession(m_pGame->GetGameSession());
	c2s_SendDebugCmd(cmd, 2, param1, param2);
}

void PlayerObject::SendDebugCmd(short cmd, void* buf, size_t sz)
{
#pragma pack(1)
	struct DEBUG_CMD
	{
		short cmd;
		int param[5];
	};
#pragma pack()

	if( sz <= sizeof(int)*5 )
	{
		DEBUG_CMD data;
		data.cmd = cmd;
		memcpy(data.param, buf, sz);
		SendGameData(&data, sizeof(short)+sz);
	}
}

///////////////////////////////////////////////////////////////////////////

// ��ҷɵ�ָ��λ��
void PlayerObject::Goto(float x, float z)
{
	A3DVECTOR3 pos(x, 0.0f, z);
	SendDebugCmd(C2S::GOTO, &pos, sizeof(pos));
}

// ������˲��
void PlayerObject::RandomMove()
{
	A3DVECTOR3 pos;
	A3DVECTOR3 vRegionMin, vRegionMax;
	MoveAgent::GetSingleton().GetRegion(vRegionMin, vRegionMax);
	Goto(a_Random(vRegionMin.x, vRegionMax.x), a_Random(vRegionMin.z, vRegionMax.z));
}

// ����ƶ�
void PlayerObject::PlayerMove(A3DVECTOR3& pos, int time, float speed, char mode)
{
	SessionManager::GetSingleton().SetTheSession(m_pGame->GetGameSession());
	c2s_SendCmdPlayerMove(pos, pos, time, speed*256.0f, mode, m_wMoveStamp);
}

// ���ֹͣ�ƶ�
void PlayerObject::PlayerStopMove(A3DVECTOR3& pos, int time, float speed, char mode)
{
	SessionManager::GetSingleton().SetTheSession(m_pGame->GetGameSession());
	c2s_SendCmdStopMove(pos, speed*256.0f, mode, m_byDir, m_wMoveStamp, time);
}

// ѡ��Ŀ��
void PlayerObject::SelectTarget(int id)
{
	SessionManager::GetSingleton().SetTheSession(m_pGame->GetGameSession());
	c2s_SendCmdSelectTarget(id);
}

// ȡ��ѡ��
void PlayerObject::Unselect()
{
	SessionManager::GetSingleton().SetTheSession(m_pGame->GetGameSession());
	c2s_SendCmdUnselect();
}

// ��ͨ����
void PlayerObject::NormalAttack(char force_attack)
{
	SessionManager::GetSingleton().SetTheSession(m_pGame->GetGameSession());
	c2s_SendCmdNormalAttack(force_attack);
}

// ������
void PlayerObject::CastSkill(int skill_id, char force_attack, size_t target_count, int* targets)
{
	SessionManager::GetSingleton().SetTheSession(m_pGame->GetGameSession());
	c2s_SendCmdCastSkill(skill_id, force_attack, target_count, targets);
}

// ����
void PlayerObject::Pickup(int id, int tid)
{
	SessionManager::GetSingleton().SetTheSession(m_pGame->GetGameSession());
	c2s_SendCmdPickup(id, tid);
}

// װ����Ʒ
void PlayerObject::EquipItem(int inv_index, int eq_index)
{
	SessionManager::GetSingleton().SetTheSession(m_pGame->GetGameSession());
	c2s_SendCmdEquipItem(inv_index, eq_index);
}

// ��ȡ��Ʒ��ϸ��Ϣ
void PlayerObject::GetInventoryDetail(int pack)
{
	SessionManager::GetSingleton().SetTheSession(m_pGame->GetGameSession());
	c2s_SendCmdGetIvtrDetailData(pack);
}

// ʹ�ø�����Ḵ��
void PlayerObject::ResurrectByItem()
{
	SessionManager::GetSingleton().SetTheSession(m_pGame->GetGameSession());
	c2s_SendCmdReviveItem();
}

// �سǸ���
void PlayerObject::ResurrectInTown()
{
	SessionManager::GetSingleton().SetTheSession(m_pGame->GetGameSession());
	c2s_SendCmdReviveVillage();
}

// ���ֿ�����
void PlayerObject::CheckSecurityPassword()
{
	SessionManager::GetSingleton().SetTheSession(m_pGame->GetGameSession());
	c2s_SendCmdOpenFashionTrash("");
}

// ȡ����һ����
void PlayerObject::CancelAction()
{
	SessionManager::GetSingleton().SetTheSession(m_pGame->GetGameSession());
	c2s_SendCmdCancelAction();
}

// �ǳ����
void PlayerObject::Logout()
{
	SessionManager::GetSingleton().SetTheSession(m_pGame->GetGameSession());
	c2s_SendCmdLogout(0);
}

// ��ҽ���
void PlayerObject::Speak(const wchar_t* strText)
{
	GNET::PublicChat p;
	p.channel = GP_CHAT_LOCAL;
	p.roleid = m_iRoleID;

	AWString strMsg = strText;
	int iMsgLen = strMsg.GetLength();
	a_ClampRoof(iMsgLen, CHAT_ACHAR_LENGTH);

	// ���û����˷���ָ��#
	if( strMsg[0] == _AL('#') )
		strMsg[0] = _AL(' ');

	int iSize = iMsgLen * sizeof(wchar_t);
	p.msg.replace(strMsg, iSize);

	m_pGame->GetGameSession()->SendNetData(p);
}

// ��Ҵ���
void PlayerObject::Sitdown(bool bSitdown)
{
	SessionManager::GetSingleton().SetTheSession(m_pGame->GetGameSession());
	if( bSitdown )
		c2s_SendCmdSitDown();
	else
		c2s_SendCmdStandUp();
}

// ��NPC�Ի�����ʼNPC����
void PlayerObject::TalkToNPC(int nid)
{
	SessionManager::GetSingleton().SetTheSession(m_pGame->GetGameSession());
	c2s_SendCmdNPCSevHello(nid);
}

// ���ѧ����
void PlayerObject::LearnSkill(int id)
{
	SessionManager::GetSingleton().SetTheSession(m_pGame->GetGameSession());
	c2s_SendCmdNPCSevLearnSkill(id);
}

// ��Ұ�̯
void PlayerObject::Booth(bool bOpen, const wchar_t* szName/* =NULL */)
{
	AWString sBoothName;
	SessionManager::GetSingleton().SetTheSession(m_pGame->GetGameSession());

	if( bOpen )
	{
		if( szName )
			sBoothName = szName;
		else
			sBoothName.Format(L"asdf", GetRoleName());

		// �չ�һ������ʯ
		C2S::cmd_open_booth::entry_t* aEntry = new C2S::cmd_open_booth::entry_t[12];
		aEntry[0].type = 3043;
		aEntry[0].index = 0;
		aEntry[0].count = 1;
		aEntry[0].price = 1000;

  		aEntry[1].type = 3043;
  		aEntry[1].index = 0xffff;
  		aEntry[1].count = 5;
  		aEntry[1].price = 50000;
 
  		aEntry[2].type = 3043;
  		aEntry[2].index = 0xffff;
  		aEntry[2].count = 1;
  		aEntry[2].price = 8000;

		aEntry[3].type = 15761;
		aEntry[3].index = 0xffff;
		aEntry[3].count = 1;
		aEntry[3].price = 5000;

		for(int i=4; i<12; i++)
		{
			aEntry[i].type = 11208;
			aEntry[i].index = 0xffff;
			aEntry[i].count = i;
			aEntry[i].price = 1000 * i;
		}

		c2s_SendCmdOpenBooth(12, (const wchar_t*)sBoothName, aEntry);
		delete[] aEntry;
	}
	else
	{
		c2s_SendCmdCloseBooth();
	}
}

// ��Ҷ���
void PlayerObject::PlayAction(int iPose)
{
	if( m_bIsDead || m_bIsMoving )
		return;

	SessionManager::GetSingleton().SetTheSession(m_pGame->GetGameSession());
	c2s_SendCmdEmoteAction(iPose);
}

// ʹ�ð����ڵ���Ʒ
void PlayerObject::UseItem(int iSlot)
{
	if( m_bIsDead || m_bIsMoving )
		return;

	if( iSlot < 0 || iSlot >= (int)m_Inventory.size() )
		return;

	if( !m_Inventory[iSlot].tid || !m_Inventory[iSlot].count )
		return;

	SessionManager::GetSingleton().SetTheSession(m_pGame->GetGameSession());
	c2s_SendCmdUseItem(PACK_INVENTORY, (BYTE)iSlot, m_Inventory[iSlot].tid, 1);
}

// ʹ����л���ʱװģʽ
void PlayerObject::SwitchFashion(bool bFashion)
{
	if( m_bInFashion != bFashion )
	{
		SessionManager::GetSingleton().SetTheSession(m_pGame->GetGameSession());
		c2s_SendCmdFashionSwitch();
	}
}

// �ٻ�����
void PlayerObject::SummonPet(int iPetIdx)
{
	if( m_bIsDead || m_bIsMoving )
		return;

	SessionManager::GetSingleton().SetTheSession(m_pGame->GetGameSession());
	c2s_SendCmdPetSummon(iPetIdx);
}

// �ٻس���
void PlayerObject::BanishPet(int iPetIdx)
{
	if( m_bIsDead || m_bIsMoving )
		return;

	SessionManager::GetSingleton().SetTheSession(m_pGame->GetGameSession());
	c2s_SendCmdPetBanish(iPetIdx);
}

// ӵ��ָ����ɫ
void PlayerObject::AttachBuddy(int iPlayerID)
{
	GameObject::ObjectInfo* pObj = m_pGame->GetPlayer(iPlayerID);
	if( pObj )
	{
		SessionManager::GetSingleton().SetTheSession(m_pGame->GetGameSession());
		if( IsMale() )
			c2s_SendCmdBindPlayerInvite(iPlayerID);
		else
			c2s_SendCmdBindPlayerRequest(iPlayerID);
	}
}

// �����������
void PlayerObject::DetachBuddy()
{
	SessionManager::GetSingleton().SetTheSession(m_pGame->GetGameSession());
	c2s_SendCmdCancelBindPlayer();
}

// �������
void PlayerObject::TeamInvite(int idPlayer)
{
	SessionManager::GetSingleton().SetTheSession(m_pGame->GetGameSession());
	c2s_SendCmdTeamInvite(idPlayer);
}

// �Զ����
// 0 unknown, 1 task, 2 activity
void PlayerObject::DoAutoTeam(int type, int goal_id, int op)
{
	SessionManager::GetSingleton().SetTheSession(m_pGame->GetGameSession());
	c2s_SendCmdAutoTeamSetGoal(type, goal_id, op);
}

///////////////////////////////////////////////////////////////////////////
// ��Ӧ����յ�S2CЭ��

void PlayerObject::OnS2CCommand(WORD cmd, void* buf, size_t size)
{
	using namespace S2C;

	elementdataman* pDataMan = SessionManager::GetSingleton().GetDataMan();

	switch(cmd)
	{
	case TASK_DATA:
		{
			// ��־����ҵ����ݻ�ȡ��
			// ������ɫ�ڷ�����ȴ
			SendDebugCmd(8903, 73125);
			m_pAI->Enter();
			m_bInitOK = true;
		}
		break;
	case OWN_EXT_PROP:
		{
			cmd_own_ext_prop* pCmd = (cmd_own_ext_prop*)buf;
			m_fRunSpeed = pCmd->prop.mv.run_speed;
			m_fAttackRange = pCmd->prop.ak.attack_range;
		}
		break;
	case SELF_INFO_00:
		{
			cmd_self_info_00* pCmd = (cmd_self_info_00*)buf;
			m_iLevel = pCmd->sLevel;
			m_iLevel2 = pCmd->Level2;
			m_iHP = pCmd->iHP;
			m_iMaxHP = pCmd->iMaxHP;
			m_iMP = pCmd->iMP;
			m_iMaxMP = pCmd->iMaxMP;
			m_iExp = pCmd->iExp;
			m_iSP = pCmd->iSP;
		}
		break;
	case PLAYER_EXT_PROP_MOVE:
		{
			cmd_pep_move* pCmd = (cmd_pep_move*)buf;
			if( pCmd->idPlayer == m_iRoleID )
				m_fRunSpeed = pCmd->ep_move.run_speed;
		}
		break;
	case OWN_IVTR_DETAIL_DATA:
		{
			// ��ȡ��Ұ�������
			UserPackage* pack;
			cmd_own_ivtr_detail_info* pCmd = (cmd_own_ivtr_detail_info*)buf;
			if( pCmd->byPackage == PACK_INVENTORY )
				pack = &m_Inventory;
			else if( pCmd->byPackage == PACK_EQUIPMENTS )
				pack = &m_Equips;
			else break;

			pack->clear();
			pack->insert(pack->end(), pCmd->ivtr_size, ITEM());

			ITEM it;
			char* pEnd = (char*)buf + size;
			char* p = (char*)pCmd->content;
			size_t count = *(size_t*)p; p += sizeof(size_t);
			bool err = false;
			size_t index;
			unsigned short crc, len;
			for( size_t i=0;i<count;i++ )
			{
				index = *(size_t*)p;	p += sizeof(size_t);
				it.tid = *(int*)p;		p += sizeof(int);
				p += sizeof(int);
				p += sizeof(int);
				it.count = *(size_t*)p;		p += sizeof(size_t);
				crc = *(unsigned short*)p;	p += sizeof(unsigned short);
				len = *(unsigned short*)p;	p += sizeof(unsigned short);
				if( len ) p += len;
				if( p > pEnd )
				{
					err = true;
					break;
				}
				if( index >= pCmd->ivtr_size )
				{
					err = true;
					break;
				}

				(*pack)[index] = it;
			}

			if( err )
			{
				a_LogOutput(1, "OWN_IVTR_DETAIL_DATA, Invalid data size when build the inventory content!");
				pack->clear();
			}
		}
		break;
	case GET_OWN_MONEY:
		{
			cmd_get_own_money* pCmd = (cmd_get_own_money*)buf;
			m_nMoney = pCmd->amount;
			m_nMoneyCapacity = pCmd->max_amount;
		}
		break;
	case PICKUP_MONEY:
		{
			cmd_pickup_money* pCmd = (cmd_pickup_money*)buf;
			m_nMoney += pCmd->iAmount;
		}
		break;
	case PICKUP_ITEM:
		{
			cmd_pickup_item* pCmd = (cmd_pickup_item*)buf;
			if( pCmd->byPackage != PACK_INVENTORY ) break;

			int slot, slot_count;
			if( !PushItem(pCmd->tid, pCmd->iAmount, slot, slot_count) ||
				slot != pCmd->bySlot || slot_count != (int)pCmd->iSlotAmount )
			{
				// ����������ݲ�һ��
				a_LogOutput(1, "S2C::PICKUP_ITEM, PushItem error, ����������ݲ�һ�£�");
				GetInventoryDetail(PACK_INVENTORY);
			}
		}
		break;
	case EQUIP_ITEM:
		{
			cmd_equip_item* pCmd = (cmd_equip_item*)buf;
			if( pCmd->index_inv >= m_Inventory.size() || pCmd->index_equip >= m_Inventory.size() )
				break;
			abase::swap(m_Inventory[pCmd->index_inv], m_Equips[pCmd->index_equip]);
			if( m_Inventory[pCmd->index_inv].count != pCmd->count_inv || m_Equips[pCmd->index_equip].count != pCmd->count_equip )
			{
				// ����������ݲ�һ��
				a_LogOutput(1, "S2C::EQUIP_ITEM, ����������ݲ�һ�£�");
				GetInventoryDetail(PACK_INVENTORY);
				GetInventoryDetail(PACK_EQUIPMENTS);
			}
		}
		break;
	case PLAYER_DROP_ITEM:
		{
			UserPackage* pack;
			cmd_player_drop_item* pCmd = (cmd_player_drop_item*)buf;
			if( pCmd->byPackage == PACK_INVENTORY )
				pack = &m_Inventory;
			else if( pCmd->byPackage == PACK_EQUIPMENTS )
				pack = &m_Equips;
			else break;

			if( pCmd->bySlot >= pack->size() )
				break;
			ITEM& it = (*pack)[pCmd->bySlot];
			if( it.tid != pCmd->tid ) break;
			if( it.count > pCmd->count )
				it.count -= pCmd->count;
			else
			{
				it.tid = 0;
				it.count = 0;
			}
		}
		break;
	case TRASHBOX_PWD_STATE:
		{
			cmd_trashbox_pwd_state* pCmd = (cmd_trashbox_pwd_state*)buf;
			if( !pCmd->has_passwd ) CheckSecurityPassword();
		}
		break;
	case SKILL_DATA:
		{
			// ��ȡ��������
			SKILL skill;
			cmd_skill_data* pCmd = (cmd_skill_data*)buf;
			for( size_t i=0;i<pCmd->skill_count;i++ )
			{
				skill.id = pCmd->skill_list[i].id_skill;
				skill.level = pCmd->skill_list[i].level;

				// ���ûس���
				if( skill.id == 167 ) continue;

				if( IsPositiveSkill(skill.id,skill.mp_cost,skill.range) )
				{
					if( skill.range < 1e-6 ) skill.range = m_fAttackRange;
					m_Skills.push_back(skill);
				}
			}
		}
		break;
	case HOST_CORRECT_POS:
		{
			cmd_host_correct_pos* pCmd = (cmd_host_correct_pos*)buf;
			m_vPos = pCmd->pos;
			ResetMoveStamp(pCmd->stamp);
			ClearTask();

			// �����������ϱ�����λ�ã����·�һ��
			if( m_iPosError++ >= 3 )
			{
				A3DVECTOR3 vMin, vMax;
				MoveAgent::GetSingleton().GetRegion(vMin, vMax);
				float fRnd = a_Random(-10.0f, 10.0f);
				A3DVECTOR3 vDest = m_vPos + A3DVECTOR3(fRnd, 0.0f, fRnd);
				a_Clamp(vDest.x, vMin.x, vMax.x);
				a_Clamp(vDest.z, vMin.z, vMax.z);
				SendDebugCmd(C2S::GOTO, &vDest, sizeof(vDest));
				m_iPosError = 0;
			}
		}
		break;
	case SET_MOVE_STAMP:
		{
			cmd_set_move_stamp* pCmd = (cmd_set_move_stamp*)buf;
			ResetMoveStamp(pCmd->move_stamp);
		}
		break;
	case NOTIFY_HOSTPOS:
		{
			cmd_notify_hostpos* pCmd = (cmd_notify_hostpos*)buf;
			m_vPos = pCmd->vPos;
			ClearTask();
		}
		break;

	case SELECT_TARGET:
		{
			cmd_select_target* pCmd = (cmd_select_target*)buf;
			m_iSelectTarget = pCmd->idTarget;
		}
		break;
	case UNSELECT:
		{
			m_iSelectTarget = -1;
		}
		break;
	case HOST_START_ATTACK:
		{
			cmd_host_start_attack* pCmd = (cmd_host_start_attack*)buf;
			AddAttackTask();

			if( !pCmd->ammo_remain )
			{
				m_Equips[11].tid = 0;
				m_Equips[11].count = 0;
			}
			else
			{
				m_Equips[11].count = pCmd->ammo_remain;
			}
		}
		break;
	case ATTACK_ONCE:
		{
			cmd_attack_once* pCmd = (cmd_attack_once*)buf;
			if( pCmd->ammo_num && m_Equips[11].tid )
			{
				if( m_Equips[11].count <= pCmd->ammo_num )
				{
					m_Equips[11].tid = 0;
					m_Equips[11].count = 0;
				}
				else
				{
					m_Equips[11].count -= pCmd->ammo_num;
				}
			}
		}
		break;
	case HOST_STOPATTACK:
		{
			if( m_pCurTask && m_pCurTask->GetTaskType() == TASK_TYPE_NORMAL_ATTACK )
			{
				EndCurTask();
				StartTask();
			}
		}
		break;
	case OBJECT_CAST_SKILL:
		{
			cmd_object_cast_skill* pCmd = (cmd_object_cast_skill*)buf;
			if( pCmd->caster == m_iRoleID )
			{
				AddCastSkillTask();
			}
		}
		break;
	case HOST_STOP_SKILL:
		{
			if( m_pCurTask && m_pCurTask->GetTaskType() == TASK_TYPE_CAST_SKILL )
			{
				EndCurTask();
				StartTask();
			}
		}
		break;
	case HOST_DIED:
		{
			cmd_host_died* pCmd = (cmd_host_died*)buf;
			m_bIsDead = true;
			m_vPos = pCmd->pos;
			ClearTask();
		}
		break;

	case BE_HURT:
		{
			cmd_be_hurt* pCmd = (cmd_be_hurt*)buf;
			if( ISPLAYERID(pCmd->attacker_id) )
				SetForceAttack(true);
		}
		break;

	case LEARN_SKILL:
		{
			bool bFound = false;
			cmd_learn_skill* pCmd = (cmd_learn_skill*)buf;
			for( size_t i=0;i<m_Skills.size();i++ )
			{
				if( m_Skills[i].id == pCmd->skill_id )
				{
					if( !pCmd->skill_level )
						m_Skills.erase(m_Skills.begin() + i);
					else
						m_Skills[i].level = pCmd->skill_level;
					bFound = true;
					break;
				}
			}

			if( !bFound )
			{
				SKILL skill;
				skill.id = pCmd->skill_id;
				skill.level = pCmd->skill_level;

				CECSkill skData(skill.id, 1);
				skill.mp_cost = skData.GetRequiredMP();
				skill.range = skData.GetCastRange(0.0f, 0.0f);
				m_Skills.push_back(skill);
			}
		}
		break;

	case COST_SKILL_POINT:
		{
			cmd_cost_skill_point* pCmd = (cmd_cost_skill_point*)buf;
			m_iSP -= pCmd->skill_point;
			a_ClampFloor(m_iSP, 0);
		}
		break;

	case PLAYER_ENABLE_EFFECT:
		{
			cmd_player_enable_effect* pCmd = (cmd_player_enable_effect*)buf;
			DATA_TYPE DataType = pDataMan->get_data_type(pCmd->effect, ID_SPACE_ESSENCE);
			if( DataType == DT_FACEPILL_ESSENCE )
				m_bChangingFace = true;
		}
		break;
	case PLAYER_DISABLE_EFFECT:
		{
			cmd_player_disable_effect* pCmd = (cmd_player_disable_effect*)buf;
			DATA_TYPE DataType = pDataMan->get_data_type(pCmd->effect, ID_SPACE_ESSENCE);
			if( DataType == DT_FACEPILL_ESSENCE )
				m_bChangingFace = false;
		}
		break;

	case PLAYER_ENABLE_FASHION:
		{
			cmd_player_enable_fashion* pCmd = (cmd_player_enable_fashion*)buf;
			if( pCmd->idPlayer == m_iRoleID )
				m_bInFashion = pCmd->is_enabble ? true : false;
		}
		break;
	case PET_ROOM:
		{
			cmd_pet_room* pCmd = (cmd_pet_room*)buf;
			const BYTE* pData = (BYTE*)buf + sizeof(cmd_pet_room);

			PET_DATA pet_data;
			m_PetData.clear();
			for( int i=0;i<pCmd->count;i++ )
			{
				int iSlot = *(int*)pData;
				pData += sizeof(int);
				const info_pet* pPet = (const info_pet*)pData;
				pData += sizeof(info_pet);

				strncpy(pet_data.name, pPet->name, 16);
				pet_data.pet_class = pPet->pet_class;
				pet_data.level = pPet->level;
				m_PetData.push_back(pet_data);
			}
		}
		break;
	case PLAYER_BIND_REQUEST:
		{
			cmd_player_bind_request* pCmd = (cmd_player_bind_request*)buf;
			SessionManager::GetSingleton().SetTheSession(m_pGame->GetGameSession());
			c2s_SendCmdBindPlayerRequestRe(pCmd->who, 0);
		}
		break;
	case PLAYER_BIND_INVITE:
		{
			cmd_player_bind_invite* pCmd = (cmd_player_bind_invite*)buf;
			SessionManager::GetSingleton().SetTheSession(m_pGame->GetGameSession());
			c2s_SendCmdBindPlayerInviteRe(pCmd->who, 0);
		}
		break;

	case COOLTIME_DATA:
		{
		}
		break;
	case ENCHANT_RESULT:
		{
		}
		break;

	default: break;
	}
}

// ��ȡ��ɫ������
const wchar_t* PlayerObject::GetRoleName()
{
	return m_pGame->GetGameSession()->GetRoleName();
}
