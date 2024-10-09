/********************************************************************
	created:	2005/08/15
	created:	15:8:2005   15:04
	file name:	DlgSkillTree.cpp
	author:		yaojun
	
	purpose:	
*********************************************************************/

#include "DlgSkillTree.h"
#include "AUIImagePicture.h"
#include "EC_GameUIMan.h"

#include "DlgWikiRecipe.h"
#include "DlgAutoHelp.h"
#include "ElementSkill.h"
#include "EC_Skill.h"
#include "EC_HostPlayer.h"

#define new A_DEBUG_NEW

AUI_BEGIN_EVENT_MAP(CDlgSkillTree, CDlgBase)
AUI_ON_EVENT("Skill_*", WM_LBUTTONDOWN, OnEventLButtonSkill)
AUI_END_EVENT_MAP()

CDlgSkillTree::CDlgSkillTree()
{

}

CDlgSkillTree::~CDlgSkillTree()
{

}

void CDlgSkillTree::UpdateView()
{
	PAUIOBJECTLISTELEMENT pElement = GetFirstControl();
	
	while( pElement )
	{
		AString strName = pElement->pThis->GetName();
		strName.MakeLower();

		if( strstr(strName, "skill_") )
		{
			AString strFile = strName.Mid(strlen("skill_"));
			int id = strFile.ToInt();

			AString iconStr = GNET::ElementSkill::GetIcon(id);
			iconStr.MakeLower();
			PAUIIMAGEPICTURE pImage = static_cast<PAUIIMAGEPICTURE>(pElement->pThis);
			pImage->SetCover(
				GetGameUIMan()->m_pA2DSpriteIcons[CECGameUIMan::ICONS_SKILL],
				GetGameUIMan()->m_IconMap[CECGameUIMan::ICONS_SKILL][iconStr]);
		}
		
		pElement = pElement->pNext;
	}
}

void CDlgSkillTree::OnShowDialog()
{
	CDlgBase::OnShowDialog();

	UpdateView();
}

void CDlgSkillTree::OnEventLButtonSkill(WPARAM wParam, LPARAM lParam, AUIObject *pObj)
{
	AString command = pObj->GetName();
	command.CutLeft(strlen("Skill_"));
	int id = command.ToInt();

	if(CDlgAutoHelp::IsAutoHelp() && id)
	{
		CDlgWikiShortcut::PopSkillWiki(GetGameUIMan(),id);
	}
}

