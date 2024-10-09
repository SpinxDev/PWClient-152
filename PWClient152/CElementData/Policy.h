#ifndef _AI_POLICY_H_
#define _AI_POLICY_H_


#include <vector.h>
#include "policytype.h"
#include <stdio.h>
#include <string.h>

#define F_POLICY_VERSION	1
#define F_TRIGGER_VERSION   21
#define F_POLICY_EXP_VERSION 1


//----------------------------------------------------------------------------------------
//CTriggerData
//----------------------------------------------------------------------------------------

class CTriggerData
{

public:	
	CTriggerData()
	{
		rootConditon = 0;
		bActive = false;
		bAttackValid = true;
	}

	//0-武侠, 1-法师, 2-巫师, 3-妖精, 4-妖兽, 5-刺客, 6-羽芒, 7-羽灵，8-剑灵，9-魅灵，10-夜影，11-月仙
	enum _e_occupation
	{
		o_wuxia			= 1,
		o_fashi			= 2,
		o_wushi			= 4,
		o_yaojing		= 8,
		o_yaoshou		= 16,
		o_cike			= 32,
		o_yumang		= 64,
		o_yuling		= 128,
		o_jianling		= 256,
		o_meiling		= 512,
		o_yeying		= 1024,
		o_yuexian		= 2048,
	};

	enum _e_condition
	{
		c_time_come = 0,		//指定定时器到时 	参数：定时器ID **只能单独用
		c_hp_less,				//血量少于百分比	参数：血量比例
		c_start_attack,			//战斗开始			参数：无意义   **只能单独用
		c_random,				//随机				参数：随机概率
		c_kill_player,			//杀死玩家　　　　	参数：无意义   **只能单独用
		c_not,					//一个条件的非
		c_or,					//两个条件或
		c_and,					//两个条件与
		c_died,                 //死亡时                           **只能单独用
		
		c_plus,					//加
		c_minus,                //减
		c_multiply,             //乘
		c_divide,               //除
		c_great,                //大于
		c_less,                 //小于
		c_equ,					//等于
		c_var,                  //变量，           参数：变量ID
		c_constant,             //常量，           参数：int
//		c_time_point,			//时间条件，	   参数：小时，分钟

		c_be_hurt,				//受到伤害，	   参数： 受到伤害上限， 受到伤害下限			
		c_reach_end,			//到达路径终点，
		c_at_history_stage,		//处于历史阶段
		c_history_value,		//历史变量值
		c_stop_fight,			//脱离战斗			参数：无
		c_local_var,			//局部变量			参数：局部变量ID
		c_reach_end_2,			//到达路径终点（路径ID允许由间接方式指定）
		c_has_filter,			//持有状态包        参数：状态包ID
		c_room_index,			//在随机地图主线路径中的顺序（以可随机地图块为单位）

		c_num,
	};

	enum _e_target
	{
		t_hate_first = 0,		//仇恨排名第一位
		t_hate_second,			//仇恨排名第二位
		t_hate_others,			//仇恨排名第二位及其以后所有对象中的随机一个
		t_most_hp,				//最多hp
		t_most_mp,				//最多mp
		t_least_hp,				//最少hp
		t_occupation_list,		//某些职业的组合	参数：职业组合表
		t_self,					//自己 
		t_monster_killer,		//怪物击杀归属玩家
		t_monster_birthplace_faction,	//怪物出生地归属帮派
		t_hate_random_one,		//仇恨列表随机一个目标
		t_hate_nearest,			//仇恨列表中最近目标
		t_hate_farthest,		//仇恨列表中最远目标
		t_hate_first_redirected,//仇恨排名第一位（宠物转为主人）
		t_num,
	};

	enum _e_operation
	{
		o_attact = 0,			//发起攻击　	参数：0 物理肉搏 1 物理弓箭 2 魔法类 3 肉搏＋远程
		o_use_skill,			//使用技能　	参数：指定技能和级别
		o_talk,					//说话		    参数：话的文字内容，unicode编码
		o_reset_hate_list,		//重置仇恨列表
		o_run_trigger,			//执行一个新触发器
		o_stop_trigger,			//停止一个触发器	参数：id
		o_active_trigger,		//激活一个触发器	参数：id
		o_create_timer,		    //创建一个定时器	参数：id
		o_kill_timer,			//停止一个定时器	参数：id
		
		o_flee,                 //逃跑
		o_set_hate_to_first,    //将选定目标调整到仇恨列表第一位
		o_set_hate_to_last,     //将选定目标的仇恨度调整到最低
		o_set_hate_fifty_percent,     //仇恨度列表中所有对象的仇恨度减少50%，最少减少到1
		o_skip_operation,       //跳过后面的操作条目
		o_active_controller,
		o_set_global,           //设置全局变量   param 变量ID 数值
		o_revise_global,        //修正全局变量   param 变量ID 数值
		o_summon_monster,
		o_walk_along,			//沿路径行走 参数：0 世界id ; 1 路径id; 2巡逻方式; 3速度类型
		o_play_action,			//播放指定动作 参数：0 动作名; 1 是否循环；2循环间隔
		o_revise_history,		//修正历史进度   param 变量ID 数值
		o_set_history,			//设置历史变量	 param 变量ID 数值
		o_deliver_faction_pvp_points,	//	发放帮派掠夺战积分 参数：0 帮派资源车 1 帮派资源基地
		o_calc_var,				//使用简单表达式计算变量（全局变量/局部变量）值
		o_summon_monster_2,		//召唤怪物（新）
		o_walk_along_2,			//沿路巡径（新）
		o_use_skill_2,			//使用技能（新）
		o_active_controller_2,	//激活一个触发器
		o_deliver_task,			//发放任务
		o_summon_mine,			//召唤矿
		o_summon_npc,			//召唤NPC
		o_deliver_random_task_in_region,	//	区域内随机发放任务
		o_deliver_task_in_hate_list,		//	仇恨列表内发放任务
		o_num,
	};

public:
	
	struct _s_condition
	{
		int iType;
		void *pParam;
	};

	struct _s_target
	{
		int iType;
		void *pParam;
	};

	struct _s_operation
	{
		int iType;
		void *pParam;
		_s_target mTarget;
	};

	struct _s_tree_item
	{
		_s_tree_item(){ mConditon.iType = 0; mConditon.pParam = 0; pLeft = 0; pRight = 0; }
		~_s_tree_item()
		{
			Free();
		}
		void FreeParam()
		{
			if(mConditon.pParam)
			{
				free(mConditon.pParam);
				mConditon.pParam = 0;
			}
		}
		void FreeBranch()
		{
			if (pLeft)
			{
				delete pLeft;
				pLeft = 0;
			}
			if (pRight)
			{
				delete pRight;
				pRight = 0;
			}
		}
		void Free()
		{
			FreeParam();
			FreeBranch();
		}
		_s_condition mConditon;
		_s_tree_item *pLeft;
		_s_tree_item *pRight;
	};
public:
	
	void			AddOperaion(unsigned int iType, void *pParam, _s_target *pTarget);
	void            AddOperaion(_s_operation*pOperation);
	int				GetOperaionNum(){ return listOperation.size(); }
	int				GetOperaion(unsigned int idx, unsigned int &iType, void **ppData, _s_target& target);
	_s_operation *	GetOperaion( unsigned int idx){ return listOperation[idx]; };
	void			SetOperation(unsigned int idx, unsigned int iType, void *pData, _s_target *pTarget);
	void			DelOperation(unsigned int idx);

	_s_tree_item *	GetConditonRoot();
	void            ReleaseConditionTree();
	void            SetConditionRoot( _s_tree_item *pRoot){ rootConditon = pRoot; }
	unsigned int GetID(){ return uID; }
	void         SetID( unsigned int id){ uID = id; }
	char*        GetName(){ return szName; };
	void         SetName(const char *name);

	
	
	bool        Save(FILE *pFile);
	bool        Load(FILE *pFile);

	
	void Release();
	bool IsActive(){ return bActive; };
	void ActiveTrigger(){ bActive = true; }
	void ToggleTrigger(){ bActive = false; }
	bool IsRun(){ return bRun; }
	void SetRunStatus(bool br){ bRun = br; }
	bool OnlyAttackValid(){ return bAttackValid; }
	void SetAttackValid( bool bValid ){  bAttackValid = bValid; };
	
	CTriggerData*        CopyObject();
	
	static _s_tree_item* CopyConditonTree(_s_tree_item* pRoot);

	static size_t		 GetConditionParamSize(int condition);
	static void          CopyConditionParam(_s_tree_item* pDst, const _s_tree_item* pSrc);
	
	static _s_operation* CopyOperation(_s_operation *pOperation);
	static void			 FreeOperation(_s_operation *&pOperation);

	static size_t		 GetOperationParamSize(int operation);
	static void			 CopyOperationParam(int operation, void *&pDst, const void *pSrc);
	static void			 CopyOperationParam(_s_operation* pDst, const _s_operation* pSrc);
	static void			 ReadOperationParam(_s_operation *p, FILE *pFile, unsigned int dwVersion);
	static void			 WriteOperationParam(const _s_operation *p, FILE *pFile);
	static void			 FreeOperationParam(_s_operation *pOperation);

	static void			 FreeOperationTarget(_s_target & mTarget);
	static void			 FreeOperationTarget(_s_operation *pOperation);
	static void			 WriteOperationTarget(const _s_operation *pOperation, FILE *pFile);
	static void			 ReadOperationTarget(_s_operation *pOperation, FILE *pFile);
	static void			 CopyOperationTarget(_s_operation* pDst, const _s_operation* pSrc);

protected:
	bool				SaveConditonTree(FILE *pFile, _s_tree_item *pNode);
	bool				ReadConditonTree(FILE *pFile, _s_tree_item *pNode);
	
private:

	char         szName[128];
	bool         bActive;
	bool         bRun;
	bool         bAttackValid;
	unsigned int uID;
	_s_tree_item* rootConditon;
	abase::vector<_s_operation *>	listOperation;

};


//----------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------


class CPolicyData
{

public:

	inline unsigned int GetID(){ return uID; }
	inline void SetID( unsigned int id){ uID = id; }
	
	inline int GetTriggerPtrNum(){ return listTriggerPtr.size(); }
	inline CTriggerData *GetTriggerPtr( int idx ){ return listTriggerPtr[idx]; }
	CTriggerData *		GetTriggerPtrByID(unsigned int uTriggerID);
	unsigned int		GetNextTriggerID();
	int			GetIndex( unsigned int id);//如果查找失败返回-1
	inline void SetTriggerPtr( int idx, CTriggerData *pTrigger){ listTriggerPtr[idx] = pTrigger; }
	inline void AddTriggerPtr( CTriggerData *pTrigger){ listTriggerPtr.push_back(pTrigger); }
	void        DelTriggerPtr( int idx);
	bool Save(const char* szPath);
	bool Load(const char* szPath);
	bool Save(FILE *pFile);
	bool Load(FILE *pFile);

	void Release();

private:

	unsigned int uID;
	abase::vector<CTriggerData*> listTriggerPtr;
};


//----------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------


class CPolicyDataManager
{

public:
	
	inline int GetPolicyNum(){ return listPolicy.size(); }
	inline CPolicyData *GetPolicy( int idx) 
	{
		if( idx >= 0 && idx < (int)listPolicy.size())
			return listPolicy[idx];
		else return 0;
	}
	
	bool Load(const char* szPath);
	bool Save(const char* szPath);
	void Release();

private:
	
	abase::vector<CPolicyData *> listPolicy;
};

#endif
