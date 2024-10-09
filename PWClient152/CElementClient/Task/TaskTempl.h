#ifndef _ATASK_TEMPL_H_
#define _ATASK_TEMPL_H_

#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <wchar.h>
#include <time.h>
#if defined(WIN32) || defined(_LUA_TASKTEST)
#pragma warning(disable:4786)
#endif
#include <set>
using namespace std;

#ifdef WIN32
	#include "ExpTypes.h"
#elif defined LINUX
	#include "../template/exptypes.h"
#else
	#include "exptypes.h"
#endif

#include "TaskInterface.h"
#include "TaskProcess.h"
#include "TaskExpAnalyser.h"


#define	MAX_TASK_NAME_LEN		30      
#define MAX_AWARD_NPC_NUM		8
#define MAX_PREM_TASK_COUNT		20
#define MAX_MUTEX_TASK_COUNT	5
#define MAX_TEAM_MEM_WANTED		MAX_OCCUPATIONS
#define MAX_TIMETABLE_SIZE		24

#define MAX_AWARD_SCALES		5
#define MAX_AWARD_CANDIDATES	12
#define MAX_LIVING_SKILLS		4
#define MAX_CONTRIB_MONSTERS	100		// PQ子任务贡献度最大怪物种类	
#define MAX_AWARD_PQ_RANKING	32		// PQ子任务奖励最大排名数
#define MAX_TITLE_NUM			10

#define MAX_TASKREGION			8 // 最大区域个数

#define TASK_GENDER_NONE		0
#define TASK_GENDER_MALE		1
#define TASK_GENDER_FEMALE		2

#define TASK_MAX_PATH 260

#define INVALID_VAL	((unsigned long) -1)

#define TASK_MAX_LINE_LEN TASK_MAX_PATH

#define TASK_MAX_VALID_COUNT	6

#define TASK_TREASURE_MAP_SIDE_MULTIPLE	30    //藏宝图边长与小区域边长的比

// 目前有两种奖励，用于账号补填；
static const int NUM_SPECIAL_AWARD = 2;
// 奖励任务id
static const int TASK_SPECIAL_AWARD[NUM_SPECIAL_AWARD] =  {26969,26970};

enum DynTaskType
{
	enumDTTNone = 0,
	enumDTTSpecialAward,
	enumDTTNormal,
	enumDTTGiftCard,
};

struct NPC_INFO
{
	unsigned long id;
	short x;
	short y;
	short z;
};

/*
 *	Server Notifications
 */

#define TASK_SVR_NOTIFY_NEW				1	// 新任务发放
#define TASK_SVR_NOTIFY_COMPLETE		2	// 任务完毕
#define TASK_SVR_NOTIFY_GIVE_UP			3	// 任务放弃
#define TASK_SVR_NOTIFY_MONSTER_KILLED	4	// 杀怪数量
#define TASK_SVR_NOTIFY_FINISHED		5	// 处于得到奖励状态
#define TASK_SVR_NOTIFY_ERROR_CODE		6	// 错误码
#define TASK_SVR_NOTIFY_FORGET_SKILL	7	// 遗忘生活技能
#define TASK_SVR_NOTIFY_DYN_TIME_MARK	8	// 动态任务时间标记
#define TASK_SVR_NOTIFY_DYN_DATA		9	// 动态任务数据
#define TASK_SVR_NOTIFY_SPECIAL_AWARD	10	// 特殊奖励信息
#define	TASK_SVR_NOTIFY_STORAGE			11	// 仓库数据
#define TASK_SVR_NOTIFY_DIS_GLOBAL_VAL  12  // 显示全局变量
#define TASK_SVR_NOTIFY_TREASURE_MAP	13  // 藏宝位置
#define TASK_SVR_NOTIFY_SET_TASK_LIMIT  14	// 设置任务列表上限
#define TASK_SVR_NOTIFY_PLAYER_KILLED	15  // 杀人数量

/*
 *	Client Notifications
 */

#define TASK_CLT_NOTIFY_CHECK_FINISH	1
#define TASK_CLT_NOTIFY_CHECK_GIVEUP	2
#define TASK_CLT_NOTIFY_REACH_SITE		3
#define TASK_CLT_NOTIFY_AUTO_DELV		4
#define TASK_CLT_NOTIFY_MANUAL_TRIG		5
#define TASK_CLT_NOTIFY_FORCE_GIVEUP	6
#define TASK_CLT_NOTIFY_DYN_TIMEMARK	7
#define TASK_CLT_NOTIFY_DYN_DATA		8
#define TASK_CLT_NOTIFY_SPECIAL_AWARD	9
#define TASK_CLT_NOTIFY_LEAVE_SITE		10
#define TASK_CLT_NOTIFY_PQ_CHECK_INIT	11
#define	TASK_CLT_NOTIFY_STORAGE			12
#define TASK_CLT_NOTIFY_REQUEST_TREASURE_INDEX 14
#define TASK_CLT_NOTIFY_15DAYS_NOLOGIN	15
#define TASK_CLT_NOTIFY_SPECIAL_AWARD_MASK 16
#define TASK_CLT_NOTIFY_TITLE_TASK 17
#define TASK_CLT_NOTIFY_CHOOSE_AWARD 18
#define TASK_CLT_NOTIFY_BUY_TOKENSHOP_ITEM 20
#define TASK_CLT_NOTIFY_FINISH_TASK_BY_WORLD_CONTRIBUTION 21
#define	TASK_CLT_NOTIFY_RM_FINISH_TASK	150

/*
 *	Player Notifications
 */

#define TASK_PLY_NOTIFY_NEW_MEM_TASK	1	// 通知接收队员任务
#define TASK_PLY_NOTIFY_FORCE_FAIL		2	// 全队失败
#define TASK_PLY_NOTIFY_FORCE_SUCC		3	// 全队成功

/*
 *	Global Data Reason
 */

#define TASK_GLOBAL_CHECK_RCV_NUM		1
#define TASK_GLOBAL_CHECK_COTASK		2
#define TASK_GLOBAL_CHECK_ADD_MEM		3
#define TASK_GLOBAL_NPC_KILLED_TIME		4
#define TASK_GLOBAL_ADD_TIME_MARK		5
#define TASK_GLOBAL_DEL_TIME_MARK		6
#define TASK_GLOBAL_PROTECT_NPC			7

/*
 *	Cotask Condition
 */

#define COTASK_CORRESPOND				0
#define COTASK_ONCE						1

/*
 *	任务类型
 */
enum ENUM_TASK_TYPE
{
	enumTTDaily = 100,			// 每日
	enumTTLevel2,				// 修真
	enumTTMajor,				// 主线
	enumTTBranch,				// 支线
	enumTTEvent,				// 活动
	enumTTQiShaList,			// 七杀榜
	enumTTFaction,				// 帮派
	enumTTFunction,				// 经营
	enumTTLegend,				// 传奇
	enumTTQuestion,				// 答题

	enumTTEnd,
};

/* 完成方式 */
enum
{
	enumTMNone = 0,					// 无
	enumTMKillNumMonster,			// 杀数量怪
	enumTMCollectNumArticle,		// 获得数量道具
	enumTMTalkToNPC,				// 与特定NPC对话
	enumTMReachSite,				// 到达特定地点
	enumTMWaitTime,					// 等待特定时间
	enumTMAnswerQuestion,			// 选择问答
	enumTMTinyGame,					// 小游戏
	enumTMProtectNPC,				// 保护特定NPC
	enumTMNPCReachSite,				// NPC到达特定地点
	enumTMGlobalValOK,              // 全局变量满足条件
	enumTMLeaveSite,				// 离开特定地点
	enumTMReachTreasureZone,		// 达到藏宝区域
	enumTMKillPlayer,				// 杀死玩家
	enumTMTransform,				// 变身状态
	enumTMReachLevel,				// 检查等级：普通等级，转生次数，境界等级
	enumTMSimpleClientTask,			// 简单任务，只做客户端验证，目前只验证表情动作
	enumTMSimpleClientTaskForceNavi,// 强制移动
};

/* 完成条件 */
enum
{
	enumTFTDirect = 0,		// 直接完成
	enumTFTNPC,
	enumTFTConfirm,			// 需要确认完成
};

/* 奖励方式 */
enum
{
	enumTATNormal = 0,		// 普通
	enumTATEach,			// 按杀怪数目、获得物品乘
	enumTATRatio,			// 按时间比例
	enumTATItemCount		// 按获得物品个数分不同档次
};

/* 任务重复间隔 */
enum
{
	enumTAFNormal = 0,		// 普通
	enumTAFEachDay,			// 每天
	enumTAFEachWeek,		// 每周
	enumTAFEachMonth,		// 每月
	enumTAFEachYear			// 每年
};

#pragma pack(1)

#define TASK_PACK_MAGIC		0x93858361

struct TASK_PACK_HEADER
{
	unsigned long	magic;
	unsigned long	version;
	unsigned long	item_count;
};

struct ZONE_VERT
{
	union
	{
		struct
		{
			float x;
			float y;
			float z;
		};

		float v[3];
	};

	bool operator == (const ZONE_VERT& src) const
	{
		return (v[0] == src.v[0] && v[1] == src.v[1] && v[2] == src.v[2]);
	}

	bool great_than(const float v[3]) const { return x >= v[0] && y >= v[1] && z >= v[2]; }
	bool less_than (const float v[3]) const { return x <= v[0] && y <= v[1] && z <= v[2]; }
};

inline bool is_in_zone(
	const ZONE_VERT& _min,
	const ZONE_VERT& _max,
	const float pos[3])
{
	return _min.less_than(pos) && _max.great_than(pos);
}

struct PLAYER_WANTED
{
	unsigned long m_ulPlayerNum;
	unsigned long m_ulDropItemId;
	unsigned long m_ulDropItemCount;
	bool		  m_bDropCmnItem;
	float		  m_fDropProb;
	Kill_Player_Requirements m_Requirements;
};
struct MONSTER_WANTED
{
	unsigned long	m_ulMonsterTemplId;
	unsigned long	m_ulMonsterNum;
	unsigned long	m_ulDropItemId;
	unsigned long	m_ulDropItemCount;
	bool			m_bDropCmnItem;
	float			m_fDropProb;
	bool			m_bKillerLev;
	int				m_iDPH;
	int				m_iDPS;

	bool operator == (const MONSTER_WANTED& src) const
	{
		return (m_ulMonsterTemplId == src.m_ulMonsterTemplId && 
				m_ulMonsterNum == src.m_ulMonsterNum && 
				m_ulDropItemId == src.m_ulDropItemId && 
				m_ulDropItemCount == src.m_ulDropItemCount && 
				m_bDropCmnItem == src.m_bDropCmnItem && 
				m_fDropProb == src.m_fDropProb &&
				m_bKillerLev == src.m_bKillerLev);
	}
};

struct ITEM_WANTED
{
	unsigned long	m_ulItemTemplId;
	bool			m_bCommonItem;
	unsigned long	m_ulItemNum;
	float			m_fProb;
	long            m_lPeriod;

	bool operator == (const ITEM_WANTED& src) const
	{
		return (m_ulItemTemplId == src.m_ulItemTemplId && m_bCommonItem == src.m_bCommonItem && m_ulItemNum == src.m_ulItemNum && m_lPeriod == src.m_lPeriod);
	}
};

struct TITLE_AWARD
{
	unsigned long m_ulTitleID;
	long		  m_lPeriod;
	TITLE_AWARD():m_ulTitleID(0), m_lPeriod(0){}
};

struct TEAM_MEM_ITEM_WANTED
{
	unsigned long	m_ulItemTemplId;
	bool			m_bCommonItem;
	unsigned long	m_ulItemNum;
};

struct MONSTERS_SUMMONED
{
	unsigned long	m_ulMonsterTemplId;
	unsigned long	m_ulMonsterNum;
	float			m_fSummonProb;
	long            m_lPeriod;

	bool operator == (const MONSTERS_SUMMONED& src) const
	{
		return (m_ulMonsterTemplId == src.m_ulMonsterTemplId && m_ulMonsterNum == src.m_ulMonsterNum && m_fSummonProb == src.m_fSummonProb && m_lPeriod == src.m_lPeriod);
	}
};

struct MONSTERS_CONTRIB
{
	unsigned long	m_ulMonsterTemplId;		//  怪物ID
	int				m_iWholeContrib;		//  组队间独享贡献度
	int				m_iShareContrib;		//	组队间共享贡献度
	int				m_iPersonalWholeContrib; //	个人独享贡献度

	bool operator == (const MONSTERS_CONTRIB& src) const
	{
		return (m_ulMonsterTemplId == src.m_ulMonsterTemplId && m_iWholeContrib == src.m_iWholeContrib && m_iShareContrib == src.m_iShareContrib && m_iPersonalWholeContrib == src.m_iPersonalWholeContrib);
	}

};

struct RANKING_AWARD
{
	unsigned int	m_iRankingStart;
	unsigned int	m_iRankingEnd;
	bool			m_bCommonItem;
	unsigned long	m_ulAwardItemId;
	unsigned long	m_ulAwardItemNum;
	long			m_lPeriod;

	bool operator == (const RANKING_AWARD& src) const
	{
		return (m_iRankingStart == src.m_iRankingStart && m_iRankingEnd == src.m_iRankingEnd && m_bCommonItem == src.m_bCommonItem && m_ulAwardItemId == src.m_ulAwardItemId && m_ulAwardItemNum == src.m_ulAwardItemNum && m_lPeriod == src.m_lPeriod);
	}
};


extern const unsigned long _race_occ_map[MAX_OCCUPATIONS];

struct TEAM_MEM_WANTED
{
	unsigned long	m_ulLevelMin;
	unsigned long	m_ulLevelMax;
	unsigned long	m_ulRace;
	unsigned long	m_ulOccupation;
	unsigned long	m_ulGender;
	unsigned long	m_ulMinCount;
	unsigned long	m_ulMaxCount;
	unsigned long	m_ulTask;
	int				m_iForce;
// 	unsigned long	m_ulTeamMemsItemWanted;
// 	TEAM_MEM_ITEM_WANTED* m_TeamMemItemWanted; //[MAX_ITEM_WANTED];
// 	bool			m_bJustCheck;

	TEAM_MEM_WANTED(){Init();}

	bool IsMeetBaseInfo(const task_team_member_info* pInfo) const
	{
		if (m_ulLevelMin && pInfo->m_ulLevel < m_ulLevelMin) return false;

		if (m_ulLevelMax && pInfo->m_ulLevel > m_ulLevelMax) return false;

		if (m_ulRace)
		{
			if (pInfo->m_ulOccupation >= SIZE_OF_ARRAY(_race_occ_map)) return false;
			if (_race_occ_map[pInfo->m_ulOccupation] != m_ulRace) return false;
		}
		else if (m_ulOccupation != INVALID_VAL && pInfo->m_ulOccupation != m_ulOccupation)
			return false;

		if (m_iForce)
		{
			if (m_iForce == -1)
			{
				if (pInfo->m_iForce == 0)
					return false;
			}
			else if (pInfo->m_iForce != m_iForce) return false;
		}

#ifndef _TASK_CLIENT
		if (m_ulGender == TASK_GENDER_MALE   && !pInfo->m_bMale
		 || m_ulGender == TASK_GENDER_FEMALE && pInfo->m_bMale)
			return false;
#endif

		return true;
	}

	bool IsMeetCount(unsigned long ulCount) const
	{
		if (m_ulMinCount && ulCount < m_ulMinCount) return false;

		if (m_ulMaxCount && ulCount > m_ulMaxCount) return false;

		return true;
	}

	void Init()
	{
		m_ulLevelMin	= 0;
		m_ulLevelMax	= 0;
		m_ulRace		= 0;
		m_ulOccupation	= INVALID_VAL;
		m_ulGender		= 0;
		m_ulMinCount	= 0;
		m_ulMaxCount	= 0;
		m_ulTask		= 0;
		m_iForce		= 0;
// 		m_ulTeamMemsItemWanted = 0;
// 		m_TeamMemItemWanted = NULL;
//		m_bJustCheck	= false;
	}

	bool IsValid() const
	{
		return !(
			m_ulLevelMin	== 0 &&
			m_ulLevelMax	== 0 &&
			m_ulRace		== 0 &&
			m_ulOccupation	== INVALID_VAL &&
			m_ulGender		== 0 &&
			m_ulMinCount	== 0 &&
			m_ulMaxCount	== 0 &&
			m_ulTask		== 0 // &&
// 			m_ulTeamMemsItemWanted == 0 &&
// 			m_TeamMemItemWanted == NULL &&
//			m_bJustCheck    == false);
			);
	}

	bool operator == (const TEAM_MEM_WANTED& src) const
	{
		return (
			m_ulLevelMin	== src.m_ulLevelMin &&
			m_ulLevelMax	== src.m_ulLevelMax &&
			m_ulRace		== src.m_ulRace &&
			m_ulOccupation	== src.m_ulOccupation &&
			m_ulGender		== src.m_ulGender &&
			m_ulMinCount	== src.m_ulMinCount &&
			m_ulMaxCount	== src.m_ulMaxCount &&
			m_ulTask		== src.m_ulTask// &&
//			m_ulTeamMemsItemWanted == src.m_ulTeamMemsItemWanted &&
//			*m_TeamMemItemWanted == *(src.m_TeamMemItemWanted) &&
//			m_bJustCheck	== src.m_bJustCheck
			);
	}
};

struct AWARD_ITEMS_CAND
{
	AWARD_ITEMS_CAND()
	{
		memset(this, 0, sizeof(*this));

#ifdef TASK_TEMPL_EDITOR
		m_AwardItems = new ITEM_WANTED[MAX_ITEM_AWARD];
		g_ulNewCount++;
#endif
	}
	~AWARD_ITEMS_CAND()
	{
		LOG_DELETE_ARR(m_AwardItems);
	}

	AWARD_ITEMS_CAND& operator = (const AWARD_ITEMS_CAND& src)
	{
		m_ulAwardItems		= src.m_ulAwardItems;
		m_ulAwardCmnItems	= src.m_ulAwardCmnItems;
		m_ulAwardTskItems	= src.m_ulAwardTskItems;
		m_bRandChoose		= src.m_bRandChoose;

#ifndef TASK_TEMPL_EDITOR

		if (m_ulAwardItems)
		{
			m_AwardItems = new ITEM_WANTED[m_ulAwardItems];
			g_ulNewCount++;
		}

#endif

		for (unsigned long i = 0; i < m_ulAwardItems; i++)
			m_AwardItems[i] = src.m_AwardItems[i];

		return *this;
	}

	bool operator == (const AWARD_ITEMS_CAND& src) const
	{
		if (m_ulAwardItems != src.m_ulAwardItems)
		{
			return false;
		}

		for (unsigned int i  = 0;i < m_ulAwardItems;++i)
		{
			if (!(m_AwardItems[i] == src.m_AwardItems[i]))
			{
				return false;
			}
		}
		return (m_ulAwardCmnItems == src.m_ulAwardCmnItems && m_ulAwardTskItems == src.m_ulAwardTskItems && m_bRandChoose == src.m_bRandChoose);
	}

	unsigned long	m_ulAwardItems;
	unsigned long	m_ulAwardCmnItems;
	unsigned long	m_ulAwardTskItems;
	ITEM_WANTED*	m_AwardItems; //[MAX_ITEM_AWARD];
	bool			m_bRandChoose;

	int MarshalBasicData(char* pData)
	{
		char* p = pData;

		*p = m_bRandChoose;
		p++;

		char cnt = (char)m_ulAwardItems;
		*p = cnt;
		p++;

		size_t sz = sizeof(ITEM_WANTED) * cnt;
		if (sz)
		{
			memcpy(p, m_AwardItems, sz);
			p += sz;
		}

		return p - pData;
	}

	int UnmarshalBasicData(const char* pData)
	{
		const char* p = pData;

		m_bRandChoose = (*p != 0);
		p++;

		m_ulAwardItems = *p;
		p++;

		if (m_ulAwardItems)
		{
#ifndef TASK_TEMPL_EDITOR
			m_AwardItems = new ITEM_WANTED[m_ulAwardItems];
			g_ulNewCount++;
#endif

			size_t sz = sizeof(ITEM_WANTED) * m_ulAwardItems;
			memcpy(m_AwardItems, p, sz);
			p += sz;

			for (unsigned long i = 0; i < m_ulAwardItems; i++)
			{
				if (m_AwardItems[i].m_bCommonItem)
					m_ulAwardCmnItems++;
				else
					m_ulAwardTskItems++;
			}
		}

		return p - pData;
	}
};

struct AWARD_MONSTERS_SUMMONED
{
	AWARD_MONSTERS_SUMMONED()
	{
		memset(this, 0, sizeof(*this));

#ifdef TASK_TEMPL_EDITOR
		m_Monsters = new MONSTERS_SUMMONED[MAX_MONSTER_SUMMONED];
		g_ulNewCount++;
#endif
	}
	~AWARD_MONSTERS_SUMMONED()
	{
		LOG_DELETE_ARR(m_Monsters);
	}

	AWARD_MONSTERS_SUMMONED& operator = (const AWARD_MONSTERS_SUMMONED& src)
	{
		m_ulMonsterNum		= src.m_ulMonsterNum;
		m_bRandChoose		= src.m_bRandChoose;
		m_ulSummonRadius	= src.m_ulSummonRadius;
		m_bDeathDisappear	= src.m_bDeathDisappear;

#ifndef TASK_TEMPL_EDITOR

		if (m_ulMonsterNum)
		{
			m_Monsters = new MONSTERS_SUMMONED[m_ulMonsterNum];
			g_ulNewCount++;
		}

#endif

		for (unsigned long i = 0; i < m_ulMonsterNum; i++)
			m_Monsters[i] = src.m_Monsters[i];

		return *this;
	}

	bool operator == (const AWARD_MONSTERS_SUMMONED& src) const
	{
		if (m_ulMonsterNum != src.m_ulMonsterNum)
		{
			return false;
		}

		for (unsigned int i = 0;i < m_ulMonsterNum;++i)
		{
			if (!(m_Monsters[i] == src.m_Monsters[i]))
			{
				return false;
			}
		}
		
		return ( m_bRandChoose == src.m_bRandChoose && m_ulSummonRadius == src.m_ulSummonRadius && m_bDeathDisappear == src.m_bDeathDisappear);
	}

	unsigned long		m_ulMonsterNum;
	bool				m_bRandChoose;
	unsigned long		m_ulSummonRadius;
	bool				m_bDeathDisappear;
	MONSTERS_SUMMONED*	m_Monsters; //[MAX_MONSTER_SUMMONED];

	int MarshalBasicData(char* pData)
	{
		char* p = pData;

		*p = m_bRandChoose;
		p++;

		*p = (char)m_ulSummonRadius;
		p++;

		char cnt = (char)m_ulMonsterNum;
		*p = cnt;
		p++;
	
		*p = m_bDeathDisappear;
		p++;

		size_t sz = sizeof(MONSTERS_SUMMONED) * cnt;
		if (sz)
		{
			memcpy(p, m_Monsters, sz);
			p += sz;
		}

		return p - pData;
	}

	int UnmarshalBasicData(const char* pData)
	{
		const char* p = pData;

		m_bRandChoose = (*p != 0);
		p++;

		m_ulSummonRadius = *p;
		p++;

		m_ulMonsterNum = *p;
		p++;

		m_bDeathDisappear = (*p != 0);
		p++;

		if (m_ulMonsterNum)
		{
#ifndef TASK_TEMPL_EDITOR
			m_Monsters = new MONSTERS_SUMMONED[m_ulMonsterNum];
			g_ulNewCount++;
#endif

			size_t sz = sizeof(MONSTERS_SUMMONED) * m_ulMonsterNum;
			memcpy(m_Monsters, p, sz);
			p += sz;
		}

		return p - pData;
	}
};

struct AWARD_PQ_RANKING
{
	AWARD_PQ_RANKING()
	{
		memset(this, 0, sizeof(*this));

#ifdef TASK_TEMPL_EDITOR
		m_RankingAward = new RANKING_AWARD[MAX_AWARD_PQ_RANKING];
		g_ulNewCount++;
#endif
	}
	~AWARD_PQ_RANKING()
	{
		LOG_DELETE_ARR(m_RankingAward);
	}

	AWARD_PQ_RANKING& operator = (const AWARD_PQ_RANKING& src)
	{
		m_bAwardByProf = src.m_bAwardByProf;
		m_ulRankingAwardNum = src.m_ulRankingAwardNum;

#ifndef TASK_TEMPL_EDITOR

		if (m_ulRankingAwardNum)
		{
			m_RankingAward = new RANKING_AWARD[m_ulRankingAwardNum];
			g_ulNewCount++;
		}

#endif

		for (unsigned long i = 0; i < m_ulRankingAwardNum; i++)
			m_RankingAward[i] = src.m_RankingAward[i];

		return *this;
	}

	bool operator == (const AWARD_PQ_RANKING& src) const
	{
		if (m_bAwardByProf != src.m_bAwardByProf)
		{
			return false;
		}
		if (m_ulRankingAwardNum != src.m_ulRankingAwardNum)
		{
			return false;
		}
		
		for (unsigned int i = 0;i < m_ulRankingAwardNum;++i)
		{
			if (!(m_RankingAward[i] == src.m_RankingAward[i]))
			{
				return false;
			}
		}
		return true;
	}	

	bool			m_bAwardByProf;
	unsigned long	m_ulRankingAwardNum;
	RANKING_AWARD*	m_RankingAward; //[MAX_AWARD_PQ_RANKING]

	int MarshalBasicData(char* pData)
	{
		char* p = pData;

		*p = m_bAwardByProf;
		p++;

		char cnt = (char)m_ulRankingAwardNum;
		*p = cnt;
		p++;
	
		size_t sz = sizeof(RANKING_AWARD) * cnt;
		if (sz)
		{
			memcpy(p, m_RankingAward, sz);
			p += sz;
		}

		return p - pData;
	}

	int UnmarshalBasicData(const char* pData)
	{
		const char* p = pData;

		m_bAwardByProf = (*p != 0);
		p++;

		m_ulRankingAwardNum = *p;
		p++;

		if (m_ulRankingAwardNum)
		{
#ifndef TASK_TEMPL_EDITOR
			m_RankingAward = new RANKING_AWARD[m_ulRankingAwardNum];
			g_ulNewCount++;
#endif

			size_t sz = sizeof(RANKING_AWARD) * m_ulRankingAwardNum;
			memcpy(m_RankingAward, p, sz);
			p += sz;
		}

		return p - pData;
	}
};

//必须定义operator ==
template<typename T_NAME>
inline bool CompareTwoPointer(T_NAME* p1,T_NAME* p2)
{
	if (p1 == 0 && p2 == 0)
	{
		return true;
	}
	else if (p1 != 0 && p2 != 0)
	{
		return (*p1 == *p2);
	}
	else
		return false;
}

struct AWARD_DATA
{
	AWARD_DATA()
	{
		memset(this, 0, sizeof(*this));

#ifdef TASK_TEMPL_EDITOR
		m_CandItems = new AWARD_ITEMS_CAND[MAX_AWARD_CANDIDATES];
		g_ulNewCount++;

		m_SummonedMonsters = new AWARD_MONSTERS_SUMMONED;
		g_ulNewCount++;

		m_PQRankingAward = new AWARD_PQ_RANKING;
		g_ulNewCount++;

		m_pTitleAward = new TITLE_AWARD[MAX_TITLE_NUM];
		g_ulNewCount++;
#endif
	}
	AWARD_DATA(const AWARD_DATA& src)
	{
#ifdef TASK_TEMPL_EDITOR
		m_CandItems = new AWARD_ITEMS_CAND[MAX_AWARD_CANDIDATES];
		g_ulNewCount++;

		m_SummonedMonsters = new AWARD_MONSTERS_SUMMONED;
		g_ulNewCount++;

		m_PQRankingAward = new AWARD_PQ_RANKING;
		g_ulNewCount++;

		m_pTitleAward = new TITLE_AWARD[MAX_TITLE_NUM];
		g_ulNewCount++;
#endif

		*this = src;
	}
	~AWARD_DATA()
	{
		LOG_DELETE_ARR(m_CandItems);
		LOG_DELETE(m_SummonedMonsters);
		LOG_DELETE(m_PQRankingAward);
		LOG_DELETE_ARR(m_plChangeKey);
		LOG_DELETE_ARR(m_plChangeKeyValue);
		LOG_DELETE_ARR(m_pbChangeType);
		LOG_DELETE_ARR(m_plDisplayKey);
		LOG_DELETE_ARR(m_pszExp);
		LOG_DELETE_ARR(m_pExpArr);
		LOG_DELETE_ARR(m_pTaskChar);
		LOG_DELETE_ARR(m_pTitleAward);

		LOG_DELETE_ARR(m_plHistoryChangeKey);
		LOG_DELETE_ARR(m_plHistoryChangeKeyValue);
		LOG_DELETE_ARR(m_pbHistoryChangeType);
	}

	AWARD_DATA& operator = (const AWARD_DATA& src)
	{
#ifdef TASK_TEMPL_EDITOR
		AWARD_ITEMS_CAND* pCans = m_CandItems;
		AWARD_MONSTERS_SUMMONED* pSummoned = m_SummonedMonsters;
		AWARD_PQ_RANKING* pRanking	= m_PQRankingAward;
		TITLE_AWARD* pTitle = m_pTitleAward;
#endif

		memcpy(this, &src, sizeof(*this));

#ifdef TASK_TEMPL_EDITOR
		m_CandItems = pCans;
		m_SummonedMonsters = pSummoned;
		m_PQRankingAward = pRanking;
		m_pTitleAward = pTitle;
#else
		if (m_ulCandItems)
		{
			m_CandItems = new AWARD_ITEMS_CAND[m_ulCandItems];
			g_ulNewCount++;
		}
		else
			m_CandItems = NULL;

		if (m_ulSummonedMonsters)
		{
			m_SummonedMonsters = new AWARD_MONSTERS_SUMMONED;
			g_ulNewCount++;
		}
		else
			m_SummonedMonsters = NULL;

		if (m_ulPQRankingAwardCnt)
		{
			m_PQRankingAward = new AWARD_PQ_RANKING;
			g_ulNewCount++;
		}
		else
			m_PQRankingAward = NULL;
		if (m_ulTitleNum)
		{
			m_pTitleAward = new TITLE_AWARD[m_ulTitleNum];
			g_ulNewCount++;
		}
		else m_pTitleAward = NULL;

#endif

		unsigned long i;
		for (i = 0; i < m_ulCandItems; i++)
			m_CandItems[i] = src.m_CandItems[i];

		for (i = 0; i < m_ulTitleNum; i++)
			m_pTitleAward[i] = src.m_pTitleAward[i];

		if(m_ulSummonedMonsters)
			*m_SummonedMonsters = *src.m_SummonedMonsters;

		if(m_ulPQRankingAwardCnt)
			*m_PQRankingAward = *src.m_PQRankingAward;

		if (m_ulChangeKeyCnt)
		{
			m_plChangeKey = new long[m_ulChangeKeyCnt];
			m_plChangeKeyValue = new long[m_ulChangeKeyCnt];
			m_pbChangeType = new bool[m_ulChangeKeyCnt];

			for (unsigned long i = 0; i < m_ulChangeKeyCnt; i++)
			{
				m_plChangeKey[i] = src.m_plChangeKey[i];
				m_plChangeKeyValue[i] = src.m_plChangeKeyValue[i];
				m_pbChangeType[i] = src.m_pbChangeType[i];
			}
		}
		else
		{
			m_plChangeKey = 0;
			m_plChangeKeyValue = 0;
			m_pbChangeType = 0;
		}

		if (m_ulHistoryChangeCnt)
		{
			m_plHistoryChangeKey = new long[m_ulHistoryChangeCnt];
			m_plHistoryChangeKeyValue = new long[m_ulHistoryChangeCnt];
			m_pbHistoryChangeType = new bool[m_ulHistoryChangeCnt];
			
			for (unsigned long i = 0; i < m_ulHistoryChangeCnt; i++)
			{
				m_plHistoryChangeKey[i] = src.m_plHistoryChangeKey[i];
				m_plHistoryChangeKeyValue[i] = src.m_plHistoryChangeKeyValue[i];
				m_pbHistoryChangeType[i] = src.m_pbHistoryChangeType[i];
			}
		}
		else
		{
			m_plHistoryChangeKey = 0;
			m_plHistoryChangeKeyValue = 0;
			m_pbHistoryChangeType = 0;
		}

		if (m_ulDisplayKeyCnt)
		{
			m_plDisplayKey = new long[m_ulDisplayKeyCnt];
			memcpy(m_plDisplayKey, src.m_plDisplayKey, sizeof(long) * m_ulDisplayKeyCnt);
		}
		else
			m_plDisplayKey = 0;

		if (m_ulExpCnt)
		{
			m_pszExp = new char[m_ulExpCnt][TASK_AWARD_MAX_DISPLAY_CHAR_LEN];
			m_pExpArr = new TASK_EXPRESSION[m_ulExpCnt][TASK_AWARD_MAX_DISPLAY_CHAR_LEN];
			for (unsigned long i=0; i<m_ulExpCnt; i++)
			{
				memcpy(m_pszExp[i], src.m_pszExp[i], TASK_AWARD_MAX_DISPLAY_CHAR_LEN);
				memcpy(m_pExpArr[i], src.m_pExpArr[i], TASK_AWARD_MAX_DISPLAY_CHAR_LEN);
			}
		}
		else
		{
			m_pszExp = NULL;
			m_pExpArr = NULL;
		}

		if (m_ulTaskCharCnt)
		{
			m_pTaskChar = new task_char[m_ulTaskCharCnt][TASK_AWARD_MAX_DISPLAY_CHAR_LEN];
			for (unsigned long i=0; i<m_ulTaskCharCnt; i++)
			{
				memcpy(m_pTaskChar[i], src.m_pTaskChar[i], TASK_AWARD_MAX_DISPLAY_CHAR_LEN);	
			}
		}
		else
			m_pTaskChar = NULL;

		return *this;
	}

	
	bool operator == (const AWARD_DATA& src) const
	{
		if (m_ulCandItems != src.m_ulCandItems || m_ulSummonedMonsters != src.m_ulSummonedMonsters || 
			m_ulChangeKeyCnt != src.m_ulChangeKeyCnt || m_ulDisplayKeyCnt != src.m_ulDisplayKeyCnt || 
			m_ulExpCnt != src.m_ulExpCnt || m_ulTaskCharCnt != src.m_ulTaskCharCnt ||
			m_ulHistoryChangeCnt != src.m_ulHistoryChangeCnt)
		{
			return false;
		}

		unsigned int i = 0;
		for (i = 0;i < m_ulCandItems;++i)
		{
			if (!(m_CandItems[i] == src.m_CandItems[i]))
			{
				return false;
			}
		}
		if (!CompareTwoPointer(m_SummonedMonsters,src.m_SummonedMonsters))
		{
			return false;
		}

		for (i = 0;i < m_ulChangeKeyCnt;++i)
		{
			if (m_plChangeKey[i] != src.m_plChangeKey[i] || m_plChangeKeyValue[i] != src.m_plChangeKeyValue[i] || m_pbChangeType[i] != src.m_pbChangeType[i])
			{
				return false;
			}
		}
		for (i = 0;i < m_ulHistoryChangeCnt;++i)
		{
			if (m_plHistoryChangeKey[i] != src.m_plHistoryChangeKey[i] || m_plHistoryChangeKeyValue[i] != src.m_plHistoryChangeKeyValue[i] || m_pbHistoryChangeType[i] != src.m_pbHistoryChangeType[i])
			{
				return false;
			}
		}

		for (i = 0;i < m_ulDisplayKeyCnt;++i)
		{
			if (m_plDisplayKey[i] != src.m_plDisplayKey[i])
			{
				return false;
			}
		}

		if (!CompareTwoPointer(m_PQRankingAward,src.m_PQRankingAward))
		{
			return false;
		}
		
		return (m_ulGoldNum					==	src.m_ulGoldNum					&&
				m_ulExp						==	src.m_ulExp						&&
				m_ulNewTask					==	src.m_ulNewTask					&&
				m_ulSP						==	src.m_ulSP						&&
				m_lReputation				==	src.m_lReputation				&&
				m_ulNewPeriod				==	src.m_ulNewPeriod				&&
				m_ulNewRelayStation			==	src.m_ulNewRelayStation			&&
				m_ulStorehouseSize			==	src.m_ulStorehouseSize			&&
				m_ulStorehouseSize2			==	src.m_ulStorehouseSize2			&&
				m_ulStorehouseSize3			==	src.m_ulStorehouseSize3			&&
				m_ulStorehouseSize4			==	src.m_ulStorehouseSize4			&&
				m_lInventorySize			==	src.m_lInventorySize			&&
				m_ulPetInventorySize		==	src.m_ulPetInventorySize		&&
				m_ulFuryULimit				==	src.m_ulFuryULimit				&&
				m_ulTransWldId				==	src.m_ulTransWldId				&&
				m_TransPt					==  src.m_TransPt					&&
				m_lMonsCtrl					==  src.m_lMonsCtrl					&&
				m_bTrigCtrl					==	src.m_bTrigCtrl					&&
				m_bUseLevCo					==	src.m_bUseLevCo					&&
				m_bDivorce					==  src.m_bDivorce					&&
				m_bSendMsg					==	src.m_bSendMsg					&&
				m_nMsgChannel				==	src.m_nMsgChannel				&&
				m_bAwardDeath				==  src.m_bAwardDeath				&&
				m_bAwardDeathWithLoss		==	src.m_bAwardDeathWithLoss		&&
				m_ulDividend				==	src.m_ulDividend				&&
				m_bAwardSkill				==	src.m_bAwardSkill				&&
				m_iAwardSkillID				==	src.m_iAwardSkillID				&&
				m_iAwardSkillLevel			==	src.m_iAwardSkillLevel			&&
				m_ulSpecifyContribTaskID	==	src.m_ulSpecifyContribTaskID	&&
				m_ulSpecifyContribSubTaskID ==	src.m_ulSpecifyContribSubTaskID &&
				m_ulSpecifyContrib			==	src.m_ulSpecifyContrib			&&
				m_ulContrib					==	src.m_ulContrib					&&
				m_ulRandContrib				==	src.m_ulRandContrib				&&
				m_ulLowestcontrib			==	src.m_ulLowestcontrib			&&
				m_iFactionContrib			==	src.m_iFactionContrib			&&
				m_iFactionExpContrib		==	src.m_iFactionExpContrib		&&
				m_ulPQRankingAwardCnt		==	src.m_ulPQRankingAwardCnt		&&
				m_bMulti					==	src.m_bMulti					&&
				m_nNumType					==	src.m_nNumType					&&
				m_lNum						==	src.m_lNum				
				);
	}

	unsigned long		m_ulGoldNum;
	unsigned long		m_ulExp;
	unsigned long		m_ulRealmExp; // 境界经验
	bool				m_bExpandRealmLevelMax; // 境界等级10整级时提升境界等级上限
	unsigned long		m_ulNewTask;
	unsigned long		m_ulSP;
	long				m_lReputation;
	unsigned long		m_ulNewPeriod;
	unsigned long		m_ulNewRelayStation;
	unsigned long		m_ulStorehouseSize;
	unsigned long		m_ulStorehouseSize2;
	unsigned long		m_ulStorehouseSize3;
	unsigned long		m_ulStorehouseSize4;	// 账号仓库
	long				m_lInventorySize;
	unsigned long		m_ulPetInventorySize;
	unsigned long		m_ulFuryULimit;
	unsigned long		m_ulTransWldId;
	ZONE_VERT			m_TransPt;
	long				m_lMonsCtrl;
	bool				m_bTrigCtrl;
	bool				m_bUseLevCo;
	bool				m_bDivorce;
	bool				m_bSendMsg;
	int					m_nMsgChannel;
	unsigned long		m_ulCandItems;
	AWARD_ITEMS_CAND*	m_CandItems; //[MAX_AWARD_CANDIDATES];
	unsigned long		m_ulSummonedMonsters;
	AWARD_MONSTERS_SUMMONED* m_SummonedMonsters;
	bool				m_bAwardDeath;
	bool				m_bAwardDeathWithLoss;
	unsigned long		m_ulDividend;	// 鸿利值
	
	bool				m_bAwardSkill;  // 是否奖励技能 [Yongdong, 2010-1-6]
	int					m_iAwardSkillID; // 技能ID
	int					m_iAwardSkillLevel;// 技能等级

//////////////////////////////////////////////////// PQ任务奖励 start
	
	unsigned long		m_ulSpecifyContribTaskID;	// 指定任务贡献度的任务id
	unsigned long		m_ulSpecifyContribSubTaskID; // 指定任务贡献度的子任务ID
	unsigned long		m_ulSpecifyContrib;			// 指定任务贡献度
	
	// 仅PQ子任务专用
	unsigned long		m_ulContrib;		// 贡献度
	unsigned long		m_ulRandContrib;	// 随机贡献度
	unsigned long		m_ulLowestcontrib;	// 最低贡献度

	//帮派贡献度
	int					m_iFactionContrib;
	int					m_iFactionExpContrib;

	unsigned long		m_ulPQRankingAwardCnt;
	AWARD_PQ_RANKING*	m_PQRankingAward;
	
//////////////////////////////////////////////////// PQ任务奖励 end
	// 改变全局key/value
	unsigned long       m_ulChangeKeyCnt;
	long                *m_plChangeKey; //[TASK_AWARD_MAX_CHANGE_VALUE]
	long                *m_plChangeKeyValue;
	bool                *m_pbChangeType;
	// 修改历史进度
	unsigned long		m_ulHistoryChangeCnt;
	long				*m_plHistoryChangeKey; //[TASK_AWARD_MAX_CHANGE_VALUE]
	long				*m_plHistoryChangeKeyValue;
	bool                *m_pbHistoryChangeType;

	
	// 倍率
	bool                m_bMulti;
	int                 m_nNumType;
	long                m_lNum;

	// 显示全局key/value
	unsigned long       m_ulDisplayKeyCnt;
	long                *m_plDisplayKey; //[TASK_AWARD_MAX_DISPLAY_VALUE]

	// 显示全局变量表达式
	unsigned long		m_ulExpCnt;
	char				(*m_pszExp)[TASK_AWARD_MAX_DISPLAY_CHAR_LEN];
	TASK_EXPRESSION 	(*m_pExpArr)[TASK_AWARD_MAX_DISPLAY_CHAR_LEN];

	// 显示全局变量表达式提示字符串
	unsigned long       m_ulTaskCharCnt;
	task_char           (*m_pTaskChar)[TASK_AWARD_MAX_DISPLAY_CHAR_LEN];

	// 势力相关
	int					m_iForceContribution;
	int					m_iForceReputation;
	int					m_iForceActivity;
	int					m_iForceSetRepu;

	int					m_iTaskLimit;
	unsigned long		m_ulTitleNum;
	TITLE_AWARD*		m_pTitleAward;	
	int					m_iLeaderShip;

	int					m_iWorldContribution; // 世界贡献度

	bool HasAward() const
	{
		return m_ulGoldNum != 0
			|| m_ulExp != 0
			|| m_ulNewTask != 0
			|| m_ulSP != 0
			|| m_lReputation != 0
			|| m_ulNewPeriod != 0
			|| m_ulNewRelayStation != 0
			|| m_ulStorehouseSize != 0
			|| m_ulStorehouseSize2 != 0
			|| m_ulStorehouseSize3 != 0
			|| m_ulStorehouseSize4 != 0
			|| m_lInventorySize != 0
			|| m_ulPetInventorySize != 0
			|| m_ulFuryULimit
			|| m_bDivorce
			|| m_bSendMsg
			|| m_bAwardDeath
			|| m_ulCandItems != 0
			|| m_ulSummonedMonsters != 0
			|| m_ulSpecifyContrib != 0
			|| m_ulSpecifyContribTaskID != 0
			|| m_ulContrib != 0
			|| m_ulRandContrib != 0
			|| m_ulDividend != 0
			|| m_ulPQRankingAwardCnt != 0
			|| m_ulSpecifyContribSubTaskID !=0
			|| m_bAwardSkill
			|| m_iFactionContrib != 0
			|| m_iFactionExpContrib != 0
			|| m_iForceActivity != 0
			|| m_iForceContribution != 0
			|| m_iForceReputation != 0
			|| m_iForceSetRepu
			|| m_iTaskLimit
			|| m_ulRealmExp != 0
			|| m_bExpandRealmLevelMax;
	}

	int MarshalBasicData(char* pData)
	{
		char* p = pData;

		long* _mask = (long*)p;
		*_mask = 0;
		p += 4;

		if (m_ulGoldNum)
		{
			*_mask |= 1;
			*(long*)p = m_ulGoldNum;
			p += sizeof(long);
		}

		if (m_ulExp)
		{
			*_mask |= 1 << 1;
			*(long*)p = m_ulExp;
			p += sizeof(long);
		}

		if (m_ulSP)
		{
			*_mask |= 1 << 2;
			*(long*)p = m_ulSP;
			p += sizeof(long);
		}

		if (m_lReputation)
		{
			*_mask |= 1 << 3;
			*(long*)p = m_lReputation;
			p += sizeof(long);
		}

		if (m_ulCandItems)
		{
			*_mask |= 1 << 4;

			char cnt = (char)m_ulCandItems;
			*p = cnt;
			p++;

			for (char i = 0; i < cnt; i++)
				p += m_CandItems[i].MarshalBasicData(p);
		}

		if (m_ulSummonedMonsters)
		{
			*_mask |= 1 << 5;

			char cnt = (char)m_ulSummonedMonsters;
			*p = cnt;
			p++;

			p += m_SummonedMonsters->MarshalBasicData(p);
		}

		if (m_ulPQRankingAwardCnt)
		{
			*_mask |= 1 << 6;

			char cnt = (char)m_ulPQRankingAwardCnt;
			*p = cnt;
			p++;

			p += m_PQRankingAward->MarshalBasicData(p);
		}

		return p - pData;
	}

	int UnmarshalBasicData(const char* pData)
	{
		const char* p = pData;

		long _mask = *(long*)p;
		p += sizeof(long);

		if (_mask & 1)
		{
			m_ulGoldNum = *(long*)p;
			p += 4;
		}

		if (_mask & (1 << 1))
		{
			m_ulExp = *(long*)p;
			p += 4;
		}

		if (_mask & (1 << 2))
		{
			m_ulSP = *(long*)p;
			p += 4;
		}

		if (_mask & (1 << 3))
		{
			m_lReputation = *(long*)p;
			p += 4;
		}

		if (_mask & (1 << 4))
		{
			m_ulCandItems = *p;
			p++;

			if (m_ulCandItems)
			{
#ifndef TASK_TEMPL_EDITOR
				m_CandItems = new AWARD_ITEMS_CAND[m_ulCandItems];
				g_ulNewCount++;
#endif

				for (unsigned long i = 0; i < m_ulCandItems; i++)
					p += m_CandItems[i].UnmarshalBasicData(p);
			}
		}

		if (_mask & (1 << 5))
		{
			m_ulSummonedMonsters = *p;
			p++;

			if (m_ulSummonedMonsters)
			{
#ifndef TASK_TEMPL_EDITOR
				m_SummonedMonsters = new AWARD_MONSTERS_SUMMONED;
				g_ulNewCount++;
#endif

				p += m_SummonedMonsters->UnmarshalBasicData(p);
			}
		}

		if (_mask & (1 << 6))
		{
			m_ulPQRankingAwardCnt = *p;
			p++;

			if (m_ulPQRankingAwardCnt)
			{
#ifndef TASK_TEMPL_EDITOR
				m_PQRankingAward = new AWARD_PQ_RANKING;
				g_ulNewCount++;
#endif

				p += m_PQRankingAward->UnmarshalBasicData(p);
			}
		}

		return p - pData;
	}
};

struct AWARD_RATIO_SCALE
{
	AWARD_RATIO_SCALE()
	{
		memset(this, 0, sizeof(*this));

#ifdef TASK_TEMPL_EDITOR
		m_Awards = new AWARD_DATA[MAX_AWARD_SCALES];
		g_ulNewCount++;
#endif
	}
	~AWARD_RATIO_SCALE()
	{
		LOG_DELETE_ARR(m_Awards);
	}

	bool operator == (const AWARD_RATIO_SCALE& src) const
	{
		if (m_ulScales != src.m_ulScales)
		{
			return false;
		}

		for (unsigned int i = 0;i < m_ulScales;++i)
		{
			if (m_Ratios[i] != src.m_Ratios[i] || !(m_Awards[i] == src.m_Awards[i]))
			{
				return false;
			}
		}
		return true;
	}
	unsigned long	m_ulScales;
	float			m_Ratios[MAX_AWARD_SCALES];
	AWARD_DATA*		m_Awards; //[MAX_AWARD_SCALES];

	AWARD_RATIO_SCALE& operator = (const AWARD_RATIO_SCALE& src)
	{
		m_ulScales = src.m_ulScales;

#ifndef TASK_TEMPL_EDITOR

		if (m_ulScales)
		{
			m_Awards = new AWARD_DATA[m_ulScales];
			g_ulNewCount++;
		}

#endif

		for (unsigned long i = 0; i < m_ulScales; i++)
		{
			m_Ratios[i] = src.m_Ratios[i];
			m_Awards[i] = src.m_Awards[i];
		}

		return *this;
	}

	bool HasAward() const
	{
		for (unsigned long i = 0; i < m_ulScales; i++)
			if (m_Awards[i].HasAward())
				return true;

		return false;
	}
};

struct AWARD_ITEMS_SCALE
{
	AWARD_ITEMS_SCALE()
	{
		memset(this, 0, sizeof(*this));

#ifdef TASK_TEMPL_EDITOR
		m_Awards = new AWARD_DATA[MAX_AWARD_SCALES];
		g_ulNewCount++;
#endif
	}
	~AWARD_ITEMS_SCALE()
	{
		LOG_DELETE_ARR(m_Awards);
	}

	unsigned long	m_ulScales;
	unsigned long	m_ulItemId;
	unsigned long	m_Counts[MAX_AWARD_SCALES];
	AWARD_DATA*		m_Awards; //[MAX_AWARD_SCALES];

	bool operator == (const AWARD_ITEMS_SCALE& src) const
	{
		if (m_ulScales != src.m_ulScales)
		{
			return false;
		}
		for (unsigned int i = 0;i < m_ulScales;++i)
		{
			if (m_Counts[i] != src.m_Counts[i] || !(m_Awards[i] == src.m_Awards[i]))
			{
				return false;
			}
		}

		return m_ulItemId == src.m_ulItemId;
	}

	AWARD_ITEMS_SCALE& operator = (const AWARD_ITEMS_SCALE& src)
	{
		m_ulScales = src.m_ulScales;
		m_ulItemId = src.m_ulItemId;

#ifndef TASK_TEMPL_EDITOR

		if (m_ulScales)
		{
			m_Awards = new AWARD_DATA[m_ulScales];
			g_ulNewCount++;
		}

#endif

		for (unsigned long i = 0; i < m_ulScales; i++)
		{
			m_Counts[i] = src.m_Counts[i];
			m_Awards[i] = src.m_Awards[i];
		}

		return *this;
	}

	bool HasAward() const
	{
		for (unsigned long i = 0; i < m_ulScales; i++)
			if (m_Awards[i].HasAward())
				return true;

		return false;
	}
};

enum task_tm_type
{
	enumTaskTimeDate = 0,
	enumTaskTimeMonth,
	enumTaskTimeWeek,
	enumTaskTimeDay
};

const long task_week_map[] =
{
	7,
	1,
	2,
	3,
	4,
	5,
	6
};

struct task_tm
{
	long year;
	long month;
	long day;
	long hour;
	long min;
	long wday;

	bool operator == (const task_tm& time) const
	{
		return ( year == time.year && month == time.month && day == time.day && hour == time.hour && min == time.min);
	}

	bool after(const tm* _tm) const
	{
		if (year < _tm->tm_year + 1900) return false;
		if (year > _tm->tm_year + 1900) return true;

		if (month < _tm->tm_mon + 1) return false;
		if (month > _tm->tm_mon + 1) return true;

		if (day < _tm->tm_mday) return false;
		if (day > _tm->tm_mday) return true;

		if (hour < _tm->tm_hour) return false;
		return hour > _tm->tm_hour || min > _tm->tm_min;
	}

	bool before(const tm* _tm) const
	{
		if (year > _tm->tm_year + 1900) return false;
		if (year < _tm->tm_year + 1900) return true;

		if (month > _tm->tm_mon + 1) return false;
		if (month < _tm->tm_mon + 1) return true;

		if (day > _tm->tm_mday) return false;
		if (day < _tm->tm_mday) return true;

		if (hour > _tm->tm_hour) return false;
		return hour < _tm->tm_hour || min <= _tm->tm_min;
	}

	bool after_per_month(const tm* _tm, bool bLastDay) const
	{
		// 如果当前天大于设定天则为false
		if (day < _tm->tm_mday) return false;

		// 如果当前天是非最后一天，且小于设定天，则返回true
		if (!bLastDay && day > _tm->tm_mday) return true;

		if (hour < _tm->tm_hour) return false;
		return hour > _tm->tm_hour || min > _tm->tm_min;
	}

	bool before_per_month(const tm* _tm, bool bLastDay) const
	{
		// 如果当前天大于设定天则为true
		if (day < _tm->tm_mday) return true;

		// 如果当前天是非最后一天，且小于设定天，则返回false
		if (!bLastDay && day > _tm->tm_mday) return false;

		if (hour > _tm->tm_hour) return false;
		return hour < _tm->tm_hour || min <= _tm->tm_min;
	}

	bool after_per_week(const tm* _tm) const
	{
		long w = task_week_map[_tm->tm_wday];

		if (wday < w) return false;
		if (wday > w) return true;

		if (hour < _tm->tm_hour) return false;
		return hour > _tm->tm_hour || min > _tm->tm_min;
	}

	bool before_per_week(const tm* _tm) const
	{
		long w = task_week_map[_tm->tm_wday];

		if (wday > w) return false;
		if (wday < w) return true;

		if (hour > _tm->tm_hour) return false;
		return hour < _tm->tm_hour || min <= _tm->tm_min;
	}

	bool after_per_day(const tm* _tm) const
	{
		if (hour < _tm->tm_hour) return false;
		return hour > _tm->tm_hour || min > _tm->tm_min;
	}

	bool before_per_day(const tm* _tm) const
	{
		if (hour > _tm->tm_hour) return false;
		return hour < _tm->tm_hour || min <= _tm->tm_min;
	}
};

inline bool judge_time_date(const task_tm* tmStart, const task_tm* tmEnd, unsigned long ulCurTime, task_tm_type tm_type)
{
	tm _time, _time_tomorrow;
	unsigned long ulTimeTomorrow;
	bool last_day;

#ifdef _TASK_CLIENT
	ulCurTime -= unsigned long(TaskInterface::GetTimeZoneBias() * 60);

	if ((long)(ulCurTime) < 0)
		ulCurTime = 0;

	_time = *gmtime((time_t*)&ulCurTime);
	ulTimeTomorrow = ulCurTime + 24 * 3600;
	_time_tomorrow = *gmtime((time_t*)&ulTimeTomorrow);
#else
	_time = *localtime((time_t*)&ulCurTime);
	ulTimeTomorrow = ulCurTime + 24 * 3600;
	_time_tomorrow = *localtime((time_t*)&ulTimeTomorrow);
#endif

	last_day = (_time.tm_mon != _time_tomorrow.tm_mon);

	switch (tm_type)
	{
	case enumTaskTimeDate:
		return tmStart->before(&_time) && tmEnd->after(&_time);
	case enumTaskTimeMonth:
		return tmStart->before_per_month(&_time, last_day) && tmEnd->after_per_month(&_time, last_day);
	case enumTaskTimeWeek:
		return tmStart->before_per_week(&_time) && tmEnd->after_per_week(&_time);
	case enumTaskTimeDay:
		return tmStart->before_per_day(&_time) && tmEnd->after_per_day(&_time);
	}

	return false;
}

#define MAX_SUB_TAGS 32

struct task_sub_tags
{
	union
	{
		unsigned short sub_task;
		unsigned char state;
	};

	unsigned char sz;
	unsigned char tags[MAX_SUB_TAGS];
	unsigned char cur_index; // for temporary use, dont take into account

	size_t get_size() const { return sz + 3; }
	bool valid_size(size_t _sz) const
	{
		if (_sz < 3) return false;
		return get_size() == _sz;
	}
};

struct task_notify_base
{
	unsigned char	reason;
	unsigned short	task;
};

struct task_notify_choose_award : public task_notify_base
{
	unsigned char choice;
};

struct task_player_notify : public task_notify_base
{
	unsigned long	param;
};

struct svr_monster_killed : public task_notify_base
{
	unsigned long	monster_id;
	unsigned short	monster_num;
	int				dps;
	int				dph;
};

struct svr_player_killed : public task_notify_base
{
	unsigned short index;
	unsigned short player_num;
};

struct svr_treasure_map : public task_notify_base
{
	int treasure_index;
};


struct svr_new_task : public task_notify_base
{
	unsigned long	cur_time;
	unsigned long	cap_task;
	task_sub_tags	sub_tags;

	inline void set_data(
		unsigned long _cur_time,
		unsigned long _cap_task,
		const task_sub_tags& _sub_tags)
	{
		cur_time	= _cur_time;
		cap_task	= _cap_task;
		memcpy(&sub_tags, &_sub_tags, _sub_tags.get_size());
	}

	inline void get_data(
		unsigned long& _cur_time,
		unsigned long& _cap_task,
		task_sub_tags& _sub_tags) const
	{
		_cur_time	= cur_time;
		_cap_task	= cap_task;
		memcpy(&_sub_tags, &sub_tags, sub_tags.get_size());
	}

	inline size_t get_size() const { return sizeof(task_notify_base) + 8 + sub_tags.get_size(); }

	inline bool valid_size(size_t sz) const
	{
		const size_t base_sz = sizeof(task_notify_base) + 8;
		if (sz <= base_sz) return false;
		return sub_tags.valid_size(sz - base_sz);
	}
};

struct svr_task_complete : public task_notify_base
{
	unsigned long	cur_time;
	task_sub_tags	sub_tags;

	inline void set_data(
		unsigned long _cur_time,
		const task_sub_tags& _sub_tags
		)
	{
		cur_time	= _cur_time;
		memcpy(&sub_tags, &_sub_tags, _sub_tags.get_size());
	}

	inline void get_data(
		unsigned long& _cur_time,
		task_sub_tags& _sub_tags) const
	{
		_cur_time	= cur_time;
		memcpy(&_sub_tags, &sub_tags, sub_tags.get_size());
	}

	inline size_t get_size() const { return sizeof(task_notify_base) + 4 + sub_tags.get_size(); }

	inline bool valid_size(size_t sz) const
	{
		const size_t base_sz = sizeof(task_notify_base) + 4;
		if (sz <= base_sz) return false;
		return sub_tags.valid_size(sz - base_sz);
	}
};

struct svr_task_err_code : public task_notify_base
{
	unsigned long err_code;
};

struct svr_task_dyn_time_mark : public task_notify_base
{
	unsigned long time_mark;
	unsigned short version;
};

struct svr_task_special_award : public task_notify_base
{
	special_award sa;
};

struct COMPARE_KEY_VALUE
{
	int nLeftType;
	long lLeftNum;
	int nCompOper;
	int nRightType;
	long lRightNum;

	bool operator == (const COMPARE_KEY_VALUE& src) const
	{
		return (nLeftType == src.nLeftType && lLeftNum == src.lLeftNum && nCompOper == src.nCompOper && nRightType == src.nRightType && lRightNum == src.lRightNum);
	}
};

struct Task_Region
{
	ZONE_VERT zvMin;
	ZONE_VERT zvMax;

	bool operator == (const Task_Region& src) const
	{
		return (zvMin == src.zvMin && zvMax == src.zvMax);
	}
};

struct ATaskTemplFixedData // No Virtual Functions, No Embedded Class
{
	ATaskTemplFixedData()
	{
		memset(this, 0, sizeof(*this));

		m_Award_S = new AWARD_DATA;
		g_ulNewCount++;
		m_Award_F = new AWARD_DATA;
		g_ulNewCount++;

		m_AwByRatio_S = new AWARD_RATIO_SCALE;
		g_ulNewCount++;
		m_AwByRatio_F = new AWARD_RATIO_SCALE;
		g_ulNewCount++;

		m_AwByItems_S = new AWARD_ITEMS_SCALE;
		g_ulNewCount++;
		m_AwByItems_F = new AWARD_ITEMS_SCALE;
		g_ulNewCount++;

#ifdef TASK_TEMPL_EDITOR
		m_pszSignature	= new task_char[MAX_TASK_NAME_LEN];
		g_ulNewCount++;
		m_tmStart		= new task_tm[MAX_TIMETABLE_SIZE];
		g_ulNewCount++;
		m_tmEnd			= new task_tm[MAX_TIMETABLE_SIZE];
		g_ulNewCount++;
		m_PremItems		= new ITEM_WANTED[MAX_ITEM_WANTED];
		g_ulNewCount++;
		m_GivenItems	= new ITEM_WANTED[MAX_ITEM_WANTED];
		g_ulNewCount++;
		m_TeamMemsWanted= new TEAM_MEM_WANTED[MAX_TEAM_MEM_WANTED];
		g_ulNewCount++;
		m_ItemsWanted	= new ITEM_WANTED[MAX_ITEM_WANTED];
		g_ulNewCount++;
		m_PlayerWanted  = new PLAYER_WANTED[MAX_PLAYER_WANTED];
		g_ulNewCount++;
		m_MonsterWanted	= new MONSTER_WANTED[MAX_MONSTER_WANTED];
		g_ulNewCount++;
		m_MonstersContrib = new MONSTERS_CONTRIB[MAX_CONTRIB_MONSTERS];
		g_ulNewCount++;

		m_pDelvRegion = new Task_Region[MAX_TASKREGION];
		g_ulNewCount++;

		m_pEnterRegion = new Task_Region[MAX_TASKREGION];
		g_ulNewCount++;

		m_pLeaveRegion = new Task_Region[MAX_TASKREGION];
		g_ulNewCount++;

		m_pReachSite = new Task_Region[MAX_TASKREGION];
		g_ulNewCount++;

		m_pLeaveSite = new Task_Region[MAX_TASKREGION];
		g_ulNewCount++;
		
		m_PremTitles = new int[MAX_TITLE_NUM];
		g_ulNewCount++;
		
#endif
	}

	~ATaskTemplFixedData()
	{
		LOG_DELETE(m_Award_S);
		LOG_DELETE(m_Award_F);
		LOG_DELETE(m_AwByRatio_S);
		LOG_DELETE(m_AwByRatio_F);
		LOG_DELETE(m_AwByItems_S);
		LOG_DELETE(m_AwByItems_F);
		LOG_DELETE_ARR(m_pszSignature);
		LOG_DELETE_ARR(m_tmStart);
		LOG_DELETE_ARR(m_tmEnd);
		LOG_DELETE_ARR(m_PremItems);
		LOG_DELETE_ARR(m_GivenItems);
		LOG_DELETE_ARR(m_TeamMemsWanted);
		LOG_DELETE_ARR(m_ItemsWanted);
		LOG_DELETE_ARR(m_PlayerWanted);
		LOG_DELETE_ARR(m_MonsterWanted);
		LOG_DELETE_ARR(m_plChangeKey);
		LOG_DELETE_ARR(m_plChangeKeyValue);
		LOG_DELETE_ARR(m_pbChangeType);
		LOG_DELETE_ARR(m_pszExp);
		LOG_DELETE_ARR(m_pExpArr);
		LOG_DELETE_ARR(m_pTaskChar);
		LOG_DELETE_ARR(m_pszPQExp);
		LOG_DELETE_ARR(m_pPQExpArr);
		LOG_DELETE_ARR(m_MonstersContrib);

		LOG_DELETE_ARR(m_pDelvRegion);
		LOG_DELETE_ARR(m_pEnterRegion);
		LOG_DELETE_ARR(m_pLeaveRegion);
		LOG_DELETE_ARR(m_pReachSite);
		LOG_DELETE_ARR(m_pLeaveSite);
		LOG_DELETE_ARR(m_PremTitles);
	}

	bool operator == (const ATaskTemplFixedData& src) const;


	/* 任务对象的属性 */

	// 任务id
	unsigned long	m_ID;
	// 任务名称
	task_char		m_szName[MAX_TASK_NAME_LEN];
	// 任务署名
	bool			m_bHasSign;
	task_char*		m_pszSignature;
	// 任务类型
	unsigned long	m_ulType;
	// 时间限制
	unsigned long	m_ulTimeLimit;
	
	// 下线任务失败
	bool			m_bOfflineFail;
	// 任务失败时间
	bool			m_bAbsFail;
	task_tm			m_tmAbsFailTime;
//	unsigned long	m_ulAbsFailTime;
	// 任务开启物品检验不收取
	bool			m_bItemNotTakeOff;


	//	是否绝对时间
	bool			m_bAbsTime;
	// 时间段个数
	unsigned long	m_ulTimetable;
	// 时间方式
	char			m_tmType[MAX_TIMETABLE_SIZE];
	// 发放起始时间
	task_tm*		m_tmStart; //[MAX_TIMETABLE_SIZE];
	// 发放终止时间
	task_tm*		m_tmEnd; //[MAX_TIMETABLE_SIZE];
	// 发放频率
	long			m_lAvailFrequency;
	long			m_lPeriodLimit;
	// 选择单个子任务执行
	bool			m_bChooseOne;
	// 随机旋转单个子任务执行
	bool			m_bRandOne;
	// 子任务是否有顺序
	bool			m_bExeChildInOrder;
	// 失败后是否认为父任务也失败
	bool			m_bParentAlsoFail;
	// 子任务成功后父任务成功
	bool			m_bParentAlsoSucc;
	// 能否放弃此任务
	bool			m_bCanGiveUp;
	// 是否可重复完成
	bool			m_bCanRedo;
	// 失败后是否可重新完成
	bool			m_bCanRedoAfterFailure;
	// 放弃清空任务
	bool			m_bClearAsGiveUp;
	// 是否需要记录
	bool			m_bNeedRecord;
	// 玩家被杀死是否认为失败
	bool			m_bFailAsPlayerDie;
	// 接受者上限
	unsigned long	m_ulMaxReceiver;
	// 发放区域
	bool			m_bDelvInZone;
	unsigned long	m_ulDelvWorld;
	unsigned long	m_ulDelvRegionCnt;
	//ZONE_VERT		m_DelvMinVert;
	//ZONE_VERT		m_DelvMaxVert;
	Task_Region*	m_pDelvRegion;

	// 进入区域任务失败
	bool			m_bEnterRegionFail;
	unsigned long	m_ulEnterRegionWorld;
	unsigned long	m_ulEnterRegionCnt;
	//ZONE_VERT		m_EnterRegionMinVert;
	//ZONE_VERT		m_EnterRegionMaxVert;
	Task_Region*	m_pEnterRegion;

	// 离开区域任务失败
	bool			m_bLeaveRegionFail;
	unsigned long	m_ulLeaveRegionWorld;
	unsigned long	m_ulLeaveRegionCnt;
	//ZONE_VERT		m_LeaveRegionMinVert;
	//ZONE_VERT		m_LeaveRegionMaxVert;
	Task_Region*	m_pLeaveRegion;

	// 离开阵营失败
	bool			m_bLeaveForceFail;

	// 传送到特定点
	bool			m_bTransTo;
	unsigned long	m_ulTransWldId;
	ZONE_VERT		m_TransPt;
	// 怪物控制器
	long			m_lMonsCtrl;
	bool			m_bTrigCtrl;
	// 满足条件自动触发
	bool			m_bAutoDeliver;
	// 是否显示在专属奖励界面里
	bool			m_bDisplayInExclusiveUI;
	bool			m_bReadyToNotifyServer;
	// 是否用于兑换
	bool			m_bUsedInTokenShop;
	// 死亡触发
	bool			m_bDeathTrig;
	// 是否清除所得物品
	bool			m_bClearAcquired;
	// 推荐级别
	unsigned long	m_ulSuitableLevel;
	// 是否显示提示
	bool			m_bShowPrompt;
	// 是否关键任务
	bool			m_bKeyTask;
	// 发放NPC
	unsigned long	m_ulDelvNPC;
	// 发奖NPC
	unsigned long	m_ulAwardNPC;
	// 是否生活技能任务
	bool			m_bSkillTask;
	// 能否被查找
	bool			m_bCanSeekOut;
	// 能否显示指示方向
	bool			m_bShowDirection;
	// 结婚
	bool			m_bMarriage;
	// 改变全局key/value
	unsigned long   m_ulChangeKeyCnt;
	long            *m_plChangeKey; //[TASK_AWARD_MAX_CHANGE_VALUE]
	long            *m_plChangeKeyValue;
	bool            *m_pbChangeType;
	// 切换场景即失败
	bool            m_bSwitchSceneFail;
	// 隐藏任务
	bool            m_bHidden;

	// 是否发放技能  [Yongdong, 2010-1-6] 
	bool			m_bDeliverySkill;
	int				m_iDeliveredSkillID;
	int				m_iDeliveredSkillLevel;
	
	// 是否显示任务完成光效 [Yongdong,2010-1-21]
	bool			m_bShowGfxFinished; // 任务完成时是否显示光效

	// PQ中是否更改玩家排名
	bool			m_bChangePQRanking;
	
	// 发放前对比发放物品与玩家包裹空位
	bool			m_bCompareItemAndInventory;
	unsigned long 	m_ulInventorySlotNum; // 所需包裹空位个数
	
/////////////////////////////////////////////// PQ Task start
	// 是否是PQ任务
	bool				m_bPQTask;
	// PQ任务全局变量显示
	unsigned long		m_ulPQExpCnt;
	char				(*m_pszPQExp)[TASK_AWARD_MAX_DISPLAY_CHAR_LEN];
	TASK_EXPRESSION		(*m_pPQExpArr)[TASK_AWARD_MAX_DISPLAY_CHAR_LEN];


	// 是否PQ子任务
	bool				m_bPQSubTask;
	// 任务开始时清空贡献度
	bool				m_bClearContrib;
	// 怪物种类数量
	unsigned long		m_ulMonsterContribCnt;
	// 怪物贡献度设定
	MONSTERS_CONTRIB*	m_MonstersContrib;//[MAX_CONTRIB_MONSTERS]
	
/////////////////////////////////////////////// Account Task start
	// 记录完成结果任务完成个数
	int			      m_iPremNeedRecordTasksNum;
	bool			  m_bShowByNeedRecordTasksNum;
	//帮派贡献度
	int					m_iPremiseFactionContrib;
	bool				m_bShowByFactionContrib;
	//  是否账号限制完成次数
	bool				m_bAccountTaskLimit;
	//  是否角色限制完成次数
	bool				m_bRoleTaskLimit;
	//  账号限制完成次数，废弃使用了  现在通过发放间隔选择“普通”和m_lPeriodLimit来判断
	unsigned long		m_ulAccountTaskLimitCnt;
	bool				m_bLeaveFactionFail;
	//  是否失败时不增加完成次数
	bool				m_bNotIncCntWhenFailed;
	
	//  任务失败时不收取任务要求的物品
	bool				m_bNotClearItemWhenFailed;
	//  是否显示在称号任务界面里
	bool				m_bDisplayInTitleTaskUI;

	/* 开启条件 */

	// 变身状态
	unsigned char	m_ucPremiseTransformedForm;
	bool			m_bShowByTransformed;
	// 等级条件
	unsigned long	m_ulPremise_Lev_Min;
	unsigned long	m_ulPremise_Lev_Max;
	unsigned long	m_bPremCheckMaxHistoryLevel;
	bool			m_bShowByLev;
	// 转生次数
	bool			m_bPremCheckReincarnation;
	unsigned long	m_ulPremReincarnationMin;
	unsigned long	m_ulPremReincarnationMax;
	bool			m_bShowByReincarnation;
	// 境界
	bool			m_bPremCheckRealmLevel;
	unsigned long	m_ulPremRealmLevelMin;
	unsigned long	m_ulPremRealmLevelMax;
	bool			m_bPremCheckRealmExpFull;
	bool			m_bShowByRealmLevel;
	// 所需道具
	unsigned long	m_ulPremItems;
	ITEM_WANTED*	m_PremItems; //[MAX_ITEM_WANTED];
	bool			m_bShowByItems;
	bool			m_bPremItemsAnyOne;
	// 发放道具
	unsigned long	m_ulGivenItems;
	unsigned long	m_ulGivenCmnCount;
	unsigned long	m_ulGivenTskCount;
	ITEM_WANTED*	m_GivenItems; //[MAX_ITEM_WANTED];
	// 押金
	unsigned long	m_ulPremise_Deposit;
	bool			m_bShowByDeposit;
	// 声望
	long			m_lPremise_Reputation;
	long			m_lPremise_RepuMax;
	bool			m_bShowByRepu;
	// 完成特定任务（成功？失败？），Task ID最高位1表示条件为失败，0为成功
	unsigned long	m_ulPremise_Task_Count;
	unsigned long	m_ulPremise_Tasks[MAX_PREM_TASK_COUNT];
	bool			m_bShowByPreTask;
	unsigned long   m_ulPremise_Task_Least_Num;//多个前提任务需要完成若干个
	// 达到特定时期
	unsigned long	m_ulPremise_Period;
	bool			m_bShowByPeriod;
	// 帮派
	unsigned long	m_ulPremise_Faction;
	int				m_iPremise_FactionRole;
	bool			m_bShowByFaction;

	// 性别
	unsigned long	m_ulGender;
	bool			m_bShowByGender;
	// 职业限制
	unsigned long	m_ulOccupations;
	unsigned long	m_Occupations[MAX_OCCUPATIONS];
	bool			m_bShowByOccup;
	// 夫妻
	bool			m_bPremise_Spouse;
	bool			m_bShowBySpouse;
	bool			m_bPremiseWeddingOwner;
	bool			m_bShowByWeddingOwner;
	// GM
	bool			m_bGM;
	// 完美神盾用户
	bool			m_bShieldUser;

	//  账号累计充值金额(下限&上限)
	bool			m_bShowByRMB;
	unsigned long	m_ulPremRMBMin;
	unsigned long	m_ulPremRMBMax;
	
	//  角色相关时间
	bool			m_bCharTime;
	bool			m_bShowByCharTime;
	int				m_iCharStartTime;
	int				m_iCharEndTime;	// 为0则为当前时间；为1则为m_tmCharEndTime指定时间；
	task_tm			m_tmCharEndTime;
	unsigned long	m_ulCharTimeGreaterThan;

	// 关联任务
	unsigned long	m_ulPremise_Cotask;
	unsigned long	m_ulCoTaskCond;
	// 互斥任务
	unsigned long	m_ulMutexTaskCount;
	unsigned long	m_ulMutexTasks[MAX_MUTEX_TASK_COUNT];
	// 生活技能级别
	long			m_lSkillLev[MAX_LIVING_SKILLS];
	// 动态任务类型
	char			m_DynTaskType;
	// 特殊奖励号，用于运营的活动
	unsigned long	m_ulSpecialAward;

	// 组队信息
	bool			m_bTeamwork;		// 组队任务
	bool			m_bRcvByTeam;		// 必须组队接收
	bool			m_bSharedTask;		// 新队员分享任务
	bool			m_bSharedAchieved;	// 分享杀怪、物品数量
	bool			m_bCheckTeammate;	// 检查队友位置
	float			m_fTeammateDist;	// 队友距离平方值
	bool			m_bAllFail;			// 任意队员失败则全部失败
	bool			m_bCapFail;			// 队长失败则全部失败
	bool			m_bCapSucc;			// 队长成功则全队成功
	float			m_fSuccDist;		// 成功时队员的距离
	bool			m_bDismAsSelfFail;	// 队员离队自身失败
	bool			m_bRcvChckMem;		// 接任务时检查队员位置
	float			m_fRcvMemDist;		// 接任务时队员距离平方值
	bool			m_bCntByMemPos;		// 队员在有效范围内杀怪有效
	float			m_fCntMemDist;		// 队员有效的范围
	bool			m_bAllSucc;			// 任意队员成功则全部成功
	bool			m_bCoupleOnly;		// 队长队员必须为夫妻
	bool			m_bDistinguishedOcc;// 队伍中不允许有相同的职业

	unsigned long	m_ulTeamMemsWanted;
	TEAM_MEM_WANTED*m_TeamMemsWanted; //[MAX_TEAM_MEM_WANTED];
	bool			m_bShowByTeam;

	// 前提全局key/value
	bool              m_bPremNeedComp;
	int				  m_nPremExp1AndOrExp2;
	COMPARE_KEY_VALUE m_Prem1KeyValue;
	COMPARE_KEY_VALUE m_Prem2KeyValue;
	
	bool			  m_bPremCheckForce;
	int				  m_iPremForce;
	bool			  m_bShowByForce;
	int				  m_iPremForceReputation; 
	bool			  m_bShowByForceReputation;	
	int				  m_iPremForceContribution;  // 扣除战功
	bool			  m_bShowByForceContribution;
	int				  m_iPremForceExp;			 // 经验兑换
	bool			  m_bShowByForceExp;
	int				  m_iPremForceSP;			 // 元神兑换
	bool			  m_bShowByForceSP;
	int				  m_iPremForceActivityLevel;
	bool			  m_bShowByForceActivityLevel;
	
	bool			  m_bPremIsKing;
	bool			  m_bShowByKing;
	bool			  m_bPremNotInTeam;
	bool			  m_bShowByNotInTeam;
	unsigned long	  m_iPremTitleNumTotal;
	unsigned long	  m_iPremTitleNumRequired;
	int*			  m_PremTitles;
	bool			  m_bShowByTitle;
	int				  m_iPremHistoryStageIndex[2]; // 历史阶段
	bool			  m_bShowByHistoryStage;

	unsigned long	  m_ulPremGeneralCardCount;	// 收集的卡牌数量
	bool			  m_bShowByGeneralCard;

	int				  m_iPremGeneralCardRank;		// 要求某品阶的卡牌数量
	unsigned long	  m_ulPremGeneralCardRankCount;
	bool			  m_bShowByGeneralCardRank;

	/* 任务完成的方式及条件 */

	unsigned long	m_enumMethod;
	unsigned long	m_enumFinishType;

	/* 任务方式 */

	unsigned long	m_ulPlayerWanted;
	PLAYER_WANTED*  m_PlayerWanted;   //[MAX_PLAYER_WANTED]
	unsigned long	m_ulMonsterWanted;
	MONSTER_WANTED*	m_MonsterWanted; //[MAX_MONSTER_WANTED];

	unsigned long	m_ulItemsWanted;
	ITEM_WANTED*	m_ItemsWanted; //[MAX_ITEM_WANTED];
	unsigned long	m_ulGoldWanted;

	int				m_iFactionContribWanted;
	int				m_iFactionExpContribWanted;

	unsigned long	m_ulNPCToProtect;
	unsigned long	m_ulProtectTimeLen;

	unsigned long	m_ulNPCMoving;
	unsigned long	m_ulNPCDestSite;

	//ZONE_VERT		m_ReachSiteMin;
	//ZONE_VERT		m_ReachSiteMax;
	Task_Region*	m_pReachSite;
	unsigned long	m_ulReachSiteCnt;
	unsigned long	m_ulReachSiteId;
	unsigned long	m_ulWaitTime;

	//藏宝图	使用已接任务列表中的m_iUsefulData1存储
	enum TREASURE_DISTANCE_LEVEL {
		DISTANCE_FAR_FAR_AWAY,
		DISTANCE_FAR,
		DISTANCE_MEDIUM,
		DISTANCE_NEAR,
		DISTANCE_VERY_NEAR,
		DISTANCE_NUM,
	};
	ZONE_VERT		m_TreasureStartZone;
	unsigned char	m_ucZonesNumX;
	unsigned char	m_ucZonesNumZ;
	unsigned char   m_ucZoneSide;

	//ZONE_VERT		m_LeaveSiteMin;
	//ZONE_VERT		m_LeaveSiteMax;
	Task_Region*	m_pLeaveSite;
	unsigned long	m_ulLeaveSiteCnt;
	unsigned long	m_ulLeaveSiteId;

	// 完成全局key/value
	bool              m_bFinNeedComp;
	int				  m_nFinExp1AndOrExp2;
	COMPARE_KEY_VALUE m_Fin1KeyValue;
	COMPARE_KEY_VALUE m_Fin2KeyValue;

	// 需显示的全局变量表达式
	unsigned long		m_ulExpCnt;
	char				(*m_pszExp)[TASK_AWARD_MAX_DISPLAY_CHAR_LEN];
	TASK_EXPRESSION 	(*m_pExpArr)[TASK_AWARD_MAX_DISPLAY_CHAR_LEN];

	// 需显示的全局变量表达式提示字符串
	unsigned long       m_ulTaskCharCnt;
	task_char           (*m_pTaskChar)[TASK_AWARD_MAX_DISPLAY_CHAR_LEN];

	// 变身状态
	unsigned char	m_ucTransformedForm;
	// 等级
	unsigned long	m_ulReachLevel;
	// 转生次数
	unsigned long	m_ulReachReincarnationCount;
	// 境界等级
	unsigned long	m_ulReachRealmLevel;

	unsigned int	m_uiEmotion; // 表情动作


	/* 任务结束后的奖励 */

	unsigned long	m_ulAwardType_S;
	unsigned long	m_ulAwardType_F;

	/* 普通和按每个方式 */
	AWARD_DATA* m_Award_S;	/* 成功 */
	AWARD_DATA* m_Award_F;	/* 失败 */

	/* 时间比例方式 */
	AWARD_RATIO_SCALE* m_AwByRatio_S;
	AWARD_RATIO_SCALE* m_AwByRatio_F;

	/* 按获得物比例方式 */
	AWARD_ITEMS_SCALE* m_AwByItems_S;
	AWARD_ITEMS_SCALE* m_AwByItems_F;

	/* 层次结构 */
	unsigned long	m_ulParent;
	unsigned long	m_ulPrevSibling;
	unsigned long	m_ulNextSibling;
	unsigned long	m_ulFirstChild;

	/* 库任务相关 */
	bool			m_bIsLibraryTask;
	float			m_fLibraryTasksProbability;
	bool			m_bIsUniqueStorageTask;
	int				m_iWorldContribution;       // 世界贡献度要求

	void* GetFixedDataPtr() { return (void*)this; }
	void Init();
	bool LoadFixedDataFromTextFile(FILE* fp, unsigned long ulVersion);
	bool LoadFixedDataFromBinFile(FILE* fp);
	bool HasFailAward() const { return m_Award_F->HasAward() || m_AwByRatio_F->HasAward() || m_AwByItems_F->HasAward(); }
	bool HasSuccAward() const {	return m_Award_S->HasAward() || m_AwByRatio_S->HasAward() || m_AwByItems_S->HasAward();	}
	bool CanRedo() const { return (!m_bNeedRecord || m_bCanRedo || m_bCanRedoAfterFailure); }

#ifdef _TASK_CLIENT
	void SaveFixedDataToTextFile(FILE* fp);
	void SaveFixedDataToBinFile(FILE* fp);
	void SetName(const wchar_t* szName)
	{
		memset(m_szName, 0, sizeof(m_szName));
		wcsncpy((wchar_t*)m_szName, szName, MAX_TASK_NAME_LEN-1);
	}
	const wchar_t* GetName() const { return (const wchar_t*)m_szName; }
#endif
};

#pragma pack()

inline void ATaskTemplFixedData::Init()
{
	m_bCanRedo				= true;
	m_bParentAlsoFail		= true;
	m_bCanGiveUp			= true;
	m_bCanRedoAfterFailure	= true;
	m_bClearAcquired		= true;
	m_enumMethod			= enumTMNone;
	m_enumFinishType		= enumTFTDirect;
	m_bShowByDeposit		= true;
	m_bShowByGender			= true;
	m_bShowByItems			= true;
	m_bShowByFactionContrib = true;
	m_bShowByNeedRecordTasksNum = true;
	m_bShowByLev			= true;
	m_bShowByOccup			= true;
	m_bShowByPreTask		= true;
	m_bShowByRepu			= true;
	m_bShowByTeam			= true;
	m_bShowByFaction		= true;
	m_bShowByPeriod			= true;
	m_bShowPrompt			= true;
	m_bShowByCharTime		= true;
	m_bShowByRMB			= true;
	m_bShowBySpouse			= true;
	m_bShowByWeddingOwner	= true;
	m_bShowByKing			= true;
	m_bShowByNotInTeam		= true;
	m_bShowByGeneralCard	= true;
	m_ulDelvWorld			= 1;
	m_ulTransWldId			= 1;
	m_ulReachSiteId			= 1;
	m_lPeriodLimit			= 1;
	m_bCanSeekOut			= false;
	m_bShowDirection		= true;

	m_bDeliverySkill		= false; // verison 77
	m_iDeliveredSkillID		= 0;
	m_iDeliveredSkillLevel  = 0;

	m_bShowGfxFinished = false; // version 78

	m_bChangePQRanking = false; // version 79. 默认 PQ 改边玩家排名。

	m_bDisplayInExclusiveUI = false;

	m_bReadyToNotifyServer = false;

	m_bDistinguishedOcc = false;

	m_bUsedInTokenShop = false;

	m_iPremise_FactionRole = 6;

	m_TreasureStartZone.x = 0;
	m_TreasureStartZone.y = 0;
	m_TreasureStartZone.z = 0;
	m_ucZonesNumX = 1;
	m_ucZonesNumZ = 1;
	m_ucZoneSide = 10;

	m_ucPremiseTransformedForm = 0xFF;

	m_bShowByForce = true;
	m_bShowByForceContribution = true;
	m_bShowByForceReputation = true;
	m_bShowByForceExp = true;
	m_bShowByForceSP = true;
	m_bShowByForceActivityLevel = true;
	m_bShowByReincarnation = true;
	m_bShowByRealmLevel = true;
	m_bShowByGeneralCardRank = true;
	m_bShowByHistoryStage = true;

	m_iPremForceActivityLevel = -1;
	m_iPremGeneralCardRank = -1;
}

class ATaskTempl : public ATaskTemplFixedData
{
public:
	ATaskTempl() :
	m_pwstrDescript(0),
	m_pwstrOkText(0),
	m_pwstrNoText(0),
	m_pwstrTribute(0),
	m_pParent(0),
	m_pPrevSibling(0),
	m_pNextSibling(0),
	m_pFirstChild(0),
	m_uDepth(1),
	m_ulMask(0),
	m_nSubCount(0),
	m_uValidCount(0),
	m_ulPremStorageItem(0)
	{
		m_szFilePath[0] = '\0';
		memset(&m_DelvTaskTalk, 0, sizeof(talk_proc));
		memset(&m_UnqualifiedTalk, 0, sizeof(talk_proc));
		memset(&m_DelvItemTalk, 0, sizeof(talk_proc));
		memset(&m_ExeTalk, 0, sizeof(talk_proc));
		memset(&m_AwardTalk, 0, sizeof(talk_proc));
	}

	virtual ~ATaskTempl()
	{
		if (m_pParent && m_pParent->m_pFirstChild == this)
			m_pParent->m_pFirstChild = m_pNextSibling;

		if (m_pPrevSibling) m_pPrevSibling->m_pNextSibling = m_pNextSibling;
		if (m_pNextSibling) m_pNextSibling->m_pPrevSibling = m_pPrevSibling;

		ATaskTempl* pChild = m_pFirstChild;
		while (pChild)
		{
			ATaskTempl* pNext = pChild->m_pNextSibling;
			LOG_DELETE(pChild);
			pChild = pNext;
		}

		LOG_DELETE_ARR(m_pwstrDescript);
		LOG_DELETE_ARR(m_pwstrOkText);
		LOG_DELETE_ARR(m_pwstrNoText);
		LOG_DELETE_ARR(m_pwstrTribute);
	}

public:
	// 任务描述
	task_char*		m_pwstrDescript;
	task_char*		m_pwstrOkText;
	task_char*		m_pwstrNoText;
	task_char*		m_pwstrTribute;

	char			m_szFilePath[TASK_MAX_PATH];

	/* 层次结构 */
	ATaskTempl*		m_pParent;
	ATaskTempl*		m_pPrevSibling;
	ATaskTempl*		m_pNextSibling;
	ATaskTempl*		m_pFirstChild;
	unsigned char	m_uDepth;
	unsigned long	m_ulMask;
	int				m_nSubCount;
	mutable unsigned char m_uValidCount;
	unsigned long	m_ulPremStorageItem;

	struct TASK_TIME_MARK
	{
		unsigned long	m_ulPlayerId;
		unsigned long	m_ulStartTime;

		void SetData(unsigned long ulPlayerId, unsigned long ulStartTime)
		{
			m_ulPlayerId	= ulPlayerId;
			m_ulStartTime	= ulStartTime;
		}
	};

	// Talk data
	talk_proc m_DelvTaskTalk;
	talk_proc m_UnqualifiedTalk;
	talk_proc m_DelvItemTalk;
	talk_proc m_ExeTalk;
	talk_proc m_AwardTalk;

protected:
	bool Load(FILE* fp, unsigned long ulVersion, bool bTextFile);

#ifdef _TASK_CLIENT
	void Save(FILE* fp, bool bTextFile);
	void SaveBinary(FILE* fp);

#endif

public:
	static ATaskTempl* CreateEmptyTaskTempl();

	void AddSubTaskTempl(ATaskTempl* pSub);
	void InsertTaskTemplBefore(ATaskTempl* pInsert);
	void SynchID()
	{
		m_ulParent = (m_pParent ? m_pParent->m_ID : 0);
		m_ulNextSibling = (m_pNextSibling ? m_pNextSibling->m_ID : 0);
		m_ulPrevSibling = (m_pPrevSibling ? m_pPrevSibling->m_ID : 0);
		m_ulFirstChild = (m_pFirstChild ? m_pFirstChild->m_ID : 0);
		if (m_pFirstChild) m_enumMethod = enumTMNone;
	}
	ATaskTempl* GetSubById(unsigned long ulId)
	{
		ATaskTempl* pChild = m_pFirstChild;

		while (pChild)
		{
			if (pChild->m_ID == ulId) return pChild;
			pChild = pChild->m_pNextSibling;
		}

		return NULL;
	}
	const ATaskTempl* GetConstSubById(unsigned long ulId) const
	{
		const ATaskTempl* pChild = m_pFirstChild;

		while (pChild)
		{
			if (pChild->m_ID == ulId) return pChild;
			pChild = pChild->m_pNextSibling;
		}

		return NULL;
	}
	const ATaskTempl* GetSubByIndex(int nIndex) const
	{
		const ATaskTempl* pChild = m_pFirstChild;

		while (pChild)
		{
			if (nIndex-- == 0)
				return pChild;

			pChild = pChild->m_pNextSibling;
		}

		return NULL;
	}
	const ATaskTempl* GetTopTask() const
	{
		const ATaskTempl* pTop = this;
		const ATaskTempl* pParent = m_pParent;

		while (pParent)
		{
			pTop = pParent;
			pParent = pParent->m_pParent;
		}

		return pTop;
	}
	int GetSubCount() const
	{
		int nCount = 0;
		ATaskTempl* pChild = m_pFirstChild;

		while (pChild)
		{
			nCount++;
			pChild = pChild->m_pNextSibling;
		}

		return nCount;
	}
	void CheckDepth()
	{
		ATaskTempl* pChild = m_pFirstChild;
		unsigned char uDepth = 0;
		bool bMaxChildDepth = m_bExeChildInOrder || m_bChooseOne || m_bRandOne;

		while (pChild)
		{
			pChild->CheckDepth();

			if (bMaxChildDepth)
			{
				if (uDepth < pChild->m_uDepth)
					uDepth = pChild->m_uDepth;
			}
			else
				uDepth += pChild->m_uDepth;

			pChild = pChild->m_pNextSibling;
		}

		m_uDepth += uDepth;
	}

	void CheckMask();
	bool LoadFromTextFile(FILE* fp, bool bLoadDescript);
	bool LoadFromTextFile(const char* szPath, bool bLoadDescript = true);
	bool LoadBinary(FILE* fp);
	bool LoadFromBinFile(FILE* fp);
	bool LoadDescription(FILE* fp);
	bool LoadDescriptionBin(FILE* fp);
	bool LoadTribute(FILE* fp);
	bool LoadTributeBin(FILE* fp);
	int UnmarshalKillMonster(const char* pData);
	int UnmarshalCollectItems(const char* pData);
	int UnmarshalDynTask(const char* pData);
	int UnmarshalSpecialAwardData(const char* pData);

	// inline funcs
	unsigned long GetID() const { return m_ID; }
	unsigned long GetType() const { return m_ulType; }
	unsigned long HasSignature() const { return m_bHasSign; }
	const task_char* GetSignature() const { return m_pszSignature; }
	bool CanShowDirection() const { return m_bShowDirection; }
	void IncValidCount() const { m_uValidCount++; }
	void ClearValidCount() const { m_uValidCount = 0; }
	bool IsValidState() const { return m_uValidCount < TASK_MAX_VALID_COUNT; }

	bool IsNotChangePQRanking() { return m_bChangePQRanking;}

	void ConfirmFinishAutoDelTask(){m_bReadyToNotifyServer = true;}
	void ResetAutoDelTask() const {const_cast<ATaskTempl*>(this)->m_bReadyToNotifyServer = false;}

#ifdef _TASK_CLIENT
	void SyncTaskType(); // 使子任务的任务类型随父任务
	bool GetShowGfxFlag() { return m_bShowGfxFinished;} 
	const wchar_t* GetDescription() const { assert(m_pwstrDescript); return (wchar_t*)m_pwstrDescript; }
	const wchar_t* GetOkText() const { assert(m_pwstrOkText); return (wchar_t*)m_pwstrOkText; }
	const wchar_t* GetNoText() const { assert(m_pwstrNoText); return (wchar_t*)m_pwstrNoText; }
	const wchar_t* GetTribute() const { assert(m_pwstrTribute); return (wchar_t*)m_pwstrTribute; }

	const talk_proc* GetDeliverTaskTalk() const { return &m_DelvTaskTalk; }
	const talk_proc* GetUnqualifiedTalk() const { return &m_UnqualifiedTalk; }
	const talk_proc* GetDeliverItemTalk() const { return &m_DelvItemTalk; }
	const talk_proc* GetUnfinishedTalk() const { return &m_ExeTalk; }
	const talk_proc* GetAwardTalk() const { return &m_AwardTalk; }

	unsigned long GetDeliverNPC() const { return m_ulDelvNPC; }
	unsigned long GetAwardNPC() const { return m_ulAwardNPC; }

	void SaveToTextFile(FILE* fp);
	bool SaveToTextFile(const char* szPath);
	void SaveToBinFile(FILE* fp) { SaveBinary(fp); }
	void SaveDescription(FILE* fp);
	void SaveDescriptionBin(FILE* fp);
	void SaveTribute(FILE* fp);
	void SaveTributeBin(FILE* fp);
	void SaveAllText(FILE* fp);
	int MarshalKillMonster(char* pData);
	int MarshalCollectItems(char* pData);
	int MarshalDynTask(char* pData);
	int MarshalSpecialAwardData(char* pData);

	ATaskTempl& operator= (const ATaskTempl& src);
	bool operator == (const ATaskTempl& src) const
	{
		return *(ATaskTemplFixedData*)this == *(const ATaskTemplFixedData*)&src;
	}
#endif
	const char* GetFilePath() const { return m_szFilePath; }
	void SetFilePath(const char* szPath) { strcpy(m_szFilePath, szPath); }

	// Process Part, all const functions

public:
	ActiveTaskEntry* DeliverTask(
		TaskInterface* pTask,
		ActiveTaskList* pList,
		ActiveTaskEntry* pEntry,
		unsigned long ulCaptainTask,
		unsigned long& ulMask,
		unsigned long ulCurTime,
		const ATaskTempl* pSubTempl,
		task_sub_tags* pSubTag,
		TaskGlobalData* pGlobal,
		unsigned char uParentIndex = (unsigned char)0xff) const;

	void RecursiveAward				(TaskInterface* pTask, ActiveTaskList* pList, ActiveTaskEntry* pEntry, unsigned long ulCurTime, int nChoice, task_sub_tags* pSubTag) const;
	void RecursiveCheckTimeLimit	(TaskInterface* pTask, ActiveTaskList* pList, ActiveTaskEntry* pEntry, unsigned long ulCurTime) const;
	bool RecursiveCheckParent		(TaskInterface* pTask, ActiveTaskList* pList, ActiveTaskEntry* pEntry) const;
	bool HasAllItemsWanted			(TaskInterface* pTask) const;
	bool HasAllMonsterWanted		(TaskInterface* pTask, const ActiveTaskEntry* pEntry) const;
	bool HasAllPlayerKilled			(TaskInterface* pTask, const ActiveTaskEntry* pEntry) const;
	void CalcAwardDataByRatio		(AWARD_DATA* pAward, ActiveTaskEntry* pEntry, unsigned long ulTaskTime, unsigned long ulCurTime) const;
	void CalcAwardDataByItems		(TaskInterface* pTask, AWARD_DATA* pAward, ActiveTaskEntry* pEntry) const;
	bool CanAwardItems				(TaskInterface* pTask, const AWARD_ITEMS_CAND* pAward) const;
	void CalcAwardItemsCount		(TaskInterface* pTask, const AWARD_ITEMS_CAND* pAward, unsigned long& ulCmnCount, unsigned long& ulTskCount) const;
	unsigned long CheckAwardWhenAtCrossServer(TaskInterface* pTask, const AWARD_DATA* pAward) const;
	unsigned long RecursiveCalcAward(TaskInterface* pTask, ActiveTaskList* pList, ActiveTaskEntry* pEntry, unsigned long ulCurTime, int nChoice, unsigned long& ulCmnCount, unsigned long& ulTskCount, unsigned long& ulTopCount, unsigned char& uBudget, long& lReputation) const;

	unsigned long CheckFnshLst		(TaskInterface* pTask, unsigned long ulCurTime) const;
	unsigned long CheckRecordTasksNum(TaskInterface* pTask) const;
	unsigned long CheckFactionContrib(TaskInterface* pTask) const;
	unsigned long CheckLevel		(TaskInterface* pTask) const;
	unsigned long CheckReincarnation(TaskInterface* pTask) const;
	unsigned long CheckRealmLevel(TaskInterface* pTask) const;
	unsigned long CheckRealmExpFull(TaskInterface* pTask) const;
	unsigned long CheckRepu			(TaskInterface* pTask) const;
	unsigned long CheckDeposit		(TaskInterface* pTask) const;
	unsigned long CheckItems		(TaskInterface* pTask) const;
	unsigned long CheckFaction		(TaskInterface* pTask) const;
	unsigned long CheckGender		(TaskInterface* pTask) const;
	unsigned long CheckOccupation	(TaskInterface* pTask) const;
	unsigned long CheckPeriod		(TaskInterface* pTask) const;
	unsigned long CheckGM			(TaskInterface* pTask) const;
	unsigned long CheckShieldUser	(TaskInterface* pTask) const;
	unsigned long CheckTeamTask		(TaskInterface* pTask) const;
	unsigned long CheckMutexTask	(TaskInterface* pTask, unsigned long ulCurTime) const;
	unsigned long CheckInZone		(TaskInterface* pTask) const;
	unsigned long CheckGivenItems	(TaskInterface* pTask) const;
	unsigned long CheckLivingSkill	(TaskInterface* pTask) const;
	unsigned long CheckPreTask		(TaskInterface* pTask) const;
	unsigned long CheckTimetable	(unsigned long ulCurTime) const;
	unsigned long CheckDeliverTime	(TaskInterface* pTask, unsigned long ulCurTime) const;	
	unsigned long CheckSpecialAward	(TaskInterface* pTask) const;
	unsigned long CheckSpouse		(TaskInterface* pTask) const;
	unsigned long CheckWeddingOwner (TaskInterface* pTask) const;
	unsigned long CheckGlobalKeyValue(TaskInterface* pTask, bool bFinCheck = false) const;
	unsigned long CheckGlobalPQKeyValue(bool bFinCheck = false) const;
	unsigned long CheckDeliverCount (TaskInterface* pTask) const;
	unsigned long CheckAccountRMB	(TaskInterface* pTask) const;
	unsigned long CheckCharTime		(TaskInterface* pTask) const;

	unsigned long CheckIvtrEmptySlot(TaskInterface* pTask) const;
	
	unsigned long CheckTransform(TaskInterface* pTask) const;
	unsigned long CheckForce(TaskInterface* pTask) const;
	unsigned long CheckForceReputation(TaskInterface* pTask) const;
	unsigned long CheckForceContribution(TaskInterface* pTask) const;
	unsigned long CheckExp(TaskInterface* pTask) const;
	unsigned long CheckSP(TaskInterface* pTask) const;
	unsigned long CheckForceActivityLevel(TaskInterface* pTask) const;
	unsigned long CheckKing(TaskInterface* pTask) const;
	unsigned long CheckNotInTeam(TaskInterface* pTask) const;
	unsigned long CheckTitle(TaskInterface* pTask) const;
	unsigned long CheckHistoryStage(TaskInterface* pTask) const;
	bool		  CheckReachLevel(TaskInterface* pTask) const;
	unsigned long CheckCardCollection(TaskInterface* pTask) const;
	unsigned long CheckCardRankCount(TaskInterface* pTask) const;
	unsigned long CheckInTransformShape(TaskInterface* pTask) const;

#ifdef _TASK_CLIENT
#else
	void RemovePrerequisiteItem		(TaskInterface* pTask) const;
	unsigned long DeliverByAwardData(TaskInterface* pTask, ActiveTaskList* pList, ActiveTaskEntry* pEntry, const AWARD_DATA* pAward, unsigned long ulCurTime, int nChoice) const;
	void DeliverTeamMemTask			(TaskInterface* pTask, TaskGlobalData* pGlobal, unsigned long ulCurTime) const;
	bool CheckTeamMemPos			(TaskInterface* pTask, ActiveTaskEntry* pEntry, float fSqrDist) const;
	void DeliverGivenItems			(TaskInterface* pTask) const;
	const ATaskTempl* RandOneChild	(TaskInterface* pTask, int& nSub) const;
	void AwardNotifyTeamMem			(TaskInterface* pTask, ActiveTaskEntry* pEntry) const;
	unsigned long CalMultiByGlobalKeyValue(TaskInterface* pTask, const AWARD_DATA* pAward) const;

#endif

public:
	unsigned long CheckBudget		(ActiveTaskList* pList) const;
	unsigned long CheckPrerequisite	(TaskInterface* pTask, ActiveTaskList* pList, unsigned long ulCurTime, bool bCheckPrevTask = true, bool bCheckTeam = true, bool bCheckBudge = true, bool bCheckLevel = true) const;
	unsigned long CheckMarriage		(TaskInterface* pTask) const;
	bool CanFinishTask				(TaskInterface* pTask, const ActiveTaskEntry* pEntry, unsigned long ulCurTime) const;
	void OnServerNotify				(TaskInterface* pTask, ActiveTaskEntry* pEntry, const task_notify_base* pNotify, size_t sz) const;
	void CalcAwardData				(TaskInterface* pTask, AWARD_DATA* pAward, ActiveTaskEntry* pEntry, unsigned long ulTaskTime, unsigned long ulCurTime) const;
	unsigned long CalcAwardMulti	(TaskInterface* pTask, ActiveTaskEntry* pEntry, unsigned long ulTaskTime, unsigned long ulCurTime) const;
	unsigned long GetMemTaskByInfo	(const task_team_member_info* pInfo) const;
	bool IsAutoDeliver				() const;
	unsigned long GetSuitableLevel	() { return GetTopTask()->m_ulSuitableLevel; }
	bool CanShowPrompt				() const { return GetTopTask()->m_bShowPrompt; }
	bool IsKeyTask					() const { return GetTopTask()->m_bKeyTask; }
	unsigned long HasAllTeamMemsWanted(TaskInterface* pTask, bool bStrict) const;
	unsigned long RecursiveCheckAward(TaskInterface* pTask, ActiveTaskList* pList, ActiveTaskEntry* pEntry, unsigned long ulCurTime, int nChoice) const;
	bool CanDeliverWorldContribution(TaskInterface* pTask) const;

	class TaskRecursiveChecker{
	public:
		virtual ~TaskRecursiveChecker(){}
		virtual bool Downward()const=0;
		virtual bool IsMatch(const ATaskTempl *pTask)const=0;
	};
	bool RecursiveCheck(TaskRecursiveChecker *pChecker)const;
	

#ifdef _TASK_CLIENT
	bool CanShowTask				(TaskInterface* pTask) const;
	bool HasShowCond()				const;	
	void GetGlobalTaskChar			(TaskInterface* pTask, abase::vector<wchar_t*>& TaskCharArr) const;
	float CalcOneGlobalExp			(TaskInterface* pTask, int nIndex) const;
	bool CanShowInExclusiveUI		(TaskInterface* pTask, unsigned long ulCurTime) const;
#else
	void NotifyClient				(TaskInterface* pTask, const ActiveTaskEntry* pEntry, unsigned char uReason, unsigned long ulCurTime, unsigned long ulParam = 0, int dps = 0, int dph = 0) const;
	bool CheckGlobalRequired		(TaskInterface* pTask, unsigned long ulSubTaskId, const TaskPreservedData* pPreserve, const TaskGlobalData* pGlobal, unsigned short reason) const;
	bool CheckKillMonster			(TaskInterface* pTask, ActiveTaskList* pList, ActiveTaskEntry* pEntry, unsigned long ulTemplId, unsigned long ulLev, bool bTeam, float fRand, int dps, int dph) const;
	bool CheckKillPlayer			(TaskInterface* pTask, ActiveTaskList* pList, ActiveTaskEntry* pEntry, int iOccupation, int iLevel, bool bGender, int iForce, float fRand) const;
	void CheckCollectItem			(TaskInterface* pTask, ActiveTaskList* pList, ActiveTaskEntry* pEntry, bool bAtNPC, int nChoice) const;
	void CheckMonsterKilled			(TaskInterface* pTask, ActiveTaskList* pList, ActiveTaskEntry* pEntry, bool bAtNPC, int nChoice) const;
	void CheckMining				(TaskInterface* pTask, ActiveTaskList* pList, ActiveTaskEntry* pEntry) const;
	void CheckWaitTime				(TaskInterface* pTask, ActiveTaskList* pList, ActiveTaskEntry* pEntry, unsigned long ulCurTime, bool bAtNPC, int nChoice) const;
	void GiveUpOneTask				(TaskInterface* pTask, ActiveTaskList* pList, ActiveTaskEntry* pEntry, bool bForce) const;
	void OnSetFinished				(TaskInterface* pTask, ActiveTaskList* pList, ActiveTaskEntry* pEntry, bool bNotifyMem = true) const;
	bool DeliverAward				(TaskInterface* pTask, ActiveTaskList* pList, ActiveTaskEntry* pEntry, int nChoice, bool bNotifyTeamMem = true, TaskGlobalData* pGlobal = NULL) const;
	void RemoveAcquiredItem			(TaskInterface* pTask, bool bClearTask, bool bSuccess) const;
	void TakeAwayGivenItems			(TaskInterface* pTask) const;
	bool OnDeliverTeamMemTask		(TaskInterface* pTask, TaskGlobalData* pGlobal) const;
	unsigned long CheckDeliverTask	(TaskInterface* pTask, unsigned long ulSubTaskId, TaskGlobalData* pGlobal, bool bNotifyErr = true, bool bMemTask = false, unsigned long ulCapId = 0) const;
	bool HasGlobalData() const;
#endif

};

#ifdef _TASK_CLIENT
enum ENUM_TASK_TYPE_OLD
{
	enumTTCommonOld = 0,		// 普通
		enumTTRepeatedOld,			// 重复
		enumTTKeyOld,				// 主线
		enumTTBossOld,				// Boss
		enumTTInstanceOld,			// 副本
		enumTTScrollOld,			// 卷轴
		enumTTBusinessOld,			// 跑商
		enumTTFeelingOld,			// 真情
		enumTTTacticsOld,			// 破阵
		enumTTRegionOld,			// 区域
		enumTTSevenKillListOld,	// 七杀榜
		enumTTFactionOld,			// 帮派任务
		enumTTHintOld,				// 提示任务
};

int GetTaskTypeFromComboBox(int nComboBoxData);
int GetComboBoxFromTaskType(int nTaskType);
#endif

#include "TaskTempl.inl"

#endif
