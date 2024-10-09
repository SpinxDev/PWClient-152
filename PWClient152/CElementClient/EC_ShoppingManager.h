// File		: EC_ShoppingManager.h
// Creator	: Xu Wenbin
// Date		: 2014/5/20

#ifndef _ELEMENTCLIENT_EC_SHOPPINGMANAGER_H_
#define _ELEMENTCLIENT_EC_SHOPPINGMANAGER_H_

class CECShoppingCart;
class CECShoppingController;

//	所有购物车管理类
class CECShoppingManager
{
public:
	enum{
		QSHOP_FASHION_SHOPPING_CART,
		BACKSHOP_FASHION_SHOPPING_CART,
		SHOPPING_CART_COUNT,
	};

private:
	CECShoppingCart	*		m_pShoppingCarts[SHOPPING_CART_COUNT];
	CECShoppingController *	m_pShoppingControllers[SHOPPING_CART_COUNT];
	
	CECShoppingManager();

	//	禁用
	CECShoppingManager(const CECShoppingManager &);
	CECShoppingManager & operator = (const CECShoppingManager &);

public:
	~CECShoppingManager();
	static CECShoppingManager & Instance();

	void Initialize();
	void Clear();

	CECShoppingCart * ShoppingCartAt(int index);
	CECShoppingController * ShoppingControllerAt(int index);
	CECShoppingController * ShoppingControllerFor(const CECShoppingCart *pShoppingCart);

	CECShoppingCart * QShopFashionShoppingCart();
	CECShoppingController * QShopFashionShoppingController();

	CECShoppingCart * BackShopFashionShoppingCart();
	CECShoppingController * BackShopFashionShoppingController();

	void Tick();
	bool OnObtainItem(int iPack, int idItem, int iAmount);
};

#endif	//	_ELEMENTCLIENT_EC_SHOPPINGMANAGER_H_