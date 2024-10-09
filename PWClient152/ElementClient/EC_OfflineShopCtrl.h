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
		OST_VEHICLE = 0,//��裬������
		OST_LOTTERY,// ��Ʊ
		OST_MEDICINE,// ҩƿ
		OST_EQUIPMENT,// װ��
		OST_FASHION,// ʱװ
		OST_GROCERIES, // �ӻ�
		OST_PRODUCEMETERIAL, //����ԭ��
		OST_REFINE,		// ����
		OST_NUM,
	};
	enum
	{
		OSTM_VEHICLE = 0x01,//��裬������
		OSTM_LOTTERY = 0x02,// ��Ʊ
		OSTM_MEDICINE= 0x04,// ҩƿ
		OSTM_EQUIPMENT = 0x08,// װ��
		OSTM_FASHION = 0x10,// ʱװ
		OSTM_GROCERIES = 0x20, // �ӻ�
		OSTM_PRODUCE = 0x40, //����ԭ��
		OSTM_REFINE = 0x80,	// ����
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
	// �Լ����̵�
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
		int shopStatus; // 0:������ 1:����
		int expireTime;		
		int createtime;
		
		CECInventory buyInv;
		CECInventory sellInv;
		
		int money; // ���ֿ̲����Ǯ
		int yinpiao;// ���ֿ̲������Ʊ
		CECInventory shopStore; //���ֿ̲���Ʒ��Ϣ	
	};

	// ��ѯ����
	struct OfShop
	{
		OfShop() {roleid = 0; shopType = 0; bNameOK = false;invState = 0;createtime=0;}
		int roleid;
		int shopType;
		int invState; // 0x01:�������ǿ�;0x02:�չ����ǿ�
		bool bNameOK;// �̵�����
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
	
	// �������̵�
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
		CECInventory buyInv;// �չ�����Ʒ���б�	
		CECInventory sellInv;// ��������Ʒ���б�
	};
	
	// ����Ʒ��ѯ���
	struct ItemInfo
	{
		ItemInfo() {roleid = 0; pItem = NULL;pos = -1;}
		int roleid; // �ĸ������������Ʒ
		int pos; // �ڵ������λ��
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

		ItemInfo itemResult[SHOP_COUNT_PERPAGE];// �չ�����Ʒ���б� , ���20����һ��ҳ��
		bool bSellFlag; // true for sell, false for buy
		int iPageNum; // �ڼ�ҳ
		bool bQueryItemInShopDlg; // �ڵ����б�����ѯ
		int iItemTid;

		void Clear(bool bClearItemID);
		int GetItemCount();
		void UpdateItem(int role,int item_id,int item_pos,int item_count);
	};
	
//	typedef int(* PSHOPFUNC)(void* pData);

	public:
		COfflineShopCtrl(CECHostPlayer* pHost);
		~COfflineShopCtrl();		

		void SetNPCSevFlag(int iCreate) { m_iNPCSevFlag = iCreate;}; // ����������
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

		bool CheckMoneyInv(bool bSell,__int64 price,int count); // �����ܼ�
		bool CheckMoneyStore(int yinpiao, int money); // ��Ǯ���
		bool CanSellItemByPrice(__int64 price);
		bool CanPutMoneyToPack(int yinpiao, int money); // ȡǮ���
		bool CanBuyItemByPrice(__int64 price,__int64& yinpiao,__int64& money); // ����Ʒʱ���
		bool CanTakeMoneyFromStore(int yinpiao,int money); // �Ƿ��ܴӲֿ�ȡǮ 

		int TakeItemFromStoreToPack(); //��0λ�ÿ�ʼ����Ʒ���ҵ���Ʒ���Ѹ���Ʒ���̵�ȡ��Ʒ������

		bool IsShopFull(); // ������+�չ���+�ֿ� ��Ʒ<= 40

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
		abase::vector<int> m_sellShopArray; // ���������̵��б�	,	��m_ShopList�е�����
		abase::vector<int> m_buyShopArray; // ���������̵��б�
		int m_curShopListType; // ��ǰ�����б������
		
		int m_iCurSellShopPage; // ��ǰ�̵�ҳ,, 0,1,2
		int m_iCurBuyShopPage;//0,1,2			
		
		ShopSelf m_Self; // �Լ���������
		
		ShopOther m_curOther;// ��ǰ�����������ҵ�������
		
		// ����Ʒ��ѯ���
		QueryItemResult m_QueryItemResult;

		int m_iNPCSevFlag; // 1 ����, 2 ������Ʒ, 3 ������ 0 ��
	//	bool m_bInQueryItemState; // true �ڲ�ѯ�������������false�� �ڵ��̽�������
		int m_iItemState;	

		std::set<int> m_prepareGetAllItems; // Ϊ��ά��ȫȡ���̣���¼��ʱ�ֿ������������ά��������̵Ļ���û������ȡ��Ʒ�������
		int m_iEmptySlotNumWhenSnap;
};

//////////////////////////////////////////////////////////////////////////