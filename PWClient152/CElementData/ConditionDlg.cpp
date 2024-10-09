// ConditionDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ElementData.h"
#include "ConditionDlg.h"
#include "Policy.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define ParamAs(T) ((T)m_pRoot->mConditon.pParam)

CString strConditonList[CTriggerData::c_num] = {"定时器到时(只能单独使用)","血量少于百分比","战斗开始(只能单独使用)","随机","杀死玩家(只能单独使用)","非","或","与","怪物死亡","加","减","乘","除","大于","小于","等于","变量","常量"/*,"时间点"*/,"受到伤害","到达路径终点","处于历史阶段","历史变量","脱离战斗(只能单独使用)", "局部变量", "到达路径终点（新）", "持有状态包", "迷宫主线地图块下标"};
CString strConditonSign[CTriggerData::c_num] = {"Timer(%d)","HpLess(%f)","StartAttack()","Random(%f)","KillPlayer()","!","||","&&","Died()","+","-","*","/",">","<","==","var[%d]","%d"/*,"TimePoint(%u:%u)"*/,"BeHurt(%d:%d)","ReachEnd(%d)","AtHistoryStage(%d)","HistoryValue(%d)", "StopFight()", "LocalVar[%d]", "ReachEnd2(%d:%s)", "HasFilter(%d)", "RoomIndex()"};

CString GetDisppearTypeName(int type)
{
	static CString strName[enumPSDTNum] = {"不随任何死亡死亡", "随召唤者死亡", "随召唤目标死亡", "召唤者和召唤目标任意一个死亡即死亡"};
	if (type >= 0 && type < enumPSDTNum){
		return strName[type];
	}
	CString str;
	str.Format("Unknown Disappear(%d)", type);
	return str;
}

CString GetPatrolType(int type)
{
	static CString strName[enumPMPTNum] = {"终点停止", "原路返回", "始终循环"};
	if (type >= 0 && type < enumPMPTNum){
		return strName[type];
	}
	CString str;
	str.Format("Unknown Patrol(%d)", type);
	return str;
}

CString GetPatrolSpeedType(int type)
{
	if (type == enumPMPSTSlow){
		return "慢速";
	}else if (type == enumPMPSTFast){
		return "快速";
	}else{
		CString str;
		str.Format("Unknown Speed(%d)", type);
		return str;
	}
}

CString GetVarTypeName(int type)
{
	static CString strName[enumPVTNum] = {"全局变量", "局部变量", "常量", "【0-给定值的随机数】"};
	if (type >= 0 && type < enumPVTNum){
		return strName[type];
	}
	CString str;
	str.Format("Unknown VarType(%d)", type);
	return str;
}

CString GetOperatorName(int op)
{
	static CString strName[enumPOTNum] = {"+", "-", "*", "/", "%"};
	if (op >= 0 && op < enumPOTNum){
		return strName[op];
	}
	CString str;
	str.Format("Unknown Op(%d)", op);
	return str;
}

CString GetConditionSign(const CTriggerData::_s_tree_item * temp){
	CString result;
	switch(temp->mConditon.iType) 
	{
	case CTriggerData::c_time_come:
		result.Format(strConditonSign[temp->mConditon.iType],((C_TIME_COME*)temp->mConditon.pParam)->uID);
		break;
	case CTriggerData::c_be_hurt:
		result.Format(strConditonSign[temp->mConditon.iType],((C_BE_HURT*)temp->mConditon.pParam)->iHurtLow,((C_BE_HURT*)temp->mConditon.pParam)->iHurtHigh);
		break;
	case CTriggerData::c_hp_less:
		result.Format(strConditonSign[temp->mConditon.iType],((C_HP_LESS*)temp->mConditon.pParam)->fPercent);
		break;
	case CTriggerData::c_random:
		result.Format(strConditonSign[temp->mConditon.iType],((C_RANDOM*)temp->mConditon.pParam)->fProbability);
		break;
	case CTriggerData::c_var:
		result.Format(strConditonSign[temp->mConditon.iType],((C_VAR*)temp->mConditon.pParam)->iID);
		break;
	case CTriggerData::c_constant:
		result.Format(strConditonSign[temp->mConditon.iType],((C_CONSTANT*)temp->mConditon.pParam)->iValue);
		break;
	case CTriggerData::c_reach_end:
		result.Format(strConditonSign[temp->mConditon.iType],((C_REACH_END*)temp->mConditon.pParam)->iPathID);
		break;
	case CTriggerData::c_at_history_stage:
		result.Format(strConditonSign[temp->mConditon.iType],((C_HISTORY_STAGE*)temp->mConditon.pParam)->iID);
		break;
	case CTriggerData::c_history_value:
		result.Format(strConditonSign[temp->mConditon.iType],((C_HISTORY_VALUE*)temp->mConditon.pParam)->iValue);
		break;
	case CTriggerData::c_local_var:
		result.Format(strConditonSign[temp->mConditon.iType],((C_LOCAL_VAR*)temp->mConditon.pParam)->iID);
		break;
	case CTriggerData::c_reach_end_2:
		result.Format(strConditonSign[temp->mConditon.iType],((C_REACH_END_2*)temp->mConditon.pParam)->iPathID,GetVarTypeName(((C_REACH_END_2*)temp->mConditon.pParam)->iPathIDType));
		break;
	case CTriggerData::c_has_filter:
		result.Format(strConditonSign[temp->mConditon.iType],((C_HAS_FILTER*)temp->mConditon.pParam)->iID);
		break;
	default:
		result = strConditonSign[temp->mConditon.iType];
	}
	return result;
}

/////////////////////////////////////////////////////////////////////////////
// CConditionDlg dialog

CConditionDlg::CConditionDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CConditionDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CConditionDlg)
	m_fConditionParam = 0.1f;
	m_iConditionParam = 0;
	m_strLeft = _T("");
	m_strRight = _T("");
	//}}AFX_DATA_INIT
	m_uConditionFunc = 0;
	m_pRoot = NULL;
	m_bIsRoot = false;
	bModify = false;
}


void CConditionDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CConditionDlg)
	DDX_Control(pDX, IDC_COMBO_FUNC, m_cbFunc);
	DDX_Text(pDX, IDC_EDIT_PARAM_FLOAT, m_fConditionParam);
	DDX_Text(pDX, IDC_EDIT_PARAM_INT, m_iConditionParam);
	DDX_Text(pDX, IDC_EDIT_SUB1, m_strLeft);
	DDX_Text(pDX, IDC_EDIT_SUB2, m_strRight);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CConditionDlg, CDialog)
	//{{AFX_MSG_MAP(CConditionDlg)
	ON_CBN_SELCHANGE(IDC_COMBO_FUNC, OnSelchangeComboFunc)
	ON_BN_CLICKED(IDC_BUTTON_SUB1, OnButtonSub1)
	ON_BN_CLICKED(IDC_BUTTON_SUB2, OnButtonSub2)
	ON_EN_CHANGE(IDC_EDIT_PARAM_FLOAT, OnChangeEditParamFloat)
	ON_EN_CHANGE(IDC_EDIT_PARAM_INT, OnChangeEditParamInt)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CConditionDlg message handlers

void CConditionDlg::OnSelchangeComboFunc() 
{
	// TODO: Add your control notification handler code here
	FreshInterface();
}

void CConditionDlg::FreshInterface()
{
	CWnd *pParamI = GetDlgItem(IDC_EDIT_PARAM_INT);
	CWnd *pParamF  = GetDlgItem(IDC_EDIT_PARAM_FLOAT);
	CWnd *pButton1 = GetDlgItem(IDC_BUTTON_SUB1);
	CWnd *pButton2 = GetDlgItem(IDC_BUTTON_SUB2);
	pButton1->EnableWindow(false);
	pButton2->EnableWindow(false);

	m_iConditionParam = 0;
	m_fConditionParam = 0.1f;
	int sel = m_cbFunc.GetCurSel();
	if(sel==-1)
	{
		pParamI->ShowWindow(SW_HIDE);
		pParamF->ShowWindow(SW_HIDE);
	}else
	{
		m_uConditionFunc = sel;
		switch(sel) 
		{
		case CTriggerData::c_plus:
			pParamI->ShowWindow(SW_HIDE);
			pParamF->ShowWindow(SW_HIDE);
			pButton1->EnableWindow(true);
			pButton2->EnableWindow(true);
			break;
		case CTriggerData::c_minus:
			pParamI->ShowWindow(SW_HIDE);
			pParamF->ShowWindow(SW_HIDE);
			pButton1->EnableWindow(true);
			pButton2->EnableWindow(true);
			break;
		case CTriggerData::c_multiply:
			pParamI->ShowWindow(SW_HIDE);
			pParamF->ShowWindow(SW_HIDE);
			pButton1->EnableWindow(true);
			pButton2->EnableWindow(true);
			break;
		case CTriggerData::c_divide:
			pParamI->ShowWindow(SW_HIDE);
			pParamF->ShowWindow(SW_HIDE);
			pButton1->EnableWindow(true);
			pButton2->EnableWindow(true);
			break;
		case CTriggerData::c_great:
			pParamI->ShowWindow(SW_HIDE);
			pParamF->ShowWindow(SW_HIDE);
			pButton1->EnableWindow(true);
			pButton2->EnableWindow(true);
			break;
		case CTriggerData::c_less:
			pParamI->ShowWindow(SW_HIDE);
			pParamF->ShowWindow(SW_HIDE);
			pButton1->EnableWindow(true);
			pButton2->EnableWindow(true);
			break;
		case CTriggerData::c_equ:
			pParamI->ShowWindow(SW_HIDE);
			pParamF->ShowWindow(SW_HIDE);
			pButton1->EnableWindow(true);
			pButton2->EnableWindow(true);
			break;
		case CTriggerData::c_var:
			pParamI->ShowWindow(SW_SHOW);
			pParamF->ShowWindow(SW_HIDE);
			pButton1->EnableWindow(false);
			pButton2->EnableWindow(false);
			break;
		case CTriggerData::c_constant:
			pParamI->ShowWindow(SW_SHOW);
			pParamF->ShowWindow(SW_HIDE);
			pButton1->EnableWindow(false);
			pButton2->EnableWindow(false);
			break;
		case CTriggerData::c_and:
			pParamI->ShowWindow(SW_HIDE);
			pParamF->ShowWindow(SW_HIDE);
			pButton1->EnableWindow(true);
			pButton2->EnableWindow(true);
			break;
		case CTriggerData::c_not:
			pParamI->ShowWindow(SW_HIDE);
			pParamF->ShowWindow(SW_HIDE);
			pButton2->EnableWindow(true);
			break;
		case CTriggerData::c_or:
			pParamI->ShowWindow(SW_HIDE);
			pParamF->ShowWindow(SW_HIDE);
			pButton1->EnableWindow(true);
			pButton2->EnableWindow(true);
			break;
		case CTriggerData::c_hp_less:
			pParamI->ShowWindow(SW_HIDE);
			pParamF->ShowWindow(SW_SHOW);
			if(m_pRoot->mConditon.pParam && m_pRoot->mConditon.iType == CTriggerData::c_hp_less)
				m_fConditionParam = ((C_HP_LESS*)m_pRoot->mConditon.pParam)->fPercent;
			break;
		case CTriggerData::c_kill_player:
			pParamI->ShowWindow(SW_HIDE);
			pParamF->ShowWindow(SW_HIDE);
			break;
		case CTriggerData::c_died:
			pParamI->ShowWindow(SW_HIDE);
			pParamF->ShowWindow(SW_HIDE);
			break;
		case CTriggerData::c_random:
			pParamI->ShowWindow(SW_HIDE);
			pParamF->ShowWindow(SW_SHOW);
			if(m_pRoot->mConditon.pParam && m_pRoot->mConditon.iType == CTriggerData::c_random)
				m_fConditionParam = ((C_RANDOM*)m_pRoot->mConditon.pParam)->fProbability;
			break;
		case CTriggerData::c_start_attack:
			pParamI->ShowWindow(SW_HIDE);
			pParamF->ShowWindow(SW_HIDE);
			break;
		case CTriggerData::c_time_come:
			pParamI->ShowWindow(SW_SHOW);
			pParamF->ShowWindow(SW_HIDE);
			if(m_pRoot->mConditon.pParam && m_pRoot->mConditon.iType == CTriggerData::c_time_come)
				m_iConditionParam = ((C_TIME_COME*)m_pRoot->mConditon.pParam)->uID;
			break;
// 		case CTriggerData::c_time_point:
// 			pParamI->ShowWindow(SW_SHOW);
// 			pParamF->ShowWindow(SW_SHOW);
// 			if(m_pRoot->mConditon.pParam && m_pRoot->mConditon.iType == CTriggerData::c_time_point)
// 			{
// 				m_iConditionParam = ((C_TIME_POINT*)m_pRoot->mConditon.pParam)->uHour;
// 				m_fConditionParam = ((C_TIME_POINT*)m_pRoot->mConditon.pParam)->uMinute;
// 			}else
// 			{
// 				m_fConditionParam = 0;
// 			}
// 			break;
		case CTriggerData::c_be_hurt:
			pParamI->ShowWindow(SW_SHOW);
			pParamF->ShowWindow(SW_SHOW);
			if(m_pRoot->mConditon.pParam && m_pRoot->mConditon.iType == CTriggerData::c_be_hurt)
			{
				m_iConditionParam = ((C_BE_HURT*)m_pRoot->mConditon.pParam)->iHurtLow;
				m_fConditionParam = ((C_BE_HURT*)m_pRoot->mConditon.pParam)->iHurtHigh;
			}else
			{
				m_fConditionParam = 0;
			}
 			break;

		case CTriggerData::c_reach_end:
			pParamI->ShowWindow(SW_SHOW);
			pParamF->ShowWindow(SW_HIDE);
			if(m_pRoot->mConditon.pParam && m_pRoot->mConditon.iType == CTriggerData::c_reach_end)
				m_iConditionParam = ((C_REACH_END*)m_pRoot->mConditon.pParam)->iPathID;
			break;
			
		case CTriggerData::c_at_history_stage:
			pParamI->ShowWindow(SW_SHOW);
			pParamF->ShowWindow(SW_HIDE);
			if(m_pRoot->mConditon.pParam && m_pRoot->mConditon.iType == sel)
				m_iConditionParam = ((C_HISTORY_STAGE*)m_pRoot->mConditon.pParam)->iID;
			break;
			
		case CTriggerData::c_history_value:
			pParamI->ShowWindow(SW_SHOW);
			pParamF->ShowWindow(SW_HIDE);
			if(m_pRoot->mConditon.pParam && m_pRoot->mConditon.iType == sel)
				m_iConditionParam = ((C_HISTORY_VALUE*)m_pRoot->mConditon.pParam)->iValue;
			break;
		case CTriggerData::c_stop_fight:
			pParamI->ShowWindow(SW_HIDE);
			pParamF->ShowWindow(SW_HIDE);
			break;
		case CTriggerData::c_local_var:
			pParamI->ShowWindow(SW_SHOW);
			pParamF->ShowWindow(SW_HIDE);
			pButton1->EnableWindow(false);
			pButton2->EnableWindow(false);
			break;
		case CTriggerData::c_reach_end_2:
			pParamI->ShowWindow(SW_SHOW);
			pParamF->ShowWindow(SW_SHOW);
			if(m_pRoot->mConditon.pParam && m_pRoot->mConditon.iType == CTriggerData::c_reach_end_2)
			{
				m_iConditionParam = ((C_REACH_END_2*)m_pRoot->mConditon.pParam)->iPathID;
				m_fConditionParam = ((C_REACH_END_2*)m_pRoot->mConditon.pParam)->iPathIDType;
			}else
			{
				m_fConditionParam = 0;
			}
			break;
		case CTriggerData::c_has_filter:
			ShowIntParam(IsCondition(CTriggerData::c_has_filter) ? ParamAs(const C_HAS_FILTER *)->iID : 0);
			HideFloatParam();
			break;
		case CTriggerData::c_room_index:
			HideIntParam();
			HideFloatParam();
			break;
		}
		UpdateData(false);
	}
}

bool CConditionDlg::IsCondition(CTriggerData::_e_condition c)const{
	return m_pRoot->mConditon.pParam && m_pRoot->mConditon.iType == c;
}

void CConditionDlg::ShowIntParam(int valueOnShow){
	GetDlgItem(IDC_EDIT_PARAM_INT)->ShowWindow(SW_SHOW);
	m_iConditionParam = valueOnShow;
}

void CConditionDlg::HideIntParam(){
	GetDlgItem(IDC_EDIT_PARAM_INT)->ShowWindow(SW_HIDE);
	m_iConditionParam = 0;
}

void CConditionDlg::ShowFloatParam(float valueOnShow){
	GetDlgItem(IDC_EDIT_PARAM_FLOAT)->ShowWindow(SW_SHOW);
	m_fConditionParam = valueOnShow;
}

void CConditionDlg::HideFloatParam(){
	GetDlgItem(IDC_EDIT_PARAM_FLOAT)->ShowWindow(SW_HIDE);
	m_fConditionParam = 0.0f;
}

BOOL CConditionDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	for( int i = 0; i < CTriggerData::c_num; i++)
	{
		m_cbFunc.InsertString(i,strConditonList[i]);
	}
	m_cbFunc.SetCurSel(m_pRoot->mConditon.iType);
	FreshInterface();
	
	if(m_pRoot)
	{	
		m_strLeft = TraverselTree(m_pRoot->pLeft);
		m_strRight = TraverselTree(m_pRoot->pRight);
	}
	
	CRect rc;
	GetWindowRect(&rc);
	rc.left +=20;
	rc.top  += 20;
	rc.right += 20;
	rc.bottom +=20;
	MoveWindow(rc);
	OnSelchangeComboFunc();

	UpdateData(false);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CConditionDlg::DisableConditon2(bool bDisable)
{
}

void CConditionDlg::OnOK()
{
	UpdateData(true);
	switch(m_uConditionFunc) 
	{
	case CTriggerData::c_plus:
	case CTriggerData::c_minus:
	case CTriggerData::c_multiply:
	case CTriggerData::c_divide:
	case CTriggerData::c_great:
	case CTriggerData::c_less:
	case CTriggerData::c_equ:
	case CTriggerData::c_and:
	case CTriggerData::c_or:
		if(m_pRoot->pLeft==0) 
		{
			MessageBox("条件1不能为空!");
			return;
		}
		if(m_pRoot->pRight==0) 
		{
			MessageBox("条件2不能为空!");
			return;
		}

		if(!Check(m_pRoot->pLeft,m_pRoot->pRight))
		{
			MessageBox("逻辑错误!");
			return;
		}

		if(m_pRoot->mConditon.pParam) free(m_pRoot->mConditon.pParam);
		m_pRoot->mConditon.pParam = NULL;
		break;
	case CTriggerData::c_not:
		if(m_pRoot->pLeft) delete m_pRoot->pLeft;
		m_pRoot->pLeft = 0;
		if(m_pRoot->pRight==0) 
		{
			MessageBox("条件2不能为空!");
			return;
		}
		if(!Check(m_pRoot->pLeft,m_pRoot->pRight))
		{
			MessageBox("逻辑错误!");
			return;
		}
		if(m_pRoot->mConditon.pParam) free(m_pRoot->mConditon.pParam);
		m_pRoot->mConditon.pParam = NULL;
		break;
	case CTriggerData::c_hp_less:
		if(m_pRoot->mConditon.pParam) free(m_pRoot->mConditon.pParam);
		m_pRoot->mConditon.pParam = NULL;
		if(m_pRoot->pLeft) delete m_pRoot->pLeft;
		m_pRoot->pLeft = 0;
		if(m_pRoot->pRight) delete m_pRoot->pRight;
		m_pRoot->pRight = 0;
		m_pRoot->mConditon.pParam = malloc(sizeof(C_HP_LESS));
		((C_HP_LESS*)m_pRoot->mConditon.pParam)->fPercent = m_fConditionParam;
		break;
	case CTriggerData::c_var:
		if(m_pRoot->mConditon.pParam) free(m_pRoot->mConditon.pParam);
		m_pRoot->mConditon.pParam = NULL;
		if(m_pRoot->pLeft) delete m_pRoot->pLeft;
		m_pRoot->pLeft = 0;
		if(m_pRoot->pRight) delete m_pRoot->pRight;
		m_pRoot->pRight = 0;
		m_pRoot->mConditon.pParam = malloc(sizeof(C_VAR));
		((C_VAR*)m_pRoot->mConditon.pParam)->iID = m_iConditionParam;
		break;
	case CTriggerData::c_constant:
		if(m_pRoot->mConditon.pParam) free(m_pRoot->mConditon.pParam);
		m_pRoot->mConditon.pParam = NULL;
		if(m_pRoot->pLeft) delete m_pRoot->pLeft;
		m_pRoot->pLeft = 0;
		if(m_pRoot->pRight) delete m_pRoot->pRight;
		m_pRoot->pRight = 0;
		m_pRoot->mConditon.pParam = malloc(sizeof(C_CONSTANT));
		((C_CONSTANT*)m_pRoot->mConditon.pParam)->iValue = m_iConditionParam;
		break;
	case CTriggerData::c_kill_player:
		if(m_pRoot->mConditon.pParam) free(m_pRoot->mConditon.pParam);
		m_pRoot->mConditon.pParam = NULL;
		if(m_pRoot->pLeft) delete m_pRoot->pLeft;
		m_pRoot->pLeft = 0;
		if(m_pRoot->pRight) delete m_pRoot->pRight;
		m_pRoot->pRight = 0;
		break;
	case CTriggerData::c_died:
		if(m_pRoot->mConditon.pParam) free(m_pRoot->mConditon.pParam);
		m_pRoot->mConditon.pParam = NULL;
		if(m_pRoot->pLeft) delete m_pRoot->pLeft;
		m_pRoot->pLeft = 0;
		if(m_pRoot->pRight) delete m_pRoot->pRight;
		m_pRoot->pRight = 0;
		break;
	case CTriggerData::c_random:
		if(m_pRoot->mConditon.pParam) free(m_pRoot->mConditon.pParam);
		m_pRoot->mConditon.pParam = NULL;
		if(m_pRoot->pLeft) delete m_pRoot->pLeft;
		m_pRoot->pLeft = 0;
		if(m_pRoot->pRight) delete m_pRoot->pRight;
		m_pRoot->pRight = 0;
		m_pRoot->mConditon.pParam = malloc(sizeof(C_RANDOM));
		((C_RANDOM*)m_pRoot->mConditon.pParam)->fProbability = m_fConditionParam;
		break;
	case CTriggerData::c_start_attack:
		if(m_pRoot->mConditon.pParam) free(m_pRoot->mConditon.pParam);
		m_pRoot->mConditon.pParam = NULL;
		if(m_pRoot->pLeft) delete m_pRoot->pLeft;
		m_pRoot->pLeft = 0;
		if(m_pRoot->pRight) delete m_pRoot->pRight;
		m_pRoot->pRight = 0;
		break;
	case CTriggerData::c_time_come:
		if(m_pRoot->mConditon.pParam) free(m_pRoot->mConditon.pParam);
		m_pRoot->mConditon.pParam = NULL;
		if(m_pRoot->pLeft) delete m_pRoot->pLeft;
		m_pRoot->pLeft = 0;
		if(m_pRoot->pRight) delete m_pRoot->pRight;
		m_pRoot->pRight = 0;
		m_pRoot->mConditon.pParam = malloc(sizeof(C_TIME_COME));
		((C_TIME_COME*)m_pRoot->mConditon.pParam)->uID = m_iConditionParam;
		break;
// 	case CTriggerData::c_time_point:
// 		if(m_pRoot->mConditon.pParam) free(m_pRoot->mConditon.pParam);
// 		m_pRoot->mConditon.pParam = NULL;
// 		if(m_pRoot->pLeft) delete m_pRoot->pLeft;
// 		m_pRoot->pLeft = 0;
// 		if(m_pRoot->pRight) delete m_pRoot->pRight;
// 		m_pRoot->pRight = 0;
// 		m_pRoot->mConditon.pParam = malloc(sizeof(C_TIME_POINT));
// 		((C_TIME_POINT*)m_pRoot->mConditon.pParam)->uHour = m_iConditionParam;
// 		((C_TIME_POINT*)m_pRoot->mConditon.pParam)->uMinute = (unsigned int)m_fConditionParam;
// 
// 		if(((C_TIME_POINT*)m_pRoot->mConditon.pParam)->uHour > 23) ((C_TIME_POINT*)m_pRoot->mConditon.pParam)->uHour = 23;
// 		if(((C_TIME_POINT*)m_pRoot->mConditon.pParam)->uMinute > 59) ((C_TIME_POINT*)m_pRoot->mConditon.pParam)->uHour = 59;
// 		break; 	
	case CTriggerData::c_be_hurt:
		if(m_pRoot->mConditon.pParam) free(m_pRoot->mConditon.pParam);
		m_pRoot->mConditon.pParam = NULL;
		if(m_pRoot->pLeft) delete m_pRoot->pLeft;
		m_pRoot->pLeft = 0;
		if(m_pRoot->pRight) delete m_pRoot->pRight;
		m_pRoot->pRight = 0;
		m_pRoot->mConditon.pParam = malloc(sizeof(C_BE_HURT));
		((C_BE_HURT*)m_pRoot->mConditon.pParam)->iHurtLow  = m_iConditionParam;
		((C_BE_HURT*)m_pRoot->mConditon.pParam)->iHurtHigh = (int)m_fConditionParam;
		break;
	case CTriggerData::c_reach_end:
		if(m_pRoot->mConditon.pParam) free(m_pRoot->mConditon.pParam);
		m_pRoot->mConditon.pParam = NULL;
		if(m_pRoot->pLeft) delete m_pRoot->pLeft;
		m_pRoot->pLeft = 0;
		if(m_pRoot->pRight) delete m_pRoot->pRight;
		m_pRoot->pRight = 0;
		m_pRoot->mConditon.pParam = malloc(sizeof(C_REACH_END));
		((C_REACH_END*)m_pRoot->mConditon.pParam)->iPathID = m_iConditionParam;
		break;
	case CTriggerData::c_at_history_stage:
		if(m_pRoot->mConditon.pParam) free(m_pRoot->mConditon.pParam);
		m_pRoot->mConditon.pParam = NULL;
		if(m_pRoot->pLeft) delete m_pRoot->pLeft;
		m_pRoot->pLeft = 0;
		if(m_pRoot->pRight) delete m_pRoot->pRight;
		m_pRoot->pRight = 0;
		m_pRoot->mConditon.pParam = malloc(sizeof(C_HISTORY_STAGE));
		((C_HISTORY_STAGE*)m_pRoot->mConditon.pParam)->iID = m_iConditionParam;
		break;
	case CTriggerData::c_history_value:
		if(m_pRoot->mConditon.pParam) free(m_pRoot->mConditon.pParam);
		m_pRoot->mConditon.pParam = NULL;
		if(m_pRoot->pLeft) delete m_pRoot->pLeft;
		m_pRoot->pLeft = 0;
		if(m_pRoot->pRight) delete m_pRoot->pRight;
		m_pRoot->pRight = 0;
		m_pRoot->mConditon.pParam = malloc(sizeof(C_HISTORY_VALUE));
		((C_HISTORY_VALUE*)m_pRoot->mConditon.pParam)->iValue = m_iConditionParam;
		break;
	case CTriggerData::c_stop_fight:
		if(m_pRoot->mConditon.pParam) free(m_pRoot->mConditon.pParam);
		m_pRoot->mConditon.pParam = NULL;
		if(m_pRoot->pLeft) delete m_pRoot->pLeft;
		m_pRoot->pLeft = 0;
		if(m_pRoot->pRight) delete m_pRoot->pRight;
		m_pRoot->pRight = 0;
		break;
	case CTriggerData::c_local_var:
		if(m_pRoot->mConditon.pParam) free(m_pRoot->mConditon.pParam);
		m_pRoot->mConditon.pParam = NULL;
		if(m_pRoot->pLeft) delete m_pRoot->pLeft;
		m_pRoot->pLeft = 0;
		if(m_pRoot->pRight) delete m_pRoot->pRight;
		m_pRoot->pRight = 0;
		m_pRoot->mConditon.pParam = malloc(sizeof(C_LOCAL_VAR));
		((C_LOCAL_VAR*)m_pRoot->mConditon.pParam)->iID = m_iConditionParam;
		break;
	case CTriggerData::c_reach_end_2:
		{
			int type = (int)m_fConditionParam;
			if (type < (int)enumPVTGlobalVarID || type > enumPVTConst){
				CString strMsg;
				strMsg.Format("类型参数越界(%d~%d)", enumPVTGlobalVarID, enumPVTConst);
				MessageBox(strMsg);
				return;
			}
		}
		if(m_pRoot->mConditon.pParam) free(m_pRoot->mConditon.pParam);
		m_pRoot->mConditon.pParam = NULL;
		if(m_pRoot->pLeft) delete m_pRoot->pLeft;
		m_pRoot->pLeft = 0;
		if(m_pRoot->pRight) delete m_pRoot->pRight;
		m_pRoot->pRight = 0;
		m_pRoot->mConditon.pParam = malloc(sizeof(C_REACH_END_2));
		((C_REACH_END_2*)m_pRoot->mConditon.pParam)->iPathID  = m_iConditionParam;
		((C_REACH_END_2*)m_pRoot->mConditon.pParam)->iPathIDType = (int)m_fConditionParam;
		break;
	case CTriggerData::c_has_filter:
		FreeTree();
		AllocParameter(sizeof(C_HAS_FILTER));
		ParamAs(C_HAS_FILTER *)->iID = m_iConditionParam;
		break;
	case CTriggerData::c_room_index:
		FreeTree();
		break;
	default:
		FreeTree();
		break;
	}

	if(m_bIsRoot) 
	{
		if(m_uConditionFunc == CTriggerData::c_plus||
			m_uConditionFunc == CTriggerData::c_minus||
			m_uConditionFunc == CTriggerData::c_multiply||
			m_uConditionFunc == CTriggerData::c_divide||
			m_uConditionFunc == CTriggerData::c_var||
			m_uConditionFunc == CTriggerData::c_constant ||
			m_uConditionFunc == CTriggerData::c_history_value ||
			m_uConditionFunc == CTriggerData::c_local_var ||
			m_uConditionFunc == CTriggerData::c_room_index)
		{
			MessageBox("逻辑错误!");
			return;
		}
	}

	m_pRoot->mConditon.iType = m_uConditionFunc;
	
	CDialog::OnOK();
}

void CConditionDlg::FreeTree(){
	m_pRoot->Free();
}

void CConditionDlg::AllocParameter(size_t size){
	m_pRoot->mConditon.pParam = malloc(size);
}

void CConditionDlg::OnCancel()
{
	CDialog::OnCancel();
}

bool CConditionDlg::Check(CTriggerData::_s_tree_item* left, CTriggerData::_s_tree_item* right)
{
	if(m_uConditionFunc == CTriggerData::c_plus ||
	   m_uConditionFunc == CTriggerData::c_minus ||
	   m_uConditionFunc == CTriggerData::c_multiply ||
	   m_uConditionFunc == CTriggerData::c_divide ||
	   m_uConditionFunc == CTriggerData::c_great ||
	   m_uConditionFunc == CTriggerData::c_less ||
	   m_uConditionFunc == CTriggerData::c_equ)
	   {
			if(left->mConditon.iType == CTriggerData::c_plus || 
				left->mConditon.iType == CTriggerData::c_minus || 
				left->mConditon.iType == CTriggerData::c_multiply || 
				left->mConditon.iType == CTriggerData::c_divide ||
				left->mConditon.iType == CTriggerData::c_var || 
				left->mConditon.iType == CTriggerData::c_constant ||
				left->mConditon.iType == CTriggerData::c_history_value ||
				left->mConditon.iType == CTriggerData::c_local_var ||
				left->mConditon.iType == CTriggerData::c_room_index)
			{
				if(right->mConditon.iType == CTriggerData::c_plus || 
				right->mConditon.iType == CTriggerData::c_minus || 
				right->mConditon.iType == CTriggerData::c_multiply || 
				right->mConditon.iType == CTriggerData::c_divide ||
				right->mConditon.iType == CTriggerData::c_var || 
				right->mConditon.iType == CTriggerData::c_constant ||
				right->mConditon.iType == CTriggerData::c_history_value ||
				right->mConditon.iType == CTriggerData::c_local_var ||
				right->mConditon.iType == CTriggerData::c_room_index)
					return true;
			}
	   }else if( m_uConditionFunc == CTriggerData::c_or ||
				 m_uConditionFunc == CTriggerData::c_and)
	   {
			if(left->mConditon.iType == CTriggerData::c_plus || 
				left->mConditon.iType == CTriggerData::c_minus || 
				left->mConditon.iType == CTriggerData::c_multiply || 
				left->mConditon.iType == CTriggerData::c_divide ||
				left->mConditon.iType == CTriggerData::c_var || 
				left->mConditon.iType == CTriggerData::c_constant ||
				left->mConditon.iType == CTriggerData::c_history_value ||
				left->mConditon.iType == CTriggerData::c_local_var ||
				left->mConditon.iType == CTriggerData::c_room_index)
					return false;
			
				if(right->mConditon.iType == CTriggerData::c_plus || 
				right->mConditon.iType == CTriggerData::c_minus || 
				right->mConditon.iType == CTriggerData::c_multiply || 
				right->mConditon.iType == CTriggerData::c_divide ||
				right->mConditon.iType == CTriggerData::c_var || 
				right->mConditon.iType == CTriggerData::c_constant ||
				right->mConditon.iType == CTriggerData::c_history_value ||
				right->mConditon.iType == CTriggerData::c_local_var ||
				right->mConditon.iType == CTriggerData::c_room_index)
					return false;

				return true;
	   }else if(m_uConditionFunc == CTriggerData::c_not)
	   {
		   if(right->mConditon.iType == CTriggerData::c_plus || 
			   right->mConditon.iType == CTriggerData::c_minus || 
			   right->mConditon.iType == CTriggerData::c_multiply || 
			   right->mConditon.iType == CTriggerData::c_divide ||
			   right->mConditon.iType == CTriggerData::c_var || 
			   right->mConditon.iType == CTriggerData::c_constant ||
			   right->mConditon.iType == CTriggerData::c_history_value ||
			   right->mConditon.iType == CTriggerData::c_local_var ||
			   right->mConditon.iType == CTriggerData::c_room_index)
			   return false;
		   
		   return true;
	   }

	return false;
}




void CConditionDlg::OnButtonSub1() 
{
	// TODO: Add your control notification handler code here
	CConditionDlg dlg;
	CTriggerData::_s_tree_item *pOld = CTriggerData::CopyConditonTree(m_pRoot->pLeft);
	if(m_pRoot->pLeft==0) m_pRoot->pLeft = new CTriggerData::_s_tree_item;
	dlg.m_pRoot = m_pRoot->pLeft;
	if(IDOK==dlg.DoModal())
	{
		if(pOld) delete pOld;
	}else
	{
		if(m_pRoot->pLeft) delete m_pRoot->pLeft;
		m_pRoot->pLeft = pOld; 
	}
	m_strLeft = TraverselTree(m_pRoot->pLeft);
	m_strRight = TraverselTree(m_pRoot->pRight);
	UpdateData(false);
}

void CConditionDlg::OnButtonSub2() 
{
	CConditionDlg dlg;
	// TODO: Add your control notification handler code here
	CTriggerData::_s_tree_item *pOld = CTriggerData::CopyConditonTree(m_pRoot->pRight);
	if(m_pRoot->pRight==0) m_pRoot->pRight = new CTriggerData::_s_tree_item;
	dlg.m_pRoot = m_pRoot->pRight;
	
	dlg.bModify = bModify;
	if(IDOK==dlg.DoModal())
	{
		if(pOld) delete pOld;
	}else
	{
		if(m_pRoot->pRight) delete m_pRoot->pRight;
		m_pRoot->pRight = pOld; 
	}
	m_strLeft = TraverselTree(m_pRoot->pLeft);
	m_strRight = TraverselTree(m_pRoot->pRight);
	UpdateData(false);
}

CString CConditionDlg::TraverselTree( void *pTree)
{
	if(pTree==0) return "";
	CTriggerData::_s_tree_item * temp = (CTriggerData::_s_tree_item*)pTree;
	CString str;	
	str +="(";
	if(temp->pLeft) str += TraverselTree(temp->pLeft);
	str += GetConditionSign(temp);
	if(temp->pRight) str += TraverselTree(temp->pRight);
	str +=")";
	return str;
}

void CConditionDlg::OnChangeEditParamFloat() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	UpdateData(true);
}

void CConditionDlg::OnChangeEditParamInt() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	UpdateData(true);
}
