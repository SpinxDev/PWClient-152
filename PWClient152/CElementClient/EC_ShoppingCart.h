// File		: EC_ShoppingCart.h
// Creator	: Xu Wenbin
// Date		: 2014/5/14

#ifndef _ELEMENTCLIENT_EC_SHOPPINGCART_H_
#define _ELEMENTCLIENT_EC_SHOPPINGCART_H_

#include "EC_Observer.h"
#include <vector.h>

//	前置声明
class CECShopBase;
typedef CECObserver<CECShopBase> CECShopBaseObserver;
class CECShoppingCartItem;
class CECShoppingCart;

//	typedef
typedef int	ItemShoppingCartID;
extern const ItemShoppingCartID INVALID_ITEM_SHOPPINGCART_ID;

//	class CECShoppingCartChange
//	类 CECShoppingCart 数据改变通知类型
class CECShoppingCartChange : public CECObservableChange
{
public:
	enum ChangeMask{
		CART_CLEARED,			//	购物车清空
		CART_LOCKED,			//	购物车被锁定
		CART_UNLOCKED,			//	购物车被取消锁定
		ITEM_ADDED,				//	新增物品项
		ITEM_COUNT_CHANGE,		//	物品数目变化
		ITEM_REMOVED,			//	物品被删除
		MALL_ITEMS_CHANGED,		//	商城更新
	};
private:
	ChangeMask m_changeMask;
	CECShoppingCartItem*	m_pItem;						//	关联物品指针（仅供查询）
	int						m_addedCount;					//	变化的数量，可为负值

	//	禁用
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

//	定义 CECShoppingCart 的观察者
typedef CECObserver<CECShoppingCart>	CECShoppingCartObserver;

//	购物车内一项物品
class CECShoppingCartItem
{
	ItemShoppingCartID	m_shopCardItemID;	//	在购物车中ID，用于查找定位
	int					m_itemIndex;		//	在普通商城（或鸿利商城）总数据中的下标
	int					m_buyIndex;			//	在 GSHOP_ITEM::buy[4] 中的下标
	int					m_count;			//	购买份数

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

//	购物车数据模型
class CECShoppingController;
class CECShoppingCart : public CECObservable<CECShoppingCart>, public CECShopBaseObserver
{
	CECShopBase		*m_pShop;

	typedef abase::vector<CECShoppingCartItem>	ShoppingCartItems;
	ShoppingCartItems	m_items;
	ItemShoppingCartID	m_nextID;

	bool			m_locked;		//	是否被锁定

private:	
	const CECShoppingCartItem *ItemForIDImpl(ItemShoppingCartID id)const;
	const CECShoppingCartItem *ItemForShopIndexImpl(int itemIndex, int buyIndex)const;
	
	CECShoppingCartItem *ItemForID(ItemShoppingCartID id);
	CECShoppingCartItem *ItemForShopIndex(int itemIndex, int buyIndex);

	bool ModifyItemCount(CECShoppingCartItem *pItem, int count);
	
	//	授权 CECShoppingController 购买过程中对已购买物品从购物车中移除，同时禁止界面操作
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

	//	派生自 CECShopBaseObserver
	virtual void OnModelChange(const CECShopBase *p, const CECObservableChange *q);

	//	查询方法
	const CECShopBase * Shop()const;
	bool  IsEmpty()const;
	int	  Count()const;
	int	  CashCost()const;
	const CECShoppingCartItem *ItemAt(int index)const;
	const CECShoppingCartItem *ItemForID(ItemShoppingCartID id)const;
	const CECShoppingCartItem *ItemForShopIndex(int itemIndex, int buyIndex)const;
	bool  IsLocked()const;

	//	（界面操作）物品添加、修改、删除
	bool AddItem(int itemIndex, int buyIndex, int count);
	bool IncreaseItemCount(ItemShoppingCartID id, int count);
	bool DecreaseItemCount(ItemShoppingCartID id, int count);
	bool SetItemCount(ItemShoppingCartID id, int count);
	bool RemoveItem(ItemShoppingCartID id);

	//	购买等相关
	void Lock();
	void Unlock();

	//	清空购物车
	void Clear();
};

#endif	//	_ELEMENTCLIENT_EC_SHOPPINGCART_H_