// Filename	: DlgWikiRandomProperty.cpp
// Creator	: Feng Ning
// Date		: 2010/04/15

#include "DlgWikiRandomProperty.h"
#include "EC_IvtrEquip.h"
#include "WikiSearchCommand.h"
#include "WikiEquipmentProp.h"

#define new A_DEBUG_NEW

AUI_BEGIN_COMMAND_MAP(CDlgWikiRandomProperty, CDlgWikiItemListBase)
AUI_END_COMMAND_MAP()

AUI_BEGIN_EVENT_MAP(CDlgWikiRandomProperty, CDlgWikiItemListBase)
AUI_END_EVENT_MAP()

CDlgWikiRandomProperty::CDlgWikiRandomProperty()
{
}

void CDlgWikiRandomProperty::SetSearchDataPtr(void* ptr)
{
	const WikiEquipmentProp *pEP = (const WikiEquipmentProp *)(ptr);

	SetContentProvider(!pEP ? NULL : &WikiSerialDataProvider(0, pEP->GetRandPropSize()));
	SetSearchCommand(!pEP ? NULL : &WikiSearchRandPropOnEquipment(pEP->GetID()));

	CDlgWikiBase::SetSearchDataPtr(ptr);
}

bool CDlgWikiRandomProperty::OnModityItem(WikiEntityPtr p, PAUIOBJECT pObj, PAUIOBJECT pName)
{
	const WikiEquipmentProp *pEP = (const WikiEquipmentProp *)(GetDataPtr());
	if(!pEP) return false;

	WikiSerialDataProvider::Entity* pEE = dynamic_cast<WikiSerialDataProvider::Entity*>(p.Get());
	ASSERT(pEE); // should always got the ptr
	if(!pEE) return false;

	unsigned int iCandidate = pEE->GetID();
	
	ACString szTxt;
	unsigned int rand_id = pEP->GetRandPropID(iCandidate);
	if (!rand_id || GetRandomPropName(pEP->GetID(), rand_id, szTxt).IsEmpty())
	{	
		return false;
	}
	
	pObj->Show(true);
	pObj->SetText(szTxt);
	
	pName->SetText(pEP->GetRandPropName(iCandidate));
	pName->Show(true);
	return true;
}

ACString& CDlgWikiRandomProperty::GetRandomPropName(unsigned int item_id, unsigned int id_rand, ACString& szTxt)
{	
	szTxt.Empty();
	
	AString szFile;
	CECIvtrItem* pItem = CECIvtrItem::CreateItem(item_id, 0, 1);
	if(pItem)
	{
		CECIvtrEquip* pEqu = dynamic_cast<CECIvtrEquip*>(pItem);
		if(pEqu) pEqu->FormatRefineData(id_rand, szTxt);
		delete pItem;
	}
	
	return szTxt;
}