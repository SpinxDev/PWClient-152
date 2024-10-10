// Filename	: DlgELFGeniusReset.cpp
// Creator	: Chen Zhixin
// Date		: October 17, 2008

#include "WindowsX.h"
#include "AFI.h"
#include "AUICTranslate.h"
#include "AUIFrame.h"
#include "AUICommon.h"
#include "AUIManager.h"
#include "DlgELFGeniusReset.h"
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

AUI_BEGIN_COMMAND_MAP(CDlgELFGeniusReset, CDlgBase)

AUI_ON_COMMAND("IDCANCEL",		OnCommand_CANCEL)
AUI_ON_COMMAND("confirm",		OnCommand_Confirm)

AUI_END_COMMAND_MAP()

AUI_BEGIN_EVENT_MAP(CDlgELFGeniusReset, CDlgBase)

AUI_ON_EVENT("Img_ELF",		WM_LBUTTONDOWN,		OnEventLButtonDown_ELF)

AUI_END_EVENT_MAP()

//------------------------------------------------------------------------
// Right-Click Shortcut for CDlgELFGeniusReset
//------------------------------------------------------------------------
typedef CECShortcutMgr::SimpleClickShortcut<CDlgELFGeniusReset> ELFGeniusResetClickShortcut;
//------------------------------------------------------------------------

CDlgELFGeniusReset::CDlgELFGeniusReset()
{
	m_pImg_ELF = NULL;
	m_pBtn_Confirm = NULL;
	m_pTxt_Item = NULL;
	m_pTxt_Item2 = NULL;
	m_nSolt = -1;
}

CDlgELFGeniusReset::~CDlgELFGeniusReset()
{
}

bool CDlgELFGeniusReset::OnInitDialog()
{
	GetGameUIMan()->GetShortcutMgr()->RegisterShortCut(new ELFGeniusResetClickShortcut(this));

	DDX_Control("Img_ELF", m_pImg_ELF);
	DDX_Control("Btn_Confirm", m_pBtn_Confirm);
	DDX_Control("Txt_A", m_pTxt_Item);
	DDX_Control("Txt_B", m_pTxt_Item2);	

	return true;
}

void CDlgELFGeniusReset::OnShowDialog()
{
	OnEventLButtonDown_ELF(0, 0, NULL);
	if (0 == GetHostPlayer()->GetPack()->GetItemTotalNum(23553) + GetHostPlayer()->GetPack()->GetItemTotalNum(24338))
	{
		m_pTxt_Item->Show(true);
		m_pTxt_Item2->Show(true);
	}
	else
	{
		m_pTxt_Item->Show(false);
		m_pTxt_Item2->Show(false);
	}
	m_nLastSolt = -1;
}


void CDlgELFGeniusReset::RefreshHostDetails()
{
 	if( !IsShow() ) return;

	if( m_pImg_ELF->GetDataPtr() ) 
	{
		CECIvtrGoblin *pELF = (CECIvtrGoblin*)GetHostPlayer()->GetPack()->GetItem(m_nSolt);
		if( pELF )
		{
			AUICTranslate trans;
			const ACHAR * szDesc = pELF->GetDesc();
			if( szDesc )
				m_pImg_ELF->SetHint(trans.Translate(szDesc));
			else
				m_pImg_ELF->SetHint(_AL(""));

		}
	}
}

void CDlgELFGeniusReset::SetELF(CECIvtrItem *pItem1, int nSlot)
{
	CECIvtrGoblin* pELF = (CECIvtrGoblin*)pItem1;
	OnEventLButtonDown_ELF(0, 0, NULL);
	m_pImg_ELF->SetDataPtr(pELF);
	m_nSolt = nSlot;
	pItem1->Freeze(true);
	AString strFile;
	af_GetFileTitle(pELF->GetIconFile(), strFile);
	strFile.MakeLower();
	m_pImg_ELF->SetCover(GetGameUIMan()->m_pA2DSpriteIcons[CECGameUIMan::ICONS_INVENTORY],
		GetGameUIMan()->m_IconMap[CECGameUIMan::ICONS_INVENTORY][strFile]);
	AUICTranslate trans;
	const ACHAR * szDesc = pELF->GetDesc();
	if( szDesc )
		m_pImg_ELF->SetHint(trans.Translate(szDesc));
	else
		m_pImg_ELF->SetHint(_AL(""));
}

void CDlgELFGeniusReset::OnCommand_Confirm(const char *szCommand)
{
	CECIvtrGoblin* pELF = (CECIvtrGoblin*)m_pImg_ELF->GetDataPtr();
	if (!pELF)
	{
		GetGameUIMan()->MessageBox("", GetStringFromTable(7170), MB_OK, A3DCOLORRGBA(255, 255, 255, 160));
		return;
	}
	if (0 == GetHostPlayer()->GetPack()->GetItemTotalNum(23553) + GetHostPlayer()->GetPack()->GetItemTotalNum(24338))
	{
		ACString str;
		CECIvtrItem *pItem = CECIvtrItem::CreateItem(23553, 0, 1);
		str.Format(GetStringFromTable(7171), pItem->GetName());
		delete pItem;
		GetGameUIMan()->MessageBox("", str, MB_OK, A3DCOLORRGBA(255, 255, 255, 160));
		return;
	}
	if( m_nSolt >= 0 ) 
	{
		int index = GetHostPlayer()->GetPack()->FindItem(23553);
		if (index == -1)
		{
			index = GetHostPlayer()->GetPack()->FindItem(24338);
		}
		GetGameSession()->c2s_CmdNPCSevGoblinReturnGeniusPt(m_nSolt, index);
		m_nLastSolt = m_nSolt;
		OnEventLButtonDown_ELF(0, 0, NULL);
//		m_pBtn_Confirm->Enable(false);
	}
}

void CDlgELFGeniusReset::OnCommand_CANCEL(const char * szCommand)
{
	OnEventLButtonDown_ELF(0, 0, NULL);
	GetGameUIMan()->EndNPCService();
	Show(false);
	GetGameUIMan()->GetDialog("Win_Inventory")->Show(false);
	GetHostPlayer()->GetPack(IVTRTYPE_PACK)->UnfreezeAllItems();
}

void CDlgELFGeniusReset::OnEventLButtonDown_ELF(WPARAM wParam, LPARAM lParam, AUIObject * pObj)
{
 	CECIvtrItem *pELF = (CECIvtrItem *)m_pImg_ELF->GetDataPtr();
 	if( pELF )
 		pELF->Freeze(false);
 	m_pImg_ELF->ClearCover();
 	m_pImg_ELF->SetHint(GetStringFromTable(7308));
 	m_pImg_ELF->SetDataPtr(NULL);
 	m_nSolt = -1;
// 	m_pTxt_Item->Show(true);
// 	m_pTxt_Item2->Show(true);
}

void CDlgELFGeniusReset::OnItemDragDrop(CECIvtrItem* pIvtrSrc, int iSrc, PAUIOBJECT pObjSrc, PAUIOBJECT pObjOver)
{
	// no need to check name "Img_ELF", only one available drag-drop target
	if( pIvtrSrc->GetClassID() == CECIvtrItem::ICID_GOBLIN )
		this->SetELF(pIvtrSrc, iSrc);
}