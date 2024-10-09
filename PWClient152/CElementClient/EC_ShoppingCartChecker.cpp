// File		: EC_ShoppingCartChecker.cpp
// Creator	: Xu Wenbin
// Date		: 2014/5/16

#include "EC_ShoppingCartChecker.h"
#include "EC_ShoppingCart.h"
#include "EC_Shop.h"

#include "EC_Game.h"
#include "EC_GameRun.h"
#include "EC_HostPlayer.h"
#include "EC_Inventory.h"

#include "globaldataman.h"

extern CECGame * g_pGame;

CECShoppingCardChecker::CECShoppingCardChecker(const CECShoppingCart* pShoppingCart)
:m_pShoppingCart(pShoppingCart){
}

const CECShoppingCart* CECShoppingCardChecker::ShoppingCart()const{
	return m_pShoppingCart;
}

//	检查是否有足够元宝购买购物车中所有商品
bool CECShoppingCardChecker::HaveEnoughCash()const{
	int price = ShoppingCart()->CashCost();
	return price != CECSHOP_INVALID_PRICE && price <= ShoppingCart()->Shop()->GetCash();
}

//	检查包裹是否有足够空间成功购买购物车中所有商品
//	购物车为空时返回 false
bool CECShoppingCardChecker::HaveEnoughSpace()const{
	bool bHave(false);

	CECInventory::GShopItemBatches	batches;
	CECInventory::GShopItemBatch	batch;

	batches.reserve(ShoppingCart()->Count());
	for (int i(0); i < ShoppingCart()->Count(); ++ i){
		const CECShoppingCartItem *item = ShoppingCart()->ItemAt(i);
		if (item->Count() > 0){
			const GSHOP_ITEM *pGShopItem = ShoppingCart()->Shop()->GetItem(item->GShopItemIndex());
			batch.id = pGShopItem->id;
			batch.num = pGShopItem->num;
			batch.gift = pGShopItem->idGift;
			batch.giftNum = pGShopItem->iGiftNum;
			batch.times = item->Count();
			batches.push_back(batch);
		}
	}
	if (g_pGame->GetGameRun()->GetHostPlayer()->GetPack()->CanBuy(batches)){
		bHave = true;
	}

	return bHave;
}

bool CECShoppingCardChecker::HaveItemsToBuy()const{
	bool bHave(false);
	for (int i(0); i < ShoppingCart()->Count(); ++ i){
		const CECShoppingCartItem *item = ShoppingCart()->ItemAt(i);
		if (item->Count() > 0){
			bHave = true;
			break;
		}
	}
	return bHave;
}