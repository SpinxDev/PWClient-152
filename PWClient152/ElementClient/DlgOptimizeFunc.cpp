// Filename	: DlgOptimizeFunc.cpp
// Creator	: zhangyitian
// Date		: 2013/9/27

#include "DlgOptimizeFunc.h"
#include "EC_UIHelper.h"
#include "EC_GameUIMan.h"
#include "DlgIceThunderBall.h"
#include <AUICheckBox.h>


AUI_BEGIN_COMMAND_MAP(CDlgOptimizeFunc, CDlgOptimizeBase)
AUI_ON_COMMAND("Btn_Decide", OnCommand_Confirm)
AUI_ON_COMMAND("Btn_FunSelectAll", OnCommand_SelectAll)
AUI_ON_COMMAND("Btn_FunClear", OnCommand_Clear)
AUI_END_COMMAND_MAP()

CDlgOptimizeFunc::CDlgOptimizeFunc() {
	m_pChkEquipCompare = NULL;
	m_pChkLowHPGfx = NULL;
	m_pChkPetBuff = NULL;
	m_pChkLockQBar = NULL;
	m_pChkHideIceThunderBall = NULL;
	m_pChkHideTargetOfTarget = NULL;
}

bool CDlgOptimizeFunc::OnInitDialog() {
	DDX_Control("Chk_LowHPGfx", m_pChkLowHPGfx);
	DDX_Control("Chk_EquipCompare", m_pChkEquipCompare);
	DDX_Control("Chk_PetBuff", m_pChkPetBuff);
	DDX_Control("Chk_LockQBar", m_pChkLockQBar);
	DDX_Control("Chk_Ball", m_pChkHideIceThunderBall);
	DDX_Control("Chk_HideTargetOfTarget", m_pChkHideTargetOfTarget);
	return CDlgOptimizeBase::OnInitDialog();
}

void CDlgOptimizeFunc::OnShowDialog() {
	m_setting = GetGame()->GetConfigs()->GetGameSettings();
	UpdateFunctionView();
}

void CDlgOptimizeFunc::UpdateFunctionView() {
	CECGameUIMan* pGameUIMan = CECUIHelper::GetGameUIMan();
	m_pChkLowHPGfx->Check(!pGameUIMan->IsShowLowHP());
	m_pChkEquipCompare->Check(!pGameUIMan->IsShowItemDescCompare());
	m_pChkHideTargetOfTarget->Check(!pGameUIMan->IsShowTargetOfTarget());

	m_pChkPetBuff->Check(m_setting.bPetAutoSkill);
	m_pChkLockQBar->Check(m_setting.bLockQuickBar);
	m_pChkHideIceThunderBall->Check(m_setting.bHideIceThunderBall);
}

void CDlgOptimizeFunc::OnCommand_Confirm(const char *szCommand) {
	CECGameUIMan* pGameUIMan = CECUIHelper::GetGameUIMan();
	pGameUIMan->SetShowItemDescCompare(!m_pChkEquipCompare->IsChecked());
	pGameUIMan->SetShowLowHP(!m_pChkLowHPGfx->IsChecked());
	pGameUIMan->SetShowTargetOfTarget(!m_pChkHideTargetOfTarget->IsChecked());

	m_setting.bPetAutoSkill = m_pChkPetBuff->IsChecked();
	m_setting.bLockQuickBar = m_pChkLockQBar->IsChecked();
	m_setting.bHideIceThunderBall = m_pChkHideIceThunderBall->IsChecked();
	GetGame()->GetConfigs()->SetGameSettings_OptimizeFunction(m_setting);

	CDlgIceThunderBall *pDlgIceThunderBall = static_cast<CDlgIceThunderBall *>(pGameUIMan->GetDialog("Win_IceThunderBall"));
	if( m_setting.bHideIceThunderBall ){
		pDlgIceThunderBall->Show(false);
	}else{
		if( pDlgIceThunderBall->GetCurrentState() != 0 ){
			pDlgIceThunderBall->Show(true);
			pDlgIceThunderBall->ShowCurrentBall(pDlgIceThunderBall->GetCurrentState());
		}
	}
	Show(false);
}

void CDlgOptimizeFunc::OnCommand_SelectAll(const char *szCommand) {
	m_pChkLowHPGfx->Check(true);
	m_pChkEquipCompare->Check(true);
	m_pChkLockQBar->Check(true);
	m_pChkPetBuff->Check(true);
	m_pChkHideIceThunderBall->Check(true);
	m_pChkHideTargetOfTarget->Check(true);
}

void CDlgOptimizeFunc::OnCommand_Clear(const char *szCommand) {
	m_pChkLowHPGfx->Check(false);
	m_pChkEquipCompare->Check(false);
	m_pChkLockQBar->Check(false);
	m_pChkPetBuff->Check(false);
	m_pChkHideIceThunderBall->Check(false);
	m_pChkHideTargetOfTarget->Check(false);
}

void CDlgOptimizeFunc::SetLockQBar(bool bLock) {
	m_pChkLockQBar->Check(bLock);
}

