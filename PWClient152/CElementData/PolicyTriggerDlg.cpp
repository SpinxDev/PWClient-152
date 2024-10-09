// PolicyTriggerDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ElementData.h"
#include "PolicyTriggerDlg.h"
#include "PolicyOperationDlg.h"
#include "OperationParam.h"
#include "ConditionDlg.h"
#include "BaseDataIDMan.h"
#include "PolicyDlg.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
extern CString GetVarTypeName(int type);
extern CString GetOperatorName(int op);
extern CString GetPatrolType(int type);
extern CString GetPatrolSpeedType(int type);
extern CString GetConditionSign(const CTriggerData::_s_tree_item * temp);
extern CString GetTargetTypeName(int type);

/////////////////////////////////////////////////////////////////////////////
// CPolicyTriggerDlg dialog
extern CString strOperationList[];

CPolicyTriggerDlg::CPolicyTriggerDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CPolicyTriggerDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPolicyTriggerDlg)
	m_strProgram = _T("");
	m_strTriggerName = _T("");
	m_bActive = FALSE;
	m_uTriggerID = 0;
	//}}AFX_DATA_INIT
	m_pTriggerData = NULL;
	m_bModifyed = false;
	m_dwTriggerID = 0;
	m_bIsChanged = false; 
	m_bAttackValid = true;
	m_pCurrentPolicy = 0;
	m_pOldTree = 0;
}


void CPolicyTriggerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPolicyTriggerDlg)
	DDX_Control(pDX, IDC_LISTBOX_OPERATION, m_listOperation);
	DDX_Text(pDX, IDC_EDIT_PROGRAM, m_strProgram);
	DDX_Text(pDX, IDC_EDIT_TRIGGER_NAME, m_strTriggerName);
	DDX_Check(pDX, IDC_CHECK_ACTIVE, m_bActive);
	DDX_Text(pDX, IDC_EDIT_TRIGGER_ID, m_uTriggerID);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPolicyTriggerDlg, CDialog)
	//{{AFX_MSG_MAP(CPolicyTriggerDlg)
	ON_BN_CLICKED(IDC_ADD_OPERATION, OnAddOperation)
	ON_BN_CLICKED(ID_MODIFY, OnModify)
	ON_BN_CLICKED(IDC_DEL_OPERATION, OnDelOperation)
	ON_BN_CLICKED(IDC_MOVE_UP, OnMoveUp)
	ON_BN_CLICKED(IDC_MOVE_DOWN, OnMoveDown)
	ON_BN_CLICKED(IDC_ADD_CONDITION, OnAddCondition)
	ON_EN_CHANGE(IDC_EDIT_TRIGGER_NAME, OnChangeEditTriggerName)
	ON_BN_CLICKED(IDC_CHECK_ACTIVE, OnCheckActive)
	ON_LBN_DBLCLK(IDC_LISTBOX_OPERATION, OnDblclkListboxOperation)
	ON_BN_CLICKED(IDC_RADIO_ATTACK_EFFECT, OnRadioAttackEffect)
	ON_BN_CLICKED(IDC_RADIO_ATTACT_NOEFFECT, OnRadioAttactNoeffect)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPolicyTriggerDlg message handlers

void CPolicyTriggerDlg::OnAddOperation() 
{
	// TODO: Add your control notification handler code here
	CPolicyOperationDlg dlg;
	dlg.m_pPolicyData = m_pCurrentPolicy;
	if(IDOK==dlg.DoModal())
	{
		void *pOperationParam = NULL;
		COperationParam *pParam = dlg.GetOperationModifier();
		pParam->CopyTo(pOperationParam);
		if(m_pTriggerData) m_pTriggerData->AddOperaion(pParam->GetOperationType(),pOperationParam,&pParam->mTarget);
		m_bIsChanged = true;
		FreshList();
		delete pParam;
	}
}

void CPolicyTriggerDlg::OnModify() 
{
	if(m_pTriggerData==NULL || m_pCurrentPolicy==NULL) return;
	UpdateData(true);
	
	if(m_pTriggerData->GetConditonRoot()==0)
	{
		MessageBox("触发器必须有条件表达式!");
		return;
	}
	
	if(m_pTriggerData->GetOperaionNum()<=0)
	{
		MessageBox("触发器中至少有一个操作!");
		return;
	}

	if(m_bModifyed)
	{
		if(m_strTriggerName.IsEmpty())
		{
			MessageBox("触发器名字不能为空!");
			return;
		}
		if(m_pTriggerData) m_pTriggerData->SetName(m_strTriggerName);
		if(m_pOldTree) delete m_pOldTree;
		
	}else
	{
		CString strName;
		unsigned int tempId = 0;
		strName = m_strTriggerName;
		if(m_strTriggerName.IsEmpty()) 
		{
			MessageBox("触发器名字不能为空!");
			return;
		}
		for( int i = 0; i < m_pCurrentPolicy->GetTriggerPtrNum(); ++i)
		{
			CTriggerData *pData = m_pCurrentPolicy->GetTriggerPtr(i);
			if(tempId <= pData->GetID()) tempId = pData->GetID() + 1;
		}
		m_dwTriggerID = tempId;
		m_pTriggerData->SetName(strName);
		m_pTriggerData->SetID(m_dwTriggerID);
	}
	if(m_bActive) m_pTriggerData->ActiveTrigger();
	else m_pTriggerData->ToggleTrigger();
	m_pTriggerData->SetAttackValid(m_bAttackValid);
	m_bIsChanged = true;
	CDialog::OnOK();
}

void CPolicyTriggerDlg::OnCancel()
{
	if(m_bModifyed)
	{
		RestoreTrigger(m_pOldTree);
		if(m_pOldTree) delete m_pOldTree;
	}else
	{	
		if(m_pTriggerData)
		{
			m_pTriggerData->Release();
			delete m_pTriggerData;
		}
	}
	CDialog::OnCancel();
}

BOOL CPolicyTriggerDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	if(!m_bModifyed)
	{
		CButton *pButton = (CButton *)GetDlgItem(IDC_RADIO_ATTACK_EFFECT);
		pButton->SetCheck(true);
		m_bAttackValid =  true;
		m_pTriggerData = new CTriggerData;
	}else 
	{
		CWnd *pWnd = GetDlgItem(IDC_EDIT_TRIGGER_NAME);
		m_strTriggerName = m_pTriggerData->GetName();
		//pWnd->EnableWindow(false);
		FreshList();
		m_bActive = m_pTriggerData->IsActive();
		m_bAttackValid = m_pTriggerData->OnlyAttackValid();
		
		CButton *pButton = (CButton *)GetDlgItem(IDC_RADIO_ATTACK_EFFECT);
		if(m_bAttackValid) pButton->SetCheck(true);
		else pButton->SetCheck(false);
		pButton = (CButton *)GetDlgItem(IDC_RADIO_ATTACT_NOEFFECT);
		if(m_bAttackValid) pButton->SetCheck(false);
		else pButton->SetCheck(true);


		CTriggerData::_s_tree_item *root = m_pTriggerData->GetConditonRoot();
		m_strProgram = TraverselTree(root);
		m_uTriggerID = m_pTriggerData->GetID();
		
		//保存原来的触发数据，以后便于取消是恢复
		StartRecord();
		
		UpdateData(false);
	}
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CPolicyTriggerDlg::FreshList()
{
	int count = m_listOperation.GetCount();
	int i(0);
	for( i = 0; i < count; i++ )
		m_listOperation.DeleteString(0);

	if(m_pTriggerData==NULL) return;
	int n = m_pTriggerData->GetOperaionNum();
	unsigned int iType;
	CString strCommand;
	CString strParam;
	CTriggerData::_s_operation* pOperation = 0;
	void *pData;
	for( i = 0; i < n; i++)
	{
		pOperation = m_pTriggerData->GetOperaion(i);
		pData = pOperation->pParam;
		iType = pOperation->iType;
		strCommand = strOperationList[iType];
		strCommand +=" ";
		strParam = "";
		switch(iType)
		{
		case CTriggerData::o_active_trigger:
			strParam.Format("[触发器(%s)]",GetTriggerName(((O_ACTIVE_TRIGGER *)pData)->uID));
			break;
		case CTriggerData::o_attact:
			switch(((O_ATTACK_TYPE *)pData)->uType)
			{
			case COperationParam_Attack::a_close_battle:
				strParam ="[物理肉搏]";
				break;
			case COperationParam_Attack::a_skill:
				strParam ="[物理弓箭]";
				break;
			case COperationParam_Attack::a_long_distance:
				strParam ="[魔法]";
				break;
			case COperationParam_Attack::a_skill_and_battle:
				strParam ="[肉搏＋远程]";
				break;
			}
			break;
		case CTriggerData::o_create_timer:
			strParam.Format("[定时器(%d)] [间隔 %d] [次数 %d]",((O_CREATE_TIMER *)pData)->uID,((O_CREATE_TIMER *)pData)->uPeriod,((O_CREATE_TIMER *)pData)->uCounter);
			break;
		case CTriggerData::o_kill_timer:
			strParam.Format("[定时器(%d)]",((O_KILL_TIMER *)pData)->uID);
			break;
		case CTriggerData::o_active_controller:
			strParam.Format("[产生怪物控制器(%d)] [停止激活(%d)]",((O_ACTIVE_CONTROLLER *)pData)->uID,((O_ACTIVE_CONTROLLER *)pData)->bStop);
			break;
		case CTriggerData::o_run_trigger:
			strParam.Format("[触发器(%s)]",GetTriggerName(((O_RUN_TRIGGER *)pData)->uID));
			break;
		case CTriggerData::o_stop_trigger:
			strParam.Format("[触发器(%s)]",GetTriggerName(((O_STOP_TRIGGER *)pData)->uID));
			break;
		case CTriggerData::o_set_global:
			{
				strParam.Format("[(id=%d)(value=%d)(Is value = %d)]",((O_SET_GLOBAL *)pData)->iID,((O_SET_GLOBAL *)pData)->iValue,((O_SET_GLOBAL *)pData)->bIsValue);
				break;
			}
		case CTriggerData::o_revise_global:
			{
				strParam.Format("[(id=%d)(value=%d)]",((O_REVISE_GLOBAL *)pData)->iID,((O_REVISE_GLOBAL *)pData)->iValue);
				break;
			}
		case CTriggerData::o_summon_monster:
			{
				strParam.Format("[%d,%d,%d,%d,%d,%d]",((O_SUMMON_MONSTER*)pData)->iDispearCondition,((O_SUMMON_MONSTER*)pData)->iMonsterID,((O_SUMMON_MONSTER*)pData)->iRange,((O_SUMMON_MONSTER*)pData)->iLife,((O_SUMMON_MONSTER*)pData)->iPathID,((O_SUMMON_MONSTER*)pData)->iMonsterNum);
				break;
			}
		case CTriggerData::o_walk_along:
			{
				strParam.Format("[%d,%d,%d,%d]",((O_WALK_ALONG*)pData)->iWorldID,((O_WALK_ALONG*)pData)->iPathID,((O_WALK_ALONG*)pData)->iPatrolType,((O_WALK_ALONG*)pData)->iSpeedType);
				break;
			}
		case CTriggerData::o_play_action:
			{
				strParam.Format("[%s,%d,%d,%d]",((O_PLAY_ACTION*)pData)->szActionName,((O_PLAY_ACTION*)pData)->iLoopCount,((O_PLAY_ACTION*)pData)->iInterval,((O_PLAY_ACTION*)pData)->iPlayTime);
				break;
			}
		case CTriggerData::o_revise_history:
			{
				strParam.Format("[(id=%d)(value=%d)]",((O_REVISE_HISTORY *)pData)->iID,((O_REVISE_HISTORY *)pData)->iValue);
				break;
			}
		case CTriggerData::o_set_history:
			{
				strParam.Format("[(id=%d)(value=%d)(Is value = %d)]",((O_SET_HISTORY *)pData)->iID,((O_SET_HISTORY *)pData)->iValue,((O_SET_HISTORY *)pData)->bIsHistoryValue);
				break;
			}
		case CTriggerData::o_deliver_faction_pvp_points:
			switch(((O_DELIVER_FACTION_PVP_POINTS *)pData)->uType)
			{
			case enumPFPPTMineCar:
				strParam ="[帮派资源车]";
				break;
			case enumPFPPTMineBase:
				strParam ="[帮派资源基地]";
				break;
			case enumPFPPTMineCarArrived:
				strParam ="[帮派资源车到达目的地]";
				break;
			}
			break;
		case CTriggerData::o_calc_var:
			{
				const O_CALC_VAR *p = (const O_CALC_VAR *)pData;
				strParam.Format("[%s(%d)=%s(%d)%s%s(%d)]",
					GetVarTypeName(p->iDstType), p->iDst,
					GetVarTypeName(p->iSrc1Type), p->iSrc1,
					GetOperatorName(p->iOp),
					GetVarTypeName(p->iSrc2Type), p->iSrc2);
			}
			break;
		case CTriggerData::o_summon_monster_2:
			{
				const O_SUMMON_MONSTER_2 *p = (const O_SUMMON_MONSTER_2 *)pData;
				strParam.Format("[%d,%s(%d),%d,%d,%s(%d),%s(%d)]",
					p->iDispearCondition,
					GetVarTypeName(p->iMonsterIDType), p->iMonsterID,
					p->iRange,p->iLife,
					GetVarTypeName(p->iPathIDType), p->iPathID,
					GetVarTypeName(p->iMonsterNumType), p->iMonsterNum);
			}
			break;
		case CTriggerData::o_walk_along_2:
			{
				const O_WALK_ALONG_2 *p = (const O_WALK_ALONG_2 *)pData;
				strParam.Format("[%d,%s(%d),%s,%s]",
					p->iWorldID,
					GetVarTypeName(p->iPathIDType), p->iPathID,
					GetPatrolType(p->iPatrolType),
					GetPatrolSpeedType(p->iSpeedType));
				break;
			}
		case CTriggerData::o_talk:
			{
				const O_TALK_TEXT *p = (const O_TALK_TEXT *)pData;
				strParam.Format("[内容%s,附加数据(0x%x)]","...",p->uAppendDataMask);
			}
			break;
		case CTriggerData::o_use_skill:
			strParam.Format("[技能%d] [等级%d]",((O_USE_SKILL *)pData)->uSkill,((O_USE_SKILL *)pData)->uLevel);
			break;
		case CTriggerData::o_use_skill_2:
			{
				const O_USE_SKILL_2 *p = (const O_USE_SKILL_2 *)pData;
				strParam.Format("[技能 %s(%d)] [等级 %s(%d)]",
					GetVarTypeName(p->uSkillType), p->uSkill,
					GetVarTypeName(p->uLevelType), p->uLevel);
			}
			break;			
		case CTriggerData::o_active_controller_2:
			{				
				const O_ACTIVE_CONTROLLER_2 *p = (const O_ACTIVE_CONTROLLER_2 *)pData;
				strParam.Format("[产生怪物控制器 %s(%d)] [停止激活(%d)]",
					GetVarTypeName(p->uIDType), p->uID,
					p->bStop);
			}
			break;
		case CTriggerData::o_deliver_task:
			{
				const O_DELIVER_TASK *p = (const O_DELIVER_TASK *)pData;
				strParam.Format("[任务ID %s(%d)]", GetVarTypeName(p->uIDType), p->uID);
			}
			break;
		case CTriggerData::o_summon_mine:
			{
				const O_SUMMON_MINE *p = (const O_SUMMON_MINE *)pData;
				strParam.Format("[%s(%d),%d,%s(%d),%s(%d)]",
					GetVarTypeName(p->iMineIDType), p->iMineID,
					p->iRange,
					GetVarTypeName(p->iLifeType),p->iLife,
					GetVarTypeName(p->iMineNumType), p->iMineNum);
			}
			break;
		case CTriggerData::o_summon_npc:
			{
				const O_SUMMON_NPC *p = (const O_SUMMON_NPC *)pData;
				strParam.Format("[%s(%d),%d,%s(%d),%s(%d),%s(%d)]",
					GetVarTypeName(p->iNPCIDType), p->iNPCID,
					p->iRange,
					GetVarTypeName(p->iLifeType), p->iLife,
					GetVarTypeName(p->iPathIDType), p->iPathID,
					GetVarTypeName(p->iNPCNumType), p->iNPCNum);
			}
			break;
		case CTriggerData::o_deliver_random_task_in_region:
			{
				const O_DELIVER_RANDOM_TASK_IN_REGION *p = (const O_DELIVER_RANDOM_TASK_IN_REGION *)pData;
				strParam.Format("[(配置表%d) (最小点[%f,%f,%f]) (最大点[%f,%f,%f])]",
					p->uID,
					p->zvMin.x, p->zvMin.y, p->zvMin.z,
					p->zvMax.x, p->zvMax.y, p->zvMax.z);
			}
			break;
		case CTriggerData::o_deliver_task_in_hate_list:
			{
				const O_DELIVER_TASK_IN_HATE_LIST *p = (const O_DELIVER_TASK_IN_HATE_LIST *)pData;
				strParam.Format("[任务ID %s(%d),距离%d米,玩家%d个]", GetVarTypeName(p->uIDType), p->uID, p->iRange, p->iPlayerNum);
			}
			break;
		}
		strParam += GetTargetStr(&pOperation->mTarget);
		m_listOperation.AddString(strCommand + strParam);
	}
}

void CPolicyTriggerDlg::OnDelOperation() 
{
	// TODO: Add your control notification handler code here
	int sel = m_listOperation.GetCurSel();
	if(sel!=-1) 
	{
		if(IDYES ==	AfxMessageBox("你确定要删除当前选中的操作吗?",MB_YESNO|MB_ICONQUESTION))
		{
			m_pTriggerData->DelOperation(sel);
			FreshList();
			m_bIsChanged = true;
		}
	}
}

void CPolicyTriggerDlg::OnMoveUp() 
{
	// TODO: Add your control notification handler code here
	int sel = m_listOperation.GetCurSel();
	if(sel == -1 || sel == 0) return;
	unsigned int iType1,iType2;
	void *pData1, *pData2;
	CTriggerData::_s_target target1,target2;
	m_pTriggerData->GetOperaion(sel,iType1,&pData1, target1);
	m_pTriggerData->GetOperaion(sel-1,iType2,&pData2,target2);

	m_pTriggerData->SetOperation(sel,iType2,pData2,&target2);
	m_pTriggerData->SetOperation(sel-1,iType1, pData1,&target1);
	FreshList();
	m_listOperation.SetCurSel(sel-1);
	m_bIsChanged = true;

}

void CPolicyTriggerDlg::OnMoveDown() 
{
	// TODO: Add your control notification handler code here
	int sel = m_listOperation.GetCurSel();
	if(sel == -1 || sel > m_listOperation.GetCount()-2) return;
	unsigned int iType1,iType2;
	void *pData1,*pData2;
	CTriggerData::_s_target target1,target2;
	m_pTriggerData->GetOperaion(sel,iType1,&pData1, target1);
	m_pTriggerData->GetOperaion(sel+1,iType2,&pData2,target2);

	m_pTriggerData->SetOperation(sel,iType2,pData2,&target2);
	m_pTriggerData->SetOperation(sel+1,iType1, pData1,&target1);
	FreshList();
	m_listOperation.SetCurSel(sel+1);
	m_bIsChanged = true;
}


void CPolicyTriggerDlg::FreshCondition()
{
	/*
	int count = m_listCondition.GetCount();
	for( int i = 0; i < count; i++ )
		m_listCondition.DeleteString(0);

	if(m_pTriggerData==NULL) return;
	int n = m_pTriggerData->GetConditionNum();
	unsigned int iType;
	CString strCommand;
	CString strParam;
	CString strTotal;
	CString strSign;
	void *pData;
	for( i = 0; i < n; i++)
	{
		m_pTriggerData->GetCondition(i,iType,&pData);
		strCommand = strConditonList[iType];
		strParam = "";
		switch(iType)
		{
		case CTriggerData::c_hp_less:
			strParam.Format("%d",((C_HP_LESS*)pData)->uPercent);
			break;
		case CTriggerData::c_time_come:
			strParam.Format("%d",((C_TIME_COME*)pData)->uID);
			break;
		case CTriggerData::c_random:
			strParam.Format("%d",((C_RANDOM*)pData)->uProbability);
			break;
		}
		if(!strParam.IsEmpty())
		{
			strCommand +="(";
			strCommand +=strParam;
			strCommand +=")";
			m_listCondition.AddString(strCommand);
		}else m_listCondition.AddString(strCommand);
		
		strSign.Format(strConditonSign[iType],strParam);
		strTotal += strSign;
		strTotal +=" ";
	}

	m_strProgram = strTotal;
	*/
	UpdateData(false);
}

CString CPolicyTriggerDlg::TraverselTree( void *pTree)
{
	CTriggerData::_s_tree_item * temp = (CTriggerData::_s_tree_item*)pTree;
	if(temp==0) return "";
	CString str;	
	str +="(";
	if(temp->pLeft) str += TraverselTree(temp->pLeft);
	str += GetConditionSign(temp);
	if(temp->pRight) str += TraverselTree(temp->pRight);
	str +=")";
	return str;
}

void CPolicyTriggerDlg::OnAddCondition() 
{
	// TODO: Add your control notification handler code here
	UpdateData(true);

	CTriggerData::_s_tree_item* root = m_pTriggerData->GetConditonRoot();
	CTriggerData::_s_tree_item *old = 0;
	if(root==0)
		root = new CTriggerData::_s_tree_item;
	else old = CTriggerData::CopyConditonTree(root);
	
	CConditionDlg dlg;
	dlg.m_pRoot = root;
	dlg.m_bIsRoot = true;


	if(IDOK == dlg.DoModal())
	{
		m_pTriggerData->SetConditionRoot(root);
		m_strProgram = TraverselTree(root);
		m_bIsChanged = true;
		if(old) delete old;
	}else 
	{
		delete root;
		m_pTriggerData->SetConditionRoot(old);
		m_strProgram = TraverselTree(old);
	}
	UpdateData(false);
}

void CPolicyTriggerDlg::OnDelCondition() 
{
	// TODO: Add your control notification handler code here
	/*
	int sel = m_listCondition.GetCurSel();
	if(sel!=-1) 
	{
		if(IDYES ==	AfxMessageBox("你确定要删除当前选中的条件吗?",MB_YESNO|MB_ICONQUESTION))
		{
			m_pTriggerData->DelCondition(sel);
			FreshCondition();
		}
	}*/
}

void CPolicyTriggerDlg::OnMoveUpCondition() 
{
	// TODO: Add your control notification handler code here
	/*
	int sel = m_listCondition.GetCurSel();
	if(sel == -1 || sel == 0) return;
	unsigned int iType1,iType2;
	void *pData1, *pData2;
	m_pTriggerData->GetCondition(sel,iType1,&pData1);
	m_pTriggerData->GetCondition(sel-1,iType2,&pData2);

	m_pTriggerData->SetCondition(sel,iType2,pData2);
	m_pTriggerData->SetCondition(sel-1,iType1, pData1);
	FreshCondition();
	m_listCondition.SetCurSel(sel-1);*/

}

void CPolicyTriggerDlg::OnMoveDownCondition() 
{
	/*
	// TODO: Add your control notification handler code here
		int sel = m_listCondition.GetCurSel();
	if(sel == -1 || sel > m_listCondition.GetCount()-2) return;
	unsigned int iType1,iType2;
	void *pData1,*pData2;
	m_pTriggerData->GetCondition(sel,iType1,&pData1);
	m_pTriggerData->GetCondition(sel+1,iType2,&pData2);

	m_pTriggerData->SetCondition(sel,iType2,pData2);
	m_pTriggerData->SetCondition(sel+1,iType1, pData1);
	FreshCondition();
	m_listCondition.SetCurSel(sel+1);
	*/
	
}

void CPolicyTriggerDlg::OnCheckProgramError() 
{
	// TODO: Add your control notification handler code here
	
}


void CPolicyTriggerDlg::OnChangeEditTriggerName() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	UpdateData(true);
}

void CPolicyTriggerDlg::OnCheckActive() 
{
	// TODO: Add your control notification handler code here
	m_bIsChanged = true;
}

CString CPolicyTriggerDlg::GetTriggerName( unsigned int id)
{
	if(m_pCurrentPolicy)
	{
		int n = m_pCurrentPolicy->GetTriggerPtrNum();
		for( int i = 0; i < n; ++i)
		{
			CTriggerData *pData = m_pCurrentPolicy->GetTriggerPtr(i);
			if(pData->GetID() == id) return pData->GetName();
		}
	}
	return "错误的触发器ID";
}

CString GetTargetStr(void *pTarget)
{
	if(pTarget==NULL) return "没目标";
	CString msg;
	CTriggerData::_s_target *pTempTarget = (CTriggerData::_s_target *)pTarget;
	if(pTempTarget==0) return msg;  
	msg +=" [";	
	msg += GetTargetTypeName(pTempTarget->iType);
	switch(pTempTarget->iType)
	{
	case CTriggerData::t_occupation_list:
		break;
	}

	msg +="]";
	return msg;
}

void CPolicyTriggerDlg::OnDblclkListboxOperation() 
{
	// TODO: Add your control notification handler code here
	int sel = m_listOperation.GetCurSel();
	if(sel == -1) return;

	CPolicyOperationDlg dlg;
	dlg.m_pPolicyData = m_pCurrentPolicy;
	dlg.m_bModify = true;
	dlg.m_pOperation = m_pTriggerData->GetOperaion(sel);
	if(IDOK==dlg.DoModal())
	{
		void *pOperationParam = NULL;
		dlg.GetOperationModifier()->CopyTo(pOperationParam);
		
		//Release old operation data
		CTriggerData::FreeOperationParam(m_pTriggerData->GetOperaion(sel));
		CTriggerData::FreeOperationTarget(m_pTriggerData->GetOperaion(sel));

		//Set to new operation data
		m_pTriggerData->SetOperation(sel,dlg.GetOperationModifier()->GetOperationType(),pOperationParam,&dlg.GetOperationModifier()->mTarget);
		m_bIsChanged = true;
		FreshList();
		delete dlg.GetOperationModifier();
	}
}

void CPolicyTriggerDlg::OnRadioAttackEffect() 
{
	// TODO: Add your control notification handler code here
	m_bAttackValid = true;
}

void CPolicyTriggerDlg::OnRadioAttactNoeffect() 
{
	// TODO: Add your control notification handler code here
	m_bAttackValid = false;
}

void CPolicyTriggerDlg::SaveOldTree(TRIGGER_TREE *pTreeRoot,CTriggerData *pTrigger)
{
	if(pTreeRoot==0 || pTrigger==0) return;
	
	pTreeRoot->pTrigger = pTrigger;
	int n = pTrigger->GetOperaionNum();
	for( int i = 0; i < n; ++i)
	{
		CTriggerData::_s_operation *pOperation = pTrigger->GetOperaion(i);
		if(pOperation->iType==CTriggerData::o_run_trigger)
		{
			int index = m_pCurrentPolicy->GetIndex(((O_RUN_TRIGGER*)pOperation->pParam)->uID);
			if(index!=-1)
			{
				CTriggerData *pData = m_pCurrentPolicy->GetTriggerPtr(index);
				CTriggerData *pNewData = pData->CopyObject();
				ASSERT(pNewData);
				TRIGGER_TREE *pNewTree = new TRIGGER_TREE;
				ASSERT(pNewTree);
				pTreeRoot->listChild.push_back(pNewTree);
				SaveOldTree(pNewTree,pNewData);
			}else pTreeRoot->listChild.push_back(0);
		}
	}
}

void CPolicyTriggerDlg::StartRecord()
{
	m_pOldTree = new TRIGGER_TREE;
	if(m_pOldTree==0) return;
	CTriggerData *pNewData = m_pTriggerData->CopyObject();
	ASSERT(pNewData);
	SaveOldTree(m_pOldTree,pNewData);
}

void CPolicyTriggerDlg::RestoreTrigger(TRIGGER_TREE *pTreeRoot)
{
	if(pTreeRoot==0) return;
	
	int index = m_pCurrentPolicy->GetIndex(pTreeRoot->pTrigger->GetID());
	if(index!=-1)
	{
		CTriggerData *pData = m_pCurrentPolicy->GetTriggerPtr(index);
		if(pData) 
		{
			pData->Release();
			delete pData;
		}
		m_pCurrentPolicy->SetTriggerPtr(index,pTreeRoot->pTrigger->CopyObject());
	}
	
	int n = pTreeRoot->pTrigger->GetOperaionNum();
	int k = 0;
	for( int i = 0; i < n; ++i)
	{
		CTriggerData::_s_operation *pOperation = pTreeRoot->pTrigger->GetOperaion(i);
		if(pOperation->iType==CTriggerData::o_run_trigger)
			RestoreTrigger(pTreeRoot->listChild[k++]);
	}
}
