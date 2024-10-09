// File		: DlgWebMyShop.h
// Creator	: Feng Ning
// Date		: 2010/3/18

#pragma once

#include "DlgWebTradeBase.h"
#include "AUIComboBox.h"
#include "AUIRadioButton.h"
#include "AUICheckBox.h"

class CDlgWebMyShop : public CDlgWebTradeBase
{
	AUI_DECLARE_COMMAND_MAP()
	AUI_DECLARE_EVENT_MAP()
		
public:
	CDlgWebMyShop();
	virtual ~CDlgWebMyShop();

	void OnCommandGotoViewProduct(const char* szCommand);
	void OnCommandPrepostItem(const char* szCommand);
	void OnCommandPrepostMoney(const char* szCommand);
	void OnCommandPrepostRole(const char* szCommand);
	void OnCommandConfirmDeal(const char* szCommand);
	void OnCommandCancelDeal(const char* szCommand);
	void OnCommandCancelPrepost(const char* szCommand);
	void OnCommandSpecificBuyer(const char* szCommand);
	void OnCommandCheckSell(const char* szCommand);
	void OnCommandCancel(const char* szCommand);

	virtual void OnMessageBox(AUIDialog* pDlg, int iRetVal);
	virtual void WebTradeAction(int idAction, void *pData);
	
	void OnEventLButtonDown_Item_Prepost(WPARAM wParam, LPARAM lParam, AUIObject * pObj);
	void SetPrepostItem(CECIvtrItem *pItem, int nItemPos);

	void NotifyInvalidName(const ACString& strName, bool bObsoleteName);

	virtual void OnItemDragDrop(CECIvtrItem* pIvtrSrc, int iSrc, PAUIOBJECT pObjSrc, PAUIOBJECT pObjOver);

private:
	int m_PageBegin, m_PageEnd;

	PAUIOBJECT				m_pTxt_Currency;
	PAUIOBJECT				m_pTxt_Price1;
	PAUICHECKBOX			m_pChk_Sell;
	PAUICOMBOBOX			m_pCombo_Period;
	PAUIOBJECT				m_pTxt_BuyerName;
	
	struct ItemSelected
	{
		PAUIIMAGEPICTURE		pImg;
		PAUIOBJECT				pName;
		int						ID;
		int						Pos;
		CECIvtrItem *			pData;

		ItemSelected();
		void Reset();
		void Enable(bool bEnable);
		void Bind(CECIvtrItem * pItem, int nItemPos);

	} m_ItemSelected;

protected:
	enum {MODE_ITEM, MODE_MONEY, MODE_ROLE};
	int						m_Mode;

	struct PrepostInfo
	{
		// BuyerRoleID will be set to these flag values
		enum {NOBUYER = 0, NOTGET = -1, GETTING = -2};

		int Mode;
		int Price;
		int SellPeriod;
		ACString BuyerName;
		int BuyerRoleID;

		int Confirm;
		
		struct _Item
		{
			int ID;
			int Slot;
			int Count;
		};
		
		struct _Money
		{
			int Count;
		};

		struct _Role
		{
			int ID;
		};
		
		union 
		{
			_Item Item;
			_Money Money;
			_Role Role;
		};
	};

	typedef AList2<PrepostInfo> InfoList;
	InfoList m_Infos;

	virtual void OnTick();
	virtual bool OnInitDialog();
	virtual void OnShowDialog();

	virtual void OnPageSwitch(int mode);
	virtual ACString GetRowString(const CDlgWebTradeBase::ItemInfo& tradeItem);
	virtual void InsertTradeInfo(int i, ItemInfo* pInfo);

	bool ValidatePrepost(PrepostInfo& info);
	void CheckPrepostList();
	void ClearSelectData();
	void ShowObjectsForMoney(bool bShow);
	void ShowObjectsForItem(bool bShow);
	void ShowObjectsForRole(bool bShow);

	// use these functions to lock controls and record current item
	void EnableObjectsForSell(bool bEnable);
	
	virtual int GetMinSellPrice()const{ return 1000; }
};