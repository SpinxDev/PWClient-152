#include "TaskInterface.h"
#include "TaskTempl.h"
#include "TaskTemplMan.h"
#include "EC_Global.h"
#include "EC_UIManager.h"
#include "EC_GameUIMan.h"
#include "EC_Game.h"
#include "EC_GameRun.h"
#include "EC_FixedMsg.h"
#include "EC_HostPlayer.h"
#include "EC_AutoTeam.h"
#include "EC_UIHelper.h"

#ifdef _TASK_CLIENT

#define FINISH_DLG_SHOWN_TIME 60

void UpdateTaskToConfirm(TaskInterface* pTask, const ATaskTempl* pTempl, bool bReadyConfirm)
{
	if (pTask && pTempl)
	{
		// 直接完成
		if (pTempl->m_enumFinishType == enumTFTDirect)
		{
			// 达到完成条件
			if (bReadyConfirm)
			{
				pTempl->IncValidCount();
				_notify_svr(pTask, TASK_CLT_NOTIFY_CHECK_FINISH, (unsigned short)pTempl->GetID());
			}
		}
		else if (pTempl->m_enumFinishType == enumTFTConfirm)
			pTask->SetTaskReadyToConfirm(pTempl->m_ID, bReadyConfirm);
		else
			assert(!"need adding task code to deal with new finish type.");
	}
}

void OnTaskCheckStatus(TaskInterface* pTask)
{
	if (!pTask->CheckVersion() || !pTask->IsDeliverLegal())
		return;

	ActiveTaskList* pLst = static_cast<ActiveTaskList*>(pTask->GetActiveTaskList());
	ActiveTaskEntry* aEntries = pLst->m_TaskEntries;
	unsigned long ulCurTime = pTask->GetCurTime();
	unsigned char i;

	unsigned long iRegion;

	for (i = 0; i < pLst->m_uTaskCount; i++)
	{
		ActiveTaskEntry& CurEntry = aEntries[i];

		if (!CurEntry.m_ulTemplAddr)
		{
			//assert(false);
			continue;
		}

		const ATaskTempl* pTempl = CurEntry.GetTempl();

		if (!pTempl->IsValidState())
			continue;

		// PQ子任务
		if (pTempl->m_bPQSubTask)
		{
			if(pTempl->CheckGlobalPQKeyValue(true) == 0)
			{
 				pTempl->IncValidCount();
 				_notify_svr(pTask, TASK_CLT_NOTIFY_CHECK_FINISH, CurEntry.m_ID);
 				continue;
			}
		}

		// 超时判断
		if (pTempl->m_ulTimeLimit
		&& CurEntry.m_ulTaskTime + pTempl->m_ulTimeLimit < ulCurTime) // 超时
		{
			pTempl->IncValidCount();
			_notify_svr(pTask, TASK_CLT_NOTIFY_CHECK_FINISH, CurEntry.m_ID);
			continue;
		}

		// 超过任务失效日期
		if (pTempl->m_bAbsFail)
		{
			long ulCurTime1 = ulCurTime;
			ulCurTime1 -= unsigned long(TaskInterface::GetTimeZoneBias() * 60);

			if((long)ulCurTime1 < 0)
				ulCurTime1 = 0;
			tm cur = *gmtime((long*)&ulCurTime1);			
			
			if(pTempl->m_tmAbsFailTime.before(&cur))
			{
 				pTempl->IncValidCount();
 				_notify_svr(pTask, TASK_CLT_NOTIFY_CHECK_FINISH, CurEntry.m_ID);
 				continue;
			}
		}
		// 进入or离开区域任务失败	
		float pos[3];
		unsigned long ulWorldId = pTask->GetPos(pos);
		
		if (pTempl->m_bEnterRegionFail && ulWorldId == pTempl->m_ulEnterRegionWorld)
		{
			for (iRegion=0;iRegion<pTempl->m_ulEnterRegionCnt;iRegion++)
			{
				const Task_Region& t = pTempl->m_pEnterRegion[iRegion];
				if(is_in_zone(t.zvMin, t.zvMax, pos))
				{
					pTempl->IncValidCount();
					_notify_svr(pTask, TASK_CLT_NOTIFY_CHECK_FINISH, CurEntry.m_ID);
					break;
				}
			}			
		}
		
		if (pTempl->m_bLeaveRegionFail)
		{
			bool bLeaveRegion = false;

  			if ( ulWorldId != pTempl->m_ulLeaveRegionWorld) bLeaveRegion = true;
			else
			{
				for (iRegion=0;iRegion<pTempl->m_ulLeaveRegionCnt;iRegion++)
				{
					const Task_Region& t = pTempl->m_pLeaveRegion[iRegion];
					if(is_in_zone(t.zvMin, t.zvMax, pos))					
						break;					
				}
				if (iRegion>=pTempl->m_ulLeaveRegionCnt) bLeaveRegion = true;		
			}
			if(bLeaveRegion)	
			{
				pTempl->IncValidCount();
				_notify_svr(pTask, TASK_CLT_NOTIFY_CHECK_FINISH, CurEntry.m_ID);
			}
		}
		
		// 离开帮派失败
		if (!pTask->IsAtCrossServer() && pTempl->m_bLeaveFactionFail && !pTask->IsInFaction(1))
		{
			pTempl->IncValidCount();
			_notify_svr(pTask, TASK_CLT_NOTIFY_CHECK_FINISH, CurEntry.m_ID);
			continue;
		}

		if (pTempl->m_enumMethod == enumTMTalkToNPC || pTempl->m_bMarriage || pTempl->m_enumFinishType == enumTFTNPC)
			continue;

		// 判断未完成的任务可否完成
		if (!CurEntry.IsFinished())
		{
			if (pTempl->m_enumMethod == enumTMReachSite && pTempl->m_enumFinishType == enumTFTDirect) // 到达地点判断
			{
				if (ulCurTime - TaskInterface::GetFinishDlgShowTime() < FINISH_DLG_SHOWN_TIME)
					continue;

				float pos[3];
				unsigned long ulWorldId = pTask->GetPos(pos);

				if (ulWorldId == pTempl->m_ulReachSiteId)
				{
					for (iRegion=0;iRegion<pTempl->m_ulReachSiteCnt;iRegion++)
					{
						const Task_Region& t = pTempl->m_pReachSite[iRegion];

						if (is_in_zone(t.zvMin, t.zvMax, pos))
						{
							const talk_proc* pTalk = &pTempl->m_AwardTalk;
							
							if (pTalk->num_window == 0 || pTalk->num_window == 1 && pTalk->windows[0].num_option == 0)
							{
								pTempl->IncValidCount();
								_notify_svr(pTask, TASK_CLT_NOTIFY_REACH_SITE, (unsigned short)pTempl->GetID());
							}
							else
							{
								pTask->PopupTaskFinishDialog(pTempl->m_ID, const_cast<talk_proc*>(&pTempl->m_AwardTalk));
								TaskInterface::SetFinishDlgShowTime(ulCurTime);
							}

							break;
						}
					}				
				}
				continue;
			}

			if (pTempl->m_enumMethod == enumTMLeaveSite && pTempl->m_enumFinishType == enumTFTDirect) // 离开地点判断
			{
				if (ulCurTime - TaskInterface::GetFinishDlgShowTime() < FINISH_DLG_SHOWN_TIME)
					continue;

				float pos[3];
				unsigned long ulWorldId = pTask->GetPos(pos);

				bool regRet = false;

				if (ulWorldId == pTempl->m_ulLeaveSiteId)
				{
					for (iRegion=0;iRegion<pTempl->m_ulLeaveSiteCnt;iRegion++)
					{
						const Task_Region& t = pTempl->m_pLeaveSite[iRegion];

						if (is_in_zone(t.zvMin, t.zvMax, pos))
						{
							regRet = true;
							break;
						}
					}
				}
				if(!regRet)
				{
					const talk_proc* pTalk = &pTempl->m_AwardTalk;

					if (pTalk->num_window == 0 || pTalk->num_window == 1 && pTalk->windows[0].num_option == 0)
					{
						pTempl->IncValidCount();
						_notify_svr(pTask, TASK_CLT_NOTIFY_LEAVE_SITE, (unsigned short)pTempl->GetID());
					}
					else
					{
						pTask->PopupTaskFinishDialog(pTempl->m_ID, const_cast<talk_proc*>(&pTempl->m_AwardTalk));
						TaskInterface::SetFinishDlgShowTime(ulCurTime);
					}
				}

				continue;
			}
		}
		if (!CurEntry.HasChildren())
		{
			bool bNeedServerCheck = pTempl->RecursiveCheckAward(pTask, pLst, &CurEntry, ulCurTime, -1) == 0 && pTempl->CanFinishTask(pTask, &CurEntry, ulCurTime);
			if (pTempl->m_bDisplayInExclusiveUI && pTempl->m_bAutoDeliver && pTempl->m_enumFinishType == enumTFTDirect) {
				CECGameUIMan* pGameUI = g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan();
				unsigned long ulRemainTime = 0;
				if (pTempl->m_enumMethod == enumTMWaitTime) {
					unsigned long ultime = CurEntry.m_ulTaskTime + pTempl->m_ulWaitTime;
					if (ultime > ulCurTime)
						ulRemainTime = ultime - ulCurTime;
				}
				if (pGameUI && !pTempl->m_bReadyToNotifyServer) pGameUI->UpdateAutoDelTask(CurEntry.m_ID,ulRemainTime);
				if (pTempl->m_bReadyToNotifyServer && bNeedServerCheck)
				{
					pTempl->IncValidCount();
					pTempl->ResetAutoDelTask();
					_notify_svr(pTask, TASK_CLT_NOTIFY_CHECK_FINISH, CurEntry.m_ID);
				}
			} else UpdateTaskToConfirm(pTask,pTempl, bNeedServerCheck);
		}
	}

	GetTaskTemplMan()->UpdateStatus(pTask);
}

void OnServerNotify(TaskInterface* pTask, const void* pBuf, size_t sz)
{
	if (!pTask->CheckVersion())
		return;

	if (sz < sizeof(task_notify_base)) return;
	const task_notify_base* pNotify = static_cast<const task_notify_base*>(pBuf);

	const ATaskTempl* pTempl = NULL;
	ActiveTaskEntry* pEntry = NULL;

	if (pNotify->reason == TASK_SVR_NOTIFY_ERROR_CODE)
	{
		if (sz != sizeof(svr_task_err_code)) return;

#ifdef _ELEMENTCLIENT
		pEntry = static_cast<ActiveTaskList*>(pTask->GetActiveTaskList())->GetEntry(pNotify->task);
		if (pEntry) pEntry->SetErrReported();
		TaskShowErrMessage(static_cast<const svr_task_err_code*>(pNotify)->err_code);
#endif

		return;
	}
	else if (pNotify->reason == TASK_SVR_NOTIFY_FORGET_SKILL)
	{
		GetTaskTemplMan()->OnForgetLivingSkill(pTask);
		return;
	}
	else if (pNotify->reason == TASK_SVR_NOTIFY_NEW)
		pTempl = GetTaskTemplMan()->GetTopTaskByID(pNotify->task);
	else if (pNotify->reason == TASK_SVR_NOTIFY_DYN_TIME_MARK)
	{
		if (sz != sizeof(svr_task_dyn_time_mark)) return;
		GetTaskTemplMan()->OnDynTasksTimeMark(
			pTask,
			static_cast<const svr_task_dyn_time_mark*>(pNotify)->time_mark,
			static_cast<const svr_task_dyn_time_mark*>(pNotify)->version);
		return;
	}
	else if (pNotify->reason == TASK_SVR_NOTIFY_DYN_DATA)
	{
		if (sz <= sizeof(task_notify_base)) return;
		GetTaskTemplMan()->OnDynTasksData(
			pTask,
			static_cast<const char*>(pBuf) + sizeof(task_notify_base),
			sz - sizeof(task_notify_base),
			pNotify->task != 0);
		return;
	}
	// 库任务相关
	else if (pNotify->reason == TASK_SVR_NOTIFY_STORAGE)
	{
		if (sz != sizeof(task_notify_base) + sizeof(StorageTaskList)) return;
		GetTaskTemplMan()->OnStorageData(
			pTask,
			static_cast<const char*>(pBuf) + sizeof(task_notify_base)
			);

		TaskInterface::UpdateTaskUI(pNotify->task, pNotify->reason);		
		return;
	}
	else if (pNotify->reason == TASK_SVR_NOTIFY_SPECIAL_AWARD)
	{
		if (sz != sizeof(svr_task_special_award)) return;
		const special_award &sa = static_cast<const svr_task_special_award*>(pNotify)->sa;
		GetTaskTemplMan()->OnSpecialAward(&sa,pTask);
		if (sa.id1 == 0)
		{
			// ID 为0，说明没有领过新手卡，根据配置提示他领取
			CECGameUIMan* pGameUI = g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan();
			pGameUI->PopupNewbieGiftRemind();
		}
		return;
	}
	else if (pNotify->reason == TASK_SVR_NOTIFY_SET_TASK_LIMIT)
	{
		ActiveTaskList* pLst = static_cast<ActiveTaskList*>(pTask->GetActiveTaskList());
		pLst->ExpandMaxSimultaneousCount();
		TaskInterface::PopChatMessage(FIXMSG_TASK_LIMIT_INCREASED);
		return;
	}
	else
	{
		ActiveTaskList* pLst = static_cast<ActiveTaskList*>(pTask->GetActiveTaskList());
		ActiveTaskEntry* aEntries = pLst->m_TaskEntries;
		unsigned char i;

		for (i = 0; i < pLst->m_uTaskCount; i++)
		{
			ActiveTaskEntry& CurEntry = pLst->m_TaskEntries[i];

			if (CurEntry.m_ID != pNotify->task
			|| !CurEntry.m_ulTemplAddr)
				continue;

			pTempl = CurEntry.GetTempl();
			pEntry = &CurEntry;
			break;
		}
	}

	if (pNotify->reason == TASK_SVR_NOTIFY_PLAYER_KILLED)
		CECUIHelper::OnTaskProcessUpdated(pNotify->task);
	if (pNotify->reason == TASK_SVR_NOTIFY_MONSTER_KILLED)
	{
		// 杀死两只以上任务怪后，发起自动组队
		if (sz == sizeof(svr_monster_killed))
		{
			const svr_monster_killed* pKilled = static_cast<const svr_monster_killed*>(pNotify);
			if( pKilled->monster_num >= 2 )
			{
				CECAutoTeam* pAutoTeam = g_pGame->GetGameRun()->GetHostPlayer()->GetAutoTeam();
				pAutoTeam->DoAutoTeam(CECAutoTeam::TYPE_TASK, pNotify->task);
			}
		}
		CECUIHelper::OnTaskProcessUpdated(pNotify->task);
	}
	else if (pNotify->reason == TASK_SVR_NOTIFY_COMPLETE || pNotify->reason == TASK_SVR_NOTIFY_GIVE_UP)
	{
		CECUIHelper::OnTaskCompleted(pNotify->task);
	}

	if (!pTempl)
	{
		assert(false);
		return;
	}

	pTempl->ClearValidCount();
	pTempl->OnServerNotify(
		pTask,
		pEntry,
		pNotify,
		sz
		);
}

#endif
