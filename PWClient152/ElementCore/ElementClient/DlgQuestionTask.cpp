// Filename	: DlgQuestionTask.cpp
// Creator	: zhangyitian
// Date		: 2014/11/11

#include "DlgQuestionTask.h"
#include "DlgInfo.h"
#include "EC_UIHelper.h"
#include "EC_HostPlayer.h"
#include "EC_GameUIMan.h"
#include "Task/TaskTempl.h"
#include "Task/EC_TaskInterface.h"
#include "Task/TaskTemplMan.h"

#include <AUIStillImageButton.h>
#include <AUITextArea.h>
#include <AUILabel.h>

const int QUESTION_TASK_TYPE = enumTTQuestion;

AUI_BEGIN_COMMAND_MAP(CDlgQuestionTask, CDlgBase)
AUI_ON_COMMAND("Btn_Next",	OnCommand_Next)
AUI_ON_COMMAND("Btn_Prev",	OnCommand_Prev)
AUI_ON_COMMAND("Btn_Close",	OnCommand_Minmize)
AUI_ON_COMMAND("IDCANCEL",	OnCommand_Minmize)
AUI_END_COMMAND_MAP()

CDlgQuestionTask::CDlgQuestionTask() {
	m_pTxtQuest = NULL;
	m_pLblTime = NULL;
	m_pBtnNext = NULL;
	m_pBtnPrev = NULL;

	m_iCurQuestionTask = 0;
}

bool CDlgQuestionTask::OnInitDialog() {
	DDX_Control("Txt_Q",	m_pTxtQuest);
	DDX_Control("Lbl_Time",	m_pLblTime);
	DDX_Control("Btn_Next",	m_pBtnNext);
	DDX_Control("Btn_Prev", m_pBtnPrev);

	return CDlgBase::OnInitDialog();
}

void CDlgQuestionTask::OnShowDialog() {
	CDlgBase::OnShowDialog();
}

void CDlgQuestionTask::OnTick() {
	UpdateTimeLimit();
	CDlgBase::OnTick();
}

void CDlgQuestionTask::UpdateView() {
	UpdateQuestDesc();
	UpdateTimeLimit();
	UpdateSwitchBtn();
}

void CDlgQuestionTask::AddQuestionTask(int iTaskID) {
	// 需要iTaskID为答题父任务
	if (!IsLegalTask(iTaskID, true)) {
		return;
	}

	m_vecQuestionTasks.push_back(iTaskID);
	if (m_iCurQuestionTask == 0) {
		m_iCurQuestionTask = iTaskID;
		if (!IsShow()) {
			Show(true);
		}
		UpdateView();
	} else {
		UpdateSwitchBtn();
	}
}

void CDlgQuestionTask::RemoveQuestionTask(int iTaskID) {
	// 需要iTaskID为答题父任务的第一个子任务或父任务
	if (!IsLegalTask(iTaskID, false)) {
		return;
	}
	
	// 这个任务是答题任务的第一个子任务或父任务，找到父任务的ID
	ATaskTemplMan *pMan = CECUIHelper::GetGame()->GetTaskTemplateMan();
	ATaskTempl *pTemp = pMan->GetTaskTemplByID(iTaskID);
	int iParentID;
	if (pTemp->m_pParent) {
		iParentID = pTemp->m_pParent->m_ID;
	} else {
		iParentID = pTemp->m_ID;
	}
	

	int iIndexOfCurTask = GetIndexOfTask(iParentID);
	if (iIndexOfCurTask == -1) {
		return;
	}

	m_vecQuestionTasks.erase(m_vecQuestionTasks.begin() + iIndexOfCurTask);

	if (m_vecQuestionTasks.empty()) {
		m_iCurQuestionTask = 0;
		if (IsShow()) {
			Show(false);
		}
		GetGameUIMan()->RemoveInformation(CDlgInfo::INFO_QUESTIONTASK, _AL(""));
	} else {
		if (iParentID == m_iCurQuestionTask) {
			m_iCurQuestionTask = m_vecQuestionTasks[0];
			UpdateView();
		} else {
			UpdateSwitchBtn();
		}
	}
}

int CDlgQuestionTask::GetIndexOfTask(int iTaskID) {
	int i;
	for (i = 0; i < m_vecQuestionTasks.size(); i++) {
		if (m_vecQuestionTasks[i] == iTaskID) {
			return i;
		}
	}
	return -1;
}

bool CDlgQuestionTask::IsLegalTask(int iTaskID, bool bNewTask) {
	ATaskTemplMan *pMan = CECUIHelper::GetGame()->GetTaskTemplateMan();
	ATaskTempl *pTemp = pMan->GetTaskTemplByID(iTaskID);
	if (!pTemp || pTemp->GetType() != QUESTION_TASK_TYPE) {
		return false;
	}
	
	if (bNewTask) {
		// 如果是新任务发放，则iTaskID一定是父任务，因为OnServerNotify里的参数是父任务ID
		if (pTemp->m_pParent) {
			return false;
		}
	}
	// 如果是任务完成或任务放弃，则iTask可能是父任务也可能是子任务，因此这里不做判断
	
	CECTaskInterface *pTask = GetHostPlayer()->GetTaskInterface();
	if (pTask->HasTask(iTaskID) != bNewTask) {
		return false;
	}

	return true;
}

void CDlgQuestionTask::UpdateQuestDesc() {
	ATaskTemplMan *pMan = g_pGame->GetTaskTemplateMan();
	ATaskTempl *pTemp = pMan->GetTaskTemplByID(m_iCurQuestionTask);
	if (!pTemp || pTemp->GetType() != QUESTION_TASK_TYPE) {
		return;
	}
	
	ACString strDesc = pTemp->GetDescription();
	m_pTxtQuest->SetText(strDesc);
}

void CDlgQuestionTask::UpdateTimeLimit() {
	ATaskTemplMan *pMan = g_pGame->GetTaskTemplateMan();
	ATaskTempl *pTemp = pMan->GetTaskTemplByID(m_iCurQuestionTask);
	if (!pTemp || pTemp->GetType() != QUESTION_TASK_TYPE) {
		return;
	}

	CECTaskInterface *pTask = GetHostPlayer()->GetTaskInterface();
	Task_State_info tsi;
	
	pTask->GetTaskStateInfo(pTemp->m_pFirstChild->m_ID, &tsi, true);
	int iTimeLimit = pTemp->m_pFirstChild->m_enumMethod == enumTMWaitTime ? pTemp->m_pFirstChild->m_ulWaitTime : 0;
	int iTimePassed = tsi.m_ulTimePassed;
	

	int iTimeLeft = iTimeLimit - iTimePassed;
	if (iTimeLeft < 0) {
		iTimeLeft = 0;
	}
	m_pLblTime->SetText(ACString().Format(_AL("%d"), (int)iTimeLeft));
}

void CDlgQuestionTask::UpdateSwitchBtn() {
	if (m_vecQuestionTasks.size() <= 1) {
		if (m_pBtnNext->IsShow()) {
			m_pBtnNext->Show(false);
		}
		if (m_pBtnPrev->IsShow()) {
			m_pBtnPrev->Show(false);
		}
	} else {
		int iIndex = GetIndexOfTask(m_iCurQuestionTask);
		if (!m_pBtnNext->IsShow() && iIndex != m_vecQuestionTasks.size() - 1) {
			m_pBtnNext->Show(true);
		} else if (m_pBtnNext->IsShow() && iIndex == m_vecQuestionTasks.size() - 1) {
			m_pBtnNext->Show(false);
		}

		if (!m_pBtnPrev->IsShow() && iIndex != 0) {
			m_pBtnPrev->Show(true);
		} else if (m_pBtnPrev->IsShow() && iIndex == 0) {
			m_pBtnPrev->Show(false);
		}
	}
}

void CDlgQuestionTask::OnCommand_Next(const char* szCommand) {
	int iIndex = GetIndexOfTask(m_iCurQuestionTask);
	if (iIndex != -1 && iIndex != m_vecQuestionTasks.size() - 1) {
		m_iCurQuestionTask = m_vecQuestionTasks[iIndex + 1];
		UpdateView();
	}
}

void CDlgQuestionTask::OnCommand_Prev(const char* szCommand) {
	int iIndex = GetIndexOfTask(m_iCurQuestionTask);
	if (iIndex != -1 && iIndex != 0) {
		m_iCurQuestionTask = m_vecQuestionTasks[iIndex - 1];
		UpdateView();
	}
}

void CDlgQuestionTask::OnCommand_Minmize(const char* szCommand) {
	Show(false);
	GetGameUIMan()->AddInformation(CDlgInfo::INFO_QUESTIONTASK, "Game_QuestionTask", _AL(""), 1000000, 0, 0, 0);
}