// Filename	: DlgBShop.h
// Creator	: Feng Ning
// Date		: 2010/05/12

#pragma once

#include "DlgBase.h"
#include "AUIStillImageButton.h"
#include "AUIEditBox.h"
#include "AUIListBox.h"
#include "AUITextArea.h"
#include "hashmap.h"
#include "gnoctets.h"

namespace GNET
{
	class GRoleInventory;
	class GSysAuctionItem;
};

class CECIvtrItem;
class CDlgBShop : public CDlgBase
{
	AUI_DECLARE_EVENT_MAP()
	AUI_DECLARE_COMMAND_MAP()

public:
	CDlgBShop();

	virtual bool OnInitDialog();
	virtual void OnShowDialog();
	virtual bool Release();
	virtual void OnTick();

	void OnSysAuctionAction(int idAction, void *pData);
	void OnMessageBox(AUIDialog* pDlg, int iRetVal);
	bool OnSysAuctionMessage(int idType, const void* pBuf, size_t sz, bool isEnqueue  = true);

	void OnCommand_BidItem(const char *szCommand);
	void OnCommand_ReturnCash(const char *szCommand);
	void OnCommand_StartAuto(const char *szCommand);
	void OnCommand_CancelAuto(const char *szCommand);
	void OnCommand_Payment(const char *szCommand);

protected:
	void SetDirty(bool v) {m_Dirty = v;}
	bool IsDirty() const {return m_Dirty;}
	
	void SetAuto(unsigned int v) {m_AutoBidItem = v;}
	bool IsAuto() const {return m_AutoBidItem != 0;}

	bool IsBiddable();

	enum AuctionState
	{
		ASTATE_PREPARE_START,
		ASTATE_START,
		ASTATE_PREPARE_END,
		ASTATE_END,
	};

	class AuctionItemInfo
	{
	public:
		GNET::GSysAuctionItem* pNetData;
		CECIvtrItem* pItem;
		A2DSprite* pCover;

		unsigned int MaxPrice;
		unsigned int DeltaPrice;
		
		AuctionItemInfo()
			:pNetData(0), pItem(0), pCover(0), MaxPrice(0), DeltaPrice(0){}

		~AuctionItemInfo();
	};
	typedef abase::hash_map<unsigned int, AuctionItemInfo*> AuctionItemInfoMap;

	AuctionItemInfo* SetSelectedItem(AuctionItemInfo* p) {return m_pCurrent = p;}
	AuctionItemInfo* GetSelectedItem() { return m_pCurrent;}

	ACString GetCashText(int nCash, bool bFullText);

private:
	void RefreshContent();
	
	void ShowAuctionItem(const GNET::GSysAuctionItem* pInfo);
	void ShowItemDesc(AuctionItemInfo* pInfo);
	void ShowItems(AuctionItemInfoMap* pItems);
	void ShowCash(unsigned int freezeCash, unsigned int curCash);

	AuctionItemInfo* UpdateAuctionItem(const GNET::GSysAuctionItem* pItem);
	
	void OnSysAuctionBid(void* pData);
	void OnSysAuctionAccount(void* pData);
	void OnSysAuctionCashTransfer(void* pData);
	void OnSysAuctionGetItem(void* pData);
	void OnSysAuctionList(void* pData);

	void DoBidding();
	void DoAutoBidding();

	ACString GetItemName(unsigned int sa_id, unsigned int item_id);
	AuctionItemInfo* GetListSelectedItem();
	bool LoadItemIcon(AuctionItemInfo* pInfo);
	
private:
	PAUISTILLIMAGEBUTTON	m_pBtn_BidItem;
	PAUISTILLIMAGEBUTTON	m_pBtn_StartAuto;
	PAUISTILLIMAGEBUTTON	m_pBtn_CancelAuto;
	PAUISTILLIMAGEBUTTON	m_pBtn_ReturnCash;
	PAUIOBJECT				m_pEdt_BidPrice;
	PAUIOBJECT				m_pEdt_MaxPrice;
	PAUIOBJECT				m_pEdt_DeltaPrice;
	PAUILISTBOX				m_pList_Items;
	PAUITEXTAREA			m_pTxt_Details;
	abase::vector<A2DSprite*> m_vecIconList;
	ACString				m_strLastSelectedItem;
	
	// force update GUI
	bool m_UpdateNow;

	// avoid update GUI in same second
	int m_LastUpdateTime;

	// record last lock time
	int m_LastLockTime;

	// whether need to refresh on tick
	bool m_Dirty;

	// the trade id that bidding
	unsigned int m_AutoBidItem;

	// current auction status
	enum STATE
	{
		STATE_WAIT,
		STATE_CONFIRM,
		STATE_BIDDING,
		STATE_BIDDING_WAIT,
		STATE_AUTOTRANSFERING,
		STATE_TRANSFERING,
	};
	STATE m_State;
	STATE m_StateLast;

	STATE GetBidState() const {return m_State;}
	STATE SetBidState(STATE state);

	// account information
	class AuctionAccount
	{
	public:
		unsigned int Cash;
		abase::vector<unsigned int> Items;

		AuctionAccount():Cash(0){};
		bool Add(unsigned int id);
		bool Remove(unsigned int id);
		int IsIn(unsigned int id);
	};
	AuctionAccount m_Account;

	// current shop contents
	AuctionItemInfoMap* m_pShopItems;
	AuctionItemInfo*	m_pCurrent;

	void SwitchItemList(AuctionItemInfoMap* pItems);

	// store the message in queue
	struct AuctionMessage
	{
		int type;
		GNET::Octets o;
	};
	typedef AList2<AuctionMessage> MessageCache;
	MessageCache m_MsgCache;
	void CheckDelayMessage();
	const ACHAR* CheckUserName(int idType, GNET::Octets& o, int roleid, bool isEnqueue);
};