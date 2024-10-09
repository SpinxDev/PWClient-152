/********************************************************************
	created:	2005/08/15
	created:	15:8:2005   18:00
	file name:	DlgRevive.cpp
	author:		yaojun
	
	purpose:	
*********************************************************************/

#include "AUIObject.h"
#include "DlgRevive.h"
#include "EC_GameSession.h"
#include "EC_HostPlayer.h"
#include "EC_Inventory.h"
#include "EC_IvtrItem.h"

#define new A_DEBUG_NEW

AUI_BEGIN_COMMAND_MAP(CDlgRevive, CDlgBase)

AUI_ON_COMMAND("confirm", OnCommandConfirm)
AUI_ON_COMMAND("back", OnCommandBack)
AUI_ON_COMMAND("accept", OnCommandAccept)
AUI_ON_COMMAND("IDCANCEL", OnCommandCancel)

AUI_END_COMMAND_MAP()

CDlgRevive::CDlgRevive(): m_bHasScroll(false)
{
}

CDlgRevive::~CDlgRevive()
{
}

bool CDlgRevive::OnInitDialog()
{
	m_pTxt_D = (PAUILABEL)GetDlgItem("Txt_D");
	m_pBtn_Back = (PAUISTILLIMAGEBUTTON)GetDlgItem("Btn_Back");
	m_pBtn_Decide = (PAUISTILLIMAGEBUTTON)GetDlgItem("Btn_Decide");
	m_pBtn_Accept = (PAUISTILLIMAGEBUTTON)GetDlgItem("Btn_Accept");

	return true;
}

void CDlgRevive::OnCommandConfirm(const char *szCommand)
{
	GetGameSession()->c2s_CmdReviveVillage();
}

void CDlgRevive::OnCommandBack(const char *szCommand)
{
	//	Revive with soul stone, check cool time at first
	CECHostPlayer* pHost = GetHostPlayer();
	if (pHost && !pHost->GetCoolTime(GP_CT_SOUL_STONE))
		GetGameSession()->c2s_CmdReviveItem();
}

void CDlgRevive::OnCommandAccept(const char *szCommand)
{
	GetGameSession()->c2s_CmdRevivalAgree();
}

void CDlgRevive::OnCommandCancel(const char *szCommand)
{
}

void CDlgRevive::OnShowDialog()
{
	CECHostPlayer* pHost = GetHostPlayer();
	if (pHost)
	{
		m_bHasScroll = pHost->GetPack()->GetItemTotalNum(3043) > 0 || pHost->GetPack()->GetItemTotalNum(32021) > 0;
		
		m_pBtn_Decide->Enable(true);
		
		if( pHost->GetBasicProps().iLevel < 10 )
			m_pTxt_D->SetText(GetStringFromTable(274));
		else
			m_pTxt_D->SetText(GetStringFromTable(a_Random(283, 292)));
	}
}

void CDlgRevive::OnTick()
{
	CECHostPlayer* pHost = GetHostPlayer();
	if(pHost)
	{
		m_pBtn_Accept->Enable(pHost->GetReviveLostExp() >= 0.0f ? true : false);		
		m_pBtn_Back->Enable(m_bHasScroll && pHost->GetCoolTime(GP_CT_SOUL_STONE) == 0);
	}	
}