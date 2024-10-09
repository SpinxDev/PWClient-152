/*
* FILE: EC_OfflineShopCtrl.cpp
*
* DESCRIPTION: 
*
* CREATED BY: WYD
*
* HISTORY: 
*
* Copyright (c) 2013, All Rights Reserved.
*/

#include "EC_OfflineShopCtrl.h"

#include "EC_HostPlayer.h"
#include "EC_GameSession.h"
#include "EC_Global.h"
#include "EC_Game.h"
#include "EC_GameRun.h"
#include "EC_UIManager.h"
#include "DlgOfflineShopCreate.h"
#include "DlgOfflineShopItemsList.h"
#include "DlgOfflineShopSelfSetting.h"
#include "DlgOfflineShopList.h"
#include "EC_GameUIMan.h"
#include <Alog.h>
#include "DlgOfflineShopStore.h"

#include "pshopactive_re.hpp"
#include "pshopbuy_re.hpp"
#include "pshopcancelgoods_re.hpp"
#include "pshopcleargoods_re.hpp"
#include "pshopcreate_re.hpp"
#include "pshopdrawitem_re.hpp"
#include "pshoplist_re.hpp"
#include "pshoplistitem_re.hpp"
#include "pshopmanagefund_re.hpp"
#include "pshopplayerbuy_re.hpp"
#include "pshopplayerget_re.hpp"
#include "pshopplayersell_re.hpp"

#include "pshopsell_re.hpp"
#include "pshopsettype_re.hpp"

const __int64 MAX_MONEY_INSHOP = 100000000000; // 总价上限
const int YINPIAO = 10000000; // 银票
const __int64 MAX_UNIT_PRICE = 4000000000;// 单价上限
const int MAX_MONEY_INVENTORY = 200000000;// 包裹金钱上限
//////////////////////////////////////////////////////////////////////////
//
//
//
//////////////////////////////////////////////////////////////////////////

enum ERR_CODE_OFFLINESHOP
{
	ERR_PLAYERSHOP_VERIFY_FAILED = 500,
	ERR_SHOPMARKET_NOT_INIT = 501,
	ERR_PLAYERSHOP_NOTFIND = 502,
	ERR_PLAYERSHOP_EXIST = 503,
	ERR_PLAYERSHOP_BLIST_FULL = 504,
	ERR_PLAYERSHOP_SLIST_FULL = 505,
	ERR_PLAYERSHOP_STORE_FULL = 507,
	ERR_PLAYERSHOP_MONEY_FULL = 508,
	ERR_PLAYERSHOP_NO_MONEY = 509,
	ERR_PLAYERSHOP_EXPIRED = 510,
	ERR_PLAYERSHOP_BUSY = 511,
	ERR_PLAYERSHOP_ITEM_NOTFOUND = 512,
	ERR_PLAYERSHOP_ITEM_NOTMATCH = 513,
};

COfflineShopCtrl::COfflineShopCtrl(CECHostPlayer* pHost):
m_pHostPlayer(pHost),
m_iNPCSevFlag(COfflineShopCtrl::NPCSEV_NULL),
m_iCurSellShopPage(0),
m_iCurBuyShopPage(0),
m_iItemState(0),
m_curShopListType(OSTM_ALL),
m_iEmptySlotNumWhenSnap(0)
{
	m_pGameSession = g_pGame->GetGameSession();
	m_pGameUIMan = g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan();
}
COfflineShopCtrl::~COfflineShopCtrl()
{

}
bool COfflineShopCtrl::IsTailShopPage(bool bSell) const 
{ 
	if (bSell)
		return m_sellShopArray.size()== 0 ? true: (m_iCurSellShopPage == ((int)m_sellShopArray.size() -1)/SHOP_COUNT_PERPAGE);
	else
		return m_buyShopArray.size()== 0 ? true: (m_iCurBuyShopPage == ((int)m_buyShopArray.size() -1)/SHOP_COUNT_PERPAGE);
}
bool COfflineShopCtrl::IsHeadShopPage(bool bSell) const 
{
	if(bSell)
		return m_iCurSellShopPage == 0;
	else
		return m_iCurBuyShopPage == 0;
}
void COfflineShopCtrl::NextShopPage(bool bSell) 
{
	if(bSell)
	{
		if(!IsTailShopPage(true)) 
			m_iCurSellShopPage++;
	}
	else
	{
		if(!IsTailShopPage(false)) 
			m_iCurBuyShopPage++;
	}
}

void COfflineShopCtrl::PreShopPage(bool bSell) 
{
	if(bSell)
	{
		if(m_iCurSellShopPage>0) 
			m_iCurSellShopPage--;
	}
	else
	{
		if(m_iCurBuyShopPage>0) 
			m_iCurBuyShopPage--;
	}
}

int COfflineShopCtrl::GetShopID(bool bSell,int idx)
{
	if(bSell)
	{
		int cur = m_iCurSellShopPage * SHOP_COUNT_PERPAGE + idx;
		if(cur>=0 && cur<(int)m_sellShopArray.size())
			return m_ShopArray[m_sellShopArray[cur]].roleid;
	}
	else
	{
		int cur = m_iCurBuyShopPage * SHOP_COUNT_PERPAGE + idx;
		if(cur>=0 && cur<(int)m_buyShopArray.size())
			return m_ShopArray[m_buyShopArray[cur]].roleid;
	}
	
	return 0;
}
CECIvtrItem* COfflineShopCtrl::GetELShopItem(bool bSell, int idx)
{
	if (bSell)	
		return m_curOther.sellInv.GetItem(idx);	
	else
		return m_curOther.buyInv.GetItem(idx);
}
CECIvtrItem* COfflineShopCtrl::GetMyShopItem(bool bSell, int idx)
{
	if (bSell)	
		return m_Self.sellInv.GetItem(idx);	
	else
		return m_Self.buyInv.GetItem(idx);
}
CECIvtrItem* COfflineShopCtrl::GetMyStoreItem(int idx)
{
	return m_Self.shopStore.GetItem(idx);
}

COfflineShopCtrl::ItemInfo* COfflineShopCtrl::GetQueryResultItem(bool bSell,int idx)
{
	if(idx<SHOP_COUNT_PERPAGE && idx >=0 && bSell == m_QueryItemResult.bSellFlag)
		return &m_QueryItemResult.itemResult[idx];
	
	return NULL;
}
bool COfflineShopCtrl::CanDo(int roleid)
{
	return (m_pHostPlayer->GetCharacterID() != roleid) && (GetNPCSevFlag() == NPCSEV_SELLBUY);
}
void COfflineShopCtrl::UpdateShopInv(CECInventory& inv, GNET::PShopItemVector& vec)
{
	int i=0;
	int lsize = vec.size();						
	for (i=0;i<lsize;i++)
	{
		CECIvtrItem* pItem = NULL;
		const GNET::GRoleInventory& item = vec[i].item;
		pItem = CECIvtrItem::CreateItem(item.id,0,item.count);
		pItem->SetUnitPrice(vec[i].price);
		pItem->SetProcType(item.proctype);
		pItem->SetItemInfo((unsigned char*)item.data.begin(),item.data.size());
		pItem->SetPriceScale(CECIvtrItem::SCALE_OFFLINESHOP, 1.0f);
		inv.SetItem(item.pos,pItem);
	}
}
void COfflineShopCtrl::UpdateShopStore(CECInventory& store, GNET::GRoleInventoryVector& vec)
{
	int i=0;
	int lsize = vec.size();
	for (i=0;i<lsize;i++)
	{
		CECIvtrItem* pItem = NULL;
		const GNET::GRoleInventory& item = vec[i];
		pItem = CECIvtrItem::CreateItem(item.id,0,item.count);							
		pItem->SetProcType(item.proctype);
		pItem->SetItemInfo((unsigned char*)item.data.begin(),item.data.size());
		store.SetItem(item.pos,pItem);
		pItem->SetPriceScale(CECIvtrItem::SCALE_OFFLINESHOP, 1.0f);
	}
}
void COfflineShopCtrl::OnServerNotify(int iProtocol, GNET::Protocol* pProtocol)
{
	int iErrCode = 0;
	switch(iProtocol)
	{
	case PROTOCOL_PSHOPCREATE_RE:
		{
			PShopCreate_Re*p = dynamic_cast<PShopCreate_Re*>(pProtocol);
			iErrCode = p->retcode;
			if(p->retcode == ERR_SUCCESS)
			{
				CDlgOfflineShopCreate* pDlg = dynamic_cast<CDlgOfflineShopCreate*>(m_pGameUIMan->GetDialog("Win_ShopCreate"));
				if(pDlg) pDlg->OnCommandCANCEL(NULL);
				m_pGameUIMan->MessageBox("",m_pGameUIMan->GetStringFromTable(10518),MB_OK,A3DCOLORRGB(255,255,255));
			}
		}
		break;
	case PROTOCOL_PSHOPBUY_RE:
		{
			PShopBuy_Re* p = dynamic_cast<PShopBuy_Re*>(pProtocol);
			iErrCode = p->retcode;
			if(p->retcode == ERR_SUCCESS)
			{
				CECIvtrItem* pItem = CECIvtrItem::CreateItem(p->item_id,0,p->item_count);
				if (pItem)
				{
					pItem->SetUnitPrice(p->item_price);
					m_Self.buyInv.SetItem(p->item_pos,pItem);
					pItem->SetPriceScale(CECIvtrItem::SCALE_OFFLINESHOP, 1.0f);
				}
			}
		}
		break;
	case PROTOCOL_PSHOPSELL_RE:
		{
			PShopSell_Re* p = dynamic_cast<PShopSell_Re*>(pProtocol);
			iErrCode = p->retcode;
			
			if(p->retcode == ERR_SUCCESS)
			{
				const GNET::GRoleInventory& sitem = p->itemsell.item;
				
				CECIvtrItem* pItem = CECIvtrItem::CreateItem(sitem.id,sitem.expire_date,sitem.count);
				if (pItem)
				{
					pItem->SetUnitPrice(p->itemsell.price);
					pItem->SetProcType(sitem.proctype);
					pItem->SetItemInfo((unsigned char*)sitem.data.begin(),sitem.data.size());
					pItem->SetPriceScale(CECIvtrItem::SCALE_OFFLINESHOP, 1.0f);
					m_Self.sellInv.SetItem(sitem.pos,pItem);					
				}
			}
			else
			{
				CECIvtrItem* pItem = m_pHostPlayer->GetPack()->GetItem(p->inv_pos);
				if(pItem && pItem->GetTemplateID() == p->item_id)
					pItem->Freeze(false);
				else
				{
					ASSERT(FALSE);
					a_LogOutput(1,"PROTOCOL_PSHOPSELL_RE error: inv_pos %d,tid %d", p->inv_pos,p->item_id);
				}
			}
		}
		break;
	case PROTOCOL_PSHOPCANCELGOODS_RE:
		{
			PShopCancelGoods_Re* p = dynamic_cast<PShopCancelGoods_Re*>(pProtocol);
			iErrCode = p->retcode;
			
			if (p->retcode == ERR_SUCCESS)
			{
				bool bSell = p->canceltype == 0;
				if (bSell)
				{
					m_Self.sellInv.SetItem(p->pos,NULL);
					
					CECIvtrItem* pItem = CECIvtrItem::CreateItem(p->itemstore.id,p->itemstore.expire_date,p->itemstore.count);
					CECIvtrItem* pStoreItem = NULL;
					if (pItem)
					{
						pItem->SetProcType(p->itemstore.proctype);
						pItem->SetItemInfo((unsigned char*)p->itemstore.data.begin(),p->itemstore.data.size());
						pStoreItem = m_Self.shopStore.PutItem(p->itemstore.pos,pItem);
						pItem->SetPriceScale(CECIvtrItem::SCALE_OFFLINESHOP, 1.0f);
					}
					
					if (pStoreItem)
					{
					//	ASSERT(FALSE);
					//	a_LogOutput(1,"PROTOCOL_PSHOPCANCELGOODS_RE, the item should not be at %d", p->itemstore.pos);
						delete pStoreItem;
					}
				}
				else
				{
					m_Self.buyInv.SetItem(p->pos,NULL);
				}
			}
		}
		break;
	case PROTOCOL_PSHOPPLAYERBUY_RE:
		{
			PShopPlayerBuy_Re* p = dynamic_cast<PShopPlayerBuy_Re*>(pProtocol);
			iErrCode = p->retcode;
			if (p->retcode == ERR_SUCCESS)
			{
				// 购买成功
				const GNET::GRoleInventory& item =  p->itemchange.item;
				if (IsInQueryItemState())
				{
					if (!m_QueryItemResult.bSellFlag)
					{
						ASSERT(FALSE);
						a_LogOutput(1,"PROTOCOL_PSHOPPLAYERBUY_RE, search type is error. role %d, tid %d, pos %d", p->master,item.id,item.pos);
					}
					m_QueryItemResult.UpdateItem(p->master,item.id,item.pos,item.count);
				}
				else
				{
					ASSERT(p->master == m_curOther.roleId);
					if (p->master == m_curOther.roleId)
					{
						CECIvtrItem* pItem = m_curOther.sellInv.GetItem(item.pos);
						if (pItem)
						{
							if (item.count==0)							
								m_curOther.sellInv.SetItem(item.pos,NULL);
							else
								pItem->SetCount(item.count);							
						}
						else
						{
							ASSERT(FALSE);
							a_LogOutput(1,"PROTOCOL_PSHOPPLAYERBUY_RE, Buy error. role %d, tid %d, pos %d, local count %d, sev count %d", p->master,item.id,item.pos, pItem->GetCount(),item.count);
						}						
					}					
				}				
			}
		}
		break;
	case PROTOCOL_PSHOPPLAYERSELL_RE:
		{
			PShopPlayerSell_Re* p = dynamic_cast<PShopPlayerSell_Re*>(pProtocol);
			iErrCode = p->retcode;
			if (p->retcode == ERR_SUCCESS)
			{
				// 出售成功
				const GNET::GRoleInventory& item =  p->itembuy.item;
				
				if (IsInQueryItemState())
				{
					if (m_QueryItemResult.bSellFlag)
					{
						ASSERT(FALSE);
						a_LogOutput(1,"PROTOCOL_PSHOPPLAYERSELL_RE, search type is error. role %d, tid %d, pos %d", p->master,item.id,item.pos);
					}
					m_QueryItemResult.UpdateItem(p->master,item.id,item.pos,item.count);
				}
				else
				{
					ASSERT(p->master == m_curOther.roleId);
					if (p->master == m_curOther.roleId)
					{
						CECIvtrItem* pItem = m_curOther.buyInv.GetItem(item.pos);
						if (pItem)
						{
							if (item.count==0)							
								m_curOther.buyInv.SetItem(item.pos,NULL);
							else
								pItem->SetCount(item.count);							
						}
						else
						{
							ASSERT(FALSE);
							a_LogOutput(1,"PROTOCOL_PSHOPPLAYERSELL_RE, Sell error. role %d, tid %d, pos %d, local count %d, sev count %d", p->master,item.id,item.pos, pItem->GetCount(),item.count);
						}						
					}	
				}
				
			}
		}
		break;
	case PROTOCOL_PSHOPSETTYPE_RE:
		{
			PShopSetType_Re* p = dynamic_cast<PShopSetType_Re*>(pProtocol);
			iErrCode = p->retcode;
			if (p->retcode == ERR_SUCCESS)
			{
				m_Self.shopType = p->newtype;
				CDlgOfflineShopCreate* pDlg = dynamic_cast<CDlgOfflineShopCreate*>(m_pGameUIMan->GetDialog("Win_ShopCreate"));
				if(pDlg) pDlg->OnCommandCANCEL(NULL);
				m_pGameUIMan->MessageBox("",m_pGameUIMan->GetStringFromTable(10519),MB_OK,A3DCOLORRGB(255,255,255));
			}
		}
		break;
	case PROTOCOL_PSHOPACTIVE_RE:
		{
			PShopActive_Re* p = dynamic_cast<PShopActive_Re*>(pProtocol);
			iErrCode = p->retcode;
			if (p->retcode == ERR_SUCCESS)
			{
				m_Self.expireTime = p->expiretime;
				m_Self.shopStatus = p->status;
				CDlgOfflineShopCreate* pDlg = dynamic_cast<CDlgOfflineShopCreate*>(m_pGameUIMan->GetDialog("Win_ShopCreate"));
				if(pDlg) pDlg->OnCommandCANCEL(NULL);
				m_pGameUIMan->MessageBox("",m_pGameUIMan->GetStringFromTable(10520),MB_OK,A3DCOLORRGB(255,255,255));
			}
		}
		break;
	case PROTOCOL_PSHOPMANAGEFUND_RE:
		{
			PShopManageFund_Re* p = dynamic_cast<PShopManageFund_Re*>(pProtocol);
			iErrCode = p->retcode;
			if (p->retcode == ERR_SUCCESS)
			{
				m_Self.money = p->money;
				m_Self.yinpiao = p->yinpiao;
			}
		}
		break;
	case PROTOCOL_PSHOPDRAWITEM_RE:
		{
			PShopDrawItem_Re* p = dynamic_cast<PShopDrawItem_Re*>(pProtocol);
			CDlgOfflineShopStore* pDlg = dynamic_cast<CDlgOfflineShopStore*>(m_pGameUIMan->GetDialog("Win_ShopBag"));
			iErrCode = p->retcode;
			if (p->retcode == ERR_SUCCESS)
			{
				m_Self.shopStore.SetItem(p->item_pos,NULL);
				
				RemoveItemFromStoreSnapShot(p->item_pos);
				if(IsEmptyStoreSnapShot() || m_iEmptySlotNumWhenSnap<=0)
					pDlg->SetAllItemsFlag(false);

				if(pDlg->IsGetAllItems())
					TakeItemFromStoreToPack();
			}
			else 
			{
				pDlg->SetAllItemsFlag(false);
			}
		}
		break;
	case PROTOCOL_PSHOPCLEARGOODS_RE:
		{
			PShopClearGoods_Re* p = dynamic_cast<PShopClearGoods_Re*>(pProtocol);
			iErrCode = p->retcode;
			if (p->retcode == ERR_SUCCESS)
			{
				m_Self.buyInv.RemoveAllItems();
				m_Self.sellInv.RemoveAllItems();
				UpdateShopInv(m_Self.sellInv,p->slist);
				UpdateShopStore(m_Self.shopStore,p->store);
			}
		}
		break;
	case PROTOCOL_PSHOPSELFGET_RE: // 获取自己店铺信息
		{
			PShopSelfGet_Re* p = dynamic_cast<PShopSelfGet_Re*>(pProtocol);			
			
			if (p->retcode == ERR_SUCCESS)
			{
				if (m_iNPCSevFlag == NPCSEV_SETTING) //取到自己店铺信息
				{					
					if(m_pGameSession->GetCharacterID() == p->detail.roleid)
					{
						m_Self.Clear();

						m_Self.shopType = p->detail.shoptype;
						m_Self.shopStatus = p->detail.status;
						m_Self.createtime = p->detail.createtime;
						m_Self.expireTime = p->detail.expiretime;
						m_Self.money = p->detail.money;
						m_Self.yinpiao = 0;
						for (int i=0;i<(int)p->detail.yinpiao.size();i++)
						{
							m_Self.yinpiao += p->detail.yinpiao[i].count;
						}					
						
						UpdateShopInv(m_Self.buyInv,p->detail.blist);						
						UpdateShopInv(m_Self.sellInv,p->detail.slist);						
						UpdateShopStore(m_Self.shopStore,p->detail.store);
						
						m_pGameUIMan->GetDialog("Win_ShopSet")->Show(true);
						PAUIDIALOG pInv = m_pGameUIMan->GetDialog("Win_Inventory");
						if(pInv)
							pInv->Show(true);
					}
					else
					{
						ASSERT(FALSE);
						a_LogOutput(1,"PROTOCOL_PSHOPSELFGET_RE, roleid(%d) is mismatch with host player(%d)",p->detail.roleid,m_pGameSession->GetCharacterID());
						m_pHostPlayer->EndNPCService();
					}
				}
				else
				{
					ASSERT(FALSE);
					iErrCode = p->retcode;
					m_pHostPlayer->EndNPCService();
				}
			}
			else if (p->retcode == ERR_PLAYERSHOP_NOTFIND) // 没有发现店铺
			{				
				CDlgOfflineShopCreate* pDlg = dynamic_cast<CDlgOfflineShopCreate*>(m_pGameUIMan->GetDialog("Win_ShopCreate"));
				pDlg->SetDlgType(true);
				pDlg->Show(true);
				PAUIDIALOG pInv = m_pGameUIMan->GetDialog("Win_Inventory");
				if(pInv)
					pInv->Show(true);				
			}
			else 
			{
				iErrCode = p->retcode;
				m_pHostPlayer->EndNPCService();
			}
		}
		break;
	case PROTOCOL_PSHOPPLAYERGET_RE:
		{
			PShopPlayerGet_Re* p = dynamic_cast<PShopPlayerGet_Re*>(pProtocol);
			iErrCode = p->retcode;
			
			m_curOther.Clear();
			m_curOther.roleId = p->base.roleid;
			m_curOther.shopType = p->base.shoptype;			

			UpdateShopInv(m_curOther.buyInv,p->base.blist);			
			UpdateShopInv(m_curOther.sellInv,p->base.slist);

			SetItemState(ITEMSTATE_RECV_ENTERSHOP);

			CDlgOfflineShopItemsList* pDlgItem = dynamic_cast<CDlgOfflineShopItemsList*>(m_pGameUIMan->GetDialog("Win_ShopItemList"));
			CDlgOfflineShopList* pShopList = dynamic_cast<CDlgOfflineShopList*>(m_pGameUIMan->GetDialog("Win_ShopList"));
			if(pDlgItem && pShopList)
			{
				pDlgItem->SetItemViewer(CDlgOfflineShopItemsList::IT_SHOPITEM,pShopList->IsSellMode());
				if(!pDlgItem->IsShow())
					pDlgItem->ToggleDlg(false);		
				pDlgItem->ClearSearchHint(true);
			}
		}
		break;
	case PROTOCOL_PSHOPLIST_RE:
		{
			PShopList_Re* p = dynamic_cast<PShopList_Re*>(pProtocol);			
			
			m_ShopArray.clear();		

			int i=0;

			for(;i<(int)p->shoplist.size();i++)
			{
				OfShop shop;
				shop.roleid = p->shoplist[i].roleid;
				shop.shopType = p->shoplist[i].shoptype;
				shop.invState = p->shoplist[i].invstate; // 0x01:出售栏非空;0x02:收购栏非空
				shop.bNameOK = false;
				shop.createtime = p->shoplist[i].createtime;
				m_ShopArray.push_back(shop);
			}
			
			std::sort(m_ShopArray.begin(),m_ShopArray.end(),earlier());	
			
			ViewShopListByType(OSTM_ALL);

			CDlgOfflineShopList* pDlgShop = dynamic_cast<CDlgOfflineShopList*>(m_pGameUIMan->GetDialog("Win_ShopList"));
			if (pDlgShop)
			{
				pDlgShop->SetShowType(CDlgOfflineShopList::SHOWTYPE_NPCSEV);
				pDlgShop->Show(true);
				m_pGameUIMan->GetDialog("Win_ShopItemList")->Show(false);
			}			
		}
		break;
	case PROTOCOL_PSHOPLISTITEM_RE: // 翻页请求一次
		{
			PShopListItem_Re* p = dynamic_cast<PShopListItem_Re*>(pProtocol);
			ASSERT(p->listtype == 0 || p->listtype== 1);
			m_QueryItemResult.Clear(false);

			m_QueryItemResult.iPageNum = p->page_num;			
			m_QueryItemResult.bSellFlag = p->listtype == 0;			
			
			for(int i=0;i<(int)p->itemlist.size();i++)
			{
				ItemInfo info;
				const PShopItemEntry& entry = p->itemlist[i];
				const GRoleInventory& shopItem = entry.item.item;
				info.roleid = entry.roleid;
				info.pos = shopItem.pos;
				info.pItem = CECIvtrItem::CreateItem(shopItem.id,0,shopItem.count);
				if(info.pItem)
				{
					info.pItem->SetUnitPrice(entry.item.price);
					info.pItem->SetProcType(shopItem.proctype);
					info.pItem->SetItemInfo((unsigned char*)shopItem.data.begin(),shopItem.data.size());
					info.pItem->SetPriceScale(CECIvtrItem::SCALE_OFFLINESHOP, 1.0f);
				}
				m_QueryItemResult.itemResult[i] = info;
			}		
			
			SetItemState(GetItemState() | 0x0010);

			CDlgOfflineShopItemsList* pDlgItem = dynamic_cast<CDlgOfflineShopItemsList*>(m_pGameUIMan->GetDialog("Win_ShopItemList"));
			if (pDlgItem)
			{			
				pDlgItem->SetItemViewer(CDlgOfflineShopItemsList::IT_QUERYITEM,m_QueryItemResult.bSellFlag);
				if(!pDlgItem->IsShow()) // 从店铺列表搜索执行过来的
				{					
					pDlgItem->ToggleDlg(false);
				}
				pDlgItem->ClearSearchHint();
			}			
		}
		break;
	default:
		ASSERT(false);
		a_LogOutput(1,"COfflineShopCtrl::OnServerNotify, error protocol %d",iProtocol);
		m_pHostPlayer->EndNPCService();
		break;
	}
	
	if(iErrCode!=0)
	{
		ACString strError;
		m_pGameSession->GetServerError(iErrCode, strError);
		g_pGame->GetGameRun()->AddChatMessage(strError, GP_CHAT_MISC);
	}
}
void COfflineShopCtrl::ViewShopListByType(int mask)
{
	m_curShopListType = mask;
	m_sellShopArray.clear();
	m_buyShopArray.clear();
	m_iCurBuyShopPage = 0;
	m_iCurSellShopPage = 0;
	int i;
	for (i=0;i<(int)m_ShopArray.size();i++)
	{
		const OfShop& shop = m_ShopArray[i];
		
		if(((1<<shop.shopType) & m_curShopListType) == 0) continue;

		if ((shop.invState & 0x01) != 0)
		{
			m_sellShopArray.push_back(i);
		}
		if ((shop.invState & 0x02) != 0)
		{
			m_buyShopArray.push_back(i);
		}
	}
}
int COfflineShopCtrl::FirstEmptyPosInMyShop(bool bSell)
{
	if(bSell)
		return m_Self.sellInv.SearchEmpty();
	else
		return m_Self.buyInv.SearchEmpty();
}
bool COfflineShopCtrl::IsELShopInvEmpty(bool bSell) 
{
	if (bSell)
		return m_curOther.sellInv.GetEmptySlotNum() == m_curOther.sellInv.GetSize();
	else
		return m_curOther.buyInv.GetEmptySlotNum() == m_curOther.buyInv.GetSize();
}
//////////////////////////////////////////////////

void COfflineShopCtrl::QueryItemResult::Clear(bool bClearItemID)
{
	iPageNum = 0;
	bSellFlag = true;
	if(bClearItemID)
	{
		iItemTid = 0;
	}
	for (int i=0;i<SHOP_COUNT_PERPAGE;i++)
	{
		if (itemResult[i].pItem)
			delete itemResult[i].pItem;
		
		itemResult[i].roleid = 0;
		itemResult[i].pos = -1;
		itemResult[i].pItem = NULL;
	}
}
int COfflineShopCtrl::QueryItemResult::GetItemCount()
{
	int ret = 0;
	for (int i=0;i<SHOP_COUNT_PERPAGE;i++)
	{
		if (itemResult[i].roleid)
			ret++;
	}
	
	return ret;
}
void COfflineShopCtrl::QueryItemResult::UpdateItem(int role,int item_id,int item_pos,int item_count)
{
	if(role == 0 || item_id == 0) return;
	ASSERT(item_pos >=0 && item_count>=0);
	
	for (int i=0;i<SHOP_COUNT_PERPAGE;i++)
	{
		int tid = itemResult[i].pItem == NULL ? 0:itemResult[i].pItem->GetTemplateID();
		if (itemResult[i].roleid == role && item_id == tid && item_pos == itemResult[i].pos)
		{
			CECIvtrItem* pItem = itemResult[i].pItem;
			
			if (pItem)
			{
				if (item_count == 0)
				{
					itemResult[i].roleid = 0;
					itemResult[i].pos = -1;
					delete itemResult[i].pItem;
					itemResult[i].pItem = NULL;
				}
				else
				{
					pItem->SetCount(item_count);
				}
			}	
			
			return ;
		}
	}
}
__int64 COfflineShopCtrl::ShopSelf::GetTotalPrice(bool bSell)
{
	CECInventory& inv = bSell ? sellInv:buyInv;
	__int64 ret = 0;
	for (int i=0;i<inv.GetSize();i++)
	{
		CECIvtrItem* pItem = inv.GetItem(i);
		if (pItem)
		{
			ret += (unsigned int)pItem->GetUnitPrice() * (__int64)pItem->GetCount();
		}
	}

	return ret;
}
bool COfflineShopCtrl::CheckMoneyInv(bool bSell, __int64 price,int count)
{
	if(price > MAX_UNIT_PRICE) return false;

	__int64 allM = price * count + m_Self.GetTotalPrice(bSell);

	if (bSell)
	{
		allM  += (__int64)m_Self.money + (__int64)m_Self.yinpiao * YINPIAO;
		return allM <= MAX_MONEY_INSHOP;
	}
	else
		return allM <= (__int64)m_Self.money + (__int64)m_Self.yinpiao * YINPIAO;
}
bool COfflineShopCtrl::CanTakeMoneyFromStore(int yinpiao,int money)
{
	if(m_Self.money < money || m_Self.yinpiao < yinpiao) return false;

	__int64 leftMoney = (__int64)m_Self.money - (__int64)money + ((__int64)m_Self.yinpiao - (__int64)yinpiao) * YINPIAO;

	return (leftMoney >= m_Self.GetTotalPrice(false));
}
bool COfflineShopCtrl::CheckMoneyStore(int yinpiao, int money) // 存钱检查
{
	__int64 allM  = (__int64)money + m_Self.money + ((__int64)yinpiao + m_Self.yinpiao) * YINPIAO + m_Self.GetTotalPrice(true);
	return allM <= MAX_MONEY_INSHOP;	
}
bool COfflineShopCtrl::CanSellItemByPrice(__int64 price)
{
	if(price < 0) return false;

	__int64 yinpiao = price / YINPIAO;

	__int64 money = price - yinpiao * YINPIAO;

	return CanPutMoneyToPack((int)yinpiao,(int)money);
}
bool COfflineShopCtrl::CanBuyItemByPrice(__int64 price,__int64& yinpiao,__int64& money)
{
	if(price < 0) return false;
		
	__int64 needY = price / YINPIAO;
	needY = a_Min((__int64)m_pHostPlayer->GetYinpiaoTotalAmount(),needY);
	price -= needY * YINPIAO;

	yinpiao = needY;
	money = price;
	return m_pHostPlayer->GetMoneyAmount() >= price;
}
bool COfflineShopCtrl::CanPutMoneyToPack(int yinpiao, int money)
{
	if(money < 0 || yinpiao < 0 ) return false;

	if( money + m_pHostPlayer->GetMoneyAmount() > MAX_MONEY_INVENTORY) return false;
	
	CECIvtrItem* pItem = CECIvtrItem::CreateItem(21652,0,1);
	if (pItem)
	{
		int count_pile = (int)ceil((float)yinpiao / pItem->GetPileLimit());

		delete pItem;
		
		return m_pHostPlayer->GetPack()->GetEmptySlotNum() >= count_pile;
	}
	
	return false;
}
int COfflineShopCtrl::TakeItemFromStoreToPack()
{	
/*	CECIvtrItem *pItem = NULL;
	int i = 0;
	for( i = 0; i < SHOP_COUNT_STORE; i++ )
	{
		pItem = GetMyStoreItem(i);
		if(pItem)
			break;
	}
	if(i<SHOP_COUNT_STORE)
		m_pGameSession->OffShop_TakeItemFromStore(i);
	*/
	if (m_pHostPlayer->GetPack()->GetEmptySlotNum()<1)
	{
		m_pGameUIMan->MessageBox("",m_pGameUIMan->GetStringFromTable(704),MB_OK,A3DCOLORRGB(255,255,255));
		return -1;
	}
	std::set<int>::iterator itr = m_prepareGetAllItems.begin();
	int idx = *itr;
	if (idx>=0 && idx < SHOP_COUNT_STORE && GetMyStoreItem(idx) && m_iEmptySlotNumWhenSnap>0)
	{
		m_pGameSession->OffShop_TakeItemFromStore(idx);
		m_iEmptySlotNumWhenSnap--;
	}

	return idx;
}
bool COfflineShopCtrl::IsShopFull()
{
	return (m_Self.buyInv.GetEmptySlotNum() + m_Self.sellInv.GetEmptySlotNum() + m_Self.shopStore.GetEmptySlotNum()) <= SHOP_COUNT_STORE;
}
void COfflineShopCtrl::StoreSnapShot()
{
	m_prepareGetAllItems.clear();

	int i = 0;
	CECIvtrItem *pItem = NULL;
	for( i = 0; i < SHOP_COUNT_STORE; i++ )
	{
		pItem = GetMyStoreItem(i);
		if(pItem)
			m_prepareGetAllItems.insert(i);
	}

	m_iEmptySlotNumWhenSnap = m_pHostPlayer->GetPack()->GetEmptySlotNum();
}
void COfflineShopCtrl::RemoveItemFromStoreSnapShot(int idx)
{
	if(m_prepareGetAllItems.find(idx)!=m_prepareGetAllItems.end())
		m_prepareGetAllItems.erase(idx);
}
bool COfflineShopCtrl::IsEmptyStoreSnapShot()
{
	return m_prepareGetAllItems.empty();
}