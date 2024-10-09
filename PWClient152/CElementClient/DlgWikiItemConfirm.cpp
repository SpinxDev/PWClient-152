// Filename	: DlgWikiItemConfirm.cpp
// Creator	: Feng Ning
// Date		: 2010/07/21

#include "DlgWikiItemConfirm.h"
#include "WikiItemProp.h"
#include "AUICTranslate.h"
#include "EC_GameUIMan.h"
#include "AUIImagePicture.h" 
#include "ExpTypes.h"

#define new A_DEBUG_NEW

AUI_BEGIN_COMMAND_MAP(CDlgWikiItemConfirm, CDlgWikiItemListBase)
AUI_END_COMMAND_MAP()

AUI_BEGIN_EVENT_MAP(CDlgWikiItemConfirm, CDlgWikiItemListBase)
AUI_END_EVENT_MAP()

WikiItemProvider::WikiItemProvider(const ACHAR* pName)
:WikiElementDataProvider(ID_SPACE_ESSENCE)
,m_Name(pName)
{}

CDlgWikiItemConfirm::CDlgWikiItemConfirm()
:m_OneCmdOnly(false)
,m_pObj(NULL)
{
}

void CDlgWikiItemConfirm::SetItemProvider(WikiItemProvider* pData)
{
	SetContentProvider(pData);

	if(pData)
	{
		SetSearchCommand(&WikiSearchItemByName(false));
		// set pattern to name search command if existed
		WikiSearchByName* pCommand = dynamic_cast<WikiSearchByName*>(GetSearchCommand());
		pCommand->SetPattern(pData->GetName());
	}
}

void CDlgWikiItemConfirm::OnBeginSearch()
{
	CDlgWikiItemListBase::OnBeginSearch();

	if(!HavePrevPage())
	{
		m_OneCmdOnly = false;
		m_pObj = NULL;
	}
}

void CDlgWikiItemConfirm::OnEndSearch()
{
	CDlgWikiItemListBase::OnEndSearch();

	// execute the default command and clear result
	if(m_OneCmdOnly && m_pObj)
	{
		PAUITEXTAREA pText = dynamic_cast<PAUITEXTAREA>(m_pObj);
		CDlgNameLink::LinkCommand* pCmd = GetLinkCommand(pText, NULL);
		if(pCmd)
		{
			(*pCmd)(NULL);
			WikiSearcher::Clear();
		}
	}
}

bool CDlgWikiItemConfirm::OnModityItem(WikiEntityPtr p, PAUIOBJECT pObj, PAUIOBJECT pName)
{
	WikiItemProvider* pData = dynamic_cast<WikiItemProvider*>(GetContentProvider());
	if(!pData) return false;

	WikiItemProvider::Entity* pEE = dynamic_cast<WikiItemProvider::Entity*>(p.Get());
	ASSERT(pEE); // should always got the ptr
	if(!pEE) return false;
	
	CDlgNameLink::LinkCommand* pCmd = pData->CreateLinkCommand(pEE);
	if(!pCmd) return false;
	BindLinkCommand(dynamic_cast<PAUITEXTAREA>(pObj), NULL, pCmd);
	delete pCmd;

	WikiItemProp* pEP = NULL;
	if (!pEE->GetData(pEP)) return false;

	AUICTranslate trans;
	pObj->SetHint(trans.Translate(pEP->GetDesc()));
	pObj->Show(true);

	// check whether we only got 1 result
	if(m_pObj == NULL && !m_OneCmdOnly)
	{
		m_pObj = pObj;
		m_OneCmdOnly = true;
	}
	else if(m_OneCmdOnly)
	{
		m_OneCmdOnly = false;
	}

	PAUIIMAGEPICTURE pIcon = dynamic_cast<PAUIIMAGEPICTURE>(pName);
	if(pIcon)
	{
		pIcon->SetCover( GetGameUIMan()->m_pA2DSpriteIcons[CECGameUIMan::ICONS_INVENTORY],
			GetGameUIMan()->m_IconMap[CECGameUIMan::ICONS_INVENTORY][pEP->GetIconFile()]
		);
	}
	pName->SetText(_AL(""));
	pName->SetHint(pObj->GetHint());
	pName->Show(true);

	return true;
}
