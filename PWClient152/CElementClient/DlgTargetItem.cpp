// Filename	: DlgTargetItem.cpp
// Creator	: Shizhenhua
// Date		: 2014/2/12

#include "DlgTargetItem.h"
#include "EC_Global.h"
#include "EC_Game.h"
#include "EC_GameRun.h"
#include "EC_GameSession.h"
#include "EC_World.h"
#include "EC_ManNPC.h"
#include "EC_NPC.h"
#include "EC_HostPlayer.h"
#include "EC_GameUIMan.h"
#include "EC_Inventory.h"
#include "EC_IvtrItem.h"
#include "EC_IvtrConsume.h"
#include "DlgAutoHelp.h"
#include "ExpTypes.h"

#include <AUICTranslate.h>
#include <AFI.h>

#define new A_DEBUG_NEW

AUI_BEGIN_EVENT_MAP(CDlgTargetItem, CDlgBase)

AUI_ON_EVENT("Img_Item", WM_LBUTTONDOWN, OnEvent_LButtonDown)

AUI_END_EVENT_MAP()

CDlgTargetItem::CDlgTargetItem()
{
	m_pImg_Item = NULL;
	m_pLbl_Name = NULL;

	m_cntCheck.SetPeriod(1000);
	m_cntCheck.Reset();
}

bool CDlgTargetItem::OnInitDialog()
{
	DDX_Control("Img_Item", m_pImg_Item);
	DDX_Control("Lbl_Name", m_pLbl_Name);
	return true;
}

CECIvtrTargetItem * CDlgTargetItem::GetItem()
{
	CECInventory* pPack = GetHostPlayer()->GetPack();
	CECIvtrItem* pItem = pPack->GetItem((int)GetData());
	return dynamic_cast<CECIvtrTargetItem*>(pItem);
}

void CDlgTargetItem::OnShowDialog()
{
	CECIvtrTargetItem* pItem = GetItem();
	if( pItem )
	{
		AString strFile;
		af_GetFileTitle(pItem->GetIconFile(), strFile);
		strFile.MakeLower();
		m_pImg_Item->SetCover(GetGameUIMan()->m_pA2DSpriteIcons[CECGameUIMan::ICONS_INVENTORY],
				GetGameUIMan()->m_IconMap[CECGameUIMan::ICONS_INVENTORY][strFile]);

		ACString strDesc;
		strDesc.Format(_AL("%d"), pItem->GetCount());
		m_pImg_Item->SetText(strDesc);
		AUICTranslate trans;
		strDesc = trans.Translate(pItem->GetDesc());
		m_pImg_Item->SetHint(strDesc);
		m_pImg_Item->SetDataPtr(pItem, "ptr_CECIvtrItem");
	}
	else
	{
		m_pImg_Item->ClearCover();
		m_pImg_Item->SetText(_AL(""));
		m_pImg_Item->SetHint(_AL(""));
		m_pImg_Item->SetDataPtr(NULL);
		m_pImg_Item->SetColor(A3DCOLORRGB(255, 255, 255));
		m_pLbl_Name->SetText(GetStringFromTable(11255));
		return;
	}

	int iCurSel = GetHostPlayer()->GetSelectedTarget();
	if( ISNPCID(iCurSel) )
	{
		ACString strName;
		CECNPC* pNPC = GetWorld()->GetNPCMan()->GetNPC(iCurSel);
		if( pNPC && pItem->FindTargetIDForPop(pNPC->GetTemplateID()) )
			strName = pNPC->GetNameToShow();
		else
			strName = GetStringFromTable(11255);
		m_pLbl_Name->SetText(strName);
	}
	else if( ISPLAYERID(iCurSel) )
	{
		const ACHAR* szName = GetGameRun()->GetPlayerName(iCurSel, false);
		if( szName )
			m_pLbl_Name->SetText(szName);
		else
		{
			m_pLbl_Name->SetData(iCurSel);
			GetGameSession()->CacheGetPlayerBriefInfo(1, &iCurSel, 2);
		}
	}
}

void CDlgTargetItem::OnTick()
{
	if( IsShow() )
	{
		UpdateIconState();

		if( m_pLbl_Name->GetData() )
		{
			const ACHAR* szName = GetGameRun()->GetPlayerName(m_pLbl_Name->GetData(), false);
			if( szName )
			{
				m_pLbl_Name->SetText(szName);
				m_pLbl_Name->SetData(0);
			}
		}
	}

	CDlgBase::OnTick();
}

void CDlgTargetItem::UpdateIconState()
{
	int nMax;
	CECIvtrTargetItem* pItem = GetItem();
	if( pItem )
	{
		AUIClockIcon* pClock = m_pImg_Item->GetClockIcon();
		pClock->SetProgressRange(0, 1);
		pClock->SetProgressPos(1);

		if( pItem->IsFrozen() )
			m_pImg_Item->SetColor(A3DCOLORRGB(128, 128, 128));
		else
		{
			if( pItem->IsUseable() && !pItem->CheckUseCondition() )
				m_pImg_Item->SetColor(A3DCOLORRGB(192, 0, 0));
			else
				m_pImg_Item->SetColor(A3DCOLORRGB(255, 255, 255));
		}

		if( pItem->GetCoolTime(&nMax) > 0 )
		{
			pClock->SetProgressRange(0, nMax);
			pClock->SetProgressPos(nMax - pItem->GetCoolTime());
			pClock->SetColor(A3DCOLORRGBA(0, 0, 0, 128));
		}

		ACString strDesc;
		strDesc.Format(_AL("%d"), pItem->GetCount());
		m_pImg_Item->SetText(strDesc);
	}
}

void CDlgTargetItem::UseItem()
{
	CECIvtrTargetItem *pItem = GetItem();
	if( !pItem || pItem->IsFrozen() )
		return;

	if( CDlgAutoHelp::IsAutoHelp() )
		return;

	GetHostPlayer()->UseItemInPack(IVTRTYPE_PACK, (int)GetData());
}

void CDlgTargetItem::OnEvent_LButtonDown(WPARAM wParam, LPARAM lParam, AUIObject* pObj)
{
	CECIvtrTargetItem *pItem = GetItem();
	if( !pItem || pItem->IsFrozen() )
		return;

	A3DVIEWPORTPARAM *p = m_pA3DEngine->GetActiveViewport()->GetParam();
	GetGameUIMan()->m_ptLButtonDown.x = GET_X_LPARAM(lParam) - p->X;
	GetGameUIMan()->m_ptLButtonDown.y = GET_Y_LPARAM(lParam) - p->Y;
	GetGameUIMan()->InvokeDragDrop(this, pObj, GetGameUIMan()->m_ptLButtonDown);
	GetGameUIMan()->PlayItemSound(pItem, false);
}