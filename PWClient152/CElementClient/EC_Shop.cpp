// Filename	: EC_Shop.cpp
// Creator	: Xu Wenbin
// Date		: 2013/12/11

#include "EC_Shop.h"
#include "globaldataman.h"
#include "EC_Game.h"
#include "EC_GameRun.h"
#include "EC_GameSession.h"
#include "EC_HostPlayer.h"
#include "EC_UIConfigs.h"
#include "EC_ShoppingItemsMover.h"

const int CECSHOP_INVALID_PRICE = -1;
const int BUY_COUNT = sizeof(((GSHOP_ITEM *)0)->buy)/sizeof(((GSHOP_ITEM *)0)->buy[0]);

//	class CECShopItemCategory
CECShopItemCategory::CECShopItemCategory(int mainType, int subType)
: m_mainType(mainType)
, m_subType(subType)
{
}

void CECShopItemCategory::InitFromQShopConfig(int idCategory){
	int mainType(-1), subType(-1);
	CECQShopConfig::Instance().FindCategory(idCategory, mainType, subType);
	SetCategory(mainType, subType);
}

void CECShopItemCategory::SetCategory(int mainType, int subType){
	m_mainType = mainType;
	m_subType = subType;
}

int CECShopItemCategory::GetMainType()const{
	return m_mainType;
}

int CECShopItemCategory::GetSubType()const{
	return m_subType;
}

bool CECShopItemCategory::MatchMainType(int mainType)const{
	return m_mainType >= 0 && m_mainType == mainType;
}

bool CECShopItemCategory::MatchItem(const GSHOP_ITEM &item)const{
	return MatchMainType(item.main_type)					//	主分类有效时才检查
		&& (m_subType < 0 || item.sub_type == m_subType);	//	次分类针对所有、或者针对给定值
}

//	class CECShopItemOwnerNPC
CECShopItemOwnerNPC::CECShopItemOwnerNPC(unsigned int ownerNPCID)
: m_ownerNPCID(ownerNPCID)
{
}

void CECShopItemOwnerNPC::SetOwnerNPCID(unsigned int ownerNPCID){
	m_ownerNPCID = ownerNPCID;
}

unsigned int CECShopItemOwnerNPC::GetOwnerNPCID()const{
	return m_ownerNPCID;
}

bool CECShopItemOwnerNPC::IsEmpty()const{
	return GetOwnerNPCID() == 0;
}

bool CECShopItemOwnerNPC::operator == (const CECShopItemOwnerNPC &rhs)const{
	return GetOwnerNPCID() == rhs.GetOwnerNPCID();
}

bool CECShopItemOwnerNPC::MatchID(unsigned int id)const{
	return id == m_ownerNPCID;
}

bool CECShopItemOwnerNPC::MatchItem(const GSHOP_ITEM &item)const{
	if (IsEmpty()){
		return !HasOwnerNPC(item);
	}
	bool result(false);
	for (int i(0); i < TREASURE_ITEM_OWNER_NPC_COUNT; ++ i){
		if (MatchID(item.owner_npcs[i])){
			result = true;					//	找到了
			break;
		}
		if (!item.owner_npcs[i]){
			break;							//	已经是最后一个，不用再找了
		}
	}
	return result;
}

bool CECShopItemOwnerNPC::HasOwnerNPC(const GSHOP_ITEM &item){
	return item.owner_npcs[0] != 0;			//	导出编辑器保证，如果有，则一定是都排在前面
}

//	class CECShopBaseChange
CECShopBaseChange::CECShopBaseChange(unsigned int changeMask)
: m_changeMask(changeMask)
{
}

unsigned int CECShopBaseChange::GetChangeMask()const{
	return m_changeMask;
}

bool CECShopBaseChange::ItemChanged()const{
	return (GetChangeMask() & ITEM_CHANGED) != 0;
}

//	CECShopBase

void CECShopBase::OnItemChange(){
	CECShopBaseChange change(CECShopBaseChange::ITEM_CHANGED);
	NotifyObservers(&change);
}

bool CECShopBase::ValidateTimeStamp()const{
	return GetLocalTimeStamp() == GetServerTimeStamp();
}

void CECShopBase::SetOwnerNPCID(unsigned int ownerNPCID){
	if (ownerNPCID == m_ownerNPC.GetOwnerNPCID()){
		return;
	}
	m_ownerNPC.SetOwnerNPCID(ownerNPCID);
	OnItemChange();
}

unsigned int CECShopBase::GetOwnerNPCID()const{
	return m_ownerNPC.GetOwnerNPCID();
}

bool CECShopBase::HasOwnerNPC()const{
	return !m_ownerNPC.IsEmpty();
}

const CECShopItemOwnerNPC & CECShopBase::GetOwnerNPC()const{
	return m_ownerNPC;
}

bool CECShopBase::MatchOwnerNPC(const GSHOP_ITEM &rhs)const{
	return m_ownerNPC.MatchItem(rhs);
}

bool CECShopBase::IsValid(int itemIndex, int buyIndex)const{
	bool bValid(false);
	if (const GSHOP_ITEM *pItem = GetItem(itemIndex)){
		if (buyIndex >= 0 && buyIndex < BUY_COUNT){
			bValid = true;
		}
	}
	return bValid;
}

bool CECShopBase::ReadyToBuy(int itemIndex, int buyIndex)const{
	bool result(false);
	int buyType[BUY_COUNT] = {0};
	if (IsValid(itemIndex, buyIndex) && CalcBuyType(itemIndex, buyType)){
		for (int i(0); i < BUY_COUNT; ++ i){
			if (buyType[i] == buyIndex){
				result = true;
				break;
			}
		}
	}
	return result;
}

bool CECShopBase::CalcBuyType(int itemIndex, int *buyTypes)const{
	bool bOK(false);
	if (const GSHOP_ITEM *pItem = GetItem(itemIndex)){
		int typeDefault[BUY_COUNT];
		int typeNew[BUY_COUNT];
		int index1 =0, index2 = 0;
		for (int i = 0; i < BUY_COUNT; i++){
			typeDefault[i] = -1;
			typeNew[i] = -1;
			if (pItem->buy[i].type == 3 && pItem->buy[i].price > 0){
				typeNew[index1] = i;
				index1++;
			}else if (pItem->buy[i].type == -1 && pItem->buy[i].price > 0){
				typeDefault[index2] = i;
				index2++;
			}
		}
		if (index1 > 0){
			memcpy(buyTypes, typeNew, sizeof(typeNew));
			bOK = true;
		}else if (index2 > 0){
			memcpy(buyTypes, typeDefault, sizeof(typeDefault));
			bOK = true;
		}
	}
	return bOK;
}

bool CECShopBase::HasSameBuyType(int itemIndexA, int itemIndexB)const{
	int typeA[BUY_COUNT];
	int typeB[BUY_COUNT];
	if (CalcBuyType(itemIndexA, typeA) &&
		CalcBuyType(itemIndexB, typeB) &&
		!memcmp(typeA, typeB, sizeof(typeA))){
		const GSHOP_ITEM *pItemA = GetItem(itemIndexA);
		const GSHOP_ITEM *pItemB = GetItem(itemIndexB);
		for (int i(0); i < BUY_COUNT; ++ i){
			int buyIndex = typeA[i];
			if (buyIndex != -1){
				bool priceAValid = (pItemA->buy[buyIndex].price != 0);
				bool priceBValid = (pItemB->buy[buyIndex].price != 0);
				if (priceAValid != priceBValid){
					return false;
				}
				if (!priceAValid){
					continue;
				}
				if (pItemA->buy[buyIndex].time != pItemB->buy[buyIndex].time){
					return false;
				}
			}
		}
		return true;
	}
	return false;
}

bool CECShopBase::CalcBuyIndex(int itemIndex, int *pBuyIndex, int cash/* =-1 */)const
{
	bool bOK(false);
	const GSHOP_ITEM *pItem = GetItem(itemIndex);
	if (pItem){
		//	以下内容来自 CDlgQShopItem::SetItem

		//	判断需要显示的购买方式
		int m_TypeDefault[BUY_COUNT];
		int m_TypeNew[BUY_COUNT];
		int index1 =0, index2 = 0;
		for (int i = 0; i < BUY_COUNT; i++)
		{
			m_TypeDefault[i] = -1;
			m_TypeNew[i] = -1;
			if (pItem->buy[i].type == 3 && pItem->buy[i].price > 0)
			{
				m_TypeNew[index1] = i;
				index1++;
			}
			else if (pItem->buy[i].type == -1 && pItem->buy[i].price > 0)
			{
				m_TypeDefault[index2] = i;
				index2++;
			}
		}
		int m_BuyType = -1;
		if (index1 > 0)
		{
			m_BuyType = 0;
		}
		else
			m_BuyType = 1;
		
		if (cash == -1){
			cash = GetCash();
		}
		for(i = 0; i < BUY_COUNT; i++)
		{
			int BuyIndex = 0;
			if (m_BuyType == 0)
			{
				BuyIndex = m_TypeNew[i];
			}
			else
			{
				BuyIndex = m_TypeDefault[i];
			}
			if (BuyIndex != -1){
				int price = pItem->buy[BuyIndex].price;
				if (price != 0 && price <= cash){
					bOK = true;
					if (pBuyIndex){
						*pBuyIndex = BuyIndex;
					}
					break;
				}
			}
		}
	}
	return bOK;
}

int CECShopBase::GetPrice(int itemIndex, int buyIndex)const{
	int price = CECSHOP_INVALID_PRICE;
	if (IsValid(itemIndex, buyIndex)){
		const GSHOP_ITEM *pItem = GetItem(itemIndex);
		price = pItem->buy[buyIndex].price;
	}else{
		ASSERT(false);
	}
	return price;
}

unsigned int CECShopBase::GetStatus(int itemIndex, int buyIndex)const{
	unsigned int status = -1;
	if (IsValid(itemIndex, buyIndex)){
		const GSHOP_ITEM *pItem = GetItem(itemIndex);
		status = pItem->buy[buyIndex].status;
	}else{
		ASSERT(false);
	}
	return status;
}

bool CECShopBase::IsSame(const GSHOP_ITEM &lhs, const GSHOP_ITEM &rhs){
	bool result(false);
	while (true){
		if (lhs.main_type != rhs.main_type ||
			lhs.sub_type != rhs.sub_type ||
			lhs.id != rhs.id ||
			lhs.num != rhs.num ||
			lhs.idGift != rhs.idGift ||
			lhs.iGiftNum != rhs.iGiftNum){
			break;
		}
		result = true;
		for (int i(0); i < BUY_COUNT; ++ i){
			if (lhs.buy[i].type != rhs.buy[i].type ||
				lhs.buy[i].price != rhs.buy[i].price ||
				lhs.buy[i].time != rhs.buy[i].time ||
				lhs.buy[i].status != rhs.buy[i].status){
				result = false;
				break;
			}
		}
		break;
	}
	return result;
}

int CECShopBase::GetOriginalPrice(int finalPrice, unsigned int discountStatus){
	int result = finalPrice;
	if (discountStatus >= 4 && discountStatus <= 12){
		float originalPrice = finalPrice*(10.0f/(discountStatus-3));
		if (CECUIConfig::Instance().GetGameUI().bEnableCeilPriceBeforeDiscountToGold){
			result = (int)ceil(originalPrice)+99;
			result /= 100;
			result *= 100;
		}else{
			result = (int)ceil(originalPrice);
		}
	}
	return result;
}

//	CECShopArrayItems
CECShopArrayItems::CECShopArrayItems(GShopItemsPtr pItems)
: m_pItems(pItems)
{
}

int CECShopArrayItems::GetCount()const
{
	return (int)m_pItems->size();
}

const GSHOP_ITEM * CECShopArrayItems::GetItem(int index)const
{
	return (index >= 0 && index < GetCount()) ? &(*m_pItems)[index] : NULL;
}

GSHOP_ITEM * CECShopArrayItems::GetItem(int index)
{
	return (index >= 0 && index < GetCount()) ? &(*m_pItems)[index] : NULL;
}

bool CECShopArrayItems::IsSame(const GShopItemsPtr lhs, const GShopItemsPtr rhs)
{
	bool result = false;
	if (lhs == rhs){
		result = true;
	}else if (lhs && rhs){
		while (lhs->size() == rhs->size()){
			result = true;
			for (size_t u(0); u < lhs->size(); ++ u){
				if (!IsSame((*lhs)[u], (*rhs)[u])){
					result = false;
					break;
				}
			}
			break;
		}
	}
	return result;
}

extern CECGame *g_pGame;

//	CECQShop
CECQShop & CECQShop::Instance()
{
	static CECQShop s_instance;
	return s_instance;
}

CECQShop::CECQShop()
: CECShopArrayItems(globaldata_getgshopitems())
{
	if (IsFashionShopEnabled()){
		m_fashionShopCategory.InitFromQShopConfig(CECQShopConfig::CID_QSHOP_FASHION);
	}
}

bool CECQShop::GetFromServer(int beginIndex, int endIndex){
	bool result = false;
	if (!g_pGame->GetGameRun()->GetHostPlayer()->GetCoolTime(GP_CT_GET_MALL_PRICE)){
		g_pGame->GetGameSession()->c2s_CmdGetMallItemPrice(0, 0);
		result = true;
	}else if (beginIndex != 0 || endIndex != 0){
		g_pGame->GetGameSession()->c2s_CmdGetMallItemPrice(beginIndex, endIndex);
		result = true;
	}
	return result;
}

unsigned int CECQShop::GetLocalTimeStamp()const{
	return globaldata_getgshop_timestamp();
}

unsigned int CECQShop::GetServerTimeStamp()const{
	return g_pGame->GetGameRun()->GetGShopTimeStamp();
}

bool CECQShop::UpdateFromServer(const S2C::cmd_mall_item_price *pCmd){
	GShopItems items = *m_pItems;
	if (!ApplyChangesFromServer(&items, pCmd)){
		return false;
	}
	if (IsSame(&items, m_pItems)){
		return true;
	}
	m_pItems->swap(items);
	OnItemChange();
	return true;
}

bool CECQShop::ApplyChangesFromServer(GShopItemsPtr pItems, const S2C::cmd_mall_item_price *pCmd)
{
	int i(0);
	for(i=pCmd->start_index; i<pCmd->end_index;i++){
		GSHOP_ITEM& data = (*pItems)[i];
		for(int j=0;j<4;j++){
			if(data.buy[j].type != -1){
				data.buy[j].type = -1;
				data.buy[j].price = 0;
			}
		}
	}	
	for(i=0;i<pCmd->count;i++){
		const S2C::cmd_mall_item_price::good_item& tempList = pCmd->list[i];
		GSHOP_ITEM& data = (*pItems)[tempList.good_index];		
		if(data.id == (unsigned int)tempList.good_id){					
			data.buy[tempList.good_slot].price  = tempList.goods_price;
			data.buy[tempList.good_slot].status = tempList.good_status;
			data.buy[tempList.good_slot].time   = tempList.expire_time;
			data.buy[tempList.good_slot].type   = 3;
		}else{
			ASSERT(data.id == (unsigned int)tempList.good_id);
			return false;
		}
	}
	return true;
}

int CECQShop::GetCash()const
{
	return g_pGame->GetGameRun()->GetHostPlayer()->GetCash();
}

bool CECQShop::Buy(int itemIndex, int buyIndex)const
{
	bool bOK(false);

	if (ReadyToBuy(itemIndex, buyIndex)){		
		const GSHOP_ITEM *pItem = GetItem(itemIndex);
		C2S::mall_shopping::goods item;
		item.goods_index = itemIndex;
		item.goods_id = pItem->id;
		item.goods_pos = buyIndex;
		if (!CECShopItemOwnerNPC::HasOwnerNPC(*pItem)){
			g_pGame->GetGameSession()->c2s_SendCmdMallShopping(1, &item);
		}else{
			g_pGame->GetGameSession()->c2s_CmdNPCSevMallShopping(1, &item);
		}
		CECQShopConfig::Instance().OnItemBuyed(item.goods_id);
		CECShoppingItemsMover::Instance().OnItemBuyed(this, itemIndex, buyIndex);
		bOK = true;
	}else{
		assert(false);
	}
	return bOK;
}

bool CECQShop::IsFashionShopEnabled()const{
	return CECUIConfig::Instance().GetGameUI().bEnableQShopFashionShop;
}

bool CECQShop::IsFashionShopFlashSaleEnabled()const{
	return CECUIConfig::Instance().GetGameUI().bEnableQShopFashionShopFlashSale;
}

const ACString &CECQShop::GetFashionShopFlashSaleTitle()const{
	return CECUIConfig::Instance().GetGameUI().strQShopFashionShopFlashSaleTitle;
}

const CECShopItemCategory & CECQShop::GetFashionShopCategory()const{
	return m_fashionShopCategory;
}

//	CECBackShop
CECBackShop & CECBackShop::Instance()
{
	static CECBackShop s_instance;
	return s_instance;
}

CECBackShop::CECBackShop()
: CECShopArrayItems(globaldata_getgshopitems2())
{	
	if (IsFashionShopEnabled()){
		m_fashionShopCategory.InitFromQShopConfig(CECQShopConfig::CID_BACKSHOP_FASHION);
	}
}

unsigned int CECBackShop::GetLocalTimeStamp()const{
	return globaldata_getgshop_timestamp2();
}

unsigned int CECBackShop::GetServerTimeStamp()const{
	return g_pGame->GetGameRun()->GetGShopTimeStamp2();
}

bool CECBackShop::GetFromServer(int beginIndex, int endIndex){
	bool result = false;
	if (!g_pGame->GetGameRun()->GetHostPlayer()->GetCoolTime(GP_CT_GET_DIVIDEND_MALL_PRICE)){
		g_pGame->GetGameSession()->c2s_CmdGetDividendMallItemPrice(0, 0);
		result = true;
	}else if (beginIndex != 0 || endIndex != 0){
		g_pGame->GetGameSession()->c2s_CmdGetDividendMallItemPrice(beginIndex, endIndex);
		result = true;
	}
	return result;
}

bool CECBackShop::UpdateFromServer(const S2C::cmd_dividend_mall_item_price *pCmd){
	GShopItems items = *m_pItems;
	if (!ApplyChangesFromServer(&items, pCmd)){
		return false;
	}
	if (IsSame(&items, m_pItems)){
		return true;
	}
	m_pItems->swap(items);
	OnItemChange();
	return true;
}

bool CECBackShop::ApplyChangesFromServer(GShopItemsPtr pItems, const S2C::cmd_dividend_mall_item_price *pCmd)
{
	int i(0);
	for(i=pCmd->start_index; i<pCmd->end_index;i++){
		GSHOP_ITEM& data = (*pItems)[i];
		for(int j=0;j<4;j++){
			if(data.buy[j].type != -1){
				data.buy[j].type = -1;
				data.buy[j].price = 0;
			}
		}
	}	
	for(i=0;i<pCmd->count;i++){
		const S2C::cmd_dividend_mall_item_price::good_info& tempList = pCmd->list[i]; 
		GSHOP_ITEM& data = (*pItems)[tempList.good_index];
		if(data.id == (unsigned int)tempList.good_id){					
			data.buy[tempList.good_slot].price  = tempList.good_price;
			data.buy[tempList.good_slot].status = tempList.good_status;
			data.buy[tempList.good_slot].time   = tempList.expire_time;
			data.buy[tempList.good_slot].type   = 3;
		}else{
			ASSERT(data.id == (unsigned int)tempList.good_id);
			return false;
		}
	}
	return true;
}

int CECBackShop::GetCash()const
{
	extern CECGame *g_pGame;
	return g_pGame->GetGameRun()->GetHostPlayer()->GetDividend();
}

bool CECBackShop::Buy(int itemIndex, int buyIndex)const
{
	bool bOK(false);
	
	if (ReadyToBuy(itemIndex, buyIndex)){
		const GSHOP_ITEM *pItem = GetItem(itemIndex);
		C2S::cmd_dividend_mall_shopping::goods item;
		item.goods_index = itemIndex;
		item.goods_id = pItem->id;
		item.goods_slot = buyIndex;
		if (!CECShopItemOwnerNPC::HasOwnerNPC(*pItem)){
			g_pGame->GetGameSession()->c2s_SendCmdDividendMallShopping(1, &item);
		}else{			
			g_pGame->GetGameSession()->c2s_CmdNPCSevDividendMallShopping(1, &item);
		}
		CECQShopConfig::Instance().OnItemBuyed(item.goods_id);
		CECShoppingItemsMover::Instance().OnItemBuyed(this, itemIndex, buyIndex);
		bOK = true;
	}else{
		assert(false);
	}
	return bOK;
}

bool CECBackShop::IsFashionShopEnabled()const{
	return CECUIConfig::Instance().GetGameUI().bEnableBackShopFashionShop;
}

bool CECBackShop::IsFashionShopFlashSaleEnabled()const{
	return CECUIConfig::Instance().GetGameUI().bEnableBackShopFashionShopFlashSale;
}

const ACString &CECBackShop::GetFashionShopFlashSaleTitle()const{
	return CECUIConfig::Instance().GetGameUI().strBackShopFashionShopFlashSaleTitle;
}

const CECShopItemCategory & CECBackShop::GetFashionShopCategory()const{
	return m_fashionShopCategory;
}