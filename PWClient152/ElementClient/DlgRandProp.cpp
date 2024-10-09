// Filename	: DlgRandProp.cpp
// Creator	: Feng Ning
// Date		: 2011/06/15

#include "DlgRandProp.h"
#include "EC_ShortcutMgr.h"
#include "AUIImagePicture.h"
#include "AUIStillImageButton.h"
#include "AUIProgress.h"
#include "EC_Global.h"
#include "EC_Game.h"
#include "EC_GameSession.h"
#include "EC_GameUIMan.h"
#include "EC_Inventory.h"
#include "EC_HostPlayer.h"
#include "EC_IvtrEquip.h"
#include "AUICTranslate.h"
#include "elementdataman.h"
#include "WikiEquipmentProp.h"
#include "EC_FixedMsg.h"
#include "AFI.h"

#define new A_DEBUG_NEW

#define PROP_COLOR_INVALID		A3DCOLORRGB(128, 128, 128)
#define PROP_COLOR_NOTENOUGH	A3DCOLORRGB(192, 0, 0)
#define PROP_COLOR_NORMAL		A3DCOLORRGB(255, 255, 255)
#define PROP_COLOR_SELECTED		A3DCOLORRGB(255, 255, 0)


AUI_BEGIN_COMMAND_MAP(CDlgRandProp, CDlgBase)

AUI_ON_COMMAND("set*",			OnCommand_set)
AUI_ON_COMMAND("begin",			OnCommand_begin)
AUI_ON_COMMAND("IDCANCEL",		OnCommand_CANCEL)

AUI_END_COMMAND_MAP()

AUI_BEGIN_EVENT_MAP(CDlgRandProp, CDlgBase)

AUI_ON_EVENT("Item_Target",			WM_LBUTTONUP,	OnEventLButtonUp_Target)

AUI_END_EVENT_MAP()

//------------------------------------------------------------------------
// Right-Click Shortcut for CDlgRandProp
//------------------------------------------------------------------------
typedef CECShortcutMgr::SimpleClickShortcut<CDlgRandProp> RandPropClickShortcut;
//------------------------------------------------------------------------

CDlgRandProp::CDlgRandProp()
{
	m_nCurIcon = 0;	// starts from 1
	m_nCurShopSet = 0;	// starts from 1

	m_RegenCount = 0;
	memset(m_nSoltMaterail, 0, sizeof(m_nSoltMaterail));
}

CDlgRandProp::~CDlgRandProp()
{

}

bool CDlgRandProp::OnInitDialog()
{
	GetGameUIMan()->GetShortcutMgr()->RegisterShortCut(new RandPropClickShortcut(this));
	return CDlgBase::OnInitDialog();
}

void CDlgRandProp::OnShowDialog()
{
	RefreshAll();
}

void CDlgRandProp::OnTick()
{
	PAUIOBJECT pBtn = GetDlgItem("Btn_Produce");
	if(!CanBegin() && pBtn) pBtn->Enable(false);
}

void CDlgRandProp::OnCommand_set(const char * szCommand)
{
	RefreshShopSet(atoi(szCommand + strlen("set")), 1);
}

int CDlgRandProp::GetCurRecipe()
{
	// fetch the correct page
	AString strName;
	PAUIOBJECT pButton = GetDlgItem(strName.Format("Btn_Set%d", m_nCurShopSet));
	if(pButton)
	{
		int page = (int)pButton->GetData();
		
		NPC_RANDPROP_SERVICE *pService = (NPC_RANDPROP_SERVICE *)GetDataPtr("ptr_NPC_RANDPROP_SERVICE");
		const int pageSize = sizeof(pService->pages) / sizeof(pService->pages[0]);
		
		if(pService && page >= 0 && page < pageSize )
		{
			return pService->pages[page].id_recipe;
		}
	}

	return 0;
}

void CDlgRandProp::OnCommand_begin(const char * szCommand)
{
	if(CanBegin())
	{
		PAUIOBJECT pBtn = GetDlgItem("Btn_Produce");
		if(pBtn) pBtn->Enable(false);

		PAUIIMAGEPICTURE pObj = (PAUIIMAGEPICTURE)GetDlgItem("Item_Target");
		int ivtr_index = (int)pObj->GetData();
		int id_recipe = GetCurRecipe();
		GetGameSession()->c2s_CmdNPCSevAddonRegen(id_recipe, ivtr_index);
	}
	else
	{
		GetGameUIMan()->AddChatMessage(GetStringFromTable(9351), GP_CHAT_MISC);
	}	
}

void CDlgRandProp::OnCommand_CANCEL(const char * szCommand)
{
	if( IsWorking() )
	{
		GetGameSession()->c2s_CmdCancelAction();
	}
	else
	{		     
		Show(false);
		GetGameUIMan()->GetDialog("Win_Inventory")->Show(false);
		GetGameUIMan()->EndNPCService();
		SetTarget(-1);
	}
}

bool CDlgRandProp::OnRandPropAction(int idAction, void* pData)
{
	using namespace S2C;
	PAUIPROGRESS pBar = dynamic_cast<PAUIPROGRESS>(GetDlgItem("Prgs_1"));

	switch(idAction)
	{
	case ADDONREGEN_START:
	{
		cmd_addonregen_start* pCmd = (cmd_addonregen_start*)pData;
		if(pCmd && pBar)
		{
			DWORD dwMSecs = pCmd->use_time * 50;
			pBar->SetData(dwMSecs);
			pBar->SetAutoProgress(dwMSecs, 0, AUIPROGRESS_MAX);
		}
		break;
	}
	case ADDONREGEN_END:
	{
		if(pBar)
		{
			pBar->SetData(0);
			pBar->SetAutoProgress(0, 0, 0);
		}
		break;
	}
	case ADDONREGEN_RESULT:
	{
		cmd_addonregen_result* pCmd = (cmd_addonregen_result*)pData;
		if(!pCmd || pCmd->addon_num <= 0)
		{
			GetGameUIMan()->AddChatMessage(GetStringFromTable(9350), GP_CHAT_MISC);
		}
		else
		{
			PAUIIMAGEPICTURE pObj = (PAUIIMAGEPICTURE)GetDlgItem("Item_Target");
			if(pObj)
			{
				CECIvtrEquip* pEqu = (CECIvtrEquip*)pObj->GetDataPtr();
				if(pEqu)
				{
					ACString strMsg;
					strMsg.Format(GetStringFromTable(9356), pEqu->GetName());
					GetGameUIMan()->AddChatMessage(strMsg, GP_CHAT_MISC);

					m_RegenCount ++;
					m_RegenDesc = pEqu->GetDesc();

					// refresh target
					SetTarget((int)pObj->GetData());
				}
			}
		}
		break;
	}
		
	default:
		a_LogOutput(1, "CDlgRandProp::OnRandPropAction(), Invalid action id %d", idAction);
		ASSERT(false);
		return false;
	}
	
	PAUIOBJECT pBtn = GetDlgItem("Btn_Produce");
	if(pBtn) pBtn->Enable(CanBegin());

	return true;
}

void CDlgRandProp::RefreshAll()
{
	RefreshButtonName();
	
	// force refresh shop set
	m_nCurShopSet = 0;
	m_nCurIcon = 0;
	RefreshShopSet(1, 1);

	SetTarget(-1);
}

bool CDlgRandProp::RefreshMaterial(int recipeID)
{
	bool ret = true;

	elementdataman *pDataMan = g_pGame->GetElementDataMan();
	DATA_TYPE dt = DT_INVALID;
	RANDPROP_ESSENCE *pRecipe = recipeID <= 0 ? 0 : (RANDPROP_ESSENCE *)pDataMan->get_data_ptr(recipeID, ID_SPACE_RECIPE, dt);

	PAUIOBJECT pGold = GetDlgItem("Txt_Gold");
	if(pGold)
	{
		pGold->SetText(pRecipe ? GetGameUIMan()->GetFormatNumber(pRecipe->money) : _AL(""));
	}

	int nIndex = 0;
	for(int j = 0; j < CDLGRANDPROP_MAX_MATERIALS; j++ )
	{
		char szName[20];
		sprintf(szName, "Item_%02d", nIndex + 1);
		PAUIIMAGEPICTURE pObj = (PAUIIMAGEPICTURE)GetDlgItem(szName);
		if( !pObj ) break;
		
		sprintf(szName, "Txt_no%d", nIndex + 1);
		
		int itemID = pRecipe ? pRecipe->materials[j].id : 0;
		CECIvtrItem *pItem = itemID ? CECIvtrItem::CreateItem(itemID, 0, 1) : NULL;
		if(pItem)
		{
			// set the valid material info
			if(m_nSoltMaterail[j] != pItem->GetTemplateID())
			{
				AString strFile;
				af_GetFileTitle(pItem->GetIconFile(), strFile);
				strFile.MakeLower();
				pObj->SetCover( GetGameUIMan()->GetIconCover(CECGameUIMan::ICONS_INVENTORY),
					GetGameUIMan()->GetIconIndex(CECGameUIMan::ICONS_INVENTORY, strFile) );

				AUICTranslate trans;
				pObj->SetHint(trans.Translate(pItem->GetDesc()));
				
				m_nSoltMaterail[j] = pItem->GetTemplateID();
			}
			
			// check the item count
			int curCount = GetGameUIMan()->GetItemCount(pItem->GetTemplateID());
			int needCount = pRecipe->materials[j].num;

			ret &= (curCount >= needCount);
			pObj->SetColor( curCount >= needCount ? PROP_COLOR_NORMAL : PROP_COLOR_NOTENOUGH );
			
			ACString strNum;
			GetDlgItem(szName)->SetText(strNum.Format(_AL("%d/%d"), curCount, needCount));
			
			delete pItem;
		}
		else
		{
			// clear the material info
			pObj->ClearCover();
			pObj->SetHint(_AL(""));
			pObj->SetColor(PROP_COLOR_INVALID);

			GetDlgItem(szName)->SetText(_AL(""));

			m_nSoltMaterail[j] = 0;
		}
		
		nIndex++;
	}

	return ret;
}

int CDlgRandProp::RefreshButtonName()
{
	char szItem[40];
	int i, nSetIndex = 1;

	for( i = 0; i < NUM_NPCIVTR; i++ )
	{
		CECNPCInventory *pDeal = GetHostPlayer()->GetNPCSevPack(i);
		if( a_strlen(pDeal->GetName()) <= 0 || pDeal->GetSize() == 0 )
		{
			// skip invalid inventory
			continue;
		}
		
		sprintf(szItem, "Btn_Set%d", nSetIndex);
		PAUIOBJECT pBtn = GetDlgItem(szItem);
		if(!pBtn) break;

		pBtn->SetText(pDeal->GetName());
		pBtn->SetData((DWORD)i);
		pBtn->Show(true);
		nSetIndex++;
	}

	while(true)
	{
		sprintf(szItem, "Btn_Set%d", nSetIndex++);
		PAUIOBJECT pBtn = GetDlgItem(szItem);
		if(!pBtn) break;
		pBtn->Show(false);
	}

	return nSetIndex;
}

void CDlgRandProp::RefreshShopSet(int nSet, int nIcon)
{
	if(m_nCurShopSet != nSet)
	{
		// update button status
		char szItem[40];
		
		PAUISTILLIMAGEBUTTON pButton;
		sprintf(szItem, "Btn_Set%d", m_nCurShopSet);
		pButton = (PAUISTILLIMAGEBUTTON)GetDlgItem(szItem);
		if(pButton) pButton->SetPushed(false);
		
		sprintf(szItem, "Btn_Set%d", nSet);
		pButton = (PAUISTILLIMAGEBUTTON)GetDlgItem(szItem);
		if(pButton) pButton->SetPushed(true);

		// refresh current recipe content
		RefreshContent(pButton ? pButton->GetData() : NULL);

		m_nCurShopSet = nSet;

		m_nCurIcon = 0;
	}

	if(m_nCurIcon != nIcon)
	{
		// handle icon selection

		m_nCurIcon = nIcon;
	}
	
	return;
}

void CDlgRandProp::RefreshContent(int page)
{
	// update page content
	char szItem[40];
	CECNPCInventory *pDeal = GetHostPlayer()->GetNPCSevPack(page);
	for(int i = 0; i < IVTRSIZE_NPCPACK; i++ )
	{
		sprintf(szItem, "U_%02d", i + 1);
		PAUIIMAGEPICTURE pObj = (PAUIIMAGEPICTURE)GetDlgItem(szItem);
		if( !pObj ) continue;
		
		CECIvtrItem* pItem = (CECIvtrItem *)pDeal->GetItem(i);
		if( pItem )
		{
			pObj->SetData(pItem->GetTemplateID());
			pObj->SetDataPtr(pItem,"ptr_CECIvtrItem");
			pObj->SetColor(PROP_COLOR_NORMAL);
			
			AString strFile;
			af_GetFileTitle(pItem->GetIconFile(), strFile);
			strFile.MakeLower();
			pObj->SetCover( GetGameUIMan()->GetIconCover(CECGameUIMan::ICONS_INVENTORY),
				GetGameUIMan()->GetIconIndex(CECGameUIMan::ICONS_INVENTORY, strFile) );

			AUICTranslate trans;
			pObj->SetHint(trans.Translate(pItem->GetDesc()));
		}
		else
		{
			pObj->SetData(0);
			pObj->SetCover(NULL, -1);
			pObj->SetDataPtr(NULL);
			pObj->SetHint(_AL(""));
		}
		
	}

	// update material for current content
	NPC_RANDPROP_SERVICE *pService = (NPC_RANDPROP_SERVICE *)GetDataPtr("ptr_NPC_RANDPROP_SERVICE");
	RefreshMaterial(pService ? pService->pages[page].id_recipe : 0);
}

bool CDlgRandProp::SaveProps(CECIvtrItem* pItem)
{
	ASSERT(pItem);
	if(!pItem) return false;

	// check item type
	CECIvtrEquip* pEqu = dynamic_cast<CECIvtrEquip*>(pItem);
	if(!pEqu)
	{
		GetGameUIMan()->AddChatMessage(GetStringFromTable(9353), GP_CHAT_MISC);
		return false;
	}
	
	// get all valid rand prop (generated properties) id form db essence
	typedef abase::hash_map<int, bool>	RandPropPool;
	RandPropPool randPropPool;

	WikiEquipmentProp* pEquProp = WikiEquipmentProp::CreateProperty(pItem->GetTemplateID());
	if(pEquProp) // some equip didn't have prop
	{
		int randPropSize = pEquProp->GetRandPropSize();
		for(int randIndex=0; randIndex<randPropSize; randIndex++)
		{
			int validID = pEquProp->GetRandPropID(randIndex);
			randPropPool[validID] = true;
		}
		delete pEquProp;
	}
	
	// store the original props
	PropMap validProps;
	int propNum = pEqu->GetPropertyNum();
	for(int propIndex=0; propIndex<propNum; propIndex++)
	{
		const CECIvtrEquip::PROPERTY& prop = pEqu->GetProperty(propIndex);
		
			// exclude the special props
		if( !prop.bEmbed && !prop.bEngraved && !prop.bSuite
			// exclude the random props (need identify)
			&& prop.iType != 336 && prop.iType != 410 && prop.iType != 472 
			// exclude the properties which may effect essence
			&& !pEqu->PropEffectMask(prop, 0) 
			// only add the generated random props 
			&& randPropPool.find(prop.iType) != randPropPool.end()
			// only set range value
			&& pEqu->CheckPropRangeValue(prop) )
		{
			validProps[propIndex] = prop;
		}
	}
	
	if(validProps.empty())
	{
		GetGameUIMan()->AddChatMessage(GetStringFromTable(9355), GP_CHAT_MISC);
		return false;
	}

	m_OriginProps.swap(validProps);

	// make the valid props desc
	CECStringTab* pDescTab = g_pGame->GetItemDesc();
	m_PropsDesc = pDescTab->GetWideString(ITEMDESC_COL_LIGHTBLUE);
	PropMap::iterator itr = m_OriginProps.begin();
	for(;itr != m_OriginProps.end();++itr)
	{
		int propIndex = itr->first;
		const CECIvtrEquip::PROPERTY& prop = itr->second;
		ACString strPropDesc;
		pEqu->FormatRefineData(prop.iType, strPropDesc);
		
		m_PropsDesc += strPropDesc;
		m_PropsDesc += _AL("\\r");
	}

	return true;
}

bool CDlgRandProp::CheckTargetValid(CECIvtrItem* pItem)
{
	if(!SaveProps(pItem))
	{
		return false;
	}

	elementdataman *pDataMan = g_pGame->GetElementDataMan();
	NPC_RANDPROP_SERVICE *pService = (NPC_RANDPROP_SERVICE *)GetDataPtr("ptr_NPC_RANDPROP_SERVICE");
	unsigned int nItemID = (unsigned int)pItem->GetTemplateID();

	for (int i = 0; i < sizeof(pService->pages)/sizeof(pService->pages[0]); ++ i)
	{
		unsigned int id_recipe = pService->pages[i].id_recipe;
		if(id_recipe == 0) continue;

		DATA_TYPE dt = DT_INVALID;
		RANDPROP_ESSENCE *pRecipe = (RANDPROP_ESSENCE *)pDataMan->get_data_ptr(id_recipe, ID_SPACE_RECIPE, dt);
		if (pRecipe && dt == DT_RANDPROP_ESSENCE)
		{
			int targetSize = sizeof(pRecipe->equip_id)/sizeof(pRecipe->equip_id[0]);
			for (int j = 0; j < targetSize; ++ j)
			{
				if(pRecipe->equip_id[j] == nItemID)
				{
					// find the valid page and select target icon
					if(!SelectIcon(i, nItemID))
					{
						ASSERT(false); // should not into here unless the UI got wrong data
						GetGameUIMan()->AddChatMessage(GetStringFromTable(9354), GP_CHAT_MISC);
						return false;
					}
					return true;
				}
			}
		}
	}

	GetGameUIMan()->AddChatMessage(GetStringFromTable(9354), GP_CHAT_MISC);
	return false;
}

bool CDlgRandProp::SelectIcon(int page, int itemID)
{
	AString strName;
	
	int btnIndex = 1;
	while(true)
	{
		PAUIOBJECT pBtn = GetDlgItem(strName.Format("Btn_Set%d", btnIndex));
		if(!pBtn) break;
		
		if(pBtn->GetData() == (DWORD)page)
		{
			int iconIndex = 1;
			RefreshShopSet(btnIndex, iconIndex);
			while(true)
			{
				PAUIOBJECT pIcon = GetDlgItem(strName.Format("U_%02d", iconIndex));
				if(!pIcon) break;
				
				if(pIcon->GetData() == (DWORD)itemID)
				{
					RefreshShopSet(btnIndex, iconIndex);
					return true;
				}

				iconIndex++;
			}
			break;
		}

		btnIndex++;
	}

	// invalid item id
	return false;
}

void CDlgRandProp::SetTarget(int iSlot)
{
	CECIvtrItem* pItem = iSlot >= 0 ? GetHostPlayer()->GetPack()->GetItem(iSlot) : NULL;	
	PAUIIMAGEPICTURE pObj = (PAUIIMAGEPICTURE)GetDlgItem("Item_Target");
	ASSERT(pObj);

	// slot changed
	if(pObj && (int)pObj->GetData() != iSlot)
	{
		if(IsWorking())
		{
			// busy now, can't accept
			GetGameUIMan()->AddChatMessage(GetStringFromTable(9357), GP_CHAT_MISC);
			return;
		}
		else if(pItem && !CheckTargetValid(pItem))
		{
			// ignore the invalid target
			return;
		}

		m_RegenCount = 0;
		m_RegenDesc = GetStringFromTable(9352);
		m_OriginDesc = pItem ? pItem->GetDesc() : GetStringFromTable(9351);
	}

	// set the target icon
	if( pObj ) 
	{ 
		// unbind the old target
		CECIvtrItem* pLast = (CECIvtrItem*)pObj->GetDataPtr();
		if(pLast)
		{
			pLast->Freeze(false);
		}
		pObj->SetDataPtr(pItem);
		pObj->SetData(iSlot);

		// refresh target info
		if(pItem)
		{		
			AString strFile;
			af_GetFileTitle(pItem->GetIconFile(), strFile);
			strFile.MakeLower();
			pObj->SetCover( GetGameUIMan()->GetIconCover(CECGameUIMan::ICONS_INVENTORY),
							GetGameUIMan()->GetIconIndex(CECGameUIMan::ICONS_INVENTORY, strFile) );

			// make the detailed hint
			ACString strCount;
			if(m_RegenCount > 0) strCount.Format(GetStringFromTable(9359), m_RegenCount);
			ACString strHint;
			strHint.Format(GetStringFromTable(9358), m_PropsDesc, m_OriginDesc, strCount, m_RegenDesc);

			AUICTranslate trans;
			pObj->SetHint(trans.Translate(strHint));
			pItem->Freeze(true);

			PAUIOBJECT pBtn = GetDlgItem("Btn_Produce");
			if(pBtn) pBtn->Enable(CanBegin());
		}
		else
		{
			pObj->SetCover(NULL, -1);
			pObj->SetHint(_AL(""));
			m_OriginProps.clear();
		}
	}

	return;
}

void CDlgRandProp::OnChangeLayoutEnd(bool bAllDone)
{
	if(bAllDone)
	{
		// update the text
		if(IsShow())
		{
			RefreshAll();
		}
	}
}

bool CDlgRandProp::CanBegin()
{
	int id_recipe = GetCurRecipe();
	if(id_recipe <= 0 || !RefreshMaterial(id_recipe) ) return false;

	if(IsWorking()) return false;

	PAUIIMAGEPICTURE pObj = (PAUIIMAGEPICTURE)GetDlgItem("Item_Target");
	if(!pObj || (int)pObj->GetData() < 0 || !pObj->GetDataPtr()) return false;

	return true;
}

bool CDlgRandProp::IsWorking()
{
	PAUIPROGRESS pBar = dynamic_cast<PAUIPROGRESS>(GetDlgItem("Prgs_1"));
	return (!pBar || pBar->GetData() > 0);	
}

void CDlgRandProp::OnEventLButtonUp_Target(WPARAM wParam, LPARAM lParam, AUIObject *pObj)
{
	SetTarget(-1);
}

void CDlgRandProp::OnItemDragDrop(CECIvtrItem* pIvtrSrc, int iSrc, PAUIOBJECT pObjSrc, PAUIOBJECT pObjOver)
{
	// no need to check name "Item_Target", only one available drag-drop target
	this->SetTarget(iSrc);
}