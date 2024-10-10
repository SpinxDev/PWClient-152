// File		: DlgHostPet.cpp
// Creator	: Xiao Zhou
// Date		: 2005/8/17

#include "AFI.h"
#include "DlgHostPet.h"
#include "DlgPetDetail.h"
#include "DlgPetList.h"
#include "EC_Game.h"
#include "EC_GameUIMan.h"
#include "EC_GameSession.h"
#include "EC_HostPlayer.h"
#include "EC_PetCorral.h"
#include "elementdataman.h"
#include "EC_ManNPC.h"
#include "EC_NPC.h"
#include "EC_Pet.h"
#include "EC_World.h"

#define new A_DEBUG_NEW

const A3DCOLOR clHunger[] = 
{
	A3DCOLORRGBA(255, 255, 255, 0),
	A3DCOLORRGBA(0, 255, 0, 64),
	A3DCOLORRGBA(255, 255, 0, 64),
	A3DCOLORRGBA(255, 128, 128, 64),
	A3DCOLORRGBA(255, 0, 0, 64),
	A3DCOLORRGBA(196, 0, 0, 64),
};

AUI_BEGIN_COMMAND_MAP(CDlgHostPet, CDlgBase)

AUI_ON_COMMAND("IDCANCEL",		OnCommand_CANCEL)

AUI_END_COMMAND_MAP()

AUI_BEGIN_EVENT_MAP(CDlgHostPet, CDlgBase)

AUI_ON_EVENT("Icon_Head",		WM_LBUTTONUP,		OnEventLButtonUp)
AUI_ON_EVENT("Icon_Head",		WM_RBUTTONUP,		OnEventRButtonUp)

AUI_END_EVENT_MAP()

CDlgHostPet::CDlgHostPet()
{
	m_nSlot = -1;
}

CDlgHostPet::~CDlgHostPet()
{
}

void CDlgHostPet::OnCommand_CANCEL(const char * szCommand)
{
	m_pAUIManager->RespawnMessage();
}

bool CDlgHostPet::OnInitDialog()
{
	m_pImg_Icon = (PAUIIMAGEPICTURE)GetDlgItem("Icon_Head");
	m_pPro_hp = (PAUIPROGRESS)GetDlgItem("Pro_hp");
	m_pPro_exp = (PAUIPROGRESS)GetDlgItem("Pro_exp");
	m_pPro_loyalty = (PAUIPROGRESS)GetDlgItem("Pro_loyalty");
	m_pTxt_Level = (PAUILABEL)GetDlgItem("Txt_Level");

	return true;
}

bool CDlgHostPet::Render()
{
	bool bRet = CDlgBase::Render();

	m_pImg_Icon->SetHint(_AL(""));
	CECPetCorral * pPetCorral = GetHostPlayer()->GetPetCorral();
	CECPetData * pPet = pPetCorral->GetActivePet();
	if( pPet && pPet->GetClass() == atoi(GetName() + strlen("Win_HpmpxpPet")))
	{
		elementdataman* pDBMan = GetGame()->GetElementDataMan();
		DATA_TYPE DataType = DT_INVALID;

		ACString strText;
		ACString strLoyalty, strHint, strHunger, strHungerHint, strLoyaltyHint;

		int nLoyalty = 0;
		int nLevel = 1;
		int nHunger = 0;

		if( pPet->GetClass() == GP_PET_CLASS_SUMMON )
		{
			nLevel = pPet->GetLevel();
			strText.Format(_AL("%d"), nLevel);
			m_pTxt_Level->SetText(strText);

			// hp bar
			m_pPro_hp->SetProgress(int(pPet->GetHPFactor() * AUIPROGRESS_MAX));
			strText.Format(GetStringFromTable(3627), 
				pPet->GetHP(), pPet->CalcMaxHP());
			m_pPro_hp->SetHint(strText);

			PAUIPROGRESS pPro_Mp = (PAUIPROGRESS)GetDlgItem("Pro_mp");
			pPro_Mp->SetProgress(int(pPet->GetMPFactor() * AUIPROGRESS_MAX));
			strText.Format(GetStringFromTable(3628), 
				pPet->GetMP(), int(pPet->GetMP() / pPet->GetMPFactor()) );
			pPro_Mp->SetHint(strText);
		}
		else if( pPet->GetClass() == GP_PET_CLASS_MOUNT ||
				 pPet->GetClass() == GP_PET_CLASS_COMBAT ||
				 pPet->GetClass() == GP_PET_CLASS_EVOLUTION)
		{
			nLevel = pPet->GetLevel();
			if( m_pTxt_Level )
			{
				strText.Format(_AL("%d"), nLevel);
				m_pTxt_Level->SetText(strText);
			}

			m_pPro_loyalty->SetProgress(pPet->GetIntimacy() * AUIPROGRESS_MAX / 999 );
			if( pPet->GetIntimacy() <= 50 )
				nLoyalty = 0;
			else if( pPet->GetIntimacy() <= 150 )
				nLoyalty = 1;
			else if( pPet->GetIntimacy() <= 500 )
				nLoyalty = 2;
			else
				nLoyalty = 3;
			if( pPet->GetIntimacy() <= 50 )
				strLoyalty = GetStringFromTable(3601);
			else if( pPet->GetIntimacy() <= 150 )
				strLoyalty = GetStringFromTable(3602);
			else if( pPet->GetIntimacy() <= 500 )
				strLoyalty = GetStringFromTable(3603);
			else
				strLoyalty = GetStringFromTable(3604);

			PLAYER_LEVELEXP_CONFIG PetLevelUpExp = *(PLAYER_LEVELEXP_CONFIG*)GetGame()->GetElementDataMan()->
				get_data_ptr(592, ID_SPACE_CONFIG, DataType);

			m_pPro_exp->SetProgress(int(double(pPet->GetExp()) / PetLevelUpExp.exp[pPet->GetLevel() - 1] * AUIPROGRESS_MAX));
			strText.Format(GetStringFromTable(3626), 
				pPet->GetExp(), PetLevelUpExp.exp[pPet->GetLevel() - 1]);
			m_pPro_exp->SetHint(strText);

			if( pPet->GetClass() == GP_PET_CLASS_MOUNT )
				strText.Format(GetStringFromTable(3622), GetStringFromTable(3601 + nLoyalty),
					hintDrop[nLoyalty], hintExp[nLoyalty]);
			else if( pPet->GetClass() == GP_PET_CLASS_COMBAT )
				strText.Format(GetStringFromTable(3623), GetStringFromTable(3601 + nLoyalty),
					hintAttack[nLoyalty], hintExp[nLoyalty]);
			else if(pPet->GetClass() == GP_PET_CLASS_EVOLUTION)
				strText.Format(GetStringFromTable(3623), GetStringFromTable(3601 + nLoyalty),
					hintAttack[nLoyalty], hintExp[nLoyalty]);
			else
				strText = _AL("");
			strLoyaltyHint.Format(GetStringFromTable(3625), pPet->GetIntimacy(), 999, strText);
			m_pPro_loyalty->SetHint(strLoyaltyHint);
			
			if( pPet->GetHunger() == CECPetData::HUNGER_LEVEL_0 )
				nHunger = 0;
			else if( pPet->GetHunger() == CECPetData::HUNGER_LEVEL_1 )
				nHunger = 1;
			else if( pPet->GetHunger() == CECPetData::HUNGER_LEVEL_2 )
				nHunger = 2;
			else if( pPet->GetHunger() <= CECPetData::HUNGER_LEVEL_4 )
				nHunger = 3;
			else if( pPet->GetHunger() <= CECPetData::HUNGER_LEVEL_7 )
				nHunger = 4;
			else
				nHunger = 5;
			strHunger = GetStringFromTable(3611 + nHunger);
			strHungerHint.Format(GetStringFromTable(3624), strHunger,
				hintAddLoyalty[nHunger], hintDecLoyalty[nHunger]);
		}

		
		PET_ESSENCE *pDB = (PET_ESSENCE*)pDBMan->get_data_ptr(pPet->GetTemplateID(), 
			ID_SPACE_ESSENCE, DataType);

		if( pDB )
		{
			AString strFile;
			af_GetFileTitle(pDB->file_icon, strFile);
			strFile.MakeLower();
			m_pImg_Icon->SetCover(GetGameUIMan()->m_pA2DSpriteIcons[CECGameUIMan::ICONS_PET],
				GetGameUIMan()->m_IconMap[CECGameUIMan::ICONS_PET][strFile]);
			float speed = pDB->speed_a + pDB->speed_b * (nLevel - 1);
			if( pPet->GetClass() == GP_PET_CLASS_MOUNT )
				strHint.Format(GetStringFromTable(3621),
					pPet->GetName(), nLevel, speed, strLoyalty,
					pPet->GetIntimacy(), strHunger, strHungerHint );
			else if( pPet->GetClass() == GP_PET_CLASS_COMBAT )
				strHint.Format(GetStringFromTable(3631),
					pPet->GetName(), nLevel, 
					pPet->GetHP(), 
					int(pDB->hp_a * (nLevel - pDB->hp_b * pDB->level_require + pDB->hp_c)),
					int((pDB->damage_a * (pDB->damage_b * nLevel * nLevel + pDB->damage_c * nLevel + pDB->damage_d)) * hintAttack[nLoyalty] / 100),
					1.0f / pDB->attack_speed,
					int(pDB->attack_a * (nLevel - pDB->attack_b * pDB->level_require + pDB->attack_c)),
					int(pDB->physic_defence_a * (pDB->physic_defence_b * (nLevel - pDB->physic_defence_c * pDB->level_require) + pDB->physic_defence_d)),
					int(pDB->armor_a * (nLevel - pDB->armor_b * pDB->level_require + pDB->armor_c)),
					int(pDB->magic_defence_a * (pDB->magic_defence_b * (nLevel - pDB->magic_defence_c * pDB->level_require) + pDB->magic_defence_d)),
					speed, strLoyalty, 
					pPet->GetIntimacy(), strHunger, strHungerHint );
			else if (pPet->GetClass() == GP_PET_CLASS_EVOLUTION)
			{
				strHint.Format(GetStringFromTable(3642),
					pPet->GetName(),nLevel,strLoyalty,pPet->GetIntimacy(),strHunger,strHungerHint);
			}
			else
				strHint.Format(GetStringFromTable(3641),
					pPet->GetName(), speed );
				
			if( m_pPro_hp && (pPet->GetClass() == GP_PET_CLASS_FOLLOW || pPet->GetClass() ==GP_PET_CLASS_COMBAT || pPet->GetClass() == GP_PET_CLASS_EVOLUTION) )
			{
				m_pPro_hp->SetProgress(int(pPet->GetHPFactor() * AUIPROGRESS_MAX));
				strText.Format(GetStringFromTable(3627), pPet->GetHP(), pPet->CalcMaxHP());
				m_pPro_hp->SetHint(strText);
			}
			m_pImg_Icon->SetHint(strHint);
			if( pPet->GetClass() == GP_PET_CLASS_MOUNT || 
				pPet->GetClass() == GP_PET_CLASS_COMBAT ||
				pPet->GetClass() == GP_PET_CLASS_EVOLUTION)
				m_pImg_Icon->SetColor(clHunger[nHunger]);
			else
				m_pImg_Icon->SetColor(A3DCOLORRGB(255, 255, 255));
		}
		else
		{
			m_pImg_Icon->ClearCover();
			m_pImg_Icon->SetHint(_AL(""));
		}
	}
	else
		Show(false);
	return bRet;
}

void CDlgHostPet::OnEventLButtonUp(WPARAM wParam, LPARAM lParam, AUIObject *pObj)
{
	CECNPCMan *pNPCMan = GetWorld()->GetNPCMan();
	const APtrArray<CECNPC *> &NPCs = pNPCMan->GetNPCsInMiniMap();
	int i;
	for( i = 0; i < NPCs.GetSize(); i++ )
	{
		if( NPCs[i]->GetMasterID() == GetHostPlayer()->GetCharacterID() &&
			NPCs[i]->IsSelectable() )
		{
			CECPet* pPetNPC = dynamic_cast<CECPet*>(NPCs[i]);
			if (pPetNPC && !pPetNPC->IsPlantPet()) {
				SetCaptureObject(NULL);
				GetHostPlayer()->SelectTarget(NPCs[i]->GetNPCID());
				break;
			}
		}
	}
}

void CDlgHostPet::OnEventRButtonUp(WPARAM wParam, LPARAM lParam, AUIObject *pObj)
{
	char szText[20];
	int index = GetHostPlayer()->GetPetCorral()->GetActivePetIndex();
	if( index >= 0 )
	{
		sprintf(szText, "detail%d", index + 1);
		GetGameUIMan()->m_pDlgPetList->OnCommandDetail(szText);
	}
}