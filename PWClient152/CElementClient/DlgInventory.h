// Filename	: DlgInventory.h
// Creator	: Tom Zhou
// Date		: October 10, 2005

#pragma once

#include "DlgBase.h"
#include "EC_IvtrTypes.h"
#include "AUILabel.h"
#include "AUIImagePicture.h"
#include "AUIStillImageButton.h"
#include "AUIScroll.h"
#include "AUICheckBox.h"

#define CECDLGSHOP_PACKMAX 32
#define CECDLGSHOP_PACKLINEMAX 8

class CECIvtrItem;
class CDlgInventory : public CDlgBase  
{
	AUI_DECLARE_EVENT_MAP();
	AUI_DECLARE_COMMAND_MAP()

public:
	CDlgInventory();
	virtual ~CDlgInventory();
	virtual bool Tick(void);

	void OnCommand_choosemoney(const char * szCommand);
	void OnCommand_normalitem(const char * szCommand);
	void OnCommand_questitem(const char * szCommand);
	void OnCommand_normalmode(const char * szCommand);
	void OnCommand_wearmode(const char * szCommand);
	void OnCommand_Fashion(const char * szCommand);
	void OnCommand_Try(const char * szCommand);
	void OnCommand_CANCEL(const char * szCommand);
	void OnCommand_FashionStore(const char * szCommand);
	void OnCommand_arrange(const char *szCommand);
	void OnCommand_meridian(const char *szCommand);
	void OnCommand_FashionShop(const char *szCommand);
	void OnCommand_GeneralCard(const char* szCommand);
	void OnCommand_Booth(const char* szCommand);

	void OnEventLButtonDown(WPARAM wParam, LPARAM lParam, AUIObject *pObj);
	void OnEventLButtonDblclkItem(WPARAM wParam, LPARAM lParam, AUIObject *pObj);
	void OnEventLButtonDblclkEquip(WPARAM wParam, LPARAM lParam, AUIObject *pObj);
	void OnEventMouseWheel(WPARAM wParam, LPARAM lParam, AUIObject *pObj);

	int GetShowItem();
	void SetShowItem(int nShowItem);

	enum
	{
		INVENTORY_ITEM_NORMAL = 0,
		INVENTORY_ITEM_TASK
	};
	
	bool ValidateEquipmentPassword();
	bool ValidateEquipmentTakeOff(int equipmentPos);
	bool ValidateEquipmentExchange(int iSrc, CECIvtrItem *pItem, int exchangePos);
	bool ValidateEquipmentExchange(int iSrc, CECIvtrItem *pItem);
	bool ValidateEquipmentShortcut(CECIvtrItem *pItem);

	enum NormalItemShowMode
	{
		NORMAL_ITEM_SHOW_MODE_INTEGRATED,  // All normal item is shown in inventory
		NORMAL_ITEM_SHOW_MODE_SEPERATED    // Normal item in expanded space shown in bag
	};
	NormalItemShowMode GetNormalItemShowMode();

	void DropItem(CECIvtrItem* pIvtrSrc, int iSrc, PAUIOBJECT pObjSrc, PAUIOBJECT pObjOver);
	void DropEquip(CECIvtrItem* pIvtrSrc, int iSrc, PAUIOBJECT pObjSrc, PAUIOBJECT pObjOver);
	void ExchangeItem(CECIvtrItem* pIvtrSrc, int iSrc, PAUIOBJECT pObjSrc, PAUIOBJECT pObjOver);
	void ExchangeEquip(CECIvtrItem* pIvtrSrc, int iSrc, PAUIOBJECT pObjSrc, PAUIOBJECT pObjOver);
	void ExchangeTradeItem(CECIvtrItem* pIvtrSrc, int iSrc, PAUIOBJECT pObjSrc, PAUIOBJECT pObjOver);

	virtual void OnItemDragDrop(CECIvtrItem* pIvtrSrc, int iSrc, PAUIOBJECT pObjSrc, PAUIOBJECT pObjOver);

	void SetBagModeSplit();

protected:
	int m_nShowIvtrItem;
	int	m_nTotalItem;

	PAUIOBJECT m_pTxt_Gold;
	PAUISTILLIMAGEBUTTON m_pBtn_NormalItem;
	PAUISTILLIMAGEBUTTON m_pBtn_QuestItem;
	PAUISTILLIMAGEBUTTON m_pBtn_NormalMode;
	PAUISTILLIMAGEBUTTON m_pBtn_WearMode;
	PAUICHECKBOX m_pChkWearMode;
	PAUICHECKBOX m_pChkBag;
	PAUISCROLL m_pScl_Item;
	PAUIIMAGEPICTURE m_pImgEquip[SIZE_EQUIPIVTR];
	PAUIIMAGEPICTURE m_pImgItem[CECDLGSHOP_PACKMAX];
	PAUIOBJECT			m_pBtn_Meridians;
	PAUIOBJECT			m_pBtn_FashionShop;
	PAUILABEL			m_pLblLing;
	PAUILABEL			m_pLblMai;

	bool UpdateInventory();
	virtual bool OnInitDialog();
	virtual void OnShowDialog();
	virtual bool Release();
	virtual bool Render();
	
	// for notify user the layout changing is over
	virtual bool OnChangeLayoutBegin();
	virtual void OnChangeLayoutEnd(bool bAllDone);

	CECIvtrItem* GetSelectItem(PAUIOBJECT pObj);

private:
	bool ValidateEquipmentPositionChange(int equipmentPos);
	bool IsFashionSlot(int iSlot);
};
