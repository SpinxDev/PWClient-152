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

//	��������
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
		//	�Ѿ������ƶ�����ļ�鳬ʱ
		if (c.m_targetSlot >= 0){
			if (CECTimeSafeChecker::ElapsedTime(dwCurrent, c.m_runTime) < 1000){
				break;						//	�����ȴ�������Ŀ����ʱ���ٴ���
			}
			it2 = m_commands.erase(it2);	//	�ƶ���������Ѿ���ʱ��δ�յ� EXG_TRASHBOX_IVTR Э�飩���������ƶ�ʧ��
			continue;
		}
		//	���Ժ�ִ���ƶ�����
		CECHostPlayer *pHost = g_pGame->GetGameRun()->GetHostPlayer();
		if (IVTRTYPE_TRASHBOX3 == c.m_iPack){
			if (pHost->GetAutoFashion()){	//	��ݻ�װʱ��ʱװ������������״̬����ʱ���ƶ�
				it2 = m_commands.erase(it2);//	ֱ��ɾ���ƶ�����
				continue;					//	���������һ��
			}
		}
		CECInventory *pInventory = pHost->GetPack(c.m_iPack);
		int targetSlot = pInventory->SearchEmpty();
		if (targetSlot >= 0){
			c.OnExecute(targetSlot);
			g_pGame->GetGameSession()->c2s_CmdExgTrashBoxIvtrItem(c.m_iPack, targetSlot, c.m_slot);
			break;							//	ÿ���ƶ�һ����Ʒ�����ƶ�Э�鷵��ȷ�Ϻ����ƶ���һ������ȷ���ƶ��ɹ�
		}else{
			it2 = m_commands.erase(it2);	//	�޷��ҵ�����λ�ã����ٳ����ƶ���ֱ��ɾ���ƶ�����
			continue;						//	���������һ��
		}
	}
}

bool CECShoppingItemsMover::OnItemExchanged(int where, int slot, const CECIvtrItem *pItem, int inventorySlot, const CECIvtrItem *pInventoryItem){
	bool bRet(false);
	if (pInventoryItem && !pItem){		//	��ͨ��������Ϊ�գ���Ŀ���������ƷΪ�գ�˵���Ǵ���ͨ��������Ŀ�����
		for (MoveCommands::iterator it2 = m_commands.begin(); it2 != m_commands.end(); ++ it2){
			MoveCommand &c = *it2;
			if (c.m_targetSlot >= 0 && c.m_iPack == where && c.m_targetSlot == slot){
				m_commands.erase(it2);	//	�ƶ��ɹ�
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
					break;	//	ʱװ����û�п���
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