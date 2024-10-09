// File		: EC_ShoppingManager.cpp
// Creator	: Xu Wenbin
// Date		: 2014/5/20

#include "EC_ShoppingManager.h"
#include "EC_Shop.h"
#include "EC_ShoppingCart.h"
#include "EC_ShoppingController.h"

#include <ABaseDef.h>

//	class CECShoppingManager
CECShoppingManager::CECShoppingManager()
{
	for (int i(0); i < SHOPPING_CART_COUNT; ++ i){
		m_pShoppingCarts[i] = NULL;
		m_pShoppingControllers[i] = NULL;
	}
}

CECShoppingManager::~CECShoppingManager(){
	Clear();
}

CECShoppingManager & CECShoppingManager::Instance(){
	static CECShoppingManager s_instance;
	return s_instance;
}

void CECShoppingManager::Initialize(){
	if (m_pShoppingCarts[0]){
		ASSERT(false);
		return;
	}
	m_pShoppingCarts[QSHOP_FASHION_SHOPPING_CART]	= new CECShoppingCart(&CECQShop::Instance());
	m_pShoppingCarts[BACKSHOP_FASHION_SHOPPING_CART]= new CECShoppingCart(&CECBackShop::Instance());
	for (int i(0); i < SHOPPING_CART_COUNT; ++ i){
		m_pShoppingControllers[i] = new CECShoppingController;
		m_pShoppingControllers[i]->SetShoppingCart(m_pShoppingCarts[i]);
	}
}

void CECShoppingManager::Clear(){
	for (int i(0); i < SHOPPING_CART_COUNT; ++ i){
		if (m_pShoppingControllers[i]){
			if (m_pShoppingControllers[i]->IsBuying()){
				m_pShoppingControllers[i]->StopBuying(SCSBR_CLEAR);
			}
			delete m_pShoppingControllers[i];
			m_pShoppingControllers[i] = NULL;
		}
		if (m_pShoppingCarts[i]){
			m_pShoppingCarts[i]->Clear();
			delete m_pShoppingCarts[i];
			m_pShoppingCarts[i] = NULL;
		}
	}
}

CECShoppingCart * CECShoppingManager::ShoppingCartAt(int index){
	return m_pShoppingCarts[index];
}
CECShoppingController * CECShoppingManager::ShoppingControllerAt(int index){
	return m_pShoppingControllers[index];
}
CECShoppingController * CECShoppingManager::ShoppingControllerFor(const CECShoppingCart *pShoppingCart){
	for (int i(0); i < SHOPPING_CART_COUNT; ++ i){
		if (ShoppingCartAt(i) == pShoppingCart){
			return ShoppingControllerAt(i);
		}
	}
	return NULL;
}

CECShoppingCart * CECShoppingManager::QShopFashionShoppingCart(){
	return ShoppingCartAt(QSHOP_FASHION_SHOPPING_CART);
}
CECShoppingController * CECShoppingManager::QShopFashionShoppingController(){
	return ShoppingControllerAt(QSHOP_FASHION_SHOPPING_CART);
}

CECShoppingCart * CECShoppingManager::BackShopFashionShoppingCart(){
	return ShoppingCartAt(BACKSHOP_FASHION_SHOPPING_CART);
}
CECShoppingController * CECShoppingManager::BackShopFashionShoppingController(){
	return ShoppingControllerAt(BACKSHOP_FASHION_SHOPPING_CART);
}

void CECShoppingManager::Tick(){
	for (int i(0); i < SHOPPING_CART_COUNT; ++ i){
		if (m_pShoppingControllers[i]){
			m_pShoppingControllers[i]->Tick();
		}
	}
}

bool CECShoppingManager::OnObtainItem(int iPack, int idItem, int iAmount){
	bool isShoppedItem(false);
	for (int i(0); i < SHOPPING_CART_COUNT; ++ i){
		if (m_pShoppingControllers[i] &&
			m_pShoppingControllers[i]->OnObtainItem(iPack, idItem, iAmount)){
			isShoppedItem = true;
			break;
		}
	}
	return isShoppedItem;
}