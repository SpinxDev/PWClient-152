#include "stdafx.h"
#include "Global.h"
#include "operationparam.h"
#include "PolicyTriggerDlg.h"
#include "EquipMaskDlg.h"

extern CString GetDisppearTypeName(int type);
extern CString GetVarTypeName(int type);
extern CString GetOperatorName(int op);
extern CString GetPatrolType(int type);
extern CString GetPatrolSpeedType(int type);

typedef CString (*pGetTypeName)(int type);

//	enumPolicyTalkTextAppendDataMask ��Ӧ����
static const int TALK_TYPE_NAME_COUNT = 4;
static LPCTSTR s_strTalkTypeName[TALK_TYPE_NAME_COUNT] = 
{
	_T("Ŀ�����ID"),
	_T("�ֲ�����0"),
	_T("�ֲ�����1"),
	_T("�ֲ�����2"),
};

template<typename enumType>
void CreateSet(ASetTemplate<int> &s, pGetTypeName pGetter, enumType tFrom, enumType tTo)
{
	for (int i = tFrom; i <= tTo; ++ i)
	{
		s.AddElement((LPCTSTR)(*pGetter)(i), i);
	}
}
template<typename enumType>
void CreateSet(ASetTemplate<int> &s, pGetTypeName pGetter, enumType tNum)
{
	for (int i = 0; i < tNum; ++ i)
	{
		s.AddElement((LPCTSTR)(*pGetter)(i), i);
	}
}

BOOL CALLBACK TRIGGER_ID_FUNCS::OnActivate(void)
{
	COperationParam_Run_Trigger *pTrigger = (COperationParam_Run_Trigger*)m_pParent;
	if(pTrigger->m_Data.uID==0)
	{//�¸�һ��������
		CPolicyTriggerDlg dlg;
		dlg.m_pCurrentPolicy = pTrigger->m_pPolicyData;
		if(IDOK==dlg.DoModal())
		{
			if(dlg.m_pTriggerData)
			{
				dlg.m_pTriggerData->SetRunStatus(true);
				pTrigger->m_pPolicyData->AddTriggerPtr(dlg.m_pTriggerData);
				pTrigger->m_Data.uID = dlg.m_pTriggerData->GetID();
				m_strText = dlg.m_pTriggerData->GetName();
				g_bPolicyModifyed = true;
			}
		}
	}else
	{//��һ���ɵĴ�����
		for( int i = 0; i < pTrigger->m_pPolicyData->GetTriggerPtrNum(); i++)
		{
			CTriggerData *pData =  pTrigger->m_pPolicyData->GetTriggerPtr(i);
			if(pData->IsRun())
			{
				if(pData->GetID() ==  pTrigger->m_Data.uID)
				{
					CPolicyTriggerDlg dlg;
					dlg.m_bModifyed = true;
					dlg.m_pTriggerData = pData;
					dlg.m_pCurrentPolicy = pTrigger->m_pPolicyData;
					if(IDOK==dlg.DoModal())
					{
						g_bPolicyModifyed = true;
					}
					m_strText = pData->GetName();
					break;
				}
			}
		}
	}
	return TRUE;
}

LPCTSTR CALLBACK TRIGGER_ID_FUNCS::OnGetShowString(void) const
{
	return m_strText;
}

AVariant CALLBACK TRIGGER_ID_FUNCS::OnGetValue(void) const
{
	return m_var;
}

void CALLBACK TRIGGER_ID_FUNCS::OnSetValue(const AVariant& var)
{
	COperationParam_Run_Trigger *pTrigger = (COperationParam_Run_Trigger *)m_pParent;
	//��һ���ɵĴ�����
	for( int i = 0; i <  pTrigger->m_pPolicyData->GetTriggerPtrNum(); i++)
	{
		CTriggerData *pData = pTrigger->m_pPolicyData->GetTriggerPtr(i);
		if(pData->IsRun())
		{
			if(pData->GetID() == pTrigger->m_Data.uID)
			{
				m_strText = pData->GetName();
				break;
			}
		}
	}
}

//
COperationParam * COperationParam::Create(int operation){
	COperationParam * result = NULL;
	switch(operation) 
	{
	case CTriggerData::o_attact:
		result = new COperationParam_Attack;
		break;
	case CTriggerData::o_use_skill:
		result = new COperationParam_Use_Skill;
		break;
	case CTriggerData::o_talk:
		result = new COperationParam_Talk;
		break;
	case CTriggerData::o_reset_hate_list:
		result = new COperationParam_Reset_hates;
		break;
	case CTriggerData::o_run_trigger:
		result = new COperationParam_Run_Trigger;
		break;
	case CTriggerData::o_stop_trigger:
		result = new COperationParam_Stop_Trigger;
		break;
	case CTriggerData::o_active_trigger:
		result = new COperationParam_Active_Trigger;
		break;
	case CTriggerData::o_create_timer:
		result = new COperationParam_Create_Timer;
		break;
	case CTriggerData::o_kill_timer:
		result = new COperationParam_Kill_Timer;
		break;
	case CTriggerData::o_active_controller:
		result = new COperationParam_Active_Controller;
		break;
	case CTriggerData::o_flee:
		result = new COperationParam_Flee;
		break;
	case CTriggerData::o_set_hate_to_first:
		result = new COperationParam_SetHateToFirst;
		break;
	case CTriggerData::o_set_hate_to_last:
		result = new COperationParam_SetHateToLast;
		break;
	case CTriggerData::o_set_hate_fifty_percent:
		result = new COperationParam_SetHateFiftyPercent;
		break;
	case CTriggerData::o_skip_operation:
		result = new COperationParam_SkipOperation;
		break;
	case CTriggerData::o_set_global:
		result = new COperationParam_Set_Global;
		break;
	case CTriggerData::o_revise_global:
		result = new COperationParam_Revise_Global;
		break;
	case CTriggerData::o_summon_monster:
		result = new COperationParam_Summon_Monster;
		break;
	case CTriggerData::o_walk_along:
		result = new COperationParam_Walk_Along;
		break;
	case CTriggerData::o_play_action:
		result = new COperationParam_Play_Action;
		break;
	case CTriggerData::o_revise_history:
		result = new COperationParam_Revise_History;
		break;
	case CTriggerData::o_set_history:
		result = new COperationParam_Set_History;
		break;
	case CTriggerData::o_deliver_faction_pvp_points:
		result = new COperationParam_DeliverFactionPVPPoints;
		break;
	case CTriggerData::o_calc_var:
		result = new COperationParam_Calc_Var;
		break;
	case CTriggerData::o_summon_monster_2:
		result = new COperationParam_Summon_Monster_2;
		break;
	case CTriggerData::o_walk_along_2:
		result = new COperationParam_Walk_Along_2;
		break;
	case CTriggerData::o_use_skill_2:
		result = new COperationParam_Use_Skill_2;
		break;
	case CTriggerData::o_active_controller_2:
		result = new COperationParam_Active_Controller_2;
		break;
	case CTriggerData::o_deliver_task:
		result = new COperationParam_Deliver_Task;
		break;
	case CTriggerData::o_summon_mine:
		result = new COperationParam_Summon_Mine;
		break;
	case CTriggerData::o_summon_npc:
		result = new COperationParam_Summon_NPC;
		break;
	case CTriggerData::o_deliver_random_task_in_region:
		result = new COperationParam_Deliver_Random_Task_In_Region;
		break;
	case CTriggerData::o_deliver_task_in_hate_list:
		result = new COperationParam_Deliver_Task_In_Hate_List;
		break;
	}
	return result;
}

//	COperationParam_Talk
COperationParam_Talk::COperationParam_Talk()
{
	uOperationType = CTriggerData::o_talk;
	char szText[MAX_PATH];
	strcpy(szText,"\0");
	m_Data.uSize = OP_TEXT_LENGTH*2;
	m_Data.szData = new policy_char[m_Data.uSize/2];
	MultiByteToWideChar(CP_ACP,0,szText,MAX_PATH,m_Data.szData,OP_TEXT_LENGTH);
	m_Data.uAppendDataMask = 0;
	m_fs = NULL;
}

COperationParam_Talk::~COperationParam_Talk()
{
	if(m_Data.szData){
		delete [] m_Data.szData;
		m_Data.szData = NULL;
	}
	if (m_fs){
		delete m_fs;
		m_fs = NULL;
	}
}

void COperationParam_Talk::BuildProperty()
{
	AString strText = glb_WideCharToMultiByte(m_Data.szData, m_Data.uSize/2);
	m_pProperty->DynAddProperty(AVariant(strText), "��������");
	
	if (!m_fs){
		m_fs = new EquipMask;
	}
	m_fs->m_var = m_Data.uAppendDataMask;
	m_fs->m_pMaskNames = s_strTalkTypeName;
	m_fs->m_nNameCount = TALK_TYPE_NAME_COUNT;
	m_pProperty->DynAddProperty(m_Data.uAppendDataMask, "��������", (ASet*)m_fs, NULL, WAY_CUSTOM | WAY_DEFAULT);
}

void COperationParam_Talk::UpdateProperty(bool bGet)
{
	char szText[MAX_PATH];
	strcpy(szText,"\0");
	if(bGet)
	{
		AString temp = AString(m_pProperty->GetPropVal(0));	
		if(!temp.IsEmpty())
		{
			strcpy(szText,temp.GetBuffer(0));
			temp.ReleaseBuffer();
		}
		MultiByteToWideChar(CP_ACP,0,szText,MAX_PATH,m_Data.szData,OP_TEXT_LENGTH);
		m_Data.uAppendDataMask = m_pProperty->GetPropVal(1);
	}else
	{
		AString strText = glb_WideCharToMultiByte(m_Data.szData,OP_TEXT_LENGTH);
		m_pProperty->SetPropVal(0, strText);
		m_pProperty->SetPropVal(1, m_Data.uAppendDataMask);
	}
}

//	COperationParam_Calc_Var
void COperationParam_Calc_Var::BuildProperty()
{
	CreateSet(dst_type,	GetVarTypeName,	enumPVTGlobalVarID, enumPVTLocalVarID);
	CreateSet(src1_type,GetVarTypeName,	enumPVTNum);
	CreateSet(op_type,	GetOperatorName,enumPOTNum);
	CreateSet(src2_type,GetVarTypeName,	enumPVTNum);

	m_pProperty->DynAddProperty(AVariant(m_Data.iDst),		"����");
	m_pProperty->DynAddProperty(AVariant(m_Data.iDstType),	"��������", &dst_type);
	m_pProperty->DynAddProperty(AVariant(m_Data.iSrc1),		"������1");
	m_pProperty->DynAddProperty(AVariant(m_Data.iSrc1Type),	"������1����", &src1_type);
	m_pProperty->DynAddProperty(AVariant(m_Data.iOp),		"��������", &op_type);
	m_pProperty->DynAddProperty(AVariant(m_Data.iSrc2),		"������2");
	m_pProperty->DynAddProperty(AVariant(m_Data.iSrc2Type),	"������2����", &src2_type);
}

//	COperationParam_Summon_Monster
void COperationParam_Summon_Monster::BuildProperty()
{
	CreateSet(condition_type, GetDisppearTypeName, enumPSDTNum);
	
	m_pProperty->DynAddProperty(AVariant(m_Data.iDispearCondition), "��ʧ����",&condition_type);
	m_pProperty->DynAddProperty(AVariant(m_Data.iMonsterID), "����ID");
	m_pProperty->DynAddProperty(AVariant(m_Data.iRange), "��Ŀ��ľ���(m)");
	m_pProperty->DynAddProperty(AVariant(m_Data.iLife), "��ʧ�ӳ�ʱ��(s)");
	m_pProperty->DynAddProperty(AVariant(m_Data.iPathID), "·��ID");
	m_pProperty->DynAddProperty(AVariant(m_Data.iMonsterNum), "�ٻ�����");
}

//	COperationParam_Summon_Monster_2
void COperationParam_Summon_Monster_2::BuildProperty()
{
	CreateSet(condition_type,	GetDisppearTypeName, enumPSDTNum);
	CreateSet(monster_id_type,	GetVarTypeName, enumPVTGlobalVarID, enumPVTConst);
	CreateSet(path_id_type,		GetVarTypeName, enumPVTGlobalVarID, enumPVTConst);
	CreateSet(monster_num_type, GetVarTypeName, enumPVTGlobalVarID, enumPVTConst);

	m_pProperty->DynAddProperty(AVariant(m_Data.iDispearCondition), "��ʧ����",&condition_type);
	m_pProperty->DynAddProperty(AVariant(m_Data.iMonsterID), "����ID");
	m_pProperty->DynAddProperty(AVariant(m_Data.iMonsterIDType), "����ID����",&monster_id_type);
	m_pProperty->DynAddProperty(AVariant(m_Data.iRange), "��Ŀ��ľ���(m)");
	m_pProperty->DynAddProperty(AVariant(m_Data.iLife), "��ʧ�ӳ�ʱ��(s)");
	m_pProperty->DynAddProperty(AVariant(m_Data.iPathID), "·��ID");
	m_pProperty->DynAddProperty(AVariant(m_Data.iPathIDType), "·��ID����",&path_id_type);
	m_pProperty->DynAddProperty(AVariant(m_Data.iMonsterNum), "�ٻ�����");
	m_pProperty->DynAddProperty(AVariant(m_Data.iMonsterNumType), "�ٻ���������",&monster_num_type);
}

//	COperationParam_Walk_Along
void COperationParam_Walk_Along::BuildProperty()
{
	CreateSet(patrol_type,	GetPatrolType,		enumPMPTNum);
	CreateSet(speed_type,	GetPatrolSpeedType, enumPMPSTBegin, enumPMPSTEnd);
	
	m_pProperty->DynAddProperty(AVariant(m_Data.iWorldID), "����ID");
	m_pProperty->DynAddProperty(AVariant(m_Data.iPathID), "·��ID");
	m_pProperty->DynAddProperty(AVariant(m_Data.iPatrolType), "Ѳ�߷�ʽ",&patrol_type);
	m_pProperty->DynAddProperty(AVariant(m_Data.iSpeedType), "�ٶ�",&speed_type);
}

//	COperationParam_Walk_Along_2
void COperationParam_Walk_Along_2::BuildProperty()
{
	CreateSet(path_id_type, GetVarTypeName,		enumPVTGlobalVarID, enumPVTConst);
	CreateSet(patrol_type,	GetPatrolType,		enumPMPTNum);
	CreateSet(speed_type,	GetPatrolSpeedType, enumPMPSTBegin, enumPMPSTEnd);
	
	m_pProperty->DynAddProperty(AVariant(m_Data.iWorldID),		"����ID");
	m_pProperty->DynAddProperty(AVariant(m_Data.iPathID),		"·��ID");
	m_pProperty->DynAddProperty(AVariant(m_Data.iPathID),		"·��ID����",	&path_id_type);
	m_pProperty->DynAddProperty(AVariant(m_Data.iPatrolType),	"Ѳ�߷�ʽ",		&patrol_type);
	m_pProperty->DynAddProperty(AVariant(m_Data.iSpeedType),	"�ٶ�",			&speed_type);
}

//	COperationParam_Use_Skill_2
void COperationParam_Use_Skill_2::BuildProperty()
{
	CreateSet(skill_type, GetVarTypeName, enumPVTGlobalVarID, enumPVTConst);
	CreateSet(level_type, GetVarTypeName, enumPVTGlobalVarID, enumPVTConst);

	m_pProperty->DynAddProperty(AVariant(m_Data.uSkill),	"����");
	m_pProperty->DynAddProperty(AVariant(m_Data.uSkillType),"��������", &skill_type);
	m_pProperty->DynAddProperty(AVariant(m_Data.uLevel),	"����");
	m_pProperty->DynAddProperty(AVariant(m_Data.uLevelType),"��������", &level_type);
}

//	COperationParam_Active_Controller_2
void COperationParam_Active_Controller_2::BuildProperty()
{
	CreateSet(id_type, GetVarTypeName, enumPVTGlobalVarID, enumPVTConst);

	m_pProperty->DynAddProperty(AVariant(m_Data.uID),		"�������������ID");
	m_pProperty->DynAddProperty(AVariant(m_Data.uIDType),	"�������������ID����", &id_type);
	m_pProperty->DynAddProperty(AVariant(m_Data.bStop),		"ֹͣ����");
}

//	COperationParam_Deliver_Task
void COperationParam_Deliver_Task::BuildProperty()
{
	CreateSet(id_type, GetVarTypeName, enumPVTGlobalVarID, enumPVTConst);

	m_pProperty->DynAddProperty(AVariant(m_Data.uID),		"����ID");
	m_pProperty->DynAddProperty(AVariant(m_Data.uIDType),	"����ID����", &id_type);
}

//	COperationParam_Summon_Mine
void COperationParam_Summon_Mine::BuildProperty()
{
	CreateSet(mine_id_type,		GetVarTypeName, enumPVTGlobalVarID, enumPVTConst);
	CreateSet(life_type,		GetVarTypeName, enumPVTGlobalVarID, enumPVTConst);
	CreateSet(mine_num_type,	GetVarTypeName, enumPVTGlobalVarID, enumPVTConst);
	
	m_pProperty->DynAddProperty(AVariant(m_Data.iMineID), "����ID");
	m_pProperty->DynAddProperty(AVariant(m_Data.iMineIDType), "����ID����",&mine_id_type);
	m_pProperty->DynAddProperty(AVariant(m_Data.iRange), "��Ŀ��ľ���(m)");
	m_pProperty->DynAddProperty(AVariant(m_Data.iLife), "��ʧ�ӳ�ʱ��(s)");
	m_pProperty->DynAddProperty(AVariant(m_Data.iLifeType), "��ʧ�ӳ�ʱ������",&life_type);
	m_pProperty->DynAddProperty(AVariant(m_Data.iMineNum), "�ٻ�����");
	m_pProperty->DynAddProperty(AVariant(m_Data.iMineNumType), "�ٻ���������",&mine_num_type);
}

//	COperationParam_Summon_NPC
void COperationParam_Summon_NPC::BuildProperty()
{
	CreateSet(npc_id_type,		GetVarTypeName, enumPVTGlobalVarID, enumPVTConst);
	CreateSet(life_type,		GetVarTypeName, enumPVTGlobalVarID, enumPVTConst);
	CreateSet(path_id_type,		GetVarTypeName, enumPVTGlobalVarID, enumPVTConst);
	CreateSet(npc_num_type,		GetVarTypeName, enumPVTGlobalVarID, enumPVTConst);
	
	m_pProperty->DynAddProperty(AVariant(m_Data.iNPCID), "NPC ID");
	m_pProperty->DynAddProperty(AVariant(m_Data.iNPCIDType), "NPC ID����",&npc_id_type);
	m_pProperty->DynAddProperty(AVariant(m_Data.iRange), "��Ŀ��ľ���(m)");
	m_pProperty->DynAddProperty(AVariant(m_Data.iLife), "��ʧ�ӳ�ʱ��(s)");
	m_pProperty->DynAddProperty(AVariant(m_Data.iLifeType), "��ʧ�ӳ�ʱ������",&npc_id_type);
	m_pProperty->DynAddProperty(AVariant(m_Data.iPathID), "·��ID");
	m_pProperty->DynAddProperty(AVariant(m_Data.iPathIDType), "·��ID����",&path_id_type);
	m_pProperty->DynAddProperty(AVariant(m_Data.iNPCNum), "�ٻ�����");
	m_pProperty->DynAddProperty(AVariant(m_Data.iNPCNumType), "�ٻ���������",&npc_num_type);
}

//	COperationParam_Deliver_Random_Task_In_Region
void COperationParam_Deliver_Random_Task_In_Region::BuildProperty()
{	
	m_pProperty->DynAddProperty(AVariant(m_Data.uID),		"�����б����ñ�ID");
	m_pProperty->DynAddProperty(AVariant(m_Data.zvMin.x),	"������С��x");
	m_pProperty->DynAddProperty(AVariant(m_Data.zvMin.y),	"������С��y");
	m_pProperty->DynAddProperty(AVariant(m_Data.zvMin.z),	"������С��z");
	m_pProperty->DynAddProperty(AVariant(m_Data.zvMax.x),	"��������x");
	m_pProperty->DynAddProperty(AVariant(m_Data.zvMax.y),	"��������y");
	m_pProperty->DynAddProperty(AVariant(m_Data.zvMax.z),	"��������z");
}

//	COperationParam_Deliver_Task_In_Hate_List
void COperationParam_Deliver_Task_In_Hate_List::BuildProperty()
{
	CreateSet(id_type, GetVarTypeName, enumPVTGlobalVarID, enumPVTConst);
	
	m_pProperty->DynAddProperty(AVariant(m_Data.uID),		"����ID");
	m_pProperty->DynAddProperty(AVariant(m_Data.uIDType),	"����ID����", &id_type);
	m_pProperty->DynAddProperty(AVariant(m_Data.iRange),	"��������ľ���(m)");
	m_pProperty->DynAddProperty(AVariant(m_Data.iPlayerNum),"��������������500��");
}