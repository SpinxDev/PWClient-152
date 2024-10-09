#include "AUIEditBox.h"
#include "DlgTaskConfirm.h"
#include "DlgAward.h"
#include "DlgTask.h"
#include "EC_Game.h"
#include "EC_GameRun.h"
#include "EC_GameUIMan.h"
#include "EC_HostPlayer.h"
#include "EC_TaskInterface.h"
#include "TaskTemplMan.h"

#define new A_DEBUG_NEW



AUI_BEGIN_COMMAND_MAP(CDlgTaskConfirm, CDlgBase)

AUI_ON_COMMAND("Btn_Done*", OnCommandFinishTask)
AUI_ON_COMMAND("Btn_Zoom", OnCommandMinimize)

AUI_END_COMMAND_MAP()

CDlgTaskConfirm::CDlgTaskConfirm()
{
}

CDlgTaskConfirm::~CDlgTaskConfirm()
{
}


bool CDlgTaskConfirm::OnInitDialog()
{
	for (int i = 0;; i++)
	{		
		AString strName;
		strName.Format("Lbl_Quest%d", i + 1);
		PAUILABEL pLab = dynamic_cast<PAUILABEL>(GetDlgItem(strName)); 
		strName.Format("Btn_Done%d",i+1);
		PAUISTILLIMAGEBUTTON pButton = dynamic_cast<PAUISTILLIMAGEBUTTON>(GetDlgItem(strName));
		if (pLab && pButton)
		{
			m_vecLblTaskName.push_back(pLab);
			m_vecBtnTaskFnsh.push_back(pButton);
		}
		else break;
	}
	return true;
}


void CDlgTaskConfirm::OnCommandFinishTask(const char *szCommand)
{
	int nSlot = atoi(szCommand + strlen("Btn_Done")) - 1;
	if (nSlot < 0 || nSlot >= GetMaxShowTask()) return;
	
	CECTaskInterface *pTask = GetHostPlayer()->GetTaskInterface();
	ATaskTemplMan *pMan = GetGame()->GetTaskTemplateMan();
	if (pTask == NULL) return;
	const CECTaskInterface::MAP_CONFIRM_TASK& TasksMap = pTask->GetConfirmTasksMap();
	int task_id = m_vecLblTaskName[nSlot]->GetData();
	if (task_id>0 && pTask->IsTaskReadyToConfirm(task_id))
	{
		ATaskTempl *pTemp = pMan->GetTaskTemplByID(task_id);
		if (pTemp && pTemp->m_Award_S->m_ulCandItems > 1) {
			CDlgAward* pAward = dynamic_cast<CDlgAward*>(m_pAUIManager->GetDialog("Win_Award"));
			if (pAward) {
				pAward->UpdateAwardItem(task_id, false);
				pAward->Show(true);
			}
		} else pTask->ConfirmFinishTask(task_id);
	}
	if (TasksMap.empty())
		Show(false);
}

void CDlgTaskConfirm::OnCommandMinimize(const char *szCommand)
{
	CECGameUIMan* pMan = GetGameUIMan();
	if (pMan)
		pMan->AddNewTaskConfirm();
	Show(false);
}

void CDlgTaskConfirm::UpdateTaskList()
{
	int index = 0;
	for (index=0;index<GetMaxShowTask();index++)
	{
		m_vecLblTaskName[index]->SetText(_AL(""));
		m_vecLblTaskName[index]->SetHint(_AL(""));
		m_vecLblTaskName[index]->SetData(0);
		m_vecBtnTaskFnsh[index]->Show(false);
		m_vecBtnTaskFnsh[index]->Show(false);
	}

	CECTaskInterface::MAP_CONFIRM_TASK::const_iterator iter;
	CECTaskInterface *pTask = GetHostPlayer()->GetTaskInterface();
	if (pTask == NULL)
		return;
	
	const CECTaskInterface::MAP_CONFIRM_TASK& TasksMap = pTask->GetConfirmTasksMap();
	index = 0;
	for (iter = TasksMap.begin(); iter != TasksMap.end(); ++iter)
	{
		if (index >= GetMaxShowTask()) break;
		if (iter->second)
		{
			ATaskTempl* pTempl = GetTaskTemplMan()->GetTaskTemplByID(iter->first);
			if (pTempl)
			{
				m_vecLblTaskName[index]->Show(true);
				m_vecBtnTaskFnsh[index]->Show(true);
//				m_vecLblTaskName[index]->SetText(pTempl->GetName());
				m_vecLblTaskName[index]->SetText(CDlgTask::GetTaskNameWithColor(pTempl));
				m_vecLblTaskName[index]->SetHint(pTempl->GetDescription());
				m_vecLblTaskName[index]->SetData(pTempl->GetID());
				
				index++;
			}
		}
	}

	if (index>0)
	{
		index--;
		POINT pt = m_vecLblTaskName[index]->GetPos(true);
		SIZE se = m_vecLblTaskName[index]->GetSize();

		int nHeight = pt.y + m_pAUIFrameDlg->GetSizeLimit().cy / 2;
		nHeight += se.cy;

		SetSize(GetSize().cx,nHeight);
	}
	if (TasksMap.empty())
	{
		Show(false);
		PAUIDIALOG pDlg = GetGameUIMan()->GetDialog("Win_Questq");
		if (pDlg)
			pDlg->Show(false);
	}
}
void CDlgTaskConfirm::Resize(A3DRECT rcOld, A3DRECT rcNew)
{
	CDlgBase::Resize(rcOld, rcNew);
	UpdateTaskList();
}
void CDlgTaskConfirm::OnChangeLayoutEnd(bool bAllDone)
{
	if(bAllDone)
	{
		UpdateTaskList();
	}
}
//////////////////////////////////////////////////////////////////////////
//
//
//
//////////////////////////////////////////////////////////////////////////
AUI_BEGIN_COMMAND_MAP(CDlgTaskHintPop, CDlgBase)

AUI_ON_COMMAND("IDCANCEL",	OnCommand_CANCEL)
AUI_ON_COMMAND("Btn_ShowTasksToConfirm", OnCommand_PopTaskList)

AUI_END_COMMAND_MAP()

void CDlgTaskHintPop::OnCommand_CANCEL(const char * szCommand)
{
	Show(false);
}
void CDlgTaskHintPop::OnCommand_PopTaskList(const char * szCommand)
{
	if(!GetGameUIMan()->m_pDlgTaskConfirm->IsShow())
	{
		GetGameUIMan()->m_pDlgTaskConfirm->Show(true);
		m_pFlashButton->SetFlash(false);
	}
	Show(false);
}
void CDlgTaskHintPop::OnNewConfirmTask()
{
	if (!IsShow())
	{
		Show(true);
	}
}
void CDlgTaskHintPop::OnShowDialog()
{
	m_pFlashButton->SetFlash(true);
	CECGameUIMan* pMan = GetGameUIMan();
	if (pMan)
	{
		PAUIDIALOG pDlg = pMan->GetDialog("Win_QuestMinion");
		if (pDlg) AlignTo(pDlg, AUIDIALOG_ALIGN_OUTSIDE, AUIDIALOG_ALIGN_LEFT, AUIDIALOG_ALIGN_OUTSIDE, AUIDIALOG_ALIGN_TOP);
	}
	CDlgBase::OnShowDialog();
}
bool CDlgTaskHintPop::OnInitDialog()
{
	m_pFlashButton = dynamic_cast<PAUISTILLIMAGEBUTTON>(GetDlgItem("Btn_ShowTasksToConfirm"));
	return true;
}