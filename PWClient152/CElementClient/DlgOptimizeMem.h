// Filename	: DlgOptimizeMem.h
// Creator	: Xu Wenbin
// Date		: 2013/4/17

#pragma once

#include "DlgOptimizeBase.h"
#include "EC_MemSimplify.h"
#include <AUICheckBox.h>

class AUICheckBox;

class CDlgOptimizeMem : public CDlgOptimizeBase
{
	friend class CDlgSettingVideo;

	AUI_DECLARE_COMMAND_MAP();

	bool m_bAutoOptimize;	// 自动优化
	int m_iMemState;		// 手动设置优化级别

protected:
	virtual bool OnInitDialog();
	virtual void OnShowDialog();
	virtual void DoDataExchange(bool bSave);

    void UpdateMemView();
	
	PAUICHECKBOX m_pChk_Player;
	PAUICHECKBOX m_pChk_Surround;
	PAUICHECKBOX m_pChk_Sight;

public:
	CDlgOptimizeMem();
	virtual ~CDlgOptimizeMem();
	
    void OnCommand_Confirm(const char *szCommand);
	void OnCommand_OptimizeMem(const char* szCommand);
};

