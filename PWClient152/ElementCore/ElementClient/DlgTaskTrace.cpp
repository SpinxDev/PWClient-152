// Filename	: DlgTaskTrace.cpp
// Creator	: Feng Ning
// Date		: 2011/11/15

#pragma warning(disable:4786)

#include "DlgTaskTrace.h"
#include "DlgTask.h"
#include "DlgAward.h"
#include "EC_Global.h"
#include "EC_Game.h"
#include "EC_GameRun.h"
#include "EC_GameSession.h"
#include "EC_GameUIMan.h"
#include "EC_World.h"
#include "EC_TaskInterface.h"
#include "TaskTemplMan.h"
#include "elementdataman.h"
#include "EC_IvtrItem.h"
#include "EC_HostPlayer.h"
#include "DlgWorldMap.h"
#include "EC_Instance.h"
#include "EC_UIConfigs.h"
#include "EC_UIHelper.h"
#include "AUIDef.h"
#include "EC_FixedMsg.h"
#define new A_DEBUG_NEW

AUI_BEGIN_COMMAND_MAP(CDlgTaskTrace, CDlgNameLink)
AUI_ON_COMMAND("Chk_Collapse", OnCommand_SwitchCollapse)
AUI_ON_COMMAND("Rdo_Quest*", OnCommand_SwitchShowType)
AUI_ON_COMMAND("Btn_UnTrace", OnCommand_UnTraceTask)
AUI_ON_COMMAND("Btn_Map", OnCommand_Map)
AUI_ON_COMMAND("Btn_ForChat", OnCommand_Chat)
AUI_ON_COMMAND("Btn_Finish", OnCommand_FinishTask)
AUI_END_COMMAND_MAP()

AUI_BEGIN_EVENT_MAP(CDlgTaskTrace, CDlgNameLink)
AUI_ON_EVENT("Edit_Title",	WM_LBUTTONDBLCLK,	OnEventDBClk_Title)
AUI_ON_EVENT("Txt_Title",	WM_LBUTTONDBLCLK,	OnEventDBClk_Title)
AUI_ON_EVENT("Txt_Link_Trace",	WM_RBUTTONDOWN,	OnEventRButton_Txt_Link_Trace)
AUI_ON_EVENT("Txt_Link_Trace",	WM_RBUTTONUP,	OnEventRButton_Txt_Link_Trace)
AUI_ON_EVENT("Txt_Link_Trace",	WM_LBUTTONDOWN,	OnEventLButton_Txt_Link_Trace)
AUI_ON_EVENT("Txt_Link_Trace",	WM_LBUTTONUP,	OnEventLButton_Txt_Link_Trace)
AUI_ON_EVENT("Txt_Link_Trace", WM_MOUSEWHEEL,	OnMouseWheel)
AUI_END_EVENT_MAP()

static const ACString COLOR_YELLOW = _AL("^ffcb4a");
static const ACString COLOR_RED = _AL("^ff0000");
static const ACString COLOR_WHITE = _AL("^ffffff");
static const ACString INDENTATION = _AL("  ");

//////////////////////////////////////////////////////////////////////////
// TaskNameHoverCommand
//////////////////////////////////////////////////////////////////////////
// 用于确定鼠标悬浮到哪个任务上
class TaskNameHoverCommand : public CDlgNameLink::LinkCommand
{
public:
	virtual bool operator()(P_AUITEXTAREA_NAME_LINK pLink){ return true; };
	TaskNameHoverCommand(ACString& buffer, ACString& strName, int task_id, bool bError, bool bCanContributionFinish = false);
	TaskNameHoverCommand(const TaskNameHoverCommand& rhs);
	
	virtual ACString GetLinkText() const;
	int GetTaskID(){ return m_iTaskID; }
	bool CanConbutionFinish() { return m_bCanContributionFinish; };
	
protected:
	virtual void AppendText();
	virtual CDlgNameLink::LinkCommand* Clone() const;
	
	ACString& m_Buffer;
	ACString m_TaskName;
	bool	 m_bError;
	int		 m_iTaskID;
	bool	 m_bCanContributionFinish;
};

TaskNameHoverCommand::TaskNameHoverCommand(ACString& buffer, ACString& strName, int task_id, bool bError, bool bCanContributionFinish):
m_Buffer(buffer),
m_TaskName(strName),
m_iTaskID(task_id),
m_bError(bError),
m_bCanContributionFinish(bCanContributionFinish)
{
	
}

TaskNameHoverCommand::TaskNameHoverCommand(const TaskNameHoverCommand& rhs):
m_Buffer(rhs.m_Buffer),
m_TaskName(rhs.m_TaskName),
m_iTaskID(rhs.m_iTaskID),
m_bError(rhs.m_bError),
m_bCanContributionFinish(rhs.m_bCanContributionFinish)
{
	
}

ACString TaskNameHoverCommand::GetLinkText() const
{
	ACString szNameLink;
	if (m_bError)
		szNameLink.Format(_AL("&^%s%s&"), COLOR_RED, m_TaskName);
	else
		szNameLink.Format(_AL("&^%s%s&"), COLOR_YELLOW, m_TaskName);
	return szNameLink;
}

void TaskNameHoverCommand::AppendText()
{
	m_Buffer += this->GetLinkText();
}
CDlgNameLink::LinkCommand* TaskNameHoverCommand::Clone() const
{
	return new TaskNameHoverCommand(*this);	
}


//////////////////////////////////////////////////////////////////////////
// TraceLinkCommandCommand
//////////////////////////////////////////////////////////////////////////
typedef abase::vector<CECGame::OBJECT_COORD> ObjectTargets;

class TraceLinkCommand : public CDlgNameLink::MoveToLinkCommand
{
public:
	TraceLinkCommand(ACString& buffer, int id, const ACString &name, int taskid = 0);
	TraceLinkCommand(ACString& buffer, const ObjectTargets& targets, const ACString &name);
	TraceLinkCommand(const TraceLinkCommand& rhs);

protected:
	virtual void AppendText();
	virtual CDlgNameLink::LinkCommand* Clone() const;

	ACString& m_Buffer;
};

TraceLinkCommand::TraceLinkCommand(ACString& buffer, int id, const ACString &name, int taskid)
:CDlgNameLink::MoveToLinkCommand(id, name, taskid)
,m_Buffer(buffer)
{}

TraceLinkCommand::TraceLinkCommand(ACString& buffer, const ObjectTargets& targets, const ACString &name)
:CDlgNameLink::MoveToLinkCommand(targets, name)
,m_Buffer(buffer)
{

}

TraceLinkCommand::TraceLinkCommand(const TraceLinkCommand& rhs)
:CDlgNameLink::MoveToLinkCommand(rhs)
,m_Buffer(rhs.m_Buffer)
{}

CDlgNameLink::LinkCommand* TraceLinkCommand::Clone() const
{
	return new TraceLinkCommand(*this);
}

void TraceLinkCommand::AppendText()
{
	// do nothing here
	m_Buffer += this->GetLinkText();
}

//////////////////////////////////////////////////////////////////////////
// CDlgTaskTrace
//////////////////////////////////////////////////////////////////////////
CDlgTaskTrace::CDlgTaskTrace()
{
	m_pTxt_Desc = NULL;
	m_nLastTracedTasks = -1;
	m_nLastShowType = ST_NONE;
	m_nTraceWorldID = -1;
	m_pBtnUnTrace = NULL;
	m_pBtnFinishTaskByContribution = NULL;
	m_pBtnContributionTaskHelp = NULL;
	m_pBtnMap = NULL;
	m_pBtnChat = NULL;
	m_iContributionCurrentPage = 0;
	m_iContributionTotalPage = 1;
	m_iIndexForContribution = 0;
	m_bShowTrace = true;
}

CDlgTaskTrace::~CDlgTaskTrace()
{
}

bool CDlgTaskTrace::OnInitDialog()
{
	if(!CDlgNameLink::OnInitDialog())
		return false;

	m_pTxt_Desc = dynamic_cast<PAUITEXTAREA>(GetDlgItem("Txt_Link_Trace"));
	if(m_pTxt_Desc) m_pTxt_Desc->SetForceRenderScroll(false);
	m_pChk_Collapse = dynamic_cast<PAUICHECKBOX>(GetDlgItem("Chk_Collapse"));
	if (m_szName == "Win_QuestMinion") {
		DDX_Control("Btn_UnTrace", m_pBtnUnTrace);
		DDX_Control("Btn_Map", m_pBtnMap);
		DDX_Control("Btn_ForChat", m_pBtnChat);
		DDX_Control("Btn_Finish", m_pBtnFinishTaskByContribution);
		DDX_Control("Btn_Help", m_pBtnContributionTaskHelp);
		m_pBtnUnTrace->Show(false);
		m_pBtnMap->Show(false);
		m_pBtnChat->Show(false);
		m_pBtnFinishTaskByContribution->Show(false);
		m_pBtnContributionTaskHelp->Show(false);
	}

	//	任务追踪界面显示默认列表
	PAUIOBJECT pObjRadio = GetDlgItem("Rdo_Quest1");
	if (pObjRadio)	SetShowType(ST_TRACED);
	pObjRadio = GetDlgItem("Rdo_Quest2");
	if (pObjRadio) pObjRadio->Show(CECUIConfig::Instance().GetGameUI().bEnableTitle);
	return true;
}

void CDlgTaskTrace::OnShowDialog()
{
	m_pChk_Collapse->Check(m_pTxt_Desc->IsShow());
	PAUIOBJECT pObjRadio = GetDlgItem("Rdo_Quest3");
	if (pObjRadio)  {
		int levelLimit = CECUIConfig::Instance().GetGameUI().nContributionTaskLevelLimit;
		CECHostPlayer* pHost = GetHostPlayer();
		if (pHost && pHost->GetMaxLevelSofar() >= levelLimit)
			pObjRadio->SetFlash(true);
	}
	
}

void CDlgTaskTrace::OnChangeLayoutEnd(bool bAllDone) {
	// 如果处于被收起状态，则对话框的大小也要相应缩小
	if (m_szName == "Win_QuestMinion" && !m_bShowTrace) {
		SIZE cur = this->GetSize();
		SIZE desc = m_pTxt_Desc->GetSize();
		cur.cy -= desc.cy;
		this->SetSize(cur.cx, cur.cy);
		m_pChk_Collapse->Check(false);
	}
}

void CDlgTaskTrace::AppendCommand(const ACHAR* pName, int idTarget, int idTask)
{
	ACString strName = pName;

	if(!strName.IsEmpty())
	{
		ACString strUniqueName = MakeNameUnique(strName);
		if (!strUniqueName.IsEmpty()){
			TraceLinkCommand cmd(m_Buffer, idTarget, strUniqueName, idTask);
			BindLinkCommand(m_pTxt_Desc, &strUniqueName, &cmd);
		}
	}	
}

void CDlgTaskTrace::ResetNameUnique(){
	m_uniqueNameCountMap.clear();
}

ACString CDlgTaskTrace::MakeNameUnique(const ACString &strName){
	ACString result;
	if (!strName.IsEmpty()){
		ACString strTrimmedName = strName;
		strTrimmedName.TrimLeft();
		strTrimmedName.TrimRight();
		if (!strTrimmedName.IsEmpty()){
			UniqueNameCountMap::iterator it = m_uniqueNameCountMap.find(strTrimmedName);
			if (it == m_uniqueNameCountMap.end()){
				m_uniqueNameCountMap[strTrimmedName] = 1;
			}else{
				++ it->second;
			}
			result = strTrimmedName;
			if (int spaceCount = m_uniqueNameCountMap[strTrimmedName]-1){
				ACString strMakeUniqueSuffix(_AL(' '), spaceCount);
				result += strMakeUniqueSuffix;
			}
		}
	}
	return result;
}

void CDlgTaskTrace::AppendCommand(int worldid, const Task_Region* pRegions, int size)
{
	ObjectTargets instCoord, tempCoord;
	int cur = g_pGame->GetGameRun()->GetWorld()->GetInstanceID();
	CECInstance *pInstance = g_pGame->GetGameRun()->GetInstance(cur);
	ACString strCurMap = AS2AC(pInstance->GetPath());
	// find the entrance of instance
	pInstance = g_pGame->GetGameRun()->GetInstance(worldid);
	ACString mapName = pInstance ? AS2AC(pInstance->GetPath()) : _AL("");

	if(cur != worldid)
	{
		g_pGame->GetObjectCoord(mapName, tempCoord);
		ObjectTargets::iterator iter = std::find(tempCoord.begin(), tempCoord.end(), strCurMap);
		if (iter != tempCoord.end()) instCoord.push_back(*iter);
	}
	else if(size > 0 && pRegions)
	{
		for(int i=0;i<size;i++)
		{
			const Task_Region& reg = pRegions[i];
			CECGame::OBJECT_COORD coor;
			coor.strMap = mapName;
			coor.vPos.x = (reg.zvMax.x - reg.zvMin.x) / 2 + reg.zvMin.x;
			coor.vPos.y = (reg.zvMax.y - reg.zvMin.y) / 2 + reg.zvMin.y;
			coor.vPos.z = (reg.zvMax.z - reg.zvMin.z) / 2 + reg.zvMin.z;
			instCoord.push_back(coor);
		}
	}

	// find the targets
	if(instCoord.size() > 0)
	{
		ACString strName;

		A3DVECTOR3 vPos = instCoord[0].vPos;
		strName.Format(GetStringFromTable(9393), ((int)vPos.x+4000)/10, ((int)vPos.z+5500)/10);

		TraceLinkCommand cmd(m_Buffer, instCoord, strName);
		BindLinkCommand(m_pTxt_Desc, &strName, &cmd);
	}
}

void CDlgTaskTrace::AppendText(const ACString & text)
{
	m_Buffer += text;
}

void CDlgTaskTrace::PrepareRebuildTaskTrace(){
	m_Buffer.Empty();
	ClearCommands();
	ResetNameUnique();
}

void CDlgTaskTrace::RefreshTaskTrace(const int* tasks, size_t size, bool withName)
{
	bool bShow = size > 0;

	// show/hide the trace dialog
	if(this->IsShow() != bShow)
	{
		this->Show(bShow);
	}
	
	if (!IsShow() || !m_pTxt_Desc->IsShow())	return;

	PrepareRebuildTaskTrace();

	// get the formatted desc
	for(int i=(int)size-1; i>=0; i--)
	{
		FormatTaskTraceDesc(tasks[i], withName, _AL(""));
	}
	
	int lastTop = m_pTxt_Desc->GetFirstLine();
	m_pTxt_Desc->SetText(m_Buffer);
	int curTop = m_pTxt_Desc->GetFirstLine();
	
	if(lastTop != curTop)
	{
		int top = (lastTop < m_pTxt_Desc->GetLines()) ? lastTop 
			: max(0, m_pTxt_Desc->GetLines() - m_pTxt_Desc->GetLinesPP());
		m_pTxt_Desc->SetFirstLine(top);
	}
}

void CDlgTaskTrace::OnEventMouseMove(int x, int y, DWORD keys, const P_AUITEXTAREA_NAME_LINK pLink)
{
	if (m_szName != "Win_QuestMinion") 
		return;
	ShowType showType = GetShowType();
	if(!pLink) GetGameUIMan()->ContinueDealMessage();
	else if (showType == ST_TRACED || showType == ST_CONTRIBUTION) {
		int pos_y = pLink->rc.top - m_pAUIManager->GetRect().top - GetPos().y;
		TaskNameHoverCommand* pCmd = dynamic_cast<TaskNameHoverCommand*>(GetLinkCommand(m_pTxt_Desc, &pLink->strName));
		if (pCmd == NULL) return;
		if (showType == ST_TRACED) {
			SetBtnUnTraceY(pos_y, pCmd->GetTaskID());
		} else {
			CECTaskInterface* pTask = GetHostPlayer()->GetTaskInterface();
			ActiveTaskList* pActiveLst = (ActiveTaskList*)pTask->GetActiveTaskList();
			int taskID = pCmd->GetTaskID();
			ATaskTempl* pTempl = GetGame()->GetTaskTemplateMan()->GetTaskTemplByID(taskID);
			if (pTempl == NULL) return;
			ActiveTaskEntry* pEntry = pActiveLst->GetEntry(taskID);
			if (pCmd->CanConbutionFinish() && pEntry && !pEntry->IsFinished() && pTempl->m_pFirstChild == NULL ) {
				SetBtnUnTraceY(pos_y, pCmd->GetTaskID());
			}
		}
	}
}

void CDlgTaskTrace::OnEventLButtonClick(int x, int y, DWORD keys, const P_AUITEXTAREA_NAME_LINK pLink)
{
	if(!pLink) GetGameUIMan()->ContinueDealMessage();
}

bool CDlgTaskTrace::PtInRect(const AUITEXTAREA_NAME_LINK &name, int x, int y)
{
	if (m_szName == "Win_QuestMinion"){
		A3DRECT rcExpanded = name.rc;
		int right = m_pAUIManager->GetRect().left + m_pTxt_Desc->GetRect().right;
		if (right > rcExpanded.right){
			rcExpanded.right = right;	//	扩充到控件最右侧，方便弹出相应“关闭追踪”等控件
		}
		return rcExpanded.PtInRect(x, y);
	}
	return CDlgNameLink::PtInRect(name, x, y);
}

void CDlgTaskTrace::FormatTaskTraceDesc(int idTask, bool withName, const ACString& strIndentIn, bool requireActive, int index, bool bCanContributionFinish, int contribution)
{
	int j;

	ATaskTemplMan *pMan = g_pGame->GetTaskTemplateMan();
	elementdataman *pDataMan = g_pGame->GetElementDataMan();
	CECTaskInterface *pTask = GetHostPlayer()->GetTaskInterface();
	CECGameUIMan *pUIMan = GetGameUIMan();

	bool bActiveTask = pTask->HasTask(idTask);
	if (requireActive && !bActiveTask) return;

	ATaskTempl *pTemp = NULL;
	if (pTemp = pMan->GetTaskTemplByID(idTask))
	{
		Task_State_info tsi;
		pTask->GetTaskStateInfo(idTask, &tsi, bActiveTask);
	
		float vMin[3] = {-6000.0f,-6000.0f,-6000.0f};
		float vMax[3] = {6000.0f,6000.0f,6000.0f};
		if(pTemp->m_ulReachSiteCnt > 0
			&& is_in_zone(pTemp->m_pReachSite[0].zvMin,pTemp->m_pReachSite[0].zvMax,vMin) && is_in_zone(pTemp->m_pReachSite[0].zvMin,pTemp->m_pReachSite[0].zvMax,vMax))
			return;

		// append the name
		if (withName)
		{
			AppendText(strIndentIn);
			ACString strNum;
			strNum.Format(_AL("%d"), index);
			ACString strName = pTemp->GetName();
			DeleteColorStr(strName);
			strName = strNum + _AL(" ") + strName;
			TaskNameHoverCommand cmd(m_Buffer, strName, idTask, tsi.m_ulErrCode != 0, bCanContributionFinish);
			BindLinkCommand(m_pTxt_Desc, &strName, &cmd);
			if (contribution != 0) {
				int colorIndex = contribution > 0 ? 11288 : 11291;
				strNum.Format(_AL(" %s【%+d】"), GetStringFromTable(colorIndex), contribution);
				AppendText(strNum);
			}
			AppendText(_AL("\r"));
		}
		// init the indent
		ACString strIndent = withName ? strIndentIn + INDENTATION :_AL("");

		//	任务未接时显示接任务NPC
		int nDNPC = pTemp->GetDeliverNPC();
		if (!bActiveTask && nDNPC > 0)
		{
			DATA_TYPE dt = DT_INVALID;
			NPC_ESSENCE *pNPC = (NPC_ESSENCE *)pDataMan->get_data_ptr(nDNPC, ID_SPACE_ESSENCE, dt);
			if (dt == DT_NPC_ESSENCE)
			{
				AppendText(strIndent);
				AppendText(pUIMan->GetStringFromTable(9764));
				AppendCommand(pNPC->name, nDNPC, idTask);
				AppendText(_AL("\r"));
			}
		}

		// check whether can finish the task
		int nANPC = pTemp->GetAwardNPC();
		// 当任务奖励类型比较特殊时，不要在这里返回
		bool bSpecialAwardType = pTemp->m_ulAwardType_S != enumTATNormal;
		if(nANPC > 0 && pTask->CanFinishTask(idTask) && !bSpecialAwardType)
		{
			DATA_TYPE DataType = DT_INVALID;
			NPC_ESSENCE *pNPC = 
				(NPC_ESSENCE *)pDataMan->get_data_ptr(nANPC, ID_SPACE_ESSENCE, DataType);
			if( DataType == DT_NPC_ESSENCE )
			{
				this->AppendText(strIndent);
				this->AppendText(pUIMan->GetStringFromTable(9391));
				this->AppendCommand(pNPC->name, nANPC, idTask);
				this->AppendText(_AL("\r"));
				return;
			}
		}

		// append global task character
		for(int i = 0; i < (int)tsi.m_TaskCharArr.size(); i++)
		{
			ACString strPrefix = strIndent;
			strPrefix += _AL("^00FF00");
			strPrefix += tsi.m_TaskCharArr[i];
			strPrefix += _AL("\r");

			this->AppendText(strPrefix);
		}

		for( j = 0; j < MAX_ITEM_WANTED; j++ )
		{
			int itemid = tsi.m_ItemsWanted[j].m_ulItemId;
			if( itemid <= 0 )
				break;

			int gained = tsi.m_ItemsWanted[j].m_ulItemsGained;
			int toget = tsi.m_ItemsWanted[j].m_ulItemsToGet;

			CECIvtrItem* pItem = CECIvtrItem::CreateItem(itemid, 0, 1);
			if (pItem)
			{
				ACString strPrefix = strIndent;
				if( gained < toget )
					strPrefix += COLOR_WHITE;
				else
					strPrefix += _AL("^00e83a");
				this->AppendText(strPrefix);

				// track the monster id which contains this item
				int monid = tsi.m_ItemsWanted[j].m_ulMonsterId;

				ACString strName;
				if (pTemp->m_enumMethod != enumTMKillPlayer)
				{
					if(monid > 0)
					{
						this->AppendText(pUIMan->GetStringFromTable(9388));
						this->AppendCommand(pItem->GetName(), monid);
						strName.Format(pUIMan->GetStringFromTable(9389), gained, toget);
					}
					else
					{
						const MINE_ESSENCE* pMine = CDlgTask::SearchTaskMine(idTask);
						if(pMine)
						{
							this->AppendText(pUIMan->GetStringFromTable(9388));
							this->AppendCommand(pItem->GetName(), pMine->id);
							strName.Format(pUIMan->GetStringFromTable(9389), gained, toget);
						}
						else
						{
							strName.Format(pUIMan->GetStringFromTable(248), pItem->GetName(), gained, toget);
						}
					}
				}
				else
				{
					this->AppendText(pUIMan->GetStringFromTable(9388));
					this->AppendText(pItem->GetName());
					strName.Format(pUIMan->GetStringFromTable(9389), gained, toget);
					strName += dynamic_cast<CDlgTask*>(m_pAUIManager->GetDialog("Win_Quest"))->GetKillPlayerRequirements(tsi,j);
				}

				this->AppendText(strName);

				delete pItem;
			}
		}

		for (j = 0; j < MAX_MONSTER_WANTED; j++)
		{
			int monid = tsi.m_MonsterWanted[j].m_ulMonsterId;			
			if (monid == 0)
				break;

			int killed = tsi.m_MonsterWanted[j].m_ulMonstersKilled;
			int tokill = tsi.m_MonsterWanted[j].m_ulMonstersToKill;
			if(killed > 0 || tokill > 0 )
			{
				DATA_TYPE DataType = DT_INVALID;
				MONSTER_ESSENCE *pMonster = (tokill <= 0) ? NULL:
					(MONSTER_ESSENCE *)pDataMan->get_data_ptr(monid, ID_SPACE_ESSENCE, DataType);

				ACString strPrefix = strIndent;
				if(killed < tokill )
					strPrefix += COLOR_WHITE;
				else
					strPrefix += _AL("^00e83a");
				this->AppendText(strPrefix);

				ACString strName;
				if( DataType == DT_MONSTER_ESSENCE )
				{
					// trace the monster
					this->AppendText(pUIMan->GetStringFromTable(9386));
					this->AppendCommand(pMonster->name, pMonster->id);
					strName.Format(pUIMan->GetStringFromTable(9387), killed, tokill);
				}
				else
				{
					strName.Format(pUIMan->GetStringFromTable(256),killed);
				}

				this->AppendText(strName);
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
			
			int killed = tsi.m_PlayerWanted[j].m_ulPlayersKilled;
			int tokill = tsi.m_PlayerWanted[j].m_ulPlayersToKill;
			ACString strPrefix = strIndent;
			if(killed < tokill )
				strPrefix += COLOR_WHITE;
			else
				strPrefix += _AL("^00e83a");

			AppendText(strPrefix);
			
			AppendText(GetStringFromTable(7630));

			this->AppendText(dynamic_cast<CDlgTask*>(m_pAUIManager->GetDialog("Win_Quest"))->GetKillPlayerRequirements(tsi,i));
		}
		if( tsi.m_ulTimeLimit > 0 )
		{
			int nSec = bActiveTask ? max(0, tsi.m_ulTimeLimit - tsi.m_ulTimePassed) : tsi.m_ulTimeLimit;

			ACString strPrefix = strIndent;
			strPrefix += CDlgTask::FormatTime(nSec, pUIMan->GetStringFromTable(bActiveTask ? 246 : 245), tsi.m_ulTimeLimit);

			this->AppendText(strPrefix);
		}

		if( tsi.m_ulWaitTime > 0 )
		{
			int nSec = max(0, int(tsi.m_ulWaitTime) - int(tsi.m_ulTimePassed));

			ACString strPrefix = strIndent + COLOR_WHITE;
			strPrefix += CDlgTask::FormatTime(nSec, pUIMan->GetStringFromTable(199), 0);

			this->AppendText(strPrefix);
		}

		if( tsi.m_ulNPCToProtect > 0 )
		{
			this->AppendText(strIndent);

			DATA_TYPE DataType = DT_INVALID;
			MONSTER_ESSENCE *pMonster = (MONSTER_ESSENCE *)pDataMan->get_data_ptr(
				tsi.m_ulNPCToProtect, ID_SPACE_ESSENCE, DataType);
			if( DataType == DT_MONSTER_ESSENCE )
			{
				// trace the protected npc
				this->AppendText(pUIMan->GetStringFromTable(9390));
				this->AppendCommand(pMonster->name, pMonster->id);
				this->AppendText(_AL("\r"));
			}

			// update protected time
			int nSec = max(0, tsi.m_ulProtectTime - tsi.m_ulTimePassed);
			ACString strTime = CDlgTask::FormatTime(nSec, pUIMan->GetStringFromTable(259), tsi.m_ulProtectTime);
			this->AppendText(strTime);
		}

		if(pTemp->m_ulReachSiteCnt > 0)
		{
			this->AppendText(strIndent);
			this->AppendText(pUIMan->GetStringFromTable(9392));
			this->AppendCommand(pTemp->m_ulReachSiteId, pTemp->m_pReachSite, pTemp->m_ulReachSiteCnt);
			this->AppendText(_AL("\r"));
		}
		CECHostPlayer* pHost = GetHostPlayer();
		ACString strTemp;
		if (pTemp->m_ulReachLevel)
		{
			this->AppendText(strIndent);
			strTemp.Format(pUIMan->GetStringFromTable(9396), pHost->GetBasicProps().iLevel, pTemp->m_ulReachLevel);
			this->AppendText(strTemp);
			this->AppendText(_AL("\r"));
		}
		if (pTemp->m_ulReachRealmLevel)
		{
			this->AppendText(strIndent);
			strTemp.Format(pUIMan->GetStringFromTable(9397), 
				pUIMan->GetRealmName(pHost->GetRealmLevel()), pUIMan->GetRealmName(pTemp->m_ulReachRealmLevel));
			this->AppendText(strTemp);
			this->AppendText(_AL("\r"));
		}
		if (pTemp->m_ulReachReincarnationCount)
		{
			this->AppendText(strIndent);
			strTemp.Format(pUIMan->GetStringFromTable(9398), pHost->GetReincarnationCount(), pTemp->m_ulReachReincarnationCount);
			this->AppendText(strTemp);
			this->AppendText(_AL("\r"));
		}
	}
}

bool CDlgTaskTrace::IsCollapsed()
{
	return !m_pChk_Collapse->IsChecked();
}

void CDlgTaskTrace::SwitchCollapse()
{
	// adjust the dialog size
	SIZE cur = this->GetSize();
	SIZE desc = m_pTxt_Desc->GetSize();
	cur.cy += m_pTxt_Desc->IsShow() ? -desc.cy : desc.cy;
	this->SetSize(cur.cx, cur.cy);
	m_pTxt_Desc->Show(!m_pTxt_Desc->IsShow());
	m_bShowTrace = !m_bShowTrace;
	if (GetShowType() == ST_TRACED || GetShowType() == ST_TITLE) {
		m_pBtnContributionTaskHelp->Show(false);
	} else {
		m_pBtnContributionTaskHelp->Show(!m_pBtnContributionTaskHelp->IsShow());
	}
	if (GetShowType() == ST_TRACED && m_pBtnUnTrace->GetData()) {
		m_pBtnUnTrace->Show(!m_pBtnUnTrace->IsShow());
		m_pBtnMap->Show(!m_pBtnMap->IsShow());
		m_pBtnChat->Show(!m_pBtnChat->IsShow());
	} else if (GetShowType() == ST_CONTRIBUTION && m_pBtnFinishTaskByContribution->GetData()) {
		m_pBtnFinishTaskByContribution->Show(!m_pBtnFinishTaskByContribution->IsShow());
	}

	PAUILABEL pTxt_Title = dynamic_cast<PAUILABEL>(GetDlgItem("Txt_Title"));
	if(pTxt_Title)
	{
		pTxt_Title->SetText(GetStringFromTable(m_pTxt_Desc->IsShow() ? 9394:9395));
	}
	m_pChk_Collapse->Check(m_pTxt_Desc->IsShow());
}

void CDlgTaskTrace::OnEventDBClk_Title(WPARAM wParam, LPARAM lParam, AUIObject *pObj)
{
	SwitchCollapse();
}

void CDlgTaskTrace::OnCommand_SwitchCollapse(const char * szCommand)
{
	SwitchCollapse();
}

void CDlgTaskTrace::OnCommand_SwitchShowType(const char * szCommand)
{
	SetBtnUnTraceY(-1, 0);
	if (IsCollapsed()){
		SwitchCollapse();
	}
	if (GetShowType() == ST_CONTRIBUTION) {
		int levelLimit = CECUIConfig::Instance().GetGameUI().nContributionTaskLevelLimit;
		if (GetHostPlayer()->GetMaxLevelSofar() < levelLimit
			&& szCommand && strlen(szCommand) != 0) {
			SetShowType(ST_TRACED);
			ACString strText;
			strText.Format(GetStringFromTable(11293), levelLimit);
			GetGameUIMan()->MessageBox("", strText, MB_OK, A3DCOLORRGBA(255, 255, 255, 160));
		}
		PAUIOBJECT pObjRadio = GetDlgItem("Rdo_Quest3");
		if (pObjRadio) pObjRadio->SetFlash(false);
	}
	m_pBtnContributionTaskHelp->Show(GetShowType() == ST_CONTRIBUTION);
	GetGameUIMan()->m_pDlgTask->RefreshTaskTrace();
}

void CDlgTaskTrace::OnEventRButton_Txt_Link_Trace(WPARAM wParam, LPARAM lParam, AUIObject *pObj)
{
	 GetGameUIMan()->ContinueDealMessage();
	 this->ChangeFocus(m_pBtnUnTrace);
}

void CDlgTaskTrace::OnEventLButton_Txt_Link_Trace(WPARAM wParam, LPARAM lParam, AUIObject *pObj)
{
	CDlgNameLink::OnEventLButtonClick_NameLink(wParam, lParam, pObj);
	if (m_pTxt_Desc->GetState() != AUITEXTAREA_STATE_PRESSBAR)
		this->ChangeFocus(m_pBtnUnTrace);
	else if (m_pBtnUnTrace->IsShow())
		SetBtnUnTraceY(-1, 0);
}

CDlgTaskTrace::ShowType CDlgTaskTrace::GetShowType()
{
	ShowType ret = ST_TRACED;
	int nCheck = GetCheckedRadioButton(0);
	switch (nCheck)
	{
	case 0:	ret = ST_TRACED; break;
	case 1: ret = ST_TITLE; break;
	case 2: ret = ST_CONTRIBUTION; break;
	default: ASSERT(false);
	}
	return ret;
}

void CDlgTaskTrace::SetShowType(ShowType rhs)
{
	int nCheck(0);
	switch (rhs)
	{
	case ST_TRACED: nCheck = 0; break;
	case ST_TITLE: nCheck = 1; break;
	case ST_CONTRIBUTION: nCheck = 2; break;
	default: ASSERT(false);
	}
	CheckRadioButton(0, nCheck);
	m_nLastShowType = rhs;
}

void CDlgTaskTrace::RefreshTaskTrace(const int* traced, int nTraced, const int* titletasks, int num, bool withName)
{
	ShowType showType = GetShowType();
	int size(0);
	const int* tasks = NULL;
	switch (showType)
	{
	case ST_TRACED: size = nTraced; tasks = traced; break;
	case ST_TITLE: size = num; tasks = titletasks; break;
	}
	
	bool bShow = !GetGameUIMan()->IsCustomizeCharacter();
	
	if(IsShow() != bShow) Show(bShow);

	int idWorld = GetGameRun()->GetWorld()->GetInstanceID();
	if (IsShow() && m_pTxt_Desc->IsShow())
	{
		if (m_nTraceWorldID != idWorld ||															//	地图有变，追踪信息需更新
			showType == ST_TRACED ||															//	已接任务信息需实时更新
			showType == ST_TITLE)																//	已接称号任务信息需实时更新	
		{
			PrepareRebuildTaskTrace();			
			for (int i = size-1; i >= 0; i--)
				FormatTaskTraceDesc(tasks[i], withName, _AL(""), false, size - i);
			KeepScrollBarPosition();
			m_nTraceWorldID = idWorld;
		}
	}
	
	m_nLastTracedTasks = nTraced;
}

void CDlgTaskTrace::DeleteColorStr(ACString& strName)
{
	if (strName.Left(1) == _AL("^")) {
		strName.CutLeft(7);
	}
}

void CDlgTaskTrace::SetBtnUnTraceY(int pos_y, int task_id)
{
	m_pBtnUnTrace->Show(pos_y != -1 && GetShowType() == ST_TRACED);
	m_pBtnUnTrace->SetPos(m_pBtnUnTrace->GetPos(true).x, pos_y);
	m_pBtnUnTrace->SetData(task_id);
	m_pBtnMap->Show(pos_y != -1 && GetShowType() == ST_TRACED);
	m_pBtnMap->SetPos(m_pBtnMap->GetPos(true).x, pos_y);
	m_pBtnMap->SetData(task_id);
	m_pBtnChat->Show(pos_y != -1 && GetShowType() == ST_TRACED);
	m_pBtnChat->SetPos(m_pBtnChat->GetPos(true).x, pos_y);
	m_pBtnChat->SetData(task_id);
	m_pBtnFinishTaskByContribution->Show(pos_y != -1 && GetShowType() == ST_CONTRIBUTION);
	m_pBtnFinishTaskByContribution->SetPos(m_pBtnFinishTaskByContribution->GetPos(true).x, pos_y);
	m_pBtnFinishTaskByContribution->SetData(task_id);
	if (m_pBtnFinishTaskByContribution->IsShow()) StartButtonGfx("Btn_Finish", "界面\\打钩");
}

void CDlgTaskTrace::OnCommand_UnTraceTask(const char * szCommand)
{
	PAUIOBJECT pObj = GetDlgItem(szCommand);
	if (pObj) {
		int idTask = pObj->GetData();
		CECGameUIMan* pGameUI = GetGameUIMan();
		if (pGameUI) {
			pGameUI->m_pDlgTask->SwitchTaskTrace(idTask);
		}
	}
}

void CDlgTaskTrace::OnCommand_Map(const char * szCommand)
{
	PAUIOBJECT pObj = GetDlgItem(szCommand);
	if (pObj) {
		int idTask = pObj->GetData();
		if (CECUIHelper::ShowOneTaskInMap(idTask))
			GetGameUIMan()->GetMapDlgsMgr()->SwitchWorldMapShow();
	}
}

void CDlgTaskTrace::OnCommand_Chat(const char * szCommand)
{
	PAUIOBJECT pObj = GetDlgItem(szCommand);
	if (pObj) {
		int idTask = pObj->GetData();
		ATaskTemplMan *pMan = g_pGame->GetTaskTemplateMan();
		elementdataman *pDataMan = g_pGame->GetElementDataMan();
		CECHostPlayer* pHost = GetHostPlayer();
		CECTaskInterface *pTask = pHost->GetTaskInterface();
		CECGameUIMan *pUIMan = GetGameUIMan();

		bool bActiveTask = pTask->HasTask(idTask);
		ACString strText;

		ATaskTempl *pTemp = NULL;
		while (pTemp = pMan->GetTaskTemplByID(idTask))
		{
			Task_State_info tsi;
			pTask->GetTaskStateInfo(idTask, &tsi, bActiveTask);
	
			float vMin[3] = {-6000.0f,-6000.0f,-6000.0f};
			float vMax[3] = {6000.0f,6000.0f,6000.0f};

			ACString strName = pTemp->GetName();
			DeleteColorStr(strName);
			strText += strName;

			ACString strIndent = INDENTATION;

			int nANPC = pTemp->GetAwardNPC();

			if(nANPC > 0 && pTask->CanFinishTask(idTask)) {
				DATA_TYPE DataType = DT_INVALID;
				NPC_ESSENCE *pNPC = 
					(NPC_ESSENCE *)pDataMan->get_data_ptr(nANPC, ID_SPACE_ESSENCE, DataType);
				if( DataType == DT_NPC_ESSENCE ) {
					strText += strIndent;
					strText += pUIMan->GetStringFromTable(9378);
					strText += pNPC->name;
					strText += _AL("\r");
					break;
				}
			}

			// append global task character
			for(int i = 0; i < (int)tsi.m_TaskCharArr.size(); i++) {
				ACString strPrefix = strIndent;
				strPrefix += tsi.m_TaskCharArr[i];
				strPrefix += _AL("\r");

				strText += strPrefix;
			}

			int j = 0;
			for( j = 0; j < MAX_ITEM_WANTED; j++ ) {
				int itemid = tsi.m_ItemsWanted[j].m_ulItemId;
				if( itemid <= 0 )
					break;

				int gained = tsi.m_ItemsWanted[j].m_ulItemsGained;
				int toget = tsi.m_ItemsWanted[j].m_ulItemsToGet;

				CECIvtrItem* pItem = CECIvtrItem::CreateItem(itemid, 0, 1);
				if (pItem)
				{
					strText += strIndent;

					ACString strName;
					strName.Format(pUIMan->GetStringFromTable(248), pItem->GetName(), gained, toget);
					if (pTemp->m_enumMethod == enumTMKillPlayer) 
						strName += dynamic_cast<CDlgTask*>(m_pAUIManager->GetDialog("Win_Quest"))->GetKillPlayerRequirements(tsi,j);
					strText += strName;
					delete pItem;
				}
			}
			for (j = 0; j < MAX_MONSTER_WANTED; j++) {
				int monid = tsi.m_MonsterWanted[j].m_ulMonsterId;			
				if (monid == 0)
					break;

				int killed = tsi.m_MonsterWanted[j].m_ulMonstersKilled;
				int tokill = tsi.m_MonsterWanted[j].m_ulMonstersToKill;
				if(killed > 0 || tokill > 0 ) {
					DATA_TYPE DataType = DT_INVALID;
					MONSTER_ESSENCE *pMonster = (tokill <= 0) ? NULL:
						(MONSTER_ESSENCE *)pDataMan->get_data_ptr(monid, ID_SPACE_ESSENCE, DataType);

					strText += strIndent;

					ACString strName;
					if( DataType == DT_MONSTER_ESSENCE ) {
						strText += pUIMan->GetStringFromTable(9386);
						strText += pMonster->name;
						strName.Format(pUIMan->GetStringFromTable(9387), killed, tokill);
					} else strName.Format(pUIMan->GetStringFromTable(256),killed);
					strText += strName;
				}
			}
			for( i = 0; i < MAX_PLAYER_WANTED; i++ ) {
				if (tsi.m_PlayerWanted[i].m_ulPlayersToKill == 0)
					break;
				if (tsi.m_ItemsWanted[i].m_ulItemId > 0)
					continue;
				
				int killed = tsi.m_PlayerWanted[j].m_ulPlayersKilled;
				int tokill = tsi.m_PlayerWanted[j].m_ulPlayersToKill;
		
				strText +=strIndent;
				strText += GetStringFromTable(7630);
				strText += dynamic_cast<CDlgTask*>(m_pAUIManager->GetDialog("Win_Quest"))->GetKillPlayerRequirements(tsi,i);
			}
			if( tsi.m_ulTimeLimit > 0 ) {
				int nSec = bActiveTask ? max(0, tsi.m_ulTimeLimit - tsi.m_ulTimePassed) : tsi.m_ulTimeLimit;

				ACString strPrefix = strIndent;
				strPrefix += CDlgTask::FormatTime(nSec, pUIMan->GetStringFromTable(bActiveTask ? 246 : 245), tsi.m_ulTimeLimit);

				strText += strPrefix;
			}

			if( tsi.m_ulWaitTime > 0 ) {
				int nSec = max(0, int(tsi.m_ulWaitTime) - int(tsi.m_ulTimePassed));

				ACString strPrefix = strIndent;
				strPrefix += CDlgTask::FormatTime(nSec, pUIMan->GetStringFromTable(199), 0);

				strText += strPrefix;
			}

			if( tsi.m_ulNPCToProtect > 0 ) {
				strText += strIndent;

				DATA_TYPE DataType = DT_INVALID;
				MONSTER_ESSENCE *pMonster = (MONSTER_ESSENCE *)pDataMan->get_data_ptr(
					tsi.m_ulNPCToProtect, ID_SPACE_ESSENCE, DataType);
				if( DataType == DT_MONSTER_ESSENCE ) {
					strText += pUIMan->GetStringFromTable(9390);
					strText += pMonster->name;
					strText += _AL("\r");
				}
				int nSec = max(0, tsi.m_ulProtectTime - tsi.m_ulTimePassed);
				ACString strTime = CDlgTask::FormatTime(nSec, pUIMan->GetStringFromTable(259), tsi.m_ulProtectTime);
				strText += strTime;
			}
			
			if(pTemp->m_ulReachSiteCnt > 0) {
				strText += strIndent;
				strText += pUIMan->GetStringFromTable(9379);
				int cur_instance = g_pGame->GetGameRun()->GetWorld()->GetInstanceID();
				A3DVECTOR3 pos;
				if (cur_instance != (int)pTemp->m_ulReachSiteId) {
					ObjectTargets tempCoord;
					CECInstance *pInstance = g_pGame->GetGameRun()->GetInstance(cur_instance);
					ACString strCurMap = AS2AC(pInstance->GetPath());
					pInstance = g_pGame->GetGameRun()->GetInstance(pTemp->m_ulReachSiteId);
					ACString mapName = pInstance ? AS2AC(pInstance->GetPath()) : _AL("");
					g_pGame->GetObjectCoord(mapName, tempCoord);
					ObjectTargets::iterator iter = std::find(tempCoord.begin(), tempCoord.end(), strCurMap);
					if (iter != tempCoord.end()) pos = iter->vPos;
				} else {
					const Task_Region& region = pTemp->m_pReachSite[0];
					pos.x = (region.zvMax.x - region.zvMin.x) / 2 + region.zvMin.x;
					pos.y = (region.zvMax.y - region.zvMin.y) / 2 + region.zvMin.y;
					pos.z = (region.zvMax.z - region.zvMin.z) / 2 + region.zvMin.z;
				}
				ACString strName;
				strName.Format(GetStringFromTable(9393), ((int)pos.x+4000) / 10, ((int)pos.z + 5500) / 10);
				strText += strName;
				strText += _AL("\r");
			}
			ACString strTemp;
			if (pTemp->m_ulReachLevel) {
				strText += strIndent;
				strTemp.Format(pUIMan->GetStringFromTable(9396), pHost->GetBasicProps().iLevel, pTemp->m_ulReachLevel);
				strText += strTemp;
				strText += _AL("\r");
			} 
			if (pTemp->m_ulReachRealmLevel) {
				strText += strIndent;
				strTemp.Format(pUIMan->GetStringFromTable(9397), pUIMan->GetRealmName(pHost->GetRealmLevel()), pUIMan->GetRealmName(pTemp->m_ulReachRealmLevel));
				strText += strTemp;
				strText += _AL("\r");
			} 
			if (pTemp->m_ulReachReincarnationCount) {
				strText += strIndent;
				strTemp.Format(pUIMan->GetStringFromTable(9398), pHost->GetReincarnationCount(), pTemp->m_ulReachReincarnationCount);
				strText += strTemp;
				strText += _AL("\r");
			}
			break;
		}
		strText.TrimRight('\r');
		CECGameSession *pSession = GetGameSession();
		int channel = pHost->GetTeam() ? GP_CHAT_TEAM : GP_CHAT_LOCAL;
		pSession->SendChatData(channel, strText, -1, -1);
		pUIMan->AddChatMessage(strText, channel);
	}

}

void CDlgTaskTrace::OnTaskCompleted(int id)
{
	if (id == (int)m_pBtnUnTrace->GetData())
		SetBtnUnTraceY(-1, 0);
}

void CDlgTaskTrace::OnMouseWheel(WPARAM wParam, LPARAM lParam, AUIObject *pObj)
{
	SetBtnUnTraceY(-1, 0);
}

void CDlgTaskTrace::UpdateContributionTask()
{
	if (GetShowType() != ST_CONTRIBUTION) return;
	CECHostPlayer* pHost = GetHostPlayer();
	CECTaskInterface* pTask = pHost->GetTaskInterface();
	StorageTaskList* pStorageList = (StorageTaskList*)pTask->GetStorageTaskList();
	ActiveTaskList* pActiveLst = (ActiveTaskList*)pTask->GetActiveTaskList();
	ATaskTemplMan* pTaskMan = GetGame()->GetTaskTemplateMan();
	elementdataman* pElementDataMan = GetGame()->GetElementDataMan();

	PrepareRebuildTaskTrace();
	// 显示贡献度和已消费贡献度
	ACString strText;
	strText.Format(_AL("%s%s%s%d   %s%s%s%d/%d"), GetStringFromTable(11289), GetStringFromTable(11280), 
		GetStringFromTable(11292), pHost->GetWorldContribution(), GetStringFromTable(11289), GetStringFromTable(11281), 
		GetStringFromTable(11292), pHost->GetWorldContributionSpend(), 
		pHost->HaveHealthStones() ? 9999 : TASK_WORLD_CONTRIBUTION_SPEND_PER_DAY);
	AppendText(strText);
	AppendText(_AL("\r"));
	//
	m_iIndexForContribution = 0;
	if (pStorageList && pTaskMan && pElementDataMan && pActiveLst) {
		int weightTaskStorageCount = 0;
		for (int i = 1; i <= TASK_STORAGE_COUNT; ++i) {
			const TASK_DICE_BY_WEIGHT_CONFIG* pEssence = pTaskMan->GetWeightTasksEssence(i);
			if (pEssence != NULL) {
				if (m_iContributionCurrentPage == weightTaskStorageCount) {
					std::set<int> TaskSetIDs, TaskIDs;
					// 奖励贡献度的任务
					for (int j = 0; j < pStorageList->m_StoragesTaskSetCount[i - 1]; ++j) {
						int id = pStorageList->m_Storages[i - 1][j];
						if (id == 0) break;
						AddTaskSetString(id, true, i);
						TaskSetIDs.insert(id);
					}
					for (j = pStorageList->m_StoragesTaskSetCount[i - 1]; j < TASK_STORAGE_LEN; ++j) {
						int id = pStorageList->m_Storages[i - 1][j];
						if (id == 0) break;
						AddTaskString(id, true, INDENTATION, true);
						TaskIDs.insert(id);
					}

					AppendText(GetStringFromTable(11290));
					// 不奖励贡献度但已领取未完成的任务					
					ActiveTaskEntry* aEntries = pActiveLst->m_TaskEntries;
					size_t count = sizeof(pEssence->uniform_weight_list) / sizeof(pEssence->uniform_weight_list[0]);
					for (j = 0; j < pActiveLst->m_uTaskCount; ++j) {
						ActiveTaskEntry& CurEntry = aEntries[j];
						if (CurEntry.IsFinished())
							continue;
						int id = CurEntry.m_ID;
						for (size_t k = 0; k < count; ++k) {
							int taskSetID = pEssence->uniform_weight_list[k].task_list_config_id;
							if (taskSetID == 0) break;
							if (TaskSetIDs.find(taskSetID) == TaskSetIDs.end() && TaskListHasID(taskSetID, id)) {
								AddTaskSetString(taskSetID, false, i);
								TaskSetIDs.insert(taskSetID);
							}
						}
						if (TaskIDs.find(id) == TaskIDs.end() && TaskListHasID(pEssence->other_task_list_config_id, id)) {
							AddTaskString(id, false, INDENTATION, true);
							TaskIDs.insert(id);
						}
					}
					
					// 其余不奖励贡献度的任务
					for (j = 0; j < (int)count; ++j) {
						int id = pEssence->uniform_weight_list[j].task_list_config_id;
						if (TaskSetIDs.find(id) == TaskSetIDs.end())
							AddTaskSetString(id, false, i);
					}
					DATA_TYPE dt;
					const TASK_LIST_CONFIG* pConfig = (TASK_LIST_CONFIG*)pElementDataMan->get_data_ptr(pEssence->other_task_list_config_id, ID_SPACE_CONFIG, dt);
					if (dt == DT_TASK_LIST_CONFIG && pConfig) {
						count = sizeof(pConfig->id_tasks) / sizeof(pConfig->id_tasks[0]);
						for (j = 0; j < (int)count; ++j)	{
							int id = pConfig->id_tasks[j];
							if (id == 0) break;
							if (TaskIDs.find(id) == TaskIDs.end())
								AddTaskString(id, false, INDENTATION, true);
						}
					}
				}
				weightTaskStorageCount++;
			}
		}
		m_iContributionTotalPage = weightTaskStorageCount;
	}
	KeepScrollBarPosition();
}

bool CDlgTaskTrace::TaskListHasID(int listID, int taskID)
{
	elementdataman* pElementDataMan = GetGame()->GetElementDataMan();
	DATA_TYPE dt;
	const TASK_LIST_CONFIG* pConfig = (const TASK_LIST_CONFIG*)pElementDataMan->get_data_ptr(listID, ID_SPACE_CONFIG, dt);
	if (pConfig && dt == DT_TASK_LIST_CONFIG) {
		size_t count = sizeof(pConfig->id_tasks) / sizeof(pConfig->id_tasks[0]);
		for (size_t i = 0; i < count; ++i) {
			int id = pConfig->id_tasks[i];
			if (id == 0) break;
			if (id == taskID) 
				return true;
		}
	}
	return false;
}

void CDlgTaskTrace::AddTaskSetString(unsigned int id, bool hasContributionAward, int idStorage)
{
	elementdataman* pElementDataMan = GetGame()->GetElementDataMan();
	CECTaskInterface* pTask = GetHostPlayer()->GetTaskInterface();
	ActiveTaskList* pActiveLst = (ActiveTaskList*)pTask->GetActiveTaskList();
	DATA_TYPE dt;
	const TASK_LIST_CONFIG* pConfig = (const TASK_LIST_CONFIG*)pElementDataMan->get_data_ptr(id, ID_SPACE_CONFIG, dt);
	const TASK_DICE_BY_WEIGHT_CONFIG* pStorage = GetGame()->GetTaskTemplateMan()->GetWeightTasksEssence(idStorage);
	if (pConfig && dt == DT_TASK_LIST_CONFIG && pStorage) {
		size_t count = sizeof(pStorage->uniform_weight_list) / sizeof(pStorage->uniform_weight_list[0]);
		int weight = 0;
		for (size_t i = 0; i < count; ++i) {
			if(pStorage->uniform_weight_list[i].task_list_config_id == 0) {
				ASSERT(0);
				return;
			}
			if (pStorage->uniform_weight_list[i].task_list_config_id == id) {
				weight = pStorage->uniform_weight_list[i].weight;
				break;
			}
		}
		ACString strColor;
		if (hasContributionAward) {
			strColor = GetStringFromTable(11286);
			AppendText(strColor);
			AppendText(GetStringFromTable(11284));
		} else {
			weight = -weight;
			strColor = GetStringFromTable(11287);
			AppendText(strColor);
			AppendText(GetStringFromTable(11285));
		}

		ACString strName = pConfig->name;
		AppendColorText(strName, strColor);

		ActiveTaskEntry* aEntries = pActiveLst->m_TaskEntries;
		for (int j = 0; j < pActiveLst->m_uTaskCount; ++j) {
			ActiveTaskEntry& CurEntry = aEntries[j];

			int taskID = CurEntry.m_ID;
			if (TaskListHasID(id, taskID))
				AddTaskString(taskID, hasContributionAward, INDENTATION);
		}	
	}
}

void CDlgTaskTrace::AddTaskString(unsigned int id, bool hasContributionAward, const ACString& strIndentIn, bool topLevel)
{
	CECTaskInterface* pTask = GetHostPlayer()->GetTaskInterface();
	ATaskTemplMan* pTaskMan = GetGame()->GetTaskTemplateMan();	
	ActiveTaskList* pActiveLst = (ActiveTaskList*)pTask->GetActiveTaskList();
	ATaskTempl* pTempl = pTaskMan->GetTaskTemplByID(id);
	ActiveTaskEntry* pEntry = pActiveLst->GetEntry(id);
	if (pTempl) {
		int contribution = 0;
		// 奖励贡献度的任务只在顶层任务显示奖励值
		if (hasContributionAward) {
			contribution = pTempl->m_pParent == NULL ? pTempl->m_Award_S->m_iWorldContribution : 0;
		} else { // 否则只在最底层任务显示消耗值
			contribution = pTempl->m_pFirstChild == NULL ? -pTempl->m_iWorldContribution : 0;
		}
		
		bool bCanContributionFinish = !hasContributionAward;
//		ACString strTask = pTempl->GetName();
		ACString strTask = CDlgTask::GetTaskNameWithColor(pTempl);
		// 顶层显示一个任务名字
		if (topLevel)
		{
			int indexMark = hasContributionAward ? 11284 : 11285;
			int indexColor = bCanContributionFinish ? 11287 : 11286;
			AppendText(GetStringFromTable(indexColor));
			AppendText(GetStringFromTable(indexMark));
			AppendColorText(strTask, GetStringFromTable(indexColor));
		}
		// 如果是奖励贡献度的任务但领取时间不是当天，则不显示
		if (hasContributionAward && pEntry) {
			unsigned long ulCurTime = pTask->GetCurTime();
			unsigned long ulTaskTime = pEntry->m_ulTaskTime;
			tm tmCur, tmTask;
			ulCurTime -= unsigned long(TaskInterface::GetTimeZoneBias() * 60);
			ulTaskTime -= unsigned long(TaskInterface::GetTimeZoneBias() * 60);
			
			if ((long)(ulCurTime) < 0)
				ulCurTime = 0;
			
			if ((long)(ulTaskTime) < 0)
				ulTaskTime = 0;
			
			tmCur = *gmtime((time_t*)&ulCurTime);
			tmTask = *gmtime((time_t*)&ulTaskTime);
			if (!(tmCur.tm_year == tmTask.tm_year && tmCur.tm_yday == tmTask.tm_yday))
				return;
		}
		if (pTask->HasTask(id)) {
			FormatTaskTraceDesc(pTempl->m_ID, true, strIndentIn, false, ++m_iIndexForContribution, bCanContributionFinish, contribution);
		}
		ActiveTaskEntry* aEntries = pActiveLst->m_TaskEntries;
		for (int j = 0; j < pActiveLst->m_uTaskCount; ++j) {
			ActiveTaskEntry& CurEntry = aEntries[j];

			int taskID = CurEntry.m_ID;
			if (CurEntry.m_ParentIndex != 0xff) {
				ActiveTaskEntry& ParentEntry = aEntries[CurEntry.m_ParentIndex];
				if (ParentEntry.m_ID == id)
					AddTaskString(taskID, hasContributionAward, strIndentIn + INDENTATION);
			}
		}
	}
}

void CDlgTaskTrace::FinishTaskSpendingContribution(int taskID)
{
	CECTaskInterface *pTask = GetHostPlayer()->GetTaskInterface();
	ATaskTemplMan *pMan = GetGame()->GetTaskTemplateMan();
	ATaskTempl *pTemp = pMan->GetTaskTemplByID(taskID);
	ActiveTaskList* pActiveLst = (ActiveTaskList*)pTask->GetActiveTaskList();
	ActiveTaskEntry* pEntry = pActiveLst->GetEntry(taskID);
	if (pTemp && pEntry && !pEntry->IsFinished()) {
		if (pTemp->m_Award_S->m_ulCandItems > 1) {
			CDlgAward* pAward = dynamic_cast<CDlgAward*>(m_pAUIManager->GetDialog("Win_Award"));
			if (pAward) {
				pAward->UpdateAwardItem(taskID, false, true);
				pAward->Show(true);
			}
		}
		else GetHostPlayer()->GetTaskInterface()->FinishTaskSpendingWorldContribution(taskID);
		UpdateContributionTask();
	}
}
extern CECStringTab _task_err;
void CDlgTaskTrace::OnCommand_FinishTask(const char * szCommand)
{
	PAUIOBJECT pObj = GetDlgItem(szCommand);
	if (pObj) {
		int idTask = pObj->GetData();
		if (idTask) {
			const ATaskTempl* pTempl = GetGame()->GetTaskTemplateMan()->GetTaskTemplByID(idTask);
			if (pTempl) {
				CECHostPlayer* pHost = GetHostPlayer();
				CECGameUIMan* pGameUI = GetGameUIMan();
				CECTaskInterface* pTask = pHost->GetTaskInterface();
				Task_State_info tsi;
				pTask->GetTaskStateInfo(idTask, &tsi);
				if (tsi.m_ulErrCode)
				{
					const wchar_t* szMsg = _task_err.GetWideString(tsi.m_ulErrCode);
					
					if (szMsg)
					{
						ACString strText = szMsg;
						ACString strTemp;
						if (tsi.m_ulErrCode == TASK_AWARD_FAIL_LEVEL_CHECK)
							strTemp.Format(GetStringFromTable(7637), tsi.m_ulPremLevelMin);	
						else strTemp = GetStringFromTable(807);
						strText += strTemp;
						strText += _AL("\r");
						strText += COLOR_WHITE;
						pGameUI->MessageBox("", strText, MB_OK, A3DCOLORRGBA(255, 255, 255, 160));
						return;
					}
				}
				if (pHost->GetWorldContribution() < pTempl->m_iWorldContribution)
					pGameUI->MessageBox("", pGameUI->GetStringFromTable(11282), MB_OK, A3DCOLORRGBA(255, 255, 255, 160));
				else if (!pHost->HaveHealthStones() && 
					pHost->GetWorldContributionSpend() + pTempl->m_iWorldContribution > TASK_WORLD_CONTRIBUTION_SPEND_PER_DAY)
					pGameUI->MessageBox("", pGameUI->GetStringFromTable(11283), MB_OK, A3DCOLORRGBA(255, 255, 255, 160));
				else 
					FinishTaskSpendingContribution(idTask);
			}
		}
	}	
}

void CDlgTaskTrace::AppendColorText(ACString& strTask, const ACString& strColor)
{
	ACString strText;
	DeleteColorStr(strTask);
	strText.Format(_AL("%s%s"), strColor, strTask);
	AppendText(strText);
	AppendText(_AL("\r"));
}

void CDlgTaskTrace::KeepScrollBarPosition()
{
	//	列表类型未改变时，保持滚动条位置
	ShowType showType = GetShowType();
	if (m_nLastShowType == showType)
	{
		int lastTop = m_pTxt_Desc->GetFirstLine();
		m_pTxt_Desc->SetText(m_Buffer);
		int curTop = m_pTxt_Desc->GetFirstLine();		
		if(lastTop != curTop)
		{
			int top = (lastTop < m_pTxt_Desc->GetLines()) ? lastTop 
				: max(0, m_pTxt_Desc->GetLines() - m_pTxt_Desc->GetLinesPP());
			m_pTxt_Desc->SetFirstLine(top);
		}		
	}
	else m_pTxt_Desc->SetText(m_Buffer);

	if (showType != m_nLastShowType)
		SetShowType(showType);
}

void CDlgTaskTrace::OnTaskNew(unsigned long idTask)
{
	ATaskTemplMan* pMan = GetGame()->GetTaskTemplateMan();
	ATaskTempl* pTempl = pMan->GetTaskTemplByID(idTask);
	if (pTempl && 
		GetShowType() != ST_CONTRIBUTION && 
		pTempl->GetTopTask()->CanDeliverWorldContribution(GetHostPlayer()->GetTaskInterface()) && 
		GetHostPlayer()->GetMaxLevelSofar() >= CECUIConfig::Instance().GetGameUI().nContributionTaskLevelLimit) {
		SetShowType(ST_CONTRIBUTION);
		OnCommand_SwitchShowType(NULL);
		if (!IsShow()) {
			CDlgTask* pDlg = dynamic_cast<CDlgTask*>(m_pAUIManager->GetDialog("Win_Quest"));
			if (pDlg)
				pDlg->OnCommand_showtrace(NULL);
		}
	}
}