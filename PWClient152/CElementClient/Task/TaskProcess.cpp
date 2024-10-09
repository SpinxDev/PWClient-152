#include "TaskTempl.h"
#include "TaskInterface.h"
#include "TaskTemplMan.h"
#include <algorithm>

#ifndef _TASK_CLIENT
#include "TaskServer.h"
#else
#ifndef TASK_TEMPL_EDITOR
#include "EC_Global.h"
#include "EC_Game.h"
#endif
#endif

#ifdef LINUX
#include "../template/elementdataman.h"
#else
#include "elementdataman.h"
#endif

#define TASK_ENTRY_DATA_CUR_VER		1
#define MAX_PLAYER_LEV				150

const unsigned long _lev_co[MAX_PLAYER_LEV] = {
	1,
	1,
	2,
	4,
	7,
	9,
	12,
	16,
	20,
	25,
	30,
	35,
	40,
	46,
	52,
	59,
	65,
	72,
	79,
	87,
	95,
	102,
	110,
	118,
	127,
	135,
	143,
	152,
	157,
	158,
	168,
	179,
	190,
	202,
	214,
	227,
	239,
	253,
	267,
	281,
	296,
	311,
	327,
	343,
	360,
	377,
	390,
	402,
	414,
	426,
	438,
	449,
	460,
	471,
	480,
	489,
	497,
	505,
	510,
	515,
	535,
	553,
	588,
	624,
	660,
	698,
	736,
	775,
	814,
	853,
	893,
	978,
	1066,
	1157,
	1250,
	1344,
	1438,
	1530,
	1620,
	1705,
	1784,
	1853,
	1910,
	1953,
	1976,
	1983,
	2116,
	2213,
	2304,
	2455,
	2679,
	2837,
	2990,
	3136,
	3272,
	3282,
	3388,
	3477,
	3543,
	4000,
	4000,
	4000,
	4000,
	4000,
	4000,
	4000,
	4000,
	4000,
	4000,
	4000,
	4000,
	4000,
	4000,
	4000,
	4000,
	4000,
	4000,
	4000,
	4000,
	4000,
	4000,
	4000,
	4000,
	4000,
	4000,
	4000,
	4000,
	4000,
	4000,
	4000,
	4000,
	4000,
	4000,
	4000,
	4000,
	4000,
	4000,
	4000,
	4000,
	4000,
	4000,
	4000,
	4000,
	4000,
	4000,
	4000,
	4000,
	4000,
	4000,
	4000,
};

// Process Part

unsigned long ATaskTempl::CheckPrerequisite(
	TaskInterface* pTask,
	ActiveTaskList* pList,
	unsigned long ulCurTime,
	bool bCheckPrevTask,
	bool bCheckTeam,
	bool bCheckBudge,
	bool bCheckLevel) const
{
	unsigned long ulRet = 0;

	// 必须是根节点
	if (m_pParent)
	{
		ulRet = TASK_PREREQU_FAIL_NOT_ROOT; 
		goto ret_here;
	}

	if (bCheckBudge)
	{
		ulRet = CheckBudget(pList);
		if (ulRet) goto ret_here;
	}
	else if (pList->GetEntry(m_ID))
	{
		ulRet = TASK_PREREQU_FAIL_SAME_TASK;
		goto ret_here;
	}

	// 是否被屏蔽
	if (pTask->CheckTaskForbid(m_ID))
	{
		ulRet = TASK_PREREQU_FAIL_TASK_FORBID;
		goto ret_here;
	}
	
	// 若为PQ任务，判断身上是否已经有PQ任务
	if (m_bPQTask)
	{
		for (unsigned char i = 0; i < pList->m_uTaskCount; i++)
		{
			ActiveTaskEntry& CurEntry = pList->m_TaskEntries[i];
			const ATaskTempl *pTempl = CurEntry.GetTempl();

			if(pTempl && pTempl->m_bPQTask)
			{
				ulRet = TASK_PREREQU_FAIL_ALREADY_HAS_PQ;
				goto ret_here;			
			}
		}
	}

	ulRet = CheckGivenItems(pTask);
	if (ulRet) goto ret_here;

	// 是否满足发放时间
	ulRet = CheckTimetable(ulCurTime);
	if (ulRet) goto ret_here;

	// 发放时间间隔
	ulRet = CheckDeliverTime(pTask, ulCurTime);
	if (ulRet) goto ret_here;

	ulRet = CheckFnshLst(pTask, ulCurTime);
	if (ulRet) goto ret_here;

	// 是否达到账号限制完成次数
	ulRet = CheckDeliverCount(pTask);
	if (ulRet) goto ret_here;
	
	// 是否达到充值金额
	ulRet = CheckAccountRMB(pTask);
	if (ulRet) goto ret_here;
	
	// 是否满足角色创建登录时间限制
	ulRet = CheckCharTime(pTask);
	if (ulRet) goto ret_here;

	// Check Level
	if(bCheckLevel)
	{
		ulRet = CheckLevel(pTask);
		if (ulRet) goto ret_here;
	}

	// 转生次数
	ulRet = CheckReincarnation(pTask);
	if (ulRet) goto ret_here;

	// 境界等级
	ulRet = CheckRealmLevel(pTask);
	if (ulRet) goto ret_here;

	// 境界经验是否已满
	ulRet = CheckRealmExpFull(pTask);
	if (ulRet) goto ret_here;

	// 声望
	ulRet = CheckRepu(pTask);
	if (ulRet) goto ret_here;

	// 押金
	ulRet = CheckDeposit(pTask);
	if (ulRet) goto ret_here;

	// 所需物品
	ulRet = CheckItems(pTask);
	if (ulRet) goto ret_here;

	// 帮派
	ulRet = CheckFaction(pTask);
	if (ulRet) goto ret_here;

	// 性别
	ulRet = CheckGender(pTask);
	if (ulRet) goto ret_here;

	// 职业
	ulRet = CheckOccupation(pTask);
	if (ulRet) goto ret_here;

	// 到达特定时期
	ulRet = CheckPeriod(pTask);
	if (ulRet) goto ret_here;

	// 是否GM
	ulRet = CheckGM(pTask);
	if (ulRet) goto ret_here;
	
	// 是否完美神盾用户
	ulRet = CheckShieldUser(pTask);
	if (ulRet) goto ret_here;

	// 任务条件
	if (bCheckPrevTask)
	{
		ulRet = CheckPreTask(pTask);
		if (ulRet) goto ret_here;
	}

	// 互斥任务
	ulRet = CheckMutexTask(pTask, ulCurTime);
	if (ulRet) goto ret_here;

	// 区域触发
	ulRet = CheckInZone(pTask);
	if (ulRet) goto ret_here;

	// 组队任务
	if (bCheckTeam)
	{
		ulRet = CheckTeamTask(pTask);
		if (ulRet) goto ret_here;
	}

	// 夫妻
	ulRet = CheckSpouse(pTask);
	if (ulRet) goto ret_here;

	ulRet = CheckWeddingOwner(pTask);
	if (ulRet) goto ret_here;

	// 婚姻
	ulRet = CheckMarriage(pTask);
	if (ulRet) goto ret_here;

	// 生活技能熟练度
	ulRet = CheckLivingSkill(pTask);
	if (ulRet) goto ret_here;

	// 特殊活动奖励
	ulRet = CheckSpecialAward(pTask);
	if (ulRet) goto ret_here;

	// 全局key/value
	ulRet = CheckGlobalKeyValue(pTask);
	if (ulRet) goto ret_here;

	if (m_bCompareItemAndInventory)
	{
		ulRet = CheckIvtrEmptySlot(pTask);
		if(ulRet) goto ret_here;
	}
	// 帮派贡献度
	ulRet = CheckFactionContrib(pTask);
	if (ulRet) goto ret_here;

	// 记录完成结果任务完成个数
	ulRet = CheckRecordTasksNum(pTask);
	if (ulRet) goto ret_here;

	// 变身状态
	ulRet = CheckTransform(pTask);
	if (ulRet) goto ret_here;

	// 势力
	ulRet = CheckForce(pTask);
	if (ulRet) goto ret_here;

	// 势力威望
	ulRet = CheckForceReputation(pTask);
	if (ulRet) goto ret_here;

	// 势力战功
	ulRet = CheckForceContribution(pTask);
	if (ulRet) goto ret_here;

	// 经验值
	ulRet = CheckExp(pTask);
	if (ulRet) goto ret_here;
	
	// 元神值
	ulRet = CheckSP(pTask);
	if (ulRet) goto ret_here;

	// 势力活跃等级
	ulRet = CheckForceActivityLevel(pTask);
	if (ulRet) goto ret_here;

	// 国王
	ulRet = CheckKing(pTask);
	if (ulRet) goto ret_here;

	// 不能组队
	ulRet = CheckNotInTeam(pTask);
	if (ulRet) goto ret_here;
	// 称号
	ulRet = CheckTitle(pTask);
	if (ulRet) goto ret_here;

	// 历史推进阶段
	ulRet = CheckHistoryStage(pTask);
	if(ulRet) goto ret_here;

	// 收集卡牌
	ulRet = CheckCardCollection(pTask);
	if(ulRet) goto ret_here;

	// 卡牌某品阶拥有数量
	ulRet = CheckCardRankCount(pTask);
	if (ulRet) goto ret_here;
	
	// 变身后，不能接取强制移动任务
	ulRet = CheckInTransformShape(pTask);
	if (ulRet) goto ret_here;

ret_here:

	char log[1024];
	sprintf(log, "CheckPrerequisite: ret = %d", ulRet);
	TaskInterface::WriteLog(pTask->GetPlayerId(), m_ID, 1, log);

	return ulRet;
}

ActiveTaskEntry* ATaskTempl::DeliverTask(
	TaskInterface* pTask,
	ActiveTaskList* pList,
	ActiveTaskEntry* pEntry,
	unsigned long ulCaptainTask,
	unsigned long& ulMask,
	unsigned long ulCurTime,
	const ATaskTempl* pSubTempl,
	task_sub_tags* pSubTag,
	TaskGlobalData* pGlobal,
	unsigned char uParentIndex) const
{
	ActiveTaskEntry* aEntries = pList->m_TaskEntries;
	if (!pEntry) pEntry = aEntries + pList->m_uTaskCount;
	else if (pEntry->m_ID != 0) // entry被占用，需要向后挪出一个空位;
		pList->RealignTask(pEntry, 1);

	unsigned char uIndex = static_cast<unsigned char>(pEntry - aEntries);

	pEntry->m_ID				= static_cast<unsigned short>(m_ID);
	pEntry->m_ulTemplAddr		= reinterpret_cast<unsigned long>(this);
	pEntry->m_ParentIndex		= uParentIndex;
	pEntry->m_PrevSblIndex		= 0xff;
	pEntry->m_NextSblIndex		= 0xff;
	pEntry->m_ChildIndex		= 0xff;
	pEntry->m_uState			= 0;
	pEntry->m_ulTaskTime		= ulCurTime;
#ifndef _TASK_CLIENT
	// 若为PQ任务，需要将时间戳记录在m_ulTaskTime里
	if (m_bPQTask)
	{
		pEntry->m_ulTaskTime = PublicQuestInterface::GetCurTaskStamp(m_ID);
	}
#endif
	if (ulCaptainTask)
	{
		pEntry->m_ulCapTemplAddr = reinterpret_cast<unsigned long>(GetTaskTemplMan()->GetTopTaskByID(ulCaptainTask));
		if (pEntry->m_ulCapTemplAddr) pEntry->m_uCapTaskId = static_cast<unsigned short>(ulCaptainTask);
		else
		{
			pEntry->m_uCapTaskId = 0;

			char log[1024];
			sprintf(log, "DeliverTask, Cant Find CapTask = %d", ulCaptainTask);
			TaskInterface::WriteLog(pTask->GetPlayerId(), m_ID, 0, log);
		}
	}
	else
	{
		pEntry->m_uCapTaskId = 0;
		pEntry->m_ulCapTemplAddr = 0;
	}

	pEntry->SetSuccess();		// Later will check whether truly succeed
	memset(pEntry->m_BufData, 0, sizeof(pEntry->m_BufData));
#ifndef _TASK_CLIENT

	// 若为PQ任务或PQ子任务，需要将时间戳记录在m_ulTaskTime里
	if (m_bPQTask)
	{
		ulCurTime = PublicQuestInterface::GetCurTaskStamp(m_ID);
	}

	else if (m_enumMethod == enumTMReachTreasureZone)
	{
		// 随机藏宝位置和藏宝图左下角位置的index
		unsigned short uZonesSum = m_ucZonesNumX * m_ucZonesNumZ;
		unsigned short uTreasureLocIndex = pTask->RandNormal(1,uZonesSum);
		char sTreasureLocMinX = (uTreasureLocIndex % m_ucZonesNumX - 1);
		char sTreasureLocMinZ = (uTreasureLocIndex / m_ucZonesNumX);
		// 限制随机出来的藏宝图位置，使藏宝位置不能处在藏宝图的边缘
		int sMapMinX = pTask->RandNormal(sTreasureLocMinX,sTreasureLocMinX + TASK_TREASURE_MAP_SIDE_MULTIPLE - 3) - TASK_TREASURE_MAP_SIDE_MULTIPLE + 2;
		int sMapMinZ = pTask->RandNormal(sTreasureLocMinZ,sTreasureLocMinZ + TASK_TREASURE_MAP_SIDE_MULTIPLE - 3) - TASK_TREASURE_MAP_SIDE_MULTIPLE + 2;
		// 存储在已接任务列表中
		pEntry->m_iUsefulData1 = uTreasureLocIndex;
		pEntry->m_iUsefulData1 |= (sMapMinX << 16) & 0x00FF0000;
		pEntry->m_iUsefulData1 |= (sMapMinZ << 24) & 0xFF000000;
	}

	ulMask |= m_ulMask;

#else
	if (m_enumMethod == enumTMReachTreasureZone)
	{
		task_notify_base notify;
		notify.reason = TASK_CLT_NOTIFY_REQUEST_TREASURE_INDEX;
		notify.task = static_cast<unsigned short>(m_ID);
		pTask->NotifyServer(&notify, sizeof(notify));
	}

#endif


	pList->m_uTaskCount++;

	if (!m_pParent)
	{
		if (m_bHidden) pList->m_uTopHideTaskCount++;
		else if (m_bDisplayInTitleTaskUI) pList->m_uTitleTaskCount++;
		else pList->m_uTopShowTaskCount++;
		pList->m_uUsedCount += m_uDepth;

#ifndef _TASK_CLIENT

		if (pGlobal)
		{
			pGlobal->AddRevNum();
			pGlobal->m_ulRcvUpdateTime = ulCurTime;
			TaskUpdateGlobalData(m_ID, pGlobal->buf);
		}

#endif

	}

	if (uParentIndex != 0xff)
	{
		ActiveTaskEntry& ParentEntry = aEntries[uParentIndex];
		if (ParentEntry.m_ChildIndex == 0xff) ParentEntry.m_ChildIndex = uIndex;
		else
		{
			unsigned char uChildEntry = ParentEntry.m_ChildIndex;
			while (aEntries[uChildEntry].m_NextSblIndex != 0xff)
				uChildEntry = aEntries[uChildEntry].m_NextSblIndex;
			aEntries[uChildEntry].m_NextSblIndex = uIndex;
			pEntry->m_PrevSblIndex = uChildEntry;
		}
	}

#ifndef _TASK_CLIENT

	if (!m_pParent)
		DeliverGivenItems(pTask);

#endif

	pEntry++;

	if (pSubTempl)
	{
		return pSubTempl->DeliverTask(
			pTask,
			pList,
			pEntry,
			0,
			ulMask,
			ulCurTime,
			NULL,
			pSubTag,
			NULL,
			uIndex);
	}
	else if (m_bRandOne)
	{
#ifdef _TASK_CLIENT
		if (pSubTag->cur_index < pSubTag->sz)
		{
			pSubTempl = GetSubByIndex(pSubTag->tags[pSubTag->cur_index]);
			pSubTag->cur_index++;

			if (pSubTempl)
			{
				return pSubTempl->DeliverTask(
					pTask,
					pList,
					pEntry,
					0,
					ulMask,
					ulCurTime,
					NULL,
					pSubTag,
					NULL,
					uIndex);
			}
		}
#else
		int nSel;
		pSubTempl = RandOneChild(pTask, nSel);

		if (pSubTempl)
		{
			if (pSubTag->sz < MAX_SUB_TAGS)
				pSubTag->tags[pSubTag->sz++] = static_cast<unsigned char>(nSel);

			return pSubTempl->DeliverTask(
				pTask,
				pList,
				pEntry,
				0,
				ulMask,
				ulCurTime,
				NULL,
				pSubTag,
				NULL,
				uIndex);
		}
#endif
	}
	else
	{
		const ATaskTempl* pChild = m_pFirstChild;

		while (pChild)
		{
			pEntry = pChild->DeliverTask(
				pTask,
				pList,
				pEntry,
				0,
				ulMask,
				ulCurTime,
				NULL,
				pSubTag,
				NULL,
				uIndex);

			if (m_bExeChildInOrder) return pEntry;
			pChild = pChild->m_pNextSibling;
		}
	}

	return pEntry;
}

unsigned long ATaskTempl::RecursiveCalcAward(
	TaskInterface* pTask,
	ActiveTaskList* pList,
	ActiveTaskEntry* pEntry,
	unsigned long ulCurTime,
	int nChoice,
	unsigned long& ulCmnCount,
	unsigned long& ulTskCount,
	unsigned long& ulTopCount,
	unsigned char& uBudget,
	long& lReputation) const
{
	unsigned long ulRet;

	AWARD_DATA ad;
	CalcAwardData(pTask, &ad, pEntry, pEntry->m_ulTaskTime,	ulCurTime);
	
	if (pTask->IsAtCrossServer() && ((ulRet = CheckAwardWhenAtCrossServer(pTask, &ad)) != 0))
		return ulRet;
	if (m_ulPremise_Lev_Min && !pEntry->IsGiveUp()) {
		if (!m_bPremCheckMaxHistoryLevel && pTask->GetPlayerLevel() < m_ulPremise_Lev_Min)
			return TASK_AWARD_FAIL_LEVEL_CHECK;
	}
	lReputation += ad.m_lReputation;

	if (ad.m_ulCandItems)
	{
		if (nChoice < 0 || nChoice >= static_cast<int>(ad.m_ulCandItems))
			nChoice = 0;

		CalcAwardItemsCount(pTask, &ad.m_CandItems[nChoice], ulCmnCount, ulTskCount);
	}

	if (ad.m_ulNewTask)
	{
		ATaskTempl* pNewTask = GetTaskTemplMan()->GetTopTaskByID(ad.m_ulNewTask);

		if (pNewTask)
		{
			if ((ulRet = pNewTask->CheckPrerequisite(pTask, pList, ulCurTime, false)) != 0)
				return ulRet;

			ulCmnCount += pNewTask->m_ulGivenCmnCount;
			ulTskCount += pNewTask->m_ulGivenTskCount;
			
			//高16位存放可见主任务个数，低16位存放隐藏任务个数和称号任务个数
			unsigned long ulTopShowCount = (ulTopCount & 0xffff0000) >> 16;
			unsigned long ulTopHideCount = ulTopCount & 0x000000ff;
			unsigned long ulTitleTaskCount = (ulTopCount & 0x0000ff00) >> 8;
			
			if (pNewTask->m_bHidden) ulTopHideCount++;
			else if(pNewTask->m_bDisplayInTitleTaskUI) ulTitleTaskCount++;
			else ulTopShowCount++;
			
			ulTopShowCount = ulTopShowCount << 16;
			ulTitleTaskCount = ulTitleTaskCount << 8;
			ulTopCount = 0;
			ulTopCount = ulTopCount | ulTopShowCount | ulTitleTaskCount | ulTopHideCount;

			uBudget += pNewTask->m_uDepth;
		}
	}

	if (pEntry->m_ParentIndex != 0xff)
	{
		ActiveTaskEntry ParentEntry = pList->m_TaskEntries[pEntry->m_ParentIndex];

		if (!pEntry->IsSuccess() && m_bParentAlsoFail)
		{
			ParentEntry.ClearSuccess();
			ParentEntry.SetFinished();

			if ((ulRet = m_pParent->RecursiveCalcAward(pTask, pList, &ParentEntry, ulCurTime, -1, ulCmnCount, ulTskCount, ulTopCount, uBudget, lReputation)) != 0)
				return ulRet;
		}
		else if (pEntry->IsSuccess() && m_bParentAlsoSucc)
		{
			ParentEntry.SetFinished();

			if ((m_pParent->m_enumFinishType == enumTFTDirect || m_pParent->m_enumFinishType == enumTFTConfirm)
			&& (ulRet = m_pParent->RecursiveCalcAward(pTask, pList, &ParentEntry, ulCurTime, -1, ulCmnCount, ulTskCount, ulTopCount, uBudget, lReputation)) != 0)
				return ulRet;
		}
		else if (m_pParent->m_bExeChildInOrder && m_pNextSibling)
			return 0;
		else if (pEntry->m_PrevSblIndex == 0xff && pEntry->m_NextSblIndex == 0xff)
		{
			ParentEntry.SetFinished();

			if ((m_pParent->m_enumFinishType == enumTFTDirect || m_pParent->m_enumFinishType == enumTFTConfirm)
			&& (ulRet = m_pParent->RecursiveCalcAward(pTask, pList, &ParentEntry, ulCurTime, -1, ulCmnCount, ulTskCount, ulTopCount, uBudget, lReputation)) != 0)
				return ulRet;
		}
	}

	return 0;
}


unsigned long ATaskTempl::CheckAwardWhenAtCrossServer(TaskInterface* pTask, const AWARD_DATA* pAward) const
{
	unsigned long ret(0);
	while(true)
	{
		if (!pAward || !pTask) break;
		if (!pTask->IsAtCrossServer()) 
		{
			ret = 0;
			break;
		}
		// 在跨服上禁止开启账号仓库
		if (pAward->m_ulStorehouseSize4) 
		{	
			ret = TASK_AWARD_FAIL_CROSSSERVER_NO_ACOUNT_STORAGE;
			break;
		}
		// 在跨服上禁止离婚
		if (pAward->m_bDivorce)
		{
			ret = TASK_AWARD_FAIL_CROSSSERVER_NO_DIVORCE;
			break;
		}
		// 在跨服上禁止完成帮派相关的任务
		if (pAward->m_iFactionContrib || pAward->m_iFactionExpContrib)
		{
			ret = TASK_AWARD_FAIL_CROSSSERVER_NO_FACTION_RALATED;
			break;
		}
		// 在跨服上禁止完成势力相关的任务
		if (pAward->m_iForceActivity || pAward->m_iForceContribution 
			|| pAward->m_iForceReputation || pAward->m_iForceSetRepu)
		{
			ret = TASK_AWARD_FAIL_CROSSSERVER_NO_FORCE_RALATED;
			break;
		}
		// 在跨服上禁止完成奖励鸿利值的任务
		if (pAward->m_ulDividend)
		{
			ret = TASK_AWARD_FAIL_CROSSSERVER_NO_DIVIEND;
			break;
		}
		break;
	}
	return ret;
}

unsigned long ATaskTempl::RecursiveCheckAward(
	TaskInterface* pTask,
	ActiveTaskList* pList,
	ActiveTaskEntry* pEntry,
	unsigned long ulCurTime,
	int nChoice) const
{
	unsigned long ulCmnCount = 0;
	unsigned long ulTskCount = 0;
	unsigned long ulTopCount = 0;
	unsigned char uBudget = 0;
	long lReputation = 0;

	// 任务被屏蔽的话不能完成此任务
	if (pTask->CheckTaskForbid(m_ID)) return TASK_PREREQU_FAIL_TASK_FORBID;
	
	// 跨服状态下不能完成账号限次任务
	if (m_bAccountTaskLimit && pTask->IsAtCrossServer())
		return TASK_AWARD_FAIL_CROSSSERVER_NO_ACOUNT_LIMIT;

	unsigned long ulRet = RecursiveCalcAward(pTask, pList, pEntry, ulCurTime, nChoice, ulCmnCount, ulTskCount, ulTopCount, uBudget, lReputation);
	if (ulRet) return ulRet;

	unsigned long ulTopShowCount = (ulTopCount & 0xffff0000) >> 16;
	unsigned long ulTopHideCount = ulTopCount & 0x000000ff;
	unsigned long ulTitleTaskCount = (ulTopCount & 0x0000ff00) >> 8;
	
	if (ulTopShowCount && pList->m_uTopShowTaskCount + ulTopShowCount > (unsigned long)pList->GetMaxSimultaneousCount()
	 || ulTopHideCount && pList->m_uTopHideTaskCount + ulTopHideCount > TASK_HIDDEN_COUNT
	 || ulTitleTaskCount && pList->m_uTitleTaskCount + ulTitleTaskCount > TASK_TITLE_TASK_COUNT)
		return TASK_PREREQU_FAIL_FULL;

	if (uBudget && pList->m_uUsedCount + uBudget > TASK_ACTIVE_LIST_MAX_LEN)
		return TASK_PREREQU_FAIL_NO_SPACE;

	if (ulCmnCount && !pTask->CanDeliverCommonItem(ulCmnCount)
	 || ulTskCount && !pTask->CanDeliverTaskItem(ulTskCount))
		return TASK_AWARD_FAIL_GIVEN_ITEM;

	if (lReputation && pTask->GetReputation() + lReputation < 0)
		return TASK_AWARD_FAIL_REPUTATION;

	return 0;
}

void ATaskTempl::RecursiveCheckTimeLimit(
	TaskInterface* pTask,
	ActiveTaskList* pList,
	ActiveTaskEntry* pEntry,
	unsigned long ulCurTime) const
{
	if (m_ulTimeLimit && pEntry->m_ulTaskTime + m_ulTimeLimit < ulCurTime) // 超时
		pEntry->ClearSuccess();

//	if (m_ulAbsFailTime && m_ulAbsFailTime < ulCurTime)	// 超过任务失效日期
	// 超过任务失效日期
	if (m_bAbsFail)
	{
		tm cur = *localtime((long*)&ulCurTime);

		if(m_tmAbsFailTime.before(&cur))
		{
			pEntry->ClearSuccess();
		}
	}

	if (m_pParent && pEntry->m_ParentIndex != 0xff)
	{
		ActiveTaskEntry& ParentEntry = pList->m_TaskEntries[pEntry->m_ParentIndex];
		m_pParent->RecursiveCheckTimeLimit(pTask, pList, &ParentEntry, ulCurTime);
	}
}

bool ATaskTempl::RecursiveCheckParent(TaskInterface* pTask, ActiveTaskList* pList, ActiveTaskEntry* pEntry) const
{
	if (m_pParent && pEntry->m_ParentIndex != 0xff)
	{
		ActiveTaskEntry& ParentEntry = pList->m_TaskEntries[pEntry->m_ParentIndex];

		if (!ParentEntry.IsSuccess())
			return false;

		return m_pParent->RecursiveCheckParent(pTask, pList, &ParentEntry);
	}

	return true;
}

void ATaskTempl::RecursiveAward(
	TaskInterface* pTask,
	ActiveTaskList* pList,
	ActiveTaskEntry* pEntry,
	unsigned long ulCurtime,
	int nChoice,
	task_sub_tags* pSubTag) const
{
	{
		char log[1024];
		sprintf(log, "RecursiveAward: state = 0x%x", pEntry->m_uState);
		TaskInterface::WriteLog(pTask->GetPlayerId(), m_ID, 1, log);
	}

	ActiveTaskEntry* aEntries = pList->m_TaskEntries;

	// 任务失败不收取物品
	bool bFailedTaskDoNotTakeItem = !pEntry->IsSuccess() && m_bNotClearItemWhenFailed && m_bClearAcquired;
	// 清空子任务
	pList->ClearChildrenOf(pTask, pEntry, !bFailedTaskDoNotTakeItem);
	if (!pEntry->m_ulTemplAddr) return; // must check it

	if (!m_pParent && m_bNeedRecord)
	{
		static_cast<FinishedTaskList*>(pTask->GetFinishedTaskList())->AddOneTask(
			m_ID,
			pEntry->IsSuccess());
	}

	// 账号限次任务增加完成次数
	if (!m_pParent && m_bAccountTaskLimit)
	{
		// 完成任务的时候判断是否清空已完成次数
		CheckDeliverTime(pTask, ulCurtime);
		// 没有勾选“失败的时候不记录完成次数”，或者勾选“失败的时候不记录完成次数”并且任务成功的时候
		if (!m_bNotIncCntWhenFailed || (m_bNotIncCntWhenFailed && pEntry->IsSuccess()))
			static_cast<TaskFinishCountList*>(pTask->GetFinishedCntList())->AddOrUpdate(m_ID,ulCurtime);
	}
	// 角色限次任务增加完成次数
	else if (!m_pParent && m_bRoleTaskLimit)
	{
		// 完成任务的时候判断是否清空已完成次数
		CheckDeliverTime(pTask, ulCurtime);
		// 没有勾选“失败的时候不记录完成次数”，或者勾选“失败的时候不记录完成次数”并且任务成功的时候
		if (!m_bNotIncCntWhenFailed || (m_bNotIncCntWhenFailed && pEntry->IsSuccess())) {
			// FinishedTaskList里保存任务完成次数
			static_cast<FinishedTaskList*>(pTask->GetFinishedTaskList())->AddForFinishCount(m_ID,pEntry->IsSuccess());
			// TaskFinishTimeList里保存任务完成时间
			static_cast<TaskFinishTimeList*>(pTask->GetFinishedTimeList())->AddOrUpdate(m_ID, ulCurtime);
		}
	}

#ifndef _TASK_CLIENT

	// 发放物品
	AWARD_DATA ad;
	CalcAwardData(
		pTask,
		&ad,
		pEntry,
		pEntry->m_ulTaskTime,
		ulCurtime);

	unsigned long ulRet = DeliverByAwardData(pTask, pList, pEntry, &ad, ulCurtime, nChoice);
	if (ulRet)
	{
		char log[1024];
		sprintf(log, "RecursiveAward, ret = %d", ulRet);
		TaskInterface::WriteLog(pTask->GetPlayerId(), m_ID, 0, log);
	}

	// 去掉获得的或预先给的物品，在发放物品后执行	
	if (m_bClearAcquired)
	{
		if (!bFailedTaskDoNotTakeItem)
			RemoveAcquiredItem(pTask, false, pEntry->IsSuccess());
	}
	else if (!pEntry->IsSuccess())
		TakeAwayGivenItems(pTask);

#endif

	pEntry->m_ulTemplAddr = 0;
	pEntry->m_ID = 0;

	if (pList->m_uTaskCount)
		pList->m_uTaskCount--;
	else
		TaskInterface::WriteLog(pTask->GetPlayerId(), m_ID, 0, "Award, TaskCount == 0");

	if (pEntry->m_ParentIndex != 0xff)
	{
		ActiveTaskEntry& ParentEntry = aEntries[pEntry->m_ParentIndex];

		if (pEntry->m_PrevSblIndex != 0xff)
			aEntries[pEntry->m_PrevSblIndex].m_NextSblIndex = pEntry->m_NextSblIndex;
		else
			ParentEntry.m_ChildIndex = pEntry->m_NextSblIndex;

		if (pEntry->m_NextSblIndex != 0xff)
			aEntries[pEntry->m_NextSblIndex].m_PrevSblIndex = pEntry->m_PrevSblIndex;

		if (!pEntry->IsSuccess() && m_bParentAlsoFail)
		{
			pList->RealignTask(pEntry, 0);
			ParentEntry.ClearSuccess();
			ParentEntry.SetFinished();
			m_pParent->RecursiveAward(pTask, pList, &ParentEntry, ulCurtime, -1, pSubTag);
		}
		else if (pEntry->IsSuccess() && m_bParentAlsoSucc)
		{
			pList->RealignTask(pEntry, 0);
			ParentEntry.SetFinished();
			pList->ClearChildrenOf(pTask, &ParentEntry);
			if (m_pParent->m_enumFinishType == enumTFTDirect)
				m_pParent->RecursiveAward(pTask, pList, &ParentEntry, ulCurtime, -1, pSubTag);
#ifdef _TASK_CLIENT
			else if (!m_pParent->m_bHidden && !m_pParent->m_bDisplayInTitleTaskUI)
				TaskInterface::TraceTask(m_pParent->m_ID);
#endif
		}
		else if (m_pParent->m_bExeChildInOrder && m_pNextSibling)
		{
			if (ParentEntry.m_ChildIndex != 0xff || pList->GetEntry(m_pNextSibling->m_ID)) // 错误情况，正常情况为0xff
				pList->RealignTask(pEntry, 0);
			else
			{
				m_pNextSibling->DeliverTask(
					pTask,
					pList,
					pEntry,
					0,
					*pTask->GetTaskMask(),
					ulCurtime,
					NULL,
					pSubTag,
					NULL,
					pEntry->m_ParentIndex);
#ifdef _TASK_CLIENT
				// 子任务有四种情况：选择一个，随机一个，并列子任务和顺序执行
				// 前三种情况在追踪父任务时已经处理
				// 对于第二个及以后的顺序执行的子任务：
				if (!m_pParent->m_bHidden && !m_pParent->m_bDisplayInTitleTaskUI)
					TaskInterface::TraceTask(m_pNextSibling->m_ID);
#endif
			}
		}
		else if (ParentEntry.m_ChildIndex == 0xff) // 当复合任务无子任务时，即为完成
		{
			pList->RealignTask(pEntry, 0);
			ParentEntry.SetFinished();
			if (m_pParent->m_enumFinishType == enumTFTDirect)
				m_pParent->RecursiveAward(pTask, pList, &ParentEntry, ulCurtime, -1, pSubTag);
#ifdef _TASK_CLIENT
			else if (!m_pParent->m_bHidden && !m_pParent->m_bDisplayInTitleTaskUI)
				TaskInterface::TraceTask(m_pParent->m_ID);
#endif
		}
		else
			pList->RealignTask(pEntry, 0);
	}
	else // Root Node
	{
		pList->RealignTask(pEntry, 0);

		if (pList->m_uUsedCount >= m_uDepth)
			pList->m_uUsedCount -= m_uDepth;
		else
		{
			char log[1024];
			sprintf(log, "Award, Used = %d", pList->m_uUsedCount);
			TaskInterface::WriteLog(pTask->GetPlayerId(), m_ID, 0, log);

			pList->m_uUsedCount = 0;
		}

		if (m_bHidden)
		{
			if (pList->m_uTopHideTaskCount)
				pList->m_uTopHideTaskCount--;
		}
		else if(m_bDisplayInTitleTaskUI)
		{
			if (pList->m_uTitleTaskCount)
				pList->m_uTitleTaskCount--;
		}
		else
		{
			if (pList->m_uTopShowTaskCount)
				pList->m_uTopShowTaskCount--;
		}	
	}
}

void ActiveTaskList::UpdateUsedCount()
{
	m_uUsedCount = 0;
	for (unsigned char i = 0; i < m_uTaskCount; i++)
	{
		const ATaskTempl* pTempl = m_TaskEntries[i].GetTempl();
		if (!pTempl) continue;
		if (pTempl->m_pParent) continue;
		m_uUsedCount += pTempl->m_uDepth;
	}
}

unsigned long ATaskTempl::CheckMutexTask(TaskInterface* pTask, unsigned long ulCurTime) const
{
	ActiveTaskList* pLst = static_cast<ActiveTaskList*>(pTask->GetActiveTaskList());
	FinishedTaskList* pFinished = static_cast<FinishedTaskList*>(pTask->GetFinishedTaskList());
	unsigned long i;

	for (i = 0; i < m_ulMutexTaskCount; i++)
	{
		// 是否已接收了互斥任务
		if (pLst->GetEntry(m_ulMutexTasks[i]))
			return TASK_PREREQU_FAIL_MUTEX_TASK;

		const ATaskTempl* pMutex = GetTaskTemplMan()->GetTopTaskByID(m_ulMutexTasks[i]);

		if (!pMutex)
			return TASK_PREREQU_FAIL_MUTEX_TASK;

		// 互斥任务是否满足发放时间
		if (pMutex->CheckDeliverTime(pTask, ulCurTime) != 0)
			return TASK_PREREQU_FAIL_MUTEX_TASK;

		// 互斥任务是否能重复完成
		if (pMutex->CheckFnshLst(pTask, ulCurTime) != 0)
			return TASK_PREREQU_FAIL_MUTEX_TASK;
	}

	return 0;
}
static const unsigned long MAX_SECONDS = 0x80000000;

long _ConvertPeriod(long lPeriod, TaskInterface* pTask)
{
	long ret(lPeriod);
	if (static_cast<unsigned long>(lPeriod) > MAX_SECONDS)
	{
		tm ExpiredTime;
		memset(&ExpiredTime,0,sizeof(tm));
		//0x  FF  FF FF FF
		//    年  月 日 时
		//最高位用于区分是有效时间还是截止日期
		int year = ((lPeriod & ~MAX_SECONDS) >> 24) + 2000;
		int mon = (lPeriod & 0x00FF0000) >> 16;
		int day = (lPeriod & 0x0000FF00) >> 8;
		int hour = lPeriod & 0x000000FF;
		ExpiredTime.tm_year = year - 1900;
		ExpiredTime.tm_mon = mon - 1;
		ExpiredTime.tm_mday = day;
		ExpiredTime.tm_hour = hour;
		ret = mktime(&ExpiredTime);
		ret -= pTask->GetCurTime();
		if (ret <= 0) ret = -1;
	}
	return ret;
}
#ifdef _TASK_CLIENT

#else

inline void _DeliverItem(
	TaskInterface* pTask,
	unsigned long ulTask,
	const ITEM_WANTED& wi,
	unsigned long ulMulti)
{
	unsigned long ulCount = wi.m_ulItemNum * ulMulti;

	char log[1024];
	sprintf(log, "DeliverItem: Item id = %d, Count = %d", wi.m_ulItemTemplId, ulCount);
	TaskInterface::WriteLog(pTask->GetPlayerId(), ulTask, 1, log);
	TaskInterface::WriteKeyLog(pTask->GetPlayerId(), ulTask, 1, log);

	long lPeriod = _ConvertPeriod(wi.m_lPeriod, pTask);
	if (lPeriod < 0) return;

	if (wi.m_bCommonItem) pTask->DeliverCommonItem(wi.m_ulItemTemplId, ulCount, ulCount > 1 ? 0 : lPeriod);
	else pTask->DeliverTaskItem(wi.m_ulItemTemplId, ulCount);
}

inline void _DeliverMonsters(
	TaskInterface* pTask,
	unsigned long ulTask,
	const MONSTERS_SUMMONED& ms,
	unsigned long ulSummonRadius, 
	bool bDieDisappear)
{
	char log[1024];
	sprintf(log, "DeliverMonsters: Monster id = %d, Count = %d", ms.m_ulMonsterTemplId, ms.m_ulMonsterNum);
	TaskInterface::WriteLog(pTask->GetPlayerId(), ulTask, 1, log);
	TaskInterface::WriteKeyLog(pTask->GetPlayerId(), ulTask, 1, log);

	pTask->SummonMonster(ms.m_ulMonsterTemplId, ms.m_ulMonsterNum, ulSummonRadius, ms.m_lPeriod, bDieDisappear);
}

unsigned long ATaskTempl::DeliverByAwardData(
	TaskInterface* pTask,
	ActiveTaskList* pList,
	ActiveTaskEntry* pEntry,
	const AWARD_DATA* pAward,
	unsigned long ulCurTime,
	int nChoice) const
{
	unsigned long i, ulRet = 0;
	ATaskTempl* pNewTask;

	// 只能在子任务的奖励中设置最低贡献度
	if(m_pParent && pAward->m_ulLowestcontrib)
	{
		int iContrib = PublicQuestInterface::GetCurContrib(m_pParent->m_ID, pTask->GetPlayerId());
		if(iContrib < pAward->m_ulLowestcontrib)
			return 1;
	}

	unsigned long ulMulti = CalcAwardMulti(
		pTask,
		pEntry,
		pEntry->m_ulTaskTime,
		ulCurTime
		);

	if (!ulMulti) return 1;
	ulMulti *= CalMultiByGlobalKeyValue(pTask, pAward);

	unsigned long ulGold	= pAward->m_ulGoldNum * ulMulti;
	unsigned long ulExp		= pAward->m_ulExp * ulMulti;
	unsigned long ulSP		= pAward->m_ulSP * ulMulti;
	long lRepu				= pAward->m_lReputation * ulMulti;

	if (pAward->m_bUseLevCo)
	{
		unsigned long ulLev = pTask->GetPlayerLevel();

		if (ulLev > MAX_PLAYER_LEV)
			ulLev = MAX_PLAYER_LEV;
		else if (ulLev == 0)
			ulLev = 1;

		unsigned long ulUpper = GetTopTask()->m_ulPremise_Lev_Max;

		if (ulUpper && ulLev > ulUpper)
			ulLev = ulUpper;

		unsigned long co = _lev_co[ulLev-1];
		ulExp *= co;
		ulSP *= co;
	}

	if (pAward->m_iFactionContrib || pAward->m_iFactionExpContrib) pTask->DeliverFactionContrib(pAward->m_iFactionContrib,pAward->m_iFactionExpContrib);
	if (pAward->m_ulGoldNum)	pTask->DeliverGold(ulGold);
	if (pAward->m_ulExp)		pTask->DeliverExperience(ulExp);
	if (pAward->m_ulSP)			pTask->DeliverSP(ulSP);
	if (pAward->m_lReputation)	pTask->DeliverReputation(lRepu);
	if (pAward->m_ulNewPeriod)	pTask->SetCurPeriod(pAward->m_ulNewPeriod);
	if (pAward->m_ulNewRelayStation) pTask->SetNewRelayStation(pAward->m_ulNewRelayStation);
	if (pAward->m_ulStorehouseSize) pTask->SetStorehouseSize(pAward->m_ulStorehouseSize);
	if (pAward->m_ulStorehouseSize2) pTask->SetStorehouseSize2(pAward->m_ulStorehouseSize2);
	if (pAward->m_ulStorehouseSize3) pTask->SetStorehouseSize3(pAward->m_ulStorehouseSize3);
	if (pAward->m_ulStorehouseSize4) pTask->SetAccountStorehouseSize(pAward->m_ulStorehouseSize4);
	if (pAward->m_ulDividend)	pTask->AddDividend(pAward->m_ulDividend);
	if (pAward->m_lInventorySize) pTask->SetInventorySize(pAward->m_lInventorySize);
	if (pAward->m_ulPetInventorySize) pTask->SetPetInventorySize(pAward->m_ulPetInventorySize);
	if (pAward->m_ulFuryULimit) pTask->SetFuryUpperLimit(pAward->m_ulFuryULimit);
	if (pAward->m_iForceActivity) pTask->AddForceActivity(pAward->m_iForceActivity);
	if (pAward->m_iForceContribution) pTask->ChangeForceContribution(pAward->m_iForceContribution);
	if (pAward->m_iForceReputation) pTask->ChangeForceReputation(pAward->m_iForceReputation);
	if (pAward->m_iForceSetRepu) pTask->SetForceReputation(pAward->m_iForceSetRepu);
	if (pAward->m_iTaskLimit) pTask->ExpandTaskLimit();
	if (pAward->m_ulTitleNum) {
		for(unsigned long i = 0; i < pAward->m_ulTitleNum; ++i) {
			if (pAward->m_pTitleAward[i].m_ulTitleID) {
				long lPeriod = _ConvertPeriod(pAward->m_pTitleAward[i].m_lPeriod, pTask);
				if (lPeriod >= 0)
					pTask->DeliverTitle(pAward->m_pTitleAward[i].m_ulTitleID, lPeriod);
			}
		}
	}
	if (pAward->m_ulRealmExp) pTask->DeliverRealmExp(pAward->m_ulRealmExp);
	if (pAward->m_bExpandRealmLevelMax) pTask->ExpandRealmLevelMax();
	if (pAward->m_iLeaderShip) pTask->AddLeaderShip(pAward->m_iLeaderShip);
	if (pAward->m_iWorldContribution && CanDeliverWorldContribution(pTask)) pTask->DeliverWorldContribution(pAward->m_iWorldContribution);

	if (pAward->m_bDivorce)
		pTask->Divorce();

	if (pAward->m_ulTransWldId)
		pTask->TransportTo(pAward->m_ulTransWldId, pAward->m_TransPt.v, pAward->m_lMonsCtrl);
	else if (pAward->m_lMonsCtrl && !m_bPQTask && !m_bPQSubTask)
		pTask->SetMonsterController(pAward->m_lMonsCtrl, pAward->m_bTrigCtrl);

	// version 77
	if(pAward->m_bAwardSkill && (pAward->m_iAwardSkillID >0) && (pAward->m_iAwardSkillLevel >0))
		pTask->CastSkill(pAward->m_iAwardSkillID,pAward->m_iAwardSkillLevel);

	char log[1024];
	sprintf(log,
		"DeliverByAwardData: success = %d, gold = %d, exp = %d, sp = %d, reputation = %d",
		pEntry->IsSuccess() ? 1 : 0,
		ulGold,
		ulExp,
		ulSP,
		lRepu
		);
	
	TaskInterface::WriteLog(pTask->GetPlayerId(), m_ID, 1, log);
	TaskInterface::WriteKeyLog(pTask->GetPlayerId(), m_ID, 1, log);
	
	if (pAward->m_ulCandItems)
	{
		if (nChoice < 0 || nChoice >= static_cast<int>(pAward->m_ulCandItems))
			nChoice = 0;

		const AWARD_ITEMS_CAND& ic = pAward->m_CandItems[nChoice];

		if (CanAwardItems(pTask, &ic))
		{
			if (ic.m_bRandChoose)
			{
				float fProb = pTask->UnitRand();
				bool bGiven = false;

				for (i = 0; i < ic.m_ulAwardItems; i++)
				{
					const ITEM_WANTED& wi = ic.m_AwardItems[i];

					if (wi.m_fProb > 0.9999f ) _DeliverItem(pTask, m_ID, wi, ulMulti);
					else if (!bGiven)
					{
						if (fProb <= wi.m_fProb)
						{
							_DeliverItem(pTask, m_ID, wi, ulMulti);
							bGiven = true;
						}
						else fProb -= wi.m_fProb;
					}
				}
			}
			else
			{
				for (i = 0; i < ic.m_ulAwardItems; i++)
				{
					const ITEM_WANTED& wi = ic.m_AwardItems[i];
					if (pTask->UnitRand() <= wi.m_fProb) _DeliverItem(pTask, m_ID, wi, ulMulti);
				}
			}
		}
		else
			ulRet = 2;
	}

//---------------------------------------------------------------------------------Monsters
	if (pAward->m_ulSummonedMonsters)
	{
		const AWARD_MONSTERS_SUMMONED* ms = pAward->m_SummonedMonsters;
		
		if(ms->m_bRandChoose)
		{
			float fTotalProb = 0.0f;
			for(i=0; i < ms->m_ulMonsterNum; i++)
			{
				fTotalProb += ms->m_Monsters[i].m_fSummonProb;
			}

			if((fTotalProb > 0.999999f) && (fTotalProb < 1.000001f ))	// fTotalProb == 1.0f
			{
				float fProb = pTask->UnitRand();
				bool bGiven = false;

				for(i=0; i < ms->m_ulMonsterNum; i++)
				{
					const MONSTERS_SUMMONED& monster = ms->m_Monsters[i];
					if(!bGiven)
					{
						if(fProb <= monster.m_fSummonProb)
						{
							_DeliverMonsters(pTask, m_ID, monster, ms->m_ulSummonRadius, ms->m_bDeathDisappear);
							bGiven = true;
						}
						else fProb -= monster.m_fSummonProb;
					}
				}
			}
			else
			{
				for(i=0; i < ms->m_ulMonsterNum; i++)
				{
					float fProb = pTask->UnitRand();

					const MONSTERS_SUMMONED& monster = ms->m_Monsters[i];
					if(fProb <= monster.m_fSummonProb )
						_DeliverMonsters(pTask, m_ID, monster, ms->m_ulSummonRadius, ms->m_bDeathDisappear);
				}
			}
		}
		else
		{
			for(i=0; i < ms->m_ulMonsterNum; i++)
			{
				const MONSTERS_SUMMONED& monster = ms->m_Monsters[i];
				_DeliverMonsters(pTask, m_ID, monster, ms->m_ulSummonRadius, ms->m_bDeathDisappear);
			}
		}
	}

//---------------------------------------------------------------------------------PQ Award
	if (pAward->m_ulSpecifyContribTaskID && pAward->m_ulSpecifyContrib)
	{
		if((pAward->m_ulSpecifyContribSubTaskID ==0) || (pAward->m_ulSpecifyContribSubTaskID == PublicQuestInterface::GetCurSubTask(pAward->m_ulSpecifyContribTaskID)))
			PublicQuestInterface::QuestUpdatePlayerContrib(pAward->m_ulSpecifyContribTaskID, pTask->GetPlayerId(), pAward->m_ulSpecifyContrib);
	}

// 	// PQ子任务奖励贡献度(改为由服务器heartbeat中添加)
// 	if (pAward->m_ulContrib || pAward->m_ulRandContrib)
// 	{
// 		float fProb = pTask->UnitRand();
// 
// 		if(m_pParent)
// 			PublicQuestInterface::QuestUpdatePlayerContrib(m_pParent->m_ID, pTask->GetPlayerId(), (unsigned long)(pAward->m_ulRandContrib * fProb) + pAward->m_ulContrib);
//	}

	// PQ子任务排名奖励
	if(pAward->m_ulPQRankingAwardCnt)
	{
		if(m_pParent)
		{
			int iContrib = PublicQuestInterface::GetCurContrib(m_pParent->m_ID, pTask->GetPlayerId());
			if(iContrib >= pAward->m_ulLowestcontrib)
			{
				int iCurRank;
				if(pAward->m_PQRankingAward->m_bAwardByProf)
					iCurRank = PublicQuestInterface::GetCurClsPlace(m_pParent->m_ID, pTask->GetPlayerId());
				else 
					iCurRank = PublicQuestInterface::GetCurAllPlace(m_pParent->m_ID, pTask->GetPlayerId());
				
				for(int i=0;i<pAward->m_PQRankingAward->m_ulRankingAwardNum;i++)
				{
					const RANKING_AWARD& ra= pAward->m_PQRankingAward->m_RankingAward[i];
					
					if(iCurRank >= ra.m_iRankingStart && iCurRank <= ra.m_iRankingEnd)
					{
						ITEM_WANTED wi;
						wi.m_ulItemTemplId	= ra.m_ulAwardItemId;
						wi.m_ulItemNum		= ra.m_ulAwardItemNum;
						wi.m_bCommonItem	= ra.m_bCommonItem;
						wi.m_lPeriod		= ra.m_lPeriod;

						_DeliverItem(pTask, m_ID, wi, ulMulti);
						break;
					}
				}				
			}
		}
	}

	if (pAward->m_ulNewTask)
	{
		pNewTask = GetTaskTemplMan()->GetTopTaskByID(pAward->m_ulNewTask);

		if (pNewTask)
		{
			pNewTask->CheckDeliverTask(
				pTask,
				0,
				NULL
				);
		}
		else
		{
			TaskInterface::WriteLog(
				pTask->GetPlayerId(),
				pAward->m_ulNewTask,
				0,
				"DeliverNewTask, Cant Find Task");
		}
	}

	if (pAward->m_bSendMsg)
		pTask->SendMessage(m_ID, pAward->m_nMsgChannel, 0);

	if (pAward->m_bAwardDeath)
		pTask->SetAwardDeath(pAward->m_bAwardDeathWithLoss);
	
	//改变全局Value
	for (i=0; i<pAward->m_ulChangeKeyCnt; i++)
	{
		long lKey = pAward->m_plChangeKey[i];
		
		if (lKey == 0)
			continue;
		
		bool bChangeType = pAward->m_pbChangeType[i];
		long lValue = pAward->m_plChangeKeyValue[i];
		
		if (bChangeType)
		{
			pTask->ModifyGlobalValue(lKey, lValue);
		}
		else
			pTask->PutGlobalValue(lKey, lValue);
	}	
	//改变历史阶段全局Value
	for (i=0; i<pAward->m_ulHistoryChangeCnt; i++)
	{
		int lKey = pAward->m_plHistoryChangeKey[i];
		
		if (lKey == 0)
			continue;
		
		bool bChangeType = pAward->m_pbHistoryChangeType[i];
		int lValue = pAward->m_plHistoryChangeKeyValue[i];
		
		if (bChangeType)
		{
			pTask->ModifyHistoryGlobalValue(lKey, lValue);
		}
		else
			pTask->PutHistoryGlobalValue(lKey, lValue);
	}

	//显示全局Value
	if (pAward->m_ulTaskCharCnt)
	{
		// 通知客户端显示全局变量
		NotifyClient(pTask, pEntry, TASK_SVR_NOTIFY_DIS_GLOBAL_VAL, 0);
	}

	return ulRet;
}

bool ATaskTempl::CheckTeamMemPos(TaskInterface* pTask, ActiveTaskEntry* pEntry, float fSqrDist) const
{
	const int nMemNum = pTask->GetTeamMemberNum();
	unsigned long ulPlayer = pTask->GetPlayerId();
	float fDist, self_pos[3], pos[3], x, y, z;
	int i;

	unsigned long ulWorldId = pTask->GetPos(self_pos);

	for (i = 0; i < nMemNum; i++)
	{
		unsigned long ulOther = pTask->GetTeamMemberId(i);
		if (ulOther == ulPlayer) continue;

		if (pTask->GetTeamMemberPos(i, pos) != ulWorldId)
			return false;

		x = self_pos[0] - pos[0];
		y = self_pos[1] - pos[1];
		z = self_pos[2] - pos[2];
		
		fDist = x * x + y * y + z * z;

		if (fDist > fSqrDist) return false;
	}

	return true;
}

bool ATaskTempl::OnDeliverTeamMemTask(
	TaskInterface* pTask,
	TaskGlobalData* pGlobal) const
{
	TaskInterface::WriteLog(pTask->GetPlayerId(), m_ID, 1, "OnDeliverTeamMemTask");

	if (!m_bTeamwork || pTask->IsCaptain()) return false;
 
	ActiveTaskList* pList = static_cast<ActiveTaskList*>(pTask->GetActiveTaskList());
	unsigned long ulCurTime = pTask->GetCurTime();

	if (m_ulTeamMemsWanted)
	{
		unsigned long i;
		task_team_member_info MemInfo;

		MemInfo.m_ulLevel		= pTask->GetPlayerLevel();
		MemInfo.m_ulOccupation	= pTask->GetPlayerOccupation();
		MemInfo.m_bMale			= pTask->IsMale();
		MemInfo.m_iForce		= pTask->GetForce();

		for (i = 0; i < m_ulTeamMemsWanted; i++)
		{
			const TEAM_MEM_WANTED& tmw = m_TeamMemsWanted[i];

			if (!tmw.IsMeetBaseInfo(&MemInfo))
				continue;

			if (!tmw.m_ulTask) break; // 无队员任务则发放队长任务

			const ATaskTempl* pTempl = GetTaskTemplMan()->GetTopTaskByID(tmw.m_ulTask);

			if (pTempl && pTempl->CheckDeliverTask(
				pTask,
				0,
				this == pTempl ? pGlobal : NULL,
				true,
				true,
				m_ID) == 0)
				return true;
			else
				return false;
		}

		if (i == m_ulTeamMemsWanted) return false;
	}

	return CheckDeliverTask(pTask, 0, pGlobal, true, true, 0) == 0;
}

bool ATaskTempl::CheckGlobalRequired(
	TaskInterface* pTask,
	unsigned long ulSubTaskId,
	const TaskPreservedData* pPreserve,
	const TaskGlobalData* pGlobal,
	unsigned short reason) const
{
	TaskPreservedData tpd;

	if (pPreserve)
	{
		char log[1024];
		sprintf(log, "CheckGlobalRcv: reason = %d, RcvNum = %d", reason, pGlobal->m_ulReceiverNum);
		TaskInterface::WriteLog(pTask->GetPlayerId(), m_ID, 1, log);

		switch (pPreserve->reason)
		{
		case TASK_GLOBAL_CHECK_RCV_NUM:
			if (m_bTeamwork
			 && m_bRcvByTeam
			 && pTask->IsInTeam()
			 && pTask->GetTeamMemberNum() + pGlobal->m_ulReceiverNum > m_ulMaxReceiver
			 || pGlobal->m_ulReceiverNum >= m_ulMaxReceiver)
			{
				TaskInterface::WriteLog(pTask->GetPlayerId(), m_ID, 1, "CheckGlobal: Reach Max Num");
				return false;
			}

			if (m_ulPremise_Cotask)
			{
				switch (m_ulCoTaskCond)
				{
				case COTASK_CORRESPOND:
					return pGlobal->m_ulReceiverNum < pPreserve->m_ulRcvNum;
				case COTASK_ONCE:
					return pPreserve->m_ulRcvNum > 0;
				}
			}

			break;
		case TASK_GLOBAL_CHECK_ADD_MEM:
			return pGlobal->m_ulReceiverNum < m_ulMaxReceiver;
		case TASK_GLOBAL_CHECK_COTASK:
			tpd.reason			= TASK_GLOBAL_CHECK_RCV_NUM;
			tpd.m_ulSrcTask		= m_ID;
			tpd.m_ulSubTaskId	= pPreserve->m_ulSubTaskId;
			tpd.m_ulRcvNum		= pGlobal->m_ulReceiverNum;

			TaskQueryGlobalData(
				m_ID,
				pTask->GetPlayerId(),
				&tpd,
				sizeof(tpd)
				);
			return false;
		}
	}
	else if (HasGlobalData())
	{
		unsigned long ulTask;
		tpd.m_ulSrcTask		= m_ID;
		tpd.m_ulSubTaskId	= ulSubTaskId;

		if (reason == TASK_GLOBAL_CHECK_RCV_NUM
		&& m_ulPremise_Cotask)
		{
			reason = TASK_GLOBAL_CHECK_COTASK;
			ulTask = m_ulPremise_Cotask;
		}
		else
			ulTask = m_ID;

		tpd.reason = reason;

		char log[1024];
		sprintf(log, "CheckGlobalSnd: QueryTask = %d, reason = %d", ulTask, reason);
		TaskInterface::WriteLog(pTask->GetPlayerId(), m_ID, 1, log);

		TaskQueryGlobalData(
			ulTask,
			pTask->GetPlayerId(),
			&tpd,
			sizeof(tpd)
			);

		return false;
	}

	return true;
}

unsigned long ATaskTempl::CheckDeliverTask(
	TaskInterface* pTask,
	unsigned long ulSubTaskId,
	TaskGlobalData* pGlobal,
	bool bNotifyErr,
	bool bMemTask,
	unsigned long ulCapId) const
{
	const ATaskTempl* pSubTempl = NULL;

	if (m_bChooseOne)
	{
		pSubTempl = GetConstSubById(ulSubTaskId);
		if (!pSubTempl) return TASK_PREREQU_FAIL_WRONG_SUB;
	}

	ActiveTaskList* pList = static_cast<ActiveTaskList*>(pTask->GetActiveTaskList());
	unsigned long ulCurTime = pTask->GetCurTime();

	unsigned long ulRet = CheckPrerequisite(pTask, pList, ulCurTime, true, !bMemTask);

	if (ulRet)
	{
		if (bNotifyErr && !IsAutoDeliver()) NotifyClient(
			pTask,
			NULL,
			TASK_SVR_NOTIFY_ERROR_CODE,
			0,
			ulRet
		);

		return ulRet;
	}

	// 检查是否达到每日库任务领取上限
	StorageTaskList* pStorageLst = static_cast<StorageTaskList*> (pTask->GetStorageTaskList());
	ATaskTemplMan *pTaskMan = GetTaskTemplMan();
	unsigned int idStorage = pTaskMan->GetTaskStorageId(m_ID);
	if (pTaskMan && pTaskMan->IsStorageTask(m_ID) && pTaskMan->HasStorageTaskDeliverLimit(idStorage) && idStorage > 0 && idStorage <= TASK_STORAGE_COUNT)
	{
		unsigned char receiveCount = pStorageLst->m_StoragesReceivePerDay[idStorage - 1];
		if (pTaskMan->IsReceiveMaxStorageTasksPerDay(idStorage,receiveCount))
			return TASK_PREREQU_FAIL_OVER_RECEIVE_PER_DAY;
		else
			(pStorageLst->m_StoragesReceivePerDay[idStorage - 1])++;
	}


	// PQ task
	if (m_bPQTask)
	{
		int ulChildTaskId = PublicQuestInterface::GetCurSubTask(m_ID);
		pSubTempl = GetConstSubById(ulChildTaskId);

		if (!pSubTempl) 
			return TASK_PREREQU_FAIL_WRONG_SUB;
	}

	if(!m_bItemNotTakeOff)
		RemovePrerequisiteItem(pTask);

	task_sub_tags tags;
	memset(&tags, 0, sizeof(tags));
	if (pSubTempl) tags.sub_task = static_cast<unsigned short>(pSubTempl->m_ID);

	DeliverTask(pTask, pList, NULL, ulCapId, *pTask->GetTaskMask(), ulCurTime, pSubTempl, &tags, pGlobal);

	GetTaskTemplMan()->RemoveActiveStorageTask(pStorageLst, m_ID);

	if (m_lAvailFrequency != enumTAFNormal && !m_bAccountTaskLimit && !m_bRoleTaskLimit)
	{
		static_cast<TaskFinishTimeList*>(pTask->GetFinishedTimeList())->AddOrUpdate(
			m_ID,
			ulCurTime);
	}

	// 向客户端发出通知
	NotifyClient(
		pTask,
		reinterpret_cast<const ActiveTaskEntry*>(ulCapId),
		TASK_SVR_NOTIFY_NEW,
		ulCurTime,
		reinterpret_cast<unsigned long>(&tags));

	// version 81， 对包裹枷锁
	if (m_bCompareItemAndInventory)
	{
		pTask->LockInventory(true);
	}

	if (m_bPQTask)
	{
		PublicQuestInterface::QuestAddPlayer(m_ID, pTask->GetPlayerId());
	}

	if (!bMemTask && m_bTeamwork && m_bRcvByTeam)
		DeliverTeamMemTask(pTask, pGlobal, ulCurTime);

	if (m_bTransTo)
		pTask->TransportTo(m_ulTransWldId, m_TransPt.v, m_lMonsCtrl);
	else if (m_lMonsCtrl && !m_bPQTask && !m_bPQSubTask)
		pTask->SetMonsterController(m_lMonsCtrl, m_bTrigCtrl);

	// version 77
	if (m_bDeliverySkill && (m_iDeliveredSkillID >0) && (m_iDeliveredSkillLevel >0))
	{
		pTask->CastSkill(m_iDeliveredSkillID,m_iDeliveredSkillLevel);
	}

	//改变全局Value
	for (int i=0; i<m_ulChangeKeyCnt; i++)
	{
		long lKey = m_plChangeKey[i];
		
		if (lKey == 0)
			continue;
		
		bool bChangeType = m_pbChangeType[i];
		long lValue = m_plChangeKeyValue[i];
		
		if (bChangeType)
		{
			pTask->ModifyGlobalValue(lKey, lValue);
		}
		else
			pTask->PutGlobalValue(lKey, lValue);
	}

	TaskInterface::WriteKeyLog(pTask->GetPlayerId(), m_ID, 1, "CheckDeliverTask");
	return 0;
}

bool ATaskTempl::DeliverAward(
	TaskInterface* pTask,
	ActiveTaskList* pList,
	ActiveTaskEntry* pEntry,
	int nChoice,
	bool bNotifyTeamMem,
	TaskGlobalData* pGlobal) const
{
	char log[1024];
	sprintf(log, "DeliverAward: Choice = %d", nChoice);
	TaskInterface::WriteLog(pTask->GetPlayerId(), m_ID, 1, log);
	TaskInterface::WriteKeyLog(pTask->GetPlayerId(), m_ID, 1, log);

	// 检查时间
	unsigned long ulCurTime = pTask->GetCurTime();
	RecursiveCheckTimeLimit(pTask, pList, pEntry, ulCurTime);

	if (!RecursiveCheckParent(pTask, pList, pEntry))
		pEntry->ClearSuccess();

	// 处理组队失败情况
	if (!pEntry->IsAwardNotifyTeam() && bNotifyTeamMem && pEntry->GetCapOrSelf()->m_bTeamwork && !pEntry->IsSuccess())
	{
		AwardNotifyTeamMem(pTask, pEntry);
		pEntry->SetAwardNotifyTeam(); // Nofity only once
	}

	if (!pTask->IsDeliverLegal()) // 处于死亡，交易等特殊状态
		return false;

	if (pEntry->IsGiveUp() && m_bClearAsGiveUp)
	{
		pList->ClearTask(pTask, pEntry, true);
		pList->UpdateTaskMask(*pTask->GetTaskMask());
		NotifyClient(pTask, NULL, TASK_SVR_NOTIFY_GIVE_UP, 0);

		// 把包裹解锁
		if (m_bCompareItemAndInventory)
		{
			pTask->LockInventory(false);
		}

		return true;
	}

	pEntry->SetFinished();

#ifndef _TASK_CLIENT

	unsigned long ulRet;
	if ((ulRet = RecursiveCheckAward(
		pTask,
		pList,
		pEntry,
		ulCurTime,
		nChoice
		)) != 0)
	{
		if (m_enumFinishType == enumTFTNPC || !pEntry->IsErrReported())
		{
			NotifyClient(
				pTask,
				NULL,
				TASK_SVR_NOTIFY_ERROR_CODE,
				0,
				ulRet
				);

			pEntry->SetErrReported();
		}

		char log[1024];
		sprintf(log, "DeliverAward: ret = %d", ulRet);
		TaskInterface::WriteLog(pTask->GetPlayerId(), m_ID, 1, log);
		return false;
	}

#endif

	// 处理组队成功情况
	if (!pEntry->IsAwardNotifyTeam() && bNotifyTeamMem &&  pEntry->IsSuccess())
	{
		if(pEntry->GetCapOrSelf()->m_bTeamwork)
		{
			AwardNotifyTeamMem(pTask, pEntry);
			pEntry->SetAwardNotifyTeam(); // Nofity only once
		}
		else
		{
			const ATaskTempl* pParent = pEntry->GetTempl()->GetTopTask();
			ActiveTaskEntry* pParentEntry = static_cast<ActiveTaskList*>(pTask->GetActiveTaskList())->GetEntry(pParent->GetID());

			if(pParentEntry->GetCapOrSelf()->m_bTeamwork && pEntry->m_ChildIndex == 0xff)
			{
				bool bIsLastChild = true;
				const ATaskTempl* pTempParent = pEntry->GetTempl();
				while(pTempParent)// && pTempParent->m_pNextSibling == NULL)
				{					
					if(pTempParent->m_pNextSibling != NULL)
					{
						bIsLastChild = false;
						break;
					}

					pTempParent = pTempParent->m_pParent;
				}

				if(bIsLastChild)
				{
					AwardNotifyTeamMem(pTask, pParentEntry);
					pParentEntry->SetAwardNotifyTeam(); // Nofity only once
				}
			}
		}
	}

	task_sub_tags sub_tags;
	memset(&sub_tags, 0, sizeof(sub_tags));
	sub_tags.state = pEntry->m_uState;

	// 完成任务
	RecursiveAward(pTask, pList, pEntry, ulCurTime, nChoice, &sub_tags);

	// 通知客户端
	NotifyClient(
		pTask,
		NULL,
		TASK_SVR_NOTIFY_COMPLETE,
		ulCurTime,
		reinterpret_cast<unsigned long>(&sub_tags));

	// 检查Mask
	pList->UpdateTaskMask(*pTask->GetTaskMask());

	// 把包裹解锁
	if (m_bCompareItemAndInventory)
	{
		pTask->LockInventory(false);
	}

	return true;
}

inline void ATaskTempl::AwardNotifyTeamMem(TaskInterface* pTask, ActiveTaskEntry* pEntry) const
{
	const ATaskTempl* pCap = pEntry->GetCapOrSelf();
	bool bCheckPos = false;
	unsigned long ulWorldId;
	float pos[3];
	unsigned long ulParam;
	bool bIsCap = pTask->IsCaptain();

	if (pEntry->IsSuccess())
	{
		if (!pCap->m_bAllSucc && (!bIsCap || !m_bCapSucc)) return;
		ulParam = TASK_PLY_NOTIFY_FORCE_SUCC;

		if (m_fSuccDist > 0.0f)
		{
			bCheckPos = true;
			ulWorldId = pTask->GetPos(pos);
		}
	}
	else if (pCap->m_bAllFail || pCap->m_bCapFail && bIsCap)
		ulParam = TASK_PLY_NOTIFY_FORCE_FAIL;
	else
		return;

	const int nMemNum = pTask->GetTeamMemberNum();
	unsigned long ulId = pTask->GetPlayerId();
	int i;

	for (i = 0; i < nMemNum; i++)
	{
		unsigned long ulMemId = pTask->GetTeamMemberId(i);
		if (ulMemId == ulId) continue;

		if (bCheckPos)
		{
			float mem_pos[3];

			if (pTask->GetTeamMemberPos(i, mem_pos) != ulWorldId)
				continue;

			float x = mem_pos[0] - pos[0];
			float y = mem_pos[1] - pos[1];
			float z = mem_pos[2] - pos[2];

			if (x * x + y * y + z * z > m_fSuccDist)
				continue;
		}

		TaskNotifyPlayer(
			pTask,
			ulMemId,
			pCap->m_ID,
			(unsigned char)ulParam);
	}
}

void ActiveTaskList::UpdateTaskMask(unsigned long& ulMask) const
{
	ulMask = 0;
	for (unsigned char i = 0; i < m_uTaskCount; i++)
	{
		const ATaskTempl* pTempl = m_TaskEntries[i].GetTempl();
		if (!pTempl) continue;
		ulMask |= pTempl->m_ulMask;
	}
}

#endif

void TaskInterface::InitActiveTaskList()
{
	ActiveTaskList* pLst = static_cast<ActiveTaskList*>(GetActiveTaskList());
	FinishedTaskList* pFnsh = static_cast<FinishedTaskList*>(GetFinishedTaskList());
	TaskFinishTimeList* pFnshTime = static_cast<TaskFinishTimeList*>(GetFinishedTimeList());
	TaskFinishCountList* pFnshCount = static_cast<TaskFinishCountList*>(GetFinishedCntList());
	ActiveTaskEntry* pEntries = pLst->m_TaskEntries;
	ATaskTemplMan* pMan = GetTaskTemplMan();

	if (!CheckVersion() || !pLst->IsValid() || !pFnsh->IsValid() || !pFnshTime->IsValid())
	{
		pLst->RemoveAll();
		pFnsh->RemoveAll();
		pFnshTime->RemoveAll();		
		TaskInterface::WriteLog(0, 0, 0, "InitLst, list is invalid");
	}
	
	if(!pFnshCount->IsValid())
	{
		pFnshCount->RemoveAll();
		TaskInterface::WriteLog(0, 0, 0, "InitLst, finish count list is invalid");
	}
	if (pFnsh->m_FnshHeader.m_Version == 0){
		abase::vector<FnshedTaskEntryOld> list_old;
		list_old.reserve(pFnsh->m_FnshHeader.m_uTaskCount);
		FnshedTaskListOld* pListOld = (FnshedTaskListOld*)pFnsh;
		int i(0);
		for (i = 0; i < pFnsh->m_FnshHeader.m_uTaskCount; ++i)
			list_old.push_back(pListOld->m_aTaskList[i]);

		pFnsh->m_FnshHeader.m_Version = 1;
		for (i = 0; i < pFnsh->m_FnshHeader.m_uTaskCount; ++i)
		{
			pFnsh->m_aTaskList[i].m_uTaskId = list_old[i].m_uTaskId & 0x7fff;
			pFnsh->m_aTaskList[i].m_Mask = list_old[i].m_uTaskId >> 15;
		}
	}

#ifdef _TASK_CLIENT

	FILE* fp = fopen("logs\\Tasks.log", "wb");

	if (fp)
	{
		unsigned short magic = 0xfeff;
		fwrite(&magic, sizeof(magic), 1, fp);

		for (unsigned long n = 0; n < pFnsh->m_FnshHeader.m_uTaskCount; n++)
		{
			ATaskTempl* pTempl = GetTaskTemplMan()->GetTaskTemplByID(pFnsh->m_aTaskList[n].m_uTaskId);

			fwprintf(
				fp,
				L"task = %d, name = %s\r\n",
				pFnsh->m_aTaskList[n].m_uTaskId,
				pTempl ? pTempl->GetName() : L"");
		}

		for (unsigned short m = 0; m < pFnshTime->m_uCount; m++)
		{
			ATaskTempl* pTempl = GetTaskTemplMan()->GetTaskTemplByID(pFnshTime->m_aList[m].m_uTaskId);
			unsigned long his_time = pFnshTime->m_aList[m].m_ulTimeMark;
			his_time -= unsigned long(TaskInterface::GetTimeZoneBias() * 60);

			if ((long)(his_time) < 0)
				his_time = 0;

			tm t = *gmtime((time_t*)&his_time);
			wchar_t buf[256];
			swprintf(buf, L"%d-%02d-%02d-%02d-%02d-%02d", t.tm_year + 1900, t.tm_mon + 1, t.tm_mday, t.tm_hour, t.tm_min, t.tm_sec);

			fwprintf(
				fp,
				L"task = %d, deliver time = %s, name = %s\r\n",
				pFnshTime->m_aList[m].m_uTaskId,
				buf,
				pTempl ? pTempl->GetName() : L"");
		}

		fclose(fp);
	}

	if (!GetTaskTemplMan()->IsDynTasksVerified())
	{
		// 获取动态任务的时间标签
		_notify_svr(this, TASK_CLT_NOTIFY_DYN_TIMEMARK, 0);
	}
	else
	{
		// 清除特殊奖励信息
		GetTaskTemplMan()->ClearSpecailAward();
		_notify_svr(this, TASK_CLT_NOTIFY_SPECIAL_AWARD, 0);

		// 获取仓库数据
		_notify_svr(this, TASK_CLT_NOTIFY_STORAGE, 0);
	}

#else
	unsigned long ulCurTime = GetCurTime();
	const ATaskTempl* pTempl;
	pLst->m_Version = TASK_ENTRY_DATA_CUR_VER;
	bool bTimeMarkUpdated = pLst->IsTimeMarkUpdate();
	pLst->m_uTopShowTaskCount = 0;
	pLst->m_uTopHideTaskCount = 0;
	pLst->m_uTitleTaskCount = 0;
#endif

	unsigned char i = 0;

	for (; i < pLst->m_uTaskCount; i++)
	{
		ActiveTaskEntry& entry = pEntries[i];

		if (!entry.IsValid(i, pLst->m_uTaskCount))
		{
			pLst->RemoveAll();
			TaskInterface::WriteLog(0, 0, 0, "InitLst, active list is invalid");
			break;
		}
	}

	i = 0;

	while (i < pLst->m_uTaskCount)
	{
		ActiveTaskEntry& entry = pEntries[i];

		if (entry.m_NextSblIndex != 0xff) {
			ActiveTaskEntry& entryNextSbl = pEntries[entry.m_NextSblIndex];
			if (entryNextSbl.m_PrevSblIndex != i) entry.m_NextSblIndex = 0xff;
		}
		if (entry.m_ParentIndex == 0xff)
			entry.m_ulTemplAddr = reinterpret_cast<unsigned long>(pMan->GetTopTaskByID(entry.m_ID));
		else
		{
			const ATaskTempl* pParent = pLst->m_TaskEntries[entry.m_ParentIndex].GetTempl();

			if (pParent)
				entry.m_ulTemplAddr = reinterpret_cast<unsigned long>(pParent->GetConstSubById(entry.m_ID));
			else
				entry.m_ulTemplAddr = 0;
		}

#ifndef _TASK_CLIENT

		if (!entry.m_ulTemplAddr)
		{
			TaskInterface::WriteLog(0, entry.m_ID, 0, "InitLst, Cant Find Task");

			pLst->ClearTask(this, &entry, false);
			continue;
		}

		// 若无子任务，且没完成，则清除
		if (entry.m_ChildIndex == 0xff
		 && entry.GetTempl()->m_enumMethod == enumTMNone
		 && !entry.IsFinished())
		{
			TaskInterface::WriteLog(0, entry.m_ID, 0, "InitLst, Task is Impossible");
			pLst->ClearTask(this, &entry, false);
			continue;
		}



#endif

		if (entry.m_uCapTaskId)
		{
			entry.m_ulCapTemplAddr = reinterpret_cast<unsigned long>(GetTaskTemplMan()->GetTopTaskByID(entry.m_uCapTaskId));
			if (!entry.m_ulCapTemplAddr)
			{
				entry.m_uCapTaskId = 0;
				TaskInterface::WriteLog(0, entry.m_uCapTaskId, 0, "InitLst, Cant Find CapTask");
			}
		}
		else
			entry.m_ulCapTemplAddr = 0;


#ifndef _TASK_CLIENT

		if (!bTimeMarkUpdated)
		{
			pTempl = entry.GetTempl();

			if (!pTempl->m_bAbsTime && !pTempl->m_bPQTask && !pTempl->m_bPQSubTask)
				entry.m_ulTaskTime = ulCurTime - entry.m_ulTaskTime;
		}
#endif

#ifndef _TASK_CLIENT
		// 初始化活动任务链表中的隐藏、显示任务个数
		if (entry.m_ParentIndex == 0xff)
		{	
			if (entry.GetTempl()->m_bHidden)
				pLst->m_uTopHideTaskCount++;
			else if (entry.GetTempl()->m_bDisplayInTitleTaskUI)
				pLst->m_uTitleTaskCount++;
			else pLst->m_uTopShowTaskCount++;

		}
#endif

		i++;
	}

#ifndef _TASK_CLIENT
	pLst->SetTimeMarkUpdate();
	pLst->UpdateTaskMask(*GetTaskMask());
#endif

	pLst->UpdateUsedCount();
}

unsigned long TaskInterface::GetActLstDataSize()
{
	ActiveTaskList* pLst = static_cast<ActiveTaskList*>(GetActiveTaskList());
	return TASK_ACTIVE_LIST_HEADER_LEN + sizeof(ActiveTaskEntry) * pLst->m_uTaskCount;
}

unsigned long TaskInterface::GetFnshLstDataSize()
{
	FinishedTaskList* pLst = static_cast<FinishedTaskList*>(GetFinishedTaskList());
	return sizeof(FnshedTaskListHeader) + sizeof(FnshedTaskEntry) * pLst->m_FnshHeader.m_uTaskCount;
}

unsigned long TaskInterface::GetFnshTimeLstDataSize()
{
	TaskFinishTimeList* pLst = static_cast<TaskFinishTimeList*>(GetFinishedTimeList());
	return sizeof(short) + sizeof(TaskFinishTimeEntry) * pLst->m_uCount;
}

unsigned long TaskInterface::GetFnshCntLstDataSize()
{
	TaskFinishCountList* pLst = static_cast<TaskFinishCountList*>(GetFinishedCntList());
	return sizeof(short) + sizeof(TaskFinishCountEntry) * pLst->m_uCount;

}
unsigned long TaskInterface::GetStorageTaskLstDataSize()
{
	return sizeof(StorageTaskList);
}

bool TaskInterface::CheckVersion()
{
	return static_cast<ActiveTaskList*>(GetActiveTaskList())->m_Version == TASK_ENTRY_DATA_CUR_VER;
}

bool TaskInterface::CanDoMining(unsigned long ulTaskId)
{

#ifndef _TASK_CLIENT

	if (!IsDeliverLegal())
		return false;

	ActiveTaskEntry* pEntry = static_cast<ActiveTaskList*>(GetActiveTaskList())->GetEntry(ulTaskId);

	if (!pEntry || !pEntry->IsSuccess())
		return false;

	const ATaskTempl* pTempl = pEntry->GetTempl();

	if (!pTempl)
		return false;

	// 超时判断
	if (pTempl->m_ulTimeLimit)
	{
		unsigned long ulCurTime = TaskInterface::GetCurTime();

		if (pEntry->m_ulTaskTime + pTempl->m_ulTimeLimit < ulCurTime)
			return false;
	}
	
	// 超过任务失效日期判断
	if (pTempl->m_bAbsFail)
	{
		unsigned long ulCurTime = TaskInterface::GetCurTime();

		tm cur = *localtime((long*)&ulCurTime);
		if(pTempl->m_tmAbsFailTime.before(&cur))
		{
			return false;
		}
	}
// 	if (pTempl->m_ulAbsFailTime)
// 	{
// 		unsigned long ulCurTime = TaskInterface::GetCurTime();
// 
// 		if ( pTempl->m_ulAbsFailTime < ulCurTime)
// 			return false;
//	}

	if (pTempl->m_enumMethod == enumTMCollectNumArticle && pTempl->m_ulItemsWanted > 0)
	{
		const ITEM_WANTED& iw = pTempl->m_ItemsWanted[0];
		unsigned long ulNum = _get_item_count(this, iw.m_ulItemTemplId, iw.m_bCommonItem);

		if (iw.m_ulItemNum && ulNum >= iw.m_ulItemNum)
			return false;

		if (iw.m_bCommonItem && !CanDeliverCommonItem(1) || !iw.m_bCommonItem && !CanDeliverTaskItem(1))
		{
			pTempl->NotifyClient(this, NULL, TASK_SVR_NOTIFY_ERROR_CODE, 0, TASK_PREREQU_FAIL_GIVEN_ITEM);
			return false;
		}
	}

#endif

	return true;
}

static bool HasTopTaskRelating(ActiveTaskList *pLst, ATaskTempl::TaskRecursiveChecker *pChecker, abase::vector<unsigned long> *pTopTaskIDs)
{
	bool result(false);
	if (pTopTaskIDs){
		pTopTaskIDs->clear();
	}
	for (int i = 0; i < pLst->m_uTaskCount; i++){
		ActiveTaskEntry& CurEntry = pLst->m_TaskEntries[i];
		const ATaskTempl *pTempl = CurEntry.GetTempl();
		if (!pTempl){
			continue;
		}
		if (CurEntry.m_ParentIndex != 0xff){
			continue;
		}
		if (pTempl->RecursiveCheck(pChecker)){
			result = true;
			if (pTopTaskIDs){
				pTopTaskIDs->push_back(pTempl->GetID());
			}else{
				break;
			}
		}
	}
	return result;
}

bool TaskInterface::HasTopTaskRelatingMarriage(abase::vector<unsigned long> *pTopTaskIDs)
{
	class TaskMarriageChecker : public ATaskTempl::TaskRecursiveChecker{
	public:
		virtual bool Downward()const{ return true; }
		virtual bool IsMatch(const ATaskTempl *pTask)const{
			return pTask != NULL && pTask->m_bMarriage;
		}
	};
	ActiveTaskList* pLst = static_cast<ActiveTaskList*>(GetActiveTaskList());
	TaskMarriageChecker checker;
	return HasTopTaskRelating(pLst, &checker, pTopTaskIDs);
}

bool TaskInterface::HasTopTaskRelatingWedding(abase::vector<unsigned long> *pTopTaskIDs)
{
	class TaskWeddingChecker : public ATaskTempl::TaskRecursiveChecker{
	public:
		virtual bool Downward()const{ return true; }
		virtual bool IsMatch(const ATaskTempl *pTask)const{
			return pTask != NULL && pTask->m_bPremiseWeddingOwner;
		}
	};
	ActiveTaskList* pLst = static_cast<ActiveTaskList*>(GetActiveTaskList());
	TaskWeddingChecker checker;
	return HasTopTaskRelating(pLst, &checker, pTopTaskIDs);
}

bool TaskInterface::HasTopTaskRelatingSpouse(abase::vector<unsigned long> *pTopTaskIDs)
{
	class TaskSpouseChecker : public ATaskTempl::TaskRecursiveChecker{
	public:
		virtual bool Downward()const{ return true; }
		virtual bool IsMatch(const ATaskTempl *pTask)const{
			return pTask != NULL && pTask->m_bPremise_Spouse;
		}
	};
	ActiveTaskList* pLst = static_cast<ActiveTaskList*>(GetActiveTaskList());
	TaskSpouseChecker checker;
	return HasTopTaskRelating(pLst, &checker, pTopTaskIDs);
}

bool TaskInterface::HasTopTaskRelatingGender(abase::vector<unsigned long> *pTopTaskIDs)
{
	class TaskGenderChecker : public ATaskTempl::TaskRecursiveChecker{
	public:
		virtual bool Downward()const{ return true; }
		virtual bool IsMatch(const ATaskTempl *pTask)const{
			return pTask != NULL && pTask->m_ulGender != 0;
		}
	};
	ActiveTaskList* pLst = static_cast<ActiveTaskList*>(GetActiveTaskList());
	TaskGenderChecker checker;
	return HasTopTaskRelating(pLst, &checker, pTopTaskIDs);
}

//	class ATaskTempl
bool ATaskTempl::CanDeliverWorldContribution(TaskInterface* pTask) const
{
	// 只有顶层任务才能奖励贡献度
	if (GetTopTask() != this) 
		return false;
	bool ret(false);
	StorageTaskList* pStorage = static_cast<StorageTaskList*> (pTask->GetStorageTaskList());
	ATaskTemplMan* pMan = GetTaskTemplMan();
	ActiveTaskList* pLst = static_cast<ActiveTaskList*>(pTask->GetActiveTaskList());
	ActiveTaskEntry* pEntry = pLst->GetEntry(m_ID);
	if (pEntry == NULL) return false;
	else if (pEntry->IsContributionFinish()) return false;

	tm tmCur, tmTask;
	unsigned long ulCurTime = pTask->GetCurTime();
	unsigned long ulTaskTime = pEntry->m_ulTaskTime;
#ifdef _TASK_CLIENT
	ulCurTime -= unsigned long(TaskInterface::GetTimeZoneBias() * 60);
	ulTaskTime -= unsigned long(TaskInterface::GetTimeZoneBias() * 60);
	
	if ((long)(ulCurTime) < 0)
		ulCurTime = 0;
	
	if ((long)(ulTaskTime) < 0)
		ulTaskTime = 0;
	
	tmCur = *gmtime((time_t*)&ulCurTime);
	tmTask = *gmtime((time_t*)&ulTaskTime);
	
#else
	tmCur = *localtime((time_t*)&ulCurTime);
	tmTask = *localtime((time_t*)&ulTaskTime);
#endif
	
	// 如果任务领取时间跟当前时间不是同一天，则不能发放贡献度奖励
	if (!(tmCur.tm_year == tmTask.tm_year && tmCur.tm_yday == tmTask.tm_yday)) {
		return false;
	}

	if (pStorage && pMan) {
		elementdataman* pDataMan = pMan->GetEleDataMan();
		unsigned short idCheck = m_ID;
		for (int i = 1; i <= TASK_STORAGE_COUNT; ++i){
			// 检查是否是每日任务的库
			if (pMan->GetWeightTasksEssence(i)) {
				int taskSetCount = pStorage->m_StoragesTaskSetCount[i - 1];
				if (taskSetCount > TASK_STORAGE_LEN) 
					return false;
				
				for (int k = taskSetCount; k < TASK_STORAGE_LEN; ++k) {
					if (idCheck == pStorage->m_Storages[i - 1][k]) {
						ret = true;
						break;
					}
				}
				if (ret) break;				
	
				DATA_TYPE dt;
				for (int j = 0; j < taskSetCount; ++j) {
					const TASK_LIST_CONFIG* pConfig = (TASK_LIST_CONFIG*)pDataMan->get_data_ptr(pStorage->m_Storages[i - 1][j], ID_SPACE_CONFIG, dt);
					if (dt == DT_TASK_LIST_CONFIG && pConfig) {
						int sizeTasks = sizeof(pConfig->id_tasks) / sizeof(pConfig->id_tasks[0]);
						for (int k = 0; k < sizeTasks; ++k)	{
							int id = pConfig->id_tasks[k];
							if (id == 0) break;
							if (idCheck == id) {
								ret = true;
								break;
							}
						}
						if (ret) break;
					}
				}
			}
			if (ret) break;
		}
	}
	return ret;
}

bool ATaskTempl::RecursiveCheck(ATaskTempl::TaskRecursiveChecker *pChecker)const
{
	if (pChecker->IsMatch(this)){
		return true;
	}
	if (pChecker->Downward()){
		if (m_pFirstChild){
			ATaskTempl* pChild = m_pFirstChild;
			while (pChild){
				if (pChild->RecursiveCheck(pChecker)){
					return true;
				}
				pChild = pChild->m_pNextSibling;
			}
			return false;
		}else{
			return false;
		}
	}else{
		if (m_pParent){
			return m_pParent->RecursiveCheck(pChecker);
		}else{
			return false;
		}
	}
}

#ifdef _TASK_CLIENT

time_t TaskInterface::m_tmFinishDlgShown = 0;

#ifndef TASK_TEMPL_EDITOR
void RecursiveCheckPunchMonster(const ATaskTempl* pTempl)
{
	if (pTempl == NULL)
	{
		return;
	}
	for (unsigned int i = 0; i < pTempl->m_ulMonsterWanted; i++)
	{
		elementdataman *pDataMan = g_pGame->GetElementDataMan();
		
		DATA_TYPE dt;
		MONSTER_ESSENCE *pMonster = (MONSTER_ESSENCE *)pDataMan->get_data_ptr((pTempl->m_MonsterWanted[i]).m_ulMonsterTemplId, ID_SPACE_ESSENCE, dt);
		
		ACString strNpcName;
		if( dt == DT_MONSTER_ESSENCE )
		{
			if( pMonster && (pMonster->combined_switch & MCS_SUMMONER_ATTACK_ONLY) && (pMonster->combined_switch & MCS_RECORD_DPS_RANK))
			{
				TaskInterface::ShowPunchBagMessage(false,false,(pTempl->m_MonsterWanted[i]).m_ulMonsterTemplId,0,0);
				return;
			}	
		}

	}
	const ATaskTempl* child = pTempl->m_pFirstChild;
	while (child)
	{
		RecursiveCheckPunchMonster(child);
		child = child->m_pNextSibling;
	}
}
#endif
void ATaskTempl::OnServerNotify(
	TaskInterface* pTask,
	ActiveTaskEntry* pEntry,
	const task_notify_base* pNotify,
	size_t sz) const
{
	unsigned long ulTime, ulCaptainTask;
	ActiveTaskList* pLst;
	const ATaskTempl* pSub;
	task_sub_tags sub_tags;
	memset(&sub_tags, 0, sizeof(sub_tags));
	unsigned long i;
	const svr_monster_killed* pKilled;
	const svr_player_killed* pKilledPlayer;
	StorageTaskList* pStorage = static_cast<StorageTaskList*> (pTask->GetStorageTaskList());
	const svr_treasure_map* pTreasure;

	switch (pNotify->reason)
	{
	case TASK_SVR_NOTIFY_PLAYER_KILLED:
		{
			if (sz != sizeof(svr_player_killed)) break;
			if (m_enumMethod != enumTMKillPlayer) break;
		
			pKilledPlayer = static_cast<const svr_player_killed*>(pNotify);
			int iIndex = pKilledPlayer->index;
			if (iIndex < MAX_MONSTER_WANTED)
			{
				pEntry->m_wMonsterNum[iIndex] = pKilledPlayer->player_num;
			}
		}
		break;
	case TASK_SVR_NOTIFY_TREASURE_MAP:
		if (m_enumMethod == enumTMReachTreasureZone)
		{
			pTreasure = static_cast<const svr_treasure_map*>(pNotify);
			pEntry->m_iUsefulData1 = pTreasure->treasure_index;
		}
		break;
	case TASK_SVR_NOTIFY_MONSTER_KILLED:
		if (sz != sizeof(svr_monster_killed)) break;
		if (m_enumMethod != enumTMKillNumMonster) break;

		pKilled = static_cast<const svr_monster_killed*>(pNotify);

		for (i = 0; i < m_ulMonsterWanted; i++)
		{
			const MONSTER_WANTED& mw = m_MonsterWanted[i];

			if (mw.m_ulMonsterTemplId == pKilled->monster_id)
			{
				pEntry->m_wMonsterNum[i] = pKilled->monster_num;

				if (pKilled->dps && pKilled->dph)
				{
					TaskInterface::ShowPunchBagMessage(true,pKilled->dps >= mw.m_iDPS && pKilled->dph >= mw.m_iDPH,pKilled->monster_id,pKilled->dps,pKilled->dph);
				}
				break;
			}
		}

		break;
	case TASK_SVR_NOTIFY_NEW:
		if (!static_cast<const svr_new_task*>(pNotify)->valid_size(sz)) break;
		pLst = static_cast<ActiveTaskList*>(pTask->GetActiveTaskList());
		static_cast<const svr_new_task*>(pNotify)->get_data(
			ulTime,
			ulCaptainTask,
			sub_tags
			);

		GetTaskTemplMan()->RemoveActiveStorageTask(pStorage, m_ID);

		if (sub_tags.sub_task)
		{
			pSub = GetConstSubById(sub_tags.sub_task);
			if (!pSub) break;
		}
		else
			pSub = NULL;

		if (CheckBudget(pLst)) break;

		DeliverTask(
			pTask,
			pLst,
			NULL,
			ulCaptainTask,
			*pTask->GetTaskMask(),
			ulTime,
			pSub,
			&sub_tags,
			NULL);

		if (m_lAvailFrequency != enumTAFNormal && !m_bAccountTaskLimit && !m_bRoleTaskLimit)
		{
			static_cast<TaskFinishTimeList*>(pTask->GetFinishedTimeList())->AddOrUpdate(
				m_ID,
				ulTime);
		}

		if (CanShowPrompt() && !m_bDisplayInTitleTaskUI) TaskInterface::ShowTaskMessage(m_ID, TASK_MSG_NEW);
		// 父任务的追踪发生接收新任务的时候；
		if (!m_bHidden && !m_bDisplayInTitleTaskUI)
			TaskInterface::TraceTask(m_ID);
		if (m_bDisplayInTitleTaskUI)
			TaskInterface::UpdateTitleUI(m_ID);
		
		if ((m_enumMethod == enumTMSimpleClientTask) && m_uiEmotion)
			TaskInterface::PopEmotionUI(m_ID,m_uiEmotion,true);
		
		pTask->OnNewTask(m_ID);

		break;
	case TASK_SVR_NOTIFY_COMPLETE:
		if (!static_cast<const svr_task_complete*>(pNotify)->valid_size(sz)) break;
		static_cast<const svr_task_complete*>(pNotify)->get_data(
			ulTime,
			sub_tags
			);

		pEntry->m_uState = static_cast<const svr_task_complete*>(pNotify)->sub_tags.state;

		if (!pEntry->IsSuccess())
		{
#ifndef TASK_TEMPL_EDITOR
			RecursiveCheckPunchMonster(this);
#endif
		}
		if (CanShowPrompt() && !m_bDisplayInTitleTaskUI) TaskInterface::ShowTaskMessage(
			m_ID,
			(pEntry->IsSuccess() && !pEntry->IsGiveUp()) ? TASK_MSG_SUCCESS : TASK_MSG_FAIL);

		RecursiveAward(
			pTask,
			static_cast<ActiveTaskList*>(pTask->GetActiveTaskList()),
			pEntry,
			ulTime,
			-1,
			&sub_tags);

		pTask->UpdateConfirmTasksMap();
		
		pTask->UpdateTaskEmotionAction(m_ID);		
		if ((m_enumMethod == enumTMSimpleClientTask) && m_uiEmotion)
			TaskInterface::PopEmotionUI(m_ID,m_uiEmotion,false);
		
		if (m_bDisplayInTitleTaskUI) TaskInterface::UpdateTitleUI(m_ID);	

		pTask->OnCompleteTask(m_ID);

		break;
	case TASK_SVR_NOTIFY_GIVE_UP:
		pLst = static_cast<ActiveTaskList*>(pTask->GetActiveTaskList());
		pLst->ClearTask(pTask, pEntry, false);	
		if (m_bDisplayInTitleTaskUI) TaskInterface::UpdateTitleUI(m_ID);
		if ((m_enumMethod == enumTMSimpleClientTask) && m_uiEmotion)
			TaskInterface::PopEmotionUI(m_ID,m_uiEmotion,false);

		pTask->OnGiveupTask(m_ID);

		break;
	case TASK_SVR_NOTIFY_FINISHED:
		pEntry->SetFinished();
		break;
	case TASK_SVR_NOTIFY_DIS_GLOBAL_VAL:
//		DisplayTaskCharInfo(pTask, pEntry);
		break;
	default:
		assert(false);
	}
	TaskInterface::UpdateTaskUI(pNotify->task, pNotify->reason);
}

int TaskInterface::GetFirstSubTaskPosition(unsigned long ulParentTaskId)
{
	ActiveTaskList* pLst = static_cast<ActiveTaskList*>(GetActiveTaskList());
	unsigned char i;

	for (i = 0; i < pLst->m_uTaskCount; i++)
	{
		ActiveTaskEntry& CurEntry = pLst->m_TaskEntries[i];
		if (ulParentTaskId != CurEntry.m_ID) continue;
		if (CurEntry.m_ChildIndex == 0xff || CurEntry.m_ID == 0) return -1;
		else return (int)CurEntry.m_ChildIndex;
	}

	return -1;
}

unsigned long TaskInterface::GetNextSub(int& nPosition)
{
	ActiveTaskList* pLst = static_cast<ActiveTaskList*>(GetActiveTaskList());
	if (nPosition < 0 || nPosition >= (int)pLst->m_uTaskCount) return 0;

	ActiveTaskEntry& CurEntry = pLst->m_TaskEntries[nPosition];
	if (CurEntry.m_NextSblIndex == 0xff) nPosition = -1;
	else nPosition = (int)CurEntry.m_NextSblIndex;

	return CurEntry.m_ID;
}

unsigned long TaskInterface::GetSubAt(int nPosition)
{
	ActiveTaskList* pLst = static_cast<ActiveTaskList*>(GetActiveTaskList());
	if (nPosition < 0 || nPosition >= (int)pLst->m_uTaskCount) return 0;

	return pLst->m_TaskEntries[nPosition].m_ID;
}

unsigned long TaskInterface::GetTaskCount()
{
	ActiveTaskList* pLst = static_cast<ActiveTaskList*>(GetActiveTaskList());
	unsigned long ulCount = 0;

	for (unsigned char i = 0; i < pLst->m_uTaskCount; i++)
	{
		ActiveTaskEntry& CurEntry = pLst->m_TaskEntries[i];

		const ATaskTempl *pTempl = CurEntry.GetTempl();

		if (pTempl && !pTempl->m_bDisplayInTitleTaskUI && CurEntry.m_ParentIndex == 0xff)
		{
			if (!pTempl->m_bHidden || pTempl->CanShowPrompt())
			{
				ulCount++;
			}	
		}		
	}

	return ulCount;
}

unsigned long TaskInterface::GetTaskId(unsigned long ulIndex)
{
	ActiveTaskList* pLst = static_cast<ActiveTaskList*>(GetActiveTaskList());
	unsigned char uTopCount = 0, uCount = 0;
	
	while (uCount < pLst->m_uTaskCount)
	{
		ActiveTaskEntry& CurEntry = pLst->m_TaskEntries[uCount];

		const ATaskTempl *pTempl = CurEntry.GetTempl();

		if (pTempl && !pTempl->m_bDisplayInTitleTaskUI && CurEntry.m_ParentIndex == 0xff)
		{
			if (!pTempl->m_bHidden || pTempl->CanShowPrompt())
			{
				if (ulIndex == uTopCount)
					return CurEntry.m_ID;
				else
					uTopCount++;
			}	
		}

		uCount++;
	}

	return 0;
}

bool TaskInterface::CheckParent(unsigned long ulParent, unsigned long ulChild)
{
	ATaskTempl* pTempl = GetTaskTemplMan()->GetTaskTemplByID(ulChild);
	bool ret(false);
	if (pTempl)
	{
		ATaskTempl* pParent = pTempl;
		while (pParent->m_pParent)
			pParent = pParent->m_pParent;
		ret = ulParent == pParent->m_ID;
	}
		
	return ret;
}
unsigned long TaskInterface::CanDeliverTask(unsigned long ulTaskId)
{
	const ATaskTempl* pTempl = GetTaskTemplMan()->GetTopTaskByID(ulTaskId);
	if (!pTempl) return TASK_PREREQU_FAIL_NO_TASK;
	return pTempl->CheckPrerequisite(this, static_cast<ActiveTaskList*>(GetActiveTaskList()), GetCurTime(), true, true, false);
}

bool TaskInterface::CanShowTask(unsigned long ulTaskId)
{
	const ATaskTempl* pTempl = GetTaskTemplMan()->GetTopTaskByID(ulTaskId);
	return pTempl && pTempl->CanShowTask(this);
}

bool TaskInterface::HasTask(unsigned long ulTaskId)
{
	ActiveTaskEntry* pEntry = static_cast<ActiveTaskList*>(GetActiveTaskList())->GetEntry(ulTaskId);
	return pEntry && pEntry->GetTempl();
}

bool TaskInterface::CanFinishTask(unsigned long ulTaskId)
{
	ActiveTaskEntry* pEntry = static_cast<ActiveTaskList*>(GetActiveTaskList())->GetEntry(ulTaskId);

	if (!pEntry)
		return false;

	const ATaskTempl* pTempl = pEntry->GetTempl();

	if (!pTempl)
		return false;

	if (pTempl->m_bMarriage && !IsCaptain())
		return false;

	return pTempl->CanFinishTask(this, pEntry, GetCurTime());
}

void TaskInterface::GiveUpTask(unsigned long ulTaskId)
{
	ActiveTaskEntry* pEntry = static_cast<ActiveTaskList*>(GetActiveTaskList())->GetEntry(ulTaskId);
	if (!pEntry || !pEntry->GetTempl()) return;
	_notify_svr(this, TASK_CLT_NOTIFY_CHECK_GIVEUP, static_cast<unsigned short>(pEntry->GetTempl()->GetTopTask()->GetID()));
}

const unsigned short* TaskInterface::GetStorageTasks(unsigned int uStorageId)
{
	assert(uStorageId != 0 && uStorageId <= TASK_STORAGE_COUNT);
	StorageTaskList* pLst = static_cast<StorageTaskList*>(GetStorageTaskList());
	return pLst->m_Storages[uStorageId-1];
}

void TaskInterface::GetTaskStateInfo(unsigned long ulTaskId, Task_State_info* pInfo, bool bActiveTask)
{
	memset(pInfo, 0, sizeof(*pInfo));

	ActiveTaskList* pLst = static_cast<ActiveTaskList*>(GetActiveTaskList());
	unsigned long i, j, ulCurTime = GetCurTime();
	const ATaskTempl* pTempl = NULL;

	if(bActiveTask)
	{
		for (i = 0; i < pLst->m_uTaskCount; i++)
		{
			ActiveTaskEntry& CurEntry = pLst->m_TaskEntries[i];
			if (CurEntry.m_ID != ulTaskId || !CurEntry.m_ulTemplAddr) continue;

			pTempl = CurEntry.GetTempl();

			if (pTempl->CanFinishTask(this, &CurEntry, ulCurTime)) {
				pInfo->m_ulErrCode = pTempl->RecursiveCheckAward(this, pLst, &CurEntry, ulCurTime, -1);
				if (pInfo->m_ulErrCode == TASK_AWARD_FAIL_LEVEL_CHECK)
				{
					const ATaskTempl* pParent = pTempl;
					while (pParent && !pParent->m_ulPremise_Lev_Min)
						pParent = pParent->m_pParent;
					pInfo->m_ulPremLevelMin = pParent->m_ulPremise_Lev_Min;
				}
			}

			pInfo->m_ulTimePassed = ulCurTime > CurEntry.m_ulTaskTime ?
				ulCurTime - CurEntry.m_ulTaskTime : 0;

			if (pTempl->m_ulPremise_Lev_Min) {
				if (!pTempl->m_bPremCheckMaxHistoryLevel && GetPlayerLevel() < pTempl->m_ulPremise_Lev_Min){
					pInfo->m_ulErrCode = TASK_AWARD_FAIL_LEVEL_CHECK;
					pInfo->m_ulPremLevelMin = pTempl->m_ulPremise_Lev_Min;
				}
			}

			break;
		}
	}
	else
	{
		pTempl = GetTaskTemplMan()->GetTopTaskByID(ulTaskId);
	}

	if (pTempl == NULL)
		return;

	if (pTempl->m_ulTimeLimit) pInfo->m_ulTimeLimit = pTempl->m_ulTimeLimit;

	if (pTempl->m_ulGoldWanted) pInfo->m_ulGoldWanted = pTempl->m_ulGoldWanted;

	if (pTempl->m_enumMethod == enumTMCollectNumArticle)
	{
		for (j = 0; j < pTempl->m_ulItemsWanted; j++)
		{
			const ITEM_WANTED& iw = pTempl->m_ItemsWanted[j];

			pInfo->m_ItemsWanted[j].m_ulItemId		= iw.m_ulItemTemplId;
			pInfo->m_ItemsWanted[j].m_ulItemsToGet	= iw.m_ulItemNum;
			pInfo->m_ItemsWanted[j].m_ulItemsGained = _get_item_count(this, iw.m_ulItemTemplId, iw.m_bCommonItem);
		}
	}
	else if (pTempl->m_enumMethod == enumTMKillNumMonster)
	{
		unsigned long ulItemCount = 0;
		unsigned long ulMonsterCount = 0;

		for (j = 0; j < pTempl->m_ulMonsterWanted; j++)
		{
			const MONSTER_WANTED& mw = pTempl->m_MonsterWanted[j];

			if (mw.m_ulDropItemId)
			{
				pInfo->m_ItemsWanted[ulItemCount].m_ulMonsterId = mw.m_ulMonsterTemplId;
				pInfo->m_ItemsWanted[ulItemCount].m_ulItemId = mw.m_ulDropItemId;
				pInfo->m_ItemsWanted[ulItemCount].m_ulItemsToGet = mw.m_ulDropItemCount;
				pInfo->m_ItemsWanted[ulItemCount].m_ulItemsGained = _get_item_count(this, mw.m_ulDropItemId, mw.m_bDropCmnItem);
				ulItemCount++;
			}
			else
			{
				pInfo->m_MonsterWanted[ulMonsterCount].m_ulMonsterId = mw.m_ulMonsterTemplId;
				pInfo->m_MonsterWanted[ulMonsterCount].m_ulMonstersToKill = mw.m_ulMonsterNum;
	
				if(bActiveTask)
				{
					ActiveTaskEntry& CurEntry = pLst->m_TaskEntries[i];
					pInfo->m_MonsterWanted[ulMonsterCount].m_ulMonstersKilled = CurEntry.m_wMonsterNum[j];
				}

				ulMonsterCount++;
			}
		}
	}
	else if (pTempl->m_enumMethod == enumTMKillPlayer)
	{
		unsigned long ulItemCount = 0;
		unsigned long ulPlayerCount = 0;
		
		for (j = 0; j < pTempl->m_ulPlayerWanted; j++)
		{
			const PLAYER_WANTED& pw = pTempl->m_PlayerWanted[j];
			
			if (pw.m_ulDropItemId)
			{
				pInfo->m_ItemsWanted[ulItemCount].m_ulItemId = pw.m_ulDropItemId;
				pInfo->m_ItemsWanted[ulItemCount].m_ulItemsToGet = pw.m_ulDropItemCount;
				pInfo->m_ItemsWanted[ulItemCount].m_ulItemsGained = _get_item_count(this, pw.m_ulDropItemId, pw.m_bDropCmnItem);
				ulItemCount++;
			}
			else
			{
				pInfo->m_PlayerWanted[ulPlayerCount].m_ulPlayersToKill = pw.m_ulPlayerNum;
				
				if(bActiveTask)
				{
					ActiveTaskEntry& CurEntry = pLst->m_TaskEntries[i];
					pInfo->m_PlayerWanted[ulPlayerCount].m_ulPlayersKilled = CurEntry.m_wMonsterNum[j];
				}

				pInfo->m_PlayerWanted[ulPlayerCount].m_Requirements = pw.m_Requirements;

				ulPlayerCount++;
			}
		}
	}
	else if (pTempl->m_enumMethod == enumTMProtectNPC)
	{
		pInfo->m_ulNPCToProtect	= pTempl->m_ulNPCToProtect;
		pInfo->m_ulProtectTime	= pTempl->m_ulProtectTimeLen;
	}
	else if (pTempl->m_enumMethod == enumTMWaitTime)
	{
		pInfo->m_ulWaitTime = pTempl->m_ulWaitTime;
	}	
	else if (pTempl->m_enumMethod == enumTMGlobalValOK)
	{
		pTempl->GetGlobalTaskChar(this, pInfo->m_TaskCharArr);
	}
	else if (pTempl->m_enumMethod == enumTMReachLevel)
	{
		pInfo->m_ulReachLevel = pTempl->m_ulReachLevel;
		pInfo->m_ulReachReincarnation = pTempl->m_ulReachReincarnationCount;
		pInfo->m_ulReachRealm = pTempl->m_ulReachRealmLevel;
	}

	return;
}

void TaskInterface::GetTaskAwardPreview(unsigned long ulTaskId, Task_Award_Preview* p, bool bActiveTask)
{
	memset(p, 0, sizeof(*p));

	ActiveTaskList* pLst = static_cast<ActiveTaskList*>(GetActiveTaskList());
	unsigned long i, j, ulCurTime = GetCurTime();
	const ATaskTempl* pTempl;
	AWARD_DATA ad;
	unsigned long ulMulti = 1;
	
	if(bActiveTask)
	{
		for (i = 0; i < pLst->m_uTaskCount; i++)
		{
			ActiveTaskEntry& CurEntry = pLst->m_TaskEntries[i];
			if (CurEntry.m_ID != ulTaskId || !CurEntry.m_ulTemplAddr) continue;

			pTempl = CurEntry.GetTempl();


			pTempl->CalcAwardData(this, &ad, &CurEntry, CurEntry.m_ulTaskTime, ulCurTime);

			ulMulti = pTempl->CalcAwardMulti(
				this,
				&CurEntry,
				CurEntry.m_ulTaskTime,
				ulCurTime
				);

			if (!ulMulti) 
				return;

			break;
		}
	}
	else
	{
		pTempl = GetTaskTemplMan()->GetTopTaskByID(ulTaskId);
		if (pTempl == NULL)
			return;

		ad = *(pTempl->m_Award_S);
	}

	p->m_ulGold	= ad.m_ulGoldNum * ulMulti;
	p->m_ulExp	= ad.m_ulExp * ulMulti;
	p->m_ulSP	= ad.m_ulSP * ulMulti;
	p->m_iForceActivity = ad.m_iForceActivity;
	p->m_iForceContrib = ad.m_iForceContribution;
	p->m_iForceRepu = ad.m_iForceReputation;
	p->m_ulRealmExp = ad.m_ulRealmExp;

	if (ad.m_bUseLevCo)
	{
		unsigned long ulLev = GetPlayerLevel();

		if (ulLev > MAX_PLAYER_LEV)
			ulLev = MAX_PLAYER_LEV;
		else if (ulLev == 0)
			ulLev = 1;

		unsigned long ulUpper = pTempl->GetTopTask()->m_ulPremise_Lev_Max;

		if (ulUpper && ulLev > ulUpper)
			ulLev = ulUpper;

		unsigned long co = _lev_co[ulLev-1];
		p->m_ulExp *= co;
		p->m_ulSP *= co;
	}

	if (ad.m_ulCandItems == 1)
	{
		p->m_bHasItem = true;
		p->m_bItemKnown = true;

		const AWARD_ITEMS_CAND& ic = ad.m_CandItems[0];

		if (ic.m_bRandChoose)
			p->m_bItemKnown = false;
		else
		{
			for (j = 0; j < ic.m_ulAwardItems; j++)
			{
				const ITEM_WANTED& wi = ic.m_AwardItems[j];

				if (!wi.m_bCommonItem) continue;
				else if (wi.m_fProb != 1.0f)
				{
					p->m_bItemKnown = false;
					break;
				} 
				else if (_ConvertPeriod(wi.m_lPeriod, this) < 0)
				{
					continue;
				}
				else
				{
					p->m_ItemsId[p->m_ulItemTypes] = wi.m_ulItemTemplId;
					p->m_ItemsNum[p->m_ulItemTypes] = wi.m_ulItemNum;
					p->m_ulItemTypes++;
				}
			}
		}
	}
	else if (ad.m_ulCandItems > 1)
		p->m_bHasItem = true;

	return;

}

bool TaskInterface::GetAwardCandidates(unsigned long ulTaskId, AWARD_DATA* pAward)
{
	ActiveTaskList* pLst = static_cast<ActiveTaskList*>(GetActiveTaskList());
	ActiveTaskEntry* pEntry = pLst->GetEntry(ulTaskId);
	if (!pEntry || !pEntry->m_ulTemplAddr) return false;

	unsigned long ulCurTime = GetCurTime();
	pEntry->GetTempl()->CalcAwardData(
		this,
		pAward,
		pEntry,
		pEntry->m_ulTaskTime,
		ulCurTime);

	return true;
}

void TaskInterface::OnUIDialogEnd(unsigned long ulTask)
{
	TaskInterface::SetFinishDlgShowTime(0);
	ActiveTaskList* pLst = static_cast<ActiveTaskList*>(GetActiveTaskList());
	ActiveTaskEntry* pEntry = pLst->GetEntry(ulTask);
	if (!pEntry || !pEntry->m_ulTemplAddr) return;
	const ATaskTempl* pTempl = reinterpret_cast<const ATaskTempl*>(pEntry->m_ulTemplAddr);

	switch (pTempl->m_enumMethod)
	{
	case enumTMReachSite:
		pTempl->IncValidCount();
		_notify_svr(this, TASK_CLT_NOTIFY_REACH_SITE, static_cast<unsigned short>(ulTask));
		break;
	}
}

// 进入世界时检查PQ任务初始信息
void TaskInterface::CheckPQEnterWorldInit()
{
	ActiveTaskList* pList = static_cast<ActiveTaskList*>(GetActiveTaskList());
	ActiveTaskEntry* aEntries = pList->m_TaskEntries;

	for(unsigned char i = 0; i < pList->m_uTaskCount; i++)
	{
		ActiveTaskEntry& CurEntry = aEntries[i];

		if (!CurEntry.m_ulTemplAddr)
			continue;
		
		const ATaskTempl* pTempl = CurEntry.GetTempl();
		if (!pTempl || !pTempl->m_bPQTask)
			continue;

		pTempl->IncValidCount();
		_notify_svr(this, TASK_CLT_NOTIFY_PQ_CHECK_INIT, CurEntry.m_ID);
	}
}
#else

bool TaskInterface::RefreshTaskStorage(unsigned long storage_id)
{
	StorageTaskList* pLst = static_cast<StorageTaskList*>(GetStorageTaskList());
	return GetTaskTemplMan()->UpdateStorage(this, pLst, GetCurTime(), storage_id);
}

void TaskInterface::BeforeSaveData()
{
	ActiveTaskList* pList = static_cast<ActiveTaskList*>(GetActiveTaskList());

	if (!pList->IsTimeMarkUpdate())
		return;

	pList->ClearTimeMarkUpdate();
	ActiveTaskEntry* aEntries = pList->m_TaskEntries;
	unsigned long ulCurTime = GetCurTime();
	unsigned char i;
	const ATaskTempl* pTempl;

	for (i = 0; i < pList->m_uTaskCount; i++)
	{
		ActiveTaskEntry& CurEntry = aEntries[i];
		pTempl = CurEntry.GetTempl();

		if (pTempl && !pTempl->m_bAbsTime && !pTempl->m_bPQTask && !pTempl->m_bPQSubTask)
			CurEntry.m_ulTaskTime = ulCurTime - CurEntry.m_ulTaskTime;
	}
}

bool _player_switch_scene(TaskInterface* pTask)
{
	ActiveTaskList* pLst = (ActiveTaskList*)pTask->GetActiveTaskList();
	ActiveTaskEntry* pEntries = pLst->m_TaskEntries;
	
	for (int i = 0; i < pLst->m_uTaskCount; i++)
	{
		ActiveTaskEntry& CurEntry = pEntries[i];

		if (!CurEntry.IsSuccess())
			continue;
		
		const ATaskTempl* pTempl = CurEntry.GetTempl();	
		if (pTempl && pTempl->m_bSwitchSceneFail)
		{
			CurEntry.ClearSuccess();
			pTempl->OnSetFinished(pTask, pLst, &CurEntry, false);
			return true;
		}
	}

	return false;
}


void TaskInterface::PlayerSwitchScene()
{
	while (_player_switch_scene(this)){}
}

void TaskInterface::PlayerLeaveScene()
{
	ActiveTaskList* pLst = (ActiveTaskList*)GetActiveTaskList();
	ActiveTaskEntry* pEntries = pLst->m_TaskEntries;

	for (int i = 0; i < pLst->m_uTaskCount; i++)
	{
		ActiveTaskEntry& CurEntry = pEntries[i];
	
		const ATaskTempl* pTempl = CurEntry.GetTempl();	
		if(pTempl->m_bPQTask)
		{
			PublicQuestInterface::QuestRemovePlayer(pTempl->m_ID, GetPlayerId());
			return;
		}
	}	
}

void TaskInterface::PlayerLeaveWorld()
{
	ActiveTaskList* pLst = (ActiveTaskList*)GetActiveTaskList();
	ActiveTaskEntry* pEntries = pLst->m_TaskEntries;

	for (int i = 0; i < (int)pLst->m_uTaskCount; i++)
	{
		ActiveTaskEntry& CurEntry = pEntries[i];

		const ATaskTempl* pTempl = CurEntry.GetTempl();	
		// 若任务为下线即失败
		if (pTempl && pTempl->m_bOfflineFail)
		{
			TaskInterface::WriteLog(0, pTempl->m_ID, 0, "PlayerLeaveWorld, Task off line fail");
			
			pLst->ClearTask(this, &CurEntry, false);
			// 若遍历已接任务列表的途中完成并清空了某个任务，则需要重新遍历列表
			i = -1;
			continue;
		}
		if(pTempl && pTempl->m_bPQTask)
		{
			PublicQuestInterface::QuestLeaveWorld(pTempl->m_ID, GetPlayerId());
			continue;
		}
	}	
}

void TaskInterface::PQEnterWorldInit(unsigned long task_id)
{
	ActiveTaskList* pLst = static_cast<ActiveTaskList*>(GetActiveTaskList());
	ActiveTaskEntry* pEntries = pLst->m_TaskEntries;
	
	for (int i = 0; i < pLst->m_uTaskCount; i++)
	{
		ActiveTaskEntry& CurEntry = pEntries[i];
		
		const ATaskTempl* pTempl = CurEntry.GetTempl();	
		if (pTempl && pTempl->m_bPQTask && pTempl->m_ID == task_id)
		{
			if(CurEntry.m_ulTaskTime == PublicQuestInterface::GetCurTaskStamp(pTempl->m_ID))
				PublicQuestInterface::QuestEnterWorldInit(pTempl->m_ID, GetPlayerId());
			else
			{
				CurEntry.ClearSuccess();
				pTempl->OnSetFinished(this, pLst, &CurEntry, false);
			}

			return;
		}
	}
}


bool TaskInterface::OnCheckTeamRelationship(int nReason, TaskTeamInterface* pTeam)
{
	if (!IsDeliverLegal())
		return false;

	if (nReason == TASK_TEAM_RELATION_MARRIAGE)
	{
		if (IsMarried() || IsAtCrossServer())
			return false;

		ActiveTaskList* pList = static_cast<ActiveTaskList*>(GetActiveTaskList());
		ActiveTaskEntry* aEntries = pList->m_TaskEntries;
		unsigned char i;
		const ATaskTempl* pTempl;

		for (i = 0; i < pList->m_uTaskCount; i++)
		{
			ActiveTaskEntry& CurEntry = aEntries[i];
			pTempl = CurEntry.GetTempl();

			if (pTempl && pTempl->m_bMarriage)
			{
				if (!CurEntry.IsSuccess())
					return false;

				unsigned long ulCurTime = GetCurTime();
				return pTempl->CanFinishTask(this, &CurEntry, ulCurTime) && pTempl->RecursiveCheckAward(this, pList, &CurEntry, ulCurTime, -1) == 0;
			}
		}
	}

	return false;
}

void TaskInterface::OnCheckTeamRelationshipComplete(int nReason, TaskTeamInterface* pTeam)
{
	if (nReason == TASK_TEAM_RELATION_MARRIAGE)
	{
		pTeam->SetMarriage(GetPlayerId());

		ActiveTaskList* pList = static_cast<ActiveTaskList*>(GetActiveTaskList());
		ActiveTaskEntry* aEntries = pList->m_TaskEntries;
		unsigned char i;
		const ATaskTempl* pTempl;

		for (i = 0; i < pList->m_uTaskCount; i++)
		{
			ActiveTaskEntry& CurEntry = aEntries[i];
			pTempl = CurEntry.GetTempl();

			if (pTempl && pTempl->m_bMarriage)
			{
				pTempl->DeliverAward(this, pList, &CurEntry, -1);
				return;
			}
		}
	}
}

void TaskInterface::ExpandTaskLimit()
{
	ActiveTaskList* pList = static_cast<ActiveTaskList*>(GetActiveTaskList());
	pList->ExpandMaxSimultaneousCount();
	task_notify_base data;
	data.reason		= TASK_SVR_NOTIFY_SET_TASK_LIMIT;

	NotifyClient(&data, sizeof(task_notify_base));
}
bool TaskInterface::OnGiftCardTask(int type)
{
	ATaskTemplMan* pMan = GetTaskTemplMan();
	TaskTemplMap& map_dyn = pMan->GetDynTemplMap();
	TaskTemplMap::iterator iter = map_dyn.begin();
	const ATaskTempl* pTempl;
	for (; iter != map_dyn.end(); ++iter) {
		pTempl = iter->second;
		if (pTempl->m_DynTaskType == enumDTTGiftCard && 
			pTempl->m_ulSpecialAward == type)
			return pTempl->CheckDeliverTask(this, 0, NULL) == 0;
	}
	return TASK_PREREQU_FAIL_NO_GIFTCARD_TASK;
}

void TaskInterface::UpdateOneStorageDebug(int idStorage, bool bUseDayAsSeed)
{
	GetTaskTemplMan()->UpdateOneStorageDebug(this, GetCurTime(), idStorage, bUseDayAsSeed);
}
#endif

void TaskGlobalData::CheckRcvUpdateTime(unsigned long ulCurTime, int nFrequency)
{
	if (nFrequency == enumTAFNormal || m_ulRcvUpdateTime == 0)
		return;

	tm tmCur = *localtime((time_t*)&ulCurTime);
	tm tmRcv = *localtime((time_t*)&m_ulRcvUpdateTime);

	if (nFrequency == enumTAFEachDay)
	{
		if (tmCur.tm_year != tmRcv.tm_year || tmCur.tm_yday != tmRcv.tm_yday)
			m_ulReceiverNum = 0;
	}
	else if (nFrequency == enumTAFEachWeek)
	{
		if (!_is_same_week(&tmCur, &tmRcv, ulCurTime, m_ulRcvUpdateTime))
			m_ulReceiverNum = 0;
	}
	else if (nFrequency == enumTAFEachMonth)
	{
		if (tmCur.tm_year != tmRcv.tm_year || tmCur.tm_mon != tmRcv.tm_mon)
			m_ulReceiverNum = 0;
	}
	else if (nFrequency == enumTAFEachYear)
	{
		if (tmCur.tm_year != tmRcv.tm_year)
			m_ulReceiverNum = 0;
	}
}

const unsigned short _mask = 1 << 15;
#define MASK_TASK_ID(n) static_cast<unsigned long>((n) & (~_mask))

int FinishedTaskList::GetTaskPos(unsigned long ulID) const
{
	if (m_FnshHeader.m_uTaskCount == 0) return -1;
	else if (m_FnshHeader.m_uTaskCount == 1)
	{
		if (ulID == m_aTaskList[0].m_uTaskId)
			return 0;
		else
			return -1;
	}

	unsigned long ulStart = 0;
	unsigned long ulEnd = m_FnshHeader.m_uTaskCount - 1;

	while (ulStart + 1 < ulEnd)
	{
		unsigned long ulMid = (ulStart + ulEnd) >> 1;

		if (ulID == m_aTaskList[ulMid].m_uTaskId)
			return ulMid;
		else if (ulID < m_aTaskList[ulMid].m_uTaskId)
			ulEnd = ulMid;
		else
			ulStart = ulMid;
	}

	if (m_aTaskList[ulStart].m_uTaskId == ulID)
		return ulStart;

	if (m_aTaskList[ulEnd].m_uTaskId == ulID)
		return ulEnd;

	return -1;
}

int FinishedTaskList::SearchTask(unsigned long ulID) const
{
	/*
	 *	无此任务返回	-1
	 *	此任务成功返回	0
	 *	此任务失败返回	1
	 */

	int nPos = GetTaskPos(ulID);
	if (nPos < 0) return -1;

	return m_aTaskList[nPos].m_Mask;
}

unsigned char FinishedTaskList::SearchTaskFinishCount(unsigned long ulID) const
{
	int nPos = GetTaskPos(ulID);
	unsigned char ret = nPos < 0 ? 0 : m_aTaskList[nPos].m_FnshedCount;
	return ret;
}
void FinishedTaskList::ResetFinishCount(unsigned long ulID)
{
	int nPos = GetTaskPos(ulID);
	if (nPos >= 0) m_aTaskList[nPos].m_FnshedCount = 0;
}
void FinishedTaskList::AddForFinishCount(unsigned long ulID, bool bSuccess)
{
	int nPos = GetTaskPos(ulID);
	if (nPos >= 0) m_aTaskList[nPos].m_FnshedCount++;
	else { 
		AddOneTask(ulID, bSuccess);
		nPos = GetTaskPos(ulID);
		if (nPos >= 0) m_aTaskList[nPos].m_FnshedCount = 1;
	}
	
}
void FinishedTaskList::RemoveTask(unsigned long ulID)
{
	unsigned long ulPos = GetTaskPos(ulID);
	if (static_cast<int>(ulPos) < 0) return;

	m_FnshHeader.m_uTaskCount--;

	if (m_FnshHeader.m_uTaskCount <= ulPos)
		return;

	memmove(
		&m_aTaskList[ulPos],
		&m_aTaskList[ulPos+1],
		(m_FnshHeader.m_uTaskCount-ulPos) * sizeof(FnshedTaskEntry));
}

#define FNSH_LST_LOG

void FinishedTaskList::AddOneTask(unsigned long ulID, bool bSuccess)
{
	if (m_FnshHeader.m_uTaskCount >= TASK_FINISHED_LIST_MAX_LEN)
		return;

	// 二分法插入
	if (m_FnshHeader.m_uTaskCount == 0)
	{
		m_aTaskList[0].m_uTaskId =ulID;
		m_aTaskList[0].m_Mask = bSuccess ? 0 : 1;
		m_FnshHeader.m_uTaskCount++;
	}
	else if (m_FnshHeader.m_uTaskCount == 1)
	{
		if (ulID == m_aTaskList[0].m_uTaskId)
		{
			m_aTaskList[0].m_Mask = bSuccess ? 0 : 1;
			FNSH_LST_LOG
			return;
		}
		else if (ulID > m_aTaskList[0].m_uTaskId){
			m_aTaskList[1].m_uTaskId = ulID;
			m_aTaskList[1].m_Mask = bSuccess ? 0 : 1;
		}
		else
		{
			m_aTaskList[1] = m_aTaskList[0];
			m_aTaskList[0].m_uTaskId = ulID;
			m_aTaskList[0].m_Mask = bSuccess ? 0 : 1;
		}

		m_FnshHeader.m_uTaskCount++;
	}
	else
	{
		unsigned long ulStart = 0;
		unsigned long ulEnd = m_FnshHeader.m_uTaskCount - 1;

		while (ulStart + 1 < ulEnd)
		{
			unsigned long ulMid = (ulStart + ulEnd) >> 1;

			if (ulID == m_aTaskList[ulMid].m_uTaskId)
			{
				m_aTaskList[ulMid].m_Mask = bSuccess ? 0 : 1;
				FNSH_LST_LOG
				return;
			}
			else if (ulID < m_aTaskList[ulMid].m_uTaskId)
				ulEnd = ulMid;
			else
				ulStart = ulMid;
		}

		unsigned long ulInsert;
		if (ulID == m_aTaskList[ulStart].m_uTaskId)
		{
			m_aTaskList[ulStart].m_Mask = bSuccess ? 0 : 1;
			FNSH_LST_LOG
			return;
		}
		else if (ulID == m_aTaskList[ulEnd].m_uTaskId)
		{
			m_aTaskList[ulEnd].m_Mask = bSuccess ? 0 : 1;
			FNSH_LST_LOG
			return;
		}
		else if (ulID < m_aTaskList[ulStart].m_uTaskId) ulInsert = ulStart;
		else if (ulID > m_aTaskList[ulEnd].m_uTaskId) ulInsert = ulEnd + 1;
		else ulInsert = ulEnd;

		if (m_FnshHeader.m_uTaskCount > ulInsert)
		{
			memmove(
				&m_aTaskList[ulInsert+1],
				&m_aTaskList[ulInsert],
				(m_FnshHeader.m_uTaskCount - ulInsert) * sizeof(FnshedTaskEntry));
		}

		m_aTaskList[ulInsert].m_uTaskId = ulID;
		m_aTaskList[ulInsert].m_Mask = bSuccess ? 0 : 1;
		m_FnshHeader.m_uTaskCount++;
	}

	FNSH_LST_LOG
}

unsigned long TaskFinishTimeList::Search(unsigned long ulID ) const
{
	for (unsigned short i = 0; i < m_uCount; i++)
		if (m_aList[i].m_uTaskId == (unsigned short)ulID)
			return m_aList[i].m_ulTimeMark;

	return 0;
}

void TaskFinishTimeList::AddOrUpdate(unsigned long ulID, unsigned long ulTime)
{
	for (unsigned short i = 0; i < m_uCount; i++)
	{
		if (m_aList[i].m_uTaskId == (unsigned short)ulID)
		{
			m_aList[i].m_ulTimeMark = ulTime;
			return;
		}
	}

	if (m_uCount >= TASK_FINISH_TIME_MAX_LEN)
		return;

	m_aList[m_uCount].m_uTaskId = (unsigned short)ulID;
	m_aList[m_uCount].m_ulTimeMark = ulTime;
	m_uCount++;
}

unsigned long TaskFinishCountList::Search(unsigned long ulID,unsigned long& ulTime) const
{
	for (unsigned short i = 0; i < m_uCount; i++)
		if (m_aList[i].m_uTaskId == (unsigned short)ulID)
		{
			ulTime = m_aList[i].m_ulFinishTime;
			return m_aList[i].m_ulFinishCount;
		}

	return 0;
}
void TaskFinishCountList::ResetAt(unsigned long ulID)
{
	for (unsigned short i = 0; i < m_uCount; i++)
		if (m_aList[i].m_uTaskId == (unsigned short)ulID)
			m_aList[i].m_ulFinishCount = 0;
}
void TaskFinishCountList::AddOrUpdate(unsigned long ulID,unsigned long ulFinishTime)
{
	for (unsigned short i = 0; i < m_uCount; i++)
	{
		if (m_aList[i].m_uTaskId == (unsigned short)ulID)
		{
			m_aList[i].m_ulFinishCount ++;
			m_aList[i].m_ulFinishTime = ulFinishTime;
			return;
		}
	}

	if (m_uCount >= TASK_FINISH_COUNT_MAX_LEN)
		return;

	m_aList[m_uCount].m_uTaskId = (unsigned short)ulID;
	m_aList[m_uCount].m_ulFinishCount = 1;
	m_aList[m_uCount].m_ulFinishTime = ulFinishTime;
	m_uCount++;
}

void ActiveTaskList::RealignTask(ActiveTaskEntry* pEntry, unsigned char uReserve)
{
	unsigned char uCurIndex = static_cast<unsigned char>(pEntry - m_TaskEntries);
	unsigned long ulCount = m_uTaskCount - uCurIndex; // 剩余的任务数

	if (ulCount == 0) return; // 最后一个任务

	unsigned char uEmptyCount = 0;
	for (unsigned char uEmpty = uCurIndex; uEmpty < TASK_ACTIVE_LIST_MAX_LEN; uEmpty++)
	{
		if (!m_TaskEntries[uEmpty].m_ID)
			uEmptyCount++;
		else
			break;
	}

	if (uReserve == uEmptyCount) return;

	ActiveTaskEntry* pSrc = pEntry + uEmptyCount;
	ActiveTaskEntry* pInsert = pEntry + uReserve;

	// move it
	memmove(pInsert, pSrc, sizeof(ActiveTaskEntry) * ulCount);

	// clear reserve part
	ActiveTaskEntry *pClearStart, *pClearEnd;

	if (pInsert > pSrc)
	{
		pClearStart = pSrc;
		pClearEnd = pInsert;
	}
	else
	{
		pClearStart = pInsert + ulCount;
		pClearEnd = pSrc + ulCount;
	}

	while (pClearStart < pClearEnd)
	{
		pClearStart->m_ulTemplAddr = 0;
		pClearStart->m_ID = 0;
		pClearStart++;
	}

	// calc gap
	unsigned char uGap = static_cast<unsigned char>(pInsert - pSrc);
	unsigned long i = 0;

	for (; i < static_cast<unsigned long>(uCurIndex); i++)
	{
		// Parent, Prev小于uCurIndex
		ActiveTaskEntry& CurEntry = m_TaskEntries[i];

		if(!CurEntry.m_ID)
			continue;

		if (CurEntry.m_ChildIndex != 0xff && CurEntry.m_ChildIndex >= uCurIndex)
			CurEntry.m_ChildIndex += uGap;
		if (CurEntry.m_NextSblIndex != 0xff && CurEntry.m_NextSblIndex >= uCurIndex)
			CurEntry.m_NextSblIndex += uGap;
	}

	for (i = 0; i < ulCount; i++)
	{
		ActiveTaskEntry& CurEntry = *(pInsert + i);
		if(!CurEntry.m_ID)
			continue;

		if (CurEntry.m_ParentIndex != 0xff && CurEntry.m_ParentIndex >= uCurIndex)
			CurEntry.m_ParentIndex += uGap;
		if (CurEntry.m_PrevSblIndex != 0xff && CurEntry.m_PrevSblIndex >= uCurIndex)
			CurEntry.m_PrevSblIndex += uGap;
		if (CurEntry.m_ChildIndex != 0xff)
			CurEntry.m_ChildIndex += uGap;
		if (CurEntry.m_NextSblIndex != 0xff)
			CurEntry.m_NextSblIndex += uGap;
	}
}

void ActiveTaskList::RecursiveClearTask(
	TaskInterface* pTask,
	ActiveTaskEntry* pEntry,
	bool bRemoveItem,
	bool bRemoveAcquired,
	bool bClearTask)
{
	while (pEntry->m_ChildIndex != 0xff)
	{
		RecursiveClearTask(
			pTask,
			&m_TaskEntries[pEntry->m_ChildIndex],
			bRemoveItem,
			bRemoveAcquired,
			bClearTask);
	}

	const ATaskTempl* pTempl = pEntry->GetTempl();

	// 去掉获得的物品
#ifndef _TASK_CLIENT
	if (bRemoveItem && pTempl)
	{
		if (bRemoveAcquired || pTempl->m_bClearAcquired) pTempl->RemoveAcquiredItem(pTask, bClearTask, false);
		pTempl->TakeAwayGivenItems(pTask);
	}
#endif

	unsigned short uTaskId = pEntry->m_ID;

	pEntry->m_ulTemplAddr = 0;
	pEntry->m_ID = 0;

	if (m_uTaskCount)
		m_uTaskCount--;
	else
		TaskInterface::WriteLog(pTask->GetPlayerId(), uTaskId, 0, "ClearTask, TaskCount == 0");

	if (pEntry->m_ParentIndex != 0xff)
	{
		if (pEntry->m_PrevSblIndex != 0xff)
			m_TaskEntries[pEntry->m_PrevSblIndex].m_NextSblIndex = pEntry->m_NextSblIndex;
		else
			m_TaskEntries[pEntry->m_ParentIndex].m_ChildIndex = pEntry->m_NextSblIndex;
		if (pEntry->m_NextSblIndex != 0xff) m_TaskEntries[pEntry->m_NextSblIndex].m_PrevSblIndex = pEntry->m_PrevSblIndex;
	}
	else
	{
		if (pTempl)
		{	
			if (pTempl->m_bHidden)
			{
				if (m_uTopHideTaskCount)
					m_uTopHideTaskCount--;
			}
			else if(pTempl->m_bDisplayInTitleTaskUI)
			{
				if (m_uTitleTaskCount)
					m_uTitleTaskCount--;
			}
			else
			{
				if (m_uTopShowTaskCount)
					m_uTopShowTaskCount--;
			}	
			
			if (m_uUsedCount >= pTempl->m_uDepth)
				m_uUsedCount -= pTempl->m_uDepth;
			else
			{
				TaskInterface::WriteLog(pTask->GetPlayerId(), uTaskId, 0, "ClearTask, No Enough Used Count");
				m_uUsedCount = 0;
			}
		}
	}
}

bool TaskInterface::SetForceReputation(int iValue)
{
	int iRepuNow = GetForceReputation();
	return ChangeForceReputation(-iRepuNow + iValue);
}