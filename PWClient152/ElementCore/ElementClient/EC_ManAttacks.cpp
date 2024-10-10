/*
 * FILE: EC_ManAttacks.cpp
 *
 * DESCRIPTION: Manager to manager all attack effects and results. 
 *
 * CREATED BY: Hedi, 2005/3/10
 *
 * HISTORY: 
 *
 * Copyright (c) 2004 Archosaur Studio, All Rights Reserved.
 */

#include "EC_Global.h"
#include "EC_Player.h"
#include "EC_ManPlayer.h"
#include "EC_ManNPC.h"
#include "EC_NPC.h"
#include "EC_ManAttacks.h"
#include "EC_ManSkillGfx.h"
#include "EC_World.h"
#include "EC_FixedMsg.h"
#include "EC_Decal.h"
#include "EC_ManDecal.h"
#include "EC_Resource.h"
#include "ElementSkill.h"
#include "EC_GPDataType.h"
#include "EC_HostPlayer.h"
#include "EC_Configs.h"
#include "EC_Monster.h"
#include "EC_UIManager.h"
#include "EC_BaseUIMan.h"
#include "EC_CountryConfig.h"
#include "A3DSkillGfxComposer2.h"
#include "EC_pet.h"
#include "EC_Optimize.h"
#include "CSplit.h"
#include <AScriptFile.h>
#include <AFI.h>

#include "elementdataman.h"

inline bool _get_pos_by_id(int nID,	A3DVECTOR3& vPos)
{
	if (ISPLAYERID(nID))
	{
		CECPlayer* pPlayer = g_pGame->GetGameRun()->GetWorld()->GetPlayerMan()->GetPlayer(nID);
		if (pPlayer)
		{
			vPos = pPlayer->GetPos();
			return true;
		}
	}
	else if (ISNPCID(nID))
	{
		CECNPC* pNPC = g_pGame->GetGameRun()->GetWorld()->GetNPCMan()->GetNPCFromAll(nID);
		if (pNPC)
		{
			vPos = pNPC->GetPos();
			return true;
		}
	}

	return false;
}

//////////////////////////////////////////////////////////////////////////////////////
// class CECAttackEvent
//////////////////////////////////////////////////////////////////////////////////////
CECAttackEvent::CECAttackEvent()
{
	m_pManager = NULL;

	m_bSignaled	= false;
	m_bDoFired = false;
	m_bDoDamaged = false;
	m_bFinished	= false;
	m_timeLived = 0;

	m_idHost = 0;
	m_idCastTarget = 0;
	m_idWeapon = 0;
	m_idSkill = 0;
	m_nSkillLevel = 0;
	m_timeToBeFired = 0;
	m_timeToDoDamage = 0;
	m_nSkillSection = 0;
}

CECAttackEvent::CECAttackEvent(CECAttacksMan* pManager, int idHost, int idCastTarget, int idTarget, int idWeapon, int idSkill, int nSkillLevel, DWORD dwModifier, int nDamage, int timeToBeFired, int timeToDoDamage)
: m_pManager(pManager), m_idHost(idHost), m_idCastTarget(idCastTarget), m_idWeapon(idWeapon), m_idSkill(idSkill), m_nSkillLevel(nSkillLevel), 
m_timeToBeFired(timeToBeFired), m_timeToDoDamage(timeToDoDamage),m_nSkillSection(0)
{
	m_bSignaled			= false;
	m_bDoFired			= false;
	m_bDoDamaged		= false;
	m_bFinished			= false;
	m_timeLived			= 0;
	
	m_targets.reserve(6);
	TARGET_DATA data;
	data.idTarget	= idTarget;
	data.dwModifier = dwModifier;
	data.nDamage	= nDamage;
	m_targets.push_back(data);
}

CECAttackEvent::~CECAttackEvent()
{
}

bool CECAttackEvent::DoFire()
{
	float vFlyScale = 1.0f;
	float vHitScale = 1.0f;

	m_bDoFired = true;

	if( ISPLAYERID(m_idHost) )
	{
		if( m_idSkill != 0 )
		{
			const A3DSkillGfxComposer* pComposer = NULL;

			// we use skill composed gfx to present the skill effect
			if (m_nSkillSection>0) // 多段技能
			{
				CECMultiSectionSkillMan* pMan = m_pManager->GetMultiSkillGfxComposerMan();
				if(pMan)
				{
					pMan->Play(m_idSkill, m_nSkillSection,m_idHost, m_idCastTarget, m_targets,GNET::ElementSkill::IsGoblinSkill(m_idSkill));
					pComposer = pMan->GetSkillGfxComposer(m_idSkill, m_nSkillSection);
				}
			}
			else
			{
				if(GNET::ElementSkill::IsGoblinSkill(m_idSkill))
					m_pManager->GetSkillGfxComposerMan()->Play(m_idSkill, m_idHost, m_idCastTarget, m_targets, true);
				else
					m_pManager->GetSkillGfxComposerMan()->Play(m_idSkill, m_idHost, m_idCastTarget, m_targets);
				pComposer = m_pManager->GetSkillGfxComposerMan()->GetSkillGfxComposer(m_idSkill);
			}

			if (pComposer && pComposer->m_dwFlyTime == 0) // 技能gfx没有飞行实际，则马上头顶冒字
			{
				m_timeToDoDamage = 1;
			}
			else
			{
				A3DVECTOR3 vecHost, vecTarget;
				// now we estimated a time to do damage
				if( m_targets.size() && _get_pos_by_id(m_idHost, vecHost) && _get_pos_by_id(m_targets[0].idTarget, vecTarget) )
				{
					m_timeToDoDamage = int(Magnitude(vecHost - vecTarget) / 20.0f * 1000.0f);
					a_ClampFloor(m_timeToDoDamage, 10ul);
				}
			}
		}
		else if( m_idWeapon != 0 )
		{
			// first determine gfx used
			const char * szFlyGFX = NULL;
			const char * szHitGFX = NULL;

			// using weapon gfx
			DATA_TYPE dt;
			const void * pData = g_pGame->GetElementDataMan()->get_data_ptr(
				m_idWeapon, ID_SPACE_ESSENCE, dt);
			ASSERT(dt == DT_WEAPON_ESSENCE || dt == DT_PROJECTILE_ESSENCE);
			
			if( dt == DT_PROJECTILE_ESSENCE )
			{
				// 远程武器，使用弹药的效果
				PROJECTILE_ESSENCE * pProjectile = (PROJECTILE_ESSENCE *) pData;

				szFlyGFX = pProjectile->file_firegfx + 4;	// skip gfx/
				szHitGFX = pProjectile->file_hitgfx + 4;	// skip gfx/
			}
			else if (dt == DT_WEAPON_ESSENCE)
			{
				// 近程物理攻击，使用武器的效果
				WEAPON_ESSENCE * pWeapon = (WEAPON_ESSENCE *) pData;
				WEAPON_SUB_TYPE * pWeaponType = (WEAPON_SUB_TYPE*) g_pGame->GetElementDataMan()->get_data_ptr(pWeapon->id_sub_type, ID_SPACE_ESSENCE, dt);
				ASSERT(dt == DT_WEAPON_SUB_TYPE);
				szFlyGFX = NULL;
				szHitGFX = pWeaponType->file_hitgfx + 4; // skip gfx/
			}			
			
			bool bHideFlyGfx = !CECOptimize::Instance().GetGFX().CanShowFly(m_idHost);
			bool bHideHitGfx = !CECOptimize::Instance().GetGFX().CanShowHit(m_idHost);
			int nNumTargets = m_targets.size();
			for(int i=0; i<nNumTargets; i++)
			{
				const TARGET_DATA& data = m_targets[i];
				
				const char * pszFlyGFX = szFlyGFX;
				const char * pszHitGFX = szHitGFX;

				if( data.dwModifier & MOD_NULLITY )
					pszHitGFX = "程序联入\\击中\\无效攻击击中.gfx";

				if (bHideFlyGfx) pszFlyGFX = NULL;
				if (bHideHitGfx) pszHitGFX = NULL;

				g_pGame->GetGameRun()->GetWorld()->GetSkillGfxMan()->AddSkillGfxEvent(m_idHost, data.idTarget, 
					pszFlyGFX, pszHitGFX, m_timeToDoDamage, false, enumLinearMove, 1, 0, NULL, vFlyScale, vHitScale, data.dwModifier);
			}
		}
		else
		{
			// without weapon
			// 使用拳套类的击中效果
			DATA_TYPE dt;
			const void * pData = g_pGame->GetElementDataMan()->get_data_ptr(183, ID_SPACE_ESSENCE, dt);
			ASSERT(dt == DT_WEAPON_SUB_TYPE);
			WEAPON_SUB_TYPE	* pWeaponType = (WEAPON_SUB_TYPE *) pData;
			
			bool bHideHitGfx = !CECOptimize::Instance().GetGFX().CanShowHit(m_idHost);
			const char * szGFX = NULL;
			int nNumTargets = m_targets.size();
			for(int i=0; i<nNumTargets; i++)
			{
				const TARGET_DATA& data = m_targets[i];
				
				szGFX = pWeaponType->file_hitgfx + 4; // skip gfx/
				if( data.dwModifier & MOD_NULLITY )
					szGFX = "程序联入\\击中\\无效攻击击中.gfx";
				if (bHideHitGfx) szGFX = NULL;
				
				g_pGame->GetGameRun()->GetWorld()->GetSkillGfxMan()->AddSkillGfxEvent(m_idHost, data.idTarget, NULL, 
					szGFX, m_timeToDoDamage, false, enumLinearMove, 1, 0, NULL, vFlyScale, vHitScale, data.dwModifier);
			}
		}
	}
	else if( ISNPCID(m_idHost) )
	{
		if( m_idSkill != 0 )
		{
			const A3DSkillGfxComposer* pComposer = NULL;
			// we use skill composed gfx to present the skill effect
			if (m_nSkillSection>0) // 多段技能
			{
				CECMultiSectionSkillMan* pMan = m_pManager->GetMultiSkillGfxComposerMan();
				if(pMan)
				{
					pMan->Play(m_idSkill, m_nSkillSection,m_idHost, m_idCastTarget, m_targets);
					pComposer = pMan->GetSkillGfxComposer(m_idSkill, m_nSkillSection);
				}
			}
			else
			{
				m_pManager->GetSkillGfxComposerMan()->Play(m_idSkill, m_idHost, m_idCastTarget, m_targets);
				pComposer = m_pManager->GetSkillGfxComposerMan()->GetSkillGfxComposer(m_idSkill);
			}

			if (pComposer && pComposer->m_dwFlyTime == 0) // 技能没有飞行时间，则直接头顶冒字
			{
				m_timeToDoDamage = 1;
			} 
			else
			{
				A3DVECTOR3 vecHost, vecTarget;
				// now we estimated a time to do damage
				if( m_targets.size() && _get_pos_by_id(m_idHost, vecHost) && _get_pos_by_id(m_targets[0].idTarget, vecTarget) )
				{
					m_timeToDoDamage = int(Magnitude(vecHost - vecTarget) / 20.0f * 1000.0f);
					a_ClampFloor(m_timeToDoDamage, 10ul);
				}
			}
		}
		else
		{
			CECNPC * pNPC = g_pGame->GetGameRun()->GetWorld()->GetNPCMan()->GetNPC(m_idHost);
			if (!pNPC)
				return true;

			const char * szFlyGFX = NULL;
			const char * szHitGFX = NULL;

			if (pNPC->IsMonsterNPC())
			{
				const MONSTER_ESSENCE * pEssence = ((CECMonster *)pNPC)->GetDBEssence();

				szFlyGFX = pEssence->file_gfx_short + 4; // skip gfx/
				szHitGFX = pEssence->file_gfx_short_hit + 4; // skip gfx/
			}
			else if (pNPC->IsPetNPC())
			{
				//szFlyGFX = "";
				const PET_ESSENCE* pEssence = ((CECPet*)pNPC)->GetDBEssence();

				szFlyGFX = pEssence->file_gfx_short + 4; //"策划联入\\通用飞行\\弓箭飞行.gfx";

				szHitGFX = "策划联入\\怪物击中\\怪物肉搏击中.gfx";
			}
			else
				return false;

			if( !szFlyGFX[0] )
				szFlyGFX = NULL;

			if( !szHitGFX[0] )
				szHitGFX = NULL;			
			
			bool bHideFlyGfx = !CECOptimize::Instance().GetGFX().CanShowFly(m_idHost);
			bool bHideHitGfx = !CECOptimize::Instance().GetGFX().CanShowHit(m_idHost);
			int nNumTargets = m_targets.size();
			for(int i=0; i<nNumTargets; i++)
			{
				const TARGET_DATA& data = m_targets[i];
				
				const char * pszFlyGFX = szFlyGFX;
				const char * pszHitGFX = szHitGFX;
				if( data.nDamage <= 0 )
				{
					pszHitGFX = NULL;
				}
				else if( data.dwModifier & MOD_NULLITY )
				{
					pszHitGFX = "程序联入\\击中\\无效攻击击中.gfx";
				}

				if (bHideFlyGfx) pszFlyGFX = NULL;
				if (bHideHitGfx) pszHitGFX = NULL;
			
				g_pGame->GetGameRun()->GetWorld()->GetSkillGfxMan()->AddSkillGfxEvent(m_idHost, data.idTarget, 
					pszFlyGFX, pszHitGFX, m_timeToDoDamage, false, enumLinearMove, 1,
					0, NULL, vFlyScale, vHitScale, data.dwModifier);
			}
		}
	}
	else
		return true;
	
	return true;
}

static ACString GetPlayerName(CECPlayer *pPlayer)
{
	ACString strName;
	if (pPlayer)
	{
		if (pPlayer->GetShowNameInCountryWar())
			strName = pPlayer->GetNameInCountryWar();
		else
			strName = pPlayer->GetName();
	}
	return strName;
}

bool CECAttackEvent::DoDamage()
{
	m_bDoDamaged = true;
	m_bFinished = true;

	CECGameRun* pGameRun = g_pGame->GetGameRun();
	int idHostPlayer = pGameRun->GetHostPlayer()->GetCharacterID();

	//	Get host name
	ACString strHostName;
	CECObject* pHostObject = pGameRun->GetWorld()->GetObject(m_idHost, 0);
	if (pHostObject)
	{
		if (ISNPCID(m_idHost))
			strHostName = ((CECNPC*)pHostObject)->GetName();
		else if (ISPLAYERID(m_idHost))
			strHostName = GetPlayerName((CECPlayer*)pHostObject);
	}
	
	int nNumTargets = m_targets.size();
	for(int i=0; i<nNumTargets; i++)
	{
		const TARGET_DATA& data = m_targets[i];
		int idTarget = data.idTarget;
		ACString strName;

		if (ISNPCID(idTarget))
		{
			CECNPC* pNPC = NULL;
			if (data.dwModifier & CECAttackEvent::MOD_SUCCESS)
				pNPC = pGameRun->GetWorld()->GetNPCMan()->GetNPCFromAll(idTarget);
			else
			{
				pNPC = pGameRun->GetWorld()->GetNPCMan()->GetNPCFromAll(idTarget);
				if (!pNPC)
					return true;

				strName = pNPC->GetNameToShow();
			}

			if (!pNPC)
				return true;
			
			pNPC->Damaged(data.nDamage, data.dwModifier);
		}
		else if (ISPLAYERID(idTarget))
		{
			CECPlayer* pPlayer = pGameRun->GetWorld()->GetPlayerMan()->GetPlayer(idTarget);
			if (!pPlayer)
				return true;

			strName = GetPlayerName(pPlayer);

			pPlayer->Damaged(data.nDamage, data.dwModifier,m_idSkill);
		}

		if (data.nDamage > 0)
		{
			if (m_idHost == idHostPlayer)
			{
				if (!strName.IsEmpty())
					pGameRun->AddFixedChannelMsg(FIXMSG_DODAMAGE, GP_CHAT_DAMAGE, strName, data.nDamage);
			}
			else if (data.idTarget == idHostPlayer)
			{
				if (!strHostName.IsEmpty())
					pGameRun->AddFixedChannelMsg(FIXMSG_BEDAMAGED, GP_CHAT_DAMAGE, strHostName, data.nDamage);
			}
		}
	}

	return true;
}

bool CECAttackEvent::UpdateTargetFlag()
{
	// update all targets' bAboutToDie flag

	int nNumTargets = m_targets.size();
	for(int i=0; i<nNumTargets; i++)
	{
		const TARGET_DATA& data = m_targets[i];
		/*
		if( data.dwModifier & MOD_DEADLYSTRIKE )
		{
			int idTarget = data.idTarget;
			if (ISNPCID(idTarget))
			{
				CECNPC* pNPC = g_pGame->GetGameRun()->GetWorld()->GetNPCMan()->GetNPC(idTarget);
				if (!pNPC)
					return true;
				
				pNPC->SetAboutToDie(true);
			}
			else if (ISPLAYERID(idTarget))
			{
				CECPlayer* pPlayer = g_pGame->GetGameRun()->GetWorld()->GetPlayerMan()->GetPlayer(idTarget);
				if (!pPlayer)
					return true;

				pPlayer->SetAboutToDie(true);
			}
		}*/
	}

	return true;
}

bool CECAttackEvent::Tick(DWORD dwDeltaTime)
{
	m_timeLived += dwDeltaTime;

	if( !m_bSignaled )
	{
		if( m_timeLived > 3500 )
		{
			// too long time, this event will be deleted now
			m_bFinished = true;
			DoFire();
			DoDamage();
		}

		return true;
	}
	else
	{
		if( m_timeToBeFired )
		{
			if( m_timeToBeFired <= dwDeltaTime )
			{
				m_timeToBeFired = 0;

				// Fire here
				DoFire();
			}
			else
				m_timeToBeFired -= dwDeltaTime;
		}	
		else if( m_timeToDoDamage )
		{
			if( m_timeToDoDamage <= dwDeltaTime )
			{
				m_timeToDoDamage = 0;

				// Do damage here
				DoDamage();
			}
			else
				m_timeToDoDamage -= dwDeltaTime;
		}
	}

	return true;
}

bool CECAttackEvent::Stop()
{
	m_bFinished = true;
	return true;
}

bool CECAttackEvent::AddTarget(int idTarget, DWORD dwModifier, int nDamage)
{
	TARGET_DATA data;

	data.idTarget = idTarget;
	data.nDamage = nDamage;
	data.dwModifier = dwModifier;

	m_targets.push_back(data);

	UpdateTargetFlag();
	return true;
}

//////////////////////////////////////////////////////////////////////////////////////
// class CECAttackerEvents
//////////////////////////////////////////////////////////////////////////////////////
void CECAttackerEvents::Add(CECAttackEvent *pEvent)
{
	if (pEvent){
		m_list.push_back(pEvent);
	}
}

void CECAttackerEvents::Signal()
{
	if (!IsEmpty()){
		for (EventList::iterator it = m_list.begin(); it != m_list.end(); ++ it)
		{
			CECAttackEvent *pCur = *it;
			pCur->m_bSignaled = true;
		}
		m_list.clear();
	}
}

CECAttackEvent * CECAttackerEvents::Find(int idSkill/* =0 */, int nSkillSection/* =0 */)
{
	CECAttackEvent *pEvent = NULL;
	for (EventList::iterator it = m_list.begin(); it != m_list.end(); ++ it)
	{
		CECAttackEvent *pCur = *it;
		if (idSkill == pCur->m_idSkill && nSkillSection == pCur->m_nSkillSection){
			pEvent = pCur;
			break;
		}
	}
	return pEvent;
}

//////////////////////////////////////////////////////////////////////////////////////
// class CECAttacksMan
//////////////////////////////////////////////////////////////////////////////////////
CECAttacksMan::CECAttacksMan(CECGameRun* pGameRun)
: CECManager(pGameRun), m_AttackList(128)
{
	m_pSkillGfxComposerMan = new A3DSkillGfxComposerMan();
	
	// now load all skills composer
	unsigned int idSkill = 0;
	
	// now build player's skill action list
	while(true)
	{
		idSkill = GNET::ElementSkill::NextSkill(idSkill);
		if( idSkill == 0 )
			break;
	
		const char * pszSGCFile = GNET::ElementSkill::GetEffect(idSkill);
		while( pszSGCFile[0] == '\\' )
			pszSGCFile ++;

		char szSGCFile[MAX_PATH];
		if( pszSGCFile[0] == '\0' )
			strcpy(szSGCFile, "gfx\\sgc\\nosuchthing.sgc");
		else
			sprintf(szSGCFile, "gfx\\sgc\\%s", pszSGCFile);

		if( af_IsFileExist(szSGCFile) )
		{
			if( !m_pSkillGfxComposerMan->LoadOneComposer(idSkill, szSGCFile) )
			{
				// a_LogOutput(1, "CECAttacksMan::CECAttacksMan(), failed to load skill [%d]'s gfx composer [%s]", idSkill, szSGCFile);
			}
		}
	}

	char szMultiSectionFile[MAX_PATH] = {0};
	strcpy(szMultiSectionFile, "configs\\multi_section_skill.txt");
	m_pMultiSkillGfxComposerMan = new CECMultiSectionSkillMan();
	if (!m_pMultiSkillGfxComposerMan||!m_pMultiSkillGfxComposerMan->LoadConfig(szMultiSectionFile))
	{
		a_LogOutput(1, "CECAttacksMan::CECAttacksMan(), failed to load multi skill sgc config file [%s]", szMultiSectionFile);
	}

	strcpy(szMultiSectionFile, "configs\\skill_state_action.txt");
	if(!LoadSkillStateActionConfig(szMultiSectionFile))
		a_LogOutput(1, "CECAttacksMan::CECAttacksMan(), failed to load multi skill action config file [%s]", szMultiSectionFile);
}

CECAttacksMan::~CECAttacksMan()
{
	if( m_pSkillGfxComposerMan )
	{
		m_pSkillGfxComposerMan->Release();
		delete m_pSkillGfxComposerMan;
		m_pSkillGfxComposerMan = NULL;
	}
	if (m_pMultiSkillGfxComposerMan)
	{
		delete m_pMultiSkillGfxComposerMan;
		m_pMultiSkillGfxComposerMan = NULL;
	}

	m_SkillStateActionVec.clear();
}

bool CECAttacksMan::Tick(DWORD dwDeltaTime)
{
	ALISTPOSITION pos = m_AttackList.GetHeadPosition();

	while (pos)
	{
		ALISTPOSITION curPos = pos;
		CECAttackEvent& attack = m_AttackList.GetNext(pos);

		if( attack.m_bFinished )
		{
			m_AttackList.RemoveAt(curPos);
		}
		else
			attack.Tick(dwDeltaTime);
	}

	return true;
}

CECAttackEvent * CECAttacksMan::AddMeleeAttack(int idHost, int idTarget, int idWeapon, DWORD dwModifier, int nDamage, int nTimeFly)
{
	CECAttackEvent newEvent(this, idHost, 0, idTarget, idWeapon, 0, 0, dwModifier, nDamage, 200, nTimeFly);
	m_AttackList.AddTail(newEvent);

	newEvent.UpdateTargetFlag();
	return &m_AttackList.GetTail();
}

CECAttackEvent * CECAttacksMan::AddSkillAttack(int idHost, int idCastTarget, int idTarget, int idWeapon, int idSkill, int nSkillLevel, DWORD dwModifier, int nDamage)
{
	CECAttackEvent newEvent(this, idHost, idCastTarget, idTarget, idWeapon, idSkill, nSkillLevel, dwModifier, nDamage, 200, 1000);
	m_AttackList.AddTail(newEvent);

	newEvent.UpdateTargetFlag();
	return &m_AttackList.GetTail();
}

CECAttackerEvents CECAttacksMan::FindAttackByAttacker(int idHost)
{
	CECAttackerEvents result;

	ALISTPOSITION pos = m_AttackList.GetHeadPosition();
	while (pos)
	{
		ALISTPOSITION curPos = pos;
		CECAttackEvent& attack = m_AttackList.GetNext(pos);
		if( attack.m_idHost == idHost ){
			result.Add(&attack);
		}
	}

	return result;
}
bool CECAttacksMan::GetSkillSectionActionSuffix(int skill,int section,AString& suffix)
{
	if (m_pMultiSkillGfxComposerMan)
	{
		const CECMultiSectionSkillMan::SectionInfo* info = m_pMultiSkillGfxComposerMan->GetSecionInfo(skill,section);
		if (info && info->action_suffix != "0") // 0 表示动作没有后缀
		{
			suffix = info->action_suffix;
			return true;
		}
	}
	return false;
}
bool CECAttacksMan::LoadSkillStateActionConfig(const char* szFile)
{
	AScriptFile sf;
	if (!sf.Open(szFile))
	{
		a_LogOutput(1, "CECAttacksMan::LoadSkillStateActionConfig, failed to open file %s",szFile);
		return false;
	}
	
	m_SkillStateActionVec.clear();
	while (sf.PeekNextToken(true))
	{
		sf.GetNextToken(true);

		CSplit split(sf.m_szToken);
		CSplit::VectorAString v = split.Split(",");
		if (v.size()<3) // 3个值： 技能，状态包，被打动作，晕倒动作
		{
			ASSERT(0);
			sf.Close();
			return false;
		}
		SkillStateAction act;
		act.skill = v[0].ToInt();
		act.state = v[1].ToInt();
		act.beHitAction = v[2];
		if(v.size()>3)
			act.stayDownAction = v[3];

		m_SkillStateActionVec.push_back(act);
	}
	
	sf.Close();	
	return true;
}
bool CECAttacksMan::GetSkillStateActionName(int skill,int state,AString& name1,AString& name2)
{
	for (int i=0;i<(int)m_SkillStateActionVec.size();i++)
	{
		if (m_SkillStateActionVec[i].skill == skill && m_SkillStateActionVec[i].state == state)
		{
			name1 = m_SkillStateActionVec[i].beHitAction;
			name2 = m_SkillStateActionVec[i].stayDownAction;
			return true;
		}
	}
	return false;
}
//////////////////////////////////////////////////////////////////////////
CECMultiSectionSkillMan::~CECMultiSectionSkillMan()
{
	m_MultiSectionSkillComposerVec.clear();

	SgcName2ComposerMap::iterator itr = m_SgcName2ComposerMap.begin();
	for (;itr!=m_SgcName2ComposerMap.end();++itr)
		delete itr->second;

	m_SgcName2ComposerMap.clear();
}
bool CECMultiSectionSkillMan::LoadConfig(const char* szFile)
{
	AScriptFile sf;
	if (!sf.Open(szFile))
	{
		a_LogOutput(1, "CECMultiSectionSkillMan::LoadConfig, failed to open file %s",szFile);
		return false;
	}
	
	int skill = 0;
	
	while (sf.GetNextToken(true))
	{
		//	skill id
		skill = sf.GetNextTokenAsInt(false);
		
		if (!sf.MatchToken("{", false))
		{
			ASSERT(0);
			sf.Close();
			return false;
		}
		
		while (sf.PeekNextToken(true))
		{
			if (!stricmp(sf.m_szToken, "}"))
			{
				sf.GetNextToken(true);
				break;
			}
			
			sf.GetNextToken(true); // 读取每个地图配置
			
			CSplit split(sf.m_szToken);
			CSplit::VectorAString v = split.Split(",");
			if (v.size()<2) // 3个值： 段号，后缀，sgc
			{
				ASSERT(0);
				sf.Close();
				return false;
			}
			
			int section = v[0].ToInt();
			AString suffix = v[1];

			AString sgc;
			if (v.size()>=3)
				sgc = v[2];
			
			MultiSectionSkillComposerVec::iterator vecItr = m_MultiSectionSkillComposerVec.begin();
			for (;vecItr!=m_MultiSectionSkillComposerVec.end();++vecItr)
			{
				if(skill == vecItr->skill_id && section == vecItr->section)
					break;
			}
			if(vecItr!= m_MultiSectionSkillComposerVec.end())
				continue;

			A3DSkillGfxComposer* pComposer = NULL;

			if (!sgc.IsEmpty())
			{
				SgcName2ComposerMap::iterator itr = m_SgcName2ComposerMap.find(sgc);
				if (itr!= m_SgcName2ComposerMap.end())
				{
					pComposer = itr->second;
				}
				else
				{
					pComposer = new A3DSkillGfxComposer();
					if (!pComposer->Load(sgc))
					{
						delete pComposer;
						pComposer = NULL;
					}			
					else
						pComposer->Init(g_pGame->GetGameRun()->GetWorld()->GetSkillGfxMan()->GetPtr());
					
					m_SgcName2ComposerMap[sgc] = pComposer;
				}
			}
			

			SectionInfo info;
			info.skill_id = skill;
			info.section = section;
			info.action_suffix = suffix;
			info.pComposer = pComposer;

			m_MultiSectionSkillComposerVec.push_back(info);
		}
	}
	
	sf.Close();
	
	return true;
}
void CECMultiSectionSkillMan::Play(
								  int nSkillID, int section,
								  clientid_t nHostID,
								  clientid_t nCastTargetID,
								  const abase::vector<TARGET_DATA>& Targets,
								  bool bIsGoblinSkill)
{
	MultiSectionSkillComposerVec::iterator vecItr = m_MultiSectionSkillComposerVec.begin();
	for (;vecItr!=m_MultiSectionSkillComposerVec.end();++vecItr)
	{
		if(nSkillID == vecItr->skill_id && section == vecItr->section && vecItr->pComposer)
		{
			vecItr->pComposer->Play(nHostID, nCastTargetID, Targets, bIsGoblinSkill);		
			return;
		}
	}
}
const A3DSkillGfxComposer* CECMultiSectionSkillMan::GetSkillGfxComposer(int skill, int section)
{
	MultiSectionSkillComposerVec::iterator vecItr = m_MultiSectionSkillComposerVec.begin();
	for (;vecItr!=m_MultiSectionSkillComposerVec.end();++vecItr)
	{
		if(skill == vecItr->skill_id && section == vecItr->section && vecItr->pComposer)
		{
			return vecItr->pComposer;
		}
	}
	return NULL;
}
const CECMultiSectionSkillMan::SectionInfo* CECMultiSectionSkillMan::GetSecionInfo(int skill,int section)
{
	MultiSectionSkillComposerVec::iterator vecItr = m_MultiSectionSkillComposerVec.begin();
	for (;vecItr!=m_MultiSectionSkillComposerVec.end();++vecItr)
	{
		if(skill == vecItr->skill_id && section == vecItr->section)
		{
			return vecItr;
		}
	}
	return NULL;
}