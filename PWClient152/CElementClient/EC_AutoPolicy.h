/*
* FILE: EC_AutoPolicy.h
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
#include <vector.h>
#include <AAssist.h>
#include "EC_Counter.h"
#include "ScriptValue.h"


///////////////////////////////////////////////////////////////////////////
//	
//	Define and Macro
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Types and Global variables
//	
///////////////////////////////////////////////////////////////////////////

class CECPlayerWrapper;


///////////////////////////////////////////////////////////////////////////
//	
//	Declare of Global functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Class CECAutoPolicy
//	
///////////////////////////////////////////////////////////////////////////

class CECAutoPolicy
{
public:

	// ��������
	enum
	{
		POLICY_AUTOKILLMONSTER,		// �Զ�ɱ��
	};

	// ϵͳ�¼�
	enum
	{
		EVENT_BEHURT = 1,
		EVENT_SKILLINTERRUPT,
		EVENT_RETURNTOWNOK,
		EVENT_CONFIGCHANGED,
	};

	struct CONFIG
	{
		int attack_skill;
		bool attack_iscombo;
		int assist_skill;
		bool assist_iscombo;
		int nAssistInterval;
		int nTime;
		int iAutoPickMode;	// �ޡ�������Ʒ����ʰȡ���
		int nPetrolRadius;	// Ѳ�߰뾶

		CONFIG() : attack_skill(0), attack_iscombo(false), assist_skill(0), assist_iscombo(false),
			nAssistInterval(60000), nTime(3600000), iAutoPickMode(0), nPetrolRadius(500)
		{
		}
	};

public:
	virtual ~CECAutoPolicy();

	// ��ʼ������Զ�����
	bool Init();
	void Release();

	// ���²���
	void Tick(DWORD dwDeltaTime);
	void Render();

	// ��ʼ/�����Զ�����
	void StartPolicy(int policyType);
	void StopPolicy();

	// ������Ϸ����
	void OnEnterWorld();

	// �뿪��Ϸ����
	void OnLeaveWorld();

	// ϵͳ�¼�
	void SendEvent_BeHurt(int attacker);
	void SendEvent_SkillInterrupt(int skill_id);
	void SendEvent_ReturnTown();
	void SendEvent_ConfigChanged();

	// ��ȡ��Ҷ���
	CECPlayerWrapper* GetPlayerWrapper() { return m_pPlayer; }

	// ��⵱ǰ�Ƿ�������
	bool IsAutoPolicyEnabled() const { return !m_strCurPolicy.IsEmpty(); }

	// ��ȡ��ǰ��������
	const char* GetCurPolicy() const { return m_strCurPolicy; }

	// ��ȡ��������
	const CONFIG& GetConfigData() const { return m_Config; }

	// ������������
	void SetConfigData(const CONFIG& data);

	// ��ȡʣ��ʱ��
	DWORD GetRemainTime() const;

	// ��ȡʵ��
	static CECAutoPolicy& GetInstance();

protected:
	CONFIG m_Config;
	CECPlayerWrapper* m_pPlayer;
	AString m_strCurPolicy;
	CECCounter m_cntTick;
	DWORD m_dwKeepingTime;
	DWORD m_dwCurrentTime;

private:
	CECAutoPolicy();

	// ����ָ���ű��ļ�
	bool LoadLuaFile(const char* filename);
	// ִ��ָ���Ľű�����
	void CallLuaFunc(const char* szTable, const char* szName, const abase::vector<CScriptValue>& args, abase::vector<CScriptValue>* ret=NULL);
	// ���õ�ǰ����
	void SetCurPolicy(const char* szPolicyName);

	// ��������
	bool LoadConfigData();
	// ��������
	void SaveConfigData();
};

///////////////////////////////////////////////////////////////////////////
//	
//	Inline functions
//	
///////////////////////////////////////////////////////////////////////////
