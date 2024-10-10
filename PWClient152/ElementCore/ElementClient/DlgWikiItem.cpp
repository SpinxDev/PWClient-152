// Filename	: DlgWikiItem.cpp
// Creator	: Feng Ning
// Date		: 2010/07/20

#include "DlgWikiItem.h"
#include "DlgWikiMonsterList.h"
#include "DlgWikiTaskList.h"
#include "DlgWikiRecipe.h"

#include "EC_Global.h"
#include "EC_Game.h"
#include "EC_GameRun.h"
#include "EC_UIManager.h"
#include "EC_GameUIMan.h"

#include "AUICTranslate.h"
#include "AUIImagePicture.h"

#include "WikiItemProp.h"
#include "WikiSearchCommand.h"
#include "ExpTypes.h"

#define new A_DEBUG_NEW

AUI_BEGIN_COMMAND_MAP(CDlgWikiItem, CDlgWikiByNameBase)
AUI_ON_COMMAND("drop", OnCommand_Drop)
AUI_ON_COMMAND("task", OnCommand_Task)
AUI_END_COMMAND_MAP()

AUI_BEGIN_EVENT_MAP(CDlgWikiItem, CDlgWikiByNameBase)
AUI_ON_EVENT("List_Item",	WM_LBUTTONDOWN,	OnEventLButtonDown_List)
AUI_ON_EVENT("List_Item",	WM_KEYDOWN,		OnEventKeyDown_List)
AUI_ON_EVENT("Img_*",	WM_LBUTTONDBLCLK,	OnEventLButtonDBLCLK_Icon)
AUI_END_EVENT_MAP()

// =======================================================================
// Link Command
// =======================================================================
CDlgWikiItem::ShowSpecficLinkCommand::ShowSpecficLinkCommand(unsigned int item_id, const ACString& name)
:m_ItemID(item_id)
,m_Name(name)
{
}

CDlgNameLink::LinkCommand* CDlgWikiItem::ShowSpecficLinkCommand::Clone() const
{
	return new ShowSpecficLinkCommand(m_ItemID, m_Name);
}

bool CDlgWikiItem::ShowSpecficLinkCommand::operator()(P_AUITEXTAREA_NAME_LINK pLink)
{
	// goto search item
	CDlgWikiItem* pDlg = dynamic_cast<CDlgWikiItem*>(g_pGame->GetGameRun()->GetUIManager()->
		GetInGameUIMan()->GetDialog("Win_WikiItemSearch"));
	ASSERT(pDlg); // should always get this dialog
	if(pDlg) 
	{
		// ensure the provider is correct
		pDlg->SetContentProvider(&WikiElementDataProvider(ID_SPACE_ESSENCE));
		pDlg->RefreshByNewCommand(&WikiSearchBySpecificID(m_ItemID), true);
		return true;
	}

	return false;
}

ACString CDlgWikiItem::ShowSpecficLinkCommand::GetLinkText() const
{
	ACString strLinkedName;
	
	strLinkedName.Format(_AL("^00FF00&%s&^FFFFFF"), m_Name);

	return strLinkedName;
}

// =======================================================================
CDlgWikiItem::CDlgWikiItem()
:m_pList(NULL)

,m_pBtn_Drop(NULL)
,m_pBtn_Task(NULL)
,m_pDlgDrop(NULL)
,m_pDlgTask(NULL)
,m_pEP(NULL)
{
}

bool CDlgWikiItem::OnInitDialog()
{
	if (!CDlgWikiByNameBase::OnInitDialog())
		return false;

	DDX_Control("List_Item", m_pList);
	
	DDX_Control("Btn_Drop", m_pBtn_Drop);
	DDX_Control("Btn_Task", m_pBtn_Task);
	
	m_pDlgDrop = dynamic_cast<CDlgWikiBase*>(GetGameUIMan()->GetDialog("Win_WikiItemDrop"));
	m_pDlgTask = dynamic_cast<CDlgWikiBase*>(GetGameUIMan()->GetDialog("Win_WikiItemTask"));
	return true;
}

void CDlgWikiItem::ResetSearchCommand()
{
	CDlgWikiByNameBase::ResetSearchCommand();
	SetContentProvider(&WikiElementDataProvider(ID_SPACE_ESSENCE));
	SetSearchCommand(&WikiSearchItemByName(true));
}

void CDlgWikiItem::OnBeginSearch()
{
 	m_pList->ResetContent();
 	OnEventLButtonDown_List(0, 0, m_pList);
}

bool CDlgWikiItem::OnAddSearch(WikiEntityPtr p)
{
	WikiElementDataProvider::Entity* pEE = dynamic_cast<WikiElementDataProvider::Entity*>(p.Get());
	ASSERT(pEE); // should always got the ptr
	if(!pEE) return false;
	
	unsigned int id = pEE->GetID();
	
	ASSERT(m_pList->GetCount() < GetPageSize());
	
	// get data from entity first
	WikiItemProp* pResult = NULL;
	WikiItemProp* pEP = NULL;
	if (!(pEP = pEE->GetData(pResult)))
	{
		pEP = WikiItemProp::CreateProperty(id);
	}
	ASSERT(pEP);
	if (!pEP) return false;
	
	ACString strItem;	
	strItem.Format(_AL("%s"), pEP->GetName());
	m_pList->AddString(strItem);
	m_pList->SetItemData(m_pList->GetCount()-1, id);
	
	// set hint info
	AUICTranslate trans;
	m_pList->SetItemHint(m_pList->GetCount()-1, trans.Translate(pEP->GetDesc()));
	
	if(!pResult) delete pEP;
	return true;
}

void CDlgWikiItem::OnEndSearch()
{
	CDlgWikiByNameBase::OnEndSearch();

	if (m_pList->GetCount() <= 0)
	{
		return;
	}

	m_pList->SetCurSel(0);

	OnEventLButtonDown_List(0, 0, m_pList);
}

WikiItemProp* CDlgWikiItem::GetSeletedProperty()
{
	if(m_pEP)
	{
		delete m_pEP;
		m_pEP=NULL;
	}

	if (m_pList->GetCount() > 0)
	{		
		int nCurSel = m_pList->GetCurSel();
		if (nCurSel >= 0 && nCurSel < m_pList->GetCount())
		{
			unsigned int id = m_pList->GetItemData(nCurSel);
			m_pEP = WikiItemProp::CreateProperty(id);
		}
	}
	
	return m_pEP;
}

void CDlgWikiItem::OnEventLButtonDown_List(WPARAM wParam, LPARAM lParam, AUIObject *pObj)
{
	// mapping to the properties, put them here to make the header clean.
	PAUILABEL pTxt_ItemName = 
		dynamic_cast<PAUILABEL>(GetDlgItem("Txt_ItemName"));
	PAUILABEL pTxt_Type = 
		dynamic_cast<PAUILABEL>(GetDlgItem("Txt_Type"));
	
	PAUIIMAGEPICTURE pIcon = 
		dynamic_cast<PAUIIMAGEPICTURE>(GetDlgItem("Img_Icon"));

	if(m_pDlgDrop) m_pDlgDrop->OnCommand_CANCEL("");
	if(m_pDlgTask) m_pDlgTask->OnCommand_CANCEL("");

	WikiItemProp* pEP = GetSeletedProperty();
	if (pEP)
	{	
		// refresh the monster search window to search item drop
		if(m_pDlgDrop)
		{
			m_pDlgDrop->SetSearchDataPtr((void *)pEP);
			m_pBtn_Drop->Enable(!m_pDlgDrop->TestEmpty());
		}

		// refresh the random property window
		if(m_pDlgTask)
		{
			m_pDlgTask->SetSearchDataPtr((void *)pEP);
			m_pBtn_Task->Enable(!m_pDlgTask->TestEmpty());
		}

		pTxt_ItemName->SetText(pEP->GetName());
		pTxt_Type->SetText(pEP->GetTypeName());

		if(pEP->GetID()>0) {
			pIcon->SetCover( GetGameUIMan()->m_pA2DSpriteIcons[CECGameUIMan::ICONS_INVENTORY],
							 GetGameUIMan()->m_IconMap[CECGameUIMan::ICONS_INVENTORY][pEP->GetIconFile()]
							 );
			AUICTranslate trans;
			pIcon->SetHint( trans.Translate(pEP->GetDesc()) );
			pIcon->SetData(pEP->GetID());
		} else {
			pIcon->SetCover(NULL, -1);
			pIcon->SetHint(_AL(""));
			pIcon->SetData(0);
		}
	}
	else
	{
 		m_pBtn_Drop->Enable(false);
 		m_pBtn_Task->Enable(false);

		pTxt_ItemName->SetText(_AL(""));
		pTxt_Type->SetText(_AL(""));
		pIcon->SetCover(NULL, -1);
		pIcon->SetHint(_AL(""));
		pIcon->SetData(0);
	}
}

void CDlgWikiItem::OnEventKeyDown_List(WPARAM wParam, LPARAM lParam, AUIObject *pObj)
{
	if (wParam == VK_UP ||
		wParam == VK_DOWN)
	{
		OnEventLButtonDown_List(0, 0, NULL);
	}
}


void CDlgWikiItem::OnCommand_Drop(const char *szCommand)
{
	// goto search monster
	if(m_pDlgDrop) m_pDlgDrop->Show(!m_pDlgDrop->IsShow());
}

void CDlgWikiItem::OnCommand_Task(const char *szCommand)
{
	// goto search task
	if(m_pDlgTask) m_pDlgTask->Show(!m_pDlgTask->IsShow());
}

void CDlgWikiItem::OnCommandCancel()
{
	if(m_pDlgTask) m_pDlgDrop->OnCommand_CANCEL("");
	if(m_pDlgTask) m_pDlgTask->OnCommand_CANCEL("");

	CDlgWikiByNameBase::OnCommandCancel();
}

bool CDlgWikiItem::Release(void)
{
	if(m_pEP)
	{
		delete m_pEP;
		m_pEP=NULL;
	}

	return CDlgWikiByNameBase::Release();
}

void CDlgWikiItem::OnEventLButtonDBLCLK_Icon(WPARAM wParam, LPARAM lParam, AUIObject * pObj)
{
	if(!pObj) return;
	
	DWORD id = pObj->GetData();
	if(id == 0) return;
	
	PAUILABEL pTxt_ItemName = 
		dynamic_cast<PAUILABEL>(GetDlgItem("Txt_ItemName"));
	CDlgWikiRecipe::ShowSpecficLinkCommand(id, pTxt_ItemName->GetText())(NULL);
}