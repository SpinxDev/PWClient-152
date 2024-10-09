/*
 * FILE: EC_GameUIMan.cpp
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Duyuxin, 2004/8/27
 *
 * HISTORY: 
 *
 * Copyright (c) 2004 Archosaur Studio, All Rights Reserved.
 */

#pragma warning (disable: 4284)

#include "AF.h"
#include "A3DFuncs.h"
#include "A3DCamera.h"
#include "A3DEngine.h"
#include "A3DTexture.h"
#include "A3DViewport.h"

#include "AUI\\CSplit.h"
#include "elementdataman.h"
#include "privilege.hxx"

#include "EC_Global.h"
#include "EC_Game.h"
#include "EC_GameRun.h"
#include "EC_GameSession.h"
#include "EC_GameUIMan.h"
#include "EC_ManPlayer.h"
#include "EC_ManNPC.h"
#include "EC_NPC.h"
#include "EC_HostPlayer.h"
#include "EC_ElsePlayer.h"
#include "EC_DealInventory.h"
#include "EC_IvtrTypes.h"
#include "EC_Inventory.h"
#include "EC_IvtrItem.h"
#include "EC_IvtrEquip.h"
#include "EC_IvtrStone.h"
#include "EC_Resource.h"
#include "EC_Shortcut.h"
#include "EC_ShortcutSet.h"
#include "EC_Skill.h"
#include "EC_Team.h"
#include "EC_Viewport.h"
#include "EC_World.h"
#include "EC_Friend.h"
#include "Task\\EC_TaskInterface.h"

#include "DlgBooth.h"
#include "DlgChat.h"
#include "DlgInputName.h"
#include "DlgInputNO.h"
#include "DlgInventory.h"
#include "DlgMiniMap.h"
#include "DlgNPC.h"
#include "DlgProduce.h"
#include "DlgShop.h"
#include "DlgTask.h"
#include "DlgWorldMap.h"

#define new A_DEBUG_NEW

///////////////////////////////////////////////////////////////////////////
//	
//	Define and Macro
//	
///////////////////////////////////////////////////////////////////////////
#define GetPixelsByPos(x)	int((m_nMinSize * (x)) / (2.0f * 11.0f * 512.0f))
#define GetPosByPixels(x)	(2.0f * 11.0f * 512.0f * float(x) / float(m_nMinSize))
#define MSGBOX(sid) MessageBox("", GetStringFromTable(sid), MB_OK, A3DCOLORRGBA(255, 255, 255, 160))

///////////////////////////////////////////////////////////////////////////
//	
//	Reference to External variables and functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Local Types and Variables and Global variables
//	
///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////
//	
//	Local functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Implement CECGameUIMan
//	
///////////////////////////////////////////////////////////////////////////

//	Handle event
bool CECGameUIMan::OnEvent(UINT uMsg, WPARAM wParam,
	LPARAM lParam, AUIDialog* pDlg, AUIObject* pObj)
{
	if (AUILuaManager::OnEvent(uMsg, wParam, lParam, pDlg, pObj))
		return true;

	CECHostPlayer *pHost = g_pGame->GetGameRun()->GetHostPlayer();
	if( pHost->IsDead() ) return false;
	
	if( 0 == stricmp(pDlg->GetName(), "DragDrop") )
		return OnEvent_DragDrop(uMsg, wParam, lParam, pDlg, pObj);
	if( 0 == stricmp(pDlg->GetName(), "Win_BBSDlg") )
		return m_BBSDlg.OnEvent(uMsg, wParam, lParam, pDlg, pObj);

 	if( GetDialog("Win_Customize") && GetDialog("Win_Customize")->IsShow() )
		return m_Customize.OnEvent_Customize(uMsg, wParam, lParam, pDlg, pObj);

	return false;
}

bool CECGameUIMan::OnEvent_DragDrop(UINT uMsg, WPARAM wParam,
	LPARAM lParam, AUIDialog* pDlg, AUIObject* pObj)
{
	CECHostPlayer *pHost = g_pGame->GetGameRun()->GetHostPlayer();
	CECDealInventory *pBuy = pHost->GetBuyPack();
	CECDealInventory *pSell = pHost->GetSellPack();
	CECDealInventory *pDeal = pHost->GetDealPack();

	if( uMsg == WM_MOUSEMOVE )
	{
		PAUIDIALOG pDlgOver;
		PAUIOBJECT pObjOver;
		A3DVIEWPORTPARAM *p = m_pA3DEngine->GetActiveViewport()->GetParam();
		int x = GET_X_LPARAM(lParam) - p->X;
		int y = GET_Y_LPARAM(lParam) - p->Y;

		HitTest(x, y, &pDlgOver, &pObjOver, pDlg);
		if( !pDlgOver || !pObjOver ) return true;
	}

	if( uMsg != WM_LBUTTONUP ) return true;

	PAUIIMAGEPICTURE pItem = (PAUIIMAGEPICTURE)pDlg->GetDlgItem("Goods");
	PAUIOBJECT pObjSrc = (PAUIOBJECT)pItem->GetDataPtr();
	if( !pObjSrc->GetDataPtr() )
	{
		pDlg->Show(false);
		pItem->ClearCover();
		return true;
	}

	AString strFile;
	PAUIDIALOG pDlgOver;
	PAUIOBJECT pObjOver;
	A3DVIEWPORTPARAM *p = m_pA3DEngine->GetActiveViewport()->GetParam();
	int x = GET_X_LPARAM(lParam) - p->X;
	int y = GET_Y_LPARAM(lParam) - p->Y;
	HitTest(x, y, &pDlgOver, &pObjOver, pDlg);

	int iSrc, iDst;
	PAUIDIALOG pTrade = GetDialog("Win_Trade");
	PAUIDIALOG pDlgSrc = pObjSrc->GetParent();
	CECGameSession *pSession = g_pGame->GetGameSession();
	CECIvtrItem *pIvtrSrc = (CECIvtrItem *)pObjSrc->GetDataPtr();
	
	if( abs(x - m_ptLButtonDown.x) < 3 && abs(y - m_ptLButtonDown.y) < 3 )
	{
		if( strstr(pDlgSrc->GetName(), "Win_Quickbar") ||
			0 == stricmp(pDlgSrc->GetName(), "Win_Action") )
		{
			CECShortcut *pSC = (CECShortcut *)pDlg->GetDataPtr();
			if( pSC ) pSC->Execute();
		}
		else if( 0 == stricmp(pDlgSrc->GetName(), "Win_Skill") )
		{
			CECSkill *pSkill = (CECSkill *)pDlg->GetDataPtr();
			if( pSkill ) pHost->ApplySkillShortcut(pSkill->GetSkillID());
		}

		m_ptLButtonDown.x = m_ptLButtonDown.y = -1;
	}
	else if( pTrade->IsShow() )
	{
		int idItem = pIvtrSrc->GetTemplateID();
		int nCount = pIvtrSrc->GetCount();

		if( 0 == stricmp(pDlgSrc->GetName(), "Win_Inventory") &&
			strstr(pObjSrc->GetName(), "Item_") && pDlgOver )
		{
			iSrc = atoi(pObjSrc->GetName() + strlen("Item_"));
			
			if( 0 == stricmp(pDlgOver->GetName(), "Win_Trade")
				&& pTrade->GetDlgItem("Btn_Lock")->IsShow() )
			{
				if( AUI_PRESS(VK_MENU) && pIvtrSrc->GetPileLimit() > 1 && pIvtrSrc->GetCount() > 1 )
				{
					InvokeNumberDialog(pObjSrc, pObjOver,
						CDlgInputNO::INPUTNO_TRADE_ADD, pIvtrSrc->GetCount());
				}
				else
				{
					pSession->trade_AddGoods(m_idTrade, idItem, iSrc, nCount, 0);
					pIvtrSrc->Freeze(true);
				}
				PlayItemSound(pIvtrSrc, true);
			}
			else if( 0 == stricmp(pDlgOver->GetName(), "Win_Inventory")
				&& pObjOver && strstr(pObjOver->GetName(), "Item_") )
			{
				CECIvtrItem *pIvtrDst = (CECIvtrItem *)pObjOver->GetDataPtr();
				if( !pIvtrDst || !pIvtrDst->IsFrozen() )
				{
					iDst = atoi(pObjOver->GetName() + strlen("Item_"));

					if( AUI_PRESS(VK_MENU) && pIvtrSrc->GetPileLimit() > 1 && pIvtrSrc->GetCount() > 1 )
					{
						int nMax = pIvtrSrc->GetCount();
						if( pIvtrDst && pIvtrSrc->GetPileLimit() - pIvtrDst->GetCount() < nMax )
							nMax = pIvtrSrc->GetPileLimit() - pIvtrDst->GetCount();

						InvokeNumberDialog(pObjSrc, pObjOver,
							CDlgInputNO::INPUTNO_TRADE_MOVE, max(nMax, 0));
					}
					else
					{
						pSession->trade_MoveItem(m_idTrade, idItem, iSrc, iDst, nCount);
					}
					PlayItemSound(pIvtrSrc, true);
				}
			}
		}
		else if( strstr(pObjSrc->GetName(), "I_") &&
			0 == stricmp(pDlgSrc->GetName(), "Win_Trade") )
		{
			if( !pDlgOver || 0 != stricmp(pDlgOver->GetName(), "Win_Trade") )
			{
				iSrc = atoi(pObjSrc->GetName() + strlen("I_")) - 1;
				const CECDealInventory::ITEMINFO &ii = pDeal->GetItemInfo(iSrc);

				pSession->trade_RemoveGoods(m_idTrade,
					idItem, ii.iOrigin, ii.iAmount, 0);
				pIvtrSrc->Freeze(false);
				PlayItemSound(pIvtrSrc, true);
			}
		}
	}
	else if( 0 == stricmp(pDlgSrc->GetName(), "Win_Inventory") )
	{
		if( strstr(pObjSrc->GetName(), "Item_") )
		{
			iSrc = atoi(pObjSrc->GetName() + strlen("Item_"));
			if( !pDlgOver )		// Drop item.
			{
				if( AUI_PRESS(VK_MENU) && pIvtrSrc->GetPileLimit() > 1 && pIvtrSrc->GetCount() > 1 )
				{
					InvokeNumberDialog(pObjSrc, NULL,
						CDlgInputNO::INPUTNO_DROP_ITEM, pIvtrSrc->GetCount());
				}
				else
				{
					pSession->c2s_CmdDropIvtrItem(iSrc, pIvtrSrc->GetCount());
				}
				PlayItemSound(pIvtrSrc, true);
			}
			else if( 0 == stricmp(pDlgOver->GetName(), "Win_Inventory") && pObjOver )
			{
				if( strstr(pObjOver->GetName(), "Item_") )
				{
					iDst = atoi(pObjOver->GetName() + strlen("Item_"));
					if( iSrc != iDst )
					{
						CECIvtrItem *pIvtrDst = (CECIvtrItem *)pObjOver->GetDataPtr();
						if( pIvtrSrc && pIvtrSrc->GetClassID() == CECIvtrItem::ICID_ELEMENT &&
							pIvtrDst && pIvtrDst->GetClassID() == CECIvtrItem::ICID_FLYSWORD )
						{
							pSession->c2s_CmdChargeFlySword(iSrc, iDst,
								pIvtrSrc->GetCount(), pIvtrDst->GetTemplateID());
							PlayItemSound(pIvtrSrc, true);
						}
						else if( !pIvtrDst || !pIvtrDst->IsFrozen() )
						{
							if( pIvtrSrc->GetPileLimit() > 1 && (!pIvtrDst ||
								pIvtrSrc->GetTemplateID() == pIvtrDst->GetTemplateID()) )
							{
								int nMax = pIvtrSrc->GetCount();
								if( pIvtrDst && pIvtrSrc->GetPileLimit() - pIvtrDst->GetCount() < nMax )
									nMax = pIvtrSrc->GetPileLimit() - pIvtrDst->GetCount();

								if( AUI_PRESS(VK_MENU) && nMax > 1 )
								{
									InvokeNumberDialog(pObjSrc, pObjOver,
										CDlgInputNO::INPUTNO_MOVE_ITEM, max(nMax, 0));
								}
								else if( pIvtrDst && pIvtrSrc->GetTemplateID() == pIvtrDst->GetTemplateID() )
									pSession->c2s_CmdMoveIvtrItem(iSrc, iDst, nMax);
								else
									pSession->c2s_CmdExgIvtrItem(iSrc, iDst);
							}
							else
							{
								pSession->c2s_CmdExgIvtrItem(iSrc, iDst);
							}
							PlayItemSound(pIvtrSrc, true);
						}
					}
				}
				else if( strstr(pObjOver->GetName(), "Equip_") )
				{
					CECIvtrItem *pIvtrDst = (CECIvtrItem *)pObjOver->GetDataPtr();

					iDst = atoi(pObjOver->GetName() + strlen("Equip_"));
					
					if( pIvtrSrc && pIvtrSrc->GetClassID() == CECIvtrItem::ICID_ELEMENT &&
						pIvtrDst && pIvtrDst->GetClassID() == CECIvtrItem::ICID_FLYSWORD )
					{
						pSession->c2s_CmdChargeEquipFlySword(iSrc, pIvtrSrc->GetCount());
					}
					else if( pIvtrDst && pIvtrSrc->GetPileLimit() > 1 &&
						pIvtrSrc->GetTemplateID() == pIvtrDst->GetTemplateID() )
					{
						pSession->c2s_CmdMoveItemToEquip(iSrc, iDst);
					}
					else
					{
						pSession->c2s_CmdEquipItem(iSrc, iDst);
					}
					PlayItemSound(pIvtrSrc, true);
				}
			}
			else if( strstr(pDlgOver->GetName(), "Win_Quickbar")
				&& pObjOver && strstr(pObjOver->GetName(), "Item_") )
			{
				int iSlot = atoi(pObjOver->GetName() + strlen("Item_"));
				CECShortcutSet *pSCS = GetSCSByDlg(pDlgOver->GetName());
				
				pSCS->CreateItemShortcut(iSlot - 1, IVTRTYPE_PACK, iSrc, pIvtrSrc);
				PlayItemSound(pIvtrSrc, true);
			}
			else if( 0 == stricmp(pDlgOver->GetName(), "Win_Shop")
				&& pObjOver && strstr(pObjOver->GetName(), "Sell_") )
			{
				if( AUI_PRESS(VK_MENU) && pIvtrSrc->GetCount() > 1 && pIvtrSrc->GetPileLimit() > 1 )
				{
					InvokeNumberDialog(pObjSrc, NULL,
						CDlgInputNO::INPUTNO_SELL_ADD, pIvtrSrc->GetCount());
					PlayItemSound(pIvtrSrc, true);
				}
				else if( pSell->SearchEmpty() >= 0 )
				{
					pSell->AddItem(pIvtrSrc, iSrc, pIvtrSrc->GetCount());
					pIvtrSrc->Freeze(true);
					PlayItemSound(pIvtrSrc, true);
				}
			}
			else if( (0 == stricmp(pDlgOver->GetName(), "Win_Enchase")
				|| 0 == stricmp(pDlgOver->GetName(), "Win_Disenchase"))
				&& pObjOver && strstr(pObjOver->GetName(), "Item_") )
			{
				pIvtrSrc->Freeze(true);
				pObjOver->SetData(iSrc);
				pObjOver->SetDataPtr(pObjSrc->GetDataPtr());
				PlayItemSound(pIvtrSrc, true);

				af_GetFileTitle(pIvtrSrc->GetIconFile(), strFile);
				PAUIIMAGEPICTURE pImage = (PAUIIMAGEPICTURE)pObjOver;
				pImage->SetCover(m_pA2DSpriteIcons[ICONS_INVENTORY],
					m_IconMap[ICONS_INVENTORY][strFile]);

				CECIvtrStone *pStone;
				STONE_ESSENCE *pEssence;
				ACHAR szText[40] = _AL("0");

				if( 0 == stricmp(pDlgOver->GetName(), "Win_Enchase")
					&& 0 == stricmp(pObjOver->GetName(), "Item_b")
					&& pIvtrSrc->GetClassID() == CECIvtrItem::ICID_STONE )
				{
					pStone = (CECIvtrStone *)pIvtrSrc;
					pEssence = (STONE_ESSENCE *)pStone->GetDBEssence();
					a_sprintf(szText, _AL("%d"), pEssence->install_price);
					pDlgOver->GetDlgItem("Txt_Gold")->SetText(szText);
				}
				else if( 0 == stricmp(pDlgOver->GetName(), "Win_Disenchase")
					&& pIvtrSrc->IsEquipment() )
				{
					int i, nAmount = 0, idItem;
					CECIvtrEquip *pEquip = (CECIvtrEquip *)pIvtrSrc;

					for( i = 0; i < pEquip->GetHoleNum(); i++ )
					{
						idItem = pEquip->GetHoleItem(i);
						if( idItem <= 0 ) continue;

						pStone = (CECIvtrStone *)CECIvtrItem::CreateItem(idItem, 1);
						pEssence = (STONE_ESSENCE *)pStone->GetDBEssence();
						nAmount += pEssence->uninstall_price;
						delete pStone;
					}

					a_sprintf(szText, _AL("%d"), nAmount);
					pDlgOver->GetDlgItem("Txt_Gold")->SetText(szText);
				}
			}
			else if( 0 == stricmp(pDlgOver->GetName(), "Win_Split")	&&
				pObjOver && 0 == stricmp(pObjOver->GetName(), "Item_a") &&
				((PAUIPROGRESS)pDlgOver->GetDlgItem("Prgs_1"))->GetProgress() <= 0 )
			{
				pObjOver->SetData(iSrc);
				pObjOver->SetDataPtr(pObjSrc->GetDataPtr());
				PlayItemSound(pIvtrSrc, true);

				af_GetFileTitle(pIvtrSrc->GetIconFile(), strFile);
				PAUIIMAGEPICTURE pImage = (PAUIIMAGEPICTURE)pObjOver;
				pImage->SetCover(m_pA2DSpriteIcons[ICONS_INVENTORY],
					m_IconMap[ICONS_INVENTORY][strFile]);

				pHost->GetPack(IVTRTYPE_PACK)->UnfreezeAllItems();
				pIvtrSrc->Freeze(true);

				bool bCan = true;
				DATA_TYPE DataType;
				int idSkillNeed = -1, nLevelNeed = 0;
				CECSkill *pSkill, *pSkillThis = NULL;
				elementdataman *pDataMan = g_pGame->GetElementDataMan();
				int i, uiID, nItem = -1, nTime = 0, idItem = 0, nNumItem = 0;
				ACHAR szText[40], szPrice[40] = _AL("0"), szNumber[40] = _AL("0");

				pDlgOver->GetDlgItem("Txt_SkillName")->SetText(_AL(""));
				pDlgOver->GetDlgItem("Txt_SkillLevel")->SetText(_AL(""));
				for( i = 0; i < (int)pDataMan->get_data_num(ID_SPACE_RECIPE); i++ )
				{
					uiID = pDataMan->get_data_id(ID_SPACE_RECIPE, i, DataType);
					if( DataType != DT_RECIPE_ESSENCE ) continue;

					const RECIPE_ESSENCE *pRecipe = (const RECIPE_ESSENCE *)
						pDataMan->get_data_ptr(uiID, ID_SPACE_RECIPE, DataType);
					if( (int)pRecipe->id_to_make != pIvtrSrc->GetTemplateID() )
						continue;

					pDlgOver->GetDlgItem("Txt_SkillName")->SetText(
						GNET::ElementSkill::GetName(pRecipe->id_skill));
					a_sprintf(szText, _AL("%d"), pRecipe->skill_level);
					pDlgOver->GetDlgItem("Txt_SkillLevel")->SetText(szText);
					idSkillNeed = pRecipe->id_skill;
					nLevelNeed = pRecipe->skill_level;

					break;
				}

				for( i = 0; i < pHost->GetPassiveSkillNum(); i++ )
				{
					pSkill = pHost->GetPassiveSkillByIndex(i);
					if( (pSkill->GetType() == CECSkill::TYPE_LIVE ||
						pSkill->GetType() == CECSkill::TYPE_PRODUCE) &&
						(!pSkillThis || idSkillNeed == pSkill->GetSkillID()) )
					{
						pSkillThis = pSkill;
					}
				}
				if( idSkillNeed > 0 )
				{
					if( pSkillThis )
					{
						if( pSkillThis->GetSkillID() == idSkillNeed )
							pDlgOver->GetDlgItem("Txt_SkillName")->SetColor(A3DCOLORRGB(255, 255, 255));
						else
						{
							bCan = false;
							pDlgOver->GetDlgItem("Txt_SkillName")->SetColor(A3DCOLORRGB(128, 128, 128));
						}
						
						if( pSkillThis->GetSkillLevel() >= nLevelNeed )
							pDlgOver->GetDlgItem("Txt_SkillLevel")->SetColor(A3DCOLORRGB(255, 255, 255));
						else
						{
							bCan = false;
							pDlgOver->GetDlgItem("Txt_SkillLevel")->SetColor(A3DCOLORRGB(128, 128, 128));
						}

						pDlgOver->GetDlgItem("Txt_CurrentSkillName")->SetText(
							GNET::ElementSkill::GetName(pSkillThis->GetSkillID()));
						a_sprintf(szText, _AL("%d"), pSkillThis->GetSkillLevel());
						pDlgOver->GetDlgItem("Txt_CurrentSkillLevel")->SetText(szText);
					}
					else
					{
						bCan = false;
						pDlgOver->GetDlgItem("Txt_CurrentSkillName")->SetText(_AL(""));
						pDlgOver->GetDlgItem("Txt_CurrentSkillLevel")->SetText(_AL(""));
						pDlgOver->GetDlgItem("Txt_SkillName")->SetColor(A3DCOLORRGB(128, 128, 128));
						pDlgOver->GetDlgItem("Txt_SkillLevel")->SetColor(A3DCOLORRGB(128, 128, 128));
					}
				}
				else
				{
					pDlgOver->GetDlgItem("Txt_CurrentSkillName")->SetText(_AL(""));
					pDlgOver->GetDlgItem("Txt_CurrentSkillLevel")->SetText(_AL(""));
					pDlgOver->GetDlgItem("Txt_SkillName")->SetColor(A3DCOLORRGB(128, 128, 128));
					pDlgOver->GetDlgItem("Txt_SkillLevel")->SetColor(A3DCOLORRGB(128, 128, 128));
				}

				CECIvtrEquip *pEquip = (CECIvtrEquip *)pIvtrSrc;
				const void *pData = pDataMan->get_data_ptr(
					pIvtrSrc->GetTemplateID(), ID_SPACE_ESSENCE, DataType);
				if( DataType == DT_WEAPON_ESSENCE )
				{
					__int64 nCur = pEquip->GetCurEndurance();
					__int64 nMax = pEquip->GetMaxEndurance();
					WEAPON_ESSENCE *pEssence = (WEAPON_ESSENCE *)pData;
					
					a_sprintf(szPrice, _AL("%d"), pEssence->decompose_price);
					nNumItem = int(pEssence->element_num * nCur / nMax);
					a_sprintf(szNumber, _AL("%d"), nNumItem);
					idItem = pEssence->element_id;
				}
				else if( DataType == DT_ARMOR_ESSENCE )
				{
					__int64 nCur = pEquip->GetCurEndurance();
					__int64 nMax = pEquip->GetMaxEndurance();
					ARMOR_ESSENCE *pEssence = (ARMOR_ESSENCE *)pData;
					
					a_sprintf(szPrice, _AL("%d"), pEssence->decompose_price);
					nNumItem = int(pEssence->element_num * nCur / nMax);
					a_sprintf(szNumber, _AL("%d"), nNumItem);
					idItem = pEssence->element_id;
				}
				else if( DataType == DT_DECORATION_ESSENCE )
				{
					__int64 nCur = pEquip->GetCurEndurance();
					__int64 nMax = pEquip->GetMaxEndurance();
					DECORATION_ESSENCE *pEssence = (DECORATION_ESSENCE *)pData;
					
					a_sprintf(szPrice, _AL("%d"), pEssence->decompose_price);
					nNumItem = int(pEssence->element_num * nCur / nMax);
					a_sprintf(szNumber, _AL("%d"), nNumItem);
					idItem = pEssence->element_id;
				}
				if( a_atoi(szPrice) > pHost->GetMoneyAmount() )
					bCan = false;

				A2DSprite *pCover;
				if( idItem > 0 && nNumItem > 0 )
				{
					const void *pData = pDataMan->get_data_ptr(
						idItem, ID_SPACE_ESSENCE, DataType);
					ELEMENT_ESSENCE *pElement = (ELEMENT_ESSENCE *)pData;

					af_GetFileTitle(pElement->file_icon, strFile);
					nItem = m_IconMap[ICONS_INVENTORY][strFile];
					pCover = m_pA2DSpriteIcons[ICONS_INVENTORY];
					pDlgOver->GetDlgItem("Btn_Start")->Enable(bCan);
				}
				else
				{
					pCover = NULL;
					pDlgOver->GetDlgItem("Btn_Start")->Enable(false);
				}

				pImage = (PAUIIMAGEPICTURE)pDlgOver->GetDlgItem("Item_b");
				pImage->SetCover(pCover, nItem);

				pDlgOver->GetDlgItem("Txt_Gold")->SetText(szPrice);
				pDlgOver->GetDlgItem("Txt_no2")->SetText(szNumber);
			}
			else if( 0 == stricmp(pDlgOver->GetName(), "Win_Identify")
				&& pObjOver && 0 == stricmp(pObjOver->GetName(), "Item") )
			{
				pObjOver->SetData(iSrc);
				pObjOver->SetDataPtr(pObjSrc->GetDataPtr());
				PlayItemSound(pIvtrSrc, true);

				af_GetFileTitle(pIvtrSrc->GetIconFile(), strFile);
				PAUIIMAGEPICTURE pImage = (PAUIIMAGEPICTURE)pObjOver;
				pImage->SetCover(m_pA2DSpriteIcons[ICONS_INVENTORY],
					m_IconMap[ICONS_INVENTORY][strFile]);
				pDlgOver->GetDlgItem("Txt")->SetText(pIvtrSrc->GetName());

				pHost->GetPack(IVTRTYPE_PACK)->UnfreezeAllItems();
				pIvtrSrc->Freeze(true);

				ACHAR szPrice[40];
				DATA_TYPE DataType;
				int idSevice = m_pCurNPCEssence->id_identify_service;
				elementdataman *pDataMan = g_pGame->GetElementDataMan();
				NPC_IDENTIFY_SERVICE *pService = (NPC_IDENTIFY_SERVICE *)
					pDataMan->get_data_ptr(idSevice, ID_SPACE_ESSENCE, DataType);

				a_sprintf(szPrice, _AL("%d"), pService->fee);
				pDlgOver->GetDlgItem("Txt_Gold")->SetText(szPrice);
				pDlgOver->GetDlgItem("Btn_Confirm")->Enable(true);
			}
			else if( 0 == stricmp(pDlgOver->GetName(), "Win_Storage")
				&& pObjOver && strstr(pObjOver->GetName(), "Item_") )
			{
				CECIvtrItem *pIvtrDst = (CECIvtrItem *)pObjOver->GetDataPtr();

				iDst = atoi(pObjOver->GetName() + strlen("Item_")) - 1;
				if( pIvtrDst )
				{
					if( pIvtrSrc->GetPileLimit() > 1 &&
						pIvtrSrc->GetTemplateID() == pIvtrDst->GetTemplateID() )
					{
						if( AUI_PRESS(VK_MENU) && pIvtrSrc->GetCount() > 1 )
						{
							InvokeNumberDialog(pObjSrc, pObjOver,
								CDlgInputNO::INPUTNO_STORAGE_PUT_ITEMS, pIvtrSrc->GetCount());
						}
						else
						{
							pSession->c2s_CmdMoveIvtrToTrashBox(iDst, iSrc,
								min(pIvtrSrc->GetCount(), pIvtrSrc->GetPileLimit() - pIvtrSrc->GetCount()));
						}
					}
					else
						pSession->c2s_CmdExgTrashBoxIvtrItem(iDst, iSrc);
				}
				else
				{
					if( AUI_PRESS(VK_MENU) && pIvtrSrc->GetCount() > 1 && pIvtrSrc->GetPileLimit() > 1 )
					{
						InvokeNumberDialog(pObjSrc, pObjOver,
							CDlgInputNO::INPUTNO_STORAGE_PUT_ITEMS, pIvtrSrc->GetCount());
					}
					else
						pSession->c2s_CmdExgTrashBoxIvtrItem(iDst, iSrc);
				}
				PlayItemSound(pIvtrSrc, true);
			}
			else if( 0 == stricmp(pDlgOver->GetName(), "Win_PShop1") &&
				pDlgOver->GetDlgItem("DEFAULT_Txt_Name")->IsEnabled() )
			{
				ACHAR szText[40];
				PAUIEDITBOX pEdit;
				PAUIDIALOG pShow = GetDialog("Win_InputNO2");

				pEdit = (PAUIEDITBOX)pShow->GetDlgItem("Txt_Price");
				pEdit->SetIsNumberOnly(true);
				a_sprintf(szText, _AL("%d"), pIvtrSrc->GetUnitPrice());
				pEdit->SetText(szText);
				pEdit->SetData(iSrc);
				pShow->ChangeFocus(pEdit);

				pEdit = (PAUIEDITBOX)pShow->GetDlgItem("Txt_Amount");
				pEdit->SetIsNumberOnly(true);
				pEdit->SetText(_AL("1"));
				pEdit->Enable(pIvtrSrc->GetPileLimit() > 0 ? true : false);

				if( pObjOver && strstr(pObjOver->GetName(), "BuyItem_") )
					pShow->SetData(CDlgInputNO::INPUTNO_BUY_ADD);
				else
					pShow->SetData(CDlgInputNO::INPUTNO_SELL_ADD);
				pShow->SetDataPtr(pIvtrSrc);
				pShow->Show(true, true);
			}
			else if( 0 == stricmp(pDlgOver->GetName(), "Win_PShop2") && pObjOver &&
				pObjOver->GetDataPtr() && strstr(pObjOver->GetName(), "BuyItem_") )
			{
				CECIvtrItem *pIvtrDst = (CECIvtrItem *)pObjOver->GetDataPtr();
				if( pIvtrSrc->GetTemplateID() != pIvtrDst->GetTemplateID() )
				{
					pDlg->Show(false);
					pItem->ClearCover();
					return true;
				}

				if( pIvtrSrc->GetPileLimit() > 1 && pIvtrDst->GetCount() > 1 )
				{
					PAUIOBJECT pEdit = m_pDlgInputNO->GetDlgItem("DEFAULT_Txt_No.");

					pEdit->SetData(min(pIvtrSrc->GetCount(), pIvtrDst->GetCount()));
					pEdit->SetText(_AL("1"));

					iDst = atoi(pObjOver->GetName() + strlen("BuyItem_")) - 1;
					m_pDlgInputNO->SetData((DWORD)pObjOver);
					m_pDlgInputNO->SetDataPtr(pObjSrc);
					m_pDlgInputNO->Show(true, true);
					m_pDlgInputNO->SetType(CDlgInputNO::INPUTNO_SELL_ADD);
				}
				else
				{
					CECDealInventory *pSell = pHost->GetSellPack();
					CECDealInventory *pBuy = pHost->GetBoothBuyPack();
					iDst = atoi(pObjOver->GetName() + strlen("BuyItem_")) - 1;
					int iOrigin = pBuy->GetItemInfo(iDst).iOrigin;
					int nIndex = pSell->AddBoothItem(pIvtrSrc,
						iOrigin, 1, pIvtrDst->GetUnitPrice());

					pIvtrSrc->Freeze(true);
					pBuy->GetItem(iDst)->Freeze(true);
					pSell->SetItemInfoFlag(nIndex, iSrc);
				}
			}
		}
		else if( strstr(pObjSrc->GetName(), "Equip_") )
		{
			iSrc = atoi(pObjSrc->GetName() + strlen("Equip_"));
			if( !pDlgOver )		// Drop item.
				pSession->c2s_CmdDropEquipItem(iSrc);
			else if( 0 == stricmp(pDlgOver->GetName(), "Win_Inventory") && pObjOver )
			{
				CECIvtrItem *pIvtrDst = (CECIvtrItem *)pObjOver->GetDataPtr();
				if( !pIvtrDst || !pIvtrDst->IsFrozen() )
				{
					if( strstr(pObjOver->GetName(), "Item_") )
					{
						iDst = atoi(pObjOver->GetName() + strlen("Item_"));
						pSession->c2s_CmdEquipItem(iDst, iSrc);
						PlayItemSound(pIvtrSrc, true);
					}
					else if( strstr(pObjOver->GetName(), "Equip_") )
					{
						iDst = atoi(pObjOver->GetName() + strlen("Equip_"));
						if( iSrc != iDst ) pSession->c2s_CmdExgEquipItem(iSrc, iDst);
						PlayItemSound(pIvtrSrc, true);
					}
				}
			}
			else if( strstr(pDlgOver->GetName(), "Win_Quickbar")
				&& pObjOver && strstr(pObjOver->GetName(), "Item_") )
			{
				int iSlot = atoi(pObjOver->GetName() + strlen("Item_"));
				CECShortcutSet *pSCS = GetSCSByDlg(pDlgOver->GetName());
				
				pSCS->CreateItemShortcut(iSlot - 1, IVTRTYPE_EQUIPPACK, iSrc, pIvtrSrc);
				PlayItemSound(pIvtrSrc, true);
			}
			else if( (0 == stricmp(pDlgOver->GetName(), "Win_Shop")
				&& pObjOver && strstr(pObjOver->GetName(), "Sell_"))
				|| (0 == stricmp(pDlgOver->GetName(), "Win_PShop2")
				&& pObjOver && strstr(pObjOver->GetName(), "BuyItem_")) )
			{
				MSGBOX(572);
			}
		}
	}
	else if( 0 == stricmp(pDlgSrc->GetName(), "Win_Action") )
	{
		if( pDlgOver && strstr(pDlgOver->GetName(), "Win_Quickbar")
			&& pObjOver && strstr(pObjOver->GetName(), "Item_") )
		{
			CECShortcut *pSC = (CECShortcut *)pObjSrc->GetDataPtr();
			int iSlot = atoi(pObjOver->GetName() + strlen("Item_"));
			CECShortcutSet *pSCS = GetSCSByDlg(pDlgOver->GetName());
			
			pSCS->CreateClonedShortcut(iSlot - 1, pSC);
		}
	}
	else if( 0 == stricmp(pDlgSrc->GetName(), "Win_Skill") )
	{
		if( pDlgOver && strstr(pDlgOver->GetName(), "Win_Quickbar")
			&& pObjOver && strstr(pObjOver->GetName(), "Item_") )
		{
			CECSkill *pSkill = (CECSkill *)pObjSrc->GetDataPtr();
			int iSlot = atoi(pObjOver->GetName() + strlen("Item_"));
			CECShortcutSet *pSCS = GetSCSByDlg(pDlgOver->GetName());
			
			pSCS->CreateSkillShortcut(iSlot - 1, pSkill);
		}
	}
	else if( strstr(pDlgSrc->GetName(), "Win_Quickbar") )
	{
		if( !pDlgOver )		// Remove it.
		{
			int iSlot = atoi(pObjSrc->GetName() + strlen("Item_"));
			CECShortcutSet *pSCS = GetSCSByDlg(pDlgSrc->GetName());
			
			pSCS->SetShortcut(iSlot - 1, NULL);
		}
		else if( strstr(pDlgOver->GetName(), "Win_Quickbar")
			&& pObjOver && strstr(pObjOver->GetName(), "Item_") )
		{
			int iSlotSrc = atoi(pObjSrc->GetName() + strlen("Item_"));
			int iSlotDst = atoi(pObjOver->GetName() + strlen("Item_"));
			CECShortcutSet *pSCSSrc = GetSCSByDlg(pDlgSrc->GetName());
			CECShortcutSet *pSCSDst = GetSCSByDlg(pDlgOver->GetName());
			CECShortcut *pSCSrc = pSCSSrc->GetShortcut(iSlotSrc - 1, true);
			CECShortcut *pSCDst = pSCSDst->GetShortcut(iSlotDst - 1, true);

			pSCSSrc->SetShortcut(iSlotSrc - 1, pSCDst);
			pSCSDst->SetShortcut(iSlotDst - 1, pSCSrc);
		}
	}
	else if( 0 == stricmp(pDlgSrc->GetName(), "Win_Shop") )
	{
		if( strstr(pObjSrc->GetName(), "U_") &&
			pObjOver && strstr(pObjOver->GetName(), "Buy_") )
		{
			m_pDlgShop->OnEventLButtonDblclk(0, 0, pObjSrc);
			PlayItemSound(pIvtrSrc, true);
		}
		else if( strstr(pObjSrc->GetName(), "Buy_") || strstr(pObjSrc->GetName(), "Sell_") )
		{
			if( !pDlgOver || (pObjOver && strstr(pObjOver->GetName(), "U_")
				&& 0 == stricmp(pDlgOver->GetName(), "Win_Shop")) || (pDlgOver
				&& 0 == stricmp(pDlgOver->GetName(), "Win_Inventory")) )
			{
				m_pDlgShop->OnEventLButtonDblclk(0, 0, pObjSrc);
				PlayItemSound(pIvtrSrc, true);
			}
		}
	}
	else if( 0 == stricmp(pDlgSrc->GetName(), "Win_Storage") &&
		pDlgOver && pObjOver && strstr(pObjOver->GetName(), "Item_") )
	{
		iSrc = atoi(pObjSrc->GetName() + strlen("Item_")) - 1;
		if( 0 == stricmp(pDlgOver->GetName(), "Win_Storage") )
		{
			CECIvtrItem *pIvtrDst = (CECIvtrItem *)pObjOver->GetDataPtr();

			iDst = atoi(pObjOver->GetName() + strlen("Item_")) - 1;
			if( iSrc != iDst )
			{
				if( pIvtrDst )
				{
					if( pIvtrSrc->GetPileLimit() > 1 &&
						pIvtrSrc->GetTemplateID() == pIvtrDst->GetTemplateID() )
					{
						if( AUI_PRESS(VK_MENU) && pIvtrSrc->GetCount() > 1 )
						{
							InvokeNumberDialog(pObjSrc, pObjOver,
								CDlgInputNO::INPUTNO_STORAGE_MOVE_ITEMS, pIvtrSrc->GetCount());
						}
						else
						{
							pSession->c2s_CmdMoveTrashBoxItem(iSrc, iDst,
								min(pIvtrSrc->GetCount(), pIvtrSrc->GetPileLimit() - pIvtrSrc->GetCount()));
						}
					}
					else
						pSession->c2s_CmdExgTrashBoxItem(iSrc, iDst);
				}
				else
				{
					if( AUI_PRESS(VK_MENU) && pIvtrSrc->GetCount() > 1 && pIvtrSrc->GetPileLimit() > 1 )
					{
						InvokeNumberDialog(pObjSrc, pObjOver,
							CDlgInputNO::INPUTNO_STORAGE_MOVE_ITEMS, pIvtrSrc->GetCount());
					}
					else
						pSession->c2s_CmdExgTrashBoxItem(iSrc, iDst);
				}
				PlayItemSound(pIvtrSrc, true);
			}
		}
		else if( 0 == stricmp(pDlgOver->GetName(), "Win_Inventory") &&
			m_pDlgInventory->GetShowItem() == CDlgInventory::INVENTORY_ITEM_NORMAL )
		{
			CECIvtrItem *pIvtrDst = (CECIvtrItem *)pObjOver->GetDataPtr();

			iDst = atoi(pObjOver->GetName() + strlen("Item_"));
			if( pIvtrDst )
			{
				if( AUI_PRESS(VK_MENU) && 
					pIvtrSrc->GetTemplateID() == pIvtrDst->GetTemplateID() &&
					pIvtrSrc->GetCount() > 1 &&	pIvtrSrc->GetPileLimit() > 1 )
				{
					InvokeNumberDialog(pObjSrc, pObjOver,
						CDlgInputNO::INPUTNO_STORAGE_GET_ITEMS, pIvtrSrc->GetCount());
				}
				else
					pSession->c2s_CmdExgTrashBoxIvtrItem(iSrc, iDst);
			}
			else
			{
				if( AUI_PRESS(VK_MENU) && pIvtrSrc->GetCount() > 1 &&	pIvtrSrc->GetPileLimit() > 1 )
				{
					InvokeNumberDialog(pObjSrc, pObjOver,
						CDlgInputNO::INPUTNO_STORAGE_GET_ITEMS, pIvtrSrc->GetCount());
				}
				else
					pSession->c2s_CmdExgTrashBoxIvtrItem(iSrc, iDst);
			}
			PlayItemSound(pIvtrSrc, true);
		}
	}
	else if( 0 == stricmp(pDlgSrc->GetName(), "Win_Split") )
	{
		if( !pDlgOver || 0 != stricmp(pDlgOver->GetName(), "Win_Split") ||
			!pObjOver || 0 != stricmp(pObjOver->GetName(), "Item_a")  )
		{
			PAUIIMAGEPICTURE pImage;

			pImage = (PAUIIMAGEPICTURE)pDlgSrc->GetDlgItem("Item_a");
			pImage->ClearCover();
			pImage->SetData(0);
			pImage->SetDataPtr(NULL);

			pImage = (PAUIIMAGEPICTURE)pDlgSrc->GetDlgItem("Item_b");
			pImage->ClearCover();
			pImage->SetData(0);
			pImage->SetDataPtr(NULL);

			pDlgSrc->GetDlgItem("Txt_no1")->SetText(_AL("0"));
			pDlgSrc->GetDlgItem("Txt_no2")->SetText(_AL("0"));
			pDlgSrc->GetDlgItem("Txt_Gold")->SetText(_AL("0"));
			pDlgSrc->GetDlgItem("Txt_SkillName")->SetText(_AL(""));
			pDlgSrc->GetDlgItem("Txt_SkillLevel")->SetText(_AL(""));

			pHost->GetPack(IVTRTYPE_PACK)->UnfreezeAllItems();
			PlayItemSound(pIvtrSrc, true);
		}
	}
	else if( 0 == stricmp(pDlgSrc->GetName(), "Win_Identify") )
	{
		if( !pDlgOver || 0 != stricmp(pDlgOver->GetName(), "Win_Split") ||
			!pObjOver || 0 != stricmp(pObjOver->GetName(), "Item")  )
		{
			PAUIIMAGEPICTURE pImage;

			pImage = (PAUIIMAGEPICTURE)pDlgSrc->GetDlgItem("Item");
			pImage->ClearCover();
			pImage->SetData(0);
			pImage->SetDataPtr(NULL);

			pDlgSrc->GetDlgItem("Txt_Gold")->SetText(_AL("0"));

			pHost->GetPack(IVTRTYPE_PACK)->UnfreezeAllItems();
			PlayItemSound(pIvtrSrc, true);
		}
	}
	else if( 0 == stricmp(pDlgSrc->GetName(), "Win_PShop2") )
	{
		if( strstr(pObjSrc->GetName(), "Item_") &&
			pObjOver && strstr(pObjOver->GetName(), "Buy_") )
		{
			m_pDlgBooth2->OnEventLButtonDblclk_Buy(0, 0, pObjSrc);
			PlayItemSound(pIvtrSrc, true);
		}
		else if( strstr(pObjSrc->GetName(), "Buy_") || strstr(pObjSrc->GetName(), "Sell_") )
		{
			if( !pDlgOver || (pObjOver && strstr(pObjOver->GetName(), "Item_")
				&& 0 == stricmp(pDlgOver->GetName(), "Win_PShop2")) || (pDlgOver
				&& 0 == stricmp(pDlgOver->GetName(), "Win_Inventory")) )
			{
				m_pDlgBooth2->OnEventLButtonDblclk_Buy(0, 0, pObjSrc);
				PlayItemSound(pIvtrSrc, true);
			}
		}
	}

	pDlg->Show(false);
	pItem->ClearCover();

	return true;
}
