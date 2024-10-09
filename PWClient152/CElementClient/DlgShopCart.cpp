// Filename	: DlgShopCart.cpp
// Creator	: Xu Wenbin
// Date		: 2014/5/20

#include "DlgShopCart.h"
#include "DlgShopCartSubList.h"
#include "EC_GPDataType.h"

#include "EC_GameUIMan.h"
#include "EC_ShoppingCart.h"
#include "EC_ShoppingCartChecker.h"
#include "EC_ShoppingController.h"
#include "EC_Shop.h"
#include "globaldataman.h"
#include "EC_ShoppingManager.h"

//	class CECShopCartBuyCommand
class CECShopCartBuyCommand : public CECBuyCommand
{
	CECGameUIMan	  *		m_pGameUIMan;
	const CECShoppingCart * m_pShoppingCart;
protected:	
	virtual void OnShow(){}
	virtual void OnConfirm();
	virtual void OnCancel(){}	
public:
	CECShopCartBuyCommand(const CECShoppingCart *pShoppingCart, CECGameUIMan *pGameUIMan);
	virtual ACString GetMessage()const;
};

//	class CECShopCartBuyCommand
CECShopCartBuyCommand::CECShopCartBuyCommand(const CECShoppingCart *pShoppingCart, CECGameUIMan *pGameUIMan)
: m_pShoppingCart(pShoppingCart)
, m_pGameUIMan(pGameUIMan)
{
}

void CECShopCartBuyCommand::OnConfirm(){
	//	执行购买命令
	if (CECShoppingController *pShoppingController = CECShoppingManager::Instance().ShoppingControllerFor(m_pShoppingCart)){
		if (!pShoppingController->IsBuying()){
			pShoppingController->Buy();
		}
	}
}

ACString CECShopCartBuyCommand::GetMessage()const{
	ACString strMessage;
	if (m_pShoppingCart){
		const CECShopBase *pShop = m_pShoppingCart->Shop();
		ACString strFormat = m_pGameUIMan->GetStringFromTable(11304);		
		ACString strText;
		for (int i(0); i < m_pShoppingCart->Count(); ++ i){
			const CECShoppingCartItem *item = m_pShoppingCart->ItemAt(i);
			if (item->Count() > 0){
				const GSHOP_ITEM *pGShopItem = m_pShoppingCart->Shop()->GetItem(item->GShopItemIndex());
				int unitPrice = pShop->GetPrice(item->GShopItemIndex(), item->BuyIndex());
				strText.Format(strFormat,
					m_pGameUIMan->GetCashText(unitPrice * item->Count()),	//	此商品购买总价
					item->Count(),											//	商品件数
					pGShopItem->szName,										//	商品名称
					m_pGameUIMan->GetShopItemFormatTime(pGShopItem->buy[item->BuyIndex()].time));	//	时间期限
				if (!strMessage.IsEmpty()){
					strMessage += _AL("\r");
				}
				strMessage += strText;
			}
		}
		if (!strMessage.IsEmpty()){
			strText.Format(m_pGameUIMan->GetStringFromTable(11303)
				, m_pGameUIMan->GetCashText(m_pShoppingCart->CashCost())
				, m_pShoppingCart->Count());
			strText += _AL("\r");
			strMessage = strText + strMessage;
		}
	}
	return strMessage;
}

//	class CDlgShopCart

AUI_BEGIN_COMMAND_MAP(CDlgShopCart, CDlgBase)
AUI_ON_COMMAND("IDCANCEL", OnCommand_CANCEL)
AUI_ON_COMMAND("Btn_Close", OnCommand_CANCEL)
AUI_ON_COMMAND("Btn_Buy", OnCommand_Buy)
AUI_END_COMMAND_MAP()

CDlgShopCart::CDlgShopCart()
: m_pShoppingCart(NULL)
, m_pShoppingCartBeforeChangeLayout(NULL)
, m_pDlgCartSubList(NULL)
, m_pTxt_TotalPrice(NULL)
, m_pBtn_Buy(NULL)
, m_inRelease(false)
{
}

bool CDlgShopCart::OnInitDialog(){
	if (CDlgBase::OnInitDialog()){
		DDX_Control("Txt_TotalPrice", m_pTxt_TotalPrice);
		DDX_Control("Btn_Buy",	m_pBtn_Buy);
		m_priceDefaultColor = m_pTxt_TotalPrice->GetColor();
		return true;
	}
	return false;
}

void CDlgShopCart::OnCommand_CANCEL(const char *szCommand)
{
	if (!IsBuying() && IsShow()){
		Show(false);
	}
}

bool CDlgShopCart::OnChangeLayoutBegin(){
	if (m_pShoppingCart){
		m_pShoppingCartBeforeChangeLayout = m_pShoppingCart;
		SetShoppingCart(NULL);
		m_pAUIManager->Tick(0);
	}
	m_pDlgCartSubList = NULL;
	return true;
}

void CDlgShopCart::OnChangeLayoutEnd(bool bAllDone)
{
	if (m_pShoppingCartBeforeChangeLayout){
		SetShoppingCart(m_pShoppingCartBeforeChangeLayout);
	}
}

bool CDlgShopCart::Release(){
	m_inRelease = true;
	SetShoppingCart(NULL);
	return CDlgBase::Release();
}

bool CDlgShopCart::SetShoppingCart(CECShoppingCart *pShoppingCart){
	if (pShoppingCart == m_pShoppingCart){
		return true;
	}
	if (IsBuying() && !m_inRelease){
		return false;
	}
	if (!m_pDlgCartSubList){
		m_pDlgCartSubList = dynamic_cast<CDlgShopCartSubList *>(GetAUIManager()->GetDialog("Win_ShopCartSubList"));
	}
	if (m_pShoppingCart){
		ShoppingController()->UnregisterObserver(this);
		m_pShoppingCart->UnregisterObserver(this);
	}
	m_pShoppingCart = pShoppingCart;
	if (m_pShoppingCart){
		m_pShoppingCart->RegisterObserver(this);
		ShoppingController()->RegisterObserver(this);
	}
	return true;
}

void CDlgShopCart::OnRegistered(const CECShoppingCart *p){
	if (p != m_pShoppingCart){
		ASSERT(false);
		return;
	}
	RefreshPrice();
	RefreshBuyButton();
	if (m_pDlgCartSubList){
		m_pDlgCartSubList->SetShopCart(m_pShoppingCart);
	}
}

void CDlgShopCart::OnModelChange(const CECShoppingCart *p, const CECObservableChange *q){
	if (p != m_pShoppingCart){
		ASSERT(false);
		return;
	}
	//	更新购物车各物品显示界面
	if (m_pDlgCartSubList){
		m_pDlgCartSubList->OnModelChange(p, q);
	}
	//	更新当前界面
	RefreshPrice();
	RefreshBuyButton();
	//	非购买状态下，如果购物车内所有内容都被删除，则没必要再显示购物车
	if (!IsBuying() && m_pShoppingCart->Count() <= 0){
		OnCommand("IDCANCEL");
	}
}

void CDlgShopCart::OnUnregister(const CECShoppingCart *p){
	if (m_inRelease){
		return;
	}
	if (p != m_pShoppingCart){
		ASSERT(false);
		return;
	}
	m_pShoppingCart = NULL;
	if (m_pDlgCartSubList){
		m_pDlgCartSubList->SetShopCart(NULL);
	}
	RefreshPrice();
	RefreshBuyButton();
}

void CDlgShopCart::RefreshPrice(){
	int nPrice(0);
	if (m_pShoppingCart){
		nPrice = m_pShoppingCart->CashCost();
	}
	GetGameUIMan()->SetCashText(m_pTxt_TotalPrice, nPrice);
	if (m_pShoppingCart){
		CECShoppingCardChecker checker(m_pShoppingCart);
		m_pTxt_TotalPrice->SetColor(checker.HaveEnoughCash() ? m_priceDefaultColor : A3DCOLORRGB(255, 0, 0));	//	金钱不够时显示红色
	}else{
		m_pTxt_TotalPrice->SetColor(m_priceDefaultColor);
	}
}

void CDlgShopCart::RefreshBuyButton(){
	m_pBtn_Buy->Enable(CanClickBuy() || CanClickStopBuy());
	m_pBtn_Buy->SetText(GetStringFromTable(IsBuying() ? 11301 : 11300));
}

CECShoppingController * CDlgShopCart::ShoppingController(){
	return m_pShoppingCart != NULL ? CECShoppingManager::Instance().ShoppingControllerFor(m_pShoppingCart) : NULL;
}

bool CDlgShopCart::IsBuying(){
	if (CECShoppingController *pShoppingController = ShoppingController()){
		return pShoppingController->IsBuying();
	}
	return false;
}

bool CDlgShopCart::CanClickBuy(){
	bool bCan(false);
	if (!IsBuying() && m_pShoppingCart){
		CECShoppingCardChecker checker(m_pShoppingCart);
		if (checker.HaveItemsToBuy() && checker.HaveEnoughCash()){
			bCan = true;
		}
	}
	return bCan;
}

bool CDlgShopCart::CanClickStopBuy(){
	return IsBuying();
}

void CDlgShopCart::OnCommand_Buy(const char *szCommand){
	if (CanClickStopBuy()){
		ShoppingController()->StopBuying(SCSBR_BY_USER);
		return;
	}
	CECGameUIMan *pGameUIMan = GetGameUIMan();
	if (CanClickBuy()){
		CECShoppingCardChecker checker(m_pShoppingCart);
		if (!checker.HaveEnoughSpace()){
			pGameUIMan->ShowErrorMsg(11302);
			return;
		}
	}
	//	弹框确认后再执行购买
	pGameUIMan->m_pDlgBuyConfirm->Buy(new CECShopCartBuyCommand(m_pShoppingCart, pGameUIMan));
}

void CDlgShopCart::OnModelChange(const CECShoppingController *p, const CECObservableChange *q){
	CECShoppingController *pShoppingController = ShoppingController();
	if (p != pShoppingController){
		ASSERT(false);
		return;
	}
	const CECShoppingControllerChange *pChange = dynamic_cast<const CECShoppingControllerChange *>(q);
	if (!pChange){
		ASSERT(false);
		return;
	}
	switch (pChange->GetChangeMask()){
	case CECShoppingControllerChange::START_SHOPPING:
		RefreshBuyButton();
		break;
	case CECShoppingControllerChange::STOP_SHOPPING:
		RefreshBuyButton();
		ShowStopReason(pChange->StopReason(), pChange->BuyedItemsCount(), pChange->InBuyItemsCount(), pChange->ToBuyItemsCount());
		if (m_pShoppingCart->Count() <= 0){
			OnCommand("IDCANCEL");
		}
		break;
	}
}

void CDlgShopCart::ShowStopReason(int reason, int buyedCount, int inbuyCount, int tobuyCount){
	ACString strMsg;
	switch (reason){
	case SCSBR_FINISHED:
		strMsg.Format(GetStringFromTable(11312), buyedCount);
		break;
	case SCSBR_TIME_ARRIVED:
		if (inbuyCount <= 0){
			strMsg.Format(GetStringFromTable(11313), (buyedCount+inbuyCount+tobuyCount), buyedCount);
		}else{
			strMsg.Format(GetStringFromTable(11314), (buyedCount+inbuyCount+tobuyCount), buyedCount, inbuyCount);
		}
		break;
	case SCSBR_MALL_ITEM_CHANGED:
		if (inbuyCount <= 0){
			strMsg.Format(GetStringFromTable(11315), (buyedCount+inbuyCount+tobuyCount), buyedCount);
		}else{
			strMsg.Format(GetStringFromTable(11316), (buyedCount+inbuyCount+tobuyCount), buyedCount, inbuyCount);
		}
		break;
	}
	if (!strMsg.IsEmpty()){
		GetGameUIMan()->AddChatMessage(strMsg, GP_CHAT_SYSTEM);
	}
}