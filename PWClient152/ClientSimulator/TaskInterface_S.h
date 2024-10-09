/*
 * FILE: TaskInterface_S.h
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Shizhenhua, 2013/8/15
 *
 * HISTORY: 
 *
 * Copyright (c) 2011 ACE Studio, All Rights Reserved.
 */


#pragma once

#include <hashmap.h>
#include "TaskInterface.h"
#include "TaskClient.h"

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

class PlayerObject;
class ATaskTemplMan;


///////////////////////////////////////////////////////////////////////////
//	
//	Declare of Global functions
//	
///////////////////////////////////////////////////////////////////////////

ATaskTemplMan* GetTaskTemplMan();


///////////////////////////////////////////////////////////////////////////
//	
//	Class TaskInterface_S
//	
///////////////////////////////////////////////////////////////////////////

struct talk_proc;

class TaskInterface_S : public TaskInterface
{
public:		//	Types

public:		//	Constructor and Destructor

	TaskInterface_S(PlayerObject* pHost);
	virtual ~TaskInterface_S();

public:		//	Attributes

public:		//	Operations

	//	Initialize object
	bool Init(void* pActiveListBuf, int iActiveListLen, void* pFinishedListBuf, int iFinishedListLen, void* pFinishedTimeListBuf, int iFinishedTimeListLen, void* pFinishedCountListBuf, int iFinishedCountListLen,void* pStorageTaskListBuf,int iStorageTaskListLen);
	//	Release object
	void Release();

	virtual int GetFactionContrib();
	virtual int GetFactionConsumeContrib();
	virtual int GetFactionExpContrib();
	virtual void TakeAwayFactionConsumeContrib(int ulNum){}
	virtual void TakeAwayFactionExpContrib(int ulNum){}
	virtual unsigned long GetPlayerLevel();
	virtual void* GetActiveTaskList();
	virtual void* GetFinishedTaskList();
	virtual void* GetFinishedTimeList();
	virtual void* GetFinishedCntList();
	virtual void* GetStorageTaskList();
	virtual int GetTaskItemCount(unsigned long ulTaskItem);
	virtual int GetCommonItemCount(unsigned long ulCommonItem);
	virtual bool IsInFaction(unsigned long ulFactionId);
	virtual int GetFactionRole();
	virtual unsigned long GetGoldNum();
	virtual long GetReputation();
	virtual unsigned long GetCurPeriod();
	virtual unsigned long GetPlayerId();
	virtual unsigned long GetPlayerRace();
	virtual unsigned long GetPlayerOccupation();

	virtual unsigned long* GetTaskMask() { return NULL; }
	virtual void DeliverGold(unsigned long ulGoldNum) {}
	virtual void DeliverExperience(unsigned long ulExp) {}
	virtual void DeliverSP(unsigned long ulSP) {}
	virtual void DeliverReputation(long ulReputation) {}
	virtual void DeliverFactionContrib(int iConsumeContrib,int iExpContrib) {}
	virtual bool CastSkill(int skill_id, int skill_level){ return true;}
	virtual void TakeAwayGold(unsigned long ulNum) {}
	virtual void TakeAwayTaskItem(unsigned long ulTemplId, unsigned long ulNum) {}
	virtual void TakeAwayCommonItem(unsigned long ulTemplId, unsigned long ulNum) {}
	virtual bool IsDeliverLegal();
	virtual bool CanDeliverCommonItem(unsigned long);
	virtual bool CanDeliverTaskItem(unsigned long);
	virtual void DeliverCommonItem(unsigned long ulItem, unsigned long ulNum, long lPeriod = 0) {}
	virtual void DeliverTaskItem(unsigned long ulItem, unsigned long ulNum) {}

	virtual int GetTeamMemberNum();
	virtual void GetTeamMemberInfo(int nIndex, task_team_member_info* pInfo);
	virtual unsigned long GetTeamMemberId(int nIndex);
	virtual bool IsCaptain();
	virtual bool IsInTeam();
	virtual bool IsMale();
	virtual bool IsGM();
	virtual bool IsShieldUser();
	virtual bool IsMarried();
	virtual bool IsWeddingOwner();
	virtual unsigned long GetPos(float pos[3]);
	virtual void NotifyServer(const void* pBuf, size_t sz);
	virtual bool HasLivingSkill(unsigned long ulSkill);
	virtual long GetLivingSkillProficiency(unsigned long ulSkill);
	virtual long GetLivingSkillLevel(unsigned long ulSkill);
	virtual void GetSpecailAwardInfo(special_award* p);
	virtual void DisplayGlobalValue(long lKey, long lValue);
	virtual long GetGlobalValue(long lKey);
	virtual unsigned long GetRoleCreateTime();
	virtual unsigned long GetRoleLastLoginTime();
	virtual unsigned long GetAccountTotalCash();
	virtual unsigned long GetSpouseID();

	virtual size_t GetInvEmptySlot();
	virtual void LockInventory(bool is_lock) {};
	virtual unsigned char GetShapeMask();
	virtual bool IsAtCrossServer();

	//势力相关
	virtual int GetForce();
	virtual int GetForceContribution();
	virtual int GetForceReputation();
	virtual bool ChangeForceContribution(int iValue);
	virtual bool ChangeForceReputation(int iValue);
	virtual int GetExp();
	virtual int GetSP();
	virtual bool ReduceExp(int iExp);
	virtual bool ReduceSP(int iSP);
	virtual int GetForceActivityLevel();
	virtual void AddForceActivity(int iActivity);

	virtual bool CanGetLoginReward();
	virtual void GetLoginReward();

	virtual bool IsKing();
	virtual bool HaveGotTitle(unsigned long id_designation);
	virtual void DeliverTitle(unsigned long id_designation, long lPeriod = 0);
	virtual bool CheckRefine(unsigned long level_refine, unsigned long num_equips);
	bool IsTitleDataReady();
	virtual int GetCurHistoryStageIndex();

protected:	//	Attributes

	PlayerObject*	m_pHost;
	void*			m_pActiveListBuf;	//	Active task list buffer
	void*			m_pFinishedListBuf;	//	Finished task list buffer
	void*			m_pFinishedTimeListBuf; //	Finished time list buffer
	void*			m_pFinishedCountListBuf;//  Finished count list buffer  
	void*			m_pStorageTaskListBuf;  //  Storage tasks list buffer

};

///////////////////////////////////////////////////////////////////////////
//	
//	Inline functions
//	
///////////////////////////////////////////////////////////////////////////
