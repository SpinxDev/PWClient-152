// File		: DlgSkillEdit.cpp
// Creator	: Xiao Zhou
// Date		: 2005/12/20

#include "AFI.h"
#include "DlgSkillEdit.h"
#include "DlgSkillSubOther.h"
#include "DlgSkillAction.h"

#include "EC_GameUIMan.h"
#include "EC_Skill.h"
#include "EC_Game.h"
#include "EC_Configs.h"
#include "EC_HostPlayer.h"

#define new A_DEBUG_NEW

AUI_BEGIN_COMMAND_MAP(CDlgSkillEdit, CDlgBase)

AUI_ON_COMMAND("confirm",		OnCommandConfirm)
AUI_ON_COMMAND("moveleft",		OnCommandMoveLeft)
AUI_ON_COMMAND("moveright",		OnCommandMoveRight)

AUI_END_COMMAND_MAP()

AUI_BEGIN_EVENT_MAP(CDlgSkillEdit, CDlgBase)

AUI_ON_EVENT("Item_*",		WM_LBUTTONDOWN,		OnEventLButtonDown_Item)
AUI_ON_EVENT("Item_*",		WM_LBUTTONDBLCLK,	OnEventLButtonDown_Item)
AUI_ON_EVENT("Special_*",	WM_LBUTTONDOWN,		OnEventLButtonDown_Item)
AUI_ON_EVENT("Special_*",	WM_LBUTTONDBLCLK,	OnEventLButtonDown_Item)
AUI_ON_EVENT("Img_Icon*",	WM_LBUTTONDOWN,		OnEventLButtonDown_Icon)

AUI_END_EVENT_MAP()

CDlgSkillEdit::CDlgSkillEdit()
{
	m_nItemSelect = 0;
	m_nIcon = 1;
	m_pImg_Item = NULL;
	m_pTxt_Name = NULL;
}

CDlgSkillEdit::~CDlgSkillEdit()
{
}

bool CDlgSkillEdit::OnInitDialog()
{
	DDX_Control("Img_Item", m_pImg_Item);
	DDX_Control("Txt_Name", m_pTxt_Name);

	return true;
}

void CDlgSkillEdit::OnCommandConfirm(const char *szCommand)
{
	EC_VIDEO_SETTING setting = GetGame()->GetConfigs()->GetVideoSettings();
	setting.comboSkill[GetData() - 1].nIcon = m_nIcon;
	int i;
	int j = 0;
	for (i = 0; ; i++)
	{
		AString strName;
		strName.Format("Item_%d", i + 1);
		PAUIIMAGEPICTURE pImage = static_cast<PAUIIMAGEPICTURE>(GetDlgItem(strName));
		if (!pImage) break;

		CECSkill *pSkill = (CECSkill *)pImage->GetDataPtr("ptr_CECSkill");
		int iType = pImage->GetData();
		if( iType == 0 && pSkill )
		{
			setting.comboSkill[GetData() - 1].idSkill[j] = pSkill->GetSkillID();
			j++;
		}
		else
		{
			setting.comboSkill[GetData() - 1].idSkill[j] = -iType;
			j++;
		}
	}
	GetGame()->GetConfigs()->SetVideoSettings(setting);
	Show(false);
	GetGameUIMan()->m_pDlgSkillSubOther->UpdateComboSkill();
}

void CDlgSkillEdit::OnCommandMoveLeft(const char *szCommand)
{
	if( m_nItemSelect == 0 )
		return;

	AString strName;
	strName.Format("Item_%d", m_nItemSelect);
	PAUIIMAGEPICTURE pImage1 = static_cast<PAUIIMAGEPICTURE>(GetDlgItem(strName));
	strName.Format("Item_%d", m_nItemSelect - 1);
	PAUIIMAGEPICTURE pImage2 = static_cast<PAUIIMAGEPICTURE>(GetDlgItem(strName));
	if( pImage1 && pImage2 )
	{
		CECSkill *pSkill1 = (CECSkill *)pImage1->GetDataPtr("ptr_CECSkill");
		CECSkill *pSkill2 = (CECSkill *)pImage2->GetDataPtr("ptr_CECSkill");
		int iType1 = pImage1->GetData();
		int iType2 = pImage2->GetData();
		if( iType2 != 0 )
			SetSpecialIcon(pImage1, iType2);
		else
			GetGameUIMan()->m_pDlgSkillSubOther->SetImage(pImage1, pSkill2);
		if( iType1 != 0 )
			SetSpecialIcon(pImage2, iType1);
		else
			GetGameUIMan()->m_pDlgSkillSubOther->SetImage(pImage2, pSkill1);
		SelectSkill(m_nItemSelect - 1);
	}
}

void CDlgSkillEdit::OnCommandMoveRight(const char *szCommand)
{
	if( m_nItemSelect == 0 )
		return;

	AString strName;
	strName.Format("Item_%d", m_nItemSelect);
	PAUIIMAGEPICTURE pImage1 = static_cast<PAUIIMAGEPICTURE>(GetDlgItem(strName));
	strName.Format("Item_%d", m_nItemSelect + 1);
	PAUIIMAGEPICTURE pImage2 = static_cast<PAUIIMAGEPICTURE>(GetDlgItem(strName));
	if( pImage1 && pImage2 )
	{
		CECSkill *pSkill1 = (CECSkill *)pImage1->GetDataPtr("ptr_CECSkill");
		CECSkill *pSkill2 = (CECSkill *)pImage2->GetDataPtr("ptr_CECSkill");
		int iType1 = pImage1->GetData();
		int iType2 = pImage2->GetData();
		if( iType2 != 0 )
			SetSpecialIcon(pImage1, iType2);
		else
			GetGameUIMan()->m_pDlgSkillSubOther->SetImage(pImage1, pSkill2);
		if( iType1 != 0 )
			SetSpecialIcon(pImage2, iType1);
		else
			GetGameUIMan()->m_pDlgSkillSubOther->SetImage(pImage2, pSkill1);
		SelectSkill(m_nItemSelect + 1);
	}
}

void CDlgSkillEdit::OnShowDialog()
{
	int i;
	EC_VIDEO_SETTING setting = GetGame()->GetConfigs()->GetVideoSettings();
	if( GetData() == 0 )
	{
		for (i = 0; i < EC_COMBOSKILL_NUM; i++)
			if( setting.comboSkill[i].nIcon == 0 )
			{
				SetData(i + 1);
				break;
			}
		if(GetData() == 0)
		{
			Show(false);
			return;
		}
	}
	ACString strText;
	strText.Format(GetStringFromTable(804), GetData() - 1);
	m_pTxt_Name->SetText(strText);
	if( setting.comboSkill[GetData() - 1].nIcon == 0 )
	{
		m_nIcon = 1;
		m_pImg_Item->SetCover(GetGameUIMan()->m_pA2DSpriteIcons[CECGameUIMan::ICONS_SKILLGRP], 2);
		for (i = 1; ; i++)
		{
			AString strName;
			strName.Format("Item_%d", i);
			PAUIIMAGEPICTURE pImage = static_cast<PAUIIMAGEPICTURE>(GetDlgItem(strName));
			if (!pImage) break;
			
			//GetGameUIMan()->m_pDlgSkill->SetImage(pImage, NULL);
			GetGameUIMan()->m_pDlgSkillSubOther->SetImage(pImage, NULL);
		}
	}
	else
	{
		m_nIcon = setting.comboSkill[GetData() - 1].nIcon;
		m_pImg_Item->SetCover(GetGameUIMan()->m_pA2DSpriteIcons[CECGameUIMan::ICONS_SKILLGRP], 
				m_nIcon + 1);
		for (i = 0; i < EC_COMBOSKILL_LEN; i++)
		{
			AString strName;
			strName.Format("Item_%d", i + 1);
			PAUIIMAGEPICTURE pImage = static_cast<PAUIIMAGEPICTURE>(GetDlgItem(strName));
			if (!pImage) break;

			int idSkill = setting.comboSkill[GetData() - 1].idSkill[i];
			if( idSkill > 0 )
			{
				CECSkill *pSkill = GetHostPlayer()->GetNormalSkill(idSkill);
				if (!pSkill) pSkill = GetHostPlayer()->GetEquipSkillByID(idSkill);
				GetGameUIMan()->m_pDlgSkillSubOther->SetImage(pImage, pSkill);
			}
			else
			{
				GetGameUIMan()->m_pDlgSkillSubOther->SetImage(pImage, NULL);
				int iType = -idSkill;
				pImage->SetData(iType);
				if( idSkill < 0 )
					SetSpecialIcon(pImage, iType);
			}
		}
	}
	for (i = 1; ; i++)
	{
		AString strName;
		strName.Format("Img_Icon%02d", i);
		PAUIIMAGEPICTURE pImage = static_cast<PAUIIMAGEPICTURE>(GetDlgItem(strName));
		if (!pImage) break;

		pImage->SetCover(GetGameUIMan()->m_pA2DSpriteIcons[CECGameUIMan::ICONS_SKILLGRP], i + 1);
	}
	if( m_nItemSelect != 0 )
	{
		AString strName;
		strName.Format("Item_%d", m_nItemSelect);
		PAUIIMAGEPICTURE pImage = static_cast<PAUIIMAGEPICTURE>(GetDlgItem(strName));
		if( pImage )
			pImage->SetColor(A3DCOLORRGB(255, 255, 255));
	}
	PAUIIMAGEPICTURE pImage;
	pImage = (PAUIIMAGEPICTURE)GetDlgItem("Special_1");
	SetSpecialIcon(pImage, 1);
	pImage = (PAUIIMAGEPICTURE)GetDlgItem("Special_2");
	SetSpecialIcon(pImage, 2);
	m_nItemSelect = 0;
}

void CDlgSkillEdit::OnEventLButtonDown_Item(WPARAM wParam, LPARAM lParam, AUIObject * pObj)
{
	A3DVIEWPORTPARAM *p = m_pA3DEngine->GetActiveViewport()->GetParam();

	GetGameUIMan()->m_ptLButtonDown.x = GET_X_LPARAM(lParam) - p->X;
	GetGameUIMan()->m_ptLButtonDown.y = GET_Y_LPARAM(lParam) - p->Y;
	GetGameUIMan()->InvokeDragDrop(this, pObj, GetGameUIMan()->m_ptLButtonDown);
}

void CDlgSkillEdit::OnEventLButtonDown_Icon(WPARAM wParam, LPARAM lParam, AUIObject * pObj)
{
	int iSlot = atoi(pObj->GetName() + strlen("Img_Icon"));
	m_pImg_Item->SetCover(GetGameUIMan()->m_pA2DSpriteIcons[CECGameUIMan::ICONS_SKILLGRP], iSlot + 1);
	m_nIcon = iSlot;
}

void CDlgSkillEdit::DragDropItem(PAUIDIALOG pDlgSrc, PAUIOBJECT pObjSrc, PAUIOBJECT pObjOver)
{
	if( pObjSrc == pObjOver )
		return;

	if( !pObjOver )
	{
		if( !(pDlgSrc == this && strstr(pObjSrc->GetName(), "Special")) )
			GetGameUIMan()->m_pDlgSkillSubOther->SetImage((PAUIIMAGEPICTURE)pObjSrc, NULL);
	}
	else
	{
		int iSlot = atoi(pObjOver->GetName() + strlen("Item_"));
		if( strstr(pObjSrc->GetName(), "Special_") )
		{
			int iType = atoi(pObjSrc->GetName() + strlen("Special_"));
			SetSpecialIcon((PAUIIMAGEPICTURE)pObjOver, iType);
		}
		else
		{
			CECSkill *pSkill = (CECSkill *)pObjSrc->GetDataPtr("ptr_CECSkill");
			int iType = pObjSrc->GetData();
			if( pDlgSrc != this )
			{
				if( iType != 0)
					SetSpecialIcon((PAUIIMAGEPICTURE)pObjOver, iType);
				else
					GetGameUIMan()->m_pDlgSkillSubOther->SetImage((PAUIIMAGEPICTURE)pObjOver, pSkill);
			}
			else
			{
				CECSkill *pSkill1 = (CECSkill *)pObjOver->GetDataPtr("ptr_CECSkill");
				int iType1 = pObjOver->GetData();
				if( iType != 0)
					SetSpecialIcon((PAUIIMAGEPICTURE)pObjOver, iType);
				else
					GetGameUIMan()->m_pDlgSkillSubOther->SetImage((PAUIIMAGEPICTURE)pObjOver, pSkill);
				if( iType1 != 0)
					SetSpecialIcon((PAUIIMAGEPICTURE)pObjSrc, iType1);
				else
					GetGameUIMan()->m_pDlgSkillSubOther->SetImage((PAUIIMAGEPICTURE)pObjSrc, pSkill1);
			}
		}
	}
}

void CDlgSkillEdit::SelectSkill(int n)
{
	AString strName;
	if( m_nItemSelect == n )
	{
		strName.Format("Item_%d", n);
		PAUIIMAGEPICTURE pImage = static_cast<PAUIIMAGEPICTURE>(GetDlgItem(strName));
		if( pImage )
			pImage->SetColor(A3DCOLORRGB(255, 255, 255));
		m_nItemSelect = 0;
	}
	else
	{
		if( m_nItemSelect != 0 )
		{
			strName.Format("Item_%d", m_nItemSelect);
			PAUIIMAGEPICTURE pImage = static_cast<PAUIIMAGEPICTURE>(GetDlgItem(strName));
			if( pImage )
				pImage->SetColor(A3DCOLORRGB(255, 255, 255));
		}
		m_nItemSelect = n;
		strName.Format("Item_%d", m_nItemSelect);
		PAUIIMAGEPICTURE pImage = static_cast<PAUIIMAGEPICTURE>(GetDlgItem(strName));
		if( pImage )
			pImage->SetColor(A3DCOLORRGB(160, 160, 160));
	}
}

void CDlgSkillEdit::SetSpecialIcon(PAUIIMAGEPICTURE pImage, int iType)
{
	if( iType == 1 )
	{
		pImage->SetCover(
			GetGameUIMan()->m_pA2DSpriteIcons[CECGameUIMan::ICONS_SKILLGRP], 0);
		pImage->SetDataPtr((void*)1, "ptr_CECSkill");
		pImage->SetData(1);
		pImage->SetHint(GetStringFromTable(805));
	}
	else
	{
		pImage->SetCover(
			GetGameUIMan()->m_pA2DSpriteIcons[CECGameUIMan::ICONS_SKILLGRP], 1);
		pImage->SetDataPtr((void*)1, "ptr_CECSkill");
		pImage->SetData(2);
		pImage->SetHint(GetStringFromTable(806));
	}
}

void CDlgSkillEdit::OnTick() {
	if (IsShow() && GetGameUIMan()->m_pDlgSkillAction->IsShow()) {
		POINT parentPos = GetGameUIMan()->m_pDlgSkillAction->GetPos();
		SetPosEx(parentPos.x + 420, parentPos.y + 100);
	}
}