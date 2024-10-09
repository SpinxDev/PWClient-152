// AwardDlg.cpp : implementation file
//

#include "stdafx.h"
#include "TaskTemplEditor.h"
#include "TaskTempl.h"
#include "AwardDlg.h"
#include "TaskIDSelDlg.h"
#include "ItemsWantedDlg.h"
#include "ItemsCandDlg.h"
#include "MonsterSummonedDlg.h"
#include "VectorDlg.h"
#include "AwardChangeValue.h"
#include "AwardDisplayValue.h"
#include "NumDlg.h"
#include "BaseDataIDMan.h"
#include "AwardRanking.h"
#include "TaskTitle.h"
#include "TaskTemplEditorView.h"

#ifdef _DEBUG
#define new A_DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern unsigned long period_map[];
extern int find_period_index(unsigned long ulPeriod);

/////////////////////////////////////////////////////////////////////////////
// CAwardDlg dialog

CAwardDlg::CAwardDlg(AWARD_DATA* pAward, CWnd* pParent /*=NULL*/)
	: m_pAward(pAward), CDialog(CAwardDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CAwardDlg)
	m_dwExp = 0;
	m_dwGold = 0;
	m_strNewTask = _T("");
	m_lRepu = 0;
	m_dwSP = 0;
	m_iFactionContrib = 0;
	m_iFactionExpContrib = 0;
	m_nPeriod = 0;
	m_dwStation = 0;
	m_dwStorehouse = 0;
	m_dwFury = 0;
	m_dwTransWld = 0;
	m_dwPetNum = 0;
	m_lMonsCtrl = 0;
	m_bTrigCtrl = FALSE;
	m_lInventorySize = 0;
	m_bUseLevCo = FALSE;
	m_bDivorce = FALSE;
	m_bSendMsg = FALSE;
	m_nMsgChannel = 0;
	m_dwStorehouse2 = 0;
	m_dwStorehouse3 = 0;
	m_bMulti = FALSE;
	m_nNumType = 0;
	m_lNum = 0;
	m_bDeath = FALSE;
	m_nDeathType = 0;
	m_lSpecifyTaskID = 0;
	m_lSpecifySubTaskID = 0;
	m_lSpecifyTaskContrib = 0;
	m_lRandContrib = 0;
	m_lLowestContrib = 0;
	m_iContrib = 0;
	m_dwStorehouse4 = 0;
	m_lDividend = 0;

	m_bAwardSkill = FALSE;
	m_iAwardSkillID = 0;
	m_iAwardSkillLevel = 0;

	m_iForceActivity = 0;
	m_iForceContrib = 0;
	m_iForceRepu = 0;
	m_iForceSetRepu = 0;

	m_bTaskLimit = 0;
	m_iRealmExp = 0;
	m_bExpandRealmLevelMax = FALSE;
	m_iLeaderShip = 0;
	m_iWorldContribution = 0;

	//}}AFX_DATA_INIT
}


void CAwardDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAwardDlg)
	DDX_Text(pDX, IDC_EXP_NUM, m_dwExp);
	DDX_Text(pDX, IDC_GOLD_NUM, m_dwGold);
	DDX_Text(pDX, IDC_NEW_TASK, m_strNewTask);
	DDX_Text(pDX, IDC_REPU_NUM, m_lRepu);
	DDX_Text(pDX, IDC_SP_NUM, m_dwSP);
	DDX_Text(pDX, IDC_AW_FACTION_CONTRIB, m_iFactionContrib);
	DDX_Text(pDX, IDC_AW_FACTION_EXP_CONTRIB, m_iFactionExpContrib);
	DDX_CBIndex(pDX, IDC_PERIOD, m_nPeriod);
	DDX_Text(pDX, IDC_STATION, m_dwStation);
	DDX_Text(pDX, IDC_STOREHOUSE, m_dwStorehouse);
	DDX_Text(pDX, IDC_FURY, m_dwFury);
	DDX_Text(pDX, IDC_TRANS_WLD, m_dwTransWld);
	DDX_Text(pDX, IDC_PET_NUM, m_dwPetNum);
	DDX_Text(pDX, IDC_CONTROLLER, m_lMonsCtrl);
	DDX_Check(pDX, IDC_TRIG_CTRL, m_bTrigCtrl);
	DDX_Text(pDX, IDC_INVENTORY, m_lInventorySize);
	DDX_Check(pDX, IDC_USE_LEV_CO, m_bUseLevCo);
	DDX_Check(pDX, IDC_DIVORCE, m_bDivorce);
	DDX_Check(pDX, IDC_SEND_MSG, m_bSendMsg);
	DDX_CBIndex(pDX, IDC_MSG_CHANNEL, m_nMsgChannel);
	DDX_Text(pDX, IDC_STOREHOUSE2, m_dwStorehouse2);
	DDX_Text(pDX, IDC_STOREHOUSE3, m_dwStorehouse3);
	DDX_Check(pDX, IDC_AWARD_NEED_MULTI, m_bMulti);
	DDX_CBIndex(pDX, IDC_AWARD_NUM_TYPE, m_nNumType);
	DDX_Text(pDX, IDC_AWARD_NUM_VAL, m_lNum);
	DDX_Check(pDX, IDC_AWARD_DEATH, m_bDeath);
	DDX_CBIndex(pDX, IDC_AWARD_DEATH_TYPE, m_nDeathType);
	DDX_Text(pDX, IDC_PQ_SPECIFY_TASKID, m_lSpecifyTaskID);
	DDX_Text(pDX, IDC_PQ_SPECIFY_SUBTASKID, m_lSpecifySubTaskID);
	DDX_Text(pDX, IDC_PQ_SPECIFY_TASK_CONTRIB, m_lSpecifyTaskContrib);
	DDX_Text(pDX, IDC_PQ_RANDOM_CONTRIB, m_lRandContrib);
	DDX_Text(pDX, IDC_PQ_LOWEST_CONTRIB, m_lLowestContrib);
	DDX_Text(pDX, IDC_PQ_CONTRIB, m_iContrib);
	DDX_Text(pDX, IDC_STOREHOUSE4, m_dwStorehouse4);
	DDV_MinMaxDWord(pDX, m_dwStorehouse4, 0, 80);
	DDX_Text(pDX, IDC_DIVIDEND, m_lDividend);
	
	DDX_Check(pDX,IDC_AWARD_DELIVERYSKILL,m_bAwardSkill);
	DDX_Text(pDX,IDC_AWARDSKILLID,m_iAwardSkillID);
	DDX_Text(pDX,IDC_AWARDSKILLLEVEL,m_iAwardSkillLevel);

	DDX_Text(pDX,IDC_FORCE_ACTIVITY,m_iForceActivity);
	DDX_Text(pDX,IDC_FORCE_CONTRIB,m_iForceContrib);
	DDX_Text(pDX,IDC_FORCE_REPU,m_iForceRepu);
	DDX_Text(pDX,IDC_FORCE_SET_REPU,m_iForceSetRepu);

	DDX_Check(pDX,IDC_30_TASKLIMIT,m_bTaskLimit);
	DDX_Text(pDX, IDC_REINCARNATIONLEVEL_EXP, m_iRealmExp);
	DDX_Check(pDX, IDC_AWARD_EXPAND_REALMLEVEL_MAX, m_bExpandRealmLevelMax);
	DDX_Text(pDX, IDC_LEADERSHIP, m_iLeaderShip);
	DDX_Text(pDX, IDC_AWARD_WORLD_CONTRIB, m_iWorldContribution);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CAwardDlg, CDialog)
	//{{AFX_MSG_MAP(CAwardDlg)
	ON_BN_CLICKED(IDC_SEL_NEW_TASK, OnSelNewTask)
	ON_BN_CLICKED(IDC_EDIT_ITEMS_WANTED, OnEditItemsWanted)
	ON_BN_CLICKED(IDC_TRANS_PT, OnTransPt)
	ON_BN_CLICKED(IDC_AWARD_CHANGE_VALUE, OnAwardChangeValue)
	ON_BN_CLICKED(IDC_BTN_EDITHISTORYVAR, OnAwardHistoryChangeValue)
	ON_BN_CLICKED(IDC_AWARD_DISPLAY_VALUE, OnAwardDisplayValue)
	ON_BN_CLICKED(IDC_AWARD_SUMMON_MONSTER, OnAwardSummonMonster)
	ON_BN_CLICKED(IDC_PQ_RANKING, OnPQRankingAward)
	ON_BN_CLICKED(IDC_AWARD_TITLE, OnTitleAward)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAwardDlg message handlers

BOOL CAwardDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_dwGold	= m_pAward->m_ulGoldNum;
	m_dwExp		= m_pAward->m_ulExp;
	m_dwSP		= m_pAward->m_ulSP;
	m_iFactionContrib = m_pAward->m_iFactionContrib;
	m_iFactionExpContrib = m_pAward->m_iFactionExpContrib;
	m_lRepu		= m_pAward->m_lReputation;
	m_nPeriod	= find_period_index(m_pAward->m_ulNewPeriod);
	m_dwStation	= m_pAward->m_ulNewRelayStation;
	m_dwStorehouse = m_pAward->m_ulStorehouseSize;
	m_dwStorehouse2 = m_pAward->m_ulStorehouseSize2;
	m_dwStorehouse3 = m_pAward->m_ulStorehouseSize3;
	m_dwStorehouse4 = m_pAward->m_ulStorehouseSize4;
	m_lInventorySize = m_pAward->m_lInventorySize;
	m_dwPetNum	= m_pAward->m_ulPetInventorySize;
	m_dwFury	= m_pAward->m_ulFuryULimit;
	m_dwTransWld= m_pAward->m_ulTransWldId;
	m_lMonsCtrl	= m_pAward->m_lMonsCtrl;
	m_bTrigCtrl	= m_pAward->m_bTrigCtrl;
	m_bUseLevCo = m_pAward->m_bUseLevCo;
	m_bDivorce	= m_pAward->m_bDivorce;
	m_bSendMsg  = m_pAward->m_bSendMsg;
	m_bMulti    = m_pAward->m_bMulti;
	m_nNumType  = m_pAward->m_nNumType;
	m_lNum      = m_pAward->m_lNum;
	
	m_bDeath	= m_pAward->m_bAwardDeath;
	m_nDeathType= m_pAward->m_bAwardDeathWithLoss;

	m_lSpecifyTaskID = m_pAward->m_ulSpecifyContribTaskID;
	m_lSpecifySubTaskID = m_pAward->m_ulSpecifyContribSubTaskID;
	m_lSpecifyTaskContrib = m_pAward->m_ulSpecifyContrib;
	m_lRandContrib = m_pAward->m_ulRandContrib;
	m_lLowestContrib = m_pAward->m_ulLowestcontrib;
	m_iContrib = m_pAward->m_ulContrib;
	m_lDividend = m_pAward->m_ulDividend;

	m_bAwardSkill = m_pAward->m_bAwardSkill;
	m_iAwardSkillID = m_pAward->m_iAwardSkillID;
	m_iAwardSkillLevel = m_pAward->m_iAwardSkillLevel;

	m_iForceActivity = m_pAward->m_iForceActivity;
	m_iForceContrib = m_pAward->m_iForceContribution;
	m_iForceRepu = m_pAward->m_iForceReputation;
	m_iForceSetRepu = m_pAward->m_iForceSetRepu;

	m_bTaskLimit = m_pAward->m_iTaskLimit;
	m_iRealmExp = m_pAward->m_ulRealmExp;
	m_bExpandRealmLevelMax = m_pAward->m_bExpandRealmLevelMax;
	m_iLeaderShip = m_pAward->m_iLeaderShip;
	m_iWorldContribution = m_pAward->m_iWorldContribution;

	switch (m_pAward->m_nMsgChannel)
	{
	case TASK_MSG_CHANNEL_LOCAL:
		m_nMsgChannel = 0;
		break;
	case TASK_MSG_CHANNEL_WORLD:
		m_nMsgChannel = 1;
		break;
	case TASK_MSG_CHANNEL_BROADCAST:
		m_nMsgChannel = 2;
		break;
	default:
		m_nMsgChannel = 0;
	}

	if (m_pAward->m_ulNewTask)
		m_strNewTask= GetTaskNameByID(m_pAward->m_ulNewTask);

	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CAwardDlg::OnSelNewTask()
{
	CNumDlg dlgNum;
	dlgNum.m_nNum = m_pAward->m_ulNewTask;

	if(dlgNum.DoModal() == IDOK)
	{
		if(dlgNum.m_nNum <= 0)
		{
			m_pAward->m_ulNewTask = 0;
			GetDlgItem(IDC_NEW_TASK)->SetWindowText("");
			return;
		}

		ATaskTempl* pTask = new ATaskTempl;
		
		CString strPath = g_TaskIDMan.GetPathByID(dlgNum.m_nNum);
		strPath = "BaseData\\TaskTemplate\\" + strPath + ".tkt";

		if(!pTask->LoadFromTextFile(strPath))
		{
			AfxMessageBox("找不到此任务，请输入正确的任务ID。");
			delete pTask;
			return;
		}

		CString strName = pTask->GetName();

		m_pAward->m_ulNewTask = dlgNum.m_nNum;
		GetDlgItem(IDC_NEW_TASK)->SetWindowText(strName);

		delete pTask;
	}

	/*
	CTaskIDSelDlg dlg;
	dlg.m_bShowSub = false;
	dlg.m_ulID = m_pAward->m_ulNewTask;

	if (dlg.DoModal() == IDOK)
	{
		m_pAward->m_ulNewTask = dlg.m_ulID;
		GetDlgItem(IDC_NEW_TASK)->SetWindowText(dlg.m_strName);
	}
	*/
}

void CAwardDlg::OnEditItemsWanted()
{
	CItemsCandDlg dlg(m_pAward);
	dlg.DoModal();
}

void CAwardDlg::OnOK() 
{
	UpdateData();
	m_pAward->m_ulGoldNum			= m_dwGold;
	m_pAward->m_ulExp				= m_dwExp;
	m_pAward->m_ulSP				= m_dwSP;
	m_pAward->m_iFactionContrib		= m_iFactionContrib;
	m_pAward->m_iFactionExpContrib	= m_iFactionExpContrib;
	m_pAward->m_lReputation			= m_lRepu;
	m_pAward->m_ulNewPeriod			= period_map[m_nPeriod];
	m_pAward->m_ulNewRelayStation	= m_dwStation;
	m_pAward->m_ulStorehouseSize	= m_dwStorehouse;
	m_pAward->m_ulStorehouseSize2	= m_dwStorehouse2;
	m_pAward->m_ulStorehouseSize3	= m_dwStorehouse3;
	m_pAward->m_ulStorehouseSize4	= m_dwStorehouse4;
	m_pAward->m_lInventorySize		= m_lInventorySize;
	m_pAward->m_ulPetInventorySize	= m_dwPetNum;
	m_pAward->m_ulFuryULimit		= m_dwFury;
	m_pAward->m_ulTransWldId		= m_dwTransWld;
	m_pAward->m_lMonsCtrl			= m_lMonsCtrl;
	m_pAward->m_bTrigCtrl			= (m_bTrigCtrl != 0);
	m_pAward->m_bUseLevCo			= (m_bUseLevCo != 0);
	m_pAward->m_bDivorce			= (m_bDivorce != 0);
	m_pAward->m_bSendMsg			= (m_bSendMsg != 0);
	m_pAward->m_bMulti				= (m_bMulti != 0);
	m_pAward->m_nNumType			= m_nNumType;
	m_pAward->m_lNum				= m_lNum;

	m_pAward->m_bAwardDeath			= (m_bDeath != 0);
	m_pAward->m_bAwardDeathWithLoss = (m_nDeathType != 0);

	m_pAward->m_ulSpecifyContribTaskID = m_lSpecifyTaskID;
	m_pAward->m_ulSpecifyContribSubTaskID = m_lSpecifySubTaskID;
	m_pAward->m_ulSpecifyContrib	= m_lSpecifyTaskContrib;
	m_pAward->m_ulRandContrib		= m_lRandContrib;
	m_pAward->m_ulLowestcontrib		= m_lLowestContrib;
	m_pAward->m_ulContrib			= m_iContrib;
	m_pAward->m_ulDividend			= m_lDividend;

	m_pAward->m_bAwardSkill			= (m_bAwardSkill != 0);
	m_pAward->m_iAwardSkillID		= m_iAwardSkillID;
	m_pAward->m_iAwardSkillLevel	= m_iAwardSkillLevel;

	m_pAward->m_iForceActivity		= m_iForceActivity;
	m_pAward->m_iForceContribution  = m_iForceContrib;
	m_pAward->m_iForceReputation	= m_iForceRepu;
	m_pAward->m_iForceSetRepu		= m_iForceSetRepu;

	m_pAward->m_iTaskLimit			= m_bTaskLimit;
	m_pAward->m_ulRealmExp = m_iRealmExp;
	m_pAward->m_bExpandRealmLevelMax = (m_bExpandRealmLevelMax != 0);
	m_pAward->m_iLeaderShip			= m_iLeaderShip;
	m_pAward->m_iWorldContribution	= m_iWorldContribution;

	switch (m_nMsgChannel)
	{
	case 0:
		m_pAward->m_nMsgChannel = TASK_MSG_CHANNEL_LOCAL;
		break;
	case 1:
		m_pAward->m_nMsgChannel = TASK_MSG_CHANNEL_WORLD;
		break;
	case 2:
		m_pAward->m_nMsgChannel = TASK_MSG_CHANNEL_BROADCAST;
		break;
	}

	CDialog::OnOK();
}

void CAwardDlg::OnTransPt() 
{
	CVectorDlg dlg;
	dlg.m_fX = m_pAward->m_TransPt.x;
	dlg.m_fY = m_pAward->m_TransPt.y;
	dlg.m_fZ = m_pAward->m_TransPt.z;

	if (dlg.DoModal() == IDOK)
	{
		m_pAward->m_TransPt.x = dlg.m_fX;
		m_pAward->m_TransPt.y = dlg.m_fY;
		m_pAward->m_TransPt.z = dlg.m_fZ;
	}
}

void CAwardDlg::OnAwardChangeValue() 
{
	// TODO: Add your control notification handler code here
	CAwardChangeValue dlg(&m_pAward->m_ulChangeKeyCnt, &m_pAward->m_plChangeKey, &m_pAward->m_plChangeKeyValue, &m_pAward->m_pbChangeType);
	dlg.DoModal();
}
void CAwardDlg::OnAwardHistoryChangeValue() 
{
	// TODO: Add your control notification handler code here
	CAwardChangeValue dlg(&m_pAward->m_ulHistoryChangeCnt, &m_pAward->m_plHistoryChangeKey, &m_pAward->m_plHistoryChangeKeyValue, &m_pAward->m_pbHistoryChangeType);
	dlg.DoModal();
}
void CAwardDlg::OnAwardDisplayValue() 
{
	// TODO: Add your control notification handler code here
	CAwardDisplayValue dlg(m_pAward);
	dlg.DoModal();
}

void CAwardDlg::OnAwardSummonMonster() 
{
	// TODO: Add your control notification handler code here
	// for test
	CMonsterSummonedDlg dlg(m_pAward);
	dlg.DoModal();
}

void CAwardDlg::OnPQRankingAward() 
{
	CAwardRanking dlg(m_pAward);
	dlg.DoModal();	
}
void CAwardDlg::OnTitleAward()
{
	CMDIFrameWnd *pFrame = (CMDIFrameWnd*)AfxGetApp()->m_pMainWnd;
	CMDIChildWnd *pChild = (CMDIChildWnd *) pFrame->GetActiveFrame();
	CTaskTemplEditorView *pView = (CTaskTemplEditorView *) pChild->GetActiveView();
	HTREEITEM hSel = pView->m_TaskTree.GetSelectedItem();
	if (hSel == 0) return;
	ATaskTempl* pTask = (ATaskTempl*)pView->m_TaskTree.GetItemData(hSel);
	
	CTaskTitle dlg(pTask, 
		m_pAward == pTask->m_Award_S ? CTaskTitle::TITLE_FOR_AWARD_SUCCED : CTaskTitle::TITLE_FOR_AWARD_FAILED);
	dlg.DoModal();
}
