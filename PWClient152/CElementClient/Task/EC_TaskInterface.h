/*
 * FILE: EC_TaskInterface.h
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Duyuxin, 2005/2/3
 *
 * HISTORY: 
 *
 * Copyright (c) 2005 Archosaur Studio, All Rights Reserved.
 */

#pragma once

#include "TaskInterface.h"
#include "TaskClient.h"
#include "hashmap.h"
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

class CECHostPlayer;
class ATaskTemplMan;

///////////////////////////////////////////////////////////////////////////
//	
//	Declare of Global functions
//	
///////////////////////////////////////////////////////////////////////////

ATaskTemplMan* GetTaskTemplMan();

///////////////////////////////////////////////////////////////////////////
//	
//	Class CECTaskInterface
//	
///////////////////////////////////////////////////////////////////////////

struct talk_proc;
class CECTaskInterface : public TaskInterface
{
public:		//	Types

public:		//	Constructor and Destructor

	CECTaskInterface(CECHostPlayer* pHost);
	virtual ~CECTaskInterface();

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

	typedef abase::hash_map<int, bool> MAP_CONFIRM_TASK;
	const MAP_CONFIRM_TASK& GetConfirmTasksMap(){return m_TasksToConfirm;};
	void SetTaskReadyToConfirm(int iTaskID, bool bReady);
	void ConfirmFinishTask(int iTaskID);
	bool IsTaskReadyToConfirm(int iTaskID);
	void UpdateConfirmTasksMap();
	virtual bool HaveGotTitle(unsigned long id_designation);
	virtual void DeliverTitle(unsigned long id_designation, long lPeriod = 0);
	virtual bool CheckRefine(unsigned long level_refine, unsigned long num_equips);
	bool IsTitleDataReady();
	void ChooseAward(unsigned short taskid, unsigned char choice);

	virtual int GetCurHistoryStageIndex();
	virtual bool CheckSimpleTaskFinshConditon(unsigned long task_id) const;

	void SetEmotion(int emotion); // emotion is CMD_EMOTION_BINDBUDDY when bindbuddy,
	void UpdateTaskEmotionAction(unsigned int task_id);
	void UpdateEmotionDlg(unsigned int task = 0); // 判断是否有表情动作任务，如果有则弹出表情图标

	unsigned long GetMaxHistoryLevel();
	unsigned long GetReincarnationCount();
	unsigned long GetRealmLevel();
	bool IsRealmExpFull();
	void DeliverRealmExp(unsigned long exp){};
	void ExpandRealmLevelMax(){};

	// 打怪获取物品的任务发起自动组队
	void DoAutoTeamForTask(int item_id);
	virtual unsigned int	 GetObtainedGeneralCardCount() ;
	virtual void AddLeaderShip(unsigned long leader_ship);
	virtual unsigned long GetObtainedGeneralCardCountByRank(int rank);

	ACString FormatTaskTalk(ACHAR* task_talk);
	void DeliverTokenShopTask(unsigned int task);
	bool CanFinishTaskSpendingWorldContribution(unsigned short taskID);
	void FinishTaskSpendingWorldContribution(unsigned short taskID, unsigned char choice = 0);

	virtual bool CheckTaskForbid(unsigned long task_id){ return false; }
	int GetWorldContribution();
	void DeliverWorldContribution(int contribution) {}
	void TakeAwayWorldContribution(int contribution) {}
	int GetWorldContributionSpend();
	bool PlayerCanSpendContributionAsWill();

	virtual void OnNewTask(int iTaskID);
	virtual void OnGiveupTask(int iTaskID);
	virtual void OnCompleteTask(int iTaskID);
	
	void SetForceNavigateFinishFlag(bool bFinish) { m_bForceNavigateFinish = bFinish;} // 
	bool GetForceNavigateFinishFlag() const { return m_bForceNavigateFinish;}



protected:	//	Attributes

	CECHostPlayer*	m_pHost;
	void*			m_pActiveListBuf;	//	Active task list buffer
	void*			m_pFinishedListBuf;	//	Finished task list buffer
	void*			m_pFinishedTimeListBuf; //	Finished time list buffer
	void*			m_pFinishedCountListBuf;//  Finished count list buffer  
	void*			m_pStorageTaskListBuf;  //  Storage tasks list buffer
	MAP_CONFIRM_TASK	m_TasksToConfirm; //  待确定完成的任务 

	
	abase::hash_map<int,bool> m_emotionTask; // 标记表情任务是否完成
	bool			m_bForceNavigateFinish; // 强制移动任务是否完成

protected:	//	Operations
	void OnAddNewTaskToConfirm();
	void OnTaskConfirmUpdate();

};

///////////////////////////////////////////////////////////////////////////
//	
//	Inline functions
//	
///////////////////////////////////////////////////////////////////////////


