// Filename	: DlgTask.cpp
// Creator	: Tom Zhou
// Date		: October 12, 2005

#include "AFI.h"
#include "AUIDef.h"
#include "AUICTranslate.h"
#include "DlgTask.h"
#include "DlgWorldMap.h"
#include "EC_HostPlayer.h"
#include "EC_Game.h"
#include "EC_Global.h"
#include "EC_GameRun.h"
#include "EC_GameUIMan.h"
#include "EC_Manager.h"
#include "EC_TaskInterface.h"
#include "TaskTemplMan.h"
#include "elementdataman.h"
#include "EC_IvtrItem.h"
#include "EC_IvtrEquip.h"
#include "EC_Resource.h"
#include "EC_FixedMsg.h"
#include "EC_UIManager.h"
#include "EC_Utility.h"
#include "DlgTreasureMap.h"
#include "DlgTaskTrace.h"
#include "DlgWikiRecipe.h"
#include "DlgAutoHelp.h"
#include "DlgAward.h"
#include "EC_UIHelper.h"
#include "EC_QuickBuyActivity.h"
#include "EC_QuickBuyPopMan.h"
#include "EC_AutoTeamConfig.h"
#include "EC_World.h"
#include "EC_GameSession.h"
#include "EC_ProfConfigs.h"

#define new A_DEBUG_NEW

AUI_BEGIN_COMMAND_MAP(CDlgTask, CDlgBase)

AUI_ON_COMMAND("searchquest",	OnCommand_searchquest)
AUI_ON_COMMAND("havequest",		OnCommand_havequest)
AUI_ON_COMMAND("Btn_ShowTrace",	OnCommand_showtrace)
AUI_ON_COMMAND("focus",			OnCommand_focus)
AUI_ON_COMMAND("abandon",		OnCommand_abandon)
AUI_ON_COMMAND("IDCANCEL",		OnCommand_CANCEL)
AUI_ON_COMMAND("Btn_FinishTask", OnCommand_FinishTask)
AUI_ON_COMMAND("Btn_GotoNPC",	OnCommand_GotoNPC)

AUI_END_COMMAND_MAP()

AUI_BEGIN_EVENT_MAP(CDlgTask, CDlgBase)

AUI_ON_EVENT("Tv_Quest",		WM_LBUTTONDOWN,		OnEventLButtonDown_Tv_Quest)
AUI_ON_EVENT("Txt_Content",	WM_LBUTTONDOWN,		OnEventLButtonDown_Txt_QuestItem)
AUI_ON_EVENT("Txt_Content",	WM_MOUSEMOVE,		OnEventMouseMove_Txt_QuestItem)
AUI_ON_EVENT("Txt_QuestItem",	WM_LBUTTONDOWN,		OnEventLButtonDown_Txt_QuestItem)
AUI_ON_EVENT("Txt_QuestItem",	WM_MOUSEMOVE,		OnEventMouseMove_Txt_QuestItem)
AUI_ON_EVENT("Img_Item*",	WM_LBUTTONDOWN,		OnEventLButtonDown_Award_Item)
AUI_END_EVENT_MAP()

abase::vector<int> CDlgTask::m_vecTasksUnFinish;
abase::vector<int> CDlgTask::m_vecTasksCanFinish;
CECGame::ObjectCoords CDlgTask::m_TargetCoord;
ACString CDlgTask::m_strTraceName;
abase::hash_map<int, const MINE_ESSENCE*> CDlgTask::m_TaskMines;

//////////////////////////////////////////////////////////////////////////
// CDlgTask
//////////////////////////////////////////////////////////////////////////
CDlgTask::CDlgTask()
{
	m_idLastTask = -2;
	m_iType = 0;

	m_idSelTask = 0;
	m_bTraceNew = true;
	m_bShowTrace = false;
	m_vecTasksUnFinish.clear();

	m_pQuickBuyTrigger = NULL;
}

CDlgTask::~CDlgTask()
{}

bool CDlgTask::OnInitDialog()
{
	m_pTxt_QuestNO = (PAUILABEL)GetDlgItem("Txt_QuestNO");
	m_pTv_Quest = (PAUITREEVIEW)GetDlgItem("Tv_Quest");
	m_pTxt_Content = dynamic_cast<PAUITEXTAREA>(GetDlgItem("Txt_Content"));
	m_pTxt_QuestItem = dynamic_cast<PAUITEXTAREA>(GetDlgItem("Txt_QuestItem"));
	m_pBtn_Abandon = (PAUISTILLIMAGEBUTTON)GetDlgItem("Btn_Abandon");
	m_pTxt_BaseAward = (PAUILABEL)GetDlgItem("Txt_BaseAward");
	m_pBtn_SearchQuest = (PAUISTILLIMAGEBUTTON)GetDlgItem("Btn_SearchQuest");
	m_pBtn_HaveQuest = (PAUISTILLIMAGEBUTTON)GetDlgItem("Btn_HaveQuest");
	m_pBtn_bShowTrace = (PAUISTILLIMAGEBUTTON)GetDlgItem("Btn_ShowTrace");
	m_pBtn_FinishTask = (PAUISTILLIMAGEBUTTON)GetDlgItem("Btn_FinishTask");
	m_pBtn_GotoNPC = (PAUISTILLIMAGEBUTTON)GetDlgItem("Btn_GotoNPC");
	if (m_pBtn_FinishTask)
		m_pBtn_FinishTask->Show(false);
	if (m_pBtn_GotoNPC)
		m_pBtn_GotoNPC->Show(false);

	if(m_pBtn_HaveQuest) m_pBtn_HaveQuest->SetPushed(true);
	PAUIOBJECT pObj = GetDlgItem("Btn_TreasureMap");
	if (pObj)
		pObj->Enable(false);

	// calc the actual size
	memset(m_pImg_Item, 0, sizeof(m_pImg_Item));
	for(m_ImgCount = 0; m_ImgCount < CDLGTASK_AWARDITEM_MAX ; m_ImgCount++ )
	{
		char szText[20];
		sprintf(szText, "Img_Item%d", m_ImgCount);
		m_pImg_Item[m_ImgCount] = (PAUIIMAGEPICTURE)GetDlgItem(szText);
		if(m_pImg_Item[m_ImgCount] == NULL)
		{
			break;
		}
	}
	pObj = GetDlgItem("Img_New");
	if (pObj) pObj->Show(false);

	if( m_pTxt_QuestNO ) m_pTxt_QuestNO->SetText(_AL("0"));
	
	m_TaskTraceCounter.SetPeriod(950);
	return true;
}

void CDlgTask::OnCommand_havequest(const char * szCommand)
{
	m_iType = 0;
	if(GetDlgItem("Lab_Trace")) GetDlgItem("Lab_Trace")->Show(true);
	if(m_pBtn_bShowTrace) m_pBtn_bShowTrace->Show(true);
	GetDlgItem("Btn_Abandon")->Show(true);
	GetDlgItem("Btn_Focus")->Show(true);
	GetDlgItem("Lab_QuestNO")->Show(true);
	GetDlgItem("Txt_QuestNO")->Show(true);
	m_pBtn_SearchQuest->SetPushed(false);
	m_pBtn_HaveQuest->SetPushed(true);
	UpdateTask();
}

void CDlgTask::OnCommand_searchquest(const char * szCommand)
{
	if (m_szName != "Win_Quest") return;
	m_iType = 1;
	PAUIOBJECT pObj = GetDlgItem("Img_New");
	if (pObj && IsShow()) pObj->Show(false);
	if(GetDlgItem("Lab_Trace")) GetDlgItem("Lab_Trace")->Show(false);
	if(m_pBtn_bShowTrace) m_pBtn_bShowTrace->Show(false);
	GetDlgItem("Btn_Abandon")->Show(false);
	GetDlgItem("Btn_Focus")->Show(false);
	GetDlgItem("Lab_QuestNO")->Show(false);
	GetDlgItem("Txt_QuestNO")->Show(false);
	m_pBtn_SearchQuest->SetPushed(true);
	m_pBtn_HaveQuest->SetPushed(false);
	SearchForTask();
}

void CDlgTask::OnCommand_showtrace(const char * szCommand)
{
	m_bShowTrace = !m_bShowTrace;

	// hide the trace window
	PAUIDIALOG pTrace = m_pAUIManager->GetDialog("Win_QuestMinion");
	if (pTrace) {
		pTrace->Show(m_bShowTrace);
	}
}

void CDlgTask::OnCommand_focus(const char * szCommand)
{
	PAUITREEVIEW pTree = m_pTv_Quest;
	P_AUITREEVIEW_ITEM pItem = pTree->GetSelectedItem();
	if( !pItem || pTree->GetParentItem(pItem) == pTree->GetRootItem())
		return;

	int idTask = pTree->GetItemData(pItem);
	SwitchTaskTrace(idTask);
}

void CDlgTask::SwitchTaskTrace(int idTask)
{
	CDlgTaskTrace* pDlg = 
		dynamic_cast<CDlgTaskTrace*>(m_pAUIManager->GetDialog("Win_QuestMinion"));
	if (pDlg) pDlg->SetBtnUnTraceY(-1, 0);

	if (IsPQTaskOrSubTask(idTask))
		return;

	if(!IsTaskTraceable(idTask))
		return;

	CECTaskInterface *pTask = GetHostPlayer()->GetTaskInterface();
	bool bFinishedTask = pTask->CanFinishTask(idTask);

	abase::vector<int>& vec_task = bFinishedTask ? m_vecTasksCanFinish : m_vecTasksUnFinish;
	for(int i = 0; i < (int)vec_task.size(); i++ )
	{
		if( idTask != vec_task[i] )
			continue;
		
		vec_task.erase(vec_task.begin() + i);
		return;
	}

	// if not found then added to queue
	vec_task.push_back(idTask);
}
void CDlgTask::OnCommand_abandon(const char * szCommand)
{
	PAUITREEVIEW pTree = m_pTv_Quest;
	P_AUITREEVIEW_ITEM pItem = pTree->GetSelectedItem();

	if( pItem && pTree->GetParentItem(pItem) != pTree->GetRootItem() )
	{
		PAUIDIALOG pMsgBox;

		m_pAUIManager->MessageBox("Game_AbortTask", m_pAUIManager->GetStringFromTable(236),
			MB_OKCANCEL, A3DCOLORRGBA(255, 255, 255, 160), &pMsgBox);
		pMsgBox->SetData(pTree->GetItemData(pItem));
	}
}

void CDlgTask::OnCommand_CANCEL(const char * szCommand)
{
	Show(false);
}

extern CECStringTab _task_err;
bool CDlgTask::UpdateTask(int idTask)
{
	if( m_szName != "Win_Quest" || m_iType != 0)
	{
		return true;
	}

	ATaskTemplMan *pMan = GetGame()->GetTaskTemplateMan();
	CECTaskInterface *pTask = GetHostPlayer()->GetTaskInterface();

	if (pTask == NULL) return false;

	PAUITEXTAREA pTextDesc = m_pTxt_Content;
	PAUITEXTAREA pTextItem = m_pTxt_QuestItem;
	
	ACString strNewTextItem = _AL("");
	ACString strNewHintItem = _AL("");
	bool bLastTaskChanged(false);

	PAUIOBJECT pObj = GetDlgItem("Txt_Contribution");
	if (pObj) {
		ACString strText;
		strText.Format(_AL("%d"), GetHostPlayer()->GetWorldContribution());
		pObj->SetText(strText);
	}

	if( idTask >= 0 )
	{
		ATaskTempl *pTemp = pMan->GetTaskTemplByID(idTask);
		if( pTemp )
		{
			if( idTask != m_idLastTask )
			{
				pTextDesc->SetText(FormatTaskText(pTemp->GetDescription(), pTextDesc->GetColor()));
				m_idLastTask = idTask;
				bLastTaskChanged = true;
			}
			m_pBtn_Abandon->Enable(pMan->CanGiveUpTask(idTask));

			// Get info
			Task_State_info tsi;
			pTask->GetTaskStateInfo(idTask, &tsi);
			
			// Clear first
			strNewTextItem = _AL("");

			// Base desc
			UpdateTaskBaseDesc(strNewTextItem, tsi);

			// Award NPC
			int nANPC = pTemp->GetAwardNPC();
			UpdateAwardNPC(strNewTextItem, nANPC);

			// Complete condition
			UpdateCompleteCondition(strNewTextItem, strNewHintItem, tsi);

			// Wanted Item
			UpdateItemWanted(strNewTextItem, tsi, idTask);

			// Treasure Map
			UpdateTreasureMap(strNewTextItem);

			// Task Confirm
			UpdateTaskConfirm(idTask, pTemp->m_enumFinishType == enumTFTConfirm);

			// Award
			Task_Award_Preview award;
			pTask->GetTaskAwardPreview(idTask, &award);

			UpdateBaseAward(award);

			UpdateItemAward(award);

			PAUIOBJECT pObj = GetDlgItem("Btn_TreasureMap");
			if (pObj)
			{
				if (pTemp->m_enumMethod == enumTMReachTreasureZone)
					pObj->Enable(true);
				else
					pObj->Enable(false);
			}

		}
	}
	else
	{
		ClearContent(true);

		if (m_pBtn_FinishTask)
			m_pBtn_FinishTask->Show(false);
		
		for(size_t i = 0; i < pTask->GetTaskCount(); i++ )
		{
			int id = pTask->GetTaskId(i);
			AddTaskNode(id);
		}
		SortTaskNodeByType();
		ACString strTemp;
		ActiveTaskList* pLst = (ActiveTaskList*)pTask->GetActiveTaskList();
		int iMaxTaskCount = pLst->GetMaxSimultaneousCount();
		strTemp.Format(_AL("%d/%d"), pTask->GetTaskCount(), iMaxTaskCount);
		m_pTxt_QuestNO->SetText(strTemp);
	}

	GetGameUIMan()->ReplaceColor(&strNewTextItem, A3DCOLORRGB(255, 255, 255), pTextItem->GetColor());
	SetTextItemText(strNewTextItem, pMan->GetTaskTemplByID(idTask) && !bLastTaskChanged, strNewHintItem);

	return true;
}

void CDlgTask::InsertTaskChildren(P_AUITREEVIEW_ITEM pRoot,
	unsigned long idTask, bool bExpand, bool bKey)
{
	int nPos;
	unsigned long id;
	ATaskTempl *pTemp;
	P_AUITREEVIEW_ITEM pItem;
	PAUITREEVIEW pTreeTask = m_pTv_Quest;
	ATaskTemplMan *pMan = GetGame()->GetTaskTemplateMan();
	CECTaskInterface *pTask = GetHostPlayer()->GetTaskInterface();

	nPos = pTask->GetFirstSubTaskPosition(idTask);
	while( nPos > -1 )
	{
		id = pTask->GetNextSub(nPos);
		pTemp = pMan->GetTaskTemplByID(id);
		if( !pTemp ) continue;
//		pItem = pTreeTask->InsertItem(pTemp->GetName(), pRoot);
		pItem = pTreeTask->InsertItem(CDlgTask::GetTaskNameWithColor(pTemp), pRoot);
		if( pTemp->IsKeyTask() )
			pTreeTask->SetItemTextColor(pItem, A3DCOLORRGB(255, 203, 74));
		if( bExpand ) pTreeTask->Expand(pItem, AUITREEVIEW_EXPAND_EXPAND);
		pTreeTask->SetItemData(pItem, id);
		if( (int)id == m_idSelTask )
		{
			pTreeTask->SelectItem(pItem);
			m_pBtn_Abandon->Enable(true);
			UpdateTask(id);
		}
		InsertTaskChildren(pItem, id, bExpand, bKey);
	}
}

bool CDlgTask::Tick(void)
{
	if( m_szName == "Win_Quest" && IsShow() )
	{
		PAUITREEVIEW pTree = m_pTv_Quest;
		P_AUITREEVIEW_ITEM pItem = pTree->GetSelectedItem();

		if( pItem )
		{
			for( size_t i = 0; i < m_ImgCount; i++ )
				m_pImg_Item[i]->Show(false);
			m_pTxt_BaseAward->Show(false);

			if( pTree->GetParentItem(pItem) != pTree->GetRootItem() )
			{
				if (m_iType == 0)
				{
					UpdateTask(pTree->GetItemData(pItem));
				}
				else if (m_iType == 1)
				{
					SearchForTask(pTree->GetItemData(pItem));
				}
			}
			else
			{
				m_idLastTask = -2;
				m_pTxt_Content->SetText(_AL(""));
				m_pTxt_QuestItem->SetText(_AL(""));
				m_pBtn_Abandon->Enable(false);
				UpdateTaskConfirm(0, false);
				PAUIOBJECT pObj = GetDlgItem("Btn_TreasureMap");
				if (pObj)
					pObj->Enable(false);
			}
		}
		UpdateGotoNPC();
	}

	return CDlgBase::Tick();
}

void CDlgTask::SyncTrace(void* pData, bool fromServer)
{
	CECGameUIMan::P_USER_LAYOUT pul = (CECGameUIMan::P_USER_LAYOUT)pData;
	if(fromServer)
	{
		m_vecTasksUnFinish.clear();
		m_vecTasksCanFinish.clear();
		
		DWORD dwTraceMask = pul->dwTraceMask;
		CECTaskInterface *pTask = GetHostPlayer()->GetTaskInterface();
		for(int i = 0; i < (int)pTask->GetTaskCount(); i++ )
		{
			if( (dwTraceMask & (1 << i)) )
			{
				TraceTask(pTask->GetTaskId(i));
			}
		}

		// store m_bShowTrace flag instead of bTraceAll flag
		m_bShowTrace = pul->bTraceAll;
	}
	else
	{
		pul->bTraceAll = m_bShowTrace;

		DWORD dwTraceMask = 0;
		size_t i = 0;
		for(i=0;i<m_vecTasksCanFinish.size();i++)
		{
			int index = GetTaskIndex(m_vecTasksCanFinish[i]);
			if(index >= 0) dwTraceMask |= (1 << index);
		}
		for(i=0;i<m_vecTasksUnFinish.size();i++)
		{
			int index = GetTaskIndex(m_vecTasksUnFinish[i]);
			if(index >= 0) dwTraceMask |= (1 << index);
		}

		pul->dwTraceMask = dwTraceMask;
	}
}

void CDlgTask::TickTaskTrace()
{
	if (m_TaskTraceCounter.IncCounter(GetGame()->GetRealTickTime())) {
		m_TaskTraceCounter.Reset();
		RefreshTaskTrace();
	} 
}

void CDlgTask::RefreshTaskTrace()
{
	CDlgTaskTrace* pDlgTaskTrace = 
		dynamic_cast<CDlgTaskTrace*>(m_pAUIManager->GetDialog("Win_QuestMinion"));
	if (pDlgTaskTrace == NULL) 
		return;
	if (m_pBtn_bShowTrace && !m_bShowTrace)
		return;

	CDlgTaskTrace::ShowType showType = pDlgTaskTrace->GetShowType();

	if (showType == CDlgTaskTrace::ST_TRACED || showType == CDlgTaskTrace::ST_TITLE) {
		CECHostPlayer* pHost = GetHostPlayer();
		CECTaskInterface* pTask = pHost->GetTaskInterface();
		// 检查任务是否从可完成状态变化到不可完成状态
		if( m_vecTasksCanFinish.size() > 0 )
		{
			for(int i = 0; i < (int)m_vecTasksCanFinish.size(); i++ )
			{
				int idTask = m_vecTasksCanFinish[i];
				bool bCanFinish = pTask->CanFinishTask(idTask);
				bool bHasTask = pTask->HasTask(idTask);
				if(!bHasTask || !bCanFinish)
				{
					// 如果任务没有了，或者状态变化要移除任务
					m_vecTasksCanFinish.erase(m_vecTasksCanFinish.begin() + i);
					i--;
					if (bHasTask && !bCanFinish) {
						if (std::find(m_vecTasksUnFinish.begin(), m_vecTasksUnFinish.end(), idTask) == m_vecTasksUnFinish.end())
							m_vecTasksUnFinish.push_back(idTask);
					}
				}
			}
		}
		// 检查任务是否从不可完成状态变化到可完成状态
		if( m_vecTasksUnFinish.size() > 0 )
		{
			for(int i = 0; i < (int)m_vecTasksUnFinish.size(); i++ )
			{
				int idTask = m_vecTasksUnFinish[i];
				bool bCanFinish = pTask->CanFinishTask(idTask);
				bool bHasTask = pTask->HasTask(idTask);
				if(!bHasTask || bCanFinish)
				{
					// 如果任务没有了，或者状态变化要移除任务
					m_vecTasksUnFinish.erase(m_vecTasksUnFinish.begin() + i);
					i--;
					if (bHasTask && bCanFinish) {
						if (std::find(m_vecTasksCanFinish.begin(), m_vecTasksCanFinish.end(), idTask) == m_vecTasksCanFinish.end())
							m_vecTasksCanFinish.push_back(idTask);
					}
				}
			}
		}
		std::vector<int> tasks;
		tasks.resize(m_vecTasksCanFinish.size() + m_vecTasksUnFinish.size());
		tasks.assign(m_vecTasksCanFinish.begin(), m_vecTasksCanFinish.end());
		tasks.insert(tasks.begin(), m_vecTasksUnFinish.begin(), m_vecTasksUnFinish.end());

		// refresh trace dialog by searched information
		typedef abase::vector<int> ATask;
		typedef abase::hash_map<DWORD, ATask> MTasks;
		
		//  构造已接称号任务列表
		TaskTemplLst title_list;
		ATask titletasks;
		if (pDlgTaskTrace->GetShowType() == CDlgTaskTrace::ST_TITLE) 
		{
			GetGame()->GetTaskTemplateMan()->GetTitleTasks(pTask, title_list);
			for (size_t i = 0; i < title_list.size(); ++i) {
				if (IsTaskTraceable(title_list[i]->m_ID))
					titletasks.push_back(title_list[i]->m_ID);
			}
		}
		
		//	使用已追踪列表、可接任务列表，更新任务追踪界面
		if (!(pHost->IsDead() || pHost->IsTrading() || 0 != pHost->GetBoothState()))
		{
			pDlgTaskTrace->RefreshTaskTrace(tasks.begin(), tasks.size(), titletasks.begin(), titletasks.size(), true);
		}
	} else if (showType == CDlgTaskTrace::ST_CONTRIBUTION) {
		pDlgTaskTrace->UpdateContributionTask();
	}
}

void CDlgTask::OnEventLButtonDown_Tv_Quest(WPARAM wParam, LPARAM lParam, AUIObject *pObj)
{
	POINT ptPos = pObj->GetPos();
	A3DVIEWPORTPARAM *p = m_pA3DEngine->GetActiveViewport()->GetParam();
	int x = GET_X_LPARAM(lParam) - ptPos.x - p->X;
	int y = GET_Y_LPARAM(lParam) - ptPos.y - p->Y;
	PAUITREEVIEW pTree = (PAUITREEVIEW)pObj;

	if( AUI_PRESS(VK_SHIFT) && m_iType == 0)
	{	
		P_AUITREEVIEW_ITEM pItem = pTree->HitTest(x, y);

		if( pItem ) OnCommand_focus("focus");
	}
	P_AUITREEVIEW_ITEM pItem = pTree->GetSelectedItem();
	int idTask(0);
	if( pItem && pTree->GetParentItem(pItem) != pTree->GetRootItem())
		idTask = pTree->GetItemData(pItem);

	if (idTask == 0) return;

	if (m_szName == "Win_Quest" && CDlgAutoHelp::IsAutoHelp())
	{
		if(pTree->GetHitArea(x,y) == AUITREEVIEW_RECT_FRAME)
			CDlgWikiShortcut::PopQuestWiki(GetGameUIMan(),idTask);
	}
	m_idSelTask = idTask;

}

const MINE_ESSENCE* CDlgTask::SearchTaskMine(int idTask)
{
	if(idTask <= 0)
	{
		return NULL;
	}

	if(m_TaskMines.empty())
	{
		elementdataman *pDataMan = g_pGame->GetElementDataMan();
		DATA_TYPE dataType = DT_INVALID;

		unsigned int idCandidate = pDataMan->get_first_data_id(ID_SPACE_ESSENCE, dataType);
		while(idCandidate)
		{
			if(dataType == DT_MINE_ESSENCE)
			{
				// got the mine essence
				const MINE_ESSENCE* pCur = (const MINE_ESSENCE*)pDataMan->get_data_ptr(
					idCandidate, ID_SPACE_ESSENCE, dataType);

				if(pCur && pCur->task_in)
				{
					m_TaskMines[pCur->task_in] = pCur;
				}
			}

			idCandidate = pDataMan->get_next_data_id(ID_SPACE_ESSENCE, dataType);
		}

		// avoid duplicated init
		m_TaskMines[0] = NULL;
	}

	MineMap::iterator itr = m_TaskMines.find(idTask);
	return itr != m_TaskMines.end() ? itr->second : NULL;
}

bool CDlgTask::SearchForTask(int idTask)
{
	if (m_iType != 1)
	{
		return true;
	}

	ATaskTemplMan *pMan = GetGame()->GetTaskTemplateMan();
	CECTaskInterface *pTask = GetHostPlayer()->GetTaskInterface();

	PAUITEXTAREA pTextDesc = m_pTxt_Content;
	PAUITEXTAREA pTextItem = m_pTxt_QuestItem;

	ACString strNewTextItem = _AL("");
	ACString strNewHintItem = _AL("");
	bool bLastTaskChanged(false);

	bool bQuestUI = m_szName == "Win_Quest";

	if( idTask >= 0 )
	{
		ATaskTempl *pTemp = pMan->GetTaskTemplByID(idTask);
		if( pTemp )
		{
			if( idTask != m_idLastTask )
			{
				pTextDesc->SetText(FormatTaskText(pTemp->GetDescription(), pTextDesc->GetColor()));
				m_idLastTask = idTask;
				bLastTaskChanged = true;
			}
			
			if (bQuestUI) {
				P_AUITREEVIEW_ITEM pItem = m_pTv_Quest->GetSelectedItem();
				int id = m_pTv_Quest->GetItemData(pItem);
				if (pMan->IsTaskToPush(id)) {
					ACString strItem(CDlgTask::GetTaskNameWithColor(pTemp));
					m_pTv_Quest->SetItemText(pItem, strItem);
				}
			}
			// Get info
			Task_State_info tsi;
			pTask->GetTaskStateInfo(idTask, &tsi, false);

			// Clear first
			strNewTextItem = _AL("");

			// Base desc
			UpdateTaskBaseDesc(strNewTextItem, tsi);

			// Deliver NPC
			UpdateDeliverNPC(strNewTextItem, pTemp->GetDeliverNPC());

			// Award NPC
			int nANPC = pTemp->GetAwardNPC();
			UpdateAwardNPC(strNewTextItem, nANPC);

			// Complete condition
			UpdateCompleteCondition(strNewTextItem, strNewHintItem, tsi);

			// Wanted Item
			UpdateItemWanted(strNewTextItem, tsi, idTask);

			// Award
			Task_Award_Preview award;
			pTask->GetTaskAwardPreview(idTask, &award, false);

			UpdateBaseAward(award);

			UpdateItemAward(award);
		}
		else
		{
			m_idLastTask = -2;
			m_pTxt_Content->SetText(_AL(""));
			m_pTxt_QuestItem->SetText(_AL(""));
		}
	}
	else
	{
		// zhangyitian 20140521 先将可接任务列表清空，再判断是否有可接任务
		// 修正了原先没有可接任务时，可接任务列表显示已接任务的问题
		ClearContent(false);

		TaskTemplLst ttl;
		pMan->GetAvailableTasks(pTask, ttl);
		if( ttl.size() <= 0 ) return true;

		

		for(size_t i = 0; i < ttl.size(); i++ )
		{
			int id = ttl[i]->GetID();
			AddTaskNode(id);
		}

		SortTaskNodeByType();

		ACString strTemp;
		ActiveTaskList* pLst = (ActiveTaskList*)pTask->GetActiveTaskList();
		int iMaxTaskCount = pLst->GetMaxSimultaneousCount();
		strTemp.Format(_AL("%d/%d"), pTask->GetTaskCount(), iMaxTaskCount);
		m_pTxt_QuestNO->SetText(strTemp);
	}

	GetGameUIMan()->ReplaceColor(&strNewTextItem, A3DCOLORRGB(255, 255, 255), pTextItem->GetColor());
	SetTextItemText(strNewTextItem, pMan->GetTaskTemplByID(idTask) && !bLastTaskChanged, strNewHintItem);

	return true;
}

bool CDlgTask::UpdateQuestView()
{
	return UpdateTask() && SearchForTask();
}

void CDlgTask::OnEventMouseMove_Txt_QuestItem(WPARAM wParam, LPARAM lParam, AUIObject *pObj)
{
	AUITEXTAREA_EDITBOX_ITEM Item;
	int x = GET_X_LPARAM(lParam);
	int y = GET_Y_LPARAM(lParam);
	GetItemLinkItemOn(x, y, pObj, &Item);
}

void CDlgTask::GetItemLinkItemOn(int x, int y, PAUIOBJECT pObj, AUITEXTAREA_EDITBOX_ITEM *pLink)
{
	PAUITEXTAREA pText = dynamic_cast<PAUITEXTAREA>(pObj);
	
	GetGame()->ChangeCursor(RES_CUR_NORMAL);
	if( 1 )
	{
		int i;
		abase::vector<AUITEXTAREA_EDITBOX_ITEM> &vecItemLink = pText->GetEditBoxItems();
		for( i = 0; i < (int)vecItemLink.size(); i++ )
		{
			if( (vecItemLink[i].m_pItem->GetType() == enumEIIvtrlItem ||
				vecItemLink[i].m_pItem->GetType() == enumEICoord) &&
				vecItemLink[i].rc.PtInRect(x, y) )
			{
				GetGame()->ChangeCursor(RES_CUR_HAND);
				*pLink = vecItemLink[i];
				break;
			}
		}
	}

	if (CDlgAutoHelp::IsAutoHelp())
	{
		GetGame()->ChangeCursor(RES_CUR_HAND);
	}
}

void CDlgTask::OnEventLButtonDown_Txt_QuestItem(WPARAM wParam, LPARAM lParam, AUIObject *pObj)
{
	int x = GET_X_LPARAM(lParam);
	int y = GET_Y_LPARAM(lParam);
	
	AUITEXTAREA_EDITBOX_ITEM Item;
	Item.m_pItem = NULL;
	GetItemLinkItemOn(x, y, pObj, &Item);
	bool bFind = false;
	
	if( Item.m_pItem != NULL )
	{
		if( Item.m_pItem->GetType() == enumEICoord )
		{
			if (IsTreasureMapSelected()){
				OnCommand_TreasureMap(NULL);
			}else{
				CECUIHelper::FollowCoord(Item.m_pItem, m_idSelTask);
			}
		}
	}

	//	与 Btn_GotoNPC、Btn_FinishTask 有遮挡关系
	ChangeFocus(NULL);
}

ACString CDlgTask::FormatTaskText(const ACHAR* szText, A3DCOLOR background)
{
	ACString result = CECUIHelper::FormatCoordText(szText);
	CECGameUIMan* pUIMan = g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan();
	pUIMan->ReplaceColor(&result, A3DCOLORRGB(255, 255, 255), background);
	return result;
}

void CDlgTask::SetTextItemText(const ACString &strNewTextItem, bool keepScrollPos, const ACString& strNewHintItem)
{
	PAUITEXTAREA pTextItem = m_pTxt_QuestItem;

	if (strNewTextItem != m_pTxt_QuestItem->GetText())
	{
		int oldFirstLine = pTextItem->GetFirstLine();
		
		pTextItem->SetText(strNewTextItem);
		pTextItem->SetHint(strNewHintItem);
		
		// Adjust scroll to its last position before text change
		if (keepScrollPos)
		{
			int newLines = pTextItem->GetLines();
			if (oldFirstLine<newLines)
			{
				while (pTextItem->GetFirstLine()<oldFirstLine)
				{
					int line1 = pTextItem->GetFirstLine();
					pTextItem->OnDlgItemMessage(WM_MOUSEWHEEL, MAKEWPARAM(0, -120), 0);
					if (line1 <= pTextItem->GetFirstLine())
						break;
				}
			}
		}
	}
}

bool CDlgTask::IsPQTaskOrSubTask(int idTask)
{
	ATaskTemplMan *pMan = GetGame()->GetTaskTemplateMan();
	ATaskTempl *pTemp = pMan->GetTaskTemplByID(idTask);
	return (pTemp!=NULL && (pTemp->m_bPQTask || pTemp->m_bPQSubTask));
}

bool CDlgTask::IsTreasureMapTask(int idTask)
{
	ATaskTemplMan *pMan = GetGame()->GetTaskTemplateMan();
	ATaskTempl *pTempl = pMan->GetTaskTemplByID(idTask);
	return (pTempl != NULL && pTempl->m_enumMethod == enumTMReachTreasureZone);
}

static bool IsQuestionTask(const ATaskTempl *pTemp){
	return pTemp->GetType() == enumTTQuestion;
}

bool CDlgTask::IsTaskTraceable(int idTask)
{
	ATaskTemplMan *pMan = GetGame()->GetTaskTemplateMan();
	ATaskTempl *pTemp = pMan->GetTaskTemplByID(idTask);
	
	// 答题任务不追踪
	if (IsQuestionTask(pTemp)) {
		return false;
	}

	Task_State_info tsi;
	CECTaskInterface *pTask = GetHostPlayer()->GetTaskInterface();
	pTask->GetTaskStateInfo(idTask, &tsi);
	
	bool bTrace = tsi.m_ulTimeLimit > 0 ||
		tsi.m_ulProtectTime > 0 ||
		tsi.m_ulNPCToProtect > 0 ||
		tsi.m_MonsterWanted[0].m_ulMonstersToKill > 0 ||
		tsi.m_PlayerWanted[0].m_ulPlayersToKill > 0 ||
		tsi.m_ItemsWanted[0].m_ulItemId > 0 ||
		tsi.m_ulWaitTime > 0 ||
		tsi.m_TaskCharArr.size()> 0 ||
		tsi.m_ulReachLevel ||
		tsi.m_ulReachRealm ||
		tsi.m_ulReachReincarnation;

	// check the condition from template
	if(!bTrace)
	{
		if (pTemp->m_ulReachSiteCnt > 0 ||
			(pTemp->GetAwardNPC() > 0 && pTask->CanFinishTask(idTask))){
			bTrace = true;
		}
	}

	return bTrace;
}

A3DCOLOR CDlgTask::GetTaskColor(const ATaskTempl *pTempl){	
	DWORD nTaskType = pTempl->GetType();
	if (pTempl->m_DynTaskType){
		nTaskType = enumTTEvent;
	}
	return GetTaskColor(nTaskType);
}

A3DCOLOR CDlgTask::GetTaskColor(int idType)
{
	if (idType < enumTTDaily || idType >= enumTTEnd) {
		ASSERT(false && "wrong task type");
		return A3DCOLORRGB(255,255,255);
	}
	A3DCOLOR result;
	STRING_TO_A3DCOLOR(CECUIHelper::GetGameUIMan()->GetStringFromTable(idType - enumTTDaily + 3121), result);
	return result;
}

ACString CDlgTask::GetTaskNameWithOutColor(const ATaskTempl* pTempl) {
	return CECUIHelper::TrimColorPrefix(pTempl->m_szName);
}

ACString CDlgTask::GetTaskNameWithColor(const ATaskTempl* pTempl) {
	if (pTempl->GetType() == enumTTQiShaList && pTempl->m_szName[0] == '^') {
		// 如果是七杀榜任务且已经加了颜色，则颜色不变
		return pTempl->m_szName;
	}
	ACString strTaskName = GetTaskNameWithOutColor(pTempl);
	ACString strColorPreFix = A3DCOLOR_TO_STRING(GetTaskColor(pTempl));
	strTaskName = strColorPreFix + strTaskName;
	return strTaskName;
}

bool CDlgTask::OnChangeLayout(PAUIOBJECT lhs, PAUIOBJECT rhs)
{
	if(lhs && rhs)
	{
		AUIOBJECT_SETPROPERTY lhsProp;
		AUIOBJECT_SETPROPERTY rhsProp;

		if( lhs == m_pTxt_Content ||
			lhs == m_pTxt_QuestItem ||
			lhs == m_pTxt_BaseAward )
		{
			// swap this property
			AUI_SWAP_RES("Text Color");

			// refresh the content
			ACString szText = lhs->GetText();
			GetGameUIMan()->ReplaceColor(&szText, rhs->GetColor(), lhs->GetColor());
			lhs->SetText(szText);
		}
	}

	return true;
}

void CDlgTask::OnChangeLayoutEnd(bool bAllDone)
{
	// refresh
}

void CDlgTask::SetTaskText(PAUIOBJECT pObj, ACString* pStr)
{
	GetGameUIMan()->ReplaceColor(pStr, A3DCOLORRGB(255, 255, 255), pObj->GetColor());
	pObj->SetText(*pStr);
}

int CDlgTask::GetTaskIndex(int idTask)
{
	// find the task index in task list
	if(idTask <= 0)
	{
		return -1;
	}

	CECTaskInterface *pTask = GetHostPlayer()->GetTaskInterface();
	for(int i = 0; i < (int)pTask->GetTaskCount(); i++ )
	{
		int curTask = pTask->GetTaskId(i);
		if(idTask == curTask ||	pTask->CheckParent(curTask, idTask))
			return i;
	}

	return -1;
}

bool CDlgTask::TraceTask(int idTask)
{
	if (IsPQTaskOrSubTask(idTask))
		return false;

	CECTaskInterface *pTask = GetHostPlayer()->GetTaskInterface();
	bool bFinishedTask = pTask->CanFinishTask(idTask);
	abase::vector<int>& vec_task = bFinishedTask ? m_vecTasksCanFinish : m_vecTasksUnFinish;
	bool traced = false;
	if (IsTaskTraceable(idTask))
	{
		if (std::find(vec_task.begin(), vec_task.end(), idTask) == vec_task.end())
			vec_task.push_back(idTask);
		traced = true;
	}
	
	// FocusChildren
	typedef std::vector<unsigned long> Children;
	Children children;
	int nPos = pTask->GetFirstSubTaskPosition(idTask);
	while( nPos > -1 )
	{
		unsigned long idSub = pTask->GetNextSub(nPos);
		children.push_back(idSub);
	}

	//	逆向添加各子任务，使得任务追踪里显示顺序与任务面板中相同
	for (Children::reverse_iterator it = children.rbegin(); it != children.rend(); ++ it)
	{
		unsigned long idSub = *it;
		traced |= TraceTask(idSub);
	}	

	return traced;
}

void CDlgTask::OnCommand_TreasureMap(const char * szCommand)
{
	int idTask = GetSelectedTaskFromUI();
	if (!IsTreasureMapTask(idTask)){
		return;
	}
	ATaskTempl* pTempl = GetGame()->GetTaskTemplateMan()->GetTaskTemplByID(idTask);

	CECTaskInterface *pTask = GetHostPlayer()->GetTaskInterface();
	ActiveTaskEntry* pEntry = static_cast<ActiveTaskList*>(pTask->GetActiveTaskList())->GetEntry(idTask);

	unsigned short uTreasureLocIndex = pEntry->m_iUsefulData1 & 0xFFFF;
	float fTreasureLowerLeftX = pTempl->m_TreasureStartZone.x + (uTreasureLocIndex % pTempl->m_ucZonesNumX - 1) * pTempl->m_ucZoneSide;
	float fTreasureLowerLeftZ = pTempl->m_TreasureStartZone.z + (uTreasureLocIndex / pTempl->m_ucZonesNumX) * pTempl->m_ucZoneSide;

	char usMapX = 0;
	usMapX |= (pEntry->m_iUsefulData1 & 0x00FF0000) >> 16;
	char usMapZ = 0;
	usMapZ |= (pEntry->m_iUsefulData1 & 0xFF000000) >> 24;
	float fMapLowerLeftX = pTempl->m_TreasureStartZone.x + usMapX * pTempl->m_ucZoneSide;
	FLOAT fMapLowerLeftZ = pTempl->m_TreasureStartZone.z + usMapZ * pTempl->m_ucZoneSide;

	CDlgTreasureMap* pDlg = dynamic_cast<CDlgTreasureMap*> (m_pAUIManager->GetDialog("Win_TreasureMap"));

	pDlg->ShowTreasureMap((int)fMapLowerLeftX, (int)fMapLowerLeftZ, (int)fTreasureLowerLeftX, (int)fTreasureLowerLeftZ,pTempl->m_ucZoneSide,TASK_TREASURE_MAP_SIDE_MULTIPLE / 2);
}

void CDlgTask::SetTraceObjects(const ObjectCoords& objs, const ACString& name)
{
	m_TargetCoord = objs;
	m_strTraceName = name;
}

void CDlgTask::UpdateBaseAward(const Task_Award_Preview& award)
{
	ACString strTemp;
	ACString strText;

	// insert a newline when necessary
	int colCount = 0;
	const int col = 3;

	if( award.m_ulGold > 0 )
	{
		strTemp.Format(GetStringFromTable(3201), award.m_ulGold);
		strText += strTemp;
		if( (++colCount) % col == 0 ) strText += _AL("\r");
	}
	if( award.m_ulExp > 0 )
	{
		strTemp.Format(GetStringFromTable(3202), award.m_ulExp);
		strText += strTemp;
		if( (++colCount) % col == 0 ) strText += _AL("\r");
	}
	if( award.m_ulSP > 0 )
	{
		strTemp.Format(GetStringFromTable(3203), award.m_ulSP);
		strText += strTemp;
		if( (++colCount) % col == 0 ) strText += _AL("\r");
	}
	if( award.m_ulRealmExp > 0 )
	{
		strTemp.Format(GetStringFromTable(3207), award.m_ulRealmExp);
		strText += strTemp;
		if( (++colCount) % col == 0 ) strText += _AL("\r");
	}
	if( award.m_iForceContrib > 0 )
	{
		strTemp.Format(GetStringFromTable(3205), award.m_iForceContrib);
		strText += strTemp;
		if( (++colCount) % col == 0 ) strText += _AL("\r");
	}
	if( award.m_iForceRepu > 0 )
	{
		strTemp.Format(GetStringFromTable(3206), award.m_iForceRepu);
		strText += strTemp;
		if( (++colCount) % col == 0 ) strText += _AL("\r");
	}

	if( strText != _AL("") )
	{
		SetTaskText(m_pTxt_BaseAward, &strText);
		m_pTxt_BaseAward->Show(true);
	}
}

void CDlgTask::UpdateItemAward(const Task_Award_Preview& award)
{
	bool bShowItem = false;
	if( award.m_bHasItem )
	{
		if( !award.m_bItemKnown )
		{
			if( m_pTxt_BaseAward->IsShow() )
			{
				ACString strAward = GetStringFromTable(3204) + ACString(_AL("\r"));
				strAward += m_pTxt_BaseAward->GetText();
				SetTaskText(m_pTxt_BaseAward, &strAward);
			}
			else
			{
				SetTaskText(m_pTxt_BaseAward, &ACString(GetStringFromTable(3204)));
				m_pTxt_BaseAward->Show(true);
			}
		}
		else
		{
			for( DWORD i = 0; i < m_ImgCount ; i++ )
			{
				if( i < award.m_ulItemTypes )
				{
					CECIvtrItem *pItem = CECIvtrItem::CreateItem(award.m_ItemsId[i], 0, award.m_ItemsNum[i]);
					if( !pItem ) continue;

					AString strFile;
					af_GetFileTitle(pItem->GetIconFile(), strFile);
					strFile.MakeLower();
					m_pImg_Item[i]->SetCover(GetGameUIMan()->m_pA2DSpriteIcons[CECGameUIMan::ICONS_INVENTORY],
						GetGameUIMan()->m_IconMap[CECGameUIMan::ICONS_INVENTORY][strFile]);

					m_pImg_Item[i]->SetColor(
						(pItem->IsEquipment() && ((CECIvtrEquip *)pItem)->IsDestroying())
						? A3DCOLORRGB(128, 128, 128)
						: A3DCOLORRGB(255, 255, 255));

					AUICTranslate trans;
					pItem->GetDetailDataFromLocal();
					const ACHAR *pszDesc = pItem->GetDesc(CECIvtrItem::DESC_REWARD);
					if( pszDesc )
						m_pImg_Item[i]->SetHint(trans.Translate(pszDesc));
					else
						m_pImg_Item[i]->SetHint(pItem->GetName());
					m_pImg_Item[i]->SetData(pItem->GetTemplateID());

					delete pItem;

					ACString strNum;
					strNum.Format(_AL("%d"), award.m_ItemsNum[i]);
					m_pImg_Item[i]->SetText(strNum);
					m_pImg_Item[i]->Show(true);
					bShowItem = true;
				}
			}
		}
	}

	// adjust the correct pos
	POINT pos = m_pImg_Item[0]->GetPos(true);
	if(bShowItem)
	{
		SIZE sz = m_pImg_Item[0]->GetSize();
		const int iMargin = int(2 * GetGameUIMan()->GetWindowScale() + 0.5f);
		m_pTxt_BaseAward->SetPos(pos.x, pos.y + sz.cy + iMargin);
	}
	else
	{
		m_pTxt_BaseAward->SetPos(pos.x, pos.y);
	}
}

void CDlgTask::UpdateTaskBaseDesc(ACString& strText, const Task_State_info& tsi)
{
	size_t i;
	for(i = 0; i < tsi.m_TaskCharArr.size(); i++)
	{
		strText += tsi.m_TaskCharArr[i];
		strText += _AL("\r");
	}
	if (tsi.m_ulErrCode)
	{
		const wchar_t* szMsg = _task_err.GetWideString(tsi.m_ulErrCode);

		if (szMsg)
		{
			strText += _AL("^ff0000");
			strText += szMsg;
			ACString strTemp;
			if (tsi.m_ulErrCode == TASK_AWARD_FAIL_LEVEL_CHECK)
				strTemp.Format(GetStringFromTable(7637), tsi.m_ulPremLevelMin);	
			else strTemp = GetStringFromTable(807);
			strText += strTemp;
			strText += _AL("\r^ffffff");
		}
	}

	int nSec;
	if( tsi.m_ulTimeLimit > 0 )
	{
		nSec = tsi.m_ulTimeLimit;
		strText += FormatTime(nSec, GetStringFromTable(245), 0);

		nSec = max(0, int(tsi.m_ulTimeLimit) - tsi.m_ulTimePassed);
		strText += FormatTime(nSec, GetStringFromTable(246), 0);
	}

	if( tsi.m_ulWaitTime > 0 )
	{
		nSec = max(0, int(tsi.m_ulWaitTime) - int(tsi.m_ulTimePassed));
		strText += FormatTime(nSec, GetStringFromTable(199), 0);
	}

	if( tsi.m_ulNPCToProtect > 0 )
	{
		DATA_TYPE DataType = DT_INVALID;
		elementdataman *pDataMan = GetGame()->GetElementDataMan();
		MONSTER_ESSENCE *pMonster = 
			(MONSTER_ESSENCE *)pDataMan->get_data_ptr(tsi.m_ulNPCToProtect, ID_SPACE_ESSENCE, DataType);
		
		if( DataType == DT_MONSTER_ESSENCE )
		{
			ACString strTemp;
			strTemp.Format(GetStringFromTable(257), pMonster->name);
			strText += strTemp;
		}

		nSec = tsi.m_ulProtectTime;
		strText += FormatTime(nSec, GetStringFromTable(258), 0);

		nSec = max(0, tsi.m_ulProtectTime - tsi.m_ulTimePassed);
		strText += FormatTime(nSec, GetStringFromTable(259), 0);
	}
}

void CDlgTask::UpdateDeliverNPC(ACString& strText, int nDNPC)
{
	//Deliver NPC
	if( nDNPC == 0 )
	{
		return;
	}

	DATA_TYPE dt = DT_INVALID;
	elementdataman *pDataMan = GetGame()->GetElementDataMan();
	NPC_ESSENCE *pNPC = 
		(NPC_ESSENCE *)pDataMan->get_data_ptr(nDNPC, ID_SPACE_ESSENCE, dt);
	if( dt != DT_NPC_ESSENCE || !pNPC )
	{
		return;
	}

	bool bFind = false;
	A3DVECTOR3 vPos = CECUIHelper::GetTaskObjectCoordinates(nDNPC, bFind);
	ACString strName;

	EditBoxItemBase item(enumEICoord);
	item.SetName(pNPC->name);
	ACHAR szPos[100];
	if (bFind) {
		a_sprintf(szPos, _AL("%f %f %f %d"), vPos.x, vPos.y, vPos.z, nDNPC);
		item.SetInfo(szPos);
		item.SetColor(A3DCOLORRGB(0, 255, 0));
		A3DPOINT2 p = CECUIHelper::GetShowPos(vPos.x, vPos.z);
		strName.Format(_AL("%s (%d, %d)"), (ACHAR)AUICOMMON_ITEM_CODE_START + item.Serialize(), p.x, p.y);
	} else strName = pNPC->name;

	strText += GetStringFromTable(7620);
	strText += strName;
	strText += _AL("\r");
}

A3DVECTOR3 CDlgTask::UpdateAwardNPC(ACString& strText, int nANPC)
{
	A3DVECTOR3 ret;
	ret.Set(0, 0, 0);

	//Award NPC
	if( nANPC == 0 )
	{
		return ret;
	}

	DATA_TYPE dt = DT_INVALID;
	elementdataman *pDataMan = GetGame()->GetElementDataMan();
	NPC_ESSENCE *pNPC = 
		(NPC_ESSENCE *)pDataMan->get_data_ptr(nANPC, ID_SPACE_ESSENCE, dt);

	if( dt != DT_NPC_ESSENCE || !pNPC)
	{
		return ret;
	}

	bool bFind = false;
	A3DVECTOR3 vPos = CECUIHelper::GetTaskObjectCoordinates(nANPC, bFind);
	ACString strName;
	
	EditBoxItemBase item(enumEICoord);
	item.SetName(pNPC->name);
	ACHAR szPos[100];
	if (bFind) {
		a_sprintf(szPos, _AL("%f %f %f %d"), vPos.x, vPos.y, vPos.z, nANPC);
		item.SetInfo(szPos);
		item.SetColor(A3DCOLORRGB(0, 255, 0));
		A3DPOINT2 p = CECUIHelper::GetShowPos(vPos.x, vPos.z);
		strName.Format(_AL("%s (%d, %d)"), (ACHAR)AUICOMMON_ITEM_CODE_START + item.Serialize(), p.x, p.y);
	} else strName = pNPC->name;

	strText += GetStringFromTable(7621);
	strText += strName;
	strText += _AL("\r");

	return ret;
}
void CDlgTask::UpdateCompleteCondition(ACString& strText, ACString& strHint, const Task_State_info& tsi)
{
	int i,iLevel;
	ACString strTemp, strColor;
	CECHostPlayer* pHost = GetHostPlayer();
	//Complete Conditions
	for( i = 0; i < MAX_MONSTER_WANTED; i++ )
	{
		if (tsi.m_MonsterWanted[i].m_ulMonsterId == 0)
			break;

		int id = tsi.m_MonsterWanted[i].m_ulMonsterId;

		if( tsi.m_MonsterWanted[i].m_ulMonstersKilled > 0 || tsi.m_MonsterWanted[i].m_ulMonstersToKill > 0 )
		{
			DATA_TYPE DataType = DT_INVALID;
			elementdataman *pDataMan = GetGame()->GetElementDataMan();
			MONSTER_ESSENCE* pMonster = 
				(MONSTER_ESSENCE *)pDataMan->get_data_ptr(tsi.m_MonsterWanted[i].m_ulMonsterId, ID_SPACE_ESSENCE, DataType);

			ACString strName = _AL("^00FF00????^FFFFFF");
			if( DataType == DT_MONSTER_ESSENCE && pMonster)
			{
				bool bFind = false;
				A3DVECTOR3 vPos = CECUIHelper::GetTaskObjectCoordinates(id, bFind);

				ACHAR szPos[100];
				EditBoxItemBase item(enumEICoord);
				item.SetName(pMonster->name);
				if (bFind)
				{
					a_sprintf(szPos, _AL("%f %f %f %d"), vPos.x, vPos.y, vPos.z, id);
					item.SetInfo(szPos);
					item.SetColor(A3DCOLORRGB(0, 255, 0));
					strName = (ACHAR)AUICOMMON_ITEM_CODE_START + item.Serialize();
				}
				else
				{
					strName = pMonster->name;
				}
			}

			if( tsi.m_MonsterWanted[i].m_ulMonstersToKill > 0 )
			{
				strTemp.Format(GetStringFromTable(7624), strName,
					tsi.m_MonsterWanted[i].m_ulMonstersKilled, tsi.m_MonsterWanted[i].m_ulMonstersToKill);
			}
			else
			{
				strTemp.Format(GetStringFromTable(256), tsi.m_MonsterWanted[i].m_ulMonstersKilled);
			}

			if (i == 0)
			{
				strText += GetStringFromTable(7622);
			}
			else
				strText += GetStringFromTable(7626);

			strText += strTemp;
		}
	}
	for( i = 0; i < MAX_PLAYER_WANTED; i++ )
	{
		if (tsi.m_PlayerWanted[i].m_ulPlayersToKill == 0)
			break;
		if (tsi.m_ItemsWanted[i].m_ulItemId > 0)
		{
			continue;
		}
		
		if (i == 0)
			strText += GetStringFromTable(7630);
		else
			strText += GetStringFromTable(7626);

		strText += GetKillPlayerRequirements(tsi,i);
	}
	if (tsi.m_ulGoldWanted)
	{
		strTemp.Format(GetStringFromTable(7636), tsi.m_ulGoldWanted);
		strText += strTemp;
	}
	if (tsi.m_ulReachReincarnation){
		iLevel = pHost->GetReincarnationCount();
		if (iLevel < (int)tsi.m_ulReachReincarnation) {
			strColor =  _AL("^ff0000");
			strTemp.Format(GetStringFromTable(11144), iLevel);
			strHint += strTemp;
		} else strColor = _AL("^00ff00");
		strTemp.Format(GetStringFromTable(11141), tsi.m_ulReachReincarnation);
		strText += strColor;
		strText += strTemp;
	}
	if (tsi.m_ulReachLevel){
		iLevel = pHost->GetBasicProps().iLevel;
		if (iLevel < (int)tsi.m_ulReachLevel) {
			strColor =  _AL("^ff0000");
			strTemp.Format(GetStringFromTable(11143), iLevel);
			strHint += strTemp;
		} else strColor = _AL("^00ff00");
		strTemp.Format(GetStringFromTable(11140), tsi.m_ulReachLevel);
		strText += strColor;
		strText += strTemp;
	}
	if (tsi.m_ulReachRealm){
		iLevel = pHost->GetRealmLevel();
		if (iLevel < (int)tsi.m_ulReachRealm) {
			strColor =  _AL("^ff0000");
			strTemp.Format(GetStringFromTable(11145), GetGameUIMan()->GetRealmName(iLevel));
			strHint += strTemp;
		} else strColor = _AL("^00ff00");
		strTemp.Format(GetStringFromTable(11142), GetGameUIMan()->GetRealmName(tsi.m_ulReachRealm));
		strText += strColor;
		strText += strTemp;
	}
}
void CDlgTask::UpdateItemWanted(ACString& strText, const Task_State_info& tsi, int idTask)
{
	int i;
	const MINE_ESSENCE* pMineSearch = NULL;
	bool bSearchedMine = false;
	ATaskTemplMan *pMan = GetGame()->GetTaskTemplateMan();
	ATaskTempl *pTempl = pMan->GetTaskTemplByID(idTask);
	if (pTempl == NULL)
	{
		return;
	}

	for( i = 0; i < MAX_ITEM_WANTED; i++ )
	{
		if( tsi.m_ItemsWanted[i].m_ulItemId <= 0)
			break;

		CECIvtrItem* pItem = CECIvtrItem::CreateItem(
			tsi.m_ItemsWanted[i].m_ulItemId, 0, 1);

		if (pItem)
		{
			int id = tsi.m_ItemsWanted[i].m_ulMonsterId;
			ACString strName;
			int search_id(0);
			if (pTempl->m_enumMethod != enumTMKillPlayer) {
				if (id > 0) search_id = id;
				else {
					if(!bSearchedMine) {
						bSearchedMine = true;
						pMineSearch = SearchTaskMine(idTask);
					}
					const MINE_ESSENCE* pMine = pMineSearch;
					if(pMine) search_id = pMine->id;
				}
				bool bFind = false;
				A3DVECTOR3 vPos = CECUIHelper::GetTaskObjectCoordinates(search_id, bFind);
				if (bFind && search_id > 0) {
					ACHAR szPos[100];
					EditBoxItemBase item(enumEICoord);
					item.SetName(pItem->GetName());
					a_sprintf(szPos, _AL("%f %f %f %d"), vPos.x, vPos.y, vPos.z, search_id);
					item.SetInfo(szPos);
					item.SetColor(A3DCOLORRGB(0, 255, 0));
					strName = (ACHAR)AUICOMMON_ITEM_CODE_START + item.Serialize();
				} else strName = pItem->GetName();
			}

			ACString strTemp;
			strTemp.Format(GetStringFromTable(7625), strName,
				tsi.m_ItemsWanted[i].m_ulItemsGained,
				tsi.m_ItemsWanted[i].m_ulItemsToGet);
			if (i == 0)
			{
				strText += GetStringFromTable(7623);
			}
			else
				strText += GetStringFromTable(7626);
			strText += strTemp;
			if (pTempl->m_enumMethod == enumTMKillPlayer && i < MAX_PLAYER_WANTED)
			{
				strText += GetKillPlayerRequirements(tsi,i);
			}
			delete pItem;
		}
	}
}

void CDlgTask::UpdateTreasureMap(ACString& strText)
{
	if (IsTreasureMapSelected())
	{
		EditBoxItemBase item(enumEICoord);
		item.SetName(GetStringFromTable(7629));
		item.SetInfo(GetStringFromTable(7629));
		item.SetColor(A3DCOLORRGB(0, 255, 0));

		strText += (ACHAR)AUICOMMON_ITEM_CODE_START + item.Serialize();
	}
}
void CDlgTask::ClearContent(bool clearNPC)
{
	m_idLastTask = -2;

	m_pTxt_Content->SetText(_AL(""));
	m_pTxt_BaseAward->Show(false);

	for( size_t j = 0; j < m_ImgCount; j++ )
	{
		m_pImg_Item[j]->Show(false);
		m_pImg_Item[j]->SetData(0);
	}

	m_pTv_Quest->DeleteAllItems();
}

void CDlgTask::AddTaskNode(int id)
{
	PAUITREEVIEW pTreeTask = m_pTv_Quest;

	ATaskTemplMan *pMan = GetGame()->GetTaskTemplateMan();
	ATaskTempl *pTemp = pMan->GetTaskTemplByID(id);
	if( !pTemp )
	{
		return;
	}

	DWORD nTaskType = pTemp->GetType();
	if (pTemp->m_DynTaskType) nTaskType = enumTTEvent;
	DWORD nAfterType(0);
	P_AUITREEVIEW_ITEM pAfter(NULL), pParent(NULL);
	P_AUITREEVIEW_ITEM pItem = pTreeTask->GetFirstChildItem(pTreeTask->GetRootItem());
	while( pItem )
	{
		DWORD nType = pTreeTask->GetItemData(pItem);
		if( nType == nTaskType ){
			pParent = pItem;
			break;
		} else if (nType < nTaskType && nType > nAfterType){
			nAfterType = nType;
			pAfter = pItem;
		}
		pItem = pTreeTask->GetNextSiblingItem(pItem);
	}

	if(!pParent)
	{
		pParent = pTreeTask->InsertItem(GetStringFromTable(3101 + nTaskType - 100), NULL, pAfter);
		pTreeTask->Expand(pParent, AUITREEVIEW_EXPAND_EXPAND);
		pTreeTask->SetItemData(pParent, nTaskType);
		
		//if(nTaskType == enumTTLevel2)
			pTreeTask->SetItemTextColor(pParent, GetTaskColor(nTaskType));
	}

	CECTaskInterface* pTask = GetHostPlayer()->GetTaskInterface();
	ACString strItem(GetTaskNameWithColor(pTemp));
	bool bTaskPushed = pMan->IsTaskToPush(id) && !pTask->HasTask(id);
	if (bTaskPushed) {
		strItem += GetStringFromTable(3100);
	}
	pItem = pTreeTask->InsertItem(strItem, pParent);
	
	if( pTemp->IsKeyTask() )
		pTreeTask->SetItemTextColor(pItem, GetTaskColor(enumTTLevel2));
	pTreeTask->SetItemHint(pItem, pTemp->GetSignature());
	pTreeTask->SetItemData(pItem, id);
	InsertTaskChildren(pItem, id, true, pTemp->IsKeyTask());

	if( (int)id == m_idSelTask )
	{
		pTreeTask->SelectItem(pItem);
		m_pBtn_Abandon->Enable(true);
		UpdateTask(id);
	}
}

ACString CDlgTask::FormatTime(int nSec, const ACString& desc, int timeLimit)
{
	ACString strText;
	int nHour, nMin;

	if(timeLimit > 0)
	{
		if( nSec < (int)timeLimit / 10 )
			strText += _AL("  ^ff4a4a");
		else if( nSec < (int)timeLimit / 3 )
			strText += _AL("  ^ffbe7d");
		else
			strText += _AL("  ^00e83a");
	}

	nHour = nSec / 3600;
	nSec -= nHour * 3600;
	nMin = nSec / 60;
	nSec -= nMin * 60;
	strText.Format(desc, nHour, nMin, nSec);

	return strText;
}

ACString CDlgTask::GetKillPlayerRequirements(const Task_State_info& tsi,int iIndex)
{
	ACString strOccupation;
	int i = iIndex;
	int j;
	if (tsi.m_PlayerWanted[i].m_ulPlayersToKill == 0)
		return strOccupation;
	if (!CECProfConfig::Instance().ContainsAllProfession(tsi.m_PlayerWanted[i].m_Requirements.m_ulOccupations)){
		for (j = 0; j < NUM_PROFESSION; ++j)
		{
			if (tsi.m_PlayerWanted[i].m_Requirements.m_ulOccupations & (1 << j))
			{
				if (strOccupation.GetLength() > 0)
				{
					strOccupation += _AL("/");
				}
				strOccupation += GetGameRun()->GetProfName(j);
			}
		}
		if (strOccupation.GetLength() > 0)
		{
			strOccupation += _AL("; ");
		}
	}
	ACString strLevel;
	strLevel.Format(GetStringFromTable(7631),
		tsi.m_PlayerWanted[i].m_Requirements.m_iMinLevel, 
		tsi.m_PlayerWanted[i].m_Requirements.m_iMaxLevel);
	strLevel += _AL("; ");
	ACString strGender;
	if (tsi.m_PlayerWanted[i].m_Requirements.m_iGender == 1)
	{
		strGender = GetStringFromTable(8650);
	}
	else if (tsi.m_PlayerWanted[i].m_Requirements.m_iGender == 2)
	{
		strGender = GetStringFromTable(8651);
	}
	if (strGender.GetLength() > 0)
	{
		strGender += _AL("; ");
	}
	ACString strForce;
	for (j = 0; j < 3; ++j)
	{
		if (tsi.m_PlayerWanted[i].m_Requirements.m_iForce & (1 << j))
		{
			if (strForce.GetLength() > 0)
			{
				strForce += _AL("/");
			}
			strForce += GetStringFromTable(7632 + j);
		}
	}
	if (tsi.m_ItemsWanted[i].m_ulItemId > 0)
	{
		return ACString(_AL("(")) + strOccupation + strLevel + strGender + strForce + GetStringFromTable(7635) + _AL(")");
	}
	else
	{
		ACString strKilled;
		strKilled.Format(GetStringFromTable(9389),tsi.m_PlayerWanted[i].m_ulPlayersKilled, tsi.m_PlayerWanted[i].m_ulPlayersToKill);
		return strOccupation + strLevel + strGender + strForce + strKilled;
	}
}

void CDlgTask::UpdateTaskConfirm(int idTask, bool bFinishType)
{
	CECTaskInterface *pTask = GetHostPlayer()->GetTaskInterface();
	if (m_pBtn_FinishTask && pTask && bFinishType)
	{
		m_pBtn_FinishTask->Show(true);
		m_pBtn_FinishTask->Enable(pTask->IsTaskReadyToConfirm(idTask));
	}
	else m_pBtn_FinishTask->Show(false);
}

void CDlgTask::OnCommand_FinishTask(const char * szCommand)
{
	CECTaskInterface *pTask = GetHostPlayer()->GetTaskInterface();
	ATaskTemplMan *pMan = GetGame()->GetTaskTemplateMan();
	ATaskTempl *pTemp = pMan->GetTaskTemplByID(m_idLastTask);
	if (pTemp) {
		if (pTemp->m_Award_S->m_ulCandItems > 1) {
			CDlgAward* pAward = dynamic_cast<CDlgAward*>(m_pAUIManager->GetDialog("Win_Award"));
			if (pAward) {
				pAward->UpdateAwardItem(m_idLastTask, false);
				pAward->Show(true);
			}
		}
		else pTask->ConfirmFinishTask(m_idLastTask);
	}
}
void CDlgTask::OnEventLButtonDown_Award_Item(WPARAM wParam, LPARAM lParam, AUIObject *pObj)
{
	if(CDlgAutoHelp::IsAutoHelp())
	{
		CECIvtrItem* pItem = CECIvtrItem::CreateItem(pObj->GetData(),0,1);
		if (pItem)
		{
			CDlgWikiShortcut::PopItemWiki(GetGameUIMan(),pItem);
			delete pItem;
		}
	}
}

void CDlgTask::SortTaskNodeByType()
{
	PAUITREEVIEW pTreeTask = m_pTv_Quest;
	P_AUITREEVIEW_ITEM pItem = pTreeTask->GetFirstChildItem(pTreeTask->GetRootItem());
	typedef std::vector<TypeNode> TypeNodeContainer;
	TypeNodeContainer containerNode;
	while( pItem )
	{
		DWORD nType = pTreeTask->GetItemData(pItem);
		TypeNode node(nType, pItem);
		containerNode.push_back(node);
		pItem = pTreeTask->GetNextSiblingItem(pItem);
	}
	std::sort(containerNode.begin(), containerNode.end());
	size_t count = containerNode.size();
	for (size_t i = 0; i < count; ++i)
		pTreeTask->MoveItem(containerNode[i].item);
}

void CDlgTask::OnHideDialog()
{
	if (m_szName != "Win_Quest") return;
	PAUITREEVIEW pTree = m_pTv_Quest;
	P_AUITREEVIEW_ITEM pItem = pTree->GetSelectedItem();
	if( pItem && pTree->GetParentItem(pItem) != pTree->GetRootItem() )
		m_idSelTask = pTree->GetItemData(pItem);
	else m_idSelTask = 0;
	if(m_iType == 1) {
		ATaskTemplMan *pMan = GetGame()->GetTaskTemplateMan();
		if (pMan->HasTaskToPush()){
			OnCommand_searchquest(NULL);
		}
	}
	if (m_pQuickBuyTrigger){
		//	隐藏时、取消注册，减少 CECQuickBuyPopManager::Tick 消耗
		CECQuickBuyPopManager::Instance().UnregisterPolicy(QUICK_BUY_POP_ACTIVITY);
		m_pQuickBuyTrigger = NULL;
	}
}

void CDlgTask::OnShowDialog()
{
	if (!m_idSelTask)
		UpdateTask();
}

void CDlgTask::OnTaskPush()
{
	PAUIOBJECT pObj = GetDlgItem("Img_New");
	if (pObj) pObj->Show(true);
}

void CDlgTask::ClearGotoNPC()
{
	m_pBtn_GotoNPC->SetData(0);
	m_pBtn_GotoNPC->SetDataPtr(0);
	m_pBtn_GotoNPC->Show(false);
	if (m_pQuickBuyTrigger){
		m_pQuickBuyTrigger->SetActivityID(0);
	}
}

void CDlgTask::UpdateGotoNPC()
{
	if (!m_pBtn_GotoNPC || m_szName != "Win_Quest"){
		//	不是普通任务界面
		return;
	}
	if (m_iType != 0){
		//	不是已接任务
		if (m_pBtn_GotoNPC->GetData() != 0){
			ClearGotoNPC();
		}
		return;
	}
	int idCurTask = m_idSelTask;
	if (m_pBtn_GotoNPC->GetData() == idCurTask){
		//	选中任务未变时，更新可前往状态
		if (idCurTask > 0){
			const CECAutoTeamConfig::Activity *pActivity = (const CECAutoTeamConfig::Activity *)m_pBtn_GotoNPC->GetDataPtr();
			m_pBtn_GotoNPC->Enable(CECAutoTeamConfig::Instance().CanJumpToGoal(pActivity, false));
		}
		return;
	}
	if (idCurTask <= 0){
		//	选中任务为空
		ClearGotoNPC();
		return;
	}
	//	选中任务与之前选中不同
	bool bFound(false);
	int idInstance = GetWorld()->GetInstanceID();
	const CECAutoTeamConfig &config = CECAutoTeamConfig::Instance();
	for (int i(0); i < config.GetCount(); ++ i)
	{
		const CECAutoTeamConfig::Activity *pActivity = config.Get(i);
		if (!pActivity->IsDisable() && pActivity->FindTask(idCurTask)){
			m_pBtn_GotoNPC->SetData(idCurTask);
			m_pBtn_GotoNPC->SetDataPtr((void *)pActivity);
			m_pBtn_GotoNPC->Show(true);
			m_pBtn_GotoNPC->Enable(CECAutoTeamConfig::Instance().CanJumpToGoal(pActivity, false));
			if (m_pQuickBuyTrigger){
				m_pQuickBuyTrigger->SetActivityID(0);	//	下次前往再触发
			}
			bFound = true;
			break;
		}
	}
	if (!bFound){
		ClearGotoNPC();
	}
}

void CDlgTask::OnCommand_GotoNPC(const char * szCommand)
{
	if (!m_pBtn_GotoNPC || !m_pBtn_GotoNPC->IsEnabled()){
		return;
	}
	const CECAutoTeamConfig::Activity *pActivity = (const CECAutoTeamConfig::Activity *)m_pBtn_GotoNPC->GetDataPtr();
	if (CECAutoTeamConfig::Instance().CanJumpToGoal(pActivity)){
		GetGameSession()->c2s_CmdAutoTeamJumpToGoal(pActivity->p->id);
		if (m_pQuickBuyTrigger){
			m_pQuickBuyTrigger->SetActivityID(0);	//	意外情况处理、确保弹出界面消失
		}
		return;
	}
	//	缺少物品、弹出购买提示
	if (!m_pQuickBuyTrigger){
		CECQuickBuyPopPolicies policy = QuickBuyPopPolicy::CreateActivityPolicy(m_pQuickBuyTrigger);
		CECQuickBuyPopManager::Instance().RegisterPolicy(policy);
	}
	m_pQuickBuyTrigger->SetActivityID(pActivity->p->id);
}

void CDlgTask::OnTaskProcessUpdated(int idTask)
{
	CECTaskInterface *pTask = GetHostPlayer()->GetTaskInterface();
	if (!pTask->CanFinishTask(idTask)) {
		abase::vector<int> updated_tasks;
		size_t count = m_vecTasksUnFinish.size();
		for (size_t i = 0; i < count; ++i) {
			if (idTask != m_vecTasksUnFinish[i])
				updated_tasks.push_back(m_vecTasksUnFinish[i]);
		}
		updated_tasks.push_back(idTask);
		m_vecTasksUnFinish.swap(updated_tasks);
	}
}

void CDlgTask::OnTaskItemGained(int idItem)
{
	CECTaskInterface *pTask = GetHostPlayer()->GetTaskInterface();
	size_t count = m_vecTasksUnFinish.size();
	for (size_t i = 0; i < count; ++i) {
		Task_State_info tsi;
		pTask->GetTaskStateInfo(m_vecTasksUnFinish[i], &tsi);
		for(int j = 0; j < MAX_ITEM_WANTED; j++ )
		{
			int itemid = tsi.m_ItemsWanted[j].m_ulItemId;
			if( itemid <= 0 )
				break;

			if (itemid == idItem)
				OnTaskProcessUpdated(m_vecTasksUnFinish[i]);
		}
	}
}

int CDlgTask::GetSelectedTaskFromUI(){
	int result(0);
	if (m_pTv_Quest){		
		P_AUITREEVIEW_ITEM pItem = m_pTv_Quest->GetSelectedItem();
		if (pItem && m_pTv_Quest->GetParentItem(pItem) != m_pTv_Quest->GetRootItem()){
			result = m_pTv_Quest->GetItemData(pItem);
		}
	}
	return result;
}

bool CDlgTask::IsQuest()const{
	return m_szName == "Win_Quest";
}

bool CDlgTask::IsShowHaveQuest()const{
	return IsQuest() && m_iType == 0;
}

bool CDlgTask::IsTreasureMapSelected(){
	return IsShowHaveQuest() && IsTreasureMapTask(GetSelectedTaskFromUI());
}