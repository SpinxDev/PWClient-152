// File		: DlgStoneChange.cpp
// Creator	: Wang And Shi
// Date		: 2013/01/14


#include "DlgStoneChange.h"
#include "AUIEditBox.h"
#include "AUIImagePicture.h"
#include "auilabel.h"
#include "EC_ShortcutMgr.h"
#include "EC_GameSession.h"
#include "EC_GameUIMan.h"
#include "EC_HostPlayer.h"
#include "EC_Inventory.h"
#include "EC_IvtrItem.h"
#include "EC_IvtrEquip.h"
#include "EC_IvtrStone.h"
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

static const char* STONE_CHANGEEND_GFX = "界面\\宝箱发光.gfx";

AUI_BEGIN_COMMAND_MAP(CDlgStoneChange, CDlgBase)

AUI_ON_COMMAND("IDCANCEL",		OnCommandCANCEL)
AUI_ON_COMMAND("Btn_Transfer*",	OnCommandTransfer)
AUI_ON_COMMAND("Btn_Replace*",	OnCommandReplace)

AUI_END_COMMAND_MAP()


AUI_BEGIN_EVENT_MAP(CDlgStoneChange, CDlgBase)

AUI_ON_EVENT("Img_Item", WM_LBUTTONDOWN,	OnEventLButtonDown)
AUI_ON_EVENT("Img_Item", WM_LBUTTONDBLCLK,  OnEventLButtonDown)


AUI_END_EVENT_MAP()

//------------------------------------------------------------------------
// Right-Click Shortcut for CDlgStoneChange
//------------------------------------------------------------------------
//typedef CECShortcutMgr::SimpleClickShortcut<CDlgStoneChange> InstallClickShortcut;
//------------------------------------------------------------------------

class InstallClickShortcut : public CECShortcutMgr::SimpleClickShortcut<CDlgStoneChange>
{
	typedef CECShortcutMgr::SimpleClickShortcut<CDlgStoneChange> BASECLASS;
public:
	InstallClickShortcut(CDlgStoneChange* pDlg):BASECLASS(pDlg){}
	bool CanTrigger(AUIObject* pSrcObj)
	{
		if (SimpleClickShortcutBase::CanTrigger(pSrcObj))
		{
			CECIvtrItem* pItem = (CECIvtrItem*)pSrcObj->GetDataPtr(GetItemName());
			if (pItem && pItem->IsEquipment())
				return true;
		}

		return false;
	}
};
CDlgStoneChange::CDlgStoneChange()
{
	m_pItem = NULL;

	memset(m_pTransferBtn,0,sizeof(m_pTransferBtn));
	memset(m_pReplaceBtn,0,sizeof(m_pReplaceBtn));
	memset(m_pStoneImg,0,sizeof(m_pStoneImg));
	memset(m_pStoneName,0,sizeof(m_pStoneName));
	memset(m_pStoneBackGround,0,sizeof(m_pStoneBackGround));

	m_iEquipSlot = -1;
	m_iSelHoleIndex = -1;
	m_pEquipName = NULL;

	m_gfxCounter.SetPeriod(2000);
}

CDlgStoneChange::~CDlgStoneChange()
{
}

bool CDlgStoneChange::OnInitDialog()
{
	m_nOriginalHeight = m_nHeight;
	GetGameUIMan()->GetShortcutMgr()->RegisterShortCut(new InstallClickShortcut(this));
	EnumStoneRecipe(); // 枚举宝石替换配方

	return CDlgBase::OnInitDialog();
}
void CDlgStoneChange::OnShowDialog()
{
	ClearEquip(m_pItem);

	AlignTo(GetGameUIMan()->m_pDlgInventory,
		AUIDIALOG_ALIGN_OUTSIDE, AUIDIALOG_ALIGN_LEFT,
		AUIDIALOG_ALIGN_INSIDE, AUIDIALOG_ALIGN_TOP,
				-60 , 60 );

	m_gfxCounter.Reset(true);
}
void CDlgStoneChange::OnHideDialog()
{
	if(GetGameUIMan()->m_pDlgStoneTransfer->IsShow())
		GetGameUIMan()->m_pDlgStoneTransfer->Show(false);
	if(GetGameUIMan()->m_pDlgStoneReplace->IsShow())
		GetGameUIMan()->m_pDlgStoneReplace->Show(false);

	ClearGfx();
}
void CDlgStoneChange::ClearGfx()
{
	for (int i=0;i<STONE_NUM;i++)
	{
		m_pStoneImg[i]->SetGfx("");
	}
}
void CDlgStoneChange::OnTick()
{
	if (m_gfxCounter.IncCounter(g_pGame->GetRealTickTime()))
		ClearGfx();
}
void CDlgStoneChange::DoDataExchange(bool bSave)
{
	CDlgBase::DoDataExchange(bSave);

	DDX_Control("Img_Item", m_pItem);
	DDX_Control("Lbl_EquipName",m_pEquipName);

	AString str;
	for (int i=0;i<STONE_NUM;i++)
	{		
		str.Format("Img_Item%d",i+1);
		DDX_Control(str,m_pStoneImg[i]);
		str.Format("Btn_Transfer%d",i+1);
		DDX_Control(str,m_pTransferBtn[i]);
		str.Format("Btn_Replace%d",i+1);
		DDX_Control(str,m_pReplaceBtn[i]);
		str.Format("Lbl_Name%d",i+1);
		DDX_Control(str,m_pStoneName[i]);
		str.Format("Img_Background%d",i+1);
		DDX_Control(str,m_pStoneBackGround[i]);
	}
}

void CDlgStoneChange::OnCommandCANCEL(const char * szCommand)
{
	Show(false);
	GetHostPlayer()->EndNPCService();
	GetGameUIMan()->m_pCurNPCEssence = NULL;
	GetGameUIMan()->GetDialog("Win_Inventory")->Show(false);
	GetHostPlayer()->GetPack(IVTRTYPE_PACK)->UnfreezeAllItems();
}

void CDlgStoneChange::OnCommandTransfer(const char * szCommand)
{
	int iLen = strlen("Btn_Transfer");
	int index = atoi(szCommand + iLen);
	ASSERT(index>=1 && index<=4);

	if (GetGameUIMan()->m_pDlgStoneTransfer->IsShow() && (index -1 == m_iSelHoleIndex)) return;

	m_iSelHoleIndex = index - 1;

	if (GetGameUIMan()->m_pDlgStoneTransfer->IsShow())
		GetGameUIMan()->m_pDlgStoneTransfer->Show(false);

	GetGameUIMan()->m_pDlgStoneTransfer->Show(true);

	if(GetGameUIMan()->m_pDlgStoneReplace->IsShow())
		GetGameUIMan()->m_pDlgStoneReplace->Show(false);
}
void CDlgStoneChange::OnCommandReplace(const char * szCommand)
{
	int iLen = strlen("Btn_Replace");
	int index = atoi(szCommand + iLen);
	ASSERT(index>=1 && index<=4);

	if (GetGameUIMan()->m_pDlgStoneReplace->IsShow() && (index -1 == m_iSelHoleIndex)) return;
	
	m_iSelHoleIndex = index - 1;

	if (GetGameUIMan()->m_pDlgStoneReplace->IsShow())
		GetGameUIMan()->m_pDlgStoneReplace->Show(false);
	
	GetGameUIMan()->m_pDlgStoneReplace->Show(true);

	if(GetGameUIMan()->m_pDlgStoneTransfer->IsShow())
		GetGameUIMan()->m_pDlgStoneTransfer->Show(false);
}

void CDlgStoneChange::OnEventLButtonDown(WPARAM wParam, LPARAM lParam, AUIObject * pObj)
{
	if( !pObj || !pObj->GetDataPtr("ptr_CECIvtrItem") ) return;
	PAUIIMAGEPICTURE pImage = (PAUIIMAGEPICTURE)pObj;

	ClearEquip(pImage);
}

void CDlgStoneChange::OnItemDragDrop(CECIvtrItem* pIvtrSrc, int iSrc, PAUIOBJECT pObjSrc, PAUIOBJECT pObjOver)
{
	// for right-click scene
	if(!pIvtrSrc || !pIvtrSrc->IsEquipment())
		return;

	if(!pObjOver)
	{
		pObjOver = this->GetDlgItem("Img_Item");
	}
	
	// for drag-drop scene
	if(!pObjOver || !strstr(pObjOver->GetName(), "Img_Item"))
	{
		return;
	}

	OnEventLButtonDown(0, 0, pObjOver);
	
	pIvtrSrc->Freeze(true);
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
	pImage->SetCover( GetGameUIMan()->GetIconCover(CECGameUIMan::ICONS_INVENTORY),
		GetGameUIMan()->GetIconIndex(CECGameUIMan::ICONS_INVENTORY, strFile) );
	
	pImage->SetColor(
		(pIvtrSrc->IsEquipment() && ((CECIvtrEquip *)pIvtrSrc)->IsDestroying())
		? A3DCOLORRGB(128, 128, 128)
		: A3DCOLORRGB(255, 255, 255));

	m_pEquipName->SetText(pIvtrSrc->GetName());
	AUICTranslate trans;
	m_pItem->SetHint(trans.Translate(pIvtrSrc->GetDesc()));

	// 石头信息
	SetEquipStone(dynamic_cast<CECIvtrEquip*>(pIvtrSrc));

	m_iEquipSlot = iSrc;
}

void CDlgStoneChange::ClearEquip(PAUIIMAGEPICTURE pImage)
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
	m_pEquipName->SetText(_AL(""));	

	for (int i=0;i<STONE_NUM;i++)
	{
		m_pStoneImg[i]->ClearCover();
		m_pStoneImg[i]->SetHint(_AL(""));
		m_pStoneImg[i]->SetData(0);
		m_pStoneName[i]->SetText(_AL(""));
	}

	m_iEquipSlot = -1;

	if(GetGameUIMan()->m_pDlgStoneTransfer->IsShow())
		GetGameUIMan()->m_pDlgStoneTransfer->Show(false);
	if(GetGameUIMan()->m_pDlgStoneReplace->IsShow())
		GetGameUIMan()->m_pDlgStoneReplace->Show(false);

 	//int y = m_pStoneImg[0]->GetPos(true).y - 3;
 	//SetSize(m_nWidth,y);
	ShowStoneControls(0,NULL);
}

void CDlgStoneChange::SetEquipStone(CECIvtrEquip* pIvtrEquip)
{
	if(!pIvtrEquip) return;

	bool bIsWeapon = pIvtrEquip->IsWeapon() ? true:false;
	
	int nStoneNum = pIvtrEquip->GetHoleNum() - pIvtrEquip->GetEmptyHoleNum();
	ShowStoneControls(nStoneNum,pIvtrEquip);
		
	for (int i=0;i<pIvtrEquip->GetHoleNum();i++)
	{
		int id = pIvtrEquip->GetHoleItem(i);
		
		CECIvtrItem* pTempItem = CECIvtrItem::CreateItem(id,0,1);
		CECIvtrStone* pStone = dynamic_cast<CECIvtrStone*>(pTempItem);
		if (pStone)
		{
			m_pStoneName[i]->SetText(pStone->GetName());
			
			AString strFile;
			af_GetFileTitle(pStone->GetIconFile(), strFile);
			strFile.MakeLower();
			m_pStoneImg[i]->SetCover( GetGameUIMan()->GetIconCover(CECGameUIMan::ICONS_INVENTORY),
				GetGameUIMan()->GetIconIndex(CECGameUIMan::ICONS_INVENTORY, strFile));
			
			if(bIsWeapon)	m_pStoneImg[i]->SetHint(pStone->GetDBEssence()->weapon_desc);
			else m_pStoneImg[i]->SetHint(pStone->GetDBEssence()->armor_desc);
			
			m_pStoneImg[i]->SetData(pStone->GetTemplateID());		
		}
		delete pTempItem;
	}
}
void CDlgStoneChange::ShowStoneControls(int stone_num,CECIvtrEquip* pEquip)
{
	for (int i=0;i<STONE_NUM;i++)
	{
		bool bShow = i<stone_num;
		m_pStoneImg[i]->Show(bShow);
		m_pStoneName[i]->Show(bShow);
		m_pTransferBtn[i]->Show(bShow);
		m_pReplaceBtn[i]->Show(bShow);
		m_pStoneBackGround[i]->Show(bShow);

		m_pTransferBtn[i]->Enable(false);
		m_pReplaceBtn[i]->Enable(false);
	}

 	{ 		
 		for (int i=0;i<STONE_NUM;i++)
 		{
			bool bEnable = i<stone_num;
 			if ((m_Type & EM_TRANSFER) != 0)
 			{
				bool bHasRecipe = pEquip && i< pEquip->GetHoleNum() && CanTransfer(pEquip->GetHoleItem(i));
				if (bEnable && bHasRecipe){
					m_pTransferBtn[i]->Enable(true);
					m_pTransferBtn[i]->SetHint(_AL(""));
				}else{
					m_pTransferBtn[i]->Enable(false);
					m_pTransferBtn[i]->SetHint(GetStringFromTable(11233));
				}
 			}
 			if ((m_Type & EM_REPLACE) != 0)
 			{
 				m_pReplaceBtn[i]->Enable(bEnable);				
 			}
 			
 			m_pStoneName[i]->SetText(GetGameUIMan()->GetStringFromTable(786));
 		}
 	}
	const char *szHelpCtrls[] = {
		"Lbl_1",
		"Lbl_2",
		"Lbl_3",
		"Lbl_4",
		"Lbl_5",
		"Lbl_6",
		"Lbl_7",
		"Lbl_8",
		"Img_Help01",
		"Img_Help02",
		"Img_bg1",
		"Img_bg2",
	};
	for (int j(0); j < ARRAY_SIZE(szHelpCtrls); ++ j)
	{
		PAUIOBJECT pObj = GetDlgItem(szHelpCtrls[j]);
		if (pObj){
			pObj->Show(stone_num<=0);
		}
	}
}
bool CDlgStoneChange::IsWeapon()
{
	CECIvtrEquip* pItem = dynamic_cast<CECIvtrEquip*>(GetHostPlayer()->GetPack()->GetItem(m_iEquipSlot));
	if (pItem)
	{
		return pItem->IsWeapon();
	}
	else
		return false;
}
int CDlgStoneChange::GetOldStoneTid()
{
	if(m_iSelHoleIndex<0|| m_iSelHoleIndex>= STONE_NUM) 
		return 0;
	return m_pStoneImg[m_iSelHoleIndex]->GetData();
}
void CDlgStoneChange::OnStoneChangeEnd(int type,int equip_idx,int hole_idx)
{
	CECIvtrEquip* pEquipItem = dynamic_cast<CECIvtrEquip*>(GetHostPlayer()->GetPack()->GetItem(equip_idx));
	if (!pEquipItem) return;

	AUICTranslate trans;
	m_pItem->SetHint(trans.Translate(pEquipItem->GetDesc()));

	SetEquipStone(pEquipItem);

	// gfx
	ASSERT(hole_idx>=0 && hole_idx<STONE_NUM);

	m_pStoneImg[hole_idx]->SetGfx(STONE_CHANGEEND_GFX);
	m_pStoneImg[hole_idx]->Resize();
	
	m_gfxCounter.Reset();

	if (type == 0) // transfer
	{
		GetGameUIMan()->m_pDlgStoneTransfer->Show(false);
	}
	else if (type == 1)
	{
		GetGameUIMan()->m_pDlgStoneReplace->Show(false);
	}
	else
		ASSERT(FALSE);
}
void CDlgStoneChange::EnumStoneRecipe()
{
	m_stone_recipes.clear();
		
	elementdataman *pDataMan = g_pGame->GetElementDataMan();
	int i;
	DATA_TYPE DataType;
	for( i = 0; i < (int)pDataMan->get_data_num(ID_SPACE_RECIPE); i++ )
	{
		unsigned int uiID = pDataMan->get_data_id(ID_SPACE_RECIPE, i, DataType);
		if( DataType != DT_STONE_CHANGE_RECIPE ) continue;
		
		STONE_CHANGE_RECIPE *pRecipe = (STONE_CHANGE_RECIPE *)pDataMan->get_data_ptr(uiID, ID_SPACE_RECIPE, DataType);

		if(m_stone_recipes.find(pRecipe->id_old_stone) != m_stone_recipes.end())
			m_stone_recipes[pRecipe->id_old_stone].push_back(pRecipe);
		else
		{
			RECIPES recipe;
			recipe.push_back(pRecipe);
			m_stone_recipes[pRecipe->id_old_stone]  = recipe;
		}
	}
}
bool CDlgStoneChange::CanTransfer(int stone_id)
{
	return m_stone_recipes.find(stone_id) != m_stone_recipes.end();
}
const CDlgStoneChange::RECIPES& CDlgStoneChange::GetRecipe(int stone_id)
{
	static RECIPES temp;

	if(m_stone_recipes.find(stone_id)!=m_stone_recipes.end())
		return m_stone_recipes[stone_id];
	else
		return temp;
}