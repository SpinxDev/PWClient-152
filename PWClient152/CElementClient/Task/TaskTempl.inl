#ifndef _TASKTEMPL_INL_
#define _TASKTEMPL_INL_

// Process Part

#define MONSTER_PLAYER_LEVEL_MAX_DIFF 8

#ifdef _ELEMENTCLIENT
extern void TaskShowErrMessage(int nIndex);
#endif

inline bool ATaskTempl::IsAutoDeliver() const
{
	return m_bDeathTrig || m_bAutoDeliver;
}

inline unsigned long ATaskTempl::CheckBudget(ActiveTaskList* pList) const
{
	// 任务达到上限
	bool bReachLimit(false);
	if (m_bHidden)
		bReachLimit = pList->m_uTopHideTaskCount >= TASK_HIDDEN_COUNT;
	else if (m_bDisplayInTitleTaskUI)
		bReachLimit = bReachLimit || pList->m_uTitleTaskCount >= TASK_TITLE_TASK_COUNT;
	else bReachLimit = bReachLimit || pList->m_uTopShowTaskCount >= pList->GetMaxSimultaneousCount();
	
	if (bReachLimit)
		return TASK_PREREQU_FAIL_FULL;

	// Check Task List Empty Space
	if (pList->m_uUsedCount + m_uDepth > TASK_ACTIVE_LIST_MAX_LEN) 
		return TASK_PREREQU_FAIL_NO_SPACE;

	// 是否已有相同任务
	if (pList->GetEntry(m_ID)) return TASK_PREREQU_FAIL_SAME_TASK;

	return 0;
}

inline unsigned long ATaskTempl::GetMemTaskByInfo(const task_team_member_info* pInfo) const
{
	if (!m_ulTeamMemsWanted) return m_ID; // 无成员要求则返回本身

	unsigned long i;

	for (i = 0; i < m_ulTeamMemsWanted; i++)
	{
		const TEAM_MEM_WANTED& tmw = m_TeamMemsWanted[i];

		if (!tmw.IsMeetBaseInfo(pInfo))
			continue;

		return tmw.m_ulTask == 0 ? m_ID : tmw.m_ulTask; // 成员任务为0则返回队长任务
	}

	return 0;
}

inline unsigned long _get_item_count(TaskInterface* pTask, unsigned long ulItemId, bool bCommon)
{
	return bCommon ? pTask->GetCommonItemCount(ulItemId) : pTask->GetTaskItemCount(ulItemId);
}

inline bool ATaskTempl::HasAllItemsWanted(TaskInterface* pTask) const
{
	if (m_ulGoldWanted && pTask->GetGoldNum() < m_ulGoldWanted)
		return false;

	if (m_iFactionContribWanted && pTask->GetFactionConsumeContrib() < m_iFactionContribWanted)
		return false;

	if (m_iFactionExpContribWanted && pTask->GetFactionExpContrib() < m_iFactionExpContribWanted)
		return false;

	unsigned long i = 0;

	for (; i < m_ulItemsWanted; i++)
	{
		const ITEM_WANTED& iw = m_ItemsWanted[i];
		unsigned long ulNum = _get_item_count(
			pTask,
			iw.m_ulItemTemplId,
			iw.m_bCommonItem);

		if (!ulNum || ulNum < iw.m_ulItemNum) return false;
	}

	return true;
}

inline bool ATaskTempl::HasAllMonsterWanted(TaskInterface* pTask, const ActiveTaskEntry* pEntry) const
{
	bool bHasOne = false;

	for (unsigned long i = 0; i < m_ulMonsterWanted; i++)
	{
		const MONSTER_WANTED& mw = m_MonsterWanted[i];

		if (mw.m_ulDropItemId)
		{
			unsigned long ulCount = _get_item_count(pTask, mw.m_ulDropItemId, mw.m_bDropCmnItem);
			if (ulCount < mw.m_ulDropItemCount) return false;
			if (ulCount) bHasOne = true;
		}
		else if (pEntry->m_wMonsterNum[i] < mw.m_ulMonsterNum)
			return false;
		else if (pEntry->m_wMonsterNum[i])
			bHasOne = true;
	}

	return bHasOne;
}

inline bool ATaskTempl::HasAllPlayerKilled(TaskInterface* pTask, const ActiveTaskEntry* pEntry) const
{
	bool bHasOne = false;
	for (unsigned long i = 0; i < m_ulPlayerWanted; ++i)
	{
		const PLAYER_WANTED& pw = m_PlayerWanted[i];

		if (pw.m_ulDropItemId)
		{
			unsigned long ulCount = _get_item_count(pTask, pw.m_ulDropItemId, pw.m_bDropCmnItem);
			if (ulCount < pw.m_ulDropItemCount) return false;
			if (ulCount) bHasOne = true;
		}
		else if (pEntry->m_wMonsterNum[i] < pw.m_ulPlayerNum)
			return false;
		else if (pEntry->m_wMonsterNum[i])
			bHasOne = true;
	}
	return bHasOne;
}

inline unsigned long ATaskTempl::HasAllTeamMemsWanted(TaskInterface* pTask, bool bStrict) const
{
	if (m_ulTeamMemsWanted)
	{
		unsigned long pMemEligibleNum[MAX_TEAM_MEM_WANTED];
		memset(pMemEligibleNum, 0, sizeof(pMemEligibleNum));

		const int nMemNum = pTask->GetTeamMemberNum();
		int i;
		unsigned long j;
		task_team_member_info MemInfo;

		float pos[3];
		unsigned long ulWorldTag = pTask->GetPos(pos);
		std::set<unsigned long> setOccupation;
		pTask->GetTeamMemberInfo(0,&MemInfo);
		setOccupation.insert(MemInfo.m_ulOccupation);
#ifndef	_TASK_CLIENT
		unsigned long ulWorldIndex = MemInfo.m_ulWorldIndex;
#endif

		// 检查基本信息
		for (i = 1; i < nMemNum; i++) // 跳过队长
		{
			pTask->GetTeamMemberInfo(i, &MemInfo);
			setOccupation.insert(MemInfo.m_ulOccupation);

#ifndef	_TASK_CLIENT
			if (m_bRcvChckMem)
			{
				if (ulWorldTag != MemInfo.m_ulWorldTag || ulWorldIndex != MemInfo.m_ulWorldIndex)
					return TASK_PREREQU_FAIL_OUTOF_DIST;

				float x = pos[0] - MemInfo.m_Pos[0];
				float y = pos[1] - MemInfo.m_Pos[1];
				float z = pos[2] - MemInfo.m_Pos[2];
				float fDist = x * x + y * y + z * z;

				if (fDist > m_fRcvMemDist)
					return TASK_PREREQU_FAIL_OUTOF_DIST;
			}
#endif

			for (j = 0; j < m_ulTeamMemsWanted; j++)
			{
				if (m_TeamMemsWanted[j].IsMeetBaseInfo(&MemInfo))
				{
					pMemEligibleNum[j]++;
					break;
				}
			}

			if (bStrict && j == m_ulTeamMemsWanted) return TASK_PREREQU_FAIL_ILLEGAL_MEM;
		}
		//组队任务中不能有相同的职业
		if (m_bDistinguishedOcc)
		{
			if (setOccupation.size() != static_cast<unsigned int>(nMemNum))
			{
				return TASK_PREREQU_FAIL_ILLEGAL_MEM;
			}
		}

		//  夫妻任务
		if (m_bCoupleOnly)
		{
			pTask->GetTeamMemberInfo(1, &MemInfo);
			if (nMemNum != 2 || pTask->GetSpouseID() != MemInfo.m_ulId)
				return TASK_PREREQU_FAIL_NOT_COUPLE;			
		}

		// 检查人数
		for (j = 0; j < m_ulTeamMemsWanted; j++)
			if (!m_TeamMemsWanted[j].IsMeetCount(pMemEligibleNum[j]))
				return TASK_PREREQU_FAIL_ILLEGAL_MEM;
	}
#ifndef	_TASK_CLIENT
	else if (m_bRcvChckMem)
	{
		const int nMemNum = pTask->GetTeamMemberNum();
		int i;
		float pos[3], MemPos[3];
		unsigned long ulWorldTag = pTask->GetPos(pos);

		for (i = 1; i < nMemNum; i++)
		{
			if (pTask->GetTeamMemberPos(i, MemPos) != ulWorldTag)
				return TASK_PREREQU_FAIL_OUTOF_DIST;

			float x = pos[0] - MemPos[0];
			float y = pos[1] - MemPos[1];
			float z = pos[2] - MemPos[2];
			float fDist = x * x + y * y + z * z;

			if (fDist > m_fRcvMemDist) return TASK_PREREQU_FAIL_OUTOF_DIST;
		}
	}
#endif

	return 0;
}

inline bool ATaskTempl::CanFinishTask(
	TaskInterface* pTask,
	const ActiveTaskEntry* pEntry,
	unsigned long ulCurTime) const
{
	bool ret(false);
	while(true)
	{
		if (!pTask->IsDeliverLegal()) break;
		if (pEntry->IsFinished())
		{
			ret = true;
			break;
		};
		switch (m_enumMethod)
		{
		case enumTMCollectNumArticle:
			ret = HasAllItemsWanted(pTask);
			break;
		case enumTMWaitTime:
			ret = (m_ulWaitTime == 0) || (pEntry->m_ulTaskTime + m_ulWaitTime < ulCurTime);
			break;
		case enumTMTalkToNPC:
			ret = true;
			break;
		case enumTMGlobalValOK:
			ret = CheckGlobalKeyValue(pTask, true) == 0;
			break;
		case enumTMTransform:
			{
				int playerShapeType = pTask->GetShapeMask();
				// 任意非职业变身
				if (m_ucTransformedForm == 0x80)
				{
					if ((playerShapeType >> 6) == 2) ret = true;
				}
				// 特定变身
				else if (m_ucTransformedForm == playerShapeType) ret = true;
			}
			break;
		case enumTMReachTreasureZone:
			{
				float pos[3];
				unsigned long ulWorldId = pTask->GetPos(pos);
				if (ulWorldId == 1)
				{
					unsigned short uTreasureLocIndex = pEntry->m_iUsefulData1 & 0xFFFF;
					float fTreasureLocX = m_TreasureStartZone.x + (uTreasureLocIndex % m_ucZonesNumX - 1) * m_ucZoneSide;
					float fTreasureLocZ = m_TreasureStartZone.z + (uTreasureLocIndex / m_ucZonesNumX) * m_ucZoneSide;
					
					ret = pos[0] >= fTreasureLocX && pos[2] >= fTreasureLocZ && pos[0] <= fTreasureLocX + m_ucZoneSide && pos[2] <= fTreasureLocZ + m_ucZoneSide;
				}
			}
			break;
		case enumTMSimpleClientTask:
		case enumTMSimpleClientTaskForceNavi:
			{
				ret = pTask->CheckSimpleTaskFinshConditon(m_ID);
			}
			
			break;
		case enumTMReachLevel:
			ret = CheckReachLevel(pTask);
			break;
		default:
			break;
		}
		break;
	}
	if (m_ulPremise_Lev_Min) {
		if (!m_bPremCheckMaxHistoryLevel && pTask->GetPlayerLevel() < m_ulPremise_Lev_Min)
			ret = false;
	}
	return ret;
}

inline unsigned long ATaskTempl::CheckTeamTask(TaskInterface* pTask) const
{
	if (m_bTeamwork && m_bRcvByTeam) // 组队接收
	{
		if (!pTask->IsCaptain()) return TASK_PREREQU_FAIL_NOT_CAPTAIN;
		return HasAllTeamMemsWanted(pTask, true);
	}

	return 0;
}

inline unsigned long ATaskTempl::CheckMarriage(TaskInterface* pTask) const
{
	if (m_bMarriage)
	{
		// 跨服上不能领取结婚任务
		if (pTask->IsAtCrossServer())
			return TASK_PREREQU_FAIL_CROSSSERVER_NO_MARRIAGE;
		if (pTask->IsMarried())
			return TASK_PREREQU_FAIL_ILLEGAL_MEM;

		if (!pTask->IsInTeam())
			return TASK_PREREQU_FAIL_ILLEGAL_MEM;

		if (pTask->GetTeamMemberNum() != 2)
			return TASK_PREREQU_FAIL_ILLEGAL_MEM;

		task_team_member_info m1, m2;
		pTask->GetTeamMemberInfo(0, &m1);
		pTask->GetTeamMemberInfo(1, &m2);

		if (m1.m_bMale == m2.m_bMale)
			return TASK_PREREQU_FAIL_ILLEGAL_MEM;
	}

	return 0;
}

inline unsigned long ATaskTempl::CheckInZone(TaskInterface* pTask) const
{
	if (m_bDelvInZone)
	{
		float pos[3];
		unsigned long ulWorldId = pTask->GetPos(pos);

	/*	if (ulWorldId != m_ulDelvWorld ||
		   !is_in_zone(
			m_DelvMinVert,
			m_DelvMaxVert,
			pos))
			return TASK_PREREQU_FAIL_NOT_IN_ZONE;*/
		if(ulWorldId != m_ulDelvWorld)
			return TASK_PREREQU_FAIL_NOT_IN_ZONE;

		for (unsigned long i=0;i<m_ulDelvRegionCnt;i++)
		{
			const Task_Region& t = m_pDelvRegion[i];
			if(is_in_zone(t.zvMin,t.zvMax,pos))
				return 0;			
		}
		return TASK_PREREQU_FAIL_NOT_IN_ZONE;
	}

	return 0;
}
inline unsigned long ATaskTempl::CheckIvtrEmptySlot(TaskInterface* pTask) const
{
//	if (!pTask->IsDeliverLegal()) return TASK_PREREQU_FAIL_GIVEN_ITEM;
	if(!m_bCompareItemAndInventory) return 0;

	if (pTask->GetInvEmptySlot() < m_ulInventorySlotNum)
	{
		return TASK_PREREQU_FAIL_NOT_IVTRSLOTNUM;
	}

	return 0;
}
inline unsigned long ATaskTempl::CheckGivenItems(TaskInterface* pTask) const
{
	if (m_ulGivenItems)
	{
		if (!pTask->IsDeliverLegal()) return TASK_PREREQU_FAIL_GIVEN_ITEM;

		if (m_ulGivenCmnCount && !pTask->CanDeliverCommonItem(m_ulGivenCmnCount)
		 || m_ulGivenTskCount && !pTask->CanDeliverTaskItem  (m_ulGivenTskCount))
			return TASK_PREREQU_FAIL_GIVEN_ITEM;
	}

	return 0;
}

inline bool _is_same_week(const tm* t1, const tm* t2, long l1, long l2)
{
	long d = abs(l1 - l2);

	if (d >= (7 * 24 * 3600))
		return false;

	int w1 = task_week_map[t1->tm_wday];
	int w2 = task_week_map[t2->tm_wday];

	if (w1 == w2)
		return d <= 24 * 3600;
	else if (w1 > w2)
		return l1 > l2;
	else
		return l1 < l2;
}

inline unsigned long ATaskTempl::CheckFnshLst(TaskInterface* pTask, unsigned long ulCurTime) const
{
	// 判断失败成功条件
	/*
	 *	!!!IMPORTANT, 成功或失败后不能重复的任务的种类不能超过TASK_FINISHED_LIST_MAX_LEN
	 */
	if (!m_bCanRedo || !m_bCanRedoAfterFailure)
	{
		FinishedTaskList* pFinished = (FinishedTaskList*)pTask->GetFinishedTaskList();
		int nRet = pFinished->SearchTask(m_ID);

		// 成功后不能接此任务，或失败后不能重新接此任务
		if (nRet == 0 && !m_bCanRedo || nRet == 1 && !m_bCanRedoAfterFailure)
			return TASK_PREREQU_FAIL_CANT_REDO;
	}

	return 0;
}


inline unsigned long ATaskTempl::CheckTimetable(unsigned long ulCurTime) const
{
	if (!m_ulTimetable) return 0;

	unsigned long i;

	for (i = 0; i < m_ulTimetable; i++)
		if (judge_time_date(&m_tmStart[i], &m_tmEnd[i], ulCurTime, (task_tm_type)m_tmType[i]))
			return 0;

	return TASK_PREREQU_FAIL_WRONG_TIME;
}

inline unsigned long ATaskTempl::CheckDeliverTime(TaskInterface* pTask, unsigned long ulCurTime) const
{
	if (m_lAvailFrequency == enumTAFNormal)
		return 0;
	
	TaskFinishTimeList* pTimeList = (TaskFinishTimeList*)pTask->GetFinishedTimeList();
	TaskFinishCountList* pFinishCntList = (TaskFinishCountList*)pTask->GetFinishedCntList();

	if (pTimeList->m_uCount >= TASK_FINISH_TIME_MAX_LEN)
		return TASK_PREREQU_FAIL_FULL;

	unsigned long ulTaskTime;
	if (m_bAccountTaskLimit)
	{
		// 跨服状态下不能领取账号限次任务
		if (pTask->IsAtCrossServer())
		{
			return TASK_PREREQU_FAIL_CROSSSERVER_NO_ACOUNT_LIMIT;
		}
		if (pFinishCntList->Search(m_ID,ulTaskTime) == 0)
		{
			return 0;
		}	
	}
	else
	{
		ulTaskTime = pTimeList->Search(m_ID);

		if (ulTaskTime == 0)
			return 0;
	}

	tm tmCur, tmTask;

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

	if (m_lAvailFrequency == enumTAFEachDay)
	{
		if (tmCur.tm_year == tmTask.tm_year
		 && tmCur.tm_yday == tmTask.tm_yday)
		{

			if(!m_bAccountTaskLimit && !m_bRoleTaskLimit)
				return TASK_PREREQU_FAIL_WRONG_TIME;
			else if (CheckDeliverCount(pTask))
				return TASK_PREREQU_FAIL_WRONG_TIME;
		}
		else if(m_bAccountTaskLimit)
		{
			TaskFinishCountList* pFnshList = (TaskFinishCountList*)pTask->GetFinishedCntList();
			pFnshList->ResetAt(m_ID);
		}
		else if(m_bRoleTaskLimit)
		{
			FinishedTaskList* pFnshList = (FinishedTaskList*)pTask->GetFinishedTaskList();
			pFnshList->ResetFinishCount(m_ID);
		}
	}
	else if (m_lAvailFrequency == enumTAFEachWeek)
	{
		if (_is_same_week(&tmCur, &tmTask, ulCurTime, ulTaskTime))
		{
			if(!m_bAccountTaskLimit && !m_bRoleTaskLimit)
				return TASK_PREREQU_FAIL_WRONG_TIME;
			else if (CheckDeliverCount(pTask))
				return TASK_PREREQU_FAIL_WRONG_TIME;
		}
		else if(m_bAccountTaskLimit)
		{
			TaskFinishCountList* pFnshList = (TaskFinishCountList*)pTask->GetFinishedCntList();
			pFnshList->ResetAt(m_ID);
		}
		else if(m_bRoleTaskLimit)
		{
			FinishedTaskList* pFnshList = (FinishedTaskList*)pTask->GetFinishedTaskList();
			pFnshList->ResetFinishCount(m_ID);
		}
	}
	else if (m_lAvailFrequency == enumTAFEachMonth)
	{
		if (tmCur.tm_year == tmTask.tm_year
		 && tmCur.tm_mon  == tmTask.tm_mon)
		{
			if(!m_bAccountTaskLimit && !m_bRoleTaskLimit)
				return TASK_PREREQU_FAIL_WRONG_TIME;
			else if (CheckDeliverCount(pTask))
				return TASK_PREREQU_FAIL_WRONG_TIME;
		}
		else if(m_bAccountTaskLimit)
		{
			TaskFinishCountList* pFnshList = (TaskFinishCountList*)pTask->GetFinishedCntList();
			pFnshList->ResetAt(m_ID);
		}
		else if(m_bRoleTaskLimit)
		{
			FinishedTaskList* pFnshList = (FinishedTaskList*)pTask->GetFinishedTaskList();
			pFnshList->ResetFinishCount(m_ID);
		}
	}
	else if (m_lAvailFrequency == enumTAFEachYear)
	{
		if (tmCur.tm_year == tmTask.tm_year)
		{			
			if(!m_bAccountTaskLimit && !m_bRoleTaskLimit)
				return TASK_PREREQU_FAIL_WRONG_TIME;
			else if (CheckDeliverCount(pTask))
				return TASK_PREREQU_FAIL_WRONG_TIME;
		}
		else if(m_bAccountTaskLimit)
		{
			TaskFinishCountList* pFnshList = (TaskFinishCountList*)pTask->GetFinishedCntList();
			pFnshList->ResetAt(m_ID);
		}
		else if(m_bRoleTaskLimit)
		{
			FinishedTaskList* pFnshList = (FinishedTaskList*)pTask->GetFinishedTaskList();
			pFnshList->ResetFinishCount(m_ID);
		}
	}
	else
		return TASK_PREREQU_FAIL_WRONG_TIME;

	return 0;
}

inline unsigned long ATaskTempl::CheckRecordTasksNum(TaskInterface* pTask) const
{
	if (m_iPremNeedRecordTasksNum)
	{
		int iRecordTasksCnt = 0;
		FinishedTaskList* pLst = static_cast<FinishedTaskList*>(pTask->GetFinishedTaskList());
		for (unsigned long i = 0;i < pLst->m_FnshHeader.m_uTaskCount;++i)
		{
			if (pLst->m_aTaskList[i].m_Mask == 0)
			{
				++iRecordTasksCnt;
			}
		}
		if (iRecordTasksCnt < m_iPremNeedRecordTasksNum)
		{
			return TASK_PREREQU_FAIL_BELOW_RECORD_TASKS_NUM;
		}
	}
	return 0;
}

inline unsigned long ATaskTempl::CheckFactionContrib(TaskInterface* pTask) const
{
	int iContrib = pTask->GetFactionContrib();

	if (m_iPremiseFactionContrib && iContrib < m_iPremiseFactionContrib) return TASK_PREREQU_FAIL_BELOW_FACTION_CONTRIB;

	return 0;
}

inline unsigned long ATaskTempl::CheckLevel(TaskInterface* pTask) const
{
	unsigned long ulLevel = m_bPremCheckMaxHistoryLevel ? pTask->GetMaxHistoryLevel() : pTask->GetPlayerLevel();
	if (m_ulPremise_Lev_Min && ulLevel < m_ulPremise_Lev_Min) return TASK_PREREQU_FAIL_BELOW_LEVEL;
	if (m_ulPremise_Lev_Max && ulLevel > m_ulPremise_Lev_Max) return TASK_PREREQU_FAIL_ABOVE_LEVEL;
	return 0;
}
inline unsigned long ATaskTempl::CheckReincarnation(TaskInterface* pTask) const
{
	if (m_bPremCheckReincarnation) {
		unsigned long ulCount = pTask->GetReincarnationCount();
		if (ulCount < m_ulPremReincarnationMin) return TASK_PREREQU_FAIL_BELOW_REINCARNATION;
		if (ulCount > m_ulPremReincarnationMax) return TASK_PREREQU_FAIL_ABOVE_REINCARNATION;
	}
	return 0;
}
inline unsigned long ATaskTempl::CheckRealmLevel(TaskInterface* pTask) const
{
	if (m_bPremCheckRealmLevel) {
		unsigned long ulLevel = pTask->GetRealmLevel();
		if (ulLevel < m_ulPremRealmLevelMin) return TASK_PREREQU_FAIL_BELOW_REALMLEVEL;
		if (ulLevel > m_ulPremRealmLevelMax) return TASK_PREREQU_FAIL_ABOVE_REALMLEVEL;
	}
	return 0;
}
inline unsigned long ATaskTempl::CheckRealmExpFull(TaskInterface* pTask) const
{
	if (m_bPremCheckRealmExpFull) {
		if (!pTask->IsRealmExpFull()) return TASK_PREREQU_FAIL_REALM_EXP_FULL;
	}
	return 0;
}
inline unsigned long ATaskTempl::CheckRepu(TaskInterface* pTask) const
{
	if (m_lPremise_Reputation && pTask->GetReputation() < m_lPremise_Reputation) return TASK_PREREQU_FAIL_BELOW_REPU;
	if (m_lPremise_RepuMax && pTask->GetReputation() > m_lPremise_RepuMax)  return TASK_PREREQU_FAIL_BELOW_REPU;
	return 0;
}

inline unsigned long ATaskTempl::CheckDeposit(TaskInterface* pTask) const
{
	if (m_ulPremise_Deposit && pTask->GetGoldNum() < m_ulPremise_Deposit) return TASK_PREREQU_FAIL_NO_DEPOSIT;
	return 0;
}

inline unsigned long ATaskTempl::CheckItems(TaskInterface* pTask) const
{
	unsigned long i = 0;
	int ret = m_bPremItemsAnyOne ? TASK_PREREQU_FAIL_NO_ITEM : 0;

	for (; i < m_ulPremItems; i++)
	{
		const ITEM_WANTED& wi = m_PremItems[i];

		if (m_bPremItemsAnyOne)
		{
			if (_get_item_count(pTask, wi.m_ulItemTemplId, wi.m_bCommonItem) >= wi.m_ulItemNum)
			{
				ret = 0;
				break;
			}
		}
		else
		{
			if (_get_item_count(pTask, wi.m_ulItemTemplId, wi.m_bCommonItem) < wi.m_ulItemNum)
			{
				ret = TASK_PREREQU_FAIL_NO_ITEM;
				break;
			}
		}
	}

	return ret;
}

inline unsigned long ATaskTempl::CheckFaction(TaskInterface* pTask) const
{
	int iRole = pTask->GetFactionRole();

	bool bFactionRoleSatisfied = iRole <= m_iPremise_FactionRole;
	
	if (m_ulPremise_Faction && !(pTask->IsInFaction(m_ulPremise_Faction) && bFactionRoleSatisfied)) return TASK_PREREQU_FAIL_CLAN;
	
	return 0;
}

inline unsigned long ATaskTempl::CheckGender(TaskInterface* pTask) const
{
	bool bMale = pTask->IsMale();

	if (m_ulGender == TASK_GENDER_MALE && !bMale
	 || m_ulGender == TASK_GENDER_FEMALE && bMale)
		return TASK_PREREQU_FAIL_WRONG_GENDER;

	return 0;
}

inline unsigned long ATaskTempl::CheckOccupation(TaskInterface* pTask) const
{
	if (!m_ulOccupations) return 0;

	unsigned long i;
	unsigned long ulOccup = pTask->GetPlayerOccupation();

	for (i = 0; i < m_ulOccupations; i++)
		if (m_Occupations[i] == ulOccup)
			return 0;

	return TASK_PREREQU_FAIL_NOT_IN_OCCU;
}

inline unsigned long ATaskTempl::CheckPeriod(TaskInterface* pTask) const
{
	unsigned long cur = pTask->GetCurPeriod();

	// 当前修真级别大于或等于所需级别
	if (cur < m_ulPremise_Period) return TASK_PREREQU_FAIL_WRONG_PERIOD;

	// 0 - 19
	if (m_ulPremise_Period < 20)
		return 0;

	// 20 - 29
	if (m_ulPremise_Period < 30)
		return cur < 30 ? 0 : TASK_PREREQU_FAIL_WRONG_PERIOD;

	// 30 - 39
	if (m_ulPremise_Period < 40)
		return cur < 40 ? 0 : TASK_PREREQU_FAIL_WRONG_PERIOD;

	return TASK_PREREQU_FAIL_WRONG_PERIOD;
}

inline unsigned long ATaskTempl::CheckGM(TaskInterface* pTask) const
{
	return m_bGM ? (pTask->IsGM() ? 0 : TASK_PREREQU_FAIL_GM) : 0;
}

inline unsigned long ATaskTempl::CheckShieldUser(TaskInterface* pTask) const
{
	return m_bShieldUser ? (pTask->IsShieldUser() ? 0 : TASK_PREREQU_FAIL_SHIELD_USER) : 0;
}

inline unsigned long ATaskTempl::CheckSpouse(TaskInterface* pTask) const
{
	if (m_bPremise_Spouse && !pTask->IsMarried())
		return TASK_PREREQU_FAIL_INDETERMINATE;

	return 0;
}

inline unsigned long ATaskTempl::CheckWeddingOwner(TaskInterface* pTask) const
{
	if (m_bPremiseWeddingOwner)
	{
		if (pTask->IsAtCrossServer())
		{
			return TASK_PREREQU_FAIL_CROSSSERVER_NO_MARRIAGE;
		}
		if (!pTask->IsWeddingOwner())
		{
			return TASK_PREREQU_FAIL_WEDDING_OWNER;
		}
	}
	return 0;
}

inline unsigned long ATaskTempl::CheckPreTask(TaskInterface* pTask) const
{
	unsigned long i;
	FinishedTaskList* pFinished = (FinishedTaskList*)pTask->GetFinishedTaskList();

	unsigned long iPremTaskFinishedCount = 0;
	for (i = 0; i < m_ulPremise_Task_Count; i++)
	{
		if (!m_ulPremise_Task_Least_Num)
		{
			if (pFinished->SearchTask(m_ulPremise_Tasks[i]) != 0)
				return TASK_PREREQU_FAIL_PREV_TASK;
		}
		else if (pFinished->SearchTask(m_ulPremise_Tasks[i]) == 0)
		{
			iPremTaskFinishedCount++;
		}
	}

	return m_ulPremise_Task_Least_Num ? (iPremTaskFinishedCount < m_ulPremise_Task_Least_Num ? TASK_PREREQU_FAIL_PREV_TASK : 0) : 0;
}

extern unsigned long _living_skill_ids[];

inline unsigned long ATaskTempl::CheckLivingSkill(TaskInterface* pTask) const
{
	unsigned long i;

	for (i = 0; i < MAX_LIVING_SKILLS; i++)
	{
		if (m_lSkillLev[i] == 0)
			continue;

		if (!pTask->HasLivingSkill(_living_skill_ids[i])
		  || pTask->GetLivingSkillLevel(_living_skill_ids[i]) < m_lSkillLev[i])
			return TASK_PREREQU_FAIL_LIVING_SKILL;
	}

	return 0;
}

inline bool _compare_key_value(TaskInterface* pTask, const COMPARE_KEY_VALUE& CompKeyVal)
{
	long lleftValue = CompKeyVal.lLeftNum;
	if (CompKeyVal.nLeftType == 0)
	{
		lleftValue = pTask->GetGlobalValue(CompKeyVal.lLeftNum);
	}

	long lRightValue = CompKeyVal.lRightNum;
	if (CompKeyVal.nRightType == 0)
	{
		lRightValue = pTask->GetGlobalValue(CompKeyVal.lRightNum);
	}

	switch(CompKeyVal.nCompOper)
	{
	case 0:
		{
			if (lleftValue > lRightValue)
				return true;
		}
		break;
	case 1:
		{
			if (lleftValue == lRightValue)
				return true;
		}
	    break;
	case 2:
		{
			if (lleftValue < lRightValue)
				return true;
		}
		break;
	default:
	    break;
	}

	return false;
}

inline unsigned long ATaskTempl::CheckGlobalKeyValue(TaskInterface* pTask, bool bFinCheck) const
{
	if (bFinCheck && !m_bFinNeedComp
	 ||!bFinCheck && !m_bPremNeedComp)
		return 0;

	
	bool bFlag1 = false;
	bool bFlag2 = false;

	if (bFinCheck)
	{
		bFlag1 = _compare_key_value(pTask, m_Fin1KeyValue);
		bFlag2 = _compare_key_value(pTask, m_Fin2KeyValue);	

		if (m_nFinExp1AndOrExp2 == 0 && (bFlag1 || bFlag2)  //或
		 || m_nFinExp1AndOrExp2 == 1 && (bFlag1 && bFlag2)) //与
			return 0;
	}
	else
	{		
		bFlag1 = _compare_key_value(pTask, m_Prem1KeyValue);
		bFlag2 = _compare_key_value(pTask, m_Prem2KeyValue);

		if (m_nPremExp1AndOrExp2 == 0 && (bFlag1 || bFlag2)  //或
		 || m_nPremExp1AndOrExp2 == 1 && (bFlag1 && bFlag2)) //与
			return 0;
	}		

	return TASK_PREREQU_FAIL_GLOBAL_KEYVAL;	
}

inline bool _compare_pq_key_value(const COMPARE_KEY_VALUE& CompKeyVal)
{
	long lleftValue = CompKeyVal.lLeftNum;
	if (CompKeyVal.nLeftType == 0)
	{
		lleftValue = PublicQuestInterface::QuestGetGlobalValue(CompKeyVal.lLeftNum);
//			pTask->GetGlobalValue(CompKeyVal.lLeftNum);
	}

	long lRightValue = CompKeyVal.lRightNum;
	if (CompKeyVal.nRightType == 0)
	{
		lRightValue = PublicQuestInterface::QuestGetGlobalValue(CompKeyVal.lRightNum);
//			pTask->GetGlobalValue(CompKeyVal.lRightNum);
	}

	switch(CompKeyVal.nCompOper)
	{
	case 0:
		{
			if (lleftValue > lRightValue)
				return true;
		}
		break;
	case 1:
		{
			if (lleftValue == lRightValue)
				return true;
		}
	    break;
	case 2:
		{
			if (lleftValue < lRightValue)
				return true;
		}
		break;
	default:
	    break;
	}

	return false;
}

inline unsigned long ATaskTempl::CheckGlobalPQKeyValue(bool bFinCheck) const
{
	if (bFinCheck && !m_bFinNeedComp
	 ||!bFinCheck && !m_bPremNeedComp)
		return 0;

	
	bool bFlag1 = false;
	bool bFlag2 = false;

	if (bFinCheck)
	{
		bFlag1 = _compare_pq_key_value(m_Fin1KeyValue);
		bFlag2 = _compare_pq_key_value(m_Fin2KeyValue);	

		if (m_nFinExp1AndOrExp2 == 0 && (bFlag1 || bFlag2)  //或
		 || m_nFinExp1AndOrExp2 == 1 && (bFlag1 && bFlag2)) //与
			return 0;
	}
	else
	{		
		bFlag1 = _compare_pq_key_value(m_Prem1KeyValue);
		bFlag2 = _compare_pq_key_value(m_Prem2KeyValue);

		if (m_nPremExp1AndOrExp2 == 0 && (bFlag1 || bFlag2)  //或
		 || m_nPremExp1AndOrExp2 == 1 && (bFlag1 && bFlag2)) //与
			return 0;
	}		

	return TASK_PREREQU_FAIL_GLOBAL_KEYVAL;	
}

inline unsigned long ATaskTempl::CheckDeliverCount(TaskInterface* pTask) const
{
	if (m_bAccountTaskLimit && m_lPeriodLimit)
	{
		if (pTask->IsAtCrossServer())
			return TASK_PREREQU_FAIL_CROSSSERVER_NO_ACOUNT_LIMIT;

		TaskFinishCountList* pFnshList = (TaskFinishCountList*)pTask->GetFinishedCntList();
		unsigned long ulTemp;
		unsigned long nRet = pFnshList->Search(m_ID,ulTemp);

		if (nRet >= static_cast<unsigned long>(m_lPeriodLimit))
			return TASK_PREREQU_FAIL_MAX_ACC_CNT;
	}
	else if (m_bRoleTaskLimit && m_lPeriodLimit)
	{
		FinishedTaskList* pFnshList = (FinishedTaskList*)pTask->GetFinishedTaskList();
		long finish_count = pFnshList->SearchTaskFinishCount(m_ID);
		if (finish_count >= m_lPeriodLimit)
			return TASK_PREREQU_FAIL_MAX_ROLE_CNT;
	}
	return 0;
}

inline unsigned long ATaskTempl::CheckAccountRMB(TaskInterface* pTask) const
{
	if(m_ulPremRMBMin && m_ulPremRMBMax)
	{
		unsigned long  ulTotalRMB = pTask->GetAccountTotalCash();
		if(ulTotalRMB < m_ulPremRMBMin || ulTotalRMB > m_ulPremRMBMax)
			return TASK_PREREQU_FAIL_RMB_NOT_ENOUGH;
	}

	return 0;
}

inline unsigned long ATaskTempl::CheckCharTime(TaskInterface* pTask) const
{
	if(m_bCharTime)
	{
		int iStartTime, iEndTime;

		// 开始时间
		if(m_iCharStartTime == 0)
			iStartTime = pTask->GetRoleCreateTime();
		else
			iStartTime = pTask->GetRoleLastLoginTime();

		// 结束时间
		if(m_iCharEndTime == 0)
			iEndTime = pTask->GetCurTime();
		else
		{
			tm tmEnd;
			memset(&tmEnd, 0, sizeof(tm));
			tmEnd.tm_year	= m_tmCharEndTime.year - 1900;
			tmEnd.tm_mon	= m_tmCharEndTime.month - 1;
			tmEnd.tm_mday	= m_tmCharEndTime.day;

			iEndTime = mktime(&tmEnd);
		}

		if(iEndTime - iStartTime < (int)(m_ulCharTimeGreaterThan * 3600 * 24))
			return TASK_PREREQU_FAIL_ERR_CHAR_TIME;
	}

	return 0;
}

inline unsigned long ATaskTempl::CheckTransform(TaskInterface* pTask) const
{
	unsigned char playerShapeType = pTask->GetShapeMask();

	// 不要求变身
	if (m_ucPremiseTransformedForm == 0xFF)
	{
		return 0;
	}
	// 任意非职业变身
	else if (m_ucPremiseTransformedForm == 0x80)
	{
		if ((playerShapeType >> 6) != 2)
		{
			return TASK_PREREQU_FAIL_TRANSFORM_MASK;
		}
		else
		{
			return 0;
		}
		
	}
	// 特定变身
	else if (m_ucPremiseTransformedForm != playerShapeType)
	{
		return TASK_PREREQU_FAIL_TRANSFORM_MASK;
	}

	return 0;
}

inline unsigned long ATaskTempl::CheckForce(TaskInterface* pTask) const
{
	if (m_bPremCheckForce)
	{
		if (pTask->IsAtCrossServer())
		{
			return TASK_PREREQU_FAIL_CROSSSERVER_NO_FORCE;
		}
		if (m_iPremForce == -1)
		{
			if (pTask->GetForce() == 0)
				return TASK_PREREQU_FAIL_FORCE;
		}
		else if (pTask->GetForce() != m_iPremForce)
			return TASK_PREREQU_FAIL_FORCE;
	}
	return 0;
}

inline unsigned long ATaskTempl::CheckForceReputation(TaskInterface* pTask) const
{
	if (m_iPremForceReputation)
	{
		if (pTask->IsAtCrossServer())
			return TASK_PREREQU_FAIL_CROSSSERVER_NO_FORCE;
		if (pTask->GetForceReputation() < m_iPremForceReputation)
			return TASK_PREREQU_FAIL_FORCE_REPUTATION;	
	}
	return 0;
}

inline unsigned long ATaskTempl::CheckForceContribution(TaskInterface* pTask) const
{
	if (m_iPremForceContribution)
	{
		if (pTask->IsAtCrossServer())
			return TASK_PREREQU_FAIL_CROSSSERVER_NO_FORCE;
		if (pTask->GetForceContribution() < m_iPremForceContribution)
			return TASK_PREREQU_FAIL_FORCE_REPUTATION;
	}
	return 0;
}

inline unsigned long ATaskTempl::CheckExp(TaskInterface* pTask) const
{
	if (m_iPremForceExp)
	{
		if (pTask->IsAtCrossServer())
			return TASK_PREREQU_FAIL_CROSSSERVER_NO_FORCE;
		if (pTask->GetExp() < m_iPremForceExp)
			return TASK_PREREQU_FAIL_EXP;
	}
	return 0;
}

inline unsigned long ATaskTempl::CheckSP(TaskInterface* pTask) const
{
	if (m_iPremForceSP)
	{
		if (pTask->IsAtCrossServer())
			return TASK_PREREQU_FAIL_CROSSSERVER_NO_FORCE;
		if (pTask->GetSP() < m_iPremForceSP)
			return TASK_PREREQU_FAIL_SP;
	}
	return 0;
}

inline unsigned long ATaskTempl::CheckForceActivityLevel(TaskInterface* pTask) const
{
	if (m_iPremForceActivityLevel != -1)
	{
		if (pTask->IsAtCrossServer())
			return TASK_PREREQU_FAIL_CROSSSERVER_NO_FORCE;
		if (m_iPremForceActivityLevel != pTask->GetForceActivityLevel())
			return TASK_PREREQU_FAIL_FORCE_AL;
	}
	return 0;
}

inline unsigned long ATaskTempl::CheckKing(TaskInterface* pTask) const
{
	if (m_bPremIsKing && !pTask->IsKing())
	{
		return TASK_PREREQU_FAIL_KING;
	}
	return 0;
}

inline unsigned long ATaskTempl::CheckNotInTeam(TaskInterface* pTask) const
{
	if (m_bPremNotInTeam && pTask->IsInTeam())
	{
		return TASK_PREREQU_FAIL_IN_TEAM;
	}
	return 0;
}

inline unsigned long ATaskTempl::CheckTitle(TaskInterface* pTask) const
{
	if (m_iPremTitleNumTotal) {
		int iNumRequired(m_iPremTitleNumTotal);
		if (m_iPremTitleNumRequired > 0 && m_iPremTitleNumRequired < m_iPremTitleNumTotal) 
			iNumRequired = m_iPremTitleNumRequired;
		int iTitleCount(0);
		for (unsigned long i = 0; i < m_iPremTitleNumTotal; ++i) {
			if (pTask->HaveGotTitle(m_PremTitles[i]))
				iTitleCount++;
		}
		if (iTitleCount < iNumRequired)
			return TASK_PREREQU_FAIL_TITLE;
	}
	if (m_Award_S->m_ulTitleNum) {
		for (unsigned long i = 0; i < m_Award_S->m_ulTitleNum; ++i) {
			if (pTask->HaveGotTitle(m_Award_S->m_pTitleAward[i].m_ulTitleID))
				return TASK_PREREQU_FAIL_TITLE;
		}
	}
	if (m_Award_F->m_ulTitleNum) {
		for (unsigned long i = 0; i < m_Award_F->m_ulTitleNum; ++i) {
			if (pTask->HaveGotTitle(m_Award_F->m_pTitleAward[i].m_ulTitleID))
				return TASK_PREREQU_FAIL_TITLE;
		}
	}
	return 0;
}
inline unsigned long ATaskTempl::CheckHistoryStage(TaskInterface* pTask) const
{
	int index = pTask->GetCurHistoryStageIndex();
	
	if (m_iPremHistoryStageIndex[0] && (index <=0 || index < m_iPremHistoryStageIndex[0])) return TASK_PREREQU_FAIL_HISTORYSTAGE;
	if (m_iPremHistoryStageIndex[1] && (index <=0 || index > m_iPremHistoryStageIndex[1])) return TASK_PREREQU_FAIL_HISTORYSTAGE;
	
	return 0;
}
inline unsigned long ATaskTempl::CheckCardCollection(TaskInterface* pTask) const
{
	unsigned int count = pTask->GetObtainedGeneralCardCount();
	
	if (m_ulPremGeneralCardCount && (count < m_ulPremGeneralCardCount)) return TASK_PREREQU_FAIL_CARD_COUNT_COLLECTION;
	
	return 0;
}
inline unsigned long ATaskTempl::CheckCardRankCount(TaskInterface* pTask) const
{
	if (m_iPremGeneralCardRank >= 0 && m_ulPremGeneralCardRankCount) {
		unsigned int count = pTask->GetObtainedGeneralCardCountByRank(m_iPremGeneralCardRank);
		if (count < m_ulPremGeneralCardRankCount) return TASK_PREREQU_FAIL_CARD_COUNT_RANK;
	}
	return 0;
}
inline unsigned long ATaskTempl::CheckInTransformShape(TaskInterface* pTask) const
{
	if (m_enumMethod == enumTMSimpleClientTaskForceNavi && pTask->GetShapeMask() != 0 ){
		return TASK_PREREQU_FAIL_NO_NAVIGATE_INSHPAED;
	}
	return 0;
}
inline bool ATaskTempl::CheckReachLevel(TaskInterface* pTask) const
{
	bool bLevel(true), bReincarnationCount(true), bRealmLevel(true); 
	if (m_ulReachLevel) bLevel = pTask->GetPlayerLevel() >= m_ulReachLevel;
	if (m_ulReachReincarnationCount) bReincarnationCount = pTask->GetReincarnationCount() >= m_ulReachReincarnationCount;
	if (m_ulReachRealmLevel) bRealmLevel = pTask->GetRealmLevel() >= m_ulReachRealmLevel;
	return bLevel && bReincarnationCount && bRealmLevel;
}
inline void ATaskTempl::CalcAwardItemsCount(
	TaskInterface* pTask,
	const AWARD_ITEMS_CAND* pAward,
	unsigned long& ulCmnCount,
	unsigned long& ulTskCount) const
{
	if (pAward->m_bRandChoose)
	{
		for (unsigned long i = 0; i < pAward->m_ulAwardItems; i++)
		{
			const ITEM_WANTED& iw = pAward->m_AwardItems[i];

			if (iw.m_fProb < 1.0f) continue;
			if (iw.m_bCommonItem) ulCmnCount++;
			else ulTskCount++;
		}

		ulCmnCount++;
		ulTskCount++;
	}
	else
	{
		ulCmnCount += pAward->m_ulAwardCmnItems;
		ulTskCount += pAward->m_ulAwardTskItems;
	}
}

inline bool ATaskTempl::CanAwardItems(TaskInterface* pTask, const AWARD_ITEMS_CAND* pAward) const
{
	unsigned long ulCmnCount = 0;
	unsigned long ulTskCount = 0;

	CalcAwardItemsCount(pTask, pAward, ulCmnCount, ulTskCount);
	return pTask->CanDeliverCommonItem(ulCmnCount) && pTask->CanDeliverTaskItem(ulTskCount);
}

inline void ATaskTempl::CalcAwardData(
	TaskInterface* pTask,
	AWARD_DATA* pAward,
	ActiveTaskEntry* pEntry,
	unsigned long ulTaskTime,
	unsigned long ulCurTime) const
{
	memset(pAward, 0, sizeof(AWARD_DATA));
	unsigned ulType = (pEntry->IsSuccess() ? m_ulAwardType_S : m_ulAwardType_F);

	switch (ulType)
	{
	case enumTATNormal:
	case enumTATEach:
		*pAward = (pEntry->IsSuccess() ? *m_Award_S : *m_Award_F);
		break;
	case enumTATRatio:
		CalcAwardDataByRatio(pAward, pEntry, ulTaskTime, ulCurTime);
		break;
	case enumTATItemCount:
		CalcAwardDataByItems(pTask, pAward, pEntry);
		break;
	default:
		assert(false);
		break;
	}
}

inline void ATaskTempl::CalcAwardDataByRatio(
	AWARD_DATA* pAward,
	ActiveTaskEntry* pEntry,
	unsigned long ulTaskTime,
	unsigned long ulCurTime) const
{
	if (!m_ulTimeLimit) return;

	const AWARD_RATIO_SCALE* p = (pEntry->IsSuccess() ? m_AwByRatio_S : m_AwByRatio_F);
	float ratio = (float)(ulCurTime - ulTaskTime) / m_ulTimeLimit;
	unsigned long i;

	for (i = 0; i < p->m_ulScales; i++)
	{
		if (ratio <= p->m_Ratios[i])
		{
			*pAward = p->m_Awards[i];
			return;
		}
	}
}

inline void ATaskTempl::CalcAwardDataByItems(
	TaskInterface* pTask,
	AWARD_DATA* pAward,
	ActiveTaskEntry* pEntry) const
{
	const AWARD_ITEMS_SCALE* p = (pEntry->IsSuccess() ? m_AwByItems_S : m_AwByItems_F);
	unsigned long ulCount = pTask->GetTaskItemCount(p->m_ulItemId), i;

	for (i = 0; i < p->m_ulScales; i++)
	{
		if (ulCount >= p->m_Counts[i])
		{
			*pAward = p->m_Awards[i];
			return;
		}
	}
}

inline unsigned long ATaskTempl::CalcAwardMulti(
	TaskInterface* pTask,
	ActiveTaskEntry* pEntry,
	unsigned long ulTaskTime,
	unsigned long ulCurTime) const
{
	unsigned ulType = (pEntry->IsSuccess() ? m_ulAwardType_S : m_ulAwardType_F);
	if (ulType != enumTATEach) return 1;

	if (m_enumMethod == enumTMCollectNumArticle)
	{
		unsigned long ulCount = 0, i;

		// 计算收集的物品数量
		for (i = 0; i < m_ulItemsWanted; i++)
		{
			ulCount += _get_item_count(
				pTask,
				m_ItemsWanted[i].m_ulItemTemplId,
				m_ItemsWanted[i].m_bCommonItem);
		}

		return ulCount;
	}
	else if (m_enumMethod == enumTMKillNumMonster)
	{
		unsigned long ulCount = 0;

		for (unsigned long i = 0; i < m_ulMonsterWanted; i++)
		{
			const MONSTER_WANTED& mw = m_MonsterWanted[i];

			if (mw.m_ulDropItemId)
				ulCount += _get_item_count(pTask, mw.m_ulDropItemId, mw.m_bDropCmnItem);
			else
				ulCount += pEntry->m_wMonsterNum[i];
		}

		return ulCount;
	}

	return 0;
}

#ifdef _TASK_CLIENT

inline void _notify_svr(TaskInterface* pTask, unsigned char uReason, unsigned short uTaskID)
{
	task_notify_base notify;
	notify.reason = uReason;
	notify.task = uTaskID;
	pTask->NotifyServer(&notify, sizeof(notify));
}
inline void _notify_svr_choose_award(TaskInterface* pTask, unsigned short uTaskID, unsigned char nChoice)
{
	task_notify_choose_award notify;
	notify.reason = TASK_CLT_NOTIFY_CHOOSE_AWARD;
	notify.task = uTaskID;
	notify.choice = nChoice;
	pTask->NotifyServer(&notify, sizeof(notify));
}

inline void _notify_svr_finishtask_by_contribution(TaskInterface* pTask, unsigned short uTaskID, unsigned char nChoice)
{
	task_notify_choose_award notify;
	notify.reason = TASK_CLT_NOTIFY_FINISH_TASK_BY_WORLD_CONTRIBUTION;
	notify.task = uTaskID;
	notify.choice = nChoice;
	pTask->NotifyServer(&notify, sizeof(notify));
}

inline void ATaskTempl::SyncTaskType()
{
	const ATaskTempl* pTop = GetTopTask();
	if (pTop && pTop != this)
		m_ulType = pTop->m_ulType;
}

inline bool ATaskTempl::HasShowCond() const
{
	if (!m_bShowByDeposit
	|| !m_bShowByGender
	|| !m_bShowByItems
	|| !m_bShowByFactionContrib
	|| !m_bShowByTransformed
	|| !m_bShowByForceReputation
	|| !m_bShowByFactionContrib
	|| !m_bShowByForceExp
	|| !m_bShowByForceSP
	|| !m_bShowByForceActivityLevel
	|| !m_bShowByForce
	|| !m_bShowByNeedRecordTasksNum
	|| !m_bShowByLev
	|| !m_bShowByOccup
	|| !m_bShowByPreTask
	|| !m_bShowByRepu
	|| !m_bShowByTeam
	|| !m_bShowByFaction
	|| !m_bShowByPeriod
	|| !m_bShowByCharTime
	|| !m_bShowByRMB
	|| !m_bShowByWeddingOwner
	|| !m_bShowByKing
	|| !m_bShowByNotInTeam
	|| !m_bShowByGeneralCard)
		return true;
	return false;
}

inline bool ATaskTempl::CanShowTask(TaskInterface* pTask) const
{
	unsigned long ulCurTime = TaskInterface::GetCurTime();

	if (m_pParent
	 || static_cast<ActiveTaskList*>(pTask->GetActiveTaskList())->GetEntry(GetID())
	 || CheckTimetable(ulCurTime)
	 || CheckDeliverTime(pTask, ulCurTime)
	 || CheckFnshLst(pTask, ulCurTime)
	 || CheckMutexTask(pTask, ulCurTime)
	 || CheckLivingSkill(pTask)
	 || CheckSpecialAward(pTask)
	 || CheckGlobalKeyValue(pTask))
		return false;

	unsigned long ulRet;

	if (m_bShowByDeposit && CheckDeposit(pTask)) return false;
	if (m_bShowByGender && CheckGender(pTask)) return false;
	if (m_bShowByItems && CheckItems(pTask)) return false;
	ulRet = CheckLevel(pTask);
	if (ulRet == TASK_PREREQU_FAIL_ABOVE_LEVEL || (m_bShowByLev && ulRet)) return false;
	if (m_bShowByTransformed && CheckTransform(pTask)) return false;
	if (m_bShowByForce && CheckForce(pTask)) return false;
	if (m_bShowByForceActivityLevel && CheckForceActivityLevel(pTask)) return false;
	if (m_bShowByForceExp && CheckExp(pTask)) return false;
	if (m_bShowByForceSP && CheckSP(pTask)) return false;
	if (m_bShowByForceContribution && CheckForceContribution(pTask)) return false;
	if (m_bShowByForceReputation && CheckForceReputation(pTask)) return false;
	if (m_bShowByNeedRecordTasksNum && CheckRecordTasksNum(pTask)) return false;
	if (m_bShowByFactionContrib && CheckFactionContrib(pTask)) return false;
	if (m_bShowByOccup && CheckOccupation(pTask)) return false;
	if (m_bShowByPreTask && CheckPreTask(pTask)) return false;
	if (m_bShowByRepu && CheckRepu(pTask)) return false;
	if (m_bShowByTeam && CheckTeamTask(pTask)) return false;
	if (m_bShowByFaction && CheckFaction(pTask)) return false;
	if (m_bShowByPeriod && CheckPeriod(pTask)) return false;
	if (m_bShowByCharTime && CheckCharTime(pTask)) return false;
	if (m_bShowByRMB && CheckAccountRMB(pTask)) return false;
	if (m_bShowByWeddingOwner && CheckWeddingOwner(pTask)) return false;
	if (m_bShowByKing && CheckKing(pTask)) return false;
	if (m_bShowByNotInTeam && CheckNotInTeam(pTask)) return false;
	if (m_bShowByReincarnation && CheckReincarnation(pTask)) return false;
	if (m_bShowByRealmLevel && (CheckRealmLevel(pTask) || CheckRealmExpFull(pTask))) return false;
	if (m_bShowByGeneralCard && CheckCardCollection(pTask)) return false;
	if (m_bShowByGeneralCardRank && CheckCardRankCount(pTask)) return false;
	if (m_bShowByHistoryStage && CheckHistoryStage(pTask)) return false;

	return true;
}

inline bool ATaskTempl::CanShowInExclusiveUI(TaskInterface* pTask, unsigned long ulCurTime) const
{
	bool ret(false);
	while (true) {
		// 任务中有时间限制，且当前服务器时间不在范围内，则不做展示
		if (CheckTimetable(ulCurTime)) break;
		// 任务有最高等级要求
		if (m_ulPremise_Lev_Max) {
			unsigned long ulLevel = m_bPremCheckMaxHistoryLevel ? pTask->GetMaxHistoryLevel() : pTask->GetPlayerLevel();
			if (ulLevel > m_ulPremise_Lev_Max) break;
		}
		// 任务有最高境界等级要求
		if (m_bPremCheckRealmLevel && m_ulPremRealmLevelMax) 
			if (pTask->GetRealmLevel() > m_ulPremRealmLevelMax) break;
		// 不可重复完成
		if (CheckFnshLst(pTask, ulCurTime)) break;
		// 发放次数
		if (CheckDeliverTime(pTask, ulCurTime)) break;
		// 前提任务
		if (CheckPreTask(pTask)) break;
		// 任务前提中有物品要求
		if (m_ulPremItems > 0) break;
		// 任务前提中检查转生次数
		if (m_bPremCheckReincarnation && pTask->GetReincarnationCount() > m_ulPremReincarnationMax){
			break;
		}
		ret = true;
		break;
	}
	return ret;
}


inline void ATaskTempl::GetGlobalTaskChar(TaskInterface* pTask, abase::vector<wchar_t*>& TaskCharArr) const
{
	for (int i=0; i<(int)m_ulTaskCharCnt; i++)
	{		
		wchar_t *pszNewchar = new wchar_t[TASK_AWARD_MAX_DISPLAY_CHAR_LEN];
		memset(pszNewchar, 0, sizeof(wchar_t)*TASK_AWARD_MAX_DISPLAY_CHAR_LEN);

		wchar_t *pSrc  = (wchar_t*)m_pTaskChar[i]; 
		wchar_t *pDest = NULL;	
		wchar_t pChar[2] = {0x0025, 0x0000}; //符号%
		int nPos = 0;

		do 
		{	
			pDest = wcsstr(pSrc, pChar);
			if (pDest)
			{
				nPos = (int)(pDest - pSrc);
				wcsncat(pszNewchar, pSrc, nPos);
				
				pDest++;
				int nVal = _wtoi(pDest);
				if (nVal) //获取引用的全局表达式行号
				{
					//计算全局表达式
					int nRet = (int)CalcOneGlobalExp(pTask, nVal-1);

					//转换成字符放入新字符串
					wchar_t szRet[10] = {0};
					_itow(nRet, szRet, 10);
					wcsncat(pszNewchar, szRet, 10);

					while(iswdigit(*pDest))
					{
						pDest++;
					}	
				}
				else
				{
					wcsncat(pszNewchar, pChar, 1);
				}
			}
			else	// 未找到全局变量表达式，则直接显示描述文字
			{	
				wcsncat(pszNewchar, pSrc, wcslen(pSrc));
			}

			pSrc = pDest;
			
		} while(pSrc != NULL);
		
		if (wcslen(pszNewchar))
		{			
			TaskCharArr.push_back(pszNewchar);
		}
		else
		{
			delete pszNewchar;
		}
	}
}

inline float ATaskTempl::CalcOneGlobalExp(TaskInterface*pTask, int nIndex) const
{
	TaskExpAnalyser Analyser;
	TaskExpressionArr ExpArrMulti;
	
	if (nIndex < 0 || nIndex >= (int)m_ulExpCnt)
	{
		return 0;
	}

	TASK_EXPRESSION *pExpArr = (TASK_EXPRESSION *)(m_pExpArr[nIndex]);	

	int i=0;
	while (pExpArr[i].type != -1)
	{
		ExpArrMulti.push_back(pExpArr[i]);
		i++;
	}
	
	float fRetValue = 0.0f;
	try
	{
		fRetValue = Analyser.Run(pTask, ExpArrMulti);
	}
	catch (char* szErr)
	{
		char log[1024];
		sprintf(log, "CalcOneGlobalExp, Expression run err: %s", szErr);
		TaskInterface::WriteLog(pTask->GetPlayerId(), m_ID, 0, log);
		
		return 0;
	}

	return fRetValue;
}

#else // Linux Part

inline const ATaskTempl* ATaskTempl::RandOneChild(TaskInterface* pTask, int& nSub) const
{
	if (!m_nSubCount)
		return NULL;

	nSub = static_cast<int>(pTask->UnitRand() * m_nSubCount);
	if (nSub >= m_nSubCount) nSub = m_nSubCount - 1;
	return GetSubByIndex(nSub);
}

inline void ATaskTempl::DeliverGivenItems(TaskInterface* pTask) const
{
	if (CheckGivenItems(pTask) != 0) return;
	unsigned long i;

	for (i = 0; i < m_ulGivenItems; i++)
	{
		const ITEM_WANTED& iw = m_GivenItems[i];

		if (iw.m_bCommonItem) pTask->DeliverCommonItem(iw.m_ulItemTemplId, iw.m_ulItemNum);
		else pTask->DeliverTaskItem(iw.m_ulItemTemplId, iw.m_ulItemNum);
	}
}

inline void ATaskTempl::TakeAwayGivenItems(TaskInterface* pTask) const
{
	unsigned long i, ulCount;

	for (i = 0; i < m_ulGivenItems; i++)
	{
		const ITEM_WANTED& wi = m_GivenItems[i];

		if (wi.m_bCommonItem)
		{
			ulCount = pTask->GetCommonItemCount(wi.m_ulItemTemplId);
			if (ulCount > wi.m_ulItemNum) ulCount = wi.m_ulItemNum;
			if (ulCount) pTask->TakeAwayCommonItem(wi.m_ulItemTemplId, ulCount);
		}
		else
		{
			ulCount = pTask->GetTaskItemCount(wi.m_ulItemTemplId);
			if (ulCount) pTask->TakeAwayTaskItem(wi.m_ulItemTemplId, ulCount);
		}
	}
}

inline void ATaskTempl::RemovePrerequisiteItem(TaskInterface* pTask) const
{
	unsigned long i;

	if (m_ulPremise_Deposit)
		pTask->TakeAwayGold(m_ulPremise_Deposit);

	if (m_bPremCheckForce)
	{
		if (m_iPremForceContribution)
			pTask->ChangeForceContribution(-m_iPremForceContribution);
		if (m_iPremForceExp)
			pTask->ReduceExp(m_iPremForceExp);
		if (m_iPremForceSP)
			pTask->ReduceSP(m_iPremForceSP);
	}


	for (i = 0; i < m_ulPremItems; i++)
	{
		const ITEM_WANTED& wi = m_PremItems[i];

		if (wi.m_ulItemTemplId && wi.m_ulItemNum)
		{
			if (m_bPremItemsAnyOne)
			{
				if (_get_item_count(pTask, wi.m_ulItemTemplId, wi.m_bCommonItem) >= wi.m_ulItemNum)
				{
					pTask->TakeAwayItem(wi.m_ulItemTemplId, wi.m_ulItemNum, wi.m_bCommonItem);
					break;
				}
			}
			else
				pTask->TakeAwayItem(wi.m_ulItemTemplId, wi.m_ulItemNum, wi.m_bCommonItem);
		}
	}
}

inline void ATaskTempl::RemoveAcquiredItem(TaskInterface* pTask, bool bClearTask, bool bSuccess) const
{
	if (m_enumMethod == enumTMCollectNumArticle)
	{
		for (unsigned char i = 0; i < m_ulItemsWanted; i++)
		{
			const ITEM_WANTED& wi = m_ItemsWanted[i];
			unsigned long ulCount;

			if (wi.m_bCommonItem)
			{
				if (bClearTask) continue;
				ulCount = pTask->GetCommonItemCount(wi.m_ulItemTemplId);
				if (!ulCount) continue;
				if (wi.m_ulItemNum && ulCount > wi.m_ulItemNum) ulCount = wi.m_ulItemNum;
				pTask->TakeAwayCommonItem(wi.m_ulItemTemplId, ulCount);
			}
			else
			{
				ulCount = pTask->GetTaskItemCount(wi.m_ulItemTemplId);
				if (ulCount) pTask->TakeAwayTaskItem(wi.m_ulItemTemplId, ulCount);
			}
		}

		// 成功后把钱收走
		if (m_ulGoldWanted && !bClearTask && bSuccess)
		{
			unsigned long ulGold = pTask->GetGoldNum();
			if (ulGold > m_ulGoldWanted) ulGold = m_ulGoldWanted;
			pTask->TakeAwayGold(ulGold);
		}

		//成功后把帮派贡献度收走
		if (m_iFactionContribWanted && !bClearTask && bSuccess)
		{
			int iContrib = pTask->GetFactionConsumeContrib();
			if (iContrib > m_iFactionContribWanted) iContrib = m_iFactionContribWanted;
			pTask->TakeAwayFactionConsumeContrib(iContrib);
		}

		//成功后把帮派经验收走
		if (m_iFactionExpContribWanted && !bClearTask && bSuccess)
		{
			int iContrib = pTask->GetFactionExpContrib();
			if (iContrib > m_iFactionExpContribWanted) iContrib = m_iFactionExpContribWanted;
			pTask->TakeAwayFactionExpContrib(iContrib);
		}
	}
	else if (m_enumMethod == enumTMKillNumMonster)
	{
		for (unsigned long i = 0; i < m_ulMonsterWanted; i++)
		{
			const MONSTER_WANTED& mw = m_MonsterWanted[i];
			if (!mw.m_ulDropItemId) continue;

			unsigned long ulCount;

			if (mw.m_bDropCmnItem)
			{
				ulCount = pTask->GetCommonItemCount(mw.m_ulDropItemId);
				if (mw.m_ulDropItemCount && ulCount > mw.m_ulDropItemCount) ulCount = mw.m_ulDropItemCount;
				if (ulCount) pTask->TakeAwayCommonItem(mw.m_ulDropItemId, ulCount);
			}
			else
			{
				ulCount = pTask->GetTaskItemCount(mw.m_ulDropItemId);
				if (ulCount) pTask->TakeAwayTaskItem(mw.m_ulDropItemId, ulCount);
			}
		}
	}
	else if (m_enumMethod == enumTMKillPlayer)
	{
		for (unsigned long i = 0; i < m_ulPlayerWanted; ++i)
		{
			const PLAYER_WANTED& pw = m_PlayerWanted[i];
			if (!pw.m_ulDropItemId) continue;
			
			unsigned long ulCount;
			if (pw.m_bDropCmnItem)
			{
				ulCount = pTask->GetCommonItemCount(pw.m_ulDropItemId);
				if (pw.m_ulDropItemCount && ulCount > pw.m_ulDropItemCount) ulCount = pw.m_ulDropItemCount;
				if (ulCount) pTask->TakeAwayCommonItem(pw.m_ulDropItemId, ulCount);
			}
			else
			{
				ulCount = pTask->GetTaskItemCount(pw.m_ulDropItemId);
				if (ulCount) pTask->TakeAwayTaskItem(pw.m_ulDropItemId, ulCount);
			}
		}
	}
}

inline void ATaskTempl::CheckMask()
{
	if (m_enumMethod == enumTMKillNumMonster)
		m_ulMask = TASK_MASK_KILL_MONSTER;
	else if (m_enumMethod == enumTMCollectNumArticle)
		m_ulMask = TASK_MASK_COLLECT_ITEM;
	else if (m_enumMethod == enumTMTalkToNPC)
		m_ulMask = TASK_MASK_TALK_TO_NPC;
	else if (m_enumMethod == enumTMKillPlayer)
		m_ulMask = TASK_MASK_KILL_PLAYER;
	
	if(m_bPQSubTask)
		m_ulMask = TASK_MASK_KILL_PQ_MONSTER;
}

inline bool ATaskTempl::CheckKillPlayer(
	TaskInterface* pTask, 
	ActiveTaskList* pList, 
	ActiveTaskEntry* pEntry, 
	int iOccupation, 
	int iLevel, 
	bool bGender, 
	int iForce,
	float fRand) const
{
	if (m_enumMethod != enumTMKillPlayer)
		return false;
	
	const ATaskTempl* pTempl = pEntry->GetTempl();
	bool bRet = false;
	for (unsigned long i = 0; i < m_ulPlayerWanted; ++i)
	{
		const PLAYER_WANTED& pw = m_PlayerWanted[i];
		if (!m_PlayerWanted[i].m_Requirements.CheckRequirements(iOccupation,iLevel,bGender,iForce))
			continue;
	
		if (pw.m_ulDropItemId)
		{
			if (!pTask->IsDeliverLegal())
				continue;
			
			unsigned long ulCount = _get_item_count(pTask, pw.m_ulDropItemId, pw.m_bDropCmnItem);
			
			if (pw.m_ulDropItemCount && ulCount >= pw.m_ulDropItemCount)
			{
				if (!pEntry->IsFinished() && HasAllPlayerKilled(pTask, pEntry))
				{
					OnSetFinished(pTask, pList, pEntry);
					return true;
				}
				
				continue;
			}
			
			bRet = true;
			
			if (pw.m_fDropProb < fRand)
			{
				fRand -= pw.m_fDropProb;
				continue;
			}
			
			if (pw.m_bDropCmnItem)
			{
				if (pTask->CanDeliverCommonItem(1))
					pTask->DeliverCommonItem(pw.m_ulDropItemId, 1);
			}
			else
			{
				if (pTask->CanDeliverTaskItem(1))
					pTask->DeliverTaskItem(pw.m_ulDropItemId, 1);
			}
			
			if (HasAllPlayerKilled(pTask, pEntry)) OnSetFinished(pTask, pList, pEntry);
			return true;
		}
		else
		{
			if (pw.m_ulPlayerNum && pEntry->m_wMonsterNum[i] >= pw.m_ulPlayerNum)
			{
				if (!pEntry->IsFinished() && HasAllPlayerKilled(pTask, pEntry))
				{
					OnSetFinished(pTask, pList, pEntry);
					return true;
				}
				
				continue;
			}
			
			pEntry->m_wMonsterNum[i]++;

			NotifyClient(pTask, pEntry, TASK_SVR_NOTIFY_PLAYER_KILLED, 0, i);
			
			if (HasAllPlayerKilled(pTask, pEntry)) OnSetFinished(pTask, pList, pEntry);
			return true;
		}
	}
	return bRet;
}

inline bool ATaskTempl::CheckKillMonster(
	TaskInterface* pTask,
	ActiveTaskList* pList,
	ActiveTaskEntry* pEntry,
	unsigned long ulTemplId,
	unsigned long ulLev,
	bool bTeam,
	float fRand,
	int dps,
	int dph) const
{
	if (m_enumMethod != enumTMKillNumMonster)
		return false;

	const ATaskTempl* pTempl = pEntry->GetTempl();
	if (bTeam != ((pTempl->m_bTeamwork || pTempl->GetTopTask()->m_bTeamwork) && pTask->IsInTeam()))
		return false;

	bool bRet = false;
	unsigned long ulPlayerLev = pTask->GetPlayerLevel();

	for (unsigned long i = 0; i < m_ulMonsterWanted; i++)
	{
		const MONSTER_WANTED& mw = m_MonsterWanted[i];

		if (mw.m_ulMonsterTemplId && mw.m_ulMonsterTemplId != ulTemplId)
			continue;

		if (mw.m_bKillerLev && ulPlayerLev > ulLev + MONSTER_PLAYER_LEVEL_MAX_DIFF)
			continue;

		if (mw.m_ulDropItemId)
		{
			if (!pTask->IsDeliverLegal())
				continue;

			unsigned long ulCount = _get_item_count(pTask, mw.m_ulDropItemId, mw.m_bDropCmnItem);

			if (mw.m_ulDropItemCount && ulCount >= mw.m_ulDropItemCount)
			{
				if (!pEntry->IsFinished() && HasAllMonsterWanted(pTask, pEntry))
				{
					OnSetFinished(pTask, pList, pEntry);
					return true;
				}

				continue;
			}

			bRet = true;

			if (mw.m_fDropProb < fRand)
			{
				fRand -= mw.m_fDropProb;
				continue;
			}

			if (mw.m_bDropCmnItem)
			{
				if (pTask->CanDeliverCommonItem(1))
					pTask->DeliverCommonItem(mw.m_ulDropItemId, 1);
			}
			else
			{
				if (pTask->CanDeliverTaskItem(1))
					pTask->DeliverTaskItem(mw.m_ulDropItemId, 1);
			}

			if (HasAllMonsterWanted(pTask, pEntry)) OnSetFinished(pTask, pList, pEntry);
			return true;
		}
		else
		{
			if (mw.m_ulMonsterNum && pEntry->m_wMonsterNum[i] >= mw.m_ulMonsterNum)
			{
				if (!pEntry->IsFinished() && HasAllMonsterWanted(pTask, pEntry))
				{
					OnSetFinished(pTask, pList, pEntry);
					return true;
				}

				continue;
			}

			pEntry->m_wMonsterNum[i]++;
			if (dph && dps)
			{
				NotifyClient(pTask, pEntry, TASK_SVR_NOTIFY_MONSTER_KILLED, 0, i, dps, dph);	
			}
			else
				NotifyClient(pTask, pEntry, TASK_SVR_NOTIFY_MONSTER_KILLED, 0, i);

			if (HasAllMonsterWanted(pTask, pEntry)) OnSetFinished(pTask, pList, pEntry);
			return true;
		}
	}

	return bRet;
}

inline void ATaskTempl::CheckCollectItem(
	TaskInterface* pTask,
	ActiveTaskList* pList,
	ActiveTaskEntry* pEntry,
	bool bAtNPC,
	int nChoice) const
{
	if (m_pFirstChild || !HasAllItemsWanted(pTask)) return;

	if (bAtNPC || m_enumFinishType == enumTFTDirect || m_enumFinishType == enumTFTConfirm)
	{
		pEntry->SetFinished();
		DeliverAward(pTask, pList, pEntry, nChoice);
	}
	else
		TaskInterface::WriteLog(pTask->GetPlayerId(), m_ID, 0, "CollectItem Must At NPC, Not Direct");
}

inline void ATaskTempl::CheckMonsterKilled(
	TaskInterface* pTask,
	ActiveTaskList* pList,
	ActiveTaskEntry* pEntry,
	bool bAtNPC,
	int nChoice) const
{
	if (!pEntry->IsFinished() || m_pFirstChild || !HasAllMonsterWanted(pTask, pEntry)) return;

	if (bAtNPC || m_enumFinishType == enumTFTDirect || m_enumFinishType == enumTFTConfirm)
		DeliverAward(pTask, pList, pEntry, nChoice);
	else
		TaskInterface::WriteLog(pTask->GetPlayerId(), m_ID, 0, "Kill Monster Must At NPC, Not Direct");
}

inline void ATaskTempl::CheckMining(
	TaskInterface* pTask,
	ActiveTaskList* pList,
	ActiveTaskEntry* pEntry) const
{
	if (m_enumMethod != enumTMCollectNumArticle
	 ||	m_ulItemsWanted == 0
	 || !pTask->IsDeliverLegal())
		return;

	const ITEM_WANTED& iw = m_ItemsWanted[0];
	unsigned long ulNum = _get_item_count(pTask, iw.m_ulItemTemplId, iw.m_bCommonItem);

	if (iw.m_ulItemNum && ulNum >= iw.m_ulItemNum)
		return;

	bool bDeliver = false;

	if (iw.m_bCommonItem)
	{
		if (pTask->CanDeliverCommonItem(1))
		{
			pTask->DeliverCommonItem(iw.m_ulItemTemplId, 1);
			bDeliver = true;
		}
	}
	else
	{
		if (pTask->CanDeliverTaskItem(1))
		{
			pTask->DeliverTaskItem(iw.m_ulItemTemplId, 1);
			bDeliver = true;
		}
	}

	if (bDeliver)
	{
		char log[1024];
		sprintf(log, "DeliverMineItem: Item id = %d", iw.m_ulItemTemplId);
		TaskInterface::WriteKeyLog(pTask->GetPlayerId(), m_ID, 1, log);
	}

	if (HasAllItemsWanted(pTask)) OnSetFinished(pTask, pList, pEntry);
}

inline void ATaskTempl::CheckWaitTime(
	TaskInterface* pTask,
	ActiveTaskList* pList,
	ActiveTaskEntry* pEntry,
	unsigned long ulCurTime,
	bool bAtNPC,
	int nChoice) const
{
	if (m_pFirstChild || pEntry->m_ulTaskTime + m_ulWaitTime >= ulCurTime)
		return;

	if (bAtNPC || m_enumFinishType == enumTFTDirect || m_enumFinishType == enumTFTConfirm)
	{
		pEntry->SetFinished();
		DeliverAward(pTask, pList, pEntry, nChoice);
	}
	else
		TaskInterface::WriteLog(pTask->GetPlayerId(), m_ID, 0, "WaitTime Must At NPC, Not Direct");
}

inline void ATaskTempl::GiveUpOneTask(
	TaskInterface* pTask,
	ActiveTaskList* pList,
	ActiveTaskEntry* pEntry,
	bool bForce) const
{
	if (m_pParent || !m_bCanGiveUp) return;
	pEntry->ClearSuccess();
	pEntry->SetGiveUp();
	OnSetFinished(pTask, pList, pEntry);
	TaskInterface::WriteLog(pTask->GetPlayerId(), m_ID, 1, "GiveUpTask");
	TaskInterface::WriteKeyLog(pTask->GetPlayerId(), m_ID, 1, "GiveUpTask");
}

inline void ATaskTempl::OnSetFinished(
	TaskInterface* pTask,
	ActiveTaskList* pList,
	ActiveTaskEntry* pEntry,
	bool bNotifyMem) const
{
	pEntry->SetFinished();

	// 通知客户端已完成
	NotifyClient(
		pTask,
		pEntry,
		TASK_SVR_NOTIFY_FINISHED,
		0);

	if(pEntry->GetTempl()->m_bPQTask)
		PublicQuestInterface::QuestRemovePlayer(pEntry->GetTempl()->m_ID, pTask->GetPlayerId());
	
	if (m_enumFinishType == enumTFTDirect || !pEntry->IsSuccess())
		DeliverAward(pTask, pList, pEntry, -1, bNotifyMem);
}

inline void ATaskTempl::NotifyClient(
	TaskInterface* pTask,
	const ActiveTaskEntry* pEntry,
	unsigned char uReason,
	unsigned long ulCurTime,
	unsigned long ulParam,
	int dps,
	int dph) const
{
	char log[1024];
	unsigned char buf[512];
	task_notify_base* pNotify = reinterpret_cast<task_notify_base*>(buf);
	size_t sz;

	pNotify->reason = uReason;
	pNotify->task = static_cast<unsigned short>(m_ID);
	bool bWriteLog = true;

	switch (uReason)
	{
	case TASK_SVR_NOTIFY_PLAYER_KILLED:
		static_cast<svr_player_killed*>(pNotify)->player_num = pEntry->m_wMonsterNum[ulParam];
		static_cast<svr_player_killed*>(pNotify)->index = ulParam;
		sz = sizeof(svr_player_killed);
		break;
	case TASK_SVR_NOTIFY_MONSTER_KILLED:
		static_cast<svr_monster_killed*>(pNotify)->monster_id = m_MonsterWanted[ulParam].m_ulMonsterTemplId;
		static_cast<svr_monster_killed*>(pNotify)->monster_num = pEntry->m_wMonsterNum[ulParam];
		if (dps && dph)
		{
			static_cast<svr_monster_killed*>(pNotify)->dps = dps;
			static_cast<svr_monster_killed*>(pNotify)->dph = dph;
		}
		else
		{
			static_cast<svr_monster_killed*>(pNotify)->dps = 0;
			static_cast<svr_monster_killed*>(pNotify)->dph = 0;
		}
		sz = sizeof(svr_monster_killed);
		bWriteLog = false;

		break;
	case TASK_SVR_NOTIFY_NEW:
		static_cast<svr_new_task*>(pNotify)->set_data(
			ulCurTime,
			reinterpret_cast<unsigned long>(pEntry),
			*(reinterpret_cast<const task_sub_tags*>(ulParam))
			);
		sz = static_cast<svr_new_task*>(pNotify)->get_size();
		break;
	case TASK_SVR_NOTIFY_COMPLETE:
		static_cast<svr_task_complete*>(pNotify)->set_data(
			ulCurTime,
			*(reinterpret_cast<const task_sub_tags*>(ulParam))
			);
		sz = static_cast<svr_task_complete*>(pNotify)->get_size();
		break;
	case TASK_SVR_NOTIFY_GIVE_UP:
	case TASK_SVR_NOTIFY_FINISHED:
	case TASK_SVR_NOTIFY_DIS_GLOBAL_VAL:
		sz = sizeof(task_notify_base);
		break;
	case TASK_SVR_NOTIFY_ERROR_CODE:
		static_cast<svr_task_err_code*>(pNotify)->err_code = ulParam;
		sz = sizeof(svr_task_err_code);
		break;
	default:
		sprintf(log, "NotifyClient, Unknown Reason = %d", uReason);
		TaskInterface::WriteLog(pTask->GetPlayerId(), m_ID, 0, log);
		return;
	}

	if (bWriteLog)
	{
		sprintf(log, "svr: Reason = %d, Param = 0x%x", uReason, ulParam);
		TaskInterface::WriteLog(pTask->GetPlayerId(), m_ID, 1, log);
	}

	assert(sz <= sizeof(buf));
	pTask->NotifyClient(buf, sz);
}

inline void TaskNotifyPlayer(
	TaskInterface* pTask,
	unsigned long ulPlayerId,
	unsigned long ulTaskId,
	unsigned char uReason,
	unsigned long ulParam = 0)
{
	task_player_notify notify;
	notify.reason = uReason;
	notify.task = static_cast<unsigned short>(ulTaskId);
	notify.param = ulParam;
	pTask->NotifyPlayer(ulPlayerId, &notify, sizeof(notify));
}

extern void TaskUpdateGlobalData(unsigned long ulTaskId, const unsigned char pData[TASK_GLOBAL_DATA_SIZE]);

inline void ATaskTempl::DeliverTeamMemTask(
	TaskInterface* pTask,
	TaskGlobalData* pGlobal,
	unsigned long ulCurTime) const
{
	const int nMemNum = pTask->GetTeamMemberNum();
	int i;

	for (i = 1; i < nMemNum; i++) // 跳过队长
	{
		TaskNotifyPlayer(
			pTask,
			pTask->GetTeamMemberId(i),
			m_ID,
			TASK_PLY_NOTIFY_NEW_MEM_TASK,
			true);
	}
}

inline unsigned long ATaskTempl::CalMultiByGlobalKeyValue(TaskInterface* pTask, const AWARD_DATA* pAward) const
{
	long lMulti = pAward->m_lNum;

	if (!pAward->m_bMulti)
		return 1;

	if (pAward->m_nNumType == 0)
	{
		lMulti = pTask->GetGlobalValue(pAward->m_lNum);
	}

	return lMulti;
}

inline bool ATaskTempl::HasGlobalData() const {	return m_ulMaxReceiver || m_ulPremise_Cotask; }

#endif

#endif
