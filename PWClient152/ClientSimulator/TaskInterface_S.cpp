/*
 * FILE: TaskInterface_S.cpp
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Shizhenhua, 2013/8/15
 *
 * HISTORY: 
 *
 * Copyright (c) 2011 ACE Studio, All Rights Reserved.
 */


#include <time.h>

#include "Common.h"
#include "TaskInterface_S.h"
#include "TaskTemplMan.h"
#include "elementdataman.h"
#include "PlayerObject.h"
#include "GameObject.h"
#include "GameSession.h"
#include "SessionManager.h"
#include "EC_SendC2SCmds.h"

#include <AMemory.h>

#define new A_DEBUG_NEW

///////////////////////////////////////////////////////////////////////////
//	
//	Define and Macro
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Reference to External variables and functions
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


///////////////////////////////////////////////////////////////////////////
//	
//	Implement TaskInterface_S
//	
///////////////////////////////////////////////////////////////////////////

TaskInterface_S::TaskInterface_S(PlayerObject* pHost)
{
	m_pHost	= pHost;

	m_pActiveListBuf		= NULL;
	m_pFinishedListBuf		= NULL;
	m_pFinishedTimeListBuf	= NULL;
	m_pFinishedCountListBuf = NULL;
	m_pStorageTaskListBuf	= NULL;
}

TaskInterface_S::~TaskInterface_S()
{
}

//	Initialize object
bool TaskInterface_S::Init(void* pActiveListBuf, int iActiveListLen, void* pFinishedListBuf,
	int iFinishedListLen, void* pFinishedTimeListBuf, int iFinishedTimeListLen, 
	void* pFinishedCountListBuf, int iFinishedCountListLen,void* pStorageTaskListBuf,int iStorageTaskListLen)
{
	if (!(m_pActiveListBuf = a_malloc(TASK_ACTIVE_LIST_BUF_SIZE)))
	{
		glb_ErrorOutput(ECERR_NOTENOUGHMEMORY, "TaskInterface_S::Init", __LINE__);
		return false;
	}

	if (!(m_pFinishedListBuf = a_malloc(TASK_FINISHED_LIST_BUF_SIZE)))
	{
		a_free(m_pActiveListBuf);
		m_pFinishedListBuf = NULL;
		glb_ErrorOutput(ECERR_NOTENOUGHMEMORY, "TaskInterface_S::Init", __LINE__);
		return false;
	}

	if (!(m_pFinishedTimeListBuf = a_malloc(TASK_FINISH_TIME_LIST_BUF_SIZE)))
	{
		a_free(m_pActiveListBuf);
		a_free(m_pFinishedListBuf);
		glb_ErrorOutput(ECERR_NOTENOUGHMEMORY, "TaskInterface_S::Init", __LINE__);
		return false;
	}

	if (!(m_pFinishedCountListBuf = a_malloc(TASK_FINISH_COUNT_LIST_BUF_SIZE)))
	{
		a_free(m_pActiveListBuf);
		a_free(m_pFinishedListBuf);
		a_free(m_pFinishedCountListBuf);
		glb_ErrorOutput(ECERR_NOTENOUGHMEMORY, "TaskInterface_S::Init", __LINE__);
		return false;
	}

	if (!(m_pStorageTaskListBuf = a_malloc(TASK_STORAGE_LIST_BUF_SIZE)))
	{
		a_free(m_pActiveListBuf);
		a_free(m_pFinishedListBuf);
		a_free(m_pFinishedCountListBuf);
		a_free(m_pStorageTaskListBuf);
		glb_ErrorOutput(ECERR_NOTENOUGHMEMORY, "TaskInterface_S::Init", __LINE__);
		return false;
	}
	ASSERT(pActiveListBuf && pFinishedListBuf && pFinishedTimeListBuf && pFinishedCountListBuf);
	memcpy(m_pActiveListBuf, pActiveListBuf, iActiveListLen);
	memcpy(m_pFinishedListBuf, pFinishedListBuf, iFinishedListLen);
	memcpy(m_pFinishedTimeListBuf, pFinishedTimeListBuf, iFinishedTimeListLen);
	memcpy(m_pFinishedCountListBuf, pFinishedCountListBuf, iFinishedCountListLen);
	memcpy(m_pStorageTaskListBuf,pStorageTaskListBuf,iStorageTaskListLen);

	//	Clear rest buffer
	if (iActiveListLen < TASK_ACTIVE_LIST_BUF_SIZE)
		memset((BYTE*)m_pActiveListBuf+iActiveListLen, 0, TASK_ACTIVE_LIST_BUF_SIZE-iActiveListLen);

	if (iFinishedListLen < TASK_FINISHED_LIST_BUF_SIZE)
		memset((BYTE*)m_pFinishedListBuf+iFinishedListLen, 0, TASK_FINISHED_LIST_BUF_SIZE-iFinishedListLen);

	if (iFinishedTimeListLen < TASK_FINISH_TIME_LIST_BUF_SIZE)
		memset((BYTE*)m_pFinishedTimeListBuf+iFinishedTimeListLen, 0, TASK_FINISH_TIME_LIST_BUF_SIZE-iFinishedTimeListLen);

	if (iFinishedCountListLen < TASK_FINISH_COUNT_LIST_BUF_SIZE)
		memset((BYTE*)m_pFinishedCountListBuf+iFinishedCountListLen, 0, TASK_FINISH_COUNT_LIST_BUF_SIZE-iFinishedCountListLen);

	if (iStorageTaskListLen < TASK_STORAGE_LIST_BUF_SIZE)
		memset((BYTE*)m_pStorageTaskListBuf+iStorageTaskListLen, 0, TASK_STORAGE_LIST_BUF_SIZE-iStorageTaskListLen);

	ATaskTemplMan* pTaskMan = GetTaskTemplMan();
	pTaskMan->Release();
	pTaskMan->LoadTasksFromPack("data\\tasks.data", true);
	pTaskMan->LoadNPCInfoFromPack("data\\task_npc.data");
	pTaskMan->VerifyDynTasksPack("userdata\\dyn_tasks.data");
	InitActiveTaskList();

	return true;
}

//	Release object
void TaskInterface_S::Release()
{
	if (m_pActiveListBuf)
	{
		a_free(m_pActiveListBuf);
		m_pActiveListBuf = NULL;
	}

	if (m_pFinishedListBuf)
	{
		a_free(m_pFinishedListBuf);
		m_pFinishedListBuf = NULL;
	}

	if (m_pFinishedTimeListBuf)
	{
		a_free(m_pFinishedTimeListBuf);
		m_pFinishedTimeListBuf = NULL;
	}

	if (m_pFinishedCountListBuf)
	{
		a_free(m_pFinishedCountListBuf);
		m_pFinishedCountListBuf = NULL;
	}	
	
	if (m_pStorageTaskListBuf)
	{
		a_free(m_pStorageTaskListBuf);
		m_pStorageTaskListBuf = NULL;
	}
}

int TaskInterface_S::GetFactionContrib()
{
	return 0;
}

int TaskInterface_S::GetFactionConsumeContrib()
{
	return 0;
}

int TaskInterface_S::GetFactionExpContrib()
{
	return 0;
}

unsigned long TaskInterface_S::GetPlayerLevel()
{
	return (DWORD)m_pHost->GetLevel();
}

void* TaskInterface_S::GetActiveTaskList()
{
	return m_pActiveListBuf;
}

void* TaskInterface_S::GetFinishedTaskList()
{
	return m_pFinishedListBuf;
}

void* TaskInterface_S::GetFinishedTimeList()
{
	return m_pFinishedTimeListBuf;
}

void* TaskInterface_S::GetFinishedCntList()
{
	return m_pFinishedCountListBuf;
}

void* TaskInterface_S::GetStorageTaskList()
{
	return m_pStorageTaskListBuf;
}

int TaskInterface_S::GetTaskItemCount(unsigned long ulTaskItem)
{
	return 0;
}

int TaskInterface_S::GetCommonItemCount(unsigned long ulCommonItem)
{
	return m_pHost->GetItemCount(PlayerObject::PACK_INVENTORY, ulCommonItem);
}

bool TaskInterface_S::IsInFaction(unsigned long ulFactionId)
{
	return false;
}

int TaskInterface_S::GetFactionRole()
{
	return 0;
}

unsigned long TaskInterface_S::GetGoldNum()
{
	return (DWORD)m_pHost->GetMoney();
}

long TaskInterface_S::GetReputation()
{
	return 0;
}

unsigned long TaskInterface_S::GetCurPeriod()
{
	return (DWORD)m_pHost->GetLevel2();
}

unsigned long TaskInterface_S::GetPlayerId()
{
	return m_pHost->GetRoleID();
}

unsigned long TaskInterface_S::GetPlayerRace()
{
	return 0;
}

unsigned long TaskInterface_S::GetPlayerOccupation()
{
	return (DWORD)m_pHost->GetProfession();
}

void TaskInterface_S::NotifyServer(const void* pBuf, size_t sz)
{
	SessionManager::GetSingleton().SetTheSession(m_pHost->GetGameObject()->GetGameSession());
	c2s_SendCmdTaskNotify(pBuf, sz);
}

int TaskInterface_S::GetTeamMemberNum()
{
	return 0;
}

void TaskInterface_S::GetTeamMemberInfo(int nIndex, task_team_member_info* pInfo)
{
}

unsigned long TaskInterface_S::GetTeamMemberId(int nIndex)
{
	return 0;
}

bool TaskInterface_S::IsCaptain()
{
	return false;
}

bool TaskInterface_S::IsInTeam()
{
	return false;
}

bool TaskInterface_S::IsMale()
{
	return m_pHost->IsMale();
}

bool TaskInterface_S::IsGM()
{
	return false;
}

bool TaskInterface_S::IsShieldUser()
{
	return false;
}

bool TaskInterface_S::IsMarried()
{
	return false;
}

bool TaskInterface_S::IsWeddingOwner()
{
	return false;
}

unsigned long TaskInterface_S::GetPos(float pos[3])
{
	A3DVECTOR3 vPos = m_pHost->GetCurPos();
	pos[0] = vPos.x;
	pos[1] = vPos.y;
	pos[2] = vPos.z;
	return 1;
}

bool TaskInterface_S::HasLivingSkill(unsigned long ulSkill)
{
	return false;
}

long TaskInterface_S::GetLivingSkillProficiency(unsigned long ulSkill)
{
	return 0;
}

long TaskInterface_S::GetLivingSkillLevel(unsigned long ulSkill)
{
	return 0;
}

void TaskInterface_S::GetSpecailAwardInfo(special_award* p)
{
	*p = *GetTaskTemplMan()->GetSpecialAward();
}

bool TaskInterface_S::IsDeliverLegal()
{
	return true;
}

bool TaskInterface_S::CanDeliverCommonItem(unsigned long ulTypes)
{
	return true;
}

bool TaskInterface_S::CanDeliverTaskItem(unsigned long ulTypes)
{
	return true;
}

void TaskInterface_S::DisplayGlobalValue(long lKey, long lValue)
{
}

long TaskInterface_S::GetGlobalValue(long lKey)
{
	return 0;
}

unsigned long TaskInterface_S::GetSpouseID()
{
	return 0;
}

unsigned long TaskInterface_S::GetRoleCreateTime()
{
	return 0;
}

unsigned long TaskInterface_S::GetRoleLastLoginTime()
{
	return 0;
}

unsigned long TaskInterface_S::GetAccountTotalCash()
{
	return 0;
}

size_t TaskInterface_S::GetInvEmptySlot() 
{
	return 0;
}

unsigned char TaskInterface_S::GetShapeMask()
{
	return 0;
}

int TaskInterface_S::GetForce()
{
	return 0;
}

int TaskInterface_S::GetForceContribution()
{
	return 0;
}

int TaskInterface_S::GetForceReputation()
{
	return 0;
}

bool TaskInterface_S::ChangeForceContribution(int iValue)
{
	return false;
}

bool TaskInterface_S::ChangeForceReputation(int iValue)
{
	return false;
}

int TaskInterface_S::GetExp()
{
	return m_pHost->GetExp();
}

int TaskInterface_S::GetSP()
{
	return m_pHost->GetSP();
}

bool TaskInterface_S::ReduceExp(int iExp)
{
	return true;
}

bool TaskInterface_S::ReduceSP(int iSP)
{
	return true;
}

int TaskInterface_S::GetForceActivityLevel()
{
	return -1;
}

void TaskInterface_S::AddForceActivity(int iActivity)
{
}

bool TaskInterface_S::CanGetLoginReward()
{
	return false;
}

void TaskInterface_S::GetLoginReward()
{
	task_notify_base notify;
	notify.reason = TASK_CLT_NOTIFY_15DAYS_NOLOGIN;
	notify.task = 0;

	NotifyServer(&notify, sizeof(notify));
}

bool TaskInterface_S::IsAtCrossServer()
{
	return false;
}

bool TaskInterface_S::IsTitleDataReady()
{
	return false;
}

bool TaskInterface_S::IsKing()
{	
	return false;
}

bool TaskInterface_S::HaveGotTitle(unsigned long id_designation)
{
	return false;
}

void TaskInterface_S::DeliverTitle(unsigned long id_designation, long lPeriod)
{
}

bool TaskInterface_S::CheckRefine(unsigned long level_refine, unsigned long num_equips)
{
	return false;
}

int TaskInterface_S::GetCurHistoryStageIndex()
{
	return 0;
}


///////////////////////////////////////////////////////////////////////////
//  
//  TaskInterface Static Function
//  
///////////////////////////////////////////////////////////////////////////

unsigned long TaskInterface::GetCurTime()
{
	return time(0);
}

int TaskInterface::GetTimeZoneBias()
{
	return -480;
}

void TaskInterface::WriteLog(int nPlayerId,	int nTaskId, int nType, const char* szLog)
{
}

void TaskInterface::WriteKeyLog(int nPlayerId, int nTaskId, int nType, const char* szLog)
{
}

void TaskInterface::TraceTask(unsigned long idTask)
{
}

void TaskInterface::ShowPunchBagMessage(bool bSucced,bool bMax,unsigned long MonsterTemplID,int dps,int dph)
{
}

void TaskInterface::PopEmotionUI(unsigned int task_id,unsigned int uiEmotion,bool bShow)
{
}

void TaskInterface::UpdateTitleUI(unsigned long ulTask)
{
}

void TaskInterface::UpdateTaskUI(int reason)
{
}

void TaskInterface::ShowTaskMessage(unsigned long ulTask, int reason)
{
}

void TaskInterface::NotifyServerStorageTasks()
{
	_notify_svr(this, TASK_CLT_NOTIFY_STORAGE, 0);
}

///////////////////////////////////////////////////////////////////////////

int PublicQuestInterface::GetCurSubTask(int task_id)
{
	return 0;
}

int PublicQuestInterface::GetCurTaskStamp(int task_id)
{
	return 0;
}

// int PublicQuestInterface::GetCurSubTaskStamp(int task_id)
// {
// 	return 0;
//}

int PublicQuestInterface::GetCurContrib(int task_id, int role_id)
{
	return 0;
}

int PublicQuestInterface::GetCurAllPlace(int task_id, int role_id)
{
	return 0;
}

int PublicQuestInterface::GetCurClsPlace(int task_id, int role_id)
{
	return 0;
}

long PublicQuestInterface::QuestGetGlobalValue(long lKey)
{
	return 0;
}