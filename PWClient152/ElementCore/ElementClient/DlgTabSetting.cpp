// Filename	: DlgTabSetting.cpp
// Creator	: Wang Yongdong
// Date		: 2012/03/12

#include "AFI.h"
#include "AUIImagePicture.h"
#include "EC_Game.h"
#include "EC_Global.h"
#include "EC_Configs.h"
#include "DlgTabSetting.h"
#include "EC_GameRun.h"
#include "EC_GameUIMan.h"
#include "EC_HostPlayer.h"


#define new A_DEBUG_NEW

AUI_BEGIN_COMMAND_MAP(CDlgTabSetting, CDlgBase)

AUI_ON_COMMAND("selnormal",			OnCommandSelType)
AUI_ON_COMMAND("noselmonster",		OnCommandSelType)
AUI_ON_COMMAND("noselplayer",		OnCommandSelType)

AUI_END_COMMAND_MAP()


CDlgTabSetting::CDlgTabSetting()
{
}

CDlgTabSetting::~CDlgTabSetting()
{
}
void CDlgTabSetting::OnShowDialog()
{
	CDlgBase::OnShowDialog();

	const EC_VIDEO_SETTING& vs = g_pGame->GetConfigs()->GetVideoSettings();

	CheckRadioButton(0, vs.cTabSelType);
}
void CDlgTabSetting::OnCommandSelType(const char *szCommand)
{
	int nMode(0);

	if( 0 == stricmp(szCommand, "selnormal") )
		nMode = CECHostPlayer::TSL_NORMAL;
	else if(0 == stricmp(szCommand, "noselmonster"))
		nMode = CECHostPlayer::TSL_MONSTER;
	else if(0 == stricmp(szCommand, "noselplayer"))
		nMode = CECHostPlayer::TSL_PLAYER;
	else 
	{
		ASSERT(FALSE);
		return;
	}

	EC_VIDEO_SETTING vs = g_pGame->GetConfigs()->GetVideoSettings();
	if (vs.cTabSelType == nMode) return;

	vs.cTabSelType = nMode;
	g_pGame->GetConfigs()->SetVideoSettings(vs);
	

	CECHostPlayer *pHost = GetHostPlayer();
	if(pHost)
		pHost->ResetAutoSelMap();
}