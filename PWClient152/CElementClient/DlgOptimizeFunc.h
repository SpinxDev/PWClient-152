// Filename	: DlgOptimizeFunc.h
// Creator	: zhangyitian
// Date		: 2013/9/27

#ifndef _ELEMENTCLIENT_DLGOPTIMIZEFUNC_H_
#define _ELEMENTCLIENT_DLGOPTIMIZEFUNC_H_

#include "DlgOptimizeBase.h"
#include "EC_Configs.h"

class AUICheckBox;

class CDlgOptimizeFunc : public CDlgOptimizeBase {

	AUI_DECLARE_COMMAND_MAP();

protected:
	virtual bool OnInitDialog();
	virtual void OnShowDialog();
	
    void UpdateFunctionView();

private:
	AUICheckBox* m_pChkLowHPGfx;
	AUICheckBox* m_pChkEquipCompare;
	AUICheckBox* m_pChkPetBuff;
	AUICheckBox* m_pChkLockQBar;
	AUICheckBox* m_pChkHideIceThunderBall;  // 隐藏月仙冰雷球界面
	AUICheckBox* m_pChkHideTargetOfTarget;	// 关闭目标的目标

	EC_GAME_SETTING m_setting;

public:
	CDlgOptimizeFunc();
	
    void OnCommand_Confirm(const char *szCommand);
	void OnCommand_SelectAll(const char *szCommand);
    void OnCommand_Clear(const char *szCommand);

	void SetLockQBar(bool bLock);
};

#endif

