// Filename	: DlgBag.cpp
// Creator	: Xu Wenbin
// Date		: 2009/11/12

#include "DlgBag.h"
#include "DlgInventory.h"
#include "DlgInputNO.h"
#include "Ec_Game.h"
#include "EC_IvtrItem.h"
#include "EC_GameUIMan.h"
#include "EC_Viewport.h"
#include "EC_HostPlayer.h"
#include "EC_Inventory.h"
#include "EC_IvtrFlySword.h"
#include "AFI.h"
#include "AUIDef.h"
#include "EC_DealInventory.h"
#include "EC_GameRun.h"
#include "EC_FixedMsg.h"
#include "EC_GameSession.h"

#define new A_DEBUG_NEW

AUI_BEGIN_COMMAND_MAP(CDlgBag, CDlgBase)
AUI_ON_COMMAND("IDCANCEL", OnCommand_CANCEL)
AUI_END_COMMAND_MAP()

AUI_BEGIN_EVENT_MAP(CDlgBag, CDlgBase)
AUI_ON_EVENT("Item_*", WM_LBUTTONDOWN,   OnLButtonDownItem)
AUI_ON_EVENT("Item_*", WM_LBUTTONDBLCLK, OnEventLButtonDblclk)
AUI_ON_EVENT("Item_*", WM_RBUTTONUP,	 OnEventLButtonDblclk)
AUI_END_EVENT_MAP()

bool CDlgBag::OnInitDialog()
{
	int i;
	char szItem[20];
	PAUIIMAGEPICTURE pCell;
	
	for( i = 0; ; i++ )
	{
		sprintf(szItem, "Item_%02d", i + 1);
		pCell = (PAUIIMAGEPICTURE)GetDlgItem(szItem);
		if( !pCell ) break;
		m_vecImgCell.push_back(pCell);
		m_vecImgBg.push_back((PAUIIMAGEPICTURE)GetDlgItem(AString().Format("Img_Bg%02d", i + 1)));
	}

	RefreshIconName(CECDLGSHOP_PACKMAX);

	return true;
}

bool CDlgBag::Render()
{
	if(!CDlgBase::Render())
		return false;

	GetGameUIMan()->RenderItemCover((PAUIOBJECT*)m_vecImgCell.begin(), m_vecImgCell.size(), NULL);
	return true;
}

bool CDlgBag::Tick()
{
	// Test whether should hide
	// Test whether should show is processed by Inventory
	CDlgInventory *pDlgInventory = GetGameUIMan()->m_pDlgInventory;
	if (pDlgInventory->IsShow())
	{
		// Update size and content
		UpdateBag();
		
		// Align to current inventory
		// (Firstly, align to bottom)
		A3DVIEWPORTPARAM* param = GetGame()->GetViewport()->GetA3DViewport()->GetParam();
		int nAlignX = AUIDIALOG_ALIGN_RIGHT;
		if (pDlgInventory->GetPos().x+pDlgInventory->GetSize().cx+this->GetSize().cx > (int)(param->X+param->Width))
			nAlignX = AUIDIALOG_ALIGN_LEFT;
		this->AlignTo(pDlgInventory, AUIDIALOG_ALIGN_OUTSIDE, nAlignX, AUIDIALOG_ALIGN_INSIDE, AUIDIALOG_ALIGN_BOTTOM);

		// (Secondly, align first item height, 如果扩充背包的行数更多，则最后一行对齐)
		CECInventory *pInventory = GetHostPlayer()->GetPack();
		int nIvtrSize = pInventory->GetSize();
		PAUIIMAGEPICTURE pIvtrFirst = static_cast<PAUIIMAGEPICTURE>(pDlgInventory->GetDlgItem("Item_00"));
		if (pIvtrFirst)
		{
			PAUIIMAGEPICTURE pBagFirst;
			if (nIvtrSize <= 2 * CECDLGSHOP_PACKMAX) {
				pBagFirst= m_vecImgCell[0];
			} else {
				pBagFirst = m_vecImgCell[nIvtrSize - 2 * CECDLGSHOP_PACKMAX];
			}
			
			if (pBagFirst && pBagFirst->IsShow())
			{
				int delta = pIvtrFirst->GetPos().y - pBagFirst->GetPos().y;
				POINT bagPos = GetPos();
				bagPos.y += delta;
				SetPosEx(bagPos.x, bagPos.y);
			}
			// else Win_Bag in wrong state
		}
		// else Win_Inventory and Win_Bag in wrong state
	}
	else
	{
		// Win_Bag is shown or hidden by Win_Inventory or user click
		// In order to minimize code change,
		// Win_Bag test and hide by itself
		OnCommand_CANCEL("");
	}

	return CDlgBase::Tick();
}

void CDlgBag::OnCommand_CANCEL(const char *szCommand)
{
	if (GetGameUIMan()->m_pDlgInventory->IsShow())
		GetGameUIMan()->m_pDlgInventory->OnCommand_CANCEL("");
	Show(false);
}

void CDlgBag::OnLButtonDownItem(WPARAM, LPARAM lParam, AUIObject *pObj)
{
	if (!pObj || !pObj->GetDataPtr("ptr_CECIvtrItem"))
		return;
	
	CECIvtrItem *pItem = (CECIvtrItem *)pObj->GetDataPtr("ptr_CECIvtrItem");

	if (GetGameUIMan()->m_bRepairing)
	{
		if( !pItem->IsEquipment() )
			return;
		
		CECIvtrEquip *pEquip = (CECIvtrEquip *)pItem;
		if( pEquip->GetCurEndurance() >= pEquip->GetMaxEndurance() )
			return;
		
		if(!pItem->IsRepairable())
		{
			GetGameRun()->AddFixedMessage(FIXMSG_ITEM_UNREPAIRABLE);
		}
		else
		{
			GetGameSession()->c2s_CmdNPCSevRepair(IVTRTYPE_PACK,
				atoi(pObj->GetName() + strlen("Item_")), pItem->GetTemplateID());
		}
	}	
	else if(AUI_PRESS(VK_SHIFT) )
	{
		// 按住 SHIFT，向聊天栏输入框添加物品图
		//
		int nSlot = atoi(pObj->GetName() + strlen("Item_"));
		GetGameUIMan()->LinkItem(IVTRTYPE_PACK, nSlot);
	}
	else if (!pItem->IsFrozen())
	{		
		A3DVIEWPORTPARAM *p = m_pA3DEngine->GetActiveViewport()->GetParam();
		
		POINT pt = {GET_X_LPARAM(lParam)-p->X, GET_Y_LPARAM(lParam)-p->Y};
		
		GetGameUIMan()->InvokeDragDrop(this, pObj, pt);
		GetGameUIMan()->PlayItemSound((CECIvtrItem *)pObj->GetDataPtr("ptr_CECIvtrItem"), false);
	}
}

void CDlgBag::OnEventLButtonDblclk(WPARAM wParam, LPARAM lParam, AUIObject *pObj)
{
	if(strstr(pObj->GetName(), "Item_"))
	{
		GetGameUIMan()->m_pDlgInventory->OnEventLButtonDblclkItem(wParam, lParam, pObj);
	}
	else
	{
		GetGameUIMan()->m_pDlgInventory->OnEventLButtonDblclkEquip(wParam, lParam, pObj);
	}
}

void CDlgBag::UpdateBag()
{
	CECInventory *pInventory = GetHostPlayer()->GetPack();
	int nIvtrSize = pInventory->GetSize();
	int start = CECDLGSHOP_PACKMAX;

	int nMaxShow = (int)m_vecImgCell.size();
	PAUIIMAGEPICTURE pLast = NULL;
	for (int i(0); i<nMaxShow; ++ i)
	{
		PAUIIMAGEPICTURE pImage = m_vecImgCell[i];
		PAUIIMAGEPICTURE pBackGround = m_vecImgBg[i];
		int iIvtrIndex = start+i;
		if (iIvtrIndex >= nIvtrSize)
		{
			// Hide
			pImage->SetText(_AL(""));
			pImage->SetDataPtr(NULL);
			pImage->SetCover(NULL, -1);
			pImage->SetColor(A3DCOLORRGB(255, 255, 255));
			pImage->Show(false);
			pBackGround->Show(false);
			continue;
		}

		// Show now
		pImage->Show(true);
		pBackGround->Show(true);
		pLast = pImage;

		// Get Item should be put here
		CECIvtrItem *pItem = pInventory->GetItem(iIvtrIndex);

		// Set default clock icon state
		AUIClockIcon *pClock = pImage->GetClockIcon();
		pClock->SetProgressRange(0, 1);
		pClock->SetProgressPos(1);

		if (!pItem)
		{
			// Empty slot
			pImage->SetText(_AL(""));
			pImage->SetDataPtr(NULL);
			pImage->SetCover(NULL, -1);
			pImage->SetColor(A3DCOLORRGB(255, 255, 255));
			continue;
		}
		
		// Set data
		pImage->SetDataPtr(pItem,"ptr_CECIvtrItem");
		pImage->SetData(CECGameUIMan::ICONS_INVENTORY);

		// Set text
		ACString strText;
		if (pItem->GetCount() > 1)
			strText.Format(_AL("%d"), pItem->GetCount());
		pImage->SetText(strText);
		
		// Set cover
		AString strFile;
		af_GetFileTitle(pItem->GetIconFile(), strFile);
		strFile.MakeLower();		
		pImage->SetCover(
			GetGameUIMan()->m_pA2DSpriteIcons[CECGameUIMan::ICONS_INVENTORY],
			GetGameUIMan()->m_IconMap[CECGameUIMan::ICONS_INVENTORY][strFile]);

		// Set color
		A3DCOLOR clr;
		if (pItem->IsFrozen())
		{
			clr = A3DCOLORRGB(128, 128, 128);
		}
		else if (pItem->IsEquipment())
		{
			if (pItem->GetClassID() == CECIvtrItem::ICID_FASHION)
			{
				clr = A3DCOLORRGB(255, 255, 255);
			}
			else
			{
				CECIvtrEquip *pEquip = static_cast<CECIvtrEquip *>(pItem);
				if (!GetHostPlayer()->CanUseEquipment(pEquip))
				{
					clr = A3DCOLORRGB(192, 0, 0);
				}
				else
				{
					bool bUseOut = false;
					if (pEquip->GetClassID() == CECIvtrItem::ICID_FLYSWORD)
					{
						CECIvtrFlySword *pFlySword = static_cast<CECIvtrFlySword *>(pEquip);
						bUseOut = pFlySword->GetCurTime() ? false : true;
					}
					else if (pEquip->GetClassID() == CECIvtrItem::ICID_WING)
					{
						int iCurMP = GetHostPlayer()->GetBasicProps().iCurMP;
						int iMinMP = GetHostPlayer()->IsFlying() ? 0 : ((CECIvtrWing*)pEquip)->GetLaunchMP();
						bUseOut = iCurMP>=iMinMP ? false : true;
					}
					else if (pEquip->GetClassID() != CECIvtrItem::ICID_ARROW)
					{
						bUseOut = pEquip->GetCurEndurance() ? false : true;
					}
					clr = bUseOut ? A3DCOLORRGB(128, 128, 128) : A3DCOLORRGB(255, 255, 255);
				}
				
				if (pImage->GetColor()==A3DCOLORRGB(255, 255, 255) && pEquip->IsDestroying())
				{
					clr = A3DCOLORRGB(128, 128, 128);
				}
			}
		}
		else
		{
			clr = (pItem->IsUseable()&&!pItem->CheckUseCondition())
				? A3DCOLORRGB(192, 0, 0) : A3DCOLORRGB(255, 255, 255);
		}
		pImage->SetColor(clr);
		
		// Set clock
		int nMax(0);
		if (pItem->GetCoolTime(&nMax)>0)
		{
			pClock->SetProgressRange(0, nMax);
			pClock->SetProgressPos(nMax - pItem->GetCoolTime());
			pClock->SetColor(A3DCOLORRGBA(0, 0, 0, 128));
		}
	}

	// Update size according to
	if (!pLast)
	{
		// Error, should not happen
		m_nHeight = m_pAUIFrameDlg->GetSizeLimit().cy;
	}
	else
	{
		POINT pt = pLast->GetPos(true);
		SIZE s = pLast->GetSize();
		SetSize(GetSize().cx, pt.y + s.cy + m_pAUIFrameDlg->GetSizeLimit().cy / 2);
	}
}

void CDlgBag::RefreshIconName(int start)
{
	// Rename
	char szName[20];
	int i = 0;

	int nMaxShow = (int)m_vecImgCell.size();
	for (i = 0; i<nMaxShow; ++ i)
	{
		// avoid name conflict ...
		sprintf(szName, "Temp_%02d", start+i);
		m_vecImgCell[i]->SetName(szName);
	}
	for (i = 0; i<nMaxShow; ++ i)
	{
		sprintf(szName, "Item_%02d", start+i);
		m_vecImgCell[i]->SetName(szName);
	}
}

bool CDlgBag::OnChangeLayoutBegin()
{
	RefreshIconName(1);
	return true;
}

void CDlgBag::OnChangeLayoutEnd(bool bAllDone)
{
	RefreshIconName(CECDLGSHOP_PACKMAX);
}

void CDlgBag::OnItemDragDrop(CECIvtrItem* pIvtrSrc, int iSrc, PAUIOBJECT pObjSrc, PAUIOBJECT pObjOver)
{
	GetGameUIMan()->m_pDlgInventory->OnItemDragDrop(pIvtrSrc, iSrc, pObjSrc, pObjOver);
}