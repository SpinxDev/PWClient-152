// File		: DlgPetRename.cpp
// Creator	: Xiao Zhou
// Date		: 2006/6/22

#include "AFI.h"
#include "DlgPetRename.h"
#include "EC_GameUIMan.h"
#include "EC_GameSession.h"
#include "EC_HostPlayer.h"
#include "DlgPetList.h"
#include "EC_Game.h"
#include "EC_GameRun.h"
#include "elementdataman.h"
#include "EC_PetCorral.h"
#include "EC_UIManager.h"

#define new A_DEBUG_NEW

AUI_BEGIN_COMMAND_MAP(CDlgPetRename, CDlgBase)

AUI_ON_COMMAND("confirm",		OnCommandConfirm)
AUI_ON_COMMAND("IDCANCEL",		OnCommandCancel)

AUI_END_COMMAND_MAP()

AUI_BEGIN_EVENT_MAP(CDlgPetRename, CDlgBase)

AUI_ON_EVENT("Img_Item",	WM_LBUTTONDOWN,	OnEventLButtonDown)

AUI_END_EVENT_MAP()

CDlgPetRename::CDlgPetRename()
{
}

CDlgPetRename::~CDlgPetRename()
{
}

bool CDlgPetRename::OnInitDialog()
{
	m_pTxt_PetName = (PAUILABEL)GetDlgItem("Txt_PetName");
	m_pTxt_PetLevel = (PAUILABEL)GetDlgItem("Txt_PetLevel");
	m_pTxt_PetNewName = GetDlgItem("Txt_PetNewName");
	m_pImg_Item = (PAUIIMAGEPICTURE)GetDlgItem("Img_Item");
	m_nSlot = -1;

	return true;
}

void CDlgPetRename::OnCommandConfirm(const char *szCommand)
{
	if( m_nSlot >= 0 )
	{
		ACString strName = m_pTxt_PetNewName->GetText();
		if (GetGameRun()->GetUIManager()->HasBadWords(strName))
		{
			GetGameUIMan()->MessageBox("", GetStringFromTable(816),	MB_OK, A3DCOLORRGB(255, 255, 255));
			return;
		}

		m_pTxt_PetNewName->SetText(strName);
		GetGameSession()->c2s_CmdNPCSevPetName(m_nSlot, (void*)m_pTxt_PetNewName->GetText(), 
			strName.GetLength() * sizeof(ACHAR));
		GetGameUIMan()->EndNPCService();
		Show(false);
		GetGameUIMan()->m_pDlgPetList->Show(false);
	}
}

void CDlgPetRename::OnCommandCancel(const char *szCommand)
{
	GetGameUIMan()->EndNPCService();
	Show(false);
	GetGameUIMan()->m_pDlgPetList->Show(false);
}

void CDlgPetRename::OnShowDialog()
{
	OnEventLButtonDown(0, 0, NULL);
}

void CDlgPetRename::OnEventLButtonDown(WPARAM wParam, LPARAM lParam, AUIObject * pObj)
{
	m_pImg_Item->ClearCover();
	m_pImg_Item->SetHint(_AL(""));
	m_pImg_Item->SetDataPtr(NULL);
	m_pTxt_PetName->SetText(_AL(""));
	m_pTxt_PetLevel->SetText(_AL(""));
	m_pTxt_PetNewName->SetText(_AL(""));
	m_nSlot = -1;
}

void CDlgPetRename::SetPet(int nSlot)
{
	CECPetCorral * pPetCorral = GetHostPlayer()->GetPetCorral();
	elementdataman* pDB = GetGame()->GetElementDataMan();
	DATA_TYPE DataType;
	
	CECPetData * pPet = pPetCorral->GetPetData(nSlot);
	if( pPet )
	{
		PET_ESSENCE *pDBEssence = (PET_ESSENCE*)pDB->get_data_ptr(pPet->GetTemplateID(), 
			ID_SPACE_ESSENCE, DataType);

		PET_EGG_ESSENCE *pEggDBEssence = (PET_EGG_ESSENCE*)pDB->get_data_ptr(pPet->GetEggID(), 
			ID_SPACE_ESSENCE, DataType);
		if( pDBEssence && pEggDBEssence )
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
		m_nSlot = nSlot;
	}
}

void CDlgPetRename::OnTick()
{
	GetDlgItem("Btn_Confirm")->Enable(m_nSlot != -1 &&
		a_strlen(m_pTxt_PetNewName->GetText()) > 0);
}
