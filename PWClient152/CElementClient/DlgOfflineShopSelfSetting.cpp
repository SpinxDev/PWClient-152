/*
* FILE: DlgOfflineShopSelfSetting.cpp
*
* DESCRIPTION: 
*
* CREATED BY: WYD
*
* HISTORY: 
*
* Copyright (c) 2013, All Rights Reserved.
*/

#include "DlgOfflineShopSelfSetting.h"
#include <AFI.h>
#include <AUIDef.h>
#include "EC_ShortcutMgr.h"
#include "EC_GameSession.h"
#include "EC_GameUIMan.h"
#include "DlgOfflineShopStore.h"
#include "EC_HostPlayer.h"
#include <AUICTranslate.h>
#include <AUIImagePicture.h>
#include "EC_OfflineShopCtrl.h"
#include "EC_IvtrEquip.h"
#include "EC_Global.h"
#include "DlgInputNO.h"
#include "DlgInventory.h"
#include "DlgOfflineShopCreate.h"

AUI_BEGIN_EVENT_MAP(CDlgOfflineShopSelfSetting, CDlgBase)
AUI_ON_EVENT("SellItem_*",		WM_LBUTTONUP,		OnEventLButtonUp_SellItem)
AUI_ON_EVENT("BuyItem_*",		WM_LBUTTONUP,		OnEventLButtonUp_BuyItem)
AUI_END_EVENT_MAP()

AUI_BEGIN_COMMAND_MAP(CDlgOfflineShopSelfSetting, CDlgBase)
AUI_ON_COMMAND("Btn_Reset",		OnCommand_ResetItems)
AUI_ON_COMMAND("Btn_Set",		OnCommand_Store)
AUI_ON_COMMAND("IDCANCEL",		OnCommandCANCEL)
AUI_ON_COMMAND("Btn_Cancel",	OnCommandCANCEL)
AUI_ON_COMMAND("Btn_Close",		OnCommandCANCEL)
AUI_ON_COMMAND("Btn_Type",		OnCommand_Type)
AUI_END_COMMAND_MAP()

//------------------------------------------------------------------------
// Right-Click Shortcut for CDlgOfflineShopSelfSetting
//------------------------------------------------------------------------

//------------------------------------------------------------------------
class OfflineShopClickShortcut : public CECShortcutMgr::SimpleClickShortcut<CDlgOfflineShopSelfSetting>
{
	typedef CECShortcutMgr::SimpleClickShortcut<CDlgOfflineShopSelfSetting> Base;
public:
	OfflineShopClickShortcut(CDlgOfflineShopSelfSetting* pDlg):Base(pDlg){};
	virtual bool CanTrigger(PAUIOBJECT pSrcObj)
	{
		return Base::CanTrigger(pSrcObj) && 
			// skip this trigger if this window shown
			!m_pDlg->GetAUIManager()->GetDialog("Win_ShopCreate")->IsShow();
	}
};


CDlgOfflineShopSelfSetting::CDlgOfflineShopSelfSetting()
{

}

CDlgOfflineShopSelfSetting::~CDlgOfflineShopSelfSetting(){}

void CDlgOfflineShopSelfSetting::OnCommand_ResetItems(const char* szCommand)
{
	GetGameUIMan()->MessageBox("OfflineShopClear",GetGameUIMan()->GetStringFromTable(10503),MB_YESNO,A3DCOLORRGB(255,255,255));
}
void CDlgOfflineShopSelfSetting::OnCommand_Store(const char* szCommand)
{
	if(!GetGameUIMan()->m_pDlgOffShopStore->IsShow())
		GetGameUIMan()->m_pDlgOffShopStore->Show(true);
}
void CDlgOfflineShopSelfSetting::OnShowDialog()
{
	AlignTo(GetGameUIMan()->m_pDlgInventory,
		AUIDIALOG_ALIGN_OUTSIDE, AUIDIALOG_ALIGN_LEFT,
		AUIDIALOG_ALIGN_INSIDE, AUIDIALOG_ALIGN_TOP,
				-60 , 60 );
}
bool CDlgOfflineShopSelfSetting::OnInitDialog()
{
	GetGameUIMan()->GetShortcutMgr()->RegisterShortCut(new OfflineShopClickShortcut(this));

	return CDlgBase::OnInitDialog();
}
void CDlgOfflineShopSelfSetting::OnTick()
{
	UpdateShop();
}
void CDlgOfflineShopSelfSetting::OnCommand_Type(const char *szCommand)
{
	COfflineShopCtrl* pCtrl = GetHostPlayer()->GetOfflineShopCtrl();
	if(!pCtrl) return;

	CDlgOfflineShopCreate* pDlg = GetGameUIMan()->m_pDlgOffShopCreate;
	pDlg->SetDlgType(false);
	pDlg->SetShopType(pCtrl->GetMyShopType());	
	pDlg->Show(true);
}
void CDlgOfflineShopSelfSetting::OnCommandCANCEL(const char *szCommand)
{	
	GetHostPlayer()->EndNPCService();
	if(GetGameUIMan()->m_pDlgOffShopStore->IsShow())
		GetGameUIMan()->m_pDlgOffShopStore->Show(false);

	if (GetGameUIMan()->m_pDlgOffShopCreate->IsShow())
		GetGameUIMan()->m_pDlgOffShopCreate->OnCommandCANCEL(NULL);

	Show(false);
}
void CDlgOfflineShopSelfSetting::UpdateShop()
{
	if( !IsShow() ) return;

	COfflineShopCtrl* pCtrl = GetHostPlayer()->GetOfflineShopCtrl();
	if(!pCtrl) return;

	int i;
	AString strFile;
	char szItem[40] = {0};
	ACHAR szText[40] = {0};
	CECIvtrItem *pItem = NULL;
	AUICTranslate trans;
	PAUIIMAGEPICTURE pObj = NULL;
	__int64 nMoney1 = 0, nMoney2 = 0;

	PAUIDIALOG pDlgMouseOn;
	PAUIOBJECT pObjMouseOn;
	m_pAUIManager->GetMouseOn(&pDlgMouseOn, &pObjMouseOn);
	
	for( i = 0; i < SHOP_COUNT_PERPAGE; i++ )
	{
		sprintf(szItem, "SellItem_%02d", i + 1);
		pObj = (PAUIIMAGEPICTURE)GetDlgItem(szItem);
		if( !pObj ) continue;

		pItem = pCtrl->GetMyShopItem(true,i);
		pObj->SetColor(A3DCOLORRGB(255, 255, 255));
		if( pItem )
		{
			pObj->SetDataPtr(pItem, "ptr_CECIvtrItem");

			int iCount = pItem->GetCount();
			if( iCount > 1 )
			{
				a_sprintf(szText, _AL("%d"), iCount);
				pObj->SetText(szText);
			}
			else
				pObj->SetText(_AL(""));

			if( pDlgMouseOn && pDlgMouseOn == this &&
				pObjMouseOn && pObjMouseOn == pObj )
			{
				ACString strHint = GetGameUIMan()->GetItemDescHint(pItem, true, true);
				pObj->SetHint(strHint);
			}

			if( pItem->IsFrozen() )
				pObj->SetColor(A3DCOLORRGB(128, 128, 128));

			if (pItem->IsEquipment() &&	((CECIvtrEquip *)pItem)->IsDestroying())
				pObj->SetColor(A3DCOLORRGB(128, 128, 128));
			
			af_GetFileTitle(pItem->GetIconFile(), strFile);
			strFile.MakeLower();
			pObj->SetCover(GetGameUIMan()->GetIconCover(CECGameUIMan::ICONS_INVENTORY),
				GetGameUIMan()->GetIconIndex(CECGameUIMan::ICONS_INVENTORY, strFile));
			nMoney1 += (unsigned int)pItem->GetUnitPrice() * (__int64)iCount;
		}
		else
		{
			pObj->ClearCover();
			pObj->SetText(_AL(""));
			pObj->SetHint(_AL(""));
			pObj->SetDataPtr(NULL);
		}
	}

	for( i = 0; i < SHOP_COUNT_PERPAGE; i++ )
	{
		sprintf(szItem, "BuyItem_%02d", i + 1);
		pObj = (PAUIIMAGEPICTURE)GetDlgItem(szItem);
		if( !pObj ) continue;

		pItem = pCtrl->GetMyShopItem(false,i);
		pObj->SetColor(A3DCOLORRGB(255, 255, 255));
		if( pItem )
		{
			pObj->SetDataPtr(pItem,"ptr_CECIvtrItem");

			int iCount = pItem->GetCount();
			if( iCount > 1 )
			{
				a_sprintf(szText, _AL("%d"), iCount);
				pObj->SetText(szText);
			}
			else
				pObj->SetText(_AL(""));

			if( pDlgMouseOn && pDlgMouseOn == this &&
				pObjMouseOn && pObjMouseOn == pObj )
			{
				const wchar_t* szDesc = pItem->GetDesc();
				if( szDesc )
					pObj->SetHint(trans.Translate(szDesc));
				else
					pObj->SetHint(pItem->GetName());
			}

			if( pItem->IsFrozen() )
				pObj->SetColor(A3DCOLORRGB(128, 128, 128));

			if (pItem->IsEquipment() &&	((CECIvtrEquip *)pItem)->IsDestroying())
				pObj->SetColor(A3DCOLORRGB(128, 128, 128));
			
			af_GetFileTitle(pItem->GetIconFile(), strFile);
			strFile.MakeLower();
			pObj->SetCover(GetGameUIMan()->GetIconCover(CECGameUIMan::ICONS_INVENTORY),
				GetGameUIMan()->GetIconIndex(CECGameUIMan::ICONS_INVENTORY,strFile));
			nMoney2 += (unsigned int)pItem->GetUnitPrice() * (__int64)iCount;
		}
		else
		{
			pObj->ClearCover();
			pObj->SetText(_AL(""));
			pObj->SetHint(_AL(""));
			pObj->SetDataPtr(NULL);
		}
	}

	__int64 nNeedYP = nMoney1 / 10000000;			
	__int64 NeedMoney = nMoney1 - nNeedYP * 10000000;

	ACString strMoney;
	strMoney.Format(_AL("%d"),nNeedYP);
	GetDlgItem("Lbl_Silver1")->SetText(strMoney);

	GetDlgItem("Txt_Worth1")->SetColor(GetGameUIMan()->GetPriceColor(NeedMoney));
	GetDlgItem("Txt_Worth1")->SetText(GetGameUIMan()->GetFormatNumber(NeedMoney));


	nNeedYP = nMoney2 / 10000000;			
	NeedMoney = nMoney2 - nNeedYP * 10000000;

	strMoney.Format(_AL("%d"),nNeedYP);
	GetDlgItem("Lbl_Silver2")->SetText(strMoney);

	GetDlgItem("Txt_Worth2")->SetColor(GetGameUIMan()->GetPriceColor(NeedMoney));
	GetDlgItem("Txt_Worth2")->SetText(GetGameUIMan()->GetFormatNumber(NeedMoney));

	// Ê±¼ä
	ACString strTime;
	int left_time = pCtrl->GetMyShopExpiretime() - g_pGame->GetServerGMTTime();
	if(left_time < 0) left_time = 0;
	strTime = pCtrl->IsNormalMyShop() ? GetGameUIMan()->GetFormatTime(left_time):GetGameUIMan()->GetStringFromTable(10504);
	if (!pCtrl->IsNormalMyShop() || left_time ==0)
		GetDlgItem("Lbl_Time")->SetColor(A3DCOLORRGB(255,0,0));
	else
		GetDlgItem("Lbl_Time")->SetColor(A3DCOLORRGB(0,255,0));
	GetDlgItem("Lbl_Time")->SetText(strTime);
}
void CDlgOfflineShopSelfSetting::OnItemDragDrop(CECIvtrItem* pIvtrSrc, int iSrc, PAUIOBJECT pObjSrc, PAUIOBJECT pObjOver)
{
	if(GetGameUIMan()->m_pDlgOffShopCreate->IsShow()) return;

	COfflineShopCtrl* pCtrl = GetHostPlayer()->GetOfflineShopCtrl();
	if(!pCtrl) return;

	// for right-click scene
	if(!pObjOver)
	{
		// click when press control means to buy a item
		bool bSell = !(AUI_PRESS(VK_CONTROL));
		int iSlot = pCtrl->FirstEmptyPosInMyShop(bSell);
		if(iSlot >= 0)
		{
			AString strPrefix = bSell ? "SellItem_" : "BuyItem_";
			AString strName;

			pObjOver = GetDlgItem(strName.Format("%s%02d", strPrefix, iSlot+1));
		}
	}
	
	if( !pIvtrSrc->IsTradeable() )
		GetGameUIMan()->MessageBox("", GetStringFromTable(652), MB_OK, A3DCOLORRGBA(255, 255, 255, 160));		
	else if (pIvtrSrc->IsEquipment() && ((CECIvtrEquip *)pIvtrSrc)->IsDestroying())
	{
		GetGameUIMan()->AddChatMessage(GetStringFromTable(10505), GP_CHAT_MISC);
	}	
	else if( pIvtrSrc->GetExpireDate() != 0 )
	{
		GetGameUIMan()->MessageBox("", GetGameUIMan()->GetStringFromTable(10506), 
			MB_OK, A3DCOLORRGBA(255, 255, 255, 160));		
	}
	else
	{
		ACHAR szText[40];
		PAUIEDITBOX pEdit;

		bool bBuy = ((pObjOver!=NULL) && strstr(pObjOver->GetName(), "BuyItem_"));

		PAUIDIALOG pShow = GetGameUIMan()->GetDialog("Win_InputNO2");

		if(pCtrl->FirstEmptyPosInMyShop(!bBuy)<0)
		{
			GetGameUIMan()->MessageBox("",GetGameUIMan()->GetStringFromTable(bBuy ? 10535:10534),MB_OK,A3DCOLORRGB(255,255,255));
			return;
		}
		if(pCtrl->GetStoreEmptyNum()<1)
		{
			GetGameUIMan()->MessageBox("",GetGameUIMan()->GetStringFromTable(10533),MB_OK,A3DCOLORRGB(255,255,255));
			return;
		}
		if (pCtrl->IsShopFull())
		{
			GetGameUIMan()->MessageBox("",GetGameUIMan()->GetStringFromTable(10536),MB_OK,A3DCOLORRGB(255,255,255));
			return;
		}

		if(bBuy )
		{			
			pShow->SetData(CDlgInputNO::INPUTNO_OFFLINESHOP_BUY);
		}
		else
		{
			pShow->SetData(CDlgInputNO::INPUTNO_OFFLINESHOP_SELL);
		}
			
		pEdit = dynamic_cast<PAUIEDITBOX>(pShow->GetDlgItem("Txt_Price"));
		pEdit->SetIsNumberOnly(true);
		a_sprintf(szText, _AL("%u"), (unsigned int)pIvtrSrc->GetUnitPrice());
		pEdit->SetText(szText);
		pEdit->SetData(iSrc);
		pShow->ChangeFocus(pEdit);
		pEdit->SetMaxLength(13);
			
		pEdit = dynamic_cast<PAUIEDITBOX>(pShow->GetDlgItem("Txt_Amount"));
		pEdit->SetIsNumberOnly(true);
		pEdit->SetText(_AL("1"));
		pEdit->Enable(pIvtrSrc->GetPileLimit() > 0 ? true : false);
			
		pShow->SetDataPtr(pIvtrSrc,"ptr_CECIvtrItem");
		pShow->Show(true, true);
	}
}

void CDlgOfflineShopSelfSetting::OnEventLButtonUp_SellItem(WPARAM wParam, LPARAM lParam, AUIObject *pObj)
{
	if( !pObj->GetDataPtr("ptr_CECIvtrItem") )
		return;

	COfflineShopCtrl* pCtrl = GetHostPlayer()->GetOfflineShopCtrl();
	if(!pCtrl) return;

	int nIndex = atoi(pObj->GetName() + strlen("SellItem_")) - 1;
	g_pGame->GetGameSession()->OffShop_CancelItemFromMyShop(true,nIndex);
}

void CDlgOfflineShopSelfSetting::OnEventLButtonUp_BuyItem(WPARAM wParam, LPARAM lParam, AUIObject *pObj)
{
	if( !pObj->GetDataPtr("ptr_CECIvtrItem") )
		return;
	
	int nIndex = atoi(pObj->GetName() + strlen("BuyItem_")) - 1;
	g_pGame->GetGameSession()->OffShop_CancelItemFromMyShop(false,nIndex);
}
