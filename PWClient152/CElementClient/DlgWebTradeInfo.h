// File		: DlgWebTradeInfo.h
// Creator	: Feng Ning
// Date		: 2011/01/07

#pragma once

#include "DlgTheme.h"
#include "DlgWebTradeBase.h"

namespace GNET
{
	class WebTradeGetDetail_Re;
	class GWebTradeItem;
}

class CECLoginUIMan;
class CDlgWebTradeInfo : public CDlgTheme  
{
	AUI_DECLARE_COMMAND_MAP()

public:
	CDlgWebTradeInfo();
	virtual ~CDlgWebTradeInfo();

	void FetchRoleTradeInfo(int roleid);
	void SetTradeDetail(const GNET::WebTradeGetDetail_Re& data);
	
	// align the dialog position
	void RefreshPosition(int roleid);

	static void SetRoleCount(int num) { s_RoleCount = num; }
	static int GetRoleCount() { return s_RoleCount; }

protected:
	virtual void OnShowDialog();
	virtual bool OnInitDialog();
	virtual void OnTick();
	virtual bool Release();

private:
	GNET::GWebTradeItem* m_pTradeItem;
	CDlgWebTradeBase::ItemInfo *m_pItemInfo;

	// client must wait for server reply
	// before send the 2nd fetch protocol
	void ForceFetchRoleTradeInfo();
	bool m_Waiting;
	int	 m_RoleID; 
	
	static int s_RoleCount;
};
