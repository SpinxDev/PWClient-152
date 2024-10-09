// Filename	: DlgWikiGuide.cpp
// Creator	: Feng Ning
// Date		: 2010/09/25

#include "DlgWikiGuide.h"
#include "DlgWikiByNameBase.h"

#include "EC_GameUIMan.h"
#include "EC_HostPlayer.h"
#include "EC_Configs.h"

#include "WikiGuideData.h"
#include "WikiSearchCommand.h"
#include "WikiSearchTask.h"
#include "AUILabel.h"

#define WIKIGUIDE_HIDE_LEVEL 20

#define new A_DEBUG_NEW

AUI_BEGIN_COMMAND_MAP(CDlgWikiGuide, CDlgWikiBase)
AUI_ON_COMMAND("Btn_Skill", OnCommand_DetailButton)
AUI_ON_COMMAND("Btn_Task", OnCommand_DetailButton)
AUI_ON_COMMAND("Btn_Feature", OnCommand_DetailButton)
AUI_ON_COMMAND("Btn_Equipment", OnCommand_DetailButton)
AUI_ON_COMMAND("Btn_Area", OnCommand_DetailButton)
AUI_ON_COMMAND("selectlevel", OnCommand_SelectLevel)
AUI_ON_COMMAND("autohide", OnCommand_AutoHide)
AUI_END_COMMAND_MAP()

AUI_BEGIN_EVENT_MAP(CDlgWikiGuide, CDlgWikiBase)
AUI_END_EVENT_MAP()

CDlgWikiGuide::CDlgWikiGuide()
:m_pCurData(NULL)
,m_pCombo_Level(NULL)

,m_pBtn_Skill(NULL)
,m_pBtn_Task(NULL)
,m_pBtn_Feature(NULL)
,m_pBtn_Equ(NULL)
,m_pBtn_Area(NULL)
,m_pChk_AutoHide(NULL)
{
}

bool CDlgWikiGuide::OnInitDialog()
{
	if (!CDlgWikiBase::OnInitDialog())
		return false;

	DDX_Control("Btn_Skill", m_pBtn_Skill);
	SetWikiData(m_pBtn_Skill, new WikiEntityPtrProvider());

	DDX_Control("Btn_Task", m_pBtn_Task);
	SetWikiData(m_pBtn_Task, new WikiEntityPtrProvider());

	DDX_Control("Btn_Feature", m_pBtn_Feature);
	SetWikiData(m_pBtn_Feature, new WikiEntityPtrProvider());

	DDX_Control("Btn_Equipment", m_pBtn_Equ);
	SetWikiData(m_pBtn_Equ, new WikiEntityPtrProvider());

	DDX_Control("Btn_Area", m_pBtn_Area);
	SetWikiData(m_pBtn_Area, new WikiEntityPtrProvider());

	DDX_Control("Combo_Level", m_pCombo_Level);
	m_pCombo_Level->ResetContent();
	for(int i=0; i < WIKI_LEVEL_LIMIT; i+= 5)
	{
		ACString szTxt;
		szTxt.Format(_AL("%d-%d"), i+1, i+5);
		m_pCombo_Level->AddString(szTxt);
	}
	
	DDX_Control("Chk_AutoHide", m_pChk_AutoHide);
	
	return true;
}

void CDlgWikiGuide::OnShowDialog()
{
	int levelRank = (GetHostPlayer()->GetBasicProps().iLevel - 1) / 5;
	if(levelRank < m_pCombo_Level->GetCount())
	{
		m_pCombo_Level->SetCurSel(levelRank);
		ConfirmSearch();
	}

	m_pChk_AutoHide->Check(GetGame()->GetConfigs()->GetGameSettings().bHideAutoGuide);

	m_pChk_AutoHide->Show(GetHostPlayer()->GetBasicProps().iLevel >= WIKIGUIDE_HIDE_LEVEL);
	PAUIOBJECT pLab_AutoHide = GetDlgItem("Lab_AutoHide");
	pLab_AutoHide->Show(GetHostPlayer()->GetBasicProps().iLevel >= WIKIGUIDE_HIDE_LEVEL);
	
	//
	CDlgWikiBase::OnShowDialog();
}

bool CDlgWikiGuide::Release(void)
{
	SetWikiData(m_pBtn_Skill, NULL);
	SetWikiData(m_pBtn_Task, NULL);
	SetWikiData(m_pBtn_Feature, NULL);
	SetWikiData(m_pBtn_Equ, NULL);
	SetWikiData(m_pBtn_Area, NULL);

	return CDlgWikiBase::Release();
}

int CDlgWikiGuide::GetPageSize() const
{
	// set it a big number for cache
	return 1000000;
}

void CDlgWikiGuide::OnBeginSearch()
{
	CDlgWikiBase::OnBeginSearch();

	ASSERT(m_pCurData);
	if(m_pCurData)
	{
		m_pCurData->Clear();
		// clear result here
	}
}

bool CDlgWikiGuide::TryAddEquipGuide(WikiEntityPtr p)
{
	const WikiGuideEqu* pEqu = NULL;
	if(!p->GetConstData(pEqu))
	{
		return false;
	}

	// handle the equipment data
	size_t max = sizeof(pEqu->Items) / sizeof(pEqu->Items[0]);
	for(size_t i = 0; i < max ; i ++)
	{
		int id = pEqu->Items[i];
		if(id == 0)
		{
			break;
		}

		m_pCurData->Add(new WikiElementDataProvider::Entity(id));
	}

	return true;
}

bool CDlgWikiGuide::OnAddSearch(WikiEntityPtr p)
{
	if(!m_pCurData)
	{
		return false;
	}

	if(TryAddEquipGuide(p))
	{
		return true;
	}

	//
	// operation about other data type
	//

	// direct use this data
	m_pCurData->Add(p);
	
	return true;
}

void CDlgWikiGuide::OnEndSearch()
{
	//
	CDlgWikiBase::OnEndSearch();
}

void CDlgWikiGuide::OnCommandCancel()
{
	//
	GetGameUIMan()->GetDialog("Win_WikiAreaList")->Show(false);
	CDlgWikiBase::OnCommandCancel();
}

WikiEntityPtrProvider* CDlgWikiGuide::GetWikiData(PAUISTILLIMAGEBUTTON pBtn)
{
	WikiEntityPtrProvider* ptr = (WikiEntityPtrProvider*)(pBtn->GetDataPtr());
	return ptr;
}

WikiEntityPtrProvider* CDlgWikiGuide::SetWikiData(PAUISTILLIMAGEBUTTON pBtn, 
												  WikiEntityPtrProvider* ptr)
{
	WikiEntityPtrProvider* oldptr = GetWikiData(pBtn);
	delete oldptr;
	pBtn->SetDataPtr(ptr);
	return ptr;
}

void CDlgWikiGuide::ConfirmSearch()
{
	int sel = m_pCombo_Level->GetCurSel();
	if(sel < 0) return;

	int min = sel * 5 + 1;
	int max = (sel + 1) * 5;

	ACString szTxt;
	PAUILABEL pTxt_Skill = dynamic_cast<PAUILABEL>(GetDlgItem("Txt_Skill"));
	PAUILABEL pTxt_Feature = dynamic_cast<PAUILABEL>(GetDlgItem("Txt_Feature"));
	PAUILABEL pTxt_Task = dynamic_cast<PAUILABEL>(GetDlgItem("Txt_Task"));
	PAUILABEL pTxt_Equ = dynamic_cast<PAUILABEL>(GetDlgItem("Txt_Equipment"));
	PAUILABEL pTxt_Area = dynamic_cast<PAUILABEL>(GetDlgItem("Txt_Area"));

	WikiSearchSuite suite;

	// search skill
	m_pCurData = GetWikiData(m_pBtn_Skill);
	SetContentProvider(&WikiSkillDataProvider());

	// with a class filter
	suite.Clear();
	suite.Add(&WikiSearchSkillByClassLimit(1 << GetHostPlayer()->GetProfession()));
	suite.Add(&WikiSearchSkillByLevelRequire(min, max));
	Refresh(&suite);

	pTxt_Skill->SetText( m_pCurData->Size() ? 
		szTxt.Format(GetStringFromTable(8723), m_pCurData->Size()) : GetStringFromTable(8720) );
	m_pBtn_Skill->Enable( m_pCurData->Size() > 0);

	// search task
	m_pCurData = GetWikiData(m_pBtn_Task);
	SetContentProvider(&WikiTaskDataProvider());

	// with a class filter
	suite.Clear();
	suite.Add(&WikiSearchTaskByClassLimit(1 << GetHostPlayer()->GetProfession()));
	suite.Add(&WikiSearchTaskByLevel(min, max));
	suite.Add(&WikiSearchTaskForGuide());
	Refresh(&suite);

	pTxt_Task->SetText( m_pCurData->Size() ? 
		szTxt.Format(GetStringFromTable(8723), m_pCurData->Size()) : GetStringFromTable(8720) );
	m_pBtn_Task->Enable( m_pCurData->Size() > 0);

	// search feature
	m_pCurData = GetWikiData(m_pBtn_Feature);
	SetContentProvider(&WikiSerialDataProvider(0, WikiGuideData().Feature().size()));
	Refresh(&WikiSearchFeatureByLevel(min, max));
	pTxt_Feature->SetText( m_pCurData->Size() ? 
		szTxt.Format(GetStringFromTable(8723), m_pCurData->Size()) : GetStringFromTable(8720) );
	m_pBtn_Feature->Enable( m_pCurData->Size() > 0);

	// search equipment
	m_pCurData = GetWikiData(m_pBtn_Equ);
	SetContentProvider(&WikiSerialDataProvider(0, WikiGuideData().Equ().size()));
	Refresh(&WikiSearchEquipmentGuide(min, max, 1 << GetHostPlayer()->GetProfession()));
	pTxt_Equ->SetText( m_pCurData->Size() ? 
		szTxt.Format(GetStringFromTable(8723), m_pCurData->Size()) : GetStringFromTable(8720) );
	m_pBtn_Equ->Enable( m_pCurData->Size() > 0);

	// search area
	m_pCurData = GetWikiData(m_pBtn_Area);
	SetContentProvider(&WikiSerialDataProvider(0, WikiGuideData().Area().size()));
	Refresh(&WikiSearchAreaByLevel(min, max));
	pTxt_Area->SetText( m_pCurData->Size() ? 
		szTxt.Format(GetStringFromTable(8723), m_pCurData->Size()) : GetStringFromTable(8720) );
	m_pBtn_Area->Enable( m_pCurData->Size() > 0);

	m_pCurData = NULL;
}

void CDlgWikiGuide::OnCommand_DetailButton(const char* szCommand)
{
	CDlgWikiBase* pDlg = NULL;

	PAUISTILLIMAGEBUTTON pBtn = dynamic_cast<PAUISTILLIMAGEBUTTON>(GetDlgItem(szCommand));
	if(pBtn == m_pBtn_Skill)
	{
		pDlg = dynamic_cast<CDlgWikiBase*>( GetGameUIMan()->GetDialog("Win_WikiSkillSearch") );
	}
	else if(pBtn == m_pBtn_Task)
	{
		pDlg = dynamic_cast<CDlgWikiBase*>( GetGameUIMan()->GetDialog("Win_WikiTaskSearch") );
	}
	else if(pBtn == m_pBtn_Feature)
	{
		pDlg = dynamic_cast<CDlgWikiBase*>( GetGameUIMan()->GetDialog("Win_WikiFeatureSearch") );
	}
	else if(pBtn == m_pBtn_Equ)
	{
		pDlg = dynamic_cast<CDlgWikiBase*>( GetGameUIMan()->GetDialog("Win_WikiEquipmentSearch") );
	}
	else if(pBtn == m_pBtn_Area)
	{
		pDlg = dynamic_cast<CDlgWikiBase*>( GetGameUIMan()->GetDialog("Win_WikiAreaList") );
	}

	ASSERT(pDlg); // should always get this dialog
	if(pDlg)
	{
		WikiEntityPtrProvider* ptr = GetWikiData(pBtn);
		pDlg->SetContentProvider(ptr);
		pDlg->RefreshByNewCommand(&WikiSearchAll(), true);

		// try to show hint if necessary
		CDlgWikiByNameBase* pNameDlg = dynamic_cast<CDlgWikiByNameBase*>(pDlg);
		if(pNameDlg) pNameDlg->ShowHint(true);
	}
}

void CDlgWikiGuide::OnCommand_SelectLevel(const char* szCommand)
{
	ConfirmSearch();
}

void CDlgWikiGuide::OnCommand_AutoHide(const char* szCommand)
{
	EC_GAME_SETTING gs = GetGame()->GetConfigs()->GetGameSettings();
	gs.bHideAutoGuide = m_pChk_AutoHide->IsChecked();
	GetGame()->GetConfigs()->SetGameSettings(gs);
}
