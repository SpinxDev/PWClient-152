// Filename	: DlgOptimizeGfx.cpp
// Creator	: Xu Wenbin
// Date		: 2013/4/17

#include "DlgOptimizeGfx.h"
#include "EC_Global.h"
#include "EC_Game.h"
#include "EC_GameRun.h"
#include "EC_Configs.h"

#include <AUICheckBox.h>

#define new A_DEBUG_NEW

AUI_BEGIN_COMMAND_MAP(CDlgOptimizeGfx, CDlgOptimizeBase)
AUI_ON_COMMAND("Btn_Decide", OnCommand_Confirm)
AUI_ON_COMMAND("Btn_GfxSuggest", OnCommand_Suggest)
AUI_ON_COMMAND("Btn_GfxSelectAll", OnCommand_SelectAll)
AUI_ON_COMMAND("Btn_GfxClear", OnCommand_Clear)
AUI_END_COMMAND_MAP()


CDlgOptimizeGfx::CDlgOptimizeGfx()
:m_pChk_ExcludeHost(NULL)
,m_pChk_PlayerCastGfx(NULL)
,m_pChk_NPCCastGfx(NULL)
,m_pChk_PlayerAttackGfx(NULL)
,m_pChk_NPCAttackGfx(NULL)
,m_pChk_PlayerFlyGfx(NULL)
,m_pChk_NPCFlyGfx(NULL)
,m_pChk_PlayerHitGfx(NULL)
,m_pChk_NPCHitGfx(NULL)
,m_pChk_PlayerStateGfx(NULL)
,m_pChk_NPCStateGfx(NULL)
,m_pChk_WeaponStoneGfx(NULL)
,m_pChk_ArmorStoneGfx(NULL)
,m_pChk_SuiteGfx(NULL)
{
}

CDlgOptimizeGfx::~CDlgOptimizeGfx()
{
}

bool CDlgOptimizeGfx::OnInitDialog()
{
	DDX_Control("Chk_ExcludeHost", m_pChk_ExcludeHost);

	DDX_Control("Chk_PlayerCastGfx", m_pChk_PlayerCastGfx);
    DDX_Control("Chk_NPCCastGfx", m_pChk_NPCCastGfx);

	DDX_Control("Chk_PlayerAttackGfx", m_pChk_PlayerAttackGfx);
    DDX_Control("Chk_NPCAttackGfx", m_pChk_NPCAttackGfx);

    DDX_Control("Chk_PlayerFlyGfx", m_pChk_PlayerFlyGfx);
    DDX_Control("Chk_NPCFlyGfx", m_pChk_NPCFlyGfx);
    
    DDX_Control("Chk_PlayerHitGfx", m_pChk_PlayerHitGfx);
    DDX_Control("Chk_NPCHitGfx", m_pChk_NPCHitGfx);
    
    DDX_Control("Chk_PlayerStateGfx", m_pChk_PlayerStateGfx);
    DDX_Control("Chk_NPCStateGfx", m_pChk_NPCStateGfx);

	DDX_Control("Chk_WeaponStoneGfx", m_pChk_WeaponStoneGfx);
	DDX_Control("Chk_ArmorStoneGfx", m_pChk_ArmorStoneGfx);
	DDX_Control("Chk_SuiteGfx", m_pChk_SuiteGfx);

	return CDlgOptimizeBase::OnInitDialog();
}

void CDlgOptimizeGfx::OnShowDialog()
{
	m_gfx = CECOptimize::Instance().GetGFX();
	UpdateGFXView();
}

void CDlgOptimizeGfx::UpdateGFXView()
{
	m_pChk_ExcludeHost->Check(m_gfx.bExcludeHost);
	
    m_pChk_PlayerCastGfx->Check(m_gfx.bHidePlayerCast);
    m_pChk_NPCCastGfx->Check(m_gfx.bHideNPCCast);
    m_pChk_PlayerAttackGfx->Check(m_gfx.bHidePlayerAttack);
    m_pChk_NPCAttackGfx->Check(m_gfx.bHideNPCAttack);

    m_pChk_PlayerFlyGfx->Check(m_gfx.bHidePlayerFly);
    m_pChk_NPCFlyGfx->Check(m_gfx.bHideNPCFly);
    m_pChk_PlayerHitGfx->Check(m_gfx.bHidePlayerHit);
    m_pChk_NPCHitGfx->Check(m_gfx.bHideNPCHit);
    m_pChk_PlayerStateGfx->Check(m_gfx.bHidePlayerState);
    m_pChk_NPCStateGfx->Check(m_gfx.bHideNPCState);

	m_pChk_WeaponStoneGfx->Check(m_gfx.bHideWeaponStone);
	m_pChk_ArmorStoneGfx->Check(m_gfx.bHideArmorStone);
	m_pChk_SuiteGfx->Check(m_gfx.bHideSuite);
}

void CDlgOptimizeGfx::OnCommand_Confirm(const char *szCommand)
{
	m_gfx.bExcludeHost = m_pChk_ExcludeHost->IsChecked();
	
    m_gfx.bHidePlayerCast = m_pChk_PlayerCastGfx->IsChecked();
    m_gfx.bHideNPCCast = m_pChk_NPCCastGfx->IsChecked();
    m_gfx.bHidePlayerAttack = m_pChk_PlayerAttackGfx->IsChecked();
    m_gfx.bHideNPCAttack = m_pChk_NPCAttackGfx->IsChecked();
	
    m_gfx.bHidePlayerFly = m_pChk_PlayerFlyGfx->IsChecked();
    m_gfx.bHideNPCFly = m_pChk_NPCFlyGfx->IsChecked();
    m_gfx.bHidePlayerHit = m_pChk_PlayerHitGfx->IsChecked();
    m_gfx.bHideNPCHit = m_pChk_NPCHitGfx->IsChecked();
    m_gfx.bHidePlayerState = m_pChk_PlayerStateGfx->IsChecked();
    m_gfx.bHideNPCState = m_pChk_NPCStateGfx->IsChecked();
	
	m_gfx.bHideWeaponStone = m_pChk_WeaponStoneGfx->IsChecked();
	m_gfx.bHideArmorStone = m_pChk_ArmorStoneGfx->IsChecked();
	m_gfx.bHideSuite = m_pChk_SuiteGfx->IsChecked();
	
    CECOptimize::Instance().SetGFX(m_gfx);
	g_pGame->GetConfigs()->SetGfxOptimize(m_gfx);
    Show(false);
}

void CDlgOptimizeGfx::OnCommand_Suggest(const char *szCommand)
{
	m_gfx.bExcludeHost = true;
	
    m_gfx.bHidePlayerCast = true;
    m_gfx.bHideNPCCast = true;
    m_gfx.bHidePlayerAttack = true;
    m_gfx.bHideNPCAttack = true;
	
    m_gfx.bHidePlayerFly = false;
    m_gfx.bHideNPCFly = false;
    m_gfx.bHidePlayerHit = false;
    m_gfx.bHideNPCHit = false;
    m_gfx.bHidePlayerState = true;
    m_gfx.bHideNPCState = true;
	
	m_gfx.bHideWeaponStone = true;
	m_gfx.bHideArmorStone = true;
	m_gfx.bHideSuite = true;

	UpdateGFXView();
}

void CDlgOptimizeGfx::OnCommand_SelectAll(const char *szCommand)
{
	m_gfx.bExcludeHost = true;
	
    m_gfx.bHidePlayerCast = true;
    m_gfx.bHideNPCCast = true;
    m_gfx.bHidePlayerAttack = true;
    m_gfx.bHideNPCAttack = true;
	
    m_gfx.bHidePlayerFly = true;
    m_gfx.bHideNPCFly = true;
    m_gfx.bHidePlayerHit = true;
    m_gfx.bHideNPCHit = true;
    m_gfx.bHidePlayerState = true;
    m_gfx.bHideNPCState = true;
	
	m_gfx.bHideWeaponStone = true;
	m_gfx.bHideArmorStone = true;
	m_gfx.bHideSuite = true;

	UpdateGFXView();
}

void CDlgOptimizeGfx::OnCommand_Clear(const char *szCommand)
{
	m_gfx.bExcludeHost = false;
	
    m_gfx.bHidePlayerCast = false;
    m_gfx.bHideNPCCast = false;
    m_gfx.bHidePlayerAttack = false;
    m_gfx.bHideNPCAttack = false;
	
    m_gfx.bHidePlayerFly = false;
    m_gfx.bHideNPCFly = false;
    m_gfx.bHidePlayerHit = false;
    m_gfx.bHideNPCHit = false;
    m_gfx.bHidePlayerState = false;
    m_gfx.bHideNPCState = false;
	
	m_gfx.bHideWeaponStone = false;
	m_gfx.bHideArmorStone = false;
	m_gfx.bHideSuite = false;

	UpdateGFXView();
}
