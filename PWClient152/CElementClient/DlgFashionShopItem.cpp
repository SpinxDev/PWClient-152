// Filename	: DlgFashionShopItem.cpp
// Creator	: Xu Wenbin
// Date		: 2014/5/23

#include "DlgFashionShopItem.h"
#include "DlgFashionShop.h"
#include "DlgGivingFor.h"
#include "DlgQShopBuy.h"
#include "EC_GameUIMan.h"
#include "EC_UIHelper.h"

#include "EC_FashionShop.h"
#include "EC_ShoppingCart.h"
#include "EC_Shop.h"
#include "EC_UIConfigs.h"
#include "EC_IvtrItem.h"
#include "EC_ShoppingManager.h"
#include "EC_ShoppingController.h"

#include "EC_Game.h"
#include "EC_Configs.h"

#include "globaldataman.h"

#include <AUIDef.h>
#include <AUIStillImageButton.h>
#include <AUIImagePicture.h>
#include <AFI.h>

#include <algorithm>

//	前置声明
extern CECGame * g_pGame;

//	class CDlgFashionShopItem
AUI_BEGIN_COMMAND_MAP(CDlgFashionShopItem, CDlgBase)
AUI_ON_COMMAND("Btn_Buy",	OnCommand_Buy)
AUI_ON_COMMAND("Btn_AddToShopCart",	OnCommand_AddToShopCart)
AUI_ON_COMMAND("Btn_Time*",	OnCommand_Time)
AUI_ON_COMMAND("IDCANCEL",	OnCommand_CANCEL)
AUI_ON_COMMAND("Btn_Giving",OnCommand_GivePresent)
AUI_ON_COMMAND("Btn_For",	OnCommand_AskForPresent)
AUI_END_COMMAND_MAP()

AUI_BEGIN_EVENT_MAP(CDlgFashionShopItem, CDlgBase)
AUI_ON_EVENT("*",		WM_MOUSEWHEEL,		OnEventMouseWheel)
AUI_ON_EVENT(NULL,		WM_MOUSEWHEEL,		OnEventMouseWheel)
AUI_ON_EVENT("*",		WM_LBUTTONDOWN,		OnEventLButtonDown)
AUI_ON_EVENT(NULL,		WM_LBUTTONDOWN,		OnEventLButtonDown)
AUI_END_EVENT_MAP()

CDlgFashionShopItem::CDlgFashionShopItem()
: m_pPositionController(NULL)
, m_pFashionShop(NULL)
, m_fashionSelectionIndex(-1)
, m_pShoppingCart(NULL)
, m_nTimeSelect(0)
{
	for (int i = 0; i < BUY_TYPE_COUNT; i++){
		m_BuyType[i] = -1;
	}
}

bool CDlgFashionShopItem::OnInitDialog(){
	if (CDlgBase::OnInitDialog()){
		m_pTxt_ItemName = GetDlgItem("Txt_ItemName");
		m_pTxt_Price = GetDlgItem("Txt_Price");
		m_pTxt_NoDiscountPrice = GetDlgItem("Txt_NoDiscountPrice");
		m_pBtn_Buy = (PAUISTILLIMAGEBUTTON)GetDlgItem("Btn_Buy");
		m_pBtn_AddToShopCart = (PAUISTILLIMAGEBUTTON)GetDlgItem("Btn_AddToShopCart");
		m_pImg_Item = (PAUIIMAGEPICTURE)GetDlgItem("Img_Goods");
		m_pImg_Bg01 = (PAUIIMAGEPICTURE)GetDlgItem("Img_Bg01");
		m_pImg_Bg02 = (PAUIIMAGEPICTURE)GetDlgItem("Img_Bg02");
		m_pImg_Bg03 = (PAUIIMAGEPICTURE)GetDlgItem("Img_Bg03");
		m_pImg_Bg04 = (PAUIIMAGEPICTURE)GetDlgItem("Img_Bg04");
		m_pBtn_GivePresent = (PAUISTILLIMAGEBUTTON)GetDlgItem("Btn_Giving");
		m_pBtn_AskForPresent = (PAUISTILLIMAGEBUTTON)GetDlgItem("Btn_For");		
		for (int i = 0; i < TIME_TYPE_COUNT; i++){
			if (i > 0 && !m_pBtn_Time[i-1]){
				m_pBtn_Time[i] = NULL;
			}else{
				char szName[30];
				sprintf(szName, "Btn_Time%d", i + 1);
				m_pBtn_Time[i] = (PAUISTILLIMAGEBUTTON)GetDlgItem(szName);
			}
		}

		m_pPositionController = NULL;
		SetCanMove(false);		
		return true;
	}
	return false;
}

bool CDlgFashionShopItem::Render(){
	if (m_pPositionController){
		POINT pt = m_pPositionController->GetPos();
		SetPosEx(pt.x, pt.y);
	}
	return CDlgBase::Render();
}

void CDlgFashionShopItem::OnTick()
{
	CDlgBase::OnTick();
	if (m_pBtn_Buy){
		m_pBtn_Buy->Enable(CanBuyDirectly());
	}
	if (m_pBtn_AddToShopCart){
		m_pBtn_AddToShopCart->Enable(CanBuy());
	}
	if(m_pBtn_GivePresent){
		m_pBtn_GivePresent->Enable(CanGivePresent());
	}
	if(m_pBtn_AskForPresent){
		m_pBtn_AskForPresent->Enable(CanAskForPresent());
	}
}

bool CDlgFashionShopItem::CanBuy(){
	if (m_pFashionShop &&
		m_pFashionShop->ItemAt(m_fashionSelectionIndex) &&
		m_pShoppingCart){
		CECShoppingController *pShoppingController = CECShoppingManager::Instance().ShoppingControllerFor(m_pShoppingCart);
		if (!pShoppingController->IsBuying()){
			return CalculatePrice() <= m_pFashionShop->Shop()->GetCash();	//	金钱计算放在最后，计算量较大
		}
	}
	return false;
}

bool CDlgFashionShopItem::CanBuyDirectly(){
	return !IsFashionSuite() && CanBuy();
}

bool CDlgFashionShopItem::CanGivingFor()
{
	//	是否可显示赠送、索取
	bool bCan(false);
	if (CECUIConfig::Instance().GetGameUI().bEnableGivingFor &&
		m_pFashionShop &&
		m_pFashionShop->Shop() == &CECQShop::Instance()	&&	//	仅乾坤袋可赠送和索取
		m_pFashionShop->ItemAt(m_fashionSelectionIndex) &&
		!m_pFashionShop->IsFashionTypeSuite()){				//	套装件数较多，赠送/索取逻辑暂不支持
		bCan = true;
	}
	return bCan;
}

bool CDlgFashionShopItem::CanGivePresent()
{
	//	能否赠送给他人
	return CanGivingFor() && CanBuy();
}
	
bool CDlgFashionShopItem::CanAskForPresent()
{
	//	能否向他人索取
	return CanGivingFor();
}

void CDlgFashionShopItem::OnCommand_GivePresent(const char *szCommand)
{
	if (CanGivePresent()){
		CDlgGivingFor*pDlg = dynamic_cast<CDlgGivingFor*>(GetGameUIMan()->GetDialog("Win_GivingFor"));
		if (pDlg){
			const CECFashionShop::FashionSelection *pSelectedFashion = m_pFashionShop->ItemAt(m_fashionSelectionIndex);
			int itemIndex = pSelectedFashion->itemIndex;
			const GSHOP_ITEM *pGShopItem = m_pFashionShop->Shop()->GetItem(itemIndex);
			pDlg->SetType(true, pGShopItem->id, CalculatePrice(), itemIndex, GetBuyIndex());
			pDlg->Show(true);
		}
	}
}

void CDlgFashionShopItem::OnCommand_AskForPresent(const char *szCommand)
{
	if (CanAskForPresent()){
		CDlgGivingFor*pDlg = dynamic_cast<CDlgGivingFor*>(GetGameUIMan()->GetDialog("Win_GivingFor"));
		if (pDlg){
			const CECFashionShop::FashionSelection *pSelectedFashion = m_pFashionShop->ItemAt(m_fashionSelectionIndex);
			int itemIndex = pSelectedFashion->itemIndex;
			const GSHOP_ITEM *pGShopItem = m_pFashionShop->Shop()->GetItem(itemIndex);
			pDlg->SetType(false, pGShopItem->id, CalculatePrice(), itemIndex, GetBuyIndex());
			pDlg->Show(true);
		}
	}
}

struct CDlgFashionShopItemAddToShopCart : public CECFashionShop::CECSelectedFashionLooper
{
	CECShoppingCart	*	pShoppingCart;
	bool				bItemAdded;		//	至少有一件商品添加成功时返回 true
	bool				bAddFailure;	//	至少有一件商品添加失败时返回 true
	CDlgFashionShopItemAddToShopCart(CECShoppingCart * p)
		: pShoppingCart(p), bItemAdded(false), bAddFailure(false){}
	virtual bool LoopSuiteItemAndContinue(const CECShopBase *pShop, int itemIndex, int buyIndex){
		if (pShop == pShoppingCart->Shop()){
			if (pShoppingCart->AddItem(itemIndex, buyIndex, 1)){
				bItemAdded = true;
			}else{
				bAddFailure = true;
			}
			return true;
		}else{
			ASSERT(false);
			return false;
		}
	}
};

void CDlgFashionShopItem::OnCommand_Buy(const char * szCommand){
	if (!CanBuyDirectly()){
		return;
	}
	CDlgQShopBuy* pDlg = dynamic_cast<CDlgQShopBuy*>(GetGameUIMan()->GetDialog("Win_QshopBuy"));
	if (!pDlg->IsShow()) {
		pDlg->Show(true, true);
		const CECFashionShop::FashionSelection *pSelectedFashion = m_pFashionShop->ItemAt(m_fashionSelectionIndex);
		CECShopBase * pShop = const_cast<CECShopBase *>(m_pFashionShop->Shop());
		pDlg->SetBuyingItem(pSelectedFashion->itemIndex, GetBuyIndex(), pShop);
	}
}

void CDlgFashionShopItem::OnCommand_AddToShopCart(const char * szCommand){
	if (!CanBuy()){
		return;
	}
	CDlgFashionShopItemAddToShopCart addToShopCart(m_pShoppingCart);
	m_pFashionShop->LoopSelectedFashion(m_fashionSelectionIndex, GetBuyIndex(), &addToShopCart);
}

struct CDlgFashionShopItemPriceCalculator : public CECFashionShop::CECSelectedFashionLooper
{
	int	price;
	CDlgFashionShopItemPriceCalculator(): price(0){
	}
	virtual bool LoopSuiteItemAndContinue(const CECShopBase *pShop, int itemIndex, int buyIndex){
		price += pShop->GetPrice(itemIndex, buyIndex);
		return true;
	}
};
int CDlgFashionShopItem::CalculatePrice()const
{
	if (m_pFashionShop){
		CDlgFashionShopItemPriceCalculator calculator;
		m_pFashionShop->LoopSelectedFashion(m_fashionSelectionIndex, GetBuyIndex(), &calculator);
		return calculator.price;
	}else{
		return 0;
	}
}

struct CDlgFashionShopItemDiscountChecker : public CECFashionShop::CECSelectedFashionLooper
{
	bool hasDiscount;
	CDlgFashionShopItemDiscountChecker(): hasDiscount(false){
	}
	virtual bool LoopSuiteItemAndContinue(const CECShopBase *pShop, int itemIndex, int buyIndex){
		unsigned int status = pShop->GetStatus(itemIndex, buyIndex);
		if (status>=4 && status <=12){
			hasDiscount = true;
			return false;
		}
		return true;
	}
};
bool CDlgFashionShopItem::HasDiscount()const
{
	if (m_pFashionShop){
		CDlgFashionShopItemDiscountChecker checker;
		m_pFashionShop->LoopSelectedFashion(m_fashionSelectionIndex, GetBuyIndex(), &checker);
		return checker.hasDiscount;
	}else{
		return false;
	}
}

struct CDlgFashionShopItemStatusGetter : public CECFashionShop::CECSelectedFashionLooper
{
	bool			firstItem;
	bool			uniform;
	unsigned int	status;
	CDlgFashionShopItemStatusGetter(): firstItem(true), status(0), uniform(true){
	}
	virtual bool LoopSuiteItemAndContinue(const CECShopBase *pShop, int itemIndex, int buyIndex){
		unsigned int s = pShop->GetStatus(itemIndex, buyIndex);
		if (firstItem){
			firstItem = false;
			status = s;
		}else if (s != status){
			uniform = false;
			return false;
		}
		return true;
	}
};
unsigned int CDlgFashionShopItem::GetUniformStatus()const
{
	unsigned int result(0);
	if (m_pFashionShop){
		CDlgFashionShopItemStatusGetter getter;
		m_pFashionShop->LoopSelectedFashion(m_fashionSelectionIndex, GetBuyIndex(), &getter);
		if (getter.uniform){
			result = getter.status;
		}
	}
	return result;
}

struct CDlgFashionShopItemGiftChecker : public CECFashionShop::CECSelectedFashionLooper
{
	bool hasGift;
	CDlgFashionShopItemGiftChecker(): hasGift(false){
	}
	virtual bool LoopSuiteItemAndContinue(const CECShopBase *pShop, int itemIndex, int buyIndex){
		const GSHOP_ITEM *pGShopItem = pShop->GetItem(itemIndex);
		if (pGShopItem->idGift > 0 && pGShopItem->iGiftNum > 0){
			hasGift = true;
			return false;
		}
		return true;
	}
};
bool CDlgFashionShopItem::HasGift()const
{
	if (m_pFashionShop){
		CDlgFashionShopItemGiftChecker checker;
		m_pFashionShop->LoopSelectedFashion(m_fashionSelectionIndex, GetBuyIndex(), &checker);
		return checker.hasGift;
	}else{
		return false;
	}
}

struct CDlgFashionShopItemFlashSaleChecker : public CECFashionShop::CECSelectedFashionLooper
{
	bool hasFlashSale;
	CDlgFashionShopItemFlashSaleChecker(): hasFlashSale(false){
	}
	virtual bool LoopSuiteItemAndContinue(const CECShopBase *pShop, int itemIndex, int buyIndex){
		unsigned int status = pShop->GetStatus(itemIndex, buyIndex);
		if (status==13){
			hasFlashSale = true;
			return false;
		}
		return true;
	}
};
bool CDlgFashionShopItem::HasFlashSale()const
{
	if (m_pFashionShop){
		CDlgFashionShopItemFlashSaleChecker checker;
		m_pFashionShop->LoopSelectedFashion(m_fashionSelectionIndex, GetBuyIndex(), &checker);
		return checker.hasFlashSale;
	}else{
		return false;
	}
}

void CDlgFashionShopItem::OnCommand_Time(const char * szCommand)
{
	int nNewTimeSelect = atoi(szCommand + strlen("Btn_Time")) - 1;
	if (nNewTimeSelect != m_nTimeSelect){
		if (m_pBtn_Time[m_nTimeSelect]){
			m_pBtn_Time[m_nTimeSelect]->SetPushed(false);
			m_pBtn_Time[m_nTimeSelect]->SetColor(A3DCOLORRGB(255, 255, 255));
		}
	}
	if (m_pBtn_Time[nNewTimeSelect]){
		m_pBtn_Time[nNewTimeSelect]->SetPushed(true);
		m_pBtn_Time[nNewTimeSelect]->SetColor(A3DCOLORRGB(255, 203, 74));
	}
	if (nNewTimeSelect != m_nTimeSelect){
		m_nTimeSelect = nNewTimeSelect;
		OnTimeSelectChange();
	}
}

void CDlgFashionShopItem::OnTimeSelectChange()
{
	UpdatePrice();
	if (m_pImg_Bg01){
		m_pImg_Bg01->Show(HasDiscount());	//	打折图标
	}
	if (m_pImg_Bg02){
		int status = GetUniformStatus();
		if (status >= 4 && status <= 12){
			m_pImg_Bg02->Show(true);
			m_pImg_Bg02->FixFrame(12-status);
		}else{
			m_pImg_Bg02->Show(false);
		}
	}
	if (m_pImg_Bg03){
		m_pImg_Bg03->Show(HasGift());		//	赠品图标
	}
	if (m_pImg_Bg04){
		m_pImg_Bg04->Show(HasFlashSale());	//	闪购图标
	}
}

void CDlgFashionShopItem::UpdatePrice()
{
	int price = CalculatePrice();
	if (m_pTxt_Price){
		ACString strText;
		strText.Format(GetStringFromTable(11310), GetGameUIMan()->GetCashText(price));
		m_pTxt_Price->SetText(strText);
	}
	if (m_pTxt_NoDiscountPrice){
		unsigned int status = GetUniformStatus();
		if (status >= 4 && status <= 12){
			m_pTxt_NoDiscountPrice->Show(true);
			ACString strText;
			strText.Format(GetStringFromTable(11311), GetGameUIMan()->GetCashText(CECShopBase::GetOriginalPrice(price, status)));
			m_pTxt_NoDiscountPrice->SetText(strText);
		}else{
			m_pTxt_NoDiscountPrice->Show(false);
			m_pTxt_NoDiscountPrice->SetText(_AL(""));
		}
	}
}

void CDlgFashionShopItem::OnCommand_CANCEL(const char * szCommand)
{
	if (m_pPositionController){
		m_pPositionController->GetParent()->OnCommand("IDCANCEL");
	}else{
		ClearItem();
	}
}

void CDlgFashionShopItem::SetItemIcon(const char *icon){
	if (icon && strlen(icon) && af_IsFileExist(icon)){
		AUIOBJECT_SETPROPERTY p;
		strcpy(p.fn, icon + strlen("Surfaces\\"));
		m_pImg_Item->Show(true);
		{
			ScopedAUIControlSpriteModify _dummy(m_pImg_Item);
			m_pImg_Item->SetProperty("Image File", &p);
		}
	}else{		
		AUIOBJECT_SETPROPERTY p;
		strcpy(p.fn, "");
		{
			ScopedAUIControlSpriteModify _dummy(m_pImg_Item);
			m_pImg_Item->SetProperty("Image File", &p);
		}
		m_pImg_Item->Show(false);
	}
}

void CDlgFashionShopItem::SetItemHint(const ACHAR *szHint){
	if (m_pTxt_ItemName){
		m_pTxt_ItemName->SetHint(szHint);
	}
	if (m_pImg_Item){
		m_pImg_Item->SetHint(szHint);
	}
	if (m_pImg_Bg01){
		m_pImg_Bg01->SetHint(szHint);
	}
	if (m_pImg_Bg02){
		m_pImg_Bg02->SetHint(szHint);
	}
	if (m_pImg_Bg03){
		m_pImg_Bg03->SetHint(szHint);
	}
	if (m_pImg_Bg04){
		m_pImg_Bg04->SetHint(szHint);
	}
}

struct CECSortGShopBuyTypeByTime
{
	const GSHOP_ITEM * m_pGShopItem;
	CECSortGShopBuyTypeByTime(const GSHOP_ITEM * pGShopItem)
		: m_pGShopItem(pGShopItem)
	{}
	bool operator()(const int& lhs, const int& rhs) const{
		if (lhs < 0){
			return rhs < 0;
		}else if (rhs < 0){
			return true;
		}else{
			return m_pGShopItem->buy[lhs].time > m_pGShopItem->buy[rhs].time;
		}
	}
};
void CDlgFashionShopItem::SortBuyType(int buyType[BUY_TYPE_COUNT], const CECShopBase *pShop, int itemIndex){
	std::sort(buyType, buyType+BUY_TYPE_COUNT, CECSortGShopBuyTypeByTime(pShop->GetItem(itemIndex)));
}

void CDlgFashionShopItem::InitTimeSelection(const GSHOP_ITEM *pGShopItem){	
	m_nTimeSelect = GetFirstBuyTypeIndex();
	CECGameUIMan *pGameUIMan = GetGameUIMan();
	for(int i = 0; i < TIME_TYPE_COUNT; i++){
		if (!m_pBtn_Time[i]){
			continue;
		}
		int buyIndex = m_BuyType[i];				
		if (buyIndex != -1 && pGShopItem && pGShopItem->buy[buyIndex].price != 0){
			m_pBtn_Time[i]->SetText(pGameUIMan->GetShopItemFormatTime(pGShopItem->buy[buyIndex].time));
			m_pBtn_Time[i]->Show(true);
			if (pGShopItem->buy[buyIndex].time != 0){
				ACString strText;
				strText.Format(GetStringFromTable(824), pGameUIMan->GetShopItemFormatTime(pGShopItem->buy[i].time));
				m_pBtn_Time[i]->SetHint(strText);
			}else{
				m_pBtn_Time[i]->SetHint(GetStringFromTable(825));
			}
			if (i == m_nTimeSelect){
				m_pBtn_Time[i]->SetPushed(true);
				m_pBtn_Time[i]->SetColor(A3DCOLORRGB(255, 203, 74));
			}else{
				m_pBtn_Time[i]->SetPushed(false);
				m_pBtn_Time[i]->SetColor(A3DCOLORRGB(255, 255, 255));
			}
		}else{
			m_pBtn_Time[i]->Show(false);
		}
	}
	HideSingleTimeSelection(pGShopItem);
	OnTimeSelectChange();	//	价格、打折、赠品等与时间选择相关控件
}

int CDlgFashionShopItem::GetBuyTypeCount()const{
	int result(0);
	for (int i(0); i < TIME_TYPE_COUNT; ++ i){
		if (m_BuyType[i] >= 0){
			++ result;
		}
	}
	return result;
}

int CDlgFashionShopItem::GetFirstBuyTypeIndex()const{
	int result = -1;
	for (int i(0); i < TIME_TYPE_COUNT; ++ i){
		if (m_BuyType[i] >= 0){
			result = i;
			break;
		}
	}
	return result;
}

void CDlgFashionShopItem::HideSingleTimeSelection(const GSHOP_ITEM *pGShopItem){
	if (pGShopItem && GetBuyTypeCount() == 1){
		int index = GetFirstBuyTypeIndex();
		if (m_pBtn_Time[index] != NULL &&
			pGShopItem->buy[m_BuyType[index]].time == 0){	//	0 表示永久
			m_pBtn_Time[index]->Show(false);
		}
	}
}

void CDlgFashionShopItem::SetItemName(const ACHAR *szName){
	if (m_pTxt_ItemName){
		m_pTxt_ItemName->SetText(szName);
	}
}

void CDlgFashionShopItem::SetItem(PAUIOBJECT positionController, const CECFashionShop* pFashionShop, int fashionSelectionIndex, CECShoppingCart *pShoppingCart)
{	
	if (m_pBtn_Buy){
		m_pBtn_Buy->SetHint(_AL(""));
	}
	if (!positionController ||
		!pFashionShop ||
		!pFashionShop->ItemAt(fashionSelectionIndex) ||
		!pShoppingCart ||
		pFashionShop->Shop() != pShoppingCart->Shop()){
		ASSERT(false);
		return;
	}
	m_pPositionController = positionController;
	m_pFashionShop = pFashionShop;
	m_fashionSelectionIndex = fashionSelectionIndex;
	m_pShoppingCart = pShoppingCart;

	if (!IsShow()){
		Show(true);
	}
	const CECFashionShop::FashionSelection *pSelectedFashion = m_pFashionShop->ItemAt(m_fashionSelectionIndex);
	const CECShopBase *pShop = pFashionShop->Shop();
	if (pFashionShop->IsFashionTypeSuite()){
		const FASHION_SUITE_ESSENCE *pSuiteEssence = pSelectedFashion->suite.pEssence;

		//	商品名称
		SetItemName(pSuiteEssence->name);

		//	显示大图标
		SetItemIcon(pSuiteEssence->file_icon);

		//	计算购买类型
		//	由于套装中商品购买类型相同（由 CECFashionShop 保证），因此，取第一个商品即可
		int itemIndex = pSelectedFashion->FirstSuiteItemIndex();
		pShop->CalcBuyType(itemIndex, m_BuyType);
		SortBuyType(m_BuyType, pShop, itemIndex);

		//	时间选择控件
		const GSHOP_ITEM *pGShopItem = pShop->GetItem(itemIndex);
		InitTimeSelection(pGShopItem);

	}else{
		int itemIndex = pSelectedFashion->itemIndex;

		//	计算购买类型
		pShop->CalcBuyType(itemIndex, m_BuyType);
		SortBuyType(m_BuyType, pShop, itemIndex);

		//	时间选择控件
		const GSHOP_ITEM *pGShopItem = pShop->GetItem(itemIndex);
		InitTimeSelection(pGShopItem);

		//	商品名称
		SetItemName(pGShopItem->szName);
		
		//	显示大图标
		SetItemIcon(pGShopItem->icon);
	}
	//	悬浮显示商品内容
	SetItemHint(CalculateItemHint());	
	if (IsFashionSuite()){
		m_pBtn_Buy->SetHint(GetStringFromTable(11308));
	}
}

struct CDlgFashionShopItemHintCalculator : public CECFashionShop::CECSelectedFashionLooper
{
	ACString	strHint;
	bool		debugMode;
	CDlgFashionShopItemHintCalculator(bool debug)
		: debugMode(debug)
	{
	}
	virtual bool LoopSuiteItemAndContinue(const CECShopBase *pShop, int itemIndex, int buyIndex){
		const GSHOP_ITEM *pGShopItem = pShop->GetItem(itemIndex);
		if (!debugMode){
			if (!strHint.IsEmpty()){
				strHint += _AL("\r");
			}
			strHint += CECUIHelper::NameForGShopItem(pGShopItem);
		}else{
			if (!strHint.IsEmpty()){
				strHint += _AL("\t");
			}
			strHint += CECUIHelper::HintForShoppingItem(pGShopItem->id, 0, pGShopItem->num);
		}
		return true;
	}
	virtual void VisitNonSuiteItem(const CECShopBase *pShop, int itemIndex, int buyIndex){
		const GSHOP_ITEM *pGShopItem = pShop->GetItem(itemIndex);
		strHint = CECUIHelper::HintForShoppingItem(pGShopItem->id, 0, pGShopItem->num);
	}
};

ACString CDlgFashionShopItem::CalculateItemHint()const
{
	if (m_pFashionShop){
		CDlgFashionShopItemHintCalculator hintCalculator(g_pGame->GetConfigs()->GetShowIDFlag());	//	d_showid 时，认为是调试模式，显示物品详细信息
		m_pFashionShop->LoopSelectedFashion(m_fashionSelectionIndex, GetBuyIndex(), &hintCalculator);
		if (m_pFashionShop->IsFashionTypeSuite()){
			hintCalculator.strHint += _AL("\r\r");
			hintCalculator.strHint += m_pAUIManager->GetStringFromTable(11309);
		}
		return hintCalculator.strHint;
	}else{
		return _AL("");
	}
}

bool CDlgFashionShopItem::IsFashionSuite()const{
	return m_pFashionShop && m_pFashionShop->IsFashionTypeSuite();
}

void CDlgFashionShopItem::ClearItem()
{
	m_pPositionController = NULL;
	m_pFashionShop = NULL;
	m_fashionSelectionIndex = -1;
	m_pShoppingCart = NULL;

	SetItemName(_AL(""));
	SetItemIcon("");
	SetItemHint(_AL(""));
	for (int i = 0; i < BUY_TYPE_COUNT; i++){
		m_BuyType[i] = -1;
	}
	InitTimeSelection(NULL);

	Show(false);
}


int CDlgFashionShopItem::FashionShopSelectionIndex(){
	return m_fashionSelectionIndex;
}

void CDlgFashionShopItem::SetSelected(bool bSelected, AUIImagePicture *pImg_Normal, AUIImagePicture *pImg_Hightlight){
	if (pImg_Normal && pImg_Hightlight){
		AUIImagePicture *pImage = bSelected ? pImg_Hightlight : pImg_Normal;		
		AUIOBJECT_SETPROPERTY p;
		pImage->GetProperty("Image File", &p);
		ScopedDialogSpriteModify _dummy(this);
		SetProperty("Frame Image", &p);
	}
}

void CDlgFashionShopItem::OnEventMouseWheel(WPARAM wParam, LPARAM lParam, AUIObject *pObj)
{
	if (CDlgFashionShop *pDlgFashionShop = FashionShopDialog()){
		pDlgFashionShop->OnEventMouseWheel(wParam, lParam, NULL);
	}
}

void CDlgFashionShopItem::OnEventLButtonDown(WPARAM wParam, LPARAM lParam, AUIObject *pObj)
{
	if (!pObj || pObj != m_pBtn_GivePresent && pObj != m_pBtn_AskForPresent && pObj != m_pBtn_Buy && pObj != m_pBtn_AddToShopCart){
		if (CDlgFashionShop *pDlgFashionShop = FashionShopDialog()){
			pDlgFashionShop->SelectFashionShopItem(m_fashionSelectionIndex);
			pDlgFashionShop->Fit(m_fashionSelectionIndex, GetBuyIndex());
		}
	}
}

int CDlgFashionShopItem::GetBuyIndex()const
{
	// 获得当前选中的时限的物品信息下标
	return m_BuyType[m_nTimeSelect];
}

CDlgFashionShop * CDlgFashionShopItem::FashionShopDialog(){
	CDlgFashionShop *pDlgFashionShop = NULL;
	if (m_pPositionController){
		pDlgFashionShop = dynamic_cast<CDlgFashionShop *>(m_pPositionController->GetParent());
	}
	return pDlgFashionShop;
}

void CDlgFashionShopItem::OnChangeLayoutEnd(bool bAllDone){
	if (m_pFashionShop){
		SetItem(m_pPositionController, m_pFashionShop, m_fashionSelectionIndex, m_pShoppingCart);
	}
}
