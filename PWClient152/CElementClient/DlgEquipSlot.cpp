// File		: DlgEquipSlot.cpp
// Creator	: Xiao Zhou
// Date		: 2008/6/3

#include "AFI.h"
#include "AUIEditBox.h"
#include "AUIImagePicture.h"
#include "DlgEquipSlot.h"
#include "EC_ShortcutMgr.h"
#include "EC_GameSession.h"
#include "EC_GameUIMan.h"
#include "EC_HostPlayer.h"
#include "EC_Inventory.h"
#include "EC_IvtrItem.h"
#include "EC_IvtrEquip.h"
#include "EC_IvtrWeapon.h"
#include "EC_IvtrArmor.h"
#include "ExpTypes.h"

#define new A_DEBUG_NEW

AUI_BEGIN_COMMAND_MAP(CDlgEquipSlot, CDlgBase)

AUI_ON_COMMAND("IDCANCEL",		OnCommandCANCEL)
AUI_ON_COMMAND("confirm",		OnCommandConfirm)

AUI_END_COMMAND_MAP()


AUI_BEGIN_EVENT_MAP(CDlgEquipSlot, CDlgBase)

AUI_ON_EVENT("Img_Equip", WM_LBUTTONDOWN,	OnEventLButtonDown)

AUI_END_EVENT_MAP()

//------------------------------------------------------------------------
// Right-Click Shortcut for CDlgEquipSlot
//------------------------------------------------------------------------
typedef CECShortcutMgr::SimpleClickShortcut<CDlgEquipSlot> EquipSlotClickShortcut;
//------------------------------------------------------------------------

CDlgEquipSlot::CDlgEquipSlot()
{
}

CDlgEquipSlot::~CDlgEquipSlot()
{
}

bool CDlgEquipSlot::OnInitDialog()
{
	m_pImg_Equip = (PAUIIMAGEPICTURE)GetDlgItem("Img_Equip");
	m_pTxt_Name = GetDlgItem("Txt_Name");
	m_pTxt_SlotNumber = GetDlgItem("Txt_SlotNumber");
	m_pTxt_Number1 = GetDlgItem("Txt_Number1");

	GetGameUIMan()->GetShortcutMgr()->RegisterShortCut(new EquipSlotClickShortcut(this));
	return true;
}

void CDlgEquipSlot::OnCommandCANCEL(const char * szCommand)
{
	OnEventLButtonDown(0, 0, NULL);
	GetHostPlayer()->EndNPCService();
	GetGameUIMan()->m_pCurNPCEssence = NULL;
	GetGameUIMan()->GetDialog("Win_Inventory")->Show(false);
	GetHostPlayer()->GetPack(IVTRTYPE_PACK)->UnfreezeAllItems();
	Show(false);
}

void CDlgEquipSlot::OnCommandConfirm(const char * szCommand)
{
	CECIvtrItem* pItem = (CECIvtrItem*)m_pImg_Equip->GetDataPtr("ptr_CECIvtrItem");
	if( pItem ) 
	{
		GetGameSession()->c2s_CmdNPCSevMakeSlot(m_pImg_Equip->GetData(), pItem->GetTemplateID());
		OnEventLButtonDown(0, 0, NULL);
	}
}

void CDlgEquipSlot::OnEventLButtonDown(WPARAM wParam, LPARAM lParam, AUIObject * pObj)
{
	CECIvtrItem *pItem = (CECIvtrItem *)m_pImg_Equip->GetDataPtr("ptr_CECIvtrItem");
	if( pItem )
		pItem->Freeze(false);
	m_pImg_Equip->SetDataPtr(NULL);
	
	m_pImg_Equip->SetCover(NULL, -1);
	GetGameUIMan()->PlayItemSound(pItem, false);
	m_pTxt_SlotNumber->SetText(_AL(""));
	m_pTxt_Name->SetText(_AL(""));
	m_pTxt_Number1->SetText(_AL(""));
}

void CDlgEquipSlot::SetEquip(int iSrc)
{
	CECIvtrEquip *pItem = (CECIvtrEquip*)GetHostPlayer()->GetPack()->GetItem(iSrc);
	if( pItem->GetClassID() == CECIvtrItem::ICID_WEAPON && pItem->GetHoleNum() >= 2 ||
		pItem->GetHoleNum() >= 4 )
	{
		GetGameUIMan()->MessageBox("", GetStringFromTable(886), MB_OK, A3DCOLORRGBA(255, 255, 255, 160));
		return;
	}

	PAUIOBJECT pObj = GetDlgItem("Img_Equip");
	pItem->Freeze(true);
	pObj->SetData(iSrc);
	pObj->SetDataPtr(pItem, "ptr_CECIvtrItem");
	PAUIIMAGEPICTURE pImage = (PAUIIMAGEPICTURE)pObj;
	AString strFile;
	af_GetFileTitle(pItem->GetIconFile(), strFile);
	strFile.MakeLower();
	pImage->SetCover(GetGameUIMan()->m_pA2DSpriteIcons[CECGameUIMan::ICONS_INVENTORY],
		GetGameUIMan()->m_IconMap[CECGameUIMan::ICONS_INVENTORY][strFile]);

	pImage->SetColor(
		(pItem->IsEquipment() && ((CECIvtrEquip *)pItem)->IsDestroying())
		? A3DCOLORRGB(128, 128, 128)
		: A3DCOLORRGB(255, 255, 255));

	ACHAR szText[30];
	a_sprintf(szText, _AL("%d"), pItem->GetHoleNum());
	m_pTxt_SlotNumber->SetText(szText);
	m_pTxt_Name->SetText(pItem->GetName());
	int number1 = 0;
	int level = 0;
	if( pItem->GetClassID() == CECIvtrItem::ICID_WEAPON )
	{
		if( pItem->GetHoleNum() == 0 )
			number1 = 5;
		else
			number1 = 10;
		level = ((CECIvtrWeapon*)pItem)->GetDBEssence()->level;
	}
	else
	{
		if( pItem->GetHoleNum() == 0 )
			number1 = 1;
		else if( pItem->GetHoleNum() == 1 )
			number1 = 2;
		else if( pItem->GetHoleNum() == 2 )
			number1 = 3;
		else if( pItem->GetHoleNum() == 3 )
			number1 = 10;
		level = ((CECIvtrArmor*)pItem)->GetDBEssence()->level;
	}
	if( level <= 10 )
		number1 *= level;
	else if( level == 11 )
		number1 *= 100;
	else if( level == 12 )
		number1 *= 200;
	else
		number1 *= (level - 11) * 400;
	a_sprintf(szText, _AL("%d"), number1);
	m_pTxt_Number1->SetText(szText);
	int total = GetHostPlayer()->GetPack()->GetItemTotalNum(21043) + GetHostPlayer()->GetPack()->GetItemTotalNum(34232);
	if( total >= number1 )
	{
		m_pTxt_Number1->SetColor(A3DCOLORRGB(255, 255, 255));
		GetDlgItem("Btn_Confirm")->Enable(true);
	}
	else
	{
		m_pTxt_Number1->SetColor(A3DCOLORRGB(255, 0, 0));
		GetDlgItem("Btn_Confirm")->Enable(false);
	}
}

void CDlgEquipSlot::OnItemDragDrop(CECIvtrItem* pIvtrSrc, int iSrc, PAUIOBJECT pObjSrc, PAUIOBJECT pObjOver)
{
	// no need to check name "Img_Equip", only one available drag-drop target
	if( pIvtrSrc->GetClassID() == CECIvtrItem::ICID_WEAPON ||
		pIvtrSrc->GetClassID() == CECIvtrItem::ICID_ARMOR ) 
	{
		this->SetEquip(iSrc);
	}
}