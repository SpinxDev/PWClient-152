// File		: DlgLoginOther.h
// Creator	: Xu Wenbin
// Date		: 2012/5/14

#pragma once

#include "DlgExplorer.h"

class CECLoginUIMan;

class CDlgLoginOther : public CDlgExplorer  
{
	AUI_DECLARE_COMMAND_MAP()

public:
	CDlgLoginOther();
	virtual ~CDlgLoginOther();

	virtual void DocumentComplete(IDispatch* pDisp,VARIANT* URL);

protected:
	virtual void OnShowDialog();
	virtual void OnHideDialog();

	CECLoginUIMan* GetLoginUIMan();

	bool m_bLoginSwitch;
};
