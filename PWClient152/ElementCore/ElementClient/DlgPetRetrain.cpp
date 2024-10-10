// File		: DlgPetRetrain.cpp
// Creator	: Xiao Zhou
// Date		: 2006/6/22

#include "AFI.h"
#include "DlgPetRetrain.h"
#include "DlgInventory.h"
#include "EC_GameUIMan.h"
#include "EC_GameSession.h"
#include "EC_HostPlayer.h"
#include "EC_Inventory.h"
#include "EC_IvtrItem.h"
#include "EC_PetCorral.h"
#include "EC_Game.h"
#include "EC_Skill.h"
#include "elementdataman.h"
#include "EC_Global.h"

#define new A_DEBUG_NEW

AUI_BEGIN_COMMAND_MAP(CDlgPetRetrain, CDlgBase)

AUI_ON_COMMAND("confirm",		OnCommandConfirm)
AUI_ON_COMMAND("IDCANCEL",		OnCommandCancel)

AUI_END_COMMAND_MAP()

AUI_BEGIN_EVENT_MAP(CDlgPetRetrain, CDlgBase)

AUI_ON_EVENT("Img_Skill*",	WM_LBUTTONDOWN,	OnEventLButtonDownSkill)

AUI_END_EVENT_MAP()

CDlgPetRetrain::CDlgPetRetrain()
{
}

CDlgPetRetrain::~CDlgPetRetrain()
{
}

bool CDlgPetRetrain::OnInitDialog()
{
	m_pTxt_PetName = (PAUILABEL)GetDlgItem("Txt_PetName");
	m_pTxt_PetLevel = (PAUILABEL)GetDlgItem("Txt_PetLevel");
	m_pTxt_no = (PAUILABEL)GetDlgItem("Txt_no");
	m_pImg_Item = (PAUIIMAGEPICTURE)GetDlgItem("Img_Item");
	for(int i = 0; i < 4; i++ )
	{
		char szText[20];
		sprintf(szText, "Img_Skill%d", i + 1);
		m_pImg_Skill[i] = (PAUIIMAGEPICTURE)GetDlgItem(szText);
		sprintf(szText, "Txt_Skill%d", i + 1);
		m_pTxt_Skill[i] = (PAUILABEL)GetDlgItem(szText);
	}

	return true;
}

void CDlgPetRetrain::OnCommandConfirm(const char *szCommand)
{
	GetGameSession()->c2s_CmdNPCSevPetSkillDel(m_nSkillID);
	OnCommandCancel("");
}

void CDlgPetRetrain::OnCommandCancel(const char *szCommand)
{
	GetGameUIMan()->EndNPCService();
	Show(false);
	GetGameUIMan()->m_pDlgInventory->Show(false);
}

void CDlgPetRetrain::OnShowDialog()
{
	CECPetCorral * pPetCorral = GetHostPlayer()->GetPetCorral();
	CECPetData * pPet = pPetCorral->GetActivePet();
	m_nSkillID = 0;
	ACHAR szText[1000];
	if( pPet )
	{
		elementdataman* pDB = GetGame()->GetElementDataMan();
		DATA_TYPE DataType;
		PET_ESSENCE *pDBEssence = (PET_ESSENCE*)pDB->get_data_ptr(pPet->GetTemplateID(), 
			ID_SPACE_ESSENCE, DataType);

		if( pDBEssence )
		{
			AString strFile;
			af_GetFileTitle(pDBEssence->file_icon, strFile);
			strFile.MakeLower();
			m_pImg_Item->SetCover(GetGameUIMan()->m_pA2DSpriteIcons[CECGameUIMan::ICONS_INVENTORY],
				GetGameUIMan()->m_IconMap[CECGameUIMan::ICONS_INVENTORY][strFile]);
			m_pTxt_PetName->SetText(pPet->GetName());
			ACHAR szText[20];
			a_sprintf(szText, _AL("%d"), pPet->GetLevel());
			m_pTxt_PetLevel->SetText(szText);
		}
		for(int i = 0; i < 4; i++ )
		{
			const CECPetData::PETSKILL *pSkill = pPet->GetSkill(CECPetData::EM_SKILL_NORMAL,i);
			if( pSkill && pSkill->idSkill != 0 )
			{
				m_pImg_Skill[i]->Show(true);
				m_pTxt_Skill[i]->Show(true);
				m_pImg_Skill[i]->ClearCover();
				const wchar_t* szName = g_pGame->GetSkillDesc()->GetWideString(pSkill->idSkill * 10);
				CECSkill::GetDesc(pSkill->idSkill, pSkill->iLevel, szText, 1000);
				m_pImg_Skill[i]->SetHint(szText);
				const char* szIcon = GNET::ElementSkill::GetIcon(pSkill->idSkill);
				AString strFile;
				af_GetFileTitle(szIcon, strFile);
				strFile.MakeLower();
				m_pImg_Skill[i]->SetCover(GetGameUIMan()->m_pA2DSpriteIcons[CECGameUIMan::ICONS_SKILL],
					GetGameUIMan()->m_IconMap[CECGameUIMan::ICONS_SKILL][strFile]);
				m_pTxt_Skill[i]->SetText(szName);
				m_pImg_Skill[i]->SetData(pSkill->idSkill);
			}
			else
			{
				m_pImg_Skill[i]->Show(false);
				m_pTxt_Skill[i]->Show(false);
			}
		}
	}
}

void CDlgPetRetrain::OnEventLButtonDownSkill(WPARAM wParam, LPARAM lParam, AUIObject * pObj)
{
	int nSkill = atoi(pObj->GetName() + strlen("Img_Skill")) - 1;
	for(int i = 0; i < 4; i++ )
		m_pImg_Skill[i]->SetCover(NULL, -1, 1);
	m_nSkillID = pObj->GetData();
	m_pImg_Skill[nSkill]->SetCover(GetGameUIMan()->m_pA2DSpriteMask, 0, 1);
}

void CDlgPetRetrain::OnTick()
{
	ACHAR szText[20];
	int nItem = GetHostPlayer()->GetPack()->GetItemTotalNum(11690);
	if( nItem > 0 )
		m_pTxt_no->SetColor(A3DCOLORRGB(255, 255, 255));
	else
		m_pTxt_no->SetColor(A3DCOLORRGB(255, 0, 0));
	a_sprintf(szText, _AL("%d"), nItem);
	m_pTxt_no->SetText(szText);
	CECInventory *pPack = GetHostPlayer()->GetPack();
	GetDlgItem("Btn_Confirm")->Enable(m_nSkillID != 0 && nItem > 0);
}
