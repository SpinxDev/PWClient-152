/*
* FILE: CDlgOfflineShopStore.cpp
*
* DESCRIPTION: 
*
* CREATED BY: WYD
*
* HISTORY: 
*
* Copyright (c) 2013, All Rights Reserved.
*/

#include "DlgOfflineShopStore.h"
#include "EC_GameUIMan.h"
#include "EC_OfflineShopCtrl.h"
#include "EC_HostPlayer.h"
#include <AUICTranslate.h>
#include <AUIImagePicture.h>
#include <AFI.h>
#include "EC_IvtrEquip.h"
#include "EC_GameSession.h"
#include "DlgInputNO.h"

AUI_BEGIN_EVENT_MAP(CDlgOfflineShopStore, CDlgBase)
AUI_ON_EVENT("SellItem_*",		WM_LBUTTONUP,		OnEventLButtonUp_SellItem)
AUI_END_EVENT_MAP()

AUI_BEGIN_COMMAND_MAP(CDlgOfflineShopStore, CDlgBase)
AUI_ON_COMMAND("Btn_Get", OnCommand_GetMoney)
AUI_ON_COMMAND("Btn_Save", OnCommand_SaveMoney)
AUI_ON_COMMAND("Btn_GetAll",OnCommand_TakeAllItems)
AUI_END_COMMAND_MAP()

CDlgOfflineShopStore::CDlgOfflineShopStore()
{
	
}

CDlgOfflineShopStore::~CDlgOfflineShopStore(){}
bool CDlgOfflineShopStore::OnInitDialog()
{
	m_bGetAllItemsFromStoreToPack = false;

	return CDlgBase::OnInitDialog();
}
void CDlgOfflineShopStore::OnCommand_GetMoney(const char* szCommand)
{
	COfflineShopCtrl* pCtlr = GetHostPlayer()->GetOfflineShopCtrl();
	if(!pCtlr) return;

	CDlgInputNO3* pShow = dynamic_cast<CDlgInputNO3*>(GetGameUIMan()->GetDialog("Win_InputNO3"));
	pShow->SetMoney(1);
	pShow->SetYinPiao(0);
	pShow->SetMaxValue(pCtlr->GetMoneyInStore(),pCtlr->GetYinPiaoInStore());
	pShow->SetType(CDlgInputNO3::INPUTNO_OFFLINESHOP_GETMONEY);
	if(!pShow->IsShow())
		pShow->Show(true,true);
}
void CDlgOfflineShopStore::OnCommand_SaveMoney(const char* szCommand)
{
	CDlgInputNO3* pShow = dynamic_cast<CDlgInputNO3*>(GetGameUIMan()->GetDialog("Win_InputNO3"));
	pShow->SetMoney(1);
	pShow->SetYinPiao(0);
	int nY = min(GetHostPlayer()->GetYinpiaoTotalAmount(),9999);
	pShow->SetMaxValue(GetHostPlayer()->GetMoneyAmount(),nY);
	pShow->SetType(CDlgInputNO3::INPUTNO_OFFLINESHOP_SAVEMONEY);
	if(!pShow->IsShow())
		pShow->Show(true,true);
}

void CDlgOfflineShopStore::OnTick()
{
	UpdateShop();
}
void CDlgOfflineShopStore::UpdateShop()
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

	PAUIDIALOG pDlgMouseOn;
	PAUIOBJECT pObjMouseOn;
	m_pAUIManager->GetMouseOn(&pDlgMouseOn, &pObjMouseOn);
	
	for( i = 0; i < SHOP_COUNT_STORE; i++ )
	{
		sprintf(szItem, "SellItem_%02d", i + 1);
		pObj = (PAUIIMAGEPICTURE)GetDlgItem(szItem);
		if( !pObj ) continue;

		pItem = pCtrl->GetMyStoreItem(i);
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
				GetGameUIMan()->GetIconIndex(CECGameUIMan::ICONS_INVENTORY, strFile));			
		}
		else
		{
			pObj->ClearCover();
			pObj->SetText(_AL(""));
			pObj->SetHint(_AL(""));
			pObj->SetDataPtr(NULL);
		}
	}
	ACString strMoney;

	strMoney.Format(_AL("%d"),pCtrl->GetYinPiaoInStore());
	GetDlgItem("Lbl_Silver1")->SetText(strMoney);

	GetDlgItem("Txt_Worth1")->SetText(GetGameUIMan()->GetFormatNumber(pCtrl->GetMoneyInStore()));
}

void CDlgOfflineShopStore::OnEventLButtonUp_SellItem(WPARAM wParam, LPARAM lParam, AUIObject *pObj)
{
	if(m_bGetAllItemsFromStoreToPack) return;

	if( !pObj->GetDataPtr("ptr_CECIvtrItem") )
		return;

	if (GetHostPlayer()->GetPack()->GetEmptySlotNum()<1)
	{
		GetGameUIMan()->MessageBox("",GetGameUIMan()->GetStringFromTable(704),MB_OK,A3DCOLORRGB(255,255,255));
		return;
	}

	int nIndex = atoi(pObj->GetName() + strlen("SellItem_")) - 1;
	GetGameSession()->OffShop_TakeItemFromStore(nIndex);
}
void CDlgOfflineShopStore::OnCommand_TakeAllItems(const char* szCommand)
{
	COfflineShopCtrl* pCtrl = GetHostPlayer()->GetOfflineShopCtrl();
	if(!pCtrl) return;

	if (pCtrl->GetStoreEmptyNum()<SHOP_COUNT_STORE && GetHostPlayer()->GetPack()->GetEmptySlotNum()>0)
	{
		pCtrl->StoreSnapShot();
		pCtrl->TakeItemFromStoreToPack();
		m_bGetAllItemsFromStoreToPack = true;
	}	
}
//////////////////////////////////////////////////////////////////////////

/*
void CDlgInputNO3::OnCommand_add(const char * szCommand)
{
	ACHAR szText[40];

	int nMax = (int)m_pText1->GetData();
//	int nNum = a_atoi(pEdit->GetText());
	int nNum = GetValue();

	if( nNum < nMax ) nNum++;
//	a_sprintf(szText, _AL("%d"), nNum);
//	pEdit->SetText(szText);
	SetValue(nNum);

	ChangeFocus(m_pText1);
}

void CDlgInputNO3::OnCommand_minus(const char * szCommand)
{
	ACHAR szText[40];
//	PAUIOBJECT pEdit = GetDlgItem("DEFAULT_Txt_No.");
	int nMax = (int)m_pText1->GetData();
//	int nNum = a_atoi(pEdit->GetText());
	int nNum = GetValue();

	if( nNum > 0 ) nNum--;
//	a_sprintf(szText, _AL("%d"), nNum);
//	pEdit->SetText(szText);
	SetValue(nNum);

	ChangeFocus(m_pText1);
}
*/