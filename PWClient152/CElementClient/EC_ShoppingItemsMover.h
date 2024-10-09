// Filename	: EC_ShoppingItemsMover.h
// Creator	: Xu Wenbin
// Date		: 2014/6/4

#ifndef _ELEMENTCLIENT_EC_SHOPPINGITEMSMOVER_H_
#define _ELEMENTCLIENT_EC_SHOPPINGITEMSMOVER_H_

#include <ABaseDef.h>
#include <vector.h>

class CECShopBase;
class CECIvtrItem;
class CECShoppingItemsMover
{
	CECShoppingItemsMover();

	CECShoppingItemsMover(const CECShoppingItemsMover &);
	CECShoppingItemsMover & operator = (const CECShoppingItemsMover &);

	struct Record 
	{
		const CECShopBase * m_pShop;
		int					m_itemIndex;
		int					m_buyIndex;
		DWORD				m_beginTime;		//	添加到队列中的时候，用于倒计时
		bool				m_itemMoved;		//	是否已处理过 GSHOP_ITEM 中 id 的移动
		bool				m_giftMoved;		//	是否已处理过 GSHOP_ITEM 中 idGift 的移动

		Record();
		Record(const CECShopBase *pShop, int itemIndex, int buyIndex);

		bool MoveItem(int id);
		bool IsFinished()const;
	};
	typedef abase::vector<Record>	Records;
	Records	m_records;

	struct MoveCommand 
	{
		int		m_id;		//	普通包裹中物品ID（用于验证）
		int		m_slot;		//	普通包裹中位置（移动物品协议需要）
		int		m_iPack;	//	希望要移往的 Pack（移动物品协议需要）
		int		m_targetSlot;	//	目标包裹位置（为-1表明还未开始移动）
		DWORD	m_runTime;		//	执行时间

		MoveCommand();
		MoveCommand(int id, int slot, int pack);
		void OnExecute(int targetSlot);
	};
	typedef abase::vector<MoveCommand> MoveCommands;
	MoveCommands	m_commands;

public:
	static CECShoppingItemsMover &Instance();

	void OnItemBuyed(const CECShopBase *pShop, int itemIndex, int buyIndex);
	bool MoveItem(const CECIvtrItem* pItem, int where, int slot);
	bool OnItemExchanged(int where, int slot, const CECIvtrItem *pItem, int inventorySlot, const CECIvtrItem *pInventoryItem);
	void Tick();

	void Clear();
};


#endif	//	_ELEMENTCLIENT_EC_SHOPPINGITEMSMOVER_H_