// File		: DlgFashionSplit.cpp
// Creator	: Xu Wenbin
// Date		: 2013/4/15

#include "AFI.h"
#include "DlgFashionSplit.h"
#include "EC_ShortcutMgr.h"
#include "EC_GameUIMan.h"
#include "EC_GameSession.h"
#include "EC_HostPlayer.h"
#include "EC_IvtrItem.h"
#include "EC_Inventory.h"
#include "EC_IvtrFashion.h"
#include "EC_UIConfigs.h"
#include "ExpTypes.h"
#include "AUICTranslate.h"

#define new A_DEBUG_NEW

AUI_BEGIN_COMMAND_MAP(CDlgFashionSplit, CDlgBase)

AUI_ON_COMMAND("confirm",		OnCommandConfirm)
AUI_ON_COMMAND("IDCANCEL",		OnCommandCancel)

AUI_END_COMMAND_MAP()

AUI_BEGIN_EVENT_MAP(CDlgFashionSplit, CDlgBase)

AUI_ON_EVENT("Img_Item",	WM_LBUTTONDOWN,	OnEventLButtonDown)

AUI_END_EVENT_MAP()

//------------------------------------------------------------------------
// Right-Click Shortcut for CDlgFashionSplit
//------------------------------------------------------------------------
typedef CECShortcutMgr::SimpleClickShortcut<CDlgFashionSplit> FashionSplitClickShortcut;
//------------------------------------------------------------------------

CDlgFashionSplit::CDlgFashionSplit()
{
	m_pBtn_Confirm = NULL;
	m_pImg_Item = NULL;
	m_pTxt_Name = NULL;
	m_pTxt_Color = NULL;
	m_pTxt_Num = NULL;
	m_nSolt = -1;
	m_nItemID = 0;
}

CDlgFashionSplit::~CDlgFashionSplit()
{
}

bool CDlgFashionSplit::OnInitDialog()
{
	DDX_Control("Btn_Confirm", m_pBtn_Confirm);
	DDX_Control("Img_Item", m_pImg_Item);
	DDX_Control("Txt_Name", m_pTxt_Name);
	DDX_Control("Txt_Color", m_pTxt_Color);
	DDX_Control("Txt_Num", m_pTxt_Num);

	GetGameUIMan()->GetShortcutMgr()->RegisterShortCut(new FashionSplitClickShortcut(this));
	return true;
}

CECIvtrFashion * CDlgFashionSplit::GetCurFashion()
{
	CECIvtrFashion *pFashion = NULL;
	if (m_nSolt >= 0){
		CECIvtrItem *pItem = GetHostPlayer()->GetPack()->GetItem(m_nSolt);
		if (pItem != NULL && pItem->GetTemplateID() == m_nItemID){
			pFashion = dynamic_cast<CECIvtrFashion *>(pItem);
		}
	}
	return pFashion;
}

bool CDlgFashionSplit::CanConfirm()
{
	return GetCurFashion() != NULL
		&& GetHostPlayer()->GetMoneyAmount() >= CECUIConfig::Instance().GetGameUI().nFashionSplitCost;
}

void CDlgFashionSplit::OnCommandConfirm(const char *szCommand)
{
	if (!CanConfirm()) return;
	CECIvtrFashion *pFashion = GetCurFashion();
	if (!pFashion) return;

	if (!szCommand || strcmp(szCommand, "MessageBox_Confirm")){
		ACString strText = GetGameUIMan()->GetStringFromTable(10431);
		GetGameUIMan()->MessageBox("Game_FashionSplitConfirm", strText, MB_YESNO, A3DCOLORRGBA(255, 255, 255, 160));
		return;
	}
	GetGameSession()->c2s_SendCmdNPCSevSplitFashion(m_nSolt, m_nItemID);
	OnEventLButtonDown(0, 0, NULL);
}

void CDlgFashionSplit::OnCommandCancel(const char *szCommand)
{
	OnEventLButtonDown(0, 0, NULL);
	GetGameUIMan()->EndNPCService();
	Show(false);
	GetGameUIMan()->GetDialog("Win_Inventory")->Show(false);
	GetHostPlayer()->GetPack(IVTRTYPE_PACK)->UnfreezeAllItems();
}

void CDlgFashionSplit::OnShowDialog()
{
	OnEventLButtonDown(0, 0, NULL);
}

void CDlgFashionSplit::OnEventLButtonDown(WPARAM wParam, LPARAM lParam, AUIObject * pObj)
{
	CECIvtrFashion *pFashion = GetCurFashion();
	if (pFashion) pFashion->Freeze(false);
	m_pImg_Item->ClearCover();
	m_pImg_Item->SetHint(_AL(""));
	m_pTxt_Name->SetText(_AL(""));
	m_pTxt_Color->Show(false);
	m_pTxt_Num->SetText(_AL(""));
	m_pBtn_Confirm->Enable(false);
}

bool CDlgFashionSplit::SetFashion(CECIvtrFashion *pFashion, int nSlot)
{
	if (!pFashion 
		|| pFashion->GetDBEssence()->require_dye_count <= 0
		|| (pFashion->GetProcType() & CECIvtrItem::PROC_BINDING))
		return false;

	OnEventLButtonDown(0, 0, NULL);

	pFashion->Freeze(true);
	m_nSolt = nSlot;
	m_nItemID = pFashion->GetTemplateID();
	GetGameUIMan()->SetCover(m_pImg_Item, pFashion->GetIconFile());
	AUICTranslate trans;
	m_pImg_Item->SetHint(trans.Translate(pFashion->GetDesc()));
	m_pTxt_Name->SetText(pFashion->GetName());
	m_pTxt_Color->Show(true);
	m_pTxt_Color->SetColor(pFashion->GetColor());
	m_pTxt_Num->SetText(ACString().Format(_AL("%d"), pFashion->GetDBEssence()->require_dye_count));
	m_pBtn_Confirm->Enable(CanConfirm());
	return true;
}

void CDlgFashionSplit::OnItemDragDrop(CECIvtrItem* pIvtrSrc, int iSrc, PAUIOBJECT pObjSrc, PAUIOBJECT pObjOver)
{
	if (pIvtrSrc->GetClassID() == CECIvtrItem::ICID_FASHION)
		SetFashion(dynamic_cast<CECIvtrFashion *>(pIvtrSrc), iSrc);
}