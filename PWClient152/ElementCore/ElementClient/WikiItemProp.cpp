// Filename	: WikiItemProp.cpp
// Creator	: Feng Ning
// Date		: 2010/07/20

#include "WikiItemProp.h"
#include "EC_Global.h"
#include "EC_Game.h"
#include "EC_GameRun.h"
#include "EC_UIManager.h"
#include "EC_GameUIMan.h"
#include "EC_IvtrItem.h"
#include "AMemory.h"
#include "AFI.h"

#define new A_DEBUG_NEW

// =======================================================================
// Data Extractor
// =======================================================================
class ItemExtractor : public WikiItemProp
{
	friend WikiItemProp;

private:
	const CECIvtrItem* m_pItem;

	ItemExtractor(CECIvtrItem* pItem):m_pItem(pItem)
	{
		ASSERT(m_pItem != NULL);
	}

public:
	~ItemExtractor()
	{
		delete m_pItem;
		m_pItem = NULL;
	}

	virtual ACString GetName() const 
	{
		return const_cast<CECIvtrItem*>(m_pItem)->GetName();
	};

	virtual ACString GetTypeName() const
	{
		CECGameUIMan* pGameUI = g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan();
		return pGameUI->GetStringFromTable(9000 + m_pItem->GetClassID());
	}
	
	virtual ACString GetDesc() const 
	{
		return const_cast<CECIvtrItem*>(m_pItem)->GetDesc();
	}

	virtual AString GetIconFile() const
	{
		AString szFile;
		af_GetFileTitle(const_cast<CECIvtrItem*>(m_pItem)->GetIconFile(), szFile);
		szFile.MakeLower();
		return szFile;
	}

	virtual unsigned int GetID() const
	{
		return m_pItem->GetTemplateID();
	};
};

WikiItemProp* WikiItemProp::CreateProperty(unsigned int id, bool noEquip)
{
	CECIvtrItem* pItem = CECIvtrItem::CreateItem(id, 0, 1);
	if(!pItem)
	{
		return NULL;
	}

	int cid = pItem->GetClassID();
	if( CECIvtrItem::ICID_ITEM == cid ||
		(noEquip && pItem->IsEquipment()) ||
		CECIvtrItem::ICID_EQUIP == cid ||
		CECIvtrItem::ICID_TOSSMAT == cid ||
		CECIvtrItem::ICID_UNIONSCROLL == cid ||
		CECIvtrItem::ICID_ERRORITEM == cid ||
		CECIvtrItem::ICID_GM_GENERATOR == cid ||
		CECIvtrItem::ICID_RECIPE == cid ||
		CECIvtrItem::ICID_PETFACETICKET == cid ||
		CECIvtrItem::ICID_TANKCALLIN == cid ||
		CECIvtrItem::ICID_DESTROYINGESSENCE == cid ||
		cid < 0 )
	{
		delete pItem;
		return NULL;
	}

	// filter the item without a icon
	AString szFile;
	af_GetFileTitle(pItem->GetIconFile(), szFile);
	szFile.MakeLower();
	CECGameUIMan* pGameUI = g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan();
	if(pGameUI->GetIconIndex(CECGameUIMan::ICONS_INVENTORY, szFile) == 0)
	{
		delete pItem;
		return NULL;
	}
	
	pItem->GetDetailDataFromLocal();
	return new ItemExtractor(pItem);
}