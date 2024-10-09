// Filename	: DlgInputNO.cpp
// Creator	: Tom Zhou
// Date		: October 10, 2005

#include "DlgInputNO.h"
#include "DlgShop.h"
#include "DlgTrade.h"
#include "DlgMailWrite.h"
#include "DlgFortressMaterial.h"
#include "DlgGMQueryItem.h"
#include "DlgTouchShop.h"
#include "EC_ShortcutMgr.h"
#include "EC_DealInventory.h"
#include "EC_Game.h"
#include "EC_GameRun.h"
#include "EC_GameSession.h"
#include "EC_GameUIMan.h"
#include "EC_Global.h"
#include "EC_HostPlayer.h"
#include "EC_IvtrItem.h"
#include "EC_World.h"
#include "EC_Utility.h"
#include "EC_OfflineShopCtrl.h"


#define new A_DEBUG_NEW

AUI_BEGIN_COMMAND_MAP(CDlgInputNO, CDlgBase)

AUI_ON_COMMAND("max",				OnCommand_max)
AUI_ON_COMMAND("add",				OnCommand_add)
AUI_ON_COMMAND("minus",				OnCommand_minus)
AUI_ON_COMMAND("confirm",			OnCommand_confirm)
AUI_ON_COMMAND("IDCANCEL",			OnCommand_CANCEL)

AUI_END_COMMAND_MAP()

CDlgInputNO::CDlgInputNO()
{
	m_nInputNO = INPUTNO_NULL;
}

CDlgInputNO::~CDlgInputNO()
{
}

void CDlgInputNO::OnCommand_max(const char * szCommand)
{
	ACHAR szText[40];
	int nMax = (int)GetDlgItem("DEFAULT_Txt_No.")->GetData();

	a_sprintf(szText, _AL("%d"), nMax);
	GetDlgItem("DEFAULT_Txt_No.")->SetText(szText);
}

void CDlgInputNO::OnCommand_add(const char * szCommand)
{
	ACHAR szText[40];
	PAUIOBJECT pEdit = GetDlgItem("DEFAULT_Txt_No.");
	int nMax = (int)pEdit->GetData();
	int nNum = a_atoi(pEdit->GetText());

	if( nNum < nMax ) nNum++;
	a_sprintf(szText, _AL("%d"), nNum);
	pEdit->SetText(szText);
	ChangeFocus(pEdit);
}

void CDlgInputNO::OnCommand_minus(const char * szCommand)
{
	ACHAR szText[40];
	PAUIOBJECT pEdit = GetDlgItem("DEFAULT_Txt_No.");
	int nMax = (int)pEdit->GetData();
	int nNum = a_atoi(pEdit->GetText());

	if( nNum > 0 ) nNum--;
	a_sprintf(szText, _AL("%d"), nNum);
	pEdit->SetText(szText);
	ChangeFocus(pEdit);
}

void CDlgInputNO::OnCommand_confirm(const char * szCommand)
{
	int nInputNO = m_nInputNO;
	int nMax = (int)GetDlgItem("DEFAULT_Txt_No.")->GetData();
	int nNum = a_atoi(GetDlgItem("DEFAULT_Txt_No.")->GetText());
	if( nNum <= 0 ) return;
	a_Clamp(nNum, 1, nMax);

	CECInventory *pPack = GetHostPlayer()->GetPack();
	CECDealInventory *pBuy = GetHostPlayer()->GetBuyPack();
	CECDealInventory *pSell = GetHostPlayer()->GetSellPack();
	CECDealInventory *pDeal = GetHostPlayer()->GetDealPack();
	CECDealInventory *pBoothBuy = GetHostPlayer()->GetEPBoothBuyPack();
	CECDealInventory *pBoothSell = GetHostPlayer()->GetEPBoothSellPack();

	if( nInputNO == INPUTNO_TRADE_MONEY )
	{
		int idTrade = GetGameUIMan()->m_idTrade;
		int nTradeMoney = pDeal->GetMoney();

		if( nNum > nTradeMoney )
			GetGameSession()->trade_AddGoods(idTrade, 0, 0, 0, nNum - nTradeMoney);
		else if( nNum < nTradeMoney )
			GetGameSession()->trade_RemoveGoods(idTrade, 0, 0, 0, nTradeMoney - nNum);
	}
	else if( nInputNO == INPUTNO_TRADE_MOVE )
	{
		int idTrade = GetGameUIMan()->m_idTrade;
		PAUIOBJECT pObjSrc = (PAUIOBJECT)GetDataPtr("ptr_AUIObject");
		CECIvtrItem *pItem = (CECIvtrItem *)pObjSrc->GetDataPtr("ptr_CECIvtrItem");
		int idItem = pItem->GetTemplateID();
		int iSrc = atoi(pObjSrc->GetName() + strlen("Item_"));
		PAUIOBJECT pObjDst = (PAUIOBJECT)GetData("ptr_AUIObject");
		int iDst = atoi(pObjDst->GetName() + strlen("Item_"));

		GetGameSession()->trade_MoveItem(idTrade, idItem, iSrc, iDst, nNum);
	}
	else if( nInputNO == INPUTNO_TRADE_ADD )
	{
		int idTrade = GetGameUIMan()->m_idTrade;
		PAUIOBJECT pObj = (PAUIOBJECT)GetDataPtr("ptr_AUIObject");
		CECIvtrItem *pItem = (CECIvtrItem *)pObj->GetDataPtr("ptr_CECIvtrItem");
		int idItem = pItem->GetTemplateID();
		int iSrc = atoi(pObj->GetName() + strlen("Item_"));

		GetGameSession()->trade_AddGoods(idTrade, idItem, iSrc, nNum, 0);
		pItem->Freeze(true);
	}
	else if( nInputNO == INPUTNO_BUY_ADD )
	{
		char szItem[40];
		PAUIOBJECT pButton;
		int nPage, iSrc, iOrigin;
		PAUIOBJECT pObj = (PAUIOBJECT)GetDataPtr("ptr_AUIObject");
		CECIvtrItem *pItem = (CECIvtrItem *)pObj->GetDataPtr("ptr_CECIvtrItem");

		if( m_pAUIManager->GetDialog("Win_Shop")->IsShow() )
		{
			PAUIDIALOG pShop = m_pAUIManager->GetDialog("Win_Shop");
			int nCurShopSet = GetGameUIMan()->m_pDlgShop->GetCurShopSet();

			sprintf(szItem, "Btn_Set%d", nCurShopSet);
			pButton = pShop->GetDlgItem(szItem);
			nPage = pButton->GetData();
			iSrc = atoi(pObj->GetName() + strlen("U_")) + nPage * IVTRSIZE_NPCPACK - 1;
			pBuy->AddItemFree(pItem, iSrc, nNum);
		}
		else
		{
			iSrc = atoi(pObj->GetName() + strlen("SellItem_")) - 1;
			iOrigin = pBoothSell->GetItemInfo(iSrc).iOrigin;
			pBuy->AddBoothItem(pItem, iOrigin, nNum, pItem->GetUnitPrice());
			pItem->Freeze(true);
		}
	}
	else if( nInputNO == INPUTNO_BUY_REMOVE )
	{
		PAUIOBJECT pObj = (PAUIOBJECT)GetDataPtr("ptr_AUIObject");
		CECIvtrItem *pItem = (CECIvtrItem *)pObj->GetDataPtr("ptr_CECIvtrItem");
		int iSrc = atoi(pObj->GetName() + strlen("Buy_")) - 1;

		pBuy->RemoveItemByIndex(iSrc, nNum);
	}
	else if( nInputNO == INPUTNO_SELL_ADD )
	{
		int iSrc, iDst, iOrigin, nIndex;
		PAUIOBJECT pObjSrc = (PAUIOBJECT)GetDataPtr("ptr_AUIObject");
		CECIvtrItem *pIvtrSrc = (CECIvtrItem *)pObjSrc->GetDataPtr("ptr_CECIvtrItem");

		if( m_pAUIManager->GetDialog("Win_Shop")->IsShow() )
		{
			if( pIvtrSrc->GetClassID() == CECIvtrItem::ICID_MONEYCONVERTIBLE )
			{
				ACString strDesc, strPrice;
				__int64 iiMoney = (__int64)pIvtrSrc->GetUnitPrice()*GetHostPlayer()->GetCashMoneyRate()*nNum;
				strPrice = A3DCOLOR_TO_STRING(GetGameUIMan()->GetPriceColor(iiMoney)) + g_pGame->GetFormattedPrice(iiMoney);
				strDesc.Format(GetGameUIMan()->GetStringFromTable(10015), strPrice);
				GetGameUIMan()->MessageBox("", strDesc, MB_OK, A3DCOLORRGB(255, 255, 255));
			}

			iSrc = atoi(pObjSrc->GetName() + strlen("Item_"));
			pSell->AddItem(pIvtrSrc, iSrc, nNum);
			pIvtrSrc->Freeze(true);
		}
		else
		{
			PAUIOBJECT pObjDst = (PAUIOBJECT)GetData("ptr_AUIObject");
			CECIvtrItem *pIvtrDst = (CECIvtrItem *)pObjDst->GetDataPtr("ptr_CECIvtrItem");

			iSrc = atoi(pObjSrc->GetName() + strlen("Item_"));
			iDst = atoi(pObjDst->GetName() + strlen("BuyItem_")) - 1;
			iOrigin = pBoothBuy->GetItemInfo(iDst).iOrigin;
			if( pSell->GetItemCount(iOrigin) + nNum <= pIvtrDst->GetCount() )
			{
				nIndex = pSell->AddBoothItem(pIvtrSrc, iOrigin, nNum, pIvtrDst->GetUnitPrice());
				pSell->SetItemInfoFlag(nIndex, iSrc);
				pIvtrSrc->Freeze(true);
				pIvtrDst->Freeze(true);
			}
		}
	}
	else if( nInputNO == INPUTNO_SELL_REMOVE )
	{
		PAUIOBJECT pObj = (PAUIOBJECT)GetDataPtr("ptr_AUIObject");
		CECIvtrItem *pItem = (CECIvtrItem *)pObj->GetDataPtr("ptr_CECIvtrItem");
		int iSrc = atoi(pObj->GetName() + strlen("Sell_")) - 1;
		const CECDealInventory::ITEMINFO &ii = pSell->GetItemInfo(iSrc);

		if( ii.iAmount == nNum ) pItem->Freeze(false);
		pSell->RemoveItemByIndex(iSrc, nNum);
	}
	else if( nInputNO == INPUTNO_DROP_ITEM )
	{
		PAUIOBJECT pObj = (PAUIOBJECT)GetDataPtr("ptr_AUIObject");
		int iSrc = atoi(pObj->GetName() + strlen("Item_"));

		GetGameSession()->c2s_CmdDropIvtrItem(iSrc, nNum);
	}
	else if( nInputNO == INPUTNO_DROP_MONEY )
	{
		GetGameSession()->c2s_CmdThrowMoney(nNum);
	}
	else if( nInputNO == INPUTNO_MOVE_ITEM )
	{
		PAUIOBJECT pObjSrc = (PAUIOBJECT)GetDataPtr("ptr_AUIObject");
		int iSrc = atoi(pObjSrc->GetName() + strlen("Item_"));
		PAUIOBJECT pObjDst = (PAUIOBJECT)GetData("ptr_AUIObject");
		int iDst = atoi(pObjDst->GetName() + strlen("Item_"));

		GetGameSession()->c2s_CmdMoveIvtrItem(iSrc, iDst, nNum);
	}
	else if( nInputNO == INPUTNO_STORAGE_TRASH_MONEY )
	{
		bool bAccountBox = GetGameUIMan()->GetDialog("Win_Storage3")->IsShow();
		GetGameSession()->c2s_CmdExgTrashBoxMoney(nNum, 0, bAccountBox?1:0);
	}
	else if( nInputNO == INPUTNO_STORAGE_IVTR_MONEY )
	{
		bool bAccountBox = GetGameUIMan()->GetDialog("Win_Storage3")->IsShow();
		GetGameSession()->c2s_CmdExgTrashBoxMoney(0, nNum, bAccountBox?1:0);
	}
	else if( nInputNO == INPUTNO_STORAGE_GET_ITEMS )
	{
		PAUIOBJECT pObjSrc = (PAUIOBJECT)GetDataPtr("ptr_AUIObject");
		int iSrc = atoi(pObjSrc->GetName() + strlen("Item_")) - 1;
		PAUIOBJECT pObjDst = (PAUIOBJECT)GetData("ptr_AUIObject");
		int iDst = atoi(pObjDst->GetName() + strlen("Item_"));
		int iSrcTrashBox = IVTRTYPE_TRASHBOX + atoi(pObjSrc->GetParent()->GetName() + strlen("Win_Storage"));

		GetGameSession()->c2s_CmdMoveTrashBoxToIvtr(iSrcTrashBox, iSrc, iDst, nNum);
	}
	else if( nInputNO == INPUTNO_STORAGE_PUT_ITEMS )
	{
		PAUIOBJECT pObjSrc = (PAUIOBJECT)GetDataPtr("ptr_AUIObject");
		int iSrc = atoi(pObjSrc->GetName() + strlen("Item_"));
		PAUIOBJECT pObjDst = (PAUIOBJECT)GetData("ptr_AUIObject");
		int iDst = atoi(pObjDst->GetName() + strlen("Item_")) - 1;
		int iDstTrashBox = IVTRTYPE_TRASHBOX + atoi(pObjDst->GetParent()->GetName() + strlen("Win_Storage"));

		GetGameSession()->c2s_CmdMoveIvtrToTrashBox(iDstTrashBox, iDst, iSrc, nNum);
	}
	else if( nInputNO == INPUTNO_STORAGE_MOVE_ITEMS )
	{
		PAUIOBJECT pObjSrc = (PAUIOBJECT)GetDataPtr("ptr_AUIObject");
		int iSrc = atoi(pObjSrc->GetName() + strlen("Item_")) - 1;
		PAUIOBJECT pObjDst = (PAUIOBJECT)GetData("ptr_AUIObject");
		int iDst = atoi(pObjDst->GetName() + strlen("Item_")) - 1;
		int iSrcTrashBox = IVTRTYPE_TRASHBOX + atoi(pObjSrc->GetParent()->GetName() + strlen("Win_Storage"));

		GetGameSession()->c2s_CmdMoveTrashBoxItem(iSrcTrashBox, iSrc, iDst, nNum);
	}
	else if( nInputNO == INPUTNO_MOVE_MAILATTACH )
	{
		PAUIOBJECT pObjSrc = (PAUIOBJECT)GetDataPtr("ptr_AUIObject");
		CECIvtrItem *pItem = (CECIvtrItem *)pObjSrc->GetDataPtr("ptr_CECIvtrItem");
		pItem->Freeze(true);
		int iSrc = atoi(pObjSrc->GetName() + strlen("Item_"));

		((CDlgMailWrite *)GetGameUIMan()->GetDialog("Win_MailWrite"))->SetAttachItem(
			pItem, nNum, iSrc);
	}
	else if( nInputNO == INPUTNO_ATTACH_MONEY )
	{
		CDlgMailWrite *pDlgMailWrite = dynamic_cast<CDlgMailWrite *>(GetGameUIMan()->GetDialog("Win_MailWrite"));
		int nCur(0), nMax(0);
		GetInfo(nCur, nMax);
		pDlgMailWrite->AttachGold(nCur);
	}
	else if (nInputNO == INPUTNO_FACTION_MATERIAL)
	{
		PAUIOBJECT pObjSrc = (PAUIOBJECT)GetDataPtr("ptr_AUIObject");
		CECIvtrItem *pItem = (CECIvtrItem *)pObjSrc->GetDataPtr("ptr_CECIvtrItem");
		int iSrc = atoi(pObjSrc->GetName() + strlen("Item_"));

		CDlgFortressMaterial * pDlgMaterial = (CDlgFortressMaterial *)GetGameUIMan()->GetDialog("Win_FortressMaterial");
		if (pDlgMaterial)
			pDlgMaterial->ResetMaterial(iSrc, nNum);
	}	
	else if(nInputNO == INPUTNO_GM_REMOVE_ITEM)
	{
		CDlgGMQueryItem* pDlgQuery = (CDlgGMQueryItem*)GetGameUIMan()->GetDialog("Win_GMQueryItem");
		if(pDlgQuery)
		{
			pDlgQuery->ConfirmCount(nNum);
		}
	}
	else if( nInputNO == INPUTNO_CLICK_SHORTCUT )
	{
		PAUIOBJECT pObjSrc = (PAUIOBJECT)GetDataPtr("ptr_AUIObject");
		// HACK: use the same name as ptr_AUIObject to store the shortcut pointer
		CECShortcutMgr::ClickShortcut* pCS = (CECShortcutMgr::ClickShortcut*)GetData("ptr_AUIObject");
		pCS->Trigger(pObjSrc, nNum);
	}
	else if (nInputNO == INPUTNO_OFFLINESHOP_SHOPITEM_SELL) //
	{
		COfflineShopCtrl* pCtrl = GetHostPlayer()->GetOfflineShopCtrl();
		if (pCtrl)
		{
			int idx = GetData();
			CECIvtrItem* pItem = (CECIvtrItem*)GetDataPtr("ptr_CECIvtrItem");
			ASSERT(pCtrl->GetELShopItem(true,idx) == pItem);
			if (pItem == pCtrl->GetELShopItem(true,idx))
			{
				if(GetHostPlayer()->GetPack()->SearchEmpty()<0)
				{
					GetGameUIMan()->MessageBox("",GetGameUIMan()->GetStringFromTable(704),MB_OK,A3DCOLORRGB(255,255,255));
					return;
				}
				if(pItem->GetCount()< nNum)
				{
					GetGameUIMan()->MessageBox("",GetGameUIMan()->GetStringFromTable(10529),MB_OK,A3DCOLORRGB(255,255,255));
					return;
				}

				__int64 needM = (unsigned int)pItem->GetUnitPrice() * (__int64)nNum;
				__int64 yinpiao(0), money(0);

				if (pCtrl->CanBuyItemByPrice(needM,yinpiao,money))
				{
					ACString strMsg;
					strMsg.Format(GetGameUIMan()->GetStringFromTable(10527),(int)yinpiao,GetGameUIMan()->GetFormatNumber(money),nNum,pItem->GetName());

					PAUIDIALOG pMsgDlg=NULL;
					int* pData = (int*)a_malloctemp(sizeof(int)*6);
					pData[0] = pCtrl->GetCurOtherShopID();
					pData[1] = pItem->GetTemplateID();
					pData[2] = idx;
					pData[3] = nNum;
					pData[4] = (int)money;
					pData[5] = (int)yinpiao;

					GetGameUIMan()->MessageBox("OfflineShopItemSell",strMsg,MB_OKCANCEL,A3DCOLORRGB(255,255,255),&pMsgDlg);
					pMsgDlg->SetDataPtr(pData);

				//	GetGameSession()->OffShop_BuyItemFrom(pCtrl->GetCurOtherShopID(),pItem->GetTemplateID(),idx,nNum,(int)needM,(int)needY);
				}
				else
				{
					GetGameUIMan()->MessageBox("",GetGameUIMan()->GetStringFromTable(10514),MB_OK,A3DCOLORRGB(255,255,255));
				}				
			}
		}
	}
	else if (nInputNO == INPUTNO_OFFLINESHOP_SHOPITEM_BUY)
	{
		COfflineShopCtrl* pCtrl = GetHostPlayer()->GetOfflineShopCtrl();
		if (pCtrl)
		{
			int idx = GetData();
			CECIvtrItem* pItem = (CECIvtrItem*)GetDataPtr("ptr_CECIvtrItem");
			ASSERT(pCtrl->GetELShopItem(false,idx) == pItem);
			if (pItem == pCtrl->GetELShopItem(false,idx))
			{
				int inv_idx = GetHostPlayer()->GetPack()->FindItem(pItem->GetTemplateID());
				if (inv_idx>=0)
				{
					if(GetHostPlayer()->GetPack()->GetItemTotalNum(pItem->GetTemplateID())< nNum)
					{
						GetGameUIMan()->MessageBox("",GetGameUIMan()->GetStringFromTable(10526),MB_OK,A3DCOLORRGB(255,255,255));
						return;
					}
					
					unsigned int nPrice = (unsigned int)pItem->GetUnitPrice();

					if (pCtrl->CanSellItemByPrice(nPrice))
					{
						ACString strMsg; 								
						strMsg.Format(GetGameUIMan()->GetStringFromTable(10528),GetGameUIMan()->GetFormatNumber(nPrice),nNum,pItem->GetName());
						
						PAUIDIALOG pMsgDlg=NULL;
						int* pData = (int*)a_malloctemp(sizeof(int) * 6);
						pData[0] = pCtrl->GetCurOtherShopID();
						pData[1] = pItem->GetTemplateID();
						pData[2] = idx;
						pData[3] = nNum;
						pData[4] = (int)nPrice;
						pData[5] = inv_idx;
						
						GetGameUIMan()->MessageBox("OfflineShopItemBuy",strMsg,MB_OKCANCEL,A3DCOLORRGB(255,255,255),&pMsgDlg);
						pMsgDlg->SetDataPtr(pData);
					}
					else
					{
						GetGameUIMan()->MessageBox("",GetGameUIMan()->GetStringFromTable(704),MB_OK,A3DCOLORRGB(255,255,255));
					}
				//	GetGameSession()->OffShop_SellItemTo(pCtrl->GetCurOtherShopID(),pItem->GetTemplateID(),idx,nNum,pItem->GetUnitPrice(),inv_idx);
				}
			}
		}
	}
	else if (nInputNO == INPUTNO_OFFLINESHOP_SEARCHITEM_SELL)
	{
		COfflineShopCtrl* pCtrl = GetHostPlayer()->GetOfflineShopCtrl();
		if (pCtrl)
		{
			int idx = GetData();
			CECIvtrItem* pItem = (CECIvtrItem*)GetDataPtr("ptr_CECIvtrItem");
			COfflineShopCtrl::ItemInfo* pInfo = pCtrl->GetQueryResultItem(true,idx);
			ASSERT(pInfo && pInfo->pItem == pItem);
			if (pInfo && pItem == pInfo->pItem)
			{
				if(GetHostPlayer()->GetPack()->SearchEmpty()<0)
				{
					GetGameUIMan()->MessageBox("",GetGameUIMan()->GetStringFromTable(704),MB_OK,A3DCOLORRGB(255,255,255));
					return;
				}
				if(pItem->GetCount()< nNum)
				{
					GetGameUIMan()->MessageBox("",GetGameUIMan()->GetStringFromTable(10529),MB_OK,A3DCOLORRGB(255,255,255));
					return;
				}

				__int64 needM = (unsigned int)pItem->GetUnitPrice() * (__int64)nNum;
					
				__int64 yinpiao(0),money(0);
				if (pCtrl->CanBuyItemByPrice(needM,yinpiao,money))
				{
					ACString strMsg;
					strMsg.Format(GetGameUIMan()->GetStringFromTable(10527),(int)yinpiao,GetGameUIMan()->GetFormatNumber(money),nNum,pItem->GetName());

					PAUIDIALOG pMsgDlg=NULL;
					int* pData = (int*)a_malloctemp(sizeof(int)*6);
					pData[0] = pInfo->roleid;
					pData[1] = pItem->GetTemplateID();
					pData[2] = pInfo->pos;
					pData[3] = nNum;
					pData[4] = (int)money;
					pData[5] = (int)yinpiao;

					GetGameUIMan()->MessageBox("OfflineShopItemSell",strMsg,MB_OKCANCEL,A3DCOLORRGB(255,255,255),&pMsgDlg);
					pMsgDlg->SetDataPtr(pData);

			//		GetGameSession()->OffShop_BuyItemFrom(pInfo->roleid,pItem->GetTemplateID(),pInfo->pos,nNum,(int)needM,needY);
				}
				else
				{
					GetGameUIMan()->MessageBox("",GetGameUIMan()->GetStringFromTable(10514),MB_OK,A3DCOLORRGB(255,255,255));
				}					
			}
		}
	}
	else if (nInputNO == INPUTNO_OFFLINESHOP_SEARCHITEM_BUY)
	{
		int idx = GetData();
		COfflineShopCtrl* pCtrl = GetHostPlayer()->GetOfflineShopCtrl();
		if (pCtrl)
		{
			CECIvtrItem* pItem = (CECIvtrItem*)GetDataPtr("ptr_CECIvtrItem");
			COfflineShopCtrl::ItemInfo* pInfo = pCtrl->GetQueryResultItem(false,idx);
			ASSERT(pInfo && pInfo->pItem == pItem);
			if (pInfo && pItem == pInfo->pItem)
			{
				int inv_idx = GetHostPlayer()->GetPack()->FindItem(pItem->GetTemplateID());
				if (inv_idx>=0)
				{
					if(GetHostPlayer()->GetPack()->GetItemTotalNum(pItem->GetTemplateID())< nNum)
					{
						GetGameUIMan()->MessageBox("",GetGameUIMan()->GetStringFromTable(10526),MB_OK,A3DCOLORRGB(255,255,255));
						return;
					}
					unsigned int nPrice = (unsigned int)pItem->GetUnitPrice();

					if (pCtrl->CanSellItemByPrice(nPrice))
					{
						ACString strMsg;
						strMsg.Format(GetGameUIMan()->GetStringFromTable(10528),GetGameUIMan()->GetFormatNumber(nPrice),nNum,pItem->GetName());
						
						PAUIDIALOG pMsgDlg=NULL;
						int* pData = (int*)a_malloctemp(sizeof(int)*6);
						pData[0] = pInfo->roleid;
						pData[1] = pItem->GetTemplateID();
						pData[2] = pInfo->pos;
						pData[3] = nNum;
						pData[4] = (int)nPrice;
						pData[5] = inv_idx;
						
						GetGameUIMan()->MessageBox("OfflineShopItemBuy",strMsg,MB_OKCANCEL,A3DCOLORRGB(255,255,255),&pMsgDlg);
						pMsgDlg->SetDataPtr(pData);
					}
					else
						GetGameUIMan()->MessageBox("",GetGameUIMan()->GetStringFromTable(704),MB_OK,A3DCOLORRGB(255,255,255));

				//	GetGameSession()->OffShop_SellItemTo(pInfo->roleid,pItem->GetTemplateID(),pInfo->pos,nNum,pItem->GetUnitPrice(),inv_idx);
				}				
			}
		}
	}
	else if (nInputNO == INPUTNO_TOUCHSHOP_ITEM_NUM) 
	{
		GetGameUIMan()->m_pDlgTouchShop->OnBuyShopItem(nNum);
	}

	Show(false);
}

void CDlgInputNO::OnCommand_CANCEL(const char * szCommand)
{
	Show(false);
}

void CDlgInputNO::SetType(int nType)
{
	m_nInputNO = nType;	
	GetDlgItem("Txt_Name")->SetText(GetStringFromTable(8090+nType));
}

bool CDlgInputNO::OnInitDialog()
{
	if(!CDlgBase::OnInitDialog())
	{
		return false;
	}

	PAUIEDITBOX pEdit = dynamic_cast<PAUIEDITBOX>(GetDlgItem("DEFAULT_Txt_No."));
	ASSERT(pEdit);
	if(pEdit) pEdit->SetIsNumberOnly(true);
	return true;
}

void CDlgInputNO::SetInfo(int nCur, int nMax)
{
	PAUIEDITBOX pEdit = dynamic_cast<PAUIEDITBOX>(GetDlgItem("DEFAULT_Txt_No."));
	ASSERT(pEdit);
	if(pEdit)
	{
		ACHAR szText[40];
		if(nCur < 0) nCur = 0;
		if(nMax < nCur) nMax = nCur;
		a_sprintf(szText, _AL("%d"), nCur);
		pEdit->SetText(szText);
		pEdit->SetData(nMax);
	}
}

void CDlgInputNO::GetInfo(int &nCur, int &nMax)
{
	PAUIOBJECT pEdit = GetDlgItem("DEFAULT_Txt_No.");
	ASSERT(pEdit);
	if(pEdit)
	{
		nMax = pEdit->GetData();

		ACString strNum = pEdit->GetText();
		if (!strNum.IsEmpty())
			nCur = a_atoi(strNum);
		else
			nCur = 0;
	}
	else
	{
		nMax = nCur = 0;
	}
}

//////////////////////////////////////////////////////////////////////////

AUI_BEGIN_COMMAND_MAP(CDlgInputNO3, CDlgBase)

AUI_ON_COMMAND("max",				OnCommand_max)
AUI_ON_COMMAND("confirm",			OnCommand_confirm)
AUI_ON_COMMAND("IDCANCEL",			OnCommand_CANCEL)

AUI_END_COMMAND_MAP()

AUI_BEGIN_EVENT_MAP(CDlgInputNO3, CDlgBase)

AUI_ON_EVENT("DEFAULT_Txt_No.",	WM_CHAR, OnEventChar_Txt_M)
AUI_ON_EVENT("DEFAULT_Txt_No2",	WM_CHAR, OnEventChar_Txt_Y)
AUI_END_EVENT_MAP()

CDlgInputNO3::CDlgInputNO3()
{
	m_nInputNO = INPUTNO_NULL;
	m_iInputMoney = 0;
	m_iInputYinPiao = 0;
	m_pTextMoney = NULL;
	m_pTextYinPiao = NULL;
}

CDlgInputNO3::~CDlgInputNO3()
{
}

void CDlgInputNO3::OnCommand_max(const char * szCommand)
{
	int nMax = (int)m_pTextMoney->GetData();

	SetMoney(nMax);

	nMax = (int)m_pTextYinPiao->GetData();
	SetYinPiao(nMax);
}
void CDlgInputNO3::SetMoney(int v)
{
	m_iInputMoney = v;
	m_pTextMoney->SetColor(GetGameUIMan()->GetPriceColor(v));
	m_pTextMoney->SetText(GetGameUIMan()->GetFormatNumber(v));
}
void CDlgInputNO3::SetYinPiao(int v)
{
	m_iInputYinPiao = v;
	m_pTextYinPiao->SetColor(GetGameUIMan()->GetPriceColor(v));
	m_pTextYinPiao->SetText(GetGameUIMan()->GetFormatNumber(v));
}
void CDlgInputNO3::SetMaxValue(int money,int yinpiao)
{
	m_pTextMoney->SetData(money);
	m_pTextYinPiao->SetData(yinpiao);
}
void CDlgInputNO3::OnCommand_confirm(const char * szCommand)
{
	COfflineShopCtrl* pCtrl = GetHostPlayer()->GetOfflineShopCtrl();
	if (!pCtrl) return ;

	int nInputNO = m_nInputNO;
	int nMax_M = (int)m_pTextMoney->GetData();
	int nMax_Y = (int)m_pTextYinPiao->GetData();
	int nNum_M = (int)GetMoney();//a_atoi(GetDlgItem("DEFAULT_Txt_No.")->GetText());
	int nNum_Y = GetYinPiao();
	if( nNum_M < 0 ) return;
	if( nNum_Y < 0 ) return;
	if(nNum_M == 0 && nNum_Y == 0) return;

	if ((nNum_M > nMax_M) || (nNum_Y > nMax_Y))
	{
		GetGameUIMan()->MessageBox("",GetGameUIMan()->GetStringFromTable(10538),MB_OK,A3DCOLORRGB(255,255,255));
		return;
	}

	a_Clamp(nNum_M, 0, nMax_M);
	a_Clamp(nNum_Y, 0, nMax_Y);

	if( nInputNO == INPUTNO_OFFLINESHOP_GETMONEY )
	{
		if (!pCtrl->CanPutMoneyToPack(nNum_Y,nNum_M))
		{
			GetGameUIMan()->MessageBox("",GetGameUIMan()->GetStringFromTable(704),MB_OK,A3DCOLORRGB(255,255,255));
			return;
		}
		if (!pCtrl->CanTakeMoneyFromStore(nNum_Y,nNum_M))
		{
			GetGameUIMan()->MessageBox("",GetGameUIMan()->GetStringFromTable(10537),MB_OK,A3DCOLORRGB(255,255,255));
			return;
		}
		GetGameSession()->OffShop_OptMoneyStore(false,nNum_M,nNum_Y);
	}
	else if( nInputNO == INPUTNO_OFFLINESHOP_SAVEMONEY )
	{
		if (nNum_M + pCtrl->GetMoneyInStore() > 200000000)
		{
			GetGameUIMan()->MessageBox("",GetGameUIMan()->GetStringFromTable(10538),MB_OK,A3DCOLORRGB(255,255,255));
			return;
		}
		if (!pCtrl->CheckMoneyStore(nNum_Y,nNum_M))
		{
			GetGameUIMan()->MessageBox("",GetGameUIMan()->GetStringFromTable(10532),MB_OK,A3DCOLORRGB(255,255,255));
			return ;
		}

		GetGameSession()->OffShop_OptMoneyStore(true,nNum_M,nNum_Y);
	}	

	Show(false);
}
void CDlgInputNO3::ComputeValue(PAUIEDITBOX pText,__int64& ret)
{
	if(!pText) return;

	ACString str = pText->GetText();
	int len = str.GetLength();
	
	ACHAR strView[32];
	memset(strView,0,sizeof(ACHAR)*32);
	ACHAR* pView = strView + 30; // 最后保留一个 '\0'
	
	__int64 val = 0;
	int ipow = 0;
	
	int iComma = 0;
	
	while(len>0)
	{
		ACHAR c = str[len-1];
		if (c >= '0' && c <= '9')
		{
			int cv = a_atoi(&c);
			val += cv * (__int64)pow(10,ipow);
			ipow++;
			
			pView--;
			*pView = c;
			iComma++;
			
			if (iComma==3)
			{
				pView--;
				*pView = L',';
				iComma=0;
			}
		}
		len--;
	}
	ret = val;
	if (*pView == L',')
	{
		pView++;
	}
	pText->SetText(pView);
}
void CDlgInputNO3::OnEventChar_Txt_M(WPARAM wParam, LPARAM lParam, AUIObject *pObj)
{
	ComputeValue(m_pTextMoney,m_iInputMoney);
}

void CDlgInputNO3::OnEventChar_Txt_Y(WPARAM wParam, LPARAM lParam, AUIObject *pObj)
{
	__int64 temp = 0;
	ComputeValue(m_pTextYinPiao,temp);
	m_iInputYinPiao = (int)temp;
}

void CDlgInputNO3::OnCommand_CANCEL(const char * szCommand)
{
	Show(false);
}

void CDlgInputNO3::SetType(int nType)
{
	m_nInputNO = nType;	
//	GetDlgItem("Txt_Name")->SetText(GetStringFromTable(8090+nType));
	if(nType == INPUTNO_OFFLINESHOP_GETMONEY)
		m_pTextYinPiao->SetMaxLength(6);
	else if (nType == INPUTNO_OFFLINESHOP_SAVEMONEY)
		m_pTextYinPiao->SetMaxLength(5);
}
void CDlgInputNO3::OnHideDialog()
{
	SetMoney(1);	
	SetYinPiao(0);
}
void CDlgInputNO3::OnShowDialog()
{
	ChangeFocus(m_pTextMoney);
}
bool CDlgInputNO3::OnInitDialog()
{
	if(!CDlgBase::OnInitDialog())
	{
		return false;
	}

	DDX_Control("DEFAULT_Txt_No.",m_pTextMoney);
	DDX_Control("DEFAULT_Txt_No2",m_pTextYinPiao);
	
	if(m_pTextMoney) 
	{
		m_pTextMoney->SetIsNumberOnly(true);
		
		int nMax = m_pTextMoney->GetMaxLength();
		if(nMax > 24)
			m_pTextMoney->SetMaxLength(24);
	}

	if(m_pTextYinPiao) 
	{
		m_pTextYinPiao->SetIsNumberOnly(true);
		
		int nMax = m_pTextYinPiao->GetMaxLength();
		if(nMax > 24)
			m_pTextYinPiao->SetMaxLength(24);
	}

	return true;
}