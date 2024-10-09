/*
 * FILE: EC_ManAttacks.h
 *
 * DESCRIPTION: Manager to manager all attack effects and results. 
 *
 * CREATED BY: Hedi, 2005/3/10
 *
 * HISTORY: 
 *
 * Copyright (c) 2004 Archosaur Studio, All Rights Reserved.
 */

#ifndef _EC_MANATTACKS_H_
#define _EC_MANATTACKS_H_

#include "EC_Manager.h"
#include "AList2.h"
#include "vector.h"
#include "A3DSkillGfxComposer2.h"

// base class for all attack events
class CECAttackEvent
{
	friend class CECAttacksMan;

public:
	enum
	{
		MOD_PHYSIC_ATTACK_RUNE	= 0x0001,		//	物理攻击优化符生效
		MOD_MAGIC_ATTACK_RUNE	= 0x0002,		//	法术攻击优化符生效
		MOD_PHYSIC_DEFENCE_RUNE = 0x0004,		//	物理防御优化符生效
		MOD_MAGIC_DEFENCE_RUNE	= 0x0008,		//	法术防御优化符生效
		MOD_CRITICAL_STRIKE		= 0x0010,		//	爆击
		MOD_RETORT				= 0x0020,		//	反震
		MOD_NULLITY				= 0x0040,		//	无效攻击
		MOD_IMMUNE				= 0x0080,		//	免疫了此次攻击，优先级高于无效
		MOD_ENCHANT_FAILED		= 0x0100,		//	enchant 失败
		MOD_SUCCESS				= 0x0200,		//	成功
		MOD_DODGE_DAMAGE		= 0x0400,		//  伤害躲闪
		MOD_DODGE_DEBUFF		= 0x0800,		//  状态躲闪
		MOD_ATTACK_AURA			= 0x1000,		//  光环攻击
		MOD_REBOUND				= 0x2000,		//  反弹
		MOD_BEAT_BACK			= 0x4000,		//  反击
	};

public:
	CECAttacksMan * m_pManager;		// attacks event manager

	bool		m_bSignaled;		// flag indicates whether this attack event can be 
									// issued to create the bullet and then do fire work
	bool		m_bDoFired;			// flag indicates whether this attack event has done firing
	bool		m_bDoDamaged;		// flag indicates whether this attack event has done damage
	bool		m_bFinished;		// flag inidcates this event has been finished and should be removed now

	DWORD		m_timeLived;		// how long this attack event has been generated
	DWORD		m_timeToBeFired;	// how long this event will be fired after being signaled
	DWORD		m_timeToDoDamage;	// how long this event will do damage to the target

	int			m_idHost;			// owner of the attack, may be player, may be monster
	int			m_idCastTarget;		// target being casted
	abase::vector<TARGET_DATA> m_targets; // targets data array
	
	int			m_idWeapon;			// attack by what kind of weapon
	int			m_idSkill;			// attack used what kind of skill
	int			m_nSkillLevel;		// the level of skill used
	int			m_nSkillSection;	// current skill section for multi-secton skill

	/*
	DWORD		m_dwModifier;		// modifier to the attack result, 
									// 0x1 - physic attack rune
									// 0x2 - magic attack rune
									// 0x4 - physic defence rune
									// 0x8 - magic defence rune
									// 0x10 - physic critical strike
									// 0x40 - dodge
									// 0x80 - deadly strike
	
	int			m_nDamage;			// value of damage caused by this attack
	*/

protected:
	bool DoFire();
	bool DoDamage();

	bool UpdateTargetFlag();

public:
	CECAttackEvent();
	CECAttackEvent(CECAttacksMan * pManager, int idHost, int idCastTarget, int idTarget, int idWeapon, int idSkill, int nSkillLevel, DWORD dwModifier, int nDamage, int nTimeToBeFired, int nTimeToDoDamage);
	~CECAttackEvent();

	bool Tick(DWORD dwDeltaTime);

	bool Stop();
	bool AddTarget(int idTarget, DWORD dwModifier, int nDamage);
	void SetSkillSection(int nSection) {m_nSkillSection = nSection;}
};

class CECAttackerEvents
{
	typedef abase::vector<CECAttackEvent *> EventList;
	EventList	m_list;
public:
	void Add(CECAttackEvent *);
	bool IsEmpty()const{ return m_list.empty(); }
	int  Count()const{ return (int)m_list.size(); }

	CECAttackEvent * Find(int idSkill=0, int nSkillSection=0);
	void Signal();
	operator bool()const{ return !IsEmpty(); }
};

class CECMultiSectionSkillMan;
class CECAttacksMan : public CECManager
{
protected:
	AList2<CECAttackEvent, const CECAttackEvent&>			m_AttackList;	// attack event list
	A3DSkillGfxComposerMan *								m_pSkillGfxComposerMan;	// skill gfx composer mananger
	CECMultiSectionSkillMan*								m_pMultiSkillGfxComposerMan;

	struct SkillStateAction
	{
		int skill;
		int state;
		AString beHitAction; // 被击中动作
		AString stayDownAction;//倒在地上的动作
	};
	typedef abase::vector<SkillStateAction> SkillSateActionVec;

	SkillSateActionVec m_SkillStateActionVec;

public:
	inline A3DSkillGfxComposerMan * GetSkillGfxComposerMan() { return m_pSkillGfxComposerMan; }
	CECMultiSectionSkillMan*		GetMultiSkillGfxComposerMan() const { return m_pMultiSkillGfxComposerMan;}

public:
	CECAttacksMan(CECGameRun* pGameRun);
	~CECAttacksMan();

	bool Tick(DWORD dwDeltaTime);

	CECAttackEvent * AddMeleeAttack(int idHost, int idTarget, int idWeapon, DWORD dwModifier, int nDamage, int nTimeFly=10);
	CECAttackEvent * AddSkillAttack(int idHost, int idCastTarget, int idTarget, int idWeapon, int idSkill, int nSkillLevel, DWORD dwModifier, int nDamage);

	CECAttackerEvents FindAttackByAttacker(int idHost);

	bool GetSkillSectionActionSuffix(int skill,int section,AString& suffix);

	bool LoadSkillStateActionConfig(const char* szFile);
	bool GetSkillStateActionName(int skill,int state,AString& name1,AString& name2);
};

class CECMultiSectionSkillMan
{
public:
	typedef abase::hash_map<AString, A3DSkillGfxComposer*> SgcName2ComposerMap; // 用于多段技能的sgc对象
	
	struct SectionInfo 
	{
		SectionInfo(){ skill_id = 0; section = 1; pComposer = NULL;}
		int skill_id;
		unsigned char section;
		AString action_suffix;
		A3DSkillGfxComposer* pComposer;
	};
	typedef abase::vector<SectionInfo> MultiSectionSkillComposerVec; // 

public:
	CECMultiSectionSkillMan(){}
	~CECMultiSectionSkillMan();
	bool LoadConfig(const char* szFile);
	void Play(int nSkillID, int section,clientid_t nHostID,	clientid_t nCastTargetID,const abase::vector<TARGET_DATA>& Targets,bool bIsGoblinSkill=false);
	const A3DSkillGfxComposer* GetSkillGfxComposer(int skill, int section);
	const SectionInfo* GetSecionInfo(int skill,int section);

protected:
	SgcName2ComposerMap m_SgcName2ComposerMap;
	MultiSectionSkillComposerVec m_MultiSectionSkillComposerVec;
};

#endif//_EC_MANATTACKS_H
