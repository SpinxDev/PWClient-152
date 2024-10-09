/*
 * FILE: AIObject.cpp
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Shizhenhua, 2012/9/6
 *
 * HISTORY: 
 *
 * Copyright (c) 2011 ACE Studio, All Rights Reserved.
 */


#include "AIObject.h"
#include "PlayerObject.h"
#include "GameObject.h"
#include "MoveAgent.h"
#include <ALog.h>


///////////////////////////////////////////////////////////////////////////
//  
//  Define and Macro
//  
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//  
//  Types and Global variables
//  
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//  
//  Implement class AIObject
//  
///////////////////////////////////////////////////////////////////////////

AIObject::AIObject( PlayerObject* pPlayer ) : m_pHost(pPlayer), m_iState(AI_STATE_IDLE)
{
	m_iExtData = 0;
	m_iFollowID = 0;

	// ��AI���������
	m_AICommand["follow"] = &AIObject::OnCmd_Follow;
}

AIObject::~AIObject()
{
}

// �ı䵱ǰAI״̬
void AIObject::SetCurState(int iState)
{
	m_iState = iState;
	m_iExtData = 0;
}

// ��ҽ���ò���
void AIObject::Enter()
{
	m_pHost->RandomMove();
	SetCurState(AI_STATE_BORN);
}

// ����뿪�ò���
void AIObject::Leave()
{
	// ������ڹ�����ż��ܣ���ֹͣ
	m_pHost->CancelAction();
	m_pHost->ClearTask();
}

// ��������
void AIObject::Command( StringVector& cmd )
{
	if( cmd.size() )
	{
		AICommandMap::iterator it;
		it = m_AICommand.find(cmd[0]);
		if( it != m_AICommand.end() )
			(this->*it->second)(cmd);
		else
		{
			a_LogOutput(1, "Can't find the cmd (%s) in AIObject!", cmd[0]);
		}
	}
}

// �������AI
void AIObject::Tick()
{
	typedef void (AIObject::*FN_TICK_FUNC) ();
	static FN_TICK_FUNC ticks[] =
	{
		&AIObject::Tick_Born,
		&AIObject::Tick_Idle,
		&AIObject::Tick_Patrol,
		&AIObject::Tick_Follow,
		&AIObject::Tick_Combat,
		&AIObject::Tick_Dead,
	};

	if( m_iState != AI_STATE_FOLLOW )
	{
		if( m_iFollowID )
			SetCurState(AI_STATE_FOLLOW);
	}
	else
	{
		if( !m_iFollowID )
			SetCurState(AI_STATE_IDLE);
	}

	// ��������
	if( m_pHost->IsDead() )
		SetCurState(AI_STATE_DEAD);

	ASSERT(m_iState >= 0 && m_iState < AI_STATE_NUM);
	(this->*ticks[m_iState])();
}

// ��ҽ������磬���������������
void AIObject::Tick_Born()
{
	m_iExtData = 0;

	// ����
	if( m_pHost->GetLevel() < 10 )
	{
		m_pHost->SendDebugCmd(10889, 10000000);
	}
	else m_iExtData++;

	// ȷ��Ѫ��ֵ������
	if( m_pHost->GetHP() < m_pHost->GetMaxHP() / 2 )
	{
		int index = m_pHost->GetItemIndex(PlayerObject::PACK_INVENTORY, 27070);

		if( index == -1 )
			m_pHost->SendDebugCmd(10800, 27070);
		else
			m_pHost->EquipItem(index, 20);
	}
	else m_iExtData++;

	// ��֤����ֵ������
	if( m_pHost->GetMP() < m_pHost->GetMaxMP() / 2 )
	{
		int index = m_pHost->GetItemIndex(PlayerObject::PACK_INVENTORY, 27071);

		if( index == -1 )
			m_pHost->SendDebugCmd(10800, 27071);	
		else
			m_pHost->EquipItem(index, 21);
	}
	else m_iExtData++;

	// ����gm����
	if( !m_pHost->IsEquipExist(11215, 3) )
	{
		int index = m_pHost->GetItemIndex(PlayerObject::PACK_INVENTORY, 11215);
		if( index == -1 )
		{
			m_pHost->SendDebugCmd(10800, 11215);
		}
		else
		{
			m_pHost->EquipItem(index, 3);
		}
	}
	else m_iExtData++;

	// �������âְҵ������ʹ�ù���(GM������Ů�� + ������)
	if( m_pHost->GetProfession() == PROF_ARCHOR && !(m_pHost->IsEquipExist(11214, 0) &&
		m_pHost->IsEquipExist(410, 11)) )
	{
		int index;

		// װ��������Ů��
		if( !m_pHost->IsEquipExist(11214, 0) )
		{
			index = m_pHost->GetItemIndex(PlayerObject::PACK_INVENTORY, 11214);
			if( index == -1 )
				m_pHost->SendDebugCmd(10800, 11214);
			else
				m_pHost->EquipItem(index, 0);
		}

		// װ����ʸ
		if( !m_pHost->IsEquipExist(410, 11) )
		{
			index = m_pHost->GetItemIndex(PlayerObject::PACK_INVENTORY, 410);
			if( index == -1 )
				m_pHost->SendDebugCmd(10800, 410, 999);
			else
			{
				if( m_pHost->IsEquipExist(11214,0) )
					m_pHost->EquipItem(index, 11);
			}
		}
	}
	// ����ǽ���ְҵ������ʹ��GM������ǧ��
	else if( m_pHost->GetProfession() == PROF_JIANLING && !m_pHost->IsEquipExist(29719, 0) )
	{
		int index = m_pHost->GetItemIndex(PlayerObject::PACK_INVENTORY, 29719);
		if( index == -1 )
			m_pHost->SendDebugCmd(10800, 29719);
		else
			m_pHost->EquipItem(index, 0);
	}
	// ����ְҵ����ֹʹ�ù���
	else if( m_pHost->GetProfession() != PROF_ARCHOR && m_pHost->IsEquipExist(11) )
	{
		// �ѹ�������
		int index = m_pHost->GetEmptySlot();
		if( index >= 0 ) m_pHost->EquipItem(index, 0);
	}
	else m_iExtData++;

	// ��֤���㹻�ĸ������
	if( m_pHost->GetItemCount(PlayerObject::PACK_INVENTORY, 3043) < 10 )
	{
		m_pHost->SendDebugCmd(10800, 3043, 10);
	}
	else m_iExtData++;

	// һ�о�����GO GO GO
	if( m_iExtData >= 6 )
	{
		SetCurState(AI_STATE_COMBAT);
	}

	m_pHost->AddIdleTask(3000);
}

// ��ҿ���״̬
void AIObject::Tick_Idle()
{
	if( m_pHost->GetHP() < m_pHost->GetMaxHP() / 2 )
	{
		// ����
		if( !m_iExtData )
		{
			m_pHost->Sitdown(true);
			m_iExtData = 1;
		}
	}
	else
	{
		if( m_iExtData )
		{
			m_pHost->Sitdown(false);
			m_iExtData = 0;
		}
	}

	if( !m_iExtData )
	{
		// 50������Ŀ�꣬�����ս��״̬���������Ѳ��״̬
		int target = m_pHost->GetGameObject()->GatherTarget(50.0f, m_pHost->GetForceAttack());
		if( target != -1 )
			SetCurState(AI_STATE_COMBAT);
		else
			SetCurState(AI_STATE_PATROL);
	}
	else
	{
		m_pHost->AddIdleTask(1000);
	}
}

// �������״̬
void AIObject::Tick_Dead()
{
	if( m_pHost->IsDead() )
	{
		// ����������и�����ᣬֱ�Ӹ������سǸ���
		if( m_pHost->GetItemCount(PlayerObject::PACK_INVENTORY,3043) > 0 )
			m_pHost->ResurrectByItem();
		else
			m_pHost->ResurrectInTown();

		m_pHost->AddIdleTask(3000);
	}
	else
	{
		// 50������Ŀ�������ս��״̬���������Ѳ��״̬
		GameObject* pGame = m_pHost->GetGameObject();
		if( pGame->GatherTarget(50.0f, m_pHost->GetForceAttack()) != -1 )
			SetCurState(AI_STATE_COMBAT);
		else
			SetCurState(AI_STATE_PATROL);
	}
}

// ���Ѳ��״̬
void AIObject::Tick_Patrol()
{
	A3DVECTOR3 vNextPos;
	A3DVECTOR3 vCurPos = m_pHost->GetCurPos();
	BYTE byDir = m_pHost->GetDirection();
	float fRunSpeed = m_pHost->GetRunSpeed();

	// ���50������Ŀ�����ս��״̬
	GameObject* pGame = m_pHost->GetGameObject();
	int target = pGame->GatherTarget(50.0f, m_pHost->GetForceAttack());
	if( target != -1 )
	{
		SetCurState(AI_STATE_COMBAT);
		return;
	}

	// ����ƶ�
	if( m_pHost->GetNextMovePos(vCurPos,byDir,fRunSpeed,vNextPos) )
	{
		float fDist = (vNextPos - vCurPos).Magnitude();
		int use_time = (int)(fDist / fRunSpeed * 1000);
		if( use_time < 100 ) use_time = 100;
		if( use_time > 1000 ) use_time = 1000;
		m_pHost->AddMoveTask(vNextPos, use_time, fRunSpeed, 0x21);
	}
	else
	{
		if( m_pHost->IsMoving() )
		{
			m_pHost->AddStopMoveTask(vCurPos, 100, fRunSpeed, 0x21);
		}

		if( a_Random(0, 2) == 0 )
			m_pHost->SetDirection(a_Random(0, 255));
	}

	if( m_pHost->IsCurTaskEmpty() )
		m_pHost->AddIdleTask(1000);
}

// ��Ҹ���ĳһĿ��
void AIObject::Tick_Follow()
{
	GameObject* pGame = m_pHost->GetGameObject();
	GameObject::ObjectInfo* obj = pGame->GetEntity(m_iFollowID);
	if( !obj )
	{
		SetCurState(AI_STATE_IDLE);
		return;
	}

	A3DVECTOR3 next_pos;
	A3DVECTOR3 vCurPos = m_pHost->GetCurPos();
	float fRunSpeed = m_pHost->GetRunSpeed();

	float min_dis = 2.0f * 2.0f;
	if( (obj->pos - vCurPos).SquaredMagnitude() <= min_dis )
		return;

	if( m_pHost->GetNextMovePos(vCurPos,obj->pos,fRunSpeed,next_pos) )
	{
		float distance = (next_pos - vCurPos).Magnitude();
		int use_time = (int)(distance / fRunSpeed * 1000);
		if( use_time < 100 ) use_time = 100;
		if( use_time > 1000 ) use_time = 1000;

		bool stop = (next_pos - obj->pos).SquaredMagnitude() < min_dis;
		if( stop )
		{
			m_pHost->AddStopMoveTask(next_pos, use_time, fRunSpeed, 0x21);
		}
		else
		{
			m_pHost->AddMoveTask(next_pos, use_time, fRunSpeed, 0x21);
		}
	}
	else
	{
		if( m_pHost->IsMoving() )
		{
			m_pHost->AddStopMoveTask(next_pos, 100, fRunSpeed, 0x21);
		}
	}
	
	if( m_pHost->IsCurTaskEmpty() )
	{
		m_pHost->AddIdleTask(1000);
	}
}

// ����������ս��
void AIObject::Tick_Combat()
{
	GameObject* pGame = m_pHost->GetGameObject();
	char force_attack = m_pHost->GetForceAttack();
	int sel_target = m_pHost->GetSelectedTarget();

	A3DVECTOR3 vCurPos = m_pHost->GetCurPos();
	float fRunSpeed = m_pHost->GetRunSpeed();

	if( sel_target == -1 )
	{
		int iTarget = pGame->GatherTarget(50.0f, force_attack);
		if( iTarget != -1 )
		{
			m_iExtData = 0;

			if( ISPLAYERID(iTarget) || ISNPCID(iTarget) )
				m_pHost->SelectTarget(iTarget);
			else if( ISMATTERID(iTarget) )
				m_pHost->SelectMatter(iTarget);
		}
		else
		{
			// 50�׷�Χ��û�ҵ�Ŀ�꣬����Ѳ��״̬
			SetCurState(AI_STATE_PATROL);
			return;
		}

		if( m_pHost->IsMoving() )
		{
			m_pHost->AddStopMoveTask(vCurPos, 100, fRunSpeed, 0x21);
		}
	}
	else
	{
		GameObject::ObjectInfo* obj = pGame->GetEntity(sel_target);
		if( obj )
		{
			float min_dis = 0.0f;
			int use_skill = 0;
			if( ISPLAYERID(obj->id) || ISNPCID(obj->id) )
				use_skill = m_pHost->SelectAttackSkill(min_dis);
			else if( ISMATTERID(obj->id) )
				min_dis = 1.5f * 1.5f;

			if( (vCurPos - obj->pos).SquaredMagnitude() < min_dis )
			{
				if( ISPLAYERID(obj->id) || ISNPCID(obj->id) )
				{
					if( use_skill )
						m_pHost->CastSkill(use_skill, force_attack, 1, &obj->id);
					else
						m_pHost->NormalAttack(force_attack);
				}
				else if( ISMATTERID(obj->id) )
				{
					m_pHost->Pickup(obj->id, obj->tid);

					// ����������ζ�ʧ�ܣ������
					// ��������Ч�б���ֹ�ٴμ�ȡ
					if( m_pHost->IsCurTaskEmpty() && ++m_iExtData == 3 )
					{
						m_pHost->Unselect();
						m_pHost->GetGameObject()->AddInvalidMatter(obj->id);
					}
				}
			}
			else
			{
				// �����Զ
				A3DVECTOR3 next_pos;
				if( m_pHost->GetNextMovePos(vCurPos,obj->pos,fRunSpeed,next_pos) )
				{
					float distance = (next_pos - vCurPos).Magnitude();
					int use_time = (int)(distance / fRunSpeed * 1000);
					if( use_time < 100 ) use_time = 100;
					if( use_time > 1000 ) use_time = 1000;

					bool stop = (next_pos - obj->pos).SquaredMagnitude() < min_dis;
					if( stop )
					{
						m_pHost->AddStopMoveTask(next_pos, use_time, fRunSpeed, 0x21);
					}
					else
					{
						m_pHost->AddMoveTask(next_pos, use_time, fRunSpeed, 0x21);
					}
				}
				else
				{
					if( m_pHost->IsMoving() )
					{
						m_pHost->AddStopMoveTask(vCurPos, 100, fRunSpeed, 0x21);
					}
				}
			}
		}
		else
		{
			m_pHost->Unselect();

			if( m_pHost->IsMoving() )
			{
				m_pHost->AddStopMoveTask(vCurPos, 100, fRunSpeed, 0x21);
			}
		}
	}

	// �����â��ҩ�������򲹳�
	if( m_pHost->GetProfession() == PROF_ARCHOR && !m_pHost->IsEquipExist(410, 11) )
	{
		SetCurState(AI_STATE_BORN);
	}

	if( m_pHost->IsCurTaskEmpty() )
	{
		m_pHost->AddIdleTask(1000);
	}
}

// AI���ܸ�������
void AIObject::OnCmd_Follow( StringVector& cmd )
{
	int roleid = 0;
	if( cmd.size() >= 2 ) roleid = atoi(cmd[1]);
	m_iFollowID = roleid;
}
