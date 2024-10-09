#ifndef _TASKINTERFACE_H_
#define _TASKINTERFACE_H_

#include <stddef.h>
#include "vector.h"

#ifdef	WIN32
	#define _TASK_CLIENT
#endif

// Task Prerequisite Error Code
#define TASK_PREREQU_FAIL_INDETERMINATE	1
#define TASK_PREREQU_FAIL_NOT_ROOT		2
#define TASK_PREREQU_FAIL_SAME_TASK		3
#define TASK_PREREQU_FAIL_NO_SPACE		4
#define TASK_PREREQU_FAIL_FULL			5
#define TASK_PREREQU_FAIL_CANT_REDO		6
#define TASK_PREREQU_FAIL_BELOW_LEVEL	7
#define TASK_PREREQU_FAIL_ABOVE_LEVEL	8
#define TASK_PREREQU_FAIL_NO_ITEM		9
#define TASK_PREREQU_FAIL_BELOW_REPU	10
#define TASK_PREREQU_FAIL_CLAN			11
#define TASK_PREREQU_FAIL_WRONG_GENDER	12
#define TASK_PREREQU_FAIL_NOT_IN_OCCU	13
#define TASK_PREREQU_FAIL_WRONG_PERIOD	14
#define TASK_PREREQU_FAIL_PREV_TASK		15
#define TASK_PREREQU_FAIL_MAX_RCV		16
#define TASK_PREREQU_FAIL_NO_DEPOSIT	17
#define TASK_PREREQU_FAIL_NO_TASK		18
#define TASK_PREREQU_FAIL_NOT_CAPTAIN	19
#define TASK_PREREQU_FAIL_ILLEGAL_MEM	20
#define TASK_PREREQU_FAIL_WRONG_TIME	21
#define TASK_PREREQU_FAIL_NO_SUCH_SUB	22
#define TASK_PREREQU_FAIL_MUTEX_TASK	23
#define TASK_PREREQU_FAIL_NOT_IN_ZONE	24
#define TASK_PREREQU_FAIL_WRONG_SUB		25
#define TASK_PREREQU_FAIL_OUTOF_DIST	26
#define TASK_PREREQU_FAIL_GIVEN_ITEM	27
#define TASK_PREREQU_FAIL_LIVING_SKILL	28
#define TASK_PREREQU_FAIL_SPECIAL_AWARD	29
#define	TASK_PREREQU_FAIL_GM			30
#define TASK_PREREQU_FAIL_GLOBAL_KEYVAL 31
#define TASK_PREREQU_FAIL_SHIELD_USER	32
#define TASK_PREREQU_FAIL_ALREADY_HAS_PQ 33
#define TASK_PREREQU_FAIL_MAX_ACC_CNT	34
#define TASK_PREREQU_FAIL_RMB_NOT_ENOUGH 35
#define TASK_PREREQU_FAIL_NOT_COUPLE	36
#define TASK_PREREQU_FAIL_ERR_CHAR_TIME	37
#define TASK_PREREQU_FAIL_NOT_IVTRSLOTNUM 38 // version 81
#define TASK_PREREQU_FAIL_BELOW_FACTION_CONTRIB 39//version 87
#define TASK_PREREQU_FAIL_BELOW_RECORD_TASKS_NUM 40 //version 91
#define TASK_PREREQU_FAIL_OVER_RECEIVE_PER_DAY 41
#define TASK_PREREQU_FAIL_TRANSFORM_MASK 42
#define TASK_PREREQU_FAIL_FORCE 43
#define TASK_PREREQU_FAIL_FORCE_REPUTATION 44
#define TASK_PREREQU_FAIL_FORCE_CONTRIBUTION 45
#define TASK_PREREQU_FAIL_EXP 46
#define TASK_PREREQU_FAIL_SP 47
#define TASK_PREREQU_FAIL_FORCE_AL 48
#define TASK_PREREQU_FAIL_WEDDING_OWNER 49
#define TASK_PREREQU_FAIL_CROSSSERVER_NO_ACOUNT_LIMIT 50
#define TASK_PREREQU_FAIL_CROSSSERVER_NO_MARRIAGE 51
#define TASK_PREREQU_FAIL_CROSSSERVER_NO_FORCE 52
#define TASK_PREREQU_FAIL_KING 53
#define TASK_PREREQU_FAIL_IN_TEAM 54
#define TASK_PREREQU_FAIL_TITLE 55
#define TASK_PREREQU_FAIL_HISTORYSTAGE	56
#define TASK_PREREQU_FAIL_NO_GIFTCARD_TASK 57
#define TASK_PREREQU_FAIL_BELOW_REINCARNATION	57
#define TASK_PREREQU_FAIL_ABOVE_REINCARNATION	58
#define TASK_PREREQU_FAIL_BELOW_REALMLEVEL	59
#define TASK_PREREQU_FAIL_ABOVE_REALMLEVEL	60
#define TASK_PREREQU_FAIL_REALM_EXP_FULL	61
#define TASK_PREREQU_FAIL_CARD_COUNT_COLLECTION	62
#define TASK_PREREQU_FAIL_MAX_ROLE_CNT	63
#define TASK_PREREQU_FAIL_CARD_COUNT_RANK	64
#define TASK_PREREQU_FAIL_TASK_FORBID 65
#define TASK_PREREQU_FAIL_NO_NAVIGATE_INSHPAED 66

#define TASK_AWARD_FAIL_GIVEN_ITEM		150
#define TASK_AWARD_FAIL_NEW_TASK		151
#define TASK_AWARD_FAIL_REPUTATION		152
#define TASK_AWARD_FAIL_GLOBAL_KEYVAL   153
#define TASK_AWARD_FAIL_CROSSSERVER_NO_ACOUNT_LIMIT 154
#define TASK_AWARD_FAIL_CROSSSERVER_NO_ACOUNT_STORAGE 155
#define TASK_AWARD_FAIL_CROSSSERVER_NO_DIVORCE 156
#define TASK_AWARD_FAIL_CROSSSERVER_NO_FACTION_RALATED 157
#define TASK_AWARD_FAIL_CROSSSERVER_NO_FORCE_RALATED 158
#define TASK_AWARD_FAIL_CROSSSERVER_NO_DIVIEND 159
#define TASK_AWARD_FAIL_LEVEL_CHECK 160


// Task messages
#define TASK_MSG_NEW					1
#define TASK_MSG_SUCCESS				2
#define TASK_MSG_FAIL					3

#define TASK_ACTIVE_LIST_HEADER_LEN		8
#define TASK_ACTIVE_LIST_MAX_LEN		175
#define TASK_FINISHED_LIST_MAX_LEN		2040
#define TASK_DATA_BUF_MAX_LEN			32
#define TASK_FINISH_TIME_MAX_LEN		1700
#define TASK_FINISH_COUNT_MAX_LEN		730

// 库任务
static const int	TASK_MAX_DELIVER_COUNT = 5;
static const int	TASK_STORAGE_COUNT = 32;
static const int	TASK_STORAGE_LEN   = 10;
static const float	TASK_STORAGE_WHELL_SCALE = 10000.f;

// 当前激活的任务列表缓冲区大小
#define TASK_ACTIVE_LIST_BUF_SIZE		(TASK_ACTIVE_LIST_MAX_LEN * TASK_DATA_BUF_MAX_LEN + TASK_ACTIVE_LIST_HEADER_LEN)
// 已完成的任务列表缓冲区大小
#define TASK_FINISHED_LIST_BUF_SIZE		8192
#define TASK_FINISHED_LIST_BUF_SIZE_OLD	4096
// 任务全局数据大小
#define TASK_GLOBAL_DATA_SIZE			256
// 任务完成时间
#define TASK_FINISH_TIME_LIST_BUF_SIZE	10240
//任务完成次数
#define TASK_FINISH_COUNT_LIST_BUF_SIZE	10240
// 库任务
#define TASK_STORAGE_LIST_BUF_SIZE		1024


// Masks
#define	TASK_MASK_KILL_MONSTER			0x00000001
#define TASK_MASK_COLLECT_ITEM			0x00000002
#define TASK_MASK_TALK_TO_NPC			0x00000004
#define TASK_MASK_REACH_SITE			0x00000008
#define TASK_MASK_ANSWER_QUESTION		0x00000010
#define TASK_MASK_TINY_GAME				0x00000020
#define TASK_MASK_KILL_PQ_MONSTER		0x00000040
#define TASK_MASK_KILL_PLAYER			0x00000080


#define MAX_MONSTER_WANTED				3	// 受ActiveTaskEntry大小限制，最大3
#define MAX_PLAYER_WANTED				MAX_MONSTER_WANTED
#define MAX_ITEM_WANTED					10
#define MAX_ITEM_AWARD					64
#define MAX_MONSTER_SUMMONED			32	// 最大召唤出的怪物数量

#define MAX_OCCUPATIONS					12  // 职业

#define TASK_MSG_CHANNEL_LOCAL			0
#define TASK_MSG_CHANNEL_WORLD			1
#define TASK_MSG_CHANNEL_BROADCAST		9

#define TASK_TEAM_RELATION_MARRIAGE		1

#define TASK_AWARD_MAX_CHANGE_VALUE     255
#define TASK_AWARD_MAX_DISPLAY_VALUE    64

#define TASK_AWARD_MAX_DISPLAY_EXP_CNT  32    //表达式的个数
#define TASK_AWARD_MAX_DISPLAY_CHAR_LEN	64	  //表达式的长度

#define TASK_WORLD_CONTRIBUTION_SPEND_PER_DAY 30 // 免费玩家每日消费贡献度上限

#ifdef _WINDOWS
typedef wchar_t task_char;
#else
typedef unsigned short task_char;
#endif

struct task_team_member_info
{
	unsigned long	m_ulId;
	unsigned long	m_ulLevel;
	unsigned long	m_ulOccupation;
	bool			m_bMale;
	unsigned long	m_ulWorldTag;
	unsigned long	m_ulWorldIndex;
	int				m_iForce;
	float			m_Pos[3];
};

struct special_award
{
	unsigned long	id1;
	unsigned long	id2;
	unsigned long	special_mask;
};

class Kill_Player_Requirements
{
	enum
	{
		MAX_OCCPU_MASK =  (1 << MAX_OCCUPATIONS) - 1,
	};
public:
	unsigned long	m_ulOccupations;
	int				m_iMinLevel;
	int				m_iMaxLevel;
	int				m_iGender;
	int				m_iForce;
	Kill_Player_Requirements():
	m_iMinLevel(10),
	m_iMaxLevel(100),
	m_iGender(0),
	m_iForce(0)
	{
		m_ulOccupations = MAX_OCCPU_MASK;
	}
	bool IsMeetAllOccupation() const { return m_ulOccupations == MAX_OCCPU_MASK;}
	bool CheckRequirements(int iOccupation, int iLevel, bool bGender, int iForce) const
	{
		bool bForce = false;
		// 编辑器里1为男，2为女
		int iGender = bGender ? 2 : 1;
		// 编辑器里0代表没有势力要求
		if (m_iForce == 0)
		{
			bForce = true;
		}
		// 非零表示有势力要求
		else
		{
			// 编辑器里华光、暗隐和辉夜分别用第一、二、三个二进制位表示
			int iForceMask = 0;
			if (iForce == 0)
			{
				return false;
			}
			else if (iForce == 1004)
			{
				iForceMask = 1 << 0;
			}
			else if (iForce ==  1005)
			{
				iForceMask = 1 << 1;
			}
			else if (iForce == 1006)
			{
				iForceMask = 1 << 2;
			}
			bForce = (m_iForce & iForceMask) != 0;
		}
		return (m_ulOccupations & (1 << iOccupation)) &&
			m_iMinLevel <= iLevel &&
			m_iMaxLevel >= iLevel &&
			(m_iGender ? m_iGender == iGender : true) &&
			bForce;
	}
};
#ifdef _TASK_CLIENT

struct Task_State_info
{
	unsigned long	m_ulTimeLimit;
	unsigned long	m_ulTimePassed;
	unsigned long	m_ulNPCToProtect;
	unsigned long	m_ulProtectTime;
	unsigned long	m_ulWaitTime;
	unsigned long	m_ulErrCode;
	unsigned long	m_ulGoldWanted;
	unsigned long	m_ulReachLevel;
	unsigned long	m_ulReachReincarnation;
	unsigned long	m_ulReachRealm;
	unsigned long	m_ulPremLevelMin;
	struct
	{
		unsigned long	m_ulMonsterId;
		unsigned long	m_ulMonstersToKill;
		unsigned long	m_ulMonstersKilled;
	} m_MonsterWanted[MAX_MONSTER_WANTED];

	struct
	{
		unsigned long	m_ulItemId;
		unsigned long	m_ulItemsToGet;
		unsigned long	m_ulItemsGained;
		unsigned long	m_ulMonsterId;
	} m_ItemsWanted[MAX_ITEM_WANTED];

	struct TASK_INFO_PLAYER 
	{
		unsigned long m_ulPlayersToKill;
		unsigned long m_ulPlayersKilled;
		Kill_Player_Requirements m_Requirements;
	} m_PlayerWanted[MAX_PLAYER_WANTED];

	abase::vector<wchar_t*> m_TaskCharArr;
	~Task_State_info() {
		size_t s = m_TaskCharArr.size();
		for (size_t i = 0; i < s; ++i)
			delete m_TaskCharArr[i];
	}
};

struct AWARD_DATA;

struct Task_Award_Preview
{
	unsigned long	m_ulGold;
	unsigned long	m_ulExp;
	unsigned long	m_ulRealmExp;
	unsigned long	m_ulSP;
	bool			m_bHasItem;
	bool			m_bItemKnown;
	unsigned long	m_ulItemTypes;
	unsigned long	m_ItemsId[MAX_ITEM_AWARD];
	unsigned long	m_ItemsNum[MAX_ITEM_AWARD];
	int				m_iForceActivity;
	int				m_iForceContrib;
	int				m_iForceRepu;
};

#endif

#ifdef _TASK_CLIENT

	struct talk_proc;

#else

	struct TaskTeamInterface
	{
		virtual void SetMarriage(int nPlayer) = 0;
	};
	
#endif

struct TaskInterface
{
#ifdef _TASK_CLIENT
	enum
	{
		CMD_EMOTION_BINDBUDDY = 1024, //如果有其他非表情的动作，则依次为1025,1026....
		CMD_EMOTION_SITDOWN,	// 打坐
		CMD_JUMP_TRICKACTION,	// 跳起翻滚
		CMD_RUN_TRICKACTION,	// 跑动翻滚
	};
#endif

	virtual ~TaskInterface(){}
	virtual int GetFactionContrib() = 0;
	virtual int GetFactionConsumeContrib() = 0;
	virtual int GetFactionExpContrib() = 0;
	virtual void TakeAwayFactionConsumeContrib(int ulNum) = 0;
	virtual void TakeAwayFactionExpContrib(int ulNum) = 0;
	virtual unsigned long GetPlayerLevel() = 0;
	virtual void* GetActiveTaskList() = 0;
	virtual void* GetFinishedTaskList() = 0;
	virtual void* GetFinishedTimeList() = 0;
	virtual void* GetFinishedCntList() = 0;
	virtual void* GetStorageTaskList() = 0;
	virtual unsigned long* GetTaskMask() = 0;
	virtual void DeliverGold(unsigned long ulGoldNum) = 0;
	virtual void DeliverExperience(unsigned long ulExp) = 0;
	virtual void DeliverSP(unsigned long ulSP) = 0;
	virtual void DeliverReputation(long lReputation) = 0;
	virtual void DeliverFactionContrib(int iConsumeContrib,int iExpContrib) = 0;
	virtual bool CastSkill(int skill_id, int skill_level) = 0;
	virtual int GetTaskItemCount(unsigned long ulTaskItem) = 0;
	virtual int GetCommonItemCount(unsigned long ulCommonItem) = 0;
	virtual bool IsInFaction(unsigned long ulFactionId) = 0;
	virtual int GetFactionRole() = 0;
	virtual unsigned long GetGoldNum() = 0;
	virtual void TakeAwayGold(unsigned long ulNum) = 0;
	virtual void TakeAwayTaskItem(unsigned long ulTemplId, unsigned long ulNum) = 0;
	virtual void TakeAwayCommonItem(unsigned long ulTemplId, unsigned long ulNum) = 0;
	virtual long GetReputation() = 0;
	virtual unsigned long GetCurPeriod() = 0;
	virtual unsigned long GetPlayerId() = 0;
	virtual unsigned long GetPlayerRace() = 0;
	virtual unsigned long GetPlayerOccupation() = 0;
	virtual bool IsDeliverLegal() = 0;
	virtual bool CanDeliverCommonItem(unsigned long ulItemTypes) = 0;
	virtual bool CanDeliverTaskItem(unsigned long ulItemTypes) = 0;
	virtual void DeliverCommonItem(unsigned long ulItemId, unsigned long ulCount, long lPeriod = 0) = 0;
	virtual void DeliverTaskItem(unsigned long ulItemId, unsigned long ulCount) = 0;
	virtual unsigned long GetPos(float pos[3]) = 0;
	virtual bool HasLivingSkill(unsigned long ulSkill) = 0;
	virtual long GetLivingSkillLevel(unsigned long ulSkill) = 0;
	virtual long GetLivingSkillProficiency(unsigned long ulSkill) = 0;
	virtual void GetSpecailAwardInfo(special_award* p) = 0;
	virtual bool IsGM() = 0;
	virtual bool IsShieldUser() = 0;
	virtual bool IsKing() = 0;
	virtual size_t GetInvEmptySlot() = 0;
	virtual void LockInventory(bool is_lock) = 0;

	virtual unsigned char GetShapeMask() = 0;
	virtual bool IsAtCrossServer() = 0;

	/* 组队任务信息*/
	virtual int GetTeamMemberNum() = 0;
	virtual void GetTeamMemberInfo(int nIndex, task_team_member_info* pInfo) = 0;
	virtual unsigned long GetTeamMemberId(int nIndex) = 0;
	virtual bool IsCaptain() = 0;
	virtual bool IsInTeam() = 0;
	virtual bool IsMale() = 0;
	virtual bool IsMarried() = 0;
	virtual bool IsWeddingOwner() = 0;

	void InitActiveTaskList();
	unsigned long GetActLstDataSize();
	unsigned long GetFnshLstDataSize();
	unsigned long GetFnshTimeLstDataSize();
	unsigned long GetFnshCntLstDataSize();
	unsigned long GetStorageTaskLstDataSize();
	bool CanDoMining(unsigned long ulTaskId);

	bool CheckVersion();
	bool HasTask(unsigned long ulTaskId);
	bool HasTopTaskRelatingMarriage(abase::vector<unsigned long> *pTopTaskIDs);
	bool HasTopTaskRelatingWedding(abase::vector<unsigned long> *pTopTaskIDs);
	bool HasTopTaskRelatingSpouse(abase::vector<unsigned long> *pTopTaskIDs);
	bool HasTopTaskRelatingGender(abase::vector<unsigned long> *pTopTaskIDs);

	// static funcs
	static unsigned long GetCurTime();
	static void WriteLog(int nPlayerId, int nTaskId, int nType, const char* szLog);
	static void WriteKeyLog(int nPlayerId, int nTaskId, int nType, const char* szLog);
	
	//全局key/value
	virtual long GetGlobalValue(long lKey) = 0;
	
	virtual unsigned long GetRoleCreateTime() = 0; 
	virtual unsigned long GetRoleLastLoginTime() = 0;
	virtual unsigned long GetAccountTotalCash() = 0;
	virtual unsigned long GetSpouseID() = 0;

	//势力相关
	virtual int GetForce() = 0;
	virtual int GetForceContribution() = 0;
	virtual int GetForceReputation() = 0;
	virtual bool ChangeForceContribution(int iValue) = 0;
	virtual bool ChangeForceReputation(int iValue) = 0;
	virtual bool SetForceReputation(int iValue);
	virtual int GetExp() = 0;
	virtual int GetSP() = 0;
	virtual bool ReduceExp(int iExp) = 0;
	virtual bool ReduceSP(int iSP) = 0;
	virtual int GetForceActivityLevel() = 0;
	virtual void AddForceActivity(int iActivity) = 0;

	// 称号相关
	virtual bool HaveGotTitle(unsigned long id_designation) = 0;
	virtual void DeliverTitle(unsigned long id_designation, long lPeriod = 0) = 0;
	virtual bool CheckRefine(unsigned long level_refine, unsigned long num_equips) = 0;

	virtual int  GetCurHistoryStageIndex() = 0; // 当前历史阶段的序号
	virtual bool CheckSimpleTaskFinshConditon(unsigned long task_id) const = 0; // 简单任务只客户端检查，服务器可直接 返回ture。
	// 转生
	virtual unsigned long GetMaxHistoryLevel() = 0;
	virtual unsigned long GetReincarnationCount() = 0;
	// 境界
	virtual unsigned long GetRealmLevel() = 0;
	virtual bool IsRealmExpFull() = 0;
	virtual void DeliverRealmExp(unsigned long exp) = 0;
	virtual void ExpandRealmLevelMax() = 0;
	virtual unsigned int	 GetObtainedGeneralCardCount() = 0; // 获得的卡牌数量， 卡牌图鉴中的数量
	virtual void AddLeaderShip(unsigned long leader_ship) = 0;
	virtual unsigned long GetObtainedGeneralCardCountByRank(int) = 0; // 身上、包裹、和卡牌仓库中某品阶卡牌的数量之和

	virtual bool CheckTaskForbid(unsigned long task_id) = 0; // 检查任务是否被屏蔽
	virtual int GetWorldContribution() = 0;	// 世界贡献度
	virtual void DeliverWorldContribution(int contribution) = 0;
	virtual void TakeAwayWorldContribution(int contribution) = 0;
	virtual int GetWorldContributionSpend() = 0;
	virtual bool PlayerCanSpendContributionAsWill() = 0;

#ifdef _TASK_CLIENT
	static void UpdateTitleUI(unsigned long ulTask);
	static void PopChatMessage(int iMsg,int dwNum = 0);
	virtual void NotifyServerStorageTasks();
	static long m_tmFinishDlgShown;
	static void UpdateTaskUI(unsigned long idTask, int reason);
	static void TraceTask(unsigned long idTask);
	static void PopupTaskFinishDialog(unsigned long ulTaskId, const talk_proc* pTalk);
	static void ShowMessage(const wchar_t* szMsg);
	static void ShowPunchBagMessage(bool bSucced,bool bMax,unsigned long MonsterTemplID,int dps,int dph);
	static void ShowTaskMessage(unsigned long ulTask, int reason);
	static long GetFinishDlgShowTime() { return m_tmFinishDlgShown; }
	static void SetFinishDlgShowTime(long t) { m_tmFinishDlgShown = t; }
	static int GetTimeZoneBias();
	static void PopEmotionUI(unsigned int task_id,unsigned int uiEmotion,bool bShow);
	
	virtual void UpdateTaskEmotionAction(unsigned int task_id) = 0;
//	static void DisplayTaskCharInfo(const abase::vector<wchar_t*>& TaskCharArr);

	void OnUIDialogEnd(unsigned long ulTask);
	virtual void NotifyServer(const void* pBuf, size_t sz) = 0;
	unsigned long GetTaskCount();
	unsigned long GetTaskId(unsigned long ulIndex);
	bool CheckParent(unsigned long ulParent, unsigned long ulChild);

	// 无返回-1
	int GetFirstSubTaskPosition(unsigned long ulParentTaskId);
	unsigned long GetNextSub(int& nPosition);
	unsigned long GetSubAt(int nPosition);
	

	unsigned long CanDeliverTask(unsigned long ulTaskId);
	bool CanShowTask(unsigned long ulTaskId);
	bool CanFinishTask(unsigned long ulTaskId);
	void GiveUpTask(unsigned long ulTaskId);
	const unsigned short* GetStorageTasks(unsigned int uStorageId);

	void GetTaskStateInfo(unsigned long ulTaskId, Task_State_info* pInfo, bool bActiveTask = true);
	bool GetAwardCandidates(unsigned long ulTaskId, AWARD_DATA* pAward);
	void GetTaskAwardPreview(unsigned long ulTaskId, Task_Award_Preview* p, bool bActiveTask = true);

	unsigned long GetTaskFinishedTime(unsigned long) { return 0; }
	
	//全局key/value
	virtual void DisplayGlobalValue(long lKey, long lValue) = 0;

	// 进入世界时检查PQ任务初始信息
	void CheckPQEnterWorldInit();

	virtual void SetTaskReadyToConfirm(int iTaskID, bool bReady) = 0;
	virtual void UpdateConfirmTasksMap() = 0;
	virtual bool IsTitleDataReady() {return false;}

	virtual void OnGiveupTask(int iTaskID)=0;
	virtual void OnCompleteTask(int iTaskID)=0;
	virtual void OnNewTask(int iTaskID) = 0;


#else
	virtual void NotifyClient(const void* pBuf, size_t sz) = 0;
	virtual void NotifyPlayer(unsigned long ulPlayerId, const void* pBuf, size_t sz) = 0;
	virtual float UnitRand() = 0;
	virtual int  RandNormal(int lower, int upper) = 0;
	virtual int  RandSelect(const float * option, int size) = 0;
	virtual unsigned long GetTeamMemberPos(int nIndex, float pos[3]) = 0;
	virtual void SetCurPeriod(unsigned long ulPeriod) = 0;
	virtual void SetNewRelayStation(unsigned long ulStationId) = 0;
	virtual void SetStorehouseSize(unsigned long ulSize) = 0;
	virtual void SetStorehouseSize2(unsigned long ulSize) = 0;	// 时装
	virtual void SetStorehouseSize3(unsigned long ulSize) = 0;	// 材料
	virtual void SetAccountStorehouseSize(unsigned long ulSize) = 0;	// 账号
	virtual void AddDividend(int nDividend) = 0; // 鸿利值
	virtual void SetInventorySize(long lSize) = 0;
	virtual void SetFuryUpperLimit(unsigned long ulValue) = 0;
	virtual void TransportTo(unsigned long ulWorldId, const float pos[3], long lController) = 0;
	virtual void SetPetInventorySize(unsigned long ulSize) = 0;
	virtual void SetMonsterController(long id, bool bTrigger) = 0;
	virtual void CheckTeamRelationship(int nReason) = 0;
	virtual void Divorce() = 0;
	virtual void SendMessage(unsigned long task_id, int channel, int param) = 0;
	virtual int SummonMonster(int mob_id, int count, int distance, int remain_time, bool die_with_self) = 0;
	virtual void SetAwardDeath(bool bDeadWithLoss) = 0;

	void JP_Only_RemoveKeyTask();//移除玩家身上所有主线任务，慎用
	void JP_Only_RetrieveLostTasks();//根据玩家已完成任务，尝试恢复丢失的主线任务。自动任务自动发放，在NPC处接的任务要去NPC那接
	void BeforeSaveData();
	void PlayerSwitchScene();
	void PlayerLeaveScene();
	void PlayerLeaveWorld();
	void PQEnterWorldInit(unsigned long task_id);
	void TakeAwayItem(unsigned long ulTemplId, unsigned long ulNum, bool bCmnItem)
	{
		if (bCmnItem) TakeAwayCommonItem(ulTemplId, ulNum);
		else TakeAwayTaskItem(ulTemplId, ulNum);
	}

	bool OnCheckTeamRelationship(int nReason, TaskTeamInterface* pTeam);
	void OnCheckTeamRelationshipComplete(int nReason, TaskTeamInterface* pTeam);
	bool RefreshTaskStorage(unsigned long storage_id);

	//全局key/value	
	virtual void PutGlobalValue(long lKey, long lValue) = 0;
	virtual void ModifyGlobalValue(long lKey, long lValue) = 0;

	//历史推进相关的全局key/value	
	virtual void PutHistoryGlobalValue(int lKey, int lValue) = 0;
	virtual void ModifyHistoryGlobalValue(int lKey, int lValue) = 0;

	virtual void ExpandTaskLimit();
	virtual bool OnGiftCardTask(int type);
	void UpdateOneStorageDebug(int idStorage, bool bUseDayAsSeed);
	
#endif
};

class PublicQuestInterface
{
public:
	PublicQuestInterface(){}
 
	static bool InitAddQuest(int task_id, int child_task_id, int* global_value, int size);
 
	//QuestSetStart：参数 bNotChangeRanking: true， 不更改玩家排名， false, 更改玩家排名。
	static bool QuestSetStart(int task_id, int* first_child_global_value, int size,bool bNotChangeRanking);		//	开始一个PQ任务
	static bool QuestSetFinish(int task_id);	//	PQ任务结束
	//QuestSetNextChildTask： 参数 bNotChangeRanking: true， 不更改玩家排名， false, 更改玩家排名。
	static bool QuestSetNextChildTask(int task_id,  int child_task_id, int* child_global_value, int size,bool bNotChangeRanking);

	static int GetCurSubTask(int task_id);		// 获取PQ任务的当前子任务id
	static int GetCurTaskStamp(int task_id);	// 获取PQ任务的时戳
	static int GetCurContrib(int task_id, int role_id);	// 获取PQ任务当前贡献度

	static int GetCurAllPlace(int task_id, int role_id);	// 获取当前总体排名
	static int GetCurClsPlace(int task_id, int role_id);	// 获取当前职业排名

	static bool QuestAddPlayer(int task_id, int role_id);
	static bool QuestRemovePlayer(int task_id, int role_id);
	static bool QuestUpdatePlayerContrib(int task_id, int roleid, int inc_contrib);

	static long QuestGetGlobalValue(long lKey);	// 获取PQ任务全局变量

	static void QuestEnterWorldInit(int task_id, int role_id); // 进入世界初始化link相关数据
	
	static bool QuestSetRandContrib(int task_id, int fixed_contrib, int max_rand_contrib, int lowest_contrib); // 设置PQ随机贡献度
	
	static void SetMonsterController(long ctrl_id, bool trigger);	// 设置怪物控制器
	
	static void QuestLeaveWorld(int task_id, int role_id);
};

#endif
