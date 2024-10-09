// Filename	: DlgELFToPill.cpp
// Creator	: Chen Zhixin
// Date		: October 17, 2008

#include "WindowsX.h"
#include "AFI.h"
#include "AUICTranslate.h"
#include "AUIFrame.h"
#include "AUICommon.h"
#include "AUIManager.h"
#include "DlgELFToPill.h"
#include "EC_Global.h"
#include "EC_Game.h"
#include "EC_GameRun.h"
#include "EC_GameUIMan.h"
#include "EC_GameSession.h"
#include "EC_HostPlayer.h"
#include "EC_Faction.h"
#include "elementdataman.h"

#include "EC_IvtrEquip.h"
#include "EC_Inventory.h"
#include "EC_IvtrItem.h"
#include "EC_IvtrEquip.h"
#include "AUIStillImageButton.h"
#include "DlgInventory.h"
#include "EC_IvtrGoblin.h"
#include "EC_ShortcutMgr.h"

#define new A_DEBUG_NEW
#define INTERAL_LONG	500
#define INTERAL_SHORT	200
#define TEMPERED_TIME	3000

AUI_BEGIN_COMMAND_MAP(CDlgELFToPill, CDlgBase)

AUI_ON_COMMAND("IDCANCEL",		OnCommand_CANCEL)
AUI_ON_COMMAND("confirm",		OnCommand_Confirm)

AUI_END_COMMAND_MAP()

AUI_BEGIN_EVENT_MAP(CDlgELFToPill, CDlgBase)

AUI_ON_EVENT("Img_ELF",		WM_LBUTTONDOWN,		OnEventLButtonDown_ELF)

AUI_END_EVENT_MAP()

//------------------------------------------------------------------------
// Right-Click Shortcut for CDlgELFToPill
//------------------------------------------------------------------------
typedef CECShortcutMgr::SimpleClickShortcut<CDlgELFToPill> ELFToPillClickShortcut;
//------------------------------------------------------------------------

CDlgELFToPill::CDlgELFToPill()
{
	m_pTxt_Name = NULL;
	m_pTxt_Item = NULL;
	m_pImg_Item = NULL;
	m_pImg_ELF = NULL;
	m_pBtn_Confirm = NULL;
	m_nSolt = -1;
}

CDlgELFToPill::~CDlgELFToPill()
{
}

bool CDlgELFToPill::OnInitDialog()
{
	GetGameUIMan()->GetShortcutMgr()->RegisterShortCut(new ELFToPillClickShortcut(this));

	DDX_Control("Img_ELF", m_pImg_ELF);
	DDX_Control("Txt_ELF", m_pTxt_Name);

	DDX_Control("Img_NEW", m_pImg_Item);
	DDX_Control("Txt_NEW", m_pTxt_Item);
	
	DDX_Control("Btn_OK", m_pBtn_Confirm);

	return true;
}

void CDlgELFToPill::OnShowDialog()
{
	OnEventLButtonDown_ELF(0, 0, NULL);
	m_pTxt_Item->SetText(_AL("0"));
}

void CDlgELFToPill::SetELF(CECIvtrItem *pItem1, int nSlot)
{
	CECIvtrGoblin* pELF = (CECIvtrGoblin*)pItem1;
	OnEventLButtonDown_ELF(0, 0, NULL);
	m_pImg_ELF->SetDataPtr(pELF);
	m_nSolt = nSlot;
	pItem1->Freeze(true);
	AString strFile;
	af_GetFileTitle(pELF->GetIconFile(), strFile);
	strFile.MakeLower();
	m_pImg_ELF->SetCover(GetGameUIMan()->m_pA2DSpriteIcons[CECGameUIMan::ICONS_INVENTORY],
		GetGameUIMan()->m_IconMap[CECGameUIMan::ICONS_INVENTORY][strFile]);
	AUICTranslate trans;
	const ACHAR * szDesc = pELF->GetDesc();
	if( szDesc )
		m_pImg_ELF->SetHint(trans.Translate(szDesc));
	else
		m_pImg_ELF->SetHint(_AL(""));
	m_pTxt_Name->SetText(pELF->GetName());
	//得到的灵魂珠的图标
	CECIvtrItem* pItem = CECIvtrItem::CreateItem(23544, 0, 1);
	af_GetFileTitle(pItem->GetIconFile(), strFile);
	strFile.MakeLower();
	m_pImg_Item->SetCover(GetGameUIMan()->m_pA2DSpriteIcons[CECGameUIMan::ICONS_INVENTORY],
		GetGameUIMan()->m_IconMap[CECGameUIMan::ICONS_INVENTORY][strFile]);
	ACHAR szText[50];
	a_sprintf(szText, _AL("%u"), pELF->GetDestroyExp());
 	m_pTxt_Item->SetText(szText);
	//m_pBtn_Confirm->Enable(true);
	delete pItem;
}

void CDlgELFToPill::OnCommand_Confirm(const char *szCommand)
{
	CECIvtrGoblin* pELF = (CECIvtrGoblin*)m_pImg_ELF->GetDataPtr();
	if (!pELF)
	{
		GetGameUIMan()->MessageBox("", GetStringFromTable(7140), MB_OK, A3DCOLORRGBA(255, 255, 255, 160));
		return;
	}
	if (pELF->GetEssence().data.status_value != -1 && pELF->GetEssence().data.level >= 40)
	{
		GetGameUIMan()->MessageBox("", GetStringFromTable(7141), MB_OK, A3DCOLORRGBA(255, 255, 255, 160));
		return;
	}
	if( pELF && m_nSolt >= 0 ) 
	{
		PAUIDIALOG pMsgBox;
		GetGameUIMan()->MessageBox("Game_GoblinToPill", GetStringFromTable(7142), MB_YESNO, A3DCOLORRGBA(255, 255, 255, 160), &pMsgBox);
		pMsgBox->SetData((DWORD)m_nSolt);
//		m_pBtn_Confirm->Enable(false);
	}
}

void CDlgELFToPill::OnCommand_CANCEL(const char * szCommand)
{
	OnEventLButtonDown_ELF(0, 0, NULL);
	GetGameUIMan()->EndNPCService();
	Show(false);
	GetGameUIMan()->m_pDlgInventory->Show(false);
	GetHostPlayer()->GetPack(IVTRTYPE_PACK)->UnfreezeAllItems();
}

void CDlgELFToPill::OnEventLButtonDown_ELF(WPARAM wParam, LPARAM lParam, AUIObject * pObj)
{
 	CECIvtrItem *pELF = (CECIvtrItem *)m_pImg_ELF->GetDataPtr();
 	if( pELF )
 		pELF->Freeze(false);
 	m_pImg_ELF->ClearCover();
 	m_pImg_ELF->SetHint(GetStringFromTable(7303));
 	m_pImg_ELF->SetDataPtr(NULL);
 	m_pTxt_Name->SetText(_AL(""));
	m_pImg_Item->ClearCover();
	m_pImg_Item->SetHint(GetStringFromTable(7304));
	m_pTxt_Item->SetText(_AL(""));
//	m_pBtn_Confirm->Enable(false);
 	m_nSolt = -1;
}

void CDlgELFToPill::OnItemDragDrop(CECIvtrItem* pIvtrSrc, int iSrc, PAUIOBJECT pObjSrc, PAUIOBJECT pObjOver)
{
	// no need to check name "Img_ELF", only one available drag-drop target
	if( pIvtrSrc->GetClassID() == CECIvtrItem::ICID_GOBLIN )
		this->SetELF(pIvtrSrc, iSrc);
}