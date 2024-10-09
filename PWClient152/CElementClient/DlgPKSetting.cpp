// Filename	: DlgPKSetting.cpp
// Creator	: Xiao Zhou
// Date		: 2005/11/18

#include "DlgPKSetting.h"
#include "DlgSetting.h"
#include "EC_Game.h"
#include "EC_Configs.h"
#include "EC_GameUIMan.h"
#include "EC_Utility.h"
#include "EC_GameSession.h"
#include "EC_HostPlayer.h"
#include "EC_GameRun.h"

#define new A_DEBUG_NEW

AUI_BEGIN_COMMAND_MAP(CDlgPKSetting, CDlgBase)

AUI_ON_COMMAND("confirm",		OnCommand_Confirm)
AUI_ON_COMMAND("Chk_SwitchPK",	OnCommand_pk)

AUI_END_COMMAND_MAP()

CDlgPKSetting::CDlgPKSetting()
{
	m_pChk_Aprotect = NULL;
	m_pChk_Gprotect = NULL;
	m_pChk_Wprotect = NULL;
	m_pChk_Allianceprotect = NULL;
	m_pChk_Fprotect = NULL;
	m_pChk_Buff1 = NULL;
	m_pChk_Buff2 = NULL;
	m_pChk_Buff3 = NULL;
	m_pChk_Buff4 = NULL;
	m_pChk_Buff5 = NULL;
	m_pChk_Buff6 = NULL;
	m_pChk_BlsNoForce = NULL;
	m_pChk_SwitchPK = NULL;
	m_pTxt_SwitchPK = NULL;
}

CDlgPKSetting::~CDlgPKSetting()
{
}

void CDlgPKSetting::OnCommand_Confirm(const char * szCommand)
{
	EC_GAME_SETTING gs = GetGame()->GetConfigs()->GetGameSettings();
	gs.bAtk_Player = !m_pChk_Aprotect->IsChecked();
	gs.bAtk_NoMafia = m_pChk_Gprotect->IsChecked();
	gs.bAtk_NoWhite = m_pChk_Wprotect->IsChecked();
	gs.bAtk_NoAlliance = m_pChk_Allianceprotect->IsChecked();
	gs.bAtk_NoForce = m_pChk_Fprotect->IsChecked();
	gs.bBls_NoRed = m_pChk_Buff1->IsChecked();
	gs.bBls_NoMafia = m_pChk_Buff2->IsChecked();
	gs.bBls_Self = m_pChk_Buff3->IsChecked();
	gs.bBlsRefuse_Neutral = m_pChk_Buff4->IsChecked();
	gs.bBls_NoAlliance = m_pChk_Buff5->IsChecked();
	gs.bBlsRefuse_NonTeammate = m_pChk_Buff6->IsChecked();
	gs.bBls_NoForce = m_pChk_BlsNoForce->IsChecked();

	GetGame()->GetConfigs()->SetGameSettings(gs);
	Show(false);
	GetGameUIMan()->m_pDlgSettingCurrent->UpdateView();

	BYTE forceAttack = glb_BuildPVPMask(false);
	BYTE refuseBless = glb_BuildRefuseBLSMask();
	GetGameSession()->c2s_CmdNotifyForceAttack(forceAttack, refuseBless);

	int nMode(0);
	switch (GetCheckedRadioButton(0))
	{
	case 0: nMode = CECHostPlayer::TSL_NORMAL; break;
	case 1: nMode = CECHostPlayer::TSL_MONSTER; break;
	case 2: nMode = CECHostPlayer::TSL_PLAYER; break;
	default: ASSERT(false); break;
	}	
	CECConfigs *pConfigs = GetGame()->GetConfigs();
	EC_VIDEO_SETTING vs = pConfigs->GetVideoSettings();
	if (vs.cTabSelType != nMode){
		vs.cTabSelType = nMode;
		pConfigs->SetVideoSettings(vs);
		GetHostPlayer()->ResetAutoSelMap();
	}
}

void CDlgPKSetting::OnCommand_pk(const char * szCommand)
{
	const CECPlayer::PVPINFO &pvp = GetHostPlayer()->GetPVPInfo();
	
	if( pvp.bEnable && pvp.dwCoolTime == 0 )
	{
		GetGameUIMan()->MessageBox("Game_PVPClose", GetStringFromTable(561),
			MB_OKCANCEL, A3DCOLORRGBA(255, 255, 255, 160));
	}
	else if( !pvp.bEnable )
	{
		if( GetHostPlayer()->GetBasicProps().iLevel < 30 )
		{
			GetGameUIMan()->MessageBox("", GetStringFromTable(581),
				MB_OK, A3DCOLORRGBA(255, 255, 255, 160));
		}
		else
		{
			GetGameUIMan()->MessageBox("Game_PVPOpen", GetStringFromTable(542),
				MB_OKCANCEL, A3DCOLORRGBA(255, 255, 255, 160));
		}
	}

	//	玩家点击后，选中状态被改变、并导致图标切换，但实际上不一定会切换，
	//	此处改回原图，如果切换成功，下次 OnShowDialog 时更新
	m_pChk_SwitchPK->Check(!m_pChk_SwitchPK->IsChecked());
}

bool CDlgPKSetting::OnInitDialog()
{
	DDX_Control("Chk_Aprotect", m_pChk_Aprotect);
	DDX_Control("Chk_Gprotect", m_pChk_Gprotect);
	DDX_Control("Chk_Wprotect", m_pChk_Wprotect);
	DDX_Control("Chk_Allianceprotect", m_pChk_Allianceprotect);
	DDX_Control("Chk_Other",m_pChk_Fprotect);

	DDX_Control("Chk_Buff1", m_pChk_Buff1);
	DDX_Control("Chk_Buff2", m_pChk_Buff2);
	DDX_Control("Chk_Buff3", m_pChk_Buff3);
	DDX_Control("Chk_Buff4", m_pChk_Buff4);
	DDX_Control("Chk_Buff5", m_pChk_Buff5);
	DDX_Control("Chk_Buff6", m_pChk_Buff6);
	DDX_Control("Chk_Buff7", m_pChk_BlsNoForce);

	DDX_Control("Chk_SwitchPK", m_pChk_SwitchPK);
	DDX_Control("Txt_SwitchPK", m_pTxt_SwitchPK);

	return true;
}

void CDlgPKSetting::OnShowDialog()
{
	const EC_GAME_SETTING& gs = GetGame()->GetConfigs()->GetGameSettings();

	m_pChk_Aprotect->Check(!gs.bAtk_Player);
	m_pChk_Gprotect->Check(gs.bAtk_NoMafia);
	m_pChk_Wprotect->Check(gs.bAtk_NoWhite);
	m_pChk_Allianceprotect->Check(gs.bAtk_NoAlliance);
	m_pChk_Fprotect->Check(gs.bAtk_NoForce);
	m_pChk_Buff1->Check(gs.bBls_NoRed);
	m_pChk_Buff2->Check(gs.bBls_NoMafia);
	m_pChk_Buff3->Check(gs.bBls_Self);
	m_pChk_Buff4->Check(gs.bBlsRefuse_Neutral);
	m_pChk_Buff5->Check(gs.bBls_NoAlliance);
	m_pChk_Buff6->Check(gs.bBlsRefuse_NonTeammate);
	m_pChk_BlsNoForce->Check(gs.bBls_NoForce);
	
	const EC_VIDEO_SETTING& vs = GetGame()->GetConfigs()->GetVideoSettings();	
	CheckRadioButton(0, vs.cTabSelType);	
	
	if( GetGameRun()->GetPVPMode() )
	{
		m_pChk_SwitchPK->Show(false);
		m_pTxt_SwitchPK->Show(false);
	}
	else
	{
		const CECPlayer::PVPINFO &pvp = GetHostPlayer()->GetPVPInfo();	
		m_pTxt_SwitchPK->Show(true);
		m_pChk_SwitchPK->Show(true);
		m_pChk_SwitchPK->Check(pvp.bEnable);
		
		if( pvp.bEnable )
		{			
			ACString strText;
			bool bRed = GetHostPlayer()->IsPariah() || GetHostPlayer()->IsInvader();
			if( bRed )
			{
				if( GetHostPlayer()->IsPariah() )
					strText.Format(GetStringFromTable(834), GetHostPlayer()->GetPariahTime() / 3600000 + 1);
				else
					strText = GetStringFromTable(615);
			}
			else if( pvp.dwCoolTime > 0 )
				strText.Format(GetStringFromTable(545), pvp.dwCoolTime / 3600000 + 1);
			else
				strText = GetStringFromTable(614);
			m_pChk_SwitchPK->SetHint(strText);
			m_pChk_SwitchPK->Enable(pvp.dwCoolTime > 0 || bRed ? false : true);
		}
		else
		{
			m_pChk_SwitchPK->Enable(true);
			m_pChk_SwitchPK->SetHint(GetStringFromTable(547));
		}
	}
}

void CDlgPKSetting::OnTick()
{
	m_pChk_Gprotect->Enable(!m_pChk_Aprotect->IsChecked());
	m_pChk_Wprotect->Enable(!m_pChk_Aprotect->IsChecked());
	m_pChk_Allianceprotect->Enable(!m_pChk_Aprotect->IsChecked());
	if( m_pChk_Aprotect->IsChecked() )
	{
		m_pChk_Gprotect->Check(true);
		m_pChk_Wprotect->Check(true);
		m_pChk_Allianceprotect->Check(true);
	}
}
void CDlgPKSetting::OnChangePVPNoPenalty(bool bEnter)
{
	m_pChk_Fprotect->Check(bEnter);
	m_pChk_BlsNoForce->Check(bEnter);

	EC_GAME_SETTING gs = GetGame()->GetConfigs()->GetGameSettings();
	gs.bAtk_NoForce = bEnter;
	gs.bBls_NoForce = bEnter;

	GetGame()->GetConfigs()->SetGameSettings(gs);
// 	Show(false);
 	GetGameUIMan()->m_pDlgSettingCurrent->UpdateView();
	
	BYTE forceAttack = glb_BuildPVPMask(false);
	BYTE refuseBless = glb_BuildRefuseBLSMask();
	GetGameSession()->c2s_CmdNotifyForceAttack(forceAttack, refuseBless);
}
