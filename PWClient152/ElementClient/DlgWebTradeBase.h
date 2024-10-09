// File		: DlgWebTradeBase.h
// Creator	: Feng Ning
// Date		: 2010/3/18

#pragma once

#include "DlgBase.h"
#include "hashmap.h"
#include "IOLib\\Gnoctets.h"
#include "AUIListBox.h"
#include "AUIStillImageButton.h"
#include "AUIImagePicture.h"
#include "AUIDialog.h"
#include "AUIEditBox.h"

#define WEBTRADE_PAGE_SIZE			16

namespace GNET
{
	class GWebTradeItem;
	class GRoleInventory;
	class Octets;
}
class CECIvtrItem;
class CDlgWebTradeBase : public CDlgBase  
{
	AUI_DECLARE_EVENT_MAP();
	AUI_DECLARE_COMMAND_MAP();
	
public:
	CDlgWebTradeBase();
	virtual ~CDlgWebTradeBase();
	
	void OnCommandPageUp(const char* szCommand);
	void OnCommandPageDown(const char* szCommand);
	void OnCommandPageHome(const char* szCommand);
	void OnCommandPageEnd(const char* szCommand);
	void OnCommandGotoOnlineShop(const char* szCommand);
	
	void OnEventRButtonUp_Item(WPARAM wParam, LPARAM lParam, AUIObject *pObj);

	virtual void OnMessageBox(AUIDialog* pDlg, int iRetVal){};
	virtual void WebTradeAction(int idAction, void *pData);
	virtual bool Release(void);

	// convert a string to double by specific precision
	// check the position of decimal point if precision >= 0
	bool ConvertToDouble(const ACHAR* szInput, int precision, int& iOut, int& iOutTime) const;

	void UpdateHint(int i);

	class ItemInfo;
private:
	int		m_PageMode;

	enum	{MAX_COLUMN = 8};
	bool	m_bSortFlag[MAX_COLUMN];

	typedef abase::hash_map<__int64, ACString> HintMap;
	static HintMap s_KnownHints;

protected:

	int		GetPageMode() const {return m_PageMode;}
	enum	{	PAGE_INVALID = -1,
				PAGE_REFRESH_FLAG = 0, 
				PAGE_DOWN_FLAG = 1, 
				PAGE_UP_FLAG = 2, 
				PAGE_HOME_FLAG = 3, 
				PAGE_END_FLAG = 4, };

	// called when page up/down was pressed
	virtual void OnPageSwitch(int mode);

	// parse the command string and sort
	void OnCommandSortByCommand(const char* szCommand);
	void OnCommandSortDataByCommand(const char* szCommand);

	// update current page
	enum {INSERT_DISABLE = -2, INSERT_TAIL = -1};
	void UpdateItemList(GNET::GWebTradeItem* pBegin, size_t size, bool bClear, int bInsert = INSERT_DISABLE);

	// update the item text
	void UpdateText();

	// override this if the derived class need to use its own format
	virtual ACString GetRowString(const ItemInfo& tradeItem);

	virtual bool OnInitDialog();
	virtual void OnShowDialog();
	virtual void OnTick();

	void InitKnownID();

public:
	class ItemInfo
	{
	public:
		ItemInfo(const GNET::GWebTradeItem& tradeItem);
		ItemInfo();
		~ItemInfo();
		
		enum
		{
			STATE_PRE_POST,
			STATE_POST,
			STATE_PRE_CANCEL_POST,
			STATE_SHOW,
			STATE_SELL,
		};

		enum
		{
			POSTTYPE_MONEY = 1,
			POSTTYPE_ITEM = 2,
			POSTTYPE_ROLE = 4,
		};

	private:
		ACString m_szItemName;
		ACString m_szBuyerName;
		ACString m_szSellerName;
		GNET::GWebTradeItem* m_pBasicInfo;
		CECIvtrItem* m_pItem;
		
	public:
		// return true if all names had already been got
		bool CheckNameKnown(AArray<int, int>* pBuf = NULL);
		
		CECIvtrItem* GetIvtrItem() {return m_pItem;}
		const GNET::GWebTradeItem& GetBasicInfo() const {return *m_pBasicInfo;}
		ACString GetTime() const;
		const ACString& GetItemName() const {return m_szItemName;}
		int GetTimeValue() const;
		ACString GetFormatedPrice() const;
		static ACString GetFormatedPrice(int price);
		ACString GetTradeIDHint() const;

		const ACHAR* GetSellerName() const;
		const ACHAR* GetBuyerName() const;

		void SetState(int state);
	};

protected:
	// hint related methods and members
	bool AddHint(__int64 sn, const GNET::Octets& info);
	bool AddHint(__int64 sn, const GNET::GRoleInventory& info);
	bool CheckHintKnown(__int64 sn);
	void UpdateHints();


	// category related methods and members
	typedef abase::hash_map<int, ACString> CategoryMap;
	static CategoryMap s_Category;
	bool AddCategory(int id, const ACHAR* szName);
	bool CheckCategoryKnown(int id);

	// list for the trade information
	PAUILISTBOX				m_pLst_Item;
	DWORD					m_dwLastUpdateTime;
	abase::vector<PAUIIMAGEPICTURE> m_vecImgCell;
	virtual void InsertTradeInfo(int i, ItemInfo* pInfo);
	int DeleteTradeInfo(__int64 sn);

	__int64 GetTradeID(int index);
	void SetItemInfo(int index, ItemInfo* pInfo);
	ItemInfo* GetItemInfo(int index);
	ItemInfo* GetItemInfo(__int64 sn);
	void ClearItemInfo();
	void SetIcon(int i, CECIvtrItem* pItem);
};