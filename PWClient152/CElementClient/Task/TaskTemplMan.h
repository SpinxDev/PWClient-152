#ifndef _TASKTEMPLMAN_H_
#define _TASKTEMPLMAN_H_

#include "hashmap.h"
#include "vector.h"
#include "TaskTempl.h"

#ifdef _TASK_CLIENT
#include <vector>
#endif

class elementdataman;
struct NPC_TASK_OUT_SERVICE;
struct TASK_DICE_BY_WEIGHT_CONFIG;
class _task_hash_func
{
public:
	inline unsigned long operator() (unsigned long data) const { return data; }
};

class ATaskTempl;
typedef abase::hash_map<unsigned long, ATaskTempl*, _task_hash_func> TaskTemplMap;
typedef abase::vector<ATaskTempl*> TaskTemplLst;
typedef abase::hash_map<unsigned long, NPC_INFO, _task_hash_func> TaskNPCInfoMap;

#ifdef TASK_TEMPL_EDITOR
#include <afxcmn.h>
struct TreeData 
{
	HTREEITEM m_hItem;
	task_char m_szName[MAX_TASK_NAME_LEN];
	char m_DynTaskType;
	bool m_bIsClassType;
	
	TreeData(HTREEITEM hItem,const wchar_t* szName,char DynType = 0, bool bClassType = false):
	m_hItem(hItem),m_DynTaskType(DynType), m_bIsClassType(bClassType)
	{
		memset(m_szName, 0, sizeof(m_szName));
		wcsncpy((wchar_t*)m_szName, szName, MAX_TASK_NAME_LEN-1);
	}
	
	TreeData& operator = (const TreeData& src)
	{
		m_hItem = src.m_hItem;
		m_DynTaskType = src.m_DynTaskType;
		m_bIsClassType = src.m_bIsClassType;
		memset(m_szName, 0, sizeof(m_szName));
		wcsncpy((wchar_t*)m_szName, src.GetName(), MAX_TASK_NAME_LEN-1);
		return *this;
	}
	
	TreeData():m_hItem(0),m_DynTaskType(0),m_bIsClassType(false)
	{}
	
	const wchar_t* GetName() const { return (const wchar_t*)m_szName; }
	
	void SetName(const wchar_t* szName)
	{
		memset(m_szName, 0, sizeof(m_szName));
		wcsncpy((wchar_t*)m_szName, szName, MAX_TASK_NAME_LEN-1);
	}
	
};
typedef abase::hash_map<unsigned long, TreeData,_task_hash_func> TreeItemMap;
#endif

struct TaskInterface;

class ATaskTemplMan
{
public:
	ATaskTemplMan();
	~ATaskTemplMan();

protected:
	TaskTemplMap	m_TaskTemplMap;
	TaskTemplMap	m_AllTemplMap;
	TaskTemplMap	m_AutoDelvMap;
	TaskTemplMap	m_DeathTrigMap;
	TaskTemplMap	m_ProtectNPCMap;
	TaskTemplMap	m_TitleTaskMap;
	TaskTemplMap	m_ExlusiveAwardTaskMap;

	abase::hash_map<int, int>	m_StorageTaskMap;
	typedef abase::hash_map<unsigned int, unsigned int> IdEssenceMap;
	IdEssenceMap	m_StorageEssenseMap;
	IdEssenceMap	m_WeightEssenseMap;
	
	TaskTemplMap	m_PQTemplMap;

	TaskTemplLst	m_SkillTaskLst;
	elementdataman*	m_pEleDataMan;
#ifdef TASK_TEMPL_EDITOR
	TreeItemMap		m_TreeItemMap;
	TreeItemMap		m_TreeTopItemMap;
#endif
#ifdef _TASK_CLIENT
	char			m_szDynPackPath[512];
	bool			m_bDynTasksVerified;
	special_award	m_SpecialAward;
	TaskTemplLst	m_TasksCanSeekOut;
	TaskTemplLst	m_TasksCanDeliver;
	struct TasksSeekOutDiff
	{
		ATaskTempl* task;
		TasksSeekOutDiff():task(NULL){}
		TasksSeekOutDiff(ATaskTempl* p):task(p){}

		bool operator < (const TasksSeekOutDiff& rhs) const;
		bool operator == (const TasksSeekOutDiff& rhs) const { return task == rhs.task; }
	};
	typedef std::vector<TasksSeekOutDiff> TasksSeekOutDiffList;
	TasksSeekOutDiffList m_TasksToPush;
#else
	TaskTemplLst	m_TmLmtChkLst;
#endif

	TaskTemplMap	m_DynTaskMap;
	unsigned long	m_ulDynTasksTimeMark;
	char*			m_pDynTasksData;
	unsigned long	m_ulDynTasksDataSize;

	char*			m_pNPCInfoData;
	unsigned long	m_ulNPCInfoDataSize;
	unsigned long	m_ulNPCInfoTimeMark;
	TaskNPCInfoMap	m_NPCInfoMap;

protected:
	void AddTaskToMap(ATaskTempl* pTempl);
	void UpdateDynDataNPCService();
	bool UpdateOneStorage(TaskInterface* pTask,StorageTaskList* pStorageLst, unsigned long ulCurTime, int idStorage,bool init_refresh);
	bool PickTaskByProbability(TaskInterface* pTask,StorageTaskList* pStorageLst, int idStorage, unsigned long ulCurTime, const NPC_TASK_OUT_SERVICE* pEssence);
	bool PickTaskByWeight(TaskInterface* pTask,StorageTaskList* pStorageLst, int idStorage, unsigned long ulCurTime, const TASK_DICE_BY_WEIGHT_CONFIG* pEssence, bool bDebug = false);

#ifdef _TASK_CLIENT
	void CheckAutoDelv(TaskInterface* pTask);
	void CheckSpecialAwardMask(TaskInterface* pTask);
#else
	void UpdateTimeLimitCheckList();
#endif
public:
#ifdef _ELEMENTCLIENT
	void UpdateStatus(TaskInterface* pTask);
	void CheckTitleTask(TaskInterface* pTask);
	ATaskTempl::TREASURE_DISTANCE_LEVEL CheckTreasureDistance(TaskInterface* pTask);
#endif 
	elementdataman* GetEleDataMan() { return m_pEleDataMan; }
	ATaskTempl* GetTopTaskByID(unsigned long ulID);
	ATaskTempl* GetTaskTemplByID(unsigned long ulID);
	TaskTemplMap& GetAllTemplMap() { return m_AllTemplMap; }
	TaskTemplMap& GetTopTemplMap() { return m_TaskTemplMap; }
	TaskTemplMap& GetPQTemplMap() { return m_PQTemplMap; }
	TaskTemplMap& GetAutoTemplMap() { return m_AutoDelvMap; }
	TaskTemplMap& GetDynTemplMap() { return m_DynTaskMap; }
	TaskTemplMap& GetExclusiveAwardMap() { return m_ExlusiveAwardTaskMap; }
	ATaskTempl* LoadOneTaskTempl(const char* szPath, bool bLoadDescript = true);
	void AddOneTaskTempl(ATaskTempl* pTask);
	void DelOneTaskTempl(ATaskTempl* pTask);
	const ATaskTempl* GetProtectNPCTask(unsigned long ulNPCId);
	bool UnmarshalDynTasks(const char* data, size_t data_size, bool header_only);
	bool UnmarshalNPCInfo(const char* data, size_t data_size, bool header_only);
	const NPC_INFO* GetTaskNPCInfo(unsigned long ulNPC)
	{
		TaskNPCInfoMap::iterator it = m_NPCInfoMap.find(ulNPC);
		if (it == m_NPCInfoMap.end()) return 0;
		return &it->second;
	}

	// 库任务相关
	bool IsStorageTask(unsigned long id)
	{
		return m_StorageTaskMap.find(id) != m_StorageTaskMap.end();
	}
	unsigned long GetStoragePremItemID(unsigned int idStorage);
	int GetStorageRefreshPerDay(unsigned int idStorage);
	
	void RemoveActiveStorageTask(StorageTaskList* pLst, unsigned long id);
	const NPC_TASK_OUT_SERVICE* GetStorageTaskEssence(unsigned int idStorage);
	const TASK_DICE_BY_WEIGHT_CONFIG* GetWeightTasksEssence(unsigned int idStorage);
	
	unsigned int GetTaskStorageId(unsigned long id)
	{
		// id从1开始
		abase::hash_map<int, int>::iterator it = m_StorageTaskMap.find(id);
		return it == m_StorageTaskMap.end() ? 0 : it->second;
	}


	int GetMaxDeliverPerDay(unsigned int idStorage);
	bool IsReceiveMaxStorageTasksPerDay(unsigned int idStorage,unsigned char receiveCount);
	bool HasStorageTaskDeliverLimit(unsigned int idStorage);


#ifdef TASK_TEMPL_EDITOR
	HTREEITEM GetTopTreeItemByID(unsigned long ulID);
	HTREEITEM GetTreeItemByID(unsigned long ulID);
	TreeItemMap& GetAllTreeItemMap() { return m_TreeItemMap; }
	TreeItemMap& GetTopTreeItemMap(){return m_TreeTopItemMap;}
	void AddOneTreeItem(ATaskTempl* pTask,TreeData& sData,bool bIsTopItem);
	bool SaveTasksToPack2(const char* szPackPath);
	bool SaveDynTasksToPack2(const char* szPath, bool bMarshalAll = false);

	bool SaveTasksToPack(const char* szPackPath);
	bool SaveDynTasksToPack(const char* szPath, bool bMarshalAll = false);
	bool SaveNPCInfoToPack(const char* szPath);
	TaskNPCInfoMap& GetNPCInfoMap() { return m_NPCInfoMap; }
#endif

	bool CanGiveUpTask(unsigned long ulTaskId);
	void OnForgetLivingSkill(TaskInterface* pTask);

	// process part
#ifdef _TASK_CLIENT
	void GetTitleTasks(TaskInterface* pTask, TaskTemplLst& lst);
	void GetAvailableTasks(TaskInterface* pPlayer, TaskTemplLst& lst);
	void ManualTrigTask(TaskInterface* pTask, unsigned long ulTask);
	void ForceGiveUpTask(TaskInterface* pTask, unsigned long ulTask);
	void ForceRemoveFinishTask(TaskInterface* pTask, unsigned long ulTask);
	bool IsDynTasksVerified() const { return m_bDynTasksVerified; }
	void SetDynTasksVerified(bool b) { m_bDynTasksVerified = b; }
	void OnDynTasksTimeMark(TaskInterface* pTask, unsigned long ulTimeMark, unsigned short version);
	void OnDynTasksData(TaskInterface* pTask, const void* data, size_t sz, bool ended);
	void OnStorageData(TaskInterface* pTask, const void* data);
	void OnSpecialAward(const special_award* p,TaskInterface* pTask);
	void VerifyDynTasksPack(const char* szPath);
	const special_award* GetSpecialAward() const { return &m_SpecialAward; }
	void ClearSpecailAward() { memset(&m_SpecialAward, 0, sizeof(m_SpecialAward)); }
	void SortTasksCanSeekOut();
	void UpdateTasksSeekOutDiff(TaskInterface* pTask);
	bool IsTaskToPush(int id);
	void ClearTasksToPush() { m_TasksToPush.clear(); }
	bool HasTaskToPush() { return !m_TasksToPush.empty(); }
#else
	void CheckDeathTrig(TaskInterface* pTask);
	void OnTaskCheckAllTimeLimits(unsigned long ulCurTime);
	void OnTaskGetDynTasksTimeMark(TaskInterface* pTask);
	void OnTaskGetDynTasksData(TaskInterface* pTask);
	void OnTaskGetSpecialAward(TaskInterface* pTask);
	void OnTaskRemoveFinishTask(TaskInterface* pTask, unsigned long ulTask);
	void OnTaskUpdateStorage(TaskInterface* pTask, unsigned long ulCurTime);
	bool UpdateStorage(TaskInterface* pTask, StorageTaskList* pLst, unsigned long ulCurTime, unsigned long idStorage);
	bool UpdateOneStorageDebug(TaskInterface* pTask, unsigned long ulCurTime, int idStorage, bool bUseDayAsSeed);
#endif

public:
	/*
	 *	export functions
	 */

	void Init(elementdataman* pMan);
	bool InitStorageTask();
	void Release();

	// 从Dir中加载所有任务模板
	void LoadAllFromDir(const char* szDir, bool bLoadDescript);

	// 从包中读取所有任务模板
	bool LoadTasksFromPack(const char* szPackPath, bool bLoadDescript);

	// 从包中读取模板，带出错提示。海外工具使用。
	int LoadTasksFromPackForTimeTool(const char* szPackPath, bool bLoadDescript,unsigned long& lPackVersion,unsigned long& lToolVersion);

	// 从包中读取动态任务模板
	bool LoadDynTasksFromPack(const char* szPath);

	// 从包中读取任务NPC信息
	bool LoadNPCInfoFromPack(const char* szPath);
};

ATaskTemplMan* GetTaskTemplMan();

#endif
