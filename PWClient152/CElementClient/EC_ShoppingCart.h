// File		: EC_ShoppingCart.h
// Creator	: Xu Wenbin
// Date		: 2014/5/14

#ifndef _ELEMENTCLIENT_EC_SHOPPINGCART_H_
#define _ELEMENTCLIENT_EC_SHOPPINGCART_H_

#include "EC_Observer.h"
#include <vector.h>

//	ǰ������
class CECShopBase;
typedef CECObserver<CECShopBase> CECShopBaseObserver;
class CECShoppingCartItem;
class CECShoppingCart;

//	typedef
typedef int	ItemShoppingCartID;
extern const ItemShoppingCartID INVALID_ITEM_SHOPPINGCART_ID;

//	class CECShoppingCartChange
//	�� CECShoppingCart ���ݸı�֪ͨ����
class CECShoppingCartChange : public CECObservableChange
{
public:
	enum ChangeMask{
		CART_CLEARED,			//	���ﳵ���
		CART_LOCKED,			//	���ﳵ������
		CART_UNLOCKED,			//	���ﳵ��ȡ������
		ITEM_ADDED,				//	������Ʒ��
		ITEM_COUNT_CHANGE,		//	��Ʒ��Ŀ�仯
		ITEM_REMOVED,			//	��Ʒ��ɾ��
		MALL_ITEMS_CHANGED,		//	�̳Ǹ���
	};
private:
	ChangeMask m_changeMask;
	CECShoppingCartItem*	m_pItem;						//	������Ʒָ�루������ѯ��
	int						m_addedCount;					//	�仯����������Ϊ��ֵ

	//	����
	CECShoppingCartChange(const CECShoppingCartChange &);
	CECShoppingCartChange & operator=(const CECShoppingCartChange &);
public:
	CECShoppingCartChange(ChangeMask changeMask, CECShoppingCartItem *pCartItem=NULL, int addedItemCount=0);

	ChangeMask GetChangeMask()const;
	const CECShoppingCartItem *Item()const;
	int	  AddedItemCount()const;

	bool	IsClear()const;
	bool	IsLock()const;
	bool	IsUnlock()const;
	bool	IsItemCountChanged()const;
	bool	IsRemoveItem()const;
	bool	IsAddItem()const;
	bool	IsMallItemsChanged()const;
};

//	���� CECShoppingCart �Ĺ۲���
typedef CECObserver<CECShoppingCart>	CECShoppingCartObserver;

//	���ﳵ��һ����Ʒ
class CECShoppingCartItem
{
	ItemShoppingCartID	m_shopCardItemID;	//	�ڹ��ﳵ��ID�����ڲ��Ҷ�λ
	int					m_itemIndex;		//	����ͨ�̳ǣ�������̳ǣ��������е��±�
	int					m_buyIndex;			//	�� GSHOP_ITEM::buy[4] �е��±�
	int					m_count;			//	�������

	friend class CECShoppingCart;
	void	Increase(int count);
	void	Decrease(int count);

public:
	CECShoppingCartItem(ItemShoppingCartID id, int itemIndex, int buyIndex, int count);
	ItemShoppingCartID ItemCartID()const;
	int		GShopItemIndex()const;
	int		BuyIndex()const;
	int		Count()const;

	bool operator == (ItemShoppingCartID id)const;
};

//	���ﳵ����ģ��
class CECShoppingController;
class CECShoppingCart : public CECObservable<CECShoppingCart>, public CECShopBaseObserver
{
	CECShopBase		*m_pShop;

	typedef abase::vector<CECShoppingCartItem>	ShoppingCartItems;
	ShoppingCartItems	m_items;
	ItemShoppingCartID	m_nextID;

	bool			m_locked;		//	�Ƿ�����

private:	
	const CECShoppingCartItem *ItemForIDImpl(ItemShoppingCartID id)const;
	const CECShoppingCartItem *ItemForShopIndexImpl(int itemIndex, int buyIndex)const;
	
	CECShoppingCartItem *ItemForID(ItemShoppingCartID id);
	CECShoppingCartItem *ItemForShopIndex(int itemIndex, int buyIndex);

	bool ModifyItemCount(CECShoppingCartItem *pItem, int count);
	
	//	��Ȩ CECShoppingController ��������ж��ѹ�����Ʒ�ӹ��ﳵ���Ƴ���ͬʱ��ֹ�������
	friend class CECShoppingController;

	bool AddItemImpl(int itemIndex, int buyIndex, int count, bool bForce);
	bool IncreaseItemCountImpl(ItemShoppingCartID id, int count, bool bForce);
	bool DecreaseItemCountImpl(ItemShoppingCartID id, int count, bool bForce);
	bool SetItemCountImpl(ItemShoppingCartID id, int count, bool bForce);
	bool RemoveItemImpl(ItemShoppingCartID id, bool bForce);
	
	CECShoppingCart(const CECShoppingCart &);
	CECShoppingCart & operator = (const CECShoppingCart &);

public:
	CECShoppingCart(CECShopBase *pShop);
	~CECShoppingCart();

	//	������ CECShopBaseObserver
	virtual void OnModelChange(const CECShopBase *p, const CECObservableChange *q);

	//	��ѯ����
	const CECShopBase * Shop()const;
	bool  IsEmpty()const;
	int	  Count()const;
	int	  CashCost()const;
	const CECShoppingCartItem *ItemAt(int index)const;
	const CECShoppingCartItem *ItemForID(ItemShoppingCartID id)const;
	const CECShoppingCartItem *ItemForShopIndex(int itemIndex, int buyIndex)const;
	bool  IsLocked()const;

	//	�������������Ʒ��ӡ��޸ġ�ɾ��
	bool AddItem(int itemIndex, int buyIndex, int count);
	bool IncreaseItemCount(ItemShoppingCartID id, int count);
	bool DecreaseItemCount(ItemShoppingCartID id, int count);
	bool SetItemCount(ItemShoppingCartID id, int count);
	bool RemoveItem(ItemShoppingCartID id);

	//	��������
	void Lock();
	void Unlock();

	//	��չ��ﳵ
	void Clear();
};

#endif	//	_ELEMENTCLIENT_EC_SHOPPINGCART_H_