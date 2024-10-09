// Filename	: DlgWikiAreaList.cpp
// Creator	: Feng Ning
// Date		: 2010/09/30

#include "DlgWikiAreaList.h"
#include "WikiSearchCommand.h"
#include "WikiGuideData.h"

#define new A_DEBUG_NEW

AUI_BEGIN_COMMAND_MAP(CDlgWikiAreaList, CDlgWikiItemListBase)
AUI_END_COMMAND_MAP()

AUI_BEGIN_EVENT_MAP(CDlgWikiAreaList, CDlgWikiItemListBase)
AUI_END_EVENT_MAP()

CDlgWikiAreaList::CDlgWikiAreaList()
{
}

bool CDlgWikiAreaList::OnInitDialog()
{
	SetContentProvider(&WikiSerialDataProvider(0, WikiGuideData().Area().size()));
	return CDlgWikiItemListBase::OnInitDialog();
}

bool CDlgWikiAreaList::OnModityItem(WikiEntityPtr p, PAUIOBJECT pObj, PAUIOBJECT pName)
{
	WikiSerialDataProvider::Entity* pEE = dynamic_cast<WikiSerialDataProvider::Entity*>(p.Get());
	ASSERT(pEE); // should always got the ptr
	if(!pEE) return false;

	const WikiGuideArea* pData = NULL;
	if(!pEE->GetConstData(pData))
	{
		return false;
	}
	
	A3DVECTOR3 pos;
	if(WikiGuideData().GetEntry(pData->ID, &pos))
	{
		BindLinkCommand(dynamic_cast<PAUITEXTAREA>(pName), NULL, 
			&CDlgNameLink::MoveToLinkCommand(pData->ID, pos, pData->Name));
	}
	else
	{
		pName->SetText(pData->Name);
	}
	pName->Show(true);

	pObj->SetText(pData->Desc);
	pObj->Show(true);

	return true;
}
