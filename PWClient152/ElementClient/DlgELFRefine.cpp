// Filename	: DlgELFRefine.cpp
// Creator	: Chen Zhixin
// Date		: October 17, 2008

#include "WindowsX.h"
#include "AFI.h"
#include "AUICTranslate.h"
#include "AUIFrame.h"
#include "AUICommon.h"
#include "AUIManager.h"
#include "DlgELFRefine.h"
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

AUI_BEGIN_COMMAND_MAP(CDlgELFRefine, CDlgBase)

AUI_ON_COMMAND("IDCANCEL",		OnCommand_CANCEL)
AUI_ON_COMMAND("confirm",		OnCommand_Confirm)
AUI_ON_COMMAND("fillmax",		OnCommand_FillMax)

AUI_END_COMMAND_MAP()

AUI_BEGIN_EVENT_MAP(CDlgELFRefine, CDlgBase)

AUI_ON_EVENT("Img_Item",	WM_LBUTTONDOWN,		OnEventLButtonDown_Item)
AUI_ON_EVENT("Img_ELF",		WM_LBUTTONDOWN,		OnEventLButtonDown_ELF)
AUI_ON_EVENT("Btn_Up",		WM_LBUTTONDOWN,		OnEventLButtonDown_Add)
AUI_ON_EVENT("Btn_Down",	WM_LBUTTONDOWN,		OnEventLButtonDown_Minus)

AUI_END_EVENT_MAP()

//------------------------------------------------------------------------
// Right-Click Shortcut for CDlgELFRefine
//------------------------------------------------------------------------
typedef CECShortcutMgr::SimpleClickShortcut<CDlgELFRefine> ELFRefineClickShortcut;
//------------------------------------------------------------------------

CDlgELFRefine::CDlgELFRefine()
{
	m_pTxt_DescNorm = NULL;
	m_pTxt_Name = NULL;
	m_pTxt_Item = NULL;
	m_pImg_Item = NULL;
	m_pImg_ELF = NULL;
	m_pBtn_Confirm = NULL;
	m_pTxt_DescRefined = NULL;
	m_pTxt_RefineList = NULL;
	m_pTxt_Prob = NULL;
	m_pTxt_FailResult = NULL;
	m_nSolt = -1;
	m_nLastSoltItem = -1;
	m_nCount = 0;
	m_nMaxCount = 1;
}

CDlgELFRefine::~CDlgELFRefine()
{
}

bool CDlgELFRefine::OnInitDialog()
{
	GetGameUIMan()->GetShortcutMgr()->RegisterShortCut(new ELFRefineClickShortcut(this));

	DDX_Control("Txt_Norm", m_pTxt_DescNorm);
	DDX_Control("Txt_ELFName", m_pTxt_Name);
	DDX_Control("Txt_Item", m_pTxt_Item);
	DDX_Control("Img_Item", m_pImg_Item);
	DDX_Control("Img_ELF", m_pImg_ELF);
	DDX_Control("Btn_Confirm", m_pBtn_Confirm);
	DDX_Control("Txt_Refined", m_pTxt_DescRefined);
	DDX_Control("Txt_RefineList", m_pTxt_RefineList);
	DDX_Control("Txt_Prob", m_pTxt_Prob);
	DDX_Control("Txt_FailResult", m_pTxt_FailResult);
	return true;
}

void CDlgELFRefine::DoDataExchange(bool bSave)
{
	CDlgBase::DoDataExchange(bSave);
	DDX_EditBox(bSave, "Txt_Item", m_nCount);
}

void CDlgELFRefine::OnShowDialog()
{
	OnEventLButtonDown_Item(0, 0, NULL);
	OnEventLButtonDown_ELF(0, 0, NULL);
	m_pTxt_RefineList->SetText(_AL(""));
	m_nLastSolt = -1;
	m_nLastSoltItem = -1;
	m_nCount = 0;
	UpdateData(false);
}

void CDlgELFRefine::OnTick(void)
{
	UpdateData(true);
	if (m_nCount < 0)
	{
		m_nCount = 0;
	}
	if (m_nCount > m_nMaxCount)
	{
		m_nCount = m_nMaxCount;
	}
	UpdateData(false);
}

bool CDlgELFRefine::Render(void)
{

	RefreshHostDetails();

	return CDlgBase::Render();
}

void CDlgELFRefine::RefreshHostDetails()
{
 	if( !IsShow() ) return;

	if( m_nSolt >= 0 &&  GetHostPlayer()->GetPack()->GetItem(m_nSolt)
		&& m_nSoltItem >= 0 && GetHostPlayer()->GetPack()->GetItem(m_nSolt)) 
	{
		CECIvtrGoblin* pELF = (CECIvtrGoblin *)m_pImg_ELF->GetDataPtr();
		CECIvtrItem* pItem = (CECIvtrItem *)m_pImg_Item->GetDataPtr();
		const IVTR_ESSENCE_GOBLIN essence = pELF->GetEssence();
		int level = essence.data.refine_level;
		int id = pItem->GetTemplateID();
		float fPro;
		int index = 0;
		if (ELF_REFINE_TICKET0 == id)
		{
			fPro = pELF->GetRefineSuccProb0(level);
			index = 0;
		}
		else if (ELF_REFINE_TICKET1 == id)
		{
			fPro = pELF->GetRefineSuccProb1(level);
			index = 0;
		}
		else if (ELF_REFINE_TICKET2 == id)
		{
			fPro = pELF->GetRefineSuccProb2(level);
			index = 1;
		}
		else if (ELF_REFINE_TICKET3 == id)
		{
			int MaxNeed = CECIvtrGoblin::elf_refine_max_use_ticket3[level];
			if (m_nCount >= MaxNeed)
			{
				m_nCount = MaxNeed;
			}
			fPro = pELF->GetRefineSuccProb3(level, m_nCount);
			index = 2;
		}
		ACHAR strText[30];
		a_sprintf(strText, _AL("%.1f%%"), fPro*100);
		m_pTxt_Prob->SetText(strText);
		m_pTxt_FailResult->SetText(GetGameUIMan()->GetStringFromTable(7200+index));
//		m_pBtn_Confirm->Enable(true);
	}
	else
	{
		m_pTxt_Prob->SetText(_AL(""));
		m_pTxt_FailResult->SetText(_AL(""));
	}
	UpdateData(false);
}

void CDlgELFRefine::SetELF(CECIvtrItem *pItem1, int nSlot)
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
	m_pTxt_Name->SetText(pELF->GetName());
	const IVTR_ESSENCE_GOBLIN essence = pELF->GetEssence();
	m_pTxt_DescNorm->SetText(GetAUIManager()->GetStringFromTable(7000 + essence.data.refine_level));
	m_pTxt_DescRefined->SetText(GetAUIManager()->GetStringFromTable(7000 + essence.data.refine_level + 1));
}

void CDlgELFRefine::SetItem(CECIvtrItem *pItem2, int nSlot)
{
	CECIvtrItem* pItem = pItem2;
	OnEventLButtonDown_Item(0, 0, NULL);
	m_pImg_Item->SetDataPtr(pItem);
	m_nSoltItem = nSlot;
	pItem2->Freeze(true);
	AString strFile;
	af_GetFileTitle(pItem->GetIconFile(), strFile);
	strFile.MakeLower();
	m_pImg_Item->SetCover(GetGameUIMan()->m_pA2DSpriteIcons[CECGameUIMan::ICONS_INVENTORY],
		GetGameUIMan()->m_IconMap[CECGameUIMan::ICONS_INVENTORY][strFile]);
	AUICTranslate trans;
	const ACHAR * szDesc = pItem->GetDesc();
	if( szDesc )
		m_pImg_Item->SetHint(trans.Translate(szDesc));
	else
		m_pImg_Item->SetHint(_AL(""));

	if (ELF_REFINE_TICKET3 == pItem->GetTemplateID())
	{
		m_nMaxCount = GetHostPlayer()->GetPack()->GetItemTotalNum(pItem->GetTemplateID());
	}
	else
		m_nMaxCount = 1;
	m_nCount = 1;
	m_pTxt_Item->Enable(true);
	UpdateData(false);
}

void CDlgELFRefine::OnCommand_Confirm(const char *szCommand)
{
	UpdateData(true);
	CECIvtrGoblin* pELF = (CECIvtrGoblin*)m_pImg_ELF->GetDataPtr();
	CECIvtrItem* pItem = (CECIvtrItem*)m_pImg_Item->GetDataPtr();
	if (!pELF)
	{
		GetGameUIMan()->MessageBox("", GetStringFromTable(7130), MB_OK, A3DCOLORRGBA(255, 255, 255, 160));
		return;
	}
	if (!pItem || m_nCount <= 0)
	{
		GetGameUIMan()->MessageBox("", GetStringFromTable(7131), MB_OK, A3DCOLORRGBA(255, 255, 255, 160));
		return;
	}

	GetGameSession()->c2s_CmdNPCSevGoblinRefine(m_nSolt, m_nSoltItem, m_nCount);
	m_nLastSolt = m_nSolt;
	m_nLastSoltItem = m_nSoltItem;
// 	m_nCount = 0;
// 	m_nMaxCount = 0;
	UpdateData(false);
}

void CDlgELFRefine::OnCommand_CANCEL(const char * szCommand)
{
	OnEventLButtonDown_Item(0, 0, NULL);
	OnEventLButtonDown_ELF(0, 0, NULL);
	GetGameUIMan()->EndNPCService();
	Show(false);
	GetGameUIMan()->GetDialog("Win_Inventory")->Show(false);
	GetHostPlayer()->GetPack(IVTRTYPE_PACK)->UnfreezeAllItems();
	m_nCount = 0;
}

void CDlgELFRefine::OnCommand_FillMax(const char * szCommand)
{
	m_nCount = m_nMaxCount;
	UpdateData(false);
}

void CDlgELFRefine::OnEventLButtonDown_Item(WPARAM wParam, LPARAM lParam, AUIObject * pObj)
{
 	CECIvtrItem *pItem = (CECIvtrItem *)m_pImg_Item->GetDataPtr();
 	if( pItem )
 		pItem->Freeze(false);
 	m_pImg_Item->ClearCover();
 	m_pImg_Item->SetHint(GetStringFromTable(7307));
 	m_pImg_Item->SetDataPtr(NULL);
 	m_nSoltItem = -1;
	m_nCount = 0;
	m_nMaxCount = 0;
	UpdateData(false);
//	m_pTxt_Item->Enable(true);
}

void CDlgELFRefine::OnEventLButtonDown_ELF(WPARAM wParam, LPARAM lParam, AUIObject * pObj)
{
 	CECIvtrItem *pELF = (CECIvtrItem *)m_pImg_ELF->GetDataPtr();
 	if( pELF )
 		pELF->Freeze(false);
 	m_pImg_ELF->ClearCover();
 	m_pImg_ELF->SetHint(GetStringFromTable(7306));
 	m_pImg_ELF->SetDataPtr(NULL);
 	m_pTxt_Name->SetText(_AL(""));
	m_pTxt_DescNorm->SetText(_AL(""));
	m_pTxt_DescRefined->SetText(_AL(""));
 	m_nSolt = -1;
}

void CDlgELFRefine::OnEventLButtonDown_Add(WPARAM wParam, LPARAM lParam, AUIObject * pObj)
{
	if (m_nCount < m_nMaxCount)
	{
		m_nCount++;
	}
	UpdateData(false);
}

void CDlgELFRefine::OnEventLButtonDown_Minus(WPARAM wParam, LPARAM lParam, AUIObject * pObj)
{
	if (m_nCount > 1)
	{
		m_nCount--;
	}
	UpdateData(false);
}

void CDlgELFRefine::RefineResult(int result)
{
	m_pTxt_RefineList->AppendText(GetStringFromTable(770 + result));
	m_pTxt_RefineList->ScrollToBottom();
	CECIvtrItem *pItem;
	if( m_nLastSolt != -1 )
	{
		pItem = GetHostPlayer()->GetPack()->GetItem(m_nLastSolt);
		if( pItem )
			SetELF(pItem, m_nLastSolt);
	}
	if( m_nLastSoltItem != -1 )
	{
		pItem = GetHostPlayer()->GetPack()->GetItem(m_nLastSoltItem);
		if( pItem && pItem->GetCount() >= 1 )
			SetItem(pItem, m_nLastSoltItem);
		else
		{
			OnEventLButtonDown_Item(0, 0, NULL);
		}
	}
//	m_pBtn_Confirm->Enable(false);
	m_dwEnalbeTick = GetTickCount();
}

void CDlgELFRefine::OnItemDragDrop(CECIvtrItem* pIvtrSrc, int iSrc, PAUIOBJECT pObjSrc, PAUIOBJECT pObjOver)
{
	// for right-click scene
	if(!pObjOver)
	{
		if(pIvtrSrc->GetClassID() == CECIvtrItem::ICID_GOBLIN)
		{
			pObjOver = this->GetDlgItem("Img_ELF");
		}
		else
		{
			pObjOver = this->GetDlgItem("Img_Item");
		}
	}

	if( pObjOver && 0 == stricmp(pObjOver->GetName(), "Img_ELF") )
	{
		if( pIvtrSrc->GetClassID() == CECIvtrItem::ICID_GOBLIN )
			this->SetELF(pIvtrSrc, iSrc);
	}
	else if( pObjOver && 0 == stricmp(pObjOver->GetName(), "Img_Item") )
	{
		int RefineID = pIvtrSrc->GetTemplateID();
		if( RefineID == ELF_REFINE_TICKET0 || RefineID == ELF_REFINE_TICKET1
			|| RefineID == ELF_REFINE_TICKET2 || RefineID == ELF_REFINE_TICKET3)
			this->SetItem(pIvtrSrc, iSrc);
	}
}