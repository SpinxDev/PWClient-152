// Filename	: DlgInventoryDetail.h
// Creator	: Feng Ning
// Date		: 2011/10/26

#pragma once

#include "DlgBase.h"
#include "AUIImagePicture.h"
#include "AUISubDialog.h"

class CECInventory;
namespace S2C
{
	struct cmd_player_inventory_detail;
}

class CDlgInventoryDetail : public CDlgBase  
{
	AUI_DECLARE_COMMAND_MAP()

public:
	CDlgInventoryDetail();
	virtual ~CDlgInventoryDetail();

	virtual void OnInventoryDetail(S2C::cmd_player_inventory_detail* pDetail);
	virtual void OnChangeLayoutEnd(bool bAllDone);

protected:
	virtual void OnTick();
	virtual void OnShowDialog();
	virtual bool OnInitDialog();
	virtual bool Release();

	void RefreshResult();

private:
	CECInventory* m_pIvtr;

	PAUISUBDIALOG m_pSubItems;
	typedef abase::vector<PAUIIMAGEPICTURE> ItemIcons;
	ItemIcons m_pIcons;
};
