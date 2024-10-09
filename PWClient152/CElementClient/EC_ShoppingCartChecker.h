// File		: EC_ShoppingCartChecker.h
// Creator	: Xu Wenbin
// Date		: 2014/5/16

#ifndef _ELEMENTCLIENT_EC_SHOPPINGCARTCHECKER_H_
#define _ELEMENTCLIENT_EC_SHOPPINGCARTCHECKER_H_

//	前置声明和定义
class CECShoppingCart;

//	购物车可购买性等检查类
class CECShoppingCardChecker
{
	const CECShoppingCart*	m_pShoppingCart;
public:
	CECShoppingCardChecker(const CECShoppingCart* pShoppingCart);
	const CECShoppingCart* ShoppingCart()const;

	bool HaveEnoughCash()const;
	bool HaveEnoughSpace()const;

	bool HaveItemsToBuy()const;
};

#endif	//	_ELEMENTCLIENT_EC_SHOPPINGCARTCHECKER_H_