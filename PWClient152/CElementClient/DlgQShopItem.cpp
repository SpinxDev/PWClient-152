// Filename	: DlgQShopItem.cpp
// Creator	: Tom Zhou
// Date		: 2006/5/22

#include "AFI.h"
#include "DlgQShopItem.h"
#include "DlgQShop.h"
#include "DlgBuyConfirm.h"
#include "EC_GameUIMan.h"
#include "AUIDef.h"
#include "AUICTranslate.h"
#include "globaldataman.h"
#include "EC_IvtrItem.h"
#include "EC_IvtrConsume.h"
#include "EC_GameSession.h"
#include "EC_HostPlayer.h"
#include "EC_GPDataType.h"
#include "EC_Model.h"
#include "EC_Game.h"
#include "EC_GameRun.h"
#include "EC_UIManager.h"
#include "EC_Global.h"
#include "elementdataman.h"
#include "TaskTemplMan.h"
#include "EC_TaskInterface.h"
#include "EC_Inventory.h"
#include "DlgGivingFor.h"
#include "EC_UIConfigs.h"
#include "DlgQShopBuy.h"
#include "EC_Shop.h"
#include "EC_ShopSearch.h"

#define new A_DEBUG_NEW

AUI_BEGIN_COMMAND_MAP(CDlgQShopItem, CDlgBase)

AUI_ON_COMMAND("buy",		OnCommand_Buy)
AUI_ON_COMMAND("time*",		OnCommand_Time)
AUI_ON_COMMAND("IDCANCEL",	OnCommand_CANCEL)
AUI_ON_COMMAND("Btn_Giving",	OnCommand_GivePresent)
AUI_ON_COMMAND("Btn_For",		OnCommand_AskForPresent)
AUI_ON_COMMAND("Btn_Batch",		OnCommand_BatchBuy)

AUI_END_COMMAND_MAP()

AUI_BEGIN_EVENT_MAP(CDlgQShopItem, CDlgBase)

AUI_ON_EVENT("*",		WM_MOUSEWHEEL,		OnEventMouseWheel)
AUI_ON_EVENT(NULL,		WM_MOUSEWHEEL,		OnEventMouseWheel)
AUI_ON_EVENT("*",		WM_LBUTTONDOWN,		OnEventLButtonDown)
AUI_ON_EVENT(NULL,		WM_LBUTTONDOWN,		OnEventLButtonDown)

AUI_END_EVENT_MAP()


CDlgQShopItem::CDlgQShopItem()
{
	m_pModel = NULL;
	m_nTimeSelect = 0;
	m_BuyType = -1;
	for (int i = 0; i < 4; i++)
	{
		m_TypeNew[i] = -1;
		m_TypeDefault[i] = -1;
	}

}

CDlgQShopItem::~CDlgQShopItem()
{
	A3DRELEASE(m_pModel);
}

bool CDlgQShopItem::OnInitDialog()
{
	m_nItemIndex = -1;
	m_nDlgPostion = -1;
	m_pTxt_ItemName = (PAUILABEL)GetDlgItem("Txt_Itemname");
	m_pTxt_Time = (PAUILABEL)GetDlgItem("Txt_Time");
	m_pTxt_Price = (PAUILABEL)GetDlgItem("Txt_Price");
	m_pBtn_Buy = (PAUISTILLIMAGEBUTTON)GetDlgItem("Btn_Buy");
	m_pImg_Item = (PAUIIMAGEPICTURE)GetDlgItem("Pic_Goods");
	m_pImg_New = (PAUIIMAGEPICTURE)GetDlgItem("img_new");
	m_pImg_Hot = (PAUIIMAGEPICTURE)GetDlgItem("img_hot");
	m_pImg_Gift = (PAUIIMAGEPICTURE)GetDlgItem("img_gift");
	m_pImg_Bg01 = (PAUIIMAGEPICTURE)GetDlgItem("Img_Bg01");
	m_pImg_Bg02 = (PAUIIMAGEPICTURE)GetDlgItem("Img_Bg02");
	m_pImg_Bg03 = (PAUIIMAGEPICTURE)GetDlgItem("Img_Bg03");
	m_pImg_Bg04 = (PAUIIMAGEPICTURE)GetDlgItem("Img_Bg04");
	m_pBtn_BatchBuy = (PAUISTILLIMAGEBUTTON)GetDlgItem("Btn_Batch");

	m_pBtn_GivePresent = (PAUISTILLIMAGEBUTTON)GetDlgItem("Btn_Giving");
	m_pBtn_AskForPresent = (PAUISTILLIMAGEBUTTON)GetDlgItem("Btn_For");
	
	for(int i = 0; i < 3; i++)
	{
		char szName[30];
		sprintf(szName, "Btn_Time%d", i + 1);
		m_pBtn_Time[i] = (PAUISTILLIMAGEBUTTON)GetDlgItem(szName);
	}
	SetCanMove(false);

	return true;
}

bool CDlgQShopItem::Render()
{
	char szText[20];
	sprintf(szText, "Lab_Dialog%d", m_nDlgPostion);
	POINT pt = GetShop()->GetDlgItem(szText)->GetPos();
	SetPosEx(pt.x, pt.y);

	return CDlgBase::Render();
}

bool CDlgQShopItem::CheckBuyedItem()
{
	bool bCanBuy(false);
	CECShopBase &shop = GetShop()->GetShopData();
	if (m_nItemIndex >= 0 && m_nItemIndex < shop.GetCount()){
		CECShopSearchTaskPolicy taskPolicy;
		bCanBuy = taskPolicy.CanAccept(&shop, m_nItemIndex);
	}
	return bCanBuy;
}

void CDlgQShopItem::OnTick()
{
	abase::vector<GSHOP_ITEM> *items = GetShop()->GetShopItems();
	if (m_nItemIndex < 0 || static_cast<unsigned int>(m_nItemIndex) >= items->size())
	{
		return;
	}

	bool bEnableBuy = m_nPrice <= GetShop()->GetCash() && CheckBuyedItem();
	m_pBtn_Buy->Enable(bEnableBuy);
	m_pBtn_BatchBuy->Enable(bEnableBuy);

	if(m_pBtn_GivePresent)
	{
		m_pBtn_GivePresent->Show(CanGivingFor());
		m_pBtn_GivePresent->Enable(CanGivePresent());
	}

	if(m_pBtn_AskForPresent)
	{
		m_pBtn_AskForPresent->Show(CanGivingFor());
		m_pBtn_AskForPresent->Enable(CanAskForPresent());
	}
}

bool CDlgQShopItem::CanGivingFor()
{
	return CECUIConfig::Instance().GetGameUI().bEnableGivingFor
		&& GetShop()->IsQShop()
		&& !GetShop()->GetShopDataConst().HasOwnerNPC();
}

bool CDlgQShopItem::CanGivePresent()
{
	abase::vector<GSHOP_ITEM> *items = GetShop()->GetShopItems();

	return m_nPrice <= GetShop()->GetCash() && (CECUIConfig::Instance().GetGameUI().bEnableGivingForTaskLimitedItem || !CECQShopConfig::Instance().CanFilterID((*items)[m_nItemIndex].id));
}
	
bool CDlgQShopItem::CanAskForPresent()
{
	abase::vector<GSHOP_ITEM> *items = GetShop()->GetShopItems();

	return CheckBuyedItem() && (CECUIConfig::Instance().GetGameUI().bEnableGivingForTaskLimitedItem || !CECQShopConfig::Instance().CanFilterID((*items)[m_nItemIndex].id));
}

void CDlgQShopItem::OnCommand_GivePresent(const char *szCommand)
{
	abase::vector<GSHOP_ITEM> *items = GetShop()->GetShopItems();
	if( m_nItemIndex != -1 && CanGivePresent() )
	{
		int index = GetBuyIndex();
	
		CDlgGivingFor*pDlg = dynamic_cast<CDlgGivingFor*>(GetGameUIMan()->GetDialog("Win_GivingFor"));
		if(pDlg)
		{
			pDlg->SetType(true, (*items)[m_nItemIndex].id, m_nPrice, m_nItemIndex, index);
			pDlg->Show(true);
		}
	}
}

void CDlgQShopItem::OnCommand_AskForPresent(const char *szCommand)
{
	abase::vector<GSHOP_ITEM> *items = GetShop()->GetShopItems();
	if( m_nItemIndex != -1 && CanAskForPresent() )
	{
		int index = GetBuyIndex();
		
		CDlgGivingFor*pDlg = dynamic_cast<CDlgGivingFor*>(GetGameUIMan()->GetDialog("Win_GivingFor"));
		if(pDlg)
		{
			pDlg->SetType(false, (*items)[m_nItemIndex].id, m_nPrice, m_nItemIndex, index);
			pDlg->Show(true);
		}
	}
}

void CDlgQShopItem::OnCommand_Buy(const char * szCommand)
{
	CDlgQShop *pShop = GetShop();
	if( m_nItemIndex != -1 && m_nPrice <= pShop->GetCash() && CheckBuyedItem() )
	{
// 		if( (*items)[m_nItemIndex].buy[0].end_time != 0 )
// 		{
// 			long stime = (*items)[m_nItemIndex].buy[0].end_time;
// 			stime -= GetGame()->GetTimeZoneBias() * 60;	// localtime = UTC - bias, which bias is in minute
// 			tm *gtime = gmtime(&stime);
// 			strText1.Format(GetStringFromTable(1505), 
// 				gtime->tm_year - 100, gtime->tm_mon + 1, gtime->tm_mday);
// 			strText.Format(GetStringFromTable(822), GetGameUIMan()->m_pDlgQShop->GetCashText(m_nPrice), m_pTxt_ItemName->GetText(), strText1);
// 		}
// 		else
// 		{
//		}
		GetGameUIMan()->m_pDlgBuyConfirm->Buy(&pShop->GetShopData(), m_nItemIndex, GetBuyIndex());
	}
}

void CDlgQShopItem::OnCommand_Time(const char * szCommand)
{
	CDlgQShop * pShop = GetShop();
	abase::vector<GSHOP_ITEM> *items = pShop->GetShopItems();
	m_pBtn_Time[m_nTimeSelect]->SetPushed(false);
	m_pBtn_Time[m_nTimeSelect]->SetColor(A3DCOLORRGB(255, 255, 255));
	m_nTimeSelect = atoi(szCommand + strlen("time")) - 1;
	m_pBtn_Time[m_nTimeSelect]->SetPushed(true);
	m_pBtn_Time[m_nTimeSelect]->SetColor(A3DCOLORRGB(255, 203, 74));
	int index = GetBuyIndex();
	m_nPrice = (*items)[m_nItemIndex].buy[index].price;
	SetPriceText(m_nPrice, (*items)[m_nItemIndex].buy[index].status);
	m_pImg_New->Show((*items)[m_nItemIndex].buy[index].status == 1);
	m_pImg_Hot->Show((*items)[m_nItemIndex].buy[index].status == 3);
	m_pImg_Gift->Show((*items)[m_nItemIndex].buy[index].status == 2);

	const GSHOP_ITEM &curItem = (*items)[m_nItemIndex];
	unsigned int status = curItem.buy[index].status;
	bool discount = (status>=4 && status <=12);
	
	// 打折底层图标
	m_pImg_Bg01->Show(discount);
	
	// 打折角标
	m_pImg_Bg02->Show(discount);
	if (discount)
		m_pImg_Bg02->FixFrame(12-status);    // status=4 对应打1折
	
	// 赠品
	m_pImg_Bg03->Show(curItem.idGift>0 && curItem.iGiftNum>0);

	// 闪购
	m_pImg_Bg04->Show(status == 13);

	// 通知 shop 更改相关信息
	OnEventLButtonDown(0, 0, NULL);
}

void CDlgQShopItem::SetPriceText(int nPrice, unsigned int status)
{
	// 根据打折信息、更新价格字符串

	CDlgQShop * pShop = GetShop();
	ACString strPrice;
	if (status>=4 && status<=12)
	{
		// 打折
		strPrice.Format(GetStringFromTable(8590),
			pShop->GetCashText(nPrice),
			pShop->GetCashText(CECShopBase::GetOriginalPrice(nPrice, status)));
	}
	else
	{
		// 不打折
		strPrice = pShop->GetCashText(nPrice);
	}
	m_pTxt_Price->SetText(strPrice);
}

void CDlgQShopItem::OnCommand_CANCEL(const char * szCommand)
{
	GetShop()->OnCommand_CANCEL("");
}

void CDlgQShopItem::SetItem(int dlgpostion, int itemindex)
{
	m_nDlgPostion = dlgpostion;
	m_nItemIndex = itemindex;

	if( m_nDlgPostion == -1 )
	{
		Show(false);
		return;
	}

	if( !IsShow() )
		Show(true);
	CDlgQShop *pShop = GetShop();
	abase::vector<GSHOP_ITEM> *items = pShop->GetShopItems();

	CECIvtrItem *pItem = CECIvtrItem::CreateItem((*items)[itemindex].id, 0, (*items)[itemindex].num);
	if( pItem )
	{
		//判断需要显示的购买方式
		int index1 =0, index2 = 0;
		m_BuyType = -1;
		int i;
		for (i = 0; i < 4; i++)
		{
			m_TypeDefault[i] = -1;
			m_TypeNew[i] = -1;
			if ((*items)[itemindex].buy[i].type == 3 && (*items)[itemindex].buy[i].price > 0)
			{
				m_TypeNew[index1] = i;
				index1++;
			}
			else if ((*items)[itemindex].buy[i].type == -1 && (*items)[itemindex].buy[i].price > 0)
			{
				m_TypeDefault[index2] = i;
				index2++;
			}
		}
		if (index1 > 0)
		{
			m_BuyType = 0;
		}
		else
			m_BuyType = 1;

		AString strFile;
		af_GetFileTitle(pItem->GetIconFile(), strFile);
		strFile.MakeLower();
		m_pImg_Item->SetCover(GetGameUIMan()->m_pA2DSpriteIcons[CECGameUIMan::ICONS_INVENTORY],
			GetGameUIMan()->m_IconMap[CECGameUIMan::ICONS_INVENTORY][strFile]);
		
		m_pImg_Item->SetColor(
			(pItem->IsEquipment() && ((CECIvtrEquip *)pItem)->IsDestroying())
			? A3DCOLORRGB(128, 128, 128)
			: A3DCOLORRGB(255, 255, 255));

		ACString strText, strText1;
// 		if(0 /*(*items)[itemindex].buy[0].end_time != 0*/ )
// 		{
// 			long stime = (*items)[itemindex].buy[0].end_time;
// 			stime -= GetGame()->GetTimeZoneBias() * 60;	// localtime = UTC - bias, which bias is in minute
// 			tm *gtime = gmtime(&stime);
// 			strText1.Format(GetStringFromTable(1505), 
// 				gtime->tm_year - 100, gtime->tm_mon + 1, gtime->tm_mday);
// 			strText.Format(GetStringFromTable(1504), strText1);
// 			m_pTxt_Time->SetText(strText);
// 			m_pTxt_Time->Show(true);
// 			for(int i = 0; i < 3; i++)
// 				m_pBtn_Time[i]->Show(false);
// 		}
// 		else
// 		{
		m_pTxt_Time->Show(false);
		for(i = 0; i < 3; i++)
		{
			int BuyIndex = 0;
			if (m_BuyType == 0)
			{
				 BuyIndex = m_TypeNew[i];
			}
			else
			{
				BuyIndex = m_TypeDefault[i];
			}
				
			if( BuyIndex != -1 && (*items)[itemindex].buy[BuyIndex].price != 0 )
			{
				m_pBtn_Time[i]->SetText(GetFormatTime((*items)[itemindex].buy[BuyIndex].time));
				m_pBtn_Time[i]->Show(true);
				if( (*items)[itemindex].buy[BuyIndex].time != 0 )
				{
					strText.Format(GetStringFromTable(824), GetFormatTime((*items)[itemindex].buy[i].time));
					m_pBtn_Time[i]->SetHint(strText);
				}
				else
					m_pBtn_Time[i]->SetHint(GetStringFromTable(825));
				if( i == 0 )
				{
					m_pBtn_Time[i]->SetPushed(true);
					m_pBtn_Time[i]->SetColor(A3DCOLORRGB(255, 203, 74));
				}
				else
				{
					m_pBtn_Time[i]->SetPushed(false);
					m_pBtn_Time[i]->SetColor(A3DCOLORRGB(255, 255, 255));
				}
			}
			else
				m_pBtn_Time[i]->Show(false);
		}
/*		}*/
		m_nTimeSelect = 0;
		int index = GetBuyIndex();
		m_nPrice = (*items)[itemindex].buy[index].price;
		SetPriceText(m_nPrice, (*items)[itemindex].buy[index].status);
		m_pTxt_ItemName->SetText((*items)[itemindex].szName);
		m_pImg_New->Show((*items)[itemindex].buy[index].status == 1);
		m_pImg_Hot->Show((*items)[itemindex].buy[index].status == 3);
		m_pImg_Gift->Show((*items)[itemindex].buy[index].status == 2);
		pItem->GetDetailDataFromLocal();
		pItem->SetPriceScale(CECIvtrItem::SCALE_BUY, 0.0f);
		ACString strHint = pItem->GetDesc();
		if (!strHint.IsEmpty())
		{
			AUICTranslate trans;
			strHint = trans.Translate(strHint);
		}
		m_pImg_Item->SetHint(strHint);

		const GSHOP_ITEM & curItem = (*items)[itemindex];
		unsigned int status = curItem.buy[index].status;
		bool discount = (status>=4 && status <=12);

		// 打折底层图标
		m_pImg_Bg01->Show(discount);
		m_pImg_Bg01->SetHint(strHint);

		// 打折角标
		m_pImg_Bg02->Show(discount);
		m_pImg_Bg02->SetHint(strHint);
		if (discount)
			m_pImg_Bg02->FixFrame(12-status);    // status=4 对应打1折

		// 赠品
		m_pImg_Bg03->Show(curItem.idGift>0 && curItem.iGiftNum>0);

		// 闪购
		m_pImg_Bg04->Show(status == 13);

		delete pItem;
	}
}

void CDlgQShopItem::OnEventMouseWheel(WPARAM wParam, LPARAM lParam, AUIObject *pObj)
{
	GetShop()->OnEventMouseWheel(wParam, lParam, NULL);
}

void CDlgQShopItem::OnEventLButtonDown(WPARAM wParam, LPARAM lParam, AUIObject *pObj)
{
	CDlgQShop *pShop = GetShop();

	int nOldSelect = pShop->GetData();
	int nOldBuyIndex = (int)pShop->GetDataPtr();

	int buyIndex = GetBuyIndex();
	if( nOldSelect != m_nItemIndex ||
		nOldBuyIndex != buyIndex)
		pShop->SetSelectItem(m_nItemIndex, buyIndex);
}

ACString CDlgQShopItem::GetFormatTime(int nTime)
{
	CECGameUIMan *pGameUIMan = g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan();
	return pGameUIMan->GetShopItemFormatTime(nTime);
}

CDlgQShop * CDlgQShopItem::GetShop()
{
	CDlgQShop *pShop = GetGameUIMan()->m_pDlgQShop;
	if (!pShop->IsShow())
		pShop = GetGameUIMan()->m_pDlgBackShop;
	return pShop;
}

int CDlgQShopItem::GetBuyIndex()
{
	// 获得当前选中的时限的物品信息下标
	int index;
	if (m_BuyType == 0)
		index = m_TypeNew[m_nTimeSelect];
	else
		index = m_TypeDefault[m_nTimeSelect];
	return index;
}
void CDlgQShopItem::OnCommand_BatchBuy(const char *szCommand)
{
	CDlgQShopBuy* pDlg = dynamic_cast<CDlgQShopBuy*>(GetGameUIMan()->GetDialog("Win_QshopBuy"));
	if (!pDlg->IsShow()) {
		pDlg->Show(true, true);
		pDlg->SetBuyingItem(m_nItemIndex, GetBuyIndex(), &GetShop()->GetShopData());
	}
}