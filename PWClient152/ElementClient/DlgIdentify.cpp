// Filename	: DlgIdentify.cpp
// Creator	: Tom Zhou
// Date		: October 11, 2005

#include "DlgIdentify.h"
#include "EC_ShortcutMgr.h"
#include "EC_GameUIMan.h"
#include "EC_GameSession.h"
#include "EC_IvtrEquip.h"
#include "EC_HostPlayer.h"
#include "EC_Inventory.h"
#include "AFI.h"
#include "EC_Global.h"
#include "elementdataman.h"

#define new A_DEBUG_NEW

AUI_BEGIN_COMMAND_MAP(CDlgIdentify, CDlgBase)

AUI_ON_COMMAND("start",			OnCommand_start)
AUI_ON_COMMAND("IDCANCEL",		OnCommand_CANCEL)

AUI_END_COMMAND_MAP()

AUI_BEGIN_EVENT_MAP(CDlgIdentify, CDlgBase)

AUI_ON_EVENT("Item",		WM_LBUTTONDOWN,		OnEventLButtonDown)

AUI_END_EVENT_MAP()

//------------------------------------------------------------------------
// Right-Click Shortcut for CDlgIdentify
//------------------------------------------------------------------------
typedef CECShortcutMgr::SimpleClickShortcut<CDlgIdentify> IdentifyClickShortcut;
//------------------------------------------------------------------------

CDlgIdentify::CDlgIdentify()
{
}

CDlgIdentify::~CDlgIdentify()
{
}

bool CDlgIdentify::OnInitDialog()
{
	m_pImgItem = (PAUIIMAGEPICTURE)GetDlgItem("Item");
	m_pBtn_Confirm = (PAUISTILLIMAGEBUTTON)GetDlgItem("Btn_Confirm");

	GetGameUIMan()->GetShortcutMgr()->RegisterShortCut(new IdentifyClickShortcut(this));
	return true;
}

void CDlgIdentify::OnCommand_start(const char * szCommand)
{
	int iSrc = int(m_pImgItem->GetData());
	CECIvtrItem *pIvtr = (CECIvtrItem *)m_pImgItem->GetDataPtr("ptr_CECIvtrItem");
	if( !pIvtr ) return;
	int idItem = pIvtr->GetTemplateID();
	
	if( pIvtr->IsEquipment() && ((CECIvtrEquip *)pIvtr)->HasRandomProp() )
	{
		GetGameSession()->c2s_CmdNPCSevIdentify(iSrc, idItem);
		m_pBtn_Confirm->Enable(false);
	}
	else
	{
		m_pAUIManager->MessageBox("", m_pAUIManager->GetStringFromTable(504),
			MB_OK, A3DCOLORRGBA(255, 255, 255, 160));
	}
}

void CDlgIdentify::OnCommand_CANCEL(const char * szCommand)
{
	Show(false);
	GetGameUIMan()->EndNPCService();
	GetGameUIMan()->GetDialog("Win_Inventory")->Show(false);
	GetHostPlayer()->GetPack(IVTRTYPE_PACK)->UnfreezeAllItems();
}

void CDlgIdentify::OnEventLButtonDown(WPARAM wParam, LPARAM lParam, AUIObject *pObj)
{
	CECIvtrItem *pIvtr = (CECIvtrItem *)m_pImgItem->GetDataPtr("ptr_CECIvtrItem");
	if( !pIvtr ) return;

	m_pImgItem->ClearCover();
	m_pImgItem->SetData(0);
	m_pImgItem->SetDataPtr(NULL);

	this->GetDlgItem("Txt_Gold")->SetText(_AL("0"));
	this->GetDlgItem("Txt")->SetText(_AL(""));

	GetHostPlayer()->GetPack(IVTRTYPE_PACK)->UnfreezeAllItems();
	GetGameUIMan()->PlayItemSound(pIvtr, true);

	m_pBtn_Confirm->Enable(false);
}

void CDlgIdentify::OnItemDragDrop(CECIvtrItem* pIvtrSrc, int iSrc, PAUIOBJECT pObjSrc, PAUIOBJECT pObjOver)
{
	// default drag-drop to this object
	pObjOver = GetDlgItem("Item");

	if(!pObjOver || !GetGameUIMan()->m_pCurNPCEssence)
	{
		return;
	}
				
	pObjOver->SetData(iSrc);
	void *ptr;
	AString szType;
	pObjSrc->ForceGetDataPtr(ptr,szType);
	pObjOver->SetDataPtr(ptr,szType);
	GetGameUIMan()->PlayItemSound(pIvtrSrc, true);
	
	AString strFile;
	af_GetFileTitle(pIvtrSrc->GetIconFile(), strFile);
	strFile.MakeLower();
	PAUIIMAGEPICTURE pImage = (PAUIIMAGEPICTURE)pObjOver;
	pImage->SetCover(GetGameUIMan()->m_pA2DSpriteIcons[CECGameUIMan::ICONS_INVENTORY],
		GetGameUIMan()->m_IconMap[CECGameUIMan::ICONS_INVENTORY][strFile]);
	
	pImage->SetColor(
		(pIvtrSrc->IsEquipment() && ((CECIvtrEquip *)pIvtrSrc)->IsDestroying())
		? A3DCOLORRGB(128, 128, 128)
		: A3DCOLORRGB(255, 255, 255));
	
	this->GetDlgItem("Txt")->SetText(pIvtrSrc->GetName());
	
	GetHostPlayer()->GetPack(IVTRTYPE_PACK)->UnfreezeAllItems();
	pIvtrSrc->Freeze(true);
	
	ACHAR szPrice[40];
	DATA_TYPE DataType;
	int idSevice = GetGameUIMan()->m_pCurNPCEssence->id_identify_service;
	elementdataman *pDataMan = g_pGame->GetElementDataMan();
	NPC_IDENTIFY_SERVICE *pService = (NPC_IDENTIFY_SERVICE *)
		pDataMan->get_data_ptr(idSevice, ID_SPACE_ESSENCE, DataType);
	
	a_sprintf(szPrice, _AL("%d"), pService->fee);
	this->GetDlgItem("Txt_Gold")->SetText(szPrice);
	this->GetDlgItem("Btn_Confirm")->Enable(true);
}