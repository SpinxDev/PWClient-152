// TaskPropertyPage.cpp : implementation file
//

#include "stdafx.h"
#include "TaskTemplEditor.h"
#include "TaskPropertyPage.h"
#include "TaskTemplEditorDoc.h"
#include "TaskTemplEditorView.h"
#include "TimetableDlg.h"
#include "TalkModifyDlg.h"
#include "BaseDataIDMan.h"
#include "VssOperation.h"
#include "VectorDlg.h"
#include "TemplIDSelDlg.h"
#include "AwardChangeValue.h"
#include "AbsTimeDlg.h"
#include "PQGobalValue.h"
#include "PQSubTaskProp.h"
#include "TaskRegionDlg.h"

#ifdef _DEBUG
#define new A_DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern LPCSTR g_strTaskIdSvrPath;
extern BOOL _recover_mode;
#include "TalkModifyDlg.h"

#ifdef _TASK_CLIENT
/*
void _export_task_talk(const ATaskTempl* pTempl, talk_proc* pTalk)
{
	if (_recover_mode)
		return;

	if (pTalk->id_talk == 0) return;
	CTalkModifyDlg dlg;

	AString strPath = g_TalkIDMan.GetPathByID(pTalk->id_talk);

	if (strPath.IsEmpty())
	{
		CString str;
		str.Format("错误的TalkID，ID = %d, TaskName = %s", pTalk->id_talk, CSafeString(pTempl->GetName()).GetAnsi());
		AfxMessageBox(str);
		if (pTalk->windows)
		{
			delete[] pTalk->windows;
			g_ulDelCount++;
		}
		memset(pTalk, 0, sizeof(*pTalk));
		return;
	}

	dlg.ExportData(
		pTalk,
		g_TalkIDMan.GetPathByID(pTalk->id_talk),
		pTalk->id_talk
		);
}
*/

#endif

/////////////////////////////////////////////////////////////////////////////
// CTaskPropertyPage property page

IMPLEMENT_DYNCREATE(CTaskPropertyPage, CPropertyPage)

CTaskPropertyPage::CTaskPropertyPage() : CPropertyPage(CTaskPropertyPage::IDD), m_pTask(0)
{
	//{{AFX_DATA_INIT(CTaskPropertyPage)
	m_bCanRedoAfterFailure = FALSE;
	m_bCanRedo = FALSE;
	m_dwMaxTime = 0;
	m_strName = _T("");
	m_bNeedRecord = FALSE;
	m_bMaxReceiver = FALSE;
	m_bCanGiveUp = FALSE;
	m_bFailAsKilled = FALSE;
	m_bChildInOrder = FALSE;
	m_bParentAlsoFail = FALSE;
	m_dwRcvNum = 0;
	m_bParentAlsoSucc = FALSE;
	m_bClearAsGiveUp = FALSE;
	m_strDescript = _T("");
	m_strTribute = _T("");
	m_bChooseOne = FALSE;
	m_bHasZone = FALSE;
	m_strId = _T("");
	m_bAutoDelv = FALSE;
	m_bPromptWhenAutoDelv = FALSE;
	m_bRandOne = FALSE;
	m_bDeathTrig = FALSE;
	m_bClearAcquired = FALSE;
	m_dwSuitedLev = 0;
	m_bKeyTask = FALSE;
	m_bShowPrompt = FALSE;
	m_strAwardNPC = _T("");
	m_strDelvNPC = _T("");
	m_bSkillTask = FALSE;
	m_dwWorldId = 0;
	m_dwTransWld = 0;
	m_bTransTo = FALSE;
	m_strSignature = _T("");
	m_nType = 0;
	m_bCanSeekOut = FALSE;

	m_bShowDirection = FALSE;
	m_lMonsCtrl = 0;
	m_bTrigCtrl = FALSE;
	m_bAbsTime = FALSE;
	m_nVailFrequency = 0;
	m_nPeriodLimit = 1;
	m_bMarriage = FALSE;
	m_bSwitchSceneFail = FALSE;
	m_bHidden = FALSE;
	m_bLeaveRegion = FALSE;
	m_iLeaveRegionID = 0;
	m_bEnterRegion = FALSE;
	m_iEnterRegionID = 0;
	m_bOffLineFail = FALSE;
	m_bTaskFailTime = FALSE;
	m_bItemNotTakeOff = FALSE;
	m_bIsPQTask = FALSE;
	m_bIsPQSubTask = FALSE;
	m_bAccountTaskLimit = FALSE;
//	m_iAccountTaskLimitCnt = 0;

	m_bDeliverySkill = FALSE;
	m_iDeliverySkillID = 0;
	m_iDeliverySkillLevel = 0;

	m_bShowGfx		= FALSE;

	m_bChangePQRanking = FALSE;
	
	m_bCompareItemAndInventory = FALSE;
	m_ulNeedInventorySlotNum = 0;

	m_bLeaveForceFail = FALSE;
	m_bLeaveFactionFail = FALSE;

	m_bNotClearItemWhenFailed = FALSE;
	m_bDisplayInTitleTaskUI = FALSE;
	m_bNotAddCountWhenFailed = FALSE;
	m_bUsedInTokenShop = FALSE;
	//}}AFX_DATA_INIT
}

CTaskPropertyPage::~CTaskPropertyPage()
{
}

void CTaskPropertyPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTaskPropertyPage)
	DDX_Control(pDX, IDC_NEED_RECORD, m_BtnNeedRecord);
	DDX_Control(pDX, IDC_CAN_REDO_AFTER_FAILURE, m_BtnCanRedoAfterFailure);
	DDX_Control(pDX, IDC_CAN_REDO, m_BtnCanRedo);
	DDX_Check(pDX, IDC_CAN_REDO_AFTER_FAILURE, m_bCanRedoAfterFailure);
	DDX_Check(pDX, IDC_CAN_REDO, m_bCanRedo);
	DDX_Text(pDX, IDC_MAX_TIME, m_dwMaxTime);
	DDX_Text(pDX, IDC_TASK_NAME, m_strName);
	DDX_Check(pDX, IDC_NEED_RECORD, m_bNeedRecord);
	DDX_Check(pDX, IDC_MAX_RECEIVER, m_bMaxReceiver);
	DDX_Check(pDX, IDC_CAN_GIVEUP, m_bCanGiveUp);
	DDX_Check(pDX, IDC_KILLED_AS_FAIL, m_bFailAsKilled);
	DDX_Check(pDX, IDC_ORDER_EXE_CHILD, m_bChildInOrder);
	DDX_Check(pDX, IDC_PARENT_ALSO_FAIL, m_bParentAlsoFail);
	DDX_Text(pDX, IDC_RECEIVER_NUM, m_dwRcvNum);
	DDX_Check(pDX, IDC_PARENT_ALSO_SUCCESS, m_bParentAlsoSucc);
	DDX_Check(pDX, IDC_GIVEUP_CLEAR, m_bClearAsGiveUp);
	DDX_Text(pDX, IDC_DESCRIPT, m_strDescript);
	DDX_Text(pDX, IDC_TRIBUTE, m_strTribute);
	DDX_Check(pDX, IDC_CHOOSE_ONE, m_bChooseOne);
	DDX_Check(pDX, IDC_HAS_ZONE, m_bHasZone);
	DDX_Text(pDX, IDC_TASK_ID, m_strId);
	DDX_Check(pDX, IDC_AUTO_DELV, m_bAutoDelv);
	DDX_Check(pDX,IDC_WHETHER_PROMPT, m_bPromptWhenAutoDelv);
	DDX_Control(pDX, IDC_WHETHER_PROMPT, m_BtnPrompt);
	DDX_Check(pDX, IDC_RAND_ONE, m_bRandOne);
	DDX_Check(pDX, IDC_DEATH_TRIG, m_bDeathTrig);
	DDX_Check(pDX, IDC_CLEAR_ACQUIRED, m_bClearAcquired);
	DDX_Text(pDX, IDC_SUIT_LEVEL, m_dwSuitedLev);
	DDX_Check(pDX, IDC_KEY_TASK, m_bKeyTask);
	DDX_Check(pDX, IDC_SHOW_PROMPT, m_bShowPrompt);
	DDX_Text(pDX, IDC_AWARD_NPC, m_strAwardNPC);
	DDX_Text(pDX, IDC_DELV_NPC, m_strDelvNPC);
	DDX_Check(pDX, IDC_SKILL_TASK, m_bSkillTask);
	DDX_Text(pDX, IDC_WORD_ID, m_dwWorldId);
	DDX_Text(pDX, IDC_TRANS_WORLD, m_dwTransWld);
	DDX_Check(pDX, IDC_TRANSPORT, m_bTransTo);
	DDX_Text(pDX, IDC_SIGNATURE, m_strSignature);
	DDV_MaxChars(pDX, m_strSignature, 28);
	DDX_CBIndex(pDX, IDC_TYPE, m_nType);
	DDX_Check(pDX, IDC_SEEK_OUT, m_bCanSeekOut);
	DDX_Check(pDX, IDC_SHOW_DIRECTION, m_bShowDirection);
	DDX_Text(pDX, IDC_CONTROLLER, m_lMonsCtrl);
	DDX_Check(pDX, IDC_TRIG_CTRL, m_bTrigCtrl);
	DDX_Check(pDX, IDC_ABS_TIME, m_bAbsTime);
	DDX_CBIndex(pDX, IDC_AVAIL_FREQUENCY, m_nVailFrequency);
	DDX_Text(pDX,IDC_TASK_PERIOD_LIMIT,m_nPeriodLimit);
	DDV_MinMaxInt(pDX, m_nPeriodLimit, 1, 65535);
	DDX_Check(pDX, IDC_MARRIAGE, m_bMarriage);
	DDX_Check(pDX, IDC_SWITCH_SCENE_FAIL, m_bSwitchSceneFail);
	DDX_Check(pDX, IDC_HIDDEN, m_bHidden);
	DDX_Check(pDX, IDC_LEAVE_REGION_FAIL, m_bLeaveRegion);
	DDX_Text(pDX, IDC_LEAVE_WORLD_ID, m_iLeaveRegionID);
	DDX_Check(pDX, IDC_ENTER_REGION_FAIL, m_bEnterRegion);
	DDX_Text(pDX, IDC_ENTER_WORLD_ID, m_iEnterRegionID);
	DDX_Check(pDX, IDC_OFFLINE_FAIL, m_bOffLineFail);
	DDX_Check(pDX, IDC_TASK_FAIL_TIME, m_bTaskFailTime);
	DDX_Check(pDX, IDC_ITEM_NOT_TAKE_OFF, m_bItemNotTakeOff);
	DDX_Check(pDX, IDC_PQ_TASK, m_bIsPQTask);
	DDX_Check(pDX, IDC_PQ_SUB_TASK, m_bIsPQSubTask);
	DDX_Check(pDX, IDC_ACCOUNT_TASK_LIMIT, m_bAccountTaskLimit);
	DDX_Check(pDX, IDC_ROLE_TASK_LIMIT, m_bRoleTaskLimit);
//	DDX_Text(pDX, IDC_ACCOUNT_TASK_LIMIT_CNT, m_iAccountTaskLimitCnt);
//	DDV_MinMaxInt(pDX, m_iAccountTaskLimitCnt, 0, 65535);

	DDX_Check(pDX,IDC_DELIVERYSKILL,m_bDeliverySkill);
	DDX_Text(pDX,IDC_DELIVERYSKILLID,m_iDeliverySkillID);
	DDX_Text(pDX,IDC_DELIVERYSKILLLEVEL,m_iDeliverySkillLevel);

	DDX_Check(pDX,IDC_SHOWGFX,m_bShowGfx);

	DDX_Check(pDX,IDC_CHANGE_PQRANK,m_bChangePQRanking);
	DDX_Check(pDX,IDC_CHK_COMPAREITEMANDINVENTORY,m_bCompareItemAndInventory);
	DDX_Text(pDX,IDC_EDIT_INVENTORYSLOTNUM,m_ulNeedInventorySlotNum);

	DDX_Check(pDX,IDC_LEAVEFORCE_FAIL,m_bLeaveForceFail);
	DDX_Check(pDX,IDC_LEAVEFACTION_FAIL,m_bLeaveFactionFail);

	DDX_Check(pDX, IDC_NOT_CLEAR_WHEN_FAILED, m_bNotClearItemWhenFailed);
	DDX_Check(pDX, IDC_TITLE, m_bDisplayInTitleTaskUI);
	DDX_Check(pDX, IDC_NOT_ADD_COUNT_WHEN_FAILED, m_bNotAddCountWhenFailed);
	DDX_Check(pDX, IDC_TOKENSHOP, m_bUsedInTokenShop);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTaskPropertyPage, CPropertyPage)
	//{{AFX_MSG_MAP(CTaskPropertyPage)
	ON_BN_CLICKED(IDC_CAN_REDO, OnCanRedo)
	ON_BN_CLICKED(IDC_CAN_REDO_AFTER_FAILURE, OnCanRedoAfterFailure)
	ON_BN_CLICKED(IDC_EDIT_TIMETABLE, OnEditTimetable)
	ON_BN_CLICKED(IDC_DELIVER_TALK, OnDeliverTalk)
	ON_BN_CLICKED(IDC_DELIVER_ITEM_TALK, OnDeliverItemTalk)
	ON_BN_CLICKED(IDC_ACCEPTED_TALK, OnAcceptedTalk)
	ON_BN_CLICKED(IDC_AWARD_TALK, OnAwardTalk)

	ON_BN_CLICKED(IDC_SEL_MAX_POINT, OnSelMaxPoint)
	ON_BN_CLICKED(IDC_SHOW_TALK, OnShowTalk)
	ON_BN_CLICKED(IDC_SEL_AWARD_NPC, OnSelAwardNpc)
	ON_BN_CLICKED(IDC_SEL_DELV_NPC, OnSelDelvNpc)
	ON_BN_CLICKED(IDC_TRANS_POINT, OnTransPoint)
	ON_BN_CLICKED(IDC_EDIT_CHANGE_VAL, OnEditChangeVal)

	ON_BN_CLICKED(IDC_EDIT_ENTER_BIG, OnEditEnterBig)
	ON_BN_CLICKED(IDC_EDIT_LEAVE_BIG, OnEditLeaveBig)

	ON_BN_CLICKED(IDC_EDIT_FAIL_TIME, OnEditFailTime)
	ON_BN_CLICKED(IDC_PQ_VALUE, OnPQValueDisplay)
	ON_BN_CLICKED(IDC_PQ_SUB_TASK_PROP, OnPQSubTaskProp)
	ON_BN_CLICKED(IDC_AUTO_DELV,OnAutoDelvChecked)
	ON_BN_CLICKED(IDC_ACCOUNT_TASK_LIMIT,OnAccountLimitChecked)
	ON_BN_CLICKED(IDC_ROLE_TASK_LIMIT, OnRoleLimitChecked)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTaskPropertyPage message handlers
bool bEnable = true;
BOOL CTaskPropertyPage::UpdatePage(ATaskTempl* pTask, BOOL bUpdateData, BOOL bReadOnly)
{
	BOOL bChanged = FALSE;

	if (bUpdateData)
	{
		UpdateData();

#ifdef _TASK_CLIENT
		if (wcscmp(pTask->GetName(), CSafeString(m_strName)))
		{
			pTask->SetName(CSafeString(m_strName));
			bChanged = TRUE;
		}
#endif

		pTask->m_ulType					= GetTaskTypeFromComboBox(m_nType);
		pTask->m_ulTimeLimit			= m_dwMaxTime;
		pTask->m_bExeChildInOrder		= (m_bChildInOrder != 0);
		pTask->m_bChooseOne				= (m_bChooseOne != 0);
		pTask->m_bRandOne				= (m_bRandOne != 0);
		pTask->m_bParentAlsoFail		= (m_bParentAlsoFail != 0);
		pTask->m_bParentAlsoSucc		= (m_bParentAlsoSucc != 0);
		pTask->m_bCanGiveUp				= (m_bCanGiveUp != 0);
		pTask->m_bFailAsPlayerDie		= (m_bFailAsKilled != 0);
		pTask->m_bCanRedo				= (m_bCanRedo != 0);
		pTask->m_bCanRedoAfterFailure	= (m_bCanRedoAfterFailure != 0);
		pTask->m_bClearAsGiveUp			= (m_bClearAsGiveUp != 0);
		pTask->m_bNeedRecord			= (m_bNeedRecord != 0);
		pTask->m_ulMaxReceiver			= (m_bMaxReceiver ? m_dwRcvNum : 0);
		pTask->m_bDelvInZone			= (m_bHasZone != 0);
		pTask->m_ulDelvWorld			= m_dwWorldId;
		pTask->m_bTransTo				= (m_bTransTo != 0);
		pTask->m_ulTransWldId			= m_dwTransWld;
		pTask->m_lMonsCtrl				= m_lMonsCtrl;
		pTask->m_bTrigCtrl				= (m_bTrigCtrl != 0);
		pTask->m_bAutoDeliver			= (m_bAutoDelv != 0);
		pTask->m_bDisplayInExclusiveUI = (m_bPromptWhenAutoDelv != 0);
		pTask->m_bDeathTrig				= (m_bDeathTrig != 0);
		pTask->m_bClearAcquired			= (m_bClearAcquired != 0);
		pTask->m_ulSuitableLevel		= m_dwSuitedLev;
		pTask->m_bKeyTask				= (m_bKeyTask != 0);
		pTask->m_bShowPrompt			= (m_bShowPrompt != 0);
		pTask->m_bSkillTask				= (m_bSkillTask != 0);
		pTask->m_bCanSeekOut			= (m_bCanSeekOut != 0);
		pTask->m_bShowDirection			= (m_bShowDirection != 0);
		pTask->m_bAbsTime				= (m_bAbsTime != 0);
		pTask->m_lAvailFrequency		= m_nVailFrequency;
		pTask->m_lPeriodLimit			= m_nPeriodLimit;
		pTask->m_bMarriage				= (m_bMarriage != 0);
		pTask->m_bSwitchSceneFail       = (m_bSwitchSceneFail != 0);
		pTask->m_bHidden                = (m_bHidden != 0);
		pTask->m_bEnterRegionFail		= (m_bEnterRegion != 0);
		pTask->m_ulEnterRegionWorld		= m_iEnterRegionID;
		pTask->m_bLeaveRegionFail		= (m_bLeaveRegion != 0);
		pTask->m_ulLeaveRegionWorld		= m_iLeaveRegionID;
		pTask->m_bOfflineFail			= (m_bOffLineFail != 0);
		pTask->m_bItemNotTakeOff		= (m_bItemNotTakeOff != 0);
		pTask->m_bAbsFail				= (m_bTaskFailTime != 0);

		pTask->m_bPQTask				= (m_bIsPQTask != 0);
		pTask->m_bPQSubTask				= (m_bIsPQSubTask != 0);
		
		pTask->m_bAccountTaskLimit		= (m_bAccountTaskLimit != 0);
		pTask->m_bRoleTaskLimit			= (m_bRoleTaskLimit != 0);
//		pTask->m_ulAccountTaskLimitCnt	= m_iAccountTaskLimitCnt;

		pTask->m_bDeliverySkill			= (m_bDeliverySkill!=0);
		pTask->m_iDeliveredSkillID		= m_iDeliverySkillID;
		pTask->m_iDeliveredSkillLevel	= m_iDeliverySkillLevel;

		pTask->m_bShowGfxFinished		= (m_bShowGfx!=0);

		pTask->m_bChangePQRanking		= (m_bChangePQRanking != 0);

		pTask->m_bCompareItemAndInventory = (m_bCompareItemAndInventory!=0);
		pTask->m_ulInventorySlotNum = m_ulNeedInventorySlotNum;

		pTask->m_bLeaveForceFail = (m_bLeaveForceFail != 0);
		pTask->m_bLeaveFactionFail = (m_bLeaveFactionFail != 0);

		pTask->m_bNotClearItemWhenFailed = (m_bNotClearItemWhenFailed != 0);
		pTask->m_bDisplayInTitleTaskUI = (m_bDisplayInTitleTaskUI != 0);
		pTask->m_bNotIncCntWhenFailed = (m_bNotAddCountWhenFailed != 0);
		pTask->m_bUsedInTokenShop = (m_bUsedInTokenShop != 0);

		m_strSignature.TrimLeft();
		m_strSignature.TrimRight();

		if (!m_strSignature.IsEmpty())
		{
			pTask->m_bHasSign = true;
			wcscpy(pTask->m_pszSignature, CSafeString(m_strSignature));
		}
		else
			pTask->m_bHasSign = false;

		int len;

		CSafeString strDes(m_strDescript);
		len = wcslen(strDes);
	
		if (pTask->m_pwstrDescript)
		{
			delete[] pTask->m_pwstrDescript;
			pTask->m_pwstrDescript = NULL;
			g_ulDelCount++;
		}

		if (len)
		{
			pTask->m_pwstrDescript = new wchar_t[len+1];
			wcscpy(pTask->m_pwstrDescript, strDes);
			g_ulNewCount++;
		}

		CSafeString strTribute(m_strTribute);
		len = wcslen(strTribute);

		if (pTask->m_pwstrTribute)
		{
			delete[] pTask->m_pwstrTribute;
			pTask->m_pwstrTribute = NULL;
			g_ulDelCount++;
		}

		if (len)
		{
			pTask->m_pwstrTribute = new wchar_t[len+1];
			wcscpy(pTask->m_pwstrTribute, strTribute);
			g_ulNewCount++;
		}

		if (m_bRoleTaskLimit && m_nPeriodLimit > 250) 
		{ 
			AfxMessageBox("角色限次不能超过250次");
			pTask->m_lPeriodLimit = 1;
			bChanged = FALSE;
		} else bChanged = TRUE;
	}
	else
	{
#ifdef _TASK_CLIENT
		m_strName				= pTask->GetName();
#endif
		m_strId.Format("%d", pTask->GetID());
		m_nType					= GetComboBoxFromTaskType(pTask->m_ulType);
		m_bChildInOrder			= pTask->m_bExeChildInOrder;
		m_bChooseOne			= pTask->m_bChooseOne;
		m_bRandOne				= pTask->m_bRandOne;
		m_bParentAlsoFail		= pTask->m_bParentAlsoFail;
		m_bParentAlsoSucc		= pTask->m_bParentAlsoSucc;
		m_bCanGiveUp			= pTask->m_bCanGiveUp;
		m_bFailAsKilled			= pTask->m_bFailAsPlayerDie;
		m_dwMaxTime				= pTask->m_ulTimeLimit;
		m_bCanRedo				= pTask->m_bCanRedo;
		m_bCanRedoAfterFailure	= pTask->m_bCanRedoAfterFailure;
		m_bClearAsGiveUp		= pTask->m_bClearAsGiveUp;
		m_bNeedRecord			= pTask->m_bNeedRecord;
		m_bMaxReceiver			= pTask->m_ulMaxReceiver != 0;
		m_dwRcvNum				= pTask->m_ulMaxReceiver;
		m_bHasZone				= pTask->m_bDelvInZone;
		m_dwWorldId				= pTask->m_ulDelvWorld;
		m_bTransTo				= pTask->m_bTransTo;
		m_dwTransWld			= pTask->m_ulTransWldId;
		m_lMonsCtrl				= pTask->m_lMonsCtrl;
		m_bTrigCtrl				= pTask->m_bTrigCtrl;
		m_bAutoDelv				= pTask->m_bAutoDeliver;
		m_bPromptWhenAutoDelv   = pTask->m_bDisplayInExclusiveUI;
		m_bDeathTrig			= pTask->m_bDeathTrig;
		m_bClearAcquired		= pTask->m_bClearAcquired;
		m_dwSuitedLev			= pTask->m_ulSuitableLevel;
		m_bKeyTask				= pTask->m_bKeyTask;
		m_bShowPrompt			= pTask->m_bShowPrompt;
		m_bSkillTask			= pTask->m_bSkillTask;
		m_bCanSeekOut			= pTask->m_bCanSeekOut;
		m_bShowDirection		= pTask->m_bShowDirection;
		m_bAbsTime				= pTask->m_bAbsTime;
		m_nVailFrequency		= pTask->m_lAvailFrequency;
		m_nPeriodLimit			= pTask->m_lPeriodLimit;
		m_bMarriage				= pTask->m_bMarriage;
		m_bSwitchSceneFail      = pTask->m_bSwitchSceneFail;
		m_bHidden               = pTask->m_bHidden;
		m_bEnterRegion			= pTask->m_bEnterRegionFail;
		m_iEnterRegionID		= pTask->m_ulEnterRegionWorld;
		m_bLeaveRegion			= pTask->m_bLeaveRegionFail;
		m_iLeaveRegionID		= pTask->m_ulLeaveRegionWorld;
		m_bOffLineFail			= pTask->m_bOfflineFail;
		m_bItemNotTakeOff		= pTask->m_bItemNotTakeOff;
		m_bTaskFailTime			= pTask->m_bAbsFail;

		m_bIsPQTask				= pTask->m_bPQTask;
		m_bIsPQSubTask			= pTask->m_bPQSubTask;

		m_bAccountTaskLimit		= pTask->m_bAccountTaskLimit;
		m_bRoleTaskLimit		= pTask->m_bRoleTaskLimit;
//		m_iAccountTaskLimitCnt	= pTask->m_ulAccountTaskLimitCnt;

		m_bDeliverySkill		= pTask->m_bDeliverySkill;
		m_iDeliverySkillID		= pTask->m_iDeliveredSkillID;
		m_iDeliverySkillLevel	= pTask->m_iDeliveredSkillLevel;

		m_bShowGfx				= pTask->m_bShowGfxFinished;

		m_bChangePQRanking		= pTask->m_bChangePQRanking;
		m_bCompareItemAndInventory = pTask->m_bCompareItemAndInventory;
		m_ulNeedInventorySlotNum = pTask->m_ulInventorySlotNum;

		m_bLeaveForceFail = pTask->m_bLeaveForceFail;
		m_bLeaveFactionFail = pTask->m_bLeaveFactionFail;

		m_bNotClearItemWhenFailed = pTask->m_bNotClearItemWhenFailed;
		m_bDisplayInTitleTaskUI = pTask->m_bDisplayInTitleTaskUI;
		m_bNotAddCountWhenFailed = pTask->m_bNotIncCntWhenFailed;
		m_bUsedInTokenShop = pTask->m_bUsedInTokenShop;

		m_pTask					= pTask;

		if (pTask->m_bHasSign)
			m_strSignature = pTask->m_pszSignature;
		else
			m_strSignature.Empty();

		m_strDelvNPC = GetBaseDataNameByID(pTask->m_ulDelvNPC);
		m_strAwardNPC = GetBaseDataNameByID(pTask->m_ulAwardNPC);

		m_strDescript	= pTask->m_pwstrDescript ? CSafeString(pTask->m_pwstrDescript).GetAnsi() : "";
		m_strTribute	= pTask->m_pwstrTribute ? CSafeString(pTask->m_pwstrTribute).GetAnsi() : "";

		CWnd* pFirst = GetWindow(GW_CHILD);
		while (pFirst)
		{
			if (pFirst->GetDlgCtrlID() != 0xFFFF)
				pFirst->EnableWindow(!bReadOnly);
			pFirst = pFirst->GetNextWindow();
		}

		if (!bReadOnly && pTask->m_pParent)
		{
			GetDlgItem(IDC_CAN_REDO)->EnableWindow(FALSE);
			GetDlgItem(IDC_CAN_REDO_AFTER_FAILURE)->EnableWindow(FALSE);
			GetDlgItem(IDC_GIVEUP_CLEAR)->EnableWindow(FALSE);
			GetDlgItem(IDC_NEED_RECORD)->EnableWindow(FALSE);
			GetDlgItem(IDC_KILLED_AS_FAIL)->EnableWindow(FALSE);
			GetDlgItem(IDC_KILLED_AS_FAIL)->EnableWindow(FALSE);
			GetDlgItem(IDC_MAX_RECEIVER)->EnableWindow(FALSE);
			GetDlgItem(IDC_RECEIVER_NUM)->EnableWindow(FALSE);
			GetDlgItem(IDC_DELIVER_TALK)->EnableWindow(FALSE);
			GetDlgItem(IDC_CHOOSE_ONE)->EnableWindow(FALSE);
			GetDlgItem(IDC_EDIT_TIMETABLE)->EnableWindow(FALSE);
			GetDlgItem(IDC_HAS_ZONE)->EnableWindow(FALSE);
	//		GetDlgItem(IDC_SEL_MIN_POINT)->EnableWindow(FALSE);
			GetDlgItem(IDC_SEL_MAX_POINT)->EnableWindow(FALSE);
			GetDlgItem(IDC_CHOOSE_ONE)->EnableWindow(FALSE);
			GetDlgItem(IDC_DEATH_TRIG)->EnableWindow(FALSE);
			GetDlgItem(IDC_SUIT_LEVEL)->EnableWindow(FALSE);
			GetDlgItem(IDC_KEY_TASK)->EnableWindow(FALSE);
			GetDlgItem(IDC_SHOW_PROMPT)->EnableWindow(FALSE);
			GetDlgItem(IDC_SKILL_TASK)->EnableWindow(FALSE);
			GetDlgItem(IDC_AUTO_DELV)->EnableWindow(FALSE);
			
			GetDlgItem(IDC_PQ_TASK)->EnableWindow(FALSE);
//			GetDlgItem(IDC_PQ_VALUE)->EnableWindow(FALSE);
		}

		//Begin: PQ task [Yongdong,2010-1-21]
		if (!bReadOnly && pTask->m_pParent && pTask->m_bPQSubTask)
		{
			GetDlgItem(IDC_PQ_SUB_TASK)->EnableWindow(FALSE);
		//	GetDlgItem(IDC_PQ_SUB_TASK_PROP)->EnableWindow(FALSE);
		//	GetDlgItem(IDC_SWITCH_SCENE_FAIL)->EnableWindow(FALSE);
		}
		if(pTask->m_bPQTask || pTask->m_bPQSubTask)
		{
			GetDlgItem(IDC_MAX_TIME)->EnableWindow(FALSE);
			GetDlgItem(IDC_EDIT_TIMETABLE)->EnableWindow(FALSE);
		}
		if (pTask->m_pParent && pTask->m_pParent->m_bPQTask)
		{
			GetDlgItem(IDC_PARENT_ALSO_FAIL)->EnableWindow(FALSE);
			m_bParentAlsoFail = 0;

		}

		if (!pTask->m_bAutoDeliver)
		{
			GetDlgItem(IDC_WHETHER_PROMPT)->EnableWindow(FALSE);
			bEnable = false;
		}
		else
		{
			bEnable = true;
		}

		if (pTask->m_bAccountTaskLimit || pTask->m_bRoleTaskLimit)
		{
			GetDlgItem(IDC_TASK_PERIOD_LIMIT)->EnableWindow(TRUE);
			if (pTask->m_bAccountTaskLimit) GetDlgItem(IDC_ROLE_TASK_LIMIT)->EnableWindow(FALSE);
			else GetDlgItem(IDC_ACCOUNT_TASK_LIMIT)->EnableWindow(FALSE);
		}
		else
		{
			//GetDlgItem(IDC_ACCOUNT_TASK_LIMIT_CNT)->EnableWindow(FALSE);
			if (!bReadOnly)
			{
				GetDlgItem(IDC_TASK_PERIOD_LIMIT)->EnableWindow(FALSE);
			}
		}

		//End.

		UpdateData(FALSE);
	}

	return bChanged;
}

void CTaskPropertyPage::OnCanRedo()
{
	if (m_BtnCanRedo.GetCheck() != BST_CHECKED)
	{
		m_BtnNeedRecord.SetCheck(BST_CHECKED);
		m_BtnNeedRecord.EnableWindow(FALSE);
	}
	else if (m_BtnCanRedoAfterFailure.GetCheck() == BST_CHECKED)
		m_BtnNeedRecord.EnableWindow(TRUE);
}

void CTaskPropertyPage::OnCanRedoAfterFailure()
{
	if (m_BtnCanRedoAfterFailure.GetCheck() != BST_CHECKED)
	{
		m_BtnNeedRecord.SetCheck(BST_CHECKED);
		m_BtnNeedRecord.EnableWindow(FALSE);
	}
	else if (m_BtnCanRedo.GetCheck() == BST_CHECKED)
		m_BtnNeedRecord.EnableWindow(TRUE);
}

void CTaskPropertyPage::OnEditTimetable() 
{
	if (!m_pTask) return;
	CTimetableDlg dlg(m_pTask);
	dlg.DoModal();
}

void CTaskPropertyPage::OnDeliverTalk() 
{
	UpdateTalk(&m_pTask->m_DelvTaskTalk, NPC_GIVE_TASK);
}

void CTaskPropertyPage::OnShowTalk()
{
	UpdateTalk(&m_pTask->m_UnqualifiedTalk, NPC_TALK);
}

void CTaskPropertyPage::OnDeliverItemTalk()
{
	UpdateTalk(&m_pTask->m_DelvItemTalk, NPC_GIVE_TASK_MATTER);
}

void CTaskPropertyPage::OnAcceptedTalk() 
{
	UpdateTalk(&m_pTask->m_ExeTalk, NPC_TALK);
}

void CTaskPropertyPage::OnAwardTalk() 
{
	UpdateTalk(&m_pTask->m_AwardTalk, NPC_COMPLETE_TASK);
}

extern CVssOperation g_VSS;
extern bool FileIsReadOnly(AString szFilePathName);

void CTaskPropertyPage::UpdateTalk(talk_proc* pTalk, int ts)
{
#ifdef _TASK_CLIENT

	if (!m_pTask) return;

	CTalkModifyDlg dlg;
	dlg.m_bPreset = true;
	dlg.m_dwSvrToSel = ts;
	dlg.m_dwParamToSel = m_pTask->m_ID;
	dlg.m_dwParamToSel2 = m_pTask->GetTopTask()->m_ID;
	dlg.LoadTalkFromTalkProc(pTalk);
	dlg.DoModal();

#endif
}

void CTaskPropertyPage::OnSelMaxPoint() 
{
	if (!m_pTask) return;

	CTaskRegionDlg dlg(m_pTask->m_pDelvRegion,m_pTask->m_ulDelvRegionCnt);

	dlg.DoModal();
}

extern bool sel_task_delv_npc(ATaskTempl* pTask);
extern bool sel_task_award_npc(ATaskTempl* pTask);

void CTaskPropertyPage::OnSelAwardNpc()
{
	if (!m_pTask) return;

	if (sel_task_award_npc(m_pTask))
		GetDlgItem(IDC_AWARD_NPC)->SetWindowText(GetBaseDataNameByID(m_pTask->m_ulAwardNPC));	
}

void CTaskPropertyPage::OnSelDelvNpc() 
{
	if (!m_pTask) return;

	if (sel_task_delv_npc(m_pTask))
		GetDlgItem(IDC_DELV_NPC)->SetWindowText(GetBaseDataNameByID(m_pTask->m_ulDelvNPC));
}

void CTaskPropertyPage::OnTransPoint() 
{
	if (!m_pTask) return;

	CVectorDlg dlg;
	dlg.m_fX = m_pTask->m_TransPt.x;
	dlg.m_fY = m_pTask->m_TransPt.y;
	dlg.m_fZ = m_pTask->m_TransPt.z;
	
	if (dlg.DoModal() == IDOK)
	{
		m_pTask->m_TransPt.x = dlg.m_fX;
		m_pTask->m_TransPt.y = dlg.m_fY;
		m_pTask->m_TransPt.z = dlg.m_fZ;
	}
}

void CTaskPropertyPage::OnEditChangeVal() 
{
	// TODO: Add your control notification handler code here
	CAwardChangeValue dlg(&m_pTask->m_ulChangeKeyCnt, &m_pTask->m_plChangeKey, &m_pTask->m_plChangeKeyValue, &m_pTask->m_pbChangeType);
	dlg.DoModal();
}

void CTaskPropertyPage::OnEditEnterBig() 
{
	if (!m_pTask) return;

	CTaskRegionDlg dlg(m_pTask->m_pEnterRegion,m_pTask->m_ulEnterRegionCnt);
	dlg.DoModal();
}

void CTaskPropertyPage::OnEditLeaveBig() 
{
	if (!m_pTask) return;

	CTaskRegionDlg dlg(m_pTask->m_pLeaveRegion,m_pTask->m_ulLeaveRegionCnt);
	dlg.DoModal();
}


void CTaskPropertyPage::OnEditFailTime() 
{
 	if (!m_pTask) return;
 	CAbsTimeDlg dlg;
	if(m_pTask->m_bAbsFail)
	{
		dlg.m_tmAbsTime = CTime(m_pTask->m_tmAbsFailTime.year,m_pTask->m_tmAbsFailTime.month,m_pTask->m_tmAbsFailTime.day, 0,0,0);
		dlg.m_iHour	= m_pTask->m_tmAbsFailTime.hour;
		dlg.m_iMinute = m_pTask->m_tmAbsFailTime.min;
	}

	if(dlg.DoModal() == IDOK)
	{
		m_pTask->m_tmAbsFailTime.year = dlg.m_tmAbsTime.GetYear();
		m_pTask->m_tmAbsFailTime.month = dlg.m_tmAbsTime.GetMonth();
		m_pTask->m_tmAbsFailTime.day = dlg.m_tmAbsTime.GetDay();
		m_pTask->m_tmAbsFailTime.hour = dlg.m_iHour;
		m_pTask->m_tmAbsFailTime.min = dlg.m_iMinute;

		m_pTask->m_bAbsFail = true;
	}
}

//  PQ任务 全局变量显示
void CTaskPropertyPage::OnPQValueDisplay() 
{
	CPQGobalValue dlg(m_pTask);
	dlg.DoModal();
}

//  PQ子任务 属性编辑
void CTaskPropertyPage::OnPQSubTaskProp() 
{
	CPQSubTaskProp dlg(m_pTask);
	dlg.DoModal();
}

void CTaskPropertyPage::OnAutoDelvChecked()
{
	bEnable = !bEnable;
	GetDlgItem(IDC_WHETHER_PROMPT)->EnableWindow(bEnable);
	if (!bEnable)
	{
		m_BtnPrompt.SetCheck(BST_UNCHECKED);
	}
}

void CTaskPropertyPage::OnAccountLimitChecked()
{
	bool bEnable = ((CButton*)GetDlgItem(IDC_ACCOUNT_TASK_LIMIT))->GetCheck() == 1;
	GetDlgItem(IDC_TASK_PERIOD_LIMIT)->EnableWindow(bEnable);
	GetDlgItem(IDC_ROLE_TASK_LIMIT)->EnableWindow(!bEnable);
}
void CTaskPropertyPage::OnRoleLimitChecked()
{
	bool bEnable = ((CButton*)GetDlgItem(IDC_ROLE_TASK_LIMIT))->GetCheck() == 1;
	GetDlgItem(IDC_TASK_PERIOD_LIMIT)->EnableWindow(bEnable);
	GetDlgItem(IDC_ACCOUNT_TASK_LIMIT)->EnableWindow(!bEnable);
}
