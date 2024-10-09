// Filename	: DlgWikiTaskList.cpp
// Creator	: Feng Ning
// Date		: 2010/07/20

#include "DlgWikiTaskList.h"
#include "WikiSearchTask.h"
#include "DlgWikiTask.h"
#include "WikiItemProp.h"
#include "DlgTask.h"

#define new A_DEBUG_NEW

AUI_BEGIN_COMMAND_MAP(CDlgWikiTaskList, CDlgWikiItemListBase)
AUI_END_COMMAND_MAP()

AUI_BEGIN_EVENT_MAP(CDlgWikiTaskList, CDlgWikiItemListBase)
AUI_END_EVENT_MAP()

CDlgWikiTaskList::CDlgWikiTaskList()
{
}

bool CDlgWikiTaskList::OnInitDialog()
{
	SetContentProvider(&WikiTaskDataProvider());
	return CDlgWikiItemListBase::OnInitDialog();
}

void CDlgWikiTaskList::SetSearchDataPtr(void* ptr)
{
	// NOTICE: here for convenient we use
	// WikiSearchTaskByItemID as default command
	const WikiItemProp* pIP = (const WikiItemProp *)(ptr);
	SetSearchCommand(!pIP ? NULL : &WikiSearchTaskByItemID(pIP->GetID()));
	CDlgWikiItemListBase::SetSearchDataPtr(ptr);
}

bool CDlgWikiTaskList::OnModityItem(WikiEntityPtr p, PAUIOBJECT pObj, PAUIOBJECT pName)
{
	WikiTaskDataProvider::Entity* pEE = dynamic_cast<WikiTaskDataProvider::Entity*>(p.Get());
	ASSERT(pEE); // should always got the ptr
	if(!pEE) return false;

	ATaskTempl *pTempl = pEE->GetIterator()->second;

//	pObj->SetText(pTempl->GetName());
	pObj->SetText(CDlgTask::GetTaskNameWithColor(pTempl));
	pObj->Show(true);
	
//	BindLinkCommand(dynamic_cast<PAUITEXTAREA>(pObj), NULL,
//			&CDlgWikiTask::ShowSpecficLinkCommand(pTempl->GetID(), pTempl->GetName()));
	BindLinkCommand(dynamic_cast<PAUITEXTAREA>(pObj), NULL,
			&CDlgWikiTask::ShowSpecficLinkCommand(pTempl->GetID(), CDlgTask::GetTaskNameWithColor(pTempl)));

	pName->SetText(GetStringFromTable(8663));
	pName->Show(true);

	return true;
}
