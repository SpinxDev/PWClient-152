// Filename	: DlgGeneralCard.cpp
// Creator	: Han Guanghui
// Date		: 2013/09/06

#include "DlgGeneralCard.h"
#include "DlgInventory.h"
#include "DlgDragDrop.h"
#include "DlgBag.h"
#include "DlgAutoHelp.h"
#include "ExpTypes.h"
#include "AFI.h"
#include "AUIDef.h"
#include "A2DSprite.h"
#include "AUIImagepicture.h"
#include "AUIStillImageButton.h"
#include "AUICTranslate.h"
#include "EC_Global.h"
#include "EC_Game.h"
#include "EC_GameUIMan.h"
#include "EC_GameSession.h"
#include "EC_HostPlayer.h"
#include "EC_IvtrItem.h"
#include "EC_Inventory.h"
#include "EC_IvtrItem.h"
#include "EC_IvtrEquip.h"
#include "EC_IvtrEquipMatter.h"
#include "EC_IvtrConsume.h"
#include "EC_ShortcutMgr.h"
#include "EC_FixedMsg.h"
#include "EC_Resource.h"
#include "EC_Configs.h"
#include "EC_Ease.h"
#include "EC_UIHelper.h"

#define new A_DEBUG_NEW

AUI_BEGIN_COMMAND_MAP(CDlgGeneralCard, CDlgBase)
AUI_ON_COMMAND("Btn_Award*",	OnCommandConfirm)
AUI_ON_COMMAND("IDCANCEL",		OnCommandCancel)
AUI_ON_COMMAND("Btn_Cancel",	OnCommandCancel)
AUI_ON_COMMAND("Btn_CardBuy",	OnCommandBuyCard)
AUI_ON_COMMAND("Btn_Storage",	OnCommandStorage)
AUI_ON_COMMAND("Btn_Swallow",	OnCommandSwallow)
AUI_ON_COMMAND("Btn_Volume",	OnCommandVolume)
AUI_ON_COMMAND("Btn_MonsterSpirit", OnCommandMonsterSpirit)
AUI_END_COMMAND_MAP()

AUI_BEGIN_EVENT_MAP(CDlgGeneralCard, CDlgBase)
AUI_ON_EVENT("Img_Card*",	WM_LBUTTONDOWN,		OnEventLButtonDown)
AUI_ON_EVENT("Img_Card*",	WM_LBUTTONDBLCLK,	OnEventLButtonDblclkCard)
AUI_ON_EVENT("Img_Card*",	WM_RBUTTONUP,		OnEventLButtonDblclkCard)
AUI_ON_EVENT("Img_GeneralCardFrame1", WM_LBUTTONDOWN,	OnEventLButtonDown_Left)
AUI_ON_EVENT("Img_GeneralCardFrame2", WM_LBUTTONDOWN,	OnEventLButtonDown_Right)
AUI_END_EVENT_MAP()

static const char* CARD_FRAME_IMG[ELEMENTDATA_NUM_POKER_RANK] = {
	"卡牌\\边框\\blue.dds",
	"卡牌\\边框\\purple.dds",
	"卡牌\\边框\\yellow.dds",
	"卡牌\\边框\\orange.dds",
	"卡牌\\边框\\red.dds",
};
//------------------------------------------------------------------------
//------------------------------------------------------------------------
// Right-Click Shortcut for GeneralCard Storage
//------------------------------------------------------------------------
class GeneralCardStorageClickShortcut : public CECShortcutMgr::ClickShortcut
{
public:
	GeneralCardStorageClickShortcut(const char* pName, CDlgGeneralCard* pDlg)
		:m_pDlg(pDlg)
		,m_GroupName(pName)
	{}
	
	virtual const char* GetGroupName() { return m_GroupName; }
	
	virtual bool CanTrigger(PAUIOBJECT pSrcObj) 
	{
		if(!m_pDlg)
			return false;
		
		PAUIMANAGER pUIMan = m_pDlg->GetAUIManager();
		if( !pUIMan->GetDialog("Win_GeneralCard")->IsShow() ||
			!pUIMan->GetDialog("Win_Storage4")->IsShow())
			return false;
		
		return pSrcObj && pSrcObj->GetDataPtr("ptr_CECIvtrItem");
	}
	
	virtual void Trigger(PAUIOBJECT pSrcObj, int num) 
	{ 
		CECIvtrItem* pItem = (CECIvtrItem*)pSrcObj->GetDataPtr("ptr_CECIvtrItem");
		PAUIOBJECT pObj = NULL;
		CECIvtrGeneralCard* pCard = dynamic_cast<CECIvtrGeneralCard*>(pItem);
		if (pCard) {
			AString strName;
			strName.Format("Img_Card%d", pCard->GetEssence().type + 1);
			pObj = m_pDlg->GetDlgItem(strName);
		}

		CDlgDragDrop* pDragDrop = dynamic_cast<CDlgDragDrop*>(m_pDlg->GetAUIManager()->GetDialog("DragDrop"));
		if (pDragDrop) pDragDrop->OnStorageDragDrop(pSrcObj->GetParent(), pSrcObj, m_pDlg, pObj, pItem);
	}
	
protected:
	CDlgGeneralCard* m_pDlg;
	AString m_GroupName;
};
//------------------------------------------------------------------------

CDlgGeneralCard::CDlgGeneralCard():
m_pImgCard(NULL),
m_pImgFrame(NULL),
m_pImgGfx(NULL),
m_ucParamForSwallow_isinv(0),
m_ucParamForSwallow_swallowed_card_index(0),
m_uiParamForSwallow_count(0),
m_bSwallowing(false),
m_enumDlgType(CARD_EQUIP),
m_pImgCardLeft(NULL),
m_pImgCardRight(NULL),
m_pImgCardLeftFrame(NULL),
m_pImgCardRightFrame(NULL),
m_pImgCardRightOut(NULL),
m_pImgCardLeftOut(NULL),
m_pImgCardLeftFrameOut(NULL),
m_pImgCardRightFrameOut(NULL),
m_ctrl(NULL)
{
	memset(m_pImgEquip, 0, sizeof(m_pImgEquip));
	memset(m_dwDataToDetectChange, 0, sizeof(m_dwDataToDetectChange));
}

CDlgGeneralCard::~CDlgGeneralCard()
{
}
bool CDlgGeneralCard::Release()
{
	OnHideDialog();
	if(m_ctrl)
		A3DRELEASE(m_ctrl);

	return CDlgBase::Release();
}

bool CDlgGeneralCard::OnInitDialog()
{
	int i;
	char szItem[40];
	if (stricmp(GetName(), "Win_GeneralCard") == 0) m_enumDlgType = CARD_EQUIP;
	else if (stricmp(GetName(), "Win_GeneralCardHint") == 0) m_enumDlgType = CARD_CHAT_LINK_ITEM;
	else m_enumDlgType = CARD_MOUSE_HOVER;
	if (m_enumDlgType == CARD_EQUIP) {
		for(i = 0; i < SIZE_GENERALCARD_EQUIPIVTR; ++i) {
			sprintf(szItem, "Img_Card%d", i + 1);
			DDX_Control(szItem, m_pImgEquip[i]);
			m_pImgEquip[i]->SetData(UN_SELECTED);
		}
		m_pImgEquip[0]->SetData(SELECTED);
		if (m_enumDlgType == CARD_EQUIP)
			GetGameUIMan()->GetShortcutMgr()->
				RegisterShortCut(new GeneralCardStorageClickShortcut("Win_Storage4", this));
	}
	
	DDX_Control("Img_GeneralCard", m_pImgCard);
	DDX_Control("Img_CardGfx", m_pImgGfx);
	DDX_Control("Img_CardFrame", m_pImgFrame);

	m_pImgCardLeft = (PAUIIMAGEPICTURE)GetDlgItem("Img_GeneralCard1");
	m_pImgCardRight = (PAUIIMAGEPICTURE)GetDlgItem("Img_GeneralCard2");
	m_pImgCardLeftFrame = (PAUIIMAGEPICTURE)GetDlgItem("Img_GeneralCardFrame1");
	m_pImgCardRightFrame = (PAUIIMAGEPICTURE)GetDlgItem("Img_GeneralCardFrame2");

	m_pImgCardLeftOut = (PAUIIMAGEPICTURE)GetDlgItem("Img_LeftCardOut");
	m_pImgCardRightOut = (PAUIIMAGEPICTURE)GetDlgItem("Img_RightCardOut");
	m_pImgCardLeftFrameOut = (PAUIIMAGEPICTURE)GetDlgItem("Img_CardFrameLeftOut");
	m_pImgCardRightFrameOut = (PAUIIMAGEPICTURE)GetDlgItem("Img_CardFrameRightOut");


	if(CARD_EQUIP == m_enumDlgType)
	{
		m_ctrl = new CECControlAnimControl(this);
		m_ctrl->Init();
	}	

	m_launchAnimCounterNext.SetCounter(0);
	m_launchAnimCounterNext.SetPeriod(800);
	m_launchAnimCounterPrev.SetPeriod(0);
	m_launchAnimCounterPrev.SetPeriod(800);

	return true;
}
void CDlgGeneralCard::OnCommandBuyCard(const char* szCommand)
{
	CECUIHelper::ShowPokerShop(GetDlgItem("Btn_CardBuy"));
}
void CDlgGeneralCard::OnCommandStorage(const char* szCommand)
{
	PAUIDIALOG pDlg = NULL;
	CDlgInventory *pDlgInventory = (CDlgInventory *)(GetGameUIMan()->GetDialog("Win_Inventory"));
	if(pDlgInventory && pDlgInventory->ValidateEquipmentPassword())
	{
		pDlg = m_pAUIManager->GetDialog("Win_Storage4");
		if (pDlg) pDlg->Show(!pDlg->IsShow());
	}
}
void CDlgGeneralCard::OnCommandConfirm(const char* szCommand)
{
}
void CDlgGeneralCard::OnCommandCancel(const char *szCommand)
{
	Show(false);
}
void CDlgGeneralCard::OnCommandSwallow(const char* szCommand)
{
	PAUISTILLIMAGEBUTTON pBtn = dynamic_cast<PAUISTILLIMAGEBUTTON>(GetDlgItem(szCommand));
	if (pBtn) {
		GetGame()->ChangeCursor(RES_CUR_SWALLOW);
		SetSwallowing(true);
	}
}
void CDlgGeneralCard::OnCommandMonsterSpirit(const char* szCommand)
{
	PAUIDIALOG pDlg = GetGameUIMan()->GetDialog("Win_MonsterSpirit");
	if (pDlg) pDlg->Show(!pDlg->IsShow());
}
void CDlgGeneralCard::SetSelectCard(int index)
{
	if (m_enumDlgType != CARD_EQUIP) return;
	for(int i = 0; i < SIZE_GENERALCARD_EQUIPIVTR; ++i) {
		if (i == index)	m_pImgEquip[i]->SetData(SELECTED);
		else m_pImgEquip[i]->SetData(UN_SELECTED);
	}
	UpdateEquipment();
	UpdateDisplay();
}
unsigned char CDlgGeneralCard::GetSelectedCardIndex()
{
	unsigned char ret(EQUIPIVTR_GENERALCARD1);
	for (int i = 0; i < SIZE_GENERALCARD_EQUIPIVTR && m_enumDlgType == CARD_EQUIP; ++i) {
		if (m_pImgEquip[i]->GetData() == SELECTED)
			ret = i;
	}
	return ret;	
}
void CDlgGeneralCard::SendSwallowCardCmd()
{
	if (m_uiParamForSwallow_count > 0){
		GetGameSession()->c2s_CmdSwallowGeneralCard(
			EQUIPIVTR_GENERALCARD1 + GetSelectedCardIndex(), 
			m_ucParamForSwallow_isinv, 
			m_ucParamForSwallow_swallowed_card_index, 
			m_uiParamForSwallow_count);
		m_uiParamForSwallow_count = 0;
	}else{
		ASSERT(false);
	}
}
bool CDlgGeneralCard::SwallowCard(unsigned char is_inv, unsigned char swallowed_card_index, unsigned int count, bool bBatchSwallow)
{
	CECInventory* pInv = is_inv ? GetHostPlayer()->GetPack() : GetHostPlayer()->GetGeneralCardBox();

	CECIvtrItem* pItem = (CECIvtrItem*)pInv->GetItem(swallowed_card_index);
	if (!pItem) return false;
	CECIvtrGeneralCard* pSwallow = dynamic_cast<CECIvtrGeneralCard*>(
		GetHostPlayer()->GetEquipment()->GetItem(EQUIPIVTR_GENERALCARD1 + GetSelectedCardIndex()));
	if (!pSwallow) return false;
	const IVTR_ESSENCE_GENERALCARD& card_data = pSwallow->GetEssence();
	CECIvtrGeneralCard* pCard = dynamic_cast<CECIvtrGeneralCard*>(pItem);
	CECIvtrGeneralCardDice* pDice = dynamic_cast<CECIvtrGeneralCardDice*>(pItem);
	if (!pCard && !pDice) return false;
	// 只能吞噬品阶小于等于自身的卡牌
	if (pCard && card_data.rank < pCard->GetEssence().rank) {
		if (!bBatchSwallow){
			GetGameUIMan()->MessageBox("", GetStringFromTable(10977), MB_OK, A3DCOLORRGBA(255, 255, 255, 160));
		}
		return false;
	}
	// 无法吞噬品阶为S或S+的卡牌
	if (pCard && pCard->GetEssence().rank >= 3) {
		if (!bBatchSwallow){
			GetGameUIMan()->MessageBox("", GetStringFromTable(10982), MB_OK, A3DCOLORRGBA(255, 255, 255, 160));
		}
		return false;
	}
	m_ucParamForSwallow_isinv = is_inv;
	m_ucParamForSwallow_swallowed_card_index = swallowed_card_index;
	m_uiParamForSwallow_count = count;
	int iExpNeed = pSwallow->GetLevelUpExp(card_data.level) - card_data.exp;
	for (int i = card_data.level + 1; i < ELEMENTDATA_MAX_POKER_LEVEL; ++i)
		iExpNeed += pSwallow->GetLevelUpExp(i);
	int iExpSwallow = 0;
	if (pCard) iExpSwallow = pCard->GetSwallowExp();
	else if (pDice) {
		iExpSwallow = pDice->GetDBEssence()->swallow_exp * count;
		// 吞噬这摞箱子总经验大于所需
		if (iExpSwallow > iExpNeed) {
			// 只吞噬所需经验用到的数量箱子
			m_uiParamForSwallow_count = iExpNeed / pDice->GetDBEssence()->swallow_exp;
			if (iExpNeed % pDice->GetDBEssence()->swallow_exp) m_uiParamForSwallow_count++;
			iExpSwallow = m_uiParamForSwallow_count * pDice->GetDBEssence()->swallow_exp;
		}
	}

	if (!bBatchSwallow) {
		ACString strText;
		// 经验溢出是否继续
		if (iExpSwallow > iExpNeed) strText.Format(GetStringFromTable(10978), iExpSwallow - iExpNeed, m_uiParamForSwallow_count);
		// 是否确定吞噬该高品阶卡牌
		if (pCard && pCard->GetEssence().rank > 2) strText += GetStringFromTable(10970);
		if (strText.GetLength() > 0) {
			strText += GetStringFromTable(10979);
			GetGameUIMan()->MessageBox("SwallowGeneralCard", strText, MB_YESNO, A3DCOLORRGBA(255, 255, 255, 160));
			return true;
		}
	}
	if ((m_bSwallowing || bBatchSwallow)) {
		SendSwallowCardCmd();
		return true;
	}
	return false;
}

void CDlgGeneralCard::OnEventLButtonDown(WPARAM wParam, LPARAM lParam, AUIObject *pObj)
{	
	if (m_ctrl && m_ctrl->HasAnimRunning())
		return;

	bool bLinkItem(false);
	CECIvtrItem *pItem = (CECIvtrItem *)pObj->GetDataPtr("ptr_CECIvtrItem");
	if (strstr(pObj->GetName(), "Img_Card") == NULL) return;
	int card_index = atoi(pObj->GetName() + strlen("Img_Card")) - 1;
	if (pItem) {
		if (m_enumDlgType == CARD_EQUIP) {
			if(AUI_PRESS(VK_SHIFT) ) {
				// 按住 SHIFT，向聊天栏输入框添加物品图
				//
				bLinkItem = true;
			} else if( !pItem->IsFrozen() ) {
				A3DVIEWPORTPARAM *p = m_pA3DEngine->GetActiveViewport()->GetParam();
				
				GetGameUIMan()->m_ptLButtonDown.x = GET_X_LPARAM(lParam) - p->X;
				GetGameUIMan()->m_ptLButtonDown.y = GET_Y_LPARAM(lParam) - p->Y;
				GetGameUIMan()->InvokeDragDrop(this, pObj, GetGameUIMan()->m_ptLButtonDown);
				GetGameUIMan()->PlayItemSound((CECIvtrItem *)pObj->GetDataPtr("ptr_CECIvtrItem"), false);
			}
			if (bLinkItem) {
				// 按住 SHIFT，向聊天栏输入框添加装备图
				
				// 装备包裹
				int nPack = IVTRTYPE_EQUIPPACK;
				int nSlot = EQUIPIVTR_GENERALCARD1 + card_index;
				GetGameUIMan()->LinkItem(nPack, nSlot);
			}
		}				
	}

	if(m_ctrl)
		m_ctrl->JumpTo(card_index);
	else
		SetSelectCard(card_index);
}

void CDlgGeneralCard::OnEventLButtonDblclkCard(WPARAM wParam, LPARAM lParam, AUIObject *pObj)
{
	if (GetHostPlayer()->IsTrading() || m_enumDlgType != CARD_EQUIP) return;
	
	CECIvtrItem *pItem = (CECIvtrItem *)pObj->GetDataPtr("ptr_CECIvtrItem");
	
	if(!pItem || !strstr(pObj->GetName(), "Img_Card") || pItem->IsFrozen())
		return;
	
	int iSrc = EQUIPIVTR_GENERALCARD1 + atoi(pObj->GetName() + strlen("Img_Card")) - 1;
	
	CDlgInventory *pDlgInventory = (CDlgInventory *)(GetGameUIMan()->GetDialog("Win_Inventory"));
	if (!pDlgInventory->ValidateEquipmentTakeOff(iSrc))
		return;
	
	// try to trigger shortcuts
	if(!GetGameUIMan()->GetShortcutMgr()->TriggerShortcuts(this->GetName(), pObj)&&!CDlgAutoHelp::IsAutoHelp())
		GetHostPlayer()->UseItemInPack(IVTRTYPE_EQUIPPACK, iSrc);
}

void CDlgGeneralCard::OnShowDialog()
{
	Update();
	m_launchAnimCounterNext.Reset();
	m_launchAnimCounterPrev.Reset();
	if(m_ctrl)
		m_ctrl->Reset();

	if (m_enumDlgType == CARD_EQUIP){
		CECUIHelper::CheckBuyPokerShop(this, "Btn_CardBuy");
	}
}
bool CDlgGeneralCard::CanAnimateCard()
{
	if (m_bSwallowing || GetGameUIMan()->m_bRepairing)
		return false;
	else
		return true;
}
void CDlgGeneralCard::OnTick()
{
	DWORD dt = g_pGame->GetRealTickTime();

	if (m_enumDlgType == CARD_EQUIP) {
		UpdateEquipment();
		if (IsCardChanged()) 
		{
			UpdateDisplay();
			if(m_ctrl)
				m_ctrl->Reset();
		}
	}

	if (m_ctrl)
	{
		if (CanAnimateCard())
		{
			PAUIIMAGEPICTURE pTargetCard = NULL;
			GetGameUIMan()->GetMouseOn(NULL, (PAUIOBJECT*)(&pTargetCard));
			if( pTargetCard && !stricmp(pTargetCard->GetName(),"Img_GeneralCardFrame2") )
			{
				if (m_launchAnimCounterNext.IncCounter(dt))
				{
					m_ctrl->Next();
					m_launchAnimCounterNext.Reset();
				}			
			}
			else if (pTargetCard && !stricmp(pTargetCard->GetName(),"Img_GeneralCardFrame1"))
			{
				if (m_launchAnimCounterPrev.IncCounter(dt))
				{
					m_ctrl->Prev();
					m_launchAnimCounterPrev.Reset();
				}			
			}
			else
			{
				m_launchAnimCounterPrev.Reset();
				m_launchAnimCounterNext.Reset();
			}
		}		
		
		m_ctrl->Tick(dt);
	}
	

	m_pImgGfx->Show(m_ctrl !=NULL ? !m_ctrl->HasAnimRunning():true);
}
bool CDlgGeneralCard::IsCardChanged()
{
	bool ret(false);
	for (int i = 0; i < SIZE_GENERALCARD_EQUIPIVTR && m_enumDlgType == CARD_EQUIP; ++i) {
		DWORD dwTemp = (DWORD)m_pImgEquip[i]->GetDataPtr("ptr_CECIvtrItem");
		if (m_dwDataToDetectChange[i] != dwTemp) {
			ret = true;
			m_dwDataToDetectChange[i] = dwTemp;
		}
	}
	return ret;
}
void CDlgGeneralCard::OnItemDragDrop(CECIvtrItem* pIvtrSrc, int iSrc, PAUIOBJECT pObjSrc, PAUIOBJECT pObjOver)
{
	if (pIvtrSrc->GetClassID() == CECIvtrItem::ICID_GENERALCARD) {
		if( strstr(pObjOver->GetName(), "Img_Card") )
		{
			CECIvtrItem *pIvtrDst = (CECIvtrItem *)pObjOver->GetDataPtr("ptr_CECIvtrItem");
			
			int iDst = EQUIPIVTR_GENERALCARD1 + atoi(pObjOver->GetName() + strlen("Img_Card")) - 1;
			
			// Equipment exchange validation (from Inventory Item to Inventory Equipment)
			CDlgInventory *pDlgInventory = (CDlgInventory *)(GetGameUIMan()->GetDialog("Win_Inventory"));
			if (!pDlgInventory->ValidateEquipmentExchange(iSrc, pIvtrSrc, iDst))
				return;
			
			else if( pIvtrSrc->CanEquippedTo(iDst) && 
				pIvtrSrc->IsEquipment() && GetHostPlayer()->CanUseEquipment((CECIvtrEquip*)pIvtrSrc)) {
				if( pIvtrSrc->GetProcType() & CECIvtrItem::PROC_BIND ) {
					PAUIDIALOG pMsgBox;
					GetGameUIMan()->MessageBox("Game_EquipBind1", GetStringFromTable(872), 
						MB_YESNO, A3DCOLORRGBA(255, 255, 255, 160), &pMsgBox);
					pMsgBox->SetData(iSrc);
					pMsgBox->SetDataPtr((void*)iDst);
				} else {
					//booth certification
					if (24 == iDst && GetHostPlayer()->GetBoothState() != 0) {
						//maybe need to add fixedmessage
					} else {
						g_pGame->GetGameSession()->c2s_CmdEquipItem(iSrc, iDst);
						GetGameUIMan()->PlayItemSound(pIvtrSrc, true);
					}
				}
			}
		}
	}
}
void CDlgGeneralCard::SetCardImage(CECIvtrItem* pItem)
{
	CECIvtrGeneralCard* pCard = dynamic_cast<CECIvtrGeneralCard*>(pItem);
	if (pCard) {
		const POKER_ESSENCE* pConfig = pCard->GetDBEssence();
		int rank = pCard->GetEssence().rank;
		a_Clamp(rank, 0, ELEMENTDATA_NUM_POKER_RANK - 1);
		if (pConfig) SetImagePath(pConfig->file_img, CARD_FRAME_IMG[rank], pConfig->file_gfx);
	} else Show(false);
}
void CDlgGeneralCard::SetImagePath(const char* path_card, const char* path_frame, const char* path_gfx)
{
	SetImgCover(m_pImgCard, path_card);
	SetImgCover(m_pImgFrame, path_frame);
	m_pImgGfx->SetGfx(path_gfx);
}
void CDlgGeneralCard::SetImgCover(PAUIIMAGEPICTURE pImg, const char* path_img)
{
	int nItem;
	A2DSprite *pOldA2DSprite = NULL;
	pImg->GetCover(&pOldA2DSprite, &nItem);
	A3DRELEASE(pOldA2DSprite);
	pImg->ClearCover();
	if (path_img && strlen(path_img) != 0) {
		A2DSprite* p2DSprite = new A2DSprite;
		if (p2DSprite->Init(g_pGame->GetA3DEngine()->GetA3DDevice(), path_img, 0))
			p2DSprite->SetLinearFilter(true);		
		else {
			delete p2DSprite;
			p2DSprite = NULL;
		}
		pImg->SetCover(p2DSprite, 0);
	}
}

void CDlgGeneralCard::SetCardImage(CECIvtrItem *pItem, PAUIIMAGEPICTURE pImgCard, PAUIIMAGEPICTURE pImgFrame, PAUIIMAGEPICTURE pImgGfx)
{
	if (CECIvtrGeneralCard* pCard = dynamic_cast<CECIvtrGeneralCard*>(pItem)){
		if (const POKER_ESSENCE* pConfig = pCard->GetDBEssence()){
			int rank = pCard->GetDBEssence()->rank;
			a_Clamp(rank, 0, ELEMENTDATA_NUM_POKER_RANK - 1);
			if (pImgCard){
				SetImgCover(pImgCard, pConfig->file_img);
			}
			if (pImgFrame){
				SetImgCover(pImgFrame, CARD_FRAME_IMG[rank]);
			}
			if (pImgGfx){
				pImgGfx->SetGfx(pConfig->file_gfx);
			}
			return;
		}
	}
	if (pImgCard){
		SetImgCover(pImgCard, NULL);
	}
	if (pImgFrame){
		SetImgCover(pImgFrame, NULL);
	}
	if (pImgGfx){
		pImgGfx->SetGfx("");
	}
}

void CDlgGeneralCard::SetCardImage(int tid, PAUIIMAGEPICTURE pImgCard, PAUIIMAGEPICTURE pImgFrame, PAUIIMAGEPICTURE pImgGfx)
{
	CECIvtrItem * pItem = NULL;
	if (tid > 0){
		pItem = CECIvtrItem::CreateItem(tid, 0, 1);
	}
	SetCardImage(pItem, pImgCard, pImgFrame, pImgGfx);
	delete pItem;
}

void CDlgGeneralCard::SetSwallowing(bool swallow)
{
	m_bSwallowing = swallow;
	PAUISTILLIMAGEBUTTON pBtn = dynamic_cast<PAUISTILLIMAGEBUTTON>(GetDlgItem("Btn_Swallow"));
	if (pBtn) pBtn->SetPushed(swallow);
}

void CDlgGeneralCard::OnHideDialog()
{
	// 关闭界面时释放掉图片和gfx以免浪费内存
	SetImagePath(NULL, NULL, NULL);
	if (m_enumDlgType == CARD_EQUIP) SetSwallowing(false);

	if(m_ctrl)
		m_ctrl->ReleaseAnim();
}
void CDlgGeneralCard::Update()
{
	UpdateEquipment();
	UpdateDisplay();
}
#define SET_PROP(prop, index) \
{\
	if (prop) {\
	strNum.Format(GetStringFromTable(10950 + index), prop);\
	strText += strNum;\
	strText += _AL("\r");\
	}\
}
#define SET_ZERO_PROP(prop, index) \
{\
	if (prop) {\
	strNum.Format(GetStringFromTable(10950 + index), 0);\
	strText += strNum;\
	strText += _AL("\r");\
	}\
}
CECIvtrGeneralCard* CDlgGeneralCard::GetCardByIndex(int i)
{
	if (m_enumDlgType != CARD_EQUIP) return NULL;
	if(m_pImgEquip[i] == NULL) return NULL;

	CECIvtrGeneralCard* pCard = (CECIvtrGeneralCard*)m_pImgEquip[i]->GetDataPtr("ptr_CECIvtrItem");

	return pCard;
}
void CDlgGeneralCard::UpdateDisplay()
{
	if (m_enumDlgType != CARD_EQUIP) return;
	PAUIIMAGEPICTURE pImg = NULL;
	CECStringTab* pDescTab = g_pGame->GetItemDesc();
	ACString strNum, strText, strRed(_AL("^FF0000")), strNormal(_AL("^00FFDE"));
	AString strName;
	AUICTranslate trans;
	PAUIOBJECT pObj = NULL;
	PAUIOBJECT pName = GetDlgItem("Txt_Name");
	PAUIOBJECT pNameCard = GetDlgItem("Txt_CardName");
	PAUIOBJECT pLevel = GetDlgItem("Txt_Level");
	PAUIOBJECT pLevelCard = GetDlgItem("Txt_CardLevel");
	PAUIOBJECT pExp = GetDlgItem("Txt_Exp");
	PAUIOBJECT pExpCard = GetDlgItem("Txt_CardExp");
	PAUIOBJECT pAttribute = GetDlgItem("Txt_Attribute");
	PAUIOBJECT pAttributeCard = GetDlgItem("Txt_CardAttribute");
	CECHostPlayer* pHost = GetHostPlayer();
	for (int i = 0; i < SIZE_GENERALCARD_EQUIPIVTR; ++i) {
		CECIvtrGeneralCard* pCard = (CECIvtrGeneralCard*)m_pImgEquip[i]->GetDataPtr("ptr_CECIvtrItem");
		if (pCard) {
			CECInventory* pInv = pHost->GetEquipment();
			const wchar_t *pszHint = pCard->GetDesc(CECIvtrItem::DESC_NORMAL, pInv);
			if( pszHint) {
				AWString strHint = trans.Translate(pszHint);
				m_pImgEquip[i]->SetHint(strHint);
			}
		} 
		if (m_pImgEquip[i]->GetData() == SELECTED){
			const PLAYER_SPIRIT_CONFIG* pSpirit = pHost->GetPlayerSpiritConfig(i);
			const CECHostPlayer::CARD_HOLDER& holder = pHost->GetCardHolder();
			int max_holder_level = CECHostPlayer::CARD_HOLDER::max_holder_level;
			int holder_level = holder.level[i];
			if (holder_level < 0 || holder_level > max_holder_level) continue;
			
			if (pName) pName->SetText(GetStringFromTable(390 + i));
			if (pLevel) {
				strText.Format(_AL("%d"), holder_level);
				pLevel->SetText(strText);
			}
			if (pExp) {
				int levelup_exp = holder_level < max_holder_level ? 
					pSpirit->list[holder_level].require_power : 0;
				if (levelup_exp == 0) strText.Format(_AL("-"));
				else strText.Format(_AL("%d/%d"), holder.exp[i], levelup_exp);
				pExp->SetText(strText);
			}
			if (pAttribute) {
				strText.Empty();
				strText += pDescTab->GetWideString(ITEMDESC_COL_DARKGOLD);
				if (holder_level == 0) {
					SET_ZERO_PROP(pSpirit->hp, 0);
					SET_ZERO_PROP(pSpirit->damage, 1);
					SET_ZERO_PROP(pSpirit->magic_damage, 2);
					SET_ZERO_PROP(pSpirit->defence, 3);
					SET_ZERO_PROP(pSpirit->magic_defence[0], 4);
					SET_ZERO_PROP(pSpirit->vigour, 5);					
				} else {
					float gainratio = pSpirit->list[holder_level - 1].gain_ratio;
					gainratio *= CECHostPlayer::CARD_HOLDER::GetProfessionRatio(i);
					SET_PROP(static_cast<int>(pSpirit->hp * gainratio + .5f), 0);
					SET_PROP(static_cast<int>(pSpirit->damage * gainratio + .5f), 1);
					SET_PROP(static_cast<int>(pSpirit->magic_damage * gainratio + .5f), 2);
					SET_PROP(static_cast<int>(pSpirit->defence * gainratio + .5f), 3);
					SET_PROP(static_cast<int>(pSpirit->magic_defence[0] * gainratio + .5f), 4);
					SET_PROP(static_cast<int>(pSpirit->vigour * gainratio + .5f), 5);
				}
				pAttribute->SetText(strText);
				if (holder_level < max_holder_level) {
					strText.Empty();
					strText += pDescTab->GetWideString(ITEMDESC_COL_DARKGOLD);
					strText += GetStringFromTable(10983);
					float gainratio = pSpirit->list[holder_level].gain_ratio;
					gainratio *= CECHostPlayer::CARD_HOLDER::GetProfessionRatio(i);
					SET_PROP(static_cast<int>(pSpirit->hp * gainratio + .5f), 0);
					SET_PROP(static_cast<int>(pSpirit->damage * gainratio + .5f), 1);
					SET_PROP(static_cast<int>(pSpirit->magic_damage * gainratio + .5f), 2);
					SET_PROP(static_cast<int>(pSpirit->defence * gainratio + .5f), 3);
					SET_PROP(static_cast<int>(pSpirit->magic_defence[0] * gainratio + .5f), 4);
					SET_PROP(static_cast<int>(pSpirit->vigour * gainratio + .5f), 5);

					ACString strRequireLevel;
					strRequireLevel.Format(GetStringFromTable(10985), pSpirit->list[holder_level].require_level);
					strText += strRequireLevel;

					pAttribute->SetHint(strText);					
				} else pAttribute->SetHint(_AL(""));
			}
			pObj = GetDlgItem("Btn_Swallow");
			if (pObj) { 
				pObj->Enable(pCard != NULL && pCard->GetEssence().level < pCard->GetEssence().max_level);
				if (!pObj->IsEnabled() && IsSwallowing()) SetSwallowing(false);
			}
			m_pImgEquip[i]->SetCover(GetGameUIMan()->m_pA2DSpriteMask, 0, 1);
			if (pCard) {
				const IVTR_ESSENCE_GENERALCARD& card_data = pCard->GetEssence();
				const POKER_ESSENCE* pConfig = pCard->GetDBEssence();
				int rank = card_data.rank;
				a_Clamp(rank, 0, ELEMENTDATA_NUM_POKER_RANK - 1);
				SetImagePath(pConfig->file_img, CARD_FRAME_IMG[rank], pConfig->file_gfx);
				bool bCardFit = i == card_data.type;
				if (pNameCard) {
					strText = (bCardFit ? strNormal : strRed) + GetStringFromTable(360 + card_data.type);
					pNameCard->SetText(strText);
				}
				if (pLevelCard) {
					strText.Format(_AL("%d/%d"), card_data.level, card_data.max_level);
					pLevelCard->SetText(strText);
				}
				if (pExpCard) {
					int levelup_exp = pCard->GetLevelUpExp(card_data.level);
					if (levelup_exp == 0) strText.Format(_AL("-"));
					else strText.Format(_AL("%d/%d"), card_data.exp, levelup_exp);
					pExpCard->SetText(strText);
				}
				if (pAttributeCard) {
					strText.Empty();
					strText += pDescTab->GetWideString(ITEMDESC_COL_DARKGOLD);
					int hp(0), damage(0), magic_damage(0), defense(0), magic_defense(0), vigour(0);
					if (bCardFit) {
						hp = pCard->CalcGeneralCardProp(pConfig->hp, pConfig->inc_hp);
						damage = pCard->CalcGeneralCardProp(pConfig->damage, pConfig->inc_damage);
						magic_damage = pCard->CalcGeneralCardProp(pConfig->magic_damage, pConfig->inc_magic_damage);
						defense = pCard->CalcGeneralCardProp(pConfig->defence, pConfig->inc_defence);
						magic_defense = pCard->CalcGeneralCardProp(pConfig->magic_defence[0], pConfig->inc_magic_defence[0]);
						vigour = pCard->CalcGeneralCardProp(pConfig->vigour, pConfig->inc_vigour);

					} else {
						float ratio = pCard->GetRankRatio() * pCard->GetRebirthRatio() * CECHostPlayer::CARD_HOLDER::GetProfessionRatio(i);;
						hp = static_cast<int>(CECHostPlayer::CARD_HOLDER::hp[i] * ratio + .5f);
						damage = static_cast<int>(CECHostPlayer::CARD_HOLDER::damage[i] * ratio + .5f);
						magic_damage = static_cast<int>(CECHostPlayer::CARD_HOLDER::magic_damage[i] * ratio + .5f);
						defense = static_cast<int>(CECHostPlayer::CARD_HOLDER::defense[i] * ratio + .5f);
						magic_defense = static_cast<int>(CECHostPlayer::CARD_HOLDER::magic_defense[i] * ratio + .5f);
						vigour = static_cast<int>(CECHostPlayer::CARD_HOLDER::vigour[i] * ratio + .5f);
					}
					SET_PROP(hp, 0);
					SET_PROP(damage, 1);
					SET_PROP(magic_damage, 2);
					SET_PROP(defense, 3);
					SET_PROP(magic_defense, 4);
					SET_PROP(vigour, 5);
					pAttributeCard->SetText(strText);
				}
			} else {
				SetImagePath(NULL, NULL, NULL);
				if (pNameCard) {
					pNameCard->SetText(_AL(""));
					pNameCard->SetColor(A3DCOLORRGB(0, 255, 222));
				}
				if (pLevelCard) pLevelCard->SetText(_AL(""));
				if (pExpCard) pExpCard->SetText(_AL(""));
				if (pAttributeCard) pAttributeCard->SetText(_AL(""));
			}
		} else m_pImgEquip[i]->SetCover(NULL, 0, 1);
	}
	pObj = GetDlgItem("Txt_LeadShip");
	if (pObj) {
		const CECHostPlayer::GENERAL_CARD& card_data = pHost->GetGeneralCardData();
		strText.Format(_AL("%d/%d"), pHost->GetUsedLeaderShip(), card_data.leader_ship);
		pObj->SetText(strText);
	}					
}
void CDlgGeneralCard::UpdateEquipment()
{
	if (m_enumDlgType != CARD_EQUIP) return;
	int i;
	AString strFile;
	CECIvtrEquip *pEquip;
	PAUIIMAGEPICTURE pObj;
	CECInventory *pInventory;
	CECIvtrItem *pItem, *pDrag = NULL;
	PAUIDIALOG pDlgDrag = m_pAUIManager->GetDialog("DragDrop");

	if(pDlgDrag->IsShow()) {
		PAUIOBJECT pCell = pDlgDrag->GetDlgItem("Goods");
		PAUIOBJECT pObjSrc = (PAUIOBJECT)pCell->GetDataPtr("ptr_AUIObject");
		PAUIDIALOG pDlgSrc = pObjSrc->GetParent();

		if (pDlgSrc == this ||
			pDlgSrc == GetGameUIMan()->m_pDlgBag)
			pDrag = (CECIvtrItem *)pObjSrc->GetDataPtr("ptr_CECIvtrItem");
	}
	CECHostPlayer* pHost = GetHostPlayer();
	pInventory = pHost->GetEquipment();

	for(i = 0; i < SIZE_GENERALCARD_EQUIPIVTR; i++) { 
		pObj = m_pImgEquip[i];
		if(!pObj) continue;
		int index_card = EQUIPIVTR_GENERALCARD1 + i;
		pItem = pInventory->GetItem(index_card);
		if(pItem) {
			pEquip = (CECIvtrEquip *)pItem;
			if( m_enumDlgType == CARD_EQUIP && (!pHost->CanUseEquipment(pEquip) 
				|| pHost->IsEquipDisabled(index_card)) )
				pObj->SetColor(A3DCOLORRGB(192, 0, 0));
			else
				pObj->SetColor(A3DCOLORRGB(255, 255, 255));
			
			if (pObj->GetColor() == A3DCOLORRGB(255, 255, 255) &&
				pEquip->IsDestroying())
				pObj->SetColor(A3DCOLORRGB(128, 128, 128));

			pObj->SetDataPtr(pItem,"ptr_CECIvtrItem");

			af_GetFileTitle(pItem->GetIconFile(), strFile);
			strFile.MakeLower();
			pObj->SetCover(GetGameUIMan()->m_pA2DSpriteIcons[CECGameUIMan::ICONS_INVENTORY],
				GetGameUIMan()->m_IconMap[CECGameUIMan::ICONS_INVENTORY][strFile]);
		} else {
			pObj->SetCover(NULL, 0, 0);
			pObj->SetText(_AL(""));
			pObj->SetDataPtr(NULL);
			pObj->SetColor(A3DCOLORRGB(255, 255, 255));
			pObj->SetHint(GetStringFromTable(390 + i));
			if (pObj->GetData() == SELECTED)
				pObj->SetCover(GetGameUIMan()->m_pA2DSpriteMask, 0, 1);
	
		}
	}
}

void CDlgGeneralCard::OnCommandVolume(const char* szCommand)
{
	PAUIDIALOG dlg = GetGameUIMan()->GetDialog("Win_PokerVolume");
	if(dlg)
		dlg->Show(!dlg->IsShow());
}

const char* CDlgGeneralCard::GetCardFrameFile(unsigned int rank)
{
	if(rank>= sizeof(CARD_FRAME_IMG) ) return NULL;
	return CARD_FRAME_IMG[rank];
}

void CDlgGeneralCard::OnEventLButtonDown_Left(WPARAM wParam, LPARAM lParam, AUIObject *pObj)
{
	bool bCanDo = m_ctrl !=NULL ? !m_ctrl->HasAnimRunning():false;
	if(bCanDo)
		m_ctrl->Prev();
}
void CDlgGeneralCard::OnEventLButtonDown_Right(WPARAM wParam, LPARAM lParam, AUIObject *pObj)
{
	bool bCanDo = m_ctrl !=NULL ? !m_ctrl->HasAnimRunning():false;
	if(bCanDo)
		m_ctrl->Next();
}
bool CDlgGeneralCard::OnChangeLayoutBegin()
{
	if (m_ctrl)
	{
		m_ctrl->ReleaseAnim();
		m_ctrl->RecoverControl();	
		m_ctrl->ApplyCardImgToControl(true);
	}
	return true;
}
void CDlgGeneralCard::OnChangeLayoutEnd(bool bAllDone)
{
	if (m_ctrl)
	{
		m_ctrl->Init();
	}
}
//////////////////////////////////////////////////////////////////////////

POINT_f& POINT_f::operator=(const POINT& t)
{
	x = (float)t.x;
	y = (float)t.y;

	return *this;
}
POINT_f& POINT_f::operator=(const SIZE& t)
{
	x = (float)t.cx;
	y = (float)t.cy;
	
	return *this;
}

POINT_f operator+(POINT_f s,POINT_f d)
{
	POINT_f r;
	r.x = s.x + d.x;
	r.y = s.y + d.y;

	return r;
}

POINT_f operator-(POINT_f s,POINT_f d)
{
	POINT_f r;
	r.x = s.x - d.x;
	r.y = s.y - d.y;

	return r;
}
POINT_f operator*(POINT_f s, float k)
{
	POINT_f r;
	r.x = s.x * k;
	r.y = s.y * k;

	return r;
}

bool operator==(POINT_f s, POINT_f d)
{
	if (fabs(s.x - d.x) <=1.f && fabs(s.y - d.y)<=1.f)
		return true;

	return false;
}

CECControlAnimation::CECControlAnimation(int moveType, DWORD dwTimeCost):m_bFinished(false),m_timeElapsed(0),m_moveType(moveType),m_timeCost(dwTimeCost)
{
}

void CECControlAnimation::AddControl(PAUIOBJECT src,PAUIOBJECT des)
{
	AddControl(src, des->GetPos(true), des->GetDefaultSize());
}

void CECControlAnimation::AddControl(PAUIOBJECT src, const POINT &desPos, const SIZE &desSize)
{
	m_pSrc = src;
	
	m_start = src->GetPos(true);
	m_end = desPos;
	m_delta = m_end - m_start;
	m_cur = m_start;

	m_startSize = src->GetDefaultSize();
	m_endSize = desSize;
	m_curSize = m_startSize;
	m_deltaSize = m_endSize - m_startSize;
}

void CECControlAnimation::ChangeMoveType(int newMoveType)
{
	if (m_moveType == newMoveType){
		return;
	}
	if (!CanFinish()){
		//	切换时、保持当前位置
		float t = GetMoveRatio();
		DWORD timeElapsed = 0;
		while (timeElapsed < m_timeCost)
		{
			float r = GetMoveRatio(newMoveType, timeElapsed, m_timeCost);
			if (r >= t){
				break;
			}
			timeElapsed = GetElapsed(newMoveType, timeElapsed, 10);
		}
		m_timeElapsed = timeElapsed;
	}
	m_moveType = newMoveType;
}

float CECControlAnimation::GetMoveRatio(int moveType, DWORD timeElapsed, DWORD timeCost)const
{
	float t = 1.f;
	switch (moveType){
	case ANIM_MOVE_EXP:
		t = timeElapsed  / (float)timeCost;
		t = Ease::ExponentialOut(t);
		break;
	case ANIM_MOVE_ACC:
	case ANIM_MOVE_FAST:
		t = timeElapsed  / (float)timeCost;
		break;
	default:
		ASSERT(false);
		break;
	}
	return t;
}

DWORD CECControlAnimation::GetElapsed(int moveType, DWORD timeElapsed, DWORD dwDelta)const
{
	timeElapsed += dwDelta;
	switch (moveType)
	{
	case ANIM_MOVE_ACC:
		timeElapsed *= 2;
		break;
	case ANIM_MOVE_FAST:
		timeElapsed += dwDelta*2;
		break;
	}
	return timeElapsed;
}

DWORD CECControlAnimation::GetElapsed(DWORD dwDelta)const
{
	return GetElapsed(m_moveType, m_timeElapsed, dwDelta);
}

float CECControlAnimation::GetMoveRatio()const
{
	return GetMoveRatio(m_moveType, m_timeElapsed, m_timeCost);
}

bool CECControlAnimation::Tick(DWORD dwDelta)
{
	if(m_bFinished) return true;

	m_timeElapsed = GetElapsed(dwDelta);

	float t = GetMoveRatio();
	m_cur = m_start + m_delta * t;
	m_curSize = m_startSize + m_deltaSize * t;

	if (CanFinish())
	{
		m_bFinished = true;
		m_pSrc->SetPos((int)m_end.x,(int)m_end.y);
		m_pSrc->SetDefaultSize((int)m_endSize.x,(int)m_endSize.y);
		m_cur = m_end;
		m_curSize = m_endSize;
		return true; // finished
	}
	
	m_pSrc->SetPos((int)m_cur.x,(int)m_cur.y);
	m_pSrc->SetDefaultSize((int)m_curSize.x,(int)m_curSize.y);
	
	return false;
}
bool CECControlAnimation::CanFinish()
{
/*	if (m_type == ANIM_TYPE_POS)
	{
		if (m_cur == m_end)
			return true;
		if ((m_start.x - m_end.x ) * (m_cur.x - m_end.x) < 0 || (m_start.y - m_end.y ) * (m_cur.y - m_end.y) < 0)
			return true;
	}
	else
	{
		if (m_curSize == m_endSize)
			return true;
		if ((m_startSize.x - m_endSize.x ) * (m_curSize.x - m_endSize.x) < 0 || (m_startSize.y - m_endSize.y ) * (m_curSize.y - m_endSize.y) < 0)
			return true;
	}*/
	return m_timeElapsed>= m_timeCost;
}
//////////////////////////////////////////////////////////////////////////
void CECUIControlData::Register(PAUIOBJECT obj)
{
	if(!obj) return;

	m_ControlPos[obj] = obj->GetPos(true);

	m_ControlSize[obj] = obj->GetDefaultSize();
}

void CECUIControlData::UpdateUI()
{
	abase::hash_map<PAUIOBJECT,POINT>::iterator itrPos = m_ControlPos.begin();
	for(;itrPos!=m_ControlPos.end();++itrPos)
	{
		itrPos->first->SetPos(itrPos->second.x,itrPos->second.y);
	}
	abase::hash_map<PAUIOBJECT,SIZE>::iterator itrSize = m_ControlSize.begin();
	for (;itrSize!=m_ControlSize.end();++itrSize)
	{
		itrSize->first->SetDefaultSize(itrSize->second.cx,itrSize->second.cy);
	}
}

void CECCardIndexSelector::UpdateRealCardIndex(CDlgGeneralCard* pDlg)
{
	m_realCardIndex.clear();
	memset(m_realdCardIndexMap,0,sizeof(m_realdCardIndexMap));
	for (int i=0;i<SIZE_GENERALCARD_EQUIPIVTR;i++)
	{
	//	if (pDlg->GetCardByIndex(i)) // 策划要求没有卡牌的位置也要显示
		{
			m_realCardIndex.push_back(i);
			m_realdCardIndexMap[i] = m_realCardIndex.size()-1;
		}
	}

	if (m_realCardIndex.size())
	{
		m_curIndex = 0;
	}
	else m_curIndex = -1;
}
bool CECCardIndexSelector::MovePrev()
{
	if(!IsValid() || m_realCardIndex.size()<=1) return false;

	m_curIndex = m_curIndex-1;
	if(m_curIndex<0)
		m_curIndex += (int)m_realCardIndex.size();

	return true;
}
bool CECCardIndexSelector::MoveNext()
{
	if(!IsValid() || m_realCardIndex.size()<=1) return false;

	m_curIndex = (m_curIndex+1) % m_realCardIndex.size();

	return true;
}
int CECCardIndexSelector::GetCardPrevSib(int card)
{
	if(m_realCardIndex.size()==0 || card >= SIZE_GENERALCARD_EQUIPIVTR) return -1;

	int temp = m_realdCardIndexMap[card];

	temp = (temp >=1) ? (temp-1):(temp-1+m_realCardIndex.size());

	temp = temp % m_realCardIndex.size();

	return m_realCardIndex[temp];
}
int CECCardIndexSelector::GetCardNextSib(int card)
{
	if(m_realCardIndex.size()==0 || card >= SIZE_GENERALCARD_EQUIPIVTR) return -1;

	card = m_realdCardIndexMap[card];

	int temp = (card+1) % m_realCardIndex.size();

	return m_realCardIndex[temp];
}
int CECCardIndexSelector::GetCurCardIndex() const
{
	if (m_curIndex<(int)m_realCardIndex.size() && m_curIndex>=0)
	{
		return m_realCardIndex[m_curIndex];
	}
	return -1;
}
bool CECCardIndexSelector::ComputeMoveTimes(int destIndex,int& times)
{
	unsigned int i=0;
	// 转换索引
	for ( i=0;i<m_realCardIndex.size();i++)
	{
		if(m_realCardIndex[i] == destIndex)
		{
			destIndex = i;
			break;
		}
	}
	if (i == m_realCardIndex.size())
	{
		times = 0;
		return false;
	}

	if (destIndex>m_curIndex)
	{
		int a1 = destIndex-m_curIndex;
		int a2 = m_curIndex + SIZE_GENERALCARD_EQUIPIVTR - destIndex;
		if (a1<a2)
		{
			times = a1;
			return true; // forwards
		}
		else
		{
			times = a2;
			return false; // backwards
		}		
	}
	else
	{
		int a1 = m_curIndex - destIndex;
		int a2 = destIndex + SIZE_GENERALCARD_EQUIPIVTR - m_curIndex;
		if (a1<a2)
		{
			times = a1;
			return false; // backwards
		}
		else
		{
			times = a2;
			return true; // forwards
		}
	}	
}

CECControlAnimControl::CECControlAnimControl(CDlgGeneralCard* pDlg){ m_pDlg = pDlg;};
void CECControlAnimControl::Init()
{
	m_pNewViewport=new A3DViewport;
	A3DVIEWPORTPARAM parm = { 0,0,0,0,0.0f,1.0f };
	bool bret=m_pNewViewport->Init(g_pGame->GetA3DDevice(), &parm, false, false, 0);
	if(!bret)
		return;
	
	PAUIOBJECTLISTELEMENT pElement;
	
	pElement = m_pDlg->GetFirstControl();
	while( pElement )
	{
		if( pElement->pThis->IsShow() )
		{
			pElement->pThis->SetClipViewport(m_pNewViewport);
		}
		
		pElement = pElement->pNext;
	}		
	
	int err = g_pGame->GetConfigs()->GetSystemSettings().iTheme == 0 ? 15:5;
	A3DRECT rc;
	rc.left = err;
	rc.right = m_pDlg->GetSize().cx-err;
	rc.top = 0;
	rc.bottom = m_pDlg->GetSize().cy;
	m_pDlg->ChangeFrame(m_pNewViewport,rc);
	
	m_UIData.Register(m_pDlg->m_pImgCard);
	m_UIData.Register(m_pDlg->m_pImgFrame);
	
	m_UIData.Register(m_pDlg->m_pImgCardLeft);
	m_UIData.Register(m_pDlg->m_pImgCardRight);
	m_UIData.Register(m_pDlg->m_pImgCardLeftFrame);
	m_UIData.Register(m_pDlg->m_pImgCardRightFrame);
	
	m_UIData.Register(m_pDlg->m_pImgCardLeftFrameOut);
	m_UIData.Register(m_pDlg->m_pImgCardRightFrameOut);
	m_UIData.Register(m_pDlg->m_pImgCardLeftOut);
	m_UIData.Register(m_pDlg->m_pImgCardRightOut);

	m_bMoveForwards = true;
	m_iLoopTimes = 1;
}
void CECControlAnimControl::Release()
{
	//A3DRELEASE(m_pNewViewport);
	ReleaseAnim();

	CDlgGeneralCard::SetImgCover(m_pDlg->m_pImgCardLeft,NULL);
	CDlgGeneralCard::SetImgCover(m_pDlg->m_pImgCardLeftFrame,NULL);

	CDlgGeneralCard::SetImgCover(m_pDlg->m_pImgCardRight,NULL);
	CDlgGeneralCard::SetImgCover(m_pDlg->m_pImgCardRightFrame,NULL);

	CDlgGeneralCard::SetImgCover(m_pDlg->m_pImgCardLeftOut,NULL);
	CDlgGeneralCard::SetImgCover(m_pDlg->m_pImgCardLeftFrameOut,NULL);
	CDlgGeneralCard::SetImgCover(m_pDlg->m_pImgCardRightOut,NULL);
	CDlgGeneralCard::SetImgCover(m_pDlg->m_pImgCardRightFrameOut,NULL);
}
void CECControlAnimControl::RecoverControl()
{
	m_UIData.UpdateUI();
}
void CECControlAnimControl::Prev()
{
	if(!m_pDlg || !m_pDlg->CanAnimateCard()) return;

	if(m_pAnimVec.size()) ReleaseAnim();
	
	if (!m_cardIndexSelector.MovePrev())
		return;

	int movetype = m_iLoopTimes > 1 ? CECControlAnimation::ANIM_MOVE_ACC:CECControlAnimation::ANIM_MOVE_EXP;

	CECControlAnimation* pAnim = NULL;
	pAnim = new CECControlAnimation(movetype);
	pAnim->AddControl(m_pDlg->m_pImgFrame,m_pDlg->m_pImgCardRightFrame);
	m_pAnimVec.push_back(pAnim);
	pAnim = new CECControlAnimation(movetype);
	pAnim->AddControl(m_pDlg->m_pImgCard,m_pDlg->m_pImgCardRight);
	m_pAnimVec.push_back(pAnim);
	
	pAnim = new CECControlAnimation(movetype);
	pAnim->AddControl(m_pDlg->m_pImgCardLeftFrame,m_pDlg->m_pImgFrame);
	m_pAnimVec.push_back(pAnim);
	pAnim = new CECControlAnimation(movetype);
	pAnim->AddControl(m_pDlg->m_pImgCardLeft,m_pDlg->m_pImgCard);
	m_pAnimVec.push_back(pAnim);
	
	pAnim = new CECControlAnimation(movetype);
	pAnim->AddControl(m_pDlg->m_pImgCardLeftFrameOut,m_pDlg->m_pImgCardLeftFrame);
	m_pAnimVec.push_back(pAnim);
	pAnim = new CECControlAnimation(movetype);
	pAnim->AddControl(m_pDlg->m_pImgCardLeftOut,m_pDlg->m_pImgCardLeft);
	m_pAnimVec.push_back(pAnim);
	
	pAnim = new CECControlAnimation(movetype);
	pAnim->AddControl(m_pDlg->m_pImgCardRightFrame,m_pDlg->m_pImgCardRightFrameOut);
	m_pAnimVec.push_back(pAnim);
	pAnim = new CECControlAnimation(movetype);
	pAnim->AddControl(m_pDlg->m_pImgCardRight,m_pDlg->m_pImgCardRightOut);
	m_pAnimVec.push_back(pAnim);

	m_bMoveForwards = false;
	
}
void CECControlAnimControl::Next()
{
	if(!m_pDlg || !m_pDlg->CanAnimateCard()) return;

	if(m_pAnimVec.size()) ReleaseAnim();
	if (!m_cardIndexSelector.MoveNext())
		return;

	int movetype = m_iLoopTimes > 1 ? CECControlAnimation::ANIM_MOVE_ACC:CECControlAnimation::ANIM_MOVE_EXP;

	CECControlAnimation* pAnim = NULL;
	pAnim = new CECControlAnimation(movetype);
	pAnim->AddControl(m_pDlg->m_pImgFrame,m_pDlg->m_pImgCardLeftFrame);
	m_pAnimVec.push_back(pAnim);
	pAnim = new CECControlAnimation(movetype);
	pAnim->AddControl(m_pDlg->m_pImgCard,m_pDlg->m_pImgCardLeft);
	m_pAnimVec.push_back(pAnim);
	
	pAnim = new CECControlAnimation(movetype);
	pAnim->AddControl(m_pDlg->m_pImgCardRightFrame,m_pDlg->m_pImgFrame);
	m_pAnimVec.push_back(pAnim);
	pAnim = new CECControlAnimation(movetype);
	pAnim->AddControl(m_pDlg->m_pImgCardRight,m_pDlg->m_pImgCard);
	m_pAnimVec.push_back(pAnim);
	
	pAnim = new CECControlAnimation(movetype);
	pAnim->AddControl(m_pDlg->m_pImgCardRightFrameOut,m_pDlg->m_pImgCardRightFrame);
	m_pAnimVec.push_back(pAnim);
	pAnim = new CECControlAnimation(movetype);
	pAnim->AddControl(m_pDlg->m_pImgCardRightOut,m_pDlg->m_pImgCardRight);
	m_pAnimVec.push_back(pAnim);
	
	pAnim = new CECControlAnimation(movetype);
	pAnim->AddControl(m_pDlg->m_pImgCardLeftFrame,m_pDlg->m_pImgCardLeftFrameOut);
	m_pAnimVec.push_back(pAnim);
	pAnim = new CECControlAnimation(movetype);
	pAnim->AddControl(m_pDlg->m_pImgCardLeft,m_pDlg->m_pImgCardLeftOut);
	m_pAnimVec.push_back(pAnim);

	m_bMoveForwards = true;
}
void CECControlAnimControl::JumpTo(int dest)
{
	if(!m_pDlg || !m_pDlg->CanAnimateCard()) return;

	bool bMoveForwards = m_cardIndexSelector.ComputeMoveTimes(dest,m_iLoopTimes);
	if (m_iLoopTimes>0)
	{
		if (bMoveForwards)
			Next();	
		else
			Prev();
	}
}

void CECControlAnimControl::Tick(DWORD dt)
{
	if(!HasAnimRunning()) return;

	// 动画
	unsigned int iFinished = 0;
	for (unsigned int i=0;i<m_pAnimVec.size();i++)
	{
		if(m_pAnimVec[i]->Tick(dt))
			iFinished++;
	}
	// 播完动画，移动后的control复原
	if(iFinished == m_pAnimVec.size())
	{
		ReleaseAnim();
		m_UIData.UpdateUI();
		ApplyCardImgToControl(m_iLoopTimes<=1);

		m_iLoopTimes--;
	}
	// 如果是多段动画，则继续播
	if (m_iLoopTimes>0 && !HasAnimRunning())
	{
		if(m_bMoveForwards)
			Next();
		else
			Prev();
	}
}
void CECControlAnimControl::ReleaseAnim()
{
	for (unsigned int i=0;i<m_pAnimVec.size();i++)
	{
		delete m_pAnimVec[i];
	}
	m_pAnimVec.clear();
}
void CECControlAnimControl::Reset()
{
	ReleaseAnim();
	RecoverControl();
	m_iLoopTimes = 0;
	m_cardIndexSelector.UpdateRealCardIndex(m_pDlg);
	ApplyCardImgToControl(true);
}

void CECControlAnimControl::SetImgCover(int cur,int sib, PAUIIMAGEPICTURE cardCover,PAUIIMAGEPICTURE cardFrame)
{
	CECIvtrGeneralCard*	pCard = m_pDlg->GetCardByIndex(sib);
	if (sib != cur && sib != -1 && pCard)
	{
		const POKER_ESSENCE* pConfig = pCard->GetDBEssence();
		int rank = pCard->GetEssence().rank;
		a_Clamp(rank, 0, ELEMENTDATA_NUM_POKER_RANK - 1);
		if (pConfig)
		{
			CDlgGeneralCard::SetImgCover(cardCover,pConfig->file_img);
			CDlgGeneralCard::SetImgCover(cardFrame,CARD_FRAME_IMG[rank]);
		}
	}
	else
	{
		CDlgGeneralCard::SetImgCover(cardCover,NULL);
		CDlgGeneralCard::SetImgCover(cardFrame,NULL);
	}
}	
void CECControlAnimControl::ApplyCardImgToControl(bool bSel)
{
	int cur = m_cardIndexSelector.GetCurCardIndex();
	int prev = m_cardIndexSelector.GetCardPrevSib(cur);
	int prev_prev = m_cardIndexSelector.GetCardPrevSib(prev);
	int next = m_cardIndexSelector.GetCardNextSib(cur);
	int next_next = m_cardIndexSelector.GetCardNextSib(next);

	if(bSel)
	{
		if(cur>=0)
		m_pDlg->SetSelectCard(cur);
	}
	else
		SetImgCover(-1,cur,m_pDlg->m_pImgCard,m_pDlg->m_pImgFrame);	

	SetImgCover(cur,prev,m_pDlg->m_pImgCardLeft,m_pDlg->m_pImgCardLeftFrame);
	SetImgCover(cur,next,m_pDlg->m_pImgCardRight,m_pDlg->m_pImgCardRightFrame);
	SetImgCover(prev,prev_prev,m_pDlg->m_pImgCardLeftOut,m_pDlg->m_pImgCardLeftFrameOut);
	SetImgCover(next,next_next,m_pDlg->m_pImgCardRightOut,m_pDlg->m_pImgCardRightFrameOut);
}
