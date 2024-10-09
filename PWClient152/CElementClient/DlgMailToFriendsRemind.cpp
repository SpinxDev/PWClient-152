#include "DlgMailToFriendsRemind.h"
#include "DlgMailToFriends.h"
#include "DlgWebList.h"
#include "EC_UIManager.h"
#include "EC_GameUIMan.h"
#include "EC_Configs.h"
#include "EC_Global.h"
#include "EC_Game.h"
#include "EC_GameRun.h"
#include "EC_UIConfigs.h"
#include "EC_CrossServer.h"
#include "EC_HostPlayer.h"
#include "EC_UIHelper.h"
#include "EC_Time.h"

#include "AUICheckBox.h"

#define new A_DEBUG_NEW

AUI_BEGIN_COMMAND_MAP(CDlgOnlineRemind, CDlgBase)
AUI_ON_COMMAND("confirm", OnCommandConfirm)
AUI_ON_COMMAND("IDCANCEL",OnCommandCANCAL)
AUI_ON_COMMAND("Btn_PageDown",OnCommandPageNext)
AUI_ON_COMMAND("Btn_PageUp", OnCommandPagePrevious)
AUI_END_COMMAND_MAP()

AUI_BEGIN_EVENT_MAP(CDlgOnlineRemind, CDlgBase)

AUI_END_EVENT_MAP()

static const int ONE_WEEK_SECONDS = 7 * 24 * 3600;

ACString IOnlineReminder::GetString(int index)
{
	ACString ret(_AL(""));
	CECGameUIMan* pMan = g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan();
	if (NULL != pMan) ret = pMan->GetStringFromTable(index);
	return ret;
}

bool CMailtoFriendReminder::CanShow()
{
	bool ret(false);
	while (true)
	{
		// 首先检查千里传情开关
		if (!CECUIConfig::Instance().GetGameUI().bMailToFriendsSwitch ||
			CECCrossServer::Instance().IsOnSpecialServer())
			break;
		
		EC_VIDEO_SETTING vs = g_pGame->GetConfigs()->GetVideoSettings();
		int iTimeInterval = g_pGame->GetServerAbsTime() - vs.iMailToFriendsCheckTime;
		
		CECGameUIMan* pMan = g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan();
		if (NULL == pMan) break;	
		
		CDlgMailToFriends* pDlg = dynamic_cast<CDlgMailToFriends*>(pMan->GetDialog("Win_MailtoFriend"));
		if (NULL == pDlg) break;
		// 检查"一周内不再提示"设置
		if (pDlg->NeedAttrackAttention() && (vs.bMailToFriendsRemind || iTimeInterval > ONE_WEEK_SECONDS))
			ret = true;
		break;
	}
	return ret;
}

ACString CMailtoFriendReminder::GetTitle()
{
	return GetString(9950);
}

ACString CMailtoFriendReminder::GetContent()
{
	return GetString(9960);
}

ACString CMailtoFriendReminder::GetButtonText()
{
	return GetString(9970);
}

void CMailtoFriendReminder::OnCommandConfirm()
{
	CECGameUIMan* pMan = g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan();
	if (NULL != pMan)
	{
		PAUIDIALOG pDlg = pMan->GetDialog("Win_MailtoFriend");
		if (pDlg) pDlg->Show(true);
	}	
}

bool CActivityReminder::CanShow()
{
	bool ret(false);
	while (true)
	{
		// 首先检查活动开关
		if (!CECUIConfig::Instance().GetGameUI().bActivityReminder ||
			CECCrossServer::Instance().IsOnSpecialServer()){
			break;
		}
		// 检查"一周内不再提示"设置
		EC_VIDEO_SETTING vs = g_pGame->GetConfigs()->GetVideoSettings();
		int iTimeInterval = g_pGame->GetServerAbsTime() - vs.ibActivityReminderCheckTime;
		if (!vs.bActivityRemind && iTimeInterval <= ONE_WEEK_SECONDS){
			break;
		}
		//	检查等级限制等
		CECHostPlayer *pHost = CECUIHelper::GetHostPlayer();
		if (pHost->GetBasicProps().iLevel < CECUIConfig::Instance().GetGameUI().nActivityReminderLevel ||
			pHost->GetMaxLevelSofar() < CECUIConfig::Instance().GetGameUI().nActivityReminderMaxLevelSoFar ||
			pHost->GetBasicProps().iLevel2 < CECUIConfig::Instance().GetGameUI().nActivityReminderLevel2){
			break;
		}
		//	检查转生
		if (pHost->GetReincarnationCount() < CECUIConfig::Instance().GetGameUI().nActivityReminderReincarnationTimes ||
			pHost->GetRealmLevel() < CECUIConfig::Instance().GetGameUI().nActivityReminderRealmLevel){
			break;
		}
		//	检查声望
		if (pHost->GetReputation() < CECUIConfig::Instance().GetGameUI().nActivityReminderReputation){
			break;
		}
		//	检查显示时间
		struct tm serverTime = g_pGame->GetServerLocalTime(g_pGame->GetServerAbsTime());
		const struct tm &tBegin = CECUIConfig::Instance().GetGameUI().tActivityReminderStartTime;
		const struct tm &tEnd = CECUIConfig::Instance().GetGameUI().tActivityReminderEndTime;
		if (!CECTime::IsHappenOnWithZeroRange(serverTime, tBegin, tEnd)){
			break;
		}
		ret = true;
		break;
	}
	return ret;
}
ACString CActivityReminder::GetTitle()
{
	return GetString(9951);
}
ACString CActivityReminder::GetContent()
{
	return GetString(9961);
}
ACString CActivityReminder::GetButtonText()
{
	return GetString(9971);
}
void CActivityReminder::OnCommandConfirm()
{
	CECGameUIMan* pMan = g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan();
	if (NULL != pMan)
	{
		CDlgWebList* pDlg = dynamic_cast<CDlgWebList*>(pMan->GetDialog("Win_WebList"));
		if (pDlg) pDlg->OnCommand_Btn_Activity(NULL);
	}
}

CDlgOnlineRemind::CDlgOnlineRemind():
m_iIndex(0)
,m_pTitle(NULL)
,m_pContent(NULL)
,m_pButton(NULL)
,m_pNext(NULL)
,m_pPrevious(NULL)
{
}

CDlgOnlineRemind::~CDlgOnlineRemind()
{
	size_t i;
	for ( i = 0; i < m_Reminders.size(); ++i)
		delete m_Reminders[i];
	m_Reminders.clear();
}

void CDlgOnlineRemind::DoDataExchange(bool bSave)
{
	CDlgBase::DoDataExchange(bSave);
	
}

void CDlgOnlineRemind::OnCommandConfirm(const char *szCommand)
{
	m_Reminders[m_iIndex]->OnCommandConfirm();
}
void CDlgOnlineRemind::OnCommandPageNext(const char *szCommand)
{
	Select(m_iIndex + 1);
}
void CDlgOnlineRemind::OnCommandPagePrevious(const char *szCommand)
{
	Select(m_iIndex - 1);
}
bool CDlgOnlineRemind::OnInitDialog()
{
	DDX_Control("txt_mailtips1", m_pTitle);
	DDX_Control("txt_mailtips2", m_pContent);
	DDX_Control("Btn_mailtofriend", m_pButton);
	DDX_Control("Btn_PageDown", m_pNext);
	DDX_Control("Btn_PageUp", m_pPrevious);
	return true;
}
void CDlgOnlineRemind::OnShowDialog()
{
	Select(0);
}

void CDlgOnlineRemind::Select(int index)
{
	a_Clamp(index, 0, a_Max(int(m_Reminders.size() - 1), 0));
	m_iIndex = index;
	// 设置标题、内容和按钮文字
	m_pTitle->SetText(m_Reminders[index]->GetTitle());
	m_pContent->SetText(m_Reminders[index]->GetContent());
	m_pButton->SetText(m_Reminders[index]->GetButtonText());
	// 上一页和下一页是否显示
	m_pPrevious->Show(index != 0);
	m_pNext->Show(index != (int)m_Reminders.size() - 1);
	// 换页时取消checkbox的勾选
	PAUICHECKBOX pCheck = (PAUICHECKBOX)GetDlgItem("Chk_noremind");
	if (pCheck) pCheck->Check(false);
}

IOnlineReminder* CDlgOnlineRemind::CreateReminder(AString str)
{
	IOnlineReminder* ret = NULL;

	if (str == "MailToFriendsSwitch") ret = new CMailtoFriendReminder;
	else if (str == "ActivityReminder") ret = new CActivityReminder;
	// 是CanShow状态才加入页面
	if (ret->CanShow()) m_Reminders.push_back(ret);
	else 
	{	
		delete ret;
		ret = NULL;
	}
	return ret;
}

void CDlgOnlineRemind::OnCommandCANCAL(const char *szCommand)
{
	EC_VIDEO_SETTING vs = GetGame()->GetConfigs()->GetVideoSettings();
	PAUICHECKBOX pCheck = (PAUICHECKBOX)GetDlgItem("Chk_noremind");
	if (dynamic_cast<CActivityReminder*>(m_Reminders[m_iIndex]))
	{
		if (pCheck && pCheck->IsChecked())
		{
			vs.bActivityRemind = false;
			vs.ibActivityReminderCheckTime = g_pGame->GetServerAbsTime();
		}
		else vs.bActivityRemind = true;		
	}
	else if (dynamic_cast<CMailtoFriendReminder*>(m_Reminders[m_iIndex]))
	{
		if (pCheck && pCheck->IsChecked())
		{
			vs.bMailToFriendsRemind = false;
			vs.iMailToFriendsCheckTime = g_pGame->GetServerAbsTime();
		}
		else vs.bMailToFriendsRemind = true;
	}

	GetGame()->GetConfigs()->SetVideoSettings(vs);

	Show(false);
}

bool CDlgOnlineRemind::CanShow()
{
	if (m_Reminders.empty())
	{
		// 创建顺序等于页面出现顺序
		CreateReminder("MailToFriendsSwitch");
		CreateReminder("ActivityReminder");
	}

	bool ret(false);
	if (!m_Reminders.empty()) ret = true;
	
	return ret;
}

