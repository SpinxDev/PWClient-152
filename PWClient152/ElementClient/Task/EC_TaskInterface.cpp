/*
 * FILE: EC_TaskInterface.cpp
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Duyuxin, 2005/2/3
 *
 * HISTORY: 
 *
 * Copyright (c) 2005 Archosaur Studio, All Rights Reserved.
 */

#include <time.h>

#include "EC_Global.h"
#include "EC_TaskInterface.h"
#include "EC_HostPlayer.h"
#include "EC_Inventory.h"
#include "EC_Game.h"
#include "EC_GameSession.h"
#include "EC_GameRun.h"
#include "EC_GameUIMan.h"
#include "EC_UIManager.h"
#include "EC_Team.h"
#include "EC_Skill.h"
#include "EC_World.h"
#include "TaskTemplMan.h"
#include "elementdataman.h"
#include "EC_ForceMgr.h"
#include "EC_FixedMsg.h"
#include "EC_CrossServer.h"
#include "EC_UIHelper.h"
#include "EC_AutoTeam.h"
#include "EC_IvtrEquipMatter.h"
#include "EC_HPWorkForceNavigate.h"

#include "ATime.h"

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

static bool _task_dialog_is_show = false;

///////////////////////////////////////////////////////////////////////////
//	
//	Local functions
//	
///////////////////////////////////////////////////////////////////////////

ATaskTemplMan* GetTaskTemplMan()
{
	return g_pGame->GetTaskTemplateMan();
}

///////////////////////////////////////////////////////////////////////////
//	
//	Implement CECTaskInterface
//	
///////////////////////////////////////////////////////////////////////////

CECTaskInterface::CECTaskInterface(CECHostPlayer* pHost)
{
	m_pHost	= pHost;

	m_pActiveListBuf		= NULL;
	m_pFinishedListBuf		= NULL;
	m_pFinishedTimeListBuf	= NULL;
	m_pFinishedCountListBuf = NULL;
	m_pStorageTaskListBuf	= NULL;
}

CECTaskInterface::~CECTaskInterface()
{
}

//	Initialize object
bool CECTaskInterface::Init(void* pActiveListBuf, int iActiveListLen, void* pFinishedListBuf,
	int iFinishedListLen, void* pFinishedTimeListBuf, int iFinishedTimeListLen, 
	void* pFinishedCountListBuf, int iFinishedCountListLen,void* pStorageTaskListBuf,int iStorageTaskListLen)
{
	if (!(m_pActiveListBuf = a_malloc(TASK_ACTIVE_LIST_BUF_SIZE)))
	{
		glb_ErrorOutput(ECERR_NOTENOUGHMEMORY, "CECTaskInterface::Init", __LINE__);
		return false;
	}

	if (!(m_pFinishedListBuf = a_malloc(TASK_FINISHED_LIST_BUF_SIZE)))
	{
		a_free(m_pActiveListBuf);
		m_pFinishedListBuf = NULL;
		glb_ErrorOutput(ECERR_NOTENOUGHMEMORY, "CECTaskInterface::Init", __LINE__);
		return false;
	}

	if (!(m_pFinishedTimeListBuf = a_malloc(TASK_FINISH_TIME_LIST_BUF_SIZE)))
	{
		a_free(m_pActiveListBuf);
		a_free(m_pFinishedListBuf);
		glb_ErrorOutput(ECERR_NOTENOUGHMEMORY, "CECTaskInterface::Init", __LINE__);
		return false;
	}

	if (!(m_pFinishedCountListBuf = a_malloc(TASK_FINISH_COUNT_LIST_BUF_SIZE)))
	{
		a_free(m_pActiveListBuf);
		a_free(m_pFinishedListBuf);
		a_free(m_pFinishedCountListBuf);
		glb_ErrorOutput(ECERR_NOTENOUGHMEMORY, "CECTaskInterface::Init", __LINE__);
		return false;
	}

	if (!(m_pStorageTaskListBuf = a_malloc(TASK_STORAGE_LIST_BUF_SIZE)))
	{
		a_free(m_pActiveListBuf);
		a_free(m_pFinishedListBuf);
		a_free(m_pFinishedCountListBuf);
		a_free(m_pStorageTaskListBuf);
		glb_ErrorOutput(ECERR_NOTENOUGHMEMORY, "CECTaskInterface::Init", __LINE__);
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

	m_bForceNavigateFinish = false;

	return true;
}

//	Release object
void CECTaskInterface::Release()
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

void TaskInterface::PopChatMessage(int iIndex,int dwNum)
{
	switch(iIndex)
	{
	case FIXMSG_TASK_LIMIT_INCREASED:
		g_pGame->GetGameRun()->AddFixedChannelMsg(FIXMSG_TASK_LIMIT_INCREASED,GP_CHAT_SYSTEM);
		break;
	default:
		break;
	}
}
void TaskInterface::NotifyServerStorageTasks()
{
	_notify_svr(this, TASK_CLT_NOTIFY_STORAGE, 0);
}
void TaskInterface::UpdateTaskUI(unsigned long idTask, int reason)
{
	CECGameUIMan* pGameUI = g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan();
	if (pGameUI)
	{
		pGameUI->UpdateTask(idTask, reason);
	}
}

void TaskInterface::TraceTask(unsigned long idTask)
{
	CECGameUIMan* pGameUI = g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan();
	if (pGameUI)
	{
		pGameUI->TraceTask(idTask);
	}
}

void TaskInterface::UpdateTitleUI(unsigned long ulTask)
{
	CECUIHelper::OnDeliverChallengeTask((unsigned short)ulTask);
}

void TaskInterface::PopupTaskFinishDialog(unsigned long ulTaskId, const talk_proc* pTalk)
{
	CECGameUIMan* pGameUI = g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan();

	if (pGameUI && pTalk->num_window)
		pGameUI->PopupTaskFinishDialog(ulTaskId, const_cast<talk_proc*>(pTalk));
}

void TaskInterface::ShowMessage(const wchar_t* szMsg)
{
	CECGameUIMan* pGameUI = g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan();
	if (pGameUI) pGameUI->ShowErrorMsg(szMsg);
}

void TaskInterface::ShowPunchBagMessage(bool bSucced,bool bMax,unsigned long MonsterTemplID,int dps,int dph)
{
	CECGameUIMan* pGameUI = g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan();

	if (pGameUI == NULL) return;

	elementdataman *pDataMan = g_pGame->GetElementDataMan();

	DATA_TYPE dt;
	MONSTER_ESSENCE *pMonster = (MONSTER_ESSENCE *)pDataMan->get_data_ptr(MonsterTemplID, ID_SPACE_ESSENCE, dt);

	ACString strNpcName;
	if( dt == DT_MONSTER_ESSENCE )
	{
		if( pMonster)
			strNpcName = pMonster->name;
	}

	ACString str;
	if (!bSucced)
	{
		str.Format(pGameUI->GetStringFromTable(303),strNpcName);
		pGameUI->MessageBox("",str,MB_OK,A3DCOLORRGBA(255, 255, 255, 160));
	}
	
	else if (bMax)
	{
		str.Format(pGameUI->GetStringFromTable(304),dph);
		pGameUI->MessageBox("",str,MB_OK,A3DCOLORRGBA(255, 255, 255, 160));
	}
	else
	{
		str.Format(pGameUI->GetStringFromTable(305),dph);
		pGameUI->MessageBox("",str,MB_OK,A3DCOLORRGBA(255, 255, 255, 160));
	}

	 
}
void TaskInterface::ShowTaskMessage(unsigned long ulTask, int reason)
{
	CECGameUIMan* pGameUI = g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan();
	if (pGameUI) pGameUI->AddTaskHint(ulTask, reason);
}

int CECTaskInterface::GetFactionContrib()
{
	return m_pHost->GetContribInfo()->cumulate_contrib;
}
int CECTaskInterface::GetFactionConsumeContrib()
{
	return m_pHost->GetContribInfo()->consume_contrib;
}

int CECTaskInterface::GetFactionExpContrib()
{
	return m_pHost->GetContribInfo()->exp_contrib;
}

unsigned long CECTaskInterface::GetPlayerLevel()
{
	return (DWORD)m_pHost->GetBasicProps().iLevel;
}

void* CECTaskInterface::GetActiveTaskList()
{
	return m_pActiveListBuf;
}

void* CECTaskInterface::GetFinishedTaskList()
{
	return m_pFinishedListBuf;
}

void* CECTaskInterface::GetFinishedTimeList()
{
	return m_pFinishedTimeListBuf;
}

void* CECTaskInterface::GetFinishedCntList()
{
	return m_pFinishedCountListBuf;
}

void* CECTaskInterface::GetStorageTaskList()
{
	return m_pStorageTaskListBuf;
}
int CECTaskInterface::GetTaskItemCount(unsigned long ulTaskItem)
{
	CECInventory* pPack = m_pHost->GetTaskPack();
	return pPack ? pPack->GetItemTotalNum((int)ulTaskItem) : 0;
}

int CECTaskInterface::GetCommonItemCount(unsigned long ulCommonItem)
{
	CECInventory* pPack = m_pHost->GetPack();
	return pPack ? pPack->GetItemTotalNum((int)ulCommonItem) : 0;
}

bool CECTaskInterface::IsInFaction(unsigned long ulFactionId)
{
	return m_pHost->GetFactionID() != 0;
}

int CECTaskInterface::GetFactionRole()
{
	return m_pHost->GetFRoleID();
}

unsigned long CECTaskInterface::GetGoldNum()
{
	return (DWORD)m_pHost->GetMoneyAmount();
}

unsigned long TaskInterface::GetCurTime()
{
	// use this to avoid task hack by changing the system time
	return g_pGame->GetServerAbsTime();
}

int TaskInterface::GetTimeZoneBias()
{
	return g_pGame->GetTimeZoneBias();
}

void TaskInterface::WriteLog(int nPlayerId,	int nTaskId, int nType, const char* szLog)
{
}

void TaskInterface::WriteKeyLog(int nPlayerId, int nTaskId, int nType, const char* szLog)
{
}

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
	return g_pGame->GetGameRun()->GetCommonData(lKey);
}

long CECTaskInterface::GetReputation()
{
	return m_pHost->GetReputation();
}

unsigned long CECTaskInterface::GetCurPeriod()
{
	return (DWORD)m_pHost->GetBasicProps().iLevel2;
}

unsigned long CECTaskInterface::GetPlayerId()
{
	return m_pHost->GetCharacterID();
}

unsigned long CECTaskInterface::GetPlayerRace()
{
	return (DWORD)m_pHost->GetRace();
}

unsigned long CECTaskInterface::GetPlayerOccupation()
{
	return (DWORD)m_pHost->GetProfession();
}

void CECTaskInterface::NotifyServer(const void* pBuf, size_t sz)
{
	g_pGame->GetGameSession()->c2s_CmdTaskNotify(pBuf, sz);
}

int CECTaskInterface::GetTeamMemberNum()
{
	CECTeam* pTeam = m_pHost->GetTeam();
	return pTeam ? pTeam->GetMemberNum() : 0;
}

void CECTaskInterface::GetTeamMemberInfo(int nIndex, task_team_member_info* pInfo)
{
	CECTeam* pTeam = m_pHost->GetTeam();
	if (!pTeam)
	{
		memset(pInfo, 0, sizeof (task_team_member_info));
		return;
	}

	CECTeamMember* pMember = pTeam->GetMemberByIndex(nIndex);
	if (!pMember)
	{
		memset(pInfo, 0, sizeof (task_team_member_info));
		return;
	}

	pInfo->m_ulId			= pMember->GetCharacterID();
	pInfo->m_ulLevel		= pMember->GetLevel();
	pInfo->m_ulOccupation	= pMember->GetProfession();
	pInfo->m_ulWorldTag		= pMember->GetInstanceID();
	pInfo->m_bMale			= (pMember->GetGender() == GENDER_MALE);
	pInfo->m_iForce			= pMember->GetForce();

	const A3DVECTOR3& v = pMember->GetPos();
	pInfo->m_Pos[0]	= v.x;
	pInfo->m_Pos[1]	= v.y;
	pInfo->m_Pos[2]	= v.z;
}

unsigned long CECTaskInterface::GetTeamMemberId(int nIndex)
{
	CECTeam* pTeam = m_pHost->GetTeam();
	if (!pTeam)
		return 0;
		
	CECTeamMember* pMember = pTeam->GetMemberByIndex(nIndex);
	return pMember ? (unsigned long)pMember->GetCharacterID() : 0;
}

bool CECTaskInterface::IsCaptain()
{
	CECTeam* pTeam = m_pHost->GetTeam();
	if (!pTeam)
		return false;

	return pTeam->GetLeaderID() == m_pHost->GetCharacterID();
}

bool CECTaskInterface::IsInTeam()
{
	return m_pHost->GetTeam() ? true : false;
}

bool CECTaskInterface::IsMale()
{
	return m_pHost->GetGender() == GENDER_MALE;
}

bool CECTaskInterface::IsGM()
{
	return m_pHost->IsGM();
}

bool CECTaskInterface::IsShieldUser()
{
	return m_pHost->IsShieldUser();
}

bool CECTaskInterface::IsMarried()
{
	return m_pHost->GetSpouse() != 0;
}

bool CECTaskInterface::IsWeddingOwner()
{
	int iCid = m_pHost->GetCharacterID();
	const CECHostPlayer::WEDDING_SCENE_INFO* wInfo = m_pHost->GetWeddingSceneInfo();
	return iCid == wInfo->bride || iCid == wInfo->groom;
}

unsigned long CECTaskInterface::GetPos(float pos[3])
{
	A3DVECTOR3 vPos = m_pHost->GetPos();
	pos[0] = vPos.x;
	pos[1] = vPos.y;
	pos[2] = vPos.z;
	return g_pGame->GetGameRun()->GetWorld()->GetInstanceID();
}

bool CECTaskInterface::HasLivingSkill(unsigned long ulSkill)
{
	return m_pHost->GetNormalSkill(ulSkill) != 0;
}

long CECTaskInterface::GetLivingSkillProficiency(unsigned long ulSkill)
{
	return m_pHost->GetSkillAbility(ulSkill);
}

long CECTaskInterface::GetLivingSkillLevel(unsigned long ulSkill)
{
	CECSkill* pSkill = m_pHost->GetNormalSkill(ulSkill);
	return pSkill ? pSkill->GetSkillLevel() : 0;
}

void CECTaskInterface::GetSpecailAwardInfo(special_award* p)
{
	*p = *GetTaskTemplMan()->GetSpecialAward();
}

bool CECTaskInterface::IsDeliverLegal()
{
	return !m_pHost->IsTrading() && m_pHost->GetBoothState() == 0 && !m_pHost->IsDead();
}

bool CECTaskInterface::CanDeliverCommonItem(unsigned long ulTypes)
{
	return m_pHost->GetPack()->GetEmptySlotNum() >= static_cast<int>(ulTypes);
}

bool CECTaskInterface::CanDeliverTaskItem(unsigned long ulTypes)
{
	return m_pHost->GetTaskPack()->GetEmptySlotNum() >= static_cast<int>(ulTypes);
}

void CECTaskInterface::DisplayGlobalValue(long lKey, long lValue)
{
}

long CECTaskInterface::GetGlobalValue(long lKey)
{
	return g_pGame->GetGameRun()->GetCommonData(lKey);
}
unsigned long CECTaskInterface::GetSpouseID()
{
	return m_pHost->GetSpouse();
}

unsigned long CECTaskInterface::GetRoleCreateTime()
{
	return m_pHost->GetRoleCreateTime();
}
unsigned long CECTaskInterface::GetRoleLastLoginTime()
{
	return m_pHost->GetRoleLastLoginTime();
}
unsigned long CECTaskInterface::GetAccountTotalCash()
{
	return m_pHost->GetAccountTotalCash();
}
size_t CECTaskInterface::GetInvEmptySlot() 
{
	return m_pHost->GetPack()->GetEmptySlotNum();
}

unsigned char CECTaskInterface::GetShapeMask()
{
	return m_pHost->GetShapeMask();
}

int CECTaskInterface::GetForce()
{
	return m_pHost->GetForce();
}

int CECTaskInterface::GetForceContribution()
{
	const CECHostPlayer::FORCE_INFO* pInfo = m_pHost->GetForceInfo(m_pHost->GetForce());
	if (pInfo)
	{
		return pInfo->contribution;
	}
	return 0;
}

int CECTaskInterface::GetForceReputation()
{
	const CECHostPlayer::FORCE_INFO* pInfo = m_pHost->GetForceInfo(m_pHost->GetForce());
	if (pInfo)
	{
		return pInfo->reputation;
	}
	return 0;
}

bool CECTaskInterface::ChangeForceContribution(int iValue)
{
	const CECHostPlayer::FORCE_INFO* pInfo = m_pHost->GetForceInfo(m_pHost->GetForce());

	if (pInfo)
	{
		CECHostPlayer::FORCE_INFO info = *pInfo;
		info.contribution += iValue;
		m_pHost->SetForceInfo(m_pHost->GetForce(),&info);
		return true;
	}

	return false;
}

bool CECTaskInterface::ChangeForceReputation(int iValue)
{
	const CECHostPlayer::FORCE_INFO* pInfo = m_pHost->GetForceInfo(m_pHost->GetForce());

	if (pInfo)
	{
		CECHostPlayer::FORCE_INFO info = *pInfo;
		info.reputation += iValue;
		m_pHost->SetForceInfo(m_pHost->GetForce(),&info);
		return true;
	}

	return false;
}

int CECTaskInterface::GetExp()
{
	return m_pHost->GetBasicProps().iExp;
}

int CECTaskInterface::GetSP()
{
	return m_pHost->GetBasicProps().iSP;
}

bool CECTaskInterface::ReduceExp(int iExp)
{
	ROLEBASICPROP basic = m_pHost->GetBasicProps();
	ROLEEXTPROP extend = m_pHost->GetExtendProps();
	basic.iExp -= iExp;
	m_pHost->SetProps(&basic,&extend);
	
	return true;
}

bool CECTaskInterface::ReduceSP(int iSP)
{
	ROLEBASICPROP basic = m_pHost->GetBasicProps();
	ROLEEXTPROP extend = m_pHost->GetExtendProps();
	basic.iSP -= iSP;
	m_pHost->SetProps(&basic,&extend);

	return true;
}

int CECTaskInterface::GetForceActivityLevel()
{
	const CECForceMgr::FORCE_GLOBAL* pForce = m_pHost->GetForceMgr()->GetForceGlobal(m_pHost->GetForce());
	if (pForce)
	{
		return pForce->activity_level;
	}
	return -1;
}

void CECTaskInterface::AddForceActivity(int iActivity)
{

}

bool CECTaskInterface::CanGetLoginReward()
{
	FinishedTaskList* pLst = (FinishedTaskList*)m_pFinishedListBuf;

	if(pLst->SearchTask(26191) == -1)
		return true;
	else
		return false;
}

void CECTaskInterface::GetLoginReward()
{
	task_notify_base notify;
	notify.reason = TASK_CLT_NOTIFY_15DAYS_NOLOGIN;
	notify.task = 0;

	NotifyServer(&notify, sizeof(notify));
}

void CECTaskInterface::SetTaskReadyToConfirm(int iTaskID, bool bReady)
{
	MAP_CONFIRM_TASK::iterator iter = m_TasksToConfirm.find(iTaskID);
	bool bNeedUIShow(false);
	bool bNeedUpdate(false);
	if (iter == m_TasksToConfirm.end())
	{
		m_TasksToConfirm[iTaskID] = bReady;
		bNeedUIShow = bReady;
		bNeedUpdate = bReady;
	}
	else
	{
		bool bWasReady = iter->second;
		iter->second = bReady;
		bNeedUIShow = !bWasReady && bReady;	
		bNeedUpdate = bWasReady != bReady;
	}
	if (bNeedUIShow)
		OnAddNewTaskToConfirm();
	if (bNeedUpdate)
		OnTaskConfirmUpdate();
	
}
// 有新的待确认任务时，界面提醒
void CECTaskInterface::OnAddNewTaskToConfirm()
{
	CECGameUIMan* pGameUI = g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan();
	if (pGameUI) pGameUI->AddNewTaskConfirm();	
}

void CECTaskInterface::OnTaskConfirmUpdate()
{
	CECGameUIMan* pGameUI = g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan();
	if (pGameUI) pGameUI->UpdateTaskConfirm();	
}

void CECTaskInterface::ConfirmFinishTask(int iTaskID)
{
	ATaskTempl* pTempl = GetTaskTemplMan()->GetTaskTemplByID(iTaskID);
	if(pTempl && IsTaskReadyToConfirm(iTaskID))
	{
		_notify_svr(this, TASK_CLT_NOTIFY_CHECK_FINISH, iTaskID);
		m_TasksToConfirm.erase(iTaskID);
		CECGameUIMan* pGameUI = g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan();
		if (pGameUI) pGameUI->UpdateTaskConfirm();		
	}
}

bool CECTaskInterface::IsTaskReadyToConfirm(int iTaskID)
{
	bool ret(false);
	MAP_CONFIRM_TASK::iterator iter = m_TasksToConfirm.find(iTaskID);
	if (iter != m_TasksToConfirm.end())	ret = iter->second;
	return ret;
}

bool CECTaskInterface::IsAtCrossServer()
{
	return CECCrossServer::Instance().IsOnSpecialServer();
}

void CECTaskInterface::UpdateConfirmTasksMap()
{
	m_TasksToConfirm.clear();
	OnTaskCheckStatus(this);
	OnTaskConfirmUpdate();
}
bool CECTaskInterface::IsTitleDataReady()
{
	return m_pHost->IsTitleDataReady();
}
void CECTaskInterface::ChooseAward(unsigned short taskid, unsigned char choice)
{
	ATaskTempl* pTempl = GetTaskTemplMan()->GetTaskTemplByID(taskid);
	if(pTempl && IsTaskReadyToConfirm(taskid))
	{
		_notify_svr_choose_award(this, taskid, choice);
		m_TasksToConfirm.erase(static_cast<int>(taskid));
		CECGameUIMan* pGameUI = g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan();
		if (pGameUI) pGameUI->UpdateTaskConfirm();		
	}	
}

bool CECTaskInterface::IsKing()
{	
	return m_pHost->IsKing();
}
bool CECTaskInterface::HaveGotTitle(unsigned long id_designation)
{
	const CECHostPlayer::TITLE_CONTAINER& titles = m_pHost->GetTitles();
	CECHostPlayer::TITLE title_tofind((unsigned short)id_designation, 0);
	CECHostPlayer::TITLE_CONTAINER::const_iterator iter = std::find(titles.begin(), titles.end(), title_tofind);
	return iter != titles.end();
}
void CECTaskInterface::DeliverTitle(unsigned long id_designation, long lPeriod)
{

}
bool CECTaskInterface::CheckRefine(unsigned long level_refine, unsigned long num_equips)
{
	bool ret(false);
	return ret;
}
int CECTaskInterface::GetCurHistoryStageIndex()
{
	return g_pGame->GetGameRun()->GetCurStageIndex() + 1;
}
bool CECTaskInterface::CheckSimpleTaskFinshConditon(unsigned long task_id) const
{
	ATaskTempl* pTempl = GetTaskTemplMan()->GetTaskTemplByID(task_id);
	if(!pTempl) return false;

	if(pTempl->m_enumMethod == enumTMSimpleClientTask && pTempl->m_uiEmotion)
	{
		abase::hash_map<int,bool>::const_iterator itr = m_emotionTask.find(task_id);
		if(itr!=m_emotionTask.end())
			return itr->second;

		return false;
	}
	else if (pTempl->m_enumMethod == enumTMSimpleClientTaskForceNavi)
	{
		return GetForceNavigateFinishFlag();
	}
	

	return true;
}
void CECTaskInterface::SetEmotion(int emotion)
{
	ActiveTaskList* pList = static_cast<ActiveTaskList*>(GetActiveTaskList());
	ActiveTaskEntry* aEntries = pList->m_TaskEntries;
	unsigned char i;
	const ATaskTempl* pTempl;

	if (emotion < CMD_EMOTION_BINDBUDDY)//  pTempl->m_uiEmotion 值为0表示不检查，所以所有表情序号都后移1
		emotion += 1;

	for (i = 0; i < pList->m_uTaskCount; i++)
	{
		ActiveTaskEntry& CurEntry = aEntries[i];
		pTempl = CurEntry.GetTempl();
		
		if (pTempl && pTempl->m_enumMethod==enumTMSimpleClientTask && pTempl->m_uiEmotion)
		{
			abase::hash_map<int,bool>::iterator itr = m_emotionTask.find(pTempl->GetID());
			if (itr==m_emotionTask.end() || !itr->second)
			{
				m_emotionTask[pTempl->GetID()] = pTempl->m_uiEmotion == (unsigned int)emotion;
			}
		}		
	}
}
void CECTaskInterface::UpdateTaskEmotionAction(unsigned int task_id)
{
	if (m_emotionTask.find(task_id)!=m_emotionTask.end())
	{
		m_emotionTask[task_id] = false;
	}
}
void CECTaskInterface::UpdateEmotionDlg(unsigned int task)
{
	ActiveTaskList* pList = static_cast<ActiveTaskList*>(GetActiveTaskList());
	ActiveTaskEntry* aEntries = pList->m_TaskEntries;
	unsigned char i;
	const ATaskTempl* pTempl, *pTarget = NULL;
	ActiveTaskEntry* pTargetEntry = NULL;

	for (i = 0; i < pList->m_uTaskCount; i++)
	{
		ActiveTaskEntry& CurEntry = aEntries[i];
		pTempl = CurEntry.GetTempl();
		
		if (!pTempl || pTempl->GetID() == task)
			continue;

		if (pTempl && pTempl->m_enumMethod==enumTMSimpleClientTask && pTempl->m_uiEmotion)
		{
			pTarget = pTempl;
			pTargetEntry = &CurEntry;
			break;			
		}
	}
	if (pTarget && pTargetEntry && !pTempl->CanFinishTask(this, pTargetEntry, GetCurTime()))
	{
		PopEmotionUI(pTarget->GetID(),pTarget->m_uiEmotion,true);
	}
}
void TaskInterface::PopEmotionUI(unsigned int task_id,unsigned int uiEmotion,bool bShow)
{
	CECGameUIMan* pGameUI = g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan();
	
	if (pGameUI)
	{
		pGameUI->PopTaskEmotionDlg(task_id,uiEmotion,bShow);
	}
}
unsigned long CECTaskInterface::GetMaxHistoryLevel()
{
	return a_Max(m_pHost->GetBasicProps().iLevel, m_pHost->GetReincarnationTome().max_level);
}
unsigned long CECTaskInterface::GetReincarnationCount()
{
	return m_pHost->GetReincarnationCount();
}
unsigned long CECTaskInterface::GetRealmLevel()
{
	return m_pHost->GetRealmLevel();
}
bool CECTaskInterface::IsRealmExpFull()
{
	return m_pHost->IsRealmExpFull();
}

void CECTaskInterface::DoAutoTeamForTask(int item_id)
{
	ActiveTaskList* pTaskList = static_cast<ActiveTaskList*>(GetActiveTaskList());
	for( int i=0;i<TASK_ACTIVE_LIST_MAX_LEN;i++ )
	{
		if( !pTaskList->m_TaskEntries[i].m_ID )
			continue;

		const ATaskTempl* pTempl = pTaskList->m_TaskEntries[i].GetTempl();
		if( !pTempl ) continue;
		if( pTempl->m_enumMethod != enumTMKillNumMonster )
			continue;

		Task_State_info info;
		GetTaskStateInfo(pTempl->m_ID, &info);
		for( int n=0;n<MAX_ITEM_WANTED;n++ )
		{
			if( info.m_ItemsWanted[n].m_ulItemId == item_id &&
				info.m_ItemsWanted[n].m_ulItemsGained >= 2 )
			{
				m_pHost->GetAutoTeam()->DoAutoTeam(CECAutoTeam::TYPE_TASK, pTempl->m_ID);
				return;
			}
		}
	}
}
unsigned int CECTaskInterface::GetObtainedGeneralCardCount()
{
	return m_pHost ? m_pHost->GetGeneralCardData().GetObtainedCount() : 0;
}
unsigned long CECTaskInterface::GetObtainedGeneralCardCountByRank(int rank)
{
	unsigned long ret = 0;
	CECInventory* pInven[3];
	pInven[0] = m_pHost->GetPack();
	pInven[1] = m_pHost->GetEquipment();
	pInven[2] = m_pHost->GetGeneralCardBox();
	CECIvtrGeneralCard* pCard = NULL;
	int i,j;
	for (i = 0; i < 3; ++i)	{
		if (pInven[i]) {
			int sum = pInven[i]->GetSize();
			for (j = 0; j < sum; ++j) {
				CECIvtrGeneralCard* pCard = dynamic_cast<CECIvtrGeneralCard*>(pInven[i]->GetItem(j));
				if (pCard && pCard->GetEssence().rank == rank) ret++;
			}
		}
	}
	return ret;
}
static const ACHAR* SYMBOL_HOSTNAME = _AL("$name");
ACString CECTaskInterface::FormatTaskTalk(namechar* task_talk)
{
	ACString ret = (wchar_t*)task_talk;
	if (task_talk == NULL) return ret;
	ACString strName = m_pHost->GetName();
	int nSub = ret.Find(SYMBOL_HOSTNAME);
	while (nSub != -1)
	{
		ret = ret.Left(nSub) + _AL("&") + strName + _AL("&") + ret.Right(ret.GetLength() - nSub - wcslen(SYMBOL_HOSTNAME));
		nSub = ret.Find(SYMBOL_HOSTNAME);
	}
	return ret;
}
void CECTaskInterface::AddLeaderShip(unsigned long leader_ship)
{

}

void CECTaskInterface::DeliverTokenShopTask(unsigned int task)
{
	ATaskTempl* pTempl = GetTaskTemplMan()->GetTaskTemplByID(task);
	if(pTempl && pTempl->m_bUsedInTokenShop)
		_notify_svr(this, TASK_CLT_NOTIFY_BUY_TOKENSHOP_ITEM, task);
}

int CECTaskInterface::GetWorldContribution()
{
	return m_pHost->GetWorldContribution();
}

bool CECTaskInterface::CanFinishTaskSpendingWorldContribution(unsigned short taskID)
{
	ATaskTempl* pTempl = GetTaskTemplMan()->GetTaskTemplByID(taskID);
	return pTempl && pTempl->m_iWorldContribution <= GetWorldContribution();
}

void CECTaskInterface::FinishTaskSpendingWorldContribution(unsigned short taskID, unsigned char choice)
{
	if (CanFinishTaskSpendingWorldContribution(taskID))
		_notify_svr_finishtask_by_contribution(this, taskID, choice);
}

int CECTaskInterface::GetWorldContributionSpend()
{
	return m_pHost->GetWorldContributionSpend();
}

bool CECTaskInterface::PlayerCanSpendContributionAsWill()
{
	return m_pHost->HaveHealthStones();
}
void CECTaskInterface::OnNewTask(int iTaskID)
{
	ATaskTempl* pTempl = GetTaskTemplMan()->GetTaskTemplByID(iTaskID);	
	if (pTempl && pTempl->m_enumMethod==enumTMSimpleClientTaskForceNavi)
	{
		SetForceNavigateFinishFlag(false);
		m_pHost->OnNaviageEvent(iTaskID,CECNavigateCtrl::EM_PREPARE);
	}
}
void CECTaskInterface::OnGiveupTask(int iTaskID)
{
	ATaskTempl* pTempl = GetTaskTemplMan()->GetTaskTemplByID(iTaskID);
	if (pTempl && pTempl->m_enumMethod==enumTMSimpleClientTaskForceNavi)
	{
		m_pHost->OnNaviageEvent(iTaskID,CECNavigateCtrl::EM_END);
		SetForceNavigateFinishFlag(false);
	}
}
void CECTaskInterface::OnCompleteTask(int iTaskID)
{
	ATaskTempl* pTempl = GetTaskTemplMan()->GetTaskTemplByID(iTaskID);
	if (pTempl && pTempl->m_enumMethod==enumTMSimpleClientTaskForceNavi)
	{
		m_pHost->OnNaviageEvent(iTaskID,CECNavigateCtrl::EM_END);
		SetForceNavigateFinishFlag(false);
	}
}