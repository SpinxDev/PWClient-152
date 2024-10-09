// Filename	: DlgShopCartSub.cpp
// Creator	: Xu Wenbin
// Date		: 2014/5/19

#include "DlgShopCartSub.h"
#include "DlgShopCartSubList.h"
#include "EC_ShoppingCart.h"
#include "EC_Shop.h"
#include "EC_IvtrItem.h"
#include "globaldataman.h"
#include "EC_GameUIMan.h"
#include "EC_UIHelper.h"

#include <AUICTranslate.h>
#include <AUIStillImageButton.h>
#include <AUIImagePicture.h>
#include <AUIDef.h>

AUI_BEGIN_COMMAND_MAP(CDlgShopCartSub, CDlgBase)
AUI_ON_COMMAND("Btn_+",		OnCommand_Add)
AUI_ON_COMMAND("Btn_-",		OnCommand_Sub)
AUI_ON_COMMAND("Edt_Count",	OnCommand_Edt_Count)
AUI_ON_COMMAND("Btn_Delete",OnCommand_Delete)
AUI_ON_COMMAND("IDCANCEL",	OnCommand_CANCEL)
AUI_END_COMMAND_MAP()

AUI_BEGIN_EVENT_MAP(CDlgShopCartSub, CDlgBase)
AUI_ON_EVENT("Btn_+",	WM_LBUTTONDOWN,	OnEventLButtonDown_Add)
AUI_ON_EVENT("Btn_-",	WM_LBUTTONDOWN,	OnEventLButtonDown_Sub)
AUI_ON_EVENT("Edt_Count",WM_KILLFOCUS,	OnEventKillFocus_Edt_Count)
AUI_ON_EVENT("Edt_Count",WM_KEYDOWN,	OnEventKeyDown_Edt_Count)
AUI_ON_EVENT("*",		WM_MOUSEWHEEL,	OnEventMouseWheel)
AUI_ON_EVENT(NULL,		WM_MOUSEWHEEL,	OnEventMouseWheel)
AUI_END_EVENT_MAP()

CDlgShopCartSub::CDlgShopCartSub()
: m_pShoppingCart(NULL)
, m_itemCartID(INVALID_ITEM_SHOPPINGCART_ID)
, m_pImg_Goods(NULL)
, m_pImg_Gift(NULL)
, m_pTxt_Name(NULL)
, m_pEdt_Count(NULL)
, m_pTxt_Price(NULL)
, m_pBtn_Add(NULL)
, m_pBtn_Sub(NULL)
, m_pBtn_Delete(NULL)
{
	m_numberTrigger.Reset(INTERVAL_WAIT_BEGIN);
}

bool CDlgShopCartSub::OnInitDialog(){
	if (CDlgBase::OnInitDialog()){
		DDX_Control("Img_Goods", m_pImg_Goods);
		DDX_Control("Img_Gift", m_pImg_Gift);
		DDX_Control("Txt_Name", m_pTxt_Name);
		DDX_Control("Edt_Count", m_pEdt_Count);
		DDX_Control("Txt_Price", m_pTxt_Price);
		DDX_Control("Btn_+", m_pBtn_Add);
		DDX_Control("Btn_-", m_pBtn_Sub);
		DDX_Control("Btn_Delete", m_pBtn_Delete);
		UpdateContentImpl(NULL);
		return true;
	}
	return false;
}

void CDlgShopCartSub::SetItem(CECShoppingCart *pShoppingCart, ItemShoppingCartID itemCartID){
	m_pShoppingCart = pShoppingCart;
	m_itemCartID = itemCartID;
	UpdateContentImpl(CartItem());
}

ItemShoppingCartID CDlgShopCartSub::ItemCartID()const{
	return m_itemCartID;
}

const CECShoppingCart * CDlgShopCartSub::ShoppingCart()const{
	return m_pShoppingCart;
}

const CECShoppingCartItem * CDlgShopCartSub::CartItem()const{
	return ShoppingCart() ? ShoppingCart()->ItemForID(ItemCartID()) : NULL;
}

void CDlgShopCartSub::UpdateLockImpl(const CECShoppingCartItem *pShoppingCartItem){
	if (pShoppingCartItem){
		m_pEdt_Count->Enable(!ShoppingCart()->IsLocked());
		m_pBtn_Add->Enable(!ShoppingCart()->IsLocked());
		m_pBtn_Sub->Enable(!ShoppingCart()->IsLocked());
		m_pBtn_Delete->Enable(!ShoppingCart()->IsLocked());
	}else{
		m_pEdt_Count->Enable(false);
		m_pBtn_Add->Enable(false);
		m_pBtn_Sub->Enable(false);
		m_pBtn_Delete->Enable(false);
	}
}

void CDlgShopCartSub::UpdateLock(){
	UpdateLockImpl(CartItem());
}

void CDlgShopCartSub::UpdateCountImpl(const CECShoppingCartItem *pShoppingCartItem){
	if (pShoppingCartItem){
		m_pEdt_Count->SetText(ACString().Format(_AL("%d"), pShoppingCartItem->Count()));
	}else{
		m_pEdt_Count->SetText(_AL(""));
	}
}

void CDlgShopCartSub::UpdatePriceImpl(const CECShoppingCartItem *pShoppingCartItem){
	int price(0);
	if (pShoppingCartItem){
		const CECShopBase *pShop = ShoppingCart()->Shop();
		price = pShop->GetPrice(pShoppingCartItem->GShopItemIndex(), pShoppingCartItem->BuyIndex());
		price *= pShoppingCartItem->Count();
	}
	GetGameUIMan()->SetCashText(m_pTxt_Price, price);
}

void CDlgShopCartSub::OnCountChange(){
	const CECShoppingCartItem *pShoppingCartItem = CartItem();
	UpdateCountImpl(pShoppingCartItem);
	UpdatePriceImpl(pShoppingCartItem);
}

void CDlgShopCartSub::UpdateContentImpl(const CECShoppingCartItem *pShoppingCartItem){
	PAUIOBJECT pObjFocus = GetFocus();	
	
	if (pShoppingCartItem){
		const CECShopBase *pShop = ShoppingCart()->Shop();
		const GSHOP_ITEM *pGShopItem = pShop->GetItem(pShoppingCartItem->GShopItemIndex());
		CECIvtrItem *pIvtrItem = CECIvtrItem::CreateItem(pGShopItem->id, 0, pGShopItem->num);
		pIvtrItem->GetDetailDataFromLocal();
		pIvtrItem->SetPriceScale(CECIvtrItem::SCALE_BUY, 0.0f);
		
		//	图标
		CECGameUIMan *pGameUIMan = GetGameUIMan();
		pGameUIMan->SetCover(m_pImg_Goods, pIvtrItem->GetIconFile());

		//	赠品图标
		m_pImg_Gift->Show(pGShopItem->idGift > 0 && pGShopItem->iGiftNum > 0);

		//	悬浮
		m_pImg_Goods->SetHint(CECUIHelper::HintForGShopItem(pGShopItem));
		
		//	名称
		m_pTxt_Name->SetText(pIvtrItem->GetName());	
		delete pIvtrItem;
	}else{
		m_pImg_Goods->ClearCover();
		m_pImg_Goods->SetHint(_AL(""));
		m_pImg_Gift->Show(false);
		m_pTxt_Name->SetText(_AL(""));
	}
	
	UpdateCountImpl(pShoppingCartItem);	//	更新数目显示
	UpdatePriceImpl(pShoppingCartItem);	//	更新价钱显示
	UpdateLockImpl(pShoppingCartItem);	//	更新界面锁定

	if (GetFocus() != pObjFocus){
		ChangeFocus(pObjFocus);
	}
}

void CDlgShopCartSub::UpdateContent(){
	UpdateContentImpl(CartItem());
}

void CDlgShopCartSub::OnTick()
{
	if (m_pShoppingCart){
		bool bAdd = m_pBtn_Add->GetState() != AUISTILLIMAGEBUTTON_STATE_NORMAL;
		bool bSub = m_pBtn_Sub->GetState() != AUISTILLIMAGEBUTTON_STATE_NORMAL;
		if (bAdd || bSub){
			if (m_numberTrigger.IsTimeArrived()){
				m_numberTrigger.Reset(INTERVAL_WAIT_NEXT); //	首次加点时间间隔较长，以后较短
				if (bAdd){
					m_pShoppingCart->IncreaseItemCount(ItemCartID(), 1);
				}else{
					m_pShoppingCart->DecreaseItemCount(ItemCartID(), 1);
				}
			}
		}
	}
}

void CDlgShopCartSub::OnCommand_Add(const char *szCommand)
{
	if (m_pShoppingCart){
		m_pShoppingCart->IncreaseItemCount(ItemCartID(), 1);
		m_numberTrigger.Reset(INTERVAL_WAIT_BEGIN);
	}
}

void CDlgShopCartSub::OnCommand_Sub(const char *szCommand)
{
	if (m_pShoppingCart){
		m_pShoppingCart->DecreaseItemCount(ItemCartID(), 1);
		m_numberTrigger.Reset(INTERVAL_WAIT_BEGIN);
	}
}

void CDlgShopCartSub::OnCommand_Edt_Count(const char *szCommand)
{
	if (m_pShoppingCart){
		ACString strText = m_pEdt_Count->GetText();
		int count = strText.ToInt();
		if (!m_pShoppingCart->SetItemCount(ItemCartID(), count)){
			OnCountChange();
		}
	}
}

void CDlgShopCartSub::OnCommand_CANCEL(const char *szCommand)
{
	m_pAUIManager->RespawnMessage();
}

void CDlgShopCartSub::OnCommand_Delete(const char *szCommand)
{
	if (m_pShoppingCart){
		m_pShoppingCart->RemoveItem(ItemCartID());
	}
}

void CDlgShopCartSub::OnEventLButtonDown_Add(WPARAM wParam, LPARAM lParam, PAUIOBJECT pObj)
{
	m_numberTrigger.Reset(INTERVAL_WAIT_BEGIN);
}

void CDlgShopCartSub::OnEventLButtonDown_Sub(WPARAM wParam, LPARAM lParam, PAUIOBJECT pObj)
{
	m_numberTrigger.Reset(INTERVAL_WAIT_BEGIN);
}

void CDlgShopCartSub::OnEventKillFocus_Edt_Count(WPARAM wParam, LPARAM lParam, PAUIOBJECT pObj)
{
	OnCommand("Edt_Count");
}

void CDlgShopCartSub::OnEventKeyDown_Edt_Count(WPARAM wParam, LPARAM lParam, PAUIOBJECT pObj)
{
	if (wParam == VK_ESCAPE){
		ChangeFocus(NULL);
	}
}

void CDlgShopCartSub::OnEventMouseWheel(WPARAM wParam, LPARAM lParam, PAUIOBJECT pObj)
{
	PAUISUBDIALOG pShopCartSubDialog = GetParentDlgControl()->GetParent()->GetParentDlgControl();
	pShopCartSubDialog->OnDlgItemMessage(WM_MOUSEWHEEL, wParam, lParam);
}