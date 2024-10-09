#ifndef _AI_OPERATION_PARAM_H_
#define _AI_OPERATION_PARAM_H_


#include "AObject.h"
#include "Policy.h"
#include "Global.h"



struct TRIGGER_ID_FUNCS : public CUSTOM_FUNCS
{
	virtual BOOL CALLBACK OnActivate(void);
	virtual LPCTSTR CALLBACK OnGetShowString(void) const;
	virtual AVariant CALLBACK OnGetValue(void) const;
	virtual void CALLBACK OnSetValue(const AVariant& var);

	AVariant	m_var;
	AString		m_strText;
	void*       m_pParent;
};

class COperationParam
{

public:
	COperationParam(){ m_pPolicyData = 0; mTarget.iType = 0; mTarget.pParam = 0; m_pProperty = new ADynPropertyObject;}
	virtual ~COperationParam()
	{
		if(m_pProperty) delete m_pProperty; 
	}
	ADynPropertyObject* m_pProperty;
	
	virtual void UpdateProperty(bool bGet)=0;
	unsigned int GetOperationType()const{ return uOperationType; }
	unsigned int GetOperationParamSize()const{
		return CTriggerData::GetOperationParamSize(GetOperationType());
	}

	virtual void * GetOperationParam(){ return NULL; }
	virtual void CopyTo(void *&p){
		CTriggerData::CopyOperationParam(GetOperationType(), p, GetOperationParam());
	}
	virtual void CopyFrom(const CTriggerData::_s_operation *p){
		if (GetOperationParamSize() > 0){
			void * pTemp = 0;
			CTriggerData::CopyOperationParam(GetOperationType(), pTemp, p->pParam);
			memcpy(GetOperationParam(), pTemp, GetOperationParamSize());
			free(pTemp);
		}
	}

	static COperationParam * Create(int operation);
	
	CTriggerData::_s_target mTarget;
	CPolicyData *m_pPolicyData;
	virtual void BuildProperty()=0;
protected:
	unsigned int uOperationType;
	
};

//----------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------

class COperationParam_Attack : public COperationParam
{

public:
	COperationParam_Attack(){ uOperationType = CTriggerData::o_attact; m_Data.uType = 0;}

public:
	enum _e_attack_type
	{
		a_close_battle = 0,		//物理肉搏
		a_skill,				//物理弓箭
		a_long_distance,        //魔法
		a_skill_and_battle,     //肉搏＋远程
		a_num,
	};
	O_ATTACK_TYPE m_Data;
	virtual void * GetOperationParam(){ return &m_Data; }

protected:
	ASetTemplate<int> attack_type;
	virtual void BuildProperty()
	{
		attack_type.AddElement("物理肉搏", a_close_battle);
		attack_type.AddElement("物理弓箭", a_skill);
		attack_type.AddElement("魔法", a_long_distance);
		attack_type.AddElement("肉搏＋远程", a_skill_and_battle);
				
		m_pProperty->DynAddProperty(AVariant(m_Data.uType), "攻击类型", &attack_type);
	}
	virtual void UpdateProperty(bool bGet)
	{
		if(bGet)
		{
			m_Data.uType = m_pProperty->GetPropVal(0);	
		}else
		{
			m_pProperty->SetPropVal(0,m_Data.uType);
		}
	};
};



//----------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------

class COperationParam_Use_Skill : public COperationParam
{
public:
	COperationParam_Use_Skill(){ uOperationType = CTriggerData::o_use_skill; m_Data.uSkill = 0; m_Data.uLevel = 0;}

public:
	O_USE_SKILL m_Data;
	virtual void * GetOperationParam(){ return &m_Data; }

protected:
	
	virtual void BuildProperty()
	{
		m_pProperty->DynAddProperty(AVariant(m_Data.uSkill), "技能");
		m_pProperty->DynAddProperty(AVariant(m_Data.uLevel), "级别");
	}
	virtual void UpdateProperty(bool bGet)
	{
		if(bGet)
		{
			m_Data.uSkill = m_pProperty->GetPropVal(0);
			m_Data.uLevel = m_pProperty->GetPropVal(1);	
		}else
		{
			m_pProperty->SetPropVal(0,m_Data.uSkill);
			m_pProperty->SetPropVal(1,m_Data.uLevel);
		}
	};
};


//----------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------
struct EquipMask;
class COperationParam_Talk : public COperationParam
{
public:
	COperationParam_Talk();
	virtual ~COperationParam_Talk();
	virtual void CopyFrom(const CTriggerData::_s_operation *p){
		const O_TALK_TEXT *q = (const O_TALK_TEXT *)p->pParam;
		wcscpy(m_Data.szData, q->szData);
		m_Data.uAppendDataMask = q->uAppendDataMask;
	}
	virtual void CopyTo(void *&p){
		p = malloc(GetOperationParamSize());
		O_TALK_TEXT *q = (O_TALK_TEXT *)p;
		q->uSize = wcslen(m_Data.szData)*2 + 2;
		q->szData = new policy_char[q->uSize/2];
		wcscpy(q->szData,m_Data.szData);
		q->uAppendDataMask = m_Data.uAppendDataMask;
	}

public:
	EquipMask * m_fs;
	O_TALK_TEXT m_Data;
	virtual void * GetOperationParam(){ return &m_Data; }

protected:
	virtual void BuildProperty();
	virtual void UpdateProperty(bool bGet);
};


//----------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------

class COperationParam_Run_Trigger : public COperationParam
{

public:
	COperationParam_Run_Trigger(){ uOperationType = CTriggerData::o_run_trigger;m_Data.uID = 0;}
	virtual ~COperationParam_Run_Trigger(){ if(m_pTriggerFunc) delete m_pTriggerFunc;}

public:
	O_RUN_TRIGGER m_Data;
	virtual void * GetOperationParam(){ return &m_Data; }
	
protected:
	TRIGGER_ID_FUNCS *m_pTriggerFunc;
	virtual void BuildProperty()
	{
		AString temp;
		m_pTriggerFunc = new TRIGGER_ID_FUNCS;
		m_pTriggerFunc->m_pParent = this;
		m_pProperty->DynAddProperty(AVariant(temp), "触发器", (ASet*)m_pTriggerFunc, NULL, WAY_CUSTOM);
		UpdateProperty(false);
	}
	virtual void UpdateProperty(bool bGet)
	{
		if(bGet)
		{
		}else
		{
			m_pTriggerFunc->OnSetValue(AVariant());
		}
	};
};


//----------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------

class COperationParam_Stop_Trigger : public COperationParam
{

public:
	COperationParam_Stop_Trigger(){ uOperationType = CTriggerData::o_stop_trigger;m_Data.uID = 0;}
	
public:
	O_STOP_TRIGGER m_Data;
	virtual void * GetOperationParam(){ return &m_Data; }

protected:
	ASetTemplate<int> trigger_list;
	virtual void BuildProperty()
	{
		if(m_pPolicyData)
		{
			int n = m_pPolicyData->GetTriggerPtrNum();
			for( int i = 0; i < n; ++i)
			{
				CTriggerData *pData = m_pPolicyData->GetTriggerPtr(i);
				if(!pData->IsRun()) trigger_list.AddElement(pData->GetName(),pData->GetID());
			}
		}
		m_pProperty->DynAddProperty(AVariant(m_Data.uID), "触发器",&trigger_list);
	}
	virtual void UpdateProperty(bool bGet)
	{
		if(bGet)
		{
			m_Data.uID = m_pProperty->GetPropVal(0);
		}else
		{
			m_pProperty->SetPropVal(0,m_Data.uID);
		}
	};
};


//----------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------

class COperationParam_Active_Trigger : public COperationParam
{

public:
	COperationParam_Active_Trigger(){ uOperationType = CTriggerData::o_active_trigger;m_Data.uID = 0;}

public:
	O_ACTIVE_TRIGGER m_Data;
	virtual void * GetOperationParam(){ return &m_Data; }

protected:
	ASetTemplate<int> trigger_list;
	virtual void BuildProperty()
	{
		if(m_pPolicyData)
		{
			int n = m_pPolicyData->GetTriggerPtrNum();
			for( int i = 0; i < n; ++i)
			{
				CTriggerData *pData = m_pPolicyData->GetTriggerPtr(i);
				if(!pData->IsRun()) trigger_list.AddElement(pData->GetName(),pData->GetID());
			}
		}
		m_pProperty->DynAddProperty(AVariant(m_Data.uID), "触发器",&trigger_list);
	}
	virtual void UpdateProperty(bool bGet)
	{
		if(bGet)
		{
			m_Data.uID = m_pProperty->GetPropVal(0);
		}else
		{
			m_pProperty->SetPropVal(0,m_Data.uID);
		}
	};
};

//----------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------

class COperationParam_Create_Timer : public COperationParam
{

public:
	COperationParam_Create_Timer(){ uOperationType = CTriggerData::o_create_timer;m_Data.uID = 0;m_Data.uCounter = 1;m_Data.uPeriod = 10;}

public:
	O_CREATE_TIMER m_Data;
	virtual void * GetOperationParam(){ return &m_Data; }

protected:
	virtual void BuildProperty()
	{
		m_pProperty->DynAddProperty(AVariant(m_Data.uID), "定时器ID");
		m_pProperty->DynAddProperty(AVariant(m_Data.uPeriod), "时间间隔");
		m_pProperty->DynAddProperty(AVariant(m_Data.uCounter), "次数");
	}
	virtual void UpdateProperty(bool bGet)
	{
		if(bGet)
		{
			m_Data.uID = m_pProperty->GetPropVal(0);
			m_Data.uPeriod = m_pProperty->GetPropVal(1);
			m_Data.uCounter = m_pProperty->GetPropVal(2);
		}else
		{
			m_pProperty->SetPropVal(0,m_Data.uID);
			m_pProperty->SetPropVal(1,m_Data.uPeriod);
			m_pProperty->SetPropVal(2,m_Data.uCounter);
		}
	};
};

//----------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------

class COperationParam_Kill_Timer : public COperationParam
{

public:
	COperationParam_Kill_Timer(){ uOperationType = CTriggerData::o_kill_timer;m_Data.uID = 0;}

public:
	O_KILL_TIMER m_Data;
	virtual void * GetOperationParam(){ return &m_Data; }

protected:
	virtual void BuildProperty()
	{
		m_pProperty->DynAddProperty(AVariant(m_Data.uID), "定时器ID");
	}
	virtual void UpdateProperty(bool bGet)
	{
		if(bGet)
		{
			m_Data.uID = m_pProperty->GetPropVal(0);
		}else
		{
			m_pProperty->SetPropVal(0,m_Data.uID);
		}
	};
};

//----------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------

class COperationParam_Reset_hates : public COperationParam
{

public:
	COperationParam_Reset_hates(){ uOperationType = CTriggerData::o_reset_hate_list;}

public:
	

protected:
	virtual void BuildProperty()
	{
	}
	virtual void UpdateProperty(bool bGet)
	{
	};
};

//----------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------

class COperationParam_Flee : public COperationParam
{

public:
	COperationParam_Flee(){ uOperationType = CTriggerData::o_flee;}

public:
	

protected:
	virtual void BuildProperty()
	{
	}
	virtual void UpdateProperty(bool bGet)
	{
	};
};

//----------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------

class COperationParam_SetHateToFirst : public COperationParam
{

public:
	COperationParam_SetHateToFirst(){ uOperationType = CTriggerData::o_set_hate_to_first;}

public:
	

protected:
	virtual void BuildProperty()
	{
	}
	virtual void UpdateProperty(bool bGet)
	{
	};
};


//----------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------

class COperationParam_SetHateToLast : public COperationParam
{

public:
	COperationParam_SetHateToLast(){ uOperationType = CTriggerData::o_set_hate_to_last;}

public:
	

protected:
	virtual void BuildProperty()
	{
	}
	virtual void UpdateProperty(bool bGet)
	{
	};
};

//----------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------

class COperationParam_SetHateFiftyPercent : public COperationParam
{

public:
	COperationParam_SetHateFiftyPercent(){ uOperationType = CTriggerData::o_set_hate_fifty_percent;}

public:
	

protected:
	virtual void BuildProperty()
	{
	}
	virtual void UpdateProperty(bool bGet)
	{
	};
};

//----------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------

class COperationParam_SkipOperation : public COperationParam
{

public:
	COperationParam_SkipOperation(){ uOperationType = CTriggerData::o_skip_operation;}

public:
	

protected:
	virtual void BuildProperty()
	{
	}
	virtual void UpdateProperty(bool bGet)
	{
	};
};

//----------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------

class COperationParam_Active_Controller : public COperationParam
{

public:
	COperationParam_Active_Controller(){ uOperationType = CTriggerData::o_active_controller;m_Data.uID = 0;m_Data.bStop = false; }

public:
	O_ACTIVE_CONTROLLER m_Data;
	virtual void * GetOperationParam(){ return &m_Data; }

protected:
	virtual void BuildProperty()
	{
		m_pProperty->DynAddProperty(AVariant(m_Data.uID), "产生怪物控制器ID");
		m_pProperty->DynAddProperty(AVariant(m_Data.bStop), "停止激活");
	}
	virtual void UpdateProperty(bool bGet)
	{
		if(bGet)
		{
			m_Data.uID = m_pProperty->GetPropVal(0);
			m_Data.bStop = m_pProperty->GetPropVal(1);
		}else
		{
			m_pProperty->SetPropVal(0,m_Data.uID);
			m_pProperty->SetPropVal(1,m_Data.bStop);
		}
	};
};

//----------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------

class COperationParam_Set_Global : public COperationParam
{

public:
	COperationParam_Set_Global(){ uOperationType = CTriggerData::o_set_global;m_Data.iID = 0; m_Data.iValue = 0; }

public:
	O_SET_GLOBAL m_Data;
	virtual void * GetOperationParam(){ return &m_Data; }

protected:
	virtual void BuildProperty()
	{
		m_pProperty->DynAddProperty(AVariant(m_Data.iID), "变量ID");
		m_pProperty->DynAddProperty(AVariant(m_Data.iValue), "数值");
		m_pProperty->DynAddProperty(AVariant(m_Data.bIsValue),"是否为数值");
	}
	virtual void UpdateProperty(bool bGet)
	{
		if(bGet)
		{
			m_Data.iID = m_pProperty->GetPropVal(0);
			m_Data.iValue = m_pProperty->GetPropVal(1);
			m_Data.bIsValue = m_pProperty->GetPropVal(2);
		
		}else
		{
			m_pProperty->SetPropVal(0,m_Data.iID);
			m_pProperty->SetPropVal(1,m_Data.iValue);
			m_pProperty->SetPropVal(2,m_Data.bIsValue);
		}
	};
};

//----------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------

class COperationParam_Revise_Global : public COperationParam
{

public:
	COperationParam_Revise_Global(){ uOperationType = CTriggerData::o_revise_global;m_Data.iID = 0; m_Data.iValue = 0; }

public:
	O_REVISE_GLOBAL m_Data;
	virtual void * GetOperationParam(){ return &m_Data; }

protected:
	virtual void BuildProperty()
	{
		m_pProperty->DynAddProperty(AVariant(m_Data.iID), "变量ID");
		m_pProperty->DynAddProperty(AVariant(m_Data.iValue), "数值");
	}
	virtual void UpdateProperty(bool bGet)
	{
		if(bGet)
		{
			m_Data.iID = m_pProperty->GetPropVal(0);
			m_Data.iValue = m_pProperty->GetPropVal(1);
		
		}else
		{
			m_pProperty->SetPropVal(0,m_Data.iID);
			m_pProperty->SetPropVal(1,m_Data.iValue);
		}
	};
};

//----------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------

class COperationParam_Summon_Monster : public COperationParam
{

public:
	COperationParam_Summon_Monster(){ 
		uOperationType = CTriggerData::o_summon_monster; 
		m_Data.iDispearCondition = enumPSDTNever; 
		m_Data.iLife = -1;
		m_Data.iMonsterID = 0;
		m_Data.iRange = 2;
		m_Data.iPathID = 0;
		m_Data.iMonsterNum = 1;
	}

public:
	ASetTemplate<int> condition_type;
	O_SUMMON_MONSTER m_Data;
	virtual void * GetOperationParam(){ return &m_Data; }

protected:
	virtual void BuildProperty();
	virtual void UpdateProperty(bool bGet)
	{
		if(bGet)
		{
			
			m_Data.iDispearCondition = m_pProperty->GetPropVal(0);
			m_Data.iMonsterID = m_pProperty->GetPropVal(1);
			m_Data.iRange = m_pProperty->GetPropVal(2);
			m_Data.iLife = m_pProperty->GetPropVal(3);
			m_Data.iPathID = m_pProperty->GetPropVal(4);
			m_Data.iMonsterNum = m_pProperty->GetPropVal(5);
		
		}else
		{
			
			m_pProperty->SetPropVal(0,m_Data.iDispearCondition);
			m_pProperty->SetPropVal(1,m_Data.iMonsterID);
			m_pProperty->SetPropVal(2,m_Data.iRange);
			m_pProperty->SetPropVal(3,m_Data.iLife);
			m_pProperty->SetPropVal(4,m_Data.iPathID);
			m_pProperty->SetPropVal(5,m_Data.iMonsterNum);
		}
	};
};

//----------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------

class COperationParam_Walk_Along : public COperationParam
{
	
public:
	COperationParam_Walk_Along(){ 
		uOperationType = CTriggerData::o_walk_along; 
		m_Data.iWorldID = 0; 
		m_Data.iPathID = 0;
		m_Data.iPatrolType = enumPMPTStopAtEnd;
		m_Data.iSpeedType = enumPMPSTSlow;
	}
	
public:
	ASetTemplate<int> patrol_type;
	ASetTemplate<int> speed_type;
	O_WALK_ALONG m_Data;
	virtual void * GetOperationParam(){ return &m_Data; }
	
protected:
	virtual void BuildProperty();
	virtual void UpdateProperty(bool bGet)
	{
		if(bGet)
		{
			
			m_Data.iWorldID = m_pProperty->GetPropVal(0);
			m_Data.iPathID = m_pProperty->GetPropVal(1);
			m_Data.iPatrolType = m_pProperty->GetPropVal(2);
			m_Data.iSpeedType = m_pProperty->GetPropVal(3);
			
			
		}else
		{
			
			m_pProperty->SetPropVal(0,m_Data.iWorldID);
			m_pProperty->SetPropVal(1,m_Data.iPathID);
			m_pProperty->SetPropVal(2,m_Data.iPatrolType);
			m_pProperty->SetPropVal(3,m_Data.iSpeedType);
		}
	};
};

//----------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------

class COperationParam_Play_Action : public COperationParam
{
	
public:
	COperationParam_Play_Action(){ 
		uOperationType = CTriggerData::o_play_action; 
		memset(m_Data.szActionName,0,128); 
		m_Data.iLoopCount = 1;
		m_Data.iInterval = 0;
		m_Data.iPlayTime = 5000;
	
	}
	
public:
	O_PLAY_ACTION m_Data;
	virtual void * GetOperationParam(){ return &m_Data; }
	
protected:
	virtual void BuildProperty()
	{
		m_pProperty->DynAddProperty(AVariant(AString(m_Data.szActionName)), "动作名");
		m_pProperty->DynAddProperty(AVariant(m_Data.iLoopCount), "循环次数");
		m_pProperty->DynAddProperty(AVariant(m_Data.iInterval), "循环播放间隔");
		m_pProperty->DynAddProperty(AVariant(m_Data.iPlayTime), "动作播放时间");
	}
	virtual void UpdateProperty(bool bGet)
	{
		if(bGet)
		{
			
			AString name = m_pProperty->GetPropVal(0);
			if(name.GetLength() <= 127)
			{
				strcpy(m_Data.szActionName,name);
			}else AfxMessageBox("动作名太长了，应该是小于128个字符!");
			m_Data.iLoopCount = m_pProperty->GetPropVal(1);
			m_Data.iInterval = m_pProperty->GetPropVal(2);
			m_Data.iPlayTime = m_pProperty->GetPropVal(3);
		}else
		{
			m_pProperty->SetPropVal(0,AString(m_Data.szActionName));
			m_pProperty->SetPropVal(1,m_Data.iLoopCount);
			m_pProperty->SetPropVal(2,m_Data.iInterval);
			m_pProperty->SetPropVal(3,m_Data.iPlayTime);
		}
	};
};

//----------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------

class COperationParam_Revise_History : public COperationParam
{
	
public:
	COperationParam_Revise_History(){ uOperationType = CTriggerData::o_revise_history;m_Data.iID = 0; m_Data.iValue = 0; }
	
public:
	O_REVISE_HISTORY m_Data;
	virtual void * GetOperationParam(){ return &m_Data; }
	
protected:
	virtual void BuildProperty()
	{
		m_pProperty->DynAddProperty(AVariant(m_Data.iID), "变量ID");
		m_pProperty->DynAddProperty(AVariant(m_Data.iValue), "数值");
	}
	virtual void UpdateProperty(bool bGet)
	{
		if(bGet)
		{
			m_Data.iID = m_pProperty->GetPropVal(0);
			m_Data.iValue = m_pProperty->GetPropVal(1);
			
		}else
		{
			m_pProperty->SetPropVal(0,m_Data.iID);
			m_pProperty->SetPropVal(1,m_Data.iValue);
		}
	};
};



//----------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------

class COperationParam_Set_History : public COperationParam
{
	
public:
	COperationParam_Set_History(){ uOperationType = CTriggerData::o_set_history;m_Data.iID = 0; m_Data.iValue = 0; }
	
public:
	O_SET_HISTORY m_Data;
	virtual void * GetOperationParam(){ return &m_Data; }
	
protected:
	virtual void BuildProperty()
	{
		m_pProperty->DynAddProperty(AVariant(m_Data.iID), "变量ID");
		m_pProperty->DynAddProperty(AVariant(m_Data.iValue), "数值");
		m_pProperty->DynAddProperty(AVariant(m_Data.bIsHistoryValue),"是否为数值");
	}
	virtual void UpdateProperty(bool bGet)
	{
		if(bGet)
		{
			m_Data.iID = m_pProperty->GetPropVal(0);
			m_Data.iValue = m_pProperty->GetPropVal(1);
			m_Data.bIsHistoryValue = m_pProperty->GetPropVal(2);
			
		}else
		{
			m_pProperty->SetPropVal(0,m_Data.iID);
			m_pProperty->SetPropVal(1,m_Data.iValue);
			m_pProperty->SetPropVal(2,m_Data.bIsHistoryValue);
		}
	};
};



//----------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------

class COperationParam_DeliverFactionPVPPoints : public COperationParam
{
	
public:
	COperationParam_DeliverFactionPVPPoints(){ uOperationType = CTriggerData::o_deliver_faction_pvp_points; m_Data.uType = 0;}
	
public:
	O_DELIVER_FACTION_PVP_POINTS m_Data;
	virtual void * GetOperationParam(){ return &m_Data; }
protected:
	ASetTemplate<int> point_type;
	virtual void BuildProperty()
	{
		point_type.AddElement("帮派资源车", enumPFPPTMineCar);
		point_type.AddElement("帮派资源基地", enumPFPPTMineBase);
		point_type.AddElement("帮派资源车到达目的地", enumPFPPTMineCarArrived);
		
		m_pProperty->DynAddProperty(AVariant(m_Data.uType), "积分类型", &point_type);
	}
	virtual void UpdateProperty(bool bGet)
	{
		if(bGet)
		{
			m_Data.uType = m_pProperty->GetPropVal(0);	
		}else
		{
			m_pProperty->SetPropVal(0,m_Data.uType);
		}
	};
};


//----------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------

class COperationParam_Calc_Var : public COperationParam
{
	
public:
	COperationParam_Calc_Var(){
		uOperationType = CTriggerData::o_calc_var;
		m_Data.iDst = 0;
		m_Data.iDstType = enumPVTGlobalVarID;
		m_Data.iSrc1 = 0;
		m_Data.iSrc1Type = enumPVTConst;
		m_Data.iOp = enumPOTAdd;
		m_Data.iSrc2 = 0;
		m_Data.iSrc2Type = enumPVTConst;
	}
	
public:
	O_CALC_VAR m_Data;
	virtual void * GetOperationParam(){ return &m_Data; }
	
	ASetTemplate<int> dst_type;
	ASetTemplate<int> src1_type;
	ASetTemplate<int> op_type;
	ASetTemplate<int> src2_type;
	
protected:
	virtual void BuildProperty();
	virtual void UpdateProperty(bool bGet)
	{
		if(bGet)
		{
			m_Data.iDst = m_pProperty->GetPropVal(0);
			m_Data.iDstType = m_pProperty->GetPropVal(1);
			m_Data.iSrc1 = m_pProperty->GetPropVal(2);
			m_Data.iSrc1Type = m_pProperty->GetPropVal(3);
			m_Data.iOp = m_pProperty->GetPropVal(4);
			m_Data.iSrc2 = m_pProperty->GetPropVal(5);
			m_Data.iSrc2Type = m_pProperty->GetPropVal(6);
			
		}else
		{
			m_pProperty->SetPropVal(0,m_Data.iDst);
			m_pProperty->SetPropVal(1,m_Data.iDstType);
			m_pProperty->SetPropVal(2,m_Data.iSrc1);
			m_pProperty->SetPropVal(3,m_Data.iSrc1Type);
			m_pProperty->SetPropVal(4,m_Data.iOp);
			m_pProperty->SetPropVal(5,m_Data.iSrc2);
			m_pProperty->SetPropVal(6,m_Data.iSrc2Type);
		}
	};
};

//----------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------

class COperationParam_Summon_Monster_2 : public COperationParam
{
	
public:
	COperationParam_Summon_Monster_2(){ 
		uOperationType = CTriggerData::o_summon_monster_2; 
		m_Data.iDispearCondition = enumPSDTNever; 
		m_Data.iLife = -1;
		m_Data.iMonsterID = 0;
		m_Data.iMonsterIDType = enumPVTConst;
		m_Data.iRange = 2;
		m_Data.iPathID = 0;
		m_Data.iPathIDType = enumPVTConst;
		m_Data.iMonsterNum = 1;
		m_Data.iMonsterNumType = enumPVTConst;
	}
	
public:
	ASetTemplate<int> condition_type;
	ASetTemplate<int> monster_id_type;
	ASetTemplate<int> path_id_type;
	ASetTemplate<int> monster_num_type;
	O_SUMMON_MONSTER_2 m_Data;
	virtual void * GetOperationParam(){ return &m_Data; }
	
protected:
	virtual void BuildProperty();
	virtual void UpdateProperty(bool bGet)
	{
		if(bGet){			
			m_Data.iDispearCondition = m_pProperty->GetPropVal(0);
			m_Data.iMonsterID = m_pProperty->GetPropVal(1);
			m_Data.iMonsterIDType = m_pProperty->GetPropVal(2);
			m_Data.iRange = m_pProperty->GetPropVal(3);
			m_Data.iLife = m_pProperty->GetPropVal(4);
			m_Data.iPathID = m_pProperty->GetPropVal(5);
			m_Data.iPathIDType = m_pProperty->GetPropVal(6);
			m_Data.iMonsterNum = m_pProperty->GetPropVal(7);
			m_Data.iMonsterNumType = m_pProperty->GetPropVal(8);			
		}else{			
			m_pProperty->SetPropVal(0,m_Data.iDispearCondition);
			m_pProperty->SetPropVal(1,m_Data.iMonsterID);
			m_pProperty->SetPropVal(2,m_Data.iMonsterIDType);
			m_pProperty->SetPropVal(3,m_Data.iRange);
			m_pProperty->SetPropVal(4,m_Data.iLife);
			m_pProperty->SetPropVal(5,m_Data.iPathID);
			m_pProperty->SetPropVal(6,m_Data.iPathIDType);
			m_pProperty->SetPropVal(7,m_Data.iMonsterNum);
			m_pProperty->SetPropVal(8,m_Data.iMonsterNumType);
		}
	};
};

//----------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------

class COperationParam_Walk_Along_2 : public COperationParam
{
	
public:
	COperationParam_Walk_Along_2(){ 
		uOperationType = CTriggerData::o_walk_along_2; 
		m_Data.iWorldID = 0; 
		m_Data.iPathID = 0;
		m_Data.iPathIDType = enumPVTConst;
		m_Data.iPatrolType = enumPMPTStopAtEnd;
		m_Data.iSpeedType = enumPMPSTSlow;
	}
	
public:
	ASetTemplate<int> path_id_type;
	ASetTemplate<int> patrol_type;
	ASetTemplate<int> speed_type;
	O_WALK_ALONG_2 m_Data;
	virtual void * GetOperationParam(){ return &m_Data; }
	
protected:
	virtual void BuildProperty();
	virtual void UpdateProperty(bool bGet)
	{
		if(bGet)
		{
			
			m_Data.iWorldID = m_pProperty->GetPropVal(0);
			m_Data.iPathID = m_pProperty->GetPropVal(1);
			m_Data.iPathIDType = m_pProperty->GetPropVal(2);
			m_Data.iPatrolType = m_pProperty->GetPropVal(3);
			m_Data.iSpeedType = m_pProperty->GetPropVal(4);
			
			
		}else
		{
			
			m_pProperty->SetPropVal(0,m_Data.iWorldID);
			m_pProperty->SetPropVal(1,m_Data.iPathID);
			m_pProperty->SetPropVal(2,m_Data.iPathIDType);
			m_pProperty->SetPropVal(3,m_Data.iPatrolType);
			m_pProperty->SetPropVal(4,m_Data.iSpeedType);
		}
	};
};

//----------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------

class COperationParam_Use_Skill_2 : public COperationParam
{
public:
	COperationParam_Use_Skill_2(){
		uOperationType = CTriggerData::o_use_skill_2;
		m_Data.uSkill = 0;
		m_Data.uSkillType = enumPVTConst;
		m_Data.uLevel = 0;
		m_Data.uLevelType = enumPVTConst;
	}
	
public:
	ASetTemplate<int> skill_type;
	ASetTemplate<int> level_type;
	O_USE_SKILL_2 m_Data;
	virtual void * GetOperationParam(){ return &m_Data; }
	
protected:
	
	virtual void BuildProperty();
	virtual void UpdateProperty(bool bGet)
	{
		if(bGet)
		{
			m_Data.uSkill = m_pProperty->GetPropVal(0);
			m_Data.uSkillType = m_pProperty->GetPropVal(1);
			m_Data.uLevel = m_pProperty->GetPropVal(2);	
			m_Data.uLevelType = m_pProperty->GetPropVal(3);	
		}else
		{
			m_pProperty->SetPropVal(0,m_Data.uSkill);
			m_pProperty->SetPropVal(1,m_Data.uSkillType);
			m_pProperty->SetPropVal(2,m_Data.uLevel);
			m_pProperty->SetPropVal(3,m_Data.uLevelType);
		}
	};
};

//----------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------

class COperationParam_Active_Controller_2 : public COperationParam
{
	
public:
	COperationParam_Active_Controller_2(){
		uOperationType = CTriggerData::o_active_controller_2;
		m_Data.uID = 0;
		m_Data.uIDType = enumPVTConst;
		m_Data.bStop = false;
	}
	
public:
	ASetTemplate<int> id_type;
	O_ACTIVE_CONTROLLER_2 m_Data;
	virtual void * GetOperationParam(){ return &m_Data; }
	
protected:
	virtual void BuildProperty();
	virtual void UpdateProperty(bool bGet)
	{
		if(bGet)
		{
			m_Data.uID = m_pProperty->GetPropVal(0);
			m_Data.uIDType = m_pProperty->GetPropVal(1);
			m_Data.bStop = m_pProperty->GetPropVal(2);
		}else
		{
			m_pProperty->SetPropVal(0,m_Data.uID);
			m_pProperty->SetPropVal(1,m_Data.uIDType);
			m_pProperty->SetPropVal(2,m_Data.bStop);
		}
	};
};

//----------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------

class COperationParam_Deliver_Task : public COperationParam
{
	
public:
	COperationParam_Deliver_Task(){
		uOperationType = CTriggerData::o_deliver_task;
		m_Data.uID = 0;
		m_Data.uIDType = enumPVTConst;
	}
	
public:
	ASetTemplate<int> id_type;
	O_DELIVER_TASK m_Data;
	virtual void * GetOperationParam(){ return &m_Data; }
	
protected:
	virtual void BuildProperty();
	virtual void UpdateProperty(bool bGet){
		if(bGet){
			m_Data.uID = m_pProperty->GetPropVal(0);
			m_Data.uIDType = m_pProperty->GetPropVal(1);
		}else{
			m_pProperty->SetPropVal(0,m_Data.uID);
			m_pProperty->SetPropVal(1,m_Data.uIDType);
		}
	};
};
//----------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------

class COperationParam_Summon_Mine : public COperationParam
{
	
public:
	COperationParam_Summon_Mine(){ 
		uOperationType = CTriggerData::o_summon_mine; 
		m_Data.iMineID = 0;
		m_Data.iMineIDType = enumPVTConst;
		m_Data.iRange = 2;
		m_Data.iLife = -1;
		m_Data.iLifeType = enumPVTConst; 
		m_Data.iMineNum = 1;
		m_Data.iMineNumType = enumPVTConst;
	}
	
public:
	ASetTemplate<int> life_type;
	ASetTemplate<int> mine_id_type;
	ASetTemplate<int> mine_num_type;
	O_SUMMON_MINE m_Data;
	virtual void * GetOperationParam(){ return &m_Data; }
	
protected:
	virtual void BuildProperty();
	virtual void UpdateProperty(bool bGet)
	{
		int index(0);
		if(bGet){
			m_Data.iMineID = m_pProperty->GetPropVal(index++);
			m_Data.iMineIDType = m_pProperty->GetPropVal(index++);
			m_Data.iRange = m_pProperty->GetPropVal(index++);
			m_Data.iLife = m_pProperty->GetPropVal(index++);
			m_Data.iLifeType = m_pProperty->GetPropVal(index++);
			m_Data.iMineNum = m_pProperty->GetPropVal(index++);
			m_Data.iMineNumType = m_pProperty->GetPropVal(index++);			
		}else{			
			m_pProperty->SetPropVal(index++,m_Data.iMineID);
			m_pProperty->SetPropVal(index++,m_Data.iMineIDType);
			m_pProperty->SetPropVal(index++,m_Data.iRange);
			m_pProperty->SetPropVal(index++,m_Data.iLife);
			m_pProperty->SetPropVal(index++,m_Data.iLifeType);
			m_pProperty->SetPropVal(index++,m_Data.iMineNum);
			m_pProperty->SetPropVal(index++,m_Data.iMineNumType);
		}
	};
};

//----------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------

class COperationParam_Summon_NPC : public COperationParam
{
	
public:
	COperationParam_Summon_NPC(){ 
		uOperationType = CTriggerData::o_summon_npc; 
		m_Data.iNPCID = 0;
		m_Data.iNPCIDType = enumPVTConst;
		m_Data.iRange = 2;
		m_Data.iLife = -1;
		m_Data.iLifeType = enumPVTConst; 
		m_Data.iPathID = 0;
		m_Data.iPathIDType = enumPVTConst;
		m_Data.iNPCNum = 1;
		m_Data.iNPCNumType = enumPVTConst;
	}
	
public:
	ASetTemplate<int> life_type;
	ASetTemplate<int> npc_id_type;
	ASetTemplate<int> path_id_type;
	ASetTemplate<int> npc_num_type;
	O_SUMMON_NPC m_Data;
	virtual void * GetOperationParam(){ return &m_Data; }
	
protected:
	virtual void BuildProperty();
	virtual void UpdateProperty(bool bGet)
	{
		int index(0);
		if(bGet){			
			m_Data.iNPCID = m_pProperty->GetPropVal(index++);
			m_Data.iNPCIDType = m_pProperty->GetPropVal(index++);
			m_Data.iRange = m_pProperty->GetPropVal(index++);
			m_Data.iLife = m_pProperty->GetPropVal(index++);
			m_Data.iLifeType = m_pProperty->GetPropVal(index++);
			m_Data.iPathID = m_pProperty->GetPropVal(index++);
			m_Data.iPathIDType = m_pProperty->GetPropVal(index++);
			m_Data.iNPCNum = m_pProperty->GetPropVal(index++);
			m_Data.iNPCNumType = m_pProperty->GetPropVal(index++);			
		}else{			
			m_pProperty->SetPropVal(index++,m_Data.iNPCID);
			m_pProperty->SetPropVal(index++,m_Data.iNPCIDType);
			m_pProperty->SetPropVal(index++,m_Data.iRange);
			m_pProperty->SetPropVal(index++,m_Data.iLife);
			m_pProperty->SetPropVal(index++,m_Data.iLifeType);
			m_pProperty->SetPropVal(index++,m_Data.iPathID);
			m_pProperty->SetPropVal(index++,m_Data.iPathIDType);
			m_pProperty->SetPropVal(index++,m_Data.iNPCNum);
			m_pProperty->SetPropVal(index++,m_Data.iNPCNumType);
		}
	};
};

//----------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------

class COperationParam_Deliver_Random_Task_In_Region : public COperationParam
{	
public:
	COperationParam_Deliver_Random_Task_In_Region(){
		uOperationType = CTriggerData::o_deliver_random_task_in_region;
		m_Data.uID = 0;
		m_Data.zvMin.clear();
		m_Data.zvMax.clear();
	}
	
public:
	ASetTemplate<int> id_type;
	O_DELIVER_RANDOM_TASK_IN_REGION m_Data;
	virtual void * GetOperationParam(){ return &m_Data; }
	
protected:
	virtual void BuildProperty();
	virtual void UpdateProperty(bool bGet){
		int index(0);
		if(bGet){
			m_Data.uID = m_pProperty->GetPropVal(index++);
			m_Data.zvMin.x = m_pProperty->GetPropVal(index++);
			m_Data.zvMin.y = m_pProperty->GetPropVal(index++);
			m_Data.zvMin.z = m_pProperty->GetPropVal(index++);
			m_Data.zvMax.x = m_pProperty->GetPropVal(index++);
			m_Data.zvMax.y = m_pProperty->GetPropVal(index++);
			m_Data.zvMax.z = m_pProperty->GetPropVal(index++);
		}else{
			m_pProperty->SetPropVal(index++,m_Data.uID);
			m_pProperty->SetPropVal(index++,m_Data.zvMin.x);
			m_pProperty->SetPropVal(index++,m_Data.zvMin.y);
			m_pProperty->SetPropVal(index++,m_Data.zvMin.z);
			m_pProperty->SetPropVal(index++,m_Data.zvMax.x);
			m_pProperty->SetPropVal(index++,m_Data.zvMax.y);
			m_pProperty->SetPropVal(index++,m_Data.zvMax.z);
		}
	};
};

//----------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------

class COperationParam_Deliver_Task_In_Hate_List : public COperationParam
{
	
public:
	COperationParam_Deliver_Task_In_Hate_List(){
		uOperationType = CTriggerData::o_deliver_task_in_hate_list;
		m_Data.uID = 0;
		m_Data.uIDType = enumPVTConst;
		m_Data.iRange = 2;
		m_Data.iPlayerNum = 1;
	}
	
public:
	ASetTemplate<int> id_type;
	O_DELIVER_TASK_IN_HATE_LIST m_Data;
	virtual void * GetOperationParam(){ return &m_Data; }
	
protected:
	virtual void BuildProperty();
	virtual void UpdateProperty(bool bGet){
		int index(0);
		if(bGet){
			m_Data.uID = m_pProperty->GetPropVal(index++);
			m_Data.uIDType = m_pProperty->GetPropVal(index++);			
			m_Data.iRange = m_pProperty->GetPropVal(index++);			
			m_Data.iPlayerNum = m_pProperty->GetPropVal(index++);
		}else{
			m_pProperty->SetPropVal(index++,m_Data.uID);
			m_pProperty->SetPropVal(index++,m_Data.uIDType);
			m_pProperty->SetPropVal(index++,m_Data.iRange);
			m_pProperty->SetPropVal(index++,m_Data.iPlayerNum);
		}
	};
};

#endif
