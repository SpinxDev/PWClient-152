// Filename	: DlgQShopItem.h
// Creator	: Xiao Zhou
// Date		: 2006/5/22

#pragma once

#include "DlgBase.h"
#include "AUILabel.h"
#include "AUIStillImageButton.h"
#include "AUIImagePicture.h"

class CECModel;
class CDlgQShop;
class CDlgQShopBuy;

class CDlgQShopItem : public CDlgBase
{
	AUI_DECLARE_COMMAND_MAP();
	AUI_DECLARE_EVENT_MAP();

public:
	CDlgQShopItem();
	virtual ~CDlgQShopItem();

	friend class CDlgQShopBuy;

	void OnCommand_Buy(const char * szCommand);
	void OnCommand_Time(const char * szCommand);
	void OnCommand_CANCEL(const char * szCommand);
	void OnCommand_GivePresent(const char *szCommand);
	void OnCommand_AskForPresent(const char *szCommand);
	void OnCommand_BatchBuy(const char *szCommand);

	void OnEventMouseWheel(WPARAM wParam, LPARAM lParam, AUIObject *pObj);
	void OnEventLButtonDown(WPARAM wParam, LPARAM lParam, AUIObject *pObj);

	void SetItem(int dlgpostion = -1, int itemindex = -1);
	int	GetItemIndex()	{ return m_nItemIndex; }

	static ACString  GetFormatTime(int nTime);
protected:
	virtual bool OnInitDialog();
	virtual void OnTick();
	virtual bool Render();

	bool	  CheckBuyedItem(); 
	bool      CanGivePresent();
	bool      CanAskForPresent();
	bool      CanGivingFor();

	PAUILABEL				m_pTxt_ItemName;
	PAUILABEL				m_pTxt_Time;
	PAUILABEL				m_pTxt_Price;
	PAUISTILLIMAGEBUTTON	m_pBtn_Buy;
	PAUISTILLIMAGEBUTTON	m_pBtn_Time[3];
	PAUIIMAGEPICTURE		m_pImg_Item;
	PAUIIMAGEPICTURE		m_pImg_New;
	PAUIIMAGEPICTURE		m_pImg_Hot;
	PAUIIMAGEPICTURE		m_pImg_Gift;
	PAUIIMAGEPICTURE        m_pImg_Bg01;
	PAUIIMAGEPICTURE        m_pImg_Bg02;
	PAUIIMAGEPICTURE        m_pImg_Bg03;
	PAUIIMAGEPICTURE        m_pImg_Bg04;
	PAUISTILLIMAGEBUTTON	m_pBtn_GivePresent;
	PAUISTILLIMAGEBUTTON	m_pBtn_AskForPresent;
	PAUISTILLIMAGEBUTTON	m_pBtn_BatchBuy;

	int			m_nItemIndex;
	int			m_nDlgPostion;
	int			m_nPrice;
	int			m_nTimeSelect;
	
	int m_TypeNew[4];
	int m_TypeDefault[4];
	int m_BuyType;

	CECModel*	m_pModel;


private:
	CDlgQShop * GetShop();

	int GetBuyIndex();
	void SetPriceText(int price, unsigned int status);
};
