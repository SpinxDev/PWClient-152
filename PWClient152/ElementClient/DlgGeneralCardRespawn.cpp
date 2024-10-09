// File		: DlgGeneralCardRespawn.cpp 
// Creator	: Wang
// Date		: 2013/09/13

#include "DlgGeneralCardRespawn.h"
#include "AUIEditBox.h"
#include "AUIImagePicture.h"
#include "EC_ShortcutMgr.h"
#include "EC_GameSession.h"
#include "EC_GameUIMan.h"
#include "EC_HostPlayer.h"
#include "EC_Inventory.h"
#include "EC_IvtrItem.h"
#include "ExpTypes.h"
#include "AFI.h"
#include "DlgStoneChange.h"
#include "EC_Global.h"
#include "EC_Game.h"
#include "EC_IvtrEquipMatter.h"
#include "DlgInventory.h"
#include <AUICTranslate.h>


#define new A_DEBUG_NEW

AUI_BEGIN_COMMAND_MAP(CDlgGenCardRespawn, CDlgBase)

AUI_ON_COMMAND("Btn_Cancel", OnCommandCANCEL)
AUI_ON_COMMAND("IDCANCEL",		OnCommandCANCEL)
AUI_ON_COMMAND("Btn_Confirm",	OnCommandRespwan)

AUI_END_COMMAND_MAP()


AUI_BEGIN_EVENT_MAP(CDlgGenCardRespawn, CDlgBase)
AUI_ON_EVENT("Img_1", WM_LBUTTONDOWN,	 OnEventLButtonDownMajor)
AUI_ON_EVENT("Img_2", WM_LBUTTONDBLCLK,  OnEventLButtonDownMinor)
AUI_END_EVENT_MAP()

//------------------------------------------------------------------------
// Right-Click Shortcut for CDlgStoneTransfer
//------------------------------------------------------------------------
typedef CECShortcutMgr::SimpleClickShortcut<CDlgGenCardRespawn> InstallClickShortcut;
//------------------------------------------------------------------------

CDlgGenCardRespawn::CDlgGenCardRespawn()
{
}

CDlgGenCardRespawn::~CDlgGenCardRespawn()
{
}

bool CDlgGenCardRespawn::OnInitDialog()
{
	m_pImg_Major = NULL;
	m_pImg_Minor = NULL;
	m_pBtnRespawn = NULL;
//	m_pImg_Target = NULL;

	DDX_Control("Img_1",m_pImg_Major);
	DDX_Control("Img_2",m_pImg_Minor);
	DDX_Control("Btn_Confirm",m_pBtnRespawn);
//	DDX_Control("Img_Final",m_pImg_Target);

	GetGameUIMan()->GetShortcutMgr()->RegisterShortCut(new InstallClickShortcut(this));

	return CDlgBase::OnInitDialog();
}
void CDlgGenCardRespawn::OnShowDialog()
{
	m_pImg_Minor->ClearCover();
	m_pImg_Major->ClearCover();
//	m_pImg_Target->ClearCover();
	
	m_pBtnRespawn->Enable(false);

	m_MajorIndex = -1;
	m_MinorIndex = -1;

	AlignTo(GetGameUIMan()->m_pDlgInventory,
		AUIDIALOG_ALIGN_OUTSIDE, AUIDIALOG_ALIGN_LEFT,
		AUIDIALOG_ALIGN_INSIDE, AUIDIALOG_ALIGN_TOP,
				-60 , 60 );
}
void CDlgGenCardRespawn::OnHideDialog()
{
	ClearCard(0xf);

	GetHostPlayer()->EndNPCService();
}

void CDlgGenCardRespawn::OnCommandCANCEL(const char * szCommand)
{
	Show(false);
}

void CDlgGenCardRespawn::OnCommandRespwan(const char * szCommand)
{
	GetGameUIMan()->MessageBox("GeneralCardRespawn",GetGameUIMan()->GetStringFromTable(11001),MB_OKCANCEL,A3DCOLORRGB(255,255,255));
}
void CDlgGenCardRespawn::DoRespawn()
{
	if(m_MajorIndex>= 0 && m_MinorIndex >= 0)
	{
		GetGameSession()->c2s_CmdNPCSevCardRespawn(m_MajorIndex,m_MinorIndex);
		ClearCard(0xf);
	}
}
void CDlgGenCardRespawn::OnEventLButtonDownMajor(WPARAM wParam, LPARAM lParam, AUIObject * pObj)
{
	if (pObj && strstr(pObj->GetName(),"Img_1"))
	{
		ClearCard(0xf);
	}
}

void CDlgGenCardRespawn::OnEventLButtonDownMinor(WPARAM wParam, LPARAM lParam, AUIObject * pObj)
{
	if (pObj && strstr(pObj->GetName(),"Img_2"))
	{
		ClearCard(0x6);
	}
}
void CDlgGenCardRespawn::OnItemDragDrop(CECIvtrItem* pIvtrSrc, int iSrc, PAUIOBJECT pObjSrc, PAUIOBJECT pObjOver)
{
	if (pObjSrc)
	{
		if(!strstr(pObjSrc->GetParent()->GetName(),"Win_Inventory"))
		{
			GetGameUIMan()->MessageBox("",GetGameUIMan()->GetStringFromTable(11021),MB_OK,A3DCOLORRGB(255,255,255));
			return;
		}
	}
	// for drag-drop scene
	if (IsMajorInstalled())	
		InstallMinor(pIvtrSrc,iSrc);	
	else
		InstallMajor(pIvtrSrc,iSrc);
}

void CDlgGenCardRespawn::InstallMajor(CECIvtrItem* pItem,int iSrc)
{
	if (pItem && CanMajorInstall(pItem))
	{
		ClearCard(0xf);

		pItem->Freeze(true);
		
		AString strFile;
		af_GetFileTitle(pItem->GetIconFile(), strFile);
		strFile.MakeLower();
		m_pImg_Major->SetCover( GetGameUIMan()->GetIconCover(CECGameUIMan::ICONS_INVENTORY),
			GetGameUIMan()->GetIconIndex(CECGameUIMan::ICONS_INVENTORY, strFile));
		
		AUICTranslate trans;
		m_pImg_Major->SetHint(trans.Translate(pItem->GetDesc()));
		m_pImg_Major->SetDataPtr(pItem,"ptr_CECIvtrItem");
		m_MajorIndex = iSrc;
	}	
}
void CDlgGenCardRespawn::InstallMinor(CECIvtrItem* pItem,int iSrc)
{
	if (pItem && CanMinorInstall(pItem))
	{		
		ClearCard(0x6);
		
		pItem->Freeze(true);
		
		AString strFile;
		af_GetFileTitle(pItem->GetIconFile(), strFile);
		strFile.MakeLower();
		m_pImg_Minor->SetCover( GetGameUIMan()->GetIconCover(CECGameUIMan::ICONS_INVENTORY),
			GetGameUIMan()->GetIconIndex(CECGameUIMan::ICONS_INVENTORY, strFile));
		
		AUICTranslate trans;
		m_pImg_Minor->SetHint(trans.Translate(pItem->GetDesc()));
		m_pImg_Minor->SetDataPtr(pItem,"ptr_CECIvtrItem");
		m_MinorIndex = iSrc;

		m_pBtnRespawn->Enable(true);

/*
		CECIvtrItem* pTarget = GenerateTargetCart(GetMajorCard(),pItem);

		af_GetFileTitle(pTarget->GetIconFile(), strFile);
		strFile.MakeLower();
		m_pImg_Target->SetCover( GetGameUIMan()->GetIconCover(CECGameUIMan::ICONS_INVENTORY),
			GetGameUIMan()->GetIconIndex(CECGameUIMan::ICONS_INVENTORY, strFile));
	
		m_pImg_Target->SetHint(trans.Translate(pItem->GetDesc()));
		m_pImg_Target->SetDataPtr(pTarget,"ptr_CECIvtrItem");*/
	}
}
bool CDlgGenCardRespawn::IsMajorInstalled()
{
	return m_pImg_Major->GetDataPtr("ptr_CECIvtrItem") != NULL;
}
CECIvtrItem* CDlgGenCardRespawn::GetMajorCard()
{
	return (CECIvtrItem*)m_pImg_Major->GetDataPtr("ptr_CECIvtrItem");
}
void CDlgGenCardRespawn::ClearCard(int mask)
{
	CECIvtrItem *pItem = NULL;
	
	if ((mask & 1) !=0)
	{
		pItem = (CECIvtrItem*)m_pImg_Major->GetDataPtr("ptr_CECIvtrItem");
		if(pItem)
			pItem->Freeze(false);
		m_pImg_Major->ClearCover();
		m_pImg_Major->SetHint(_AL(""));
		m_pImg_Major->SetDataPtr(NULL,"ptr_CECIvtrItem");
		m_MajorIndex = -1;
	}

	if ((mask & 2) !=0)
	{
	/*	pItem = (CECIvtrItem*)m_pImg_Target->GetDataPtr("ptr_CECIvtrItem");
		if(pItem)
			delete pItem;
		m_pImg_Target->ClearCover();
		m_pImg_Target->SetHint(_AL(""));
		m_pImg_Target->SetDataPtr(NULL,"ptr_CECIvtrItem");*/
	}


	if ((mask & 4) !=0)
	{
		pItem = (CECIvtrItem*)m_pImg_Minor->GetDataPtr("ptr_CECIvtrItem");
		if(pItem)
			pItem->Freeze(false);
		m_pImg_Minor->ClearCover();
		m_pImg_Minor->SetHint(_AL(""));
		m_pImg_Minor->SetDataPtr(NULL,"ptr_CECIvtrItem");
		m_MinorIndex = -1;
	}
	m_pBtnRespawn->Enable(false);
}
bool CDlgGenCardRespawn::CanMajorInstall(CECIvtrItem* pItem)
{
	CECIvtrGeneralCard* pCard = dynamic_cast<CECIvtrGeneralCard*>(pItem);
	if (pCard)
	{
	//	return pCard->GetEssence().rebirth_times < 2 && pCard->GetEssence().level >= pCard->GetEssence().max_level;
		if (pCard->GetEssence().rebirth_times >= 2)
		{
			GetGameUIMan()->MessageBox("",GetGameUIMan()->GetStringFromTable(11016),MB_OK,A3DCOLORRGB(255,255,255));
			return false;
		}
		if (pCard->GetEssence().level < pCard->GetEssence().max_level)
		{
			GetGameUIMan()->MessageBox("",GetGameUIMan()->GetStringFromTable(11017),MB_OK,A3DCOLORRGB(255,255,255));
			return false;
		}
		return true;
	}
	
	GetGameUIMan()->MessageBox("",GetGameUIMan()->GetStringFromTable(11014),MB_OK,A3DCOLORRGB(255,255,255));
	
	return false;
}
bool CDlgGenCardRespawn::CanMinorInstall(CECIvtrItem* pItem)
{
	CECIvtrGeneralCard* pMajor = dynamic_cast<CECIvtrGeneralCard*>(GetMajorCard());
	if (!pMajor) 
	{
		GetGameUIMan()->MessageBox("",GetGameUIMan()->GetStringFromTable(11018),MB_OK,A3DCOLORRGB(255,255,255));
		return false;
	}

	CECIvtrGeneralCard* pMinor = dynamic_cast<CECIvtrGeneralCard*>(pItem);
	if(!pMinor) 
	{
		GetGameUIMan()->MessageBox("",GetGameUIMan()->GetStringFromTable(11015),MB_OK,A3DCOLORRGB(255,255,255));
		return false;
	}

	if(pMajor->GetTemplateID() != pMinor->GetTemplateID()) 
	{
		GetGameUIMan()->MessageBox("",GetGameUIMan()->GetStringFromTable(11019),MB_OK,A3DCOLORRGB(255,255,255));
		return false;
	}

	if(pMinor->GetEssence().rebirth_times != pMajor->GetEssence().rebirth_times)
	{
		GetGameUIMan()->MessageBox("",GetGameUIMan()->GetStringFromTable(11020),MB_OK,A3DCOLORRGB(255,255,255));
		return false;
	}
	
	return true;
}
CECIvtrItem* CDlgGenCardRespawn::GenerateTargetCart(CECIvtrItem* pMajor,CECIvtrItem* pMinor)
{
	if(!pMajor || !pMinor) return false;

	return NULL;
}