// File		: DlgEquipDye.cpp
// Creator	: Xiao Zhou
// Date		: 2007/4/9

#include "AFI.h"
#include "AUICTranslate.h"
#include "DlgEquipDye.h"
#include "EC_ShortcutMgr.h"
#include "DlgFittingRoom.h"
#include "EC_Game.h"
#include "EC_GameUIMan.h"
#include "EC_GameSession.h"
#include "EC_HostPlayer.h"
#include "EC_IvtrItem.h"
#include "EC_Inventory.h"
#include "EC_IvtrEquip.h"
#include "EC_IvtrConsume.h"
#include "EC_IvtrFashion.h"
#include "EC_BaseColor.h"
#include "elementdataman.h"

#include "DlgPureColorFashionTip.h"

#define new A_DEBUG_NEW

AUI_BEGIN_COMMAND_MAP(CDlgEquipDye, CDlgBase)

AUI_ON_COMMAND("confirm",		OnCommandConfirm)
AUI_ON_COMMAND("IDCANCEL",		OnCommandCancel)
AUI_ON_COMMAND("shownormal",	OnCommandShowNormal)
AUI_ON_COMMAND("showall",		OnCommandShowAll)

AUI_END_COMMAND_MAP()

AUI_BEGIN_EVENT_MAP(CDlgEquipDye, CDlgBase)

AUI_ON_EVENT("Img_Item*",	WM_LBUTTONDOWN,	OnEventLButtonDown_Item)
AUI_ON_EVENT("Img_Dye",		WM_LBUTTONDOWN,	OnEventLButtonDown_Dye)

AUI_END_EVENT_MAP()

//------------------------------------------------------------------------
// Right-Click Shortcut for CDlgEquipDye
//------------------------------------------------------------------------
typedef CECShortcutMgr::SimpleClickShortcut<CDlgEquipDye> EquipDyeClickShortcut;
//------------------------------------------------------------------------

CDlgEquipDye::CDlgEquipDye()
{
	m_pTxt_Num = NULL;
	m_pTxt_Name = NULL;
	m_pTxt_Dye = NULL;
	::memset(m_pTxt_Color, 0, sizeof(m_pTxt_Color));
	::memset(m_pImg_Item, 0, sizeof(m_pImg_Item));
	m_pImg_Dye = NULL;
	m_pBtn_Confirm = NULL;
	m_pBtn_ShowNormal = NULL;
	m_pBtn_ShowAll = NULL;
	for (int i = 0; i < MaxImgCount; ++ i)
		m_nSolt[i] = -1;
	m_nSoltDye = -1;
	m_nRequireNum = 0;
}

CDlgEquipDye::~CDlgEquipDye()
{
}

bool CDlgEquipDye::OnInitDialog()
{
	if (IsDlgEquipDye())
	{
		DDX_Control("Txt_Name", m_pTxt_Name);
		DDX_Control("Txt_Dye", m_pTxt_Dye);
		DDX_Control("Txt_Color", m_pTxt_Color[0]);
		DDX_Control("Img_Item", m_pImg_Item[0]);
	}
	else
	{
		char szBuffer[40];
		for (int i = 0; i < MaxImgCount; ++ i)
		{
			sprintf(szBuffer, "Txt_Color%d", i+1);
			DDX_Control(szBuffer, m_pTxt_Color[i]);

			sprintf(szBuffer, "Img_Item%d", i+1);
			DDX_Control(szBuffer, m_pImg_Item[i]);
		}
	}
	DDX_Control("Txt_Num", m_pTxt_Num);
	DDX_Control("Img_Dye", m_pImg_Dye);
	DDX_Control("Btn_Confirm", m_pBtn_Confirm);
	DDX_Control("Btn_NDye", m_pBtn_ShowNormal);
	DDX_Control("Btn_ADye", m_pBtn_ShowAll);

	GetGameUIMan()->GetShortcutMgr()->RegisterShortCut(new EquipDyeClickShortcut(this));

	return true;
}

void CDlgEquipDye::OnCommandConfirm(const char *szCommand)
{
	CECIvtrDyeTicket* pDye = dynamic_cast<CECIvtrDyeTicket*>(GetDyeItem());
	if (IsDlgEquipDye())
	{
		CECIvtrEquip* pEquip = dynamic_cast<CECIvtrEquip*>(GetEquipItem(0));
		if( pEquip && m_nSolt[0] >= 0 && pDye && m_nSoltDye >= 0 ) 
		{
			if( m_nRequireNum > pDye->GetCount() )
			{
				GetGameUIMan()->MessageBox("", GetStringFromTable(875), MB_OK, A3DCOLORRGB(255, 255, 255));
				return;
			}
			GetGameSession()->c2s_CmdNPCSevDye(m_nSolt[0], pEquip->GetTemplateID(), m_nSoltDye, pDye->GetTemplateID());
			m_pBtn_Confirm->Enable(false);
		}
	}
	else
	{
		if (!pDye)
			return;

		int nEquip(0);
		for (int i = 0; i < MaxImgCount; ++ i)
		{
			if (GetEquipItem(i))
				++ nEquip;
		}
		if (!nEquip)
			return;

		if (nEquip < 2)
		{
			GetGameUIMan()->MessageBox("", GetStringFromTable(7891), MB_OK, A3DCOLORRGB(255, 255, 255));
			return;
		}

		if (m_nRequireNum > pDye->GetCount())
		{
			GetGameUIMan()->MessageBox("", GetStringFromTable(875), MB_OK, A3DCOLORRGB(255, 255, 255));
			return;
		}

		GetGameSession()->c2s_CmdNPCSevDyeBySuit(
			m_nSolt[0] >= 0 ? m_nSolt[0] : 255,
			m_nSolt[1] >= 0 ? m_nSolt[1] : 255,
			m_nSolt[3] >= 0 ? m_nSolt[3] : 255,
			m_nSolt[2] >= 0 ? m_nSolt[2] : 255,
			m_nSoltDye);

		m_pBtn_Confirm->Enable(false);
	}
	const DYE_TICKET_ESSENCE* pEssence = pDye->GetDBEssence();
	float fDiff = pEssence->h_max - pEssence->h_min + pEssence->v_max - pEssence->v_min + pEssence->s_max - pEssence->s_min;
	if (fDiff > 0.0000001) {
		GetGameUIMan()->m_pDlgPureColorFashionTip->SetRandomDyeUsed(true);
	} else {
		GetGameUIMan()->m_pDlgPureColorFashionTip->SetRandomDyeUsed(false);
	}
}

void CDlgEquipDye::OnCommandCancel(const char *szCommand)
{
	ClearDialog();
	GetGameUIMan()->EndNPCService();
	Show(false);
	GetGameUIMan()->GetDialog("Win_Inventory")->Show(false);
	GetGameUIMan()->m_pDlgFittingRoom->Show(false);
	GetHostPlayer()->GetPack(IVTRTYPE_PACK)->UnfreezeAllItems();
}

void CDlgEquipDye::OnCommandShowNormal(const char *szCommand)
{
	ShowDlgEquipDye();
}

void CDlgEquipDye::OnCommandShowAll(const char *szCommand)
{
	ShowDlgAllEquipDye();
}

void CDlgEquipDye::OnShowDialog()
{
	ClearDialog();

	PAUISTILLIMAGEBUTTON pBtnToPush = NULL;
	PAUISTILLIMAGEBUTTON pBtnToPop = NULL;
	if (IsDlgEquipDye())
	{
		pBtnToPush = m_pBtn_ShowNormal;
		pBtnToPop = m_pBtn_ShowAll;
	}
	else
	{
		pBtnToPush = m_pBtn_ShowAll;
		pBtnToPop = m_pBtn_ShowNormal;
	}

	pBtnToPush->SetPushed(true);
	pBtnToPush->SetColor(A3DCOLORRGB(255, 203, 74));

	pBtnToPop->SetPushed(false);
	pBtnToPop->SetColor(A3DCOLORRGB(255, 255, 255));
}

void CDlgEquipDye::OnEventLButtonDown_Item(WPARAM wParam, LPARAM lParam, AUIObject * pObj)
{
	PAUIIMAGEPICTURE pImageItem = dynamic_cast<PAUIIMAGEPICTURE>(pObj);
	if (!pImageItem){
		return;
	}
	int index = -1;
	if (IsDlgEquipDye()){
		if (stricmp(pImageItem->GetName(), "Img_Item")){			//	只支持 "Img_Item"
			return;
		}
		index = 0;
	}else{
		index = GetNameIndex(pImageItem->GetName(), "Img_Item")-1;	//	支持 Img_Item1~Img_Item4
		if (index < 0 || index > MaxImgCount){
			return;
		}
	}

	CECIvtrItem *pItem = GetEquipItem(index);
	if( pItem )
		pItem->Freeze(false);
	pImageItem->ClearCover();
	pImageItem->SetHint(_AL(""));
	SetEquipItemID(index, 0);
	if (IsDlgEquipDye())
		m_pTxt_Name->SetText(_AL(""));
	m_nSolt[index] = -1;
	UpdateRequireNum();
}

void CDlgEquipDye::OnEventLButtonDown_Dye(WPARAM wParam, LPARAM lParam, AUIObject * pObj)
{
	CECIvtrItem *pItem = (CECIvtrItem *)GetDyeItem();
	if( pItem )
		pItem->Freeze(false);
	m_pImg_Dye->ClearCover();
	m_pImg_Dye->SetHint(_AL(""));
	SetDyeItemID(0);
	if (IsDlgEquipDye())
		m_pTxt_Dye->SetText(_AL(""));
	m_nSoltDye = -1;
	UpdateNumDisplay();
}

void CDlgEquipDye::OnEquipChange(CECIvtrItem *pItem, int nSlot)
{
	//	判断是否是被染色物品被修改，是则更新界面
	if (!pItem || nSlot < 0 || nSlot >= GetHostPlayer()->GetPack()->GetSize())
	{
		ASSERT(false);
		return;
	}

	int tid = pItem->GetTemplateID();

	//	判断是否物品ID相同
	bool bFound(false);
	if (IsDlgEquipDye())
	{
		if (nSlot == m_nSolt[0] && tid == GetEquipItemID(0))
			bFound = true;
	}
	else
	{
		for (int i(0); i < MaxImgCount; ++ i)
		{
			if (nSlot == m_nSolt[i] && tid == GetEquipItemID(i))
			{
				bFound = true;
				break;
			}
		}
	}
	if (!bFound) return;

	// 如果染色为纯色，弹出提示：染色成功
	if (pItem->GetClassID() == CECIvtrItem::ICID_FASHION) {
		CECIvtrFashion* pFashion = dynamic_cast<CECIvtrFashion*>(pItem);
		A3DCOLOR color = pFashion->GetColor();

		const CECBaseColor::BaseColor* baseColor = CECBaseColor::Instance().GetBaseColorFromColor(color);
		if (baseColor) {
			GetGameUIMan()->m_pDlgPureColorFashionTip->StartShowTip(baseColor->m_name);
		}
	}

	CDlgFittingRoom *pDlgFittingRoom = GetGameUIMan()->m_pDlgFittingRoom;
	if (!IsShow() || !pDlgFittingRoom->IsShow())
		return;

	//	物品相同，更新试衣间
	PAUIOBJECT pImage = pDlgFittingRoom->GetEquipIcon(pItem);
	if (pImage)
	{
		GetGameUIMan()->BringWindowToTop(pDlgFittingRoom);
		pDlgFittingRoom->SetEquipIcon(pItem, pImage);
	}
}

void CDlgEquipDye::SetEquip(CECIvtrItem *pItem, int nSlot, int nImageIndex)
{
	CECIvtrEquip* pEquip = (CECIvtrEquip*)pItem;
	int requireCount(0);
	if (!GetRequireDyeCount(pItem, requireCount))
		return;
	if (requireCount == 0)
	{
		GetGameUIMan()->MessageBox("", GetStringFromTable(873), MB_OK, A3DCOLORRGB(255, 255, 255));
		return;
	}

	PAUIIMAGEPICTURE &pImgItem = m_pImg_Item[nImageIndex];
	OnEventLButtonDown_Item(0, 0, pImgItem);
	SetEquipItemID(nImageIndex, pEquip->GetTemplateID());
	m_nSolt[nImageIndex] = nSlot;
	pItem->Freeze(true);
	GetGameUIMan()->SetCover(pImgItem, pEquip->GetIconFile());

	if (IsDlgEquipDye())
		m_pTxt_Name->SetText(pEquip->GetName());

	PAUIOBJECT pObj = GetGameUIMan()->m_pDlgFittingRoom->GetEquipIcon(pEquip);
	if (pObj && GetGameUIMan()->m_pDlgFittingRoom->IsShow())
		GetGameUIMan()->m_pDlgFittingRoom->SetEquipIcon(pEquip, pObj);

	UpdateRequireNum();
}

void CDlgEquipDye::SetDye(CECIvtrItem *pItem, int nSlot)
{
	CECIvtrDyeTicket* pDye = (CECIvtrDyeTicket*)pItem;
	if (pDye->GetDBEssence()->usage != 0)
		return;

	if (IsDlgAllEquipDye())
	{
		// Validate it's random dye
		//
		const DYE_TICKET_ESSENCE * pEssence = pDye->GetDBEssence();

		const float Float_Zero = 1e-6f;

		if (fabs(pEssence->h_min) > Float_Zero ||
			fabs(pEssence->h_max-1.0f) > Float_Zero ||
			fabs(pEssence->s_min) > Float_Zero ||
			fabs(pEssence->s_max-1.0f) > Float_Zero ||
			fabs(pEssence->v_min) > Float_Zero ||
			fabs(pEssence->v_max-1.0f) > Float_Zero)
		{
			GetGameUIMan()->AddChatMessage(GetStringFromTable(7890), GP_CHAT_MISC);
			return;
		}
	}

	OnEventLButtonDown_Dye(0, 0, NULL);
	SetDyeItemID(pDye->GetTemplateID());
	m_nSoltDye = nSlot;
	pItem->Freeze(true);
	AString strFile;
	af_GetFileTitle(pDye->GetIconFile(), strFile);
	strFile.MakeLower();
	m_pImg_Dye->SetCover(GetGameUIMan()->m_pA2DSpriteIcons[CECGameUIMan::ICONS_INVENTORY],
		GetGameUIMan()->m_IconMap[CECGameUIMan::ICONS_INVENTORY][strFile]);
	AUICTranslate trans;
	const ACHAR * szDesc = pDye->GetDesc();
	if( szDesc )
		m_pImg_Dye->SetHint(trans.Translate(szDesc));
	else
		m_pImg_Dye->SetHint(_AL(""));

	if (IsDlgEquipDye())
		m_pTxt_Dye->SetText(pItem->GetName());

	UpdateNumDisplay();
}

void CDlgEquipDye::OnTick()
{
	// Update fashion color and FittingRoom
	//
	AUICTranslate trans;
	int nEquip(0);
	for (int i = 0; i < MaxImgCount; ++ i)
	{
		// Update fashion color
		if (m_nSolt[i] < 0)
		{
			if (m_pTxt_Color[i])
				m_pTxt_Color[i]->Show(false);
			continue;
		}

		CECIvtrEquip *pEquip = (CECIvtrEquip*)GetHostPlayer()->GetPack()->GetItem(m_nSolt[i]);	
		
		if (pEquip->IsFashion())
		{
			m_pTxt_Color[i]->Show(true);
			m_pTxt_Color[i]->SetColor(((CECIvtrFashion*)pEquip)->GetColor());
		}
		
		// Update hint
		const ACHAR *szDesc = pEquip->GetDesc();
		if( szDesc )
		{
			m_pImg_Item[i]->SetHint(trans.Translate(szDesc));
		}
		else
		{
			m_pImg_Item[i]->SetHint(_AL(""));
		}
		
		++ nEquip;
	}

	// Update dye number display
	//
	UpdateNumDisplay();
	
	// Update confirm button
	//
	int nCount = GetDyeCount();
	m_pBtn_Confirm->Enable(nEquip>0 && nCount>0);
}

bool CDlgEquipDye::IsDlgEquipDye()
{
	return 0 == stricmp(GetName(), "Win_EquipDye");
}

bool CDlgEquipDye::IsDlgAllEquipDye()
{
	return 0 == stricmp(GetName(), "Win_AllEquipDye");
}

void CDlgEquipDye::ShowDlgEquipDye()
{
	CDlgEquipDye *pDlgToShow = GetGameUIMan()->m_pDlgEquipDye;
	CDlgEquipDye *pDlgToHide = GetGameUIMan()->m_pDlgAllEquipDye;
	SwitchDialog(pDlgToShow, pDlgToHide);
}

void CDlgEquipDye::ShowDlgAllEquipDye()
{
	CDlgEquipDye *pDlgToShow = GetGameUIMan()->m_pDlgAllEquipDye;
	CDlgEquipDye *pDlgToHide = GetGameUIMan()->m_pDlgEquipDye;
	SwitchDialog(pDlgToShow, pDlgToHide);
}

void CDlgEquipDye::SwitchDialog(CDlgEquipDye *pDlgToShow, CDlgEquipDye *pDlgToHide)
{
	if (pDlgToShow->IsShow())
		return;
	
	pDlgToHide->ClearDialog();
	pDlgToHide->Show(false);
	
	POINT pos = pDlgToHide->GetPos();
	pDlgToShow->SetPosEx(pos.x, pos.y);
	pDlgToShow->Show(true);
}


CECIvtrItem * CDlgEquipDye::GetItemFromPack(int nSlot)
{
	return nSlot >= 0 ? GetHostPlayer()->GetPack()->GetItem(nSlot) : NULL;
}

CECIvtrDyeTicket * CDlgEquipDye::GetDyeItem()
{
	return dynamic_cast<CECIvtrDyeTicket *>(GetItemFromPack(m_nSoltDye));
}

CECIvtrEquip * CDlgEquipDye::GetEquipItem(int index)
{
	return dynamic_cast<CECIvtrEquip *>(GetItemFromPack(m_nSolt[index]));
}

int	CDlgEquipDye::GetDyeItemID()
{
	return (int)m_pImg_Dye->GetData();
}

void CDlgEquipDye::SetDyeItemID(int tid)
{
	m_pImg_Dye->SetData(tid);
}

int CDlgEquipDye::GetEquipItemID(int index)
{
	return (int)m_pImg_Item[index]->GetData();
}

void CDlgEquipDye::SetEquipItemID(int index, int tid)
{
	m_pImg_Item[index]->SetData(tid);
}

void CDlgEquipDye::ClearDialog()
{
	if (IsDlgEquipDye())
	{
		OnEventLButtonDown_Item(0, 0, m_pImg_Item[0]);
	}
	else
	{
		for (int i = 0; i < MaxImgCount; ++ i)
			OnEventLButtonDown_Item(0, 0, m_pImg_Item[i]);
	}
	OnEventLButtonDown_Dye(0, 0, NULL);
}

bool CDlgEquipDye::GetRequireDyeCount(CECIvtrItem *pItem, int &count)
{
	if (!pItem)
		return false;

	CECIvtrEquip* pEquip = (CECIvtrEquip*)pItem;
	elementdataman *pDBMan = GetGame()->GetElementDataMan();
	DATA_TYPE DataType;
	FASHION_ESSENCE* pDBEssence = (FASHION_ESSENCE*)pDBMan->get_data_ptr(pEquip->GetTemplateID(), ID_SPACE_ESSENCE, DataType);
	if( DataType != DT_FASHION_ESSENCE || !pDBEssence )
		return false;

	count = pDBEssence->require_dye_count;
	return true;
}

int CDlgEquipDye::GetDyeCount()
{
	if (m_nSoltDye >= 0)
	{
		CECIvtrItem *pDye = GetHostPlayer()->GetPack()->GetItem(m_nSoltDye);
		if (pDye)
			return pDye->GetCount();
	}
	return 0;
}

void CDlgEquipDye::UpdateRequireNum()
{
	// Update require num
	//
	m_nRequireNum = 0;

	int count(0);
	for (int i = 0; i < MaxImgCount; ++ i)
	{
		if (m_nSolt[i] >= 0)
		{
			CECIvtrItem *pItem = (CECIvtrItem *) GetEquipItem(i);
			if (GetRequireDyeCount(pItem, count))
				m_nRequireNum += count;
		}
	}

	if (IsDlgAllEquipDye())
	{
		const int ExpandRatio = 2;
		m_nRequireNum *= ExpandRatio;
	}

	// Update num display
	UpdateNumDisplay();
}

void CDlgEquipDye::UpdateNumDisplay()
{
	int nCount = GetDyeCount();
	if (nCount >= m_nRequireNum)
		m_pTxt_Num->SetColor(A3DCOLORRGB(255, 255, 255));
	else
		m_pTxt_Num->SetColor(A3DCOLORRGB(255, 0, 0));
	
	ACHAR szText[20];
	a_sprintf(szText, _AL("%d/%d"), m_nRequireNum, nCount);
	m_pTxt_Num->SetText(szText);
}

void CDlgEquipDye::OnItemDragDrop(CECIvtrItem* pIvtrSrc, int iSrc, PAUIOBJECT pObjSrc, PAUIOBJECT pObjOver)
{
	if (pIvtrSrc->GetClassID() == CECIvtrItem::ICID_DYETICKET)
	{
		// no need to check name "Img_Dye", only one available drag-drop target
		this->SetDye(pIvtrSrc, iSrc);
	}
	else // drag-drop a equip or click directly
	{
		CECIvtrEquip* pEquip = dynamic_cast<CECIvtrEquip*>(pIvtrSrc);
		if (!pEquip || !pEquip->IsFashion())
				return;

		if(IsDlgEquipDye())
		{
			// no need to check name "Img_Item", only one available drag-drop target
			this->SetEquip(pIvtrSrc, iSrc);
		}
		else if( IsDlgAllEquipDye() )
		{
			// Validate fashion match in Win_AllEquipDye dialog
			int fashionIndex = -1;
			__int64 fashionMask = pEquip->GetEquipMask();
			if (fashionMask & EQUIP_MASK64_FASHION_BODY)			fashionIndex = 0;
			else if (fashionMask & EQUIP_MASK64_FASHION_LEG)		fashionIndex = 1;
			else if (fashionMask & EQUIP_MASK64_FASHION_WRIST)	fashionIndex = 2;
			else if (fashionMask & EQUIP_MASK64_FASHION_FOOT)		fashionIndex = 3;
			if(fashionIndex < 0)
			{
				return;
			}

			// for right-click scene
			if(!pObjOver)
			{
				AString strName;
				pObjOver = GetDlgItem(strName.Format("Img_Item%d", fashionIndex+1));
			}
			
			// for drag-drop scene
			if (!pObjOver || !strstr(pObjOver->GetName(), "Img_Item"))
			{
				return;
			}
			
			// get the drag-drop index
			int iDst = atoi(pObjOver->GetName() + strlen("Img_Item")) - 1;
			if (fashionIndex == iDst)
			{
				this->SetEquip(pIvtrSrc, iSrc, fashionIndex);
			}
		}
	}
}