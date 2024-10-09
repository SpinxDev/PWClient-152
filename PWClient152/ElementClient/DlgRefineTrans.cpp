// File		: DlgRefineTrans.cpp
// Creator	: Xiao Zhou
// Date		: 2007/8/31

#include "AFI.h"
#include "AUICTranslate.h"
#include "DlgRefineTrans.h"
#include "EC_ShortcutMgr.h"
#include "EC_GameUIMan.h"
#include "EC_GameSession.h"
#include "EC_HostPlayer.h"
#include "EC_Inventory.h"
#include "EC_IvtrEquip.h"

#define new A_DEBUG_NEW

AUI_BEGIN_COMMAND_MAP(CDlgRefineTrans, CDlgBase)

AUI_ON_COMMAND("confirm",		OnCommandConfirm)
AUI_ON_COMMAND("IDCANCEL",		OnCommandCancel)

AUI_END_COMMAND_MAP()

AUI_BEGIN_EVENT_MAP(CDlgRefineTrans, CDlgBase)

AUI_ON_EVENT("Img_Source",	WM_LBUTTONDOWN,	OnEventLButtonDown_Source)
AUI_ON_EVENT("Img_Target",	WM_LBUTTONDOWN,	OnEventLButtonDown_Target)

AUI_END_EVENT_MAP()

const int RefineTransFee[] = 
{
	1, 5, 12, 28, 60, 80, 100, 150, 250, 400, 650, 1000
};

//------------------------------------------------------------------------
// Right-Click Shortcut for CDlgRefineTrans
//------------------------------------------------------------------------
typedef CECShortcutMgr::SimpleClickShortcut<CDlgRefineTrans> RefineTransClickShortcut;
//------------------------------------------------------------------------

CDlgRefineTrans::CDlgRefineTrans()
{
	m_pBtn_Trans = NULL;
	m_pTxt_Qiankun = NULL;
	m_pTxt_Trans = NULL;
	m_pTxt_Source = NULL;
	m_pTxt_Target = NULL;
	m_pImg_Source = NULL;
	m_pImg_Target = NULL;
	m_nSoltSource = -1;
	m_nSoltTarget = -1;
}

CDlgRefineTrans::~CDlgRefineTrans()
{
}

bool CDlgRefineTrans::OnInitDialog()
{
	DDX_Control("Btn_Trans", m_pBtn_Trans);
	DDX_Control("Txt_Qiankun", m_pTxt_Qiankun);
	DDX_Control("Txt_Trans", m_pTxt_Trans);
	DDX_Control("Txt_Source", m_pTxt_Source);
	DDX_Control("Txt_Target", m_pTxt_Target);
	DDX_Control("Img_Source", m_pImg_Source);
	DDX_Control("Img_Target", m_pImg_Target);

	GetGameUIMan()->GetShortcutMgr()->RegisterShortCut(new RefineTransClickShortcut(this));

	return true;
}

void CDlgRefineTrans::OnCommandConfirm(const char *szCommand)
{
	CECIvtrEquip* pEquipSource = (CECIvtrEquip*)m_pImg_Source->GetDataPtr();
	CECIvtrEquip* pEquipTarget = (CECIvtrEquip*)m_pImg_Target->GetDataPtr();
	if( pEquipSource && pEquipTarget ) 
	{
		if( pEquipSource->GetRefineLevel() <= pEquipTarget->GetRefineLevel() )
		{
			GetGameUIMan()->MessageBox("", GetStringFromTable(879), MB_OK, A3DCOLORRGBA(255, 255, 255, 160));
			return;
		}
		
		// NOTICE: webtrade is a subtype of binding
		int srcType = (pEquipSource->GetProcType() & CECIvtrItem::PROC_BINDING) ? 1 : 0;
		int dstType = (pEquipTarget->GetProcType() & CECIvtrItem::PROC_BINDING) ? 1 : 0;
		srcType += (pEquipSource->GetProcType() & CECIvtrItem::PROC_CAN_WEBTRADE) ? 1 : 0;
		dstType += (pEquipTarget->GetProcType() & CECIvtrItem::PROC_CAN_WEBTRADE) ? 1 : 0;
		if(srcType > 0 && srcType != dstType )
		{
			ASSERT(srcType <= 2);

			if(1 == srcType) // binding only
			{
				ACString strMsg;
				strMsg.Format(GetStringFromTable(880), GetStringFromTable(882), GetStringFromTable(882));
				GetGameUIMan()->MessageBox("", strMsg, MB_OK, A3DCOLORRGBA(255, 255, 255, 160));
				return;
			}
			if(2 == srcType && dstType != 1 && dstType != 2) // webtrade binding
			{
				ACString strText;
				strText.Format(GetStringFromTable(905), GetStringFromTable(896), GetStringFromTable(882));

				ACString strMsg;
				strMsg.Format(GetStringFromTable(880), GetStringFromTable(896), strText);
				GetGameUIMan()->MessageBox("", strMsg, MB_OK, A3DCOLORRGBA(255, 255, 255, 160));
				return;
			}
		}

		if( GetHostPlayer()->GetPack()->GetItemTotalNum(12980) < (int)m_pTxt_Qiankun->GetData() )
		{
			GetGameUIMan()->MessageBox("", GetStringFromTable(881), MB_OK, A3DCOLORRGBA(255, 255, 255, 160));
			return;
		}
		GetGameSession()->c2s_CmdNPCSevRefineTrans(m_nSoltSource, pEquipSource->GetTemplateID(), m_nSoltTarget, pEquipTarget->GetTemplateID());
		OnCommandCancel("");
	}
}

void CDlgRefineTrans::OnCommandCancel(const char *szCommand)
{
	OnEventLButtonDown_Source(0, 0, NULL);
	OnEventLButtonDown_Target(0, 0, NULL);
	GetGameUIMan()->EndNPCService();
	Show(false);
	GetGameUIMan()->GetDialog("Win_Inventory")->Show(false);
	GetHostPlayer()->GetPack(IVTRTYPE_PACK)->UnfreezeAllItems();
}

void CDlgRefineTrans::OnShowDialog()
{
	OnEventLButtonDown_Source(0, 0, NULL);
	OnEventLButtonDown_Target(0, 0, NULL);
}

void CDlgRefineTrans::OnEventLButtonDown_Source(WPARAM wParam, LPARAM lParam, AUIObject * pObj)
{
	CECIvtrItem *pItem = (CECIvtrItem *)m_pImg_Source->GetDataPtr();
	if( pItem )
		pItem->Freeze(false);
	m_pImg_Source->ClearCover();
	m_pImg_Source->SetHint(_AL(""));
	m_pTxt_Source->SetText(_AL(""));
	m_pTxt_Qiankun->SetData(0);
	m_pTxt_Qiankun->SetText(_AL(""));
	m_pTxt_Trans->SetText(_AL(""));
	m_pImg_Source->SetDataPtr(NULL);
	m_nSoltSource = -1;
}

void CDlgRefineTrans::OnEventLButtonDown_Target(WPARAM wParam, LPARAM lParam, AUIObject * pObj)
{
	CECIvtrItem *pItem = (CECIvtrItem *)m_pImg_Target->GetDataPtr();
	if( pItem )
		pItem->Freeze(false);
	m_pImg_Target->ClearCover();
	m_pImg_Target->SetHint(_AL(""));
	m_pImg_Target->SetDataPtr(NULL);
	m_pTxt_Target->SetText(_AL(""));
	m_nSoltTarget = -1;
}

void CDlgRefineTrans::SetSourceEquip(CECIvtrItem *pItem, int nSlot)
{
	CECIvtrEquip* pEquip = (CECIvtrEquip*)pItem;
	if( pEquip->GetRefineMaterialNum() == 0 )
	{
		GetGameUIMan()->MessageBox("", GetStringFromTable(774), MB_OK, A3DCOLORRGB(255, 255, 255));
		return;
	}
	OnEventLButtonDown_Source(0, 0, NULL);

	m_pImg_Source->SetDataPtr(pEquip);
	m_nSoltSource = nSlot;
	pItem->Freeze(true);
	AString strFile;
	af_GetFileTitle(pEquip->GetIconFile(), strFile);
	strFile.MakeLower();
	m_pImg_Source->SetCover(GetGameUIMan()->m_pA2DSpriteIcons[CECGameUIMan::ICONS_INVENTORY],
		GetGameUIMan()->m_IconMap[CECGameUIMan::ICONS_INVENTORY][strFile]);

	m_pImg_Source->SetColor(
		(pItem->IsEquipment() && ((CECIvtrEquip *)pItem)->IsDestroying())
		? A3DCOLORRGB(128, 128, 128)
		: A3DCOLORRGB(255, 255, 255));

	AUICTranslate trans;
	const ACHAR * szDesc = pEquip->GetDesc();
	if( szDesc )
		m_pImg_Source->SetHint(trans.Translate(szDesc));
	else
		m_pImg_Source->SetHint(_AL(""));
	m_pTxt_Source->SetText(pEquip->GetName());
	ACHAR szText[20];
	a_sprintf(szText, _AL("%d"), pEquip->GetRefineLevel());
	m_pTxt_Trans->SetText(szText);
	if( pEquip->GetRefineLevel() > 0 && pEquip->GetRefineLevel() <= 12 )
	{
		m_pTxt_Qiankun->SetData(RefineTransFee[pEquip->GetRefineLevel() - 1]);
		a_sprintf(szText, _AL("%d"), RefineTransFee[pEquip->GetRefineLevel() - 1]);
		m_pTxt_Qiankun->SetText(szText);
	}
	else
	{
		m_pTxt_Qiankun->SetData(0);
		m_pTxt_Qiankun->SetText(_AL(""));
	}
}

void CDlgRefineTrans::SetTargetEquip(CECIvtrItem *pItem, int nSlot)
{
	CECIvtrEquip* pEquip = (CECIvtrEquip*)pItem;
	if( pEquip->GetRefineMaterialNum() == 0 )
	{
		GetGameUIMan()->MessageBox("", GetStringFromTable(774), MB_OK, A3DCOLORRGB(255, 255, 255));
		return;
	}
	OnEventLButtonDown_Target(0, 0, NULL);

	m_pImg_Target->SetDataPtr(pEquip);
	m_nSoltTarget = nSlot;
	pItem->Freeze(true);
	AString strFile;
	af_GetFileTitle(pEquip->GetIconFile(), strFile);
	strFile.MakeLower();
	m_pImg_Target->SetCover(GetGameUIMan()->m_pA2DSpriteIcons[CECGameUIMan::ICONS_INVENTORY],
		GetGameUIMan()->m_IconMap[CECGameUIMan::ICONS_INVENTORY][strFile]);

	m_pImg_Target->SetColor(
		(pItem->IsEquipment() && ((CECIvtrEquip *)pItem)->IsDestroying())
		? A3DCOLORRGB(128, 128, 128)
		: A3DCOLORRGB(255, 255, 255));

	AUICTranslate trans;
	const ACHAR * szDesc = pEquip->GetDesc();
	if( szDesc )
		m_pImg_Target->SetHint(trans.Translate(szDesc));
	else
		m_pImg_Target->SetHint(_AL(""));
	m_pTxt_Target->SetText(pItem->GetName());
}

void CDlgRefineTrans::OnTick()
{
	if(	GetHostPlayer()->GetPack()->GetItemTotalNum(12980) >= (int)m_pTxt_Qiankun->GetData() )
		m_pTxt_Qiankun->SetColor(A3DCOLORRGB(255, 255, 255));
	else
		m_pTxt_Qiankun->SetColor(A3DCOLORRGB(255, 0, 0));
	int piMax;
	m_pBtn_Trans->Enable(GetHostPlayer()->GetCoolTime(GP_CT_REFINETRANS, &piMax) == 0);
}

void CDlgRefineTrans::OnItemDragDrop(CECIvtrItem* pIvtrSrc, int iSrc, PAUIOBJECT pObjSrc, PAUIOBJECT pObjOver)
{
	if( pIvtrSrc->GetClassID() == CECIvtrItem::ICID_WEAPON ||
		pIvtrSrc->GetClassID() == CECIvtrItem::ICID_ARMOR ||
		pIvtrSrc->GetClassID() == CECIvtrItem::ICID_DECORATION )
	{
		enum { REFINE_TRANS_INVALID, REFINE_TRANS_SOURCE, REFINE_TRANS_TARGET, };
		static int s_LastDest = REFINE_TRANS_TARGET;
		int dest = REFINE_TRANS_INVALID;

		if( pObjOver )
		{
			// for drag-drop scene
			if( 0 == stricmp(pObjOver->GetName(), "Img_Source") )		dest = REFINE_TRANS_SOURCE;
			else if( 0 == stricmp(pObjOver->GetName(), "Img_Target") )  dest = REFINE_TRANS_TARGET;
		}
		else
		{
			// for right-click scene
			if(REFINE_TRANS_SOURCE == s_LastDest)
			{
				if(m_pImg_Source->GetDataPtr() == pIvtrSrc) OnEventLButtonDown_Source(0, 0, NULL);
				dest = REFINE_TRANS_TARGET;
			}
			else if(REFINE_TRANS_TARGET == s_LastDest)
			{
				if(m_pImg_Target->GetDataPtr() == pIvtrSrc) OnEventLButtonDown_Target(0, 0, NULL);
				dest = REFINE_TRANS_SOURCE;
			}
		}

		switch(dest)
		{
		case REFINE_TRANS_SOURCE:	this->SetSourceEquip(pIvtrSrc, iSrc); break;
		case REFINE_TRANS_TARGET:	this->SetTargetEquip(pIvtrSrc, iSrc); break;
		default: break;
		}

		s_LastDest = dest;
	}
}