// File		: DlgPetRec.cpp
// Creator	: Xiao Zhou
// Date		: 2005/12/29

#include "AFI.h"
#include "DlgPetRec.h"
#include "DlgPetList.h"
#include "EC_GameUIMan.h"
#include "EC_HostPlayer.h"
#include "EC_Game.h"
#include "EC_Pet.h"
#include "EC_PetCorral.h"
#include "elementdataman.h"

#define new A_DEBUG_NEW

AUI_BEGIN_COMMAND_MAP(CDlgPetRec, CDlgBase)

AUI_ON_COMMAND("confirm",		OnCommandConfirm)
AUI_ON_COMMAND("IDCANCEL",		OnCommandCancel)

AUI_END_COMMAND_MAP()

AUI_BEGIN_EVENT_MAP(CDlgPetRec, CDlgBase)

AUI_ON_EVENT("Img_Item",	WM_LBUTTONDOWN,	OnEventLButtonDown)

AUI_END_EVENT_MAP()

CDlgPetRec::CDlgPetRec()
{
	m_pTxt_Gold = NULL;
	m_pTxt_Name = NULL;
	m_pImg_Item = NULL;
}

CDlgPetRec::~CDlgPetRec()
{
}

bool CDlgPetRec::OnInitDialog()
{
	DDX_Control("Txt_Gold", m_pTxt_Gold);
	DDX_Control("Txt_Name", m_pTxt_Name);
	DDX_Control("Img_Item", m_pImg_Item);
	m_nSlot = -1;

	return true;
}

void CDlgPetRec::OnCommandConfirm(const char *szCommand)
{
	if( m_nSlot >= 0 )
	{
		GetHostPlayer()->RestorePet(m_nSlot);
		GetGameUIMan()->EndNPCService();
		Show(false);
		GetGameUIMan()->m_pDlgPetList->Show(false);
	}
}

void CDlgPetRec::OnCommandCancel(const char *szCommand)
{
	GetGameUIMan()->EndNPCService();
	Show(false);
	GetGameUIMan()->m_pDlgPetList->Show(false);
}

void CDlgPetRec::OnShowDialog()
{
	OnEventLButtonDown(0, 0, NULL);
}

void CDlgPetRec::OnEventLButtonDown(WPARAM wParam, LPARAM lParam, AUIObject * pObj)
{
	m_pImg_Item->ClearCover();
	m_pImg_Item->SetHint(_AL(""));
	m_pImg_Item->SetDataPtr(NULL);
	m_pTxt_Gold->SetText(_AL(""));
	m_pTxt_Name->SetText(_AL(""));
	m_nSlot = -1;
}

void CDlgPetRec::SetPet(int nSlot)
{
	CECPetCorral * pPetCorral = GetHostPlayer()->GetPetCorral();
	if( pPetCorral->GetActivePetIndex() == nSlot )
	{
		GetGameUIMan()->MessageBox("", GetStringFromTable(820), MB_OK,
			A3DCOLORRGBA(255, 255, 255, 160));
		return;
	}
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
			m_pTxt_Name->SetText(pPet->GetName());
			ACHAR szText[20];
			a_sprintf(szText, _AL("%d"), pEggDBEssence->money_restored);
			m_pTxt_Gold->SetText(szText);
		}
		m_nSlot = nSlot;
	}
}
