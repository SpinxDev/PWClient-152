// Filename	: DlgTitleChallenge.cpp
// Creator	: Han Guanghui
// Date		: 2013/5/16

#include "DlgTitleChallenge.h"
#include "DlgTask.h"
#include "EC_Game.h"
#include "EC_GameRun.h"
#include "EC_GameSession.h"
#include "EC_Global.h"
#include "EC_GameUIMan.h"
#include "EC_HostPlayer.h"
#include "AUIImagePicture.h"
#include "AUIStillImageButton.h"
#include "elementdataman.h"
#include "TaskTemplMan.h"
#include "EC_TaskInterface.h"
#include "AUIDef.h"
#include "DlgMiniMap.h"


AUI_BEGIN_COMMAND_MAP(CDlgTitleChallenge, CDlgBase)

AUI_ON_COMMAND("Btn_Select*",		OnCommandChallenge)
AUI_ON_COMMAND("Btn_Up",	OnCommandPagePrevious)
AUI_ON_COMMAND("Btn_Down",	OnCommandPageNext)

AUI_END_COMMAND_MAP()

AUI_BEGIN_EVENT_MAP(CDlgTitleChallenge, CDlgBase)
AUI_ON_EVENT("*", WM_MOUSEWHEEL, OnMouseWheel)
AUI_ON_EVENT(NULL,WM_MOUSEWHEEL, OnMouseWheel)
AUI_END_EVENT_MAP()


CDlgTitleChallenge::CDlgTitleChallenge():
m_iFirstTaskIndex(0),
m_bAddInfo(false)
{

}

CDlgTitleChallenge::~CDlgTitleChallenge()
{

}

bool CDlgTitleChallenge::OnInitDialog()
{
	m_Challenges.clear();
	m_ConfirmChallenges.clear();
	return true;
}

void CDlgTitleChallenge::OnShowDialog()
{
	Update();
	m_bAddInfo = false;
}

void CDlgTitleChallenge::OnCommandChallenge(const char* szCommand)
{
	PAUIOBJECT pBtn = GetDlgItem(szCommand);
	if (pBtn == NULL) return; 
	pBtn->Enable(false);

	int challenge_index = 0;
	sscanf(szCommand, "Btn_Select%d", &challenge_index);
	challenge_index--;

	CECTaskInterface *pTask = GetHostPlayer()->GetTaskInterface();
	ActiveTaskList* pLst = (ActiveTaskList*)pTask->GetActiveTaskList();
	int index = m_iFirstTaskIndex + challenge_index;
	if (index >= 0 && index < (int)m_Challenges.size()) {	
		task_notify_base notify;
		notify.task = m_Challenges[index];
		notify.reason = pLst->GetEntry(notify.task) ? TASK_CLT_NOTIFY_CHECK_GIVEUP : TASK_CLT_NOTIFY_TITLE_TASK;
		pTask->NotifyServer(&notify, sizeof(notify));
		m_ConfirmChallenges.push_back(m_Challenges[index]);
	}
	
}

void CDlgTitleChallenge::OnCommandPagePrevious(const char* szCommand)
{
	if (m_iFirstTaskIndex > 0) {
		m_iFirstTaskIndex--;
		Update();
	}
}

void CDlgTitleChallenge::OnCommandPageNext(const char* szCommand)
{
	if (m_iFirstTaskIndex< GetChallengeCount()-1)
	{
		m_iFirstTaskIndex++;
		Update();
	}
}

void CDlgTitleChallenge::AddChallenge(unsigned short id)
{
	CECTaskInterface *pTask = GetHostPlayer()->GetTaskInterface();
	ActiveTaskList* pLst = (ActiveTaskList*)pTask->GetActiveTaskList();
	TASK_ID::iterator iter = std::find(m_Challenges.begin(), m_Challenges.end(), id);
	CECGameUIMan* pUIMan = GetGameUIMan();
	if (pUIMan && iter == m_Challenges.end()) {
		if (!IsShow() && !m_bAddInfo && pLst->GetEntry(id) == NULL) {
			m_bAddInfo = true;
			CDlgMiniMap* pMap = (CDlgMiniMap*)pUIMan->GetDialog("Win_Map");
			if (pMap) pMap->OnAddTitleChallenge();
		}
		m_Challenges.push_back(id);
		Update();
	}
}
void CDlgTitleChallenge::RemoveChallenge(unsigned short id)
{
	TASK_ID::iterator iter = std::find(m_Challenges.begin(), m_Challenges.end(), id);
	if (iter != m_Challenges.end()){
		TASK_ID temp;
		for (unsigned int i = 0; i < m_Challenges.size(); ++i) {
			if (id != m_Challenges[i]) temp.push_back(m_Challenges[i]);
		}
		m_Challenges.swap(temp);
		Update();
	}
}
void CDlgTitleChallenge::Update()
{
	int i(1);
	int sum_num = (int)m_Challenges.size();
	char szTemp[50];
	PAUIOBJECT pName(NULL), pText(NULL), pDoing(NULL), 
		pBtn(NULL), pUp(NULL), pDown(NULL);

	ATaskTempl* pTempl;
	int index(0);
	if (m_iFirstTaskIndex >= sum_num) m_iFirstTaskIndex = 0;
	CECTaskInterface *pTask = GetHostPlayer()->GetTaskInterface();
	ActiveTaskList* pLst = (ActiveTaskList*)pTask->GetActiveTaskList();
	while (true) {
		index = m_iFirstTaskIndex + i - 1;
		sprintf(szTemp, "Lbl_EventName%d", i);
		pName = GetDlgItem(szTemp);
		if (!pName) break;
		sprintf(szTemp, "Lbl_Des%d", i);
		pText = GetDlgItem(szTemp);
		if (!pText) break;
		sprintf(szTemp, "Btn_Select%d", i);
		pBtn = GetDlgItem(szTemp);
		if (!pBtn) break;
		sprintf(szTemp, "Lbl_Doing%d", i);
		pDoing = GetDlgItem(szTemp);
		if (!pDoing) break;
		if (index < 0 || index >= sum_num) {
			pName->Show(false);
			pText->Show(false);
			pBtn->Show(false);
			pDoing->Show(false);
		} else {
			pTempl = GetTaskTemplMan()->GetTopTaskByID(m_Challenges[index]);	
			if (pTempl) {
				pName->Show(true);
				pText->Show(true);
//				pName->SetText(pTempl->GetName());
				pName->SetText(CDlgTask::GetTaskNameWithColor(pTempl));
				pText->SetText(pTempl->GetDescription());
			}
			if (IsConfirmingChallenge(m_Challenges[index])){
				pBtn->Show(true);
				pDoing->Show(true);
				pBtn->SetText(GetStringFromTable(10601));
				pBtn->Enable(false);
				pDoing->SetText(GetStringFromTable(340));
			} else if (pLst->GetEntry(m_Challenges[index])) {
				pBtn->Show(true);
				pDoing->Show(true);
				pBtn->SetText(GetStringFromTable(10604));
				pBtn->Enable(true);
				pDoing->SetText(GetStringFromTable(10600));
			} else {
				pBtn->Show(true);
				pDoing->Show(false);
				pBtn->SetText(GetStringFromTable(10601));
				pBtn->Enable(true);
			}
		}
		++i;
	}
	pUp = GetDlgItem("Btn_Up");
	if (pUp) pUp->Show(m_iFirstTaskIndex != 0);
	pDown = GetDlgItem("Btn_Down");
	if (pDown) pDown->Show(m_iFirstTaskIndex + i - 1 < sum_num);
}

bool CDlgTitleChallenge::IsConfirmingChallenge(unsigned short id)
{
	TASK_ID::iterator iter = std::find(m_ConfirmChallenges.begin(), m_ConfirmChallenges.end(), id);
	return iter != m_ConfirmChallenges.end();
}

void CDlgTitleChallenge::OnReceiveChallenge(unsigned short id)
{
	TASK_ID temp;
	for (unsigned int i = 0; i < m_ConfirmChallenges.size(); ++i) {
		if (id != m_ConfirmChallenges[i]) temp.push_back(m_ConfirmChallenges[i]);
	}
	m_ConfirmChallenges.swap(temp);
	Update();
}

void CDlgTitleChallenge::OnChangeLayoutEnd(bool bAllDone)
{
	if(bAllDone)
		if(IsShow())
			Update();
}

void CDlgTitleChallenge::ChangeWorldInstance(int idInstance)
{
	m_Challenges.clear();
}
void CDlgTitleChallenge::OnMouseWheel(WPARAM wParam, LPARAM lParam, AUIObject *pObj)
{
	int zDelta = (short)HIWORD(wParam);
	if( zDelta > 0 ) OnCommandPagePrevious(NULL);
	else OnCommandPageNext(NULL);
}