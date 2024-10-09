// File		: DlgPetHatch.cpp
// Creator	: Xiao Zhou
// Date		: 2005/12/29

#include "AFI.h"
#include "DlgPetHatch.h"
#include "EC_ShortcutMgr.h"
#include "EC_GameUIMan.h"
#include "EC_HostPlayer.h"
#include "EC_IvtrItem.h"
#include "EC_IvtrPetItem.h"
#include "EC_Inventory.h"
#include "ExpTypes.h"
#include "AUICTranslate.h"

#define new A_DEBUG_NEW

AUI_BEGIN_COMMAND_MAP(CDlgPetHatch, CDlgBase)

AUI_ON_COMMAND("confirm",		OnCommandConfirm)
AUI_ON_COMMAND("IDCANCEL",		OnCommandCancel)

AUI_END_COMMAND_MAP()

AUI_BEGIN_EVENT_MAP(CDlgPetHatch, CDlgBase)

AUI_ON_EVENT("Img_Item",	WM_LBUTTONDOWN,	OnEventLButtonDown)

AUI_END_EVENT_MAP()

//------------------------------------------------------------------------
// Right-Click Shortcut for CDlgPetHatch
//------------------------------------------------------------------------
typedef CECShortcutMgr::SimpleClickShortcut<CDlgPetHatch> PetHatchClickShortcut;
//------------------------------------------------------------------------

CDlgPetHatch::CDlgPetHatch()
{
	m_pTxt_Gold = NULL;
	m_pTxt_Name = NULL;
	m_pImg_Item = NULL;
}

CDlgPetHatch::~CDlgPetHatch()
{
}

bool CDlgPetHatch::OnInitDialog()
{
	GetGameUIMan()->GetShortcutMgr()->RegisterShortCut(new PetHatchClickShortcut(this));

	DDX_Control("Txt_Gold", m_pTxt_Gold);
	DDX_Control("Txt_Name", m_pTxt_Name);
	DDX_Control("Img_Item", m_pImg_Item);

	return true;
}

void CDlgPetHatch::OnCommandConfirm(const char *szCommand)
{
	CECIvtrItem *pItem = (CECIvtrItem *)GetDataPtr();
	if( pItem )
	{
		pItem->Freeze(false);
		GetHostPlayer()->HatchPet(m_nSolt);
		SetDataPtr(NULL);
		GetGameUIMan()->EndNPCService();
		Show(false);
		GetGameUIMan()->GetDialog("Win_Inventory")->Show(false);
		GetHostPlayer()->GetPack(IVTRTYPE_PACK)->UnfreezeAllItems();
	}
}

void CDlgPetHatch::OnCommandCancel(const char *szCommand)
{
	OnEventLButtonDown(0, 0, NULL);
	GetGameUIMan()->EndNPCService();
	Show(false);
	GetGameUIMan()->GetDialog("Win_Inventory")->Show(false);
	GetHostPlayer()->GetPack(IVTRTYPE_PACK)->UnfreezeAllItems();
}

void CDlgPetHatch::OnShowDialog()
{
	OnEventLButtonDown(0, 0, NULL);
}

void CDlgPetHatch::OnEventLButtonDown(WPARAM wParam, LPARAM lParam, AUIObject * pObj)
{
	GetDlgItem("Txt_BindTip")->Show(false);
	CECIvtrItem *pItem = (CECIvtrItem *)GetDataPtr();
	if( pItem )
		pItem->Freeze(false);
	m_pImg_Item->ClearCover();
	m_pImg_Item->SetHint(_AL(""));
	m_pTxt_Name->SetText(_AL(""));
	m_pTxt_Gold->SetText(_AL(""));
	SetDataPtr(NULL);
}

void CDlgPetHatch::SetEggs(CECIvtrItem *pItem, int nSlot)
{
	OnEventLButtonDown(0, 0, NULL);
	CECIvtrPetEgg *pEgg = (CECIvtrPetEgg *)pItem;
	if( pEgg->GetProcType() & CECIvtrItem::PROC_BIND )
		GetDlgItem("Txt_BindTip")->Show(true);
	SetDataPtr(pEgg);
	m_nSolt = nSlot;
	pEgg->Freeze(true);
	AString strFile;
	af_GetFileTitle(pEgg->GetIconFile(), strFile);
	strFile.MakeLower();
	m_pImg_Item->SetCover(GetGameUIMan()->m_pA2DSpriteIcons[CECGameUIMan::ICONS_INVENTORY],
		GetGameUIMan()->m_IconMap[CECGameUIMan::ICONS_INVENTORY][strFile]);
	AUICTranslate trans;
	const wchar_t* szDesc = pEgg->GetDesc();
	if( szDesc )
		m_pImg_Item->SetHint(trans.Translate(pEgg->GetDesc()));
	else
		m_pImg_Item->SetHint(pEgg->GetName());
	
	m_pTxt_Name->SetText(pEgg->GetName());
	ACHAR szText[20];
	a_sprintf(szText, _AL("%d"), pEgg->GetDBEssence()->money_hatched);
	m_pTxt_Gold->SetText(szText);
}

void CDlgPetHatch::OnItemDragDrop(CECIvtrItem* pIvtrSrc, int iSrc, PAUIOBJECT pObjSrc, PAUIOBJECT pObjOver)
{
	// no need to check name "Img_Item", only one available drag-drop target
	if( pIvtrSrc->GetClassID() == CECIvtrItem::ICID_PETEGG )
	{
		this->SetEggs(pIvtrSrc, iSrc);
	}
}