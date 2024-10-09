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
		DWORD				m_beginTime;		//	��ӵ������е�ʱ�����ڵ���ʱ
		bool				m_itemMoved;		//	�Ƿ��Ѵ���� GSHOP_ITEM �� id ���ƶ�
		bool				m_giftMoved;		//	�Ƿ��Ѵ���� GSHOP_ITEM �� idGift ���ƶ�

		Record();
		Record(const CECShopBase *pShop, int itemIndex, int buyIndex);

		bool MoveItem(int id);
		bool IsFinished()const;
	};
	typedef abase::vector<Record>	Records;
	Records	m_records;

	struct MoveCommand 
	{
		int		m_id;		//	��ͨ��������ƷID��������֤��
		int		m_slot;		//	��ͨ������λ�ã��ƶ���ƷЭ����Ҫ��
		int		m_iPack;	//	ϣ��Ҫ������ Pack���ƶ���ƷЭ����Ҫ��
		int		m_targetSlot;	//	Ŀ�����λ�ã�Ϊ-1������δ��ʼ�ƶ���
		DWORD	m_runTime;		//	ִ��ʱ��

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