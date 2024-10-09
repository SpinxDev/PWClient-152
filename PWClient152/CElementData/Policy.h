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

	//0-����, 1-��ʦ, 2-��ʦ, 3-����, 4-����, 5-�̿�, 6-��â, 7-���飬8-���飬9-���飬10-ҹӰ��11-����
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
		c_time_come = 0,		//ָ����ʱ����ʱ 	��������ʱ��ID **ֻ�ܵ�����
		c_hp_less,				//Ѫ�����ڰٷֱ�	������Ѫ������
		c_start_attack,			//ս����ʼ			������������   **ֻ�ܵ�����
		c_random,				//���				�������������
		c_kill_player,			//ɱ����ҡ�������	������������   **ֻ�ܵ�����
		c_not,					//һ�������ķ�
		c_or,					//����������
		c_and,					//����������
		c_died,                 //����ʱ                           **ֻ�ܵ�����
		
		c_plus,					//��
		c_minus,                //��
		c_multiply,             //��
		c_divide,               //��
		c_great,                //����
		c_less,                 //С��
		c_equ,					//����
		c_var,                  //������           ����������ID
		c_constant,             //������           ������int
//		c_time_point,			//ʱ��������	   ������Сʱ������

		c_be_hurt,				//�ܵ��˺���	   ������ �ܵ��˺����ޣ� �ܵ��˺�����			
		c_reach_end,			//����·���յ㣬
		c_at_history_stage,		//������ʷ�׶�
		c_history_value,		//��ʷ����ֵ
		c_stop_fight,			//����ս��			��������
		c_local_var,			//�ֲ�����			�������ֲ�����ID
		c_reach_end_2,			//����·���յ㣨·��ID�����ɼ�ӷ�ʽָ����
		c_has_filter,			//����״̬��        ������״̬��ID
		c_room_index,			//�������ͼ����·���е�˳���Կ������ͼ��Ϊ��λ��

		c_num,
	};

	enum _e_target
	{
		t_hate_first = 0,		//���������һλ
		t_hate_second,			//��������ڶ�λ
		t_hate_others,			//��������ڶ�λ�����Ժ����ж����е����һ��
		t_most_hp,				//���hp
		t_most_mp,				//���mp
		t_least_hp,				//����hp
		t_occupation_list,		//ĳЩְҵ�����	������ְҵ��ϱ�
		t_self,					//�Լ� 
		t_monster_killer,		//�����ɱ�������
		t_monster_birthplace_faction,	//��������ع�������
		t_hate_random_one,		//����б����һ��Ŀ��
		t_hate_nearest,			//����б������Ŀ��
		t_hate_farthest,		//����б�����ԶĿ��
		t_hate_first_redirected,//���������һλ������תΪ���ˣ�
		t_num,
	};

	enum _e_operation
	{
		o_attact = 0,			//���𹥻���	������0 �����ⲫ 1 ������ 2 ħ���� 3 �ⲫ��Զ��
		o_use_skill,			//ʹ�ü��ܡ�	������ָ�����ܺͼ���
		o_talk,					//˵��		    �����������������ݣ�unicode����
		o_reset_hate_list,		//���ó���б�
		o_run_trigger,			//ִ��һ���´�����
		o_stop_trigger,			//ֹͣһ��������	������id
		o_active_trigger,		//����һ��������	������id
		o_create_timer,		    //����һ����ʱ��	������id
		o_kill_timer,			//ֹͣһ����ʱ��	������id
		
		o_flee,                 //����
		o_set_hate_to_first,    //��ѡ��Ŀ�����������б��һλ
		o_set_hate_to_last,     //��ѡ��Ŀ��ĳ�޶ȵ��������
		o_set_hate_fifty_percent,     //��޶��б������ж���ĳ�޶ȼ���50%�����ټ��ٵ�1
		o_skip_operation,       //��������Ĳ�����Ŀ
		o_active_controller,
		o_set_global,           //����ȫ�ֱ���   param ����ID ��ֵ
		o_revise_global,        //����ȫ�ֱ���   param ����ID ��ֵ
		o_summon_monster,
		o_walk_along,			//��·������ ������0 ����id ; 1 ·��id; 2Ѳ�߷�ʽ; 3�ٶ�����
		o_play_action,			//����ָ������ ������0 ������; 1 �Ƿ�ѭ����2ѭ�����
		o_revise_history,		//������ʷ����   param ����ID ��ֵ
		o_set_history,			//������ʷ����	 param ����ID ��ֵ
		o_deliver_faction_pvp_points,	//	���Ű����Ӷ�ս���� ������0 ������Դ�� 1 ������Դ����
		o_calc_var,				//ʹ�ü򵥱��ʽ���������ȫ�ֱ���/�ֲ�������ֵ
		o_summon_monster_2,		//�ٻ�����£�
		o_walk_along_2,			//��·Ѳ�����£�
		o_use_skill_2,			//ʹ�ü��ܣ��£�
		o_active_controller_2,	//����һ��������
		o_deliver_task,			//��������
		o_summon_mine,			//�ٻ���
		o_summon_npc,			//�ٻ�NPC
		o_deliver_random_task_in_region,	//	�����������������
		o_deliver_task_in_hate_list,		//	����б��ڷ�������
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
	int			GetIndex( unsigned int id);//�������ʧ�ܷ���-1
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
