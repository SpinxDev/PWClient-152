// Filename	: DlgInventory.cpp
// Creator	: Tom Zhou
// Date		: October 10, 2005

#include "AFI.h"
#include "AUIDef.h"
#include "AUICheckbox.h"
#include "DlgInventory.h"
#include "DlgInputNO.h"
#include "DlgTrade.h"
#include "DlgShop.h"
#include "DlgSplit.h"
#include "DlgIdentify.h"
#include "DlgFittingRoom.h"
#include "DlgBooth.h"
#include "DlgFaceLift.h"
#include "DlgPetHatch.h"
#include "DlgPetDye.h"
#include "DlgEngrave.h"
#include "DlgEquipRefine.h"
#include "DlgEquipBind.h"
#include "DlgEquipDestroy.h"
#include "DlgEquipUndestroy.h"
#include "DlgEquipDye.h"
#include "DlgEquipMark.h"
#include "DlgEquipSlot.h"
#include "DlgDamageRep.h"
#include "DlgRefineTrans.h"
#include "DlgPetRetrain.h"
#include "DlgProduce.h"
#include "DlgBag.h"
#include "DlgChat.h"
#include "DlgGeneralCard.h"
#include "DlgItemDesc.h"
#include "EC_GameSession.h"
#include "EC_GameUIMan.h"
#include "EC_Friend.h"
#include "EC_ManPlayer.h"
#include "EC_HostPlayer.h"
#include "EC_Inventory.h"
#include "EC_IvtrEquip.h"
#include "EC_IvtrFlySword.h"
#include "EC_Game.h"
#include "EC_Global.h"
#include "EC_Utility.h"
#include "EC_GameRun.h"
#include "EC_FixedMsg.h"
#include "EC_DealInventory.h"
#include "EC_UIManager.h"
#include "EC_Configs.h"
#include "ECScriptMan.h"
#include "ECScriptContext.h"
#include "ECScriptCheckStateInGame.h"
#include "EC_IvtrConsume.h"
#include "EC_ShortcutMgr.h"
#include "EC_ShortcutSet.h"
#include "EC_Shortcut.h"
#include "itemdataman.h"
#include "DlgDragDrop.h"
#include "DlgMeridians.h"
#include "DlgFashionSplit.h"
#include "DlgAutoHelp.h"
#include "EC_UIHelper.h"

#define new A_DEBUG_NEW

AUI_BEGIN_COMMAND_MAP(CDlgInventory, CDlgBase)

AUI_ON_COMMAND("choosemoney",		OnCommand_choosemoney)
AUI_ON_COMMAND("normalitem",		OnCommand_normalitem)
AUI_ON_COMMAND("questitem",			OnCommand_questitem)
AUI_ON_COMMAND("normalmode",		OnCommand_normalmode)
AUI_ON_COMMAND("wearmode",			OnCommand_wearmode)
AUI_ON_COMMAND("Chk_WearMode",		OnCommand_Fashion)
AUI_ON_COMMAND("try",				OnCommand_Try)
AUI_ON_COMMAND("IDCANCEL",			OnCommand_CANCEL)
AUI_ON_COMMAND("fashionstore",		OnCommand_FashionStore)
AUI_ON_COMMAND("arrange",           OnCommand_arrange)
AUI_ON_COMMAND("Btn_Meridians",		OnCommand_meridian)
AUI_ON_COMMAND("Btn_FashionShop",	OnCommand_FashionShop)
AUI_ON_COMMAND("Btn_Generalcard",	OnCommand_GeneralCard)
AUI_ON_COMMAND("Btn_Booth",			OnCommand_Booth)
AUI_END_COMMAND_MAP()

AUI_BEGIN_EVENT_MAP(CDlgInventory, CDlgBase)

AUI_ON_EVENT("Equip_*",		WM_LBUTTONDOWN,		OnEventLButtonDown)
AUI_ON_EVENT("Item_*",		WM_LBUTTONDOWN,		OnEventLButtonDown)
AUI_ON_EVENT("Equip_*",		WM_LBUTTONDBLCLK,	OnEventLButtonDblclkEquip)
AUI_ON_EVENT("Item_*",		WM_LBUTTONDBLCLK,	OnEventLButtonDblclkItem)
AUI_ON_EVENT("Equip_*",		WM_RBUTTONUP,		OnEventLButtonDblclkEquip)
AUI_ON_EVENT("Item_*",		WM_RBUTTONUP,		OnEventLButtonDblclkItem)
AUI_ON_EVENT("*",			WM_MOUSEWHEEL,		OnEventMouseWheel)
AUI_ON_EVENT(NULL,			WM_MOUSEWHEEL,		OnEventMouseWheel)

AUI_END_EVENT_MAP()

//------------------------------------------------------------------------
// Right-Click Shortcut for CDlgInventory
//------------------------------------------------------------------------
class InventoryClickShortcut : public CECShortcutMgr::ClickShortcut
{
public:
	InventoryClickShortcut(const char* pName, CDlgInventory* pDlg)
		:m_pDlg(pDlg)
		,m_GroupName(pName)
	{}
	
	virtual const char* GetGroupName() { return m_GroupName; }
	
	virtual bool CanTrigger(PAUIOBJECT pSrcObj) 
	{
		return // the target dialog is shown
			   m_pDlg && m_pDlg->IsShow() &&  
			   // the source dialog is correct
			   pSrcObj->GetParent() == m_pDlg->GetAUIManager()->GetDialog(GetGroupName()) &&
			   // contains a item pointer
			   pSrcObj && pSrcObj->GetDataPtr("ptr_CECIvtrItem");
	}
	
	PAUIOBJECT GetEmptySlot(CECIvtrItem* pIvtrSrc)
	{
		PAUIOBJECT pObjFound = NULL;

		CECInventory* pInventory = g_pGame->GetGameRun()->GetHostPlayer()->GetPack();
		int slot = pInventory->CanAddItem(pIvtrSrc->GetTemplateID(), pIvtrSrc->GetCount(), true);
		if(slot >= 0)
		{
			AString strName;
			strName.Format("Item_%02d", slot);
			PAUIOBJECT pObj = m_pDlg->GetDlgItem(strName);
			if(!pObj) // should in bag
			{
				PAUIMANAGER pUIMan = m_pDlg->GetAUIManager();
				PAUIDIALOG pDlgBag = pUIMan->GetDialog("Win_Bag");
				pObj = pDlgBag->GetDlgItem(strName);
				if(pObj && pObj->IsShow())
				{
					//m_pDlg->OnCommand_newbag("newbag");
					m_pDlg->SetBagModeSplit();
					pObjFound = pObj;
				}
			}
			else if(pObj->IsShow())
			{
				pObjFound = pObj;
			}
		}

		return pObjFound;
	}

	virtual void Trigger(PAUIOBJECT pSrcObj, int num) 
	{ 
		CECIvtrItem* pItem = (CECIvtrItem*)pSrcObj->GetDataPtr("ptr_CECIvtrItem");
		PAUIOBJECT pObjOver = GetEmptySlot(pItem);
		if(pObjOver)
		{
			PAUIMANAGER pUIMan = m_pDlg->GetAUIManager();
			CDlgDragDrop* pDragDrop = dynamic_cast<CDlgDragDrop*>(pUIMan->GetDialog("DragDrop"));
			// use the same logic as drag-drop
			pDragDrop->OnStorageDragDrop(pSrcObj->GetParent(), pSrcObj, 
										 pObjOver->GetParent(), pObjOver, pItem);
		}
	}
	
protected:
	CDlgInventory* m_pDlg;
	AString m_GroupName;
};
//------------------------------------------------------------------------

CDlgInventory::CDlgInventory()
{
	m_nShowIvtrItem = INVENTORY_ITEM_NORMAL;
}

CDlgInventory::~CDlgInventory()
{
}

bool CDlgInventory::OnInitDialog()
{
	int i;
	char szItem[40];

	CECShortcutMgr* pShortcut = GetGameUIMan()->GetShortcutMgr();
	pShortcut->RegisterShortCut(new InventoryClickShortcut("Win_Storage", this));
	pShortcut->RegisterShortCut(new InventoryClickShortcut("Win_Storage1", this));
	pShortcut->RegisterShortCut(new InventoryClickShortcut("Win_Storage2", this));
	pShortcut->RegisterShortCut(new InventoryClickShortcut("Win_Storage3", this));
	pShortcut->RegisterShortCut(new InventoryClickShortcut("Win_Storage4", this));
	
	m_pTxt_Gold = (PAUIOBJECT)GetDlgItem("Txt_Gold");
	m_pBtn_NormalItem = (PAUISTILLIMAGEBUTTON)GetDlgItem("Btn_NormalItem");
	m_pBtn_QuestItem = (PAUISTILLIMAGEBUTTON)GetDlgItem("Btn_QuestItem");
	m_pBtn_NormalMode = (PAUISTILLIMAGEBUTTON)GetDlgItem("Btn_NormalMode");
	m_pBtn_WearMode = (PAUISTILLIMAGEBUTTON)GetDlgItem("Btn_WearMode");
	m_pScl_Item = (PAUISCROLL)GetDlgItem("Scl_Item");
	m_pBtn_Meridians = GetDlgItem("Btn_Meridians");
	m_pBtn_FashionShop = GetDlgItem("Btn_FashionShop");
	m_pChkWearMode = (PAUICHECKBOX)GetDlgItem("Chk_WearMode");
	m_pChkBag = (PAUICHECKBOX)GetDlgItem("Chk_Bag");
	m_pLblLing = (PAUILABEL)GetDlgItem("Lbl_Ling");
	m_pLblMai = (PAUILABEL)GetDlgItem("Lbl_Mai");

	for( i = 0; i < SIZE_EQUIPIVTR; i++ )
	{
		sprintf(szItem, "Equip_%02d", i);
		m_pImgEquip[i] = (PAUIIMAGEPICTURE)GetDlgItem(szItem);
	}

	for( m_nTotalItem = 0; ; m_nTotalItem++ )
	{
		sprintf(szItem, "Item_%02d", m_nTotalItem);
		if( !GetDlgItem(szItem) )
			break;
		m_pImgItem[m_nTotalItem] = (PAUIIMAGEPICTURE)GetDlgItem(szItem);
	}

	// Set inventory normal item show mode to NORMAL_ITEM_SHOW_MODE_INTEGRATED
	// CheckRadioButton(3, 1);
	m_pChkBag->Check(false);
	return true;
}

bool CDlgInventory::Release()
{
	return CDlgBase::Release();
}

void CDlgInventory::OnCommand_choosemoney(const char * szCommand)
{
	CDlgInputNO *pShow = GetGameUIMan()->m_pDlgInputNO;
	pShow->Show(true, true);

	ACHAR szText[40];
	PAUIOBJECT pEdit = pShow->GetDlgItem("DEFAULT_Txt_No.");
	pEdit->SetData(GetHostPlayer()->GetMoneyAmount());
	a_sprintf(szText, _AL("%d"), min(1, GetHostPlayer()->GetMoneyAmount()));
	pEdit->SetText(szText);

	if( m_pAUIManager->GetDialog("Win_Storage")->IsShow() || 
		m_pAUIManager->GetDialog("Win_Storage1")->IsShow() ||
		m_pAUIManager->GetDialog("Win_Storage3")->IsShow()
		/*m_pAUIManager->GetDialog("Win_Storage2")->IsShow()*/ )
		pShow->SetType(CDlgInputNO::INPUTNO_STORAGE_IVTR_MONEY);
	else if( m_pAUIManager->GetDialog("Win_Trade")->IsShow() )
		pShow->SetType(CDlgInputNO::INPUTNO_TRADE_MONEY);
	else if( m_pAUIManager->GetDialog("Win_MailWrite")->IsShow() )
		pShow->SetType(CDlgInputNO::INPUTNO_ATTACH_MONEY);
	else
		pShow->SetType(CDlgInputNO::INPUTNO_DROP_MONEY);
}

void CDlgInventory::OnCommand_normalitem(const char * szCommand)
{
	m_nShowIvtrItem = INVENTORY_ITEM_NORMAL;
	m_pBtn_NormalItem->SetPushed(true);
	m_pBtn_NormalItem->SetColor(A3DCOLORRGB(255, 203, 74));
	m_pBtn_QuestItem->SetPushed(false);
	m_pBtn_QuestItem->SetColor(A3DCOLORRGB(255, 255, 255));
	m_pScl_Item->SetBarLevel(0);
}

void CDlgInventory::OnCommand_questitem(const char * szCommand)
{
	m_nShowIvtrItem = INVENTORY_ITEM_TASK;
	m_pBtn_QuestItem->SetPushed(true);
	m_pBtn_QuestItem->SetColor(A3DCOLORRGB(255, 203, 74));
	m_pBtn_NormalItem->SetPushed(false);
	m_pBtn_NormalItem->SetColor(A3DCOLORRGB(255, 255, 255));
	m_pScl_Item->SetBarLevel(0);
}

void CDlgInventory::OnCommand_normalmode(const char * szCommand)
{
	int i;

	char szItem[40];
	for( i = 0; i < SIZE_EQUIPIVTR; i++ )
	{
		if (m_pImgEquip[i])
		{
			m_pImgEquip[i]->Show(!IsFashionSlot(i));

			sprintf(szItem, "Img_%02d", i);
			PAUIOBJECT pImg = GetDlgItem(szItem);
			if(pImg) pImg->Show(m_pImgEquip[i]->IsShow());
		}
	}
	m_pBtn_NormalMode->SetPushed(true);
	m_pBtn_WearMode->SetPushed(false);
	m_pBtn_Meridians->Show(true);
	m_pBtn_FashionShop->Show(false);
}

void CDlgInventory::OnCommand_wearmode(const char * szCommand)
{
	int i;

	char szItem[40];
	for( i = 0; i < SIZE_EQUIPIVTR; i++ )
	{
		if (m_pImgEquip[i])
		{
			m_pImgEquip[i]->Show(IsFashionSlot(i));

			sprintf(szItem, "Img_%02d", i);
			PAUIOBJECT pImg = GetDlgItem(szItem);
			if(pImg) pImg->Show(m_pImgEquip[i]->IsShow());
		}
	}

	m_pBtn_WearMode->SetPushed(true);
	m_pBtn_NormalMode->SetPushed(false);
	m_pBtn_FashionShop->Show(true);
	m_pBtn_Meridians->Show(false);
}

void CDlgInventory::OnCommand_Fashion(const char * szCommand)
{
	GetGameSession()->c2s_CmdFashionSwitch();
}

void CDlgInventory::OnCommand_Try(const char * szCommand)
{
	CDlgFittingRoom *pFittingRoom = GetGameUIMan()->m_pDlgFittingRoom;
	pFittingRoom->Show(!pFittingRoom->IsShow());
}

void CDlgInventory::OnCommand_FashionStore(const char * szCommand)
{
	PAUIDIALOG pDlg = GetGameUIMan()->GetDialog("Win_Storage2");

	// Validate equipment password before shown
	if (!pDlg->IsShow())
		if (!ValidateEquipmentPassword())
			return;

	pDlg->Show(!pDlg->IsShow());
}

void CDlgInventory::OnCommand_CANCEL(const char * szCommand)
{
	PAUIDIALOG pClose = NULL;

	if( GetGameUIMan()->m_pDlgTrade->IsShow() )
	{
		pClose = GetGameUIMan()->m_pDlgTrade;
		int idTrade = (int)pClose->GetDataPtr("int");
		GetGameSession()->trade_Cancel(idTrade);
	}
	else if( GetGameUIMan()->GetDialog("Win_Storage")->IsShow() )
	{
		GetGameUIMan()->GetDialog("Win_Storage")->OnCommand("IDCANCEL");
	}
	else if( GetGameUIMan()->GetDialog("Win_Storage1")->IsShow() )
	{
		GetGameUIMan()->GetDialog("Win_Storage1")->OnCommand("IDCANCEL");
	}
	else if( GetGameUIMan()->GetDialog("Win_Storage2")->IsShow() )
	{
		GetGameUIMan()->GetDialog("Win_Storage2")->OnCommand("IDCANCEL");
	}	
	else if( GetGameUIMan()->GetDialog("Win_Storage3")->IsShow() )
	{
		GetGameUIMan()->GetDialog("Win_Storage3")->OnCommand("IDCANCEL");
	}
	else
	{
		if( GetGameUIMan()->m_pDlgShop->IsShow() )
			pClose = GetGameUIMan()->m_pDlgShop;
		else if( m_pAUIManager->GetDialog("Win_Enchase")->IsShow() )
			pClose = m_pAUIManager->GetDialog("Win_Enchase");
		else if( m_pAUIManager->GetDialog("Win_Disenchase")->IsShow() )
			pClose = m_pAUIManager->GetDialog("Win_Disenchase");
		else if( GetGameUIMan()->m_pDlgProduce->IsShow() 
			&& !GetGameUIMan()->m_pDlgProduce->IsProducing())
			pClose = GetGameUIMan()->m_pDlgProduce;
		else if( GetGameUIMan()->m_pDlgSplit->IsShow() )
			pClose = GetGameUIMan()->m_pDlgSplit;
		else if( GetGameUIMan()->m_pDlgIdentify->IsShow() )
			pClose = GetGameUIMan()->m_pDlgIdentify;
		else if( GetGameUIMan()->m_pDlgBooth2->IsShow() )
			pClose = GetGameUIMan()->m_pDlgBooth2;
		else if( GetGameUIMan()->m_pDlgFaceLift->IsShow() )
			pClose = GetGameUIMan()->m_pDlgFaceLift;
		else if( GetGameUIMan()->m_pDlgPetHatch->IsShow() )
			pClose = GetGameUIMan()->m_pDlgPetHatch;
		else if( GetGameUIMan()->m_pDlgEquipRefine->IsShow() )
			pClose = GetGameUIMan()->m_pDlgEquipRefine;
		else if( GetGameUIMan()->m_pDlgPetRetrain->IsShow() )
			pClose = GetGameUIMan()->m_pDlgPetRetrain;
		else if( GetGameUIMan()->m_pDlgEquipBind->IsShow() )
			pClose = GetGameUIMan()->m_pDlgEquipBind;
		else if( GetGameUIMan()->m_pDlgEquipDestroy->IsShow() )
			pClose = GetGameUIMan()->m_pDlgEquipDestroy;
		else if( GetGameUIMan()->m_pDlgEquipUndestroy->IsShow() )
			pClose = GetGameUIMan()->m_pDlgEquipUndestroy;
		else if( GetGameUIMan()->m_pDlgEquipDye->IsShow() )
			pClose = GetGameUIMan()->m_pDlgEquipDye;
		else if (GetGameUIMan()->m_pDlgAllEquipDye->IsShow())
			pClose = GetGameUIMan()->m_pDlgAllEquipDye;
		else if( GetGameUIMan()->m_pDlgRefineTrans->IsShow() )
			pClose = GetGameUIMan()->m_pDlgRefineTrans;
		else if( GetGameUIMan()->m_pDlgEquipSlot->IsShow() )
			pClose = GetGameUIMan()->m_pDlgEquipSlot;
		else if( GetGameUIMan()->m_pDlgDamageRep->IsShow())
			pClose = GetGameUIMan()->m_pDlgDamageRep;
		else if (GetGameUIMan()->m_pDlgPetDye->IsShow())
			pClose = GetGameUIMan()->m_pDlgPetDye;
		else if (GetGameUIMan()->m_pDlgEngrave->IsShow() && !GetGameUIMan()->m_pDlgEngrave->IsEngraving())
			pClose = GetGameUIMan()->m_pDlgEngrave;		
		else if( GetGameUIMan()->m_pDlgEquipMark->IsShow() )
			pClose = GetGameUIMan()->m_pDlgEquipMark;
		else if( GetGameUIMan()->m_pDlgFashionSplit->IsShow() )
			pClose = GetGameUIMan()->m_pDlgFashionSplit;

		if( pClose ) GetGameUIMan()->EndNPCService();
	}

	Show(false);
	if (GetGameUIMan()->m_pDlgBag->IsShow())
		GetGameUIMan()->m_pDlgBag->Show(false);
	
	GetGameUIMan()->m_bRepairing = false;

	if( pClose )
	{
		pClose->OnCommand("IDCANCEL");
		GetHostPlayer()->GetPack(IVTRTYPE_PACK)->UnfreezeAllItems();
	}
}

int CDlgInventory::GetShowItem()
{
	return m_nShowIvtrItem;
}

void CDlgInventory::SetShowItem(int nShowItem)
{
	m_nShowIvtrItem = nShowItem;
}

bool CDlgInventory::Tick(void)
{
	if( IsShow() ) UpdateInventory();

	return CDlgBase::Tick();
}

bool CDlgInventory::Render()
{
	if(!CDlgBase::Render())
		return false;

	GetGameUIMan()->RenderItemCover((PAUIOBJECT*)m_pImgEquip, SIZE_EQUIPIVTR, NULL);
	GetGameUIMan()->RenderItemCover((PAUIOBJECT*)m_pImgItem, CECDLGSHOP_PACKMAX, NULL);
	return true;
}

bool CDlgInventory::UpdateInventory()
{
	int i, nMax;
	AString strFile;
	ACHAR szText[40];
	CECIvtrEquip *pEquip;
	AUIClockIcon *pClock;
	PAUIIMAGEPICTURE pObj;
	CECInventory *pInventory;
	CECIvtrItem *pItem, *pDrag = NULL;
	PAUIDIALOG pDlgDrag = m_pAUIManager->GetDialog("DragDrop");

	if( pDlgDrag->IsShow() )
	{
		PAUIOBJECT pCell = pDlgDrag->GetDlgItem("Goods");
		PAUIOBJECT pObjSrc = (PAUIOBJECT)pCell->GetDataPtr("ptr_AUIObject");
		PAUIDIALOG pDlgSrc = pObjSrc->GetParent();

		if (pDlgSrc == this ||
			pDlgSrc == GetGameUIMan()->m_pDlgBag ||
			pDlgSrc == GetGameUIMan()->GetDialog("Win_Storage2"))
			pDrag = (CECIvtrItem *)pObjSrc->GetDataPtr("ptr_CECIvtrItem");
	}

	//CheckRadioButton(2, GetHostPlayer()->InFashionMode() ? 2 : 1);
	m_pChkWearMode->Check(!GetHostPlayer()->InFashionMode());
	m_pLblLing->Show(m_pBtn_NormalMode->IsPushed());
	m_pLblMai->Show(m_pBtn_NormalMode->IsPushed());
	
	((PAUISTILLIMAGEBUTTON)GetDlgItem("Btn_Fashion"))->Enable(GetHostPlayer()->GetTrashBox3()->GetSize() > 0);

	pInventory = GetHostPlayer()->GetEquipment();
	for( i = 0; i < SIZE_EQUIPIVTR; i++ )
	{
		pObj = m_pImgEquip[i];
		if( !pObj ) continue;

		pClock = pObj->GetClockIcon();
		pClock->SetProgressRange(0, 1);
		pClock->SetProgressPos(1);
		pItem = pInventory->GetItem(i);
		if( pItem )
		{
			pEquip = (CECIvtrEquip *)pItem;
			if( !GetHostPlayer()->CanUseEquipment(pEquip) 
				|| GetHostPlayer()->IsEquipDisabled(i) )
				pObj->SetColor(A3DCOLORRGB(192, 0, 0));
			else if( i < EQUIPIVTR_FASHION_BODY )
			{
				bool bUseOut = false;
				if (pEquip->GetClassID() == CECIvtrItem::ICID_FLYSWORD)
					bUseOut = ((CECIvtrFlySword*)pEquip)->GetCurTime() ? false : true;
				else if (pEquip->GetClassID() == CECIvtrItem::ICID_WING)
				{
					int iCurMP = GetHostPlayer()->GetBasicProps().iCurMP;
					int iMinMP = GetHostPlayer()->IsFlying() ? 0 : ((CECIvtrWing*)pEquip)->GetLaunchMP();
					bUseOut = iCurMP >= iMinMP ? false : true;
				}
				else
					bUseOut = pEquip->GetCurEndurance() ? false : true;

				if (bUseOut)
					pObj->SetColor(A3DCOLORRGB(128, 128, 128));
				else
					pObj->SetColor(A3DCOLORRGB(255, 255, 255));
			}
			else
				pObj->SetColor(A3DCOLORRGB(255, 255, 255));
			
			if (pObj->GetColor() == A3DCOLORRGB(255, 255, 255) &&
				pEquip->IsDestroying())
				pObj->SetColor(A3DCOLORRGB(128, 128, 128));

			pObj->SetDataPtr(pItem,"ptr_CECIvtrItem");
			pObj->SetData(CECGameUIMan::ICONS_INVENTORY);
			if( pItem->GetCount() > 1 )
			{
				a_sprintf(szText, _AL("%d"), pItem->GetCount());
				pObj->SetText(szText);
			}
			else
				pObj->SetText(_AL(""));

			af_GetFileTitle(pItem->GetIconFile(), strFile);
			strFile.MakeLower();
			pObj->SetCover(GetGameUIMan()->m_pA2DSpriteIcons[CECGameUIMan::ICONS_INVENTORY],
				GetGameUIMan()->m_IconMap[CECGameUIMan::ICONS_INVENTORY][strFile]);

			if( pItem->GetCoolTime(&nMax) > 0 )
			{
				pClock->SetProgressRange(0, nMax);
				pClock->SetProgressPos(nMax - pItem->GetCoolTime());
				pClock->SetColor(A3DCOLORRGBA(0, 0, 0, 128));
			}
		}
		else
		{
			pObj->ClearCover();
			pObj->SetText(_AL(""));
			pObj->SetDataPtr(NULL);
			pObj->SetColor(A3DCOLORRGB(255, 255, 255));
			pObj->SetHint(GetStringFromTable(310 + i));
		}

		if( pDrag && pDrag->CanEquippedTo(i) )
			pObj->SetCover(GetGameUIMan()->m_pA2DSpriteMask, 0, 1);
		else
			pObj->SetCover(NULL, -1, 1);
	}

	if( m_nShowIvtrItem == INVENTORY_ITEM_NORMAL ){
		pInventory = GetHostPlayer()->GetPack();
	}else{
		pInventory = GetHostPlayer()->GetTaskPack();
	}
	int nIvtrSize = pInventory->GetSize();

	bool showBag(false);
	if( nIvtrSize > CECDLGSHOP_PACKMAX)
	{
		if (m_nShowIvtrItem != INVENTORY_ITEM_NORMAL ||
			GetNormalItemShowMode() == NORMAL_ITEM_SHOW_MODE_INTEGRATED)
		{
			// Not for normal item, Or show all normal item in inventory with scroll bar
			//
			m_pScl_Item->SetScrollRange(0, (nIvtrSize - 1) / CECDLGSHOP_PACKLINEMAX - 
				CECDLGSHOP_PACKMAX / CECDLGSHOP_PACKLINEMAX + 1);
			m_pScl_Item->Show(true);
			m_pScl_Item->SetBarLength(-1);
			m_pScl_Item->SetScrollStep(0);
		}
		else
		{
			// For normal item without scroll bar, with expanded space in Win_Bag
			m_pScl_Item->SetBarLevel(0);
			m_pScl_Item->Show(false);
			showBag = true;
		}
	}
	else
	{
		m_pScl_Item->SetScrollRange(0, 0);
		m_pScl_Item->Show(false);
	}	
	
	if (GetGameUIMan()->m_pDlgBag->IsShow() != showBag)
	{
		GetGameUIMan()->m_pDlgBag->Show(showBag);

		// Disable user move for align
		if (showBag)
			GetGameUIMan()->m_pDlgBag->SetCanMove(false);
	}

	int nStart = m_pScl_Item->GetBarLevel() * CECDLGSHOP_PACKLINEMAX;
	if( nStart < atoi(m_pImgItem[0]->GetName() + strlen("Item_")) )
	{
		for(i = 0; i < m_nTotalItem; i++)
		{
			char szName[20];
			sprintf(szName, "Item_%02d", nStart + i);
			m_pImgItem[i]->SetName(szName);
		}
	}
	else if( nStart > atoi(m_pImgItem[0]->GetName() + strlen("Item_")) )
	{
		for(i = m_nTotalItem - 1; i >= 0; i--)
		{
			char szName[20];
			sprintf(szName, "Item_%02d", nStart + i);
			m_pImgItem[i]->SetName(szName);
		}
	}

	for( i = 0; i < m_nTotalItem; i++ )
	{
		pObj = m_pImgItem[i];
		if( !pObj ) break;

		if( nStart + i >= GetHostPlayer()->GetPack()->GetSize() )
		{
			m_pImgItem[i]->Show(false);
			continue;
		}
		else
			m_pImgItem[i]->Show(true);

		pClock = pObj->GetClockIcon();
		pClock->SetProgressRange(0, 1);
		pClock->SetProgressPos(1);

		if( i < nIvtrSize && (pItem = pInventory->GetItem(i + nStart)) )
		{
			pObj->SetDataPtr(pItem,"ptr_CECIvtrItem");
			pObj->SetData(CECGameUIMan::ICONS_INVENTORY);
			pObj->SetRenderHintCallback(CDlgItemDesc::CallBackGeneralCard, reinterpret_cast<unsigned long>(pItem), NULL, NULL);
			if( pItem->GetCount() > 1 )
			{
				a_sprintf(szText, _AL("%d"), pItem->GetCount());
				pObj->SetText(szText);
			}
			else
				pObj->SetText(_AL(""));

			af_GetFileTitle(pItem->GetIconFile(), strFile);
			strFile.MakeLower();
			if( pItem->IsFrozen() )
				pObj->SetColor(A3DCOLORRGB(128, 128, 128));
			else if( pItem->IsEquipment() )
			{
				if( pItem->GetClassID() == CECIvtrItem::ICID_FASHION )
					pObj->SetColor(A3DCOLORRGB(255, 255, 255));
				else
				{
					pEquip = (CECIvtrEquip *)pItem;
					if( !GetHostPlayer()->CanUseEquipment(pEquip) )
						pObj->SetColor(A3DCOLORRGB(192, 0, 0));
					else
					{
						bool bUseOut = false;
						if (pEquip->GetClassID() == CECIvtrItem::ICID_FLYSWORD)
							bUseOut = ((CECIvtrFlySword*)pEquip)->GetCurTime() ? false : true;
						else if (pEquip->GetClassID() == CECIvtrItem::ICID_WING)
						{
							int iCurMP = GetHostPlayer()->GetBasicProps().iCurMP;
							int iMinMP = GetHostPlayer()->IsFlying() ? 0 : ((CECIvtrWing*)pEquip)->GetLaunchMP();
							bUseOut = iCurMP >= iMinMP ? false : true;
						}
						else if( pEquip->GetClassID() != CECIvtrItem::ICID_ARROW )
							bUseOut = pEquip->GetCurEndurance() ? false : true;

						if (bUseOut)
							pObj->SetColor(A3DCOLORRGB(128, 128, 128));
						else
							pObj->SetColor(A3DCOLORRGB(255, 255, 255));
					}

					if (pObj->GetColor() == A3DCOLORRGB(255, 255, 255) &&
						pEquip->IsDestroying())
						pObj->SetColor(A3DCOLORRGB(128, 128, 128));
				}
			}
			else
			{
				if( pItem->IsUseable() && !pItem->CheckUseCondition() )
					pObj->SetColor(A3DCOLORRGB(192, 0, 0));
				else
					pObj->SetColor(A3DCOLORRGB(255, 255, 255));
			}

			if( pItem->GetCoolTime(&nMax) > 0 )
			{
				pClock->SetProgressRange(0, nMax);
				pClock->SetProgressPos(nMax - pItem->GetCoolTime());
				pClock->SetColor(A3DCOLORRGBA(0, 0, 0, 128));
			}
			pObj->SetCover(GetGameUIMan()->m_pA2DSpriteIcons[CECGameUIMan::ICONS_INVENTORY],
				GetGameUIMan()->m_IconMap[CECGameUIMan::ICONS_INVENTORY][strFile]);
		}
		else
		{
			pObj->SetText(_AL(""));
			pObj->SetDataPtr(NULL);
			pObj->SetRenderHintCallback(CDlgItemDesc::CallBackGeneralCard, NULL, NULL, NULL);
			pObj->SetCover(NULL, -1);
			pObj->SetColor(A3DCOLORRGB(255, 255, 255));
		}
	}

	m_pTxt_Gold->SetColor(GetGameUIMan()->GetPriceColor(GetHostPlayer()->GetMoneyAmount()));
	m_pTxt_Gold->SetText(GetGameUIMan()->GetFormatNumber(GetHostPlayer()->GetMoneyAmount()));

	bool enableBagSwitch = (GetHostPlayer()->GetPack()->GetSize() > CECDLGSHOP_PACKMAX);
	m_pChkBag->Enable(enableBagSwitch);
	
	return true;
}

void CDlgInventory::OnEventLButtonDown(WPARAM wParam, LPARAM lParam, AUIObject *pObj)
{
	if( !pObj->GetDataPtr("ptr_CECIvtrItem") )
		return;

	if(strstr(pObj->GetName(), "Item_"))
	{
		CDlgGeneralCard* pCard = dynamic_cast<CDlgGeneralCard*>(
			GetAUIManager()->GetDialog("Win_GeneralCard"));
		int iDst = atoi(pObj->GetName() + strlen("Item_"));
		CECIvtrItem* pStored = (CECIvtrItem*)pObj->GetDataPtr("ptr_CECIvtrItem");
		if (pCard && 
			pCard->IsSwallowing() && 
			pCard->SwallowCard(1, iDst, pStored->GetCount(), false))
			return;
	}
	bool bLinkItem(false);

	if( m_nShowIvtrItem == CDlgInventory::INVENTORY_ITEM_TASK )
	{
		if(AUI_PRESS(VK_SHIFT))
		{
			// 按住 SHIFT，向聊天栏输入框添加物品图
			//
			bLinkItem = true;
		}
	}
	else
	{
		CECIvtrItem *pItem = (CECIvtrItem *)pObj->GetDataPtr("ptr_CECIvtrItem");
		if( GetGameUIMan()->m_bRepairing )
		{
			if( !pItem->IsEquipment() )
				return;
			
			CECIvtrEquip *pEquip = (CECIvtrEquip *)pItem;
			if( pEquip->GetCurEndurance() >= pEquip->GetMaxEndurance() )
				return;
			
			if(!pItem->IsRepairable())
			{
				g_pGame->GetGameRun()->AddFixedMessage(FIXMSG_ITEM_UNREPAIRABLE);
			}
			else if( strstr(pObj->GetName(), "Equip_") )
			{
				GetGameSession()->c2s_CmdNPCSevRepair(IVTRTYPE_EQUIPPACK, 
					atoi(pObj->GetName() + strlen("Equip_")), pItem->GetTemplateID());
			}
			else
			{
				
				GetGameSession()->c2s_CmdNPCSevRepair(IVTRTYPE_PACK,
					atoi(pObj->GetName() + strlen("Item_")), pItem->GetTemplateID());
			}
		}
		else if(AUI_PRESS(VK_SHIFT) )
		{
			// 按住 SHIFT，向聊天栏输入框添加物品图
			//
			bLinkItem = true;
		}
		else if( !pItem->IsFrozen() )
		{
			A3DVIEWPORTPARAM *p = m_pA3DEngine->GetActiveViewport()->GetParam();
			
			GetGameUIMan()->m_ptLButtonDown.x = GET_X_LPARAM(lParam) - p->X;
			GetGameUIMan()->m_ptLButtonDown.y = GET_Y_LPARAM(lParam) - p->Y;
			GetGameUIMan()->InvokeDragDrop(this, pObj, GetGameUIMan()->m_ptLButtonDown);
			GetGameUIMan()->PlayItemSound((CECIvtrItem *)pObj->GetDataPtr("ptr_CECIvtrItem"), false);
		}
	}
	if (bLinkItem)
	{
		// 按住 SHIFT，向聊天栏输入框添加装备图
		//
		int nPack(-1), nSlot(-1);
		if (strstr(pObj->GetName(), "Equip_"))
		{
			// 装备包裹
			nPack = IVTRTYPE_EQUIPPACK;
			nSlot = atoi(pObj->GetName() + strlen("Equip_"));
		}
		else if (strstr(pObj->GetName(), "Item_"))
		{
			if (m_nShowIvtrItem == CDlgInventory::INVENTORY_ITEM_TASK)
			{
				// 任务包裹
				nPack = IVTRTYPE_TASKPACK;
			}
			else
			{
				// 普通包裹
				nPack = IVTRTYPE_PACK;
			}
			nSlot = atoi(pObj->GetName() + strlen("Item_"));
		}
		GetGameUIMan()->LinkItem(nPack, nSlot);
	}
}

CECIvtrItem* CDlgInventory::GetSelectItem(PAUIOBJECT pObj)
{
	if( !pObj->GetDataPtr("ptr_CECIvtrItem") || GetGameUIMan()->m_bRepairing )
		return NULL;
	if( m_nShowIvtrItem == CDlgInventory::INVENTORY_ITEM_TASK )
		return NULL;
	
	return (CECIvtrItem *)pObj->GetDataPtr("ptr_CECIvtrItem");
}

void CDlgInventory::OnEventLButtonDblclkItem(WPARAM wParam, LPARAM lParam, AUIObject *pObj)
{
	CECIvtrItem *pItem = GetSelectItem(pObj);

	if(!pItem || !strstr(pObj->GetName(), "Item_") || pItem->IsFrozen())
	{
		return;
	}

	int iSrc = atoi(pObj->GetName() + strlen("Item_"));

	if( CDlgAutoHelp::IsAutoHelp() )
		return;

	// try to trigger shortcuts
	if(GetGameUIMan()->GetShortcutMgr()->TriggerShortcuts(this->GetName(), pObj))
		return;

	if (!GetHostPlayer()->IsTrading() && !ValidateEquipmentExchange(iSrc, pItem))
		return;

	if( pItem->IsEquipment() && GetHostPlayer()->CanUseEquipment((CECIvtrEquip*)pItem) &&
		((CECIvtrEquip*)pItem)->GetProcType() & CECIvtrItem::PROC_BIND )
	{
		PAUIDIALOG pMsgBox;
		GetGameUIMan()->MessageBox("Game_EquipBind2", GetStringFromTable(872), 
			MB_YESNO, A3DCOLORRGBA(255, 255, 255, 160), &pMsgBox);
		pMsgBox->SetData(iSrc);
		return;
	}	

	GetHostPlayer()->UseItemInPack(IVTRTYPE_PACK, iSrc);
}

void CDlgInventory::OnEventLButtonDblclkEquip(WPARAM wParam, LPARAM lParam, AUIObject *pObj)
{
	if (GetHostPlayer()->IsTrading())
	{
		return;
	}

	CECIvtrItem *pItem = GetSelectItem(pObj);

	if(!pItem || !strstr(pObj->GetName(), "Equip_") || pItem->IsFrozen())
	{
		return;
	}

	int iSrc = atoi(pObj->GetName() + strlen("Equip_"));

	if (!ValidateEquipmentTakeOff(iSrc))
		return;

	// try to trigger shortcuts
	if(!GetGameUIMan()->GetShortcutMgr()->TriggerShortcuts("Win_Inventory_Equip", pObj)&&!CDlgAutoHelp::IsAutoHelp())
	{
		GetHostPlayer()->UseItemInPack(IVTRTYPE_EQUIPPACK, iSrc);
	}
}

void CDlgInventory::OnEventMouseWheel(WPARAM wParam, LPARAM lParam, AUIObject *pObj)
{
	int x = GET_X_LPARAM(lParam);
	int y = GET_Y_LPARAM(lParam);
	int zDelta = (short)HIWORD(wParam);
	if( zDelta > 0 )
		zDelta = 1;
	else
		zDelta = -1;
	if( m_pScl_Item->IsShow() )
		m_pScl_Item->SetBarLevel(m_pScl_Item->GetBarLevel() - zDelta);
}

bool CDlgInventory::ValidateEquipmentPassword()
{
	// Validate equipment password to protect players
	// Called when need to validate equipment password
	// Return false when a password validation dialog is to appear
	//

	// Pop up validation dialog at need
	if (GetHostPlayer()->NeedPassword())
	{
		PAUIDIALOG pValidateDlg = GetGameUIMan()->GetDialog("Win_InputString10");
		pValidateDlg->GetDlgItem("DEFAULT_Txt_Input")->SetText(_AL(""));
		pValidateDlg->Show(true, true);
		return false;
	}
	
	// Return true otherwise
	//
	return true;
}

bool CDlgInventory::ValidateEquipmentTakeOff(int equipmentPos)
{
	// Validate equipment take off to protect players, OR
	// the game state is not suitable for equipment take off
	// Called before an equipment is to take off
	// Return false when a password validation dialog is to appear, OR
	// the game state does not allow
	// Return true otherwise
	//

	switch(equipmentPos)
	{
	case EQUIPIVTR_AUTOHP:
		GetGameUIMan()->MessageBox("", GetStringFromTable(829), MB_OK, A3DCOLORRGBA(255, 255, 255, 160), NULL);
		return false;
	case EQUIPIVTR_AUTOMP:
		GetGameUIMan()->MessageBox("", GetStringFromTable(830), MB_OK, A3DCOLORRGBA(255, 255, 255, 160), NULL);
		return false;
	//case EQUIPIVTR_FORCE_TOKEN:
	//	GetGameUIMan()->MessageBox("", GetStringFromTable(9424), MB_OK, A3DCOLORRGBA(255, 255, 255, 160), NULL);
	//	return false;
	default:
		break;
	}

	// Validate parameter
	if (equipmentPos < 0 ||
		equipmentPos >= SIZE_ALL_EQUIPIVTR)
		return true;

	// Validate game state
	if (!ValidateEquipmentPositionChange(equipmentPos))
		return false;

	// Ignore some unimportant equipment
	static int s_IgnoreEquipments[] = 
	{
		EQUIPIVTR_AUTOHP,
		EQUIPIVTR_AUTOMP,
		EQUIPIVTR_POCKET,
		EQUIPIVTR_GOBLIN,
		EQUIPIVTR_CERTIFICATE,
		EQUIPIVTR_SPEAKER,
		EQUIPIVTR_RUNE,
		EQUIPIVTR_PROJECTILE,
		EQUIPIVTR_FORCE_TOKEN,
		EQUIPIVTR_DYNSKILLEQUIP1,
		EQUIPIVTR_DYNSKILLEQUIP2,
	};
	for (int i = 0; i < sizeof(s_IgnoreEquipments)/sizeof(s_IgnoreEquipments[0]); ++ i)
		if (equipmentPos == s_IgnoreEquipments[i])
			return true;

	// Validate password
	return ValidateEquipmentPassword();
}

bool CDlgInventory::ValidateEquipmentExchange(int iSrc, CECIvtrItem *pItem, int exchangePos)
{
	// Validate equipment take off to protect players, OR
	// the game state is not suitable for equipment to change
	// Called when an item is about to exchange given position equipment
	// Return false when a password validation dialog is to appear, OR
	// the game state does not allow
	//
	CECHostPlayer *pHost = GetHostPlayer();

	if(pItem)
	{
		// do more check on some specific item type
		PAUIDIALOG pMsgBox;
		if( pItem->GetClassID() == CECIvtrItem::ICID_AUTOHP )
		{
			GetGameUIMan()->MessageBox("Game_ExchangeAutoItem", GetStringFromTable(pHost->GetPack(IVTRTYPE_EQUIPPACK)->GetItem(EQUIPIVTR_AUTOHP) != NULL ? 831 : 907), MB_OKCANCEL, A3DCOLORRGBA(255, 255, 255, 160), &pMsgBox);
			pMsgBox->SetData(iSrc);
			return false;
		}
		if( pItem->GetClassID() == CECIvtrItem::ICID_AUTOMP )
		{
			GetGameUIMan()->MessageBox("Game_ExchangeAutoItem", GetStringFromTable(pHost->GetPack(IVTRTYPE_EQUIPPACK)->GetItem(EQUIPIVTR_AUTOMP) != NULL ? 832 : 908), MB_OKCANCEL, A3DCOLORRGBA(255, 255, 255, 160), &pMsgBox);
			pMsgBox->SetData(iSrc);
			return false;
		}
		//if( pItem->GetClassID() == CECIvtrItem::ICID_FORCETOKEN && 
		//	pHost->GetPack(IVTRTYPE_EQUIPPACK)->GetItem(EQUIPIVTR_FORCE_TOKEN) != NULL )
		//{
		//	GetGameUIMan()->MessageBox("", GetStringFromTable(9425), MB_OK, A3DCOLORRGBA(255, 255, 255, 160), &pMsgBox);
		//	return false;
		//}
	}

	if (exchangePos >= 0 &&
		exchangePos < SIZE_ALL_EQUIPIVTR &&
		pItem &&
		pItem->IsEquipment() &&
		pItem->CanEquippedTo(exchangePos) &&
		pHost->CanUseEquipment((CECIvtrEquip *)pItem))
	{
		// Find the equipment to exchange (already equipped)
		CECInventory *pInventory = pHost->GetPack(IVTRTYPE_EQUIPPACK);
		CECIvtrItem *pItemTakeOff = pInventory->GetItem(exchangePos);
		
		// Validate when exchangePos is not empty
		if (pItemTakeOff)
		{
			if (pItemTakeOff != pItem)
			{
				// Ignore if both already equipped
				for (int i = 0; i < SIZE_ALL_EQUIPIVTR; ++ i)
				{
					if (pItem == pInventory->GetItem(i))
						return true;
				}
					
				// Validate the equipment take off
				return ValidateEquipmentTakeOff(exchangePos);
			}
		}
		else
		{			
			// Validate when exchangePos is empty
			return ValidateEquipmentPositionChange(exchangePos);
		}
	}

	// Return true otherwise
	//
	return true;
}

bool CDlgInventory::ValidateEquipmentExchange(int iSrc, CECIvtrItem *pItem)
{
	// Validate equipment take off to protect players, OR
	// the game state is not suitable for equipment to change
	// Called when an item is about to exchange some equipment in the pack
	// Return false when a password validation dialog is to appear, OR
	// the game state does not allow
	//
	if (pItem &&
		pItem->IsEquipment() &&
		GetHostPlayer()->CanUseEquipment((CECIvtrEquip *)pItem))
	{
		// Find exchange position
		int exchangePos(0);
		for (exchangePos = 0; exchangePos < SIZE_ALL_EQUIPIVTR; ++ exchangePos)
			if (pItem->CanEquippedTo(exchangePos))
				break;

		// Validate
		if (exchangePos < SIZE_ALL_EQUIPIVTR)
			return ValidateEquipmentExchange(iSrc, pItem, exchangePos);
	}

	// Return true otherwise
	//
	return true;
}

bool CDlgInventory::ValidateEquipmentShortcut(CECIvtrItem *pItem)
{
	// Validate equipment take off to protect players, OR
	// the game state is not suitable for equipment to change
	// Called when pItem is a short cut
	// Return false when a password validation dialog is to appear, OR
	// the game state does not allow
	// This method differs from other validate* method in that a equipment
	// (already equipped) shortcut execute causes equipment take off
	// and a equipment(not equipped) shortcut execute causes equipment exchange
	//
	if (pItem &&
		pItem->IsEquipment() &&
		GetHostPlayer()->CanUseEquipment((CECIvtrEquip *)pItem))
	{
		// Find target position
		int targetPos(0);
		for (targetPos = 0; targetPos < SIZE_ALL_EQUIPIVTR; ++ targetPos)
		{
			if (pItem->CanEquippedTo(targetPos))
				break;
		}

		if (targetPos < SIZE_ALL_EQUIPIVTR)
		{
			// Validate equipment take off or exchange
			
			// when target item exists
			if (GetHostPlayer()->GetPack(IVTRTYPE_EQUIPPACK)->GetItem(targetPos))
				return ValidateEquipmentTakeOff(targetPos);

			// when is to wear on at the target position
			return ValidateEquipmentPositionChange(targetPos);
		}
	}

	// Return true otherwise
	//
	return true;
}

bool CDlgInventory::ValidateEquipmentPositionChange(int equipmentPos)
{
	// Validate equipment modification
	// (cause may be wear-on, take-off, exchange, discard, etc.)
	// Called in drag and drop, or similar operations like in quick bar,
	// mouse buttons, etc.
	// Return false when the validation FAILED!
	// NOTE: game state is validated here ONLY!

	// Validate fashion change
	if (IsFashionSlot(equipmentPos))
	{
		CECHostPlayer *pHost = GetHostPlayer();
		if (pHost->GetAutoFashion())
		{
			// Do not allow change in auto fashion mode
			GetGameUIMan()->AddChatMessage(GetStringFromTable(7830), GP_CHAT_MISC);
			return false;
		}
	}

	// Pass validation
	return true;
}

bool CDlgInventory::IsFashionSlot(int iSlot)
{
	static int s_FashionEquipments[] = 
	{
		EQUIPIVTR_FASHION_BODY,
			EQUIPIVTR_FASHION_LEG,
			EQUIPIVTR_FASHION_FOOT,
			EQUIPIVTR_FASHION_WRIST,
			EQUIPIVTR_FASHION_HEAD,
			EQUIPIVTR_FASHION_WEAPON
	};
	int *begin = s_FashionEquipments;
	int *end = begin + sizeof(s_FashionEquipments)/sizeof(s_FashionEquipments[0]);
	return std::find(begin, end, iSlot) != end;
}

CDlgInventory::NormalItemShowMode CDlgInventory::GetNormalItemShowMode()
{
	//PAUICHECKBOX pCheck1 = (PAUICHECKBOX)(GetDlgItem("Rdo_Bag1"));
	return m_pChkBag->IsChecked()
		? NORMAL_ITEM_SHOW_MODE_SEPERATED
		: NORMAL_ITEM_SHOW_MODE_INTEGRATED;
}

void CDlgInventory::OnCommand_arrange(const char *szCommand)
{
	CECHostPlayer *pHostPlayer = GetHostPlayer();
	if (pHostPlayer->GetCoolTime(GP_CT_MULTI_EXCHANGE_ITEM))
	{
		g_pGame->GetGameRun()->AddFixedMessage(FIXMSG_CMD_INCOOLTIME);
		return;
	}
	pHostPlayer->SortPack(IVTRTYPE_PACK);
}

void CDlgInventory::OnCommand_meridian(const char *szCommand)
{
	int iLevel = GetHostPlayer()->GetMaxLevelSofar();
	if (iLevel < 40)
		GetGameUIMan()->MessageBox("",GetStringFromTable(10225), MB_OK, A3DCOLORRGBA(255, 255, 255, 160));
	else
		GetGameUIMan()->m_pDlgMeridians->Show(true);
}

void CDlgInventory::OnCommand_FashionShop(const char *szCommand)
{
	CECUIHelper::ShowFashionShop(true);
}

void CDlgInventory::OnCommand_GeneralCard(const char* szCommand)
{
	if( g_pGame->GetConfigs()->IsMiniClient() )
	{
		GetGameUIMan()->MessageBox("", GetStringFromTable(10713), MB_OK, A3DCOLORRGBA(255, 255, 255, 160));
		return;
	}

	PAUIDIALOG pDlg = GetGameUIMan()->GetDialog("Win_GeneralCard");
	if (pDlg) pDlg->Show(!pDlg->IsShow());
}
// for notify user the layout changing is over
void CDlgInventory::OnChangeLayoutEnd(bool bAllDone)
{
	// refresh the buttons
	if(m_pBtn_WearMode->IsPushed())
	{
		OnCommand_wearmode(NULL);
	}
	else
	{
		OnCommand_normalmode(NULL);
	}
}

bool CDlgInventory::OnChangeLayoutBegin()
{
	m_pScl_Item->SetBarLevel(0);
	UpdateInventory();
	return true;
}

void CDlgInventory::DropItem(CECIvtrItem* pIvtrSrc, int iSrc, PAUIOBJECT pObjSrc, PAUIOBJECT pObjOver)
{
	if (!GetHostPlayer()->IsInvisible() && !GetHostPlayer()->IsGMInvisible())
	{
		if( AUI_PRESS(VK_MENU) && pIvtrSrc->GetPileLimit() > 1 && pIvtrSrc->GetCount() > 1 )
		{
			GetGameUIMan()->InvokeNumberDialog(pObjSrc, NULL,
				CDlgInputNO::INPUTNO_DROP_ITEM, pIvtrSrc->GetCount());
		}
		else
		{
			ACString strMsg;
			strMsg.Format(GetStringFromTable(8580), pIvtrSrc->GetName());
			PAUIDIALOG pMsgBox;
			GetGameUIMan()->MessageBox("Game_DropIvtrItem", strMsg, MB_YESNO, A3DCOLORRGBA(255, 255, 255, 160), &pMsgBox);
			pMsgBox->SetData(iSrc);
			pMsgBox->SetDataPtr((void *)pIvtrSrc->GetCount());
		}
		GetGameUIMan()->PlayItemSound(pIvtrSrc, true);
	}
	else
	{
		GetGameRun()->AddFixedMessage(FIXMSG_CANNOT_USE_WHEN_INVISIBLE);
	}
}

void CDlgInventory::DropEquip(CECIvtrItem* pIvtrSrc, int iSrc, PAUIOBJECT pObjSrc, PAUIOBJECT pObjOver)
{	
	// Equipment take off validation (drop)
	if (!this->ValidateEquipmentTakeOff(iSrc))
	{
		return;
	}
	
	PAUIDIALOG pMsgBox = NULL;
	ACString strMsg;
	strMsg.Format(GetStringFromTable(8580), pIvtrSrc->GetName());
	GetGameUIMan()->MessageBox("Game_DropEquipItem", strMsg, MB_YESNO, A3DCOLORRGBA(255, 255, 255, 160), &pMsgBox);
	pMsgBox->SetData(iSrc);
}

void CDlgInventory::ExchangeItem(CECIvtrItem* pIvtrSrc, int iSrc, PAUIOBJECT pObjSrc, PAUIOBJECT pObjOver)
{
	CECGameSession* pSession = GetGameSession();

	if( strstr(pObjOver->GetName(), "Item_") )
	{
		int iDst = atoi(pObjOver->GetName() + strlen("Item_"));
		if( iSrc != iDst )
		{
			CECScriptMan * pScriptMan = g_pGame->GetGameRun()->GetUIManager()->GetScriptMan();
			if( pScriptMan )
			{
				pScriptMan->GetContext()->GetCheckState()->SetInventoryItemMoved(true);
			}
			CECIvtrItem *pIvtrDst = (CECIvtrItem *)pObjOver->GetDataPtr("ptr_CECIvtrItem");
			if( pIvtrSrc && pIvtrSrc->GetClassID() == CECIvtrItem::ICID_ELEMENT &&
				pIvtrDst && pIvtrDst->GetClassID() == CECIvtrItem::ICID_FLYSWORD )
			{
				pSession->c2s_CmdGoblinCharge(iSrc, iDst,
					pIvtrSrc->GetCount(), pIvtrDst->GetTemplateID());
				GetGameUIMan()->PlayItemSound(pIvtrSrc, true);
			}
			else if( pIvtrSrc && pIvtrSrc->GetClassID() == CECIvtrItem::ICID_ELEMENT &&
				pIvtrDst && pIvtrDst->GetClassID() == CECIvtrItem::ICID_GOBLIN )
			{
				pSession->c2s_CmdChargeFlySword(iSrc, iDst,
					pIvtrSrc->GetCount(), pIvtrDst->GetTemplateID());
				GetGameUIMan()->PlayItemSound(pIvtrSrc, true);
			}
			else if( !pIvtrDst || !pIvtrDst->IsFrozen() )
			{
				if( pIvtrSrc->GetPileLimit() > 1 && (!pIvtrDst ||
					pIvtrSrc->GetTemplateID() == pIvtrDst->GetTemplateID()) )
				{
					int nMax = pIvtrSrc->GetCount();
					if( pIvtrDst && pIvtrSrc->GetPileLimit() - pIvtrDst->GetCount() < nMax )
						nMax = pIvtrSrc->GetPileLimit() - pIvtrDst->GetCount();

					if( AUI_PRESS(VK_MENU) && nMax > 1 )
					{
						GetGameUIMan()->InvokeNumberDialog(pObjSrc, pObjOver,
							CDlgInputNO::INPUTNO_MOVE_ITEM, max(nMax, 0));
					}
					else if( pIvtrDst && pIvtrSrc->GetTemplateID() == pIvtrDst->GetTemplateID() )
						pSession->c2s_CmdMoveIvtrItem(iSrc, iDst, nMax);
					else
						pSession->c2s_CmdExgIvtrItem(iSrc, iDst);
				}
				else
				{
					pSession->c2s_CmdExgIvtrItem(iSrc, iDst);
				}
				GetGameUIMan()->PlayItemSound(pIvtrSrc, true);
			}
		}
	}
	else if( strstr(pObjOver->GetName(), "Equip_") )
	{
		CECIvtrItem *pIvtrDst = (CECIvtrItem *)pObjOver->GetDataPtr("ptr_CECIvtrItem");

		int iDst = atoi(pObjOver->GetName() + strlen("Equip_"));
		
		// Equipment exchange validation (from Inventory Item to Inventory Equipment)
		CDlgInventory *pDlgInventory = (CDlgInventory *)(GetGameUIMan()->GetDialog("Win_Inventory"));
		if (!pDlgInventory->ValidateEquipmentExchange(iSrc, pIvtrSrc, iDst))
		{
			return;
		}
		
		if( pIvtrSrc && pIvtrSrc->GetClassID() == CECIvtrItem::ICID_ELEMENT &&
			pIvtrDst && pIvtrDst->GetClassID() == CECIvtrItem::ICID_FLYSWORD )
		{
			pSession->c2s_CmdChargeEquipFlySword(iSrc, pIvtrSrc->GetCount());
		}
		else if( pIvtrSrc && pIvtrSrc->GetClassID() == CECIvtrItem::ICID_ELEMENT &&
			pIvtrDst && pIvtrDst->GetClassID() == CECIvtrItem::ICID_GOBLIN )
		{
			pSession->c2s_CmdGoblinChargeEquip(iSrc, pIvtrSrc->GetCount());
		}
		else if (pIvtrSrc && pIvtrSrc->GetClassID() == CECIvtrItem::ICID_SHARPENER && pIvtrDst)
		{
			CECIvtrShapener *pSharpener = (CECIvtrShapener *)pIvtrSrc;
			if (!pSharpener->CanSharpenSlot(iDst))
			{
				GetGameRun()->AddFixedMessage(FIXMSG_CANNOT_SHARPEN_SLOT);
				return;
			}
			int level = g_pGame->GetItemDataMan()->get_item_level(pIvtrDst->GetTemplateID());
			if (level < pSharpener->GetLevel())
			{
				GetGameRun()->AddFixedMessage(FIXMSG_LEVELTOOLOW);
				return;
			}
			unsigned int uSlot = iDst;
			pSession->c2s_CmdUseSharpen(iSrc, pIvtrSrc->GetTemplateID(), iDst);
		}
		else if( pIvtrDst && pIvtrSrc->GetPileLimit() > 1 &&
			pIvtrSrc->GetTemplateID() == pIvtrDst->GetTemplateID() )
		{
			pSession->c2s_CmdMoveItemToEquip(iSrc, iDst);
			GetGameUIMan()->PlayItemSound(pIvtrSrc, true);
		}
		else if( pIvtrSrc->CanEquippedTo(iDst) && 
			pIvtrSrc->IsEquipment() && GetHostPlayer()->CanUseEquipment((CECIvtrEquip*)pIvtrSrc))
		{
			if( pIvtrSrc->GetProcType() & CECIvtrItem::PROC_BIND )
			{
				PAUIDIALOG pMsgBox;
				GetGameUIMan()->MessageBox("Game_EquipBind1", GetStringFromTable(872), 
					MB_YESNO, A3DCOLORRGBA(255, 255, 255, 160), &pMsgBox);
				pMsgBox->SetData(iSrc);
				pMsgBox->SetDataPtr((void*)iDst);
			}
			else
			{
				//booth certification
				if (24 == iDst && GetHostPlayer()->GetBoothState() != 0)
				{
					//maybe need to add fixedmessage
				}
				else
				{
					pSession->c2s_CmdEquipItem(iSrc, iDst);
					GetGameUIMan()->PlayItemSound(pIvtrSrc, true);
				}
			}
		}
	}
}

void CDlgInventory::ExchangeEquip(CECIvtrItem* pIvtrSrc, int iSrc, PAUIOBJECT pObjSrc, PAUIOBJECT pObjOver)
{
	CECGameSession* pSession = GetGameSession();
	PAUIDIALOG pMsgBox = NULL;

	CECIvtrItem *pIvtrDst = (CECIvtrItem *)pObjOver->GetDataPtr("ptr_CECIvtrItem");
	if( !pIvtrDst || !pIvtrDst->IsFrozen() )
	{
		CDlgInventory *pDlgInventory = (CDlgInventory *)(GetGameUIMan()->GetDialog("Win_Inventory"));
		
		if( strstr(pObjOver->GetName(), "Item_") )
		{
			int iDst = atoi(pObjOver->GetName() + strlen("Item_"));
			if( !pIvtrDst )
			{
				// Equipment take off validation (move to normal package)
				if (!pDlgInventory->ValidateEquipmentTakeOff(iSrc))
				{
					return;
				}

				//add for booth certification
				if (24 == iSrc && GetHostPlayer()->GetBoothState() != 0)
				{
					//maybe need to add fixedmessage
				}
				else
				{
					pSession->c2s_CmdEquipItem(iDst, iSrc);
					GetGameUIMan()->PlayItemSound(pIvtrSrc, true);
				}
			}
			else
			{
				// Equipment take off validation (exchange with normal pack item)
				if (!pDlgInventory->ValidateEquipmentExchange(iDst, pIvtrDst, iSrc))
				{
					return;
				}

				if( pIvtrDst->CanEquippedTo(iSrc) && pIvtrDst->IsEquipment() && 
					GetHostPlayer()->CanUseEquipment((CECIvtrEquip*)pIvtrDst) )
				{
					if( pIvtrDst->GetProcType() & CECIvtrItem::PROC_BIND )
					{
						GetGameUIMan()->MessageBox("Game_EquipBind1", GetStringFromTable(872), 
							MB_YESNO, A3DCOLORRGBA(255, 255, 255, 160), &pMsgBox);
						pMsgBox->SetData(iDst);
						pMsgBox->SetDataPtr((void*)iSrc);
					}
					else
					{
						//add for booth certification
						if (24 == iSrc && GetHostPlayer()->GetBoothState() != 0)
						{
							//maybe need to add fixedmessage
						}
						else
						{
							pSession->c2s_CmdEquipItem(iDst, iSrc);
							GetGameUIMan()->PlayItemSound(pIvtrSrc, true);
						}
					}
				}
			}
		}
		else if( strstr(pObjOver->GetName(), "Equip_") )
		{
			int iDst = atoi(pObjOver->GetName() + strlen("Equip_"));
			if( iSrc != iDst )
			{
				// Equipment exchange validation (exchange in equipment pack)
				if (!pDlgInventory->ValidateEquipmentExchange(iDst, pIvtrDst, iSrc))
				{
					return;
				}

				pSession->c2s_CmdExgEquipItem(iSrc, iDst);
			}
			GetGameUIMan()->PlayItemSound(pIvtrSrc, true);
		}
	}
}

void CDlgInventory::OnItemDragDrop(CECIvtrItem* pIvtrSrc, int iSrc, PAUIOBJECT pObjSrc, PAUIOBJECT pObjOver)
{
	if(GetHostPlayer()->IsTrading() && pIvtrSrc)
	{
		ExchangeTradeItem(pIvtrSrc, iSrc, pObjSrc, pObjOver);
	}
	else 
	{
		ExchangeItem(pIvtrSrc, iSrc, pObjSrc, pObjOver);
	}
}

void CDlgInventory::ExchangeTradeItem(CECIvtrItem* pIvtrSrc, int iSrc, PAUIOBJECT pObjSrc, PAUIOBJECT pObjOver)
{
	if( !pObjOver || !strstr(pObjOver->GetName(), "Item_") )
	{
		return;
	}

	CECIvtrItem *pIvtrDst = (CECIvtrItem *)pObjOver->GetDataPtr("ptr_CECIvtrItem");
	if( !pIvtrDst || !pIvtrDst->IsFrozen() )
	{
		int iDst = atoi(pObjOver->GetName() + strlen("Item_"));
		
		if( iSrc != iDst )
		{
			if( AUI_PRESS(VK_MENU) && pIvtrSrc->GetPileLimit() > 1 && pIvtrSrc->GetCount() > 1 )
			{
				int nMax = pIvtrSrc->GetCount();
				if( pIvtrDst && pIvtrSrc->GetPileLimit() - pIvtrDst->GetCount() < nMax )
					nMax = pIvtrSrc->GetPileLimit() - pIvtrDst->GetCount();
				
				GetGameUIMan()->InvokeNumberDialog(pObjSrc, pObjOver,
					CDlgInputNO::INPUTNO_TRADE_MOVE, max(nMax, 0));
			}
			else
			{
				GetGameSession()->trade_MoveItem(GetGameUIMan()->m_idTrade, pIvtrSrc->GetTemplateID(), iSrc, iDst, pIvtrSrc->GetCount());
			}
		}
		GetGameUIMan()->PlayItemSound(pIvtrSrc, true);
	}
}

void CDlgInventory::OnShowDialog()
{
	CDlgBase::OnShowDialog();
	m_pBtn_Meridians->SetFlash(GetHostPlayer()->NeedMeridiansGuide());
}

void CDlgInventory::SetBagModeSplit() {
	m_pChkBag->Check(true);
}

void CDlgInventory::OnCommand_Booth(const char* szCommand) {
	GetGameRun()->GetTradeCmdShortcuts()->GetShortcut(1)->Execute();
}