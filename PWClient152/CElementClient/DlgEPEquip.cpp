// File		: DlgEPEquip.cpp
// Creator	: Xiao Zhou
// Date		: 2005/12/15

#include "AFI.h"
#include "DlgEPEquip.h"
#include "DlgFittingRoom.h"
#include "DlgItemDesc.h"
#include "AUICTranslate.h"
#include "EC_GameUIMan.h"
#include "EC_HostPlayer.h"
#include "EC_Inventory.h"
#include "EC_IvtrItem.h"
#include "EC_IvtrEquip.h"
#include "EC_Faction.h"
#include "EC_Player.h"
#include "EC_ManPlayer.h"
#include "EC_World.h"
#include "EC_Game.h"
#include "EC_GameRun.h"
#include "EC_ShortcutMgr.h"

#define new A_DEBUG_NEW

AUI_BEGIN_COMMAND_MAP(CDlgEPEquip, CDlgBase)

AUI_ON_COMMAND("try",		OnCommandTry)

AUI_END_COMMAND_MAP()

AUI_BEGIN_EVENT_MAP(CDlgEPEquip, CDlgBase)

AUI_ON_EVENT("Equip_*",		WM_LBUTTONDOWN,		OnEventLButtonDown)
AUI_ON_EVENT("Equip_*",		WM_RBUTTONDOWN,		OnEventRButtonDown)

AUI_END_EVENT_MAP()

CDlgEPEquip::CDlgEPEquip()
{
	m_pTxt_Name = NULL;
	m_pTxt_Prof = NULL;
	m_pTxt_Level = NULL;
	m_pTxt_Faction = NULL;
	m_pTxt_Reincarnation = NULL;
	int i;
	for( i = 0; i < SIZE_EQUIPIVTR; i++ )
		m_pImgEquip[i] = NULL;
}

CDlgEPEquip::~CDlgEPEquip()
{
}

bool CDlgEPEquip::OnInitDialog()
{
	char szItem[20];
	int i;
	for( i = 0; i < SIZE_EQUIPIVTR; i++ )
	{
		sprintf(szItem, "Equip_%02d", i);
		DDX_Control(szItem, m_pImgEquip[i]);
	}
	DDX_Control("Txt_Name", m_pTxt_Name);
	DDX_Control("Txt_Prof", m_pTxt_Prof);
	DDX_Control("Txt_Level", m_pTxt_Level);
	DDX_Control("Txt_Faction", m_pTxt_Faction);
	DDX_Control("Txt_ReincarnationCount", m_pTxt_Reincarnation);

	return true;
}

void CDlgEPEquip::OnShowDialog()
{
	int i;
	PAUIIMAGEPICTURE pObj;
	for( i = 0; i < SIZE_EQUIPIVTR; i++ )
	{
		pObj = m_pImgEquip[i];
		pObj->ClearCover();
		pObj->SetText(_AL(""));
		pObj->SetColor(A3DCOLORRGB(255, 255, 255));
		pObj->SetHint(GetStringFromTable(310 + i));
	}
	m_pTxt_Name->SetText(_AL(""));
	m_pTxt_Prof->SetText(_AL(""));
	m_pTxt_Level->SetText(_AL(""));
	m_pTxt_Faction->SetText(_AL(""));
	m_pTxt_Reincarnation->SetText(_AL(""));
}

void CDlgEPEquip::UpdateEquip(int idPlayer)
{
	CECPlayer *pPlayer = GetWorld()->GetPlayerMan()->GetPlayer(idPlayer);
	if( !pPlayer )
		return;

	Show(true);
	if( pPlayer->GetGender() == 0 )
	{
		GetDlgItem("Img_M")->Show(true);
		GetDlgItem("Img_F")->Show(false);
	}
	else
	{
		GetDlgItem("Img_M")->Show(false);
		GetDlgItem("Img_F")->Show(true);
	}

	ACHAR szText[40];
	CECInventory *pInventory = GetHostPlayer()->GetEPEquipPack();
	CECIvtrItem *pItem;
	PAUIIMAGEPICTURE pObj; 
	int i;

	m_pTxt_Name->SetText(pPlayer->GetName());
	m_pTxt_Prof->SetText(GetGameRun()->GetProfName(pPlayer->GetProfession()));
	const ROLEBASICPROP& bp = pPlayer->GetBasicProps();
	int nLevel = bp.iLevel;
	a_sprintf(szText, _AL("%d"), nLevel);
	m_pTxt_Level->SetText(szText);

	Faction_Info *pInfo = NULL;
	int idFaction = pPlayer->GetFactionID();
	CECFactionMan *pFMan = GetGame()->GetFactionMan();
	if( idFaction ) pInfo = pFMan->GetFaction(idFaction);
	if( idFaction <= 0 )
		m_pTxt_Faction->SetText(GetStringFromTable(251));
	else if( pInfo )
		m_pTxt_Faction->SetText(pInfo->GetName());
	else
		m_pTxt_Faction->SetText(_AL(""));

	a_sprintf(szText, _AL("%d"), pPlayer->GetReincarnationCount());
	m_pTxt_Reincarnation->SetText(szText);

	for( i = 0; i < SIZE_EQUIPIVTR; i++ )
	{
		pObj = m_pImgEquip[i];
		pItem = pInventory->GetItem(i);
		if( pItem )
		{
			pObj->SetDataPtr(pItem, "ptr_CECIvtrItem");
			if( pItem->GetCount() > 1 )
			{
				a_sprintf(szText, _AL("%d"), pItem->GetCount());
				pObj->SetText(szText);
			}
			else
				pObj->SetText(_AL(""));

			AString strFile;
			af_GetFileTitle(pItem->GetIconFile(), strFile);
			strFile.MakeLower();
			pObj->SetCover(GetGameUIMan()->m_pA2DSpriteIcons[CECGameUIMan::ICONS_INVENTORY],
				GetGameUIMan()->m_IconMap[CECGameUIMan::ICONS_INVENTORY][strFile]);

			pObj->SetColor(
				(pItem->IsEquipment() && ((CECIvtrEquip *)pItem)->IsDestroying())
				? A3DCOLORRGB(128, 128, 128)
				: A3DCOLORRGB(255, 255, 255));

			const wchar_t *pszHint = pItem->GetDesc(CECIvtrItem::DESC_NORMAL, pInventory);
			if( pszHint )
			{
				AUICTranslate trans;
				AWString strHint = trans.Translate(pszHint);

				pObj->SetHint(strHint);
			}
		}
		else
		{
			pObj->ClearCover();
			pObj->SetText(_AL(""));
			pObj->SetHint(GetStringFromTable(310 + i));
			pObj->SetDataPtr(NULL);
		}
	}
	AString strName;
	for (i = 0; i < SIZE_GENERALCARD_EQUIPIVTR; ++i) {
		strName.Format("Img_Card%d", i + 1);
		pObj = dynamic_cast<PAUIIMAGEPICTURE>(GetDlgItem(strName));
		if (pObj) {
			int index_card = EQUIPIVTR_GENERALCARD1 + i;
			pItem = pInventory->GetItem(index_card);
			if(pItem) {
				pObj->SetDataPtr(pItem,"ptr_CECIvtrItem");
				pObj->SetRenderHintCallback(CDlgItemDesc::CallBackGeneralCard, reinterpret_cast<unsigned long>(pItem), NULL, NULL);
				af_GetFileTitle(pItem->GetIconFile(), strName);
				strName.MakeLower();
				pObj->SetCover(GetGameUIMan()->m_pA2DSpriteIcons[CECGameUIMan::ICONS_INVENTORY],
					GetGameUIMan()->m_IconMap[CECGameUIMan::ICONS_INVENTORY][strName]);
			} else {
				pObj->SetCover(NULL, 0, 0);
				pObj->SetText(_AL(""));
				pObj->SetDataPtr(NULL);
				pObj->SetRenderHintCallback(CDlgItemDesc::CallBackGeneralCard, NULL, NULL, NULL);
				pObj->SetHint(GetStringFromTable(390 + i));
			}
		}
	}
}

void CDlgEPEquip::OnCommandTry(const char * szCommand)
{
	GetGameUIMan()->m_pDlgFittingRoom->Show(true);
}

void CDlgEPEquip::OnEventLButtonDown(WPARAM wParam, LPARAM lParam, AUIObject *pObj)
{
	CECIvtrItem *pItem = (CECIvtrItem *)pObj->GetDataPtr("ptr_CECIvtrItem");
	if( !pItem || pItem->IsFrozen())
		return;

	A3DVIEWPORTPARAM *p = m_pA3DEngine->GetActiveViewport()->GetParam();
	POINT ptCapture = { GET_X_LPARAM(lParam) - p->X, GET_Y_LPARAM(lParam) - p->Y };

	GetGameUIMan()->m_ptLButtonDown = ptCapture;

	GetGameUIMan()->InvokeDragDrop(this, pObj, ptCapture);
	GetGameUIMan()->PlayItemSound(pItem, false);
}

void CDlgEPEquip::OnEventRButtonDown(WPARAM wParam, LPARAM lParam, AUIObject *pObj)
{
	// try to trigger shortcuts
	GetGameUIMan()->GetShortcutMgr()->TriggerShortcuts(this->GetName(), pObj);
}