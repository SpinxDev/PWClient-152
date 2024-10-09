// Filename	: DlgSplit.cpp
// Creator	: Tom Zhou
// Date		: October 12, 2005

#include "DlgSplit.h"
#include "EC_ShortcutMgr.h"
#include "EC_GameSession.h"
#include "EC_GameUIMan.h"
#include "EC_IvtrItem.h"
#include "EC_HostPlayer.h"
#include "EC_Inventory.h"
#include "EC_IvtrEquip.h"
#include "EC_Global.h"
#include "EC_Skill.h"
#include "elementdataman.h"
#include "AFI.h"

#define new A_DEBUG_NEW

AUI_BEGIN_COMMAND_MAP(CDlgSplit, CDlgBase)

AUI_ON_COMMAND("begin",			OnCommand_begin)
AUI_ON_COMMAND("IDCANCEL",		OnCommand_CANCEL)

AUI_END_COMMAND_MAP()

AUI_BEGIN_EVENT_MAP(CDlgSplit, CDlgBase)

AUI_ON_EVENT("Item_a",		WM_LBUTTONDOWN,		OnEventLButtonDown)

AUI_END_EVENT_MAP()

//------------------------------------------------------------------------
// Right-Click Shortcut for CDlgSplit
//------------------------------------------------------------------------
typedef CECShortcutMgr::SimpleClickShortcut<CDlgSplit> SplitClickShortcut;
//------------------------------------------------------------------------

CDlgSplit::CDlgSplit()
{
}

CDlgSplit::~CDlgSplit()
{
}

bool CDlgSplit::OnInitDialog()
{
	m_pTxt_no1 = (PAUIOBJECT)GetDlgItem("Txt_no1");
	m_pTxt_no2 = (PAUIOBJECT)GetDlgItem("Txt_no2");
	m_pTxt_Gold = (PAUIOBJECT)GetDlgItem("Txt_Gold");
	m_pPrgs_1 = (PAUIPROGRESS)GetDlgItem("Prgs_1");
	m_pImgItem_a = (PAUIIMAGEPICTURE)GetDlgItem("Item_a");
	m_pImgItem_b = (PAUIIMAGEPICTURE)GetDlgItem("Item_b");
	m_pBtn_Start = (PAUISTILLIMAGEBUTTON)GetDlgItem("Btn_Start");
	m_pBtn_Cancel = (PAUISTILLIMAGEBUTTON)GetDlgItem("Btn_Cancel");

	GetGameUIMan()->GetShortcutMgr()->RegisterShortCut(new SplitClickShortcut(this));
	return true;
}

void CDlgSplit::OnCommand_begin(const char * szCommand)
{
	NPC_DECOMPOSE_SERVICE *pService = (NPC_DECOMPOSE_SERVICE *)GetDataPtr("ptr_NPC_DECOMPOSE_SERVICE");
	int iSrc = int(m_pImgItem_a->GetData());
	CECIvtrItem *pIvtr = (CECIvtrItem *)m_pImgItem_a->GetDataPtr("ptr_CECIvtrItem");
	if (pIvtr)
	{
		GetGameSession()->c2s_CmdNPCSevBreakItem(pService->id_decompose_skill, iSrc, pIvtr->GetTemplateID());
		m_pBtn_Start->Enable(false);
	}
}

void CDlgSplit::OnCommand_CANCEL(const char * szCommand)
{
	if( m_pPrgs_1->GetProgress() <= 0 )
	{
		Show(false);
		GetGameUIMan()->EndNPCService();
		GetGameUIMan()->GetDialog("Win_Inventory")->Show(false);
		GetHostPlayer()->GetPack(IVTRTYPE_PACK)->UnfreezeAllItems();
	}
	else
		GetGameSession()->c2s_CmdCancelAction();
}

void CDlgSplit::OnEventLButtonDown(WPARAM wParam, LPARAM lParam, AUIObject *pObj)
{
	CECIvtrItem *pIvtr = (CECIvtrItem *)m_pImgItem_a->GetDataPtr("ptr_CECIvtrItem");
	if( !pIvtr ) return;

	m_pImgItem_a->ClearCover();
	m_pImgItem_a->SetData(0);
	m_pImgItem_a->SetDataPtr(NULL);
	
	m_pImgItem_b->ClearCover();
	m_pImgItem_b->SetData(0);
	m_pImgItem_b->SetDataPtr(NULL);
	
	this->GetDlgItem("Txt_no1")->SetText(_AL("0"));
	this->GetDlgItem("Txt_no2")->SetText(_AL("0"));
	this->GetDlgItem("Txt_Gold")->SetText(_AL("0"));
	this->GetDlgItem("Txt_SkillName")->SetText(_AL(""));
	this->GetDlgItem("Txt_SkillLevel")->SetText(_AL(""));
	this->GetDlgItem("Btn_Start")->Enable(false);
	
	GetHostPlayer()->GetPack(IVTRTYPE_PACK)->UnfreezeAllItems();
	GetGameUIMan()->PlayItemSound(pIvtr, true);
}

void CDlgSplit::OnItemDragDrop(CECIvtrItem* pIvtrSrc, int iSrc, PAUIOBJECT pObjSrc, PAUIOBJECT pObjOver)
{
	// default drag-drop to this object
	pObjOver = GetDlgItem("Item_a");

	if(!pObjOver || ((PAUIPROGRESS)this->GetDlgItem("Prgs_1"))->GetProgress() > 0)
	{
		return;
	}

	pObjOver->SetData(iSrc);
	void *ptr;
	AString szType;
	pObjSrc->ForceGetDataPtr(ptr,szType);
	pObjOver->SetDataPtr(ptr,szType);
	GetGameUIMan()->PlayItemSound(pIvtrSrc, true);

	AString strFile;
	af_GetFileTitle(pIvtrSrc->GetIconFile(), strFile);
	strFile.MakeLower();
	PAUIIMAGEPICTURE pImage = (PAUIIMAGEPICTURE)pObjOver;
	pImage->SetCover(GetGameUIMan()->m_pA2DSpriteIcons[CECGameUIMan::ICONS_INVENTORY],
		GetGameUIMan()->m_IconMap[CECGameUIMan::ICONS_INVENTORY][strFile]);

	pImage->SetColor(
		(pIvtrSrc->IsEquipment() && ((CECIvtrEquip *)pIvtrSrc)->IsDestroying())
		? A3DCOLORRGB(128, 128, 128)
		: A3DCOLORRGB(255, 255, 255));

	CECHostPlayer* pHost = GetHostPlayer();
	pHost->GetPack(IVTRTYPE_PACK)->UnfreezeAllItems();
	pIvtrSrc->Freeze(true);

	bool bCan = true;
	DATA_TYPE DataType;
	int idSkillNeed = -1, nLevelNeed = 0;
	CECSkill *pSkill, *pSkillThis = NULL;
	elementdataman *pDataMan = g_pGame->GetElementDataMan();
	int i, uiID, nItem = -1, nTime = 0, idItem = 0, nNumItem = 0;
	ACHAR szText[40], szNumber[40] = _AL("0");
	int curPrice = 0;

	this->GetDlgItem("Txt_SkillName")->SetText(_AL(""));
	this->GetDlgItem("Txt_SkillLevel")->SetText(_AL(""));
	for( i = 0; i < (int)pDataMan->get_data_num(ID_SPACE_RECIPE); i++ )
	{
		uiID = pDataMan->get_data_id(ID_SPACE_RECIPE, i, DataType);
		if( DataType != DT_RECIPE_ESSENCE ) continue;

		const RECIPE_ESSENCE *pRecipe = (const RECIPE_ESSENCE *)
			pDataMan->get_data_ptr(uiID, ID_SPACE_RECIPE, DataType);
		if( (int)pRecipe->targets[0].id_to_make != pIvtrSrc->GetTemplateID() )
			continue;

		this->GetDlgItem("Txt_SkillName")->SetText(
			g_pGame->GetSkillDesc()->GetWideStringObject(pRecipe->id_skill * 10));
		a_sprintf(szText, _AL("%d"), pRecipe->skill_level);
		this->GetDlgItem("Txt_SkillLevel")->SetText(szText);
		idSkillNeed = pRecipe->id_skill;
		nLevelNeed = pRecipe->skill_level;

		break;
	}

	for( i = 0; i < pHost->GetPassiveSkillNum(); i++ )
	{
		pSkill = pHost->GetPassiveSkillByIndex(i);
		if( (pSkill->GetType() == CECSkill::TYPE_LIVE ||
			pSkill->GetType() == CECSkill::TYPE_PRODUCE) &&
			(!pSkillThis || idSkillNeed == pSkill->GetSkillID()) )
		{
			pSkillThis = pSkill;
		}
	}
	if( idSkillNeed > 0 )
	{
		if( pSkillThis )
		{
			if( pSkillThis->GetSkillID() == idSkillNeed )
				this->GetDlgItem("Txt_SkillName")->SetColor(A3DCOLORRGB(255, 255, 255));
			else
			{
				bCan = false;
				this->GetDlgItem("Txt_SkillName")->SetColor(A3DCOLORRGB(128, 128, 128));
			}
			
			if( pSkillThis->GetSkillLevel() >= nLevelNeed )
				this->GetDlgItem("Txt_SkillLevel")->SetColor(A3DCOLORRGB(255, 255, 255));
			else
			{
				bCan = false;
				this->GetDlgItem("Txt_SkillLevel")->SetColor(A3DCOLORRGB(128, 128, 128));
			}

			this->GetDlgItem("Txt_CurrentSkillName")->SetText(
				g_pGame->GetSkillDesc()->GetWideString(pSkillThis->GetSkillID() * 10));
			a_sprintf(szText, _AL("%d"), pSkillThis->GetSkillLevel());
			this->GetDlgItem("Txt_CurrentSkillLevel")->SetText(szText);
		}
		else
		{
			bCan = false;
			this->GetDlgItem("Txt_CurrentSkillName")->SetText(_AL(""));
			this->GetDlgItem("Txt_CurrentSkillLevel")->SetText(_AL(""));
			this->GetDlgItem("Txt_SkillName")->SetColor(A3DCOLORRGB(128, 128, 128));
			this->GetDlgItem("Txt_SkillLevel")->SetColor(A3DCOLORRGB(128, 128, 128));
		}
	}
	else
	{
		this->GetDlgItem("Txt_CurrentSkillName")->SetText(_AL(""));
		this->GetDlgItem("Txt_CurrentSkillLevel")->SetText(_AL(""));
		this->GetDlgItem("Txt_SkillName")->SetColor(A3DCOLORRGB(128, 128, 128));
		this->GetDlgItem("Txt_SkillLevel")->SetColor(A3DCOLORRGB(128, 128, 128));
	}

	CECIvtrEquip *pEquip = (CECIvtrEquip *)pIvtrSrc;
	const void *pData = pDataMan->get_data_ptr(
		pIvtrSrc->GetTemplateID(), ID_SPACE_ESSENCE, DataType);
	if( DataType == DT_WEAPON_ESSENCE )
	{
		__int64 nCur = pEquip->GetCurEndurance();
		__int64 nMax = pEquip->GetMaxEndurance();
		WEAPON_ESSENCE *pEssence = (WEAPON_ESSENCE *)pData;
		
		curPrice = pEssence->decompose_price;
		nNumItem = int(pEssence->element_num * nCur / nMax);
		a_sprintf(szNumber, _AL("%d"), nNumItem);
		idItem = pEssence->element_id;
	}
	else if( DataType == DT_ARMOR_ESSENCE )
	{
		__int64 nCur = pEquip->GetCurEndurance();
		__int64 nMax = pEquip->GetMaxEndurance();
		ARMOR_ESSENCE *pEssence = (ARMOR_ESSENCE *)pData;
		
		curPrice = pEssence->decompose_price;
		nNumItem = int(pEssence->element_num * nCur / nMax);
		a_sprintf(szNumber, _AL("%d"), nNumItem);
		idItem = pEssence->element_id;
	}
	else if( DataType == DT_DECORATION_ESSENCE )
	{
		__int64 nCur = pEquip->GetCurEndurance();
		__int64 nMax = pEquip->GetMaxEndurance();
		DECORATION_ESSENCE *pEssence = (DECORATION_ESSENCE *)pData;
		
		curPrice = pEssence->decompose_price;
		nNumItem = int(pEssence->element_num * nCur / nMax);
		a_sprintf(szNumber, _AL("%d"), nNumItem);
		idItem = pEssence->element_id;
	}

	if( curPrice > pHost->GetMoneyAmount() )
		bCan = false;

	if (bCan &&
		pIvtrSrc->IsEquipment() &&
		((CECIvtrEquip *)pIvtrSrc)->IsDestroying())
		bCan = false;

	A2DSprite *pCover;
	if( idItem > 0 && nNumItem > 0 )
	{
		const void *pData = pDataMan->get_data_ptr(
			idItem, ID_SPACE_ESSENCE, DataType);
		ELEMENT_ESSENCE *pElement = (ELEMENT_ESSENCE *)pData;

		af_GetFileTitle(pElement->file_icon, strFile);
		nItem = GetGameUIMan()->m_IconMap[CECGameUIMan::ICONS_INVENTORY][strFile];
		pCover = GetGameUIMan()->m_pA2DSpriteIcons[CECGameUIMan::ICONS_INVENTORY];
		this->GetDlgItem("Btn_Start")->Enable(bCan);
	}
	else
	{
		pCover = NULL;
		this->GetDlgItem("Btn_Start")->Enable(false);
	}

	pImage = (PAUIIMAGEPICTURE)this->GetDlgItem("Item_b");
	pImage->SetCover(pCover, nItem);

	ACString strPrice;
	this->GetDlgItem("Txt_Gold")->SetText(strPrice.Format(_AL("%d"), curPrice));
	this->GetDlgItem("Txt_no2")->SetText(szNumber);
}