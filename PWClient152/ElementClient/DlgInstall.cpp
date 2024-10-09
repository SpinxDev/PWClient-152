// File		: DlgInstall.cpp
// Creator	: Xiao Zhou
// Date		: 2005/8/16

#include "DlgInstall.h"
#include "AUIEditBox.h"
#include "AUIImagePicture.h"
#include "EC_ShortcutMgr.h"
#include "EC_GameSession.h"
#include "EC_GameUIMan.h"
#include "EC_HostPlayer.h"
#include "EC_Inventory.h"
#include "EC_IvtrItem.h"
#include "EC_IvtrEquip.h"
#include "EC_IvtrWeapon.h"
#include "EC_IvtrArmor.h"
#include "EC_IvtrDecoration.h"
#include "EC_IvtrStone.h"
#include "ExpTypes.h"
#include "AFI.h"

#include <AUICTranslate.h>

AUI_BEGIN_COMMAND_MAP(CDlgInstall, CDlgBase)

AUI_ON_COMMAND("IDCANCEL",		OnCommandCANCEL)
AUI_ON_COMMAND("start",			OnCommandStart)

AUI_END_COMMAND_MAP()


AUI_BEGIN_EVENT_MAP(CDlgInstall, CDlgBase)

AUI_ON_EVENT("Item_a", WM_LBUTTONDOWN,	OnEventLButtonDown_Equipment)
AUI_ON_EVENT("Item_a", WM_LBUTTONDBLCLK,OnEventLButtonDown_Equipment)
AUI_ON_EVENT("Item_b", WM_LBUTTONDOWN,	OnEventLButtonDown_Stone)
AUI_ON_EVENT("Item_b", WM_LBUTTONDBLCLK,OnEventLButtonDown_Stone)

AUI_END_EVENT_MAP()

//------------------------------------------------------------------------
// Right-Click Shortcut for CDlgInstall
//------------------------------------------------------------------------
typedef CECShortcutMgr::SimpleClickShortcut<CDlgInstall> InstallClickShortcut;
//------------------------------------------------------------------------

CDlgInstall::CDlgInstall()
{
	m_pItema = NULL;
	m_pItemb = NULL;
	m_pTxtGold = NULL;
}

CDlgInstall::~CDlgInstall()
{
}

bool CDlgInstall::OnInitDialog()
{
	GetGameUIMan()->GetShortcutMgr()->RegisterShortCut(new InstallClickShortcut(this));
	return CDlgBase::OnInitDialog();
}

void CDlgInstall::DoDataExchange(bool bSave)
{
	CDlgBase::DoDataExchange(bSave);

	DDX_Control("Item_a", m_pItema);
	if(stricmp("Win_Enchase",GetName()) == 0)
		DDX_Control("Item_b", m_pItemb);
	DDX_Control("Txt_Gold", m_pTxtGold);
}

void CDlgInstall::OnShowDialog()
{
	CDlgBase::OnShowDialog();
	ClearEquipment();
	if (m_pItemb){
		ClearStone();
	}
	m_pTxtGold->SetText(_AL("0"));
}

void CDlgInstall::OnCommandCANCEL(const char * szCommand)
{
	Show(false);
	GetHostPlayer()->EndNPCService();
	GetGameUIMan()->m_pCurNPCEssence = NULL;
	GetGameUIMan()->GetDialog("Win_Inventory")->Show(false);
	GetHostPlayer()->GetPack(IVTRTYPE_PACK)->UnfreezeAllItems();
}

void CDlgInstall::OnCommandStart(const char * szCommand)
{
	PAUIDIALOG pMsgBox;
	CECHostPlayer *pHost = GetHostPlayer();

	if( !m_pItema->GetDataPtr("ptr_CECIvtrItem") ) return;

	int nMoney = a_atoi(m_pTxtGold->GetText());
	if( nMoney > pHost->GetMoneyAmount() )
	{
		GetGameUIMan()->MessageBox("", GetGameUIMan()->GetStringFromTable(226), MB_OK,
			A3DCOLORRGBA(255, 255, 255, 160), &pMsgBox);
		pMsgBox->SetLife(3);
		return;
	}
	
	CECIvtrItem *pIvtrA = (CECIvtrItem *)m_pItema->GetDataPtr("ptr_CECIvtrItem");
	if( !pIvtrA->IsEquipment() )
	{
		GetGameUIMan()->MessageBox("", GetGameUIMan()->GetStringFromTable(223), MB_OK,
			A3DCOLORRGBA(255, 255, 255, 160), &pMsgBox);
		pMsgBox->SetLife(3);
		return;
	}

	CECIvtrEquip *pEquipA = (CECIvtrEquip *)pIvtrA;
	if( 0 == stricmp(GetName(), "Win_Enchase") )
	{
		if( pEquipA->GetEmptyHoleNum() <= 0 )
		{
			GetGameUIMan()->MessageBox("", GetGameUIMan()->GetStringFromTable(224), MB_OK,
				A3DCOLORRGBA(255, 255, 255, 160), &pMsgBox);
			pMsgBox->SetLife(3);
			return;
		}

		CECIvtrItem *pIvtrB = (CECIvtrItem *)m_pItemb->GetDataPtr("ptr_CECIvtrItem");
		if( !pIvtrB || !pIvtrB->IsEmbeddable() )
		{
			GetGameUIMan()->MessageBox("", GetGameUIMan()->GetStringFromTable(225), MB_OK,
				A3DCOLORRGBA(255, 255, 255, 160), &pMsgBox);
			pMsgBox->SetLife(3);
			return;
		}
		
		if( pIvtrB->GetClassID() != CECIvtrItem::ICID_STONE)
			return;
		
		int nStoneLevel = ((CECIvtrStone*)pIvtrB)->GetDBEssence()->level;
		int nEquipLevel = -1;
		switch( pEquipA->GetClassID() )
		{
		case CECIvtrItem::ICID_WEAPON:
			nEquipLevel = ((CECIvtrWeapon*)pEquipA)->GetDBEssence()->level;
			break;
		case CECIvtrItem::ICID_ARMOR:
			nEquipLevel = ((CECIvtrArmor*)pEquipA)->GetDBEssence()->level;
			break;
		case CECIvtrItem::ICID_DECORATION:
			nEquipLevel = ((CECIvtrDecoration*)pEquipA)->GetDBEssence()->level;
			break;
		}

		if( nStoneLevel > nEquipLevel )
		{
			GetGameUIMan()->MessageBox("", GetGameUIMan()->GetStringFromTable(300), MB_OK,
				A3DCOLORRGBA(255, 255, 255, 160), &pMsgBox);
			pMsgBox->SetLife(3);
			return;
		}
		
		GetGameSession()->c2s_CmdNPCSevEmbed(
			(WORD)m_pItemb->GetData(), (WORD)m_pItema->GetData(),
			pIvtrB->GetTemplateID(), pIvtrA->GetTemplateID());
		ClearEquipment();
		ClearStone();
		pHost->GetPack(IVTRTYPE_PACK)->UnfreezeAllItems();

		GetGameUIMan()->MessageBox("", GetGameUIMan()->GetStringFromTable(228),
			MB_OK, A3DCOLORRGBA(255, 255, 255, 160), &pMsgBox);
		pMsgBox->SetLife(3);
	}
	else if( 0 == stricmp(GetName(), "Win_Disenchase") )
	{
		if( pEquipA->GetEmptyHoleNum() == pEquipA->GetHoleNum() )
		{
			GetGameUIMan()->MessageBox("", GetGameUIMan()->GetStringFromTable(227), MB_OK,
				A3DCOLORRGBA(255, 255, 255, 160), &pMsgBox);
			pMsgBox->SetLife(3);
			return;
		}

		GetGameUIMan()->MessageBox("Game_Disenchase", GetGameUIMan()->GetStringFromTable(229),
			MB_OKCANCEL, A3DCOLORRGBA(255, 255, 255, 160));
	}
}

void CDlgInstall::OnEventLButtonDown_Equipment(WPARAM wParam, LPARAM lParam, AUIObject * pObj)
{
	if( !pObj || !pObj->GetDataPtr("ptr_CECIvtrItem") ){
		return;
	}
	ClearEquipment();
}

void CDlgInstall::OnEventLButtonDown_Stone(WPARAM wParam, LPARAM lParam, AUIObject * pObj)
{
	if( !pObj || !pObj->GetDataPtr("ptr_CECIvtrItem") ){
		return;
	}
	ClearStone();
}

void CDlgInstall::ClearItem(PAUIOBJECT pObj){
	PAUIIMAGEPICTURE pImage = dynamic_cast<PAUIIMAGEPICTURE>(pObj);
	if (CECIvtrItem *pItem = (CECIvtrItem *)pObj->GetDataPtr("ptr_CECIvtrItem")){
		pItem->Freeze(false);
		GetGameUIMan()->PlayItemSound(pItem, false);
	}
	pImage->SetDataPtr(NULL);
	pImage->SetCover(NULL, -1);
	pImage->SetHint(_AL(""));
}

void CDlgInstall::ClearEquipment(){
	ClearItem(m_pItema);
	SetEquipmentName(_AL(""));
}

void CDlgInstall::ClearStone(){
	ClearItem(m_pItemb);
	SetStoneName(_AL(""));
}

void CDlgInstall::SetEquipmentName(const ACHAR *szName){
	if (PAUIOBJECT pObj = GetDlgItem("Txt_EquipName")){
		pObj->SetText(szName);
	}
}

void CDlgInstall::SetStoneName(const ACHAR *szName){
	if (PAUIOBJECT pObj = GetDlgItem("Txt_Stone")){
		pObj->SetText(szName);
	}
}

void CDlgInstall::OnItemDragDrop(CECIvtrItem* pIvtrSrc, int iSrc, PAUIOBJECT pObjSrc, PAUIOBJECT pObjOver)
{
	// for right-click scene
	if(!pObjOver)
	{
		if(pIvtrSrc->IsEquipment())
		{
			pObjOver = m_pItema;
		}
		else if(pIvtrSrc->GetClassID() == CECIvtrItem::ICID_STONE)
		{
			pObjOver = m_pItemb;
		}
	}

	// for drag-drop scene
	if(!pObjOver || !strstr(pObjOver->GetName(), "Item_"))
	{
		return;
	}

	if (pObjOver == m_pItema){
		ClearEquipment();
	}else if (pObjOver == m_pItemb){
		ClearStone();
	}
	
	pIvtrSrc->Freeze(true);
	pObjOver->SetData(iSrc);
	void *ptr;
	AString szType;
	pObjSrc->ForceGetDataPtr(ptr,szType);
	pObjOver->SetDataPtr(ptr,szType);
	GetGameUIMan()->PlayItemSound(pIvtrSrc, true);
	
	PAUIIMAGEPICTURE pImage = (PAUIIMAGEPICTURE)pObjOver;
	GetGameUIMan()->SetCover(pImage, pIvtrSrc->GetIconFile());
	pImage->SetHint(AUICTranslate().Translate(pIvtrSrc->GetDesc()));
	pImage->SetColor(
		(pIvtrSrc->IsEquipment() && ((CECIvtrEquip *)pIvtrSrc)->IsDestroying())
		? A3DCOLORRGB(128, 128, 128)
		: A3DCOLORRGB(255, 255, 255));
	
	if (pObjOver == m_pItema){
		SetEquipmentName(pIvtrSrc->GetName());
	}else if (pObjOver == m_pItemb){
		SetStoneName(pIvtrSrc->GetName());
	}
	
	CECIvtrStone *pStone = NULL;
	STONE_ESSENCE *pEssence = NULL;
	ACHAR szText[40] = _AL("0");
	
	if( 0 == stricmp(this->GetName(), "Win_Enchase")
		&& 0 == stricmp(pObjOver->GetName(), "Item_b")
		&& pIvtrSrc->GetClassID() == CECIvtrItem::ICID_STONE )
	{
		pStone = (CECIvtrStone *)pIvtrSrc;
		pEssence = (STONE_ESSENCE *)pStone->GetDBEssence();
		a_sprintf(szText, _AL("%d"), pEssence->install_price);
		m_pTxtGold->SetText(szText);
	}
	else if( 0 == stricmp(this->GetName(), "Win_Disenchase")
		&& pIvtrSrc->IsEquipment() )
	{
		int i, nAmount = 0, idItem;
		CECIvtrEquip *pEquip = (CECIvtrEquip *)pIvtrSrc;
		
		for( i = 0; i < pEquip->GetHoleNum(); i++ )
		{
			idItem = pEquip->GetHoleItem(i);
			if( idItem <= 0 ) continue;
			
			pStone = (CECIvtrStone *)CECIvtrItem::CreateItem(idItem, 0, 1);
			pEssence = (STONE_ESSENCE *)pStone->GetDBEssence();
			nAmount += pEssence->uninstall_price;
			delete pStone;
		}
		
		a_sprintf(szText, _AL("%d"), nAmount);
		m_pTxtGold->SetText(szText);
	}

}