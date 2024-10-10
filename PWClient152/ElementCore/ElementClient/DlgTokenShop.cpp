// Filename	: DlgTokenShop.cpp
// Creator	: Han Guanghui
// Date		: 2013/10/05

#include "DlgTokenShop.h"
#include "ExpTypes.h"
#include "AFI.h"
#include "AUIImagepicture.h"
#include "AUICTranslate.h"
#include "AUIDef.h"
#include "elementdataman.h"
#include "EC_Global.h"
#include "EC_UIManager.h"
#include "EC_GameUIMan.h"
#include "EC_GameSession.h"
#include "EC_HostPlayer.h"
#include "EC_IvtrItem.h"
#include "EC_Inventory.h"
#include "TaskTemplMan.h"
#include "EC_TaskInterface.h"
#include "EC_IvtrItem.h"
#include "EC_IvtrEquip.h"
#include "EC_Configs.h"
#include "EC_Game.h"
#include "EC_GameRun.h"

#define new A_DEBUG_NEW

AUI_BEGIN_COMMAND_MAP(CDlgTokenShop, CDlgBase)
AUI_ON_COMMAND("Btn_Receive*",	OnCommandConfirm)
AUI_ON_COMMAND("IDCANCEL",		OnCommandCancel)
AUI_END_COMMAND_MAP()

AUI_BEGIN_EVENT_MAP(CDlgTokenShop, CDlgBase)
AUI_ON_EVENT("*", WM_MOUSEWHEEL, OnMouseWheel)
AUI_END_EVENT_MAP()

TokenShopItemBase* TokenShopItemBase::CreateOneShopItem(int item_id, ShopItemType type)
{
	TokenShopItemBase* ret = NULL;
	switch(type) {
	case ITEM_BY_TASK: ret = new TokenShopItemByTask; break;
	default: break;
	}
	ret->Init(item_id);
	return ret;
}
TokenShopItemByTask::TokenShopItemByTask():
m_pTaskTempl(NULL)
{

}
TokenShopItemByTask::~TokenShopItemByTask()
{

}
void TokenShopItemByTask::Init(int task_id)
{
	ATaskTemplMan *pMan = g_pGame->GetTaskTemplateMan();
	m_pTaskTempl = pMan->GetTaskTemplByID(task_id);
	if (m_pTaskTempl && !m_pTaskTempl->m_bUsedInTokenShop) m_pTaskTempl = NULL;
}
CECIvtrItem* TokenShopItemByTask::GetItem()
{
	CECIvtrItem* ret = NULL;
	if (m_pTaskTempl 
		&& m_pTaskTempl->m_Award_S->m_ulCandItems != 0
		&& m_pTaskTempl->m_Award_S->m_CandItems) {
		int idItem = m_pTaskTempl->m_Award_S->m_CandItems[0].m_AwardItems[0].m_ulItemTemplId;
		ret = CECIvtrItem::CreateItem(idItem, 0, 1);
		if (ret) ret->GetDetailDataFromLocal();
	}
	return ret;
}
CECIvtrItem* TokenShopItemByTask::GetExchangeItem()
{
	CECIvtrItem* ret = NULL;
	if (m_pTaskTempl 
		&& m_pTaskTempl->m_ulPremItems != 0
		&& m_pTaskTempl->m_PremItems) {
		int idItem = m_pTaskTempl->m_PremItems[0].m_ulItemTemplId;
		ret = CECIvtrItem::CreateItem(idItem, 0, 1);
		if (ret) ret->GetDetailDataFromLocal();
	}
	return ret;
}
ACString TokenShopItemByTask::GetItemNumber()
{
	ACString ret;
	CECGameUIMan* pGameUI = g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan();
	if (m_pTaskTempl && pGameUI
		&& m_pTaskTempl->m_Award_S->m_ulCandItems != 0
		&& m_pTaskTempl->m_Award_S->m_CandItems) {
		int nNum = m_pTaskTempl->m_Award_S->m_CandItems[0].m_AwardItems[0].m_ulItemNum;
		ret.Format(pGameUI->GetStringFromTable(11150), nNum);
	}
	return ret;
}
ACString TokenShopItemByTask::GetItemPrice()
{
	ACString ret;
	CECGameUIMan* pGameUI = g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan();
	if (m_pTaskTempl && pGameUI
		&& m_pTaskTempl->m_ulPremItems != 0
		&& m_pTaskTempl->m_PremItems) {
		int idItem = m_pTaskTempl->m_PremItems[0].m_ulItemTemplId;
		int nNum = m_pTaskTempl->m_PremItems[0].m_ulItemNum;
		if (idItem) {
			CECIvtrItem* pItem = CECIvtrItem::CreateItem(idItem, 0, 1);
			if (pItem) {
				ret.Format(pGameUI->GetStringFromTable(11151), pItem->GetName(), nNum);
				delete pItem;
			}
		}
	}
	return ret;
}
ACString TokenShopItemByTask::GetItemLeft()
{
	ACString ret;
	int time_left = 0;
	CECGameUIMan* pGameUI = g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan();
	CECTaskInterface* pTask = g_pGame->GetGameRun()->GetHostPlayer()->GetTaskInterface();
	if (m_pTaskTempl && pGameUI && pTask) {
		if (m_pTaskTempl->m_lPeriodLimit) {
			unsigned long finish_time = 0;
			unsigned long finish_count = 0;
			if (m_pTaskTempl->m_bAccountTaskLimit){	
				TaskFinishCountList* pFnshList = (TaskFinishCountList*)pTask->GetFinishedCntList();
				finish_count = pFnshList->Search(m_pTaskTempl->m_ID,finish_time);
			} else if (m_pTaskTempl->m_bRoleTaskLimit) {
				FinishedTaskList* pFnshList = (FinishedTaskList*)pTask->GetFinishedTaskList();
				finish_count = pFnshList->SearchTaskFinishCount(m_pTaskTempl->m_ID);
			}
			time_left = static_cast<int>(m_pTaskTempl->m_lPeriodLimit - finish_count);
			ret.Format(pGameUI->GetStringFromTable(11152), time_left);
		} else ret = pGameUI->GetStringFromTable(11153);
	}
	return ret;
}
bool TokenShopItemByTask::BuyItem()
{
	bool ret = false;
	if (CanBuyItem()) {
		CECTaskInterface* pTask = g_pGame->GetGameRun()->GetHostPlayer()->GetTaskInterface();
		pTask->DeliverTokenShopTask(m_pTaskTempl->m_ID);
		ret = true;
	}
	return ret;
}
bool TokenShopItemByTask::CanBuyItem()
{
	bool ret = false;
	if (m_pTaskTempl) {
		CECTaskInterface* pTask = g_pGame->GetGameRun()->GetHostPlayer()->GetTaskInterface();
		ret = pTask->CanDeliverTask(m_pTaskTempl->m_ID) == 0;
	}
	return ret;
}
CDlgTokenShop::CDlgTokenShop():
m_pScl_Right(NULL)
{
}

CDlgTokenShop::~CDlgTokenShop()
{
	ClearTokenShopItem();
}

bool CDlgTokenShop::OnInitDialog()
{
	DDX_Control("Scl_Item", m_pScl_Right);
	return true;
}

void CDlgTokenShop::OnCommandConfirm(const char* szCommand)
{
	PAUIOBJECT pObj = GetDlgItem(szCommand);
	if (pObj) {
		int index = pObj->GetData();
		int start_index = m_pScl_Right->GetBarLevel() * ITEM_PER_LINE;
		TokenShopItemBase* pShopItem = GetOneItem(start_index + index);
		if (pShopItem) pShopItem->BuyItem();
	}
}

void CDlgTokenShop::ScrollPage(bool bUp, int count)
{
	if (!m_pScl_Right->IsShow()) return;
	int level = m_pScl_Right->GetBarLevel();
	int max_level = m_pScl_Right->GetScrollEnd();
	if (bUp) {
		if (level > 0) m_pScl_Right->SetBarLevel(level - 1);
	} else if (level + 1 <= max_level) m_pScl_Right->SetBarLevel(level + 1);
}
void CDlgTokenShop::OnCommandCancel(const char *szCommand)
{
	Show(false);
}

void CDlgTokenShop::OnMouseWheel(WPARAM wParam, LPARAM lParam, AUIObject *pObj)
{
	int zDelta = (short)HIWORD(wParam);
	ScrollPage(zDelta >= 0, 1);
}
void CDlgTokenShop::OnTick()
{
	Update();
}

void CDlgTokenShop::InitTokenShopItem(int token_id)
{
	ClearTokenShopItem();
	DATA_TYPE DataType;
	elementdataman *pDataMan = g_pGame->GetElementDataMan();
	SHOP_TOKEN_ESSENCE* pEssence = (SHOP_TOKEN_ESSENCE*)pDataMan->get_data_ptr(token_id, ID_SPACE_ESSENCE, DataType);
	TokenShopItemBase* pItem = NULL;
	if (DataType == DT_SHOP_TOKEN_ESSENCE) {
		TOKEN_SHOP_CONFIG* pConfig = (TOKEN_SHOP_CONFIG*)pDataMan->get_data_ptr(pEssence->id_token_shop, ID_SPACE_CONFIG, DataType);
		if (DataType == DT_TOKEN_SHOP_CONFIG) {
			unsigned int item_size = sizeof(pConfig->item) / sizeof(pConfig->item[0]);
			for (unsigned int i = 0; i < item_size; ++i) {
				if (pConfig->item[i]) {
					pItem = TokenShopItemBase::CreateOneShopItem(pConfig->item[i], TokenShopItemBase::ITEM_BY_TASK);
					m_ShopItems.push_back(pItem);
				} else break;
			}
		}
	}
	m_pScl_Right->SetBarLevel(0);
	int nMaxDisplayItem = Update();
	int nMaxDisplayLine = nMaxDisplayItem / ITEM_PER_LINE;
	int nLine = ((int)m_ShopItems.size()) / ITEM_PER_LINE + ((m_ShopItems.size() % ITEM_PER_LINE) ? 1 : 0);
	
	if (nLine <= nMaxDisplayLine) {
		m_pScl_Right->SetBarLevel(0);
		m_pScl_Right->Show(false);
	} else {
		m_pScl_Right->SetScrollRange(0, nLine-nMaxDisplayLine);
		m_pScl_Right->SetScrollStep(1);
		m_pScl_Right->SetBarLength(-1);
		m_pScl_Right->Show(true);
	}
}
void CDlgTokenShop::ClearTokenShopItem()
{
	ShopItemContainer::iterator iter = m_ShopItems.begin();
	while(iter != m_ShopItems.end()) {
		delete *iter;
		++iter;
	}
	m_ShopItems.clear();
}
TokenShopItemBase* CDlgTokenShop::GetOneItem(int index)
{
	TokenShopItemBase* ret = NULL;
	if (index < (int)m_ShopItems.size()) ret = m_ShopItems[index];
	return ret;
}
int CDlgTokenShop::Update()
{
	ShopItemContainer::iterator iter = m_ShopItems.begin();
	TokenShopItemBase ItemNull;
	TokenShopItemBase* pShow = NULL;
	int i = 0;
	int start_index = m_pScl_Right->GetBarLevel() * ITEM_PER_LINE;
	bool bNotEnd = false;
	while (true) {
		pShow = GetOneItem(start_index + i);
		if (!pShow) pShow = &ItemNull;
		bNotEnd = UpdateOneItem(pShow, i + 1);
		if (!bNotEnd) break;
		++i;
	}
	return i;
}

bool CDlgTokenShop::UpdateOneItem(TokenShopItemBase* pShopItem, int index)
{
	char szTemp[50];
	PAUIOBJECT pObj(NULL);
	PAUIIMAGEPICTURE pImg(NULL);
	ACString strTemp;
	AString strFile;
	bool ret(false);
	int i(0), j(0),nNum(0), idItem(0);
	CECIvtrItem* pItem = NULL;
	CECIvtrItem* pExchangeItem = NULL;
	AUICTranslate trans;
	ActiveTaskEntry* CurEntry = NULL;
	while (true)
	{
		if (!pShopItem) break;
		pItem = pShopItem->GetItem();
		pExchangeItem = pShopItem->GetExchangeItem();

		sprintf(szTemp, "Txt_ItemName%d", index);
		pObj = GetDlgItem(szTemp);
		if (!pObj) break;
		if (pItem) pObj->SetText(pItem->GetName());
		else pObj->SetText(_AL(""));

		sprintf(szTemp, "Txt_ItemPrice%d", index);
		pObj = GetDlgItem(szTemp);
		if (!pObj) break;
		pObj->SetText(pShopItem->GetItemPrice());
		if (pExchangeItem) {
			pObj->SetHint(trans.Translate(pExchangeItem->GetDesc()));
			delete pExchangeItem;
		} else pObj->SetHint(_AL(""));

		sprintf(szTemp, "Txt_ItemNum%d", index);
		pObj = GetDlgItem(szTemp);
		if (!pObj) break;
		pObj->SetText(pShopItem->GetItemNumber());

		sprintf(szTemp, "Txt_ReceiveTime%d", index);
		pObj = GetDlgItem(szTemp);
		if (!pObj) break;
		pObj->SetText(pShopItem->GetItemLeft());
		
		sprintf(szTemp, "Img_Item%d", index);
		pImg = dynamic_cast<PAUIIMAGEPICTURE>(GetDlgItem(szTemp));
		if (!pImg) break;
		if (pItem) {
			pImg->SetHint(trans.Translate(pItem->GetDesc()));
			af_GetFileTitle(pItem->GetIconFile(), strFile);
			strFile.MakeLower();
			pImg->SetCover(GetGameUIMan()->m_pA2DSpriteIcons[CECGameUIMan::ICONS_INVENTORY],
				GetGameUIMan()->m_IconMap[CECGameUIMan::ICONS_INVENTORY][strFile]);
			delete pItem;
		} else {
			pImg->ClearCover();
			pImg->SetHint(_AL(""));
			pImg->SetText(_AL(""));
		}

		sprintf(szTemp, "Btn_Receive%d", index);
		pObj = GetDlgItem(szTemp);
		if (!pObj) break;
		pObj->Enable(pShopItem->CanBuyItem());
		pObj->SetData(index - 1);
		ret = true;
		break;
	}
	return ret;
}
