#include "DlgFlySwordImprove.h"

#include "EC_ShortcutMgr.h"
#include "EC_GameSession.h"
#include "EC_GameUIMan.h"
#include "EC_HostPlayer.h"
#include "EC_Inventory.h"
#include "EC_IvtrItem.h"
#include "EC_IvtrFlySword.h"
#include "ExpTypes.h"
#include "AFI.h"
#include "DlgStoneReplace.h"
#include "DlgStoneTransfer.h"
#include "DlgInventory.h"
#include "AUICTranslate.h"
#include "EC_Game.h"
#include "EC_Global.h"
#include "elementdataman.h"

#define new A_DEBUG_NEW

const int IMPROVE_FLYSWORD_TICKET_ID1 = 42839;
const int IMPROVE_FLYSWORD_TICKET_ID2 = 42840;
const int IMPROVE_FLYSWORD_TICKET_ID3 = 42838;

AUI_BEGIN_COMMAND_MAP(CDlgFlySwordImprove, CDlgBase)
AUI_ON_COMMAND("IDCANCEL",		OnCommandCANCEL)
AUI_ON_COMMAND("Btn_Cancel",	OnCommandCANCEL)
AUI_ON_COMMAND("Btn_Improve",	OnCommandImprove)
AUI_END_COMMAND_MAP()


AUI_BEGIN_EVENT_MAP(CDlgFlySwordImprove, CDlgBase)
AUI_ON_EVENT("Img_AircraftIcon", WM_LBUTTONDOWN,	OnEventLButtonDown)
AUI_ON_EVENT("Img_AircraftIcon", WM_LBUTTONDBLCLK,  OnEventLButtonDown)
AUI_END_EVENT_MAP()

//------------------------------------------------------------------------
// Right-Click Shortcut for CDlgFlySwordImprove
//------------------------------------------------------------------------

typedef CECShortcutMgr::SimpleClickShortcut<CDlgFlySwordImprove> InstallFlySwordClickShortcut;

CDlgFlySwordImprove::CDlgFlySwordImprove()
{
	m_pImg_FlySwordIcon = NULL;
	m_pLbl_FlySwordName = NULL;
	m_pImg_ConsumeItemIcon = NULL;
	m_pLbl_ConsuemItemName = NULL;
	m_pLbl_ConsuemItemAmount = NULL;
	m_pBtn_Improve = NULL;
	m_pLbl_GeneralSpeed = NULL;
	m_pLbl_AcceleratedSpeed = NULL;
	m_pLbl_FlySwordImproveLevel = NULL;
}

CDlgFlySwordImprove::~CDlgFlySwordImprove()
{
}

bool CDlgFlySwordImprove::OnInitDialog()
{
	GetGameUIMan()->GetShortcutMgr()->RegisterShortCut(new InstallFlySwordClickShortcut(this));

	DDX_Control("Img_AircraftIcon",m_pImg_FlySwordIcon);
	DDX_Control("Txt_AircraftName",m_pLbl_FlySwordName);
	DDX_Control("Img_ItemIcon",m_pImg_ConsumeItemIcon);
	DDX_Control("Txt_ItemName",m_pLbl_ConsuemItemName);
	DDX_Control("Txt_ItemNum",m_pLbl_ConsuemItemAmount);
	DDX_Control("Btn_Improve",m_pBtn_Improve);

	DDX_Control("Txt_GeneralSpeed",m_pLbl_GeneralSpeed);
	DDX_Control("Txt_QuicklySpeed",m_pLbl_AcceleratedSpeed);

	DDX_Control("Txt_Level",m_pLbl_FlySwordImproveLevel);


	return CDlgBase::OnInitDialog();
}
void CDlgFlySwordImprove::OnShowDialog()
{
	ClearFlySword(m_pImg_FlySwordIcon);

	AlignTo(GetGameUIMan()->m_pDlgInventory,
		AUIDIALOG_ALIGN_OUTSIDE, AUIDIALOG_ALIGN_LEFT,
		AUIDIALOG_ALIGN_INSIDE, AUIDIALOG_ALIGN_TOP,
				-60 , 60 );
}
void CDlgFlySwordImprove::OnHideDialog()
{
	GetGameUIMan()->EndNPCService();
}
void CDlgFlySwordImprove::OnCommandCANCEL(const char * szCommand)
{
	Show(false);
	GetGameUIMan()->EndNPCService();
	GetGameUIMan()->GetDialog("Win_Inventory")->Show(false);
	GetHostPlayer()->GetPack(IVTRTYPE_PACK)->UnfreezeAllItems();
}

void CDlgFlySwordImprove::OnCommandImprove(const char * szCommand)
{
	GetGameUIMan()->MessageBox("FlySword_Improve",GetGameUIMan()->GetStringFromTable(11190),MB_OKCANCEL,A3DCOLORRGB(255,255,255));
}
void CDlgFlySwordImprove::DoImprove()
{
	unsigned int idx = m_pImg_FlySwordIcon->GetData();

	void *ptr = NULL;
	AString szType;
	m_pImg_FlySwordIcon->ForceGetDataPtr(ptr,szType);
	CECIvtrItem* pItem = (CECIvtrItem*)ptr;
	
	int tid = pItem ? pItem->GetTemplateID():0;

	GetGameSession()->c2s_CmdNPCSevFlySwordImprove(idx,tid);
}
int CDlgFlySwordImprove::GetObtainedItemCountForImprove()
{
	int c1 = GetHostPlayer()->GetPack()->GetItemTotalNum(IMPROVE_FLYSWORD_TICKET_ID1);
	int c2 = GetHostPlayer()->GetPack()->GetItemTotalNum(IMPROVE_FLYSWORD_TICKET_ID2);
	int c3 = GetHostPlayer()->GetPack()->GetItemTotalNum(IMPROVE_FLYSWORD_TICKET_ID3);

	return c1 + c2 + c3;
}

void CDlgFlySwordImprove::OnEventLButtonDown(WPARAM wParam, LPARAM lParam, AUIObject * pObj)
{
	if( !pObj || !pObj->GetDataPtr("ptr_CECIvtrItem") ) return;
	PAUIIMAGEPICTURE pImage = (PAUIIMAGEPICTURE)pObj;

	ClearFlySword(pImage);
}
bool CDlgFlySwordImprove::CanImprove(CECIvtrItem* pItem)
{
	if(!pItem)	return false;

	if(pItem->GetClassID() != CECIvtrItem::ICID_FLYSWORD) return false;

	CECIvtrFlySword* pFlySword = (CECIvtrFlySword*)pItem;	

	// 强化到顶级了
	if (pFlySword->GetMaxImproveLevel() > 0 && pFlySword->GetEssence().improve_level == pFlySword->GetMaxImproveLevel())
	{
		GetGameUIMan()->MessageBox("",GetGameUIMan()->GetStringFromTable(11192),MB_OK,A3DCOLORRGB(255,255,255));
		return false;
	}
	// 不可强化
	if (!pFlySword->CanBeImproved())
	{
		GetGameUIMan()->MessageBox("",GetGameUIMan()->GetStringFromTable(11191),MB_OK,A3DCOLORRGB(255,255,255));
		return false;
	}
	return true;
}
void CDlgFlySwordImprove::OnItemDragDrop(CECIvtrItem* pIvtrSrc, int iSrc, PAUIOBJECT pObjSrc, PAUIOBJECT pObjOver)
{
	// for right-click scene
	if(!pIvtrSrc || !pIvtrSrc->IsEquipment())
		return;

	if(!pObjOver)	
		pObjOver = this->GetDlgItem("Img_AircraftIcon");	
	
	// for drag-drop scene
	if(!pObjOver || !strstr(pObjOver->GetName(), "Img_AircraftIcon"))	
		return;	
	
	if(!CanImprove(pIvtrSrc))
		return;

	OnEventLButtonDown(0, 0, pObjOver);
	
	pIvtrSrc->Freeze(true);
	pObjOver->SetData(iSrc);
	void *ptr;
	AString szType;
	pObjSrc->ForceGetDataPtr(ptr,szType);
	pObjOver->SetDataPtr(ptr,szType);
	GetGameUIMan()->PlayItemSound(pIvtrSrc, true);
	
	// fly sword info
	AString strFile;
	af_GetFileTitle(pIvtrSrc->GetIconFile(), strFile);
	strFile.MakeLower();
	PAUIIMAGEPICTURE pImage = (PAUIIMAGEPICTURE)pObjOver;
	pImage->SetCover( GetGameUIMan()->GetIconCover(CECGameUIMan::ICONS_INVENTORY),
		GetGameUIMan()->GetIconIndex(CECGameUIMan::ICONS_INVENTORY, strFile) );
	
	pImage->SetColor(
		(pIvtrSrc->IsEquipment() && ((CECIvtrEquip *)pIvtrSrc)->IsDestroying())
		? A3DCOLORRGB(128, 128, 128)
		: A3DCOLORRGB(255, 255, 255));

	m_pLbl_FlySwordName->SetText(pIvtrSrc->GetName());
	
	// consume item info
	CECIvtrItem* consume = CECIvtrItem::CreateItem(IMPROVE_FLYSWORD_TICKET_ID3,0,1);
	if (consume)
	{
		af_GetFileTitle(consume->GetIconFile(), strFile);
		strFile.MakeLower();
		m_pImg_ConsumeItemIcon->SetCover( GetGameUIMan()->GetIconCover(CECGameUIMan::ICONS_INVENTORY),
					  GetGameUIMan()->GetIconIndex(CECGameUIMan::ICONS_INVENTORY, strFile) );

		m_pLbl_ConsuemItemName->SetText(consume->GetName());
		delete consume;
	}
}
void CDlgFlySwordImprove::SetFlySwordDynInfo(CECIvtrFlySword* pFlySword)
{
	// 速度信息
	ACString str;
	float speed1 = pFlySword->GetDBEssence()->improve_config[pFlySword->GetEssence().improve_level].speed_increase + pFlySword->GetEssence().speed_increase;
	str.Format(_AL("%+.2f"),speed1);
	m_pLbl_GeneralSpeed->SetText(str);
	float speed2 = pFlySword->GetDBEssence()->improve_config[pFlySword->GetEssence().improve_level].speed_rush_increase + pFlySword->GetEssence().speed_increase2;
	str.Format(_AL("%+.2f"),speed2);
	m_pLbl_AcceleratedSpeed->SetText(str);
	
	// 等级
	str.Format(_AL("%d"),pFlySword->GetEssence().improve_level);
	m_pLbl_FlySwordImproveLevel->SetText(str);

	//飞剑描述
	AUICTranslate trans;
	m_pImg_FlySwordIcon->SetHint(trans.Translate(pFlySword->GetDesc()));
}
void CDlgFlySwordImprove::ClearFlySword(PAUIIMAGEPICTURE pImage)
{
	CECIvtrItem *pItem = (CECIvtrItem *)pImage->GetDataPtr("ptr_CECIvtrItem");
	if (pItem)
	{
		pItem->Freeze(false);
		pImage->SetDataPtr(NULL);
		GetGameUIMan()->PlayItemSound(pItem, false);
	}
	
	pImage->SetCover(NULL, -1);	
	pImage->SetHint(_AL(""));
	m_pLbl_FlySwordName->SetText(_AL(""));	

	m_pImg_ConsumeItemIcon->ClearCover();
	m_pLbl_ConsuemItemName->SetText(_AL(""));
	m_pLbl_ConsuemItemAmount->SetText(_AL("0/0"));
	m_pLbl_ConsuemItemAmount->SetColor(A3DCOLORRGB(0,255,0));

	m_pLbl_GeneralSpeed->SetText(_AL("+0"));
	m_pLbl_AcceleratedSpeed->SetText(_AL("+0"));

	m_pLbl_FlySwordImproveLevel->SetText(_AL("0"));
}
void CDlgFlySwordImprove::OnTick()
{
	int c = GetObtainedItemCountForImprove();
	CECIvtrItem *pItem = (CECIvtrItem *)m_pImg_FlySwordIcon->GetDataPtr("ptr_CECIvtrItem");
	bool bCanImprove = false;
	bool bEnough = false;
	if (pItem && pItem->GetClassID() == CECIvtrItem::ICID_FLYSWORD)
	{
		CECIvtrFlySword* pFlySword = (CECIvtrFlySword*)pItem;
	
		// 消耗物品
		int needItemCount = pFlySword->GetDBEssence()->improve_config[pFlySword->GetEssence().improve_level].require_item_num;
		ACString str;
		str.Format(_AL("%d/%d"),c,needItemCount);
		m_pLbl_ConsuemItemAmount->SetText(str);		

		bEnough = c>=needItemCount;
		m_pLbl_ConsuemItemAmount->SetColor(bEnough ? A3DCOLORRGB(0,255,0):A3DCOLORRGB(255,0,0));	

		// 飞剑信息
		bCanImprove = pFlySword->CanBeImproved();
		if (bCanImprove)
			SetFlySwordDynInfo(pFlySword);
		else
			ClearFlySword(m_pImg_FlySwordIcon);
	}

	m_pBtn_Improve->Enable(bEnough && bCanImprove);
}