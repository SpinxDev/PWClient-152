// Filename	: DlgExclusiveAward.cpp
// Creator	: Han Guanghui
// Date		: 2013/08/24

#include "DlgExclusiveAward.h"
#include "DlgAutoTask.h"
#include "DlgTask.h"
#include "ExpTypes.h"
#include "AFI.h"
#include "AUIImagepicture.h"
#include "AUICTranslate.h"
#include "AUIDef.h"
#include "EC_GameUIMan.h"
#include "EC_GameSession.h"
#include "EC_HostPlayer.h"
#include "EC_IvtrItem.h"
#include "EC_Inventory.h"
#include "TaskTemplMan.h"
#include "EC_TaskInterface.h"
#include "EC_IvtrItem.h"
#include "EC_IvtrEquip.h"
#include "EC_Configs.h"
#include "EC_Game.h"

#define new A_DEBUG_NEW

AUI_BEGIN_COMMAND_MAP(CDlgExclusiveAward, CDlgBase)
AUI_ON_COMMAND("Btn_Award*",	OnCommandConfirm)
AUI_ON_COMMAND("IDCANCEL",		OnCommandCancel)
AUI_ON_COMMAND("Btn_Cancel",	OnCommandCancel)
AUI_ON_COMMAND("Btn_Up",		OnCommandUp)
AUI_ON_COMMAND("Btn_Down",		OnCommandDown)
AUI_ON_COMMAND("Chk_Temp",		OnCommandCheckMode)
AUI_END_COMMAND_MAP()

AUI_BEGIN_EVENT_MAP(CDlgExclusiveAward, CDlgBase)
AUI_ON_EVENT("*", WM_MOUSEWHEEL, OnMouseWheel)
AUI_END_EVENT_MAP()

CDlgExclusiveAward::CDlgExclusiveAward():
m_iStartIndex(0),
m_nPageSize(0),
m_pChk_NoAlways(NULL)
{
}

CDlgExclusiveAward::~CDlgExclusiveAward()
{
}

bool CDlgExclusiveAward::OnInitDialog()
{
	DDX_Control("Chk_Temp",m_pChk_NoAlways);
	m_UpdateCounter.SetPeriod(500);
	m_nPageSize = CalculatePageSize();
	return true;
}

int CDlgExclusiveAward::CalculatePageSize(){
	int result(0);
	char szTemp[64] = {0};
	while (true){
		sprintf(szTemp, "Btn_Award%d", result+1);
		if (!GetDlgItem(szTemp)){
			break;
		}
		++ result;
	}
	return result;
}

void CDlgExclusiveAward::OnCommandConfirm(const char* szCommand)
{
	PAUIOBJECT pObj = GetDlgItem(szCommand);
	if (pObj) {
		unsigned long task_id = pObj->GetData();
		CDlgAutoTask* pDlg = dynamic_cast<CDlgAutoTask*>(GetGameUIMan()->GetDialog("Win_AutoTask"));
		if (pDlg) pDlg->ConfirmAccept(task_id);
	}
}

void CDlgExclusiveAward::OnShowDialog()
{
	m_pChk_NoAlways->Check(GetExclusiveAwardMode());
	Update();
}

void CDlgExclusiveAward::ScrollPage(bool bUp, int count)
{
	if (bUp){
			m_iStartIndex -= count;
			a_ClampFloor(m_iStartIndex, 0);
	}else{
		if (IsValidTaskIndex(m_iStartIndex + count)){
			m_iStartIndex += count;
		}
	}
	UpdatePage();
}
void CDlgExclusiveAward::OnCommandCancel(const char *szCommand)
{
	Show(false);
}
void CDlgExclusiveAward::OnCommandUp(const char* szCommand)
{
	ScrollPage(true, m_nPageSize);
}
void CDlgExclusiveAward::OnCommandDown(const char* szCommand)
{
	ScrollPage(false, m_nPageSize);
}
void CDlgExclusiveAward::OnCommandCheckMode(const char* szCommand)
{
	EC_GAME_SETTING setting = GetGame()->GetConfigs()->GetGameSettings();
	setting.bExclusiveAwardMode = m_pChk_NoAlways->IsChecked();
	GetGame()->GetConfigs()->SetGameSettings(setting);

}
bool CDlgExclusiveAward::GetExclusiveAwardMode()
{
	return GetGame()->GetConfigs()->GetGameSettings().bExclusiveAwardMode;
}
void CDlgExclusiveAward::OnMouseWheel(WPARAM wParam, LPARAM lParam, AUIObject *pObj)
{
	int zDelta = (short)HIWORD(wParam);
	ScrollPage(zDelta >= 0, 1);
}
void CDlgExclusiveAward::OnTick()
{
	if (m_UpdateCounter.IncCounter(GetGame()->GetRealTickTime())) {
		m_UpdateCounter.Reset();
		Update();
	}
}
static unsigned long ulCurTime = 0;
static ActiveTaskList* pList = NULL;
static CECTaskInterface* pTask = NULL;
static bool compare_time(const ATaskTempl* lhs, const ATaskTempl* rhs)
{
	if (!lhs || !rhs || !pList || !pTask) return true; 
	unsigned long remain_time_lhs(0), remain_time_rhs(0);
	ActiveTaskEntry* entry_lhs = pList->GetEntry(lhs->m_ID);
	ActiveTaskEntry* entry_rhs = pList->GetEntry(rhs->m_ID);
	if (entry_lhs == NULL)
		return false;
	else if (entry_rhs == NULL)
		return true;
	unsigned long reach_time_lhs = entry_lhs->m_ulTaskTime + lhs->m_ulWaitTime;
	unsigned long reach_time_rhs = entry_rhs->m_ulTaskTime + rhs->m_ulWaitTime;
	remain_time_lhs = reach_time_lhs > ulCurTime ? reach_time_lhs - ulCurTime : 0;
	remain_time_rhs = reach_time_rhs > ulCurTime ? reach_time_rhs - ulCurTime : 0;
	return reach_time_lhs < reach_time_rhs;
}
static bool compare_level(const ATaskTempl* lhs, const ATaskTempl* rhs)
{
	if (!lhs || !rhs) return true;
	return lhs->m_ulPremise_Lev_Min < rhs->m_ulPremise_Lev_Min;
}
static bool compare_realm(const ATaskTempl* lhs, const ATaskTempl* rhs)
{
	if (!lhs || !rhs) return true;
	if (lhs->m_ulPremRealmLevelMin < rhs->m_ulPremRealmLevelMin) return true;
	else if (lhs->m_ulPremRealmLevelMin > rhs->m_ulPremRealmLevelMin) return false;
	else {
		if (lhs->m_ulPremise_Lev_Min < rhs->m_ulPremise_Lev_Min) return true;
		else if (lhs->m_ulPremise_Lev_Min > rhs->m_ulPremise_Lev_Min) return false;
		else return compare_time(lhs, rhs);
	}
}
bool CDlgExclusiveAward::HasTaskToShow()
{
	Update();
	return HasTask();
}
void CDlgExclusiveAward::GenerateTaskVec()
{
	pTask = GetHostPlayer()->GetTaskInterface();
	pList = (ActiveTaskList*)pTask->GetActiveTaskList();
	TaskTemplMap& exclusive_map = GetGame()->GetTaskTemplateMan()->GetExclusiveAwardMap();
	ActiveTaskEntry* pEntries = pList->m_TaskEntries;
	ulCurTime = pTask->GetCurTime();
	const ATaskTempl* pTempl;
	
	task_ready_to_finish.clear();
	task_realm.clear();
	task_level.clear();
	task_time.clear();

	for (TaskTemplMap::iterator iter = exclusive_map.begin(); iter != exclusive_map.end(); ++iter) {
		ActiveTaskEntry* CurEntry = pList->GetEntry(iter->first);
		pTempl = iter->second;
		if (pTempl && pTempl->m_bDisplayInExclusiveUI) {
			if (CurEntry != NULL) {
				// 已经可以完成的任务
				if (pTempl->CanFinishTask(pTask, CurEntry, ulCurTime) && 
					pTempl->RecursiveCheckAward(pTask, pList, CurEntry, ulCurTime, -1) == 0)
					task_ready_to_finish.push_back(pTempl);
				else if(pTempl->m_bPremCheckRealmLevel)
					task_realm.push_back(pTempl);
				else if (pTempl->m_ulPremise_Lev_Min)
					task_level.push_back(pTempl);
				else if (pTempl->m_enumMethod == enumTMWaitTime)
					task_time.push_back(pTempl);
			} else if (pTempl->CanShowInExclusiveUI(pTask, ulCurTime)){
				if(pTempl->m_bPremCheckRealmLevel)
					task_realm.push_back(pTempl);
				else if (pTempl->m_ulPremise_Lev_Min)
					task_level.push_back(pTempl);
				else if (pTempl->m_enumMethod == enumTMWaitTime)
					task_time.push_back(pTempl);
			}
		}
	}
	
	std::sort(task_realm.begin(), task_realm.end(), compare_realm);
	std::sort(task_level.begin(), task_level.end(), compare_level);
	std::sort(task_time.begin(), task_time.end(), compare_time);
}
void CDlgExclusiveAward::Update()
{
	GenerateTaskVec();
	ClampPageStartIndex();
	UpdatePage();
}

void CDlgExclusiveAward::UpdatePage(){
	TASKS_TO_SHOW all_tasks = GetTasksToShow();
	for (int i(0); i < m_nPageSize; ++ i){
		if (const ATaskTempl* pShow = GetOneTaskTempl(all_tasks, m_iStartIndex + i)){
			ShowLine(i, true);
			UpdateOneTask(pShow, i, ulCurTime);
		}else{
			ShowLine(i, false);
		}
	}
	GetDlgItem("Btn_Up")->Enable(HasPreviousPage());
	GetDlgItem("Btn_Down")->Enable(HasNextPage());
}

void CDlgExclusiveAward::ClampPageStartIndex(){
	a_ClampRoof(m_iStartIndex, MaxTaskIndex());
}

CDlgExclusiveAward::TASKS_TO_SHOW CDlgExclusiveAward::GetTasksToShow(){	
	TASKS_TO_SHOW result;
	result.push_back(&task_ready_to_finish);
	result.push_back(&task_realm);
	result.push_back(&task_level);
	result.push_back(&task_time);
	return result;
}

bool CDlgExclusiveAward::HasTask(){
	return !task_ready_to_finish.empty()
		|| !task_realm.empty()
		|| !task_level.empty()
		|| !task_time.empty();
}

int CDlgExclusiveAward::GetTaskCount(){
	int result(0);
	TASKS_TO_SHOW all_tasks = GetTasksToShow();
	for (int i(0); i < all_tasks.size(); ++ i){
		result += all_tasks[i]->size();
	}
	return result;
}

int CDlgExclusiveAward::LastPageStartIndex(){
	int taskCount = GetTaskCount();
	if (taskCount <= 0){
		return 0;
	}
	if (taskCount % m_nPageSize){
		return taskCount / m_nPageSize * m_nPageSize;
	}else{
		return (taskCount / m_nPageSize -1) * m_nPageSize;
	}
}

int	CDlgExclusiveAward::MaxTaskIndex(){
	return HasTask() ? GetTaskCount()-1 : 0;
}

bool CDlgExclusiveAward::IsValidTaskIndex(int index){
	return index >= 0
		&& index < GetTaskCount();
}

bool CDlgExclusiveAward::HasPreviousPage(){
	return m_iStartIndex > 0;
}

bool CDlgExclusiveAward::HasNextPage(){
	return IsValidTaskIndex(m_iStartIndex + m_nPageSize);
}

const ATaskTempl* CDlgExclusiveAward::GetOneTaskTempl(TASKS_TO_SHOW& task_to_show, int index)
{
	int cur_index = index;
	int i = 0;
	const ATaskTempl* ret = NULL;
	while (true)
	{
		if (i >= (int)task_to_show.size()) break;
		if ((int)task_to_show[i]->size() > cur_index) {
			ret = (*task_to_show[i])[cur_index];
			break;
		} else {
			cur_index -= task_to_show[i]->size();
			++i;
		}
	}
	return ret;
}
void CDlgExclusiveAward::UpdateOneTask(const ATaskTempl* pTempl, int iLine, unsigned long ulCurTime)
{
	while (true)
	{
		if (!pTempl || !pList || !pTask){
			break;
		}

		char szTemp[50];
		ACString strTemp;

		int controlIndex = iLine+1;

		sprintf(szTemp, "Lbl_Name%d", controlIndex);
		GetDlgItem(szTemp)->SetText(CDlgTask::GetTaskNameWithColor(pTempl));
		
		if (pTempl->m_ulPremise_Lev_Min) {
			strTemp.Format(_AL("%d"), pTempl->m_ulPremise_Lev_Min);
		}else{
			strTemp = _AL("-");
		}
		sprintf(szTemp, "Lbl_Level%d", controlIndex);
		GetDlgItem(szTemp)->SetText(strTemp);

		if (pTempl->m_bPremCheckRealmLevel) {
			strTemp.Format(_AL("%d"), pTempl->m_ulPremRealmLevelMin);
		}else{
			strTemp = _AL("-");
		}
		sprintf(szTemp, "Lbl_Realm%d", controlIndex);
		GetDlgItem(szTemp)->SetText(strTemp);		
		
		if (pTempl->m_enumMethod == enumTMWaitTime) {
			ActiveTaskEntry* CurEntry = pList->GetEntry(pTempl->m_ID);
			int nNum(0);
			if (CurEntry) {
				if (CurEntry->m_ulTaskTime + pTempl->m_ulWaitTime > ulCurTime)
					nNum = CurEntry->m_ulTaskTime + pTempl->m_ulWaitTime - ulCurTime;
				else nNum = 0;
			} else nNum = pTempl->m_ulWaitTime;
			strTemp = GetGameUIMan()->GetFormatTime(nNum);
		}else{
			strTemp = _AL("-");
		}
		sprintf(szTemp, "Lbl_Time%d", controlIndex);
		GetDlgItem(szTemp)->SetText(strTemp);

		bool bNoItem = pTempl->m_Award_S->m_ulCandItems == 0 || pTempl->m_Award_S->m_CandItems == NULL;
		for (int i = 1, j = 0; ; ++i){
			sprintf(szTemp, "Img_%d%d", controlIndex, i);
			PAUIIMAGEPICTURE pImg = dynamic_cast<PAUIIMAGEPICTURE>(GetDlgItem(szTemp));
			if (!pImg) break;
			if (bNoItem) {
				pImg->ClearCover();
				pImg->SetHint(_AL(""));
				pImg->SetText(_AL(""));
			} else {
				if (j < (int)pTempl->m_Award_S->m_CandItems[0].m_ulAwardItems) {
					int idItem = pTempl->m_Award_S->m_CandItems[0].m_AwardItems[j].m_ulItemTemplId;
					CECIvtrItem* pItem = CECIvtrItem::CreateItem(idItem, 0, 1);
					
					int nNum = pTempl->m_Award_S->m_CandItems[0].m_AwardItems[j].m_ulItemNum;
					if( nNum > 1 )
					{
						strTemp.Format(_AL("%d"), nNum);
						pImg->SetText(strTemp);
					}
					else
						pImg->SetText(_AL(""));
					
					pItem->GetDetailDataFromLocal();
					AUICTranslate trans;
					pImg->SetHint(trans.Translate(pItem->GetDesc()));
					GetGameUIMan()->SetCover(pImg, pItem->GetIconFile());
					
					pImg->SetColor(
						(pItem->IsEquipment() && ((CECIvtrEquip *)pItem)->IsDestroying())
						? A3DCOLORRGB(128, 128, 128)
						: A3DCOLORRGB(255, 255, 255));
					
					delete pItem;
					j++;
				} else {
					pImg->ClearCover();
					pImg->SetHint(_AL(""));
					pImg->SetText(_AL(""));
				}
			}	
		}

		sprintf(szTemp, "Btn_Award%d", controlIndex);
		PAUIOBJECT pObj = GetDlgItem(szTemp);
		pObj->Enable(std::find(task_ready_to_finish.begin(), task_ready_to_finish.end(), pTempl) != task_ready_to_finish.end());
		pObj->SetData(pTempl->m_ID);
		break;
	}
}

void CDlgExclusiveAward::ShowLine(int iLine, bool bShow){
	while (true){
		int controlIndex = iLine+1;
		char szTemp[50];
		sprintf(szTemp, "Lbl_Name%d", controlIndex);
		PAUIOBJECT pObj = GetDlgItem(szTemp);
		if (pObj->IsShow() == bShow){
			break;
		}
		pObj->Show(bShow);
		
		sprintf(szTemp, "Lbl_Level%d", controlIndex);
		GetDlgItem(szTemp)->Show(bShow);
		
		sprintf(szTemp, "Lbl_Realm%d", controlIndex);
		GetDlgItem(szTemp)->Show(bShow);
		
		sprintf(szTemp, "Lbl_Time%d", controlIndex);
		GetDlgItem(szTemp)->Show(bShow);
		
		for (int i = 1;; ++i){
			sprintf(szTemp, "Img_%d%d", controlIndex, i);
			PAUIIMAGEPICTURE pImg = dynamic_cast<PAUIIMAGEPICTURE>(GetDlgItem(szTemp));
			if (!pImg) break;
			pImg->Show(bShow);
			sprintf(szTemp, "Img_Item%d%d", controlIndex, i);			
			pImg = dynamic_cast<PAUIIMAGEPICTURE>(GetDlgItem(szTemp));
			if (pImg){
				pImg->Show(bShow);
			}
		}
		
		sprintf(szTemp, "Btn_Award%d", controlIndex);
		GetDlgItem(szTemp)->Show(bShow);
		break;
	}
}
