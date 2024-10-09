#ifndef _AI_POLICY_TYPE_H_
#define _AI_POLICY_TYPE_H_


#define OP_TEXT_LENGTH 256

#ifdef _WINDOWS
typedef wchar_t policy_char;
#else
typedef unsigned short policy_char;
#endif

//	���ٻ�����ʧ��������
enum enumPolicySummoneeDisppearType
{
	enumPSDTNever = 0,						//	�����κ���������
	enumPSDTFollowSummoner,					//	���ٻ�������
	enumPSDTFollowSummonTarget,				//	���ٻ�Ŀ������
	enumPSDTFollowSummonerOrSummonTarget,	//	�ٻ��ߺ��ٻ�Ŀ������һ������������
	enumPSDTNum,
};

//	����Ѳ�߷�ʽ
enum enumPolicyMonsterPatrolType
{
	enumPMPTStopAtEnd = 0,	//	�յ�ֹͣ
	enumPMPTReturn,			//	ԭ·����
	enumPMPTLoop,			//	ʼ��ѭ��
	enumPMPTNum,
};

//	����Ѳ���ٶ�����
enum enumPolicyMonsterPatrolSpeedType
{
	enumPMPSTBegin = 1,
	enumPMPSTSlow = enumPMPSTBegin,	//	����
	enumPMPSTFast,					//	����
	enumPMPSTEnd = enumPMPSTFast,
};

//	��������ö��
enum enumPolicyVarType
{
	enumPVTGlobalVarID = 0,	//	ȫ�ֱ���ID
	enumPVTLocalVarID,		//	�ֲ�����ID
	enumPVTConst,			//	����
	enumPVTRandom,			//	��0-99��������ֵ
	enumPVTNum,
};

//	���ʽ��������
enum enumPolicyOperatorType
{
	enumPOTAdd = 0,			//	�ӷ� +
	enumPOTSub,				//	���� -
	enumPOTMul,				//	�˷� *
	enumPOTDiv,				//	���� /
	enumPOTMod,				//	ȡ�� %
	enumPOTNum,
};

//	���� PVP ���ֽ�������
enum enumPolicyFactionPVPPointType
{
	enumPFPPTMineCar = 0,	//	������Դ�����ͻ���
	enumPFPPTMineBase,		//	������Դ�������ͻ���
	enumPFPPTMineCarArrived,	//	������Դ���������ͻ���
};

//	˵����������ѡ��
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
	int iPathIDType;	//	iPathID ֵ�ĺ�����ͣ�ȡֵ enumPolicyVarType �� 0~2
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
	unsigned int uSkill; //����
	unsigned int uLevel; //����
};

struct O_TALK_TEXT
{
	unsigned int uSize;
	policy_char* szData;
	unsigned int uAppendDataMask;	//	��������ѡ��
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
	bool         bStop;//�Ƿ���ֹͣ����
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
	bool bIsValue;	//�Ƿ��ǰ�ֱ����ȫ�ֱ������ǰ�ȫ�ֱ���ֵ��ȫ�ֱ���
};

struct O_REVISE_GLOBAL
{
	int  iID;
	int  iValue;
};


struct O_SUMMON_MONSTER_VERSION6
{
	int	  iMonsterID;//���ٻ��Ĺ���ID
	int	  iRange;//��Ŀ��ĵľ���(m)
	int   iLife; //��ʧ�ӳ�ʱ��(��) , -1��ʾ����
	bool  bDispear;//�Ƿ���Ŀ����������ʧ
	bool  bUsePolicyTarget; //Ŀ������ 0,��ʾ����1��ʾ�ò��Ե�Ŀ��
	int   iPathID; //·��ID
	int	  iMonsterNum;//��������
};

struct O_SUMMON_MONSTER
{
	int	  iMonsterID;//���ٻ��Ĺ���ID
	int	  iRange;//��Ŀ��ĵľ���(m)
	int   iLife; //��ʧ�ӳ�ʱ��(��) , -1��ʾ����
	int   iDispearCondition;//�Ƿ���Ŀ����������ʧ������Ŀ�жϱ��ٻ��Ĺ����Ƿ���Ŀ������ٻ�����ʧ
					//0��1��2��3��4��ѡ��0�ǲ����κ�����������1�����ٻ���������2�����ٻ�Ŀ��������3���ٻ��ߺ��ٻ�Ŀ������һ������������

	int   iPathID; //·��ID
	int	  iMonsterNum;//��������
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
	char szActionName[128]; //��������
	bool bLoop;				//�Ƿ�ѭ������
	int	 iInterval;			//���ʱ�����
};

struct O_PLAY_ACTION
{
	char szActionName[128]; //��������
	//bool bLoop;				//�Ƿ�ѭ������
	int  iLoopCount;		//ѭ�����Ŵ��� 
	int	 iInterval;			//���ʱ�����
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
	bool bIsHistoryValue;	//  iValue ֵΪ��ʷ����ֵ������������
};

struct O_DELIVER_FACTION_PVP_POINTS
{
	unsigned int uType;
};

struct O_CALC_VAR
{
	int  iDst;			//	�洢�������ı���ID
	int  iDstType;		//	iDst ������ ȡֵ enumPolicyVarType �� 0~1
	int	 iSrc1;			//	����1
	int  iSrc1Type;		//	����1���ͣ��μ� enumPolicyVarType
	int  iOp;			//	����������ͣ����� enumPolicyOperatorType
	int	 iSrc2;		//	����2
	int  iSrc2Type;	//	����2���ͣ��μ� enumPolicyVarType
};

struct O_SUMMON_MONSTER_2
{
	int   iDispearCondition;//�Ƿ���Ŀ����������ʧ������Ŀ�жϱ��ٻ��Ĺ����Ƿ���Ŀ������ٻ�����ʧ
	//0��1��2��3��4��ѡ��0�ǲ����κ�����������1�����ٻ���������2�����ٻ�Ŀ��������3���ٻ��ߺ��ٻ�Ŀ������һ������������
	int	  iMonsterID;//���ٻ��Ĺ���ID
	int   iMonsterIDType;	//	iMonsterID ���壺enumPolicyVarType ��ȡֵ 0~2
	int	  iRange;//��Ŀ��ĵľ���(m)
	int   iLife; //��ʧ�ӳ�ʱ��(��) , -1��ʾ����	
	int   iPathID; //·��ID
	int	  iPathIDType;		//	iPathID ���壺enumPolicyVarType ��ȡֵ 0~2
	int	  iMonsterNum;//��������
	int	  iMonsterNumType;	//	iMonsterNum ���壺enumPolicyVarType ��ȡֵ 0~2
};

struct O_WALK_ALONG_2
{
	int iWorldID;		//	��ͼID
	int iPathID;
	int iPathIDType;	//	iPathID ���壺enumPolicyVarType ��ȡֵ 0~2
	int iPatrolType;	//	Ѳ�߷�ʽ
	int iSpeedType;		//	�ٶ�����
};

struct O_USE_SKILL_2
{
	unsigned int uSkill;		//����
	unsigned int uSkillType;	//uSkill ���壺enumPolicyVarType ��ȡֵ 0~2
	unsigned int uLevel;		//����
	unsigned int uLevelType;	//uLevel ���壺enumPolicyVarType ��ȡֵ 0~2
};

struct O_ACTIVE_CONTROLLER_2
{
	unsigned int uID;
	unsigned int uIDType;		//uID ���壺enumPolicyVarType ��ȡֵ 0~2
	bool         bStop;//�Ƿ���ֹͣ����
};

struct O_DELIVER_TASK
{
	unsigned int uID;
	unsigned int uIDType;		//uID ���壺enumPolicyVarType ��ȡֵ 0~2
};

struct O_SUMMON_MINE
{
	int   iLifeType;		//	iLife ���壺enumPolicyVarType ��ȡֵ 0~2
	int	  iMineID;
	int   iMineIDType;		//	iMineID ���壺enumPolicyVarType ��ȡֵ 0~2
	int	  iRange;			//	��Ŀ��ĵľ���(m)
	int   iLife;			//	��ʧ�ӳ�ʱ��(��) , -1��ʾ����
	int	  iMineNum;
	int	  iMineNumType;		//	iMineNum ���壺enumPolicyVarType ��ȡֵ 0~2
};

struct O_SUMMON_NPC
{
	int   iLifeType;		//	iLife ���壺enumPolicyVarType ��ȡֵ 0~2
	int	  iNPCID;
	int   iNPCIDType;		//	iNPCID ���壺enumPolicyVarType ��ȡֵ 0~2
	int	  iRange;			//	��Ŀ��ĵľ���(m)
	int   iLife;			//	��ʧ�ӳ�ʱ��(��) , -1��ʾ����	
	int   iPathID;			//	·��ID
	int	  iPathIDType;		//	iPathID ���壺enumPolicyVarType ��ȡֵ 0~2
	int	  iNPCNum;
	int	  iNPCNumType;		//	iNPCNum ���壺enumPolicyVarType ��ȡֵ 0~2
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
	POLICY_ZONE_VERT	zvMin;		//	����Χ��Сֵ
	POLICY_ZONE_VERT	zvMax;		//	����Χ���ֵ
};

struct O_DELIVER_TASK_IN_HATE_LIST
{
	unsigned int uID;
	unsigned int uIDType;		//uID ���壺enumPolicyVarType ��ȡֵ 0~2
	int			 iRange;		//��������ľ���(m)
	int			 iPlayerNum;	//��������������500��
};

//----------------------------------------------------------------------------------------
//Target Occupation
//----------------------------------------------------------------------------------------
struct T_OCCUPATION
{
	unsigned int uBit;
};

#endif
