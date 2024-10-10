#include "TaskTemplMan.h"
#include "TaskInterface.h"
#include <algorithm>

#ifdef LINUX
	#include "../template/elementdataman.h"
#else
	#include "elementdataman.h"
#endif

#ifdef _ELEMENTCLIENT
	#include "../EC_StringTab.h"
	#include "../EC_GameUIMan.h"
	#include "EC_UIHelper.h"
	#include "EC_UIConfigs.h"
	#include "EC_CommandLine.h"
	CECStringTab _task_err;
#endif

#ifndef _TASK_CLIENT
	#include "TaskServer.h"
	#define TASK_SVR_MAGIC	0x9A45
#endif

#if DEBUG_LOG == 1
	const char* _log_file = "task_log.txt";
#endif

#ifdef WIN32
	#include <io.h>
//#else
//	#include <dirent.h>
#endif

/*
 *	structs, definition
 */

#define DYN_TASK_CUR_VERSION	10
#define DYN_TASK_VERIFY_SVR		8711
#define DYN_TASK_DELIVER_SVR	8889

#define TASK_NPC_INFO_VERSION	1

struct DYN_TASK_PACK_HEADER
{
	unsigned long	pack_size;
	long			time_mark;
	unsigned short	version;
	unsigned short	task_count;
};

struct TASK_NPC_PACK_HEADER
{
	unsigned long	pack_size;
	long			time_mark;
	unsigned short	version;
	unsigned short	npc_count;
};

#ifndef _ELEMENTCLIENT
	ATaskTemplMan _task_templ_man;
	ATaskTemplMan* GetTaskTemplMan() { return &_task_templ_man; }
#endif

extern unsigned long _task_templ_cur_version;

ATaskTemplMan::ATaskTemplMan() :
m_pEleDataMan(0),
m_ulDynTasksTimeMark(0),
m_pDynTasksData(0),
m_ulDynTasksDataSize(0),
m_pNPCInfoData(0),
m_ulNPCInfoDataSize(0)
{
#ifdef _TASK_CLIENT
	memset(m_szDynPackPath, 0, sizeof(m_szDynPackPath));
	m_bDynTasksVerified = false;
	ClearSpecailAward();
#endif
}

ATaskTemplMan::~ATaskTemplMan()
{
}

#ifdef _TASK_CLIENT
static bool compare_tasks_canseekout(const ATaskTempl* lhs, const ATaskTempl* rhs)
{
	if (lhs->m_ulPremItems && !rhs->m_ulPremItems) return true;
	else if (!lhs->m_ulPremItems && rhs->m_ulPremItems) return false;
	else if (lhs->m_ulPremItems && rhs->m_ulPremItems) return lhs->m_ID > rhs->m_ID;
	else return lhs->m_ulPremise_Lev_Min > rhs->m_ulPremise_Lev_Min;
}
bool ATaskTemplMan::TasksSeekOutDiff::operator < (const TasksSeekOutDiff& rhs) const
{
	if (!task && rhs.task) return false;
	else if (task && !rhs.task) return true;
	else if (!task && !rhs.task) return true;
	else return compare_tasks_canseekout(task, rhs.task);
}
void ATaskTemplMan::SortTasksCanSeekOut()
{
	std::sort(m_TasksCanSeekOut.begin(), m_TasksCanSeekOut.end(), compare_tasks_canseekout);
}
void ATaskTemplMan::CheckAutoDelv(TaskInterface* pTask)
{
	const ATaskTempl* pTempl;
	TaskTemplMap::iterator it = m_AutoDelvMap.begin();
	unsigned long ulCurTime = TaskInterface::GetCurTime();
	ActiveTaskList* pLst = (ActiveTaskList*)pTask->GetActiveTaskList();

#ifndef _ELEMENTLOCALIZE
#ifdef _ELEMENTCLIENT
	if (CECCommandLine::GetBriefConfig(_AL("noautodelivertask"))) return;
#endif
#endif
	for (; it != m_AutoDelvMap.end(); ++it)
	{
		pTempl = it->second;

		if (!pTempl->IsValidState())
			continue;

		if (pTempl->CheckPrerequisite(pTask, pLst, ulCurTime) == 0)
		{
			pTempl->IncValidCount();
			_notify_svr(pTask, TASK_CLT_NOTIFY_AUTO_DELV, static_cast<unsigned short>(pTempl->m_ID));
		}
	}
}

void ATaskTemplMan::CheckSpecialAwardMask(TaskInterface* pTask)
{
	ActiveTaskList* pLst = (ActiveTaskList*)pTask->GetActiveTaskList();
	unsigned long ulCurTime = TaskInterface::GetCurTime();
	for (int i = 0;i < NUM_SPECIAL_AWARD;i++)
	{
		if (m_SpecialAward.special_mask & (1 << i))
		{
			const ATaskTempl* pTempl = GetTaskTemplByID(static_cast<unsigned short>(TASK_SPECIAL_AWARD[i]));
			if (pTempl && pTempl->CheckPrerequisite(pTask,pLst,ulCurTime) == 0)
			{
				_notify_svr(pTask, TASK_CLT_NOTIFY_SPECIAL_AWARD_MASK,static_cast<unsigned short>(pTempl->m_ID));	
			}
		}
	}
}
void ATaskTemplMan::ManualTrigTask(TaskInterface* pTask, unsigned long ulTask)
{
	_notify_svr(pTask, TASK_CLT_NOTIFY_MANUAL_TRIG, static_cast<unsigned short>(ulTask));
}

void ATaskTemplMan::ForceGiveUpTask(TaskInterface* pTask, unsigned long ulTask)
{
	_notify_svr(pTask, TASK_CLT_NOTIFY_FORCE_GIVEUP, static_cast<unsigned short>(ulTask));
}

void ATaskTemplMan::ForceRemoveFinishTask(TaskInterface* pTask, unsigned long ulTask)
{
#ifdef _DEBUG
	_notify_svr(pTask, TASK_CLT_NOTIFY_RM_FINISH_TASK, static_cast<unsigned short>(ulTask));
#endif
}

void ATaskTemplMan::OnDynTasksTimeMark(TaskInterface* pTask, unsigned long ulTimeMark, unsigned short version)
{
	if (version != DYN_TASK_CUR_VERSION)
		return;

	if (m_ulDynTasksTimeMark == ulTimeMark && LoadDynTasksFromPack(m_szDynPackPath))
	{
		SetDynTasksVerified(true);
		pTask->InitActiveTaskList();
		UpdateDynDataNPCService();
	}
	else
		_notify_svr(pTask, TASK_CLT_NOTIFY_DYN_DATA, 0);
}

void ATaskTemplMan::OnDynTasksData(TaskInterface* pTask, const void* data, size_t sz, bool ended)
{
	if (m_bDynTasksVerified)
	{
		assert(false);
		return;
	}

	size_t new_sz = sz + m_ulDynTasksDataSize;
	char* buf = new char[new_sz];
	g_ulNewCount++;

	if (m_pDynTasksData) memcpy(buf, m_pDynTasksData, m_ulDynTasksDataSize);
	memcpy(buf + m_ulDynTasksDataSize, data, sz);

	LOG_DELETE_ARR(m_pDynTasksData);
	m_pDynTasksData = buf;
	m_ulDynTasksDataSize = new_sz;

	if (ended)
	{
		if (UnmarshalDynTasks(m_pDynTasksData, m_ulDynTasksDataSize, false))
		{
			if (m_pDynTasksData)
			{
				FILE* fp = fopen(m_szDynPackPath, "wb");

				if (fp)
				{
					fwrite(m_pDynTasksData, 1, m_ulDynTasksDataSize, fp);
					fclose(fp);
				}
			}

			SetDynTasksVerified(true);
			pTask->InitActiveTaskList();
			UpdateDynDataNPCService();
		}

		LOG_DELETE_ARR(m_pDynTasksData);
		m_pDynTasksData = 0;
		m_ulDynTasksDataSize = 0;
	}
}

void ATaskTemplMan::OnStorageData(TaskInterface* pTask, const void* data)
{
	StorageTaskList* pLst = static_cast<StorageTaskList*>(pTask->GetStorageTaskList());
	memcpy(pLst->m_Storages, data, sizeof(pLst->m_Storages));
	memcpy(pLst->m_StoragesRefreshCount,static_cast<char*>(const_cast<void*>(data)) + sizeof(pLst->m_Storages),sizeof(pLst->m_StoragesRefreshCount));
	memcpy(pLst->m_StoragesRefreshTime,static_cast<char*>(const_cast<void*>(data)) + sizeof(pLst->m_Storages) + sizeof(pLst->m_StoragesRefreshCount),sizeof(pLst->m_StoragesRefreshTime));
	memcpy(pLst->m_StoragesReceivePerDay,static_cast<char*>(const_cast<void*>(data)) + sizeof(pLst->m_Storages) + sizeof(pLst->m_StoragesRefreshCount) + sizeof(pLst->m_StoragesRefreshTime),sizeof(pLst->m_StoragesReceivePerDay));
}

void ATaskTemplMan::OnSpecialAward(const special_award* p,TaskInterface* pTask)
{
	m_SpecialAward = *p;
	CheckSpecialAwardMask(pTask);
}

void ATaskTemplMan::VerifyDynTasksPack(const char* szPath)
{
	strcpy(m_szDynPackPath, szPath);

	FILE* fp = fopen(szPath, "rb");
	if (fp == NULL) return;

	fseek(fp, 0, SEEK_END);
	size_t sz = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	const size_t header_sz = sizeof(DYN_TASK_PACK_HEADER);

	if (sz < header_sz)
	{
		fclose(fp);
		return;
	}

	char* buf = new char[header_sz];
	g_ulNewCount++;
	fread(buf, 1, header_sz, fp);
	fclose(fp);

	UnmarshalDynTasks(buf, header_sz, true);
	LOG_DELETE_ARR(buf);
}

#else

void ATaskTemplMan::CheckDeathTrig(TaskInterface* pTask)
{
	ActiveTaskList* pLst = static_cast<ActiveTaskList*>(pTask->GetActiveTaskList());
	TaskTemplMap::iterator it = m_DeathTrigMap.begin();

	for (; it != m_DeathTrigMap.end(); ++it)
		if (it->second->m_bDeathTrig)
			it->second->CheckDeliverTask(
				pTask,
				0,
				NULL,
				false
				);
}

void ATaskTemplMan::OnTaskCheckAllTimeLimits(unsigned long ulCurTime)
{
}

void ATaskTemplMan::UpdateTimeLimitCheckList()
{
	m_TmLmtChkLst.clear();

	TaskTemplMap::iterator it = m_TaskTemplMap.begin();
	for (; it != m_TaskTemplMap.end(); ++it)
		if (it->second->m_ulMaxReceiver) m_TmLmtChkLst.push_back(it->second);
}

void ATaskTemplMan::OnTaskGetDynTasksTimeMark(TaskInterface* pTask)
{
	if (m_ulDynTasksTimeMark == 0) return;

	svr_task_dyn_time_mark data;
	data.reason		= TASK_SVR_NOTIFY_DYN_TIME_MARK;
	data.task		= 0;
	data.time_mark	= m_ulDynTasksTimeMark;
	data.version	= DYN_TASK_CUR_VERSION;
	pTask->NotifyClient(&data, sizeof(svr_task_dyn_time_mark));
}

void ATaskTemplMan::OnTaskGetSpecialAward(TaskInterface* pTask)
{
	svr_task_special_award data;

	data.reason = TASK_SVR_NOTIFY_SPECIAL_AWARD;
	data.task = 0;
	pTask->GetSpecailAwardInfo(&data.sa);
	pTask->NotifyClient(&data, sizeof(svr_task_special_award));
}

void ATaskTemplMan::OnTaskGetDynTasksData(TaskInterface* pTask)
{
	if (m_ulDynTasksDataSize == 0) return;

	const char* data = m_pDynTasksData;
	int sz = m_ulDynTasksDataSize;
	const int buf_size = 0x1000;
	const int data_size = buf_size - sizeof(task_notify_base);
	char buf[buf_size];
	task_notify_base* notify = (task_notify_base*)buf;
	char* data_part = buf + sizeof(task_notify_base);
	int send_total = 0;
	bool ended = false;

	while (true)
	{
		int send_size;

		if (send_total + data_size >= sz)
		{
			send_size = sz - send_total;
			ended = true;
		}
		else
			send_size = data_size;

		notify->reason = TASK_SVR_NOTIFY_DYN_DATA;
		notify->task = (ended ? 1 : 0);
		memcpy(data_part, data + send_total, send_size);
		pTask->NotifyClient(buf, send_size + sizeof(task_notify_base));
		if (ended) break;
		send_total += data_size;
	}
}

void ATaskTemplMan::OnTaskRemoveFinishTask(TaskInterface* pTask, unsigned long ulTask)
{
	FinishedTaskList* pList = static_cast<FinishedTaskList*>(pTask->GetFinishedTaskList());
	pList->RemoveTask(ulTask);

	char log[1024];
	sprintf(log, "RemoveFinishTask");
	TaskInterface::WriteKeyLog(pTask->GetPlayerId(), ulTask, 0, log);
}

inline void _SendPlayerStorageData(TaskInterface* pTask, StorageTaskList* pLst)
{
	char buf[sizeof(task_notify_base) + sizeof(pLst->m_Storages) + sizeof(pLst->m_StoragesRefreshCount) + sizeof(pLst->m_StoragesRefreshTime) + sizeof(pLst->m_StoragesReceivePerDay)];
	reinterpret_cast<task_notify_base*>(buf)->reason = TASK_SVR_NOTIFY_STORAGE;
	memcpy(buf + sizeof(task_notify_base), pLst->m_Storages, sizeof(pLst->m_Storages));
	memcpy(buf + sizeof(task_notify_base) + sizeof(pLst->m_Storages),pLst->m_StoragesRefreshCount,sizeof(pLst->m_StoragesRefreshCount));
	memcpy(buf + sizeof(task_notify_base) + sizeof(pLst->m_Storages) + sizeof(pLst->m_StoragesRefreshCount),pLst->m_StoragesRefreshTime,sizeof(pLst->m_StoragesRefreshTime));
	memcpy(buf + sizeof(task_notify_base) + sizeof(pLst->m_Storages) + sizeof(pLst->m_StoragesRefreshCount) + sizeof(pLst->m_StoragesRefreshTime),pLst->m_StoragesReceivePerDay,sizeof(pLst->m_StoragesReceivePerDay));
	pTask->NotifyClient(buf, sizeof(buf));
}

void ATaskTemplMan::OnTaskUpdateStorage(TaskInterface* pTask, unsigned long ulCurTime)
{
	StorageTaskList* pLst = static_cast<StorageTaskList*>(pTask->GetStorageTaskList());

	UpdateStorage(pTask, pLst, ulCurTime, 0);
}

bool ATaskTemplMan::UpdateStorage(TaskInterface* pTask, StorageTaskList* pLst, unsigned long ulCurTime, unsigned long idStorage)
{
	bool result = false;

	if (idStorage == 0)
	{
		for (int i = 1; i <= TASK_STORAGE_COUNT; i++)
		{
			if (UpdateOneStorage(pTask, pLst,ulCurTime, i,true))
				result = true;
		}	
	}
	else
		result = UpdateOneStorage(pTask, pLst,ulCurTime, idStorage,false);

	_SendPlayerStorageData(pTask, pLst);
	return result;
}


bool ATaskTemplMan::UpdateOneStorage(TaskInterface* pTask,StorageTaskList* pStorageLst, unsigned long ulCurTime, int idStorage,bool init_refresh)
{
	//看时间是否过了0点，过了0点刷新次数清零，刷新时间更新
	if (idStorage == 0 || idStorage > TASK_STORAGE_COUNT) return false;
	struct tm * tm1 = localtime((time_t*)&ulCurTime);
	unsigned long t_base = ulCurTime - tm1->tm_hour*3600 - tm1->tm_min*60 - tm1->tm_sec;

	unsigned int index = idStorage - 1;
	if(t_base > pStorageLst->m_StoragesRefreshTime[index])
	{
		pStorageLst->m_StoragesRefreshCount[index] = 0;
		pStorageLst->m_StoragesReceivePerDay[index] = 0;
	}	
	else if (init_refresh)
	{
		return false;
	}
	
	const NPC_TASK_OUT_SERVICE* pEssence = GetStorageTaskEssence(idStorage);
	if (pEssence != NULL) {
		// 在跨服上要检查刷新配置
		if (pTask->IsAtCrossServer() && pEssence->storage_refresh_on_crossserver == 0)
			return false;
		// 检查刷新次数是否超过限制
		if (pStorageLst->m_StoragesRefreshCount[index] > pEssence->storage_refresh_per_day) {
			return false;
		}
		//开始刷新
		memset(pStorageLst->m_Storages[index], 0, sizeof(unsigned short) * TASK_STORAGE_LEN);
		pStorageLst->m_StoragesRefreshCount[index] = init_refresh ? 1 : pStorageLst->m_StoragesRefreshCount[index] + 1;
		pStorageLst->m_StoragesRefreshTime[index] = ulCurTime;

		return PickTaskByProbability(pTask, pStorageLst, idStorage, ulCurTime, pEssence);		
	} else {
		const TASK_DICE_BY_WEIGHT_CONFIG* pEssence = GetWeightTasksEssence(idStorage);
		if (pEssence != NULL) {
			// 在跨服上要检查刷新配置
			if (pTask->IsAtCrossServer() && pEssence->storage_refresh_on_crossserver == 0)
				return false;
			//开始刷新
			memset(pStorageLst->m_Storages[index], 0, sizeof(unsigned short) * TASK_STORAGE_LEN);
			pStorageLst->m_StoragesRefreshTime[index] = ulCurTime;
			return PickTaskByWeight(pTask, pStorageLst, idStorage, ulCurTime, pEssence);		
		}
	}

	return false;
}

bool ATaskTemplMan::UpdateOneStorageDebug(TaskInterface* pTask, unsigned long ulCurTime, int idStorage, bool bUseDayAsSeed)
{
	if (idStorage == 0 || idStorage > TASK_STORAGE_COUNT) return false;
	struct tm * tm1 = localtime((time_t*)&ulCurTime);
	unsigned long t_base = ulCurTime - tm1->tm_hour*3600 - tm1->tm_min*60 - tm1->tm_sec;

	StorageTaskList* pStorageLst = static_cast<StorageTaskList*>(pTask->GetStorageTaskList());

	unsigned int index = idStorage - 1;
	
	const NPC_TASK_OUT_SERVICE* pEssence = GetStorageTaskEssence(idStorage);
	if (pEssence != NULL) {
		//开始刷新
		memset(pStorageLst->m_Storages[index], 0, sizeof(unsigned short) * TASK_STORAGE_LEN);
		pStorageLst->m_StoragesRefreshCount[index] = pStorageLst->m_StoragesRefreshCount[index] + 1;
		pStorageLst->m_StoragesRefreshTime[index] = ulCurTime;
		
		PickTaskByProbability(pTask, pStorageLst, idStorage, ulCurTime, pEssence);		
	} else {
		const TASK_DICE_BY_WEIGHT_CONFIG* pEssence = GetWeightTasksEssence(idStorage);
		if (pEssence != NULL) {
			//开始刷新
			memset(pStorageLst->m_Storages[index], 0, sizeof(unsigned short) * TASK_STORAGE_LEN);
			pStorageLst->m_StoragesRefreshTime[index] = ulCurTime;
			unsigned long randSeed = ((tm1->tm_year % 3) << 10) | tm1->tm_yday;
			PickTaskByWeight(pTask, pStorageLst, idStorage, randSeed, pEssence, !bUseDayAsSeed);		
		}
	}

	_SendPlayerStorageData(pTask, pStorageLst);
	
	return true;
}

bool ATaskTemplMan::PickTaskByProbability(TaskInterface* pTask,StorageTaskList* pStorageLst, int idStorage, unsigned long ulCurTime, const NPC_TASK_OUT_SERVICE* pEssence)
{
	if (!pEssence || !pTask || !pStorageLst) return false;
	int index = idStorage - 1;
	ActiveTaskList *pActiveLst = static_cast<ActiveTaskList*>(pTask->GetActiveTaskList());
	abase::vector<int> RareTaskList;
	abase::vector<float> RareTaskProbability;
	std::set<int> NormalTaskList;

	size_t task_array_size = SIZE_OF_ARRAY(pEssence->id_tasks); 
	// 填充稀有任务容器和普通任务容器
	for (size_t j = 0; j < task_array_size; j++)
	{
		unsigned int idTask = pEssence->id_tasks[j];
		if (idTask == 0)
			break;
		
		TaskTemplMap::iterator it = m_TaskTemplMap.find(idTask);
		
		if (it == m_TaskTemplMap.end())
			continue;
		
		ATaskTempl* pTempl = it->second;
		//检查是否是可以接受的任务
		if (pTempl->CheckPrerequisite(pTask, pActiveLst, ulCurTime, true, true, false) != 0)
			continue;
		
		if (pTempl->m_fLibraryTasksProbability > 0.f)
		{
			RareTaskList.push_back(pTempl->m_ID);
			RareTaskProbability.push_back(pTempl->m_fLibraryTasksProbability);
		}
		else
		{
			NormalTaskList.insert(pTempl->m_ID);
		}
	}
	
	if (RareTaskList.size() == 0 && NormalTaskList.size() == 0)
		return false;
	
	//加一个空的稀有任务
	if(RareTaskList.size())
	{
		RareTaskList.push_back(0);
		RareTaskProbability.push_back(1.f);
	}
	
	int min_count = pEssence->storage_refresh_count_min > pEssence->storage_refresh_count_max ? pEssence->storage_refresh_count_max : pEssence->storage_refresh_count_min;
	int max_count = pEssence->storage_refresh_count_max > pEssence->storage_refresh_count_min ? pEssence->storage_refresh_count_max : pEssence->storage_refresh_count_min;
	size_t rand_count = pTask->RandNormal(min_count, max_count);
	
	if (rand_count > TASK_STORAGE_LEN)
		rand_count = TASK_STORAGE_LEN;
	
	for (size_t i = 0; i < rand_count; ++ i)
	{
		if(RareTaskList.size())
		{
			int r = pTask->RandSelect(RareTaskProbability.begin(),RareTaskProbability.size());
			if(RareTaskList[r] > 0)
			{
				ATaskTempl* pTempl = GetTopTaskByID(RareTaskList[r]);
				if (pTempl && pTempl->m_bIsUniqueStorageTask)
				{
					memset(pStorageLst->m_Storages[index], 0, sizeof(unsigned short) * TASK_STORAGE_LEN);
					pStorageLst->m_Storages[index][0] = RareTaskList[r];
					break;
				}
				pStorageLst->m_Storages[index][i] = RareTaskList[r];
				RareTaskList[r] = 0;
				RareTaskProbability[r] = 0.f;
				continue;
			}
		}
		
		if(NormalTaskList.size())
		{
			int r = pTask->RandNormal(0,NormalTaskList.size()-1);
			std::set<int>::iterator it = NormalTaskList.begin();
			std::advance(it,r);
			pStorageLst->m_Storages[index][i] = *it;
			NormalTaskList.erase(it);
			continue;
		}
		
		pStorageLst->m_Storages[index][i] = 0;
	}
	return true;
}
static const int s_RandTableSize = 1200;
static const int s_RandTable[s_RandTableSize] = {
41,18467,6334,26500,19169,15724,11478,29358,26962,24464,
5705,28145,23281,16827,9961,491,2995,11942,4827,5436,
32391,14604,3902,153,292,12382,17421,18716,19718,19895,
5447,21726,14771,11538,1869,19912,25667,26299,17035,9894,
28703,23811,31322,30333,17673,4664,15141,7711,28253,6868,
25547,27644,32662,32757,20037,12859,8723,9741,27529,778,
12316,3035,22190,1842,288,30106,9040,8942,19264,22648,
27446,23805,15890,6729,24370,15350,15006,31101,24393,3548,
19629,12623,24084,19954,18756,11840,4966,7376,13931,26308,
16944,32439,24626,11323,5537,21538,16118,2082,22929,16541,
4833,31115,4639,29658,22704,9930,13977,2306,31673,22386,
5021,28745,26924,19072,6270,5829,26777,15573,5097,16512,
23986,13290,9161,18636,22355,24767,23655,15574,4031,12052,
27350,1150,16941,21724,13966,3430,31107,30191,18007,11337,
15457,12287,27753,10383,14945,8909,32209,9758,24221,18588,
6422,24946,27506,13030,16413,29168,900,32591,18762,1655,
17410,6359,27624,20537,21548,6483,27595,4041,3602,24350,
10291,30836,9374,11020,4596,24021,27348,23199,19668,24484,
8281,4734,53,1999,26418,27938,6900,3788,18127,467,
3728,14893,24648,22483,17807,2421,14310,6617,22813,9514,
14309,7616,18935,17451,20600,5249,16519,31556,22798,30303,
6224,11008,5844,32609,14989,32702,3195,20485,3093,14343,
30523,1587,29314,9503,7448,25200,13458,6618,20580,19796,
14798,15281,19589,20798,28009,27157,20472,23622,18538,12292,
6038,24179,18190,29657,7958,6191,19815,22888,19156,11511,
16202,2634,24272,20055,20328,22646,26362,4886,18875,28433,
29869,20142,23844,1416,21881,31998,10322,18651,10021,5699,
3557,28476,27892,24389,5075,10712,2600,2510,21003,26869,
17861,14688,13401,9789,15255,16423,5002,10585,24182,10285,
27088,31426,28617,23757,9832,30932,4169,2154,25721,17189,
19976,31329,2368,28692,21425,10555,3434,16549,7441,9512,
30145,18060,21718,3753,16139,12423,16279,25996,16687,12529,
22549,17437,19866,12949,193,23195,3297,20416,28286,16105,
24488,16282,12455,25734,18114,11701,31316,20671,5786,12263,
4313,24355,31185,20053,912,10808,1832,20945,4313,27756,
28321,19558,23646,27982,481,4144,23196,20222,7129,2161,
5535,20450,11173,10466,12044,21659,26292,26439,17253,20024,
26154,29510,4745,20649,13186,8313,4474,28022,2168,14018,
18787,9905,17958,7391,10202,3625,26477,4414,9314,25824,
29334,25874,24372,20159,11833,28070,7487,28297,7518,8177,
17773,32270,1763,2668,17192,13985,3102,8480,29213,7627,
4802,4099,30527,2625,1543,1924,11023,29972,13061,14181,
31003,27432,17505,27593,22725,13031,8492,142,17222,31286,
13064,7900,19187,8360,22413,30974,14270,29170,235,30833,
19711,25760,18896,4667,7285,12550,140,13694,2695,21624,
28019,2125,26576,21694,22658,26302,17371,22466,4678,22593,
23851,25484,1018,28464,21119,23152,2800,18087,31060,1926,
9010,4757,32170,20315,9576,30227,12043,22758,7164,5109,
7882,17086,29565,3487,29577,14474,2625,25627,5629,31928,
25423,28520,6902,14962,123,24596,3737,13261,10195,32525,
1264,8260,6202,8116,5030,20326,29011,30771,6411,25547,
21153,21520,29790,14924,30188,21763,4940,20851,18662,13829,
30900,17713,18958,17578,8365,13007,11477,1200,26058,6439,
2303,12760,19357,2324,6477,5108,21113,14887,19801,22850,
14460,22428,12993,27384,19405,6540,31111,28704,12835,32356,
6072,29350,18823,14485,20556,23216,1626,9357,8526,13357,
29337,23271,23869,29361,12896,13022,29617,10112,12717,18696,
11585,24041,24423,24129,24229,4565,6559,8932,22296,29855,
12053,16962,3584,29734,6654,16972,21457,14369,22532,2963,
2607,2483,911,11635,10067,22848,4675,12938,2223,22142,
23754,6511,22741,20175,21459,17825,3221,17870,1626,31934,
15205,31783,23850,17398,22279,22701,12193,12734,1637,26534,
5556,1993,10176,25705,6962,10548,15881,300,14413,16641,
19855,24855,13142,11462,27611,30877,20424,32678,1752,18443,
28296,12673,10040,9313,875,20072,12818,610,1017,14932,
28112,30695,13169,23831,20040,26488,28685,19090,19497,2589,
25990,15145,19353,19314,18651,26740,22044,11258,335,8759,
11192,7605,25264,12181,28503,3829,23775,20608,29292,5997,
17549,29556,25561,31627,6467,29541,26129,31240,27813,29174,
20601,6077,20215,8683,8213,23992,25824,5601,23392,15759,
2670,26428,28027,4084,10075,18786,15498,24970,6287,23847,
32604,503,21221,22663,5706,2363,9010,22171,27489,18240,
12164,25542,7619,20913,7591,6704,31818,9232,750,25205,
4975,1539,303,11422,21098,11247,13584,13648,2971,17864,
22913,11075,21545,28712,17546,18678,1769,15262,8519,13985,
28289,15944,2865,18540,23245,25508,28318,27870,9601,28323,
21132,24472,27152,25087,28570,29763,29901,17103,14423,3527,
11600,26969,14015,5565,28,21543,25347,2088,2943,12637,
22409,26463,5049,4681,1588,11342,608,32060,21221,1758,
29954,20888,14146,690,7949,12843,21430,25620,748,27067,
4536,20783,18035,32226,15185,7038,9853,25629,11224,15748,
19923,3359,32257,24766,4944,14955,23318,32726,25411,21025,
20355,31001,22549,9496,18584,9515,17964,23342,8075,17913,
16142,31196,21948,25072,20426,14606,26173,24429,32404,6705,
20626,29812,19375,30093,16565,16036,14736,29141,30814,5994,
8256,6652,23936,30838,20482,1355,21015,1131,18230,17841,
14625,2011,32637,4186,19690,1650,5662,21634,10893,10353,
21416,13452,14008,7262,22233,5454,16303,16634,26303,14256,
148,11124,12317,4213,27109,24028,29200,21080,21318,16858,
24050,24155,31361,15264,11903,3676,29643,26909,14902,3561,
28489,24948,1282,13653,30674,2220,5402,6923,3831,19369,
3878,20259,19008,22619,23971,30003,21945,9781,26504,12392,
32685,25313,6698,5589,12722,5938,19037,6410,31461,6234,
12508,9961,3959,6493,1515,25269,24937,28869,58,14700,
13971,26264,15117,16215,24555,7815,18330,3039,30212,29288,
28082,1954,16085,20710,24484,24774,8380,29815,25951,6541,
18115,1679,17110,25898,23073,788,23977,18132,29956,28689,
26113,10008,12941,15790,1723,21363,28,25184,24778,7200,
5071,1885,21974,1071,11333,22867,26153,14295,32168,20825,
9676,15629,28650,2598,3309,4693,4686,30080,10116,12249,
26667,1528,26679,7864,29421,8405,8826,6816,7516,27726,
28666,29087,27681,19964,1340,5686,6021,11662,14721,6064,
29309,20415,17902,29873,17124,23941,32745,31762,28423,27531,
4806,12268,9318,5602,31907,24307,23481,1012,21136,26630,
24114,26809,4084,23556,12290,21293,29996,29152,1054,25345,
14708,248,7491,13712,5131,30114,16439,7958,24722,29704,
6995,1052,25269,7479,18238,26423,27918,10866,17659,32498,
28486,21196,7462,26633,12158,12022,1146,18392,13037,3925,
10647,29458,6602,30807,4098,27830,29292,14600,7278,20799,
18352,20448,13882,540,28315,4575,8762,9567,22336,18397,
31418,19897,25828,13851,26816,24230,4449,16925,658,229,
24520,10940,29560,15147,25162,1655,20675,792,22361,31754,
16398,23146,8714,26946,21188,19569,8638,27663,15075,24515,
11521,475,15615,20528,13234,12570,905,9464,19557,28962,
14161,30524,12549,17469,330,31923,28350,14333,22925,10910,
19737,16336,18337,1278,12393,7636,30714,28164,31591,19949,
19135,2505,13337,10004,16337,2623,28664,9970,25608,30568,
19281,7085,9152,18373,28652,8194,9876,23826,28396,7572,
6249,14640,29174,18819,943,32611,1941,20289,8419,5565,
3805,7585,16216,31450,1615,2609,11064,9166,16893,16074,
};

static const float s_RandMax = 32767.f;

float NormalizeRandResult(int toNormalize)
{
	float ret = toNormalize / s_RandMax;
	return ret;
}

struct WeightTaskData
{
	int id;
	int weight;
	
	WeightTaskData(int idTask, int taskWeight):
	id(idTask),
	weight(taskWeight)
	{}

	bool operator < (const WeightTaskData& rhs) const {
		return weight < rhs.weight;
	}
};

bool ATaskTemplMan::PickTaskByWeight(TaskInterface* pTask,StorageTaskList* pStorageLst, int idStorage, unsigned long ulCurTime, const TASK_DICE_BY_WEIGHT_CONFIG* pEssence, bool bDebug)
{
	if (!pEssence || !pTask || !pStorageLst || idStorage <= 0 || idStorage > TASK_STORAGE_COUNT) return false;

	// 随机种子
	struct tm * tm1 = localtime((time_t*)&ulCurTime);
	int seed1 = tm1->tm_year % 3;
	int seed2 = tm1->tm_yday;
	int startIndex = seed1 * 365 + seed2;// startIndex:0~1095
	if (bDebug) startIndex = pTask->RandNormal(0, 365 * 3);

	int offsetIndex = 0;
	int totalWeight = 0;
	int storageIndex = idStorage - 1;
	int storageSubIndex = 0;

	// 先按顺序根据每个任务集合的概率来添加
	size_t taskSetSize = SIZE_OF_ARRAY(pEssence->uniform_weight_list);
	for (size_t j = 0; j < taskSetSize && storageSubIndex < TASK_STORAGE_LEN; ++j, ++offsetIndex) {
		int id = pEssence->uniform_weight_list[j].task_list_config_id;
		int weight =  pEssence->uniform_weight_list[j].weight;
		float probability = pEssence->uniform_weight_list[j].probability;
		if (id && weight && probability > 0.f) {
			int randIndex = startIndex + offsetIndex;
			if (randIndex >= s_RandTableSize) {
				startIndex = 0;
				offsetIndex = 0;
				randIndex = 0;
			}
			float randProb = NormalizeRandResult(s_RandTable[randIndex]);
			if (randProb <= probability) {
				int weightNext = totalWeight + weight;
				if (weightNext > pEssence->max_weight) {
					continue;
				} else {
					totalWeight = weightNext;
					pStorageLst->m_Storages[storageIndex][storageSubIndex++] = id;
					if (weightNext == pEssence->max_weight) {
						break;
					}
				}
			}
		}
	}
	// 保存任务集合的数量
	pStorageLst->m_StoragesTaskSetCount[storageIndex] = storageSubIndex;
	// 权重不足的话用单个任务补充
	if (totalWeight < pEssence->max_weight) {
		typedef abase::vector<WeightTaskData> WeightTaskContainer;
		WeightTaskContainer vecTask, vecResult;
		std::set<int> setIndex;
		DATA_TYPE dt;
		const TASK_LIST_CONFIG* pConfig = (const TASK_LIST_CONFIG*)m_pEleDataMan->get_data_ptr(pEssence->other_task_list_config_id, ID_SPACE_CONFIG, dt);
		// 先放到容器中
		if ( dt == DT_TASK_LIST_CONFIG && pConfig ) {
			size_t taskArraySize = SIZE_OF_ARRAY(pConfig->id_tasks); 
			for (size_t j = 0; j < taskArraySize; j++) {
				unsigned int idTask = pConfig->id_tasks[j];
				if (idTask == 0)
					break;
				TaskTemplMap::iterator it = m_TaskTemplMap.find(idTask);
				if (it == m_TaskTemplMap.end())
					continue;
				ATaskTempl* pTempl = it->second;
				if (pTempl == NULL) continue;
				int weight = pTempl->m_Award_S->m_iWorldContribution;
				if (weight) {
					WeightTaskData data(idTask, weight);
					vecTask.push_back(data);
				}
			}
		}
		
		size_t totalSize = vecTask.size();
		for (int traversalCount = 0; setIndex.size() <= TASK_STORAGE_LEN; ++offsetIndex, ++traversalCount){
			// 遍历一次随机数表之后退出循环
			if (traversalCount >= s_RandTableSize) break;
			int randIndex = startIndex + offsetIndex;
			if (randIndex >= s_RandTableSize) {
				startIndex = 0;
				offsetIndex = 0;
				randIndex = 0;
			}
			int index = s_RandTable[randIndex] % totalSize;
			if (setIndex.find(index) == setIndex.end()) {
				int weightNext = totalWeight + vecTask[index].weight;
				if (weightNext > pEssence->max_weight) {
					continue;
				} else {
					setIndex.insert(index);
					totalWeight = weightNext;
					vecResult.push_back(vecTask[index]);
					if (weightNext == pEssence->max_weight) {
						break;
					}
				}
			}
		}
		// 最后遍历一次
		if (totalWeight < pEssence->max_weight) {
			for (size_t i = 0; i < totalSize; ++i){
				if (setIndex.find(i) == setIndex.end() 
					&& totalWeight + vecTask[i].weight <= pEssence->max_weight) {
					setIndex.insert(i);
					totalWeight += vecTask[i].weight;
					vecResult.push_back(vecTask[i]);
				}
			}
		}
		size_t resultSize = vecResult.size();
		// 普通任务放到后面
		for (size_t  k = 0; k < resultSize && storageSubIndex < TASK_STORAGE_LEN; ++k) {
			const WeightTaskData& data = vecResult[k];
			pStorageLst->m_Storages[storageIndex][storageSubIndex++] = data.id;
		}
	}
	return true;
}

#endif

// #ifndef WIN32
// 
// int sel_templ(const struct dirent* file)
// {
// 	int nLen = strlen(file->d_name);
//    	if (nLen > 4 && memcmp((char*)(file->d_name) + nLen - 4, ".tkt", 4) == 0)
// 		return 1;
// 	return 0;
// }
// 
// #endif

#ifdef TASK_TEMPL_EDITOR

#ifdef _TASK_CLIENT

static bool _compare(ATaskTempl* arg1, ATaskTempl* arg2)
{
	return arg1->GetID() > arg2->GetID();
}

bool ATaskTemplMan::SaveTasksToPack(const char* szPackPath)
{
	if (m_TaskTemplMap.size() == 0) return false;

	FILE* fp = fopen(szPackPath, "wb");
	if (fp == NULL) return false;

	std::vector<ATaskTempl*> TaskArr;
	TaskArr.reserve(2048);
	size_t dyn_task_count = 0;
	TaskTemplMap::iterator it = m_TaskTemplMap.begin();

	for (; it != m_TaskTemplMap.end(); ++it)
	{
		if (it->second->m_DynTaskType)
			dyn_task_count++;

		TaskArr.push_back(it->second);
	}

	std::sort(TaskArr.begin(), TaskArr.end(), _compare);

	TASK_PACK_HEADER tph;
	tph.magic		= TASK_PACK_MAGIC;
	tph.version		= _task_templ_cur_version;
	tph.item_count	= m_TaskTemplMap.size() - dyn_task_count;

	long* pOffs = new long[tph.item_count];
	g_ulNewCount++;

	fwrite(&tph, sizeof(TASK_PACK_HEADER), 1, fp);

	fseek(fp, sizeof(long) * tph.item_count, SEEK_CUR);

	int i = 0;
	for (size_t n = 0; n < TaskArr.size(); n++)
	{
		ATaskTempl* pTempl = TaskArr[n];

		if (pTempl->m_DynTaskType)
			continue;

		pOffs[i++] = ftell(fp);
		pTempl->SaveToBinFile(fp);
	}

	fseek(fp, sizeof(TASK_PACK_HEADER), SEEK_SET);
	fwrite(pOffs, sizeof(long), tph.item_count, fp);

	LOG_DELETE_ARR(pOffs);
	fclose(fp);
	return true;
}

bool ATaskTemplMan::SaveDynTasksToPack(const char* szPath, bool bMarshalAll)
{
	DYN_TASK_PACK_HEADER header;
	header.pack_size	= sizeof(DYN_TASK_PACK_HEADER);
	header.version		= DYN_TASK_CUR_VERSION;
	header.task_count	= 0;
	time(&header.time_mark);

	FILE* fp = fopen(szPath, "w+b");
	if (fp == NULL) return false;

	const int buf_size = 1000000;
	char* buf = new char[buf_size];
	g_ulNewCount++;

	fseek(fp, sizeof(DYN_TASK_PACK_HEADER), SEEK_SET);
	TaskTemplMap::iterator it = m_TaskTemplMap.begin();

	for (; it != m_TaskTemplMap.end(); ++it)
	{
		ATaskTempl* pTop = it->second;

		if (!bMarshalAll && !pTop->m_DynTaskType)
			continue;

		if (wcsstr(pTop->m_szName, L"测试任务_") != 0)
			continue;

		int sz = pTop->MarshalDynTask(buf);
		if (sz == 0) continue;

		assert(sz < buf_size);

		if (fwrite(buf, 1, sz, fp) != sz)
			assert(false);

		header.pack_size += sz;
		header.task_count++;
	}

	fseek(fp, 0, SEEK_SET);
	fwrite(&header, sizeof(DYN_TASK_PACK_HEADER), 1, fp);

	fclose(fp);
	LOG_DELETE_ARR(buf);
	return true;
}

bool ATaskTemplMan::SaveNPCInfoToPack(const char* szPath)
{
	TASK_NPC_PACK_HEADER header;
	header.pack_size	= sizeof(TASK_NPC_PACK_HEADER) + m_NPCInfoMap.size() * sizeof(NPC_INFO);
	header.version		= TASK_NPC_INFO_VERSION;
	header.npc_count	= m_NPCInfoMap.size();
	time(&header.time_mark);

	FILE* fp = fopen(szPath, "wb");
	if (fp == NULL) return false;

	const int buf_size = 1000000;
	char* buf = new char[buf_size];
	g_ulNewCount++;

	fwrite(&header, sizeof(TASK_NPC_PACK_HEADER), 1, fp);

	TaskNPCInfoMap::iterator it = m_NPCInfoMap.begin();
	for (; it != m_NPCInfoMap.end(); ++it)
	{
		const NPC_INFO& info = it->second;
		fwrite(&info, sizeof(info), 1, fp);
	}

	fclose(fp);
	LOG_DELETE_ARR(buf);
	return true;
}

HTREEITEM ATaskTemplMan::GetTopTreeItemByID(unsigned long ulID)
{
	TreeItemMap::iterator it = m_TreeTopItemMap.find(ulID);
	if (it == m_TreeTopItemMap.end()) return 0;
	return (it->second).m_hItem;
}

HTREEITEM ATaskTemplMan::GetTreeItemByID(unsigned long ulID)
{
	TreeItemMap::iterator it = m_TreeItemMap.find(ulID);
	if (it == m_TreeItemMap.end()) return 0;
	return (it->second).m_hItem;
}

void ATaskTemplMan::AddOneTreeItem(ATaskTempl* pTask,TreeData& sData,bool bIsTopItem)
{	
	if (bIsTopItem)
	{
		m_TreeTopItemMap[pTask->m_ID] = sData;
	}
	m_TreeItemMap[pTask->m_ID] = sData;
}

#include <set>
#include <functional>
#include "BaseDataIDMan.h"
using namespace std ;

struct ID_Descending:public binary_function<const unsigned long,const unsigned long,bool>
{
	bool operator()(const unsigned long ulID1,const unsigned long ulID2)const
	{
		return ulID1 > ulID2;
	}
};
//**
bool ATaskTemplMan::SaveTasksToPack2(const char* szPackPath)
{
#ifndef _ELEMENTLOCALIZE

	if (m_TaskTemplMap.size() == 0) return false;

	FILE* fp = fopen(szPackPath, "wb");
	if (fp == NULL) return false;

	typedef std::set<unsigned long, ID_Descending> Set_ID;
	Set_ID IdSet;

	size_t dyn_task_count = 0;
	TaskTemplMap::iterator iterTaskMap;

	TreeItemMap::iterator iter = m_TreeTopItemMap.begin();

	for (;iter != m_TreeTopItemMap.end();++iter)
	{
		unsigned long ulID = iter->first;
		iterTaskMap = m_TaskTemplMap.find(ulID);
		if (iterTaskMap != m_TaskTemplMap.end())
		{
			if (iterTaskMap->second->m_DynTaskType)
			{
				dyn_task_count++;
				continue;
			}
		}
		else if ((iter->second).m_DynTaskType)
		{
			dyn_task_count++;
			continue;
		}
		IdSet.insert(ulID);
	}



	TASK_PACK_HEADER tph;
	tph.magic		= TASK_PACK_MAGIC;
	tph.version		= _task_templ_cur_version;
	tph.item_count	= m_TreeTopItemMap.size() - dyn_task_count;

	long* pOffs = new long[tph.item_count];
	g_ulNewCount++;

	fwrite(&tph, sizeof(TASK_PACK_HEADER), 1, fp);

	fseek(fp, sizeof(long) * tph.item_count, SEEK_CUR);

	int i = 0;
	Set_ID::iterator iterIDSet = IdSet.begin();
	
	for (;iterIDSet != IdSet.end();++iterIDSet)
	{
		ATaskTempl* pTempl = 0;
		iterTaskMap = m_TaskTemplMap.find(*iterIDSet);
		if (iterTaskMap != m_TaskTemplMap.end())
		{
			pTempl = iterTaskMap->second;
			pOffs[i++] = ftell(fp);
			pTempl->SaveToBinFile(fp);
		}
		else
		{
			pTempl = new ATaskTempl;
			g_ulNewCount++;
			CString strPath = g_TaskIDMan.GetPathByID(*iterIDSet);
			strPath = "BaseData\\TaskTemplate\\" + strPath + ".tkt";
			if (!pTempl->LoadFromTextFile(strPath))
			{
				AfxMessageBox("读取硬盘上的模板文件失败！", MB_ICONSTOP);
				delete pTempl;
				g_ulDelCount++;
				return false;
			}
			pOffs[i++] = ftell(fp);
			pTempl->SaveToBinFile(fp);
			delete pTempl;
			g_ulDelCount++;
		}
	}

	fseek(fp, sizeof(TASK_PACK_HEADER), SEEK_SET);
	fwrite(pOffs, sizeof(long), tph.item_count, fp);

	LOG_DELETE_ARR(pOffs);
	fclose(fp);
#endif //_ELEMENTLOCALIZE
	return true;
}
bool ATaskTemplMan::SaveDynTasksToPack2(const char* szPath, bool bMarshalAll)
{
#ifndef _ELEMENTLOCALIZE

	DYN_TASK_PACK_HEADER header;
	header.pack_size	= sizeof(DYN_TASK_PACK_HEADER);
	header.version		= DYN_TASK_CUR_VERSION;
	header.task_count	= 0;
	time(&header.time_mark);
	
	FILE* fp = fopen(szPath, "w+b");
	if (fp == NULL) return false;
	
	const int buf_size = 1000000;
	char* buf = new char[buf_size];
	g_ulNewCount++;
	
	fseek(fp, sizeof(DYN_TASK_PACK_HEADER), SEEK_SET);
	TreeItemMap::iterator it = m_TreeTopItemMap.begin();
	
	for (; it != m_TreeTopItemMap.end(); ++it)
	{
		ATaskTempl* pTop = 0;
		
		if (!bMarshalAll && !(it->second).m_DynTaskType)
			continue;
		
		if (wcsstr((it->second).GetName(), L"测试任务_") != 0)
			continue;
		
		int sz = 0;
		TaskTemplMap::iterator iterTask = m_TaskTemplMap.find(it->first);
		if (iterTask != m_TaskTemplMap.end())
		{
			pTop = iterTask->second;
			sz = pTop->MarshalDynTask(buf);
		}
		else
		{
			pTop = new ATaskTempl;
			g_ulNewCount++;
			CString strPath = g_TaskIDMan.GetPathByID(it->first);
			strPath = "BaseData\\TaskTemplate\\" + strPath + ".tkt";
			if (!pTop->LoadFromTextFile(strPath))
			{
				AfxMessageBox("读取硬盘上的模板文件失败！", MB_ICONSTOP);
				delete pTop;
				g_ulDelCount++;
				return false;
			}
			sz = pTop->MarshalDynTask(buf);
			delete pTop;
			g_ulDelCount++;
		}
		
		if (sz == 0) continue;
		
		assert(sz < buf_size);
		
		if (fwrite(buf, 1, sz, fp) != sz)
			assert(false);
		
		header.pack_size += sz;
		header.task_count++;
	}
	
	fseek(fp, 0, SEEK_SET);
	fwrite(&header, sizeof(DYN_TASK_PACK_HEADER), 1, fp);
	
	fclose(fp);
	LOG_DELETE_ARR(buf);
#endif _ELEMENTLOCALIZE
	return true;
}

#endif

#endif

bool ATaskTemplMan::CanGiveUpTask(unsigned long ulTaskId)
{
	const ATaskTempl* pTempl = GetTaskTemplByID(ulTaskId);
	if (!pTempl) return false;
	pTempl = pTempl->GetTopTask();
	return pTempl->m_bCanGiveUp;
}

#ifndef _TASK_CLIENT
extern void OnTaskGiveUpOneTask(TaskInterface* pTask, unsigned long ulTaskId, bool bForce);
#endif

void ATaskTemplMan::OnForgetLivingSkill(TaskInterface* pTask)
{
	FinishedTaskList* pList = static_cast<FinishedTaskList*>(pTask->GetFinishedTaskList());

	for (size_t i = 0; i < m_SkillTaskLst.size(); i++)
	{
		pList->RemoveTask(m_SkillTaskLst[i]->GetID());

#ifndef _TASK_CLIENT
		OnTaskGiveUpOneTask(pTask, m_SkillTaskLst[i]->GetID(), false);
#endif
	}

#ifndef _TASK_CLIENT

	task_notify_base notify;
	notify.reason = TASK_SVR_NOTIFY_FORGET_SKILL;
	notify.task = 0;
	pTask->NotifyClient(&notify, sizeof(notify));

#endif
}

#ifdef _ELEMENTCLIENT

void TaskShowErrMessage(int nIndex)
{
	const wchar_t* szMsg = _task_err.GetWideString(nIndex);
	if (szMsg) TaskInterface::ShowMessage(szMsg);
}
void ATaskTemplMan::GetTitleTasks(TaskInterface* pTask, TaskTemplLst& lst)
{
	ActiveTaskList* pLst = (ActiveTaskList*)pTask->GetActiveTaskList();
	ActiveTaskEntry* aEntries = pLst->m_TaskEntries;
	for (unsigned char i = 0; i < pLst->m_uTaskCount; ++i) {
		ActiveTaskEntry& CurEntry = aEntries[i];
		if (!CurEntry.m_ulTemplAddr)
			continue;
		if (CurEntry.GetTempl()->m_bDisplayInTitleTaskUI){
			int nPos = pTask->GetFirstSubTaskPosition(CurEntry.m_ID);
			while( nPos > -1 )
				lst.push_back(GetTaskTemplByID(pTask->GetNextSub(nPos)));
			lst.push_back(const_cast<ATaskTempl*>(CurEntry.GetTempl()));
		}
	}
}

void ATaskTemplMan::GetAvailableTasks(TaskInterface* pPlayer, TaskTemplLst& lst)
{
	lst.reserve(256);
	size_t count = m_TasksCanSeekOut.size();
	for (size_t i = 0; i < count; ++i) {
		ATaskTempl* pTempl = m_TasksCanSeekOut[i];

		if (pTempl->CheckLevel(pPlayer))
			continue;

		if (pPlayer->CanDeliverTask(pTempl->GetID()) == 0)
			lst.push_back(pTempl);
	}
}
void ATaskTemplMan::UpdateTasksSeekOutDiff(TaskInterface* pTask)
{
	// 得到当前可接任务
	TaskTemplLst ttl;
	GetAvailableTasks(pTask, ttl);
	if (m_TasksCanDeliver.empty()) m_TasksCanDeliver.swap(ttl);
	else {
		size_t count_before = m_TasksCanDeliver.size();
		size_t count_after = ttl.size();
		if (count_after == count_after)
			if (std::equal(m_TasksCanDeliver.begin(), m_TasksCanDeliver.end(), ttl.begin()))
				return;
		// 构造之前的可接任务序列
		size_t i = 0;
		TasksSeekOutDiffList task_list_before;
		for (i = 0; i < count_before; ++i) {
			TasksSeekOutDiff diff(m_TasksCanDeliver[i]);
			task_list_before.push_back(diff);
		}
		// 构造当前的可接任务序列
		TasksSeekOutDiffList task_list_after;
		for (i = 0; i < count_after; ++i) {
			TasksSeekOutDiff diff(ttl[i]);
			task_list_after.push_back(diff);
		}
		// 计算前后两个序列的差集
		TasksSeekOutDiffList diff_list;
		TasksSeekOutDiffList::iterator it;
		diff_list.resize(a_Max(count_before, count_after));
		it = std::set_difference(task_list_after.begin(), task_list_after.end(), 
			task_list_before.begin(), task_list_before.end(), diff_list.begin());

		if (it != diff_list.begin()) {
			m_TasksToPush.insert(m_TasksToPush.end(), diff_list.begin(), it);
			m_TasksCanDeliver.swap(ttl);
			CECUIHelper::OnPushNewTasksCanDeliver();
		}
	}
}

bool ATaskTemplMan::IsTaskToPush(int id)
{
	size_t count = m_TasksToPush.size();
	for (size_t i = 0; i < count; ++i) {
		ATaskTempl* pTempl = m_TasksToPush[i].task;
		if (pTempl && (int)pTempl->m_ID == id) return true;
	}
	return false;
}

void ATaskTemplMan::CheckTitleTask(TaskInterface* pTask)
{
	unsigned long ulCurTime = TaskInterface::GetCurTime();
	ActiveTaskList* pLst = (ActiveTaskList*)pTask->GetActiveTaskList();
	TaskTemplMap::iterator iter = m_TitleTaskMap.begin();
	ATaskTempl* pTempl;
	for (; iter != m_TitleTaskMap.end(); ++iter){
		pTempl = iter->second;
		if (pLst->GetEntry(pTempl->m_ID) || pTempl->CheckPrerequisite(pTask, pLst, ulCurTime) == 0){
			CECUIHelper::AddTitleChallenge(pTempl->m_ID);
		} else {
			CECUIHelper::RemoveTitleChallenge(pTempl->m_ID);
		}
	}
}
void ATaskTemplMan::UpdateStatus(TaskInterface* pTask)
{
	// 因为CheckTitle依赖与称号数据，所以直到获取到称号数据之前都不能调用任务借口
	if (!pTask->IsTitleDataReady()) return;
	CheckAutoDelv(pTask);
	if (CECUIConfig::Instance().GetGameUI().bEnableTitle)
		CheckTitleTask(pTask);
	UpdateTasksSeekOutDiff(pTask);
}

ATaskTempl::TREASURE_DISTANCE_LEVEL ATaskTemplMan::CheckTreasureDistance(TaskInterface* pTask) 
{
	ATaskTempl::TREASURE_DISTANCE_LEVEL ret(ATaskTempl::DISTANCE_FAR_FAR_AWAY);
	ActiveTaskList* pLst = (ActiveTaskList*)pTask->GetActiveTaskList();
	ActiveTaskEntry* aEntries = pLst->m_TaskEntries;
	for (unsigned char i = 0; i < pLst->m_uTaskCount; ++i) {
		ActiveTaskEntry& CurEntry = aEntries[i];
		if (!CurEntry.m_ulTemplAddr)
			continue;
		const ATaskTempl* pTemp = CurEntry.GetTempl();
		if (pTemp->m_enumMethod == enumTMReachTreasureZone){
			float pos[3];
			unsigned long ulWorldId = pTask->GetPos(pos);
			if (ulWorldId == 1)
			{
				unsigned short uTreasureLocIndex = CurEntry.m_iUsefulData1 & 0xFFFF;
				float fTreasureLocX = pTemp->m_TreasureStartZone.x + (uTreasureLocIndex % pTemp->m_ucZonesNumX - 1) * pTemp->m_ucZoneSide;
				float fTreasureLocZ = pTemp->m_TreasureStartZone.z + (uTreasureLocIndex / pTemp->m_ucZonesNumX) * pTemp->m_ucZoneSide;
				fTreasureLocX += pTemp->m_ucZoneSide / 2;
				fTreasureLocZ += pTemp->m_ucZoneSide / 2;
				A3DVECTOR3 vec_distance(pos[0] - fTreasureLocX, 0, pos[2] - fTreasureLocZ);
				float distance = vec_distance.MagnitudeH();
				float dist2 = 1.5f * pTemp->m_ucZoneSide;
				float dist3 = 3.f * pTemp->m_ucZoneSide;
				float dist4 = 6.f * pTemp->m_ucZoneSide;
				float dist5 = 9.f * pTemp->m_ucZoneSide;
				ATaskTempl::TREASURE_DISTANCE_LEVEL level(ATaskTempl::DISTANCE_FAR_FAR_AWAY);
				
				if (distance <= dist2) { 
					if (distance <= pTemp->m_ucZoneSide / 2){
						if (pTemp->IsValidState())
						{
							pTemp->m_uValidCount = TASK_MAX_VALID_COUNT;
							_notify_svr(pTask, TASK_CLT_NOTIFY_CHECK_FINISH, (unsigned short)pTemp->GetID());
						}
					}
					level = ATaskTempl::DISTANCE_VERY_NEAR;
				} else {
					pTemp->ClearValidCount();
					if(distance > dist2  && distance <= dist3) level = ATaskTempl::DISTANCE_NEAR;
					else if(distance > dist3  && distance <= dist4) level = ATaskTempl::DISTANCE_MEDIUM;
					else if(distance > dist4  && distance <= dist5) level = ATaskTempl::DISTANCE_FAR;
					else level = ATaskTempl::DISTANCE_FAR_FAR_AWAY;
				}
				if (level > ret) ret = level;
			}
		}
	}
	
	return ret;
}
#endif

/*
 *	export functions
 */
const NPC_TASK_OUT_SERVICE* ATaskTemplMan::GetStorageTaskEssence(unsigned int idStorage)
{
	const NPC_TASK_OUT_SERVICE* ret = NULL;
	abase::hash_map<unsigned int, unsigned int>::iterator iter = m_StorageEssenseMap.find(idStorage);
	if (iter != m_StorageEssenseMap.end()) {
		DATA_TYPE dt;
		const NPC_TASK_OUT_SERVICE* pConfig = (const NPC_TASK_OUT_SERVICE*)m_pEleDataMan->get_data_ptr(iter->second, ID_SPACE_ESSENCE, dt);
		if (dt == DT_NPC_TASK_OUT_SERVICE && pConfig != NULL) {
			ret = pConfig;
		}
	}
	return ret;
}
const TASK_DICE_BY_WEIGHT_CONFIG* ATaskTemplMan::GetWeightTasksEssence(unsigned int idStorage)
{
	const TASK_DICE_BY_WEIGHT_CONFIG* ret = NULL;
	abase::hash_map<unsigned int, unsigned int>::iterator iter = m_WeightEssenseMap.find(idStorage);
	if (iter != m_WeightEssenseMap.end()) {
		DATA_TYPE dt;
		const TASK_DICE_BY_WEIGHT_CONFIG* pConfig = (const TASK_DICE_BY_WEIGHT_CONFIG*)m_pEleDataMan->get_data_ptr(iter->second, ID_SPACE_CONFIG, dt);
		if (dt == DT_TASK_DICE_BY_WEIGHT_CONFIG && pConfig != NULL) {
			ret = pConfig;
		}
	}
	return ret;
}
unsigned long ATaskTemplMan::GetStoragePremItemID(unsigned int idStorage)
{
	const NPC_TASK_OUT_SERVICE* pEssence = GetStorageTaskEssence(idStorage);
	unsigned long ret = 0;
	if (pEssence) ret = pEssence->storage_open_item;
	return ret;
}
int ATaskTemplMan::GetStorageRefreshPerDay(unsigned int idStorage)
{
	const NPC_TASK_OUT_SERVICE* pEssence = GetStorageTaskEssence(idStorage);
	int ret = 0;
	if (pEssence) ret = pEssence->storage_refresh_per_day;
	return ret;
}
int ATaskTemplMan::GetMaxDeliverPerDay(unsigned int idStorage)
{
	const NPC_TASK_OUT_SERVICE* pEssence = GetStorageTaskEssence(idStorage);
	int ret = 0;
	if (pEssence) ret = pEssence->storage_deliver_per_day;
	return ret;
}
bool ATaskTemplMan::IsReceiveMaxStorageTasksPerDay(unsigned int idStorage,unsigned char receiveCount)
{
	const NPC_TASK_OUT_SERVICE* pEssence = GetStorageTaskEssence(idStorage);
	bool ret = false;
	if (pEssence) ret = pEssence->storage_deliver_per_day <= receiveCount;
	return ret;
}
bool ATaskTemplMan::HasStorageTaskDeliverLimit(unsigned int idStorage)
{
	const NPC_TASK_OUT_SERVICE* pEssence = GetStorageTaskEssence(idStorage);
	bool ret = false;
	if (pEssence) ret = pEssence->storage_deliver_per_day != 0;// 默认0代表不限制每天发放次数
	return ret;
}
void ATaskTemplMan::Init(elementdataman* pMan)
{
	m_pEleDataMan = pMan;
}

bool ATaskTemplMan::InitStorageTask()
{
	// 库任务相关
	m_StorageEssenseMap.clear();
	m_WeightEssenseMap.clear();
	
	// 初始化任务仓库列表
	DATA_TYPE dt;
	int id = m_pEleDataMan->get_first_data_id(ID_SPACE_ESSENCE, dt);
	
	while (id) {
		if (dt == DT_NPC_TASK_OUT_SERVICE) {
			NPC_TASK_OUT_SERVICE* pData = (NPC_TASK_OUT_SERVICE*)m_pEleDataMan->get_data_ptr(id, ID_SPACE_ESSENCE, dt);
			
			if (pData->storage_id == 0) {
				id = m_pEleDataMan->get_next_data_id(ID_SPACE_ESSENCE, dt);
				continue;
			}
			
			if (pData->storage_id > TASK_STORAGE_COUNT) {
				// 库任务号超过限制
				return false;
			}
			
			IdEssenceMap::iterator iter = m_StorageEssenseMap.find(pData->storage_id);
			if (iter != m_StorageEssenseMap.end())
				return false; // 库任务号重复
			m_StorageEssenseMap[pData->storage_id] = pData->id;
			size_t sTasks = SIZE_OF_ARRAY(pData->id_tasks);
			for (size_t i = 0; i < sTasks; i++) {
				if (pData->id_tasks[i]) {			
					m_StorageTaskMap[pData->id_tasks[i]] = pData->storage_id;
				}
			}
		}
		id = m_pEleDataMan->get_next_data_id(ID_SPACE_ESSENCE, dt);
	}
	
	id = m_pEleDataMan->get_first_data_id(ID_SPACE_CONFIG, dt);	
	while (id) {
		if (dt == DT_TASK_DICE_BY_WEIGHT_CONFIG) {
			TASK_DICE_BY_WEIGHT_CONFIG* pData = (TASK_DICE_BY_WEIGHT_CONFIG*)m_pEleDataMan->get_data_ptr(id, ID_SPACE_CONFIG, dt);
			
			if (pData->storage_id == 0)	{
				id = m_pEleDataMan->get_next_data_id(ID_SPACE_CONFIG, dt);
				continue;
			}
			
			if (pData->storage_id > TASK_STORAGE_COUNT) {
				return false;
			}
			
			IdEssenceMap::iterator iter = m_WeightEssenseMap.find(pData->storage_id);
			if (iter != m_WeightEssenseMap.end())
				return false;
			m_WeightEssenseMap[pData->storage_id] = pData->id;
		}
		id = m_pEleDataMan->get_next_data_id(ID_SPACE_CONFIG, dt);
	}
	return true;
}

void ATaskTemplMan::Release()
{
	TaskTemplMap::iterator it = m_TaskTemplMap.begin();
	for (; it != m_TaskTemplMap.end(); ++it)
	{
		LOG_DELETE(it->second);
	}

	m_TaskTemplMap.clear();
	m_AllTemplMap.clear();
	m_AutoDelvMap.clear();
	m_DeathTrigMap.clear();
	m_ProtectNPCMap.clear();
	m_PQTemplMap.clear();
	m_SkillTaskLst.clear();
	m_DynTaskMap.clear();
	m_TitleTaskMap.clear();
	m_ExlusiveAwardTaskMap.clear();
#ifdef TASK_TEMPL_EDITOR
	m_TreeItemMap.clear();
	m_TreeTopItemMap.clear();
#endif

	m_ulDynTasksTimeMark = 0;
	m_ulDynTasksDataSize = 0;
	LOG_DELETE_ARR(m_pDynTasksData);
	m_pDynTasksData = 0;
	m_ulNPCInfoDataSize = 0;
	LOG_DELETE_ARR(m_pNPCInfoData);
	m_pNPCInfoData = 0;
	m_NPCInfoMap.clear();

#ifdef _TASK_CLIENT
	TaskInterface::SetFinishDlgShowTime(0);
	m_bDynTasksVerified = false;
	ClearSpecailAward();
	m_TasksCanSeekOut.clear();
	m_TasksCanDeliver.clear();
	m_TasksToPush.clear();
#endif

#ifdef _ELEMENTCLIENT
	_task_err.Release();
#endif
}

void ATaskTemplMan::LoadAllFromDir(const char* szDir, bool bLoadDescript)
{
}

int ATaskTemplMan::LoadTasksFromPackForTimeTool(const char* szPackPath, bool bLoadDescript,unsigned long& lPackVersion,unsigned long& lToolVersion)
{
	FILE* fp = fopen(szPackPath, "rb");
	if (fp == NULL) return 0;
	
	TASK_PACK_HEADER tph;
	
	fread(&tph, sizeof(tph), 1, fp);

	if (tph.magic != TASK_PACK_MAGIC)
	{
		return -1;
	}
	else if (tph.version != _task_templ_cur_version)
	{
		lPackVersion = tph.version;
		lToolVersion = _task_templ_cur_version;
		return tph.version > _task_templ_cur_version ? -2 : -3;
	}
	

	if (tph.item_count == 0) return -4;
	
	long* pOffs = new long[tph.item_count];
	g_ulNewCount++;
	
	fread(pOffs, sizeof(long), tph.item_count, fp);
	
	for (unsigned long i = 0; i < tph.item_count; i++)
	{
		if (fseek(fp, pOffs[i], SEEK_SET) != 0)
			continue;
		
		ATaskTempl* pTempl = new ATaskTempl;
		g_ulNewCount++;
		
		if (!pTempl->LoadFromBinFile(fp))
		{
			TaskInterface::WriteLog(0, pTempl->m_ID, 0, "Cant Load Task");
			LOG_DELETE(pTempl);
			continue;
		}
		
		AddOneTaskTempl(pTempl);
		TaskInterface::WriteLog(0, pTempl->m_ID, 2, "LoadTask");
	}
	
	char log[1024];
	sprintf(log, "LoadTask, Count = %d", m_TaskTemplMap.size());
	TaskInterface::WriteLog(0, 0, 2, log);
	
	LOG_DELETE_ARR(pOffs);
	fclose(fp);
	
#ifndef _TASK_CLIENT
	UpdateTimeLimitCheckList();
#endif
	
	return 1;
}

bool ATaskTemplMan::LoadTasksFromPack(const char* szPackPath, bool bLoadDescript)
{
	TaskInterface::WriteLog(0, 0, 2, "LoadPack begin");

	FILE* fp = fopen(szPackPath, "rb");
	if (fp == NULL) return false;

	TASK_PACK_HEADER tph;

	fread(&tph, sizeof(tph), 1, fp);

	if (tph.magic != TASK_PACK_MAGIC
	 || tph.version != _task_templ_cur_version)
		return false;

	if (tph.item_count == 0) return true;

	long* pOffs = new long[tph.item_count];
	g_ulNewCount++;

	fread(pOffs, sizeof(long), tph.item_count, fp);

	for (unsigned long i = 0; i < tph.item_count; i++)
	{
		if (fseek(fp, pOffs[i], SEEK_SET) != 0)
			continue;

		ATaskTempl* pTempl = new ATaskTempl;
		g_ulNewCount++;

		if (!pTempl->LoadFromBinFile(fp))
		{
			TaskInterface::WriteLog(0, pTempl->m_ID, 0, "Cant Load Task");
			LOG_DELETE(pTempl);
			continue;
		}

		AddOneTaskTempl(pTempl);
		TaskInterface::WriteLog(0, pTempl->m_ID, 2, "LoadTask");
	}

	char log[1024];
	sprintf(log, "LoadTask, Count = %d", m_TaskTemplMap.size());
	TaskInterface::WriteLog(0, 0, 2, log);

	LOG_DELETE_ARR(pOffs);
	fclose(fp);

#ifndef _TASK_CLIENT
	UpdateTimeLimitCheckList();
#else
	SortTasksCanSeekOut();
#endif

#ifdef _ELEMENTCLIENT
	_task_err.Release();
	_task_err.Init("Configs\\task_err.txt", true);
#endif

	return true;
}

void ATaskTemplMan::AddTaskToMap(ATaskTempl* pTempl)
{
	if (pTempl->m_enumMethod == enumTMProtectNPC && pTempl->m_ulNPCToProtect)
		m_ProtectNPCMap[pTempl->m_ulNPCToProtect] = pTempl;

	m_AllTemplMap[pTempl->m_ID] = pTempl;
	ATaskTempl* pChild = pTempl->m_pFirstChild;

	while (pChild)
	{
		AddTaskToMap(pChild);
		pChild = pChild->m_pNextSibling;
	}
}

void ATaskTemplMan::AddOneTaskTempl(ATaskTempl* pTask)
{
	TaskTemplMap::iterator it = m_TaskTemplMap.find(pTask->m_ID);
	if (it != m_TaskTemplMap.end())
	{
		TaskInterface::WriteLog(0, pTask->m_ID, 0, "Dup Task Found");
		return;
	}

	m_TaskTemplMap[pTask->m_ID] = pTask;

	if (pTask->m_bDeathTrig) m_DeathTrigMap[pTask->m_ID] = pTask;
	else if (pTask->m_bAutoDeliver) m_AutoDelvMap[pTask->m_ID] = pTask;

	if (pTask->m_bPQTask) m_PQTemplMap[pTask->m_ID] = pTask;

	if (pTask->m_bSkillTask) m_SkillTaskLst.push_back(pTask);

	if (pTask->m_DynTaskType)
	{
		TaskTemplMap::iterator itDyn = m_DynTaskMap.find(pTask->m_ID);
		if (itDyn != m_DynTaskMap.end())
			TaskInterface::WriteLog(0, pTask->m_ID, 0, "Dup Dyn Task Found");

		m_DynTaskMap[pTask->m_ID] = pTask;
	}
	if (pTask->m_bDisplayInTitleTaskUI)
		m_TitleTaskMap[pTask->m_ID] = pTask;
	if (pTask->m_bAutoDeliver && pTask->m_bDisplayInExclusiveUI)
		m_ExlusiveAwardTaskMap[pTask->m_ID] = pTask;

#ifdef _TASK_CLIENT
	if (pTask->m_ulDelvNPC != 0 && pTask->m_bCanSeekOut)
		m_TasksCanSeekOut.push_back(pTask);
#endif

	AddTaskToMap(pTask);
}

ATaskTempl* ATaskTemplMan::LoadOneTaskTempl(const char* szPath, bool bLoadDescript)
{
	ATaskTempl* pTask = new ATaskTempl;
	g_ulNewCount++;

	if (!pTask->LoadFromTextFile(szPath, bLoadDescript))
	{
		LOG_DELETE(pTask);
		return 0;
	}

	AddOneTaskTempl(pTask);
	return pTask;
}

void ATaskTemplMan::DelOneTaskTempl(ATaskTempl* pTask)
{
	TaskTemplMap::iterator it = m_TaskTemplMap.find(pTask->m_ID);
	if (it != m_TaskTemplMap.end()) m_TaskTemplMap.erase(pTask->m_ID);
	LOG_DELETE(pTask);
}

ATaskTempl* ATaskTemplMan::GetTopTaskByID(unsigned long ulID)
{
	TaskTemplMap::iterator it = m_TaskTemplMap.find(ulID);
	if (it == m_TaskTemplMap.end()) return 0;
	return it->second;
}

ATaskTempl* ATaskTemplMan::GetTaskTemplByID(unsigned long ulID)
{
	TaskTemplMap::iterator it = m_AllTemplMap.find(ulID);
	if (it == m_AllTemplMap.end()) return 0;
	return it->second;
}

const ATaskTempl* ATaskTemplMan::GetProtectNPCTask(unsigned long ulNPCId)
{
	TaskTemplMap::iterator it = m_ProtectNPCMap.find(ulNPCId);
	if (it != m_ProtectNPCMap.end()) return it->second;
	return NULL;
}

void ATaskTemplMan::RemoveActiveStorageTask(StorageTaskList* pLst, unsigned long id)
{
	unsigned int set_id = GetTaskTemplMan()->GetTaskStorageId(id);
	
	if (set_id)
	{
		unsigned short* arr = pLst->m_Storages[set_id-1];
		int i;
		
		for (i = 0; i < TASK_STORAGE_LEN; i++)
		{
			if (arr[i] == (unsigned short)id)
			{
				arr[i] = 0;
				break;
			}
		}
	}
}
void mount_task_out_service(const ATaskTempl* task, NPC_TASK_OUT_SERVICE* svr)
{
	if (task->IsAutoDeliver())
		return;

	if (task->m_pParent && !task->m_pParent->m_bChooseOne)
		return;

	for (size_t i = 0; i < SIZE_OF_ARRAY(svr->id_tasks); i++)
	{
		if (svr->id_tasks[i]) continue;
		svr->id_tasks[i] = task->GetID();
		break;
	}

	const ATaskTempl* child = task->m_pFirstChild;
	while (child)
	{
		mount_task_out_service(child, svr);
		child = child->m_pNextSibling;
	}
}

void mount_task_in_service(const ATaskTempl* task, NPC_TASK_IN_SERVICE* svr)
{
	if (task->m_enumFinishType == enumTFTNPC)
	{
		for (size_t i = 0; i < SIZE_OF_ARRAY(svr->id_tasks); i++)
		{
			if (svr->id_tasks[i]) continue;
			svr->id_tasks[i] = task->GetID();
			break;
		}
	}

	const ATaskTempl* child = task->m_pFirstChild;
	while (child)
	{
		mount_task_in_service(child, svr);
		child = child->m_pNextSibling;
	}
}

void ATaskTemplMan::UpdateDynDataNPCService()
{
	assert(m_pEleDataMan);

	DATA_TYPE dt;
	NPC_TASK_IN_SERVICE* service = (NPC_TASK_IN_SERVICE*)m_pEleDataMan->get_data_ptr(
		DYN_TASK_VERIFY_SVR,
		ID_SPACE_ESSENCE,
		dt
		);

	if (!service || dt != DT_NPC_TASK_IN_SERVICE)
	{
		char log[1024];
		sprintf(log, "UpdateDynDataNPCService, wrong service, dt = %d", dt);
		TaskInterface::WriteLog(0, 0, 0, log);
		return;
	}

	NPC_TASK_OUT_SERVICE* deliver = (NPC_TASK_OUT_SERVICE*)m_pEleDataMan->get_data_ptr(
		DYN_TASK_DELIVER_SVR,
		ID_SPACE_ESSENCE,
		dt
		);

	if (!deliver || dt != DT_NPC_TASK_OUT_SERVICE)
	{
		char log[1024];
		sprintf(log, "UpdateDynDataNPCService, wrong service, dt = %d", dt);
		TaskInterface::WriteLog(0, 0, 0, log);
		return;
	}

	memset(deliver->id_tasks, 0, sizeof(deliver->id_tasks));
	memset(service->id_tasks, 0, sizeof(service->id_tasks));

	TaskTemplMap::iterator it = m_DynTaskMap.begin();
	for (; it != m_DynTaskMap.end(); ++it)
	{
		const ATaskTempl* p = it->second;
		if (p && p->m_DynTaskType != enumDTTGiftCard) {
			mount_task_out_service(p, deliver);
			mount_task_in_service(p, service);
		}
	}
}

bool ATaskTemplMan::UnmarshalDynTasks(const char* data, size_t data_size, bool header_only)
{
	if (data_size < sizeof(DYN_TASK_PACK_HEADER))
	{
		TaskInterface::WriteLog(0, 0, 0, "UnmarshalDynTasks, wrong size");
		return false;
	}

	const char* p = data;
	DYN_TASK_PACK_HEADER* header = (DYN_TASK_PACK_HEADER*)p;
	p += sizeof(DYN_TASK_PACK_HEADER);

	if (header->version != DYN_TASK_CUR_VERSION)
	{
		TaskInterface::WriteLog(0, 0, 0, "UnmarshalDynTasks, wrong version");
		return false;
	}

	if (header->pack_size != data_size)
	{
		TaskInterface::WriteLog(0, 0, 0, "UnmarshalDynTasks, wrong header");
		return false;
	}

	m_ulDynTasksTimeMark = header->time_mark;

	if (header_only)
		return true;

	for (unsigned short i = 0; i < header->task_count; i++)
	{
		ATaskTempl* pTempl = new ATaskTempl;
		g_ulNewCount++;

		p += pTempl->UnmarshalDynTask(p);
		AddOneTaskTempl(pTempl);

		TaskInterface::WriteLog(0, pTempl->GetID(), 2, "LoadDynTask");
	}
#ifdef _TASK_CLIENT
	SortTasksCanSeekOut();
#endif
	assert(p == data + data_size);
	return true;
}

bool ATaskTemplMan::UnmarshalNPCInfo(const char* data, size_t data_size, bool header_only)
{
	if (data_size < sizeof(TASK_NPC_PACK_HEADER))
	{
		TaskInterface::WriteLog(0, 0, 0, "UnmarshalNPCInfo, wrong size");
		return false;
	}

	const char* p = data;
	TASK_NPC_PACK_HEADER* header = (TASK_NPC_PACK_HEADER*)p;
	p += sizeof(TASK_NPC_PACK_HEADER);

	if (header->version != TASK_NPC_INFO_VERSION)
	{
		TaskInterface::WriteLog(0, 0, 0, "UnmarshalNPCInfo, wrong version");
		return false;
	}

	if (header->pack_size != data_size)
	{
		TaskInterface::WriteLog(0, 0, 0, "UnmarshalNPCInfo, wrong header");
		return false;
	}

	m_ulNPCInfoTimeMark = header->time_mark;

	if (header_only)
		return true;

	const NPC_INFO* pInfos = (const NPC_INFO*)p;
	for (unsigned short i = 0; i < header->npc_count; i++)
	{
		const NPC_INFO& info = pInfos[i];
		m_NPCInfoMap[info.id] = info;
	}

	return true;
}

bool ATaskTemplMan::LoadDynTasksFromPack(const char* szPath)
{
	TaskInterface::WriteLog(0, 0, 2, "LoadDynPack begin");

	FILE* fp = fopen(szPath, "rb");
	if (fp == NULL)
	{
		TaskInterface::WriteLog(0, 0, 0, "LoadDynTasksFromPack, no such file");
		return false;
	}

	fseek(fp, 0, SEEK_END);
	size_t sz = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	if (sz == 0)
	{
		fclose(fp);
		return false;
	}

	char* buf = new char[sz];
	g_ulNewCount++;
	fread(buf, 1, sz, fp);
	fclose(fp);

	if (!UnmarshalDynTasks(buf, sz, false))
	{
		LOG_DELETE_ARR(buf);
		return false;
	}

#ifdef _TASK_CLIENT
	LOG_DELETE_ARR(buf);
#else
	m_pDynTasksData = buf;
	m_ulDynTasksDataSize = sz;
	UpdateDynDataNPCService();
#endif

	return true;
}

bool ATaskTemplMan::LoadNPCInfoFromPack(const char* szPath)
{
	FILE* fp = fopen(szPath, "rb");
	if (fp == NULL)
	{
		TaskInterface::WriteLog(0, 0, 0, "LoadNPCInfoFromPack, no such file");
		return false;
	}

	fseek(fp, 0, SEEK_END);
	size_t sz = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	if (sz == 0)
	{
		fclose(fp);
		return false;
	}

	char* buf = new char[sz];
	g_ulNewCount++;
	fread(buf, 1, sz, fp);
	fclose(fp);

	if (!UnmarshalNPCInfo(buf, sz, false))
	{
		LOG_DELETE_ARR(buf);
		return false;
	}

#ifdef _TASK_CLIENT
	LOG_DELETE_ARR(buf);
#else
	m_pNPCInfoData = buf;
	m_ulNPCInfoDataSize = sz;
#endif

	return true;
}
