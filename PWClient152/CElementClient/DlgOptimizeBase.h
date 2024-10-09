// Filename	: DlgOptimizeBase.h
// Creator	: Xu Wenbin
// Date		: 2013/4/17

#pragma once

#include "DlgBase.h"

class AUIObject;
class CDlgOptimizeBase : public CDlgBase
{
	AUI_DECLARE_COMMAND_MAP();
protected:
    AUIObject * m_pTxt_Memory;
    AUIObject * m_pTxt_FPS;

	virtual bool OnInitDialog();
    virtual void OnTick();

    void OnCommand_Switch(const char *szCommand);
	void OnCommand_OptimizeOS(const char *szCommand);

    void UpdateSysView();

public:
	CDlgOptimizeBase();
	virtual ~CDlgOptimizeBase();
};

