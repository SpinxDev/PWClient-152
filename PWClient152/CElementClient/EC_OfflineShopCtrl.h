/*
* FILE: EC_OfflineShopCtrl.h.h
*
* DESCRIPTION: 
*
* CREATED BY: 
*
* HISTORY: 
*
* Copyright (c) 2013, All Rights Reserved.
*/

#pragma once

#define SHOP_COUNT_PERPAGE 20
#define SHOP_COUNT_STORE 40

#include "EC_IvtrItem.h"
#include "EC_Inventory.h"
#include <set>

#include <A3DVector.h> 
#include "pshopselfget_re.hpp"


namespace GNET{
	class Protocol;
}

class CECHostPlayer;
class CECGameUIMan;
class CECGameSession;

class COfflineShopCtrl
{
public:
	enum
	{
		OST_VEHICLE = 0,//骑宠，飞行器
		OST_LOTTERY,// 彩票
		OST_MEDICINE,// 药瓶
		OST_EQUIPMENT,// 装备
		OST_FASHION,// 时装
		OST_GROCERIES, // 杂货
		OST_PRODUCEMETERIAL, //生产原料
		OST_REFINE,		// 精炼
		OST_NUM,
	};
	enum
	{
		OSTM_VEHICLE = 0x01,//骑宠，飞行器
		OSTM_LOTTERY = 0x02,// 彩票
		OSTM_MEDICINE= 0x04,// 药瓶
		OSTM_EQUIPMENT = 0x08,// 装备
		OSTM_FASHION = 0x10,// 时装
		OSTM_GROCERIES = 0x20, // 杂货
		OSTM_PRODUCE = 0x40, //生产原料
		OSTM_REFINE = 0x80,	// 精炼
		OSTM_ALL = 0xFF,
	};
	
	enum
	{
		NPCSEV_NULL = 0,
		NPCSEV_CREATE,
		NPCSEV_SETTING,
		NPCSEV_SELLBUY,
	};

	enum
	{
		ITEMSTATE_SEND_ENTERSHOP = 0x0001,
		ITEMSTATE_RECV_ENTERSHOP = 0x0003,
		ITEMSTATE_SEND_QUERYITEM_FROMSHOP = 0x0004,
		ITEMSTATE_SEND_QUERYITEM_FROMITEM = 0x0008,
		ITEMSTATE_RECV_QUERYITEM_FROMSHOP = 0x0014,
		ITEMSTATE_RECV_QUERYITEM_FROMITEM = 0x0018,
	};

public:
	// 自己的商店
	struct ShopSelf
	{
		ShopSelf()
		{
			Clear();
			buyInv.Init(SHOP_COUNT_PERPAGE);
			sellInv.Init(SHOP_COUNT_PERPAGE);
			shopStore.Init(SHOP_COUNT_STORE);
		}
		~ShopSelf()
		{
			Clear();
			buyInv.Release();
			sellInv.Release();
			shopStore.Release();
		}
		void Clear()
		{
			shopType = 0;
			shopStatus = 0;
			expireTime = 0;
			money = 0;
			yinpiao = 0;
			createtime = 0;
			buyInv.RemoveAllItems();
			sellInv.RemoveAllItems();
			shopStore.RemoveAllItems();
		}
		__int64 GetTotalPrice(bool bSell);
		int shopType;
		//ACString roleName;
		int shopStatus; // 0:正常， 1:过期
		int expireTime;		
		int createtime;
		
		CECInventory buyInv;
		CECInventory sellInv;
		
		int money; // 店铺仓库里的钱
		int yinpiao;// 店铺仓库里的银票
		CECInventory shopStore; //店铺仓库物品信息	
	};

	// 查询店铺
	struct OfShop
	{
		OfShop() {roleid = 0; shopType = 0; bNameOK = false;invState = 0;createtime=0;}
		int roleid;
		int shopType;
		int invState; // 0x01:出售栏非空;0x02:收购栏非空
		bool bNameOK;// 商店名字
		int createtime;
	};
	// functor
	struct earlier
	{
		bool operator()(const OfShop& lhs, const OfShop& rhs) const
		{
// 			if (lhs.createtime == rhs.createtime)			
// 				return lhs.roleid < rhs.roleid;			
// 			else
			return lhs.createtime < rhs.createtime;
		}
	};
	
	// 其他的商店
	struct ShopOther
	{
		ShopOther()
		{
			roleId = 0;
			shopType = 0;
			buyInv.Init(SHOP_COUNT_PERPAGE);
			sellInv.Init(SHOP_COUNT_PERPAGE);
		}
		~ShopOther()
		{
			Clear();
			buyInv.Release();
			sellInv.Release();
		}
		void Clear()
		{
			roleId = 0;
			shopType = 0;
			buyInv.RemoveAllItems();
			sellInv.RemoveAllItems();
		}
		int roleId;
		int shopType;
		CECInventory buyInv;// 收购该物品的列表	
		CECInventory sellInv;// 卖出该物品的列表
	};
	
	// 按物品查询结果
	struct ItemInfo
	{
		ItemInfo() {roleid = 0; pItem = NULL;pos = -1;}
		int roleid; // 哪个店铺有这个物品
		int pos; // 在店铺里的位置
		CECIvtrItem* pItem;
	};
	struct QueryItemResult
	{		
		QueryItemResult() 
		{
			bSellFlag = true;
			iPageNum = 0;
			iItemTid = 0;
			for (int i=0;i<SHOP_COUNT_PERPAGE;i++)
			{
				itemResult[i].roleid = 0;
				itemResult[i].pos = -1;
				itemResult[i].pItem = NULL;
			}
		}
		~QueryItemResult() {Clear(true);} 

		ItemInfo itemResult[SHOP_COUNT_PERPAGE];// 收购该物品的列表 , 最多20个，一个页面
		bool bSellFlag; // true for sell, false for buy
		int iPageNum; // 第几页
		bool bQueryItemInShopDlg; // 在店铺列表界面查询
		int iItemTid;

		void Clear(bool bClearItemID);
		int GetItemCount();
		void UpdateItem(int role,int item_id,int item_pos,int item_count);
	};
	
//	typedef int(* PSHOPFUNC)(void* pData);

	public:
		COfflineShopCtrl(CECHostPlayer* pHost);
		~COfflineShopCtrl();		

		void SetNPCSevFlag(int iCreate) { m_iNPCSevFlag = iCreate;}; // 创建，设置
		int GetNPCSevFlag() const { return m_iNPCSevFlag;}

		// 
		bool IsTailShopPage(bool bSell) const ;
		bool IsHeadShopPage(bool bSell) const ;
		void NextShopPage(bool bSell);
		void PreShopPage(bool bSell);
		
		// 		
		int GetShopID(bool bSell,int idx);
		int GetSellShopCount() { return m_sellShopArray.size();}
		int GetBuyShopCount() { return m_buyShopArray.size();}
		int GetCurOtherShopID() { return m_curOther.roleId;}
		int GetMyShopType() { return m_Self.shopType;}
		CECIvtrItem* GetELShopItem(bool bSell, int idx);
		CECIvtrItem* GetMyShopItem(bool bSell, int idx);
		CECIvtrItem* GetMyStoreItem(int idx);
		bool IsNormalMyShop() const { return m_Self.shopStatus == 0;}
		int GetMyShopExpiretime() const { return m_Self.expireTime;}
		int GetMoneyInStore() const { return m_Self.money;}
		int GetYinPiaoInStore() const { return m_Self.yinpiao;}
		int FirstEmptyPosInMyShop(bool bSell);
		bool IsELShopInvEmpty(bool bSell);
		int GetStoreEmptyNum() { return m_Self.shopStore.GetEmptySlotNum();}
		
		
		int GetQueryItemResultCount() {return m_QueryItemResult.GetItemCount();}
		COfflineShopCtrl::ItemInfo* GetQueryResultItem(bool bSell,int idx);
		int GetQueryResultItemPageNum() const { return m_QueryItemResult.iPageNum;}
		void QueryResultItemPageNumInc() { m_QueryItemResult.iPageNum++;}
		void QueryResultItemPageNumDec() { m_QueryItemResult.iPageNum--;}

		bool IsQueryItemForSell() const { return m_QueryItemResult.bSellFlag;}

		void SetItemState(int state) { m_iItemState = state;};
		bool IsInQueryItemState() const { return (m_iItemState & 0x0010)  != 0;}
		int GetItemState() const { return m_iItemState;}

		void SetQueryItemID(int id) { m_QueryItemResult.iItemTid = id; }
		int GetQueryItemID() const { return m_QueryItemResult.iItemTid;}
		
		bool CanDo(int roleid);
		void ViewShopListByType(int mask);
		int GetShopListViewType() {return m_curShopListType;} 

		bool CheckMoneyInv(bool bSell,__int64 price,int count); // 包裹总价
		bool CheckMoneyStore(int yinpiao, int money); // 存钱检查
		bool CanSellItemByPrice(__int64 price);
		bool CanPutMoneyToPack(int yinpiao, int money); // 取钱检查
		bool CanBuyItemByPrice(__int64 price,__int64& yinpiao,__int64& money); // 卖物品时检查
		bool CanTakeMoneyFromStore(int yinpiao,int money); // 是否能从仓库取钱 

		int TakeItemFromStoreToPack(); //从0位置开始找物品，找到物品，把该物品从商店取物品到背包

		bool IsShopFull(); // 出售栏+收购栏+仓库 物品<= 40

		void StoreSnapShot();
		void RemoveItemFromStoreSnapShot(int idx);
		bool IsEmptyStoreSnapShot();

	public:		
		void OnServerNotify(int iProtocol, GNET::Protocol* pProtocol);
	
	protected:
		void UpdateShopInv(CECInventory& inv, GNET::PShopItemVector& vec);
		void UpdateShopStore(CECInventory& store, GNET::GRoleInventoryVector& vec);

	protected:
		CECGameUIMan* m_pGameUIMan;		
		CECHostPlayer* m_pHostPlayer;
		CECGameSession* m_pGameSession;
		
		std::vector<OfShop> m_ShopArray;
		abase::vector<int> m_sellShopArray; // 卖东西的商店列表	,	在m_ShopList中的索引
		abase::vector<int> m_buyShopArray; // 卖东西的商店列表
		int m_curShopListType; // 当前店铺列表的类型
		
		int m_iCurSellShopPage; // 当前商店页,, 0,1,2
		int m_iCurBuyShopPage;//0,1,2			
		
		ShopSelf m_Self; // 自己店铺内容
		
		ShopOther m_curOther;// 当前浏览的其他玩家店铺内容
		
		// 按物品查询结果
		QueryItemResult m_QueryItemResult;

		int m_iNPCSevFlag; // 1 创建, 2 设置物品, 3 买卖， 0 空
	//	bool m_bInQueryItemState; // true 在查询结果界面买卖，false： 在店铺界面买卖
		int m_iItemState;	

		std::set<int> m_prepareGetAllItems; // 为了维护全取过程，记录当时仓库的情况，如果不维护这个过程的话，没法区别单取物品的情况。
		int m_iEmptySlotNumWhenSnap;
};

//////////////////////////////////////////////////////////////////////////