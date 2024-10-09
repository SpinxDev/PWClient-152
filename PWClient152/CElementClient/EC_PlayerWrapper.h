/*
* FILE: EC_PlayerWrapper.h
*
* DESCRIPTION: 
*
* CREATED BY: Shizhenhua, 2013/8/21
*
* HISTORY: 
*
* Copyright (c) 2011 ACE Studio, All Rights Reserved.
*/


#pragma once

#include <ABaseDef.h>
#include <A3DVector.h>
#include <vector.h>
#include <hashmap.h>

#include <set>
#include <list>


///////////////////////////////////////////////////////////////////////////
//	
//	Define and Macro
//	
///////////////////////////////////////////////////////////////////////////

#define MAX_PLAYER_ACTION 10


///////////////////////////////////////////////////////////////////////////
//	
//	Types and Global variables
//	
///////////////////////////////////////////////////////////////////////////

class CECPlayer;
class CECHostPlayer;
class CECGameSession;


///////////////////////////////////////////////////////////////////////////
//	
//	Declare of Global functions
//	
///////////////////////////////////////////////////////////////////////////

// �Զ������µ�һЩ�¼�
enum
{
	AP_EVENT_CANNOTMOVE,		// �����ƶ�
	AP_EVENT_MOVEFINISHED,		// �ƶ�����
	AP_EVENT_TRACEOK,			// ׷�ٳɹ�
	AP_EVENT_STARTSKILL,		// ���ܿ�ʼ
	AP_EVENT_STOPSKILL,			// ����ֹͣ
	AP_EVENT_STARTUSEITEM,		// ʹ����Ʒ��ʼ
	AP_EVENT_STOPUSEITEM,		// ʹ����Ʒ����
	AP_EVENT_PICKUPOK,			// �ɹ���ȡ��Ʒ
	AP_EVENT_CANNOTPICKUP,		// ������Ʒ�����ܼ�ȡ
	AP_EVENT_STARTMELEE,		// ��ʼ�չ�
	AP_EVENT_STOPMELEE,			// �����չ�
	AP_EVENT_COMBOCONTINUE,		// ��ϼ��ƽ�
	AP_EVENT_COMBOFINISH,		// ��ϼ���ֹ��Ӧ�����һ������û�зų������
	AP_EVENT_MELEEOUTOFRANGE,	// �չ���������
};

void AP_ActionEvent(int iEvent, int iParam = 0);

const char* AP_GetActionName(int iAction);
const char* AP_GetEventName(int iEvent);


///////////////////////////////////////////////////////////////////////////
//	
//	Class CECPlayerWrapper
//	
///////////////////////////////////////////////////////////////////////////

class CECPlayerWrapper
{
public:

	enum
	{
		ACTION_IDLE,			// ����
		ACTION_MOVE,			// �ƶ�
		ACTION_CASTSKILL,		// ������
		ACTION_USEITEM,			// ʹ����Ʒ
		ACTION_PICKUP,			// ��ʰ
		ACTION_COMBOSKILL,		// ��ϼ�
		ACTION_MELEE,			// �չ�
	};

	struct DelayTask
	{
		int iType;
		int iParam1;
		int iParam2;

		DelayTask() : iType(ACTION_IDLE), iParam1(0), iParam2(0) {}
	};

	// ��ҵ�ǰִ�е���Ϊ����
	// ���ȴ�����Ϊ����Ϊ��ɺ�ִ��AI�߼�
	class Action
	{
	protected:
		int type;
		CECPlayerWrapper* host;

	public:
		Action(CECPlayerWrapper* pHost, int t) : host(pHost), type(t) {}

		virtual bool StartAction() { return true; }
		virtual void EndAction() {}

		// ����true, ˵������Ϊִ�����
		virtual bool Tick(DWORD dwDeltaTime) { return false; }

		// ��Ӧ�¼�
		virtual void OnEvent(int iEvent, int iParam) {}

		// �Ƿ���ж�
		virtual bool CanBreak() const { return true; }

		// �Ƿ��к�����Ϊ
		bool HaveNextAction() const { return host->m_Actions.size() > 0; }

		// ��Ϊ����
		int GetType() const { return type; }
	};

	typedef std::list<Action*> ActionQueue;

	friend class Action;
	friend class CECAutoPolicy;

public:
	CECPlayerWrapper(CECHostPlayer* pHost);
	virtual ~CECPlayerWrapper();

	// �����߼�
	void Tick(DWORD dwDeltaTime);

	// ֹͣ����
	void StopPolicy();

	// ���������Ϊ
	void ClearAction();
	// ��Ӧ�����Ϊ���¼�
	void OnActionEvent(int iEvent, int iParam);

	// �����Ϊ�Ƿ��Ѿ�������
	bool HaveAction() const;
	// ���һ��������Ϊ
	bool AddIdleAction(int iTime);

	// ��ȡ�����Ʒ
	int GetItemIndex(int iPack, int tid);
	// ��ȡָ����Ʒ�ĸ���
	int GetItemCount(int iPack, int tid);

	// ��ȡ��ǰλ��
	A3DVECTOR3 GetPos() const;
	// �ƶ���ָ��λ��
	void MoveTo(float x, float z);
	// ���ȡ����ǰ��Ϊ
	void CancelAction();
	// ��ȡ��ҳ�ʼλ��
	A3DVECTOR3 GetOrigPos() const { return m_vOrigPos; }

	// ��Ѱһ��Ŀ��
	bool SearchTarget(int& id, int& tid);

	// ��ȡ��ǰѡ�е�Ŀ��
	int GetSelectedTarget() const;
	// ѡȡָ��Ŀ��
	void SelectTarget(int iTarget);
	// ȡ��ѡ��
	void Unselect();
	// �ж�ѡȡ�����ܷ񱻹���
	bool NpcCanAttack(int nid);
	// �����չ�
	bool NormalAttack();
	// ʩ����ϼ���
	bool CastComboSkill(int group_id, bool bIgnoreAtkLoop, bool bQueue = false);
	// ʩ�ż���
	bool CastSkill(int skill_id, bool bQueue = false);
	// ʹ����Ʒ
	void UseItem(int iSlot);

	// �ж���Ʒ�Ƿ���Լ�ȡ
	bool MatterCanPickup(int mid, int tid);
	// ��ȡ��Ʒ
	void Pickup(int mid);

	// �ж�����Ƿ����뿪��Ұ
	bool IsPlayerInSlice(int idPlayer);
	// ��ȡ�����־�
	int GetWeaponEndurance() const;
	// ����Ƿ�����
	bool IsDead() const;
	// �Ƿ��������ʩ�����
	bool IsRevivedByOther() const;
	// �������˸���
	void AcceptRevive();
	// ʹ�ø�����Ḵ��
	bool ReviveByItem();
	// �سǸ���
	void ReviveInTown();
	// ����ǿ�й������
	void SetForceAttack(bool bFlag) { m_bForceAttack = bFlag; }
	// �Ƿ��ڰ�ȫ����
	bool IsInSanctuary() const;
	// �����ڹ����Լ��Ĺ�
	bool IsMonsterAttackMe() const;

	// ��ȡ�����λ��
	A3DVECTOR3 GetObjectPos(int object_id) const;

	// ������ҵĳ�ʼλ��
	void SetOrigPos(const A3DVECTOR3& vPos) { m_vOrigPos = vPos; }

	// ����ָ����Ŀ����Ч
	void SetInvalidObject(int object_id);

	///////////////////////////////////////////////////////////////////////////

	// ��ӦNPC��Matter��ʧ
	void OnObjectDisappear(int object_id);
	// ��Ӧ���﹥���Լ�
	void OnMonsterAttackMe(int monster_id);

	void AddAttackError();
	void ResetAttackError() { m_iAttackErrCnt = 0; }
	void ResetPickupError() { m_iPickupErrCnt = 0; }
	int GetAttackError() const { return m_iAttackErrCnt; }
	int GetPickupError() const { return m_iPickupErrCnt; }

	void OnStopPolicy();

	CECHostPlayer* GetHostPlayer() { return m_pHost; }
	CECGameSession* GetGameSession() { return m_pSession; }

protected:
	CECHostPlayer* m_pHost;
	CECGameSession* m_pSession;
	ActionQueue m_Actions;
	Action* m_pCurAction;
	A3DVECTOR3 m_vOrigPos;				// ��ҽ��������λ��

	bool m_bForceAttack;				// ǿ�й����ı��
	int m_iAttackErrCnt;
	int m_iPickupErrCnt;
	abase::hash_map<int, int> m_InvalidObj;	// ���ɹ����Ĺ֡��򲻿ɼ�ȡ����Ʒ
	std::set<int> m_MonsterAttackMe;	// �������ҵĹ�

	DelayTask m_DelayTask;				// �ӳ��������ڴ��������ܵ�ʩ��

private:
	bool AddAction(Action* pAction);
	bool StartAction();
	bool EndCurAction();
	bool AddOneAction(int iActionType, int iParam = 0);

	// �ӳ�����
	void DoDelayTask(int iType, int iParam1 = 0, int iParam2 = 0);
	void ProcessDelayTask();
};

///////////////////////////////////////////////////////////////////////////
//	
//	Inline functions
//	
///////////////////////////////////////////////////////////////////////////
