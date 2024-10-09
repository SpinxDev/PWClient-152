// Filename	: DlgTouchShop.cpp
// Creator	: Han Guanghui
// Date		: 2013/3/30

#include "DlgTouchShop.h"
#include "DlgInputNO.h"
#include "AFI.h"
#include "EC_Configs.h"
#include "EC_Game.h"
#include "EC_GameRun.h"
#include "EC_GameSession.h"
#include "EC_Global.h"
#include "EC_Utility.h"
#include "EC_IvtrItem.h"
#include "EC_GameUIMan.h"
#include "EC_HostPlayer.h"
#include "AUIImagePicture.h"
#include "AUIStillImageButton.h"
#include "AUICTranslate.h"
#include "elementdataman.h"


AUI_BEGIN_COMMAND_MAP(CDlgTouchShop, CDlgBase)

AUI_ON_COMMAND("Btn_Touch",		OnCommandEnterTouch)
AUI_ON_COMMAND("btn_buy*",		OnCommandBuy)
AUI_ON_COMMAND("btn_pageup",	OnCommandPagePrevious)
AUI_ON_COMMAND("btn_pagedown",	OnCommandPageNext)
AUI_ON_COMMAND("btn_page*",		OnCommandMainType)

AUI_END_COMMAND_MAP()

static int PageCount = 0;
static int IndexCount = 1;

CDlgTouchShop::CDlgTouchShop():
m_pLabCurrentPoint(NULL),
m_nCurrentPage(0),
m_iConfirmBuyItemIndex(-1),
m_pObjBuyButton(NULL)
{

}

CDlgTouchShop::~CDlgTouchShop()
{

}

bool CDlgTouchShop::OnInitDialog()
{
	char szTemp[50];
	int page(1);
	m_PageIndex.clear();
	m_BuyingItemIndex.clear();
	while (true) {
		sprintf(szTemp, "btn_page%d", page);
		PAUISTILLIMAGEBUTTON pBtn = dynamic_cast<PAUISTILLIMAGEBUTTON>(GetDlgItem(szTemp));
		if (pBtn) {
			m_PageIndex.push_back(0);
			if (page == 1) {
				m_nCurrentPage = 0;
				pBtn->SetPushed(true);
			}
			page++;
		} else break; 
	}
	m_nPageItemCount = 1;
	while (true) {
		sprintf(szTemp, "txt_name%02d", m_nPageItemCount);
		if (GetDlgItem(szTemp)) {
			m_nPageItemCount++;
		} else {
			m_nPageItemCount--;	
			break; 
		}
	}

	DDX_Control("Txt_touchpoint", m_pLabCurrentPoint);
	m_pLabCurrentPoint->SetText(GetStringFromTable(10445));
	return true;
}

void CDlgTouchShop::OnShowDialog()
{
	QueryTouchPoint();
	UpdateItemDisplay();
}

void CDlgTouchShop::OnCommandBuy(const char* szCommand)
{
	m_pObjBuyButton = GetDlgItem(szCommand);
	if (m_pObjBuyButton == NULL) return; 

	m_iConfirmBuyItemIndex = 0;
	sscanf(szCommand, "btn_buy%02d", &m_iConfirmBuyItemIndex);
	m_iConfirmBuyItemIndex--;

	ShopItemParam param;
	unsigned short* title = NULL;
	int nMax(0);
	if ((unsigned int)m_nCurrentPage < m_PageIndex.size()){
		GetShopItemParam(m_nCurrentPage, m_PageIndex[m_nCurrentPage] + m_iConfirmBuyItemIndex, param, title);
		if (param.id != 0 && param.price != 0) {
			nMax = GetHostPlayer()->GetTouchPoint() / param.price;
		}
	}
	GetGameUIMan()->InvokeNumberDialog(NULL, NULL, CDlgInputNO::INPUTNO_TOUCHSHOP_ITEM_NUM, nMax);
	
}

void CDlgTouchShop::OnBuyShopItem(int lots)
{
	ShopItemParam param;
	unsigned short* title = NULL;
	if ((unsigned int)m_nCurrentPage < m_PageIndex.size()){
		GetShopItemParam(m_nCurrentPage, m_PageIndex[m_nCurrentPage] + m_iConfirmBuyItemIndex, param, title);
		if (param.id != 0) {	
			BuyItemUsingTouchPoint(m_nCurrentPage, m_PageIndex[m_nCurrentPage] + m_iConfirmBuyItemIndex, param.id, param.num, param.price, param.expire_timelength, lots);
			if (m_pObjBuyButton)((PAUIOBJECT)m_pObjBuyButton)->Enable(false);
		}
	}
}

void CDlgTouchShop::OnCommandEnterTouch(const char* szCommand)
{
	GetGameUIMan()->SSOGetTouchTicket();
}

void CDlgTouchShop::OnCommandPagePrevious(const char* szCommand)
{
	if ((unsigned int)m_nCurrentPage < m_PageIndex.size())
		m_PageIndex[m_nCurrentPage] -= m_nPageItemCount;
	UpdatePage();
}

void CDlgTouchShop::OnCommandPageNext(const char* szCommand)
{
	if ((unsigned int)m_nCurrentPage < m_PageIndex.size())
		m_PageIndex[m_nCurrentPage] += m_nPageItemCount;
	UpdatePage();
}

void CDlgTouchShop::OnCommandMainType(const char* szCommand)
{
	unsigned int page = 0;
	sscanf(szCommand, "btn_page%u", &page);
	page--;
	if (page < m_PageIndex.size()) {
		m_nCurrentPage = page;
		UpdatePage();
		// 设置按钮pushed
		int page_num = m_PageIndex.size();
		char szTemp[50];
		for (int page = 0; page < page_num; ++page)	{
			sprintf(szTemp, "btn_page%d", page + 1);
			PAUISTILLIMAGEBUTTON pBtn = dynamic_cast<PAUISTILLIMAGEBUTTON>(GetDlgItem(szTemp));
			if (pBtn) pBtn->SetPushed(page == m_nCurrentPage);
		}
	}
}

void CDlgTouchShop::UpdateItemDisplay()
{
	ShopItemParam param;
	memset(&param, 0, sizeof(param));
	unsigned short* title = NULL;
	int page_num = m_PageIndex.size();
	char szTemp[50];
	for (int page = 0; page < page_num; ++page)	{
		GetShopItemParam(page, 0, param, title);
		sprintf(szTemp, "btn_page%d", page + 1);
		PAUIOBJECT pObj = GetDlgItem(szTemp);
		if (pObj == NULL) continue;
		// 第一个物品id为0，说明该页为空
		if (param.id == 0) pObj->Show(false);
		// 设置分页名字
		else if (title != NULL){
			// 配置里没填的话就不设置
			if (title[0]) pObj->SetText(title);
		} 
	}
	UpdatePage();
}

void CDlgTouchShop::UpdatePage()
{
	if ((unsigned int)m_nCurrentPage < m_PageIndex.size()) {
		ShopItemParam param;
		memset(&param, 0, sizeof(param));
		unsigned short* title = NULL;
		// 设置item信息
		for (int index = 0; index < m_nPageItemCount; ++index) {
			GetShopItemParam(m_nCurrentPage, m_PageIndex[m_nCurrentPage] + index, param, title);
			SetOneItem(index, param);
		}
		// 设置翻页按钮
		GetShopItemParam(m_nCurrentPage, m_PageIndex[m_nCurrentPage] + m_nPageItemCount, param, title);
		PAUIOBJECT pObj = GetDlgItem("btn_pagedown");
		if (pObj) pObj->Enable(param.id != 0);
		GetShopItemParam(m_nCurrentPage, m_PageIndex[m_nCurrentPage] - 1, param, title);
		pObj = GetDlgItem("btn_pageup");
		if (pObj) pObj->Enable(param.id != 0);
	}
}

void CDlgTouchShop::SetOneItem(int index, const ShopItemParam& param)
{
	char szTemp[50];
	AString strFile;
	ACString strTemp;
	// 物品图标
	sprintf(szTemp, "SellItem_%02d", index + 1);
	PAUIIMAGEPICTURE pImg = dynamic_cast<PAUIIMAGEPICTURE>(GetDlgItem(szTemp));
	// 物品名称+数量
	sprintf(szTemp, "txt_name%02d", index + 1);
	PAUIOBJECT pName = GetDlgItem(szTemp);
	// 价格
	sprintf(szTemp, "price%02d", index + 1);
	PAUIOBJECT pPrice = GetDlgItem(szTemp);
	// 物品时限
	sprintf(szTemp, "txt_time%02d", index + 1);
	PAUIOBJECT pTime = GetDlgItem(szTemp);
	// 购买按钮
	sprintf(szTemp, "btn_buy%02d", index + 1);
	PAUIOBJECT pBtn = GetDlgItem(szTemp);

	if (param.id) {
		CECIvtrItem* pItem = CECIvtrItem::CreateItem(param.id, 0, 1);
		if (pItem) {
			if (pImg) {
				GetGameUIMan()->SetCover(pImg, pItem->GetIconFile());
				pItem->GetDetailDataFromLocal();
				const wchar_t *pszHint = pItem->GetDesc();
				AWString strHint;
				if( pszHint) {
					AUICTranslate trans;
					strHint = trans.Translate(pszHint);
				}
				pImg->SetHint(strHint);
			}
			if (pName) {
				if (param.num > 1) strTemp.Format(GetStringFromTable(10440), pItem->GetName(), param.num);
				else strTemp = pItem->GetName();
				pName->SetText(strTemp);
			}
			if (pPrice) {
				strTemp.Format(_AL("%d"), param.price);
				pPrice->SetText(strTemp);
			}
			strTemp = param.expire_timelength ? GetStringFromTable(10441) : GetStringFromTable(1600);
			if (pTime) pTime->SetText(strTemp);

			if (pBtn){ 
				pBtn->SetData(param.id);
				pBtn->Enable(!IsBuyingItem(m_nCurrentPage, m_PageIndex[m_nCurrentPage] + index) 
					&& GetHostPlayer()->GetTouchPoint() >= param.price);
			}
			delete pItem;
		}
	} else {
		if (pImg) {
			pImg->SetCover(NULL, -1);
			pImg->SetHint(_AL(""));
		}
		if (pName)
			pName->SetText(_AL(""));
		if (pPrice)
			pPrice->SetText(_AL(""));
		if (pTime)
			pTime->SetText(_AL(""));
		if (pBtn) {
			pBtn->SetData(0);
			pBtn->Enable(false);
		}
	}

}


void CDlgTouchShop::GetShopItemParam(int page, int index, ShopItemParam& param, unsigned short*& shop_title)
{
	static bool bConfigLoaded(false);
	static TOUCH_SHOP_CONFIG* pConfig = NULL;

	if (!bConfigLoaded) {
		bConfigLoaded = true;
		elementdataman *pDataMan = g_pGame->GetElementDataMan();
		DATA_TYPE DataType;
		unsigned int tid = pDataMan->get_first_data_id(ID_SPACE_CONFIG,DataType);
		
		while(tid) {
			if(DataType == DT_TOUCH_SHOP_CONFIG) {
				pConfig = (TOUCH_SHOP_CONFIG* )pDataMan->get_data_ptr(tid,ID_SPACE_CONFIG, DataType);
				if (pConfig) {
					PageCount = sizeof(pConfig->page) / sizeof(TOUCH_SHOP_CONFIG::Page);
					IndexCount = sizeof(pConfig->page[0].item) / sizeof(TOUCH_SHOP_CONFIG::Page::Item);
				}
				break;
			}
			tid = pDataMan->get_next_data_id(ID_SPACE_CONFIG, DataType);
		}
	}
	if (pConfig && page >= 0 && page < PageCount && index >= 0 && index < IndexCount) {
		param.id = pConfig->page[page].item[index].id;
		param.num = pConfig->page[page].item[index].num;
		param.price = pConfig->page[page].item[index].price;
		param.expire_timelength = pConfig->page[page].item[index].expire_timelength;
		shop_title = pConfig->page[page].title;
	} else {
		param.id = 0;
		param.num = 0;
		param.price = 0;
		param.expire_timelength = 0;
		shop_title = NULL;
	}
}

void CDlgTouchShop::QueryTouchPoint()
{
	GetGameSession()->c2s_CmdQueryTouchPoint();
}

void CDlgTouchShop::BuyItemUsingTouchPoint(int page, int index, unsigned int id, unsigned int num, unsigned int price, int expire_time, unsigned int lots /* = 1 */)
{
	int index_server = IndexCount * page + index;
	m_BuyingItemIndex.push_back(index_server);
	GetGameSession()->c2s_CmdSpendTouchPoint(index_server, lots, id, num, price, expire_time);
}

void CDlgTouchShop::SetCurrentPointText(__int64 remain)
{
	ACString strTemp;
	strTemp.Format(_AL("%I64d"), remain);
	m_pLabCurrentPoint->SetText(strTemp);
	GetHostPlayer()->SetTouchPoint(remain);
}
void CDlgTouchShop::OnQueryTouchPointRe(__int64 income, __int64 remain, int retcode)
{
	if (retcode == 1) 
		GetGameUIMan()->MessageBox("Busy", GetStringFromTable(10442), MB_OK, A3DCOLORRGBA(255, 255, 255, 160));
	else if (retcode == 0) {
		SetCurrentPointText(remain);
		UpdatePage();
	}
}

void CDlgTouchShop::OnSpendTouchPointRe(__int64 income, __int64 remain, unsigned int cost, unsigned int index, unsigned int lots, int retcode)
{
	if (0 == retcode) SetCurrentPointText(remain);
	else if (5 == retcode) 
		GetGameUIMan()->MessageBox("Busy", GetStringFromTable(10442), MB_OK, A3DCOLORRGBA(255, 255, 255, 160));
	else if (128 == retcode)
		GetGameUIMan()->MessageBox("InventoryFull", GetStringFromTable(10443), MB_OK, A3DCOLORRGBA(255, 255, 255, 160));
	int page = index / IndexCount + 1;
	int index_client = index % IndexCount;
	std::vector<int> buying_item_remain;
	int len = m_BuyingItemIndex.size();
	for (int i = 0; i < len; i++)
	{
		if ((int)index != m_BuyingItemIndex[i])
			buying_item_remain.push_back(m_BuyingItemIndex[i]);
	}
	m_BuyingItemIndex.swap(buying_item_remain);
	UpdatePage();
}

void CDlgTouchShop::OnTotalRecharge(__int64 recharge)
{
	PAUIOBJECT pObj = GetDlgItem("txt_5");
	if(pObj) {
		ACString strText;
		int r_momey = GetRMoneyByRecharge(recharge);
		if (r_momey) strText.Format(GetStringFromTable(10444), r_momey);
		pObj->SetText(strText);
	}
}
bool CDlgTouchShop::IsBuyingItem(int page, int index)
{
	int index_server = IndexCount * page + index;
	return std::find(m_BuyingItemIndex.begin(), m_BuyingItemIndex.end(), index_server) != m_BuyingItemIndex.end();
}

int CDlgTouchShop::GetRMoneyByRecharge(__int64 recharge)
{
	int ret(0);
	if (recharge >= 10000 && recharge < 30000) ret = 5000;
	else if (recharge >= 30000 && recharge < 50000) ret = 15000;
	else if (recharge >= 50000 && recharge < 100000) ret = 20000;
	else if (recharge >= 100000 && recharge < 200000) ret = 30000;
	else if (recharge >= 200000 && recharge < 500000) ret = 60000;
	else if (recharge >= 500000 && recharge < 1000000) ret = 100000;
	else if (recharge >= 1000000) ret = 200000;
	return ret;
}