/*
* FILE: EC_AutoPolicy.cpp
*
* DESCRIPTION: 
*
* CREATED BY: Shizhenhua, 2013/8/21
*
* HISTORY: 
*
* Copyright (c) 2011 ACE Studio, All Rights Reserved.
*/


#include "EC_AutoPolicy.h"
#include "EC_PlayerWrapper.h"
#include "EC_Global.h"
#include "EC_Game.h"
#include "EC_GameRun.h"
#include "EC_HostPlayer.h"
#include "EC_UIManager.h"
#include "EC_GameUIMan.h"
#include "EC_GameSession.h"

#include <AF.h>
#include <AFilePackMan.h>
#include <LuaState.h>
#include <A3DDevice.h>
#include <A3DFont.h>


///////////////////////////////////////////////////////////////////////////
//	
//	Define and Macro
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Local Types and Variables and Global variables
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Local functions
//	
///////////////////////////////////////////////////////////////////////////

extern void _InitPlayerAPI(CLuaState* pState);


///////////////////////////////////////////////////////////////////////////
//  
//  Implement class CECAutoPolicy
//  
///////////////////////////////////////////////////////////////////////////

CECAutoPolicy& CECAutoPolicy::GetInstance()
{
	static CECAutoPolicy obj;
	return obj;
}

CECAutoPolicy::CECAutoPolicy()
{
	m_pPlayer = NULL;
	m_cntTick.SetPeriod(100);	// 一秒更新10次脚本逻辑
	m_dwKeepingTime = 0;
	m_dwCurrentTime = 0;
}

CECAutoPolicy::~CECAutoPolicy()
{
}

bool CECAutoPolicy::Init()
{
	// 加载所有脚本文件
	AScriptFile sf;
	if( !sf.Open("configs\\autopolicy\\allfiles.txt") )
	{
		a_LogOutput(1, "CECAutoPolicy::Init, load the policy file list failed!");
		return false;
	}

	while( !sf.IsEnd() )
	{
		if( !sf.GetNextToken(true) )
			break;

		AString luaFile = AString("configs\\autopolicy\\") + sf.m_szToken;
		LoadLuaFile(luaFile);
	}

	sf.Close();

	// 注册玩家相关的API
	_InitPlayerAPI(g_LuaStateMan.GetAIState());
	return true;
}

void CECAutoPolicy::Release()
{
	if( m_pPlayer )
	{
		delete m_pPlayer;
		m_pPlayer = NULL;
	}
}

void CECAutoPolicy::OnEnterWorld()
{
	if( !m_pPlayer )
		m_pPlayer = new CECPlayerWrapper(g_pGame->GetGameRun()->GetHostPlayer());

	// 加载配置
	LoadConfigData();
}

void CECAutoPolicy::OnLeaveWorld()
{
	if( m_pPlayer )
	{
		SaveConfigData();

		delete m_pPlayer;
		m_pPlayer = NULL;
	}

	// 停止策略
	SetCurPolicy("");
	m_dwCurrentTime = 0;
}

void CECAutoPolicy::StartPolicy(int policyType)
{
	const char* policyMap[] =
	{
		"AutoKillMonster",		// POLICY_AUTOKILLMONSTER
	};

	if( policyType < 0 || policyType >= sizeof(policyType)/sizeof(const char*) )
		return;

	const char* policyName = policyMap[policyType];
	SetCurPolicy(policyName);
	m_dwCurrentTime = 0;
	if( m_pPlayer )
	{
		m_pPlayer->GetHostPlayer()->ClearComboSkill();
		m_pPlayer->SetOrigPos(m_pPlayer->GetHostPlayer()->GetPos());
	}

	CECGameUIMan* pGameUI = g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan();
	if( pGameUI )
		pGameUI->AddChatMessage(pGameUI->GetStringFromTable(10920), GP_CHAT_MISC);
}

void CECAutoPolicy::StopPolicy()
{
	SetCurPolicy("");
	if( m_pPlayer ) m_pPlayer->OnStopPolicy();
	m_dwCurrentTime = 0;

	// 暂停聚灵
// 	CECHostPlayer::MultiExpState state = m_pPlayer->GetHostPlayer()->MultiExp_GetState();
// 	if( state == CECHostPlayer::MES_ENHANCE )
// 	{
// 		if( m_pPlayer )
// 			m_pPlayer->GetGameSession()->c2s_CmdToggleMultiExp(false);
// 	}

	CECGameUIMan* pGameUI = g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan();
	if( pGameUI )
		pGameUI->AddChatMessage(pGameUI->GetStringFromTable(10921), GP_CHAT_MISC);
}

void CECAutoPolicy::SetCurPolicy(const char* szPolicyName)
{
	CScriptValue val;
	abase::vector<CScriptValue> args;

	if( !szPolicyName || !strcmp(szPolicyName, "") )
	{
		val.SetVal("");
		args.push_back(val);
		CallLuaFunc("AIManager", "ChangePolicy", args);
		m_strCurPolicy = "";
	}
	else
	{
		val.SetVal(szPolicyName);
		args.push_back(val);
		CallLuaFunc("AIManager", "ChangePolicy", args);
		m_strCurPolicy = szPolicyName;
	}
}

void CECAutoPolicy::Tick(DWORD dwDeltaTime)
{
	if( g_pGame->GetGameRun()->GetGameState() != CECGameRun::GS_GAME )
		return;

	if( m_pPlayer )
		m_pPlayer->Tick(dwDeltaTime);

	if( m_cntTick.IncCounter(dwDeltaTime) )
	{
		abase::vector<CScriptValue> args;
		abase::vector<CScriptValue> ret;
		args.push_back((double)m_dwKeepingTime);
		CallLuaFunc("AIManager", "OnTick", args, &ret);
		
		m_cntTick.Reset();
		m_dwKeepingTime = 0;
	}
	
	m_dwKeepingTime += dwDeltaTime;
	if( IsAutoPolicyEnabled() )
		m_dwCurrentTime += dwDeltaTime;
}

void CECAutoPolicy::Render()
{
	A3DFont* pFont = g_pGame->GetA3DEngine()->GetSystemFont();
	g_pGame->GetA3DDevice()->SetZWriteEnable(false);

	if( m_pPlayer )
	{
		ACString strMsg;
		strMsg.Format(_AL("Attack Error: %d"), m_pPlayer->m_iAttackErrCnt);
		pFont->TextOut(200, 180, strMsg, 0xffff0000);
		strMsg.Format(_AL("Pickup Error: %d"), m_pPlayer->m_iPickupErrCnt);
		pFont->TextOut(200, 200, strMsg, 0xffff0000);

		int x = 350;
		int y = 180;

		CECPlayerWrapper::Action* pCurAction = m_pPlayer->m_pCurAction;
		if( pCurAction )
		{
			pFont->TextOut(x, y, AS2AC(AP_GetActionName(pCurAction->GetType())), 0xff00ff00);
			y += 10;
		}

		CECPlayerWrapper::ActionQueue::iterator it = m_pPlayer->m_Actions.begin();
		while( it != m_pPlayer->m_Actions.end() )
		{
			CECPlayerWrapper::Action* pAction = *it;
			pFont->TextOut(x, y, AS2AC(AP_GetActionName(pAction->GetType())), 0xff00ff00);
			y += 10;
			++it;
		}
	}

	g_pGame->GetA3DDevice()->SetZWriteEnable(true);
}

void CECAutoPolicy::SendEvent_BeHurt(int attacker)
{
	abase::vector<CScriptValue> args;
	args.push_back((double)EVENT_BEHURT);
	args.push_back((double)attacker);
	CallLuaFunc("AIManager", "OnEvent", args);

	if( ISNPCID(attacker) && m_pPlayer )
		m_pPlayer->OnMonsterAttackMe(attacker);
}

void CECAutoPolicy::SendEvent_SkillInterrupt(int skill_id)
{
	abase::vector<CScriptValue> args;
	args.push_back((double)EVENT_SKILLINTERRUPT);
	args.push_back((double)skill_id);
	CallLuaFunc("AIManager", "OnEvent", args);
}

void CECAutoPolicy::SendEvent_ReturnTown()
{
	abase::vector<CScriptValue> args;
	args.push_back((double)EVENT_RETURNTOWNOK);
	CallLuaFunc("AIManager", "OnEvent", args);
}

void CECAutoPolicy::SendEvent_ConfigChanged()
{
	abase::vector<CScriptValue> args;
	args.push_back((double)EVENT_CONFIGCHANGED);
	CallLuaFunc("AIManager", "OnEvent", args);

	if( m_pPlayer )
		m_pPlayer->m_InvalidObj.clear();
}

bool CECAutoPolicy::LoadLuaFile(const char* filename)
{
	CLuaState* pLuaState = g_LuaStateMan.GetAIState();
	pLuaState->Lock();

	// 注意文件格式应为无BOM的UTF-8
	if( !pLuaState->RegisterFile(filename) )
	{
		a_LogOutput(1, "CECAutoPolicy::LoadLuaFile, register the lua script file failed! (%s)", filename);
		return false;
	}

	pLuaState->Unlock();
	return true;
}

void CECAutoPolicy::CallLuaFunc(const char* szTable, const char* szName, const abase::vector<CScriptValue>& args, abase::vector<CScriptValue>* ret)
{
	CLuaState* pLuaState = g_LuaStateMan.GetAIState();
	if( ret )
		pLuaState->LockCall(szTable, szName, args, *ret);
	else
	{
		abase::vector<CScriptValue> ret_values;
		pLuaState->LockCall(szTable, szName, args, ret_values);
	}
}

bool CECAutoPolicy::LoadConfigData()
{
	AString strFile;
	strFile.Format("userdata\\autopolicy\\%d.ini", m_pPlayer->GetHostPlayer()->GetCharacterID());

	AIniFile file;
	if( !file.Open(strFile) )
	{
		SaveConfigData();
		return false;
	}

	AString strSect = "config";
	m_Config.attack_skill = file.GetValueAsInt(strSect, "attack_skill", 0);
	m_Config.attack_iscombo = file.GetValueAsBoolean(strSect, "attack_iscombo", false);
	m_Config.assist_skill = file.GetValueAsInt(strSect, "assist_skill", 0);
	m_Config.assist_iscombo = file.GetValueAsBoolean(strSect, "assist_iscombo", false);
	m_Config.nAssistInterval = file.GetValueAsInt(strSect, "assist_interval", 60000);
	m_Config.nTime = file.GetValueAsInt(strSect, "keeping_time", 3600000);
	m_Config.iAutoPickMode = file.GetValueAsInt(strSect, "autopick", 1);
	m_Config.nPetrolRadius = file.GetValueAsInt(strSect, "petrol_radius", 500);

	file.Close();
	return true;
}

void CECAutoPolicy::SaveConfigData()
{
	AIniFile file;
	AString strFile;

	AString strSect = "config";
	file.WriteIntValue(strSect, "attack_skill", m_Config.attack_skill);
	file.WriteIntValue(strSect, "attack_iscombo", m_Config.attack_iscombo ? 1 : 0);
	file.WriteIntValue(strSect, "assist_skill", m_Config.assist_skill);
	file.WriteIntValue(strSect, "assist_iscombo", m_Config.assist_iscombo ? 1 : 0);
	file.WriteIntValue(strSect, "assist_interval", m_Config.nAssistInterval);
	file.WriteIntValue(strSect, "keeping_time", m_Config.nTime);
	file.WriteIntValue(strSect, "autopick", m_Config.iAutoPickMode);
	file.WriteIntValue(strSect, "petrol_radius", m_Config.nPetrolRadius);

	strFile.Format("%s\\userdata\\autopolicy", af_GetBaseDir());
	CreateDirectoryA(strFile, NULL);
	strFile.Format("userdata\\autopolicy\\%d.ini", m_pPlayer->GetHostPlayer()->GetCharacterID());
	file.Save(strFile);
	file.Close();
}

void CECAutoPolicy::SetConfigData(const CONFIG& data)
{
	m_Config = data;
	SaveConfigData();
}

DWORD CECAutoPolicy::GetRemainTime() const
{
	if( m_Config.nTime > (int)m_dwCurrentTime )
		return m_Config.nTime - m_dwCurrentTime;
	else
		return 0;
}