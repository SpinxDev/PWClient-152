// File		: DlgGodEvilConvert.cpp
// Creator	: Xu Wenbin
// Date		: 2010/3/16

#include "AFI.h"
#include "DlgGodEvilConvert.h"
#include "DlgInventory.h"
#include "EC_GameUIMan.h"
#include "EC_GameSession.h"
#include "EC_HostPlayer.h"
#include "EC_IvtrItem.h"
#include "EC_Inventory.h"
#include "auto_delete.h"
#include "EC_TaoistRank.h"

#include "AUICTranslate.h"

#define new A_DEBUG_NEW

AUI_BEGIN_COMMAND_MAP(CDlgGodEvilConvert, CDlgBase)

AUI_ON_COMMAND("confirm",		OnCommandNormalConfirm)
AUI_ON_COMMAND("Btn_Confirm2",	OnCommandAdvancedConfirm)
AUI_ON_COMMAND("IDCANCEL",		OnCommandCancel)

AUI_END_COMMAND_MAP()

AUI_BEGIN_EVENT_MAP(CDlgGodEvilConvert, CDlgBase)

AUI_END_EVENT_MAP()

static const A3DCOLOR COLOR_WHITE = A3DCOLORRGB(255, 255, 255);
static const A3DCOLOR COLOR_RED = A3DCOLORRGB(255, 0, 0);

enum enumGodEvilConvertType
{
	enumGECTNormal,
	enumGECTAdvanced,
};

static const int GOD_EVIL_CONVERT_ITEM_0 = 27911;
static const int GOD_EVIL_CONVERT_ITEM_1 = 44629;

void CDlgGodEvilConvert::OnCommandNormalConfirm(const char *szCommand)
{
	ConfirmConvert(enumGECTNormal, GOD_EVIL_CONVERT_ITEM_0);
}

void CDlgGodEvilConvert::OnCommandAdvancedConfirm(const char * szCommand)
{
	ConfirmConvert(enumGECTAdvanced, GOD_EVIL_CONVERT_ITEM_1);
}

void CDlgGodEvilConvert::OnCommandCancel(const char *szCommand)
{
	GetGameUIMan()->EndNPCService();
	GetGameUIMan()->GetDialog("Win_SageDemonConvertHelp")->Show(false);
	Show(false);
}

void CDlgGodEvilConvert::Update()
{
	UpdateConvertUI((PAUIIMAGEPICTURE)GetDlgItem("Img_Need1"), GetDlgItem("Lbl_Need1"), GOD_EVIL_CONVERT_ITEM_0);
	UpdateConvertUI((PAUIIMAGEPICTURE)GetDlgItem("Img_Need2"), GetDlgItem("Lbl_Need2"), GOD_EVIL_CONVERT_ITEM_1);
}

void CDlgGodEvilConvert::OnShowDialog()
{
	Update();
}

bool CDlgGodEvilConvert::HasItem(int itemID){
	return GetHostPlayer()->GetPack()->FindItem(itemID) >= 0;
}

bool CDlgGodEvilConvert::MeetTaoistRank(int converType){
	const CECTaoistRank *p = CECTaoistRank::GetTaoistRank(GetHostPlayer()->GetBasicProps().iLevel2);
	if (!p){
		return false;
	}
	if (p->IsGodRank()){
		return p == CECTaoistRank::GetLastGodRank();
	}
	if (p->IsEvilRank()){
		return p == CECTaoistRank::GetLastEvilRank();
	}
	return false;
}

void CDlgGodEvilConvert::UpdateConvertUI( AUIImagePicture *  pImgNeed1, AUIObject * pLbelNeed1, int itemID )
{
	if (!pImgNeed1 || !pLbelNeed1){
		return;
	}
	CECIvtrItem* pItem = CECIvtrItem::CreateItem(itemID, 0, 1);
	auto_delete<CECIvtrItem> _dummy(pItem);
	
	GetGameUIMan()->SetCover(pImgNeed1, pItem->GetIconFile());
	
	pImgNeed1->SetColor(HasItem(itemID) ? COLOR_WHITE : COLOR_RED);
	pLbelNeed1->SetColor(HasItem(itemID) ? COLOR_WHITE : COLOR_RED);
	
	ACString strHint = pItem->GetDesc();
	if (!strHint.IsEmpty()){
		
		AUICTranslate trans;
		strHint = trans.Translate(strHint);
	}
	pImgNeed1->SetHint(strHint);
}

void CDlgGodEvilConvert::ConfirmConvert(int convertType, int itemID)
{
	if (!MeetTaoistRank(convertType)){
		GetGameUIMan()->ShowErrorMsg(GetStringFromTable(8603));
	}else if (!HasItem(itemID)){
		GetGameUIMan()->ShowErrorMsg(GetStringFromTable(8602));
	}else{
		GetGameSession()->c2s_CmdNPCSevGodEvilConvert(convertType);
	}
	OnCommandCancel("");
}