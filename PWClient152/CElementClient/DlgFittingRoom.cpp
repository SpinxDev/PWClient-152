// Filename	: DlgFittingRoom.cpp
// Creator	: Xiao Zhou
// Date		: 2007/4/10

#include "AFI.h"
#include "DlgFittingRoom.h"
#include "DlgPalette.h"
#include "EC_Game.h"
#include "EC_GameRun.h"
#include "EC_GameUIMan.h"
#include "EC_HostPlayer.h"
#include "EC_LoginPlayer.h"
#include "EC_World.h"
#include "EC_Model.h"
#include "EC_IvtrItem.h"
#include "EC_IvtrFashion.h"
#include "EC_Inventory.h"
#include "roleinfo"
#include "EC_ManPlayer.h"
#include "EC_ShortcutMgr.h"
#include "EC_Global.h"
#include "ExpTypes.h"
#include "EC_FashionModel.h"

#include "AUIDef.h"

#define new A_DEBUG_NEW

AUI_BEGIN_COMMAND_MAP(CDlgFittingRoom, CDlgBase)

AUI_ON_COMMAND("IDCANCEL",			OnCommand_CANCEL)
AUI_ON_COMMAND("Btn_ResetPlayer",	OnCommand_ResetPlayer)
AUI_ON_COMMAND("Btn_Palette",		OnCommand_Palette)

AUI_END_COMMAND_MAP()

AUI_BEGIN_EVENT_MAP(CDlgFittingRoom, CDlgBase)

AUI_ON_EVENT("Equip_*",		WM_LBUTTONDOWN,		OnEventLButtonDown)
AUI_ON_EVENT("Equip_*",		WM_RBUTTONDOWN,		OnEventRButtonDown)
AUI_ON_EVENT("Img_Char",		WM_LBUTTONDOWN,		OnEventLButtonDown_Char)
AUI_ON_EVENT("Img_Char",		WM_LBUTTONUP,		OnEventLButtonUp_Char)
AUI_ON_EVENT("Img_Char",		WM_RBUTTONDOWN,		OnEventRButtonDown_Char)
AUI_ON_EVENT("Img_Char",		WM_RBUTTONUP,		OnEventRButtonUp_Char)
AUI_ON_EVENT("Img_Char",		WM_MOUSEWHEEL,		OnEventMouseWheel_Char)
AUI_ON_EVENT("Img_Char",		WM_MOUSEMOVE,		OnEventMouseMove_Char)

AUI_END_EVENT_MAP()

//------------------------------------------------------------------------
// Right-Click Shortcut for CDlgFittingRoom
//------------------------------------------------------------------------
class FittingRoomClickShortcut : public CECShortcutMgr::ClickShortcut
{
public:
	FittingRoomClickShortcut(const char* pName, CDlgFittingRoom* pDlg)
		:m_pDlg(pDlg)
		,m_GroupName(pName)
	{}

	virtual const char* GetGroupName() { return m_GroupName; }
	
	virtual bool CanTrigger(PAUIOBJECT pSrcObj) 
	{
		if(!m_pDlg)
		{
			return false;
		}

		PAUIMANAGER pUIMan = m_pDlg->GetAUIManager();
		if( pUIMan->GetDialog("Win_EquipDye")->IsShow() ||
			pUIMan->GetDialog("Win_AllEquipDye")->IsShow() )
		{
			// skip this trigger
			return false;
		}

		return // the target dialog is shown
				m_pDlg->IsShow() &&  
				// contains a item pointer
				pSrcObj && pSrcObj->GetDataPtr("ptr_CECIvtrItem");
	}
	
	virtual void Trigger(PAUIOBJECT pSrcObj, int num) 
	{ 
		CECIvtrItem* pItem = (CECIvtrItem*)pSrcObj->GetDataPtr("ptr_CECIvtrItem");
		PAUIOBJECT pObj = m_pDlg->GetEquipIcon(pItem);
		m_pDlg->SetEquipIcon(pItem, pObj);
	}

protected:
	CDlgFittingRoom* m_pDlg;
	AString m_GroupName;
};
//------------------------------------------------------------------------

CDlgFittingRoom::CDlgFittingRoom()
{
	m_pFashionModel = NULL;
}

CDlgFittingRoom::~CDlgFittingRoom()
{

}

bool CDlgFittingRoom::OnInitDialog()
{
	if (!CDlgBase::OnInitDialog()) {
		return false;
	}

	CECShortcutMgr* pShortcut = GetGameUIMan()->GetShortcutMgr();
	pShortcut->RegisterShortCut(new FittingRoomClickShortcut("Win_Inventory", this));
	pShortcut->RegisterShortCut(new FittingRoomClickShortcut("Win_Inventory_Equip", this));
	pShortcut->RegisterShortCut(new FittingRoomClickShortcut("Win_EPEquip", this));
	pShortcut->RegisterShortCut(new FittingRoomClickShortcut("Win_PShop2", this));

	int i;
	char szItem[40];

	m_pImg_Char = (PAUIIMAGEPICTURE)GetDlgItem("Img_Char");
	m_pImg_Char->SetAcceptMouseMessage(true);
	for( i = 0; i < SIZE_EQUIPIVTR; i++ )
	{
		sprintf(szItem, "Equip_%02d", i);
		m_pImgEquip[i] = (PAUIIMAGEPICTURE)GetDlgItem(szItem);
	}
	return true;
}

bool CDlgFittingRoom::Release() {
	if (m_pFashionModel != NULL) {
		delete m_pFashionModel;
		m_pFashionModel = NULL;

		delete m_pMoveParamFunction;
		m_pMoveParamFunction = NULL;
	}
	return CDlgBase::Release();
}

void CDlgFittingRoom::OnShowDialog()
{
	if (!m_pFashionModel) {
		CreatePlayer();
	}
	ResetPlayer();
}

void CDlgFittingRoom::OnTick()
{
	UpdatePlayer();
}

void CDlgFittingRoom::OnCommand_CANCEL(const char * szCommand)
{
	Show(false);
}

bool CDlgFittingRoom::SetEquipIcon(CECIvtrItem* pItem, PAUIOBJECT pObj)
{
	if( !pObj || !strstr(pObj->GetName(), "Equip_") )
	{
		return false;
	}

	int iDst = atoi(pObj->GetName() + strlen("Equip_"));
	if( pItem->GetClassID() != CECIvtrItem::ICID_FASHION ||
		!pItem->CanEquippedTo(iDst) || 
		!pItem->IsEquipment() ||
		GetHostPlayer()->GetGender() != ((CECIvtrFashion*)pItem)->GetGenderRequirement() ||
		GetHostPlayer()->GetMaxLevelSofar() < ((CECIvtrFashion*)pItem)->GetLevelRequirement())
	{
		return false;
	}
	const FASHION_ESSENCE* pEssence = ((CECIvtrFashion*)pItem)->GetDBEssence();
	if (pEssence && pEssence->equip_location == enumSkinShowHand)
	{
		if (!(pEssence->character_combo_id & (1 << GetHostPlayer()->GetProfession())))
			return false;
	}

	//	隐藏调色板、再显示时更新时装及其颜色
	GetGameUIMan()->m_pDlgPalette->Show(false);

	GetGameUIMan()->SetCover((PAUIIMAGEPICTURE)pObj, pItem->GetIconFile());

	m_pFashionModel->Fit(pItem->GetTemplateID());
	m_pFashionModel->ChangeFashionColor(iDst, ((CECIvtrFashion *)pItem)->GetEssence().color);

	return true;
}

void CDlgFittingRoom::SetEquipColor(PAUIOBJECT pObj, A3DCOLOR clr)
{
	while (true)
	{
		if (!pObj || !strstr(pObj->GetName(), "Equip_"))
			break;

		int iDst = atoi(pObj->GetName() + strlen("Equip_"));
		unsigned short color = (unsigned short)FASHION_A3DCOLOR_TO_WORDCOLOR(clr);
		m_pFashionModel->ChangeFashionColor(iDst, color);

		break;
	}
}

bool CDlgFittingRoom::GetEquipColor(PAUIOBJECT pObj, A3DCOLOR &clr)
{
	if (!pObj || !strstr(pObj->GetName(), "Equip_"))
		return false;
	
	int iDst = atoi(pObj->GetName() + strlen("Equip_"));
	unsigned short color;
	bool ret =  m_pFashionModel->GetFashionColor(iDst, color);
	if (ret) {
		clr = FASHION_WORDCOLOR_TO_A3DCOLOR(color);
	}
	return ret;
}

void CDlgFittingRoom::OnEventRButtonDown(WPARAM wParam, LPARAM lParam, AUIObject *pObj)
{
	GetGameUIMan()->m_pDlgPalette->Show(false);

	PAUIIMAGEPICTURE pImage = (PAUIIMAGEPICTURE)pObj;
	pImage->ClearCover();

	int iSrc = atoi(pObj->GetName() + strlen("Equip_"));

	m_pFashionModel->ClearFashion(iSrc);
}

void CDlgFittingRoom::OnEventLButtonDown(WPARAM wParam, LPARAM lParam, AUIObject *pObj)
{
	CDlgPalette * pDlgPalette = GetGameUIMan()->m_pDlgPalette;
	pDlgPalette->Show(false);
}

PAUIOBJECT CDlgFittingRoom::GetEquipIcon(CECIvtrItem* pItem)
{
	int iSlotStart = -1;	
	while(pItem && ++iSlotStart < SIZE_EQUIPIVTR)
	{
		AString strName;
		PAUIOBJECT pObj =  GetDlgItem(strName.Format("Equip_%02d", iSlotStart));
		
		if(pObj && pObj->IsShow() && pItem->CanEquippedTo(iSlotStart))
		{
			// find a empty one
			return pObj;
		}
	}

	return NULL;
}

void CDlgFittingRoom::OnItemDragDrop(CECIvtrItem* pIvtrSrc, int iSrc, PAUIOBJECT pObjSrc, PAUIOBJECT pObjOver)
{
	this->SetEquipIcon(pIvtrSrc, pObjOver);
}

void CDlgFittingRoom::OnCommand_Palette(const char * szCommand){
	GetGameUIMan()->m_pDlgPalette->Show(!GetGameUIMan()->m_pDlgPalette->IsShow());
}

void CDlgFittingRoom::OnCommand_ResetPlayer(const char * szCommand){
	ResetPlayer();
}

void CDlgFittingRoom::OnEventLButtonDown_Char(WPARAM wParam, LPARAM lParam, AUIObject *pObj)
{
	A3DVIEWPORTPARAM *p = m_pA3DEngine->GetActiveViewport()->GetParam();
	int x = GET_X_LPARAM(lParam) - p->X;
	int y = GET_Y_LPARAM(lParam) - p->Y;
	if (!m_pFashionModel->OnEventLButtonDown(x, y)){
		return;
	}
	SetCaptureObject(m_pImg_Char);
}

void CDlgFittingRoom::OnEventLButtonUp_Char(WPARAM wParam, LPARAM lParam, AUIObject *pObj)
{
	A3DVIEWPORTPARAM *p = m_pA3DEngine->GetActiveViewport()->GetParam();
	int x = GET_X_LPARAM(lParam) - p->X;
	int y = GET_Y_LPARAM(lParam) - p->Y;

	if (m_pFashionModel->OnEventLButtonUp(x, y)){
		SetCaptureObject(NULL);
	}

	ChangeFocus(NULL);
}

void CDlgFittingRoom::OnEventRButtonDown_Char(WPARAM wParam, LPARAM lParam, AUIObject *pObj)
{
	A3DVIEWPORTPARAM *p = m_pA3DEngine->GetActiveViewport()->GetParam();
	int x = GET_X_LPARAM(lParam) - p->X;
	int y = GET_Y_LPARAM(lParam) - p->Y;

	if (!m_pFashionModel->OnEventRButtonDown(x, y)){
		return;
	}
	SetCaptureObject(m_pImg_Char);
}

void CDlgFittingRoom::OnEventRButtonUp_Char(WPARAM wParam, LPARAM lParam, AUIObject *pObj)
{
	A3DVIEWPORTPARAM *p = m_pA3DEngine->GetActiveViewport()->GetParam();
	int x = GET_X_LPARAM(lParam) - p->X;
	int y = GET_Y_LPARAM(lParam) - p->Y;

	if (m_pFashionModel->OnEventRButtonUp(x, y)){
		SetCaptureObject(NULL);
	}
	ChangeFocus(NULL);
}

void CDlgFittingRoom::OnEventMouseMove_Char(WPARAM wParam, LPARAM lParam, AUIObject *pObj)
{
	A3DVIEWPORTPARAM *p = m_pA3DEngine->GetActiveViewport()->GetParam();
	int x = GET_X_LPARAM(lParam) - p->X;
	int y = GET_Y_LPARAM(lParam) - p->Y;

	if (!m_pFashionModel->OnEventMouseMove(x, y)){
		return;
	}
}

void CDlgFittingRoom::OnEventMouseWheel_Char(WPARAM wParam, LPARAM lParam, AUIObject *pObj)
{
	int zDelta = (short)HIWORD(wParam);
	m_pFashionModel->OnEventMouseWheel(zDelta);
}

void CDlgFittingRoom::ResetPlayer() {

	m_pFashionModel->ResetCamera();
	
	int i;
	CECHostPlayer* pHost = GetHostPlayer();
	CECInventory *pPack = pHost->GetEquipment();
	for( i = 0; i < SIZE_EQUIPIVTR; i++ ) {
		if( m_pImgEquip[i] )
		{
			OnEventRButtonDown(0, 0, m_pImgEquip[i]);
			CECIvtrItem *pItem = pPack->GetItem(i);
			if( pItem && pItem->IsEquipment() && ((CECIvtrEquip*)pItem)->IsFashion() )
				SetEquipIcon(pItem, m_pImgEquip[i]);
		}
	}	
}

void CDlgFittingRoom::UpdatePlayer() {
	m_pFashionModel->Tick();
	m_pImg_Char->SetRenderCallback(
		PlayerRenderDemonstration,
		(DWORD)m_pFashionModel->GetPlayer(),
		m_pFashionModel->GetAngle(),
		m_pFashionModel->ComposeCameraParameter());
	 
}

void CDlgFittingRoom::CreatePlayer() {
	if (!m_pFashionModel) {
		m_pFashionModel = new CECFashionModel();
		CECHostPlayer *pHost = GetHostPlayer();
		int profession = pHost->GetProfession();
		int gender = pHost->GetGender();
		m_pFashionModel->CreatePlayer(profession, gender);
		m_pMoveParamFunction = new CECGeneralComputeMoveParamFunction(
			m_pImg_Char->GetSize().cx, m_pImg_Char->GetSize().cy, pHost->GetGender() == 0, m_pFashionModel);
		m_pFashionModel->SetMoveParamFunction(m_pMoveParamFunction);
	}
}