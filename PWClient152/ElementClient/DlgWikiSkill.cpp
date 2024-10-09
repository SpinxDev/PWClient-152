// Filename	: DlgWikiSkill.cpp
// Creator	: Feng Ning
// Date		: 2010/09/14

#include "DlgWikiSkill.h"

#include "AFI.h"
#include "EC_Global.h"
#include "EC_Game.h"
#include "EC_GameRun.h"
#include "EC_UIManager.h"
#include "EC_GameUIMan.h"
#include "EC_HostPlayer.h"

#include "AUICTranslate.h"
#include "AUIImagePicture.h"
#include "AUILabel.h"

#include "WikiSearchCommand.h"
#include "WikiSearchNPC.h"
#include "EC_Skill.h"

#define new A_DEBUG_NEW

AUI_BEGIN_COMMAND_MAP(CDlgWikiSkill, CDlgWikiByNameBase)
AUI_ON_COMMAND("selectclass", OnCommand_SelectClass)
AUI_END_COMMAND_MAP()

AUI_BEGIN_EVENT_MAP(CDlgWikiSkill, CDlgWikiByNameBase)
AUI_ON_EVENT("List_Skill",	WM_LBUTTONDOWN,	OnEventLButtonDown_List)
AUI_ON_EVENT("List_Skill",	WM_KEYDOWN,		OnEventKeyDown_List)
AUI_END_EVENT_MAP()


// =======================================================================
// Link Command
// =======================================================================
CDlgWikiSkill::ShowSpecficLinkCommand::ShowSpecficLinkCommand(unsigned int skill_id, const ACString& name)
:m_SkillID(skill_id)
,m_Name(name)
{
}

CDlgNameLink::LinkCommand* CDlgWikiSkill::ShowSpecficLinkCommand::Clone() const
{
	return new ShowSpecficLinkCommand(m_SkillID, m_Name);
}

bool CDlgWikiSkill::ShowSpecficLinkCommand::operator()(P_AUITEXTAREA_NAME_LINK pLink)
{
	// goto search item
	CDlgWikiSkill* pDlg = dynamic_cast<CDlgWikiSkill*>(g_pGame->GetGameRun()->GetUIManager()->
		GetInGameUIMan()->GetDialog("Win_WikiSkillSearch"));
	ASSERT(pDlg); // should always get this dialog
	if(pDlg) 
	{
		// ensure the provider is correct
		pDlg->SetContentProvider(&WikiSkillDataProvider());
		pDlg->RefreshByNewCommand(&WikiSearchSkillBySpecificID(m_SkillID), true);
		return true;
	}

	return false;
}

void CDlgWikiSkill::ShowSpecficLinkCommand::AppendText()
{
	PAUITEXTAREA pArea = GetTxtArea();
	if(pArea){
		if (a_strlen(pArea->GetText()) > 0){
			pArea->AppendText(_AL(" "));
		}
		pArea->AppendText(this->GetLinkText());
	}
}

ACString CDlgWikiSkill::ShowSpecficLinkCommand::GetLinkText() const
{
	ACString strLinkedName;
	
	strLinkedName.Format(_AL("^00FF00&%s&^FFFFFF"), m_Name);

	return strLinkedName;
}

CDlgWikiSkill::SetNPCNameLinkCommand::SetNPCNameLinkCommand(const NPC_ESSENCE* pNPC)
:m_pNPC(pNPC)
{
}

CDlgNameLink::LinkCommand* CDlgWikiSkill::SetNPCNameLinkCommand::Clone() const
{
	return new SetNPCNameLinkCommand(m_pNPC);
}

bool CDlgWikiSkill::SetNPCNameLinkCommand::operator()(P_AUITEXTAREA_NAME_LINK pLink)
{
	// goto search item
	CDlgWikiSkill* pDlg = dynamic_cast<CDlgWikiSkill*>(g_pGame->GetGameRun()->GetUIManager()->
		GetInGameUIMan()->GetDialog("Win_WikiSkillSearch"));
	ASSERT(pDlg); // should always get this dialog
	if(pDlg) 
	{
		pDlg->SetCurrentNPCLink(m_pNPC);
		return true;
	}
	
	return false;
}

ACString CDlgWikiSkill::SetNPCNameLinkCommand::GetLinkText() const
{
	ACString strLinkedName;
	
	strLinkedName.Format(_AL("^00FF00&%s&^FFFFFF"), m_pNPC->name);
	
	return strLinkedName;
}

// =======================================================================
CDlgWikiSkill::CDlgWikiSkill()
:m_pList(NULL)
,m_pClassList(NULL)
,m_pWikiNPCSkillCache(NULL)
{
}

bool CDlgWikiSkill::OnInitDialog()
{
	if (!CDlgWikiByNameBase::OnInitDialog())
		return false;

	m_pWikiNPCSkillCache = new WikiNPCSkillCache();

	DDX_Control("List_Skill", m_pList);
	
	DDX_Control("Combo_Class", m_pClassList);
	m_pClassList->ResetContent();
	m_pClassList->AddString(GetStringFromTable(8720));
	for (int i=0; i < NUM_PROFESSION; i++)
	{
		m_pClassList->AddString(GetGameRun()->GetProfName(i));
	}

	return true;
}

void CDlgWikiSkill::OnShowDialog()
{
	CDlgWikiByNameBase::OnShowDialog();

	int cls = GetHostPlayer()->GetProfession();
	m_pClassList->SetCurSel(cls + 1);
}

void CDlgWikiSkill::ResetSearchCommand()
{
	CDlgWikiByNameBase::ResetSearchCommand();
	SetContentProvider(&WikiSkillDataProvider());

	WikiSearchSuite suite;

	if(m_pClassList && m_pClassList->GetCurSel() > 0)
	{
		int sel = m_pClassList->GetCurSel() - 1;
		suite.Add(&WikiSearchSkillByClassLimit(1 << sel));
	}

	ACString szTxt = m_pTxt_Search->GetText();
	
#ifdef _DEBUG
	int max, min;
	if(a_sscanf(szTxt, _AL("%d-%d"), &min, &max) == 2)
	{
		suite.Add(&WikiSearchSkillByLevelRequire(min, max));
	}
	else
#endif
	{
		// manually set search pattern in search suite
		WikiSearchSkillByName cmd;
		cmd.SetPattern(szTxt);
		suite.Add(&cmd);
	}

	SetSearchCommand(&suite);
}

void CDlgWikiSkill::OnBeginSearch()
{
 	ClearStoredInfo();
 	OnEventLButtonDown_List(0, 0, m_pList);
}

bool CDlgWikiSkill::OnAddSearch(WikiEntityPtr p)
{

	WikiSkillDataProvider::Entity* pEE = dynamic_cast<WikiSkillDataProvider::Entity*>(p.Get());
	ASSERT(pEE); // should always got the ptr
	if(!pEE) return false;
		
	ASSERT(m_pList->GetCount() < GetPageSize());
	
	CECSkill* pSkill = NULL;
	if(!pEE->GetData(pSkill))
	{
		unsigned int id = pEE->GetID();
		pSkill = new CECSkill(id, 1);
	}
	else
	{
		pSkill = new CECSkill(pSkill->GetSkillID(), pSkill->GetSkillLevel());
	}

	// filter the hidden skill
	if( pSkill->GetRequiredLevel() > WIKI_LEVEL_LIMIT || 
		(!pSkill->IsGoblinSkill() && !pSkill->IsPlayerSkill()) )
	{
		delete pSkill;
		return false;
	}
	
	AUICTranslate trans;
	ACString desc(trans.Translate(pSkill->GetDesc()));

	// skipped the skill without desc
	if(desc.IsEmpty())
	{
		delete pSkill;
		return false;
	}
	
	ACString strItem;
	m_pList->AddString( strItem.Format(GetStringFromTable(1900), 
		WikiSearchSkillByName::GetLocalizedName(pSkill->GetSkillID()), 
		pSkill->GetSkillLevel()) );

	m_pList->SetItemDataPtr(m_pList->GetCount()-1, pSkill);
	m_pList->SetItemHint(m_pList->GetCount()-1, desc);
	
	return true;
}

void CDlgWikiSkill::OnEndSearch()
{
	CDlgWikiByNameBase::OnEndSearch();

	if (m_pList->GetCount() <= 0)
	{
		return;
	}

	m_pList->SetCurSel(0);

	OnEventLButtonDown_List(0, 0, m_pList);
}

void CDlgWikiSkill::SetCurrentNPCLink(const NPC_ESSENCE* pNPC)
{
	PAUITEXTAREA pTxt_Link_Npc = 
		dynamic_cast<PAUITEXTAREA>(GetDlgItem("Txt_Link_NPC"));

	if(pTxt_Link_Npc)
	{
		BindLinkCommand(pTxt_Link_Npc, NULL,
			&CDlgNameLink::MoveToLinkCommand(pNPC->id, pNPC->name));
	}
}

void CDlgWikiSkill::ClearStoredInfo()
{
	for(int i=0;i<m_pList->GetCount();i++)
	{
		CECSkill* pSkill = (CECSkill*)m_pList->GetItemDataPtr(i);
		delete pSkill;
	}
	m_pList->ResetContent();
}

ACString CDlgWikiSkill::GetWeaponLimit(CECSkill& skill)
{
	/*
	1950 "µ¶½£"
	1951 "³¤±ø"
	1952 "¸«´¸"
	1953 "¹­åó"
	1954 "È­Ì×"
	1955 "³¤±Þ"
	1956 "·¨Æ÷"
	1957 "¿ÕÊÖ"
	1958 "Ø°Ê×"

	2000 "½üÉíÎäÆ÷"
	2001 "Ô¶³ÌÎäÆ÷"
	2002 "²»ÏÞ"
	*/

	const size_t maxWeapon = 9;
	int validWeapon[maxWeapon] = {-1};
	const int weaponID[maxWeapon] = { 1, 5, 9, 13, 182, 291, 292, 0, 23749 };
	int validCnt = 0;
	bool isMelee = false;
	bool isLongRange = false;

	for(size_t i=0;i<maxWeapon;i++)
	{
		if( i != 5 && // skip "³¤±Þ"
			skill.ValidWeapon(weaponID[i]) )
		{
			validWeapon[validCnt++] = i;
			if(i == 3) isLongRange = true;
			else isMelee = true;
		}
	}

	if( validCnt < 1 || (validCnt >= 3 && isLongRange && isMelee) )
	{
		return GetStringFromTable(2002);
	}
	else if(validCnt == 1)
	{
		return isLongRange ? GetStringFromTable(2001) : GetStringFromTable(1950 + validWeapon[0]);
	}
	else if(validCnt == 2)
	{
		ACString strTxt;
		return strTxt.Format( _AL("%s, %s"), 
							  GetStringFromTable(1950 + validWeapon[0]), 
							  GetStringFromTable(1950 + validWeapon[1]) );
	}
	else if(isLongRange)
	{
		return GetStringFromTable(2001);
	}
	else if(isMelee)
	{
		return GetStringFromTable(2000);
	}

	ASSERT(false);
	return GetStringFromTable(786);
}

void CDlgWikiSkill::OnEventLButtonDown_List(WPARAM wParam, LPARAM lParam, AUIObject *pObj)
{

	// mapping to the properties, put them here to make the header clean.
	PAUILABEL pTxt_Name = 
		dynamic_cast<PAUILABEL>(GetDlgItem("Txt_Name"));
	PAUILABEL pTxt_LevelRequire1 = 
		dynamic_cast<PAUILABEL>(GetDlgItem("Txt_Level1"));
	PAUILABEL pTxt_LevelRequire2 = 
		dynamic_cast<PAUILABEL>(GetDlgItem("Txt_Level2"));
	PAUITEXTAREA pTxt_PreSkill = 
		dynamic_cast<PAUITEXTAREA>(GetDlgItem("Txt_Link_PreSkill"));
	PAUILABEL pTxt_MoneyRequire = 
		dynamic_cast<PAUILABEL>(GetDlgItem("Txt_MoneyRequire"));
	PAUILABEL pTxt_SpRequire = 
		dynamic_cast<PAUILABEL>(GetDlgItem("Txt_SpRequire"));
	PAUITEXTAREA pTxt_Link_Npc = 
		dynamic_cast<PAUITEXTAREA>(GetDlgItem("Txt_Link_NPC"));

	PAUILABEL pTxt_Type = 
		dynamic_cast<PAUILABEL>(GetDlgItem("Txt_Type"));
	PAUILABEL pTxt_Weapon = 
		dynamic_cast<PAUILABEL>(GetDlgItem("Txt_Weapon"));
	PAUILABEL pTxt_FirstLevel = 
		dynamic_cast<PAUILABEL>(GetDlgItem("Txt_FirstLevel"));
	PAUILABEL pTxt_MaxLevel = 
		dynamic_cast<PAUILABEL>(GetDlgItem("Txt_MaxLevel"));
	
	PAUIIMAGEPICTURE pIcon = 
		dynamic_cast<PAUIIMAGEPICTURE>(GetDlgItem("Img_Icon"));

	CECSkill* pSkill = (CECSkill*)m_pList->GetItemDataPtr(m_pList->GetCurSel());
	if (pSkill)
	{
		CECSkill& skill = *pSkill;

		ACString strTxt;
		pTxt_Name->SetText( strTxt.Format(GetStringFromTable(1900), 
			WikiSearchSkillByName::GetLocalizedName(skill.GetSkillID()), 
			skill.GetSkillLevel()) );

		pTxt_Type->SetText( GetStringFromTable(1900 + min(50, skill.GetType()) ) );

		pTxt_Weapon->SetText(GetWeaponLimit(skill));
		
		int level1 = skill.GetRequiredLevel();
		pTxt_LevelRequire1->SetText(level1 > 0 ? strTxt.Format(GetStringFromTable(1899), level1) : GetStringFromTable(786));
		
		pTxt_LevelRequire2->SetText( GetStringFromTable(1001 + min(300, skill.GetRank())) );

		int firstLevel = GNET::ElementSkill::GetRequiredLevel(skill.GetSkillID(), 1);
		pTxt_FirstLevel->SetText(firstLevel > 0 ? strTxt.Format(GetStringFromTable(1899), firstLevel) : GetStringFromTable(786));

		pTxt_MaxLevel->SetText(strTxt.Format(_AL("%d"), skill.GetMaxLevel()));

		SkillArrayWrapper preSkills = skill.GetRequiredSkill();
		if(!preSkills.Empty())
		{
			pTxt_PreSkill->SetText(_AL(""));
			ACString strSkillName;
			for (size_t i(0); i < preSkills.Count(); ++ i)
			{
				strSkillName = WikiSearchSkillByName::GetLocalizedName(preSkills[i]);
				BindLinkCommand(pTxt_PreSkill,
					&strSkillName,
					&CDlgWikiSkill::ShowSpecficLinkCommand(preSkills[i], strSkillName));
			}
		}
		else
		{
			BindLinkCommand(pTxt_PreSkill, NULL, NULL);
			pTxt_PreSkill->SetText(GetStringFromTable(786));
		}

		pTxt_MoneyRequire->SetText(strTxt.Format(_AL("%d"), skill.GetRequiredMoney()));

		pTxt_SpRequire->SetText(strTxt.Format(_AL("%d"), skill.GetRequiredSp()));

		pTxt_Link_Npc->SetText(GetStringFromTable(786));
		CDlgWikiNPCList* pDlgNpc = dynamic_cast<CDlgWikiNPCList*>(GetGameUIMan()->GetDialog("Win_WikiNpcSelect"));
		if(pDlgNpc)
		{
			pDlgNpc->SetNPCProvider(&WikiNPCSkillServiceProvider(m_pWikiNPCSkillCache, skill.GetSkillID()), true);
			pDlgNpc->Show(!pDlgNpc->TestEmpty(), true);
		}

		AString strFile;
		af_GetFileTitle(skill.GetIconFile(), strFile);
		strFile.MakeLower();
		pIcon->SetCover( GetGameUIMan()->m_pA2DSpriteIcons[CECGameUIMan::ICONS_SKILL],
						 GetGameUIMan()->m_IconMap[CECGameUIMan::ICONS_SKILL][strFile]
						);
		AUICTranslate trans;
		pIcon->SetHint( trans.Translate(skill.GetDesc()) );
		pIcon->SetData( skill.GetSkillID() );
	}
	else
	{
		pTxt_Name->SetText(_AL(""));
		pTxt_LevelRequire1->SetText(_AL(""));
		pTxt_LevelRequire2->SetText(_AL(""));
		pTxt_PreSkill->SetText(_AL(""));
		BindLinkCommand(pTxt_PreSkill, NULL, NULL);
		pTxt_MoneyRequire->SetText(_AL(""));
		pTxt_SpRequire->SetText(_AL(""));
		pTxt_Link_Npc->SetText(_AL(""));
		BindLinkCommand(pTxt_Link_Npc, NULL, NULL);
		pTxt_Type->SetText(_AL(""));
		pTxt_Weapon->SetText(_AL(""));
		pTxt_FirstLevel->SetText(_AL(""));
		pTxt_MaxLevel->SetText(_AL(""));

		pIcon->SetCover(NULL, -1);
		pIcon->SetHint(_AL(""));
		pIcon->SetData(0);
	}

}

void CDlgWikiSkill::OnEventKeyDown_List(WPARAM wParam, LPARAM lParam, AUIObject *pObj)
{
	if (wParam == VK_UP ||
		wParam == VK_DOWN)
	{
		OnEventLButtonDown_List(0, 0, NULL);
	}
}

void CDlgWikiSkill::OnCommand_SelectClass(const char* szCommand)
{
	OnCommand_SearchByName(NULL);
}

void CDlgWikiSkill::OnCommandCancel()
{
	// 
	CDlgWikiByNameBase::OnCommandCancel();
}

bool CDlgWikiSkill::Release(void)
{
	//

	ClearStoredInfo();

	delete m_pWikiNPCSkillCache;
	m_pWikiNPCSkillCache = NULL;

	return CDlgWikiByNameBase::Release();
}

// =======================================================================
// WikiNPCSkillCache
// =======================================================================
WikiNPCSkillCache::WikiNPCSkillCache()
{
	SetContentProvider(&WikiElementDataProvider(ID_SPACE_ESSENCE));
	Refresh(&WikiSearchNPCByServiceSkill());
}

int  WikiNPCSkillCache::GetPageSize() const
{
	// set it a big number for cache
	return 1000000;
}

void WikiNPCSkillCache::OnBeginSearch()
{
	m_NPCCache.clear();
}

bool WikiNPCSkillCache::IsValid(unsigned int skill_id, WikiNPCSkillCache::Info* pData)
{
	CacheType::iterator itr = m_NPCCache.find(skill_id);
	
	if(itr == m_NPCCache.end())
	{
		return false;
	}
	
	if(pData)
	{
		*pData = itr->second;
	}
	
	return true;
}

WikiNPCSkillCache::CacheRange WikiNPCSkillCache::GetValidNPC(unsigned int skill_id)
{
	return m_NPCCache.equal_range(skill_id);
}

bool WikiNPCSkillCache::OnAddSearch(WikiEntityPtr p)
{
	WikiElementDataProvider::Entity* pEE = dynamic_cast<WikiElementDataProvider::Entity*>(p.Get());
	ASSERT(pEE); // should always got the ptr
	if(!pEE) return false;
	
	// get data from entity first
	const NPC_ESSENCE* pEssence = NULL;
	if(!pEE->GetConstData(pEssence)) return false;
	
	unsigned int idSkill = pEssence->id_skill_service;
	if(!idSkill) return false;
	
	DATA_TYPE dataType = DT_INVALID;
	const NPC_SKILL_SERVICE *pSkill = (const NPC_SKILL_SERVICE *)g_pGame->GetElementDataMan()->
		get_data_ptr(idSkill, ID_SPACE_ESSENCE, dataType);
	if(dataType != DT_NPC_SKILL_SERVICE) return false;
	
	size_t maxSkill = sizeof(pSkill->id_skills) / sizeof(pSkill->id_skills[0]);
	WikiNPCSkillCache::Info npcInfo = {pEssence, pSkill};
	
	for(size_t i=0;i<maxSkill;i++)
	{
		unsigned int skillID = pSkill->id_skills[i];
		if(skillID == 0) continue;
		m_NPCCache.insert(CachePair(skillID, npcInfo));
	}
	
	return true;
}


// =======================================================================
// WikiNPCSkillServiceProvider
// =======================================================================
WikiNPCSkillServiceProvider::WikiNPCSkillServiceProvider(WikiNPCSkillCache* pCache, unsigned int skill_id)
:m_pCache(pCache)
,m_SkillID(skill_id)
{
}

WikiEntityPtr WikiNPCSkillServiceProvider::Next(WikiEntityPtr p)
{
	Entity* ptr = dynamic_cast<Entity*>(p.Get());
	ASSERT(ptr);
	if(!ptr) return NULL;
	
	iterator itr = ptr->GetIterator();
	if(itr == m_Range.second) return NULL;
	
	return (++itr == m_Range.second) ? NULL:new Entity(itr);
}

WikiEntityPtr WikiNPCSkillServiceProvider::Begin()
{
	m_Range = m_pCache->GetValidNPC(m_SkillID);
	return (m_Range.first == m_Range.second) ? NULL:new Entity(m_Range.first);
}

WikiSearcher::ContentProvider* WikiNPCSkillServiceProvider::Clone() const
{
	return new WikiNPCSkillServiceProvider(m_pCache, m_SkillID);
}

CDlgNameLink::LinkCommand* WikiNPCSkillServiceProvider::CreateLinkCommand(WikiNPCEssenceProvider::Entity* ptr)
{
	WikiNPCSkillServiceProvider::Entity* pEE = dynamic_cast<WikiNPCSkillServiceProvider::Entity*>(ptr);
	if(!pEE) return NULL;
	
	iterator itr = pEE->GetIterator();
	const NPC_ESSENCE* pNPC = itr->second.pNPC;
	
	return new CDlgWikiSkill::SetNPCNameLinkCommand(pNPC);
}
