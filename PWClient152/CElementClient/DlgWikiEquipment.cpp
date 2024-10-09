// Filename	: DlgWikiEquipment.cpp
// Creator	: Feng Ning
// Date		: 2010/04/13

#include "DlgWikiEquipment.h"
#include "DlgWikiRandomProperty.h"
#include "DlgWikiEquipmentDrop.h"
#include "DlgWikiRecipe.h"
#include "EC_Global.h"
#include "EC_Game.h"
#include "EC_GameRun.h"
#include "EC_UIManager.h"
#include "EC_GameUIMan.h"
#include "AUICTranslate.h"
#include "EC_IvtrEquip.h"
#include "AUIImagePicture.h" 
#include "WikiSearchCommand.h"
#include "WikiEquipmentProp.h"
#include "ExpTypes.h"

#define new A_DEBUG_NEW

AUI_BEGIN_COMMAND_MAP(CDlgWikiEquipment, CDlgWikiByNameBase)
AUI_ON_COMMAND("drop", OnCommand_Drop)
AUI_ON_COMMAND("randomproperty", OnCommand_RandomProperty)
AUI_END_COMMAND_MAP()

AUI_BEGIN_EVENT_MAP(CDlgWikiEquipment, CDlgWikiByNameBase)
AUI_ON_EVENT("List_Equipment",	WM_LBUTTONDOWN,	OnEventLButtonDown_List)
AUI_ON_EVENT("List_Equipment",	WM_KEYDOWN,		OnEventKeyDown_List)
AUI_ON_EVENT("Img_*",	WM_LBUTTONDBLCLK,	OnEventLButtonDBLCLK_Icon)
AUI_END_EVENT_MAP()

// =======================================================================
// Link Command
// =======================================================================
CDlgWikiEquipment::ShowSpecficLinkCommand::ShowSpecficLinkCommand(unsigned int item_id, const ACString& name)
:m_ItemID(item_id)
,m_Name(name)
{
}

CDlgNameLink::LinkCommand* CDlgWikiEquipment::ShowSpecficLinkCommand::Clone() const
{
	return new ShowSpecficLinkCommand(m_ItemID, m_Name);
}

bool CDlgWikiEquipment::ShowSpecficLinkCommand::operator()(P_AUITEXTAREA_NAME_LINK pLink)
{
	// goto search item
	CDlgWikiEquipment* pDlg = dynamic_cast<CDlgWikiEquipment*>(g_pGame->GetGameRun()->GetUIManager()->
		GetInGameUIMan()->GetDialog("Win_WikiEquipmentSearch"));
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

ACString CDlgWikiEquipment::ShowSpecficLinkCommand::GetLinkText() const
{
	ACString strLinkedName;
	
	strLinkedName.Format(_AL("^00FF00&%s&^FFFFFF"), m_Name);

	return strLinkedName;
}

// =======================================================================
CDlgWikiEquipment::CDlgWikiEquipment()
:m_pList(NULL)

,m_pBtn_Drop(NULL)
,m_pBtn_RandomProperty(NULL)
,m_pEP(NULL)
{
}
bool CDlgWikiEquipment::OnInitDialog()
{
	if (!CDlgWikiByNameBase::OnInitDialog())
		return false;

	DDX_Control("List_Equipment", m_pList);
	
	DDX_Control("Btn_Drop", m_pBtn_Drop);
	DDX_Control("Btn_RandomProperty", m_pBtn_RandomProperty);
	
	return true;
}

void CDlgWikiEquipment::ResetSearchCommand()
{
	CDlgWikiByNameBase::ResetSearchCommand();

	SetContentProvider(&WikiElementDataProvider(ID_SPACE_ESSENCE));
	SetSearchCommand(&WikiSearchEquipmentByName());
}

void CDlgWikiEquipment::OnBeginSearch()
{
 	m_pList->ResetContent();
 	OnEventLButtonDown_List(0, 0, m_pList);
}

bool CDlgWikiEquipment::OnAddSearch(WikiEntityPtr p)
{
	WikiElementDataProvider::Entity* pEE = dynamic_cast<WikiElementDataProvider::Entity*>(p.Get());
	ASSERT(pEE); // should always got the ptr
	if(!pEE) return false;

	unsigned int id = pEE->GetID();

	ASSERT(m_pList->GetCount() < GetPageSize());

	// get data from entity first
	WikiEquipmentProp* pResult = NULL;
	WikiEquipmentProp* pEP = NULL;
	if (!(pEP = pEE->GetData(pResult)))
	{
		pEP = WikiEquipmentProp::CreateProperty(id);
	}
	ASSERT(pEP);
	if (!pEP) return false;

	ACString strItem;	
	strItem.Format(_AL("%s\t%d"), pEP->GetName(), pEP->GetLevel());
	m_pList->AddString(strItem);
	m_pList->SetItemData(m_pList->GetCount()-1, id);
	
	// set hint info
	AUICTranslate trans;
	m_pList->SetItemHint(m_pList->GetCount()-1, trans.Translate(pEP->GetDesc()));
	
	if(!pResult) delete pEP;
	return true;
}

void CDlgWikiEquipment::OnEndSearch()
{
	CDlgWikiByNameBase::OnEndSearch();

	if (m_pList->GetCount() <= 0)
	{
		return;
	}

	m_pList->SetCurSel(0);

	OnEventLButtonDown_List(0, 0, m_pList);
}

ACString& CDlgWikiEquipment::GetItemName(unsigned int item_id, ACString& szTxt)
{
	szTxt.Empty();
	
	if(item_id > 0)
	{
		CECIvtrItem *pItem = CECIvtrItem::CreateItem(item_id, 0, 1);
		szTxt = pItem->GetName();
		delete pItem;
	}

	return szTxt;
}

ACString& CDlgWikiEquipment::GetAddonName(unsigned int item_id, unsigned int addon_id, ACString& szTxt)
{
	szTxt.Empty();
	
	AString szFile;
	CECIvtrItem* pItem = CECIvtrItem::CreateItem(item_id, 0, 1);
	if(pItem)
	{
		CECIvtrEquip* pEqu = dynamic_cast<CECIvtrEquip*>(pItem);
		if(pEqu) pEqu->FormatRefineData(addon_id, szTxt);
		delete pItem;
	}

	return szTxt;
}

void CDlgWikiEquipment::OnEventLButtonDown_List(WPARAM wParam, LPARAM lParam, AUIObject *pObj)
{
	// mapping to the properties, put them here to make the header clean.
	PAUILABEL pTxt_EquipmentName = 
		dynamic_cast<PAUILABEL>(GetDlgItem("Txt_EquipmentName"));
	PAUILABEL pTxt_Type = 
		dynamic_cast<PAUILABEL>(GetDlgItem("Txt_Type"));
	PAUILABEL pTxt_Gender = 
		dynamic_cast<PAUILABEL>(GetDlgItem("Txt_Gender"));
	PAUILABEL pTxt_Class = 
		dynamic_cast<PAUILABEL>(GetDlgItem("Txt_Class"));
	PAUILABEL pTxt_LevelRequire = 
		dynamic_cast<PAUILABEL>(GetDlgItem("Txt_LevelRequire"));
	PAUILABEL pTxt_LevelupAddon = 
		dynamic_cast<PAUILABEL>(GetDlgItem("Txt_LevelupAddon"));
	PAUITEXTAREA pTxt_Split = 
		dynamic_cast<PAUITEXTAREA>(GetDlgItem("Txt_Link_Split"));
	PAUILABEL pTxt_SplitNumber = 
		dynamic_cast<PAUILABEL>(GetDlgItem("Txt_SplitNumber"));
	PAUITEXTAREA pTxt_Damaged = 
		dynamic_cast<PAUITEXTAREA>(GetDlgItem("Txt_Link_Damaged"));
	PAUILABEL pTxt_DamagedNumber = 
		dynamic_cast<PAUILABEL>(GetDlgItem("Txt_DamagedNumber"));
	PAUIIMAGEPICTURE pIcon = 
		dynamic_cast<PAUIIMAGEPICTURE>(GetDlgItem("Img_Icon"));

	GetGameUIMan()->m_pDlgWikiEquipmentDrop->OnCommand_CANCEL("");
	GetGameUIMan()->m_pDlgWikiRandomProperty->OnCommand_CANCEL("");
	WikiEquipmentProp* pEP = GetSeletedProperty();
	if (pEP)
	{	
		// refresh the monster search window to search item drop
		GetGameUIMan()->m_pDlgWikiEquipmentDrop->SetSearchDataPtr((void *)pEP);
		m_pBtn_Drop->Enable(!GetGameUIMan()->m_pDlgWikiEquipmentDrop->TestEmpty());

		// refresh the random property window
		GetGameUIMan()->m_pDlgWikiRandomProperty->SetSearchDataPtr((void *)pEP);
		m_pBtn_RandomProperty->Enable(!GetGameUIMan()->m_pDlgWikiRandomProperty->TestEmpty());

		pTxt_EquipmentName->SetText(pEP->GetName());
		pTxt_Type->SetText(pEP->GetTypeName());

		ACString szTxt;

		pTxt_Gender->SetText(pEP->GetGender() < 0 || pEP->GetGender() > 1 ? 
			GetStringFromTable(8720) : GetStringFromTable(8650 + pEP->GetGender()));

		pTxt_Class->SetText(GetGameUIMan()->GetFormatClass(pEP->GetClass(), true));

		pTxt_LevelRequire->SetText(
			szTxt.Format(GetStringFromTable(8652), pEP->GetLevelRequire()));

		pTxt_LevelupAddon->SetText(pEP->GetLevelupAddon() ? 
			GetAddonName(pEP->GetID(), pEP->GetLevelupAddon(), szTxt) : GetStringFromTable(8653));

		// bind search command to recipe wiki
		if(pEP->GetSplit())
		{
			BindLinkCommand(pTxt_Split, NULL,
				&CDlgWikiRecipe::ShowSpecficLinkCommand(pEP->GetSplit(), GetItemName(pEP->GetSplit(), szTxt)));
		}
		else
		{
			pTxt_Split->SetText(GetStringFromTable(8654));
		}

		pTxt_SplitNumber->SetText(szTxt.Format(_AL("%d"), 
			pEP->GetSplit() ? pEP->GetSplitNumber() : 0));

		// bind search command to recipe wiki
		if(pEP->GetDamaged())
		{
			BindLinkCommand(pTxt_Damaged, NULL,
				&CDlgWikiRecipe::ShowSpecficLinkCommand(pEP->GetDamaged(), GetItemName(pEP->GetDamaged(), szTxt)));
		}
		else
		{
			pTxt_Damaged->SetText(GetStringFromTable(8655));
		}

		pTxt_DamagedNumber->SetText(szTxt.Format(_AL("%d"),
			pEP->GetDamaged() ? pEP->GetDamagedNumber() : 0));

		if(pEP->GetID()>0) {
			pIcon->SetCover( GetGameUIMan()->m_pA2DSpriteIcons[CECGameUIMan::ICONS_INVENTORY],
							 GetGameUIMan()->m_IconMap[CECGameUIMan::ICONS_INVENTORY][pEP->GetIconFile()]
							 );

			//AUICTranslate trans;
			//pIcon->SetHint( trans.Translate(pEP->GetDesc()) );
			CECIvtrItem* pItem = CECIvtrItem::CreateItem(pEP->GetID(), 0, 1);
			pItem->GetDetailDataFromLocal();
			pIcon->SetHint(GetGameUIMan()->GetItemDescHint(pItem, false, true));
			delete pItem;

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
		m_pBtn_RandomProperty->Enable(false);

		pTxt_EquipmentName->SetText(_AL(""));
		pTxt_Type->SetText(_AL(""));
		pTxt_Gender->SetText(_AL(""));
		pTxt_Class->SetText(_AL(""));
		pTxt_LevelRequire->SetText(_AL(""));
		pTxt_LevelupAddon->SetText(_AL(""));
		BindLinkCommand(pTxt_Split, NULL, NULL);
		pTxt_SplitNumber->SetText(_AL(""));
		BindLinkCommand(pTxt_Damaged, NULL, NULL);
		pTxt_DamagedNumber->SetText(_AL(""));
		pIcon->SetCover(NULL, -1);
		pIcon->SetHint(_AL(""));
		pIcon->SetData(0);
	}
}

void CDlgWikiEquipment::OnEventKeyDown_List(WPARAM wParam, LPARAM lParam, AUIObject *pObj)
{
	if (wParam == VK_UP ||
		wParam == VK_DOWN)
	{
		OnEventLButtonDown_List(0, 0, NULL);
	}
}



WikiEquipmentProp* CDlgWikiEquipment::GetSeletedProperty()
{
	WikiEquipmentProp* pEP = NULL;

	if (m_pList->GetCount() > 0)
	{		
		int nCurSel = m_pList->GetCurSel();
		if (nCurSel >= 0 && nCurSel < m_pList->GetCount())
		{
			unsigned int id = m_pList->GetItemData(nCurSel);
			pEP = (!m_pEP || m_pEP->GetID() != id) ? 
				WikiEquipmentProp::CreateProperty(id):m_pEP;
		}
	}

	if(m_pEP != pEP)
	{
		delete m_pEP;
		m_pEP=pEP;
	}

	return m_pEP;
}

void CDlgWikiEquipment::OnCommand_Drop(const char *szCommand)
{
	CDlgWikiEquipmentDrop* pDlg = GetGameUIMan()->m_pDlgWikiEquipmentDrop;
	pDlg->Show(!pDlg->IsShow());
}

void CDlgWikiEquipment::OnCommand_RandomProperty(const char *szCommand)
{
	CDlgWikiRandomProperty *pDlg = GetGameUIMan()->m_pDlgWikiRandomProperty;
	pDlg->Show(!pDlg->IsShow());
}

void CDlgWikiEquipment::OnCommandCancel()
{
	GetGameUIMan()->m_pDlgWikiRandomProperty->OnCommand_CANCEL("");
	GetGameUIMan()->m_pDlgWikiEquipmentDrop->OnCommand_CANCEL("");
	CDlgWikiByNameBase::OnCommandCancel();
}

bool CDlgWikiEquipment::Release(void)
{
	delete m_pEP;
	m_pEP=NULL;
	
	return CDlgWikiByNameBase::Release();
}

void CDlgWikiEquipment::OnEventLButtonDBLCLK_Icon(WPARAM wParam, LPARAM lParam, AUIObject *pObj)
{
	if(!pObj) return;
	
	DWORD id = pObj->GetData();
	if(id == 0) return;
	
	PAUILABEL pTxt_EquipmentName = 
		dynamic_cast<PAUILABEL>(GetDlgItem("Txt_EquipmentName"));
	CDlgWikiRecipe::ShowSpecficLinkCommand(id, pTxt_EquipmentName->GetText())(NULL);
}