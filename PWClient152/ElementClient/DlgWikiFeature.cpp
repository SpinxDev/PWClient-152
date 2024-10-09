// Filename	: DlgWikiFeature.cpp
// Creator	: Feng Ning
// Date		: 2010/09/25

#include "DlgWikiFeature.h"
#include "EC_Global.h"
#include "EC_Game.h"
#include "EC_GameRun.h"
#include "EC_GameUIMan.h"
#include "EC_UIManager.h"
#include "AUILabel.h"

#include "WikiSearchCommand.h"
#include "WikiGuideData.h"

#define new A_DEBUG_NEW

AUI_BEGIN_COMMAND_MAP(CDlgWikiFeature, CDlgWikiByNameBase)
AUI_END_COMMAND_MAP()

AUI_BEGIN_EVENT_MAP(CDlgWikiFeature, CDlgWikiByNameBase)
AUI_ON_EVENT("List_Feature",	WM_LBUTTONDOWN,	OnEventLButtonDown_List)
AUI_ON_EVENT("List_Feature",	WM_KEYDOWN,		OnEventKeyDown_List)
AUI_END_EVENT_MAP()

CDlgWikiFeature::CDlgWikiFeature()
:m_pList(NULL)
{
}

bool CDlgWikiFeature::OnInitDialog()
{
	if (!CDlgWikiByNameBase::OnInitDialog())
		return false;

	DDX_Control("List_Feature", m_pList);
	
	return true;
}

void CDlgWikiFeature::ResetSearchCommand()
{
	CDlgWikiByNameBase::ResetSearchCommand();

	SetContentProvider(&WikiSerialDataProvider(0, WikiGuideData().Feature().size()));

	ACString szTxt = m_pTxt_Search->GetText();
	
#ifdef _DEBUG
	int max, min;
	if(a_sscanf(szTxt, _AL("%d-%d"), &min, &max) == 2)
	{
		SetSearchCommand(&WikiSearchFeatureByLevel(min, max));
	}
	else
#endif
	{
		SetSearchCommand(&WikiSearchFeatureByName());
	}
}

void CDlgWikiFeature::OnBeginSearch()
{
 	m_pList->ResetContent();
 	OnEventLButtonDown_List(0, 0, m_pList);
}

bool CDlgWikiFeature::OnAddSearch(WikiEntityPtr p)
{
	WikiSerialDataProvider::Entity* pEE = dynamic_cast<WikiSerialDataProvider::Entity*>(p.Get());
	ASSERT(pEE); // should always got the ptr
	if(!pEE) return false;

	ASSERT(m_pList->GetCount() < GetPageSize());

	const WikiGuideFeature* pData = NULL;
	if(!pEE->GetConstData(pData))
	{
		return false;
	}

	ACString strItem;	
	strItem.Format(_AL("%s"), pData->Name);
	m_pList->AddString(strItem);
	m_pList->SetItemData(m_pList->GetCount()-1, pEE->GetID());
	
	return true;
}

void CDlgWikiFeature::OnEndSearch()
{
	CDlgWikiByNameBase::OnEndSearch();

	if (m_pList->GetCount() <= 0)
	{
		return;
	}

	m_pList->SetCurSel(0);

	OnEventLButtonDown_List(0, 0, m_pList);
}

void CDlgWikiFeature::OnEventLButtonDown_List(WPARAM wParam, LPARAM lParam, AUIObject *pObj)
{
	// mapping to the properties, put them here to make the header clean.
	PAUILABEL pTxt_Name = 
		dynamic_cast<PAUILABEL>(GetDlgItem("Txt_Name"));
	PAUILABEL pTxt_Type = 
		dynamic_cast<PAUILABEL>(GetDlgItem("Txt_Type"));
	PAUILABEL pTxt_StartLevel = 
		dynamic_cast<PAUILABEL>(GetDlgItem("Txt_StartLevel"));
	PAUILABEL pTxt_Area = 
		dynamic_cast<PAUILABEL>(GetDlgItem("Txt_Area"));
	PAUITEXTAREA pTxt_Npc = 
		dynamic_cast<PAUITEXTAREA>(GetDlgItem("Txt_Link_NPC"));
	PAUILABEL pTxt_Award = 
		dynamic_cast<PAUILABEL>(GetDlgItem("Txt_Award"));
	PAUITEXTAREA pTxt_Desc = 
		dynamic_cast<PAUITEXTAREA>(GetDlgItem("Txt_Desc"));

	const abase::vector<WikiGuideFeature>& features = WikiGuideData().Feature();
	unsigned int id = m_pList->GetItemData(m_pList->GetCurSel());

	if(id < features.size())
	{
		const WikiGuideFeature& feature = features[id];

		pTxt_Name->SetText(feature.Name);
		pTxt_Type->SetText(feature.Type);

		ACString szTxt;
		pTxt_StartLevel->SetText(szTxt.Format(_AL("%d"), feature.LevelStart));
		pTxt_Award->SetText(feature.Award);
		pTxt_Area->SetText(feature.Area);
		pTxt_Npc->SetText(feature.Npc);
		pTxt_Desc->SetText(feature.Desc);
	}
	else
	{
		pTxt_Name->SetText(_AL(""));
		pTxt_Type->SetText(_AL(""));
		pTxt_StartLevel->SetText(_AL(""));
		pTxt_Award->SetText(_AL(""));
		pTxt_Area->SetText(_AL(""));
		pTxt_Npc->SetText(_AL(""));
		pTxt_Desc->SetText(_AL(""));
	}
}

void CDlgWikiFeature::OnEventKeyDown_List(WPARAM wParam, LPARAM lParam, AUIObject *pObj)
{
	if (wParam == VK_UP ||
		wParam == VK_DOWN)
	{
		OnEventLButtonDown_List(0, 0, NULL);
	}
}

void CDlgWikiFeature::OnCommandCancel()
{
	//
	CDlgWikiByNameBase::OnCommandCancel();
}

bool CDlgWikiFeature::Release(void)
{
	//
	return CDlgWikiByNameBase::Release();
}
