/*
 * FILE: EC_NPCServer.h
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Duyuxin, 2005/1/4
 *
 * HISTORY: 
 *
 * Copyright (c) 2005 Archosaur Studio, All Rights Reserved.
 */

#pragma once

#include "EC_NPC.h"
#include "AArray.h"

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

struct NPC_ESSENCE;
struct MONSTER_ESSENCE;

class CECSkill;

///////////////////////////////////////////////////////////////////////////
//	
//	Declare of Global functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Class CECNPCServer
//	
///////////////////////////////////////////////////////////////////////////

class CECNPCServer : public CECNPC
{
public:		//	Types

	//	Quest icon index
	enum
	{
		QI_NONE = -1,
		QI_OUT = 0,
		QI_IN,
		QI_OUT_N,
		QI_IN_N,
		QI_OUT_K,	
		QI_IN_K,

		QI_OUT_TYPE1,
		QI_IN_TYPE1,
		QI_OUT_TYPE2,
		QI_IN_TYPE2,
		QI_OUT_TYPE3,
		QI_IN_TYPE3,
		QI_OUT_TYPE4,
		QI_IN_TYPE4,	
	};

public:		//	Constructor and Destructor

	CECNPCServer(CECNPCMan* pNPCMan);
	virtual ~CECNPCServer();

public:		//	Attributes

public:		//	Operations

	//	Initlaize object
	virtual bool Init(int tid, const S2C::info_npc& Info);
	//	Release object
	virtual void Release();

	//	Tick routine
	virtual bool Tick(DWORD dwDeltaTime);
	//	Render routine
	virtual bool Render(CECViewport* pViewport, int iRenderFlag=0);
	virtual bool RenderHighLight(CECViewport* pViewport);

	//	Get NPC name color
	virtual DWORD GetNameColor();
	//	Is monster in invader camp in battle ?
	virtual bool IsInBattleInvaderCamp();
	//	Is monster in defender camp in battle ?
	virtual bool IsInBattleDefenderCamp();
	//	Get role in battle
	virtual int GetRoleInBattle();

	//	Get tax rate
	float GetTaxRate() { return m_fTaxRate; }
	//	Get item price scale factor
	float GetPriceScale();
	//	Get skill number
	int GetSkillNum() { return m_aSkills.GetSize(); }
	//	Get skill by index
	CECSkill* GetSkill(int n) { return m_aSkills[n]; }
	//	Get pet skill number
	int GetPetSkillNum() { return m_aPetSkills.GetSize(); }
	//	Get pet skill
	CECSkill* GetPetSkill(int n) { return m_aPetSkills[n]; }
	//	Set / Get show quest icon flag
	void SetQuestIconFlag(bool bShow) { m_bQuestIcon = bShow; }
	bool GetQuestIconFlag() { return m_bQuestIcon; }
	//	Build skill list
	bool BuildSkillList(int idSkillSevice);
	//	Build pet skill list
	bool BuildPetSkillList();
	//	Get current quest icon
	int GetCurQuestIcon() { return m_iQuestIcon; }
	//	Get way point ID bound with this NPC
	DWORD GetWayPointID();
	
	virtual bool IsImmuneDisable();

	virtual const MONSTER_ESSENCE * GetMonsterEssence()const{ return m_pMonEssence; }

	//	Get essence data in database
	const NPC_ESSENCE* GetDBEssence() { return m_pDBEssence; }

protected:	//	Attributes

	//	Data in database
	NPC_ESSENCE*		m_pDBEssence;
	MONSTER_ESSENCE*	m_pMonEssence;

	float	m_fTaxRate;		//	Tax rate
	bool	m_bQuestIcon;	//	true show quest icon
	int		m_iQuestIcon;	//	Current quest icon

	APtrArray<CECSkill*>	m_aSkills;		//	Skill array
	APtrArray<CECSkill*>	m_aPetSkills;	//	Pet skill array
	CECCounter				m_TaskCounter;	//	Task time counter

protected:	//	Operations

	//	Render quest icon on header
	bool RenderQuestIcon(CECViewport* pViewport);
	//	Update current qeust icon index
	void UpdateCurQuestIcon();
	//	Release all skills
	void ReleaseAllSkills();
	//	Release all pet skills
	void ReleaseAllPetSkills();
};

///////////////////////////////////////////////////////////////////////////
//	
//	Inline functions
//	
///////////////////////////////////////////////////////////////////////////

