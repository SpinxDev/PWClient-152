/*
 * FILE: AIObject.h
 *
 * DESCRIPTION: ���ļ�������һ��Ĭ�ϵ����AI����
 *
 * CREATED BY: Shizhenhua, 2012/9/6
 *
 * HISTORY: 
 *
 * Copyright (c) 2011 ACE Studio, All Rights Reserved.
 */


#pragma once


#include <ABaseDef.h>
#include <vector.h>
#include <hashmap.h>
#include <A3DVector.h>
#include "Common.h"


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

class PlayerObject;


///////////////////////////////////////////////////////////////////////////
//  
//  class AIObject
//  
///////////////////////////////////////////////////////////////////////////

class AIObject
{
	enum
	{
		AI_STATE_BORN,		// ������Ϸ������Լ�����
		AI_STATE_IDLE,		// ����״̬
		AI_STATE_PATROL,	// �Ĵ��ε�
		AI_STATE_FOLLOW,	// ����Ŀ��
		AI_STATE_COMBAT,	// ����ս��
		AI_STATE_DEAD,		// ����

		AI_STATE_NUM,
	};

	typedef void (AIObject::*FN_AICMD_PROC) ( StringVector& cmd );
	typedef abase::hash_map<AString,FN_AICMD_PROC> AICommandMap;

public:
	AIObject( PlayerObject* pPlayer );
	virtual ~AIObject();

	void Enter();	// ��ҽ��뱾����
	void Tick();
	void Leave();	// ����뿪������

	// ��������
	void Command( StringVector& cmd );

	int GetCurState() { return m_iState; }
	void SetCurState(int iState);

	PlayerObject* GetHostPlayer() { return m_pHost; }

protected:
	PlayerObject* m_pHost;
	int m_iState;
	int m_iExtData;
	int m_iFollowID;		// ������ID

	AICommandMap m_AICommand;

protected:

	// State handlers
	void Tick_Idle();
	void Tick_Dead();
	void Tick_Patrol();
	void Tick_Follow();
	void Tick_Combat();
	void Tick_Born();

	// AI command handlers
	void OnCmd_Follow( StringVector& cmd );
};

///////////////////////////////////////////////////////////////////////////
//  
//  Inline Functions
//  
///////////////////////////////////////////////////////////////////////////
