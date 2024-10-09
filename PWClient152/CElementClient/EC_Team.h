/*
 * FILE: EC_Team.h
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Duyuxin, 2004/12/13
 *
 * HISTORY: 
 *
 * Copyright (c) 2004 Archosaur Studio, All Rights Reserved.
 */

#pragma once

#include "AArray.h"
#include "A3DVector.h"
#include "AAssist.h"
#include "EC_GPDataType.h"

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


///////////////////////////////////////////////////////////////////////////
//	
//	Declare of Global functions
//	
///////////////////////////////////////////////////////////////////////////

class CECTeam;

namespace GNET
{
	class GetPlayerBriefInfo_Re;
}

namespace S2C
{
	struct cmd_team_member_data;
}

///////////////////////////////////////////////////////////////////////////
//	
//	Class CECTeamMember
//	
///////////////////////////////////////////////////////////////////////////

class CECTeamMember
{
public:		//	Types

public:		//	Constructor and Destructor

	CECTeamMember(CECTeam* pTeam, int cid);
	virtual ~CECTeamMember() {}

public:		//	Attributes

public:		//	Operations

	//	Get member name
	const ACHAR* GetName();

	//	Is all information ready ?
	bool IsInfoReady() { return m_bInfoReady; }
	void SetInfoReadyFlag(bool bTrue) { m_bInfoReady = bTrue; }

	//	Set / Get properties
	int GetCharacterID() { return m_cid; }
	void SetLevel(int iLevel) { m_iLevel = iLevel; }
	int GetLevel() { return m_iLevel; }
	void SetLevel2(int iLevel2) { m_iLevel2 = iLevel2; }
	int GetLevel2() { return m_iLevel2; }
	void SetWallowLevel(int iLevel) { m_iWallowLevel = iLevel; }
	int GetWallowLevel() { return m_iWallowLevel; }
	void SetCurHP(int iHP) { m_iCurHP = iHP; }
	int GetCurHP() { return m_iCurHP; }
	void SetCurMP(int iMP) { m_iCurMP = iMP; }
	int GetCurMP() { return m_iCurMP; }
	void SetMaxHP(int iHP) { m_iMaxHP = iHP; }
	int GetMaxHP() { return m_iMaxHP; }
	void SetMaxMP(int iMP) { m_iMaxMP = iMP; }
	int GetMaxMP() { return m_iMaxMP; }
	void SetPos(int idInst, const A3DVECTOR3& vPos, bool bSameInstance) { m_idInst = idInst; m_vPos = vPos; m_bSameInstance = bSameInstance; }
	const A3DVECTOR3& GetPos() { return m_vPos; }
	int GetInstanceID() { return m_idInst; }
	bool GetSameInstance(){ return m_bSameInstance; }
	bool IsSameInstanceTeamMember();
	void SetProfession(int iProfession) { m_iProfession = iProfession; }
	int GetProfession() { return m_iProfession; }
	bool IsFighting() { return m_bFight; }
	void SetFightFlag(bool bTrue) { m_bFight = bTrue; }
	void SetGender(int iGender) { m_iGender = iGender; }
	int GetGender() { return m_iGender; }
	void SetForce(int iForce){m_iForce = iForce;}
	int GetForce(){return m_iForce;}
	void SetProfitLevel(int iLevel) { m_iProfitLevel = iLevel; }
	int GetProfitLevel() const { return m_iProfitLevel; }
	void SetReincarnationCount(int count){ m_iReincarnationCount = count; }
	int GetRenicarnationCount(){ return m_iReincarnationCount; }

	//	Reset icon states
	void ResetIconStates(const S2C::IconStates &s);
	
	//	Get icon state array
	const S2C::IconStates& GetIconStates() const { return m_aIconStates; }

protected:	//	Attributes

	bool		m_bInfoReady;	//	true, this member's all info is ready

	CECTeam*	m_pTeam;		//	Team object
	int			m_cid;			//	Character ID
	int			m_iLevel;
	int			m_iLevel2;
	int			m_iWallowLevel;
	int			m_iCurHP;
	int			m_iCurMP;
	int			m_iMaxHP;
	int			m_iMaxMP;
	bool		m_bFight;		//	true, in fight state

	int			m_idInst;		//	ID of instance
	A3DVECTOR3	m_vPos;			//	Position
	bool		m_bSameInstance;	//	在同一地图副本中
	int			m_iProfession;	//	Profession
	int			m_iGender;		//	Gender
	int			m_iForce;		//  Force
	int			m_iProfitLevel;	//  Profit level
	int			m_iReincarnationCount;

	S2C::IconStates	m_aIconStates; 	//	Icon states (un-visible extend states)

protected:	//	Operations

};

///////////////////////////////////////////////////////////////////////////
//	
//	Class CECTeam
//	
///////////////////////////////////////////////////////////////////////////

class CECTeam
{
public:		//	Types

public:		//	Constructor and Destructor

	CECTeam(int idLeader);
	virtual ~CECTeam();

public:		//	Attributes

public:		//	Operations

	//	Add a new member
	void AddMember(int idMember);
	//	Remove member
	void RemoveMember(int idMember);

	//	Get member number
	int GetMemberNum() { return m_aMembers.GetSize(); }
	//	Get member by player ID
	CECTeamMember* GetMemberByID(int id, int* piIndex=NULL);
	//	Get member by index
	CECTeamMember* GetMemberByIndex(int n)
	{
		if (n >= 0 && n < m_aMembers.GetSize())
			return m_aMembers[n]; 
		else
			return NULL;
	}
	//	Update team data
	void UpdateTeamData(const S2C::cmd_team_member_data& Data);
	//	Update team member information
	void UpdateMemberInfo(GNET::GetPlayerBriefInfo_Re& p);

	//	Get team leader's ID
	int GetLeaderID() { return m_idLeader; }
	//	Set team leader's ID
	void ChangeTeamLeader(int idLeader);
	//	Get team leader
	CECTeamMember* GetLeader();
	//	Set pickup flag
	void SetPickupFlag(WORD wFlag) { m_wPickFlag = wFlag; }
	//	Get pickup flag
	WORD GetPickupFlag() { return m_wPickFlag; }

	//	Add ID of team member whose name is still unknown 
	void AddUnknownID(int id) { m_aUnknownIDs.UniquelyAdd(id); }

protected:	//	Attributes

	int		m_idLeader;		//	ID of team leader
	WORD	m_wPickFlag;	//	Pickup flag

	APtrArray<CECTeamMember*>	m_aMembers;
	AArray<int, int>			m_aUnknownIDs;	//	ID of players we only known whose ID but not name

protected:	//	Operations

};

///////////////////////////////////////////////////////////////////////////
//	
//	Inline functions
//	
///////////////////////////////////////////////////////////////////////////


