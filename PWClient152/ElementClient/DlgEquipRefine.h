// File		: DlgEquipRefine.h
// Creator	: Xiao Zhou
// Date		: 2005/3/29

#pragma once

#include "DlgQuickBuyBase.h"
#include "AUIEditBox.h"
#include "AUIImagePicture.h"
#include "AUIStillImageButton.h"
#include "AUITextArea.h"

class CECIvtrItem;

class CDlgEquipRefine : public CDlgQuickBuyBase  
{
	AUI_DECLARE_COMMAND_MAP()
	AUI_DECLARE_EVENT_MAP()

public:
	CDlgEquipRefine();
	virtual ~CDlgEquipRefine();

	void OnCommandConfirm(const char * szCommand);
	void OnCommandCancel(const char * szCommand);
	void OnCommandShop(const char * szCommand);

	void OnEventLButtonDown_Item(WPARAM wParam, LPARAM lParam, AUIObject * pObj);
	void OnEventLButtonDown_Ticket(WPARAM wParam, LPARAM lParam, AUIObject * pObj);

	void SetEquip(CECIvtrItem *pItem, int nSlot);
	void SetTicket(CECIvtrItem *pItem, int nSlot);
	void RefineResult(int result);

	virtual void OnItemDragDrop(CECIvtrItem* pIvtrSrc, int iSrc, PAUIOBJECT pObjSrc, PAUIOBJECT pObjOver);

protected:
	virtual bool OnInitDialog();
	virtual void OnShowDialog();
	virtual void OnTick();

	//	¼Ì³Ð×Ô¸¸Àà
	virtual CECShopBase * GetShopData();
	virtual CECShopSearchPolicyBase * GetShopSearchPolicy(int itemID);
	virtual void OnItemAppearInPack(int itemID, int iSlot);

	void UpdateEquipDesc();
	CECIvtrItem * GetRefineTicket();
	
	PAUIIMAGEPICTURE		m_pImg_Item;
	PAUIOBJECT				m_pTxt_Name;
	PAUIOBJECT				m_pTxt_Effect;
	PAUIOBJECT				m_pTxt_Gold;
	PAUIIMAGEPICTURE		m_pImg_Ticket;
	PAUIOBJECT				m_pTxt_Ticket;
	PAUITEXTAREA			m_pTxt_RefineList;
	PAUISTILLIMAGEBUTTON	m_pBtn_Confirm;
	int						m_nSolt;
	int						m_nSoltTicket;
	int						m_nLastSolt;
	int						m_nLastSoltTicket;
	int						m_dwEnalbeTick;
};
