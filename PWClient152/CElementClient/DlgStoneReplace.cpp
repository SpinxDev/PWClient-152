// File		: DlgStoneReplace.cpp 
// Creator	: Wang and Shi
// Date		: 2013/01/14

#include "DlgStoneReplace.h"
#include "AUIEditBox.h"
#include "AUIImagePicture.h"
#include "EC_ShortcutMgr.h"
#include "EC_GameSession.h"
#include "EC_GameUIMan.h"
#include "EC_HostPlayer.h"
#include "EC_Inventory.h"
#include "EC_IvtrItem.h"
#include "EC_IvtrStone.h"
#include "ExpTypes.h"
#include "EC_IvtrEquip.h"
#include "DlgStoneChange.h"
#include "AFI.h"
#include "EC_Game.h"
#include "EC_Global.h"
#include "EC_IvtrWeapon.h"
#include "EC_IvtrArmor.h"
#include "EC_IvtrDecoration.h"

#define new A_DEBUG_NEW

AUI_BEGIN_COMMAND_MAP(CDlgStoneReplace, CDlgBase)

AUI_ON_COMMAND("IDCANCEL",		OnCommandCANCEL)
AUI_ON_COMMAND("Btn_Replace",	OnCommandReplace)

AUI_END_COMMAND_MAP()


AUI_BEGIN_EVENT_MAP(CDlgStoneReplace, CDlgBase)

AUI_ON_EVENT("Img_Gem", WM_LBUTTONDOWN,	OnEventLButtonDown)
AUI_ON_EVENT("Img_Gem", WM_LBUTTONDBLCLK,OnEventLButtonDown)

AUI_END_EVENT_MAP()

//------------------------------------------------------------------------
// Right-Click Shortcut for CDlgStoneReplace
//------------------------------------------------------------------------
typedef CECShortcutMgr::SimpleClickShortcut<CDlgStoneReplace> InstallClickShortcut;
//------------------------------------------------------------------------

CDlgStoneReplace::CDlgStoneReplace()
{
	m_pItem = NULL;
	m_pStoneName = NULL;
	m_pStoneAttri = NULL;
	m_pReaplceBtn = NULL;
}

CDlgStoneReplace::~CDlgStoneReplace()
{
}

bool CDlgStoneReplace::OnInitDialog()
{
	GetGameUIMan()->GetShortcutMgr()->RegisterShortCut(new InstallClickShortcut(this));
	return CDlgBase::OnInitDialog();
}

void CDlgStoneReplace::OnShowDialog()
{
	OnEventLButtonDown(0, 0, m_pItem);
}
void CDlgStoneReplace::OnHideDialog()
{
	OnEventLButtonDown(0, 0, m_pItem);
}
void CDlgStoneReplace::DoDataExchange(bool bSave)
{
	CDlgBase::DoDataExchange(bSave);

	DDX_Control("Img_Gem", m_pItem);
	DDX_Control("Btn_Replace",m_pReaplceBtn);
	DDX_Control("Lbl_Attribute",m_pStoneAttri);
	DDX_Control("Lbl_StoneName",m_pStoneName);
}

void CDlgStoneReplace::OnCommandCANCEL(const char * szCommand)
{
	Show(false);
	OnEventLButtonDown(0, 0, m_pItem);
}

void CDlgStoneReplace::OnCommandReplace(const char * szCommand)
{
	CECHostPlayer *pHost = GetHostPlayer();

	CECIvtrItem* pItem = (CECIvtrItem*)m_pItem->GetDataPtr("ptr_CECIvtrItem");
	if( ! pItem) return;

	CECIvtrStone* newStone = dynamic_cast<CECIvtrStone*>(pItem);
	if (newStone)
	{
		int nStoneLevel = newStone->GetDBEssence()->level;
		int nEquipLevel = -1;
		int iSlot = GetGameUIMan()->m_pDlgStoneChange->GetEquipSlot();
		CECIvtrItem* pEquipItem = GetHostPlayer()->GetPack()->GetItem(iSlot);
		switch( pEquipItem->GetClassID() )
		{
		case CECIvtrItem::ICID_WEAPON:
			nEquipLevel = ((CECIvtrWeapon*)pEquipItem)->GetDBEssence()->level;
			break;
		case CECIvtrItem::ICID_ARMOR:
			nEquipLevel = ((CECIvtrArmor*)pEquipItem)->GetDBEssence()->level;
			break;
		case CECIvtrItem::ICID_DECORATION:
			nEquipLevel = ((CECIvtrDecoration*)pEquipItem)->GetDBEssence()->level;
			break;
		}
		
		if( nStoneLevel > nEquipLevel )
		{
			GetGameUIMan()->MessageBox("", GetGameUIMan()->GetStringFromTable(10177), MB_OK,
				A3DCOLORRGBA(255, 255, 255, 160));
		
			return;
		}

		int iOldStone = GetGameUIMan()->m_pDlgStoneChange->GetOldStoneTid();
		CECIvtrItem* pStoneItem = CECIvtrItem::CreateItem(iOldStone,0,1);
		CECIvtrStone* pStone = dynamic_cast<CECIvtrStone*>(pStoneItem);
		int iOldStoneLevel = -1;
		
		if (pStone)		
			iOldStoneLevel = pStone->GetDBEssence()->level;			
		
		delete pStoneItem;

		if (nStoneLevel< iOldStoneLevel )
		{
			GetGameUIMan()->MessageBox("", GetGameUIMan()->GetStringFromTable(10178), MB_OK,
				A3DCOLORRGBA(255, 255, 255, 160));			

			return;
		}

		GetGameUIMan()->MessageBox("StoneReplace",GetGameUIMan()->GetStringFromTable(10173),MB_OKCANCEL,A3DCOLORRGB(255,255,255));
	}
}
void CDlgStoneReplace::DoReplace()
{
	CDlgStoneChange* pDlgChange = GetGameUIMan()->m_pDlgStoneChange;
	int iEqupSlot = pDlgChange->GetEquipSlot();
	int iHoleIndex = pDlgChange->GetHoleIndex();
	int iOldTid = pDlgChange->GetOldStoneTid();

	int iNewSlot = m_pItem->GetData();

	void *ptr = NULL;
	AString szType;
	m_pItem->ForceGetDataPtr(ptr,szType);
	CECIvtrItem* pItem = (CECIvtrItem*)ptr;
	
	int iNewTid = pItem ? pItem->GetTemplateID():0;

	g_pGame->GetGameSession()->c2s_CmdNPCSevStoneReplace(iEqupSlot,iHoleIndex,iOldTid,iNewSlot,iNewTid);


	OnEventLButtonDown(0, 0, m_pItem);
}
void CDlgStoneReplace::OnEventLButtonDown(WPARAM wParam, LPARAM lParam, AUIObject * pObj)
{
	if( !pObj ) return;

	PAUIIMAGEPICTURE pImage = (PAUIIMAGEPICTURE)pObj;
	CECIvtrItem *pItem = (CECIvtrItem *)pObj->GetDataPtr("ptr_CECIvtrItem");
	if (pItem)
	{
		pItem->Freeze(false);
		GetGameUIMan()->PlayItemSound(pItem, false);
	}
	
	pImage->SetDataPtr(NULL);	
	pImage->SetCover(NULL, -1);
	pImage->SetData(0);

	GetDlgItem("Lbl_StoneName")->SetText(_AL(""));
	GetDlgItem("Lbl_Attribute")->SetText(_AL(""));	

	if(GetDlgItem("Txt_Gold"))
		GetDlgItem("Txt_Gold")->SetText(_AL(""));

	m_pReaplceBtn->Enable(FALSE);
}

void CDlgStoneReplace::OnItemDragDrop(CECIvtrItem* pIvtrSrc, int iSrc, PAUIOBJECT pObjSrc, PAUIOBJECT pObjOver)
{
	if(pIvtrSrc->GetClassID() != CECIvtrItem::ICID_STONE)	
		return;	

	// for right-click scene
	if(!pObjOver)
		pObjOver = this->GetDlgItem("Img_Gem");	
	
	// for drag-drop scene
	if(!pObjOver || strcmp(pObjOver->GetName(), "Img_Gem"))
	{
		return;
	}

	OnEventLButtonDown(0, 0, pObjOver);
	
	pIvtrSrc->Freeze(true);
	pObjOver->SetData(iSrc);
// 	void *ptr;
// 	AString szType;
// 	pObjSrc->ForceGetDataPtr(ptr,szType);
	pObjOver->SetDataPtr(pIvtrSrc,"ptr_CECIvtrItem");
	GetGameUIMan()->PlayItemSound(pIvtrSrc, true);
	
	AString strFile;
	af_GetFileTitle(pIvtrSrc->GetIconFile(), strFile);
	strFile.MakeLower();
	PAUIIMAGEPICTURE pImage = (PAUIIMAGEPICTURE)pObjOver;
	pImage->SetCover( GetGameUIMan()->GetIconCover(CECGameUIMan::ICONS_INVENTORY),
					  GetGameUIMan()->GetIconIndex(CECGameUIMan::ICONS_INVENTORY, strFile) );
	
	pImage->SetColor(
		(pIvtrSrc->IsEquipment() && ((CECIvtrEquip *)pIvtrSrc)->IsDestroying())
		? A3DCOLORRGB(128, 128, 128)
		: A3DCOLORRGB(255, 255, 255));
	
	CECIvtrStone *pStone = dynamic_cast<CECIvtrStone *>(pIvtrSrc);
	if(!pStone) return;

	STONE_ESSENCE *pEssence = (STONE_ESSENCE *)pStone->GetDBEssence();

	bool IsWeapon = GetGameUIMan()->m_pDlgStoneChange->IsWeapon();

	m_pStoneAttri->SetText(IsWeapon ? pEssence->weapon_desc: pEssence->armor_desc);
	m_pStoneName->SetText(pStone->GetName());

	if(GetDlgItem("Txt_Gold"))
	{
		int iOldStone = GetGameUIMan()->m_pDlgStoneChange->GetOldStoneTid();
		CECIvtrItem* pStoneItem = CECIvtrItem::CreateItem(iOldStone,0,1);
		CECIvtrStone* pStone = dynamic_cast<CECIvtrStone*>(pStoneItem);		
		int uninstall = 0;
		if (pStoneItem && pStone)	
		{
			uninstall = pStone->GetDBEssence()->uninstall_price;
			delete pStoneItem;
		}
		uninstall += pEssence->install_price;
		ACString str;
		str.Format(_AL("%d"),uninstall);
		GetDlgItem("Txt_Gold")->SetText(str);

		bool bNoMoney = GetHostPlayer()->GetMoneyAmount()<uninstall;
		GetDlgItem("Txt_Gold")->SetColor(bNoMoney ? A3DCOLORRGB(255,0,0):A3DCOLORRGB(0,255,0));

		m_pReaplceBtn->Enable(!bNoMoney);
	}
	else
		m_pReaplceBtn->Enable(TRUE);
}