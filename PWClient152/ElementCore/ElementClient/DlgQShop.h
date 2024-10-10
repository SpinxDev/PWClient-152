// Filename	: DlgQShop.h
// Creator	: Xiao Zhou
// Date		: 2006/5/22

#pragma once

#include "DlgBase.h"

#include "AUIScroll.h"
#include "AUILabel.h"
#include "AUIImagePicture.h"
#include "AUIStillImageButton.h"
#include "AUIRadioButton.h"
#include "AUIListbox.h"
#include "globaldataman.h"
#include <vector>

#define CDLGQSHOP_BUTTONMAX 10

class CECShopBase;
class CDlgQShop : public CDlgBase
{
	AUI_DECLARE_COMMAND_MAP();
	AUI_DECLARE_EVENT_MAP();

	AUIObject * GetShopSwitchButton();

public:
	CDlgQShop();
	virtual ~CDlgQShop();

	void OnCommand_MainType(const char * szCommand);
	void OnCommand_SubType(const char * szCommand);
	void OnCommand_Payment(const char * szCommand);
	void OnCommand_CANCEL(const char * szCommand);
	void OnCommand_QShop(const char *szCommand);
	void OnCommand_BShop(const char *szCommand);
	void OnCommand_BackShop(const char *szCommand);
	void OnCommand_BackOthers(const char *szCommand);
	void OnCommand_BackSelf(const char *szCommand);
	void OnCommand_Fit(const char *szCommand);
	void OnCommand_Coupon(const char *szCommand);
	void OnCommand_UserAddCash(const char *szCommand);
	void OnCommand_QuickPay(const char *szCommand);
	void OnCommand_Search(const char *szCommand);
	void OnCommand_BuyCard(const char* szCommand);

	void OnEventMouseWheel(WPARAM wParam, LPARAM lParam, AUIObject *pObj);
	void OnEventKeyDown_Txt_Search(WPARAM wParam, LPARAM lParam, AUIObject *pObj);
	void OnEventDBClk_Lst_Search(WPARAM wParam, LPARAM lParam, AUIObject *pObj);

	void ShowMainType(int nMainType);
	void ShowSubType(int nSubType);

	void SetSelectItem(int nIndex, int nBuyIndex);
	ACString GetCashText(int nCash, bool bFullText = false);
	ACString GetFormatTime(int nIndex, int nBuyIndex);
	void UpdateQshopData();
	void OnUserCashAction(void *pProtocol);
	bool ShowCategory(int mainType, int subType, unsigned int ownerNPCID);
	bool ShowFashionShop(unsigned int ownerNPCID);
	bool IsFashionShopCategory(int mainType);

	void EnableShopSwitch(bool bEnable);
	bool IsShopSwitchEnabled();
	void ShowFalse(bool bIgnoreShopItemDialog=false);
	void ClearOwnerNPC();
	
	// Uniform interfaces for accessing qshop or backshop
	bool IsQShop()const;
	bool IsBackShop()const;
	int  GetCash();
	int  GetItemPrice(int nIndex, int nBuyIndex);

	typedef abase::vector<GSHOP_ITEM> * GShopItemsPtr;
	GShopItemsPtr GetShopItems();
	CECShopBase & GetShopData();
	const CECShopBase & GetShopDataConst()const;
	static bool GetFirstBuyStatus(const GSHOP_ITEM &item, unsigned int *status=NULL);
	static bool IsIgnored(const GSHOP_ITEM &item, int playerLevel);

	typedef std::vector<int>	QShopItemIndexArray;

	PAUIIMAGEPICTURE GetImageGfx();	
	bool ShouldIgnore(const GSHOP_ITEM &item)const;

protected:
	virtual void OnShowDialog();
	virtual bool OnInitDialog();
	virtual void OnTick();

	// for notify user the layout changing is over
	virtual void OnChangeLayoutEnd(bool bAllDone);

	bool ShouldIgnoreByCategory(const GSHOP_ITEM &item)const;
	bool ShouldIgnoreByNPCID(const GSHOP_ITEM &item)const;
	bool ShouldIgnoreWithPlayerLevel(const GSHOP_ITEM &item)const;

	void EnableEmptyMainType(bool bEnable);
	void EnableEmptySubType(bool bEnable);

	int  GetMainTypeCount();
	int	 GetSubTypeCount(int nMainType);
	bool IsMainTypeValid(int nMainType);
	bool IsSubTypeValid(int nMainType, int nSubType);
	bool IsMainTypeEmpty(int nMainType);
	bool IsSubTypeEmpty(int nMainType, int nSubType);

	int  SearchFirstNotEmptyMainType();
	int	 SearchFirstNotEmptySubType(int nMainType);

	int  AdjustMainTypeWithOwnerNPC(int nMainType);
	int  AdjustSubTypeWithOwnerNPC(int nMainType, int nSubType);	
	
	void EnableEmptyMainType();
	void EnableEmptySubType();

	bool CanFitCurrentItem();
	void UpdateView();
	void UpdateScrollPos();
	
	// 搜索商品
	ACString GetSearchPattern();
	bool SearchByPattern(const ACString &strPattern, QShopItemIndexArray &result, int maxSearchCount=-1);
	void UpdateSearchList();
	void ShowSearchItem(int itemIndex);

	PAUILABEL				m_pTxt_ItemName;
	PAUILABEL				m_pTxt_ItemDesc;
	PAUISCROLL				m_pScl_Item;
	PAUILABEL				m_pTxt_Cash;
	PAUILABEL				m_pTxt_CashMoneyRate;
	int						m_nStartLine;
	int						m_nMainType;
	int						m_nSubType;		//	子分类，-1表示主分类下所有子分类，-2表示临时搜索结果
	PAUIIMAGEPICTURE		m_pImg_Item;
	PAUISTILLIMAGEBUTTON	m_pBtn_MainType[CDLGQSHOP_BUTTONMAX];
	PAUISTILLIMAGEBUTTON	m_pBtn_SubType[CDLGQSHOP_BUTTONMAX];
	QShopItemIndexArray		m_vecIdItem;
	bool					m_bFirstOpen;
	PAUIRADIOBUTTON         m_pBtn_BackShop;
	PAUIRADIOBUTTON         m_pBtn_QShop;

	PAUILABEL               m_pTxt_Notify;
	bool                    m_bFirstFlash;

	PAUISTILLIMAGEBUTTON    m_pBtn_BackOthers;
	PAUISTILLIMAGEBUTTON    m_pBtn_BackSelf;

	PAUISTILLIMAGEBUTTON    m_pBtn_TrySkirt;
	
	PAUIIMAGEPICTURE        m_pImg_Bg01;
	PAUIIMAGEPICTURE        m_pImg_Bg02;
	PAUIIMAGEPICTURE        m_pImg_Bg03;
	PAUIIMAGEPICTURE        m_pImg_Bg04;
	
	PAUIIMAGEPICTURE		m_pImg_Highlight;
	PAUIIMAGEPICTURE		m_pImg_Normal;

	PAUIOBJECT				m_pTxt_Search;
	PAUILISTBOX				m_pLst_Search;
	ACString				m_strLastPattern;

	int						m_playerLevelOnShow;	//	对话框显示时的玩家等级，用于商品按玩家等级屏蔽

	PAUIIMAGEPICTURE        m_pImgGfx;
private:
	abase::vector<GSHOP_MAIN_TYPE> * GetShopMainTypes();
};
