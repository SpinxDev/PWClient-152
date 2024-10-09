// Filename	: DlgELFRefineTrans.cpp
// Creator	: Chen Zhixin
// Date		: October 17, 2008

#include "WindowsX.h"
#include "AFI.h"
#include "AUICTranslate.h"
#include "AUIFrame.h"
#include "AUICommon.h"
#include "AUIManager.h"
#include "DlgELFRefineTrans.h"
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
#include "EC_IvtrGoblin.h"
#include "EC_ShortcutMgr.h"

#define new A_DEBUG_NEW
#define INTERAL_LONG	500
#define INTERAL_SHORT	200
#define TEMPERED_TIME	3000

AUI_BEGIN_COMMAND_MAP(CDlgELFRefineTrans, CDlgBase)

AUI_ON_COMMAND("IDCANCEL",		OnCommand_CANCEL)
AUI_ON_COMMAND("confirm",		OnCommand_Confirm)

AUI_END_COMMAND_MAP()

AUI_BEGIN_EVENT_MAP(CDlgELFRefineTrans, CDlgBase)

AUI_ON_EVENT("Img_Target",		WM_LBUTTONDOWN,		OnEventLButtonDown_Target)
AUI_ON_EVENT("Img_Source",		WM_LBUTTONDOWN,		OnEventLButtonDown_Source)

AUI_END_EVENT_MAP()

//------------------------------------------------------------------------
// Right-Click Shortcut for CDlgELFRefineTrans
//------------------------------------------------------------------------
typedef CECShortcutMgr::SimpleClickShortcut<CDlgELFRefineTrans> ELFRefineTransClickShortcut;
//------------------------------------------------------------------------

CDlgELFRefineTrans::CDlgELFRefineTrans()
{
	m_pTxt_Source = NULL;
	m_pTxt_Target = NULL;
	m_pImg_Source = NULL;
	m_pImg_Target = NULL;
	m_pTxt_Result = NULL;
	m_pTxt_Count = NULL;
	m_pBtn_Confirm = NULL;
	m_nSlotSource = -1;
	m_nSlotTarget = -1;
}

CDlgELFRefineTrans::~CDlgELFRefineTrans()
{
}

bool CDlgELFRefineTrans::OnInitDialog()
{
	GetGameUIMan()->GetShortcutMgr()->RegisterShortCut(new ELFRefineTransClickShortcut(this));

	DDX_Control("Img_Source", m_pImg_Source);
	DDX_Control("Txt_Source", m_pTxt_Source);

	DDX_Control("Img_Target", m_pImg_Target);
	DDX_Control("Txt_Target", m_pTxt_Target);
	
	DDX_Control("Txt_TransResult", m_pTxt_Result);
	DDX_Control("Txt_Count", m_pTxt_Count);

	DDX_Control("Btn_Confirm", m_pBtn_Confirm);

	return true;
}

void CDlgELFRefineTrans::OnShowDialog()
{
	OnEventLButtonDown_Source(0, 0, NULL);
	m_nLastSlotSource = -1;
	m_nLastSlotTarget = -1;
	m_pTxt_Count->SetText(_AL("0"));
	m_pTxt_Result->SetText(_AL("0"));
}

void CDlgELFRefineTrans::OnTick(void)
{
}

bool CDlgELFRefineTrans::Render(void)
{

//	RefreshHostDetails();

	return CDlgBase::Render();
}

void CDlgELFRefineTrans::RefreshHostDetails()
{
 	if( !IsShow() ) return;

	if( m_pImg_Source->GetDataPtr() && m_pImg_Target->GetDataPtr()) 
	{
		CECIvtrGoblin *pELF = (CECIvtrGoblin*)GetHostPlayer()->GetPack()->GetItem(m_nSlotSource);
		CECIvtrGoblin *pELF2 = (CECIvtrGoblin*)GetHostPlayer()->GetPack()->GetItem(m_nSlotTarget);
		if (pELF && pELF2)
		{
			const IVTR_ESSENCE_GOBLIN essence = pELF->GetEssence();
			const IVTR_ESSENCE_GOBLIN essence2 = pELF2->GetEssence();
			if (essence.data.refine_level > essence2.data.refine_level )
			{
				if (GetHostPlayer()->GetPack()->GetItemTotalNum(23551) >= pELF->GetRefineMaterial(essence.data.refine_level))
				{
					ACHAR szText[20];
					a_sprintf(szText, _AL("%d"), essence.data.refine_level);
					m_pTxt_Result->SetText(szText);
					m_pTxt_Result->SetColor(A3DCOLORRGB(255, 255, 255));

					a_sprintf(szText, _AL("%d"), pELF->GetRefineMaterial(essence.data.refine_level));
					m_pTxt_Count->SetText(szText);
					m_pTxt_Count->SetColor(A3DCOLORRGB(255, 255, 255));

					m_pBtn_Confirm->Enable(true);
				}
				else
				{
					ACHAR szText[20];
					a_sprintf(szText, _AL("%d"), essence.data.refine_level);
					m_pTxt_Result->SetText(szText);
					m_pTxt_Result->SetColor(A3DCOLORRGB(255, 0, 0));
					
					a_sprintf(szText, _AL("%d"), pELF->GetRefineMaterial(essence.data.refine_level));
					m_pTxt_Count->SetText(szText);
					m_pTxt_Count->SetColor(A3DCOLORRGB(255, 0, 0));
					
					m_pBtn_Confirm->Enable(false);
				}
				
			}
			else
			{
				m_pTxt_Result->SetText(_AL(""));
				m_pTxt_Count->SetText(_AL(""));
				m_pBtn_Confirm->Enable(false);
			}
		}
	}
	else
	{
		m_pTxt_Result->SetText(_AL(""));
		m_pTxt_Result->SetColor(A3DCOLORRGB(255, 255, 255));

		m_pTxt_Count->SetText(_AL(""));
		m_pTxt_Count->SetColor(A3DCOLORRGB(255, 255, 255));

		m_pBtn_Confirm->Enable(false);
	}
}

void CDlgELFRefineTrans::SetSource(CECIvtrItem *pItem1, int nSlot)
{
	CECIvtrGoblin* pELF = (CECIvtrGoblin*)pItem1;
	OnEventLButtonDown_Source(0, 0, NULL);
	m_pImg_Source->SetDataPtr(pELF);
	m_nSlotSource = nSlot;
	pItem1->Freeze(true);
	AString strFile;
	af_GetFileTitle(pELF->GetIconFile(), strFile);
	strFile.MakeLower();
	m_pImg_Source->SetCover(GetGameUIMan()->m_pA2DSpriteIcons[CECGameUIMan::ICONS_INVENTORY],
		GetGameUIMan()->m_IconMap[CECGameUIMan::ICONS_INVENTORY][strFile]);
	AUICTranslate trans;
	const ACHAR * szDesc = pELF->GetDesc();
	if( szDesc )
		m_pImg_Source->SetHint(trans.Translate(szDesc));
	else
		m_pImg_Source->SetHint(_AL(""));
	m_pTxt_Source->SetText(pELF->GetName());
	RefreshHostDetails();
}

void CDlgELFRefineTrans::SetTarget(CECIvtrItem *pItem1, int nSlot)
{
	CECIvtrGoblin* pELF = (CECIvtrGoblin*)pItem1;
	OnEventLButtonDown_Target(0, 0, NULL);
	m_pImg_Target->SetDataPtr(pELF);
	m_nSlotTarget = nSlot;
	pItem1->Freeze(true);
	AString strFile;
	af_GetFileTitle(pELF->GetIconFile(), strFile);
	strFile.MakeLower();
	m_pImg_Target->SetCover(GetGameUIMan()->m_pA2DSpriteIcons[CECGameUIMan::ICONS_INVENTORY],
		GetGameUIMan()->m_IconMap[CECGameUIMan::ICONS_INVENTORY][strFile]);
	AUICTranslate trans;
	const ACHAR * szDesc = pELF->GetDesc();
	if( szDesc )
		m_pImg_Target->SetHint(trans.Translate(szDesc));
	else
		m_pImg_Target->SetHint(_AL(""));
	m_pTxt_Target->SetText(pELF->GetName());
	RefreshHostDetails();
}

void CDlgELFRefineTrans::OnCommand_Confirm(const char *szCommand)
{
	CECIvtrGoblin* pELF = (CECIvtrGoblin*)m_pImg_Source->GetDataPtr();
	CECIvtrGoblin* pELF2 = (CECIvtrGoblin*)m_pImg_Target->GetDataPtr();
	if( pELF && pELF2 && m_nSlotSource >= 0 && m_nSlotTarget >= 0) 
	{
		GetGameSession()->c2s_CmdNPCSevGoblinTransferRefine(m_nSlotSource, m_nSlotTarget);
		m_nLastSlotSource = m_nSlotSource;
		m_nLastSlotTarget = m_nSlotTarget;
		OnEventLButtonDown_Source(0, 0, NULL);
		OnEventLButtonDown_Target(0, 0, NULL);
		m_pBtn_Confirm->Enable(false);
	}
	RefreshHostDetails();
}

void CDlgELFRefineTrans::OnCommand_CANCEL(const char * szCommand)
{
	OnEventLButtonDown_Source(0, 0, NULL);
	OnEventLButtonDown_Target(0, 0, NULL);
	GetGameUIMan()->EndNPCService();
	Show(false);
	GetGameUIMan()->GetDialog("Win_Inventory")->Show(false);
	GetHostPlayer()->GetPack(IVTRTYPE_PACK)->UnfreezeAllItems();
}

void CDlgELFRefineTrans::OnEventLButtonDown_Source(WPARAM wParam, LPARAM lParam, AUIObject * pObj)
{
 	CECIvtrItem *pELF = (CECIvtrItem *)m_pImg_Source->GetDataPtr();
 	if( pELF )
 		pELF->Freeze(false);
 	m_pImg_Source->ClearCover();
 	m_pImg_Source->SetHint(_AL(""));
 	m_pImg_Source->SetDataPtr(NULL);
 	m_pTxt_Source->SetText(_AL(""));
 	m_nSlotSource = -1;
	RefreshHostDetails();
}

void CDlgELFRefineTrans::OnEventLButtonDown_Target(WPARAM wParam, LPARAM lParam, AUIObject * pObj)
{
	CECIvtrItem *pELF = (CECIvtrItem *)m_pImg_Target->GetDataPtr();
	if( pELF )
		pELF->Freeze(false);
	m_pImg_Target->ClearCover();
	m_pImg_Target->SetHint(_AL(""));
	m_pImg_Target->SetDataPtr(NULL);
	m_pTxt_Target->SetText(_AL(""));
 	m_nSlotTarget = -1;
	RefreshHostDetails();
}

void CDlgELFRefineTrans::OnItemDragDrop(CECIvtrItem* pIvtrSrc, int iSrc, PAUIOBJECT pObjSrc, PAUIOBJECT pObjOver)
{	
	if( pIvtrSrc->GetClassID() == CECIvtrItem::ICID_GOBLIN )
	{
		enum { REFINE_TRANS_INVALID, REFINE_TRANS_SOURCE, REFINE_TRANS_TARGET, };
		static int s_LastDest = REFINE_TRANS_TARGET;
		int dest = REFINE_TRANS_INVALID;
		
		if( pObjOver )
		{
			// for drag-drop scene
			if( 0 == stricmp(pObjOver->GetName(), "Img_Source") )		dest = REFINE_TRANS_SOURCE;
			else if( 0 == stricmp(pObjOver->GetName(), "Img_Target") )  dest = REFINE_TRANS_TARGET;
		}
		else
		{
			// for right-click scene
			if(REFINE_TRANS_SOURCE == s_LastDest)
			{
				if(m_pImg_Source->GetDataPtr() == pIvtrSrc) OnEventLButtonDown_Source(0, 0, NULL);
				dest = REFINE_TRANS_TARGET;
			}
			else if(REFINE_TRANS_TARGET == s_LastDest)
			{
				if(m_pImg_Target->GetDataPtr() == pIvtrSrc) OnEventLButtonDown_Target(0, 0, NULL);
				dest = REFINE_TRANS_SOURCE;
			}
		}
		
		switch(dest)
		{
		case REFINE_TRANS_SOURCE:	this->SetSource(pIvtrSrc, iSrc); break;
		case REFINE_TRANS_TARGET:	this->SetTarget(pIvtrSrc, iSrc); break;
		default: break;
		}
		
		s_LastDest = dest;
	}
}