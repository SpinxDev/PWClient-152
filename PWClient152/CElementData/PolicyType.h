#ifndef _AI_POLICY_TYPE_H_
#define _AI_POLICY_TYPE_H_


#define OP_TEXT_LENGTH 256

#ifdef _WINDOWS
typedef wchar_t policy_char;
#else
typedef unsigned short policy_char;
#endif

//	被召唤者消失条件类型
enum enumPolicySummoneeDisppearType
{
	enumPSDTNever = 0,						//	不随任何死亡死亡
	enumPSDTFollowSummoner,					//	随召唤者死亡
	enumPSDTFollowSummonTarget,				//	随召唤目标死亡
	enumPSDTFollowSummonerOrSummonTarget,	//	召唤者和召唤目标任意一个死亡即死亡
	enumPSDTNum,
};

//	怪物巡逻方式
enum enumPolicyMonsterPatrolType
{
	enumPMPTStopAtEnd = 0,	//	终点停止
	enumPMPTReturn,			//	原路返回
	enumPMPTLoop,			//	始终循环
	enumPMPTNum,
};

//	怪物巡逻速度类型
enum enumPolicyMonsterPatrolSpeedType
{
	enumPMPSTBegin = 1,
	enumPMPSTSlow = enumPMPSTBegin,	//	慢速
	enumPMPSTFast,					//	快速
	enumPMPSTEnd = enumPMPSTFast,
};

//	变量类型枚举
enum enumPolicyVarType
{
	enumPVTGlobalVarID = 0,	//	全局变量ID
	enumPVTLocalVarID,		//	局部变量ID
	enumPVTConst,			//	常数
	enumPVTRandom,			//	【0-99】间的随机值
	enumPVTNum,
};

//	表达式操作类型
enum enumPolicyOperatorType
{
	enumPOTAdd = 0,			//	加法 +
	enumPOTSub,				//	减法 -
	enumPOTMul,				//	乘法 *
	enumPOTDiv,				//	除法 /
	enumPOTMod,				//	取余 %
	enumPOTNum,
};

//	帮派 PVP 积分奖励类型
enum enumPolicyFactionPVPPointType
{
	enumPFPPTMineCar = 0,	//	帮派资源车类型积分
	enumPFPPTMineBase,		//	帮派资源基地类型积分
	enumPFPPTMineCarArrived,	//	帮派资源车到达类型积分
};

//	说话附加数据选项
enum enumPolicyTalkTextAppendDataMask
{
	enumPTTADMName		= 0x000001,
	enumPTTADMLocalVar0	= 0x000002,
	enumPTTADMLocalVar1	= 0x000004,
	enumPTTADMLocalVar2	= 0x000008,
};

//----------------------------------------------------------------------------------------
//Condition
//----------------------------------------------------------------------------------------

struct C_TIME_COME
{
	unsigned int uID;
};

struct C_HP_LESS
{
	float fPercent;
};


struct C_RANDOM
{
	float fProbability;
};

struct C_VAR
{
	int iID;
};

struct C_CONSTANT
{
	int iValue;
};

// struct C_TIME_POINT
// {
// 	unsigned int uHour;
// 	unsigned int uMinute;
// };

struct C_BE_HURT
{
	int iHurtLow;
	int iHurtHigh;
};

struct C_REACH_END
{
	int iPathID;
};

struct C_HISTORY_STAGE
{
	int iID;
};

struct C_HISTORY_VALUE
{
	int iValue;
};

struct C_LOCAL_VAR
{
	int iID;
};

struct C_REACH_END_2
{
	int iPathID;
	int iPathIDType;	//	iPathID 值的含义解释，取值 enumPolicyVarType 中 0~2
};

struct C_HAS_FILTER
{
	int iID;
};

//----------------------------------------------------------------------------------------
//Operation
//----------------------------------------------------------------------------------------

struct O_ATTACK_TYPE
{
	unsigned int uType;
};

struct O_USE_SKILL
{
	unsigned int uSkill; //技能
	unsigned int uLevel; //级别
};

struct O_TALK_TEXT
{
	unsigned int uSize;
	policy_char* szData;
	unsigned int uAppendDataMask;	//	附加数据选项
};

struct O_RUN_TRIGGER
{
	unsigned int uID;
};

struct O_STOP_TRIGGER
{
	unsigned int uID;
};

struct O_ACTIVE_TRIGGER
{
	unsigned int uID;
};

struct O_CREATE_TIMER
{
	unsigned int uID;
	unsigned int uPeriod;
	unsigned int uCounter;
};

struct O_KILL_TIMER
{
	unsigned int uID;
};

struct O_ACTIVE_CONTROLLER
{
	unsigned int uID;
	bool         bStop;//是否是停止激活
};

struct O_SET_GLOBAL_VERSION3
{
	int  iID;
	int  iValue;
};

struct O_SET_GLOBAL
{
	int  iID;
	int  iValue;
	bool bIsValue;	//是否是把直附给全局变量或是把全局变量值给全局变量
};

struct O_REVISE_GLOBAL
{
	int  iID;
	int  iValue;
};


struct O_SUMMON_MONSTER_VERSION6
{
	int	  iMonsterID;//被召唤的怪物ID
	int	  iRange;//与目标的的距离(m)
	int   iLife; //消失延迟时间(秒) , -1表示无穷
	bool  bDispear;//是否随目标死亡而消失
	bool  bUsePolicyTarget; //目标类型 0,表示自身，1表示用策略的目标
	int   iPathID; //路径ID
	int	  iMonsterNum;//怪物数量
};

struct O_SUMMON_MONSTER
{
	int	  iMonsterID;//被召唤的怪物ID
	int	  iRange;//与目标的的距离(m)
	int   iLife; //消失延迟时间(秒) , -1表示无穷
	int   iDispearCondition;//是否随目标死亡而消失。该项目判断被召唤的怪物是否随目标或者召唤者消失
					//0，1，2，3共4个选择。0是不随任何死亡死亡，1是随召唤者死亡，2是随召唤目标死亡，3是召唤者和召唤目标任意一个死亡即死亡

	int   iPathID; //路径ID
	int	  iMonsterNum;//怪物数量
};

struct O_WALK_ALONG
{
	int iWorldID;
	int iPathID;
	int iPatrolType;
	int iSpeedType;
};

struct O_PLAY_ACTION_VERSION8
{
	char szActionName[128]; //动作名字
	bool bLoop;				//是否循环播放
	int	 iInterval;			//间隔时间豪秒
};

struct O_PLAY_ACTION
{
	char szActionName[128]; //动作名字
	//bool bLoop;				//是否循环播放
	int  iLoopCount;		//循环播放次数 
	int	 iInterval;			//间隔时间豪秒
	int	 iPlayTime;
};

struct O_REVISE_HISTORY
{
	int  iID;
	int  iValue;
};

struct O_SET_HISTORY
{
	int  iID;
	int  iValue;
	bool bIsHistoryValue;	//  iValue 值为历史变量值，或其它含义
};

struct O_DELIVER_FACTION_PVP_POINTS
{
	unsigned int uType;
};

struct O_CALC_VAR
{
	int  iDst;			//	存储计算结果的变量ID
	int  iDstType;		//	iDst 的类型 取值 enumPolicyVarType 中 0~1
	int	 iSrc1;			//	参数1
	int  iSrc1Type;		//	参数1类型：参见 enumPolicyVarType
	int  iOp;			//	计算操作类型：参照 enumPolicyOperatorType
	int	 iSrc2;		//	参数2
	int  iSrc2Type;	//	参数2类型：参见 enumPolicyVarType
};

struct O_SUMMON_MONSTER_2
{
	int   iDispearCondition;//是否随目标死亡而消失。该项目判断被召唤的怪物是否随目标或者召唤者消失
	//0，1，2，3共4个选择。0是不随任何死亡死亡，1是随召唤者死亡，2是随召唤目标死亡，3是召唤者和召唤目标任意一个死亡即死亡
	int	  iMonsterID;//被召唤的怪物ID
	int   iMonsterIDType;	//	iMonsterID 含义：enumPolicyVarType 中取值 0~2
	int	  iRange;//与目标的的距离(m)
	int   iLife; //消失延迟时间(秒) , -1表示无穷	
	int   iPathID; //路径ID
	int	  iPathIDType;		//	iPathID 含义：enumPolicyVarType 中取值 0~2
	int	  iMonsterNum;//怪物数量
	int	  iMonsterNumType;	//	iMonsterNum 含义：enumPolicyVarType 中取值 0~2
};

struct O_WALK_ALONG_2
{
	int iWorldID;		//	地图ID
	int iPathID;
	int iPathIDType;	//	iPathID 含义：enumPolicyVarType 中取值 0~2
	int iPatrolType;	//	巡逻方式
	int iSpeedType;		//	速度类型
};

struct O_USE_SKILL_2
{
	unsigned int uSkill;		//技能
	unsigned int uSkillType;	//uSkill 含义：enumPolicyVarType 中取值 0~2
	unsigned int uLevel;		//级别
	unsigned int uLevelType;	//uLevel 含义：enumPolicyVarType 中取值 0~2
};

struct O_ACTIVE_CONTROLLER_2
{
	unsigned int uID;
	unsigned int uIDType;		//uID 含义：enumPolicyVarType 中取值 0~2
	bool         bStop;//是否是停止激活
};

struct O_DELIVER_TASK
{
	unsigned int uID;
	unsigned int uIDType;		//uID 含义：enumPolicyVarType 中取值 0~2
};

struct O_SUMMON_MINE
{
	int   iLifeType;		//	iLife 含义：enumPolicyVarType 中取值 0~2
	int	  iMineID;
	int   iMineIDType;		//	iMineID 含义：enumPolicyVarType 中取值 0~2
	int	  iRange;			//	与目标的的距离(m)
	int   iLife;			//	消失延迟时间(秒) , -1表示无穷
	int	  iMineNum;
	int	  iMineNumType;		//	iMineNum 含义：enumPolicyVarType 中取值 0~2
};

struct O_SUMMON_NPC
{
	int   iLifeType;		//	iLife 含义：enumPolicyVarType 中取值 0~2
	int	  iNPCID;
	int   iNPCIDType;		//	iNPCID 含义：enumPolicyVarType 中取值 0~2
	int	  iRange;			//	与目标的的距离(m)
	int   iLife;			//	消失延迟时间(秒) , -1表示无穷	
	int   iPathID;			//	路径ID
	int	  iPathIDType;		//	iPathID 含义：enumPolicyVarType 中取值 0~2
	int	  iNPCNum;
	int	  iNPCNumType;		//	iNPCNum 含义：enumPolicyVarType 中取值 0~2
};

struct POLICY_ZONE_VERT{
	union{
		struct{
			float x;
			float y;
			float z;
		};		
		float v[3];
	};
	void clear(){
		x = y = z = 0.0f;
	}
	bool operator == (const POLICY_ZONE_VERT& src) const{
		return (v[0] == src.v[0] && v[1] == src.v[1] && v[2] == src.v[2]);
	}	
	bool great_than(const float v[3]) const {
		return x >= v[0] && y >= v[1] && z >= v[2];
	}
	bool less_than (const float v[3]) const {
		return x <= v[0] && y <= v[1] && z <= v[2];
	}
};

struct O_DELIVER_RANDOM_TASK_IN_REGION
{
	unsigned int		uID;		//	TASK_LIST_CONFIG id
	POLICY_ZONE_VERT	zvMin;		//	区域范围最小值
	POLICY_ZONE_VERT	zvMax;		//	区域范围最大值
};

struct O_DELIVER_TASK_IN_HATE_LIST
{
	unsigned int uID;
	unsigned int uIDType;		//uID 含义：enumPolicyVarType 中取值 0~2
	int			 iRange;		//玩家与怪物的距离(m)
	int			 iPlayerNum;	//发放数量（上限500）
};

//----------------------------------------------------------------------------------------
//Target Occupation
//----------------------------------------------------------------------------------------
struct T_OCCUPATION
{
	unsigned int uBit;
};

#endif
