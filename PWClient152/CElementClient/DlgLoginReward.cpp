#include "DlgLoginReward.h"
#include "EC_TaskInterface.h"
#include "EC_Global.h"
#include "EC_Game.h"
#include "EC_GameRun.h"
#include "EC_HostPlayer.h"
#include "TaskTempl.h"

#define new A_DEBUG_NEW

AUI_BEGIN_COMMAND_MAP(CDlgLoginReward, CDlgBase)


AUI_ON_COMMAND("btn_getreward", OnCommandReward)

AUI_END_COMMAND_MAP()

AUI_BEGIN_EVENT_MAP(CDlgLoginReward, CDlgBase)

AUI_END_EVENT_MAP()



CDlgLoginReward::CDlgLoginReward()
{
}

CDlgLoginReward::~CDlgLoginReward()
{
}


void CDlgLoginReward::OnCommandReward(const char *szCommand)
{
	task_notify_base notify;
	notify.reason = TASK_CLT_NOTIFY_15DAYS_NOLOGIN;
	notify.task = 0;

	g_pGame->GetGameRun()->GetHostPlayer()->GetTaskInterface()->NotifyServer(&notify, sizeof(notify));
}


