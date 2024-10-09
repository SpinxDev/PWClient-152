// File		: EC_ShoppingCartChecker.h
// Creator	: Xu Wenbin
// Date		: 2014/5/16

#ifndef _ELEMENTCLIENT_EC_SHOPPINGCARTCHECKER_H_
#define _ELEMENTCLIENT_EC_SHOPPINGCARTCHECKER_H_

//	ǰ�������Ͷ���
class CECShoppingCart;

//	���ﳵ�ɹ����Եȼ����
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