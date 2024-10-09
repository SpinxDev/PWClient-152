// Filename	: DlgWikiRecipeDetail.cpp
// Creator	: Feng Ning
// Date		: 2010/07/26

#include "DlgWikiRecipeDetail.h"
#include "DlgWikiItem.h"
#include "DlgWikiEquipment.h"
#include "WikiItemProp.h"
#include "WikiEquipmentProp.h"
#include "EC_Global.h"
#include "EC_Game.h"
#include "EC_GameRun.h"
#include "EC_UIManager.h"
#include "EC_GameUIMan.h"
#include "EC_IvtrItem.h"
#include "EC_Skill.h"
#include "AFI.h"
#include "AUIEditbox.h"
#include "AUICTranslate.h"
#include "elementdataman.h"

#define new A_DEBUG_NEW

AUI_BEGIN_COMMAND_MAP(CDlgWikiRecipeDetail, CDlgNameLink)
AUI_END_COMMAND_MAP()

AUI_BEGIN_EVENT_MAP(CDlgWikiRecipeDetail, CDlgNameLink)
AUI_ON_EVENT("Img_*",	WM_LBUTTONDBLCLK,	OnEventLButtonDBLCLK_Icon)
AUI_END_EVENT_MAP()

// =======================================================================
// Link Command
// =======================================================================
CDlgWikiRecipeDetail::ShowSpecficLinkCommand::ShowSpecficLinkCommand(const CachePair& data, 
																  const ACString& name)
																  :m_Data(data)
																  ,m_Name(name)
{
}

CDlgNameLink::LinkCommand* CDlgWikiRecipeDetail::ShowSpecficLinkCommand::Clone() const
{
	return new ShowSpecficLinkCommand(m_Data, m_Name);
}

bool CDlgWikiRecipeDetail::ShowSpecficLinkCommand::operator()(P_AUITEXTAREA_NAME_LINK pLink)
{
	// goto search item
	CDlgWikiRecipeDetail* pDlg = dynamic_cast<CDlgWikiRecipeDetail*>(g_pGame->GetGameRun()->GetUIManager()->
		GetInGameUIMan()->GetDialog("Win_WikiRecipeDetail"));
	ASSERT(pDlg); // should always get this dialog
	
	if(pDlg)
	{
		pDlg->SetDataPtr(&m_Data);
		pDlg->Show(true);
		return true;
	}
	
	return false;
}

ACString CDlgWikiRecipeDetail::ShowSpecficLinkCommand::GetLinkText() const
{
	ACString strLinkedName;
	
	strLinkedName.Format(_AL("^00FF00&%s&^FFFFFF"), m_Name);
	
	return strLinkedName;
}

// =======================================================================
CDlgWikiRecipeDetail::CDlgWikiRecipeDetail()
{
}

bool CDlgWikiRecipeDetail::OnInitDialog()
{
	if (!CDlgNameLink::OnInitDialog())
		return false;
	
	return true;
}

void CDlgWikiRecipeDetail::OnShowDialog()
{
	const RECIPE_ESSENCE *pEssence = NULL;

	// NOTICE: the input data must be this type
	ShowSpecficLinkCommand::CachePair* ptr = (ShowSpecficLinkCommand::CachePair*)this->GetDataPtr();
	if(ptr)
	{
		DATA_TYPE dataType = DT_INVALID;
		pEssence = (const RECIPE_ESSENCE *)GetGame()->GetElementDataMan()->
		get_data_ptr(ptr->first, ID_SPACE_RECIPE, dataType);
		if(dataType != DT_RECIPE_ESSENCE) pEssence = NULL;
	}
	
	PAUILABEL pTxt_ProdutionType = 
		dynamic_cast<PAUILABEL>(GetDlgItem("Txt_ProdutionType"));
	PAUITEXTAREA pTxt_NPC = 
		dynamic_cast<PAUITEXTAREA>(GetDlgItem("Txt_Link_NPC"));
	PAUILABEL pTxt_Gold = 
		dynamic_cast<PAUILABEL>(GetDlgItem("Txt_Gold"));
	PAUILABEL pTxt_Skill = 
		dynamic_cast<PAUILABEL>(GetDlgItem("Txt_Skill"));
	PAUILABEL pTxt_SkillLevel = 
		dynamic_cast<PAUILABEL>(GetDlgItem("Txt_SkillLevel"));

	if(pEssence)
	{
		const WikiNPCMakingCache::Info& info = ptr->second;
		const NPC_ESSENCE* pNPC = info.pNPC;
		const NPC_MAKE_SERVICE* pSrv = info.pSrv;

		BindLinkCommand(pTxt_NPC, NULL,
						&CDlgNameLink::MoveToLinkCommand(pNPC->id, pNPC->name));
		
		pTxt_ProdutionType->SetText(
			GetStringFromTable( (pSrv->produce_type >= 0 && pSrv->produce_type <= 5) ? 
				pSrv->produce_type + 8680 : 574) );

		pTxt_Gold->SetText(GetGameUIMan()->GetFormatNumber(pEssence->price));

		// set skill information
		CECSkill* pSkill = pEssence->id_skill == 0 ? NULL:new CECSkill(pEssence->id_skill, pEssence->skill_level);
		if(pSkill)
		{
			ACString strText = WikiSearchSkillByName::GetLocalizedName(pEssence->id_skill);
			pTxt_Skill->SetText(strText);
			strText.Format(_AL("%d"), pEssence->skill_level);
			pTxt_SkillLevel->SetText(strText);
			delete pSkill;
		}
		else
		{
			pTxt_Skill->SetText(GetStringFromTable(8720));
			pTxt_SkillLevel->SetText(GetStringFromTable(8720));
		}
	}
	else
	{
		pTxt_NPC->SetText(_AL(""));
		pTxt_ProdutionType->SetText(_AL(""));
		pTxt_Gold->SetText(_AL(""));
		pTxt_Skill->SetText(_AL(""));
		pTxt_SkillLevel->SetText(_AL(""));
	}

	ACString strText;
	size_t i;

	size_t iObj = 1;
	size_t maxTarget = !pEssence ? 0:sizeof(pEssence->targets) / sizeof(pEssence->targets[0]);
	for(i = 0;i<maxTarget;i++)
	{
		if(pEssence && (pEssence->targets[i].id_to_make == 0 || pEssence->targets[i].probability == 0.f))
		{
			continue;
		}

		AString strName;
		strName.Format("Img_Target%02d", iObj);
		PAUIIMAGEPICTURE pIcon = 
			dynamic_cast<PAUIIMAGEPICTURE>(GetDlgItem(strName));

		// no more UI container
		if(!pIcon) break;

		bool ret = SetItemInfo( !pEssence ? 0:pEssence->targets[i].id_to_make, 
								!pEssence ? 0: pEssence->num_to_make,
								pIcon);
		
		strName.Format("Lab_Probability%02d", iObj);
		PAUILABEL pTxt_Probability = 
			dynamic_cast<PAUILABEL>(GetDlgItem(strName));

		float ratio = pEssence->targets[i].probability * (1.f - pEssence->fail_probability);
		pTxt_Probability->SetText(!ret ? _AL("") 
			: strText.Format(_AL("%g%%"), ratio * 100.f));

		iObj++;
	}

	// clear the lefts
	while(true)
	{
		AString strName;
		strName.Format("Img_Target%02d", iObj);
		PAUIIMAGEPICTURE pIcon = 
			dynamic_cast<PAUIIMAGEPICTURE>(GetDlgItem(strName));
		// no more UI container
		if(!pIcon) break;

		SetItemInfo(0, 0, pIcon);
		
		strName.Format("Lab_Probability%02d", iObj);
		PAUILABEL pTxt_Probability = 
			dynamic_cast<PAUILABEL>(GetDlgItem(strName));
		pTxt_Probability->SetText(_AL(""));

		iObj++;
	}
	
	
	// set core equip
	iObj = 1;
	if(pEssence && pEssence->id_upgrade_equip > 0)
	{
		AString strName;
		strName.Format("Img_Material%02d", iObj);
		PAUIIMAGEPICTURE pIcon = 
			dynamic_cast<PAUIIMAGEPICTURE>(GetDlgItem(strName));
		if(SetItemInfo( pEssence->id_upgrade_equip, 1, pIcon))
		{
			iObj++;
		}
	}

	// set material
	size_t maxMaterial = !pEssence ? 0:sizeof(pEssence->materials) / sizeof(pEssence->materials[0]);
	for(i = 0;i<maxMaterial;i++)
	{
		if(pEssence && pEssence->materials[i].id == 0)
		{
			continue;
		}

		AString strName;
		strName.Format("Img_Material%02d", iObj);
		PAUIIMAGEPICTURE pIcon = 
			dynamic_cast<PAUIIMAGEPICTURE>(GetDlgItem(strName));

		// no more UI container
		if(!pIcon) break;

		bool ret = SetItemInfo( !pEssence ? 0:pEssence->materials[i].id, 
								!pEssence ? 0:pEssence->materials[i].num, 
								pIcon);

		iObj++;
	}

	// clear the lefts
	while(true)
	{
		AString strName;
		strName.Format("Img_Material%02d", iObj);
		PAUIIMAGEPICTURE pIcon = 
			dynamic_cast<PAUIIMAGEPICTURE>(GetDlgItem(strName));
		// no more UI container
		if(!pIcon) break;
		SetItemInfo(0, 0, pIcon);
		
		iObj++;
	}

	CDlgNameLink::OnShowDialog();
}

bool CDlgWikiRecipeDetail::Release(void)
{
	//
	return CDlgNameLink::Release();
}

bool CDlgWikiRecipeDetail::SetItemInfo(int id, int count, PAUIIMAGEPICTURE pIcon)
{
	if(!pIcon) return false;

	CECIvtrItem* pItem = id > 0 ? CECIvtrItem::CreateItem(id, 0, max(count,1)) : NULL;
	if(!pItem) 
	{
		pIcon->SetText(_AL(""));
		pIcon->SetCover(NULL, -1);
		pIcon->SetHint(_AL(""));
		pIcon->SetData(0);
		return false;
	}
	else
	{
		// refresh the desc
		pItem->GetDetailDataFromLocal();
		
		AString szFile;
		af_GetFileTitle(pItem->GetIconFile(), szFile);
		szFile.MakeLower();
		pIcon->SetCover( GetGameUIMan()->m_pA2DSpriteIcons[CECGameUIMan::ICONS_INVENTORY],
			GetGameUIMan()->m_IconMap[CECGameUIMan::ICONS_INVENTORY][szFile] );
		AUICTranslate trans;
		pIcon->SetHint( trans.Translate(pItem->GetDesc()) );

		ACString strNum;
		pIcon->SetText(count == 0 ? strNum : strNum.Format(_AL("%d"), count));
		
		pIcon->SetData(id);

		delete pItem;
		return true;
	}
}

void CDlgWikiRecipeDetail::OnEventLButtonDBLCLK_Icon(WPARAM wParam, LPARAM lParam, AUIObject * pObj)
{
	if(!pObj) return;

	DWORD id = pObj->GetData();
	if(id == 0) return;

	WikiEquipmentProp* pEqu = WikiEquipmentProp::CreateProperty(id);
	if(pEqu)
	{
		CDlgWikiEquipment::ShowSpecficLinkCommand(id, pEqu->GetName())(NULL);
		delete pEqu;
	}
	else
	{
		WikiItemProp* pItem = WikiItemProp::CreateProperty(id);
		if(pItem)
		{
			CDlgWikiItem::ShowSpecficLinkCommand(id, pItem->GetName())(NULL);
			delete pItem;
		}
	}
}