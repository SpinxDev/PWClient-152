// Filename	: DlgReincarnation.cpp
// Creator	: Han Guanghui
// Date		: 2013/08/06

#include "DlgReincarnation.h"
#include "ExpTypes.h"
#include "AFI.h"
#include "AUIImagepicture.h"
#include "AUICTranslate.h"
#include "EC_GameUIMan.h"
#include "EC_GameSession.h"
#include "EC_HostPlayer.h"
#include "EC_IvtrItem.h"
#include "EC_Inventory.h"

#define new A_DEBUG_NEW

AUI_BEGIN_COMMAND_MAP(CDlgReincarnation, CDlgBase)
AUI_ON_COMMAND("Btn_Start",		OnCommandConfirm)
AUI_ON_COMMAND("IDCANCEL",		OnCommandCancel)
AUI_ON_COMMAND("Btn_Cancel",	OnCommandCancel)
AUI_END_COMMAND_MAP()

static const A3DCOLOR COLOR_WHITE = A3DCOLORRGB(255, 255, 255);
static const A3DCOLOR COLOR_RED = A3DCOLORRGB(255, 0, 0);
const int ATTRIBUTE_POINT[] = {
	20, 5, 7, 10, 14, 20
};
CDlgReincarnation::CDlgReincarnation()
{

}

CDlgReincarnation::~CDlgReincarnation()
{
}

bool CDlgReincarnation::OnInitDialog()
{
	return true;
}

void CDlgReincarnation::OnCommandConfirm(const char* szCommand)
{
	GetGameSession()->c2s_CmdNPCSevReincarnation();
	GetGameUIMan()->EndNPCService();
	Show(false);
}

void CDlgReincarnation::OnShowDialog()
{
	Update();
}
void CDlgReincarnation::OnCommandCancel(const char *szCommand)
{
	Show(false);
	GetGameUIMan()->EndNPCService();
}
void CDlgReincarnation::Update()
{
	CECHostPlayer* pHost = GetHostPlayer();
	if (pHost) {
		bool bCanReincarnate(true);
		int iLevel = pHost->GetBasicProps().iLevel;
		PAUIOBJECT pObj = GetDlgItem("Lbl_CurrentLevel");
		ACString strText;
		strText.Format(_AL("%d"), iLevel);
		if (pObj) pObj->SetText(strText);
		int iReincarnationCount = pHost->GetReincarnationCount();
		a_Clamp(iReincarnationCount, 0, 9);
		pObj = GetDlgItem("Txt_First");
		if (pObj) pObj->SetText(GetStringFromTable(10801 + iReincarnationCount));
		PAUIIMAGEPICTURE pImg = (PAUIIMAGEPICTURE)GetDlgItem("Img_Gem");
		if (pImg) {
			const PLAYER_REINCARNATION_CONFIG* pConfig = pHost->GetReincarnationConfig();
			if (pConfig) {
				int item_id = pConfig->level[iReincarnationCount].require_item;
				CECIvtrItem* pItem = CECIvtrItem::CreateItem(item_id, 0, 1);
				AString strFile;
				af_GetFileTitle(pItem->GetIconFile(), strFile);
				strFile.MakeLower();
				pImg->SetCover(GetGameUIMan()->m_pA2DSpriteIcons[CECGameUIMan::ICONS_INVENTORY],
					GetGameUIMan()->m_IconMap[CECGameUIMan::ICONS_INVENTORY][strFile]);
				bCanReincarnate = pHost->GetPack()->GetItemTotalNum(item_id) > 0;
				pImg->SetColor(bCanReincarnate ? COLOR_WHITE : COLOR_RED);
				AUICTranslate trans;
				const ACHAR * szDesc = pItem->GetDesc();
				if(szDesc) pImg->SetHint(trans.Translate(szDesc));
				else pImg->SetHint(_AL(""));
			}
		}
		pObj = GetDlgItem("Lbl_Condition1");
		if (!pObj) return;
		static A3DCOLOR colorOriginal = pObj->GetColor();
		A3DCOLOR color;
		bool bCondition[4];
		bCondition[0] = iLevel >= 100;
		bCondition[1] = pHost->GetBasicProps().iLevel2 >= 20;
		bCondition[2] = pHost->IsMajorModel(pHost->GetPlayerModel());
		bCondition[3] = bCanReincarnate;
		AString strTemp;
		for (int i = 0; i < 4; ++i) {
			if (bCondition[i]) color = colorOriginal;
			else {
				color = COLOR_RED;
				bCanReincarnate = false;
			}
			strTemp.Format("Lbl_Condition%d", i + 1);
			pObj = GetDlgItem(strTemp);
			if (pObj) pObj->SetColor(color);
		}
		pObj = GetDlgItem("Btn_Start");
		if (pObj) pObj->Enable(bCanReincarnate);
		pObj = GetDlgItem("Lbl_Attribuite");
		if (pObj) {
			int attri_point(0);
			int count = iLevel - 100;
			a_Clamp(count, -1, int(sizeof(ATTRIBUTE_POINT) / sizeof(ATTRIBUTE_POINT[0])));
			for (i = -1; i < count; ++i)
				attri_point += ATTRIBUTE_POINT[i + 1];
			strText.Format(_AL("%d"), attri_point);
			pObj->SetText(strText);
		}
	}
}
