// Filename	: DlgFashionShop.cpp
// Creator	: Xu Wenbin
// Date		: 2014/5/22

#include "DlgFashionShop.h"
#include "DlgFashionShopItem.h"
#include "DlgShopCart.h"
#include "EC_UIHelper.h"

#include "EC_FashionModel.h"
#include "EC_FashionShop.h"
#include "EC_ShoppingCart.h"
#include "EC_Shop.h"
#include "EC_FixedMsg.h"
#include "EC_Game.h"
#include "EC_GameRun.h"
#include "EC_GameUIMan.h"
#include "EC_UIConfigs.h"
#include "EC_HostPlayer.h"
#include "EC_ProfConfigs.h"

#include "globaldataman.h"

#include <AUIDef.h>
#include <AUIImagePicture.h>
#include <AUIComboBox.h>
#include <AUIStillImageButton.h>
#include <AUIScroll.h>
#include <AUIRadioButton.h>
#include <AFI.h>

#include <A2DSprite.h>

//	class CDlgFashionShop::WearingItem
bool CDlgFashionShop::WearingItem::Valid(const CECShopBase *pShop)const{
	return pShop && pShop->ReadyToBuy(gshopItemIndex, gshopBuyIndex);
}

//	class CDlgFashionShop
AUI_BEGIN_COMMAND_MAP(CDlgFashionShop, CDlgBase)
AUI_ON_COMMAND("IDCANCEL",	OnCommand_CANCEL)
AUI_ON_COMMAND("Btn_Close",	OnCommand_CANCEL)
AUI_ON_COMMAND("Btn_New",	OnCommand_New)
AUI_ON_COMMAND("Btn_Hot",	OnCommand_Hot)
AUI_ON_COMMAND("Btn_Sale",	OnCommand_Sale)
AUI_ON_COMMAND("Btn_FlashSale",	OnCommand_FlashSale)
AUI_ON_COMMAND("Btn_Suite",	OnCommand_Suite)
AUI_ON_COMMAND("Btn_Head",	OnCommand_Head)
AUI_ON_COMMAND("Btn_UpperBody",	OnCommand_UpperBody)
AUI_ON_COMMAND("Btn_LowerBody",	OnCommand_LowerBody)
AUI_ON_COMMAND("Btn_WaistNShoes",OnCommand_WaistNShoes)
AUI_ON_COMMAND("Btn_Weapon",	OnCommand_Weapon)
AUI_ON_COMMAND("Btn_Male",		OnCommand_Male)
AUI_ON_COMMAND("Btn_Female",	OnCommand_Female)
AUI_ON_COMMAND("Btn_BuyWearing",OnCommand_BuyWearing)
AUI_ON_COMMAND("Btn_ShowCart",	OnCommand_ShowCart)
AUI_ON_COMMAND("Btn_Palette",	OnCommand_Palette)
AUI_ON_COMMAND("Btn_ResetPlayer",OnCommand_ResetPlayer)
AUI_ON_COMMAND("Btn_PrevAd",	OnCommand_PrevAd)
AUI_ON_COMMAND("Btn_NextAd",	OnCommand_NextAd)
AUI_ON_COMMAND("Rdo_Page*",		OnCommand_Rdo_Page)
AUI_END_COMMAND_MAP()

AUI_BEGIN_EVENT_MAP(CDlgFashionShop, CDlgBase)
AUI_ON_EVENT("Img_Char",		WM_LBUTTONDOWN,	OnEventLButtonDown_Char)
AUI_ON_EVENT("Img_Char",		WM_LBUTTONUP,	OnEventLButtonUp_Char)
AUI_ON_EVENT("Img_Char",		WM_RBUTTONDOWN,	OnEventRButtonDown_Char)
AUI_ON_EVENT("Img_Char",		WM_RBUTTONUP,	OnEventRButtonUp_Char)
AUI_ON_EVENT("Img_Char",		WM_MOUSEWHEEL,	OnEventMouseWheel_Char)
AUI_ON_EVENT("Img_Char",		WM_MOUSEMOVE,	OnEventMouseMove_Char)
AUI_ON_EVENT("*",				WM_MOUSEWHEEL,	OnEventMouseWheel)
AUI_ON_EVENT(NULL,				WM_MOUSEWHEEL,	OnEventMouseWheel)
AUI_ON_EVENT("Image_Prof",		WM_LBUTTONDOWN,	OnEventLButtonDown_Prof)
AUI_ON_EVENT("Img_SelProf*",	WM_LBUTTONDOWN,	OnEventLButtonDown_SelectProf)
AUI_END_EVENT_MAP()

CDlgFashionShop::CDlgFashionShop()
: m_pLbl_title(NULL)
, m_pBtn_New(NULL)
, m_pBtn_Hot(NULL)
, m_pBtn_Sale(NULL)
, m_pBtn_FlashSale(NULL)
, m_pBtn_Suite(NULL)
, m_pBtn_Head(NULL)
, m_pBtn_UpperBody(NULL)
, m_pBtn_LowerBody(NULL)
, m_pBtn_WaistNShoes(NULL)
, m_pBtn_Weapon(NULL)
, m_pBtn_Male(NULL)
, m_pBtn_Female(NULL)
, m_pImage_Prof(NULL)
, m_pTxt_Cash(NULL)
, m_pBtn_ShowCart(NULL)
, m_pScl_Item(NULL)
, m_nCurrentBarLevel(-1)
, m_pFashionShop(NULL)
, m_pShoppingCart(NULL)
, m_pImg_Ad(NULL)
, m_pBtn_PrevAd(NULL)
, m_pBtn_NextAd(NULL)
, m_AdCount(0)
, m_currentAd(-1)
, m_pCurrentAdSprite(NULL)
, m_pFashionModel(NULL)
, m_pBtn_BuyWearing(NULL)
, m_pImg_Char(NULL)
, m_pBtn_Palette(NULL)
, m_pBtn_ResetPlayer(NULL)
, m_pImg_Normal(NULL)
, m_pImg_Highlight(NULL)
, m_fashionSelectionIndex(-1)
, m_inRelease(false)
{
	m_adTimer.Reset(5*1000);
	memset(m_pImg_SelProf, 0, sizeof(m_pImg_SelProf));
}

bool CDlgFashionShop::OnInitDialog(){
	if (CDlgBase::OnInitDialog()){
		DDX_Control("Lbl_title",m_pLbl_title);
		DDX_Control("Btn_New", m_pBtn_New);
		DDX_Control("Btn_Hot",	m_pBtn_Hot);
		DDX_Control("Btn_Sale",	m_pBtn_Sale);
		DDX_Control("Btn_FlashSale",m_pBtn_FlashSale);
		DDX_Control("Btn_Suite",m_pBtn_Suite);
		DDX_Control("Btn_Head",	m_pBtn_Head);
		DDX_Control("Btn_UpperBody",	m_pBtn_UpperBody);
		DDX_Control("Btn_LowerBody",	m_pBtn_LowerBody);
		DDX_Control("Btn_WaistNShoes",	m_pBtn_WaistNShoes);
		DDX_Control("Btn_Weapon",	m_pBtn_Weapon);
		DDX_Control("Btn_Male",		m_pBtn_Male);
		DDX_Control("Btn_Female",	m_pBtn_Female);
		DDX_Control("Image_Prof",	m_pImage_Prof);
		DDX_Control("Txt_Cash",		m_pTxt_Cash);
		DDX_Control("Btn_ShowCart",	m_pBtn_ShowCart);
		DDX_Control("Scl_Item",		m_pScl_Item);
		DDX_Control("Img_Ad",		m_pImg_Ad);
		DDX_Control("Btn_PrevAd",	m_pBtn_PrevAd);
		DDX_Control("Btn_NextAd",	m_pBtn_NextAd);
		DDX_Control("Img_Char",		m_pImg_Char);
		DDX_Control("Btn_Palette",	m_pBtn_Palette);
		DDX_Control("Btn_ResetPlayer",	m_pBtn_ResetPlayer);
		DDX_Control("Btn_BuyWearing",m_pBtn_BuyWearing);
		DDX_Control("Img_Normal",	m_pImg_Normal);
		DDX_Control("Img_Highlight",m_pImg_Highlight);

		//	隐藏选中对象参考控件
		m_pImg_Normal->Show(false);
		m_pImg_Highlight->Show(false);

		//	职业选择图标
		if (A2DSprite *pSprite = m_pImage_Prof->GetImage()){
			pSprite->SetLinearFilter(true);
		}
		CECGameRun *pGameRun = GetGameRun();
		for (int i(0); i < NUM_PROFESSION; ++ i){
			DDX_Control(AString().Format("Img_SelProf%d", i), m_pImg_SelProf[i]);
			m_pImg_SelProf[i]->SetHint(pGameRun->GetProfName(i));
			if (A2DSprite *pSprite = m_pImg_SelProf[i]->GetImage()){
				pSprite->SetLinearFilter(true);
			}
		}
		SelectProfession(false);

		//	广告控件
		int nAds(0);
		while (true){
			AString strPageButton;
			strPageButton.Format("Rdo_Page%d", ++ nAds);
			AUIRadioButton *pButton = dynamic_cast<AUIRadioButton *>(GetDlgItem(strPageButton));
			if (!pButton){
				break;
			}else{
				m_AdPageButtons.push_back(pButton);
			}
		}
		//	根据广告图片个数，计算 m_AdCount
		for (m_AdCount = 0; m_AdCount < (int)m_AdPageButtons.size(); ++ m_AdCount){
			AString strAdPath = GetAdImagePath(m_AdCount);
			if (!af_IsFileExist(strAdPath)){
				break;
			}
		}
		//	隐藏多余的广告翻页控件
		for (int j = m_AdCount; j < (int)m_AdPageButtons.size(); ++ j){
			m_AdPageButtons[j]->Show(false);
		}
		if (m_AdCount > 0){
			ShowAd(0);
		}else{
			UpdateAdButtons();
		}

		m_pBtn_New->SetPushLike(true);
		m_pBtn_Hot->SetPushLike(true);
		m_pBtn_Sale->SetPushLike(true);
		m_pBtn_FlashSale->SetPushLike(true);

		m_pBtn_Suite->SetPushLike(true);
		m_pBtn_Head->SetPushLike(true);
		m_pBtn_UpperBody->SetPushLike(true);
		m_pBtn_LowerBody->SetPushLike(true);
		m_pBtn_WaistNShoes->SetPushLike(true);
		m_pBtn_Weapon->SetPushLike(true);

		m_pImg_Char->SetAcceptMouseMessage(true);
		m_pImage_Prof->SetAcceptMouseMessage(true);		
		for (int k(0); k < NUM_PROFESSION; ++ k){
			if (PAUIIMAGEPICTURE pImage = m_pImg_SelProf[k]){
				pImage->SetAcceptMouseMessage(true);
			}
		}
		return true;
	}
	return false;
}

AString CDlgFashionShop::GetAdImagePath(int index){
	AString strAdPath;
	if (index >= 0 && index < (int)CECUIConfig::Instance().GetGameUI().strFashionShopAdImage.size()){
		strAdPath.Format("surfaces\\2014\\广告\\%s", CECUIConfig::Instance().GetGameUI().strFashionShopAdImage[index]);
	}
	return strAdPath;
}

void CDlgFashionShop::ShowAd(int index){
	if (index < 0 || index >= m_AdCount){
		ASSERT(false);
		return;
	}
	if (index == m_currentAd){
		return;
	}
	//	先释放已有内容
	if (m_pCurrentAdSprite){
		m_pImg_Ad->ClearCover();
		A3DRELEASE(m_pCurrentAdSprite);
	}

	//	赋值新下标，不论后面是否成功加载图片
	m_currentAd = index;

	//	加载图片
	AString strAdPath = GetAdImagePath(index);
	strAdPath.TrimLeft("surfaces\\");
	A2DSprite* pA2DSprite = new A2DSprite;
	if (pA2DSprite->Init(GetGame()->GetA3DEngine()->GetA3DDevice(), strAdPath, 0)){
		pA2DSprite->SetLinearFilter(true);
		m_pImg_Ad->SetCover(pA2DSprite, 0);
		m_pCurrentAdSprite = pA2DSprite;	//	加载的图片在 Release 时保证释放
	}else {
		A3DRELEASE(pA2DSprite);
		delete pA2DSprite;
		pA2DSprite = NULL;
	}

	//	更新动画相关按钮状态
	UpdateAdButtons();

	//	更新自动动画计时器，为下次自动切换做准备
	m_adTimer.Reset();
}

bool CDlgFashionShop::HasNextAd(){
	return m_AdCount > 1;	//	循环查看，只要数量大于1即可
}

bool CDlgFashionShop::HasPrevAd(){
	return m_AdCount > 1;	//	循环查看，只要数量大于1即可
}

void CDlgFashionShop::UpdateAdButtons(){
	m_pBtn_PrevAd->Enable(HasPrevAd());
	m_pBtn_NextAd->Enable(HasNextAd());
	for (int i(0); i < m_AdCount; ++ i){
		AUIRadioButton *pButton = m_AdPageButtons[i];
		pButton->Check(i == m_currentAd);
	}
}

bool CDlgFashionShop::HasPlayer(){
	return m_pFashionModel != NULL && m_pFashionModel->GetPlayer() != NULL;
}

void CDlgFashionShop::OnShowDialog(){
	if (!m_pFashionShop || !m_pShoppingCart){
		ASSERT(false);
		OnCommand("IDCANCEL");
		return;
	}
	const_cast<CECShopBase *>(m_pFashionShop->Shop())->GetFromServer(0, 0);
	CDlgBase::OnShowDialog();
	UpdateCash();
	UpdateScrollPosition(true);
	if (!HasPlayer()){
		CECHostPlayer *pHost = GetHostPlayer();
		CreatePlayer(pHost->GetProfession(), pHost->GetGender());
	}
	m_adTimer.Reset();
}

void CDlgFashionShop::OnTick(){
	CDlgBase::OnTick();

	//	元宝更新
	UpdateCash();

	//	商品界面更新
	UpdateScrollPosition();

	//	试衣间模特更新
	if (HasPlayer()){
		m_pFashionModel->Tick();
		m_pImg_Char->SetRenderCallback(PlayerRenderDemonstration,
			(DWORD)m_pFashionModel->GetPlayer(),
			m_pFashionModel->GetAngle(),
			m_pFashionModel->ComposeCameraParameter());
	}else{
		m_pImg_Char->SetRenderCallback(NULL, 0, 0, 0);
	}

	//	广告更新
	if (m_AdCount > 1 && m_adTimer.IsTimeArrived()){
		ShowAd((m_currentAd+1)%m_AdCount);
	}
}

void CDlgFashionShop::ShowFalse(){
	SetModel(NULL, NULL);
	Show(false);
	CECGameUIMan *pGameUIMan = GetGameUIMan();
	for (int i(0); i < FASHION_SHOP_ITEM_COUNT; ++ i){
		pGameUIMan->m_pDlgFashionShopItem[i]->Show(false);
	}
	ShowPalette(false);
}

void CDlgFashionShop::OnCommand_CANCEL(const char *szCommand){
	if (!m_pFashionShop){
		ASSERT(false);
		return;
	}
	bool bIsQShop = (m_pFashionShop == CECFashionShopManager::Instance().QShopFashionShop());
	CECUIHelper::ShowShop(bIsQShop);
	CECUIHelper::AlignByCenter(GetGameUIMan()->GetDialog(CECUIHelper::GetShopDialogName(bIsQShop)), this);
}

void CDlgFashionShop::OnCommand_New(const char * szCommand){
	UpdateSaleType();
	if (m_pFashionShop){
		m_pFashionShop->ChangeSaleType(CECFashionShop::ST_NEW_ARRIVE);
	}
}
void CDlgFashionShop::OnCommand_Hot(const char * szCommand){
	UpdateSaleType();
	if (m_pFashionShop){
		m_pFashionShop->ChangeSaleType(CECFashionShop::ST_HOT_SALE);
	}
}
void CDlgFashionShop::OnCommand_Sale(const char * szCommand){
	UpdateSaleType();
	if (m_pFashionShop){
		m_pFashionShop->ChangeSaleType(CECFashionShop::ST_ON_SALE);
	}
}
void CDlgFashionShop::OnCommand_FlashSale(const char * szCommand){
	UpdateSaleType();
	if (m_pFashionShop){
		m_pFashionShop->ChangeSaleType(CECFashionShop::ST_FLASH_SALE);
	}
}

void CDlgFashionShop::OnCommand_Suite(const char * szCommand){
	UpdateFashionType();
	if (m_pFashionShop){
		m_pFashionShop->ChangeFashionType(CECFashionShop::FT_SUITE);
	}
}
void CDlgFashionShop::OnCommand_Head(const char * szCommand){
	UpdateFashionType();
	if (m_pFashionShop){
		m_pFashionShop->ChangeFashionType(CECFashionShop::FT_HEAD);
	}
}
void CDlgFashionShop::OnCommand_UpperBody(const char * szCommand){
	UpdateFashionType();
	if (m_pFashionShop){
		m_pFashionShop->ChangeFashionType(CECFashionShop::FT_UPPER_BODY);
	}
}
void CDlgFashionShop::OnCommand_LowerBody(const char * szCommand){
	UpdateFashionType();
	if (m_pFashionShop){
		m_pFashionShop->ChangeFashionType(CECFashionShop::FT_LOWER_BODY);
	}
}
void CDlgFashionShop::OnCommand_WaistNShoes(const char * szCommand){
	UpdateFashionType();
	if (m_pFashionShop){
		m_pFashionShop->ChangeFashionType(CECFashionShop::FT_WAIST_N_SHOES);
	}
}
void CDlgFashionShop::OnCommand_Weapon(const char * szCommand){
	UpdateFashionType();
	if (m_pFashionShop){
		m_pFashionShop->ChangeFashionType(CECFashionShop::FT_WEAPON);
	}
}

void CDlgFashionShop::OnCommand_Male(const char * szCommand){
	if (m_pFashionShop){
		int newGender = GENDER_FEMALE;		//	显示男时，点击切换到女
		if (CECProfConfig::Instance().IsExist(m_pFashionShop->Profession(), newGender)){
			m_pFashionShop->ChangeGender(newGender);
		}
	}
}

void CDlgFashionShop::OnCommand_Female(const char * szCommand){
	if (m_pFashionShop){
		int newGender = GENDER_MALE;		//	显示女时，点击切换到男
		if (CECProfConfig::Instance().IsExist(m_pFashionShop->Profession(), newGender)){
			m_pFashionShop->ChangeGender(newGender);
		}
	}
}

void CDlgFashionShop::OnCommand_BuyWearing(const char *szCommand){
	if (m_pFashionShop && m_pShoppingCart){
		for (int i(0); i < SIZE_ALL_EQUIPIVTR; ++ i){
			const WearingItem &wearingItem = m_wearingItems[i];
			if (wearingItem.Valid(m_pFashionShop->Shop())){
				m_pShoppingCart->AddItem(wearingItem.gshopItemIndex, wearingItem.gshopBuyIndex, 1);
			}
		}
	}
}

void CDlgFashionShop::OnCommand_ShowCart(const char * szCommand){
	if (m_pShoppingCart){
		CECGameUIMan *pGameUIMan = GetGameUIMan();
		CDlgShopCart * pDlgShopCart = dynamic_cast<CDlgShopCart *>(pGameUIMan->GetDialog("Win_ShopCart"));
		if (pDlgShopCart->SetShoppingCart(m_pShoppingCart)){
			if (!pDlgShopCart->IsShow()){
				pDlgShopCart->Show(true);
			}else{
				pGameUIMan->BringWindowToTop(pDlgShopCart);
			}
		}else{
			ASSERT(false);		//	购物车界面被占用（正在购买）、且当前无法释放
		}
	}
}

void CDlgFashionShop::OnCommand_Palette(const char * szCommand){
	ShowPalette(!IsPaletteShown());
}

void CDlgFashionShop::OnCommand_ResetPlayer(const char * szCommand){
	if (HasPlayer()){
		if (m_pFashionModel->HasFashionOn()){
			ResetFashion();
		}
		if (!m_pFashionModel->IsCameraDefault()){
			m_pFashionModel->ResetCamera();
			UpdateResetPlayer();
		}
	}
}

void CDlgFashionShop::OnEventLButtonDown_Char(WPARAM wParam, LPARAM lParam, AUIObject *pObj)
{
	if (!HasPlayer()){
		return;
	}
	A3DVIEWPORTPARAM *p = m_pA3DEngine->GetActiveViewport()->GetParam();
	int x = GET_X_LPARAM(lParam) - p->X;
	int y = GET_Y_LPARAM(lParam) - p->Y;
	if (!m_pFashionModel->OnEventLButtonDown(x, y)){
		return;
	}
	SetCaptureObject(m_pImg_Char);
}

void CDlgFashionShop::OnEventLButtonUp_Char(WPARAM wParam, LPARAM lParam, AUIObject *pObj)
{
	if (HasPlayer()){
		A3DVIEWPORTPARAM *p = m_pA3DEngine->GetActiveViewport()->GetParam();
		int x = GET_X_LPARAM(lParam) - p->X;
		int y = GET_Y_LPARAM(lParam) - p->Y;
		if (m_pFashionModel->OnEventLButtonUp(x, y)){
			SetCaptureObject(NULL);
		}
	}
	ChangeFocus(NULL);
}

void CDlgFashionShop::OnEventRButtonDown_Char(WPARAM wParam, LPARAM lParam, AUIObject *pObj)
{
	if (!HasPlayer()){
		return;
	}
	A3DVIEWPORTPARAM *p = m_pA3DEngine->GetActiveViewport()->GetParam();
	int x = GET_X_LPARAM(lParam) - p->X;
	int y = GET_Y_LPARAM(lParam) - p->Y;
	if (!m_pFashionModel->OnEventRButtonDown(x, y)){
		return;
	}
	SetCaptureObject(m_pImg_Char);
}

void CDlgFashionShop::OnEventRButtonUp_Char(WPARAM wParam, LPARAM lParam, AUIObject *pObj)
{
	if (HasPlayer()){
		A3DVIEWPORTPARAM *p = m_pA3DEngine->GetActiveViewport()->GetParam();
		int x = GET_X_LPARAM(lParam) - p->X;
		int y = GET_Y_LPARAM(lParam) - p->Y;
		if (m_pFashionModel->OnEventRButtonUp(x, y)){
			SetCaptureObject(NULL);
		}
	}
	ChangeFocus(NULL);
}

void CDlgFashionShop::OnEventMouseMove_Char(WPARAM wParam, LPARAM lParam, AUIObject *pObj)
{
	if (!HasPlayer()){
		return;
	}
	A3DVIEWPORTPARAM *p = m_pA3DEngine->GetActiveViewport()->GetParam();
	int x = GET_X_LPARAM(lParam) - p->X;
	int y = GET_Y_LPARAM(lParam) - p->Y;
	if (!m_pFashionModel->OnEventMouseMove(x, y)){
		return;
	}
	UpdateResetPlayer();
}

void CDlgFashionShop::OnEventMouseWheel_Char(WPARAM wParam, LPARAM lParam, AUIObject *pObj)
{
	if (!HasPlayer()){
		return;
	}
	int zDelta = (short)HIWORD(wParam);
	if (!m_pFashionModel->OnEventMouseWheel(zDelta)){
		return;
	}
	UpdateResetPlayer();
}

void CDlgFashionShop::OnEventMouseWheel(WPARAM wParam, LPARAM lParam, AUIObject *pObj)
{	
	int x = GET_X_LPARAM(lParam);
	int y = GET_Y_LPARAM(lParam);
	int zDelta = (short)HIWORD(wParam);
	if( zDelta > 0 ){
		zDelta = 1;
	}else{
		zDelta = -1;
	}
	if (m_pScl_Item->IsShow()){
		m_pScl_Item->SetBarLevel(m_pScl_Item->GetBarLevel() - zDelta);
	}
}

void CDlgFashionShop::OnEventLButtonDown_Prof(WPARAM wParam, LPARAM lParam, AUIObject *pObj){
	if (m_pFashionShop){
		SelectProfession(!IsSelectingProfession());
	}
}

void CDlgFashionShop::OnEventLButtonDown_SelectProf(WPARAM wParam, LPARAM lParam, AUIObject *pObj){
	if (m_pFashionShop && pObj){
		int profession = atoi(pObj->GetName()+strlen("Img_SelProf"));
		int gender = m_pFashionShop->Gender();
		if (!CECProfConfig::Instance().IsExist(profession, gender)){
			if (gender == GENDER_MALE){
				gender = GENDER_FEMALE;
			}else if (gender == GENDER_FEMALE){
				gender = GENDER_MALE;
			}
		}
		if (!CECProfConfig::Instance().IsExist(profession, gender)){
			return;
		}
		m_pFashionShop->ChangeProfessionGender(profession, gender);
		SelectProfession(false);
	}
}

void CDlgFashionShop::OnChangeLayoutEnd(bool bAllDone){
	UpdateItems();
	UpdateShoppingCart();
}

bool CDlgFashionShop::Release(){
	m_inRelease = true;

	SetFashionShop(NULL);
	SetShoppingCart(NULL);

	m_pImg_Ad->ClearCover();
	A3DRELEASE(m_pCurrentAdSprite);

	delete m_pFashionModel;
	m_pFashionModel = NULL;
	return CDlgBase::Release();
}

bool CDlgFashionShop::IsInconsistent(CECFashionShop *pFashionShop, CECShoppingCart *pShoppingCart){
	bool bInconsistent(false);
	if (pFashionShop && pShoppingCart &&
		pFashionShop->Shop() != pShoppingCart->Shop()){
		bInconsistent = true;		//	商城来源和购物车不是一个，不允许这种情况发生
	}
	return bInconsistent;
}

void CDlgFashionShop::SelectProfession(bool bSelect){
	if (bSelect == IsSelectingProfession()){
		return;
	}
	int gender = GENDER_MALE;
	if (m_pFashionShop){
		gender = m_pFashionShop->Gender();
	}
	for (int i(0); i < NUM_PROFESSION; ++ i){
		if (PAUIIMAGEPICTURE pImage = m_pImg_SelProf[i]){
			pImage->Show(bSelect);
			pImage->FixFrame(i + gender * NUM_PROFESSION);
		}
	}
}

bool CDlgFashionShop::IsSelectingProfession(){
	return m_pImg_SelProf[0] && m_pImg_SelProf[0]->IsShow();
}

bool CDlgFashionShop::SetShoppingCart(CECShoppingCart *pShoppingCart){
	if (pShoppingCart == m_pShoppingCart){
		return true;
	}
	if (IsInconsistent(m_pFashionShop, pShoppingCart)){
		ASSERT(false);
		return false;
	}
	if (m_pShoppingCart){
		m_pShoppingCart->UnregisterObserver(this);
	}
	m_pShoppingCart = pShoppingCart;
	if (m_pShoppingCart){
		m_pShoppingCart->RegisterObserver(this);
	}
	return true;
}

bool CDlgFashionShop::SetFashionShop(CECFashionShop *pFashionShop){
	if (pFashionShop == m_pFashionShop){
		return true;
	}
	if (IsInconsistent(pFashionShop, m_pShoppingCart)){
		ASSERT(false);
		return false;
	}
	if (m_pFashionShop){
		m_pFashionShop->UnregisterObserver(this);
	}
	m_pFashionShop = pFashionShop;
	if (m_pFashionShop){
		m_pFashionShop->RegisterObserver(this);
	}
	return true;
}

bool CDlgFashionShop::SetModel(CECShoppingCart *pShoppingCart, CECFashionShop *pFashionShop){
	if ((pShoppingCart == NULL) != (pFashionShop == NULL)){
		ASSERT(false);
		return false;
	}
	if (IsInconsistent(pFashionShop, pShoppingCart)){
		ASSERT(false);
		return false;
	}
	if (pShoppingCart == m_pShoppingCart &&
		pFashionShop == m_pFashionShop){
		return true;
	}
	
	CDlgShopCart * pDlgShopCart = dynamic_cast<CDlgShopCart *>(GetGameUIMan()->GetDialog("Win_ShopCart"));
	if (pDlgShopCart->IsBuying()){
		return false;					//	购买过程中，为维持与购物车显示保持一致，拒绝修改
	}
	if (pDlgShopCart->IsShow()){		
		pDlgShopCart->OnCommand("IDCANCEL");	//	隐藏购物车，避免出现乾坤袋时装商城与鸿利商城购物车同时出现情况，导致误会
	}
	if (pShoppingCart == NULL){
		SetFashionShop(NULL);
		SetShoppingCart(NULL);
		return true;
	}
	SetFashionShop(NULL);				//	CDlgFashionShopItem 依赖于购物车，因此，先清空时装商城数据
	SetShoppingCart(pShoppingCart);		//	再设置购物车
	SetFashionShop(pFashionShop);		//	再重新设置商城并更新 CDlgFashionShopItem
	return true;
}

const CECFashionShop * CDlgFashionShop::GetFashionShop()const{
	return m_pFashionShop;
}

void CDlgFashionShop::OnRegistered(const CECFashionShop *p){
	if (p != m_pFashionShop){
		ASSERT(false);
		return;
	}
	UpdateProfession();
	UpdateGender();
	UpdatePlayer();
	UpdateSaleType();
	UpdateFashionType();
	UpdateItems();
	UpdateFashionShopName();
}

void CDlgFashionShop::OnModelChange(const CECFashionShop *p, const CECObservableChange *q){
	if (p != m_pFashionShop){
		ASSERT(false);
		return;
	}
	const CECFashionShopChange *pChange = dynamic_cast<const CECFashionShopChange *>(q);
	if (!pChange){
		ASSERT(false);
		return;
	}
	if (pChange->SaleTypeChanged()){
		UpdateSaleType();
	}
	if (pChange->FashionTypeChanged()){
		UpdateFashionType();
	}
	if (pChange->ProfessionChanged()){
		UpdateProfession();
	}
	if (pChange->GenderChanged()){
		UpdateGender();
	}
	if (pChange->ProfessionChanged() || pChange->GenderChanged()){
		UpdatePlayer();
	}
	if (pChange->MallItemsChanged()){
		UpdateWearingItems(true);
		a_LogOutput(1, "CDlgFashionShop::OnModelChange, wearing items rechecked because mall items updated.");
	}
	UpdateItems();
	SelectFashionShopItem(-1);
}

void CDlgFashionShop::OnUnregister(const CECFashionShop *p){
	if (m_inRelease){
		return;			//	正在 Release 时，不应进行下列操作，尤其是 UpdateItems 中访问其它对话框（已被释放）
	}
	if (p != m_pFashionShop){
		ASSERT(false);
		return;
	}
	m_pFashionShop = NULL;
	m_fashionSelectionIndex = -1;
	ClearWearingItems();
	UpdateProfession();
	UpdateGender();
	UpdatePlayer();
	UpdateSaleType();
	UpdateFashionType();
	UpdateItems();
	UpdateFashionShopName();
}

void CDlgFashionShop::OnRegistered(const CECShoppingCart *p){
	if (p != m_pShoppingCart){
		ASSERT(false);
		return;
	}
	UpdateShoppingCart();
}

void CDlgFashionShop::OnModelChange(const CECShoppingCart *p, const CECObservableChange *q){
	if (p != m_pShoppingCart){
		ASSERT(false);
		return;
	}
	const CECShoppingCartChange *pChange = dynamic_cast<const CECShoppingCartChange *>(q);
	if (!pChange){
		ASSERT(false);
		return;
	}
	//	因本界面涉及购物车的内容较少，为简化起见，不做细致判断，直接更新所有相关内容
	UpdateShoppingCart();
}

void CDlgFashionShop::OnUnregister(const CECShoppingCart *p){
	if (m_inRelease){
		return;
	}
	if (p != m_pShoppingCart){
		ASSERT(false);
		return;
	}
	m_pShoppingCart = NULL;
	UpdateShoppingCart();
}


void CDlgFashionShop::UpdateSaleType(){
	m_pBtn_New->Enable(m_pFashionShop != NULL);
	m_pBtn_Hot->Enable(m_pFashionShop != NULL);
	m_pBtn_Sale->Enable(m_pFashionShop != NULL);
	m_pBtn_FlashSale->Enable(m_pFashionShop != NULL);
	if (m_pFashionShop){
		CECFashionShop::enumSaleType saleType = m_pFashionShop->SaleType();
		m_pBtn_New->SetPushed(saleType == CECFashionShop::ST_NEW_ARRIVE);
		m_pBtn_Hot->SetPushed(saleType == CECFashionShop::ST_HOT_SALE);
		m_pBtn_Sale->SetPushed(saleType == CECFashionShop::ST_ON_SALE);
		m_pBtn_FlashSale->SetPushed(saleType == CECFashionShop::ST_FLASH_SALE);
	}else{		
		m_pBtn_New->SetPushed(false);
		m_pBtn_Hot->SetPushed(false);
		m_pBtn_Sale->SetPushed(false);
		m_pBtn_FlashSale->SetPushed(false);
	}
	if (m_pFashionShop && m_pFashionShop->Shop()){
		m_pBtn_FlashSale->Show(m_pFashionShop->Shop()->IsFashionShopFlashSaleEnabled());
		m_pBtn_FlashSale->SetText(m_pFashionShop->Shop()->GetFashionShopFlashSaleTitle());
	}else{
		m_pBtn_FlashSale->Show(false);
		m_pBtn_FlashSale->SetText(_AL(""));
	}
}

void CDlgFashionShop::UpdateFashionType(){
	m_pBtn_Suite->Enable(m_pFashionShop != NULL);
	m_pBtn_Head->Enable(m_pFashionShop != NULL);
	m_pBtn_UpperBody->Enable(m_pFashionShop != NULL);
	m_pBtn_LowerBody->Enable(m_pFashionShop != NULL);
	m_pBtn_WaistNShoes->Enable(m_pFashionShop != NULL);
	m_pBtn_Weapon->Enable(m_pFashionShop != NULL);
	if (m_pFashionShop){
		CECFashionShop::enumFashionType fashionType = m_pFashionShop->FashionType();
		m_pBtn_Suite->SetPushed(fashionType == CECFashionShop::FT_SUITE);
		m_pBtn_Head->SetPushed(fashionType == CECFashionShop::FT_HEAD);
		m_pBtn_UpperBody->SetPushed(fashionType == CECFashionShop::FT_UPPER_BODY);
		m_pBtn_LowerBody->SetPushed(fashionType == CECFashionShop::FT_LOWER_BODY);
		m_pBtn_WaistNShoes->SetPushed(fashionType == CECFashionShop::FT_WAIST_N_SHOES);
		m_pBtn_Weapon->SetPushed(fashionType == CECFashionShop::FT_WEAPON);
	}else{
		m_pBtn_Suite->SetPushed(false);
		m_pBtn_Head->SetPushed(false);
		m_pBtn_UpperBody->SetPushed(false);
		m_pBtn_LowerBody->SetPushed(false);
		m_pBtn_WaistNShoes->SetPushed(false);
		m_pBtn_Weapon->SetPushed(false);
	}
}

void CDlgFashionShop::UpdateProfession(){
	m_pImage_Prof->Show(m_pFashionShop != NULL);
	if (m_pFashionShop){
		m_pImage_Prof->SetHint(GetGameRun()->GetProfName(m_pFashionShop->Profession()));
		m_pImage_Prof->FixFrame(m_pFashionShop->Profession() + m_pFashionShop->Gender() * NUM_PROFESSION);
	}else{
		m_pImage_Prof->SetHint(_AL(""));
	}
}

void CDlgFashionShop::UpdateGender(){
	if (m_pFashionShop != NULL){
		int gender = m_pFashionShop->Gender();
		m_pBtn_Male->Show(gender == GENDER_MALE);
		m_pBtn_Female->Show(gender == GENDER_FEMALE);
	}else{
		m_pBtn_Male->Show(false);
		m_pBtn_Female->Show(false);
	}
}

void CDlgFashionShop::UpdateShoppingCart(){
	//	按钮的Enable/Disable
	bool bEnableShowShopCart(false);
	if (m_pShoppingCart){
		if (m_pShoppingCart->Count() > 0){
			bEnableShowShopCart = true;
		}
	}
	m_pBtn_ShowCart->Enable(bEnableShowShopCart);

	//	购物车商品数量更新
	int nCount(0);
	if (m_pShoppingCart){
		for (int i(0); i < m_pShoppingCart->Count(); ++ i){
			const CECShoppingCartItem * pCartItem = m_pShoppingCart->ItemAt(i);
			nCount += pCartItem->Count();
		}
	}
	m_pBtn_ShowCart->SetText(ACString().Format(GetStringFromTable(11305), nCount));
}

void CDlgFashionShop::UpdateCash(){
	int nCash(0);
	if (m_pFashionShop){
		nCash = m_pFashionShop->Shop()->GetCash();
	}
	GetGameUIMan()->SetCashText(m_pTxt_Cash, nCash);
}

void CDlgFashionShop::UpdateItems(){
	int itemCount = 0;
	if (m_pFashionShop){
		itemCount = m_pFashionShop->Count();
	}
	if (itemCount > FASHION_SHOP_ITEM_COUNT){
		m_pScl_Item->SetScrollRange(0, (itemCount-1)/FASHION_SHOP_ITEM_WIDTH - (FASHION_SHOP_ITEM_HEIGHT-1));
		m_pScl_Item->Show(true);
	}else{
		m_pScl_Item->SetScrollRange(0, 0);
		m_pScl_Item->Show(false);
	}
	m_pScl_Item->SetBarLevel(0);
	m_pScl_Item->SetBarLength(-1);
	m_pScl_Item->SetScrollStep(1);
	m_nCurrentBarLevel = -1;	
	UpdateScrollPosition(true);
}

void CDlgFashionShop::UpdateFashionShopName(){
	const ACHAR *szShopName = _AL("");
	if (m_pFashionShop == CECFashionShopManager::Instance().QShopFashionShop()){
		szShopName = GetStringFromTable(11306);
	}else if (m_pFashionShop == CECFashionShopManager::Instance().BackShopFashionShop()){
		szShopName = GetStringFromTable(11307);
	}
	m_pLbl_title->SetText(szShopName);
}

void CDlgFashionShop::UpdateScrollPosition(bool bForceUpdate/* = false*/){	
	if (!IsShow()){
		//	当前对话框没显示时，留到 OnTick 中更新
		//	否则，将导致 CDlgFashionShopItem 对话框在 CDlgFashionShop 没显示时出现（更换界面皮肤时）
		return;
	}
	if (m_nCurrentBarLevel != m_pScl_Item->GetBarLevel() || bForceUpdate){
		m_nCurrentBarLevel = m_pScl_Item->GetBarLevel();

		//	计算要显示的数据范围
		int nStart = m_nCurrentBarLevel * FASHION_SHOP_ITEM_WIDTH;
		int nEnd = nStart + FASHION_SHOP_ITEM_COUNT;
		int itemCount(0);
		if (m_pFashionShop){
			itemCount = m_pFashionShop->Count();
		}
		if (nEnd > itemCount){
			nEnd = itemCount;
		}
		
		//	[nStart, nEnd)范围内的数据需要显示，其它界面隐藏
		bool showSuite = m_pFashionShop && m_pFashionShop->IsFashionTypeSuite();
		CECGameUIMan * pGameUIMan = GetGameUIMan();
		for (int i(0); i < FASHION_SHOP_ITEM_COUNT; ++ i){
			CDlgFashionShopItem *pDlgItem = pGameUIMan->m_pDlgFashionShopItem[i];
			if (nStart+i < nEnd){
				//	该界面需显示
				pDlgItem->SetItem(GetDlgItem(AString().Format("Lab_Dialog%d", i)), m_pFashionShop, nStart+i, m_pShoppingCart);
			}else{
				//	隐藏剩余界面
				pDlgItem->ClearItem();
			}
		}
		UpdateFashionSelection();
	}
}

void CDlgFashionShop::OnCommand_PrevAd(const char * szCommand){
	if (HasPrevAd()){
		ShowAd((m_currentAd+m_AdCount-1)%m_AdCount);
	}
}

void CDlgFashionShop::OnCommand_NextAd(const char * szCommand){
	if (HasNextAd()){
		ShowAd((m_currentAd+1)%m_AdCount);
	}
}

void CDlgFashionShop::OnCommand_Rdo_Page(const char * szCommand){
	int index = atoi(szCommand+strlen("Rdo_Page")) - 1;
	ShowAd(index);
}

void CDlgFashionShop::UpdatePlayer(){
	if (m_pFashionShop){
		CreatePlayer(m_pFashionShop->Profession(), m_pFashionShop->Gender());
	}
}

void CDlgFashionShop::UpdateWearingItems(bool bTryWearLastFashion){
	for (int i(0); i < SIZE_ALL_EQUIPIVTR; ++ i){
		WearingItem &wearingItem = m_wearingItems[i];
		if (bTryWearLastFashion){
			if (m_pFashionShop && wearingItem.Valid(m_pFashionShop->Shop())){
				const GSHOP_ITEM *pGShopItem = m_pFashionShop->Shop()->GetItem(wearingItem.gshopItemIndex);
				if (!m_pFashionModel->Fit(pGShopItem->id)){
					wearingItem.Clear();
				}//	else 保持不变
			}else{
				wearingItem.Clear();
			}
		}else{
			wearingItem.Clear();
		}
	}
	UpdateBuyWearing();
}

void CDlgFashionShop::CreatePlayer(int profession, int gender){
	if (!m_pFashionModel){
		m_pFashionModel = new CECFashionModel;
	}
	bool bWearLastFashion(false);	//	是否与之前角色还穿同样时装（可能是其子集，如时装武器）
	if (m_pFashionModel->GetGender() == gender){
		bWearLastFashion = true;
	}
	m_pFashionModel->CreatePlayer(profession, gender);

	//	清除对旧有指针的引用
	m_pImg_Char->SetRenderCallback(NULL, 0, 0, 0);

	//	清除或重穿已有着装
	UpdateWearingItems(bWearLastFashion);
	//	隐藏调色板
	ShowPalette(false);
	//	重置调色板入口
	UpdatePalette();
	//	角色重置
	UpdateResetPlayer();
}

void CDlgFashionShop::ResetFashion(){	
	if (!HasPlayer() ||
		!m_pFashionModel->HasFashionOn()){
		return;
	}
	m_pFashionModel->ClearFashion();
	ClearWearingItems();
	UpdateBuyWearing();
	UpdatePalette();
	ShowPalette(false);
	UpdateResetPlayer();
}

void CDlgFashionShop::ClearWearingItems(){
	for (int i(0); i < SIZE_ALL_EQUIPIVTR; ++ i){
		m_wearingItems[i].Clear();
	}
}

void CDlgFashionShop::ShowPalette(bool bShow){
	PAUIDIALOG pDlgPalatte = GetAUIManager()->GetDialog("Win_Palette2");
	if (pDlgPalatte->IsShow() != bShow){
		pDlgPalatte->Show(bShow);
	}
}

bool CDlgFashionShop::IsPaletteShown(){
	PAUIDIALOG pDlgPalatte = GetAUIManager()->GetDialog("Win_Palette2");
	return pDlgPalatte->IsShow();
}

struct CDlgFashionShopItemFitter : public CECFashionShop::CECSelectedFashionLooper
{
	CECFashionModel	*	pFashionModel;
	abase::vector<int>	fittedItems;
	CDlgFashionShopItemFitter(CECFashionModel * p)
		: pFashionModel(p){}
	virtual bool LoopSuiteItemAndContinue(const CECShopBase *pShop, int itemIndex, int buyIndex){
		const GSHOP_ITEM *pGShopItem = pShop->GetItem(itemIndex);
		if (pFashionModel && pFashionModel->Fit(pGShopItem->id)){
			fittedItems.push_back(itemIndex);
		}
		return true;
	}
};
void CDlgFashionShop::Fit(int fashionShopSelectionIndex, int buyIndex){
	if (m_pFashionShop){
		if (m_pFashionShop->IsFashionTypeSuite()){
			ResetFashion();	//	套装试穿时，先清除身上已有时装，以获得套装的整体效果
		}
		CDlgFashionShopItemFitter fitter(m_pFashionModel);
		m_pFashionShop->LoopSelectedFashion(fashionShopSelectionIndex, buyIndex, &fitter);
		for (int i(0); i < (int)fitter.fittedItems.size(); ++ i){
			int itemIndex = fitter.fittedItems[i];
			const GSHOP_ITEM *pGShopItem = m_pFashionShop->Shop()->GetItem(itemIndex);
			int equipSlot = CECFashionModel::GetFashionEquipSlot(pGShopItem->id);
			m_wearingItems[equipSlot].Set(itemIndex, buyIndex);
		}
		UpdateBuyWearing();
		UpdatePalette();
		ShowPalette(false);
		UpdateResetPlayer();
	}
}

//	修改指定位置时装的颜色
void CDlgFashionShop::ChangeFashionColor(int equipSlot, unsigned short newColor){
	if (m_pFashionModel){
		m_pFashionModel->ChangeFashionColor(equipSlot, newColor);
	}
}

bool CDlgFashionShop::GetFashionColor(int equipSlot, unsigned short &color){
	return m_pFashionModel && m_pFashionModel->GetFashionColor(equipSlot, color);
}

bool CDlgFashionShop::GetFashionBestColor(int equipSlot, unsigned short &color){
	return m_pFashionModel && m_pFashionModel->GetFashionBestColor(equipSlot, color);
}

void CDlgFashionShop::SelectFashionShopItem(int index){
	if (m_fashionSelectionIndex == index){
		return;
	}
	m_fashionSelectionIndex = index;
	UpdateFashionSelection();
}

int  CDlgFashionShop::SelectedFashionShopItem(){
	return m_fashionSelectionIndex;
}

//	根据当前选中、更新各商品的选中状态
void CDlgFashionShop::UpdateFashionSelection(){
	CECGameUIMan *pGameUIMan = GetGameUIMan();
	for (int i(0); i < FASHION_SHOP_ITEM_COUNT; ++ i){
		CDlgFashionShopItem *pDlgItem = pGameUIMan->m_pDlgFashionShopItem[i];
		if (pDlgItem->IsShow()){
			pDlgItem->SetSelected(pDlgItem->FashionShopSelectionIndex() == m_fashionSelectionIndex, m_pImg_Normal, m_pImg_Highlight);
		}
	}
}

void CDlgFashionShop::UpdateBuyWearing(){
	if (m_pBtn_BuyWearing){
		if (m_pFashionShop){
			for (int i(0); i < SIZE_ALL_EQUIPIVTR; ++ i){
				if (m_wearingItems[i].Valid(m_pFashionShop->Shop())){
					m_pBtn_BuyWearing->Enable(true);
					return;
				}
			}
		}
		m_pBtn_BuyWearing->Enable(false);
	}
}

void CDlgFashionShop::UpdatePalette(){
	if (m_pBtn_Palette){
		m_pBtn_Palette->Enable(HasPlayer() && m_pFashionModel->CanChangeColor());
	}
}

void CDlgFashionShop::UpdateResetPlayer(){
	if (m_pBtn_ResetPlayer){
		m_pBtn_ResetPlayer->Enable(HasPlayer() && (m_pFashionModel->HasFashionOn() || !m_pFashionModel->IsCameraDefault()));
	}
}