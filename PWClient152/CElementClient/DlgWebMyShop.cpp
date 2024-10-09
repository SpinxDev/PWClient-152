// File		: DlgWebMyShop.cpp
// Creator	: Feng Ning
// Date		: 2010/3/18

#include "AFI.h"
#include "AUICTranslate.h"
#include "DlgWebMyShop.h"
#include "DlgWebViewProduct.h"
#include "DlgWebTradeInfo.h"
#include "DlgAutoLock.h"
#include "EC_ShortcutMgr.h"
#include "EC_GameUIMan.h"
#include "EC_LoginUIMan.h"
#include "EC_GameSession.h"
#include "EC_GameRun.h"
#include "EC_HostPlayer.h"
#include "EC_Inventory.h"
#include "EC_IvtrItem.h"
#include "EC_IvtrEquip.h"
#include "webtradeattendlist_re.hpp"
#include "webtradeprepost_re.hpp"
#include "webtradeprecancelpost_re.hpp"
#include "webtradeupdate_re.hpp"

AUI_BEGIN_COMMAND_MAP(CDlgWebMyShop, CDlgWebTradeBase)

AUI_ON_COMMAND("goto_viewproduct",	OnCommandGotoViewProduct)
AUI_ON_COMMAND("prepost_item",		OnCommandPrepostItem)
AUI_ON_COMMAND("prepost_money",		OnCommandPrepostMoney)
AUI_ON_COMMAND("prepost_role",		OnCommandPrepostRole)
AUI_ON_COMMAND("confirm_deal",		OnCommandConfirmDeal)
AUI_ON_COMMAND("cancel_deal",		OnCommandCancelDeal)
AUI_ON_COMMAND("cancel_prepost",	OnCommandCancelPrepost)
AUI_ON_COMMAND("check_sell",		OnCommandCheckSell)
AUI_ON_COMMAND("IDCANCEL",			OnCommandCancel)
AUI_END_COMMAND_MAP()

AUI_BEGIN_EVENT_MAP(CDlgWebMyShop, CDlgWebTradeBase)

AUI_ON_EVENT("Item_Prepost",	WM_LBUTTONDOWN,	OnEventLButtonDown_Item_Prepost)

AUI_END_EVENT_MAP()

#define new A_DEBUG_NEW

#define DEFAULT_MIN_POST_MONEY      1000000
#define DEFAULT_POST_DEPOSIT        300000
#define MAX_SELL_PERIOD             (7*24*3600)
#define MAX_SELL_PRICE				100000000
#define MIN_SELL_LEVEL				95
#define MAX_SELL_COUNT				1

#define WEB_MYSHOP_MSGBOX(str) m_pAUIManager->MessageBox("", str, MB_OK, A3DCOLORRGBA(255, 255, 255, 160));

//------------------------------------------------------------------------
// Right-Click Shortcut for CDlgWebMyShop
//------------------------------------------------------------------------
typedef CECShortcutMgr::SimpleClickShortcut<CDlgWebMyShop> WebMyShopClickShortcut;
//------------------------------------------------------------------------

void CDlgWebMyShop::ShowObjectsForMoney(bool bShow)
{
	m_pTxt_Currency->Show(bShow);
	GetDlgItem("Lab_Money_Name")->Show(bShow);
	((PAUISTILLIMAGEBUTTON)GetDlgItem("Btn_SellItems"))->SetPushed(!bShow);
	((PAUISTILLIMAGEBUTTON)GetDlgItem("Btn_Currencysell"))->SetPushed(bShow);
}

void CDlgWebMyShop::ShowObjectsForItem(bool bShow)
{
	m_ItemSelected.pName->Show(bShow);
	m_ItemSelected.pImg->Show(bShow);
	((PAUISTILLIMAGEBUTTON)GetDlgItem("Btn_SellItems"))->SetPushed(bShow);
	((PAUISTILLIMAGEBUTTON)GetDlgItem("Btn_Currencysell"))->SetPushed(!bShow);
}

void CDlgWebMyShop::EnableObjectsForSell(bool bEnable)
{
	m_pTxt_Price1->Enable(bEnable);
	m_pChk_Sell->Enable(bEnable);

	m_pTxt_BuyerName->Enable(bEnable);
	
	m_pTxt_Currency->Enable(bEnable);
	m_ItemSelected.Enable(bEnable);
	
	m_pCombo_Period->Enable(bEnable);

	GetDlgItem("Btn_OK")->Enable(bEnable);
	GetDlgItem("Btn_Cancel")->Enable(bEnable);
}

CDlgWebMyShop::ItemSelected::ItemSelected()
:pImg(NULL)
,pName(NULL)
,pData(NULL)
{
	Reset();
}

void CDlgWebMyShop::ItemSelected::Reset()
{
	if(pImg)
	{
		pImg->SetCover(NULL, -1);
		pImg->SetText(_AL(""));
		pImg->SetHint(_AL(""));
	}

	if(pName)
	{
		pName->SetText(_AL(""));
	}
	
	Pos = -1;
	ID = 0;

	pData = NULL;
}

void CDlgWebMyShop::ItemSelected::Enable(bool bEnable)
{
	if(pImg)
	{
		pImg->Enable(bEnable);
	}
	
	if(pName)
	{
		pName->Enable(bEnable);
	}
}

void CDlgWebMyShop::ItemSelected::Bind(CECIvtrItem * pItem, int nItemPos)
{
	ACHAR szText[20];
	a_sprintf(szText, _AL("%d"), pItem->GetCount());
	pImg->SetText(szText);

	pName->SetText(pItem->GetName());
	
	ID = pItem->GetTemplateID();
	
	Pos = nItemPos;

	pData = pItem;
}

CDlgWebMyShop::CDlgWebMyShop()
:m_Mode(MODE_ITEM)

,m_pTxt_Currency(NULL)
,m_pTxt_Price1(NULL)
,m_pChk_Sell(NULL)
,m_pCombo_Period(NULL)
,m_pTxt_BuyerName(NULL)

,m_PageBegin(0)
,m_PageEnd(-1)
{
}

CDlgWebMyShop::~CDlgWebMyShop()
{
}

bool CDlgWebMyShop::OnInitDialog()
{
	if(!CDlgWebTradeBase::OnInitDialog())
	{
		return false;
	}

	//由CDlgWebRoleTrade初始化CDlgWebMyShop时，当前的Manager不是CECGameUIMan而是CECLoginUIMan
	if(dynamic_cast<CECGameUIMan*>(m_pAUIManager) && GetGameUIMan()->GetShortcutMgr())
	{
		GetGameUIMan()->GetShortcutMgr()->RegisterShortCut(new WebMyShopClickShortcut(this));
	}

	DDX_Control("Txt_BinPrice01", m_pTxt_Price1);
	DDX_Control("Chk_Sell", m_pChk_Sell);
	
	DDX_Control("Txt_BuyerName", m_pTxt_BuyerName);

	DDX_Control("Txt_Currency", m_pTxt_Currency);
	m_ItemSelected.pImg = dynamic_cast<PAUIIMAGEPICTURE>(GetDlgItem("Item_Prepost"));
	m_ItemSelected.pName = dynamic_cast<PAUIOBJECT>(GetDlgItem("Txt_Name"));

	// initialize the period list
	DDX_Control("Combo_Period", m_pCombo_Period);
	m_pCombo_Period->ResetContent();
	for(int i=1;i<=7;i++)
	{
		ACString strItem;
		strItem.Format(GetStringFromTable(5500), i);
		m_pCombo_Period->AddString(strItem);
	}
	m_pCombo_Period->SetCurSel(0);

	// tab style
	PAUISTILLIMAGEBUTTON pBtn_Goto = dynamic_cast<PAUISTILLIMAGEBUTTON>(GetDlgItem("Btn_Myshop"));
	if(pBtn_Goto) pBtn_Goto->SetPushed(true);

	// reset all the data
	OnCommandCancelDeal(NULL);

	return true;
}

void CDlgWebMyShop::OnShowDialog()
{
	CDlgWebTradeBase::OnShowDialog();

	OnCommandPrepostItem(NULL);
	OnPageSwitch(PAGE_HOME_FLAG);
	GetGameUIMan()->GetDialog("Win_Inventory")->Show(true);
}

void CDlgWebMyShop::OnCommandGotoViewProduct(const char* szCommand)
{
	GetGameUIMan()->m_pDlgWebMyShop->Show(false);
	GetGameUIMan()->m_pDlgWebViewProduct->SetPosEx(GetPos().x, GetPos().y);
	GetGameUIMan()->m_pDlgWebViewProduct->Show(true);
}

void CDlgWebMyShop::OnCommandPrepostItem(const char* szCommand)
{
	m_Mode = MODE_ITEM;
	ShowObjectsForMoney(false);
	ShowObjectsForItem(true);
}

void CDlgWebMyShop::OnCommandPrepostMoney(const char* szCommand)
{
	m_Mode = MODE_MONEY;
	ShowObjectsForMoney(true);
	ShowObjectsForItem(false);
}

void CDlgWebMyShop::OnCommandConfirmDeal(const char* szCommand)
{
	PrepostInfo info;

	if(!ValidatePrepost(info))
	{
		return;
	}

	// put to queue and handled in tick function
	m_Infos.AddTail(info);

	EnableObjectsForSell(false);
}

void CDlgWebMyShop::OnCommandCancelDeal(const char* szCommand)
{
	ClearSelectData();
	m_pTxt_BuyerName->SetText(_AL(""));
	m_pTxt_Currency->SetText(_AL(""));
	m_pCombo_Period->SetCurSel(0);
}

void CDlgWebMyShop::OnCommandCancelPrepost(const char* szCommand)
{
	int nSel = m_pLst_Item->GetCurSel();
	if(nSel >= 0 && nSel < m_pLst_Item->GetCount())
	{
		__int64 idTrade = GetTradeID(nSel);
		if(idTrade > 0)
		{
			//GetGameUIMan()->AddChatMessage(GetStringFromTable(5511), GP_CHAT_MISC);
			GetGameSession()->webTrade_PreCancel(idTrade);
		}
	}
}

void CDlgWebMyShop::OnCommandCheckSell(const char* szCommand)
{
	bool bEnable = m_pChk_Sell->IsChecked();
	m_pTxt_Price1->Enable(bEnable);
	
	ACString szNum;
	szNum.Format(_AL("%d"), GetMinSellPrice() / 100);
	m_pTxt_Price1->SetText(szNum);

	m_pCombo_Period->Enable(bEnable);
	m_pTxt_BuyerName->Enable(bEnable);

	A3DCOLOR col = bEnable ? 0xFFFFCB4A:0xFF808080;
	GetDlgItem("Lab_Price")->SetColor(col);
	GetDlgItem("Lab_Period")->SetColor(col);
	GetDlgItem("Lab_Radio")->SetColor(col);
}

void CDlgWebMyShop::OnCommandCancel(const char* szCommand)
{
	Show(false);
	OnCommandCancelDeal(NULL);
	GetGameUIMan()->EndNPCService();
}

void CDlgWebMyShop::WebTradeAction(int idAction, void *pData)
{
	CDlgWebTradeBase::WebTradeAction(idAction, pData);
	
	if( idAction == PROTOCOL_WEBTRADEATTENDLIST_RE)
	{
		WebTradeAttendList_Re *pList = (WebTradeAttendList_Re *)pData;

		if(pList->getsell)
		{
			if(pList->items.size() > 0)
			{
				if(GetPageMode() == PAGE_HOME_FLAG)
				{
					m_PageBegin = 0;
				}
				else if(GetPageMode() == PAGE_END_FLAG)
				{
					m_PageBegin = max(0, pList->end - WEBTRADE_PAGE_SIZE);
				}
				if(GetPageMode() == PAGE_DOWN_FLAG)
				{
					m_PageBegin += WEBTRADE_PAGE_SIZE;
				}
				else if(GetPageMode() == PAGE_UP_FLAG)
				{
					m_PageBegin = max(0, m_PageBegin - WEBTRADE_PAGE_SIZE);
				}

				m_PageEnd = (GetPageMode() == PAGE_END_FLAG || pList->items.size() < WEBTRADE_PAGE_SIZE) ? 
							 -1:m_PageBegin + WEBTRADE_PAGE_SIZE;

				UpdateItemList(pList->items.empty() ? NULL : &pList->items[0], pList->items.size(), true);
			}
			CDlgWebTradeBase::OnPageSwitch(PAGE_INVALID);
		}
	}
	else if( idAction == PROTOCOL_WEBTRADEPREPOST_RE)
	{
		WebTradePrePost_Re *pInfo = (WebTradePrePost_Re*)pData;
		
		if(!pInfo->retcode)
		{
			GetGameUIMan()->AddChatMessage(GetStringFromTable(5508), GP_CHAT_MISC);
			UpdateItemList(&pInfo->info, 1, false, INSERT_TAIL);
		}

	} else if( idAction == PROTOCOL_WEBTRADEPRECANCELPOST_RE) {
		
		WebTradePreCancelPost_Re* pInfo = (WebTradePreCancelPost_Re*)pData;
		if(!pInfo->retcode)
		{
			GetGameUIMan()->AddChatMessage(GetStringFromTable(5512), GP_CHAT_MISC);
			ItemInfo* pItemInfo = GetItemInfo(pInfo->sn);
			if(pItemInfo)
			{
				pItemInfo->SetState(ItemInfo::STATE_PRE_CANCEL_POST);
			}
		}
		
	} else if( idAction == PROTOCOL_WEBTRADEUPDATE_RE) {
		WebTradeUpdate_Re* pInfo = (WebTradeUpdate_Re*)pData;
		
		if(!pInfo->retcode)
		{
			// update or insert a item
			UpdateItemList(&pInfo->item, 1, false);
		}
		else
		{
			DeleteTradeInfo(pInfo->sn);
			// refresh current page for status 
			OnPageSwitch(PAGE_REFRESH_FLAG);
		}
	}
}

void CDlgWebMyShop::OnPageSwitch(int mode)
{
	if(GetPageMode() != PAGE_INVALID)
	{
		return;
	}

	CDlgWebTradeBase::OnPageSwitch(mode);

	//GetGameUIMan()->AddChatMessage(GetStringFromTable(5514), GP_CHAT_MISC);
	if(PAGE_HOME_FLAG == mode)
	{
		GetGameSession()->webTrade_AttendList(0, true);
	}
	else if(PAGE_END_FLAG == mode)
	{
		GetGameSession()->webTrade_AttendList(-1, true);
	}
	else if(PAGE_UP_FLAG == mode)
	{
		if(m_PageBegin > 0)
		{
			GetGameSession()->webTrade_AttendList(max(0, m_PageBegin - WEBTRADE_PAGE_SIZE), true);
		}
		else
		{
			CDlgWebTradeBase::OnPageSwitch(PAGE_INVALID);
		}
	}
	else if(PAGE_DOWN_FLAG == mode)
	{
		if(m_PageEnd != -1)
		{
			GetGameSession()->webTrade_AttendList(m_PageBegin + WEBTRADE_PAGE_SIZE, true);
		}
		else
		{
			CDlgWebTradeBase::OnPageSwitch(PAGE_INVALID);
		}
	}
	else if(PAGE_REFRESH_FLAG == mode)
	{
		GetGameSession()->webTrade_AttendList(m_PageBegin, true);
	}
}

ACString CDlgWebMyShop::GetRowString(const CDlgWebTradeBase::ItemInfo& tradeItem)
{
	ACString szText;
	
	szText.Format( _AL("%s\t%s\t%s\t%s"),
					tradeItem.GetItemName(),
					tradeItem.GetTime(),
					tradeItem.GetFormatedPrice(),
					tradeItem.GetBuyerName() );
	
	return szText;
}

void CDlgWebMyShop::InsertTradeInfo(int i, ItemInfo* pInfo)
{
	CDlgWebTradeBase::InsertTradeInfo(i, pInfo);

	m_pLst_Item->SetItemData(i, pInfo->GetTimeValue(),1);
	m_pLst_Item->SetItemData(i, pInfo->GetBasicInfo().price,2);
}

void CDlgWebMyShop::ClearSelectData()
{
	m_ItemSelected.Reset();

	m_pChk_Sell->Check(false);
	OnCommandCheckSell(NULL);
}

bool CDlgWebMyShop::ValidatePrepost(PrepostInfo& info)
{
	info.Confirm = 0;
	info.Mode = m_Mode;

	info.BuyerName = m_pTxt_BuyerName->GetText();
	info.BuyerName.TrimLeft();
	info.BuyerName.TrimRight();

	// currently, we only check buyer name in login scene for role trading
	CECLoginUIMan* pLoginUIMan = dynamic_cast<CECLoginUIMan*>(m_pAUIManager);
	if(pLoginUIMan)
	{
		const abase::vector<GNET::RoleInfo>& infos = pLoginUIMan->GetVecRoleInfo();
		for(size_t i=0;i<infos.size();i++)
		{
			const GNET::RoleInfo& roleInfo = infos[i];
			ACString strName((const ACHAR *)roleInfo.name.begin(), roleInfo.name.size() / sizeof(ACHAR));
			if(strName == info.BuyerName)
			{
				WEB_MYSHOP_MSGBOX(GetStringFromTable(5563));
				return false;
			}
		}
	}

	info.BuyerRoleID = !info.BuyerName.IsEmpty() ? PrepostInfo::NOTGET:PrepostInfo::NOBUYER;
	
	info.Price = 0;

	if(m_pChk_Sell->IsChecked())
	{
		int iOut, iOutTime;

		if(!ConvertToDouble(m_pTxt_Price1->GetText(), 2, iOut, iOutTime))
		{
			WEB_MYSHOP_MSGBOX(GetStringFromTable(5519));
			return false;
		}
		
		info.Price = (100 / iOutTime) * iOut;
	}

	if(m_pChk_Sell->IsChecked())
	{
		if(GetMinSellPrice() > info.Price)
		{
			ACString szPrice = CDlgWebTradeBase::ItemInfo::GetFormatedPrice(GetMinSellPrice());
			ACString szRes;
			szRes.Format( GetStringFromTable(5517), szPrice);
			WEB_MYSHOP_MSGBOX(szRes);
			return false;
		}
		else if(MAX_SELL_PRICE < info.Price)
		{
			ACString szPrice = CDlgWebTradeBase::ItemInfo::GetFormatedPrice(MAX_SELL_PRICE);
			ACString szRes;
			szRes.Format( GetStringFromTable(5518), szPrice);
			WEB_MYSHOP_MSGBOX(szRes);
			return false;
		}
	}
	
	info.SellPeriod = min(max(m_pCombo_Period->GetCurSel()+1,1) * 24 * 3600, DEFAULT_MIN_POST_MONEY);
	
	if(MODE_ITEM == m_Mode)
	{
		if(!m_ItemSelected.pData)
		{
			return false;
		}

		if(GetHostPlayer()->GetMoneyAmount() < DEFAULT_POST_DEPOSIT)
		{
			ACString szTxt;
			szTxt.Format(GetStringFromTable(5522), ((CECBaseUIMan*)m_pAUIManager)->GetFormatNumber(DEFAULT_POST_DEPOSIT));
			WEB_MYSHOP_MSGBOX(szTxt);
			return false;
		}

		info.Item.ID = m_ItemSelected.pData->GetTemplateID();
		info.Item.Slot = m_ItemSelected.Pos;
		info.Item.Count = m_ItemSelected.pData->GetCount();

		if(info.Item.ID <= 0 || info.Item.Slot < 0)
		{
			WEB_MYSHOP_MSGBOX(GetStringFromTable(5502));
			return false;
		}
	}
	else if(MODE_MONEY == m_Mode)
	{
		info.Money.Count = a_atoi(m_pTxt_Currency->GetText());
		if(info.Money.Count < DEFAULT_MIN_POST_MONEY)
		{
			ACString szTxt;
			szTxt.Format(GetStringFromTable(5521), ((CECBaseUIMan*)m_pAUIManager)->GetFormatNumber(DEFAULT_MIN_POST_MONEY));
			WEB_MYSHOP_MSGBOX(szTxt);
			return false;
		}
		else if(info.Money.Count > GetHostPlayer()->GetMoneyAmount())
		{
			WEB_MYSHOP_MSGBOX(GetStringFromTable(5504));
			return false;
		}
	}
	else if(MODE_ROLE == m_Mode)
	{
		info.Role.ID = GetGameRun()->GetSellingRoleID();
		if(info.Role.ID <= 0)
		{
			WEB_MYSHOP_MSGBOX(GetStringFromTable(5550));
			return false;
		}
	}

	return true;
}

void CDlgWebMyShop::CheckPrepostList()
{
	ALISTPOSITION infoPos = m_Infos.GetHeadPosition();
	while(infoPos)
	{
		ALISTPOSITION cur = infoPos;
		PrepostInfo& info = m_Infos.GetNext(infoPos);
		
		// whether should get the player id
		if(PrepostInfo::NOTGET == info.BuyerRoleID)
		{
			int id = GetGameRun()->GetPlayerID(info.BuyerName);
			if(id)
			{
				info.BuyerRoleID = id;
			}
			else
			{
				info.BuyerRoleID = PrepostInfo::GETTING;
				GetGameSession()->GetPlayerIDByName(info.BuyerName, 4);
				continue;
			}
		}
		// whether the id was got already
		else if(PrepostInfo::GETTING == info.BuyerRoleID)
		{
			int id = GetGameRun()->GetPlayerID(info.BuyerName);
			if(id)
			{
				info.BuyerRoleID = id;
			}
			else
			{
				continue;
			}
		}

		// do not handle it before confirmed
		if(info.Confirm == -1)
		{
			// waiting for confirmation
			continue;
		}
		else if(info.Confirm == 0)
		{
			// may cause multiple confirm box
			PAUIDIALOG pMsgBox;
			ACString szTxt;
			szTxt.Format(GetStringFromTable(5525), GetGameUIMan()->GetFormatNumber(DEFAULT_POST_DEPOSIT));
			m_pAUIManager->MessageBox("Game_WebTrade_Confirm", szTxt, MB_YESNO, A3DCOLORRGBA(255, 255, 255, 160), &pMsgBox);
			pMsgBox->SetDataPtr(&info);

			info.Confirm = -1;
			continue;
		}
		else if(info.Confirm > 0 && info.Confirm != IDYES)
		{
			m_Infos.RemoveAt(cur);
			continue;
		}

		// got id ready or need not to use id
		if(PrepostInfo::GETTING != info.BuyerRoleID)
		{
			m_Infos.RemoveAt(cur);
			
			if(MODE_ITEM == info.Mode)
			{
				GetGameSession()->webTrade_PrePostItem( info.Item.ID, info.Item.Slot, info.Item.Count, 
					info.Price, info.SellPeriod, info.BuyerRoleID);
			}
			else if(MODE_MONEY == info.Mode)
			{
				GetGameSession()->webTrade_PrePostMoney(info.Money.Count, 
					info.Price, info.SellPeriod, info.BuyerRoleID);
			}
			else if(MODE_ROLE == info.Mode)
			{
				GetGameSession()->webTrade_PrePostRole(info.Role.ID,
					info.Price, info.SellPeriod, info.BuyerRoleID);
			}
		}
	}
}

void CDlgWebMyShop::NotifyInvalidName(const ACString& strName, bool bObsoleteName)
{
	// only check the last one
	PrepostInfo& info = m_Infos.GetTail();
		
	if(PrepostInfo::NOBUYER != info.BuyerRoleID && info.BuyerName == strName)
	{
		m_Infos.RemoveAt(m_Infos.GetTailPosition());
		
		ACString szTxt;
		szTxt.Format(GetStringFromTable(bObsoleteName ? 10158 : 5516), strName);
		WEB_MYSHOP_MSGBOX(szTxt);
		
		EnableObjectsForSell(true);
	}
}

void CDlgWebMyShop::OnTick()
{
	CDlgWebTradeBase::OnTick();

	CheckPrepostList();

	if( !IsShow() )
	{
		return;
	}
	
	// valid the item on shop
	if( m_ItemSelected.Pos >= 0 )
	{
		CECInventory *pPack = GetHostPlayer()->GetPack();
		CECIvtrItem *pItem = pPack->GetItem(m_ItemSelected.Pos);
		if( !pItem || m_ItemSelected.pData != pItem )
		{
			ClearSelectData();
		}
	}
}

void CDlgWebMyShop::OnEventLButtonDown_Item_Prepost(WPARAM wParam, LPARAM lParam, AUIObject * pObj)
{
	if( m_ItemSelected.ID == 0 )
		return;
	
	ClearSelectData();
}

void CDlgWebMyShop::SetPrepostItem(CECIvtrItem *pItem, int nItemPos)
{
	if( !pItem->IsWebTradeable() )
	{
		WEB_MYSHOP_MSGBOX(GetStringFromTable(652));
		return;
	}
	else if( pItem->IsEquipment() && ((CECIvtrEquip *)pItem)->IsDestroying() )
	{
		WEB_MYSHOP_MSGBOX(GetStringFromTable(5501));
		return;
	}
	else if(!CheckCategoryKnown(pItem->GetTemplateID()))
	{
		WEB_MYSHOP_MSGBOX(GetStringFromTable(5501));
		return;
	}
	else if( pItem->GetExpireDate() != 0 )
	{
		WEB_MYSHOP_MSGBOX(GetStringFromTable(5506));
		return;
	}

	if( m_ItemSelected.ID != 0 )
	{
		OnEventLButtonDown_Item_Prepost(0, 0 , NULL);
	}

	AString strFile;
	af_GetFileTitle(pItem->GetIconFile(), strFile);
	strFile.MakeLower();
	m_ItemSelected.pImg->SetCover( GetGameUIMan()->m_pA2DSpriteIcons[CECGameUIMan::ICONS_INVENTORY],
								   GetGameUIMan()->m_IconMap[CECGameUIMan::ICONS_INVENTORY][strFile]);
	m_ItemSelected.pImg->SetColor( (pItem->IsEquipment() && ((CECIvtrEquip *)pItem)->IsDestroying())
									? A3DCOLORRGB(128, 128, 128) : A3DCOLORRGB(255, 255, 255));
	
	ACString strHint = GetGameUIMan()->GetItemDescHint(pItem, true, true);
	m_ItemSelected.pImg->SetHint(strHint);

	m_ItemSelected.Bind(pItem, nItemPos);
}

void CDlgWebMyShop::OnMessageBox(AUIDialog* pDlg, int iRetVal)
{
	if(!stricmp(pDlg->GetName(), "Game_WebTrade_Confirm"))
	{
		PrepostInfo* pInfo = (PrepostInfo*)pDlg->GetDataPtr();
		pInfo->Confirm = iRetVal;

		EnableObjectsForSell(true);

		if(iRetVal == IDYES)
		{
			OnCommandCancelDeal(NULL);
		}
	}
	else if(!stricmp(pDlg->GetName(), "Game_WebTrade_RoleSell"))
	{
		ShowObjectsForRole(false);

		if(iRetVal == IDYES)
			GetGameRun()->GetPendingLogOut().AppendForSaveConfig(new CECPendingSellingRole());
	}
}

void CDlgWebMyShop::OnCommandPrepostRole(const char* szCommand)
{
	if(CDlgWebTradeInfo::GetRoleCount() > MAX_SELL_COUNT)
	{
		ACString szTxt;
		szTxt.Format(GetStringFromTable(5562), CDlgWebTradeInfo::GetRoleCount());
		WEB_MYSHOP_MSGBOX(szTxt);
	}
	else if(GetHostPlayer()->GetMaxLevelSofar() < MIN_SELL_LEVEL )
	{
		ACString szTxt;
		szTxt.Format(GetStringFromTable(5561), MIN_SELL_LEVEL);
		WEB_MYSHOP_MSGBOX(szTxt);
	}
	else if(GetHostPlayer()->GetMoneyAmount() < DEFAULT_POST_DEPOSIT)
	{
		ACString szTxt;
		szTxt.Format(GetStringFromTable(5558), ((CECBaseUIMan*)m_pAUIManager)->GetFormatNumber(DEFAULT_POST_DEPOSIT));
		WEB_MYSHOP_MSGBOX(szTxt);
	}
	else
	{
		m_pAUIManager->MessageBox("Game_WebTrade_RoleSell", 
			GetStringFromTable(5557), MB_YESNO, A3DCOLORRGBA(255, 255, 255, 160));
		
		ShowObjectsForRole(true);
	}
}

void CDlgWebMyShop::ShowObjectsForRole(bool bShow)
{
	((PAUISTILLIMAGEBUTTON)GetDlgItem("Btn_SellRole"))->SetPushed(bShow);

	if(bShow)
	{
		((PAUISTILLIMAGEBUTTON)GetDlgItem("Btn_SellItems"))->SetPushed(false);
		((PAUISTILLIMAGEBUTTON)GetDlgItem("Btn_Currencysell"))->SetPushed(false);
	}
	else
	{
		if(MODE_ITEM == m_Mode)
		{
			OnCommandPrepostItem(NULL);
		}
		else if(MODE_MONEY == m_Mode)
		{
			OnCommandPrepostMoney(NULL);
		}
	}
}	

void CDlgWebMyShop::OnItemDragDrop(CECIvtrItem* pIvtrSrc, int iSrc, PAUIOBJECT pObjSrc, PAUIOBJECT pObjOver)
{
	// no need to check name "Item_Prepost", only one available drag-drop target
	this->SetPrepostItem(pIvtrSrc, iSrc);
}