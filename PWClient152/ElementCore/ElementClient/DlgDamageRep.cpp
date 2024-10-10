// File		: DlgDamageRep.cpp
// Creator	: Xu Wenbin
// Date		: 2009/07/10

#include "AFI.h"
#include "AUICTranslate.h"
#include "DlgDamageRep.h"
#include "EC_ShortcutMgr.h"
#include "EC_GameUIMan.h"
#include "EC_GameSession.h"
#include "EC_HostPlayer.h"
#include "EC_IvtrItem.h"
#include "EC_Inventory.h"
#include "EC_IvtrEquip.h"
#include "EC_IvtrTypes.h"
#include "EC_IvtrWeapon.h"
#include "EC_IvtrArmor.h"
#include "EC_IvtrDecoration.h"
#include "EC_IvtrFlySword.h"
#include "EC_IvtrEquipMatter.h"
#include "ExpTypes.h"

#define new A_DEBUG_NEW

AUI_BEGIN_COMMAND_MAP(CDlgDamageRep, CDlgBase)

AUI_ON_COMMAND("confirm",		OnCommand_Confirm)
AUI_ON_COMMAND("IDCANCEL",		OnCommand_CANCEL)

AUI_END_COMMAND_MAP()

AUI_BEGIN_EVENT_MAP(CDlgDamageRep, CDlgBase)

AUI_ON_EVENT("Img_Equip",	WM_LBUTTONDOWN,	OnEventLButtonDown)
AUI_ON_EVENT("Img_Equip",	WM_RBUTTONDOWN,	OnEventLButtonDown)

AUI_END_EVENT_MAP()

//------------------------------------------------------------------------
// Right-Click Shortcut for CDlgDamageRep
//------------------------------------------------------------------------
typedef CECShortcutMgr::SimpleClickShortcut<CDlgDamageRep> DamageRepClickShortcut;
//------------------------------------------------------------------------

CDlgDamageRep::CDlgDamageRep()
{
	m_pBtn_Confirm = NULL;
	m_pImg_Equip = NULL;
	m_pImg_Dye = NULL;
	m_pTxt_Num = NULL;
	m_nSlot = -1;
}

CDlgDamageRep::~CDlgDamageRep()
{
}

bool CDlgDamageRep::OnInitDialog()
{
	DDX_Control("Txt_Num", m_pTxt_Num);
	DDX_Control("Img_Equip", m_pImg_Equip);
	DDX_Control("Img_Dye", m_pImg_Dye);
	DDX_Control("Btn_Confirm", m_pBtn_Confirm);	

	GetGameUIMan()->GetShortcutMgr()->RegisterShortCut(new DamageRepClickShortcut(this));
	return true;
}

void CDlgDamageRep::OnShowDialog()
{
	ClearAll();
}

void CDlgDamageRep::OnTick()
{
	// We need update dye here because they're not frozen and likely to change
	UpdateDye();
	UpdateUI();
}

void CDlgDamageRep::ClearAll()
{
	SetEquip(-1);
}

void CDlgDamageRep::OnCommand_Confirm(const char *szCommand)
{
	CECIvtrItem *pEquip = (CECIvtrItem *)m_pImg_Equip->GetDataPtr();
	if (pEquip)
	{
		GetGameSession()->c2s_CmdNPCSevRepairDestroyingItem(m_nSlot);
		ClearAll();
	}
}

void CDlgDamageRep::OnCommand_CANCEL(const char *szCommand)
{
	ClearAll();
	GetGameUIMan()->EndNPCService();
	Show(false);
	GetGameUIMan()->GetDialog("Win_Inventory")->Show(false);
	GetHostPlayer()->GetPack(IVTRTYPE_PACK)->UnfreezeAllItems();
}

void CDlgDamageRep::OnEventLButtonDown(WPARAM wParam, LPARAM lParam, AUIObject * pObj)
{
	if (!stricmp(pObj->GetName(), "Img_Equip"))
		SetEquip(-1);
}

void CDlgDamageRep::SetEquip(int nSlot)
{
	if (nSlot < 0)
	{
		CECIvtrItem *pItem = (CECIvtrItem *)m_pImg_Equip->GetDataPtr();
		if (pItem)
			pItem->Freeze(false);
		m_nSlot = -1;
		m_pImg_Equip->SetDataPtr(NULL);
		m_pImg_Equip->ClearCover();
		m_pImg_Equip->SetHint(_AL(""));

		UpdateDye();
		UpdateUI();
		return;
	}

	CECInventory *pInventory = GetHostPlayer()->GetPack(IVTRTYPE_PACK);
	CECIvtrItem *pItem = pInventory->GetItem(nSlot);
	if (!CanAcceptEquip(pItem))
	{
		GetGameUIMan()->AddChatMessage(GetStringFromTable(7920), GP_CHAT_MISC);
		return;
	}

	// Clear old
	SetEquip(-1);

	// Set new equipment
	m_pImg_Equip->SetDataPtr(pItem);
	m_nSlot = nSlot;
	pItem->Freeze(true);

	AString strIconFile;
	af_GetFileTitle(pItem->GetIconFile(), strIconFile);
	strIconFile.MakeLower();
	m_pImg_Equip->SetCover(GetGameUIMan()->m_pA2DSpriteIcons[CECGameUIMan::ICONS_INVENTORY],
		GetGameUIMan()->m_IconMap[CECGameUIMan::ICONS_INVENTORY][strIconFile]);
	m_pImg_Equip->SetColor(
		((CECIvtrEquip *)pItem)->IsDestroying()
		? A3DCOLORRGB(128, 128, 128)
		: A3DCOLORRGB(255, 255, 255));

	AUICTranslate trans;
	const ACHAR *szDesc = pItem->GetDesc();
	m_pImg_Equip->SetHint(szDesc ? trans.Translate(szDesc) : _AL(""));

	// Update new dye
	UpdateDye();

	// Update UI
	UpdateUI();
}

void CDlgDamageRep::UpdateDye()
{
	CECIvtrItem *pEquip = (CECIvtrItem *)m_pImg_Equip->GetDataPtr();
	if (!pEquip)
	{
		m_pImg_Dye->ClearCover();
		m_pImg_Dye->SetHint(_AL(""));
		return;
	}

	CECIvtrItem *pItem = NULL;
	int id(0), num(0);
	if (GetRepairIDAndNum(pEquip, id, num))
		pItem = CECIvtrItem::CreateItem(id, 0, 1);
	if (!pItem ||
		pItem->GetClassID() == CECIvtrItem::ICID_ERRORITEM)
	{
		// Equipment doesn't need any dye to repair
		m_pImg_Dye->ClearCover();
		m_pImg_Dye->SetHint(_AL(""));
		delete pItem;
		return;
	}

	// Clear old dye
	m_pImg_Dye->ClearCover();
	m_pImg_Dye->SetHint(_AL(""));

	// Set new dye
	AString strIconFile;
	af_GetFileTitle(pItem->GetIconFile(), strIconFile);
	strIconFile.MakeLower();
	m_pImg_Dye->SetCover(GetGameUIMan()->m_pA2DSpriteIcons[CECGameUIMan::ICONS_INVENTORY],
		GetGameUIMan()->m_IconMap[CECGameUIMan::ICONS_INVENTORY][strIconFile]);
	
	AUICTranslate trans;
	const ACHAR *szDesc = pItem->GetDesc();
	m_pImg_Dye->SetHint(szDesc ? trans.Translate(szDesc) : _AL(""));

	// Remove temporary data
	delete pItem;
}

void CDlgDamageRep::UpdateUI()
{
	bool confirmEnable(false);
	int currentNum(0), desiredNum(0);

	CECIvtrEquip *pEquip = (CECIvtrEquip *)m_pImg_Equip->GetDataPtr();
	if (pEquip)
	{
		int id(0);
		GetRepairIDAndNum(pEquip, id, desiredNum);
		CECInventory *pInventory = GetHostPlayer()->GetPack(IVTRTYPE_PACK);
		currentNum = pInventory->GetItemTotalNum(id);
		if (currentNum >= desiredNum &&
			pEquip->IsDestroying())
			confirmEnable = true;
	}

	// Update confirm button enable state
	m_pBtn_Confirm->Enable(confirmEnable);

	// Update num text
	A3DCOLOR COLOR_WHITE = A3DCOLORRGB(255, 255, 255);
	A3DCOLOR COLOR_RED = A3DCOLORRGB(255, 0, 0);
	if (pEquip)
	{
		m_pTxt_Num->SetColor(confirmEnable ? COLOR_WHITE : COLOR_RED);
		ACHAR szText[20];
		a_sprintf(szText, _AL("%d/%d"), desiredNum, currentNum);
		m_pTxt_Num->SetText(szText);
	}
	else
	{
		m_pTxt_Num->SetColor(COLOR_WHITE);
		m_pTxt_Num->SetText(_AL(""));
	}
}

bool CDlgDamageRep::CanAcceptEquip(CECIvtrItem *pItem)
{
	static const int s_ValidClassID[] = 
	{
		CECIvtrItem::ICID_WEAPON,
			CECIvtrItem::ICID_ARMOR,
			CECIvtrItem::ICID_DECORATION,
			CECIvtrItem::ICID_FLYSWORD,
			CECIvtrItem::ICID_BIBLE
	};

	if (pItem &&
		pItem->IsEquipment())
	{
		int classID = pItem->GetClassID();
		for (int i = 0; i < sizeof(s_ValidClassID)/sizeof(s_ValidClassID[0]); ++ i)
			if (classID == s_ValidClassID[i])
				return true;
	}

	return false;
}

bool CDlgDamageRep::GetRepairIDAndNum(CECIvtrItem *pItem, int &id, int &num)
{
	bool result(false);

	if (pItem)
	{
		switch (pItem->GetClassID())
		{
		case CECIvtrItem::ICID_WEAPON:
			{
				const WEAPON_ESSENCE * pEssence = ((CECIvtrWeapon *)pItem)->GetDBEssence();
				id = (int)pEssence->id_drop_after_damaged;
				num = pEssence->num_drop_after_damaged;
				result = true;
			}
			break;

		case CECIvtrItem::ICID_ARMOR:
			{
				const ARMOR_ESSENCE * pEssence = ((CECIvtrArmor *)pItem)->GetDBEssence();
				id = (int)pEssence->id_drop_after_damaged;
				num = pEssence->num_drop_after_damaged;
				result = true;
			}
			break;			

		case CECIvtrItem::ICID_DECORATION:
			{
				const DECORATION_ESSENCE * pEssence = ((CECIvtrDecoration *)pItem)->GetDBEssence();
				id = (int)pEssence->id_drop_after_damaged;
				num = pEssence->num_drop_after_damaged;
				result = true;
			}
			break;			

		case CECIvtrItem::ICID_FLYSWORD:
			{
				const FLYSWORD_ESSENCE * pEssence = ((CECIvtrFlySword *)pItem)->GetDBEssence();
				id = (int)pEssence->id_drop_after_damaged;
				num = pEssence->num_drop_after_damaged;
				result = true;
			}
			break;

		case CECIvtrItem::ICID_BIBLE:
			{
				const BIBLE_ESSENCE * pEssence = ((CECIvtrBible *)pItem)->GetDBEssence();
				id = (int)pEssence->id_drop_after_damaged;
				num = pEssence->num_drop_after_damaged;
				result = true;
			}
			break;
		}

		if (result)
		{
			num = (int)ceil(num * 1.2);
		}
	}

	return result;
}

void CDlgDamageRep::OnItemDragDrop(CECIvtrItem* pIvtrSrc, int iSrc, PAUIOBJECT pObjSrc, PAUIOBJECT pObjOver)
{
	// no need to check name "Img_Equip", only one available drag-drop target
	this->SetEquip(iSrc);
}