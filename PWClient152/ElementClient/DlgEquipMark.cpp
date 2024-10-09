// File		: DlgEquipMark.cpp
// Creator	: Xu Wenbin
// Date		: 2012/11/7

#include "AFI.h"
#include "DlgEquipMark.h"
#include "EC_ShortcutMgr.h"
#include "EC_Game.h"
#include "EC_GameRun.h"
#include "EC_GameUIMan.h"
#include "EC_GameSession.h"
#include "EC_HostPlayer.h"
#include "EC_IvtrItem.h"
#include "EC_Inventory.h"
#include "EC_IvtrEquip.h"
#include "EC_IvtrConsume.h"
#include "EC_UIManager.h"
#include "EC_UIConfigs.h"
#include "elementdataman.h"
#include "auto_delete.h"
#include "DlgItemDesc.h"

#define new A_DEBUG_NEW

AUI_BEGIN_COMMAND_MAP(CDlgEquipMark, CDlgBase)

AUI_ON_COMMAND("confirm",		OnCommandConfirm)
AUI_ON_COMMAND("IDCANCEL",		OnCommandCancel)
AUI_ON_COMMAND("Btn_Look",		OnCommandLook)

AUI_END_COMMAND_MAP()

AUI_BEGIN_EVENT_MAP(CDlgEquipMark, CDlgBase)

AUI_ON_EVENT("Img_Item",	WM_LBUTTONDOWN,	OnEventLButtonDown_Item)
AUI_ON_EVENT("Img_Dye",		WM_LBUTTONDOWN,	OnEventLButtonDown_Dye)

AUI_END_EVENT_MAP()

//------------------------------------------------------------------------
// Right-Click Shortcut for CDlgEquipMark
//------------------------------------------------------------------------
typedef CECShortcutMgr::SimpleClickShortcut<CDlgEquipMark> EquipMarkClickShortcut;
//------------------------------------------------------------------------

CDlgEquipMark::CDlgEquipMark()
{
	m_pBtn_Confirm = NULL;
	m_pBtn_Look = NULL;
	
	m_pImg_Item = NULL;
	m_pTxt_Name = NULL;
	m_pImg_Dye = NULL;
	m_pTxt_Color = NULL;
	m_pTxt_Num = NULL;
	m_pEdt_Mark = NULL;

	m_nSlot = -1;
	m_nSlotDye = -1;
}

CDlgEquipMark::~CDlgEquipMark()
{
}

bool CDlgEquipMark::OnInitDialog()
{
	DDX_Control("Btn_Confirm", m_pBtn_Confirm);
	DDX_Control("Btn_Look", m_pBtn_Look);
	DDX_Control("Img_Item", m_pImg_Item);
	DDX_Control("Txt_Name", m_pTxt_Name);
	DDX_Control("Img_Dye", m_pImg_Dye);
	DDX_Control("Txt_Color", m_pTxt_Color);
	DDX_Control("Txt_Num", m_pTxt_Num);
	DDX_Control("Edt_Mark", m_pEdt_Mark);

	GetGameUIMan()->GetShortcutMgr()->RegisterShortCut(new EquipMarkClickShortcut(this));

	return true;
}

bool CDlgEquipMark::CanConfirm()
{
	CECIvtrEquip* pEquip = GetEquip(m_nSlot);
	CECIvtrDyeTicket* pDye = GetDye(m_nSlotDye);
	return pEquip != NULL && pDye != NULL
		&& (!(GetNewMark().IsEmpty()) || !(pEquip->GetMakerName().IsEmpty()));	//	两者均为空时，无任何意义；
}

bool CDlgEquipMark::CanLook()
{
	return CanConfirm();
}

void CDlgEquipMark::OnCommandConfirm(const char *szCommand)
{
	if (!CanConfirm()) return;

	CECIvtrEquip* pEquip = GetEquip(m_nSlot);
	CECIvtrDyeTicket* pDye = GetDye(m_nSlotDye);
	
	ACString strMark = GetNewMark();
	if (GetGameRun()->GetUIManager()->HasBadWords(strMark))
	{
		GetGameUIMan()->MessageBox("", GetStringFromTable(816), MB_OK, A3DCOLORRGB(255, 255, 255));
		return;
	}
	
	if( GetRequireDyeCount() > pDye->GetCount() )
	{
		GetGameUIMan()->MessageBox("", GetStringFromTable(9995), MB_OK, A3DCOLORRGB(255, 255, 255));
		return;
	}

	if (strMark.IsEmpty() && (!szCommand || stricmp(szCommand, "Confirm_Clear")))
	{
		GetGameUIMan()->MessageBox("EquipMark_Clear", GetStringFromTable(9997), MB_YESNO, A3DCOLORRGB(255, 255, 255));
		return;
	}
	
	GetGameSession()->c2s_CmdNPCSevMark(
		m_nSlot, pEquip->GetTemplateID(),
		m_nSlotDye, pDye->GetTemplateID(),
		(void*)(const ACHAR *)strMark, strMark.GetLength() * sizeof(ACHAR));

	OnCommandCancel(NULL);
}

void CDlgEquipMark::OnCommandCancel(const char *szCommand)
{
	ClearDialog();
	GetGameUIMan()->EndNPCService();
	Show(false);
	GetGameUIMan()->GetDialog("Win_Inventory")->Show(false);
	GetHostPlayer()->GetPack(IVTRTYPE_PACK)->UnfreezeAllItems();
}

void CDlgEquipMark::OnCommandLook(const char *szCommand)
{
	if (!CanLook()) return;

	CECIvtrEquip *pEquip = GetEquip(m_nSlot);
	CECIvtrEquip *pTemp = dynamic_cast<CECIvtrEquip *>(pEquip->Clone());
	auto_delete<CECIvtrEquip> dummy(pTemp);

	pTemp->SetNewMark(GetNewMark(), GetMarkColor());

	POINT pos = m_pBtn_Look->GetPos();
	CDlgItemDesc* pDlg = (CDlgItemDesc*)GetGameUIMan()->GetDialog("Win_ItemDesc");
	pDlg->ShowDesc(pTemp, pos.x, pos.y);
}

void CDlgEquipMark::OnShowDialog()
{
	ClearDialog();
}

void CDlgEquipMark::OnEventLButtonDown_Item(WPARAM wParam, LPARAM lParam, AUIObject * pObj)
{
	CECIvtrEquip *pEquip = GetEquip(m_nSlot);
	if( pEquip ) pEquip->Freeze(false);

	m_pImg_Item->ClearCover();
	m_pImg_Item->SetHint(_AL(""));

	m_pTxt_Name->SetText(_AL(""));

	m_nSlot = -1;
}

void CDlgEquipMark::OnEventLButtonDown_Dye(WPARAM wParam, LPARAM lParam, AUIObject * pObj)
{
	CECIvtrDyeTicket *pDye = GetDye(m_nSlotDye);
	if( pDye ) pDye->Freeze(false);

	m_pImg_Dye->ClearCover();
	m_pImg_Dye->SetHint(_AL(""));

	m_pTxt_Color->Show(false);

	m_nSlotDye = -1;
}

void CDlgEquipMark::SetEquip(int nSlot)
{
	CECIvtrEquip* pEquip = GetEquip(nSlot);
	if (!pEquip)
	{
		GetGameUIMan()->MessageBox("", GetStringFromTable(9996), MB_OK, A3DCOLORRGB(255, 255, 255));
		return;
	}

	OnEventLButtonDown_Item(0, 0, m_pImg_Item);
	GetGameUIMan()->SetCover(m_pImg_Item, pEquip->GetIconFile());
	GetGameUIMan()->SetHint(m_pImg_Item, pEquip);

	m_nSlot = nSlot;
	pEquip->Freeze(true);

	m_pTxt_Name->SetText(pEquip->GetName());
}

void CDlgEquipMark::SetDye(int nSlot)
{
	CECIvtrDyeTicket* pDye = GetDye(nSlot);
	if (!pDye) return;

	OnEventLButtonDown_Dye(0, 0, NULL);
	GetGameUIMan()->SetCover(m_pImg_Dye, pDye->GetIconFile());
	GetGameUIMan()->SetHint(m_pImg_Dye, pDye);

	m_nSlotDye = nSlot;
	pDye->Freeze(true);

	m_pTxt_Color->Show(true);
	m_pTxt_Color->SetColor(pDye->GenerateColor());
}

void CDlgEquipMark::OnTick()
{
	UpdateNumDisplay();

	m_pBtn_Confirm->Enable(CanConfirm());
	m_pBtn_Look->Enable(CanLook());
}

void CDlgEquipMark::ClearDialog()
{
	OnEventLButtonDown_Item(0, 0, m_pImg_Item);
	OnEventLButtonDown_Dye(0, 0, m_pImg_Dye);
	m_pEdt_Mark->SetText(_AL(""));
}

ACString CDlgEquipMark::GetNewMark()
{
	ACString strMark = m_pEdt_Mark->GetText();
	strMark.TrimLeft();
	strMark.TrimRight();
	return strMark;
}

A3DCOLOR CDlgEquipMark::GetMarkColor()
{
	return m_pTxt_Color->GetColor();
}

int CDlgEquipMark::GetRequireDyeCount()
{
	ACString strMark = GetNewMark();
	int count = strMark.GetLength();
	a_ClampFloor(count, CECUIConfig::Instance().GetGameUI().nEquipMarkMinInkNum);
	return count;
}

int CDlgEquipMark::GetDyeCount()
{
	CECIvtrDyeTicket *pDye = GetDye(m_nSlotDye);
	return pDye ? pDye->GetCount() : 0;
}

void CDlgEquipMark::UpdateNumDisplay()
{
	int nCount = GetDyeCount();
	int nRequired = GetRequireDyeCount();
	if (nCount >= nRequired)
		m_pTxt_Num->SetColor(A3DCOLORRGB(255, 255, 255));
	else
		m_pTxt_Num->SetColor(A3DCOLORRGB(255, 0, 0));
	
	ACHAR szText[20];
	a_sprintf(szText, _AL("%d/%d"), nRequired, nCount);
	m_pTxt_Num->SetText(szText);
}

CECIvtrEquip *CDlgEquipMark::GetEquip(int nSlot)
{
	CECIvtrEquip *pEquip = NULL;
	
	if (nSlot >= 0)
	{
		CECIvtrItem *pItem = GetHostPlayer()->GetPack()->GetItem(nSlot);
		if (pItem && pItem->IsEquipment())
		{
			pEquip = dynamic_cast<CECIvtrEquip *>(pItem);
			if (pEquip)
			{
				if (pEquip->GetClassID() != CECIvtrItem::ICID_ARMOR &&
					pEquip->GetClassID() != CECIvtrItem::ICID_DECORATION &&
					pEquip->GetClassID() != CECIvtrItem::ICID_WEAPON &&
					pEquip->GetClassID() != CECIvtrItem::ICID_FLYSWORD)
					pEquip = NULL;
			}
		}
	}

	return pEquip;
}

CECIvtrDyeTicket *CDlgEquipMark::GetDye(int nSlot)
{
	CECIvtrDyeTicket *pDye = NULL;
	
	if (nSlot >= 0)
	{
		CECIvtrItem *pItem = GetHostPlayer()->GetPack()->GetItem(nSlot);
		if (pItem && pItem->GetClassID() == CECIvtrItem::ICID_DYETICKET)
		{
			pDye = dynamic_cast<CECIvtrDyeTicket *>(pItem);
			if (pDye)
			{
				if (pDye->GetDBEssence()->usage != 1)
					pDye = NULL;
			}
		}
	}
	
	return pDye;
}

void CDlgEquipMark::OnItemDragDrop(CECIvtrItem* pIvtrSrc, int iSrc, PAUIOBJECT pObjSrc, PAUIOBJECT pObjOver)
{
	if (pIvtrSrc->GetClassID() == CECIvtrItem::ICID_DYETICKET)
	{
		// no need to check name "Img_Dye", only one available drag-drop target
		this->SetDye(iSrc);
	}
	else // drag-drop a equip or click directly
	{
		CECIvtrEquip* pEquip = dynamic_cast<CECIvtrEquip*>(pIvtrSrc);
		if (!pEquip) return;
		// no need to check name "Img_Item", only one available drag-drop target
		this->SetEquip(iSrc);
	}
}