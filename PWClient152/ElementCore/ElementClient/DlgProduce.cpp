// Filename	: DlgProduce.cpp
// Creator	: Tom Zhou
// Date		: October 11, 2005

#include "AFI.h"
#include "DlgProduce.h"
#include "DlgEquipUp.h"
#include "EC_Global.h"
#include "EC_Game.h"
#include "EC_GameRun.h"
#include "EC_GameSession.h"
#include "EC_GameUIMan.h"
#include "EC_Inventory.h"
#include "EC_HostPlayer.h"
#include "EC_IvtrItem.h"
#include "EC_IvtrMaterial.h"
#include "EC_Skill.h"
#include "AUICTranslate.h"
#include "elementdataman.h"
#include "EC_ShortcutMgr.h"
#include "DlgPreviewProduce.h"
#include "DlgWikiRecipe.h"
#include "DlgAutoHelp.h"

#define new A_DEBUG_NEW

AUI_BEGIN_COMMAND_MAP(CDlgProduce, CDlgBase)

AUI_ON_COMMAND("set*",			OnCommand_set)
AUI_ON_COMMAND("add",			OnCommand_add)
AUI_ON_COMMAND("minus",			OnCommand_minus)
AUI_ON_COMMAND("max",			OnCommand_max)
AUI_ON_COMMAND("begin",			OnCommand_begin)
AUI_ON_COMMAND("IDCANCEL",		OnCommand_CANCEL)

AUI_END_COMMAND_MAP()

AUI_BEGIN_EVENT_MAP(CDlgProduce, CDlgBase)

AUI_ON_EVENT("U_*",			WM_LBUTTONDOWN,		OnEventLButtonDown_U)
AUI_ON_EVENT("Item_*",		WM_LBUTTONDOWN,		OnEventLButtonDown_Item)

AUI_END_EVENT_MAP()

//------------------------------------------------------------------------
// Right-Click Shortcut for CDlgProduce
//------------------------------------------------------------------------
typedef CECShortcutMgr::SimpleClickShortcut<CDlgProduce> InstallClickShortcut;
//------------------------------------------------------------------------


//////////////////////////////////////////////////////////////////////////
//
//
//
//////////////////////////////////////////////////////////////////////////

CDlgProduce::CDlgProduce()
{
	m_nCurShopSet = 1;
	m_iAutoProduceCount = 0;
	m_bProduceNext = false;

	m_produceTimer.Reset();
}

CDlgProduce::~CDlgProduce()
{
}

bool CDlgProduce::OnInitDialog()
{
	GetGameUIMan()->GetShortcutMgr()->RegisterShortCut(new InstallClickShortcut(this));

	int i;
	char szItem[40];

	m_pPrgs_1 = (PAUIPROGRESS)GetDlgItem("Prgs_1");
	m_pTxt_Number = (PAUIOBJECT)GetDlgItem("Txt_Number");
	m_pTxt_Gold = (PAUIOBJECT)GetDlgItem("Txt_Gold");
	m_pTxt_out = (PAUIOBJECT)GetDlgItem("Txt_out");
	m_pItem_out = (PAUIIMAGEPICTURE)GetDlgItem("Item_out");
	m_pBtn_Produce = (PAUISTILLIMAGEBUTTON)GetDlgItem("Btn_Produce");
	m_pTxt_CurrentSkillName = (PAUILABEL)GetDlgItem("Txt_CurrentSkillName");
	m_pTxt_CurrentSkillLevel = (PAUILABEL)GetDlgItem("Txt_CurrentSkillLevel");
	m_pPrgs_Level = (PAUIPROGRESS)GetDlgItem("Prgs_Level");
	m_pTxt_SkillLevel = (PAUILABEL)GetDlgItem("Txt_SkillLevel");
	m_pTxt_SkillName = (PAUILABEL)GetDlgItem("Txt_SkillName");
	m_pTxt_Bind = (PAUILABEL)GetDlgItem("Txt_Bind");

	for( i = 0; i < NUM_NPCIVTR; i++ )
	{
		sprintf(szItem, "Btn_Set%d", i + 1);
		m_pBtnSet[i] = (PAUISTILLIMAGEBUTTON)GetDlgItem(szItem);
	}

	for( i = 0; i < IVTRSIZE_NPCPACK; i++ )
	{
		sprintf(szItem, "U_%d", i + 1);
		m_pImgU[i] = (PAUIIMAGEPICTURE)GetDlgItem(szItem);
	}

	for( i = 0; i < CDLGPRODUCE_MAX_MATERIALS; i++ )
	{
		sprintf(szItem, "Txt_no%d", i + 1);
		m_pTxt_no[i] = (PAUIOBJECT)GetDlgItem(szItem);

		sprintf(szItem, "Item_%d", i + 1);
		m_pImgItem[i] = (PAUIIMAGEPICTURE)GetDlgItem(szItem);
	}

	return true;
}

void CDlgProduce::OnCommand_set(const char * szCommand)
{
	if( m_pPrgs_1->GetData() > 0 )
	{
		int nSet = atoi(szCommand + strlen("set"));
		m_pBtnSet[nSet - 1]->SetPushed(nSet == m_nCurShopSet);
	}
	else
	{
		NPC_MAKE_SERVICE *pService = (NPC_MAKE_SERVICE *)GetDataPtr("ptr_NPC_MAKE_SERVICE");
		if( pService->produce_type == 1 ||
			pService->produce_type == 3 ||
			pService->produce_type == 4 ||
			pService->produce_type == 5)
			ClearMaterial();
		UpdateProduce(atoi(szCommand + strlen("set")), 0);
	}
}

void CDlgProduce::SetNumber(int nNewNum)
{
	if (m_pPrgs_1->GetData() > 0){
		return;
	}
	
	int nMax = m_pTxt_Number->GetData();
	int nNum = a_atoi(m_pTxt_Number->GetText());
	
	if (nNum > 0 && nNewNum > 0 && nNewNum != nNum && nNewNum <= nMax)
	{
		int nCost = a_atoi(m_pTxt_Gold->GetText());
		int nTotal = a_atoi(m_pTxt_out->GetText());
		
		int nPrice = nCost / nNum;
		int nCount = nTotal / nNum;
		
		ACHAR szText[40];
		a_sprintf(szText, _AL("%d"), nNewNum);
		m_pTxt_Number->SetText(szText);
		a_sprintf(szText, _AL("%d"), nPrice * nNewNum);
		m_pTxt_Gold->SetText(szText);
		a_sprintf(szText, _AL("%d"), nCount * nNewNum);
		m_pTxt_out->SetText(szText);
	}
}

void CDlgProduce::OnCommand_add(const char * szCommand)
{
	if( m_pPrgs_1->GetData() > 0 ) return;

	int nMax = m_pTxt_Number->GetData();
	int nNum = a_atoi(m_pTxt_Number->GetText());
	if( nNum < nMax ){
		SetNumber(nNum+1);
	}
}

void CDlgProduce::OnCommand_minus(const char * szCommand)
{
	if( m_pPrgs_1->GetData() > 0 ) return;
	int nNum = a_atoi(m_pTxt_Number->GetText());
	if (nNum > 1){
		SetNumber(nNum-1);
	}
}

void CDlgProduce::OnCommand_max(const char * szCommand)
{
	if( m_pPrgs_1->GetData() > 0 ) return;

	int nMax = m_pTxt_Number->GetData();
	int nNum = a_atoi(m_pTxt_Number->GetText());

	if( nMax > nNum ){
		SetNumber(nMax);
	}
}

bool CDlgProduce::CheckProfRestrict()
{
	AString strItem;
	strItem.Format("U_%02d", m_nCurThing + 1);
	PAUIOBJECT pObj = GetDlgItem(strItem);
	if( !pObj ) return false;
	CECIvtrItem* pItem = (CECIvtrItem*)pObj->GetDataPtr("ptr_CECIvtrItem");
	if( !pItem ) return false;
	CECIvtrRecipe* pRecipe = dynamic_cast<CECIvtrRecipe*>(pItem);
	if( !pRecipe ) return false;
	if( pRecipe->GetDBEssence()->character_combo_id &&
		!(pRecipe->GetDBEssence()->character_combo_id & (1 << GetHostPlayer()->GetProfession())) )
	{
		ACString strMsg;
		ACString strProf;		
		CECStringTab* pDescTab = g_pGame->GetItemDesc();

		for (int i=0; i < NUM_PROFESSION; i++)
		{
			if (pRecipe->GetDBEssence()->character_combo_id & (1 << i))
			{
				strProf += GetGameRun()->GetProfName(i);
				strProf += _AL(" ");
			}
		}
		
		strMsg.Format(GetStringFromTable(985), strProf);
		GetGameUIMan()->MessageBox("", strMsg, MB_OK, A3DCOLORRGB(255, 255, 255));
		return false;
	}

	return true;
}

void CDlgProduce::OnCommand_begin(const char * szCommand)
{
	if( m_pPrgs_1->GetData() > 0 ) return;

	// 生产的职业限制
	if( !CheckProfRestrict() )
		return;

	// show a msgbox if triggered by UI click
	if(!m_Confirm.IsEmpty() && szCommand != NULL)
	{
		GetGameUIMan()->MessageBox("Game_ProduceConfirm", m_Confirm, MB_YESNO, A3DCOLORRGBA(255, 255, 255, 160));
		return;
	}

	NPC_MAKE_SERVICE *pService = (NPC_MAKE_SERVICE *)GetDataPtr("ptr_NPC_MAKE_SERVICE");
	int idRecipe = (int)m_pItem_out->GetData();

	m_bProduceNext = false;

	if( stricmp(m_szName, "Win_Produce1") == 0 )
	{
		GetGameSession()->c2s_CmdNPCSevMakeItem(pService->id_make_skill, idRecipe, 1);
		m_iAutoProduceCount = 0;
	}
	else
	{
		if( m_pItem_out->GetData() == 0 ) return;

		int nMax = m_pTxt_Number->GetData();
		int nNum = a_atoi(m_pTxt_Number->GetText());
		int nCost = a_atoi(m_pTxt_Gold->GetText());
		int nTotal = a_atoi(m_pTxt_out->GetText());

		DWORD dwCount = a_atoi(m_pTxt_Number->GetText());
		
		if( pService->produce_type == 1 )
		{
			int id[16];
			int index[16];

			memset(id, 0, sizeof(int) * 16);
			memset(index, 0, sizeof(int) * 16);
			for(int j = 0; j < CDLGPRODUCE_MAX_MATERIALS; j++ )
			{
				char szName[20];
				sprintf(szName, "Item_%02d", j + 1);
				PAUIIMAGEPICTURE pObj = (PAUIIMAGEPICTURE)GetDlgItem(szName);
				if( !pObj ) break;
				if( m_nRecipeIndexMaterail[j] != -1 )
				{
					id[m_nRecipeIndexMaterail[j]] = pObj->GetData();
					index[m_nRecipeIndexMaterail[j]] = m_nSoltMaterail[j];
				}
			}
			GetGameSession()->c2s_CmdNPCSevCompose(pService->id_make_skill, idRecipe, id, index);			
			if (szCommand){
				// 发起第一次合成，记录和显示剩余次数（含当前发起的生产，因为可能失败）
				m_iAutoProduceCount = (int)dwCount;
				SetNumber(m_iAutoProduceCount);
			}
		}
		else if (pService->produce_type == 3 || pService->produce_type == 4)
		{
			CDlgEquipUp *pEquipUp = GetGameUIMan()->m_pDlgEquipUp;
			CECInventory *pPack = GetHostPlayer()->GetPack();
			CECIvtrItem *pItem = pPack->GetItem(m_nSoltMaterail[0]);
			if (pItem)
			{
				char szItem[40];
				sprintf(szItem, "U_%02d", m_nCurThing + 1);
				CECIvtrRecipe *pItem2 = (CECIvtrRecipe *)GetDlgItem(szItem)->GetDataPtr("ptr_CECIvtrItem");
				pEquipUp->SetEquipUp(pItem, pItem2, false);
				pEquipUp->Show(true, true);
			}
			m_iAutoProduceCount = 0;
			return;
		}else if (pService->produce_type == 5)
		{
			//向服务器发送协议获取能够继承的属性的信息，锁定生产按钮直到服务器回复
			CDlgEquipUp *pEquipUp = GetGameUIMan()->m_pDlgEquipUp;
			CECInventory *pPack = GetHostPlayer()->GetPack();
			CECIvtrItem *pItem = pPack->GetItem(m_nSoltMaterail[0]);
			if (pItem)
			{
				GetGameSession()->c2s_CmdQueryEquipmentPropData(pItem->GetTemplateID(), m_nSoltMaterail[0]);
				m_pBtn_Produce->Enable(false);
				m_produceTimer.Reset(3000);
			}
			m_iAutoProduceCount = 0;
			return;
		}

		else
		{
			GetGameSession()->c2s_CmdNPCSevMakeItem(pService->id_make_skill, idRecipe, dwCount);
			m_iAutoProduceCount = 0;
		}
	}
	m_pBtn_Produce->Enable(false);
}

void CDlgProduce::UpgradeEquipment(char inherit_type)
{
	NPC_MAKE_SERVICE *pService = (NPC_MAKE_SERVICE *)GetDataPtr("ptr_NPC_MAKE_SERVICE");
	if (pService->produce_type != 3 && pService->produce_type != 4 && pService->produce_type != 5)
		return;

	bool inheritRefine(false); 
	bool inheritHole(false); 
	bool inheritStone(false);
	bool inheritEngrave(false); 
	bool inheritAddons(false);
	if (inherit_type & INHERIT_REFINE) inheritRefine = true;
	if (inherit_type & INHERIT_HOLE) inheritHole = true;
	if (inherit_type & INHERIT_STONE) inheritStone = true;
	if (inherit_type & INHERIT_ENGRAVE) inheritEngrave = true;
	if (inherit_type & INHERIT_ADDONS) inheritAddons = true;
	

	int id[16] = {0};
	int index[16] = {0};
	for(int j = 1; j < CDLGPRODUCE_MAX_MATERIALS; j++ )
	{
		char szName[20];
		sprintf(szName, "Item_%02d", j + 1);
		PAUIIMAGEPICTURE pObj = (PAUIIMAGEPICTURE)GetDlgItem(szName);
		if( !pObj ) break;
		if( m_nRecipeIndexMaterail[j] != -1 )
		{
			id[m_nRecipeIndexMaterail[j]] = pObj->GetData();
			index[m_nRecipeIndexMaterail[j]] = m_nSoltMaterail[j];
		}
	}
	
	char szItem[40];
	sprintf(szItem, "U_%02d", m_nCurThing + 1);
	CECIvtrRecipe *pItem = (CECIvtrRecipe *)GetDlgItem(szItem)->GetDataPtr("ptr_CECIvtrItem");	
	const RECIPE_ESSENCE *pRecipe = (const RECIPE_ESSENCE *)pItem->GetDBEssence();
	if (pService->produce_type == 3)
	{
		GetGameSession()->c2s_CmdNPCSevLevelUpProduce(pService->id_make_skill, pRecipe->id, id, index, pRecipe->id_upgrade_equip, m_nSoltMaterail[0], inherit_type);
		ClearMaterial();
	}
	else if (pService->produce_type == 4)
	{
		CDlgPreviewProduce* pDlg = dynamic_cast<CDlgPreviewProduce*>(GetGameUIMan()->GetDialog("Win_PreviewProduce"));
		if (pDlg)
		{
			CECInventory *pPack = GetHostPlayer()->GetPack();
			CECIvtrItem *pItem = pPack->GetItem(m_nSoltMaterail[0]);
			pDlg->SetOldInfo(pItem, pRecipe->id_upgrade_equip, inheritRefine, inheritHole, inheritStone,inheritEngrave);
		}
		GetGameSession()->c2s_CmdNPCSevLevelUpProduce(pService->id_make_skill, pRecipe->id, id, index, pRecipe->id_upgrade_equip, m_nSoltMaterail[0], inherit_type, true);
	}else if (pService->produce_type == 5)
	{
		GetGameSession()->c2s_CmdNPCSevLevelUpProduce(pService->id_make_skill, pRecipe->id, id, index, pRecipe->id_upgrade_equip, m_nSoltMaterail[0], inherit_type, false, true);
		ClearMaterial();
	}
	
	UpdateProduce(m_nCurShopSet, m_nCurThing);
}

void CDlgProduce::OnCommand_CANCEL(const char * szCommand)
{
	PAUIPROGRESS pProgress = (PAUIPROGRESS)m_pPrgs_1;

	if( pProgress->GetData() > 0 )
	{
		GetGameSession()->c2s_CmdCancelAction();
		m_iAutoProduceCount = 0;
		m_bProduceNext = false;
	}
	else
	{		     
		Show(false);
		NPC_MAKE_SERVICE *pService = (NPC_MAKE_SERVICE *)GetDataPtr("ptr_NPC_MAKE_SERVICE");
		GetGameUIMan()->GetDialog("Win_Inventory")->Show(false);
		GetGameUIMan()->EndNPCService();
		GetHostPlayer()->GetPack()->UnfreezeAllItems();
		GetGameUIMan()->m_pDlgPreviewProduce->OnCommandCANCEL(NULL);
	}
}

void CDlgProduce::ClearMaterial()
{	
	CECInventory *pPack = GetHostPlayer()->GetPack();
	int nIndex = 0;
	for(int j = 0; j < CDLGPRODUCE_MAX_MATERIALS; j++ )
	{
		char szName[20];
		sprintf(szName, "Item_%02d", nIndex + 1);
		PAUIIMAGEPICTURE pObj = (PAUIIMAGEPICTURE)GetDlgItem(szName);
		if( !pObj ) break;

		sprintf(szName, "Txt_no%d", nIndex + 1);
		GetDlgItem(szName)->SetText(_AL(""));
		if (m_nSoltMaterail[j] >= 0){
			if (CECIvtrItem *pItem = pPack->GetItem(m_nSoltMaterail[j])){
				if (pItem->GetTemplateID() == (int)pObj->GetData()){
					pItem->Freeze(false);
				}
			}
		}
		pObj->SetDataPtr(NULL);
		pObj->SetData(0);
		pObj->ClearCover();
		pObj->SetHint(_AL(""));
		pObj->SetColor(A3DCOLORRGB(128, 128, 128));
		m_nSoltMaterail[j] = -1;
		
		nIndex++;
	}
}

void CDlgProduce::BuildButton()
{
	char szItem[40];
	CECNPCInventory *pDeal = NULL;
	int i, nTotalSets = 0;
	PAUIOBJECT pBtn;
	
	for( i = 0; i < NUM_NPCIVTR; i++ )
	{
		pDeal = GetHostPlayer()->GetNPCSevPack(i);
		if( a_strlen(pDeal->GetName()) <= 0 ) continue;
		
		sprintf(szItem, "Btn_Set%d", nTotalSets + 1);
		pBtn = GetDlgItem(szItem);
		pBtn->SetText(pDeal->GetName());
		pBtn->SetData((DWORD)i);
		pBtn->Show(true);
		
		nTotalSets++;
	}
	for( i = nTotalSets; i < NUM_NPCIVTR; i++ )
	{
		sprintf(szItem, "Btn_Set%d", i + 1);
		GetDlgItem(szItem)->Show(false);
	}
}

void CDlgProduce::ShowNPCPack(int nPack)
{		
	AString strFile;
	char szItem[40];
	CECIvtrRecipe *pItem = NULL;
	PAUIIMAGEPICTURE pObj = NULL;
	
	CECNPCInventory *pDeal = GetHostPlayer()->GetNPCSevPack(nPack);
	elementdataman *pDataMan = g_pGame->GetElementDataMan();
	for(int i = 0; i < IVTRSIZE_NPCPACK; i++ )
	{
		sprintf(szItem, "U_%02d", i + 1);
		pObj = (PAUIIMAGEPICTURE)GetDlgItem(szItem);
		if( !pObj ) continue;
		
		// 清除高亮边框
		pObj->SetCover(NULL, -1, 1);
		
		pItem = (CECIvtrRecipe *)pDeal->GetItem(i);
		if( pItem && pItem->GetClassID() == CECIvtrItem::ICID_RECIPE )
		{
			pObj->SetDataPtr(pItem,"ptr_CECIvtrItem");
			pObj->SetColor(A3DCOLORRGB(255, 255, 255));
			
			af_GetFileTitle(pItem->GetIconFile(), strFile);
			strFile.MakeLower();
			pObj->SetCover(GetGameUIMan()->m_pA2DSpriteIcons[CECGameUIMan::ICONS_INVENTORY],
				GetGameUIMan()->m_IconMap[CECGameUIMan::ICONS_INVENTORY][strFile]);
		}
		else
		{
			pObj->SetCover(NULL, -1);
			pObj->SetDataPtr(NULL);
		}
		pObj->SetHint(_AL(""));
	}
}

bool CDlgProduce::UpdateProduce(int nSet, int nThing)
{
	AString strFile;
	char szItem[40];
	CECIvtrRecipe *pItem;
	PAUIIMAGEPICTURE pObj;
	PAUISTILLIMAGEBUTTON pButton;
	const RECIPE_ESSENCE *pRecipe;
	AUICTranslate trans;

	NPC_MAKE_SERVICE *pService = (NPC_MAKE_SERVICE *)GetDataPtr("ptr_NPC_MAKE_SERVICE");

	if( nSet == 1 ){		// Only build buttons when nSet is 1.
		BuildButton();		
	}
	
	sprintf(szItem, "Btn_Set%d", m_nCurShopSet);
	pButton = (PAUISTILLIMAGEBUTTON)GetDlgItem(szItem);
	pButton->SetPushed(false);
	
	m_nCurShopSet = nSet;
	m_nCurThing = nThing;
	
	sprintf(szItem, "Btn_Set%d", m_nCurShopSet);
	pButton = (PAUISTILLIMAGEBUTTON)GetDlgItem(szItem);
	pButton->SetPushed(true);
	
	ShowNPCPack(pButton->GetData());	
	
	m_pTxt_Bind->Show(false);
	if( nThing >= 0 )
	{
		int j;
		char szName[40];
		
		if( pService->produce_type == 0 )
		{
			for( j = 0; j < CDLGPRODUCE_MAX_MATERIALS; j++ )
			{
				sprintf(szName, "Item_%02d", j + 1);
				pObj = (PAUIIMAGEPICTURE)GetDlgItem(szName);
				
				pObj->ClearCover();
				pObj->SetHint(_AL(""));
				pObj->SetDataPtr(NULL);
				pObj->SetColor(A3DCOLORRGB(255, 255, 255));
				
				sprintf(szName, "Txt_no%d", j + 1);
				GetDlgItem(szName)->SetText(_AL(""));
			}
		}
		
		m_pItem_out->ClearCover();
		m_pItem_out->SetHint(_AL(""));
		m_pItem_out->SetDataPtr(NULL);
		m_pItem_out->SetColor(A3DCOLORRGB(255, 255, 255));
		m_pItem_out->SetData(0);
		m_pBtn_Produce->Enable(false);

		sprintf(szItem, "U_%02d", nThing + 1);
		pItem = (CECIvtrRecipe *)GetDlgItem(szItem)->GetDataPtr("ptr_CECIvtrItem");
		if( !pItem ) return true;

		// 高亮边框
		PAUIIMAGEPICTURE pImgLight = (PAUIIMAGEPICTURE)GetDlgItem(szItem);
		pImgLight->SetCover(GetGameUIMan()->m_pA2DSpriteMask, 0, 1);

		ACHAR szText[40];
		bool bCanMake = true;
		int idSkillNeed = -1;
		CECSkill *pSkill, *pSkillThis = NULL;

		pRecipe = (const RECIPE_ESSENCE *)pItem->GetDBEssence();
		idSkillNeed = pRecipe->id_skill;

		for( j = 0; j < GetHostPlayer()->GetPassiveSkillNum(); j++ )
		{
			pSkill = GetHostPlayer()->GetPassiveSkillByIndex(j);
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
				if( pSkillThis->GetSkillID() != idSkillNeed ||
					pSkillThis->GetSkillLevel() < pRecipe->skill_level )
				{
					bCanMake = false;
				}
				m_pTxt_CurrentSkillName->SetText(
					g_pGame->GetSkillDesc()->GetWideString(pSkillThis->GetSkillID() * 10));
				a_sprintf(szText, _AL("%d"), pSkillThis->GetSkillLevel());
				m_pTxt_CurrentSkillLevel->SetText(szText);
				m_pPrgs_Level->SetProgress(GetHostPlayer()->
					GetSkillAbilityPercent(pSkillThis->GetSkillID()));
				a_sprintf(szText, _AL("%d%c"), GetHostPlayer()->GetSkillAbilityPercent(
					pSkillThis->GetSkillID()), '%');
				m_pPrgs_Level->SetHint(szText);
				m_pPrgs_Level->SetData(pSkillThis->GetSkillID());
			}
			else
			{
				bCanMake = false;
				m_pTxt_CurrentSkillName->SetText(_AL(""));
				m_pTxt_CurrentSkillLevel->SetText(_AL(""));
				m_pPrgs_Level->SetProgress(0);
				m_pPrgs_Level->SetHint(_AL(""));
			}

			a_sprintf(szText, _AL("%d"), pRecipe->skill_level);
			m_pTxt_SkillLevel->SetText(szText);
			m_pTxt_SkillName->SetText(
				g_pGame->GetSkillDesc()->GetWideString(pRecipe->id_skill * 10));
		}
		else
		{
			m_pTxt_SkillName->SetText(_AL(""));
			m_pTxt_SkillLevel->SetText(_AL(""));
		}

		if( pSkillThis && pSkillThis->GetSkillID() == idSkillNeed )
		{
			if( pSkillThis->GetSkillLevel() >= pRecipe->skill_level )
				m_pTxt_SkillLevel->SetColor(A3DCOLORRGB(255, 255, 255));
			else
				m_pTxt_SkillLevel->SetColor(A3DCOLORRGB(128, 128, 128));
			m_pTxt_SkillName->SetColor(A3DCOLORRGB(255, 255, 255));
		}
		else
		{
			m_pTxt_SkillName->SetColor(A3DCOLORRGB(128, 128, 128));
			m_pTxt_SkillLevel->SetColor(A3DCOLORRGB(128, 128, 128));
		}

		CECInventory *pPack = GetHostPlayer()->GetPack();
		int nIndex = 0, nCount, nMax = 0xFFFFFFF;
		
		// record binding type in materials
		bool bBind = false;
		bool bBindAfterEquip = false;
		bool bBindWebTrade = false;

		for( j = 0; j < CDLGPRODUCE_MAX_MATERIALS; j++ )
			m_nRecipeIndexMaterail[j] = -1;
		if( pService->produce_type != 2 )
		{
			if (pService->produce_type == 3 || pService->produce_type == 4 || pService->produce_type == 5)
			{
				// Fill Item_01 with id_upgrade_equip
				//
				sprintf(szName, "Item_%02d", nIndex + 1);
				pObj = (PAUIIMAGEPICTURE)GetDlgItem(szName);
				sprintf(szName, "Txt_no%d", nIndex + 1);

				m_nRecipeIndexMaterail[nIndex] = -1;
				bool validUpgradeEquip(false);
				unsigned int idEquipUp = pRecipe->id_upgrade_equip;
				while (idEquipUp > 0)
				{
					CECIvtrItem *pItem = CECIvtrItem::CreateItem(idEquipUp, 0, 1);
					if (!pItem)
					{
						ASSERT(false);
						break;
					}
					pItem->GetDetailDataFromLocal();
					
					af_GetFileTitle(pItem->GetIconFile(), strFile);
					strFile.MakeLower();
					pObj->SetCover(
						GetGameUIMan()->m_pA2DSpriteIcons[CECGameUIMan::ICONS_INVENTORY],
						GetGameUIMan()->m_IconMap[CECGameUIMan::ICONS_INVENTORY][strFile]);
					pObj->SetHint(trans.Translate(pItem->GetName()));
					pObj->SetData(idEquipUp);

					nCount = 0;
					if (pObj->GetDataPtr("ptr_CECIvtrItem"))
					{
						CECIvtrItem *pItem2 = pPack->GetItem(m_nSoltMaterail[nIndex]);
						if (pItem2 && pItem2 == pObj->GetDataPtr("ptr_CECIvtrItem") )
						{
							nCount = pItem2->GetCount();
							pObj->SetHint(pItem2->GetDesc());

							if( pItem2->GetProcType() & CECIvtrItem::PROC_BIND )
								bBindAfterEquip = true;

							if( pItem2->GetProcType() & CECIvtrItem::PROC_BINDING )
							{
								// webtrade is a subtype of binding
								if( pItem2->GetProcType() & CECIvtrItem::PROC_CAN_WEBTRADE )
								{
									bBindWebTrade = true;
								}
								else
								{
									bBind = true;
								}
							}
						}
						else
						{
							m_nSoltMaterail[nIndex] = -1;
							pObj->SetDataPtr(NULL);
							pObj->ClearCover();
							pObj->SetHint(_AL(""));
							pObj->SetColor(A3DCOLORRGB(128, 128, 128));
							a_sprintf(szText, _AL("%d/%d"), nCount, 1);
							GetDlgItem(szName)->SetText(szText);
							GetDlgItem(szName)->SetColor(A3DCOLORRGB(192, 0, 0));
						}
					}

					delete pItem;
					
					nMax = min(nMax, nCount);
					nMax = max(nMax, 1);
					a_sprintf(szText, _AL("%d/%d"), nCount, 1);
					if (nCount < 1)
					{
						GetDlgItem(szName)->SetColor(A3DCOLORRGB(192, 0, 0));
						bCanMake = false;
					}
					else
					{
						GetDlgItem(szName)->SetColor(A3DCOLORRGB(255, 255, 255));
					}
					if( pObj->GetDataPtr("ptr_CECIvtrItem") )
					{
						pObj->SetColor(A3DCOLORRGB(255, 255, 255));
					}
					else
					{
						pObj->SetColor(A3DCOLORRGB(128, 128, 128));
						bCanMake = false;
					}
					GetDlgItem(szName)->SetText(szText);
					validUpgradeEquip = true;
					break;
				}
				if (!validUpgradeEquip)
				{
					m_nSoltMaterail[nIndex] = -1;
					
					pObj->ClearCover();
					pObj->SetHint(_AL(""));
					pObj->SetData(0);
					pObj->SetDataPtr(NULL);
					pObj->SetColor(A3DCOLORRGB(255, 255, 255));
					
					GetDlgItem(szName)->SetText(_AL(""));
				}
				nIndex ++;
			}
			
			for( j = 0; j < ARRAY_SIZE(pRecipe->materials); j++ )
			{
				sprintf(szName, "Item_%02d", nIndex + 1);
				pObj = (PAUIIMAGEPICTURE)GetDlgItem(szName);
				if( !pObj ) break;

				if( pRecipe->materials[j].id <= 0 || pRecipe->materials[j].num <= 0 )
					continue;

				m_nRecipeIndexMaterail[nIndex] = j;
				sprintf(szName, "Txt_no%d", nIndex + 1);
				nCount = 0;
				CECIvtrItem *pItem = CECIvtrItem::CreateItem(pRecipe->materials[j].id, 0, 1);
				if( pItem )
				{
					pItem->GetDetailDataFromLocal();
					af_GetFileTitle(pItem->GetIconFile(), strFile);
					strFile.MakeLower();
					pObj->SetCover(GetGameUIMan()->m_pA2DSpriteIcons[CECGameUIMan::ICONS_INVENTORY],
						GetGameUIMan()->m_IconMap[CECGameUIMan::ICONS_INVENTORY][strFile]);
					pObj->SetHint(trans.Translate(pItem->GetDesc()));

					if( pService->produce_type == 1 ||
						pService->produce_type == 3 ||
						pService->produce_type == 4	||
						pService->produce_type == 5)
					{
						pObj->SetData(pRecipe->materials[j].id);
						nCount = 0;
						if( pObj->GetDataPtr("ptr_CECIvtrItem") )
						{
							CECIvtrItem *pItemInPack = pPack->GetItem(m_nSoltMaterail[nIndex]);
							if( pItemInPack && pItemInPack->GetTemplateID() == (int)pObj->GetData() )
							{
								nCount = pItemInPack->GetCount();
								
								if( pItemInPack->GetProcType() & CECIvtrItem::PROC_BIND )
									bBindAfterEquip = true;

								if( pItemInPack->GetProcType() & CECIvtrItem::PROC_BINDING )
								{
									// webtrade is a subtype of binding
									if( pItemInPack->GetProcType() & CECIvtrItem::PROC_CAN_WEBTRADE )
									{
										bBindWebTrade = true;
									}
									else
									{
										bBind = true;
									}
								}

								pObj->SetHint(pItemInPack->GetDesc());
							}
							else
							{
								m_nSoltMaterail[nIndex] = -1;
								pObj->SetDataPtr(NULL);
								a_sprintf(szText, _AL("%d/%d"), nCount, pRecipe->materials[j].num);
								GetDlgItem(szName)->SetText(szText);
								GetDlgItem(szName)->SetColor(A3DCOLORRGB(192, 0, 0));
							}
						}
					}
					else
						nCount = GetGameUIMan()->GetItemCount(pRecipe->materials[j].id);
					delete pItem;
				}
				else
				{
					ASSERT(false);
					continue;
				}
				
				nMax = min(nMax, nCount / pRecipe->materials[j].num);
				if( pService->produce_type == 1 ||
					pService->produce_type == 3 ||
					pService->produce_type == 4 ||
					pService->produce_type == 5)
				{
					nMax = max(nMax, 1);
					a_sprintf(szText, _AL("%d/%d"), nCount, pRecipe->materials[j].num);
					if( nCount < pRecipe->materials[j].num )
					{
						GetDlgItem(szName)->SetColor(A3DCOLORRGB(192, 0, 0));
						bCanMake = false;
					}
					else
						GetDlgItem(szName)->SetColor(A3DCOLORRGB(255, 255, 255));
					if( pObj->GetDataPtr("ptr_CECIvtrItem") )
					{
						pObj->SetColor(A3DCOLORRGB(255, 255, 255));
					}
					else
					{
						pObj->SetColor(A3DCOLORRGB(128, 128, 128));
						bCanMake = false;
					}
				}
				else
				{
					a_sprintf(szText, _AL("%d"), pRecipe->materials[j].num);
					GetDlgItem(szName)->SetColor(A3DCOLORRGB(255, 255, 255));
					if( nCount < pRecipe->materials[j].num )
					{
						bCanMake = false;
						pObj->SetColor(A3DCOLORRGB(128, 128, 128));
					}
					else
						pObj->SetColor(A3DCOLORRGB(255, 255, 255));
				}
				GetDlgItem(szName)->SetText(szText);
				GetDlgItem(szName)->SetData(pRecipe->materials[j].id);


				nIndex++;
			}
		}

		int nTimes = min(1, nMax);
		if(nTimes<1) nTimes = 1; // 即使不满足生产条件 也显示生产物品的数量等信息

		a_sprintf(szText, _AL("%d"), pRecipe->price * nTimes);
		m_pTxt_Gold->SetText(szText);
		if( pRecipe->price * nTimes > GetHostPlayer()->GetMoneyAmount() )
		{
			bCanMake = false;
			m_pTxt_Gold->SetColor(A3DCOLORRGB(255, 0, 0));
		}
		else
			m_pTxt_Gold->SetColor(A3DCOLORRGB(255, 255, 255));

		m_Confirm.Empty();
		m_pItem_out->SetData(pRecipe->id);
		if( pService->produce_type != 2 )
		{
			a_sprintf(szText, _AL("%d"), nTimes);
			m_pTxt_Number->SetText(szText);
			m_pTxt_Number->SetData(nMax);
			a_sprintf(szText, _AL("%d"), pRecipe->num_to_make * nTimes);
			m_pTxt_out->SetText(szText);

			pObj = (PAUIIMAGEPICTURE)m_pItem_out;
			CECIvtrItem *pProduceItem = CECIvtrItem::CreateItem((int)pRecipe->targets[0].id_to_make, 0, 1);
			af_GetFileTitle(pProduceItem->GetIconFile(), strFile);
			pProduceItem->GetDetailDataFromLocal();
			pProduceItem->SetInNPCPack(true);
			m_pItem_out->SetHint(trans.Translate(pProduceItem->GetDesc()));
			strFile.MakeLower();
			pObj->SetCover(GetGameUIMan()->m_pA2DSpriteIcons[CECGameUIMan::ICONS_INVENTORY],
				GetGameUIMan()->m_IconMap[CECGameUIMan::ICONS_INVENTORY][strFile]);
			delete pProduceItem;

			ACString strInfo, strHint, strConfirm;
			if( pRecipe->bind_type == 1 )
			{
				m_pTxt_Bind->Show(true);
				strInfo.Format(GetStringFromTable(897), GetStringFromTable(882));
				strConfirm.Format(GetStringFromTable(909), GetStringFromTable(882));
			}
			else if( pRecipe->bind_type == 2 )
			{
				m_pTxt_Bind->Show(true);

				// show alert message for each bind type
				if( bBind )
				{
					strInfo.Format(GetStringFromTable(897), GetStringFromTable(882));
					strConfirm.Format(GetStringFromTable(898), GetStringFromTable(882), GetStringFromTable(882));
					strHint = GetStringFromTable(884);
				}
				else if( bBindWebTrade )
				{
					strInfo.Format(GetStringFromTable(897), GetStringFromTable(896));
					strConfirm.Format(GetStringFromTable(898), GetStringFromTable(896), GetStringFromTable(896));
					strHint = GetStringFromTable(884);
				}
				else if( bBindAfterEquip )
				{
					strInfo.Format(GetStringFromTable(897), GetStringFromTable(883));
					strConfirm.Format(GetStringFromTable(898), GetStringFromTable(883), GetStringFromTable(883));
					strHint = GetStringFromTable(884);
				}
				else
				{
					m_pTxt_Bind->Show(false);
				}
			}
			m_pTxt_Bind->SetText(strInfo);
			m_pTxt_Bind->SetHint(strHint);

			m_Confirm = trans.Translate(strConfirm);
			if( pRecipe->bind_type == 0 )
			{
				if (pRecipe->proc_type & CECIvtrItem::PROC_TRADEABLE)
					m_Confirm += GetStringFromTable(920);
				else if (pRecipe->proc_type & CECIvtrItem::PROC_DEADDROP)
					m_Confirm += GetStringFromTable(921);
				else if (pRecipe->proc_type & CECIvtrItem::PROC_NO_USER_TRASH)
					m_Confirm += GetStringFromTable(922);
				else if (pRecipe->proc_type & CECIvtrItem::PROC_DROPPABLE)
					m_Confirm += GetStringFromTable(923);
			}
		}

		CDlgPreviewProduce* pDlg = dynamic_cast<CDlgPreviewProduce*>(GetGameUIMan()->GetDialog("Win_PreviewProduce"));
		if( bCanMake )
		{
			if (pDlg)
			{
				pDlg->ONCanProduce();
			}
			m_pBtn_Produce->Enable(true);
			m_pItem_out->SetColor(A3DCOLORRGB(255, 255, 255));
		}
		else
		{
			if (pDlg)
			{
				pDlg->OnCannotProduce();
			}
			m_pBtn_Produce->Enable(false);
			m_pItem_out->SetColor(A3DCOLORRGB(128, 128, 128));
		}
	}

	return true;
}

int CDlgProduce::GetCurShopSet()
{
	return m_nCurShopSet;
}

int CDlgProduce::GetCurThing()
{
	return m_nCurThing;
}

void CDlgProduce::OnEventLButtonDown_U(WPARAM wParam, LPARAM lParam, AUIObject * pObj)
{
	if( pObj->GetDataPtr("ptr_CECIvtrItem") && m_pPrgs_1->GetData() <= 0 )
	{		
		NPC_MAKE_SERVICE *pService = (NPC_MAKE_SERVICE *)GetDataPtr("ptr_NPC_MAKE_SERVICE");
		if( pService->produce_type == 1 ||
			pService->produce_type == 3 ||
			pService->produce_type == 4	||
			pService->produce_type == 5)
			ClearMaterial();
		UpdateProduce(m_nCurShopSet, atoi(pObj->GetName() + strlen("U_")) - 1);

		CECIvtrItem* pItem = (CECIvtrItem*)pObj->GetDataPtr("ptr_CECIvtrItem");
		if (pItem && CDlgAutoHelp::IsAutoHelp())
		{
			CECIvtrRecipe* recipe = dynamic_cast<CECIvtrRecipe*>(pItem);
			CDlgWikiShortcut::PopItemWiki(GetGameUIMan(),recipe->GetItem1());
		}		
	}
}

void CDlgProduce::OnEventLButtonDown_Item(WPARAM wParam, LPARAM lParam, AUIObject * pObj)
{
	if (CDlgAutoHelp::IsAutoHelp())
	{
		AString name = pObj->GetName();
		if (name != "Item_out")
		{
			name.CutLeft(strlen("Item_"));
			int idx = name.ToInt()-1;
			a_Clamp(idx,0,7);
			int tid = m_pTxt_no[idx]->GetData();
			if (tid>0)
			{
				CECIvtrItem* pItem = CECIvtrItem::CreateItem(tid,0,1);
				if (pItem)
				{
					CDlgWikiShortcut::PopItemWiki(GetGameUIMan(),pItem);
					delete pItem;
				}
			}
		}
	}	
	
	if( !pObj->GetDataPtr("ptr_CECIvtrItem") )
		return;
	
	CECIvtrItem *pItem = (CECIvtrItem*)pObj->GetDataPtr("ptr_CECIvtrItem");
	if( pItem )
	{
		pItem->Freeze(false);
		PAUIIMAGEPICTURE pImage = (PAUIIMAGEPICTURE)pObj;
		int iSlot = atoi(pObj->GetName() + strlen("Item_")) - 1;
		m_nSoltMaterail[iSlot] = -1;
		pImage->SetDataPtr(NULL);
		UpdateProduce(m_nCurShopSet, m_nCurThing);
	}
}

void CDlgProduce::SetMaterial(int iSrc, int iDst)
{
	NPC_MAKE_SERVICE *pService = (NPC_MAKE_SERVICE *)GetDataPtr("ptr_NPC_MAKE_SERVICE");
	if( pService->produce_type != 1 &&
		pService->produce_type != 3 &&
		pService->produce_type != 4	&&
		pService->produce_type != 5)
		return;

	char szName[20];
	sprintf(szName, "Item_%02d", iDst);
	PAUIIMAGEPICTURE pImage = (PAUIIMAGEPICTURE)GetDlgItem(szName);
	OnEventLButtonDown_Item(0, 0, pImage);

	CECIvtrItem *pItem = GetHostPlayer()->GetPack()->GetItem(iSrc);
	m_nSoltMaterail[iDst - 1] = iSrc;
	pImage->SetDataPtr(pItem, "ptr_CECIvtrItem");
	pItem->Freeze(true);
	UpdateProduce(m_nCurShopSet, m_nCurThing);
}

void CDlgProduce::OnChangeLayoutEnd(bool bAllDone)
{
	if(bAllDone)
	{
		// update the text
		if(IsShow())
		{
			UpdateProduce(1, 0);
		}
	}
}

void CDlgProduce::OnItemDragDrop(CECIvtrItem* pIvtrSrc, int iSrc, PAUIOBJECT pObjSrc, PAUIOBJECT pObjOver)
{
	// only in produce dialog
	if( 0 == stricmp(this->GetName(), "Win_Produce") )
	{		
		// find material item in recipe
		if (!pObjOver)
		{
			int i=1;
			while (i<=CDLGPRODUCE_MAX_MATERIALS)
			{
				AString str;
				str.Format("Item_%02d",i);
				pObjOver = GetDlgItem(str);
				if (pObjOver && pIvtrSrc->GetTemplateID() == (int)pObjOver->GetData())
				{
					int has=0,need=0;
					AString strText;
					strText.Format("Txt_no%d",i);
					PAUIOBJECT pText = GetDlgItem(strText);
					if(pText) 
					{
						ACString text = pText->GetText();
						if(	text.Find(_AL("/"))>=0)
						{
							swscanf(text,_AL("%d/%d"),&has,&need);
							if (has<need) // 材料没有放满
								break;
						}						
					}						
					i++;					
				}
				else i++;
			}
			
			if(i>CDLGPRODUCE_MAX_MATERIALS) return;
		}

		// for drag-drop scene
		if( !pObjOver || !strstr(pObjOver->GetName(), "Item_") )
		{
			return;
		}
		
		if( pIvtrSrc->GetTemplateID() == (int)pObjOver->GetData() )
		{
			int iDst = atoi(pObjOver->GetName() + strlen("Item_"));
			this->SetMaterial(iSrc, iDst);
		}
	}
}

void CDlgProduce::OnMessageBox(AUIDialog* pDlg, int iRetVal)
{
	if(iRetVal == IDYES && !stricmp(pDlg->GetName(), "Game_ProduceConfirm"))
	{
		OnCommand_begin(NULL);
	}
}

void CDlgProduce::OnProduceSucc()
{
	m_bProduceNext = true;
}

void CDlgProduce::OnProduceEnd()
{
	if( !m_bProduceNext )
	{
		m_iAutoProduceCount = 0;
	}
	else if( m_iAutoProduceCount > 1 )
	{
		m_bProduceNext = false;
		SetNumber(--m_iAutoProduceCount);
		OnCommand_begin(NULL);
	}
}

bool CDlgProduce::Tick()
{
	if (m_produceTimer.TimeLength() > 0 && m_produceTimer.IsTimeArrived())	{
		m_pBtn_Produce->Enable(true);
		m_produceTimer.Reset(0);
	}
	return CDlgBase::Tick();
}

void CDlgProduce::OpenAdvancedEquipUp(int ivtrindex, int equipid, int addonsNum, int* addonsIdList)
{
	CDlgEquipUp *pEquipUp = GetGameUIMan()->m_pDlgEquipUp;
	//pEquipUp->SetAdvancedEquipUp();
	CECInventory *pPack = GetHostPlayer()->GetPack();
	if (m_nSoltMaterail[0] != ivtrindex)
		return;
	CECIvtrItem *pItem = pPack->GetItem(m_nSoltMaterail[0]);
	if (pItem && pItem->GetTemplateID() == equipid)
	{
		char szItem[40];
		sprintf(szItem, "U_%02d", m_nCurThing + 1);
		CECIvtrRecipe *pItem2 = (CECIvtrRecipe *)GetDlgItem(szItem)->GetDataPtr("ptr_CECIvtrItem");
		pEquipUp->SetInheritableAddons(addonsNum, addonsIdList);
		pEquipUp->SetEquipUp(pItem, pItem2, true);
		pEquipUp->Show(true, true);
		m_pBtn_Produce->Enable(true);//打开成功，解锁生产按钮
	}
}