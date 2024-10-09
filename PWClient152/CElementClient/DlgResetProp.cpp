// File		: DlgResetProp.cpp
// Creator	: Xiao Zhou
// Date		: 2006/1/12

#include "AFI.h"
#include "DlgResetProp.h"
#include "EC_Game.h"
#include "EC_GameUIMan.h"
#include "EC_IvtrItem.h"
#include "EC_HostPlayer.h"
#include "EC_GameSession.h"
#include "elementdataman.h"
#include "EC_Inventory.h"
#include "EC_ShortcutMgr.h"

#define new A_DEBUG_NEW

AUI_BEGIN_COMMAND_MAP(CDlgResetProp, CDlgBase)

AUI_ON_COMMAND("confirm",		OnCommandConfirm)
AUI_ON_COMMAND("IDCANCEL",		OnCommandCancel)

AUI_END_COMMAND_MAP()

AUI_BEGIN_EVENT_MAP(CDlgResetProp, CDlgBase)

AUI_ON_EVENT("Img_Item",	WM_LBUTTONDOWN,	OnEventLButtonDown)

AUI_END_EVENT_MAP()

//------------------------------------------------------------------------
// Right-Click Shortcut for CDlgResetProp
//------------------------------------------------------------------------
typedef CECShortcutMgr::SimpleClickShortcut<CDlgResetProp> ResetPropClickShortcut;
//------------------------------------------------------------------------

CDlgResetProp::CDlgResetProp()
{
}

CDlgResetProp::~CDlgResetProp()
{
}


void CDlgResetProp::OnCommandConfirm(const char *szCommand)
{
	CECIvtrItem *pItem = (CECIvtrItem *)GetDataPtr();
	if( pItem )
	{
		int i;
		for(i = 0; i < 5; i++)
		{
			if( m_pRdo_Item[i]->IsChecked() )
			{
				pItem->Freeze(false);
				GetGameSession()->c2s_CmdNPCSevReturnStatusPt(
					m_pRdo_Item[i]->GetData(), GetData());
				SetDataPtr(NULL);
				break;
			}
		}
		Show(false);
		GetGameUIMan()->EndNPCService();
		GetGameUIMan()->GetDialog("Win_Inventory")->Show(false);
		GetHostPlayer()->GetPack(IVTRTYPE_PACK)->UnfreezeAllItems();
	}
}

void CDlgResetProp::OnCommandCancel(const char *szCommand)
{
	OnEventLButtonDown(0, 0, NULL);
	Show(false);
	GetGameUIMan()->EndNPCService();
	GetGameUIMan()->GetDialog("Win_Inventory")->Show(false);
	GetHostPlayer()->GetPack(IVTRTYPE_PACK)->UnfreezeAllItems();
}

bool CDlgResetProp::OnInitDialog()
{
	GetGameUIMan()->GetShortcutMgr()->RegisterShortCut(new ResetPropClickShortcut(this));

	int i;
	for(i = 0; i < 5; i++)
	{
		char szName[20];
		sprintf(szName, "Txt_Item%d", i + 1);
		m_pTxt_Item[i] = (PAUILABEL)GetDlgItem(szName);
		sprintf(szName, "Rdo_Item%d", i + 1);
		m_pRdo_Item[i] = (PAUIRADIOBUTTON)GetDlgItem(szName);
	}
	m_pImg_Item = (PAUIIMAGEPICTURE)GetDlgItem("Img_Item");
	m_pBtn_Confirm = (PAUISTILLIMAGEBUTTON)GetDlgItem("Btn_Confirm");
	OnEventLButtonDown(0, 0, NULL);

	return true;
}

void CDlgResetProp::OnShowDialog()
{
	OnEventLButtonDown(0, 0, NULL);
}

void CDlgResetProp::OnTick()
{
	int i;
	m_pBtn_Confirm->Enable(false);
	for(i = 0; i < 5; i++)
	{
		if( m_pRdo_Item[i]->IsChecked() )
		{
			m_pBtn_Confirm->Enable(true);
			break;
		}
	}
}

void CDlgResetProp::SetScroll(CECIvtrItem *pItem)
{
	DATA_TYPE DataType;
	NPC_RESETPROP_SERVICE *pData = (NPC_RESETPROP_SERVICE *)GetGame()->GetElementDataMan()->
		get_data_ptr(GetGameUIMan()->m_pCurNPCEssence->id_resetprop_service, ID_SPACE_ESSENCE, DataType);
	if( DataType != DT_NPC_RESETPROP_SERVICE ) return;
	bool bResetScroll = false;
	int i;
	for(i = 0; i < 15; i++)
		if( pData->prop_entry[i].id_object_need && 
			pItem->GetTemplateID() == pData->prop_entry[i].id_object_need )
		{
			bResetScroll = true;
			break;
		}
	if( !bResetScroll )
		return;

	OnEventLButtonDown(0, 0, NULL);
	int n = 0;
	for(i = 0; i < 15; i++)
		if( pData->prop_entry[i].id_object_need && 
			pItem->GetTemplateID() == pData->prop_entry[i].id_object_need )
		{
			m_pRdo_Item[n]->Show(true);
			m_pRdo_Item[n]->Check(false);
			m_pRdo_Item[n]->SetData(i);
			ACString szProp = _AL("");
			ACHAR szText[10];
			if( pData->prop_entry[i].strength_delta )
			{
				a_sprintf(szText, _AL("%d "), pData->prop_entry[i].strength_delta);
				szProp += GetStringFromTable(371);
				szProp += szText;
			}
			if( pData->prop_entry[i].agility_delta )
			{
				a_sprintf(szText, _AL("%d "), pData->prop_entry[i].agility_delta);
				szProp += GetStringFromTable(372);
				szProp += szText;
			}
			if( pData->prop_entry[i].vital_delta )
			{
				a_sprintf(szText, _AL("%d "), pData->prop_entry[i].vital_delta);
				szProp += GetStringFromTable(373);
				szProp += szText;
			}
			if( pData->prop_entry[i].energy_delta )
			{
				a_sprintf(szText, _AL("%d "), pData->prop_entry[i].energy_delta);
				szProp += GetStringFromTable(374);
				szProp += szText;
			}
			m_pTxt_Item[n]->Show(true);
			m_pTxt_Item[n]->SetText(szProp);
			n++;
			if(n == 5)
				break;
		}
	for(i = n; i < 5; i++)
	{
		m_pRdo_Item[i]->Check(false);
		m_pRdo_Item[i]->Show(false);
		m_pTxt_Item[i]->Show(false);
	}
	AString strFile;
	af_GetFileTitle(pItem->GetIconFile(), strFile);
	strFile.MakeLower();
	m_pImg_Item->SetCover(GetGameUIMan()->m_pA2DSpriteIcons[CECGameUIMan::ICONS_INVENTORY],
		GetGameUIMan()->m_IconMap[CECGameUIMan::ICONS_INVENTORY][strFile]);
	SetDataPtr(pItem);
	pItem->Freeze(true);
	SetData(pItem->GetTemplateID());
}

void CDlgResetProp::OnEventLButtonDown(WPARAM wParam, LPARAM lParam, AUIObject * pObj)
{
	CECIvtrItem *pItem = (CECIvtrItem *)GetDataPtr();
	if( pItem )
		pItem->Freeze(false);
	m_pImg_Item->ClearCover();
	m_pImg_Item->SetHint(_AL(""));
	SetDataPtr(NULL);
	SetData(0);
	int i;
	for(i = 0; i < 5; i++)
	{
		m_pRdo_Item[i]->Check(false);
		m_pRdo_Item[i]->Show(false);
		m_pTxt_Item[i]->Show(false);
	}
}

void CDlgResetProp::OnItemDragDrop(CECIvtrItem* pIvtrSrc, int iSrc, PAUIOBJECT pObjSrc, PAUIOBJECT pObjOver)
{
	// no need to check name "Img_Item", only one available drag-drop target
	this->SetScroll(pIvtrSrc);
}