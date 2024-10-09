/********************************************************************
	created:	2005/08/15
	created:	15:8:2005   13:22
	file name:	DlgSkill.cpp
	author:		yaojun
	
	purpose:	
*********************************************************************/

/*
#include "AFI.h"
#include "AUIDef.h"
#include "DlgSkill.h"
#include "EC_Skill.h"
#include "EC_HostPlayer.h"
#include "EC_GameUIMan.h"
#include "EC_Algorithm.h"
#include "DlgSkillEdit.h"
#include "EC_Game.h"
#include "EC_Configs.h"
#include "DlgAutoHelp.h"
#include "DlgWikiRecipe.h"

#define new A_DEBUG_NEW

AUI_BEGIN_EVENT_MAP(CDlgSkill, CDlgBase)

AUI_ON_EVENT("Initiative_*",WM_LBUTTONDOWN,		OnEventLButtonDownInitiative)
AUI_ON_EVENT("Passive_*",	WM_LBUTTONDOWN,		OnEventLButtonDownPassive)
AUI_ON_EVENT("Item_*",		WM_LBUTTONDOWN,		OnEventLButtonDownItem)
AUI_ON_EVENT("Item_*",		WM_LBUTTONDBLCLK,	OnEventLButtonDownItem)
AUI_ON_EVENT("*",			WM_MOUSEWHEEL,		OnEventMouseWheel)
AUI_ON_EVENT(NULL,			WM_MOUSEWHEEL,		OnEventMouseWheel)

AUI_END_EVENT_MAP()

AUI_BEGIN_COMMAND_MAP(CDlgSkill, CDlgBase)

AUI_ON_COMMAND("skilltree",	OnCommandSkillTree)
AUI_ON_COMMAND("edit",		OnCommandEdit)
AUI_ON_COMMAND("delete",	OnCommandDelete)
AUI_ON_COMMAND("new",		OnCommandNew)

AUI_END_COMMAND_MAP()

CDlgSkill::CDlgSkill()
{
	m_nComboSelect = 0;
}

CDlgSkill::~CDlgSkill()
{

}

bool CDlgSkill::OnInitDialog()
{
	m_pScl_Item = (PAUISCROLL)GetDlgItem("Scl_Item");
	for( m_nTotalItem = 0; ; m_nTotalItem++ )
	{
		char szItem[20];
		sprintf(szItem, "Initiative_%02d", m_nTotalItem + 1);
		if( !GetDlgItem(szItem) )
			break;
		m_pImgItem[m_nTotalItem] = (PAUIIMAGEPICTURE)GetDlgItem(szItem);
	}

	return true;
}

void CDlgSkill::UpdateComboSkill()
{
	EC_VIDEO_SETTING setting = GetGame()->GetConfigs()->GetVideoSettings();
	for(int i = 0; i< EC_COMBOSKILL_NUM; i++) 
	{
		AString strName;
		strName.Format("Item_%d", i + 1);
		PAUIIMAGEPICTURE pImage = static_cast<PAUIIMAGEPICTURE>(GetDlgItem(strName));
		if( pImage )
		{
			if( setting.comboSkill[i].nIcon != 0 )
			{
				pImage->SetCover(GetGameUIMan()->m_pA2DSpriteIcons[CECGameUIMan::ICONS_SKILLGRP], 
					setting.comboSkill[i].nIcon + 1);
				pImage->SetData(i + 1);
				pImage->SetDataPtr((void*)1);
				ACString strText;
				strText.Format(GetStringFromTable(804), i);
				pImage->SetHint(strText);
			}
			else
			{
				pImage->SetCover(NULL, -1);
				pImage->SetData(0);
				pImage->SetDataPtr(NULL);
				pImage->SetHint(_AL(""));
			}
		}
	}
}

void CDlgSkill::UpdatePositiveSkill(const SkillVector &sortedSkills)
{
	int i;
	if( sortedSkills.size() > CECDLGSKILL_MAX )
	{
		m_pScl_Item->SetScrollRange(0, (sortedSkills.size() - 1) / CECDLGSKILL_LINEMAX - 
			CECDLGSKILL_MAX / CECDLGSKILL_LINEMAX + 1);
		m_pScl_Item->Show(true);
		m_pScl_Item->SetBarLength(-1);
		m_pScl_Item->SetScrollStep(0);
	}
	else
	{
		m_pScl_Item->SetScrollRange(0, 0);
		m_pScl_Item->Show(false);
	}

	int nStart = m_pScl_Item->GetBarLevel() * 8;
	if( nStart < atoi(m_pImgItem[0]->GetName() + strlen("Initiative_")) )
	{
		for(i = 0; i < m_nTotalItem; i++)
		{
			char szName[20];
			sprintf(szName, "Initiative_%02d", nStart + i + 1);
			m_pImgItem[i]->SetName(szName);
		}
	}
	else if( nStart > atoi(m_pImgItem[0]->GetName() + strlen("Initiative_")) )
	{
		for(i = m_nTotalItem - 1; i >= 0; i--)
		{
			char szName[20];
			sprintf(szName, "Initiative_%02d", nStart + i + 1);
			m_pImgItem[i]->SetName(szName);
		}
	}

	for( i = 0; i < m_nTotalItem; i++ )
	{
		PAUIIMAGEPICTURE pImage = m_pImgItem[i];
		if( !pImage ) break;
		
		// get clock icon
		AUIClockIcon *pClock = pImage->GetClockIcon();
		
		if( nStart + i >= (int)sortedSkills.size() )
		{
			SetImage(pImage, NULL);

			pImage->SetColor(A3DCOLORRGB(255, 255, 255));
			pClock->SetProgressRange(0, 1);
			pClock->SetProgressPos(1);
			continue;
		}

		if( i < (int)sortedSkills.size() )
		{
			CECSkill * pSkill = sortedSkills[nStart + i];
			SetImage(pImage, pSkill);
			
			if( pSkill->ReadyToCast() && GetHostPlayer()->GetPrepSkill() != pSkill )
			{
				if( !GetHostPlayer()->CheckSkillCastCondition(pSkill) )
					pImage->SetColor(A3DCOLORRGB(255, 255, 255));
				else
					pImage->SetColor(A3DCOLORRGB(128, 128, 128));
			}
			else
				pClock->SetColor(A3DCOLORRGBA(0, 0, 0, 128));
			if( pSkill->GetCoolingTime() > 0 || GetHostPlayer()->GetPrepSkill() == pSkill )
			{
				pClock->SetProgressRange(0, pSkill->GetCoolingTime());
				if( GetHostPlayer()->GetPrepSkill() == pSkill )
					pClock->SetProgressPos(0);
				else
					pClock->SetProgressPos(pSkill->GetCoolingTime() - pSkill->GetCoolingCnt());
			}
		}
		else
		{
			SetImage(pImage, NULL);

			pImage->SetColor(A3DCOLORRGB(255, 255, 255));
			pClock->SetProgressRange(0, 1);
			pClock->SetProgressPos(1);
		}
	}
}


void CDlgSkill::UpdatePassiveSkill(const SkillVector &sortedSkills)
{
	for (int i = 0; ; i++)
	{
		AString strName;
		strName.Format("Passive_%02d", i + 1);
		PAUIIMAGEPICTURE pImage = static_cast<PAUIIMAGEPICTURE>(GetDlgItem(strName));
		if (!pImage) break;
		
		if (i < (int)sortedSkills.size())		
			SetImage(pImage, sortedSkills[i]);		
		else		
			SetImage(pImage, NULL);
	}

}

class CompareCECSkillShowOrder
{
public:
	bool operator ()(CECSkill * p1, CECSkill * p2)
	{
		return p1->GetShowOrder() < p2->GetShowOrder();
	}
};

bool CDlgSkill::Tick(void)
{
	if( IsShow() ) UpdateView();

	return CDlgBase::Tick();
}

void CDlgSkill::UpdateView()
{
	int i;
	SkillVector vecSkill;

	for( i = 0; i < GetHostPlayer()->GetPositiveSkillNum(); i++ )
	{
		CECSkill * pSkill = GetHostPlayer()->GetPositiveSkillByIndex(i);
		vecSkill.push_back(pSkill);
	}
	for (i = 0; i < GetHostPlayer()->GetEquipSkillNum(); ++ i)
	{
		CECSkill *pSkill = GetHostPlayer()->GetEquipSkillByIndex(i);
		vecSkill.push_back(pSkill);
	}
	BubbleSort(vecSkill.begin(), vecSkill.end(), CompareCECSkillShowOrder());
	UpdatePositiveSkill(vecSkill);

	vecSkill.clear();
	for( i = 0; i < GetHostPlayer()->GetPassiveSkillNum(); i++ )
	{
		CECSkill * pSkill = GetHostPlayer()->GetPassiveSkillByIndex(i);
		vecSkill.push_back(pSkill);
	}
	BubbleSort(vecSkill.begin(), vecSkill.end(), CompareCECSkillShowOrder());
	UpdatePassiveSkill(vecSkill);

	UpdateComboSkill();
}

bool CDlgSkill::Render()
{
	if (!CDlgBase::Render())
		return false;

	//	装备技能显示额外内容
	CECGameUIMan *pGameUIMan = GetGameUIMan();
	CECHostPlayer *pHost = GetHostPlayer();
	for(int i = 0; i < m_nTotalItem; i++ )
	{
		PAUIIMAGEPICTURE pImage = m_pImgItem[i];
		if( !pImage ) break;

		void *p = pImage->GetDataPtr("ptr_CECSkill");
		if (!p) continue;

		CECSkill *pSkill = (CECSkill *)p;
		if (pHost->IsEquipSkill(pSkill))
			pGameUIMan->RenderCover(pImage);
	}

	return true;
}

void CDlgSkill::OnShowDialog()
{
	CDlgBase::OnShowDialog();

	if (GetHostPlayer() && GetHostPlayer()->GetBattleInfo().IsChariotWar())
	{
		Show(false);
		GetGameUIMan()->MessageBox("",GetGameUIMan()->GetStringFromTable(10713),MB_OK,A3DCOLORRGB(255,255,255));
		return;
	}

	UpdateView();
	if( m_nComboSelect != 0 )
	{
		AString strName;
		strName.Format("Item_%d", m_nComboSelect);
		PAUIIMAGEPICTURE pImage = static_cast<PAUIIMAGEPICTURE>(GetDlgItem(strName));
		if( pImage )
			pImage->SetColor(A3DCOLORRGB(255, 255, 255));
	}
	m_nComboSelect = 0;
}

void CDlgSkill::OnCommandSkillTree(const char *szCommand)
{
}

void CDlgSkill::OnCommandEdit(const char *szCommand)
{
	GetGameUIMan()->m_pDlgSkillEdit->SetData(m_nComboSelect);
	GetGameUIMan()->m_pDlgSkillEdit->Show(true);
}

void CDlgSkill::OnCommandNew(const char *szCommand)
{
	GetGameUIMan()->m_pDlgSkillEdit->SetData(0);
	GetGameUIMan()->m_pDlgSkillEdit->Show(true);
}

void CDlgSkill::OnCommandDelete(const char *szCommand)
{
	if( m_nComboSelect < 0 || m_nComboSelect > EC_COMBOSKILL_NUM )
		return;

	EC_VIDEO_SETTING setting = GetGame()->GetConfigs()->GetVideoSettings();
	setting.comboSkill[m_nComboSelect - 1].nIcon = 0;
	m_nComboSelect = 0;
	GetGame()->GetConfigs()->SetVideoSettings(setting);
	UpdateComboSkill();
}

void CDlgSkill::OnEventLButtonDownInitiative(WPARAM wParam, LPARAM lParam, AUIObject *pObj)
{
	if( !pObj->GetDataPtr("ptr_CECSkill") )
		return;
	
	A3DVIEWPORTPARAM *p = m_pA3DEngine->GetActiveViewport()->GetParam();
	POINT pt = 
	{
		GET_X_LPARAM(lParam) - p->X,
		GET_Y_LPARAM(lParam) - p->Y,
	};
	
	GetGameUIMan()->m_ptLButtonDown = pt;
	GetGameUIMan()->InvokeDragDrop(this, pObj, pt);
}

void CDlgSkill::OnEventLButtonDownPassive(WPARAM wParam, LPARAM lParam, AUIObject *pObj)
{
	CECSkill *pSkill = (CECSkill *)pObj->GetDataPtr("ptr_CECSkill");

	if( !pSkill )
		return;
		
	int id = pSkill->GetSkillID();
	if (CDlgAutoHelp::IsAutoHelp() && id)
	{
		CDlgWikiShortcut::PopSkillWiki(GetGameUIMan(),id);
	}
}


void CDlgSkill::OnEventLButtonDownItem(WPARAM wParam, LPARAM lParam, AUIObject *pObj)
{
	if( pObj->GetData() == 0 )
		return;

	A3DVIEWPORTPARAM *p = m_pA3DEngine->GetActiveViewport()->GetParam();
	POINT pt = 
	{
		GET_X_LPARAM(lParam) - p->X,
		GET_Y_LPARAM(lParam) - p->Y,
	};
	
	GetGameUIMan()->m_ptLButtonDown = pt;
	GetGameUIMan()->InvokeDragDrop(this, pObj, pt);
}

void CDlgSkill::SetImage(AUIImagePicture *pImage, CECSkill *pSkill)
{
	ASSERT(pImage);

	if (pSkill)
	{
		AString strFile;
		af_GetFileTitle(pSkill->GetIconFile(), strFile);
		strFile.MakeLower();
		pImage->SetCover(
			GetGameUIMan()->m_pA2DSpriteIcons[CECGameUIMan::ICONS_SKILL],
			GetGameUIMan()->m_IconMap[CECGameUIMan::ICONS_SKILL][strFile]);
		pImage->SetDataPtr(pSkill,"ptr_CECSkill");
		pImage->SetData(0);
		pImage->SetHint(pSkill->GetDesc());
	}
	else
	{
		pImage->SetCover(NULL, -1);
		pImage->SetDataPtr(NULL);
		pImage->SetData(0);
		pImage->SetHint(_AL(""));
	}
}

void CDlgSkill::SelectComboSkill(int n)
{
	AString strName;
	if( m_nComboSelect == n )
	{
		strName.Format("Item_%d", n);
		PAUIIMAGEPICTURE pImage = static_cast<PAUIIMAGEPICTURE>(GetDlgItem(strName));
		if( pImage )
			pImage->SetColor(A3DCOLORRGB(255, 255, 255));
		m_nComboSelect = 0;
	}
	else
	{
		if( m_nComboSelect != 0 )
		{
			strName.Format("Item_%d", m_nComboSelect);
			PAUIIMAGEPICTURE pImage = static_cast<PAUIIMAGEPICTURE>(GetDlgItem(strName));
			if( pImage )
				pImage->SetColor(A3DCOLORRGB(255, 255, 255));
		}
		m_nComboSelect = n;
		strName.Format("Item_%d", m_nComboSelect);
		PAUIIMAGEPICTURE pImage = static_cast<PAUIIMAGEPICTURE>(GetDlgItem(strName));
		if( pImage )
			pImage->SetColor(A3DCOLORRGB(160, 160, 160));
	}
}

void CDlgSkill::OnEventMouseWheel(WPARAM wParam, LPARAM lParam, AUIObject *pObj)
{
	int x = GET_X_LPARAM(lParam);
	int y = GET_Y_LPARAM(lParam);
	int zDelta = (short)HIWORD(wParam);
	if( zDelta > 0 )
		zDelta = 1;
	else
		zDelta = -1;
	if( m_pScl_Item->IsShow() )
		m_pScl_Item->SetBarLevel(m_pScl_Item->GetBarLevel() - zDelta);
}

bool CDlgSkill::OnChangeLayoutBegin()
{
	m_pScl_Item->SetBarLevel(0);
	UpdateView();
	return true;
}
*/