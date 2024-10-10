// File		: DlgWebRoleTrade.h
// Creator	: Feng Ning
// Date		: 2010/12/30

#pragma once

#include "DlgWebMyShop.h"

namespace GNET
{
	class RoleInfo;
}

class CECLoginUIMan;
class CDlgWebRoleTrade : public CDlgWebMyShop  
{
	AUI_DECLARE_COMMAND_MAP()

public:
	CDlgWebRoleTrade();
	virtual ~CDlgWebRoleTrade();

	void OnCommandCancel(const char * szCommand);

	virtual void OnMessageBox(AUIDialog* pDlg, int iRetVal);
	virtual void WebTradeAction(int idAction, void *pData);

	void SetRoleInfo(const GNET::RoleInfo& info);

protected:
	virtual void OnShowDialog();
	virtual bool OnInitDialog();

	CECLoginUIMan* GetLoginUIMan();
	
	virtual int GetMinSellPrice()const{ return 6000; }
};
