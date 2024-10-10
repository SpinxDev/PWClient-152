// Filename	: DlgDragDrop.cpp
// Creator	: Tom Zhou
// Date		: October 20, 2005

#include "AUIDef.h"
#include "DlgDragDrop.h"
#include "DlgBooth.h"
#include "DlgChangeSkirt.h"
#include "DlgFittingRoom.h"
#include "DlgInputNO.h"
#include "DlgShop.h"
#include "DlgSkillSubOther.h"
#include "DlgSkillEdit.h"
#include "DlgInventory.h"
#include "EC_Global.h"
#include "EC_Game.h"
#include "EC_GameSession.h"
#include "EC_GameUIMan.h"
#include "EC_DealInventory.h"
#include "EC_HostPlayer.h"
#include "EC_Shortcut.h"
#include "EC_ShortcutSet.h"
#include "EC_Skill.h"
#include "EC_IvtrItem.h"
#include "EC_IvtrEquip.h"
#include "EC_GameRun.h"
#include "EC_UIManager.h"
#include "EC_Configs.h"
#include "EC_FixedMsg.h"
#include "EC_AutoPolicy.h"
#include "EC_DragDropHelper.h"

#include "DlgHostELF.h"
#include "DlgPalette.h"
#include "DlgPetList.h"
#include "DlgWikiRecipe.h"
#include "DlgAutoHelp.h"
#include "DlgAutoPolicy.h"
#include "DlgTargetItem.h"
#include "DlgSkillSubOther.h"
#include "DlgPetList.h"
#include "EC_UIHelper.h"

#define new A_DEBUG_NEW

#define CLEAN_RETURN() { Show(false); if(pItem) { pItem->ClearCover(); pItem->SetHint(_AL("")); } return; }

#define HANDLE_DLG(DlgName, Handler)								\
	if( 0 == stricmp(pDlgSrc->GetName(), DlgName) )					\
	{ Handler(pDlgSrc, pObjSrc, pDlgOver, pObjOver, pIvtrSrc); }	\
	//

#define HANDLE_DLG_GROUP(DlgName, Handler)							\
	if( strstr(pDlgSrc->GetName(), DlgName) )						\
	{ Handler(pDlgSrc, pObjSrc, pDlgOver, pObjOver, pIvtrSrc); }	\
	//

AUI_BEGIN_EVENT_MAP(CDlgDragDrop, CDlgBase)

AUI_ON_EVENT("Goods",	WM_LBUTTONUP,	OnEventLButtonUp)

AUI_END_EVENT_MAP()

CDlgDragDrop::CDlgDragDrop()
{
}

CDlgDragDrop::~CDlgDragDrop()
{
}

bool CDlgDragDrop::OnInitDialog()
{
	m_pImgGoods = (PAUIIMAGEPICTURE)GetDlgItem("Goods");

	return true;
}

void CDlgDragDrop::OnEventLButtonUp(WPARAM wParam, LPARAM lParam, AUIObject *pObj)
{
	PAUIIMAGEPICTURE pItem = m_pImgGoods;
	PAUIOBJECT pObjSrc = (PAUIOBJECT)pItem->GetDataPtr("ptr_AUIObject");
	if( !pObjSrc )
		CLEAN_RETURN();

	void *ptr;
	AString szType;
	pObjSrc->ForceGetDataPtr(ptr,szType);
	if( !ptr )
		CLEAN_RETURN();

	PAUIDIALOG pDlgOver;
	PAUIOBJECT pObjOver;
	A3DVIEWPORTPARAM *p = m_pA3DEngine->GetActiveViewport()->GetParam();
	int x = GET_X_LPARAM(lParam) - p->X;
	int y = GET_Y_LPARAM(lParam) - p->Y;
	GetGameUIMan()->HitTest(x, y, &pDlgOver, &pObjOver, this);

	PAUIDIALOG pDlgSrc = pObjSrc->GetParent();
	CECIvtrItem *pIvtrSrc;
	if( szType == "ptr_CECIvtrItem")
		pIvtrSrc = (CECIvtrItem *)ptr;
	else
		pIvtrSrc = NULL;
	
	if( abs(x - GetGameUIMan()->m_ptLButtonDown.x) < 3 &&
		abs(y - GetGameUIMan()->m_ptLButtonDown.y) < 3 )
	{
		// assume drag-drop as a click operation
		OnDragClick(pDlgSrc, pObjSrc, pDlgOver, pObjOver);
	}
	else if( GetHostPlayer()->IsTrading() && pIvtrSrc )
	{
		// drag-drop operation on trade scene
		OnTradeScene(pDlgSrc, pObjSrc, pDlgOver, pObjOver, pIvtrSrc);
	}
	else
	{
		// drag-drop operation on general scene
		OnGeneralScene(pDlgSrc, pObjSrc, pDlgOver, pObjOver, pIvtrSrc);
	}

	CLEAN_RETURN();
}

void CDlgDragDrop::SetCapture(bool bCapture, int x, int y)
{
	CDlgBase::SetCapture(bCapture, x, y);
	if( bCapture )
	{
		m_ptDragPos.x = m_ptLButtonDown.x - m_x;
		m_ptDragPos.y = m_ptLButtonDown.y - m_y;
	}
}

void CDlgDragDrop::OnGeneralScene(PAUIDIALOG pDlgSrc, PAUIOBJECT pObjSrc, PAUIDIALOG pDlgOver, PAUIOBJECT pObjOver, CECIvtrItem* pIvtrSrc)
{
	// Handle drag-drop operation from these dialog

	HANDLE_DLG("Win_Inventory", OnInventoryDragDrop)
	else HANDLE_DLG("Win_Bag", OnInventoryDragDrop)
	else HANDLE_DLG("Win_SkillSubAction", OnActionDragDrop) // 20140708 新的动作界面
	else HANDLE_DLG("Win_SkillEdit", OnSkillEditDragDrop)
	else HANDLE_DLG("Win_ELF", OnElfDragDrop) //add for ELF by czx
	else HANDLE_DLG_GROUP("Win_Quickbar", OnQuickBarDragDrop)
	else HANDLE_DLG("Win_Shop", OnShopDragDrop)
	else HANDLE_DLG_GROUP("Win_Storage", OnStorageDragDrop)
	else HANDLE_DLG("Win_ChangeSkirt", OnChangeSkirtDragDrop)
	else HANDLE_DLG("Win_PShop2", OnPShop2DragDrop)
	else HANDLE_DLG("Win_PetList", OnPetListDragDrop)
	else HANDLE_DLG("Win_EPEquip", OnEPEquipDragDrop)
	else HANDLE_DLG("Win_SystemMod", OnSysModDragDrop)
	else HANDLE_DLG("Win_SysModeQuickBar", OnSysModQuickBarDragDrop)
	else HANDLE_DLG("Win_GeneralCard", OnInventoryDragDrop)
	// zhangyitian 20140707 新的连锁技能、固定技能、物品技能界面
	else HANDLE_DLG("Win_SkillSubOther", OnSkillDragDrop)
	else HANDLE_DLG("Win_SkillSubPool", OnSkillDragDrop)
	else
	{
		// do nothing
		
		// zhangyitian 20140704 新的技能界面
		if (strstr(pDlgSrc->GetName(), "Win_SkillSubListSkillItem")) {
			OnSkillDragDrop(pDlgSrc, pObjSrc, pDlgOver, pObjOver, pIvtrSrc);
		}
	}
}

void CDlgDragDrop::OnTradeScene(PAUIDIALOG pDlgSrc, PAUIOBJECT pObjSrc, PAUIDIALOG pDlgOver, PAUIOBJECT pObjOver, CECIvtrItem* pIvtrSrc)
{
	// Handle drag-drop operation from these dialog

	HANDLE_DLG("Win_Inventory", OnInventoryTradeDragDrop)
	else HANDLE_DLG("Win_Bag", OnInventoryTradeDragDrop)
	else HANDLE_DLG("Win_Trade", OnTradeDragDrop)
	else
	{
		// do nothing
	}
}

void CDlgDragDrop::OnInventoryTradeDragDrop( PAUIDIALOG pDlgSrc, PAUIOBJECT pObjSrc, 
											 PAUIDIALOG pDlgOver, PAUIOBJECT pObjOver, CECIvtrItem* pIvtrSrc )
{
	if( !pDlgOver || !pObjOver)
	{
		return;
	}

	if( strstr(pObjSrc->GetName(), "Item_") )
	{
		int iSrc = atoi(pObjSrc->GetName() + strlen("Item_"));

		// only handle drag-drop operation to these dialog
		if ( 0 == stricmp(pDlgOver->GetName(), "Win_Inventory") ||
			 0 == stricmp(pDlgOver->GetName(), "Win_Bag") ||
			 0 == stricmp(pDlgOver->GetName(), "Win_Trade") )
		{
			CDlgBase* pDlg = dynamic_cast<CDlgBase*>(pDlgOver);
			if(pDlg) pDlg->OnItemDragDrop(pIvtrSrc, iSrc, pObjSrc, pObjOver);
		}
	}
}

void CDlgDragDrop::OnInventoryDragDrop( PAUIDIALOG pDlgSrc, PAUIOBJECT pObjSrc, 
										PAUIDIALOG pDlgOver, PAUIOBJECT pObjOver, CECIvtrItem* pIvtrSrc)
{
	CDlgInventory *pDlgInventory = (CDlgInventory *)(GetGameUIMan()->GetDialog("Win_Inventory"));

	if( strstr(pObjSrc->GetName(), "Item_") )
	{
		int iSrc = atoi(pObjSrc->GetName() + strlen("Item_"));
		if( !pDlgOver )
		{
			// throw away
			pDlgInventory->DropItem(pIvtrSrc, iSrc, pObjSrc, pObjOver);
		}
		else if( strstr(pDlgOver->GetName(), "Win_Quickbar")
			&& pObjOver && strstr(pObjOver->GetName(), "Item_") )
		{
			int iSlot = atoi(pObjOver->GetName() + strlen("Item_"));
			CECShortcutSet *pSCS = CECGameUIMan::GetSCSByDlg(pDlgOver->GetName());
			
			if( !pSCS->GetShortcut(iSlot-1) || !g_pGame->GetConfigs()->GetGameSettings().bLockQuickBar )
			{
				pSCS->CreateItemShortcut(iSlot - 1, IVTRTYPE_PACK, iSrc, pIvtrSrc);
				GetGameUIMan()->PlayItemSound(pIvtrSrc, true);
			}
		}
		else if( pObjOver )
		{
			// when drag an item on a dialog
			CDlgBase* pDlg = dynamic_cast<CDlgBase*>(pDlgOver);
			if(pDlg) pDlg->OnItemDragDrop(pIvtrSrc, iSrc, pObjSrc, pObjOver);
		}
	}
	else if( strstr(pObjSrc->GetName(), "Equip_") )
	{
		int iSrc = atoi(pObjSrc->GetName() + strlen("Equip_"));
		if( !pDlgOver )
		{
			// throw away
			pDlgInventory->DropEquip(pIvtrSrc, iSrc, pObjSrc, pObjOver);
		}
		else if( strstr(pDlgOver->GetName(), "Win_Quickbar")
			&& pObjOver && strstr(pObjOver->GetName(), "Item_") )
		{
			int iSlot = atoi(pObjOver->GetName() + strlen("Item_"));
			CECShortcutSet *pSCS = CECGameUIMan::GetSCSByDlg(pDlgOver->GetName());
			
			if( !pSCS->GetShortcut(iSlot-1) || !g_pGame->GetConfigs()->GetGameSettings().bLockQuickBar )
			{
				pSCS->CreateItemShortcut(iSlot - 1, IVTRTYPE_EQUIPPACK, iSrc, pIvtrSrc);
				GetGameUIMan()->PlayItemSound(pIvtrSrc, true);
			}
		}
		else if( (0 == stricmp(pDlgOver->GetName(), "Win_Shop")
			&& pObjOver && strstr(pObjOver->GetName(), "Sell_"))
			|| (0 == stricmp(pDlgOver->GetName(), "Win_PShop2")
			&& pObjOver && strstr(pObjOver->GetName(), "BuyItem_")) )
		{
			GetGameUIMan()->ShowErrorMsg(572);
		}
		else if( 0 == stricmp(pDlgOver->GetName(), "Win_FittingRoom" ) )
		{
			GetGameUIMan()->m_pDlgFittingRoom->SetEquipIcon(pIvtrSrc, pObjOver);
		}
		else if ((0 == stricmp(pDlgOver->GetName(), "Win_Inventory") ||
			      0 == stricmp(pDlgOver->GetName(), "Win_Bag")) && pObjOver )
		{
			pDlgInventory->ExchangeEquip(pIvtrSrc, iSrc, pObjSrc, pObjOver);
		}
	}
	else if( strstr(pObjSrc->GetName(), "Img_Card") )
	{
		int iSrc =  EQUIPIVTR_GENERALCARD1 + atoi(pObjSrc->GetName() + strlen("Img_Card")) - 1;
		if( !pDlgOver )
		{
			// throw away
			pDlgInventory->DropEquip(pIvtrSrc, iSrc, pObjSrc, pObjOver);
		}
		else if( strstr(pDlgOver->GetName(), "Win_Quickbar")
			&& pObjOver && strstr(pObjOver->GetName(), "Item_") )
		{
			int iSlot = atoi(pObjOver->GetName() + strlen("Item_"));
			CECShortcutSet *pSCS = CECGameUIMan::GetSCSByDlg(pDlgOver->GetName());
			
			if( !pSCS->GetShortcut(iSlot-1) || !g_pGame->GetConfigs()->GetGameSettings().bLockQuickBar )
			{
				pSCS->CreateItemShortcut(iSlot - 1, IVTRTYPE_EQUIPPACK, iSrc, pIvtrSrc);
				GetGameUIMan()->PlayItemSound(pIvtrSrc, true);
			}
		}
		else if( (0 == stricmp(pDlgOver->GetName(), "Win_Shop")
			&& pObjOver && strstr(pObjOver->GetName(), "Sell_"))
			|| (0 == stricmp(pDlgOver->GetName(), "Win_PShop2")
			&& pObjOver && strstr(pObjOver->GetName(), "BuyItem_")) )
		{
			GetGameUIMan()->ShowErrorMsg(572);
		}
		else if ((0 == stricmp(pDlgOver->GetName(), "Win_Inventory") ||
			0 == stricmp(pDlgOver->GetName(), "Win_Bag")) && pObjOver )
		{
			pDlgInventory->ExchangeEquip(pIvtrSrc, iSrc, pObjSrc, pObjOver);
		}
		else if ((0 == stricmp(pDlgOver->GetName(), "Win_Storage4")) && pObjOver)
		{
			// 从装备栏向卡牌仓库移动
			int iSlot = atoi(pObjOver->GetName() + strlen("Item_"));
			GetGameSession()->c2s_CmdEquipTrashBoxItem(IVTRTYPE_GENERALCARD_BOX, iSlot - 1, iSrc);
		}
		else if (pDlgOver == pDlgSrc && pObjOver && strstr(pObjOver->GetName(), "Img_Card")){
			int iDst =  EQUIPIVTR_GENERALCARD1 + atoi(pObjOver->GetName() + strlen("Img_Card")) - 1;
			GetGameSession()->c2s_CmdExgEquipItem(iSrc, iDst);
		}
	}
}

void CDlgDragDrop::OnStorageDragDrop(PAUIDIALOG pDlgSrc, PAUIOBJECT pObjSrc, PAUIDIALOG pDlgOver, PAUIOBJECT pObjOver, CECIvtrItem* pIvtrSrc)
{
	if (!pDlgOver){
		return;
	}
	if (CECUIHelper::CheckDialogName(pDlgOver,"Win_GeneralCardRe")){
		GetGameUIMan()->ShowErrorMsg(11021);
		return;
	}
	if (!pObjOver){
		return;
	}
	int iSrcTrashBox = IVTRTYPE_TRASHBOX + atoi(pDlgSrc->GetName() + strlen("Win_Storage"));
	if (iSrcTrashBox == IVTRTYPE_TRASHBOX3 && GetHostPlayer()->GetAutoFashion()){
		return;
	}
	
	if( 0 == stricmp(pDlgOver->GetName(), pDlgSrc->GetName()) ){
		OnStorageDragDropInSameStorage(pDlgSrc, pObjSrc, pDlgOver, pObjOver, pIvtrSrc);
	}else if (CECUIHelper::CheckDialogName(pDlgOver, "Win_GeneralCard") && iSrcTrashBox == IVTRTYPE_GENERALCARD_BOX){
		int iSrc = GetNameIndex(pObjSrc, "Item_") - 1;
		if (iSrc < 0){
			return;
		}
		int iCardIndex = GetNameIndex(pObjOver, "Img_Card") - 1;
		if (iCardIndex < 0){
			return;
		}
		int iDst = EQUIPIVTR_GENERALCARD1 + iCardIndex;
		GetGameSession()->c2s_CmdEquipTrashBoxItem(IVTRTYPE_GENERALCARD_BOX, iSrc, iDst);
	}else if(CECUIHelper::CheckDialogName(pDlgOver, "Win_Inventory")){
		if (CDlgBase::CheckNamePrefix(pObjOver, "Equip_")){
			int iSrc = GetNameIndex(pObjSrc, "Item_") - 1;
			if (iSrc < 0){
				return;
			}
			int iDst = CDlgBase::GetNameIndex(pObjOver, "Equip_");
			if (iDst < 0 ||
				!pIvtrSrc->CanEquippedTo(iDst) || 
				!GetHostPlayer()->CanUseEquipment(dynamic_cast<CECIvtrEquip *>(pIvtrSrc))){
				return;
			}
			CECHostFashionEquipFromStorageSystem::Instance().AddComponent(iSrc, iDst);
		}else if (GetGameUIMan()->m_pDlgInventory->GetShowItem() == CDlgInventory::INVENTORY_ITEM_NORMAL){
			OnStorageDragDropToNormalPack(pDlgSrc, pObjSrc, pDlgOver, pObjOver, pIvtrSrc);
		}
	}else if (CECUIHelper::CheckDialogName(pDlgOver, "Win_Bag")){
		OnStorageDragDropToNormalPack(pDlgSrc, pObjSrc, pDlgOver, pObjOver, pIvtrSrc);
	}else if (iSrcTrashBox == IVTRTYPE_TRASHBOX3 && CECUIHelper::CheckDialogName(pDlgOver, "Win_ChangeSkirt")){
		CDlgChangeSkirt *pChangeSkirt = (CDlgChangeSkirt *)pDlgOver;
		CECIvtrFashion *pSrcFashion = (CECIvtrFashion *)pIvtrSrc;
		pChangeSkirt->Store(pObjOver, pSrcFashion);
	}
}

void CDlgDragDrop::OnDragClick(PAUIDIALOG pDlgSrc, PAUIOBJECT pObjSrc, PAUIDIALOG pDlgOver, PAUIOBJECT pObjOver)
{
	bool bInAutoMode = CECAutoPolicy::GetInstance().IsAutoPolicyEnabled();
	
	if( strstr(pDlgSrc->GetName(), "Win_Quickbar") ||
		0 == stricmp(pDlgSrc->GetName(), "Win_SkillSubAction"))	// 20140708 新的动作界面
	{
		if( bInAutoMode ) return;
		CECShortcut *pSC = (CECShortcut *)GetDataPtr("ptr_CECShortcut");
		if (CDlgAutoHelp::IsAutoHelp() && strstr(pDlgSrc->GetName(), "Win_Quickbar"))
		{
			if (pSC->GetType() == CECShortcut::SCT_SKILL)
			{
				CECSCSkill* pSkillSC = dynamic_cast<CECSCSkill*>(pSC);
				if(pSkillSC && pSkillSC->GetSkill())			
					CDlgWikiShortcut::PopSkillWiki(GetGameUIMan(),pSkillSC->GetSkill()->GetSkillID());
				
			}
			else if(pSC->GetType() == CECShortcut::SCT_ITEM)
			{
				CECSCItem* pItemSC = dynamic_cast<CECSCItem*>(pSC);
				CECIvtrItem* pItem = CECIvtrItem::CreateItem(pItemSC->GetItemTID(),0,1);
				if(pItem)
				{
					CDlgWikiShortcut::PopItemWiki(GetGameUIMan(),pItem);
					delete pItem;
				}
			}
			else
			{
				GetGameUIMan()->MessageBox("",GetStringFromTable(10754), MB_OK, A3DCOLORRGBA(255, 255, 255, 160));
			}
		}
		else
		{			
			if( pSC && pObjSrc->GetDataPtr("ptr_CECShortcut") == pSC )
				pSC->Execute();	
		}
	}
	else if( 0 == stricmp(pDlgSrc->GetName(), "Win_SkillEdit") )
	{
		if( strncmp(pObjSrc->GetName(), "Item_", 5) == 0 )
		{
			int iSrc = atoi(pObjSrc->GetName() + strlen("Item_"));
			GetGameUIMan()->m_pDlgSkillEdit->SelectSkill(iSrc);
		}
	}
	else if (0 == stricmp(pDlgSrc->GetName(), "Win_ELF"))
	{
		if (strstr(pObjSrc->GetName(), "Skill_"))
		{
			if( bInAutoMode ) return;
			CDlgHostELF *pDlg = (CDlgHostELF *)GetGameUIMan()->GetDialog("Win_ELF");
			if (pDlg->IsShow())
			{
				pDlg->OnEventSkill(0, 0, pObjSrc);
			}
		}
	}
	else if (!stricmp(pDlgSrc->GetName(), "Win_ChangeSkirt"))
	{
		if (strstr(pObjSrc->GetName(), "Img_No") &&
			pDlgOver == pDlgSrc &&
			pObjOver == pObjSrc)
		{
			// Execute change suite like in quick bar
			int iFashion = (int)pObjSrc->GetDataPtr();
			if (!GetHostPlayer()->CheckAutoFashionCondition())
			{
				GetHostPlayer()->EquipFashionBySuitID(iFashion-1);
			}
		}
	}
	else if (stricmp(pDlgSrc->GetName(),"Win_Inventory")==0)
	{
		if (CDlgAutoHelp::IsAutoHelp())
		{
			CECIvtrItem *pItem = (CECIvtrItem *)GetDataPtr("ptr_CECIvtrItem");
			CDlgWikiShortcut::PopItemWiki(GetGameUIMan(),pItem);
		}
	}
	else if (stricmp(pDlgSrc->GetName(),"Win_EPEquip") == 0 || stricmp(pDlgSrc->GetName(),"Win_Shop") == 0
		|| stricmp(pDlgSrc->GetName(),"Win_PShop2") == 0)
	{
		if (CDlgAutoHelp::IsAutoHelp())
		{
			CECIvtrItem *pItem = (CECIvtrItem *)GetDataPtr("ptr_CECIvtrItem");
			CDlgWikiShortcut::PopItemWiki(GetGameUIMan(),pItem);
		}
	}
	else if (stricmp(pDlgSrc->GetName(),"Win_SystemMod") == 0 || stricmp(pDlgSrc->GetName(),"Win_SysModeQuickBar") == 0 )
	{
		CECSCSysModule *pSC = (CECSCSysModule *)GetDataPtr("ptr_CECSCSysModule");
		if (pSC && pObjSrc->GetDataPtr("ptr_CECSCSysModule") == pSC)
		{
			pSC->Execute();
		}
	}
	else if (stricmp(pDlgSrc->GetName(), "Win_ItemPop") == 0)
	{
		if (CDlgAutoHelp::IsAutoHelp())
		{
			CECIvtrItem* pItem = (CECIvtrItem*)GetDataPtr("ptr_CECIvtrItem");
			CDlgWikiShortcut::PopItemWiki(GetGameUIMan(), pItem);
		}
		else
		{
			CDlgTargetItem* pDlg = dynamic_cast<CDlgTargetItem*>(pDlgSrc);
			pDlg->UseItem();
		}
	}
	// 20140704 for new skill. Dialog is SkillSubListItem. 点击技能对话框中的技能图标
	else if (strstr(pDlgSrc->GetName(), "Win_SkillSubListSkillItem"))
	{
		CECSkill *pSkill = (CECSkill *)GetDataPtr("ptr_CECSkill");
		if( pSkill ) 
		{
			if (CDlgAutoHelp::IsAutoHelp())				
				CDlgWikiShortcut::PopSkillWiki(GetGameUIMan(),pSkill->GetSkillID());				
			else
			{
				if( bInAutoMode ) return;
				GetHostPlayer()->ApplySkillShortcut(pSkill->GetSkillID());
			}
		}
	}
	// 20140707 for combo skill. Dialog is SkillSubOther. 点击连锁技能、固有技能和物品技能
	else if (stricmp(pDlgSrc->GetName(), "Win_SkillSubOther") == 0)
	{
		if( strstr(pObjSrc->GetName(), "Img_ConSkill") )
		{
			int iSrc = atoi(pObjSrc->GetName() + strlen("Img_ConSkill0"));
			GetGameUIMan()->m_pDlgSkillSubOther->SelectComboSkill(iSrc);
		} 
		else if ( strstr(pObjSrc->GetName(), "Img_InSkill") 
			|| strstr(pObjSrc->GetName(), "Img_ItemSkill") ) 
		{
			CECSkill *pSkill = (CECSkill *)GetDataPtr("ptr_CECSkill");
			if (pSkill) 
			{
				if( bInAutoMode ) return;
				GetHostPlayer()->ApplySkillShortcut(pSkill->GetSkillID());
			}
		}
	}
	// 20140925 点击已学技能界面
	else if (stricmp(pDlgSrc->GetName(), "Win_SkillSubPool") == 0) {
		CECSkill *pSkill = (CECSkill*)GetDataPtr("ptr_CECSkill");
		if (pSkill) {
			if( bInAutoMode ) return;
			GetHostPlayer()->ApplySkillShortcut(pSkill->GetSkillID());
		}
	}
	
	GetGameUIMan()->m_ptLButtonDown.x = -1;
	GetGameUIMan()->m_ptLButtonDown.y = -1;
}

void CDlgDragDrop::OnTradeDragDrop(PAUIDIALOG pDlgSrc, PAUIOBJECT pObjSrc, PAUIDIALOG pDlgOver, PAUIOBJECT pObjOver, CECIvtrItem* pIvtrSrc)
{
	if( strstr(pObjSrc->GetName(), "I_") )
	{
		if( !pDlgOver || 0 != stricmp(pDlgOver->GetName(), "Win_Trade") )
		{
			CECDealInventory *pDeal = GetHostPlayer()->GetDealPack();
			int iSrc = atoi(pObjSrc->GetName() + strlen("I_")) - 1;
			const CECDealInventory::ITEMINFO &ii = pDeal->GetItemInfo(iSrc);
			
			GetGameSession()->trade_RemoveGoods(GetGameUIMan()->m_idTrade,
				pIvtrSrc->GetTemplateID(), ii.iOrigin, ii.iAmount, 0);
			pIvtrSrc->Freeze(false);
			GetGameUIMan()->PlayItemSound(pIvtrSrc, true);
		}
	}
}

void CDlgDragDrop::OnActionDragDrop(PAUIDIALOG pDlgSrc, PAUIOBJECT pObjSrc, PAUIDIALOG pDlgOver, PAUIOBJECT pObjOver, CECIvtrItem* pIvtrSrc)
{
	if( pDlgOver && strstr(pDlgOver->GetName(), "Win_Quickbar")
		&& pObjOver && strstr(pObjOver->GetName(), "Item_") )
	{
		CECShortcut *pSC = (CECShortcut *)pObjSrc->GetDataPtr("ptr_CECShortcut");
		int iSlot = atoi(pObjOver->GetName() + strlen("Item_"));
		CECShortcutSet *pSCS = CECGameUIMan::GetSCSByDlg(pDlgOver->GetName());
		
		if( !pSCS->GetShortcut(iSlot-1) || !g_pGame->GetConfigs()->GetGameSettings().bLockQuickBar )
			pSCS->CreateClonedShortcut(iSlot - 1, pSC);
	}
	else if( pDlgOver && stricmp(pDlgOver->GetName(), "Win_AutoPolicy") == 0
		&& pObjOver && strstr(pObjOver->GetName(), "Img_Skill") )
	{
		GetGameUIMan()->m_pDlgAutoPolicy->DragDropItem(pDlgSrc, pObjSrc, pObjOver);
	}
}

void CDlgDragDrop::OnSkillEditDragDrop(PAUIDIALOG pDlgSrc, PAUIOBJECT pObjSrc, PAUIDIALOG pDlgOver, PAUIOBJECT pObjOver, CECIvtrItem* pIvtrSrc)
{
	if( !pDlgOver || 
		pDlgOver && stricmp(pDlgOver->GetName(), "Win_SkillEdit") == 0
		&& pObjOver && strstr(pObjOver->GetName(), "Item_") )
			GetGameUIMan()->m_pDlgSkillEdit->DragDropItem(pDlgSrc, pObjSrc, pObjOver);
}
void CDlgDragDrop::OnSysModDragDrop(PAUIDIALOG pDlgSrc, PAUIOBJECT pObjSrc, PAUIDIALOG pDlgOver, PAUIOBJECT pObjOver, CECIvtrItem* pIvtrSrc)
{
	if( pDlgOver && strstr(pDlgOver->GetName(), "Win_SysModeQuickBar")
		&& pObjOver && strstr(pObjOver->GetName(), "Item_") )
	{
		CECShortcut *pSC = (CECShortcut *)pObjSrc->GetDataPtr("ptr_CECSCSysModule");
		int iSlot = atoi(pObjOver->GetName() + strlen("Item_"));
		CECShortcutSet *pSCS = GetHostPlayer()->GetCurSysModShortcutSet();
		if( !pSCS->GetShortcut(iSlot-1) || !g_pGame->GetConfigs()->GetGameSettings().bLockQuickBar )
				pSCS->CreateClonedShortcut(iSlot - 1, pSC);
	}
}

void CDlgDragDrop::OnSkillDragDrop(PAUIDIALOG pDlgSrc, PAUIOBJECT pObjSrc, PAUIDIALOG pDlgOver, PAUIOBJECT pObjOver, CECIvtrItem* pIvtrSrc)
{
	if( pDlgOver && strstr(pDlgOver->GetName(), "Win_Quickbar")
		&& pObjOver && strstr(pObjOver->GetName(), "Item_") )
	{
		// 连锁技能拖到快捷栏
		if( //strstr(pObjSrc->GetName(), "Item") || 
		    strstr(pObjSrc->GetName(), "Img_ConSkill"))  // 20140707新增 新的连锁技能界面
		{
			int nCombo = pObjSrc->GetData();
			int iSlot = atoi(pObjOver->GetName() + strlen("Item_"));

			CECShortcutSet *pSCS = CECGameUIMan::GetSCSByDlg(pDlgOver->GetName());
			if( !pSCS->GetShortcut(iSlot-1) || !g_pGame->GetConfigs()->GetGameSettings().bLockQuickBar )
				pSCS->CreateSkillGroupShortcut(iSlot - 1, nCombo - 1);
		}
		// 普通技能拖到快捷栏
		else
		{
			CECSkill *pSkill = (CECSkill *)pObjSrc->GetDataPtr("ptr_CECSkill");
			int iSlot = atoi(pObjOver->GetName() + strlen("Item_"));

			CECShortcutSet *pSCS = CECGameUIMan::GetSCSByDlg(pDlgOver->GetName());
			if( !pSCS->GetShortcut(iSlot-1) || !g_pGame->GetConfigs()->GetGameSettings().bLockQuickBar )
				pSCS->CreateSkillShortcut(iSlot - 1, pSkill);
		}
	}
	// 自动策略栏
	else if( pDlgOver && stricmp(pDlgOver->GetName(), "Win_AutoPolicy") == 0
		&& pObjOver && strstr(pObjOver->GetName(), "Img_Skill") )
	{
		GetGameUIMan()->m_pDlgAutoPolicy->DragDropItem(pDlgSrc, pObjSrc, pObjOver);
	}
	// 连锁技能编辑栏
	else if( pDlgOver && stricmp(pDlgOver->GetName(), "Win_SkillEdit") == 0
		&& pObjOver && strstr(pObjOver->GetName(), "Item_")
		&& !strstr(pObjSrc->GetName(), "Img_ConSkill"))		// 20140707新增，防止把连锁技能拖到连锁技能中
			GetGameUIMan()->m_pDlgSkillEdit->DragDropItem(pDlgSrc, pObjSrc, pObjOver);
}

void CDlgDragDrop::OnElfDragDrop(PAUIDIALOG pDlgSrc, PAUIOBJECT pObjSrc, PAUIDIALOG pDlgOver, PAUIOBJECT pObjOver, CECIvtrItem* pIvtrSrc)
{
	if( pDlgOver && strstr(pDlgOver->GetName(), "Win_Quickbar")
		&& pObjOver && strstr(pObjOver->GetName(), "Item_") )
	{
		
		CECSkill *pSkill = (CECSkill *)pObjSrc->GetDataPtr("ptr_CECGoblinSkill");
		int iSlot = atoi(pObjOver->GetName() + strlen("Item_"));

		CECShortcutSet *pSCS = CECGameUIMan::GetSCSByDlg(pDlgOver->GetName());
		if( !pSCS->GetShortcut(iSlot-1) || !g_pGame->GetConfigs()->GetGameSettings().bLockQuickBar )
			pSCS->CreateSkillShortcut(iSlot - 1, pSkill);
	}
}

void CDlgDragDrop::OnQuickBarDragDrop(PAUIDIALOG pDlgSrc, PAUIOBJECT pObjSrc, PAUIDIALOG pDlgOver, PAUIOBJECT pObjOver, CECIvtrItem* pIvtrSrc)
{
	if( !pDlgOver )		// Remove it.
	{
		int iSlot = atoi(pObjSrc->GetName() + strlen("Item_"));
		CECShortcutSet *pSCS = CECGameUIMan::GetSCSByDlg(pDlgSrc->GetName());
		
		if( !g_pGame->GetConfigs()->GetGameSettings().bLockQuickBar )
			pSCS->SetShortcut(iSlot - 1, NULL);
	}
	else if( strstr(pDlgOver->GetName(), "Win_Quickbar")
		&& pObjOver && strstr(pObjOver->GetName(), "Item_") )
	{
		int iSlotSrc = atoi(pObjSrc->GetName() + strlen("Item_"));
		int iSlotDst = atoi(pObjOver->GetName() + strlen("Item_"));
		CECShortcutSet *pSCSSrc = CECGameUIMan::GetSCSByDlg(pDlgSrc->GetName());
		CECShortcutSet *pSCSDst = CECGameUIMan::GetSCSByDlg(pDlgOver->GetName());
		
		if( !g_pGame->GetConfigs()->GetGameSettings().bLockQuickBar )
		{
			CECShortcut *pSCSrc = pSCSSrc->GetShortcut(iSlotSrc - 1, true);
			CECShortcut *pSCDst = pSCSDst->GetShortcut(iSlotDst - 1, true);
			pSCSSrc->SetShortcut(iSlotSrc - 1, pSCDst);
			pSCSDst->SetShortcut(iSlotDst - 1, pSCSrc);
		}
	}
	else if( pDlgOver && stricmp(pDlgOver->GetName(), "Win_AutoPolicy") == 0
		&& pObjOver && strstr(pObjOver->GetName(), "Img_Skill") )
	{
		GetGameUIMan()->m_pDlgAutoPolicy->DragDropItem(pDlgSrc, pObjSrc, pObjOver);
	}
}

void CDlgDragDrop::OnSysModQuickBarDragDrop(PAUIDIALOG pDlgSrc, PAUIOBJECT pObjSrc, PAUIDIALOG pDlgOver, PAUIOBJECT pObjOver, CECIvtrItem* pIvtrSrc)
{
	if( !pDlgOver || pDlgOver != pDlgSrc )		// Remove it.
	{
		int iSlot = atoi(pObjSrc->GetName() + strlen("Item_"));
		CECShortcutSet *pSCS = GetHostPlayer()->GetCurSysModShortcutSet();
		
		if( !g_pGame->GetConfigs()->GetGameSettings().bLockQuickBar )
			pSCS->SetShortcut(iSlot - 1, NULL);
	}
	else if( strstr(pDlgOver->GetName(), "Win_SysModeQuickBar")
		&& pObjOver && strstr(pObjOver->GetName(), "Item_") )
	{
		int iSlotSrc = atoi(pObjSrc->GetName() + strlen("Item_"));
		int iSlotDst = atoi(pObjOver->GetName() + strlen("Item_"));
		CECShortcutSet *pSCS = GetHostPlayer()->GetCurSysModShortcutSet();
		
		if( !g_pGame->GetConfigs()->GetGameSettings().bLockQuickBar )
		{
			pSCS->ExchangeShortcut(iSlotSrc-1,iSlotDst-1);
		}
	}
}
void CDlgDragDrop::OnShopDragDrop(PAUIDIALOG pDlgSrc, PAUIOBJECT pObjSrc, PAUIDIALOG pDlgOver, PAUIOBJECT pObjOver, CECIvtrItem* pIvtrSrc)
{
	if( strstr(pObjSrc->GetName(), "U_") &&
		pObjOver && strstr(pObjOver->GetName(), "Buy_") )
	{
		GetGameUIMan()->m_pDlgShop->OnEventLButtonDblclk(0, 0, pObjSrc);
		GetGameUIMan()->PlayItemSound(pIvtrSrc, true);
	}
	else if( strstr(pObjSrc->GetName(), "Buy_") || strstr(pObjSrc->GetName(), "Sell_") )
	{
		if( !pDlgOver ||
			(pObjOver && strstr(pObjOver->GetName(), "U_") && 0==stricmp(pDlgOver->GetName(), "Win_Shop")) ||
			(pDlgOver && (0==stricmp(pDlgOver->GetName(), "Win_Inventory") || (0==stricmp(pDlgOver->GetName(), "Win_Bag")))) )
		{
			GetGameUIMan()->m_pDlgShop->OnEventLButtonDblclk(0, 0, pObjSrc);
			GetGameUIMan()->PlayItemSound(pIvtrSrc, true);
		}
	}
}

void CDlgDragDrop::OnChangeSkirtDragDrop(PAUIDIALOG pDlgSrc, PAUIOBJECT pObjSrc, PAUIDIALOG pDlgOver, PAUIOBJECT pObjOver, CECIvtrItem* pIvtrSrc)
{
	if (strstr(pObjSrc->GetName(), "Item_"))
	{
		// Drag a fashion short cut
		//
		CDlgChangeSkirt *pChangeSkirt = (CDlgChangeSkirt *)GetGameUIMan()->GetDialog("Win_ChangeSkirt");
		
		if (pDlgOver == pChangeSkirt
			&& pObjOver
			&& strstr(pObjOver->GetName(), "Item_"))
		{
			// Drag within Win_ChangeSkirt
			pChangeSkirt->Replace(pObjSrc, pObjOver);
		}
		else if (!pDlgOver)
		{
			// Drop a fashion short cut
			pChangeSkirt->Discard(pObjSrc);
		}
	}
	else if (strstr(pObjSrc->GetName(), "Img_No"))
	{
		// Drag a suite item
		//
		if (pDlgOver && pObjOver)
		{
			if (strstr(pDlgOver->GetName(), "Win_Quickbar") &&
				strstr(pObjOver->GetName(), "Item_"))
			{
				// Create quick bar item
				int iSlot = atoi(pObjOver->GetName() + strlen("Item_"));
				int iFashion = (int)pObjSrc->GetDataPtr();
				CECShortcutSet *pSCS = CECGameUIMan::GetSCSByDlg(pDlgOver->GetName());

				if( !pSCS->GetShortcut(iSlot-1) || !g_pGame->GetConfigs()->GetGameSettings().bLockQuickBar )
					pSCS->CreateAutoFashionShortcut(iSlot-1, iFashion-1);
			}
		}
	}
}

void CDlgDragDrop::OnPShop2DragDrop(PAUIDIALOG pDlgSrc, PAUIOBJECT pObjSrc, PAUIDIALOG pDlgOver, PAUIOBJECT pObjOver, CECIvtrItem* pIvtrSrc)
{
	if( pDlgOver && 0 == stricmp(pDlgOver->GetName(), "Win_FittingRoom" )
		&& pObjOver && strstr(pObjOver->GetName(), "Equip_") )
	{
		GetGameUIMan()->m_pDlgFittingRoom->SetEquipIcon(pIvtrSrc, pObjOver);
	}
	else if( strstr(pObjSrc->GetName(), "SellItem_") &&
		pObjOver && strstr(pObjOver->GetName(), "Buy_") )
	{
		GetGameUIMan()->m_pDlgBooth2->OnEventLButtonDblclk_SellItem(0, 0, pObjSrc);
		GetGameUIMan()->PlayItemSound(pIvtrSrc, true);
	}
	else if( strstr(pObjSrc->GetName(), "Buy_") )
	{
		if( !pDlgOver || (0 == stricmp(pDlgOver->GetName(), "Win_PShop2")
			&& pObjOver && strstr(pObjOver->GetName(), "SellItem_")) )
		{
			GetGameUIMan()->m_pDlgBooth2->OnEventLButtonDblclk_Buy(0, 0, pObjSrc);
			GetGameUIMan()->PlayItemSound(pIvtrSrc, true);
		}
	}
	else if( strstr(pObjSrc->GetName(), "Sell_") )
	{
		if( !pDlgOver ||
			(0 == stricmp(pDlgOver->GetName(), "Win_PShop2") && pObjOver && strstr(pObjOver->GetName(), "BuyItem_")) ||
			0 == stricmp(pDlgOver->GetName(), "Win_Inventory") ||
			0 == stricmp(pDlgOver->GetName(), "Win_Bag") )
		{
			GetGameUIMan()->m_pDlgBooth2->OnEventLButtonDblclk_Sell(0, 0, pObjSrc);
			GetGameUIMan()->PlayItemSound(pIvtrSrc, true);
		}
	}
}

void CDlgDragDrop::OnPetListDragDrop(PAUIDIALOG pDlgSrc, PAUIOBJECT pObjSrc, PAUIDIALOG pDlgOver, PAUIOBJECT pObjOver, CECIvtrItem* pIvtrSrc)
{
	if(!pDlgOver || !strstr(pObjSrc->GetName(), "Img_Icon"))
		return;

	CDlgPetList* pDlgPetList = dynamic_cast<CDlgPetList*>(GetGameUIMan()->GetDialog("Win_PetList"));
	if (!pDlgPetList) {
		return;
	}

	// get pet index
	int iSrc = pDlgPetList->GetPetIndex(atoi(pObjSrc->GetName() + strlen("Img_Icon")) - 1);

	if( strstr(pDlgOver->GetName(), "Win_Quickbar") )
	{
		if( pObjOver && strstr(pObjOver->GetName(), "Item_") )
		{
			int iSlot = atoi(pObjOver->GetName() + strlen("Item_"));
			CECShortcutSet *pSCS = CECGameUIMan::GetSCSByDlg(pDlgOver->GetName());

			if( !pSCS->GetShortcut(iSlot-1) || !g_pGame->GetConfigs()->GetGameSettings().bLockQuickBar )
				pSCS->CreatePetShortcut(iSlot - 1, iSrc);
		}
	}
	else
	{
		GetGameUIMan()->m_pDlgPetList->OnPetDragDrop(iSrc, pObjOver);
	}
}

void CDlgDragDrop::OnEPEquipDragDrop(PAUIDIALOG pDlgSrc, PAUIOBJECT pObjSrc, PAUIDIALOG pDlgOver, PAUIOBJECT pObjOver, CECIvtrItem* pIvtrSrc)
{
	if( pDlgOver && 0 == stricmp(pDlgOver->GetName(), "Win_FittingRoom" ) )
	{
		GetGameUIMan()->m_pDlgFittingRoom->SetEquipIcon(pIvtrSrc, pObjOver);
	}
}

void CDlgDragDrop::OnStorageDragDropInSameStorage(PAUIDIALOG pDlgSrc, PAUIOBJECT pObjSrc, PAUIDIALOG pDlgOver, PAUIOBJECT pObjOver, CECIvtrItem* pIvtrSrc)
{
	int iSrc = GetNameIndex(pObjSrc, "Item_") - 1;
	int iDst = GetNameIndex(pObjOver, "Item_") - 1;
	if (iSrc < 0 || iDst < 0){
		return;
	}
	if( iSrc != iDst )
	{
		int iSrcTrashBox = IVTRTYPE_TRASHBOX + atoi(pDlgSrc->GetName() + strlen("Win_Storage"));
		if( CECIvtrItem *pIvtrDst = (CECIvtrItem *)pObjOver->GetDataPtr("ptr_CECIvtrItem") )
		{
			if( pIvtrSrc->GetPileLimit() > 1 &&
				pIvtrSrc->GetTemplateID() == pIvtrDst->GetTemplateID() )
			{
				if( AUI_PRESS(VK_MENU) && pIvtrSrc->GetCount() > 1 )
				{
					GetGameUIMan()->InvokeNumberDialog(pObjSrc, pObjOver,
						CDlgInputNO::INPUTNO_STORAGE_MOVE_ITEMS, pIvtrSrc->GetCount());
				}
				else
				{
					GetGameSession()->c2s_CmdMoveTrashBoxItem(iSrcTrashBox, iSrc, iDst,
						min(pIvtrSrc->GetCount(), pIvtrDst->GetPileLimit() - pIvtrDst->GetCount()));
				}
			}
			else
				GetGameSession()->c2s_CmdExgTrashBoxItem(iSrcTrashBox, iSrc, iDst);
		}
		else
		{
			if( AUI_PRESS(VK_MENU) && pIvtrSrc->GetCount() > 1 && pIvtrSrc->GetPileLimit() > 1 )
			{
				GetGameUIMan()->InvokeNumberDialog(pObjSrc, pObjOver,
					CDlgInputNO::INPUTNO_STORAGE_MOVE_ITEMS, pIvtrSrc->GetCount());
			}
			else
				GetGameSession()->c2s_CmdExgTrashBoxItem(iSrcTrashBox, iSrc, iDst);
		}
		GetGameUIMan()->PlayItemSound(pIvtrSrc, true);
	}
}

void CDlgDragDrop::OnStorageDragDropToNormalPack(PAUIDIALOG pDlgSrc, PAUIOBJECT pObjSrc, PAUIDIALOG pDlgOver, PAUIOBJECT pObjOver, CECIvtrItem* pIvtrSrc)
{	
	int iSrc = GetNameIndex(pObjSrc, "Item_") - 1;
	int iDst = GetNameIndex(pObjOver, "Item_");
	if (iSrc < 0 || iDst < 0){
		return;
	}
	int iSrcTrashBox = IVTRTYPE_TRASHBOX + atoi(pDlgSrc->GetName() + strlen("Win_Storage"));
	if( CECIvtrItem *pIvtrDst = (CECIvtrItem *)pObjOver->GetDataPtr("ptr_CECIvtrItem" ))
	{
		if( iSrcTrashBox == IVTRTYPE_TRASHBOX2 && 
			!(pIvtrDst->GetClassID() == CECIvtrItem::ICID_MATERIAL || pIvtrDst->GetClassID() == CECIvtrItem::ICID_SKILLTOME || pIvtrDst->GetClassID() == CECIvtrItem::ICID_TASKNMMATTER )){
			return;
		}
		if (iSrcTrashBox == IVTRTYPE_TRASHBOX3 && pIvtrDst->GetClassID() != CECIvtrItem::ICID_FASHION ){
			return;
		}
		
		if (iSrcTrashBox == IVTRTYPE_ACCOUNT_BOX &&
			!pIvtrDst->CanPutIntoAccBox())
		{
			GetGameUIMan()->AddChatMessage(GetStringFromTable(8081), GP_CHAT_MISC);
			return;
		}
		
		if( pIvtrSrc->GetPileLimit() > 1 &&
			pIvtrSrc->GetTemplateID() == pIvtrDst->GetTemplateID() )
		{
			if( AUI_PRESS(VK_MENU) && pIvtrSrc->GetCount() > 1 )
			{
				GetGameUIMan()->InvokeNumberDialog(pObjSrc, pObjOver,
					CDlgInputNO::INPUTNO_STORAGE_GET_ITEMS, pIvtrSrc->GetCount());
			}
			else
			{
				GetGameSession()->c2s_CmdMoveTrashBoxToIvtr(iSrcTrashBox, iSrc, iDst,
					min(pIvtrSrc->GetCount(), pIvtrDst->GetPileLimit() - pIvtrDst->GetCount()));
			}
		}
		else
		{					
			GetGameSession()->c2s_CmdExgTrashBoxIvtrItem(iSrcTrashBox, iSrc, iDst);
		}
	}
	else
	{
		if( AUI_PRESS(VK_MENU) && pIvtrSrc->GetCount() > 1 &&	pIvtrSrc->GetPileLimit() > 1 )
		{
			GetGameUIMan()->InvokeNumberDialog(pObjSrc, pObjOver,
				CDlgInputNO::INPUTNO_STORAGE_GET_ITEMS, pIvtrSrc->GetCount());
		}
		else
			GetGameSession()->c2s_CmdExgTrashBoxIvtrItem(iSrcTrashBox, iSrc, iDst);
	}
	GetGameUIMan()->PlayItemSound(pIvtrSrc, true);
}