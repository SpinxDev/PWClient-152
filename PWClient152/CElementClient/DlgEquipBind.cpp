// File		: DlgEquipBind.cpp
// Creator	: Xiao Zhou
// Date		: 2006/7/19

#include "AFI.h"
#include "DlgEquipBind.h"
#include "EC_ShortcutMgr.h"
#include "EC_GameUIMan.h"
#include "EC_GameSession.h"
#include "EC_HostPlayer.h"
#include "EC_IvtrItem.h"
#include "EC_Inventory.h"
#include "EC_IvtrEquip.h"
#include "ExpTypes.h"
#include "AUILabel.h"

#define new A_DEBUG_NEW

AUI_BEGIN_COMMAND_MAP(CDlgEquipBind, CDlgBase)

AUI_ON_COMMAND("confirm",		OnCommandConfirm)
AUI_ON_COMMAND("IDCANCEL",		OnCommandCancel)

AUI_END_COMMAND_MAP()

AUI_BEGIN_EVENT_MAP(CDlgEquipBind, CDlgBase)

AUI_ON_EVENT("Img_Item",	WM_LBUTTONDOWN,	OnEventLButtonDown)

AUI_END_EVENT_MAP()

//------------------------------------------------------------------------
// Right-Click Shortcut for CDlgEquipBind
//------------------------------------------------------------------------
typedef CECShortcutMgr::SimpleClickShortcut<CDlgEquipBind> EquipBindClickShortcut;
//------------------------------------------------------------------------

CDlgEquipBind::CDlgEquipBind()
{
	m_pTxt_Name = NULL;
	m_pImg_Item = NULL;
	m_pBtn_Confirm = NULL;
	m_dwBindType = 0;
}

CDlgEquipBind::~CDlgEquipBind()
{
}

bool CDlgEquipBind::OnInitDialog()
{
	DDX_Control("Txt_Name", m_pTxt_Name);
	DDX_Control("Img_Item", m_pImg_Item);
	DDX_Control("Btn_Confirm", m_pBtn_Confirm);

	GetGameUIMan()->GetShortcutMgr()->RegisterShortCut(new EquipBindClickShortcut(this));
	return true;
}

void CDlgEquipBind::OnCommandConfirm(const char *szCommand)
{
	CECIvtrItem* pItem = (CECIvtrItem*)GetDataPtr();
	if (!pItem) return;

	if (!szCommand || stricmp(szCommand, "MessageBox_Confirm"))
	{
		//	第一次调用，根据情形进行确认
		if (pItem->GetProcType() & CECIvtrItem::PROC_CAN_WEBTRADE)
		{
			ACString strText = GetGameUIMan()->GetStringFromTable(904);
			GetGameUIMan()->MessageBox("Game_WebTradeBindConfirm", strText, MB_YESNO, A3DCOLORRGBA(255, 255, 255, 160));
			return;
		}
	}

	int nItemNeed(0);	
	NPC_EQUIPBIND_SERVICE* pService = (NPC_EQUIPBIND_SERVICE*)GetData("ptr_NPC_EQUIPBIND_SERVICE");
	if (pService)
	{
		CECInventory *pPack = GetHostPlayer()->GetPack();
		for (int i(0); i < sizeof(pService->id_object_need)/sizeof(pService->id_object_need[0]); ++ i)
		{
			int id  = pService->id_object_need[i];
			if (id != 0 && pPack->FindItem(id) >= 0)
			{
				nItemNeed = id;
				break;
			}
		}
	}

	GetGameSession()->c2s_SendCmdNPCSevBindItem(m_nSolt, pItem->GetTemplateID(), nItemNeed);
	OnEventLButtonDown(0, 0, NULL);
}

void CDlgEquipBind::OnCommandCancel(const char *szCommand)
{
	OnEventLButtonDown(0, 0, NULL);
	GetGameUIMan()->EndNPCService();
	Show(false);
	GetGameUIMan()->GetDialog("Win_Inventory")->Show(false);
	GetHostPlayer()->GetPack(IVTRTYPE_PACK)->UnfreezeAllItems();
}

void CDlgEquipBind::OnShowDialog()
{
	// show correct info
	NPC_EQUIPBIND_SERVICE* pService = (NPC_EQUIPBIND_SERVICE*)GetData("ptr_NPC_EQUIPBIND_SERVICE");
	m_dwBindType = pService ? pService->bind_type : 0;

	// only support two bind type
	ASSERT(m_dwBindType < 2);

	PAUILABEL pTxtTitle = NULL;
	DDX_Control("Txt_Title", pTxtTitle);
	if(pTxtTitle) pTxtTitle->SetText(GetStringFromTable(m_dwBindType == 0 ? 900:902));

	PAUILABEL pTxtInfo = NULL;
	DDX_Control("Txt_Info", pTxtInfo);
	if(pTxtInfo) pTxtInfo->SetText(GetStringFromTable(m_dwBindType == 0 ? 899:901));

	OnEventLButtonDown(0, 0, NULL);
}

void CDlgEquipBind::OnEventLButtonDown(WPARAM wParam, LPARAM lParam, AUIObject * pObj)
{
	CECIvtrItem *pItem = (CECIvtrItem *)GetDataPtr();
	if( pItem )
		pItem->Freeze(false);
	m_pImg_Item->ClearCover();
	m_pImg_Item->SetHint(_AL(""));
	m_pTxt_Name->SetText(_AL(""));
	SetDataPtr(NULL);
}

void CDlgEquipBind::SetEquip(CECIvtrItem *pItem, int nSlot)
{
	OnEventLButtonDown(0, 0, NULL);
	SetDataPtr(pItem);
	m_nSolt = nSlot;
	pItem->Freeze(true);
	AString strFile;
	af_GetFileTitle(pItem->GetIconFile(), strFile);
	strFile.MakeLower();
	m_pImg_Item->SetCover(GetGameUIMan()->m_pA2DSpriteIcons[CECGameUIMan::ICONS_INVENTORY],
		GetGameUIMan()->m_IconMap[CECGameUIMan::ICONS_INVENTORY][strFile]);

	m_pImg_Item->SetColor(
		(pItem->IsEquipment() && ((CECIvtrEquip *)pItem)->IsDestroying())
		? A3DCOLORRGB(128, 128, 128)
		: A3DCOLORRGB(255, 255, 255));

	m_pTxt_Name->SetText(pItem->GetName());
}

void CDlgEquipBind::OnItemDragDrop(CECIvtrItem* pIvtrSrc, int iSrc, PAUIOBJECT pObjSrc, PAUIOBJECT pObjOver)
{
	// no need to check name "Img_Item", only one available drag-drop target
	if (pIvtrSrc->GetClassID() == CECIvtrItem::ICID_WEAPON ||
		pIvtrSrc->GetClassID() == CECIvtrItem::ICID_ARMOR ||
		pIvtrSrc->GetClassID() == CECIvtrItem::ICID_DECORATION ||
		pIvtrSrc->GetClassID() == CECIvtrItem::ICID_FASHION ||
		pIvtrSrc->GetClassID() == CECIvtrItem::ICID_FLYSWORD ||
		pIvtrSrc->GetClassID() == CECIvtrItem::ICID_BIBLE ||
		pIvtrSrc->GetClassID() == CECIvtrItem::ICID_PETEGG)
	{
		int iProcType = pIvtrSrc->GetProcType();

		if (iProcType & CECIvtrItem::PROC_BIND)
			return;	//	装备后绑定 NO

		if ((iProcType & CECIvtrItem::PROC_BINDING) && !(iProcType & CECIvtrItem::PROC_CAN_WEBTRADE))
			return;	//	天人合一 NO

		if ((iProcType & CECIvtrItem::PROC_BINDING) && (iProcType & CECIvtrItem::PROC_CAN_WEBTRADE) &&
			m_dwBindType != 0)
			return;	//	寻宝网天人合一 YES

		if (!(iProcType & CECIvtrItem::PROC_CAN_WEBTRADE))
		{
			//	对非寻宝网天人合一的装备，不符合要求的加以提示
			if (m_dwBindType == 1 && !pIvtrSrc->IsTradeable())
			{
				GetGameUIMan()->AddChatMessage(GetStringFromTable(903), GP_CHAT_MISC);
				return;
			}

			if ((iProcType & CECIvtrItem::PROC_DROPWHENDIE) &&
				(iProcType & CECIvtrItem::PROC_DROPPABLE) &&
				(iProcType & CECIvtrItem::PROC_SELLABLE) &&
				(iProcType & CECIvtrItem::PROC_TRADEABLE))
			{
				GetGameUIMan()->MessageBox("", GetStringFromTable(833), MB_OK, A3DCOLORRGBA(255, 255, 255, 160));
				return;
			}
		}

		this->SetEquip(pIvtrSrc, iSrc);
	}
}