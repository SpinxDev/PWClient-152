// File		: DlgPetDye.cpp
// Creator	: Xu Wenbin
// Date		: 2010/3/8

#include "AFI.h"
#include "AUICTranslate.h"
#include "DlgPetDye.h"
#include "EC_ShortcutMgr.h"
#include "DlgPetList.h"
#include "DlgPetFittingRoom.h"
#include "EC_Game.h"
#include "EC_GameUIMan.h"
#include "EC_GameSession.h"
#include "EC_HostPlayer.h"
#include "EC_IvtrItem.h"
#include "EC_Inventory.h"
#include "EC_IvtrConsume.h"
#include "EC_PetCorral.h"
#include "elementdataman.h"

#define new A_DEBUG_NEW

AUI_BEGIN_COMMAND_MAP(CDlgPetDye, CDlgBase)

AUI_ON_COMMAND("confirm",		OnCommandConfirm)
AUI_ON_COMMAND("IDCANCEL",		OnCommandCancel)

AUI_END_COMMAND_MAP()

AUI_BEGIN_EVENT_MAP(CDlgPetDye, CDlgBase)

AUI_ON_EVENT("Img_Item*",	WM_LBUTTONDOWN,	OnEventLButtonDown_Item)
AUI_ON_EVENT("Img_Dye",		WM_LBUTTONDOWN,	OnEventLButtonDown_Dye)

AUI_END_EVENT_MAP()

//------------------------------------------------------------------------
// Right-Click Shortcut for CDlgPetDye
//------------------------------------------------------------------------
typedef CECShortcutMgr::SimpleClickShortcut<CDlgPetDye> PetDyeClickShortcut;
//------------------------------------------------------------------------

CDlgPetDye::CDlgPetDye()
{
	m_pTxt_Num = NULL;
	m_pTxt_Name = NULL;
	m_pTxt_Dye = NULL;
	m_pTxt_Color = NULL;
	m_pImg_Item = NULL;
	m_pImg_Dye = NULL;
	m_pBtn_Confirm = NULL;
	m_nSlotItem = -1;
	m_nSlotDye = -1;
	m_nRequireNum = 0;
}

CDlgPetDye::~CDlgPetDye()
{
}

bool CDlgPetDye::OnInitDialog()
{
	DDX_Control("Txt_Name", m_pTxt_Name);
	DDX_Control("Txt_Dye", m_pTxt_Dye);
	DDX_Control("Txt_Color", m_pTxt_Color);
	DDX_Control("Img_Item", m_pImg_Item);
	DDX_Control("Txt_Num", m_pTxt_Num);
	DDX_Control("Img_Dye", m_pImg_Dye);
	DDX_Control("Btn_Confirm", m_pBtn_Confirm);
	
	GetGameUIMan()->GetShortcutMgr()->RegisterShortCut(new PetDyeClickShortcut(this));

	return true;
}

void CDlgPetDye::OnCommandConfirm(const char *szCommand)
{
	if (m_nSlotItem < 0 || m_nSlotDye < 0)
		return;

	CECPetCorral * pPetCorral = GetHostPlayer()->GetPetCorral();
	CECPetData * pPet = pPetCorral->GetPetData(m_nSlotItem);
	CECIvtrDyeTicket* pDye = (CECIvtrDyeTicket*)m_pImg_Dye->GetDataPtr();
	if (!pPet || !pDye)
		return;

	if (m_nRequireNum > pDye->GetCount())
	{
		GetGameUIMan()->MessageBox("", GetStringFromTable(9320), MB_OK, A3DCOLORRGB(255, 255, 255));
		return;
	}

	GetGameSession()->c2s_CmdNPCSevPetDye(m_nSlotItem, pPet->GetTemplateID(), m_nSlotDye, pDye->GetTemplateID());

	//	染色成功，宠物数据被修改，因此
	OnCommandCancel(NULL);
}

void CDlgPetDye::OnCommandCancel(const char *szCommand)
{
	ClearDialog();
	GetGameUIMan()->EndNPCService();
	Show(false);
	GetGameUIMan()->GetDialog("Win_Inventory")->Show(false);
	GetGameUIMan()->m_pDlgPetList->Show(false);
	GetGameUIMan()->m_pDlgPetFittingRoom->OnCommand("IDCANCEL");
	GetHostPlayer()->GetPack(IVTRTYPE_PACK)->UnfreezeAllItems();
}

void CDlgPetDye::OnShowDialog()
{
	ClearDialog();
}

void CDlgPetDye::ClearItem()
{
	m_pImg_Item->ClearCover();
	m_pImg_Item->SetHint(_AL(""));
	m_pImg_Item->SetDataPtr(NULL);

	m_pTxt_Name->SetText(_AL(""));

	m_pTxt_Color->Show(false);

	m_nSlotItem = -1;

	UpdateRequireNum();

	UpdateFittingRoom();
}

void CDlgPetDye::OnEventLButtonDown_Item(WPARAM wParam, LPARAM lParam, AUIObject * pObj)
{
	ClearItem();
}

void CDlgPetDye::ClearDye()
{
	CECIvtrItem *pItem = (CECIvtrItem *)m_pImg_Dye->GetDataPtr();
	if( pItem )
		pItem->Freeze(false);

	m_pImg_Dye->ClearCover();
	m_pImg_Dye->SetHint(_AL(""));
	m_pImg_Dye->SetDataPtr(NULL);

	m_pTxt_Dye->SetText(_AL(""));

	m_nSlotDye = -1;
	
	UpdateNumDisplay();

	UpdateFittingRoom();
}

void CDlgPetDye::OnEventLButtonDown_Dye(WPARAM wParam, LPARAM lParam, AUIObject * pObj)
{
	ClearDye();
}

void CDlgPetDye::SetPet(int nSlot)
{
	//	设置要染色的宠物

	CECPetCorral * pPetCorral = GetHostPlayer()->GetPetCorral();
	if (nSlot < 0 || nSlot >= pPetCorral->GetPetSlotNum())
	{
		//	输入非法
		return;
	}
	CECPetData * pPet = pPetCorral->GetPetData(nSlot);
	if (!pPet)
	{
		//	宠物栏为空，拖拽过程中发生错误，直接离开
		return;
	}

	if (!pPet->IsMountPet())
	{
		//	只有骑宠才能染色
		GetGameUIMan()->AddChatMessage(GetStringFromTable(9321), GP_CHAT_MISC);
		return;
	}

	if(pPetCorral->GetActivePetIndex() == nSlot)
	{
		GetGameUIMan()->AddChatMessage(GetStringFromTable(9324), GP_CHAT_MISC);
		return;
	}

	int requireCount(0);
	if (!GetRequireDyeCount(pPet, requireCount) || requireCount <= 0)
	{
		//	染色剂数目为0时，约定不允许染色
		GetGameUIMan()->AddChatMessage(GetStringFromTable(9322), GP_CHAT_MISC);
		return;
	}

	//	查询生成宠物数据
	elementdataman* pDB = GetGame()->GetElementDataMan();
	DATA_TYPE DataType;
	PET_ESSENCE *pDBEssence = (PET_ESSENCE*)pDB->get_data_ptr(pPet->GetTemplateID(), ID_SPACE_ESSENCE, DataType);
	if (!pDBEssence || DataType != DT_PET_ESSENCE)
		return;
	PET_EGG_ESSENCE *pEggDBEssence = (PET_EGG_ESSENCE*)pDB->get_data_ptr(pPet->GetEggID(), ID_SPACE_ESSENCE, DataType);
	if (!pEggDBEssence || DataType != DT_PET_EGG_ESSENCE)
		return;

	//	清除原宠物
	ClearItem();

	//	设置新宠物、更新界面
	AString strFile;
	af_GetFileTitle(pDBEssence->file_icon, strFile);
	strFile.MakeLower();
	m_pImg_Item->SetCover(GetGameUIMan()->m_pA2DSpriteIcons[CECGameUIMan::ICONS_INVENTORY], GetGameUIMan()->m_IconMap[CECGameUIMan::ICONS_INVENTORY][strFile]);

	m_pTxt_Name->SetText(pPet->GetName());	
	
	m_pTxt_Color->Show(true);
	A3DCOLOR clr;
	if (!CECPlayer::RIDINGPET::GetColor(pPet->GetColor(), clr))
		clr = CECPlayer::RIDINGPET::GetDefaultColor();
	m_pTxt_Color->SetColor(clr);

	m_nSlotItem = nSlot;
	UpdateRequireNum();
	UpdateFittingRoom();
}

void CDlgPetDye::UpdateFittingRoom()
{
	//	更新宠物预览
	CDlgPetFittingRoom *pDlgFittingRoom = GetGameUIMan()->m_pDlgPetFittingRoom;
	CECPetCorral * pPetCorral = GetHostPlayer()->GetPetCorral();
	CECPetData * pPet = (m_nSlotItem >= 0) ? pPetCorral->GetPetData(m_nSlotItem) : NULL;
	int idPet = pPet ? pPet->GetTemplateID() : 0;
	if (idPet > 0)
	{
		//	默认使用宠物当前颜色
		A3DCOLOR clr;
		if (!CECPlayer::RIDINGPET::GetColor(pPet->GetColor(), clr))
			clr = CECPlayer::RIDINGPET::GetDefaultColor();

		//	有染色剂时以染色剂颜色显示
		CECIvtrDyeTicket* pDye = (CECIvtrDyeTicket *)m_pImg_Dye->GetDataPtr();
		if (pDye) clr = pDye->GenerateColor();

		//	更新宠物及颜色
		pDlgFittingRoom->SetPet(idPet, clr);

		//	显示染色界面
		if (!pDlgFittingRoom->IsShow())
			pDlgFittingRoom->Show(true);
	}
	else
	{
		if (pDlgFittingRoom->IsShow())
			pDlgFittingRoom->SetPet(0);
	}

	if (pDlgFittingRoom->IsShow() && pDlgFittingRoom->IsPetLoaded())
		GetGameUIMan()->BringWindowToTop(pDlgFittingRoom);
}

void CDlgPetDye::SetDye(CECIvtrItem *pItem, int nSlot)
{
	if (!pItem || nSlot < 0)
		return;

	CECIvtrDyeTicket* pDye = (CECIvtrDyeTicket*)pItem;
	if (pDye->GetDBEssence()->usage != 0)
		return;

	ClearDye();

	m_nSlotDye = nSlot;
	pItem->Freeze(true);

	m_pImg_Dye->SetDataPtr(pDye);

	AString strFile;
	af_GetFileTitle(pDye->GetIconFile(), strFile);
	strFile.MakeLower();
	m_pImg_Dye->SetCover(GetGameUIMan()->m_pA2DSpriteIcons[CECGameUIMan::ICONS_INVENTORY], GetGameUIMan()->m_IconMap[CECGameUIMan::ICONS_INVENTORY][strFile]);
	AUICTranslate trans;
	const ACHAR * szDesc = pDye->GetDesc();
	if (szDesc)
		m_pImg_Dye->SetHint(trans.Translate(szDesc));
	
	m_pTxt_Dye->SetText(pItem->GetName());

	UpdateNumDisplay();

	//	设置预览
	UpdateFittingRoom();
}

void CDlgPetDye::OnTick()
{
	//	更新染色剂数目（包裹中可能有变化）
	UpdateNumDisplay();
	
	//	更新染色按钮
	m_pBtn_Confirm->Enable(m_nSlotItem >= 0 && GetDyeCount() > 0);
}

void CDlgPetDye::ClearDialog()
{
	ClearItem();
	ClearDye();
}

bool CDlgPetDye::GetRequireDyeCount(const CECPetData *pPet, int &count)
{
	if (!pPet)
		return false;

	elementdataman *pDBMan = GetGame()->GetElementDataMan();
	DATA_TYPE DataType;
	PET_ESSENCE *pDBEssence = (PET_ESSENCE*)pDBMan->get_data_ptr(pPet->GetTemplateID(), ID_SPACE_ESSENCE, DataType);
	if (!pDBEssence || DataType != DT_PET_ESSENCE)
		return false;

	count = pDBEssence->require_dye_count;
	return true;
}

int CDlgPetDye::GetDyeCount()
{
	int nDyeCount(0);
	if (m_nSlotDye >= 0)
	{
		CECIvtrItem *pDye = GetHostPlayer()->GetPack()->GetItem(m_nSlotDye);
		if (pDye) nDyeCount = pDye->GetCount();
	}
	return nDyeCount;
}

void CDlgPetDye::UpdateRequireNum()
{
	m_nRequireNum = 0;

	if (m_nSlotItem >= 0)
	{
		CECPetCorral * pPetCorral = GetHostPlayer()->GetPetCorral();
		CECPetData * pPet = pPetCorral->GetPetData(m_nSlotItem);
		int nRequire(0);
		if (pPet && GetRequireDyeCount(pPet, nRequire))
			m_nRequireNum = nRequire;
	}

	UpdateNumDisplay();
}

void CDlgPetDye::UpdateNumDisplay()
{
	int nCount = GetDyeCount();
	m_pTxt_Num->SetColor(nCount >= m_nRequireNum ? A3DCOLORRGB(255, 255, 255) : A3DCOLORRGB(255, 0, 0));
	
	ACHAR szText[20];
	a_sprintf(szText, _AL("%d/%d"), m_nRequireNum, nCount);
	m_pTxt_Num->SetText(szText);
}

void CDlgPetDye::OnItemDragDrop(CECIvtrItem* pIvtrSrc, int iSrc, PAUIOBJECT pObjSrc, PAUIOBJECT pObjOver)
{
	// no need to check name "Img_Dye", only one available drag-drop target
	if( pIvtrSrc->GetClassID() == CECIvtrItem::ICID_DYETICKET )
		this->SetDye(pIvtrSrc, iSrc);
}