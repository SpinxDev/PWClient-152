// Filename	: EC_ShoppingItemsMover.cpp
// Creator	: Xu Wenbin
// Date		: 2014/6/4

#include "EC_ShoppingItemsMover.h"
#include "EC_TimeSafeChecker.h"
#include "EC_Shop.h"
#include "EC_IvtrTypes.h"
#include "globaldataman.h"
#include "EC_IvtrItem.h"
#include "EC_Game.h"
#include "EC_GameRun.h"
#include "EC_GameSession.h"
#include "EC_HostPlayer.h"
#include "EC_Inventory.h"

#include <windows.h>

//	外置声明
extern CECGame *g_pGame;

//	class CECShoppingItemsMover::Record
CECShoppingItemsMover::Record::Record()
: m_pShop(NULL)
, m_itemIndex(-1)
, m_buyIndex(-1)
, m_beginTime(0)
, m_itemMoved(false)
, m_giftMoved(false)
{
}

CECShoppingItemsMover::Record::Record(const CECShopBase *pShop, int itemIndex, int buyIndex)
: m_pShop(pShop)
, m_itemIndex(itemIndex)
, m_buyIndex(buyIndex)
, m_itemMoved(false)
, m_giftMoved(false)
{
	m_beginTime = GetTickCount();
	const GSHOP_ITEM *pGShopItem = pShop->GetItem(itemIndex);
	if (pGShopItem->idGift <= 0 || pGShopItem->iGiftNum < 0){
		m_giftMoved = true;
	}
}

bool CECShoppingItemsMover::Record::MoveItem(int id){
	const GSHOP_ITEM *pGShopItem = m_pShop->GetItem(m_itemIndex);
	if (!m_itemMoved && id == pGShopItem->id){
		m_itemMoved = true;
		return true;
	}
	if (!m_giftMoved && id == pGShopItem->idGift){
		m_giftMoved = true;
		return true;
	}
	return false;
}

bool CECShoppingItemsMover::Record::IsFinished()const{
	return m_itemMoved && m_giftMoved;
}

//	MoveCommand
CECShoppingItemsMover::MoveCommand::MoveCommand()
: m_id(0)
, m_slot(-1)
, m_iPack(-1)
, m_targetSlot(-1)
, m_runTime(0)
{
}

CECShoppingItemsMover::MoveCommand::MoveCommand(int id, int slot, int pack)
: m_id(id)
, m_slot(slot)
, m_iPack(pack)
, m_targetSlot(-1)
, m_runTime(0)
{
}

void CECShoppingItemsMover::MoveCommand::OnExecute(int targetSlot){
	if (m_targetSlot >= 0){
		ASSERT(false);
		return;
	}
	m_targetSlot = targetSlot;
	m_runTime = GetTickCount();
}

//	class CECShoppingItemsMover
CECShoppingItemsMover::CECShoppingItemsMover()
{
}

CECShoppingItemsMover & CECShoppingItemsMover::Instance(){
	static CECShoppingItemsMover s_instance;
	return s_instance;
}

void CECShoppingItemsMover::Clear(){
	m_records.clear();
	m_commands.clear();
}

void CECShoppingItemsMover::Tick(){
	DWORD dwCurrent = GetTickCount();
	for (Records::iterator it = m_records.begin(); it != m_records.end();){
		Record &r = *it;
		if (CECTimeSafeChecker::ElapsedTime(dwCurrent, r.m_beginTime) >= 5000){
			it = m_records.erase(it);
		}else{
			++ it;
		}
	}
	for (MoveCommands::iterator it2 = m_commands.begin(); it2 != m_commands.end();){
		MoveCommand &c = *it2;
		//	已经发送移动命令的检查超时
		if (c.m_targetSlot >= 0){
			if (CECTimeSafeChecker::ElapsedTime(dwCurrent, c.m_runTime) < 1000){
				break;						//	继续等待，后续目标暂时不再处理
			}
			it2 = m_commands.erase(it2);	//	移动命令发出后已经超时（未收到 EXG_TRASHBOX_IVTR 协议），可能是移动失败
			continue;
		}
		//	测试和执行移动命令
		CECHostPlayer *pHost = g_pGame->GetGameRun()->GetHostPlayer();
		if (IVTRTYPE_TRASHBOX3 == c.m_iPack){
			if (pHost->GetAutoFashion()){	//	快捷换装时、时装包裹处于锁定状态，此时不移动
				it2 = m_commands.erase(it2);//	直接删除移动命令
				continue;					//	继续检查下一个
			}
		}
		CECInventory *pInventory = pHost->GetPack(c.m_iPack);
		int targetSlot = pInventory->SearchEmpty();
		if (targetSlot >= 0){
			c.OnExecute(targetSlot);
			g_pGame->GetGameSession()->c2s_CmdExgTrashBoxIvtrItem(c.m_iPack, targetSlot, c.m_slot);
			break;							//	每次移动一个物品，待移动协议返回确认后，再移动下一个，以确保移动成功
		}else{
			it2 = m_commands.erase(it2);	//	无法找到合适位置，不再尝试移动，直接删除移动命令
			continue;						//	继续检查下一个
		}
	}
}

bool CECShoppingItemsMover::OnItemExchanged(int where, int slot, const CECIvtrItem *pItem, int inventorySlot, const CECIvtrItem *pInventoryItem){
	bool bRet(false);
	if (pInventoryItem && !pItem){		//	普通包裹栏不为空，而目标包裹中物品为空，说明是从普通包裹移往目标包裹
		for (MoveCommands::iterator it2 = m_commands.begin(); it2 != m_commands.end(); ++ it2){
			MoveCommand &c = *it2;
			if (c.m_targetSlot >= 0 && c.m_iPack == where && c.m_targetSlot == slot){
				m_commands.erase(it2);	//	移动成功
				bRet = true;
				break;
			}
		}
	}
	return bRet;
}

void CECShoppingItemsMover::OnItemBuyed(const CECShopBase *pShop, int itemIndex, int buyIndex){
	if (!pShop || !pShop->IsValid(itemIndex, buyIndex)){
		ASSERT(false);
		return;
	}
	m_records.push_back(Record(pShop, itemIndex, buyIndex));
}

bool CECShoppingItemsMover::MoveItem(const CECIvtrItem* pItem, int where, int slot){
	if (!pItem || where != IVTRTYPE_PACK){
		return false;
	}
	for (Records::iterator it = m_records.begin(); it != m_records.end(); ++ it){
		Record &r = *it;
		if (r.MoveItem(pItem->GetTemplateID())){
			while (pItem->GetClassID() == CECIvtrItem::ICID_FASHION){
				CECHostPlayer *pHost = g_pGame->GetGameRun()->GetHostPlayer();
				CECInventory *pFashionTrashBox = pHost->GetPack(IVTRTYPE_TRASHBOX3);
				if (!pFashionTrashBox || pFashionTrashBox->GetSize() <= 0){
					break;	//	时装包裹没有开启
				}
				m_commands.push_back(MoveCommand(pItem->GetTemplateID(), slot, IVTRTYPE_TRASHBOX3));
				break;
			}
			if (r.IsFinished()){
				m_records.erase(it);
			}
			return true;
		}
	}
	return false;
}