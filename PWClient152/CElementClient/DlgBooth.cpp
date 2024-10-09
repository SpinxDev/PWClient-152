// Filename	: DlgBooth.cpp
// Creator	: Tom Zhou
// Date		: October 11, 2005

#include "AFI.h"
#include "AUICTranslate.h"
#include "AUIImagePicture.h"
#include "AUIEditbox.h"
#include "AUICombobox.h"
#include "DlgBooth.h"
#include "DlgInputNO.h"
#include "DlgChat.h"
#include "DlgFittingRoom.h"
#include "DlgDragDrop.h"
#include "EC_ShortcutMgr.h"
#include "EC_Game.h"
#include "EC_GameRun.h"
#include "EC_GameUIMan.h"
#include "EC_GameSession.h"
#include "EC_HostPlayer.h"
#include "EC_DealInventory.h"
#include "EC_IvtrItem.h"
#include "EC_IvtrEquipMatter.h"
#include "EC_UIManager.h"
#include "EC_Resource.h"
#include "EC_World.h"
#include "EC_ManPlayer.h"
#include <A2DSprite.h>
#include <A3DDevice.h>
#include "AUIDef.h"
#include "EC_OfflineShopCtrl.h"
#include "EC_IvtrFashion.h"

#define new A_DEBUG_NEW

AUI_BEGIN_COMMAND_MAP(CDlgInputNO2, CDlgBase)
AUI_ON_COMMAND("max",			OnCommand_max)
AUI_ON_COMMAND("start",			OnCommand_start)
AUI_ON_COMMAND("confirm",		OnCommand_confirm)
AUI_END_COMMAND_MAP()

AUI_BEGIN_EVENT_MAP(CDlgInputNO2, CDlgBase)
AUI_ON_EVENT("Txt_Price",	WM_CHAR, OnEventChar_Txt)
AUI_END_EVENT_MAP()

AUI_BEGIN_COMMAND_MAP(CDlgBoothSet, CDlgBase)
AUI_ON_COMMAND("confirm",		OnCommand_Confirm)
AUI_END_COMMAND_MAP()

AUI_BEGIN_COMMAND_MAP(CDlgBoothSelf, CDlgBooth)
AUI_ON_COMMAND("reset",			OnCommand_reset)
AUI_ON_COMMAND("clearmsg",		OnCommand_clearmsg)
AUI_ON_COMMAND("confirm",		OnCommand_confirm)
AUI_ON_COMMAND("IDCANCEL",		OnCommand_CANCEL)
AUI_ON_COMMAND("setting",		OnCommand_Setting)
AUI_END_COMMAND_MAP()

AUI_BEGIN_COMMAND_MAP(CDlgBoothElse, CDlgBooth)
AUI_ON_COMMAND("buy",			OnCommand_buy)
AUI_ON_COMMAND("sell",			OnCommand_sell)
AUI_ON_COMMAND("sendmsg",		OnCommand_sendmsg)
AUI_ON_COMMAND("try",			OnCommand_try)
AUI_ON_COMMAND("IDCANCEL",		OnCommand_CANCEL)
AUI_END_COMMAND_MAP()

AUI_BEGIN_EVENT_MAP(CDlgBoothSelf, CDlgBooth)
AUI_ON_EVENT("Txt_MsgList",		WM_MOUSEMOVE,		OnEventMouseMove_MsgList)
AUI_ON_EVENT("Txt_MsgList",		WM_LBUTTONDOWN,		OnEventLButtonDown_MsgList)
AUI_ON_EVENT("SellItem_*",		WM_LBUTTONUP,		OnEventLButtonUp_SellItem)
AUI_ON_EVENT("BuyItem_*",		WM_LBUTTONUP,		OnEventLButtonUp_BuyItem)
AUI_END_EVENT_MAP()

AUI_BEGIN_EVENT_MAP(CDlgBoothElse, CDlgBooth)
AUI_ON_EVENT("*",				WM_LBUTTONDOWN,		OnEventLButtonDown)
AUI_ON_EVENT("*",				WM_RBUTTONDOWN,		OnEventRButtonDown)
AUI_ON_EVENT("SellItem_*",		WM_LBUTTONDBLCLK,	OnEventLButtonDblclk_SellItem)
AUI_ON_EVENT("Sell_*",			WM_LBUTTONDBLCLK,	OnEventLButtonDblclk_Sell)
AUI_ON_EVENT("Buy_*",			WM_LBUTTONDBLCLK,	OnEventLButtonDblclk_Buy)
AUI_END_EVENT_MAP()

//------------------------------------------------------------------------
// Right-Click Shortcut for CDlgBooth
//------------------------------------------------------------------------
typedef CECShortcutMgr::SimpleClickShortcut<CDlgBooth> BoothClickShortcut;
//------------------------------------------------------------------------

CDlgBooth::CDlgBooth()
{

}

CDlgBooth::~CDlgBooth()
{

}

CDlgBoothSelf::CDlgBoothSelf():m_pA2DSpriteForbid(NULL)
{
}

CDlgBoothSelf::~CDlgBoothSelf()
{
	A3DRELEASE(m_pA2DSpriteForbid);
}

bool CDlgBooth::OnInitDialog()
{
	GetGameUIMan()->GetShortcutMgr()->RegisterShortCut(new BoothClickShortcut(this));
	return true;
}

bool CDlgBoothSelf::OnInitDialog()
{
	if(!CDlgBooth::OnInitDialog())
	{
		return false;
	}

	m_pA2DSpriteForbid = new A2DSprite;
	
	bool bval = m_pA2DSpriteForbid->Init(m_pA3DDevice, "Window\\Í¼±ê_ÎïÆ·À¸(½ûÖ¹).dds", 0);
	if( !bval )
	{
		AUI_ReportError(__LINE__, __FILE__);
		return false;
	}
	
	m_pA2DSpriteForbid->SetLinearFilter(true);

	dynamic_cast<PAUITEXTAREA>(GetDlgItem("Txt_MsgList"))->SetMaxLines(200);
	
	return true;
}

void CDlgInputNO2::OnCommand_max(const char * szCommand)
{
	ACHAR szText[20];
	CECIvtrItem *pItem = (CECIvtrItem *)GetDataPtr("ptr_CECIvtrItem");
	if( !pItem ) return;

	if( GetData() == CDlgInputNO::INPUTNO_BUY_ADD || GetData() == CDlgInputNO::INPUTNO_OFFLINESHOP_BUY)
		a_sprintf(szText, _AL("%d"), pItem->GetPileLimit());
	else
		a_sprintf(szText, _AL("%d"), pItem->GetCount());
	GetDlgItem("Txt_Amount")->SetText(szText);
}

void CDlgInputNO2::OnCommand_start(const char * szCommand)
{
	CECDealInventory *pBoothBuy = GetHostPlayer()->GetBoothBuyPack();
	int iOrigin = (int)GetDlgItem("Item")->GetData();
	CECIvtrItem *pItem = (CECIvtrItem *)GetDlgItem("Item")->GetDataPtr("ptr_CECIvtrItem");
	if( !pItem ) return;
	int nCount = a_atoi(GetDlgItem("Txt_no")->GetText());
	int nPrice = a_atoi(GetDlgItem("Txt_Gold")->GetText());

	if( pItem && nCount > 0 && nPrice > 0 )
	{
		pBoothBuy->AddBoothItem(pItem, iOrigin, nCount, nPrice);
		Show(false);
	}
}

void CDlgBoothElse::OnCommand_buy(const char * szCommand)
{
	int nGold = GetHostPlayer()->GetMoneyAmount();
	unsigned int nCost = m_nBuyWorth;
	int id = GetData("int");
	CECPlayerMan *pPlayerMan = GetWorld()->GetPlayerMan();
	CECPlayer *pPlayer = (CECPlayer *)pPlayerMan->GetPlayer(id);
	PAUIDIALOG pMsgBox;
	ACString strText;
	if( !pPlayer )
	{
		return;
	}
	bool bCanUseYP = pPlayer->GetCertificateID() > 0;
	if (!bCanUseYP)
	{
		if( (unsigned int)nGold >= nCost )
			GetHostPlayer()->BuyItemsFromBooth();
		else
		{
			m_pAUIManager->MessageBox("", GetStringFromTable(222), MB_OK,
				A3DCOLORRGBA(255, 255, 255, 160), &pMsgBox);
			pMsgBox->SetLife(3);
		}
	}
	else
	{
		int nOwnYP = GetHostPlayer()->GetYinpiaoTotalAmount();
		int nNeedYP = nCost / 10000000;
		int nYP = nOwnYP >= nNeedYP? nNeedYP:nOwnYP;
		int NeedMoney = nCost - nYP * 10000000;
		if (nGold >= NeedMoney)
		{
			strText.Format(GetStringFromTable(7611), nYP, NeedMoney);
			m_pAUIManager->MessageBox("Game_ConfirmTrade", strText, MB_YESNO, A3DCOLORRGBA(255, 255, 255, 160), &pMsgBox);
			pMsgBox->SetData(nYP);
		}
		else
		{
			strText.Format(GetStringFromTable(7612), nYP, NeedMoney);
			m_pAUIManager->MessageBox("", GetStringFromTable(222), MB_OK, A3DCOLORRGBA(255, 255, 255, 160), &pMsgBox);
		}
	}
}

void CDlgBoothElse::OnCommand_sell(const char * szCommand)
{
	m_pAUIManager->MessageBox("Game_PSellOut", GetStringFromTable(243),
		MB_OKCANCEL, A3DCOLORRGBA(255, 255, 255, 160));
}

void CDlgBoothSelf::OnCommand_reset(const char * szCommand)
{
	GetHostPlayer()->GetBoothBuyPack()->RemoveAllItems();
	GetHostPlayer()->GetBoothSellPack()->RemoveAllItems();
	GetHostPlayer()->GetPack()->UnfreezeAllItems();
}

void CDlgBoothSelf::OnCommand_confirm(const char * szCommand)
{
	ACString strName = GetDlgItem("DEFAULT_Txt_Name")->GetText();
	GetGameRun()->GetUIManager()->FilterBadWords(strName);
	GetDlgItem("DEFAULT_Txt_Name")->SetText(strName);
	GetHostPlayer()->OpenBooth(strName);
}

void CDlgBoothSelf::OnCommand_Setting(const char * szCommand)
{
	if( GetHostPlayer()->GetCertificateID() > 0 )
		GetGameUIMan()->m_pDlgBoothSet->Show(true);
}

//////////////////////////////////////////////////////////////////////////

void CDlgInputNO2::OnHideDialog()
{
	if (GetData() == CDlgInputNO::INPUTNO_OFFLINESHOP_SELL || GetData() == CDlgInputNO::INPUTNO_OFFLINESHOP_BUY)
	{
		PAUIEDITBOX pText = dynamic_cast<PAUIEDITBOX>(GetDlgItem("Txt_Price"));
		pText->SetMaxLength(10); // »Ö¸´Ä¬ÈÏ
	}	
}
void CDlgInputNO2::OnEventChar_Txt(WPARAM wParam, LPARAM lParam, AUIObject *pObj)
{
	if (GetData() == CDlgInputNO::INPUTNO_OFFLINESHOP_SELL || GetData() == CDlgInputNO::INPUTNO_OFFLINESHOP_BUY)
	{
		PAUIEDITBOX pText = dynamic_cast<PAUIEDITBOX>(GetDlgItem("Txt_Price"));
		__int64 m =0;
		CDlgInputNO3::ComputeValue(pText,m);
	}	
}

void CDlgInputNO2::OnCommand_confirm(const char * szCommand)
{
	PAUIDIALOG pMsgBox;
	CECIvtrItem *pItem = (CECIvtrItem *)GetDataPtr("ptr_CECIvtrItem");
	if( !pItem ) return;
	int iOrigin = (int)GetDlgItem("Txt_Price")->GetData();
	
	if (GetData() == CDlgInputNO::INPUTNO_OFFLINESHOP_SELL || GetData() == CDlgInputNO::INPUTNO_OFFLINESHOP_BUY)
	{
		COfflineShopCtrl* pCtrl = GetHostPlayer()->GetOfflineShopCtrl();
		if (!pCtrl) return ;

		PAUIEDITBOX pText =  dynamic_cast<PAUIEDITBOX>(GetDlgItem("Txt_Price"));
		__int64 nPrice = 0;
		CDlgInputNO3::ComputeValue(pText,nPrice);

		int nCount = a_atoi(GetDlgItem("Txt_Amount")->GetText());
	
		if(!pCtrl->CheckMoneyInv(GetData() == CDlgInputNO::INPUTNO_OFFLINESHOP_SELL,nPrice,nCount))
		{
			ACString str;

			if(GetData() == CDlgInputNO::INPUTNO_OFFLINESHOP_SELL)
				str.Format(GetStringFromTable(10531), 40,1000);
			else
				str = GetGameUIMan()->GetStringFromTable(10523);

			m_pAUIManager->MessageBox("", str, MB_OK, A3DCOLORRGBA(255, 255, 255, 160), &pMsgBox);
			pMsgBox->SetLife(3);
			return;
		}

		if( nPrice < 1 || nCount < 1 )
		{
			m_pAUIManager->MessageBox("", GetStringFromTable(563), MB_OK,
				A3DCOLORRGBA(255, 255, 255, 160), &pMsgBox);
			pMsgBox->SetLife(3);
			return;
		}
		if( (GetData() == CDlgInputNO::INPUTNO_OFFLINESHOP_SELL && nCount > pItem->GetCount()) ||
			(GetData() == CDlgInputNO::INPUTNO_OFFLINESHOP_BUY && nCount > pItem->GetPileLimit()) )
		{
			m_pAUIManager->MessageBox("", GetStringFromTable(564), MB_OK,
				A3DCOLORRGBA(255, 255, 255, 160), &pMsgBox);
			pMsgBox->SetLife(3);
			return;
		}
		
		Show(false);
		
		if(GetData() == CDlgInputNO::INPUTNO_OFFLINESHOP_SELL)		
			GetGameSession()->OffShop_AddItemToMySellInv(pItem->GetTemplateID(),pCtrl->FirstEmptyPosInMyShop(true),(unsigned int)nPrice,nCount,iOrigin);		
		else if(GetData() == CDlgInputNO::INPUTNO_OFFLINESHOP_BUY)
			GetGameSession()->OffShop_AddItemToMyBuyInv(pItem->GetTemplateID(),pCtrl->FirstEmptyPosInMyShop(false),(unsigned int)nPrice,nCount);

		return ;
	}

	ACString strPrice = GetDlgItem("Txt_Price")->GetText();
	__int64 nPrice;
	if (strPrice.GetLength() == 10)
	{
		nPrice = strPrice.Left(9).ToInt();
		nPrice *= 10;
		nPrice += strPrice.Right(1).ToInt();
	}
	else
	{
		nPrice = strPrice.ToInt();
	}	
	int nCount = a_atoi(GetDlgItem("Txt_Amount")->GetText());
	__int64 nTotal = nPrice * __int64(nCount);
	CECDealInventory *pBoothSell = GetHostPlayer()->GetBoothSellPack();
	CECDealInventory *pBoothBuy = GetHostPlayer()->GetBoothBuyPack();

	if (GetHostPlayer()->GetCertificateID() > 0)
	{
		if( nPrice > 2000000000 || nTotal > 4000000000)
		{
			ACString str;
			str.Format(GetStringFromTable(562), 20, 40);
			m_pAUIManager->MessageBox("", str, MB_OK, A3DCOLORRGBA(255, 255, 255, 160), &pMsgBox);
			pMsgBox->SetLife(3);
			return;
		}
	}
	else
	{
		if( nPrice > 200000000 || nTotal > 200000000 )
		{
			ACString str;
			str.Format(GetStringFromTable(562), 2, 2);
			m_pAUIManager->MessageBox("", str, MB_OK, A3DCOLORRGBA(255, 255, 255, 160), &pMsgBox);
			pMsgBox->SetLife(3);
			return;
		}
	}
	if( nPrice < 1 || nCount < 1 )
	{
		m_pAUIManager->MessageBox("", GetStringFromTable(563), MB_OK,
			A3DCOLORRGBA(255, 255, 255, 160), &pMsgBox);
		pMsgBox->SetLife(3);
		return;
	}
	if( (GetData() == CDlgInputNO::INPUTNO_SELL_ADD && nCount > pItem->GetCount()) ||
		(GetData() == CDlgInputNO::INPUTNO_BUY_ADD && nCount > pItem->GetPileLimit()) )
	{
		m_pAUIManager->MessageBox("", GetStringFromTable(564), MB_OK,
			A3DCOLORRGBA(255, 255, 255, 160), &pMsgBox);
		pMsgBox->SetLife(3);
		return;
	}

	Show(false);
	if( GetData() == CDlgInputNO::INPUTNO_BUY_ADD )
	{
		a_Clamp(nCount, 1, pItem->GetPileLimit());
		if (pBoothBuy->AddBoothItem(pItem, iOrigin, nCount, (int)nPrice) > -1)
		{
			pItem->Freeze(true);
		}
	}
	else
	{
		a_Clamp(nCount, 1, pItem->GetCount());
		if(pBoothSell->AddBoothItem(pItem, iOrigin, nCount, (int)nPrice) > -1)
		{
			pItem->Freeze(true);
		}
	}
}
//////////////////////////////////////////////////////////////////////////

void CDlgBoothElse::OnCommand_sendmsg(const char * szCommand)
{
	PAUIOBJECT pEdit = GetDlgItem("Txt_Msg");
	if( !pEdit ) return;
	const ACHAR *szText = pEdit->GetText();
	CECGameSession *pSession = GetGameSession();

	int id = GetData("int");
	const ACHAR *pszName = GetGameRun()->GetPlayerName(id, true);
	if( !pszName ) return;

	ACString strTextOut;

	strTextOut.Format(_AL("%s!#"), szText);

	pSession->SendPrivateChatData(pszName, strTextOut, 0, id);
	pEdit->SetText(_AL(""));
	ChangeFocus(NULL);
}

void CDlgBoothSelf::OnCommand_clearmsg(const char * szCommand)
{
	PAUITEXTAREA pMsgList = dynamic_cast<PAUITEXTAREA>(GetDlgItem("Txt_MsgList"));
	if( !pMsgList ) return;
	pMsgList->SetText(_AL(""));
}

void CDlgBoothElse::OnCommand_try(const char * szCommand)
{
	GetGameUIMan()->GetDialog("Win_FittingRoom")->Show(true);
}

void CDlgBoothSelf::OnCommand_CANCEL(const char * szCommand)
{
	Show(false);

	OnCommand_clearmsg("clearmsg");
	PAUITEXTAREA pMsgList = dynamic_cast<PAUITEXTAREA>(GetDlgItem("Txt_MsgList"));
	if( pMsgList )
		pMsgList->SetText(_AL(""));
	if( GetDlgItem("DEFAULT_Txt_Name")->IsEnabled() )
	{
		GetHostPlayer()->SetBoothState(0);
		m_pAUIManager->GetDialog("Win_Inventory")->Show(false);
	}
	else
		GetGameSession()->c2s_CmdCloseBooth();

	GetHostPlayer()->GetPack()->UnfreezeAllItems();
}

void CDlgBoothElse::OnCommand_CANCEL(const char * szCommand)
{
	Show(false);

	CECDealInventory *pBoothSell = GetHostPlayer()->GetEPBoothSellPack();
	CECDealInventory *pBoothBuy = GetHostPlayer()->GetEPBoothBuyPack();

	GetGameUIMan()->EndNPCService();
	pBoothBuy->UnfreezeAllItems();
	pBoothSell->UnfreezeAllItems();
	GetHostPlayer()->GetPack()->UnfreezeAllItems();
	m_pAUIManager->GetDialog("Win_Inventory")->Show(false);
}

bool CDlgBoothSelf::Tick(void)
{
	int i;
	char szItem[40];
	PAUIIMAGEPICTURE pObj;

	if( IsShow() )
	{
		PAUISTILLIMAGEBUTTON pBtnSet = (PAUISTILLIMAGEBUTTON)GetDlgItem("Btn_Set");

		for(i = GetHostPlayer()->GetBoothSellPack()->GetSize(); i < IVTRSIZE_BOOTHSPACK_MAX; i++)
		{
			sprintf(szItem, "SellItem_%02d", i + 1);
			pObj = (PAUIIMAGEPICTURE)GetDlgItem(szItem);
			pObj->SetCover(m_pA2DSpriteForbid, 0);
		}
		for(i = GetHostPlayer()->GetBoothBuyPack()->GetSize(); i < IVTRSIZE_BOOTHBPACK_MAX; i++)
		{
			sprintf(szItem, "BuyItem_%02d", i + 1);
			pObj = (PAUIIMAGEPICTURE)GetDlgItem(szItem);
			pObj->SetCover(m_pA2DSpriteForbid, 0);
		}
		int id = GetHostPlayer()->GetCertificateID();
		if (id > 0)
		{
			CECIvtrCertificate *pItem = (CECIvtrCertificate *)CECIvtrItem::CreateItem(GetHostPlayer()->GetCertificateID(), 0, 1);
			if (pItem)
			{
				dynamic_cast<PAUIEDITBOX>(GetDlgItem("DEFAULT_Txt_Name"))->SetMaxLength(pItem->GetBoothNameLength());
				delete pItem;
			}
		}
		else
			dynamic_cast<PAUIEDITBOX>(GetDlgItem("DEFAULT_Txt_Name"))->SetMaxLength(13);

		if( pBtnSet ) pBtnSet->Enable(id > 0);
		UpdateBooth();
		UpdateBoothSelf();
	}
	
	return CDlgBooth::Tick();
}

bool CDlgBoothElse::Tick(void)
{
	if( IsShow() )
	{
		UpdateBooth();
		UpdateBoothElse();
		UpdatePriceRemind();
		UpdateSameItemMark();
	}

	return CDlgBooth::Tick();
}

CECDealInventory * CDlgBoothSelf::GetBuyPack()
{
	return GetHostPlayer()->GetBoothBuyPack();
}

CECDealInventory * CDlgBoothElse::GetBuyPack()
{
	return GetHostPlayer()->GetEPBoothBuyPack();
}

CECDealInventory * CDlgBoothSelf::GetSellPack()
{
	return GetHostPlayer()->GetBoothSellPack();
}

CECDealInventory * CDlgBoothElse::GetSellPack()
{
	return GetHostPlayer()->GetEPBoothSellPack();
}

bool CDlgBooth::UpdateBooth()
{
	if( !IsShow() ) return true;

	int i;
	AString strFile;
	char szItem[40];
	ACHAR szText[40];
	CECIvtrItem *pItem;
	AUICTranslate trans;
	PAUIIMAGEPICTURE pObj;
	CECDealInventory *pDeal;
	__int64 nMoney1 = 0, nMoney2 = 0;
	PAUIDIALOG pDlgMouseOn;
	PAUIOBJECT pObjMouseOn;

	m_pAUIManager->GetMouseOn(&pDlgMouseOn, &pObjMouseOn);

	pDeal = GetSellPack();
	
	for( i = 0; i < pDeal->GetSize(); i++ )
	{
		sprintf(szItem, "SellItem_%02d", i + 1);
		pObj = (PAUIIMAGEPICTURE)GetDlgItem(szItem);
		if( !pObj ) continue;

		pItem = pDeal->GetItem(i);
		pObj->SetColor(A3DCOLORRGB(255, 255, 255));
		if( pItem )
		{
			pObj->SetDataPtr(pItem, "ptr_CECIvtrItem");

			const CECDealInventory::ITEMINFO &ii = pDeal->GetItemInfo(i);
			if( ii.iAmount > 1 )
			{
				a_sprintf(szText, _AL("%d"), ii.iAmount);
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
			nMoney1 += (__int64)pItem->GetUnitPrice() * (__int64)ii.iAmount;
		}
		else
		{
			pObj->ClearCover();
			pObj->SetText(_AL(""));
			pObj->SetHint(_AL(""));
			pObj->SetDataPtr(NULL);
		}
	}

	pDeal = GetBuyPack();

	for( i = 0; i < pDeal->GetSize(); i++ )
	{
		sprintf(szItem, "BuyItem_%02d", i + 1);
		pObj = (PAUIIMAGEPICTURE)GetDlgItem(szItem);
		if( !pObj ) continue;

		pItem = pDeal->GetItem(i);
		pObj->SetColor(A3DCOLORRGB(255, 255, 255));
		if( pItem )
		{
			pObj->SetDataPtr(pItem,"ptr_CECIvtrItem");

			const CECDealInventory::ITEMINFO &ii = pDeal->GetItemInfo(i);
			if( ii.iAmount > 1 )
			{
				a_sprintf(szText, _AL("%d"), ii.iAmount);
				pObj->SetText(szText);
			}
			else
				pObj->SetText(_AL(""));
			if( pDlgMouseOn && pDlgMouseOn == this &&
				pObjMouseOn && pObjMouseOn == pObj )
			{
				const wchar_t* szDesc = pItem->GetDesc(CECIvtrItem::DESC_BOOTHBUY);
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
			nMoney2 += (__int64)pItem->GetUnitPrice() * (__int64)ii.iAmount;
		}
		else
		{
			pObj->ClearCover();
			pObj->SetText(_AL(""));
			pObj->SetHint(_AL(""));
			pObj->SetDataPtr(NULL);
		}
	}

	m_nMoney1 = nMoney1;
	m_nMoney2 = nMoney2;

	return true;
}

bool CDlgBoothSelf::UpdateBoothSelf()
{
	if( !IsShow() ) return true;

	AUICTranslate trans;
	__int64 nMoney1 = m_nMoney1;
	__int64 nMoney2 = m_nMoney2;

	// Sold out.
	if( nMoney1 <= 0 && nMoney2 <= 0 &&
		!GetDlgItem("DEFAULT_Txt_Name")->IsEnabled() )
	{
		OnCommand_CANCEL("IDCANCEL");
		return true;
	}

	if (GetHostPlayer()->GetCertificateID() > 0)
	{
		if( nMoney1 > 4000000000 )
			GetDlgItem("Txt_Worth1")->SetColor(A3DCOLORRGB(255, 0, 0));
		else
			GetDlgItem("Txt_Worth1")->SetColor(A3DCOLORRGB(255, 255, 255));
	}
	else
	{
		if( nMoney1 + GetHostPlayer()->GetMoneyAmount() > 200000000 )
			GetDlgItem("Txt_Worth1")->SetColor(A3DCOLORRGB(255, 0, 0));
		else
			GetDlgItem("Txt_Worth1")->SetColor(A3DCOLORRGB(255, 255, 255));
	}
	GetDlgItem("Txt_Worth1")->SetText(GetGameUIMan()->GetFormatNumber(nMoney1));

	if( nMoney2 > GetHostPlayer()->GetMoneyAmount() )
		GetDlgItem("Txt_Worth2")->SetColor(A3DCOLORRGB(255, 0, 0));
	else
		GetDlgItem("Txt_Worth2")->SetColor(A3DCOLORRGB(255, 255, 255));
	GetDlgItem("Txt_Worth2")->SetText(GetGameUIMan()->GetFormatNumber(nMoney2));

	GetDlgItem("Txt_GoldAll")->SetColor(GetGameUIMan()->GetPriceColor(GetHostPlayer()->GetMoneyAmount()));
	GetDlgItem("Txt_GoldAll")->SetText(GetGameUIMan()->GetFormatNumber(GetHostPlayer()->GetMoneyAmount()));

	if( (nMoney1 > 0 || nMoney2 > 0) &&
		nMoney2 <= GetHostPlayer()->GetMoneyAmount() &&
		GetDlgItem("DEFAULT_Txt_Name")->IsEnabled() &&
		a_strlen(GetDlgItem("DEFAULT_Txt_Name")->GetText()) > 0 )
	{
		if(GetHostPlayer()->GetCertificateID() > 0)
		{
			if (nMoney1  <= 4000000000)
			{
				GetDlgItem("Btn_Confirm")->Enable(true);
			}
			else
			{
				GetDlgItem("Btn_Confirm")->Enable(false);
			}
		}
		else
		{
			if (nMoney1 + GetHostPlayer()->GetMoneyAmount() <= 200000000)
			{
				GetDlgItem("Btn_Confirm")->Enable(true);
			}
			else
			{
				GetDlgItem("Btn_Confirm")->Enable(false);
			}
		}
		
	}
	else
		GetDlgItem("Btn_Confirm")->Enable(false);

	return true;
}

bool CDlgBoothElse::UpdateBoothElse()
{
	if( !IsShow() ) return true;

	AString strFile;
	char szItem[40];
	int i;
	CECIvtrItem *pItem;
	AUICTranslate trans;
	PAUIIMAGEPICTURE pObj;
	CECDealInventory *pDeal;
	PAUIDIALOG pDlgMouseOn;
	PAUIOBJECT pObjMouseOn;

	m_pAUIManager->GetMouseOn(&pDlgMouseOn, &pObjMouseOn);

	// Buy.
	m_nBuyWorth = 0;
	pDeal = GetHostPlayer()->GetBuyPack();
	for( i = 0; i < IVTRSIZE_BUYPACK; i++ )
	{
		sprintf(szItem, "Buy_%02d", i + 1);
		pObj = (PAUIIMAGEPICTURE)GetDlgItem(szItem);
		if( !pObj ) continue;

		pItem = pDeal->GetItem(i);
		if( pItem )
		{
			pObj->SetDataPtr(pItem,"ptr_CECIvtrItem");

			const CECDealInventory::ITEMINFO &ii = pDeal->GetItemInfo(i);
			if( ii.iAmount > 1 )
			{
				ACString strAmount;
				strAmount.Format(_AL("%d"), ii.iAmount);
				pObj->SetText(strAmount);
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

			af_GetFileTitle(pItem->GetIconFile(), strFile);
			strFile.MakeLower();
			pObj->SetCover(GetGameUIMan()->GetIconCover(CECGameUIMan::ICONS_INVENTORY),
				GetGameUIMan()->GetIconIndex(CECGameUIMan::ICONS_INVENTORY, strFile));

			pObj->SetColor(
				(pItem->IsEquipment() && ((CECIvtrEquip *)pItem)->IsDestroying())
				? A3DCOLORRGB(128, 128, 128)
				: A3DCOLORRGB(255, 255, 255));

			m_nBuyWorth += pItem->GetUnitPrice() * ii.iAmount;
		}
		else
		{
			pObj->ClearCover();
			pObj->SetText(_AL(""));
			pObj->SetHint(_AL(""));
			pObj->SetDataPtr(NULL);
		}
	}
	GetDlgItem("Txt_Worth1")->SetText(GetGameUIMan()->GetFormatNumber(m_nBuyWorth));
	GetDlgItem("Txt_Gold")->SetText(GetGameUIMan()->GetFormatNumber(GetHostPlayer()->GetMoneyAmount()));
	GetDlgItem("Txt_Silvers")->SetText(GetGameUIMan()->GetFormatNumber(GetHostPlayer()->GetYinpiaoTotalAmount()));

	int id = GetData("int");
	CECPlayerMan *pPlayerMan = GetWorld()->GetPlayerMan();
	CECPlayer *pPlayer = (CECPlayer *)pPlayerMan->GetPlayer(id);
	if (!pPlayer)
	{
		// Íæ¼ÒÏûÊ§£¬Í£Ö¹½»Ò×
		OnCommand_CANCEL("IDCANCEL");
		return true;
	}
	bool bCanUseYP = pPlayer->GetCertificateID() > 0;
	bool bCanBuy = true;
	if (!bCanUseYP)
	{
		if( m_nBuyWorth > (unsigned int)GetHostPlayer()->GetMoneyAmount() )
			bCanBuy = false;
	}
	else
	{
		int nOwnYP = GetHostPlayer()->GetYinpiaoTotalAmount();
		int nNeedYP = m_nBuyWorth / 10000000;
		int nYP = nOwnYP >= nNeedYP? nNeedYP:nOwnYP;
		int NeedMoney = m_nBuyWorth - nYP * 10000000;
		if (GetHostPlayer()->GetMoneyAmount() < NeedMoney)
			bCanBuy = false;
	}
	if( !bCanBuy )
	{
		GetDlgItem("Txt_Gold")->SetColor(A3DCOLORRGB(255, 0, 0));
		GetDlgItem("Txt_Silvers")->SetColor(A3DCOLORRGB(255, 0, 0));
	}
	else
	{
		GetDlgItem("Txt_Gold")->SetColor(GetGameUIMan()->GetPriceColor(GetHostPlayer()->GetMoneyAmount()));
		GetDlgItem("Txt_Silvers")->SetColor(A3DCOLORRGB(255, 255, 255));
	}
	
	// Sell.
	m_nSellWorth = 0;
	pDeal = GetHostPlayer()->GetSellPack();
	for( i = 0; i < IVTRSIZE_SELLPACK; i++ )
	{
		sprintf(szItem, "Sell_%02d", i + 1);
		pObj = (PAUIIMAGEPICTURE)GetDlgItem(szItem);
		if( !pObj ) continue;

		pItem = pDeal->GetItem(i);
		if( pItem )
		{
			pObj->SetDataPtr(pItem,"ptr_CECIvtrItem");

			const CECDealInventory::ITEMINFO &ii = pDeal->GetItemInfo(i);
			if( ii.iAmount > 1 )
			{
				ACString strAmount;
				strAmount.Format(_AL("%d"), ii.iAmount);
				pObj->SetText(strAmount);
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

			af_GetFileTitle(pItem->GetIconFile(), strFile);
			strFile.MakeLower();
			pObj->SetCover(GetGameUIMan()->GetIconCover(CECGameUIMan::ICONS_INVENTORY),
				GetGameUIMan()->GetIconIndex(CECGameUIMan::ICONS_INVENTORY, strFile));

			pObj->SetColor(
				(pItem->IsEquipment() && ((CECIvtrEquip *)pItem)->IsDestroying())
				? A3DCOLORRGB(128, 128, 128)
				: A3DCOLORRGB(255, 255, 255));

			m_nSellWorth += pItem->GetUnitPrice() * ii.iAmount;
		}
		else
		{
			pObj->ClearCover();
			pObj->SetText(_AL(""));
			pObj->SetHint(_AL(""));
			pObj->SetDataPtr(NULL);
		}
	}
	GetDlgItem("Txt_Worth2")->SetText(GetGameUIMan()->GetFormatNumber(m_nSellWorth));

	return true;
}

void CDlgBoothElse::UpdatePriceRemind()
{
#define RETURN_FAIL { pDragDrop->GetImgGood()->SetHint(_AL("")); return; }

	CDlgDragDrop* pDragDrop = GetGameUIMan()->m_pDlgDragDrop;
	if( !pDragDrop->IsShow() ) RETURN_FAIL;

	PAUIDIALOG pDlgOver = NULL;
	PAUIOBJECT pObjOver = NULL;
	A3DVIEWPORTPARAM *p = m_pA3DEngine->GetActiveViewport()->GetParam();

	POINT pt;
	GetCursorPos(&pt);
	ScreenToClient(m_pA3DDevice->GetDeviceWnd(), &pt);
	int x = pt.x - p->X;
	int y = pt.y - p->Y;
	GetGameUIMan()->HitTest(x, y, &pDlgOver, &pObjOver, pDragDrop);
	if( !pDlgOver || !pObjOver || pDlgOver != this || !strstr(pObjOver->GetName(), "BuyItem_") ) RETURN_FAIL;

	CECIvtrItem* pItem = (CECIvtrItem*)pObjOver->GetDataPtr("ptr_CECIvtrItem");
	if( !pItem ) RETURN_FAIL;

	PAUIOBJECT pObjSrc = (PAUIOBJECT)pDragDrop->GetImgGood()->GetDataPtr("ptr_AUIObject");
	if( !pObjSrc || !strstr(pObjSrc->GetParent()->GetName(), "Win_Inventory") ) RETURN_FAIL;

	ACString strHint;
	strHint.Format(GetStringFromTable(11250), GetGameUIMan()->GetFormatNumber(pItem->GetUnitPrice()));
	pDragDrop->GetImgGood()->SetHint(strHint);
}

void CDlgBoothElse::UpdateSameItemMark()
{
	char szItem[40];
	CECIvtrItem *pItem, *pDrag = NULL;
	PAUIDIALOG pDlgDrag = m_pAUIManager->GetDialog("DragDrop");

	if( pDlgDrag->IsShow() )
	{
		PAUIOBJECT pCell = pDlgDrag->GetDlgItem("Goods");
		PAUIOBJECT pObjSrc = (PAUIOBJECT)pCell->GetDataPtr("ptr_AUIObject");
		PAUIDIALOG pDlgSrc = pObjSrc->GetParent();
		
		if (pDlgSrc == (PAUIDIALOG)GetGameUIMan()->m_pDlgInventory ||
			pDlgSrc == (PAUIDIALOG)GetGameUIMan()->m_pDlgBag)
			pDrag = (CECIvtrItem *)pObjSrc->GetDataPtr("ptr_CECIvtrItem");
	}

	CECDealInventory* pDeal = GetBuyPack();
	for( int i = 0; i < pDeal->GetSize(); i++ )
	{
		sprintf(szItem, "BuyItem_%02d", i + 1);
		PAUIIMAGEPICTURE pObj = (PAUIIMAGEPICTURE)GetDlgItem(szItem);
		if( !pObj ) continue;

		pItem = pDeal->GetItem(i);
		if( pItem && pDrag && pDrag->GetTemplateID() == pItem->GetTemplateID() )
			pObj->SetCover(GetGameUIMan()->m_pA2DSpriteMask, 0, 1);
		else
			pObj->SetCover(NULL, -1, 1);
	}
}

void CDlgBoothSelf::OnEventLButtonUp_SellItem(WPARAM wParam, LPARAM lParam, AUIObject *pObj)
{
	if( !pObj->GetDataPtr("ptr_CECIvtrItem") )
		return;

	if( !GetDlgItem("DEFAULT_Txt_Name")->IsEnabled() )
		return;
	
	int nIndex = atoi(pObj->GetName() + strlen("SellItem_")) - 1;
	CECDealInventory *pDeal = GetHostPlayer()->GetBoothSellPack();
	const CECDealInventory::ITEMINFO &ii = pDeal->GetItemInfo(nIndex);
	
	CECIvtrItem *pItem =  GetHostPlayer()->GetPack()->GetItem(ii.iOrigin);
	if( pItem )
		pItem->Freeze(false);
	pDeal->RemoveItemByIndex(nIndex, -1);
}

void CDlgBoothSelf::OnEventLButtonUp_BuyItem(WPARAM wParam, LPARAM lParam, AUIObject *pObj)
{
	if( !pObj->GetDataPtr("ptr_CECIvtrItem") )
		return;
	if( !GetDlgItem("DEFAULT_Txt_Name")->IsEnabled() )
		return;
	
	int nIndex = atoi(pObj->GetName() + strlen("BuyItem_")) - 1;
	CECDealInventory *pDeal = GetHostPlayer()->GetBoothBuyPack();
	const CECDealInventory::ITEMINFO &ii = pDeal->GetItemInfo(nIndex);
	
	CECIvtrItem *pItem =  GetHostPlayer()->GetPack()->GetItem(ii.iOrigin);
	if( pItem )
		pItem->Freeze(false);
	pDeal->RemoveItemByIndex(nIndex, -1);
}

void CDlgBoothElse::OnEventRButtonDown(WPARAM wParam, LPARAM lParam, AUIObject *pObj)
{
	// try to trigger shortcuts
	GetGameUIMan()->GetShortcutMgr()->TriggerShortcuts(this->GetName(), pObj);
}

void CDlgBoothElse::OnEventLButtonDown(WPARAM wParam, LPARAM lParam, AUIObject *pObj)
{
	CECIvtrItem *pItem = (CECIvtrItem *)pObj->GetDataPtr("ptr_CECIvtrItem");
	if( !pItem || pItem->IsFrozen())
		return;
	
	A3DVIEWPORTPARAM *p = m_pA3DEngine->GetActiveViewport()->GetParam();
	POINT ptCapture = { GET_X_LPARAM(lParam) - p->X, GET_Y_LPARAM(lParam) - p->Y };
	
	GetGameUIMan()->m_ptLButtonDown = ptCapture;

	GetGameUIMan()->InvokeDragDrop(this, pObj, ptCapture);
	GetGameUIMan()->PlayItemSound(pItem, false);
}

void CDlgBoothElse::OnEventLButtonDblclk_SellItem(WPARAM wParam, LPARAM lParam, AUIObject *pObj)
{
	CECIvtrItem *pItem = (CECIvtrItem *)pObj->GetDataPtr("ptr_CECIvtrItem");
	if( !pItem || pItem->IsFrozen() )
		return;

	if( pItem->GetPileLimit() > 1 && pItem->GetCount() > 1 )
	{
		ACHAR szText[40];
		PAUIOBJECT pEdit = GetGameUIMan()->m_pDlgInputNO->GetDlgItem("DEFAULT_Txt_No.");

		pEdit->SetData(pItem->GetCount());
		a_sprintf(szText, _AL("%d"), 1);
		pEdit->SetText(szText);

		GetGameUIMan()->m_pDlgInputNO->SetDataPtr(pObj,"ptr_AUIObject");
		GetGameUIMan()->m_pDlgInputNO->Show(true, true);
		GetGameUIMan()->m_pDlgInputNO->SetType(CDlgInputNO::INPUTNO_BUY_ADD);
	}
	else
	{
		CECDealInventory *pBuy = GetHostPlayer()->GetBuyPack();
		CECDealInventory *pSell = GetHostPlayer()->GetEPBoothSellPack();
		int iSrc = atoi(pObj->GetName() + strlen("SellItem_")) - 1;
		int iOrigin = pSell->GetItemInfo(iSrc).iOrigin;

		pBuy->AddBoothItem(pItem, iOrigin, 1, pItem->GetUnitPrice());
		pItem->Freeze(true);
	}
}

void CDlgBoothElse::OnEventLButtonDblclk_Sell(WPARAM wParam, LPARAM lParam, AUIObject *pObj)
{
	CECIvtrItem *pItem = (CECIvtrItem *)pObj->GetDataPtr("ptr_CECIvtrItem");
	if( !pItem ) return;

	CECDealInventory *pDeal = GetHostPlayer()->GetSellPack();
	int iSrc = atoi(pObj->GetName() + strlen("Sell_")) - 1;
	const CECDealInventory::ITEMINFO &ii = pDeal->GetItemInfo(iSrc);
	int nIndex = GetHostPlayer()->GetEPBoothBuyPack()->GetItemIndex(ii.iOrigin);

	pDeal->RemoveItemByIndex(iSrc, pItem->GetCount());

	pItem =  GetHostPlayer()->GetPack()->GetItem(ii.iFlag);
	if( pItem )
		pItem->Freeze(false);
	pItem = GetHostPlayer()->GetEPBoothBuyPack()->GetItem(nIndex);
	if( pItem )
		pItem->Freeze(false);
}

void CDlgBoothElse::OnEventLButtonDblclk_Buy(WPARAM wParam, LPARAM lParam, AUIObject *pObj)
{
	CECIvtrItem *pItem = (CECIvtrItem *)pObj->GetDataPtr("ptr_CECIvtrItem");
	if( !pItem ) return;

	CECDealInventory *pDeal = GetHostPlayer()->GetBuyPack();
	int iSrc = atoi(pObj->GetName() + strlen("Buy_")) - 1;
	int iOrigin = pDeal->GetItemInfo(iSrc).iOrigin;
	int nIndex = GetHostPlayer()->GetEPBoothSellPack()->GetItemIndex(iOrigin);

	pDeal->RemoveItemByIndex(iSrc, pItem->GetCount());
	pItem = GetHostPlayer()->GetEPBoothSellPack()->GetItem(nIndex);
	if( pItem )
		pItem->Freeze(false);
}

bool CDlgBoothSelf::GetNameLinkMouseOn(int x, int y,
	PAUIOBJECT pObj, P_AUITEXTAREA_NAME_LINK pLink)
{
	bool bClickedChatPart = false;
	A3DRECT rcWindow = m_pAUIManager->GetRect();
	POINT ptPos = pObj->GetPos();
	PAUITEXTAREA pText = dynamic_cast<PAUITEXTAREA>(pObj);

	GetGame()->ChangeCursor(RES_CUR_NORMAL);
	if( pText->GetHitArea(x - ptPos.x, y - ptPos.y) == AUITEXTAREA_RECT_FRAME )
	{
		int i;
		abase::vector<AUITEXTAREA_NAME_LINK> &vecNameLink = pText->GetNameLink();

		x += rcWindow.left;
		y += rcWindow.top;
		for( i = 0; i < (int)vecNameLink.size(); i++ )
		{
			if( vecNameLink[i].rc.PtInRect(x, y) )
			{
				GetGame()->ChangeCursor(RES_CUR_HAND);
				*pLink = vecNameLink[i];
				break;
			}
		}
		bClickedChatPart = true;
	}

	return bClickedChatPart;
}

void CDlgBoothSelf::OnEventMouseMove_MsgList(WPARAM wParam, LPARAM lParam, AUIObject *pObj)
{
	A3DVIEWPORTPARAM *p = m_pA3DEngine->GetActiveViewport()->GetParam();
	int x = GET_X_LPARAM(lParam) - p->X;
	int y = GET_Y_LPARAM(lParam) - p->Y;
	AUITEXTAREA_NAME_LINK Link;
	
	GetNameLinkMouseOn(x, y, pObj, &Link);
}

void CDlgBoothSelf::OnEventLButtonDown_MsgList(WPARAM wParam, LPARAM lParam, AUIObject *pObj)
{
	A3DVIEWPORTPARAM *p = m_pA3DEngine->GetActiveViewport()->GetParam();
	int x = GET_X_LPARAM(lParam) - p->X;
	int y = GET_Y_LPARAM(lParam) - p->Y;

	AUITEXTAREA_NAME_LINK Link;
	GetNameLinkMouseOn(x, y, pObj, &Link);
	if( Link.strName.GetLength() == 0 ) return;

	CDlgChat *pChat = GetGameUIMan()->m_pDlgChat;
	pChat->SwitchToPrivateChat(Link.strName);
}

void CDlgBoothSelf::AddBoothMessage(const ACHAR *pszMsg)
{
	ACHAR *pMsg = (ACHAR*)pszMsg;
	int i = 0;
	int p1 = 0,p2 = 0;
	int j = 0;
	while( pMsg[j] != '\0')
	{
		if( pMsg[j] == '^' )
		{
			j++;
			if( pMsg[j] == '\0' )
				break;
		}
		else if( pMsg[j] == '&')
		{
			i++;
			if( i == 1 )
				p1 = j;
			if( i == 2 )
			{
				p2 = j;
				ACHAR szPlayer[100];
				a_strncpy(szPlayer, pMsg + p1 , p2 - p1 + 1);
				szPlayer[p2 - p1 + 1] = '\0';
				PAUITEXTAREA pMsgList = dynamic_cast<PAUITEXTAREA>(GetDlgItem("Txt_MsgList"));
				pMsg = a_strstr(pMsg + j, GetStringFromTable(200));
				if( pMsg != NULL)
				{
					ACString strBoothMsg;
					ACString strMsg = pMsg+1;
					GetGameRun()->GetUIManager()->FilterBadWords(strMsg);
					strBoothMsg.Format(GetStringFromTable(778),szPlayer, strMsg);
					int nMsgLen = strBoothMsg.GetLength();
					strBoothMsg[nMsgLen - 2] = '\r';
					if( pMsgList )
						pMsgList->AppendText(strBoothMsg.Left(nMsgLen - 1));
				}
				break;
			}
		}
		j++;
	}
}

void CDlgBoothSelf::OnItemDragDrop(CECIvtrItem* pIvtrSrc, int iSrc, PAUIOBJECT pObjSrc, PAUIOBJECT pObjOver)
{
	// for right-click scene
	if(!pObjOver)
	{
		// click when press control means to buy a item
		AString strPrefix = AUI_PRESS(VK_CONTROL) ? 
			"BuyItem_" : "SellItem_";
		CECDealInventory* pBooth = AUI_PRESS(VK_CONTROL) ? 
			GetHostPlayer()->GetBoothBuyPack() : GetHostPlayer()->GetBoothSellPack();
		int slot = (pBooth->GetItemIndex(iSrc) < 0) ? 
			pBooth->SearchEmpty() : -1;
		
		if(slot >= 0)
		{
			AString strName;
			pObjOver = GetDlgItem(strName.Format("%s%02d", strPrefix, slot+1));
		}
	}

	if( this->GetDlgItem("DEFAULT_Txt_Name")->IsEnabled() )
	{
		if( !pIvtrSrc->IsTradeable() )
			GetGameUIMan()->MessageBox("", GetStringFromTable(652), MB_OK, A3DCOLORRGBA(255, 255, 255, 160));

		else
		{
			ACHAR szText[40];
			PAUIEDITBOX pEdit;
			PAUIDIALOG pShow = GetGameUIMan()->GetDialog("Win_InputNO2");
			
			pEdit = dynamic_cast<PAUIEDITBOX>(pShow->GetDlgItem("Txt_Price"));
			pEdit->SetIsNumberOnly(true);
			a_sprintf(szText, _AL("%d"), pIvtrSrc->GetUnitPrice());
			pEdit->SetText(szText);
			pEdit->SetData(iSrc);
			pShow->ChangeFocus(pEdit);
			
			pEdit = dynamic_cast<PAUIEDITBOX>(pShow->GetDlgItem("Txt_Amount"));
			pEdit->SetIsNumberOnly(true);
			pEdit->SetText(_AL("1"));
			pEdit->Enable(pIvtrSrc->GetPileLimit() > 0 ? true : false);
			
			if(pObjOver && (strstr(pObjOver->GetName(), "BuyItem_") || !stricmp(pObjOver->GetName(), "±³¾°01")))
				pShow->SetData(CDlgInputNO::INPUTNO_BUY_ADD);
			else
				pShow->SetData(CDlgInputNO::INPUTNO_SELL_ADD);
			pShow->SetDataPtr(pIvtrSrc,"ptr_CECIvtrItem");
			pShow->Show(true, true);
		}
	}
}

void CDlgBoothElse::OnItemDragDrop(CECIvtrItem* pIvtrSrc, int iSrc, PAUIOBJECT pObjSrc, PAUIOBJECT pObjOver)
{
	CECDealInventory *pSell = GetHostPlayer()->GetSellPack();
	CECDealInventory *pBuy = GetHostPlayer()->GetEPBoothBuyPack();

	CECIvtrItem* pIvtrDst = NULL;
	if( !pObjOver )
	{
		int iMaxPrice = 0;
		for(int i=0; i<pBuy->GetSize(); i++)
		{
			CECIvtrItem* pIvtr = pBuy->GetItem(i);
			if( !pIvtr ) continue;

			AString str;
			str.Format("BuyItem_%02d", i + 1);
			PAUIOBJECT pObj = GetDlgItem(str);
			if( !pObj ) continue;

			int iOrigin = pBuy->GetItemInfo(i).iOrigin;
			if( pIvtr->GetTemplateID() == pIvtrSrc->GetTemplateID() && pIvtr->GetUnitPrice() > iMaxPrice &&
				pSell->GetItemCount(iOrigin) < pIvtr->GetCount() )
			{
				pIvtrDst = pIvtr;
				iMaxPrice = pIvtr->GetUnitPrice();
				pObjOver = pObj;
			}
		}
	}
	else if( strstr(pObjOver->GetName(), "BuyItem_") )
		pIvtrDst = (CECIvtrItem*)pObjOver->GetDataPtr("ptr_CECIvtrItem");

	if( pObjOver && pIvtrDst )
	{
		if( pIvtrSrc->GetTemplateID() != pIvtrDst->GetTemplateID() ) return;
		
		if( pIvtrSrc->GetPileLimit() > 1 && pIvtrDst->GetCount() > 1 )
		{
			PAUIOBJECT pEdit = GetGameUIMan()->m_pDlgInputNO->GetDlgItem("DEFAULT_Txt_No.");
			
			pEdit->SetData(min(pIvtrSrc->GetCount(), pIvtrDst->GetCount()));
			pEdit->SetText(_AL("1"));

			int iDst = atoi(pObjOver->GetName() + strlen("BuyItem_")) - 1;
			GetGameUIMan()->m_pDlgInputNO->SetData((DWORD)pObjOver, "ptr_AUIObject");
			GetGameUIMan()->m_pDlgInputNO->SetDataPtr(pObjSrc,"ptr_AUIObject");
			GetGameUIMan()->m_pDlgInputNO->Show(true, true);
			GetGameUIMan()->m_pDlgInputNO->SetType(CDlgInputNO::INPUTNO_SELL_ADD);
		}
		else
		{
			int iDst = atoi(pObjOver->GetName() + strlen("BuyItem_")) - 1;
			int iOrigin = pBuy->GetItemInfo(iDst).iOrigin;

			if( pSell->GetItemCount(iOrigin) + 1 <= pIvtrDst->GetCount() )
			{
				int nIndex = pSell->AddBoothItem(pIvtrSrc,
					iOrigin, 1, pIvtrDst->GetUnitPrice());
				
				pIvtrSrc->Freeze(true);
				pBuy->GetItem(iDst)->Freeze(true);
				pSell->SetItemInfoFlag(nIndex, iSrc);
			}
		}
	}
}


///////////////////////////////////////////////////////////////////////////
// IMPLEMENTING CLASS CDlgBoothSet

CDlgBoothSet::CDlgBoothSet()
{
	m_pLbl_BagMoney = NULL;
	m_pTxt_MoneyMin = NULL;
	m_pTxt_MoneyMax = NULL;
}

bool CDlgBoothSet::OnInitDialog()
{
	m_pLbl_BagMoney = (PAUILABEL)GetDlgItem("Txt_BagMoney");
	m_pTxt_MoneyMin = GetDlgItem("Txt_GoldMin");
	m_pTxt_MoneyMax = GetDlgItem("Txt_GoldMax");
	return true;
}

void CDlgBoothSet::OnShowDialog()
{
	ACString str;
	int low_money, high_money;
	GetHostPlayer()->GetAutoYinpiaoMoney(low_money, high_money);
	str.Format(_AL("%d"), low_money);
	m_pTxt_MoneyMin->SetText(str);
	str.Format(_AL("%d"), high_money);
	m_pTxt_MoneyMax->SetText(str);
}

void CDlgBoothSet::OnTick()
{
	m_pLbl_BagMoney->SetColor(GetGameUIMan()->GetPriceColor(GetHostPlayer()->GetMoneyAmount()));
	m_pLbl_BagMoney->SetText(GetGameUIMan()->GetFormatNumber(GetHostPlayer()->GetMoneyAmount()));
}

void CDlgBoothSet::OnCommand_Confirm(const char * szCommand)
{
	int low_money = a_atoi(m_pTxt_MoneyMin->GetText());
	int high_money = a_atoi(m_pTxt_MoneyMax->GetText());

	if( low_money > MAX_MONEY_COUNT || low_money < MONEY_EMPTY )
	{
		GetGameUIMan()->MessageBox("", GetGameUIMan()->GetStringFromTable(10019), MB_OK, A3DCOLORRGB(255, 255, 255));
		return;
	}

	if( high_money < MIN_MONEY_COUNT || high_money > MONEY_FULL )
	{
		GetGameUIMan()->MessageBox("", GetGameUIMan()->GetStringFromTable(10020), MB_OK, A3DCOLORRGB(255, 255, 255));
		return;
	}

	if( high_money - low_money < MONEY_PER_CHECK )
	{
		GetGameUIMan()->MessageBox("", GetGameUIMan()->GetStringFromTable(10021), MB_OK, A3DCOLORRGB(255, 255, 255));
		return;
	}

	GetHostPlayer()->SetAutoYinpiaoMoney(low_money, high_money);
	Show(false);
}
