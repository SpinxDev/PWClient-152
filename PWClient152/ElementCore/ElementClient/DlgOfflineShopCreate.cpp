/*
 * FILE: DlgOfflineShopCreate.cpp
 *
 * DESCRIPTION: 
 *
 * CREATED BY: WYD
 *
 * HISTORY: 
 *
 * Copyright (c) 2013, All Rights Reserved.
 */
 
#include "DlgOfflineShopCreate.h"
#include "EC_ShortcutMgr.h"
#include "EC_GameSession.h"
#include "EC_Inventory.h"
#include "EC_IvtrItem.h"
#include "EC_IvtrEquip.h"
#include "AUICTranslate.h"
#include "EC_Game.h"
#include "EC_Global.h"
#include "EC_GameUIMan.h"
#include "EC_HostPlayer.h"
#include <AFI.h>
#include "DlgInventory.h"
#include "DlgOfflineShopList.h"

AUI_BEGIN_COMMAND_MAP(CDlgOfflineShopCreate, CDlgBase)
AUI_ON_COMMAND("IDCANCEL",		OnCommandCANCEL)
AUI_ON_COMMAND("Btn_Confirm",	OnCommandConfirm)
AUI_ON_COMMAND("Btn_Cancel",	OnCommandBtn2)
AUI_ON_COMMAND("Btn_Close",		OnCommandCANCEL)
AUI_END_COMMAND_MAP()

AUI_BEGIN_EVENT_MAP(CDlgOfflineShopCreate, CDlgBase)
AUI_ON_EVENT("Img_Dye",		WM_LBUTTONDOWN,		OnEventLButtonDown)
AUI_END_EVENT_MAP()

typedef CECShortcutMgr::SimpleClickShortcut<CDlgOfflineShopCreate> InstallClickShortcut;

CDlgOfflineShopCreate::CDlgOfflineShopCreate():m_shop_type(0),
m_pImg_Item(NULL),
m_pBtn_Confirm(NULL),
m_pBtn_Cancel(NULL),
m_bDlgCreate(true),
m_pLbl_Title(NULL)
{}

CDlgOfflineShopCreate::~CDlgOfflineShopCreate(){}
	
bool CDlgOfflineShopCreate::OnInitDialog()
{
	GetGameUIMan()->GetShortcutMgr()->RegisterShortCut(new InstallClickShortcut(this));
	
	DDX_Control("Btn_Confirm",m_pBtn_Confirm);
	DDX_Control("Btn_Cancel",m_pBtn_Cancel);
	DDX_Control("Img_Dye", m_pImg_Item);
	DDX_Control("Lbl_Title",m_pLbl_Title);

	return CDlgBase::OnInitDialog();
}
void CDlgOfflineShopCreate::DoDataExchange(bool bSave)
{
	CDlgBase::DoDataExchange(bSave);
	DDX_RadioButton(bSave, 0, m_shop_type);
}
void CDlgOfflineShopCreate::OnCommandConfirm(const char *szCommand)
{
	UpdateData(true);

	void *ptr = NULL;
	AString szType;
	m_pImg_Item->ForceGetDataPtr(ptr,szType);
	CECIvtrItem* pItem = (CECIvtrItem*)ptr;
	
	int iItemID = pItem ? pItem->GetTemplateID():0;
	int iItemPos = pItem ? m_pImg_Item->GetData():-1;
	int iItmeNum = pItem ? pItem->GetCount():0;


	if (m_bDlgCreate)
	{		
		GetGameSession()->OffShop_CreateMyShop(m_shop_type,iItemID,iItemPos,iItmeNum);
	}
	else
	{
		GetGameSession()->OffShop_ActiveMyShop(iItemID,iItemPos,iItmeNum);
	}

//	Show(false);
}
void CDlgOfflineShopCreate::OnCommandCANCEL(const char *szCommand)
{	
	if (m_bDlgCreate)
		GetHostPlayer()->EndNPCService();
	
	OnEventLButtonDown(0,0,m_pImg_Item);
	Show(false);
}
void CDlgOfflineShopCreate::OnCommandBtn2(const char *szCommand)
{	
	if (m_bDlgCreate)
	{
		OnEventLButtonDown(0,0,m_pImg_Item);
		GetHostPlayer()->EndNPCService();
		Show(false);
	}
	else // 改类型
	{
		UpdateData(true);
		GetGameSession()->OffShop_SetMyShopType(m_shop_type);
	}		
}

void CDlgOfflineShopCreate::OnHideDialog()
{
}
void CDlgOfflineShopCreate::OnShowDialog()
{
	m_pImg_Item->SetDataPtr(NULL);	
	m_pImg_Item->SetCover(NULL, -1);
	m_pImg_Item->SetData(0);

	UpdateData(false);

	if (m_bDlgCreate)
	{
		m_pBtn_Confirm->SetText(GetGameUIMan()->GetStringFromTable(10509));  // 开启
		m_pBtn_Cancel->SetText(GetGameUIMan()->GetStringFromTable(10512));   // 取消
		m_pBtn_Confirm->Enable(false);
		m_pBtn_Cancel->Enable(true);
		m_pLbl_Title->SetText(GetGameUIMan()->GetStringFromTable(10507));
	}
	else
	{
		m_pBtn_Confirm->SetText(GetGameUIMan()->GetStringFromTable(10510)); // 延期
		m_pBtn_Cancel->SetText(GetGameUIMan()->GetStringFromTable(10511)); // 修改类型
		m_pBtn_Confirm->Enable(false);
		m_pBtn_Cancel->Enable(true);
		m_pLbl_Title->SetText(GetGameUIMan()->GetStringFromTable(10522));
	}	

	AlignTo(GetGameUIMan()->m_pDlgInventory,
		AUIDIALOG_ALIGN_OUTSIDE, AUIDIALOG_ALIGN_LEFT,
		AUIDIALOG_ALIGN_INSIDE, AUIDIALOG_ALIGN_TOP,
				-60 , 60 );
}
void CDlgOfflineShopCreate::OnEventLButtonDown(WPARAM wParam, LPARAM lParam, AUIObject * pObj)
{
	if( !pObj ) return;

	PAUIIMAGEPICTURE pImage = (PAUIIMAGEPICTURE)pObj;
	CECIvtrItem *pItem = (CECIvtrItem *)pObj->GetDataPtr("ptr_CECIvtrItem");
	if (pItem)
	{
		pItem->Freeze(false);
		GetGameUIMan()->PlayItemSound(pItem, false);
	}
	
	pImage->SetDataPtr(NULL);	
	pImage->SetCover(NULL, -1);
	pImage->SetData(0);	

	m_pBtn_Confirm->Enable(false);
}

void CDlgOfflineShopCreate::OnItemDragDrop(CECIvtrItem* pIvtrSrc, int iSrc, PAUIOBJECT pObjSrc, PAUIOBJECT pObjOver)
{
	if(pIvtrSrc->GetTemplateID() != 39176  &&
		pIvtrSrc->GetTemplateID() != 39177 &&
		pIvtrSrc->GetTemplateID() != 40065)	// 创建店铺需要道具
		return;	

	// for right-click scene
	if(!pObjOver)
		pObjOver = this->GetDlgItem("Img_Dye");	
	
	// for drag-drop scene
	if(!pObjOver || strcmp(pObjOver->GetName(), "Img_Dye"))
	{
		return;
	}

	OnEventLButtonDown(0, 0, pObjOver);
	
	pIvtrSrc->Freeze(true);
	pObjOver->SetData(iSrc);
	
	pObjOver->SetDataPtr(pIvtrSrc,"ptr_CECIvtrItem");
	GetGameUIMan()->PlayItemSound(pIvtrSrc, true);
	
	AString strFile;
	af_GetFileTitle(pIvtrSrc->GetIconFile(), strFile);
	strFile.MakeLower();
	PAUIIMAGEPICTURE pImage = (PAUIIMAGEPICTURE)pObjOver;
	pImage->SetCover( GetGameUIMan()->GetIconCover(CECGameUIMan::ICONS_INVENTORY),
					  GetGameUIMan()->GetIconIndex(CECGameUIMan::ICONS_INVENTORY, strFile) );
	
	AUICTranslate trans;
	pImage->SetHint(trans.Translate(pIvtrSrc->GetDesc()));
	
	m_pBtn_Confirm->Enable(true);	
}

//////////////////////////////////////////////////////////////////////////
//
//
//
//////////////////////////////////////////////////////////////////////////

AUI_BEGIN_COMMAND_MAP(CDlgOfflineShopType, CDlgBase)
AUI_ON_COMMAND("Btn_Confirm",	OnCommandConfirm)
AUI_ON_COMMAND("Btn_Cancel",	OnCommand_CANCEL)
AUI_END_COMMAND_MAP()


CDlgOfflineShopType::CDlgOfflineShopType()
{
	memset(m_pChk_Type,0,sizeof(m_pChk_Type));
	m_typeMask = 0;
}
void CDlgOfflineShopType::OnShowDialog()
{
	int i=0;
	for (i=0;i<COfflineShopCtrl::OST_NUM;i++)
	{
		m_pChk_Type[i]->Check((m_typeMask & (1<<i))!=0);
	}	
}
bool CDlgOfflineShopType::OnInitDialog()
{
	int i=0;
	for (i=0;i<COfflineShopCtrl::OST_NUM;i++)
	{
		AString str;
		str.Format("Chk_%d",i+1);
		DDX_Control(str,m_pChk_Type[i]);
	}

	return CDlgBase::OnInitDialog();
}
void CDlgOfflineShopType::OnHideDialog()
{
	OnCommandConfirm(NULL);
}
void CDlgOfflineShopType::OnCommandConfirm(const char *szCommand)
{
	COfflineShopCtrl* pCtrl = GetHostPlayer()->GetOfflineShopCtrl();
	if(!pCtrl) return;

	int i=0;
	m_typeMask = 0;
	for (i=0;i<COfflineShopCtrl::OST_NUM;i++)
	{
		if(m_pChk_Type[i]->IsChecked())
			m_typeMask |= (1<<i);
	}

	pCtrl->ViewShopListByType(m_typeMask);

	GetGameUIMan()->m_pDlgOffShopList->ClearControls();

//	Show(false);
}

///////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//
//
//
//////////////////////////////////////////////////////////////////////////
