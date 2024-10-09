// Filename	: DlgFaceLift.cpp
// Creator	: Tom Zhou
// Date		: October 30, 2005

#include "DlgFaceLift.h"
#include "EC_ShortcutMgr.h"
#include "EC_World.h"
#include "EC_GameUIMan.h"
#include "EC_Global.h"
#include "EC_GameRun.h"
#include "EC_GameSession.h"
#include "EC_ManNPC.h"
#include "EC_NPC.h"
#include "EC_IvtrItem.h"
#include "EC_HostPlayer.h"
#include "EC_Inventory.h"
#include "EC_IvtrScroll.h"
#include "AFI.h"

#define new A_DEBUG_NEW

AUI_BEGIN_COMMAND_MAP(CDlgFaceLift, CDlgBase)

AUI_ON_COMMAND("start",			OnCommand_start)
AUI_ON_COMMAND("IDCANCEL",		OnCommand_CANCEL)

AUI_END_COMMAND_MAP()

//------------------------------------------------------------------------
// Right-Click Shortcut for CDlgFaceLift
//------------------------------------------------------------------------
typedef CECShortcutMgr::SimpleClickShortcut<CDlgFaceLift> FaceLiftClickShortcut;
//------------------------------------------------------------------------

CDlgFaceLift::CDlgFaceLift()
{
}

CDlgFaceLift::~CDlgFaceLift()
{
}

bool CDlgFaceLift::OnInitDialog()
{
	m_pImgItem = (PAUIIMAGEPICTURE)GetDlgItem("Item_a");
	GetGameUIMan()->GetShortcutMgr()->RegisterShortCut(new FaceLiftClickShortcut(this));
	return true;
}

void CDlgFaceLift::OnCommand_start(const char * szCommand)
{
	int idTarget = GetHostPlayer()->GetSelectedTarget();
	CECNPC *pNPC = GetWorld()->GetNPCMan()->GetNPC(idTarget);
	if( !pNPC ) return;

	CECIvtrItem *pItem = (CECIvtrItem *)m_pImgItem->GetDataPtr("ptr_CECIvtrItem");
	if( !pItem ) return;

	if( GetHostPlayer()->IsRidingOnPet() || GetHostPlayer()->GetAttachMode() != CECPlayer::enumAttachNone )
	{
		GetGameUIMan()->MessageBox("", GetStringFromTable(688), MB_OK, A3DCOLORRGBA(255, 255, 255, 160));
		return;
	}
	
	GetHostPlayer()->GetPack(IVTRTYPE_PACK)->UnfreezeAllItems();
	GetGameSession()->c2s_CmdNPCSevFaceChange(
		m_pImgItem->GetData(), pItem->GetTemplateID());
	GetGameUIMan()->GetDialog("Win_Inventory")->Show(false);
	GetGameUIMan()->EndNPCService();
	m_pImgItem->ClearCover();
	Show(false);
}

void CDlgFaceLift::OnCommand_CANCEL(const char * szCommand)
{
	GetHostPlayer()->GetPack(IVTRTYPE_PACK)->UnfreezeAllItems();
	GetGameUIMan()->GetDialog("Win_Inventory")->Show(false);
	GetGameUIMan()->EndNPCService();
	m_pImgItem->ClearCover();
	Show(false);
}

void CDlgFaceLift::OnItemDragDrop(CECIvtrItem* pIvtrSrc, int iSrc, PAUIOBJECT pObjSrc, PAUIOBJECT pObjOver)
{
	// default drag-drop to this object
	pObjOver = GetDlgItem("Item_a");

	if( pObjOver && pIvtrSrc->GetClassID() == CECIvtrItem::ICID_FACETICKET )
	{
		if( GetHostPlayer()->GetMaxLevelSofar() >= ((CECIvtrFaceTicket *)pIvtrSrc)->GetLevelRequirement() )
		{
			pObjOver->SetData(iSrc);
			pObjOver->SetDataPtr(pIvtrSrc,"ptr_CECIvtrItem");
			GetGameUIMan()->PlayItemSound(pIvtrSrc, true);
			
			AString strFile;
			af_GetFileTitle(pIvtrSrc->GetIconFile(), strFile);
			strFile.MakeLower();
			PAUIIMAGEPICTURE pImage = (PAUIIMAGEPICTURE)pObjOver;
			pImage->SetCover(GetGameUIMan()->m_pA2DSpriteIcons[CECGameUIMan::ICONS_INVENTORY],
				GetGameUIMan()->m_IconMap[CECGameUIMan::ICONS_INVENTORY][strFile]);
			
			GetHostPlayer()->GetPack(IVTRTYPE_PACK)->UnfreezeAllItems();
			pIvtrSrc->Freeze(true);
		}
		else
			GetGameUIMan()->AddChatMessage(GetStringFromTable(600), GP_CHAT_MISC);
	}
}