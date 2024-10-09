// Filename	: DlgWikiMonsterDrop.h
// Creator	: Xu Wenbin
// Date		: 2010/04/07

#include "DlgWikiMonsterDrop.h"
#include "DlgWikiEquipment.h"
#include "DlgWikiItem.h"
#include "WikiSearchCommand.h"
#include "EC_IvtrItem.h"
#include "elementdataman.h"

#define new A_DEBUG_NEW

AUI_BEGIN_COMMAND_MAP(CDlgWikiMonsterDrop, CDlgWikiItemListBase)
AUI_END_COMMAND_MAP()

AUI_BEGIN_EVENT_MAP(CDlgWikiMonsterDrop, CDlgWikiItemListBase)
AUI_END_EVENT_MAP()

CDlgWikiMonsterDrop::CDlgWikiMonsterDrop()
{
}


void CDlgWikiMonsterDrop::SetSearchDataPtr(void* ptr)
{
	const MONSTER_ESSENCE *pEssence = (const MONSTER_ESSENCE *)(ptr);
	int maxDrop = !pEssence ? 0 : sizeof(pEssence->drop_matters)/sizeof(pEssence->drop_matters[0]);

	// search from 0 to maxDrop
	SetContentProvider(!pEssence ? NULL : &WikiSerialDataProvider(0, maxDrop));
	SetSearchCommand(!pEssence ? NULL : &WikiSearchDropItemOnMonster(pEssence));
	
	CDlgWikiBase::SetSearchDataPtr(ptr);
}

bool CDlgWikiMonsterDrop::OnModityItem(WikiEntityPtr p, PAUIOBJECT pObj, PAUIOBJECT pName)
{
	const MONSTER_ESSENCE *pEssence = (const MONSTER_ESSENCE *)(GetDataPtr());
	if(!pEssence) return false;
	
	WikiSerialDataProvider::Entity* pEE = dynamic_cast<WikiSerialDataProvider::Entity*>(p.Get());
	ASSERT(pEE); // should always got the ptr
	if(!pEE) return false;
	
	unsigned int iCandidate = pEE->GetID();
	
	unsigned int id = pEssence->drop_matters[iCandidate].id;
	if (!id) return false;
	
	CECIvtrItem *pItem = CECIvtrItem::CreateItem(id, 0, 1);
	if (!pItem) return false;

	if(pItem->GetClassID() == CECIvtrItem::ICID_ERRORITEM)
	{
		delete pItem;
		return false;
	}

	pObj->Show(true);
	if(pItem->IsEquipment())
	{
		BindLinkCommand(dynamic_cast<PAUITEXTAREA>(pObj), NULL,
			&CDlgWikiEquipment::ShowSpecficLinkCommand(id, pItem->GetName()));
	}
	else
	{
		BindLinkCommand(dynamic_cast<PAUITEXTAREA>(pObj), NULL,
			&CDlgWikiItem::ShowSpecficLinkCommand(id, pItem->GetName()));
	}
			
	pName->SetText(GetStringFromTable(8662));
	pName->Show(true);

	delete pItem;
	return true;
}
