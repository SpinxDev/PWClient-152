// Filename	: DlgBuddyState.cpp
// Creator	: Xiao Zhou
// Date		: 2005/12/13

#include "AUIImagePicture.h"
#include "DlgBuddyState.h"
#include "EC_GameRun.h"
#include "EC_GameUIMan.h"
#include "EC_GameSession.h"
#include "EC_HostPlayer.h"
#include "EC_ManPlayer.h"
#include "EC_World.h"

#define new A_DEBUG_NEW

AUI_BEGIN_COMMAND_MAP(CDlgBuddyState, CDlgBase)

AUI_ON_COMMAND("bindcancel",	OnCommand_BindCancel)
AUI_ON_COMMAND("kiss",			OnCommand_Kiss)
AUI_ON_COMMAND("IDCANCEL",		OnCommand_Cancel)

AUI_END_COMMAND_MAP()

CDlgBuddyState::CDlgBuddyState()
{
	m_pImg_Prof = NULL;
	m_pTxt_Name = NULL;
	m_pBtn_Kiss	= NULL;
}

CDlgBuddyState::~CDlgBuddyState()
{
}

void CDlgBuddyState::OnCommand_Cancel(const char * szCommand)
{
	GetGameUIMan()->RespawnMessage();
}

void CDlgBuddyState::OnCommand_BindCancel(const char * szCommand)
{
	GetGameSession()->c2s_CmdCancelBindPlayer();
}

void CDlgBuddyState::OnCommand_Kiss(const char * szCommand)
{
	GetGameSession()->c2s_CmdEmoteAction(ROLEEXP_TWO_KISS);
}

bool CDlgBuddyState::OnInitDialog()
{
	DDX_Control("Img_Prof", m_pImg_Prof);
	DDX_Control("Txt_Name", m_pTxt_Name);
	DDX_Control("Btn_Kiss", m_pBtn_Kiss);

	return true;
}

void CDlgBuddyState::OnTick()
{
	if( GetHostPlayer()->GetBuddyState() == 0 )
		Show(false);

	bool bMoving = false;
	if( GetHostPlayer()->IsHangerOn() )
	{
		CECPlayer *pPlayer = GetWorld()->GetPlayerMan()->GetPlayer(GetHostPlayer()->GetBuddyID());
		if( pPlayer )
			bMoving = pPlayer->IsPlayerMoving();
	}
	else
		bMoving = GetHostPlayer()->IsPlayerMoving();

	if( bMoving || GetHostPlayer()->GetCoolTime(GP_CT_EMOTE, NULL) > 0 )
		m_pBtn_Kiss->Enable(false);
	else
		m_pBtn_Kiss->Enable(true);
}

void CDlgBuddyState::SetBuddyID(int idPlayer)
{
	CECPlayer *pPlayer = GetWorld()->GetPlayerMan()->GetPlayer(idPlayer);
	if( pPlayer )
	{
		m_pImg_Prof->SetHint(GetGameRun()->GetProfName(pPlayer->GetProfession()));
		m_pImg_Prof->FixFrame(pPlayer->GetProfession() + pPlayer->GetGender() * NUM_PROFESSION);
		m_pTxt_Name->SetText(pPlayer->GetName());
	}
	else
	{
		m_pTxt_Name->SetText(_AL(""));
		m_pImg_Prof->FixFrame(0);
	}
	Show(true);
}