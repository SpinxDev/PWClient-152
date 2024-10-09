/*
* FILE: DlgOfflineShopItemsList.cpp
*
* DESCRIPTION: 
*
* CREATED BY: WYD
*
* HISTORY: 
*
* Copyright (c) 2013, All Rights Reserved.
*/

#include "Dlgbase.h"
#include "DlgOfflineShopItemsList.h"
#include <AUIDef.h>
#include "DlgInputNO.h"
#include "EC_GameUIMan.h"
#include "EC_HostPlayer.h"
#include "EC_Global.h"
#include "EC_Game.h"
#include "EC_GameSession.h"
#include "DlgOfflineShopList.h"
#include "EC_TimeSafeChecker.h"
#include "EC_UIHelper.h"

#include <AUICTranslate.h>
#include <AFI.h>

#define  LIST_TICK 3000

AUI_BEGIN_COMMAND_MAP(CDlgOfflineShopItemsList, CDlgBase)
AUI_ON_COMMAND("trade*", OnCommand_BuyOrSell)
AUI_ON_COMMAND("Btn_Search", OnCommand_SearchItem)
AUI_ON_COMMAND("Btn_PageUp", OnCommand_PageUp)
AUI_ON_COMMAND("Btn_PageDown", OnCommand_PageDown)
AUI_ON_COMMAND("Rdo_Sell", OnCommand_SellList)
AUI_ON_COMMAND("Rdo_Buy", OnCommand_BuyList)
AUI_ON_COMMAND("Btn_Back", OnCommand_BackToShop)

AUI_ON_COMMAND("IDCANCEL",		OnCommandCANCEL)
AUI_ON_COMMAND("Btn_Close",		OnCommandCANCEL)

AUI_END_COMMAND_MAP()

AUI_BEGIN_EVENT_MAP(CDlgOfflineShopItemsList, CDlgBase)
AUI_ON_EVENT("*",		WM_MOUSEWHEEL,			OnEventMouseWheel)
AUI_ON_EVENT(NULL,		WM_MOUSEWHEEL,			OnEventMouseWheel)
AUI_ON_EVENT("Edt_Search",	WM_KEYDOWN,			OnEventKeyDown_Txt_Search)
AUI_ON_EVENT("Lst_Search",	WM_LBUTTONDBLCLK,	OnEventDBClk_Lst_Search)
AUI_END_EVENT_MAP()

CDlgOfflineShopItemsList::CDlgOfflineShopItemsList()
{
	memset(m_pLbl_ItemName,0,sizeof(m_pLbl_ItemName));
	memset(m_pBtn_Buy,0,sizeof(m_pBtn_Buy));
	memset(m_pLbl_Money,0,sizeof(m_pLbl_Money));	
	memset(m_pImg_Item,0,sizeof(m_pImg_Item));	
	m_pBtn_PageUp = NULL;
	m_pBtn_PageDown = NULL;
	m_pBtn_BackToShop = NULL;
	m_pLbl_Title = NULL;
	m_iCurViewer = -1;
	m_pItemViewer[0] = NULL;
	m_pItemViewer[1] = NULL;
}

CDlgOfflineShopItemsList::~CDlgOfflineShopItemsList()
{
	if(m_pItemViewer[0]) 
	{
		delete m_pItemViewer[0];
		 m_pItemViewer[0] = NULL;
	}
	if(m_pItemViewer[1]) 
	{
		delete m_pItemViewer[1];
		 m_pItemViewer[1] = NULL;
	}
}

bool CDlgOfflineShopItemsList::OnInitDialog()
{
	DDX_Control("Lst_Search",m_pLst_Search);
	DDX_Control("Edt_Search",m_pTxt_Search);
	
	DDX_Control("Btn_PageUp",m_pBtn_PageUp);
	DDX_Control("Btn_PageDown",m_pBtn_PageDown);

	DDX_Control("Btn_Back", m_pBtn_BackToShop);

	DDX_Control("Lbl_Title",m_pLbl_Title);

	for(int i=0;i<SHOP_COUNT_PERPAGE;i++)
	{
		AString str;
		
		str.Format("Lbl_Name%d",i+1);		
		DDX_Control(str,m_pLbl_ItemName[i]);
		
		str.Format("Btn_%d",i+1);
		DDX_Control(str,m_pBtn_Buy[i]);
		
		str.Format("Lbl_Money%d",i+1);
		DDX_Control(str,m_pLbl_Money[i]);
		
		str.Format("Img_%d",i+1);
		DDX_Control(str,m_pImg_Item[i]);
	}
	DDX_Control("Lbl_Search",m_pLbl_SearchHint);

	m_iCurViewer = -1;

	if(m_pItemViewer[0]) delete m_pItemViewer[0];
	if(m_pItemViewer[1]) delete m_pItemViewer[1];

	m_pItemViewer[0] = CUIActionDelegate::CreateDelegate(true,this);
	m_pItemViewer[1] = CUIActionDelegate::CreateDelegate(false,this);

	return CDlgShopSearchBase::OnInitDialog();
}
void CDlgOfflineShopItemsList::OnShowDialog()
{
	COfflineShopCtrl* pCtlr = GetHostPlayer()->GetOfflineShopCtrl();
	if(!pCtlr) return;

	ClearControls(true);

//	m_pItemViewer[m_iCurViewer]->OnInstallViewer(GetHostPlayer(),GetGameUIMan(),g_pGame->GetGameSession());
	ASSERT(m_iCurViewer>=0);

	m_pLbl_SearchHint->SetText(_AL(""));
}

void CDlgOfflineShopItemsList::OnHideDialog()
{
}
void CDlgOfflineShopItemsList::OnCommandCANCEL(const char *szCommand)
{	
	GetHostPlayer()->GetOfflineShopCtrl()->SetNPCSevFlag(COfflineShopCtrl::NPCSEV_NULL);	
	Show(false);
}
void CDlgOfflineShopItemsList::ClearControls(bool bSell)
{
	for(int i=0;i<SHOP_COUNT_PERPAGE;i++)
	{	
		m_pLbl_ItemName[i]->SetText(_AL(""));		
		m_pBtn_Buy[i]->Enable(false);
		m_pBtn_Buy[i]->SetData(-1);	
		m_pBtn_Buy[i]->SetText(GetGameUIMan()->GetStringFromTable(bSell ? 10500:10501));
		m_pBtn_Buy[i]->SetDataPtr(NULL,"ptr_CECIvtrItem");
		m_pBtn_Buy[i]->Show(false);
			
		m_pLbl_Money[i]->SetText(_AL(""));
		
		m_pImg_Item[i]->SetData(0); //
		m_pImg_Item[i]->ClearCover();
		m_pImg_Item[i]->SetHint(_AL(""));		
		m_pImg_Item[i]->SetText(_AL(""));		
	}
}
void CDlgOfflineShopItemsList::UpdateControls()
{
	if (m_iCurViewer>=0 && m_pItemViewer[m_iCurViewer])
	{
		m_pItemViewer[m_iCurViewer]->UpdateItems();
	}
}

void CDlgOfflineShopItemsList::OnTick()
{
	CDlgShopSearchBase::OnTick();
	
	if (m_iCurViewer>=0 && m_pItemViewer[m_iCurViewer])
	{
		m_pItemViewer[m_iCurViewer]->OnTick();
	}

	UpdateControls();

	GetDlgItem("Lbl_Name1")->Show(!m_pLst_Search->IsShow());
	GetDlgItem("Lbl_Name3")->Show(!m_pLst_Search->IsShow());
	GetDlgItem("Lbl_Money1")->Show(!m_pLst_Search->IsShow());
	GetDlgItem("Lbl_Money3")->Show(!m_pLst_Search->IsShow());

	GetDlgItem("Rdo_Sell")->Show(!m_pLst_Search->IsShow());
	GetDlgItem("Rdo_Buy")->Show(!m_pLst_Search->IsShow());

	GetDlgItem("Img_1")->Show(!m_pLst_Search->IsShow());
	GetDlgItem("Img_3")->Show(!m_pLst_Search->IsShow());

	GetDlgItem("Img_Icon1")->Show(!m_pLst_Search->IsShow());
	GetDlgItem("Img_Icon3")->Show(!m_pLst_Search->IsShow());
}
void CDlgOfflineShopItemsList::OnCommand_PageUp(const char *szCommand)
{
	if (m_iCurViewer>=0 && m_pItemViewer[m_iCurViewer])
	{
		m_pItemViewer[m_iCurViewer]->PageUp();
	}
}
void CDlgOfflineShopItemsList::OnCommand_PageDown(const char *szCommand)
{
	if (m_iCurViewer>=0 && m_pItemViewer[m_iCurViewer])
	{
		m_pItemViewer[m_iCurViewer]->PageDown();
	}
}
void CDlgOfflineShopItemsList::OnCommand_SellList(const char* szCommand)
{
	if(m_iCurViewer>=0 && m_pItemViewer[m_iCurViewer])
		m_pItemViewer[m_iCurViewer]->PushSellBtn();
}
void CDlgOfflineShopItemsList::OnCommand_BuyList(const char* szCommand)
{
	if(m_iCurViewer>=0 && m_pItemViewer[m_iCurViewer])
		m_pItemViewer[m_iCurViewer]->PushBuyBtn();
}

void CDlgOfflineShopItemsList::OnCommand_BuyOrSell(const char* szCommand)
{
	if(m_iCurViewer<0 || !m_pItemViewer[m_iCurViewer]) return;
	COfflineShopCtrl* pCtrl = GetHostPlayer()->GetOfflineShopCtrl();
	if(!pCtrl) return;

	int idx = atoi(szCommand + strlen("trade")) - 1;
	PAUIOBJECT pBtn = m_pBtn_Buy[idx];
	
	CDlgInputNO *pShow = GetGameUIMan()->m_pDlgInputNO;
	
	
	ACHAR szText[40];
	PAUIOBJECT pEdit = pShow->GetDlgItem("DEFAULT_Txt_No.");


	CECIvtrItem* pItem = NULL;
	if (!pCtrl->IsInQueryItemState()) // shop item
	{
		pItem = pCtrl->GetELShopItem(m_pItemViewer[m_iCurViewer]->IsSellModel(),idx);
		ASSERT(pItem == pBtn->GetDataPtr("ptr_CECIvtrItem"));

		if (pItem)
		{
			bool bSell = m_pItemViewer[m_iCurViewer]->IsSellModel();
			if (!bSell)
			{
				if(GetHostPlayer()->GetPack()->FindItem(pItem->GetTemplateID())==-1)
				{
					GetGameUIMan()->MessageBox("",GetGameUIMan()->GetStringFromTable(10525),MB_OK,A3DCOLORRGB(255,255,255));
					return;
				}
			}
			pEdit->SetData(pItem->GetCount());
			a_sprintf(szText, _AL("%d"), min(1, pItem->GetCount()));
			pEdit->SetText(szText);
			
			pShow->SetType(bSell ? CDlgInputNO::INPUTNO_OFFLINESHOP_SHOPITEM_SELL:CDlgInputNO::INPUTNO_OFFLINESHOP_SHOPITEM_BUY);
			pShow->SetData(idx);
			pShow->SetDataPtr(pBtn->GetDataPtr("ptr_CECIvtrItem"),"ptr_CECIvtrItem");
		}
	}
	else // search item
	{
		COfflineShopCtrl::ItemInfo* pInfo = pCtrl->GetQueryResultItem(m_pItemViewer[m_iCurViewer]->IsSellModel(),idx);
		ASSERT(pInfo && pInfo->pItem == pBtn->GetDataPtr("ptr_CECIvtrItem"));
		pItem = pInfo ? pInfo->pItem:NULL;

		if (pItem)
		{
			bool bSell = m_pItemViewer[m_iCurViewer]->IsSellModel();
			if (!bSell)
			{
				if(GetHostPlayer()->GetPack()->FindItem(pItem->GetTemplateID())==-1)
				{
					GetGameUIMan()->MessageBox("",GetGameUIMan()->GetStringFromTable(10525),MB_OK,A3DCOLORRGB(255,255,255));
					return;
				}
			}
			pEdit->SetData(pItem->GetCount());
			a_sprintf(szText, _AL("%d"), min(1, pItem->GetCount()));
			pEdit->SetText(szText);
			
			pShow->SetType(bSell ? CDlgInputNO::INPUTNO_OFFLINESHOP_SEARCHITEM_SELL:CDlgInputNO::INPUTNO_OFFLINESHOP_SEARCHITEM_BUY);
			pShow->SetData(idx);
			pShow->SetDataPtr(pBtn->GetDataPtr("ptr_CECIvtrItem"),"ptr_CECIvtrItem");
		}
	}

	pShow->Show(true, true);
}

void CDlgOfflineShopItemsList::OnCommand_BackToShop(const char* szCommand)
{
	if (m_iCurViewer == IT_SHOPITEM)
	{
		GetGameUIMan()->m_pDlgOffShopList->SetShowType(CDlgOfflineShopList::SHOWTYPE_SHOPITEMBACK);
	}
	else if (m_iCurViewer == IT_QUERYITEM)
	{
		GetGameUIMan()->m_pDlgOffShopList->SetShowType(CDlgOfflineShopList::SHOWTYPE_QUERYITEMBACK);
	}
	else
		GetGameUIMan()->m_pDlgOffShopList->SetShowType(CDlgOfflineShopList::SHOWTYPE_NPCSEV);
	
	ToggleDlg(true);
}
void CDlgOfflineShopItemsList::SetItemViewer(int idx,bool bSell) 
{
	m_iCurViewer = idx;

/*	bool bSell = IsSellMode();

	COfflineShopCtrl* pCtrl = GetHostPlayer()->GetOfflineShopCtrl();
	bool fromEnterShop = (pCtrl->GetItemState() & COfflineShopCtrl::ITEMSTATE_SEND_ENTERSHOP) !=0;
	bool fromQueryShop = (pCtrl->GetItemState() & COfflineShopCtrl::ITEMSTATE_SEND_QUERYITEM_FROMSHOP) !=0;
	if(pCtrl && (fromEnterShop || fromQueryShop) )
	{
		bSell = GetGameUIMan()->m_pDlgOffShopList->IsSellMode();
	}*/
	
	m_pItemViewer[m_iCurViewer]->OnInstallViewer(GetHostPlayer(),GetGameUIMan(),g_pGame->GetGameSession(),bSell);
}
void CDlgOfflineShopItemsList::ToggleDlg(bool bToShopDlg)
{
	PAUIDIALOG pOld;
	PAUIDIALOG pNew;
	
	if (bToShopDlg)
	{
		pOld = GetGameUIMan()->GetDialog("Win_ShopItemList");// dynamic_cast<PAUIDIALOG>(GetGameUIMan()->m_pDlgOffShopItemList);
		pNew = GetGameUIMan()->GetDialog("Win_ShopList");//GetGameUIMan()->m_pDlgOffShopList;
	}
	else
	{
		pOld = GetGameUIMan()->GetDialog("Win_ShopList");
		pNew = GetGameUIMan()->GetDialog("Win_ShopItemList");
	}
	
	pOld->Show(false);
	pNew->SetPosEx(GetPos().x, GetPos().y);
	pNew->Show(true);
}
bool CDlgOfflineShopItemsList::IsSellMode()
{
	if (m_iCurViewer>=0 && m_pItemViewer[m_iCurViewer])
	{
		return m_pItemViewer[m_iCurViewer]->IsSellModel();
	}

	return true;
}
bool CDlgOfflineShopItemsList::BeforeSearchAction()
{
	COfflineShopCtrl* pCtrl = GetHostPlayer()->GetOfflineShopCtrl();
	if(pCtrl) 
		pCtrl->SetItemState(COfflineShopCtrl::ITEMSTATE_SEND_QUERYITEM_FROMITEM);

	return IsSellMode();
}
//////////////////////////////////////////////////////////////////////////
//
//
//
//////////////////////////////////////////////////////////////////////////
CUIActionDelegate* CUIActionDelegate::CreateDelegate(bool bShopItem,CDlgOfflineShopItemsList* pDlg)
{
	ASSERT(pDlg);
	if (bShopItem) // shop item
	{
		return new CViewShopItem(pDlg);
	}
	else // query item
		return new CViewQueryItem(pDlg);
	
}
CUIActionDelegate::CUIActionDelegate(CDlgOfflineShopItemsList* pDlg):m_pDlg(pDlg),
m_bSellOrBuy(true),
m_pGameSession(NULL),
m_pHostPlayer(NULL),
m_pGameUIMan(NULL)
{
};
void CUIActionDelegate::OnInstallViewer(CECHostPlayer* pHost,CECGameUIMan* pUI,CECGameSession* pSession,bool bSell)
{
	m_pHostPlayer = pHost; 
	m_pGameUIMan = pUI;
	m_pCtrl = pHost ? pHost->GetOfflineShopCtrl():NULL;
	m_pGameSession = pSession;
	m_bSellOrBuy = true;
}

CViewQueryItem::CViewQueryItem(CDlgOfflineShopItemsList* pDlg):CUIActionDelegate(pDlg),
m_dwLastListTime(0),
m_bNeedQuery(false),
m_QueryNum(0)
{	
};
void CViewQueryItem::OnInstallViewer(CECHostPlayer* pHost,CECGameUIMan* pUI,CECGameSession* pSession,bool bSell)
{
	CUIActionDelegate::OnInstallViewer(pHost,pUI,pSession,bSell);

	m_pDlg->m_pBtn_PageUp->Enable(true);
	m_pDlg->m_pBtn_PageDown->Enable(true);
	m_pDlg->m_pBtn_BackToShop->Enable(true);
	
	if (m_pCtrl)
	{
/*		if (m_pCtrl->GetQueryItemResultCount() && m_pCtrl->IsQueryItemForSell())
		{
			m_pDlg->CheckRadioButton(0,0);
			PushSellBtn();
		}
		else if (m_pCtrl->GetQueryItemResultCount() && !m_pCtrl->IsQueryItemForSell())
		{
			m_pDlg->CheckRadioButton(0,1);
			PushBuyBtn();
		}
		else 
		{
			m_pDlg->CheckRadioButton(0,0);
			PushSellBtn();
		}
		*/
		if (bSell)
		{
			m_pDlg->CheckRadioButton(0,0);
			PushSellBtn();
		}
		else
		{
			m_pDlg->CheckRadioButton(0,1);
			PushBuyBtn();
		}

		m_dwLastListTime = 0;
		m_QueryNum = 0;
		m_bNeedQuery = false;
		m_pDlg->m_pLbl_Title->SetText(m_pGameUIMan->GetStringFromTable(10516));
	}
}
void CViewQueryItem::PushSellBtn()
{
	m_bSellOrBuy = true;
	m_bNeedQuery = true;
	m_pDlg->m_pLbl_SearchHint->SetText(m_pGameUIMan->GetStringFromTable(10517));
}
void CViewQueryItem::PushBuyBtn()
{
	m_bSellOrBuy = false;
	m_bNeedQuery = true;
	m_pDlg->m_pLbl_SearchHint->SetText(m_pGameUIMan->GetStringFromTable(10517));
}
void CViewQueryItem::PageDown()
{
	COfflineShopCtrl* pCtrl = m_pHostPlayer->GetOfflineShopCtrl();
	if(!pCtrl) return;

	m_QueryNum = pCtrl->GetQueryResultItemPageNum()+1;

	m_pDlg->m_pLbl_SearchHint->SetText(m_pGameUIMan->GetStringFromTable(10517));
	m_bNeedQuery = true;
}
void CViewQueryItem::PageUp()
{
	COfflineShopCtrl* pCtrl = m_pHostPlayer->GetOfflineShopCtrl();
	if(!pCtrl) return;

	if( pCtrl->GetQueryResultItemPageNum() > 0 )
	{
		m_QueryNum = pCtrl->GetQueryResultItemPageNum()-1;
		m_pDlg->m_pLbl_SearchHint->SetText(m_pGameUIMan->GetStringFromTable(10517));
		m_bNeedQuery = true;
	}
	else
		m_pGameUIMan->MessageBox("",m_pGameUIMan->GetStringFromTable(758), MB_OK, A3DCOLORRGBA(255, 255, 255, 160));
}
void CViewQueryItem::OnTick()
{
	DWORD dwTick = GetTickCount();
	
	COfflineShopCtrl* pCtrl = m_pHostPlayer->GetOfflineShopCtrl();
	if(!pCtrl) return;

	if( CECTimeSafeChecker::ElapsedTime(dwTick, m_dwLastListTime) > LIST_TICK && m_bNeedQuery && pCtrl->GetQueryItemID() > 0 )
	{
		pCtrl->SetItemState(COfflineShopCtrl::ITEMSTATE_SEND_QUERYITEM_FROMITEM);
		g_pGame->GetGameSession()->OffShop_QueryItemListPage(pCtrl->GetQueryItemID(),m_bSellOrBuy,m_QueryNum);
		
		m_dwLastListTime = dwTick;
		m_bNeedQuery = false;
	}
}
void CViewQueryItem::UpdateItems()
{
	COfflineShopCtrl* pCtrl = m_pHostPlayer->GetOfflineShopCtrl();
	if(!pCtrl) return;

	m_pDlg->ClearControls(m_bSellOrBuy);

	for(int i=0;i<SHOP_COUNT_PERPAGE;i++)
	{
		COfflineShopCtrl::ItemInfo* pItemInfo = pCtrl->GetQueryResultItem(IsSellModel(),i);
		if(pItemInfo)
		{
			CECIvtrItem* pItem = pItemInfo->pItem;
			if(!pItem) continue;

			unsigned int nPrice = (unsigned int)pItem->GetUnitPrice();
			__int64 NeedMoney = nPrice;// * (__int64)pItem->GetCount();
			
			m_pDlg->m_pLbl_ItemName[i]->SetText(pItem->GetName());
			m_pDlg->m_pBtn_Buy[i]->Enable(pCtrl->CanDo(pItemInfo->roleid));
			m_pDlg->m_pBtn_Buy[i]->Show(pCtrl->GetNPCSevFlag() == COfflineShopCtrl::NPCSEV_SELLBUY);
			m_pDlg->m_pBtn_Buy[i]->SetDataPtr(pItem,"ptr_CECIvtrItem");

			m_pDlg->m_pLbl_Money[i]->SetColor(m_pGameUIMan->GetPriceColor(NeedMoney));
			m_pDlg->m_pLbl_Money[i]->SetText(m_pGameUIMan->GetFormatNumber(NeedMoney));
			
			if (m_bSellOrBuy) {
				ACString strHint = CECUIHelper::GetGameUIMan()->GetItemDescHint(pItem, true, true);
				m_pDlg->m_pImg_Item[i]->SetHint(strHint);
			} else {
				AUICTranslate trans;
				m_pDlg->m_pImg_Item[i]->SetHint(trans.Translate(pItem->GetDesc()));
			}

			AString strFile;
			af_GetFileTitle(pItem->GetIconFile(), strFile);
			strFile.MakeLower();			
			m_pDlg->m_pImg_Item[i]->SetCover( m_pGameUIMan->GetIconCover(CECGameUIMan::ICONS_INVENTORY),
					  m_pGameUIMan->GetIconIndex(CECGameUIMan::ICONS_INVENTORY, strFile) );

			if (pItem->GetCount() > 1)
			{
				ACString str;
				str.Format(_AL("%d"),pItem->GetCount());
				m_pDlg->m_pImg_Item[i]->SetText(str);
			}
			else
				m_pDlg->m_pImg_Item[i]->SetText(_AL(""));
		}
	}	
}
//////////////////////////////////////////////////////////////////////////

CViewShopItem::CViewShopItem(CDlgOfflineShopItemsList* pDlg):CUIActionDelegate(pDlg)
{
};
void CViewShopItem::OnInstallViewer(CECHostPlayer* pHost,CECGameUIMan* pUI,CECGameSession* pSession,bool bSell)
{
	CUIActionDelegate::OnInstallViewer(pHost,pUI,pSession,bSell);

	m_pDlg->m_pBtn_PageUp->Enable(false);
	m_pDlg->m_pBtn_PageDown->Enable(false);
	m_pDlg->m_pBtn_BackToShop->Enable(true);
	
	if (m_pCtrl)
	{
/*		if (!m_pCtrl->IsELShopInvEmpty(true))
		{
			m_pDlg->CheckRadioButton(0,0);
			PushSellBtn();
		}
		else if (!m_pCtrl->IsELShopInvEmpty(false) )
		{
			m_pDlg->CheckRadioButton(0,1);
			PushBuyBtn();
		}
		else 
		{
			m_pDlg->CheckRadioButton(0,0);
			PushSellBtn();
		}*/
		if (bSell)
		{
			m_pDlg->CheckRadioButton(0,0);
			PushSellBtn();
		}
		else
		{
			m_pDlg->CheckRadioButton(0,1);
			PushBuyBtn();
		}

		m_pDlg->m_pLbl_Title->SetText(m_pGameUIMan->GetStringFromTable(10515));
	}
}
void CViewShopItem::PushSellBtn()
{
	m_bSellOrBuy = true;
	UpdateItems();
}
void CViewShopItem::PushBuyBtn()
{
	m_bSellOrBuy = false;
	UpdateItems();
}
void CViewShopItem::UpdateItems()
{
	COfflineShopCtrl* pCtrl = m_pHostPlayer->GetOfflineShopCtrl();
	m_pDlg->ClearControls(m_bSellOrBuy);
	for(int i=0;i<SHOP_COUNT_PERPAGE;i++)
	{
		CECIvtrItem* pItem = pCtrl->GetELShopItem(m_bSellOrBuy,i);
		if(pItem)
		{
			__int64 nPrice = (unsigned int)pItem->GetUnitPrice();// * (__int64)pItem->GetCount();
			
		//	int nNeedYP = nPrice / 10000000;			
		//	int NeedMoney = nPrice - nNeedYP * 10000000;

			m_pDlg->m_pLbl_ItemName[i]->SetText(pItem->GetName());
			m_pDlg->m_pBtn_Buy[i]->Enable(pCtrl->CanDo(pCtrl->GetCurOtherShopID()));
			m_pDlg->m_pBtn_Buy[i]->Show(pCtrl->GetNPCSevFlag() == COfflineShopCtrl::NPCSEV_SELLBUY);
			m_pDlg->m_pBtn_Buy[i]->SetDataPtr(pItem,"ptr_CECIvtrItem");
			
			m_pDlg->m_pLbl_Money[i]->SetColor(m_pGameUIMan->GetPriceColor(nPrice));
			m_pDlg->m_pLbl_Money[i]->SetText(m_pGameUIMan->GetFormatNumber(nPrice));

			if (m_bSellOrBuy) {
				ACString strHint = CECUIHelper::GetGameUIMan()->GetItemDescHint(pItem, true, true);
				m_pDlg->m_pImg_Item[i]->SetHint(strHint);
			} else {
				AUICTranslate trans;
				m_pDlg->m_pImg_Item[i]->SetHint(trans.Translate(pItem->GetDesc()));
			}

			AString strFile;
			af_GetFileTitle(pItem->GetIconFile(), strFile);
			strFile.MakeLower();			
			m_pDlg->m_pImg_Item[i]->SetCover( m_pGameUIMan->GetIconCover(CECGameUIMan::ICONS_INVENTORY),
					  m_pGameUIMan->GetIconIndex(CECGameUIMan::ICONS_INVENTORY, strFile) );

			if (pItem->GetCount() > 1)
			{
				ACString str;
				str.Format(_AL("%d"),pItem->GetCount());
				m_pDlg->m_pImg_Item[i]->SetText(str);
			}
			else
				m_pDlg->m_pImg_Item[i]->SetText(_AL(""));
		}
	}
}