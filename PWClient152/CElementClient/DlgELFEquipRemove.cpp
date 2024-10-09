// Filename	: DlgELFEquipRemove.cpp
// Creator	: Chen Zhixin
// Date		: November 27, 2008

#include "WindowsX.h"
#include "AFI.h"
#include "AUICTranslate.h"
#include "AUIFrame.h"
#include "AUICommon.h"
#include "AUIManager.h"
#include "DlgELFEquipRemove.h"
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

AUI_BEGIN_COMMAND_MAP(CDlgELFEquipRemove, CDlgBase)

AUI_ON_COMMAND("IDCANCEL",		OnCommand_CANCEL)
AUI_ON_COMMAND("confirm",		OnCommand_Confirm)

AUI_END_COMMAND_MAP()

AUI_BEGIN_EVENT_MAP(CDlgELFEquipRemove, CDlgBase)

AUI_ON_EVENT("Img_ELF",		WM_LBUTTONDOWN,		OnEventLButtonDown_ELF)
AUI_ON_EVENT("Newitem_*",	WM_LBUTTONDOWN,		OnEventLButtonDown_NewItem)

AUI_END_EVENT_MAP()

//------------------------------------------------------------------------
// Right-Click Shortcut for CDlgELFEquipRemove
//------------------------------------------------------------------------
typedef CECShortcutMgr::SimpleClickShortcut<CDlgELFEquipRemove> ELFEquipRemoveClickShortcut;
//------------------------------------------------------------------------

CDlgELFEquipRemove::CDlgELFEquipRemove()
{
	m_pImg_ELF = NULL;
	m_pBtn_Confirm = NULL;
	m_pTxt_Item = NULL;
	m_nSolt = -1;
	m_pChk_Equip[0] = NULL;
	m_pChk_Equip[1] = NULL;
	m_pChk_Equip[2] = NULL;
	m_pChk_Equip[3] = NULL;
}

CDlgELFEquipRemove::~CDlgELFEquipRemove()
{
}

bool CDlgELFEquipRemove::OnInitDialog()
{
	GetGameUIMan()->GetShortcutMgr()->RegisterShortCut(new ELFEquipRemoveClickShortcut(this));

	DDX_Control("Img_ELF", m_pImg_ELF);
	DDX_Control("Btn_AG", m_pBtn_Confirm);
	DDX_Control("Txt_ELF", m_pTxt_Item);

	DDX_Control("Check_1", m_pChk_Equip[0]);
	DDX_Control("Check_2", m_pChk_Equip[1]);
	DDX_Control("Check_3", m_pChk_Equip[2]);
	DDX_Control("Check_4", m_pChk_Equip[3]);
	int i;
	char szName[50];
	for (i = 0; i < 4; i++)
	{
		sprintf(szName, "Olditem_%d", i + 1);
		m_pImg_OldItem[i] = (PAUIIMAGEPICTURE)GetDlgItem(szName);
		sprintf(szName, "Newitem_%d", i + 1);
		m_pImg_NewItem[i] = (PAUIIMAGEPICTURE)GetDlgItem(szName);
		sprintf(szName, "Img_Arrow%d", i + 1);
		m_pImg_Arrow[i] = (PAUIIMAGEPICTURE)GetDlgItem(szName);
		sprintf(szName, "Txt_Noitem%d", i + 1);
		m_pLab_NoItem[i] = (PAUILABEL)GetDlgItem(szName);
		m_nSlotNewItem[i] = -1;
	}
	return true;
}

void CDlgELFEquipRemove::OnShowDialog()
{
	OnEventLButtonDown_ELF(0, 0, NULL);
	m_nLastSolt = -1;
}

void CDlgELFEquipRemove::RefreshHostDetails()
{
 	if( !IsShow() ) return;

	if( m_pImg_ELF->GetDataPtr() ) 
	{
		CECIvtrGoblin *pELF = (CECIvtrGoblin*)GetHostPlayer()->GetPack()->GetItem(m_nSolt);
		if( pELF )
		{
			for (int i = 0; i < 4; i++)
			{
				if (pELF->GetEquip(i) != 0)
				{
					m_pChk_Equip[i]->Enable(true);
					m_pImg_OldItem[i]->Show(true);
					m_pImg_NewItem[i]->Show(true);
					m_pImg_Arrow[i]->Show(true);
					CECIvtrGoblinEquip *pItem = new CECIvtrGoblinEquip(pELF->GetEquip(i), 0);
					AString strFile;
					af_GetFileTitle(pItem->GetIconFile(), strFile);
					strFile.MakeLower();
					m_pImg_OldItem[i]->SetCover(GetGameUIMan()->m_pA2DSpriteIcons[CECGameUIMan::ICONS_INVENTORY],
						GetGameUIMan()->m_IconMap[CECGameUIMan::ICONS_INVENTORY][strFile]);
					AUICTranslate trans;
					const ACHAR * szDesc = pItem->GetDesc();
					if( szDesc )
						m_pImg_OldItem[i]->SetHint(trans.Translate(szDesc));
					else
						m_pImg_OldItem[i]->SetHint(_AL(""));
				}
				else
				{
					m_pImg_OldItem[i]->ClearCover();
					m_pImg_OldItem[i]->SetHint(_AL(""));
					m_pLab_NoItem[i]->Show(true);
					m_pChk_Equip[i]->Enable(false);
				}
			}
		}
	}
}

void CDlgELFEquipRemove::SetELF(CECIvtrItem *pItem1, int nSlot)
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
	m_pTxt_Item->SetText(pELF->GetName());
	RefreshHostDetails();
}

void CDlgELFEquipRemove::SetItem(CECIvtrItem *pItem1, int nSlot, int index)
{
	CECIvtrGoblinEquip* pItem = (CECIvtrGoblinEquip*)pItem1;
	//OnEventLButtonDown_ELF(0, 0, NULL);
	m_nSlotNewItem[index] = nSlot;
	m_pImg_NewItem[index]->SetDataPtr(pItem);
	pItem1->Freeze(true);
	AString strFile;
	af_GetFileTitle(pItem->GetIconFile(), strFile);
	strFile.MakeLower();
	m_pImg_NewItem[index]->SetCover(GetGameUIMan()->m_pA2DSpriteIcons[CECGameUIMan::ICONS_INVENTORY],
		GetGameUIMan()->m_IconMap[CECGameUIMan::ICONS_INVENTORY][strFile]);
	AUICTranslate trans;
	const ACHAR * szDesc = pItem->GetDesc();
	if( szDesc )
		m_pImg_NewItem[index]->SetHint(trans.Translate(szDesc));
	else
		m_pImg_NewItem[index]->SetHint(_AL(""));
	RefreshHostDetails();
}

void CDlgELFEquipRemove::OnCommand_Confirm(const char *szCommand)
{
	CECIvtrGoblin* pELF = (CECIvtrGoblin*)m_pImg_ELF->GetDataPtr();
	if (!pELF)
	{
		GetGameUIMan()->MessageBox("", GetStringFromTable(7150), MB_OK, A3DCOLORRGBA(255, 255, 255, 160));
		return;
	}
	int select = 0;
	for (int i = 0; i < 4; i++)
	{
		if (m_pChk_Equip[i]->IsChecked())
		{
			select = 1;
			break;
		}
	}
	if (0 == select)
	{
		GetGameUIMan()->MessageBox("", GetStringFromTable(7151), MB_OK, A3DCOLORRGBA(255, 255, 255, 160));
		return;
	}
	PAUIDIALOG pMsgBox;
	GetGameUIMan()->MessageBox("Game_ELFEquipRemove", GetStringFromTable(7152), MB_YESNO, 
		A3DCOLORRGBA(255, 255, 255, 160), &pMsgBox);
}

void CDlgELFEquipRemove::ConfirmOK(bool confirm)
{
	if( confirm && m_nSolt >= 0 && GetHostPlayer()->GetPack()->GetItem(m_nSolt)) 
	{
		for (int i = 0; i < 4; i++)
		{
			if (m_pChk_Equip[i]->IsChecked())
			{
				if (m_pImg_NewItem[i]->GetDataPtr())
				{
					GetGameSession()->c2s_CmdNPCSevGoblinEquipDestroy(m_nSolt,i, m_nSlotNewItem[i]);
				}
				else
					GetGameSession()->c2s_CmdNPCSevGoblinEquipDestroy(m_nSolt,i);
			}
		}
		m_nLastSolt = m_nSolt;
		OnEventLButtonDown_ELF(0, 0, NULL);
	}
	
}

void CDlgELFEquipRemove::OnCommand_CANCEL(const char * szCommand)
{
	OnEventLButtonDown_ELF(0, 0, NULL);
	Show(false);
	if (0 != stricmp(szCommand, "NotEndNPCService"))
	{
		GetGameUIMan()->EndNPCService();
		GetGameUIMan()->GetDialog("Win_Inventory")->Show(false);
	}
	GetHostPlayer()->GetPack(IVTRTYPE_PACK)->UnfreezeAllItems();
}

void CDlgELFEquipRemove::OnEventLButtonDown_ELF(WPARAM wParam, LPARAM lParam, AUIObject * pObj)
{
 	CECIvtrItem *pELF = (CECIvtrItem *)m_pImg_ELF->GetDataPtr();
 	if( pELF )
 		pELF->Freeze(false);
 	m_pImg_ELF->ClearCover();
 	m_pImg_ELF->SetHint(_AL(""));
 	m_pImg_ELF->SetDataPtr(NULL);
	m_pTxt_Item->SetText(_AL(""));
	char szName[50];
	for (int i = 0; i < 4; i++)
	{
		m_pChk_Equip[i]->Enable(false);
		m_pChk_Equip[i]->Check(false);
		m_pImg_OldItem[i]->Show(false);
		m_pImg_NewItem[i]->Show(false);
		m_pImg_Arrow[i]->Show(false);
		m_pLab_NoItem[i]->Show(false);
		m_pImg_OldItem[i]->ClearCover();
		m_pImg_OldItem[i]->SetHint(_AL(""));
		sprintf(szName, "Newitem_%d", i+1);
		OnEventLButtonDown_NewItem(0, 0, GetDlgItem(szName));
	}
 	m_nSolt = -1;
}

void CDlgELFEquipRemove::OnEventLButtonDown_NewItem(WPARAM wParam, LPARAM lParam, AUIObject * pObj)
{
	int index = atoi(pObj->GetName() + strlen("Newitem_")) -1;
	CECIvtrItem *pNewItem = (CECIvtrItem *)m_pImg_NewItem[index]->GetDataPtr();
	if( pNewItem )
		pNewItem->Freeze(false);
	m_pImg_NewItem[index]->ClearCover();
	m_pImg_NewItem[index]->SetHint(_AL(""));
	m_pImg_NewItem[index]->SetDataPtr(NULL);
}

void CDlgELFEquipRemove::RefineResult(int result)
{
	CECIvtrItem *pItem;
	if( m_nLastSolt != -1 )
	{
		pItem = GetHostPlayer()->GetPack()->GetItem(m_nLastSolt);
		if( pItem )
			SetELF(pItem, m_nLastSolt);
	}
//	m_pBtn_Confirm->Enable(false);
	m_dwEnalbeTick = GetTickCount();
}

void CDlgELFEquipRemove::OnItemDragDrop(CECIvtrItem* pIvtrSrc, int iSrc, PAUIOBJECT pObjSrc, PAUIOBJECT pObjOver)
{
	// for right-click scene
	if(!pObjOver)
	{
		if(pIvtrSrc->GetClassID() == CECIvtrItem::ICID_GOBLIN)
		{
			pObjOver = this->GetDlgItem("Img_ELF");
		}
		else if(pIvtrSrc->GetClassID() == CECIvtrItem::ICID_GOBLIN_EQUIP)
		{
			CECIvtrGoblinEquip * pGoblinEqu = (CECIvtrGoblinEquip *)pIvtrSrc;
			int iSlotStart = 0;	
			while(!pObjOver)
			{
				AString strName;
				PAUIOBJECT pObj =  GetDlgItem(strName.Format("Newitem_%d", ++iSlotStart));
				if(!pObj) break;
				
				if(pObj->IsShow() && pGoblinEqu->CanEquippedTo(iSlotStart - 1))
				{
					// find a valid one
					pObjOver = pObj;
				}
			}
		}
	}

	if(pObjOver && 0 == stricmp(pObjOver->GetName(), "Img_ELF"))
	{
		if( pIvtrSrc->GetClassID() == CECIvtrItem::ICID_GOBLIN )
			this->SetELF(pIvtrSrc, iSrc);
	}
	else if(pObjOver && strstr(pObjOver->GetName(), "Newitem_"))
	{
		int iDst = atoi(pObjOver->GetName() + strlen("Newitem_")) - 1;
		
		if( pIvtrSrc->GetClassID() == CECIvtrItem::ICID_GOBLIN_EQUIP && ((CECIvtrGoblinEquip *)pIvtrSrc)->CanEquippedTo(iDst))			
		{
			this->SetItem(pIvtrSrc, iSrc, iDst);
		}
	}
}