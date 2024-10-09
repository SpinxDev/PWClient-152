// Filename	: DlgAuctionSellList.h
// Creator	: Xiao Zhou
// Date		: 2005/11/10


#pragma once

#include "DlgAuctionBase.h"
#include "AUICheckBox.h"
#include "AUIListBox.h"
#include "AUILabel.h"
#include "AUIEditBox.h"
#include "AUIRadioButton.h"
#include "AUIImagePicture.h"
#include "AUIStillImageButton.h"

class CECIvtrItem;

class CDlgAuctionSellList : public CDlgAuctionBase  
{
	AUI_DECLARE_COMMAND_MAP();
	AUI_DECLARE_EVENT_MAP();

public:
	CDlgAuctionSellList();
	virtual ~CDlgAuctionSellList();

	virtual void OnCommandCancel(const char* szCommand);
	virtual void OnCommandAuctionBuyList(const char* szCommand);
	virtual void OnCommandAuctionList(const char* szCommand);
	virtual void OnCommandAuctionSellList(const char* szCommand);
	virtual void OnCommandAuctionMyFavor(const char* szCommand);
	void OnCommandSell(const char* szCommand);
	void OnCommandCloseAuction(const char* szCommand);

	void OnEventLButtonDown_Img_Item(WPARAM wParam, LPARAM lParam, AUIObject * pObj);

	void SellRe(void *pData);
	void CloseRe(void *pData);
	void SetSellItem(CECIvtrItem *pItem, int nItemPos);

	virtual void OnItemDragDrop(CECIvtrItem* pIvtrSrc, int iSrc, PAUIOBJECT pObjSrc, PAUIOBJECT pObjOver);

protected:
	virtual bool OnInitDialog();
	virtual void OnShowDialog();
	virtual void OnTick();

	PAUIEDITBOX				m_pTxt_PackGold;
	PAUIOBJECT				m_pTxt_Name;
	PAUIEDITBOX				m_pTxt_SellPrice;
	PAUIEDITBOX				m_pTxt_BinPrice;
	PAUIRADIOBUTTON			m_pRdo_8h;
	PAUIRADIOBUTTON			m_pRdo_16h;
	PAUIRADIOBUTTON			m_pRdo_24h;
	PAUILABEL				m_pTxt_Poundage;
	PAUIIMAGEPICTURE		m_pImg_Item;
	PAUICHECKBOX			m_pChk_SavePrice;
	int						m_nItemPos;
	int						m_idItem;
	int						m_nIDMax;
	int						m_nSellID;
	CECIvtrItem *			m_pItem;
};

