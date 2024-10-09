// Filename	: DlgOptimizeGfx.h
// Creator	: Xu Wenbin
// Date		: 2013/4/17

#pragma once

#include "DlgOptimizeBase.h"
#include "EC_Optimize.h"

class AUICheckBox;
class CDlgOptimizeGfx : public CDlgOptimizeBase
{
	AUI_DECLARE_COMMAND_MAP();
    
    AUICheckBox * m_pChk_ExcludeHost;
	
    AUICheckBox * m_pChk_PlayerCastGfx;
    AUICheckBox * m_pChk_NPCCastGfx;
	
    AUICheckBox * m_pChk_PlayerAttackGfx;
    AUICheckBox * m_pChk_NPCAttackGfx;

    AUICheckBox * m_pChk_PlayerFlyGfx;
    AUICheckBox * m_pChk_NPCFlyGfx;
    
    AUICheckBox * m_pChk_PlayerHitGfx;
    AUICheckBox * m_pChk_NPCHitGfx;
    
    AUICheckBox * m_pChk_PlayerStateGfx;
    AUICheckBox * m_pChk_NPCStateGfx;

    AUICheckBox * m_pChk_WeaponStoneGfx;
    AUICheckBox * m_pChk_ArmorStoneGfx;
	AUICheckBox * m_pChk_SuiteGfx;

	CECOptimize::GFX m_gfx;
        
protected:
	virtual bool OnInitDialog();
	virtual void OnShowDialog();
    
    void OnCommand_Confirm(const char *szCommand);
	void OnCommand_Suggest(const char *szCommand);
    void OnCommand_SelectAll(const char *szCommand);
    void OnCommand_Clear(const char *szCommand);

    void UpdateGFXView();

public:
	CDlgOptimizeGfx();
	virtual ~CDlgOptimizeGfx();
};

