// Filename	: DlgQShopBuy.cpp
// Creator	: Han Guanghui
// Date		: 2013/11/04

#include "DlgQShopBuy.h"
#include "AUIImagepicture.h"
#include "AUICTranslate.h"
#include "AUIDef.h"
#include "elementdataman.h"
#include "EC_Global.h"
#include "EC_GameUIMan.h"
#include "EC_GameSession.h"
#include "EC_HostPlayer.h"
#include "EC_IvtrItem.h"
#include "EC_Inventory.h"
#include "EC_UIConfigs.h"
#include "EC_Shop.h"
#include "EC_ShopSearch.h"
#include "globaldataman.h"

#define new A_DEBUG_NEW

AUI_BEGIN_COMMAND_MAP(CDlgQShopBuy, CDlgBase)
AUI_ON_COMMAND("Btn_Buy",					OnCommandConfirm)
AUI_ON_COMMAND("IDCANCEL",					OnCommandCancel)
AUI_ON_COMMAND("Btn_Cancel",				OnCommandCancel)
AUI_ON_COMMAND("Btn_Max",					OnCommandMaximum)
AUI_ON_COMMAND("Btn_Add",					OnCommandAdd)
AUI_ON_COMMAND("Btn_Minus",					OnCommandMinus)
AUI_END_COMMAND_MAP()

AUI_BEGIN_EVENT_MAP(CDlgQShopBuy, CDlgBase)
AUI_ON_EVENT("*",			WM_MOUSEWHEEL,	OnEventMouseWheel)
AUI_END_EVENT_MAP()

CDlgQShopBuy::CDlgQShopBuy():
m_pShop(NULL),
m_ItemIndex(-1),
m_BuyIndex(-1),
m_SaleItemIndex(-1),
m_SaleBuyIndex(-1),
m_bBuying(false),
m_BuyItemCount(0),
m_BuySaleItemCount(0),
m_TxtInputBuyCount(NULL),
m_MaxItemCanBuy(0)
{
}
CDlgQShopBuy::~CDlgQShopBuy()
{
}

bool CDlgQShopBuy::OnInitDialog()
{
	DDX_Control("Txt_Number", m_TxtInputBuyCount);
	SetCanEscClose(true);
	return true;
}
void CDlgQShopBuy::OnCommandConfirm(const char* szCommand)
{
	int max_count = GetMaximumNumCanBuy();
	if (GetInputBuyCount() > max_count)
		SetInputBuyCount(max_count);
	if (CalcuateBuyItemCount()) {
		m_bBuying = true;
		BatchBuy();
	}
}

void CDlgQShopBuy::Clear()
{
	m_pShop = NULL;
	m_ItemIndex = -1;
	m_BuyIndex = -1;	
	m_SaleItemIndex = -1;
	m_SaleBuyIndex = -1;
	m_bBuying = false;
	m_BuyItemCount = 0;
	m_BuySaleItemCount = 0;
	m_MaxItemCanBuy = 0;
}

void CDlgQShopBuy::OnCommandCancel(const char *szCommand)
{
	Clear();
	Show(false);
}

void CDlgQShopBuy::SetBuyingItem(int itemindex, int buyindex, CECShopBase *pShop)
{
	if (!pShop || !pShop->IsValid(itemindex, buyindex)){
		return;
	}
	m_pShop = pShop;
	CECGameUIMan *pGameUIMan = GetGameUIMan();
	const GSHOP_ITEM& gshop_item = *(pShop->GetItem(itemindex));
	CECIvtrItem *pItem = CECIvtrItem::CreateItem(gshop_item.id, 0, gshop_item.num);
	while(true) {
		PAUIOBJECT pObj(NULL);
		PAUIIMAGEPICTURE pImg(NULL);
		ACString strTemp;
		AString strFile;
		AUICTranslate trans;

		pObj = GetDlgItem("Txt_ItemName");
		if (!pObj) break;
		pObj->SetText(pItem->GetName());
		
		pObj = GetDlgItem("Txt_Price");
		if (!pObj) break;
		int time_count = ARRAY_SIZE(gshop_item.buy);
		if (buyindex < 0 || buyindex >= time_count) break;
		int nPrice = gshop_item.buy[buyindex].price;
		int status = gshop_item.buy[buyindex].status;
		if (status>=4 && status<=12) {
			// 打折
			strTemp.Format(GetStringFromTable(8590),
				pGameUIMan->GetCashText(nPrice),
				pGameUIMan->GetCashText(CECShopBase::GetOriginalPrice(nPrice, status)));
		} else {
			// 不打折
			strTemp = pGameUIMan->GetCashText(nPrice);
		}
		pObj->SetText(strTemp);
		
		pImg = dynamic_cast<PAUIIMAGEPICTURE>(GetDlgItem("Img_Item"));
		if (!pImg) break;
		AUIOBJECT_SETPROPERTY p;
		p.fn[0] = 0;
		pItem->GetDetailDataFromLocal();
		pItem->SetPriceScale(CECIvtrItem::SCALE_BUY, 0.0f);
		pImg->SetHint(trans.Translate(pItem->GetDesc()));
		strcpy(p.fn, gshop_item.icon + strlen("Surfaces\\"));
		m_ItemIndex = itemindex;
		m_BuyIndex = buyindex;
		{
			ScopedAUIControlSpriteModify _dummy(pImg);
			pImg->SetProperty("Image File", &p);
		}
		SearchForSale();
		
		pObj = GetDlgItem("Txt_Discount");
		if (!pObj) break;
		if (m_SaleItemIndex != -1 && m_SaleBuyIndex != -1){
			const GSHOP_ITEM& gshop_saleitem = *(m_pShop->GetItem(m_SaleItemIndex));
			nPrice = gshop_saleitem.buy[m_SaleBuyIndex].price;
			if (nPrice > 0) {
				strTemp.Format(GetStringFromTable(8591), 
					pItem->GetName(), gshop_saleitem.num, pGameUIMan->GetCashText(nPrice));
				pObj->SetText(strTemp);
			}
		} else pObj->SetText(_AL(""));
		
		pObj = GetDlgItem("Txt_TimeLimit");
		if (pObj) {
			int type = gshop_item.buy[buyindex].type;
			int time = gshop_item.buy[buyindex].time;
			if (type == -1 || type == 3) pObj->SetText(pGameUIMan->GetShopItemFormatTime(time));
			else pObj->SetText(_AL(""));
		} else break;
		SetInputBuyCount(1);
		ChangeFocus(m_TxtInputBuyCount);
		break;
	}
	delete pItem;
}
void CDlgQShopBuy::SearchForSale()
{
	m_SaleItemIndex = -1;
	m_SaleBuyIndex = -1;
	if (!m_pShop || !m_pShop->GetItem(m_ItemIndex)){
		return;
	}
	const GSHOP_ITEM& gshop_item = *(m_pShop->GetItem(m_ItemIndex));
	// 如果购买物品有赠品则不搜索优惠
	if (gshop_item.idGift) return;
	int time_count = ARRAY_SIZE(gshop_item.buy);
	if (m_BuyIndex < 0 || m_BuyIndex >= time_count) return;
	if (gshop_item.num == 0) return;
	int tid = gshop_item.id;
	int time = gshop_item.buy[m_BuyIndex].time;
	int type = gshop_item.buy[m_BuyIndex].type;
	int price = gshop_item.buy[m_BuyIndex].price / gshop_item.num;
	int i,j;
	for (i = 0; i < m_pShop->GetCount(); ++i) {
		if (i == m_ItemIndex){
			continue;
		}
		const GSHOP_ITEM &item = *(m_pShop->GetItem(i));
		if (!m_pShop->MatchOwnerNPC(item)){
			continue;
		}
		for (j = 0; j < time_count; ++j){
			if (tid == (int)item.id 
				&& item.buy[j].price > 0
				&& item.num > 0
				&& time == (int)item.buy[j].time
				&& type == (int)item.buy[j].type
				&& price > (int)(item.buy[j].price / item.num)) {
				price = item.buy[j].price;
				m_SaleItemIndex = i;
				m_SaleBuyIndex = j;
			}
		}
	}
}
void CDlgQShopBuy::BatchBuy()
{
	if (m_bBuying && m_pShop) {
		GetMaximumNumCanBuy();
		SetBtnBuyEnable(false);
		bool bItemBuyed(false);
		if (m_BuySaleItemCount && m_SaleBuyIndex != -1 && m_SaleItemIndex != -1) {
			if (CheckBuyedItem()){
				if (m_pShop->Buy(m_SaleItemIndex, m_SaleBuyIndex)){
					bItemBuyed = true;
				}
			}
			m_BuySaleItemCount--;
		} else if (m_BuyItemCount && m_ItemIndex != -1 && m_BuyIndex != -1) {
			if (CheckBuyedItem()){
				if (m_pShop->Buy(m_ItemIndex, m_BuyIndex)){
					bItemBuyed = true;
				}
			}
			m_BuyItemCount--;
		}
		if (!bItemBuyed){
			OnCommandCancel(NULL);
			SetBtnBuyEnable(true);
		}
	}
}
void CDlgQShopBuy::OnCommandAdd(const char* szCommand)
{
	SetInputBuyCount(GetInputBuyCount() + 1);
}
void CDlgQShopBuy::OnCommandMinus(const char* szCommand)
{
	SetInputBuyCount(GetInputBuyCount() - 1);
}
void CDlgQShopBuy::OnCommandMaximum(const char* szCommand)
{
	SetInputBuyCount(GetMaximumNumCanBuy());
}
int CDlgQShopBuy::GetMaximumNumCanBuy()
{
	m_MaxItemCanBuy = 0;
	if (!m_pShop){
		return m_MaxItemCanBuy;
	}
	int money_canbuy(0), inventory_canput(0);
	int player_cash = m_pShop->GetCash();
	int price(0);
	int tid[2] = {0};
	int num[2] = {0};
	bool has_sale_item = m_SaleItemIndex != -1 && m_SaleBuyIndex != -1;
	// 有优惠的时候
	if (has_sale_item){
		const GSHOP_ITEM& gshop_saleitem = *(m_pShop->GetItem(m_SaleItemIndex));
		price = gshop_saleitem.buy[m_SaleBuyIndex].price;
		tid[0] = gshop_saleitem.id;
		num[0] = gshop_saleitem.num;
		tid[1] = gshop_saleitem.idGift;
		num[1] = gshop_saleitem.iGiftNum;
		if (price) {
			money_canbuy = player_cash / price;
			player_cash -= money_canbuy * price;
		}
	}
	if (m_ItemIndex != -1 && m_BuyIndex != -1 && m_pShop->GetItem(m_ItemIndex)->num) {
		const GSHOP_ITEM& gshop_item = *(m_pShop->GetItem(m_ItemIndex));
		price = gshop_item.buy[m_BuyIndex].price;
		// 没有优惠的时候
		if (!has_sale_item) {
			tid[0] = gshop_item.id;
			num[0] = gshop_item.num;
			tid[1] = gshop_item.idGift;
			num[1] = gshop_item.iGiftNum;
		}
		if (money_canbuy) money_canbuy = money_canbuy * 
			m_pShop->GetItem(m_SaleItemIndex)->num / gshop_item.num;
		if (price) money_canbuy += player_cash / price;
	} else{
		return m_MaxItemCanBuy;
	}

	CECHostPlayer* pHost = GetHostPlayer();
	if (pHost) {
		CECInventory* pInv = pHost->GetPack();
		if (tid[1] && num[1]) inventory_canput = pInv->GetMaxItemNumCanAdd(tid, num, 2);
		else inventory_canput = pInv->GetMaxItemNumCanAdd(tid, num, 1);
	}

	if (has_sale_item) 
		inventory_canput = inventory_canput * m_pShop->GetItem(m_SaleItemIndex)->num / m_pShop->GetItem(m_ItemIndex)->num;
	
	if (CECQShopConfig::Instance().CanFilterID(tid[0])) {
		bool task_flag = CheckBuyedItem();
		m_MaxItemCanBuy = task_flag ? a_Min(1, inventory_canput, money_canbuy) : 0;
	} else m_MaxItemCanBuy = a_Min(inventory_canput, money_canbuy);
	return m_MaxItemCanBuy;
}

int CDlgQShopBuy::CalculateMoneyNeeded()
{
	int ret(0);
	if (CalcuateBuyItemCount()) {
		if (m_BuySaleItemCount) ret += m_pShop->GetPrice(m_SaleItemIndex, m_SaleBuyIndex) * m_BuySaleItemCount;
		if (m_BuyItemCount) ret += m_pShop->GetPrice(m_ItemIndex, m_BuyIndex) * m_BuyItemCount;
	}
	return ret;
}
bool CDlgQShopBuy::CalcuateBuyItemCount()
{
	m_BuyItemCount = 0;
	m_BuySaleItemCount = 0;
	if (!m_pShop){
		return false;
	}
	if (!m_pShop->IsValid(m_ItemIndex, m_BuyIndex)){
		return false;
	}
	const GSHOP_ITEM& gshop_item = *(m_pShop->GetItem(m_ItemIndex));
	ACString strCount = m_TxtInputBuyCount->GetText();
	int buy_count = strCount.ToInt();
	int item_num = gshop_item.num * buy_count;
	if (m_SaleItemIndex != -1 && m_pShop->GetItem(m_SaleItemIndex)->num && gshop_item.num) {
		m_BuySaleItemCount = item_num / m_pShop->GetItem(m_SaleItemIndex)->num;
		m_BuyItemCount = (item_num % m_pShop->GetItem(m_SaleItemIndex)->num) / gshop_item.num;
	} else m_BuyItemCount = buy_count;
	return true;
}
void CDlgQShopBuy::SetTotalMoneyNeededText()
{
	PAUIOBJECT pObj = GetDlgItem("Txt_Total");
	if (pObj) {
		pObj->SetText(GetGameUIMan()->GetCashText(CalculateMoneyNeeded()));
	}
}
int CDlgQShopBuy::GetInputBuyCount()
{
	int ret(0);
	ACString strCount = m_TxtInputBuyCount->GetText();
	ret = strCount.ToInt();
	return ret;
}
void CDlgQShopBuy::SetInputBuyCount(int count)
{
	ACString strCount;
	int max_count = GetMaximumNumCanBuy();
	if (count <= max_count && count >= 0) 
		strCount.Format(_AL("%d"), count);
	else if (count < 0)
		strCount.Format(_AL("%d"), 0);
	else 
		strCount.Format(_AL("%d"), max_count);
	m_TxtInputBuyCount->SetText(strCount);
	SetTotalMoneyNeededText();
	SetBtnBuyEnable();
}
void CDlgQShopBuy::SetBtnBuyEnable(bool hint)
{
	PAUIOBJECT pObj = GetDlgItem("Btn_Buy");
	if (pObj && m_pShop) {
		int input_count = GetInputBuyCount();
		int max_count = m_MaxItemCanBuy;
		bool task_check = CheckBuyedItem();
		bool enable = input_count > 0 
			&& input_count <= max_count
			&& task_check;
		pObj->Enable(enable);
		int current = GetInputBuyCount();
		int str_num = current >  max_count ? 8592 : 8593;
		pObj->SetHint(enable || !hint || !task_check ? NULL : GetStringFromTable(str_num));
	}
}
void CDlgQShopBuy::OnEventMouseWheel(WPARAM wParam, LPARAM lParam, AUIObject *pObj)
{
	int zDelta = (short)HIWORD(wParam);
	if (zDelta >= 0) OnCommandAdd(NULL);
	else OnCommandMinus(NULL);
}
void CDlgQShopBuy::OnTick()
{
	if (GetFocus() == m_TxtInputBuyCount) {
		SetTotalMoneyNeededText();
	}
	int current = GetInputBuyCount();
	if (!m_bBuying) SetBtnBuyEnable();
}
void CDlgQShopBuy::OnChangeLayoutEnd(bool bAllDone)
{
	int input = GetInputBuyCount();
	SetBuyingItem(m_ItemIndex, m_BuyIndex, m_pShop);
	SetInputBuyCount(input);
}

bool CDlgQShopBuy::CheckBuyedItem(){
	if (m_pShop && m_pShop->GetItem(m_ItemIndex)){
		CECShopSearchTaskPolicy taskPolicy;
		return taskPolicy.CanAccept(m_pShop, m_ItemIndex);
	}
	return false;
}