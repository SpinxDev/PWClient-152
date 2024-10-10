// Filename	: DlgOptimizeOS.h
// Creator	: Xu Wenbin
// Date		: 2014/11/10

#ifndef _ELEMENTCLIENT_DLGOPTIMIZEOS_H_
#define _ELEMENTCLIENT_DLGOPTIMIZEOS_H_

#include "DlgBase.h"

class CDlgOptimizeOS : public CDlgBase {

	AUI_DECLARE_COMMAND_MAP();

	PAUIOBJECT	m_pBtn_Enable;
	PAUIOBJECT	m_pBtn_Disable;

protected:
	virtual bool OnInitDialog();
    virtual void OnTick();

	bool CanEnable();
	bool CanDisable();

public:
	CDlgOptimizeOS();
    void OnCommand_Enable(const char *szCommand);
	void OnCommand_Disable(const char *szCommand);
};

#endif

