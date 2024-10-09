// Filename	: DlgActivity.h
// Creator	: Xu Wenbin
// Date		: 2011/4/28

#include "AFI.h"
#include "DlgEngrave.h"
#include "EC_GameUIMan.h"
#include "EC_GameSession.h"
#include "ExpTypes.h"
#include "EC_GPDataType.h"
#include "auto_delete.h"
#include "EC_HostPlayer.h"
#include "EC_Inventory.h"
#include "EC_IvtrItem.h"
#include "itemdataman.h"
#include "elementdataman.h"
#include "EC_FixedMsg.h"
#include "EC_IvtrEquip.h"
#include "EC_ShortcutMgr.h"

#include <AUICTranslate.h>

#define new A_DEBUG_NEW

AUI_BEGIN_COMMAND_MAP(CDlgEngrave, CDlgBase)
AUI_ON_COMMAND("Btn_Engrave", OnCommand_Engrave)
AUI_ON_COMMAND("IDCANCEL", OnCommand_CANCEL)
AUI_ON_COMMAND("Btn_Cancel", OnCommand_CANCEL)
AUI_END_COMMAND_MAP()

AUI_BEGIN_EVENT_MAP(CDlgEngrave, CDlgBase)
AUI_ON_EVENT("U_*", WM_LBUTTONDOWN, OnEventLButtonDown_Img_EngraveConfig)
AUI_ON_EVENT("Equip_*", WM_LBUTTONDOWN, OnEventLButtonDown_Img_Equip)
AUI_END_EVENT_MAP()

//------------------------------------------------------------------------
// Right-Click Shortcut for CDlgEngrave
//------------------------------------------------------------------------
typedef CECShortcutMgr::SimpleClickShortcut<CDlgEngrave> EngraveClickShortcut;
//------------------------------------------------------------------------

CDlgEngrave::CDlgEngrave()
{
	m_pImg_EquipToEngrave = NULL;
	m_pPrgs_Engrave = NULL;
	m_pObj_Engrave = NULL;
	m_state = STATE_NULL;
	m_pEngrave = NULL;
	m_iSlot = -1;
}

bool CDlgEngrave::OnInitDialog()
{
	GetGameUIMan()->GetShortcutMgr()->RegisterShortCut(new EngraveClickShortcut(this));

	AString strName;
	int i(0);
	PAUIOBJECT pObj(NULL);

	//	镌刻配置
	i = 0;
	while (true)
	{
		strName.Format("U_%02d", ++i);
		pObj = GetDlgItem(strName);
		if (!pObj)	break;
		m_ImgList_EngraveConfig.push_back((PAUIIMAGEPICTURE)pObj);
	}

	//	镌刻装备
	DDX_Control("Equip_01", m_pImg_EquipToEngrave);

	//	镌刻原料
	i = 0;
	while (true)
	{
		strName.Format("Item_%02d", ++i);
		pObj = GetDlgItem(strName);
		if (!pObj)	break;
		m_ImgList_Material.push_back((PAUIIMAGEPICTURE)pObj);
	}

	//	镌刻原料数目
	i = 0;
	while (true)
	{
		strName.Format("Txt_No%d", ++i);
		pObj = GetDlgItem(strName);
		if (!pObj)	break;
		pObj->SetAlign(AUIFRAME_ALIGN_CENTER);
		m_ObjList_MaterialNum.push_back(pObj);
	}

	//	镌刻进度
	DDX_Control("Prgs_1", m_pPrgs_Engrave);

	//	镌刻按钮
	DDX_Control("Btn_Engrave", m_pObj_Engrave);

	if (m_ImgList_EngraveConfig.empty() ||
		m_pImg_EquipToEngrave == NULL ||
		m_ImgList_Material.size() != m_ObjList_MaterialNum.size() ||
		m_pPrgs_Engrave == NULL ||
		m_pObj_Engrave == NULL)
		return false;

	return true;
}

void CDlgEngrave::OnShowDialog()
{
	int idService(0);
	if (GetGameUIMan()->m_pCurNPCEssence)
		idService = GetGameUIMan()->m_pCurNPCEssence->id_engrave_service;
	SetEngraveService(idService);
}

void CDlgEngrave::OnCommand_Engrave(const char *szCommand)
{
	if (!CanEngrave())	return;

	//	检查材料
	for (int i = 0; i < sizeof(m_pEngrave->materials)/sizeof(m_pEngrave->materials[0]); ++ i)
	{
		int id = m_pEngrave->materials[i].id;
		int num = m_pEngrave->materials[i].num;
		if (!id || num<=0)	continue;

		if (GetHostPlayer()->GetPack()->GetItemTotalNum(id) < num)
		{
			GetGameUIMan()->AddChatMessage(GetStringFromTable(9332), GP_CHAT_MISC);
			return;
		}
	}

	GetGameSession()->c2s_CmdNPCSevEngrave(m_pEngrave->id, m_iSlot);
	m_state = STATE_WAIT;
	Lock();
}

void CDlgEngrave::OnCommand_CANCEL(const char * szCommand)
{
	switch(m_state)
	{
	case STATE_WAIT:
		GetGameSession()->c2s_CmdCancelAction();
		m_state = STATE_NULL;
		Lock(false);
		break;

	case STATE_ENGRAVE:
		GetGameSession()->c2s_CmdCancelAction();
		m_state = STATE_NULL;
		Lock(false);
		m_pPrgs_Engrave->SetData(0);
		m_pPrgs_Engrave->SetAutoProgress(0, 0, 0);
		break;

	case STATE_NULL:
		SetEngraveService(0);
		Show(false);
		GetGameUIMan()->GetDialog("Win_Inventory")->Show(false);
		GetGameUIMan()->EndNPCService();
		GetHostPlayer()->GetPack()->UnfreezeAllItems();
		break;
	}
}

void CDlgEngrave::OnEventLButtonDown_Img_EngraveConfig(WPARAM wParam, LPARAM lParam, AUIObject *pObj)
{
	if (m_state != STATE_NULL)	return;
	if (!pObj || !pObj->GetData())	return;
	int id_grave = (int)pObj->GetData();
	ChooseEngrave(id_grave);
}

void CDlgEngrave::OnEventLButtonDown_Img_Equip(WPARAM wParam, LPARAM lParam, AUIObject *pObj)
{
	if (m_state != STATE_NULL)	return;
	SetEquip(-1);
}

bool CDlgEngrave::SetEquip(int iSlot)
{
	bool bRet(false);

	while (true)
	{
		if (m_state != STATE_NULL)	break;

		//	清除现有装备
		if (m_iSlot >= 0)
		{
			CECIvtrItem *pItem = (CECIvtrItem*)m_pImg_EquipToEngrave->GetDataPtr("ptr_CECIvtrItem");
			if (pItem)	pItem->Freeze(false);
			m_pImg_EquipToEngrave->ClearCover();
			m_pImg_EquipToEngrave->SetHint(_AL(""));
			m_iSlot = -1;
		}

		if (iSlot < 0)
		{
			//	清除装备的特殊处理
			bRet = true;
			break;
		}

		if (m_pEngrave == NULL)	break;

		//	检查装备部位
		CECIvtrItem *pItem = GetHostPlayer()->GetPack()->GetItem(iSlot);
		if (!pItem)	break;

		if (!(pItem->GetEquipMask() & __int64(m_pEngrave->equip_mask)))
		{
			GetGameUIMan()->AddChatMessage(GetStringFromTable(9330), GP_CHAT_MISC);
			break;
		}

		//	检查品阶
		int level = GetGame()->GetItemDataMan()->get_item_level(pItem->GetTemplateID());
		if (level <= 0)
		{
			//	物品没有品阶，不符合要求
			break;
		}
		if (level < m_pEngrave->require_level_min || level > m_pEngrave->require_level_max)
		{
			GetGameUIMan()->AddChatMessage(GetStringFromTable(9331), GP_CHAT_MISC);
			break;
		}

		//	通过检查，设置新装备
		m_pImg_EquipToEngrave->SetDataPtr(pItem, "ptr_CECIvtrItem");
			
		AString strFile;
		af_GetFileTitle(pItem->GetIconFile(), strFile);
		strFile.MakeLower();
		m_pImg_EquipToEngrave->SetCover(
			GetGameUIMan()->m_pA2DSpriteIcons[CECGameUIMan::ICONS_INVENTORY],
			GetGameUIMan()->m_IconMap[CECGameUIMan::ICONS_INVENTORY][strFile]);

		AUICTranslate trans;
		m_pImg_EquipToEngrave->SetHint(trans.Translate(pItem->GetDesc()));

		m_iSlot = iSlot;

		bRet = true;
		break;
	}

	m_pObj_Engrave->Enable(CanEngrave());

	return bRet;
}

void CDlgEngrave::StartEngrave(const S2C::cmd_engrave_start *pCmd)
{
	if (!IsShow())	return;
	if (m_state != STATE_WAIT)	return;

	m_state = STATE_ENGRAVE;
	
	DWORD dwMSecs = pCmd->use_time * 50;	
	m_pPrgs_Engrave->SetData(dwMSecs);
	m_pPrgs_Engrave->SetAutoProgress(dwMSecs, 0, AUIPROGRESS_MAX);
}

void CDlgEngrave::EndEngrave()
{
	if (!IsShow())	return;
	if (m_state != STATE_ENGRAVE)	return;

	m_pPrgs_Engrave->SetData(0);
	m_pPrgs_Engrave->SetAutoProgress(0, 0, 0);

	m_state = STATE_NULL;
	Lock(false);
	ChooseEngrave(m_pEngrave->id, true);		//	更新材料数
}

void CDlgEngrave::EngraveResult(const S2C::cmd_engrave_result *pCmd)
{
	if (pCmd->addon_num == 0)
	{
		GetGameUIMan()->AddChatMessage(GetStringFromTable(9334), GP_CHAT_MISC);
	}
	else if (pCmd->addon_num > 0)
	{
		ACString strText;
		strText.Format(GetStringFromTable(9335), pCmd->addon_num);
		GetGameUIMan()->AddChatMessage(strText, GP_CHAT_MISC);
	}
}

void CDlgEngrave::SetEngraveService(int id_engrave_service)
{
	if (m_state != STATE_NULL)	return;

	PAUIIMAGEPICTURE	pImg = NULL;

	//	清除当前选中配置
	ChooseEngrave(0);
		
	//	清除所有可能配置
	for (ImgList::iterator it = m_ImgList_EngraveConfig.begin(); it != m_ImgList_EngraveConfig.end(); ++ it)
	{
		pImg = *it;
		pImg->ClearCover();
		pImg->SetHint(_AL(""));
		pImg->SetData(0);
	}

	if (id_engrave_service <= 0)
	{
		//	特殊处理
		return;
	}

	elementdataman *pDataMan = GetGame()->GetElementDataMan();

	DATA_TYPE dt = DT_INVALID;
	const NPC_ENGRAVE_SERVICE *pService = (const NPC_ENGRAVE_SERVICE *)pDataMan->get_data_ptr(id_engrave_service, ID_SPACE_ESSENCE, dt);
	if (!pService)	return;

	//	更新镌刻配置列表
	size_t nImage(0);
	AString strIcon;
	ACString strHint, strTemp, strTemp2;
	CECIvtrEquipAddonDesc addon;
	AUICTranslate trans;

	for (int i = 0; i < sizeof(pService->id_engrave)/sizeof(pService->id_engrave[0]); ++ i)
	{
		int id_engrave = pService->id_engrave[i];
		if (id_engrave == 0)	continue;

		const ENGRAVE_ESSENCE *pEngrave = (const ENGRAVE_ESSENCE *)pDataMan->get_data_ptr(id_engrave, ID_SPACE_RECIPE, dt);
		if (!pEngrave)	continue;

		if (nImage >= m_ImgList_EngraveConfig.size())	break;

		pImg = m_ImgList_EngraveConfig[nImage++];

		//	配置数据
		pImg->SetData(id_engrave);
		
		//	图标
		af_GetFileTitle(pEngrave->file_icon, strIcon);
		strIcon.MakeLower();
		pImg->SetCover(	GetGameUIMan()->m_pA2DSpriteIcons[CECGameUIMan::ICONS_INVENTORY],
			GetGameUIMan()->m_IconMap[CECGameUIMan::ICONS_INVENTORY][strIcon]);

		//	悬浮提示
		strHint.Empty();
		CECStringTab* pDescTab = GetGame()->GetItemDesc();

		//		名称
		strTemp.Format(pDescTab->GetWideString(ITEMDESC_NAME), pEngrave->name);
		strHint += strTemp;

		//		品阶
		strHint +=  _AL("\\r");
		strTemp.Format(pDescTab->GetWideString(ITEMDESC_LEVEL), pEngrave->level);
		strHint += strTemp;

		//		镌刻部位
		strTemp.Empty();
		bool bFingerChecked(false);		//	是否已合并："戒指1;戒指2"为"戒指;"
		bool bDynSkillChecked(false);	//	是否已合并："技能装备;技能装备"为"技能装备;"
		DWORD dwMask(0);
		for (int j = 0; j < SIZE_EQUIPIVTR; ++ j)
		{
			if (j == EQUIPIVTR_FINGER1 || j == EQUIPIVTR_FINGER2)
			{
				if (bFingerChecked)	continue;
				bFingerChecked = true;
				dwMask = ((1 << EQUIPIVTR_FINGER1) & pEngrave->equip_mask);
				dwMask |= ((1 << EQUIPIVTR_FINGER2) & pEngrave->equip_mask);
			}
			else if (j == EQUIPIVTR_DYNSKILLEQUIP1 || j == EQUIPIVTR_DYNSKILLEQUIP2)
			{
				if (bDynSkillChecked)	continue;
				bDynSkillChecked = true;
				dwMask = ((1 << EQUIPIVTR_DYNSKILLEQUIP1) & pEngrave->equip_mask);
				dwMask |= ((1 << EQUIPIVTR_DYNSKILLEQUIP2) & pEngrave->equip_mask);
			}
			else
			{
				dwMask = ((1 << j) & pEngrave->equip_mask);
			}

			if (dwMask)
			{
				strTemp2 = GetStringFromTable(310 + j);
				if (!strTemp.IsEmpty())
					strTemp += _AL(";");
				strTemp += strTemp2;
			}
		}
		if (!strTemp.IsEmpty())
		{
			strHint +=  _AL("\\r");
			strHint += pDescTab->GetWideString(ITEMDESC_COL_PURPLE);
			strHint += GetStringFromTable(9333);
			strHint += strTemp;
		}

		//	品阶限制
		strTemp.Format(GetStringFromTable(9337), pEngrave->require_level_min);
		if (!strTemp.IsEmpty())
		{
			strHint +=  _AL("\\r");
			strHint += pDescTab->GetWideString(ITEMDESC_COL_PURPLE);
			strHint += strTemp;
		}

		//	附加属性条数
		strTemp.Empty();
		int nMin(0), nMax(0);
		for (int p(0); p < sizeof(pEngrave->probability_addon_num)/sizeof(pEngrave->probability_addon_num[0]); ++ p)
		{
			if (pEngrave->probability_addon_num[p] > 1e-6f)
			{
				if (nMin == 0)	nMin = p;
				nMax = p;
			}
		}

		if (nMin == nMax) strTemp2.Format(_AL("%d"), nMin);
		else strTemp2.Format(_AL("%d-%d"), nMin, nMax);
		
		strTemp.Format(GetStringFromTable(9336), strTemp2);

		if (!strTemp.IsEmpty())
		{
			strHint +=  _AL("\\r");
			strHint += pDescTab->GetWideString(ITEMDESC_COL_PURPLE);
			strHint += strTemp;
		}

		//		附加属性
		strTemp.Empty();
		for (int k = 0; k < sizeof(pEngrave->addons)/sizeof(pEngrave->addons[0]); ++ k)
		{
			if (pEngrave->addons[k].probability <= 1e-6f)
				continue;
			int id_addon = pEngrave->addons[k].id;
			if (addon.SetAddon(id_addon))
			{
				strTemp2 = addon.GetText();
				if (!strTemp2.IsEmpty())
					strTemp += strTemp2;
			}
		}

		if (!strTemp.IsEmpty())
		{
			strHint +=  _AL("\\r");
			strHint += pDescTab->GetWideString(ITEMDESC_COL_LIGHTBLUE);
			strHint += strTemp;
		}

		pImg->SetHint(trans.Translate(strHint));
	}

	//	选中默认配置
	if (nImage > 0)
	{
		pImg = m_ImgList_EngraveConfig[0];
		ChooseEngrave(pImg->GetData());
	}
}

void CDlgEngrave::ChooseEngrave(unsigned int id_engrave, bool bForceChoose /* = false*/)
{
	if (m_state != STATE_NULL)	return;
	if (m_pEngrave && m_pEngrave->id == id_engrave && !bForceChoose)	return;

	//	清除当前配置
	m_pEngrave = NULL;
	
	{
		PAUIIMAGEPICTURE pImg = NULL;
		for (ImgList::iterator it = m_ImgList_Material.begin(); it != m_ImgList_Material.end(); ++ it)
		{
			pImg = *it;
			pImg->ClearCover();
			pImg->SetHint(_AL(""));
			pImg->SetData(0);
			pImg->SetColor(A3DCOLORRGB(255, 255, 255));
		}
	}

	{
		PAUIOBJECT pObj = NULL;
		for (ObjList::iterator it= m_ObjList_MaterialNum.begin(); it != m_ObjList_MaterialNum.end(); ++ it)
		{
			pObj = *it;
			pObj->SetText(_AL(""));
		}
	}

	//	清除原来选中装备
	SetEquip(-1);

	//	设置当前配置
	DATA_TYPE dt = DT_INVALID;
	const ENGRAVE_ESSENCE *pEngrave = (const ENGRAVE_ESSENCE *)GetGame()->GetElementDataMan()->get_data_ptr(id_engrave, ID_SPACE_RECIPE, dt);
	if (!pEngrave)	return;

	m_pEngrave = pEngrave;

	AUICTranslate trans;
	ACString strHint, strText;
	AString strIcon;
	size_t nImage(0);
	PAUIIMAGEPICTURE pImg(NULL);
	PAUIOBJECT pObj(NULL);
	for (int i = 0; i < sizeof(pEngrave->materials)/sizeof(pEngrave->materials[0]); ++ i)
	{
		unsigned int id = pEngrave->materials[i].id;
		int num = pEngrave->materials[i].num;
		if (!id || num <= 0)	continue;

		CECIvtrItem *pItem = CECIvtrItem::CreateItem(id, 0, 1);
		if (!pItem)	continue;
		auto_delete<CECIvtrItem> tmp(pItem);

		if (nImage >= m_ImgList_Material.size())	break;
		pImg = m_ImgList_Material[nImage ++];

		//	图标
		af_GetFileTitle(pItem->GetIconFile(), strIcon);
		strIcon.MakeLower();
		pImg->SetCover(GetGameUIMan()->m_pA2DSpriteIcons[CECGameUIMan::ICONS_INVENTORY],
			GetGameUIMan()->m_IconMap[CECGameUIMan::ICONS_INVENTORY][strIcon]);

		//	悬浮提示
		pItem->GetDetailDataFromLocal();
		pImg->SetHint(trans.Translate(pItem->GetDesc()));

		//	数量
		pObj = m_ObjList_MaterialNum[nImage-1];
		strText.Empty();
		int total = GetHostPlayer()->GetPack()->GetItemTotalNum(id);
		strText.Format(_AL("%d/%d"), total, num);
		pObj->SetText(strText);
		pObj->SetColor(total < num ? A3DCOLORRGB(255, 0, 0) : A3DCOLORRGB(255, 255, 255));
	}

	//	更新界面显示
	{
		PAUIIMAGEPICTURE pImg = NULL;
		for (ImgList::iterator it = m_ImgList_EngraveConfig.begin(); it != m_ImgList_EngraveConfig.end(); ++ it)
		{
			pImg = *it;
			pImg->SetColor(pImg->GetData() != m_pEngrave->id ? A3DCOLORRGB(255, 255, 255) : A3DCOLORRGB(128, 128, 128));
		}
	}
	m_pObj_Engrave->Enable(CanEngrave());
}

void CDlgEngrave::Lock(bool bLock)
{
	bool bEnable = !bLock;

	//	所有可能配置
	{
		PAUIIMAGEPICTURE pImg = NULL;
		for (ImgList::iterator it = m_ImgList_EngraveConfig.begin(); it != m_ImgList_EngraveConfig.end(); ++ it)
		{
			pImg = *it;
			pImg->Enable(bEnable);
		}
	}

	//	装备图标
	m_pImg_EquipToEngrave->Enable(bEnable);
	
	//	材料
	{
		PAUIIMAGEPICTURE pImg = NULL;
		for (ImgList::iterator it = m_ImgList_Material.begin(); it != m_ImgList_Material.end(); ++ it)
		{
			pImg = *it;
			pImg->Enable(bEnable);
		}
	}
	{
		PAUIOBJECT pObj = NULL;
		for (ObjList::iterator it= m_ObjList_MaterialNum.begin(); it != m_ObjList_MaterialNum.end(); ++ it)
		{
			pObj = *it;
			pObj->Enable(bEnable);
		}
	}

	//	按钮
	m_pObj_Engrave->Enable(CanEngrave());
}

bool CDlgEngrave::CanEngrave()
{
	return m_state == STATE_NULL
		&& m_pEngrave != 0
		&& m_iSlot >= 0;
}

void CDlgEngrave::OnItemDragDrop(CECIvtrItem* pIvtrSrc, int iSrc, PAUIOBJECT pObjSrc, PAUIOBJECT pObjOver)
{
	// no need to check name "Equip_01", only one available drag-drop target
	this->SetEquip(iSrc);
}