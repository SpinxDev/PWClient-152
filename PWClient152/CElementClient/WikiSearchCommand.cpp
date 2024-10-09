// Filename	: WikiSearchCommand.cpp
// Creator	: Feng Ning
// Date		: 2010/04/26

#include "WikiSearchCommand.h"
#include "AWStringWithWildcard.h"

#include "EC_Game.h"
#include "EC_Global.h"
#include "EC_Utility.h"
#include "elementdataman.h"

#include "ElementSkill.h"
#include "EC_Skill.h"
#include "EC_RoleTypes.h"
#include "EC_ProfConfigs.h"

#include "WikiEquipmentProp.h"
#include "WikiItemProp.h"
#include "WikiGuideData.h"

#include <set>

#define new A_DEBUG_NEW

#define CONVERT_ENTITY_PTR(T, p, Ret)	T* ptr = dynamic_cast<T*>(p.Get());	\
										ASSERT(ptr);						\
										if(!ptr) return Ret;				\
										// should not input a invalid ptr

//------------------------------------------------------------------------
//
// WikiSearchSuite
//
//------------------------------------------------------------------------
WikiSearchSuite::WikiSearchSuite()
{}

WikiSearchSuite::~WikiSearchSuite()
{
	Clear();
}

WikiSearcher::SearchCommand* WikiSearchSuite::Clone() const
{
	WikiSearchSuite* pSuite = new WikiSearchSuite();
	if(!IsEmpty())
	{
		ALISTPOSITION pos = m_Cmds.GetHeadPosition();
		while(pos)
		{
			WikiSearcher::SearchCommand* pCmd = m_Cmds.GetNext(pos);
			pSuite->m_Cmds.AddTail(pCmd->Clone());
		}
	}

	return pSuite;
}

bool WikiSearchSuite::operator()(WikiEntityPtr p) const
{
	if(!IsEmpty())
	{
		ALISTPOSITION pos = m_Cmds.GetHeadPosition();
		while(pos)
		{
			WikiSearcher::SearchCommand* pCmd = m_Cmds.GetNext(pos);
			if( !(*pCmd)( p ) )
			{
				return false;
			}
		}
	}

	return true;
}
	
void WikiSearchSuite::Add(const WikiSearcher::SearchCommand* pCommand)
{
	m_Cmds.AddTail(pCommand->Clone());
}

bool WikiSearchSuite::IsEmpty() const
{
	return m_Cmds.GetCount() == 0;
}

void WikiSearchSuite::Clear()
{
	if(!IsEmpty())
	{
		ALISTPOSITION pos = m_Cmds.GetHeadPosition();
		while(pos)
		{
			WikiSearcher::SearchCommand* pCmd = m_Cmds.GetNext(pos);
			delete pCmd;
		}
		m_Cmds.RemoveAll();
	}
}

//------------------------------------------------------------------------
//
// WikiEntityPtrProvider
//
//------------------------------------------------------------------------
WikiEntityPtrProvider::WikiEntityPtrProvider()
{
	m_CurPtr = m_Entities.end();
}

WikiEntityPtrProvider::WikiEntityPtrProvider(const PtrVector& entities)
:m_Entities(entities)
{
	m_CurPtr = m_Entities.end();
}

WikiEntityPtr WikiEntityPtrProvider::Next(WikiEntityPtr p)
{
	if( !p ) return NULL;

	PtrVector::iterator itr = m_CurPtr;

	if( m_CurPtr != m_Entities.end() && *m_CurPtr == p)
	{
		++itr;
	}
	else
	{
		// may very slow...
		for( itr = m_Entities.begin(); itr != m_Entities.end(); ++itr)
		{
			if( (*itr).Get() == p.Get() )
			{
				++itr;
				break;
			}
		}
	}

	m_CurPtr = itr;
	return (m_CurPtr == m_Entities.end()) ? NULL : *m_CurPtr;
}

WikiEntityPtr WikiEntityPtrProvider::Begin()
{
	return m_Entities.empty() ? NULL : *(m_CurPtr = m_Entities.begin());
}

WikiSearcher::ContentProvider* WikiEntityPtrProvider::Clone() const
{
	return m_Entities.empty() ? new WikiEntityPtrProvider() : 
								new WikiEntityPtrProvider(m_Entities);
}

void WikiEntityPtrProvider::Clear()
{
	m_Entities.clear();
	m_CurPtr = m_Entities.end();
}

void WikiEntityPtrProvider::Add(WikiEntityPtr p)
{
	m_Entities.push_back(p);
	m_CurPtr = m_Entities.end();
}

void WikiEntityPtrProvider::Swap(PtrVector& entities)
{
	m_Entities.swap(entities);
	m_CurPtr = m_Entities.end();
}

//------------------------------------------------------------------------
//
// WikiSerialDataProvider
//
//------------------------------------------------------------------------
WikiSerialDataProvider::WikiSerialDataProvider(int start, int count)
:m_Start(start)
,m_Count(count)
{
	
}

WikiEntityPtr WikiSerialDataProvider::Next(WikiEntityPtr p)
{
	CONVERT_ENTITY_PTR(Entity, p, WikiEntityPtr(NULL));

	int inc = (m_Count>0 ? 1:-1);
	int maxIndex = m_Start + m_Count - inc;

	int cur = ptr->GetID() + inc;
	
	if(inc > 0)
	{
		return (cur > maxIndex) ? NULL:new Entity(cur);
	}
	else
	{
		return (cur < maxIndex) ? NULL:new Entity(cur);
	}
}

WikiEntityPtr WikiSerialDataProvider::Begin()
{
	return new Entity(m_Start);
}

WikiSearcher::ContentProvider* WikiSerialDataProvider::Clone() const
{
	return new WikiSerialDataProvider(m_Start, m_Count);
}

//------------------------------------------------------------------------
//
// WikiSearchRandPropOnEquipment
//
//------------------------------------------------------------------------
WikiSearchRandPropOnEquipment::WikiSearchRandPropOnEquipment(unsigned int item_id)
:m_pEP(NULL)
,m_ItemID(item_id)
{
}

WikiSearchRandPropOnEquipment::~WikiSearchRandPropOnEquipment()
{
	if(m_pEP)
	{
		delete m_pEP;
		m_pEP = NULL;
	}
}

bool WikiSearchRandPropOnEquipment::operator()(WikiEntityPtr p) const
{
	// lazy initialize
	if(!m_pEP)
	{
		WikiEquipmentProp* pEP = WikiEquipmentProp::CreateProperty(m_ItemID);
		ASSERT(pEP); // should always get a invalid id
		if(!pEP) return false;

		const_cast<WikiSearchRandPropOnEquipment*>(this)->m_pEP = pEP;
	}
	
	CONVERT_ENTITY_PTR(WikiSerialDataProvider::Entity, p, false);

	unsigned int rand_id = m_pEP->GetRandPropID((unsigned int)ptr->GetID());

	return rand_id != 0;
}

WikiSearcher::SearchCommand* WikiSearchRandPropOnEquipment::Clone() const
{
	return new WikiSearchRandPropOnEquipment(m_ItemID);
}

//------------------------------------------------------------------------
//
// WikiSearchDropItemOnMonster
//
//------------------------------------------------------------------------
WikiSearchDropItemOnMonster::WikiSearchDropItemOnMonster(const MONSTER_ESSENCE* p)
:m_pEssence(p)
{
	ASSERT(p);
}

bool WikiSearchDropItemOnMonster::operator()(WikiEntityPtr p) const
{
	if(!m_pEssence) return false;

	CONVERT_ENTITY_PTR(WikiSerialDataProvider::Entity, p, false);
	
	size_t dropSize = sizeof(m_pEssence->drop_matters) / sizeof(m_pEssence->drop_matters[0]);
	if(ptr->GetID() >= (int)dropSize) return false;

	unsigned int id = m_pEssence->drop_matters[ptr->GetID()].id;

	return id != 0;
}

WikiSearcher::SearchCommand* WikiSearchDropItemOnMonster::Clone() const
{
	return new WikiSearchDropItemOnMonster(m_pEssence);
}

//------------------------------------------------------------------------
//
// WikiElementDataChecker
//
//------------------------------------------------------------------------
class WikiElementDataChecker
{
public:
	WikiElementDataChecker():m_bInit(false){}

	bool operator()(int id_space, int datatype, int id)
	{
		if(!m_bInit) Init();

		if(id_space == ID_SPACE_ESSENCE)
		{
			return m_Space.empty() || m_Space.find(id) == m_Space.end();
		}
		else if( id_space == ID_SPACE_RECIPE)
		{
			return m_Recipe.empty() || m_Recipe.find(id) == m_Recipe.end();
		}

		return true;
	}

	bool operator()(int id_skill)
	{
		if(!m_bInit) Init();
		
		return m_Skill.empty() || m_Skill.find(id_skill) == m_Skill.end();
	}
	
protected:
	std::set<unsigned int> m_Space;
	std::set<unsigned int> m_Recipe;
	std::set<unsigned int> m_Skill;
	bool m_bInit;

	void Init()
	{
		if(m_bInit) return;
		m_bInit = true;
		
		elementdataman *pDTMan = g_pGame->GetElementDataMan();
		DATA_TYPE dataType = DT_INVALID;
		int configID = pDTMan->get_first_data_id(ID_SPACE_CONFIG, dataType);

		while(configID)
		{
			if(dataType == DT_WIKI_TABOO_CONFIG)
			{
				WIKI_TABOO_CONFIG* pConfig = (WIKI_TABOO_CONFIG*)
					pDTMan->get_data_ptr(configID, ID_SPACE_CONFIG, dataType);

				// record the essence id
				int i = 0;
				int size = sizeof(pConfig->essence) / sizeof(pConfig->essence[0]);
				for(i=0;i<size;i++)
				{
					if(pConfig->essence[i]>0) m_Space.insert(pConfig->essence[i]);
				}
				
				// record the recipe id
				i = 0;
				size = sizeof(pConfig->recipe) / sizeof(pConfig->recipe[0]);
				for(i=0;i<size;i++)
				{
					if(pConfig->recipe[i]>0) m_Recipe.insert(pConfig->recipe[i]);
				}

				// record the skill id
				i = 0;
				size = sizeof(pConfig->skill) / sizeof(pConfig->skill[0]);
				for(i=0;i<size;i++)
				{
					if(pConfig->skill[i]>0) m_Skill.insert(pConfig->skill[i]);
				}
			}
			configID = pDTMan->get_next_data_id(ID_SPACE_CONFIG, dataType);
		}
	}
};
static WikiElementDataChecker g_Checker;

//------------------------------------------------------------------------
//
// WikiElementDataProvider
//
//------------------------------------------------------------------------
WikiElementDataProvider::WikiElementDataProvider(unsigned int id_space)
:m_IDSpace(id_space)
{
}

WikiEntityPtr WikiElementDataProvider::Next(WikiEntityPtr p)
{
	elementdataman *pElementDataMan = g_pGame->GetElementDataMan();
	DATA_TYPE dataType = DT_INVALID;
	
	CONVERT_ENTITY_PTR(Entity, p, WikiEntityPtr(NULL));
	unsigned int idCandidate = ptr->GetID();

	if (idCandidate != 0 && idCandidate != pElementDataMan->get_cur_data_id((ID_SPACE)m_IDSpace, dataType))
	{
		// set internal iterator to current position
		unsigned int idTemp = pElementDataMan->get_first_data_id((ID_SPACE)m_IDSpace, dataType);
		while (idTemp && idTemp != idCandidate)
			idTemp = pElementDataMan->get_next_data_id((ID_SPACE)m_IDSpace, dataType);
		
		// current id is invalid
		if(idTemp != idCandidate) return false;
	}
	
	if (idCandidate == 0)
	{
		idCandidate = pElementDataMan->get_first_data_id((ID_SPACE)m_IDSpace, dataType);
	}
	else
	{
		do 
		{
			idCandidate = pElementDataMan->get_next_data_id((ID_SPACE)m_IDSpace, dataType);
		} while (idCandidate && !g_Checker(m_IDSpace, dataType, idCandidate));
	}

	return !idCandidate ? NULL:new Entity(idCandidate);;
}

WikiEntityPtr WikiElementDataProvider::Begin()
{
	return new Entity(0);
}

WikiSearcher::ContentProvider* WikiElementDataProvider::Clone() const
{
	return new WikiElementDataProvider(m_IDSpace);
}

//------------------------------------------------------------------------
//
// WikiSearchBySpecificID
//
//------------------------------------------------------------------------
WikiSearchBySpecificID::WikiSearchBySpecificID(unsigned int id)
:m_ID(id)
{}

bool WikiSearchBySpecificID::operator()(WikiEntityPtr p) const
{
	CONVERT_ENTITY_PTR(WikiElementDataProvider::Entity, p, false);

	return ptr->GetID() == m_ID;
}

WikiSearcher::SearchCommand* WikiSearchBySpecificID::Clone() const
{
	return new WikiSearchBySpecificID(m_ID);
}

//------------------------------------------------------------------------
//
// WikiSearchMonsterByEquipmentDrop
//
//------------------------------------------------------------------------
WikiSearchMonsterDropByItemID::WikiSearchMonsterDropByItemID(unsigned int item_id)
:m_ItemID(item_id)
{}

bool WikiSearchMonsterDropByItemID::operator()(WikiEntityPtr p) const
{
	if(m_ItemID == 0) return false;

	CONVERT_ENTITY_PTR(WikiElementDataProvider::Entity, p, false);

	unsigned int idCandidate = ptr->GetID();
	
	DATA_TYPE dataType = DT_INVALID;
	const MONSTER_ESSENCE *pEssence = 
		(const MONSTER_ESSENCE *)g_pGame->GetElementDataMan()->get_data_ptr(idCandidate, ID_SPACE_ESSENCE, dataType);
	
	if(dataType != DT_MONSTER_ESSENCE)
	{
		return false;
	}

	// filter by the empty name
	ACString name = pEssence->name;
	name.TrimLeft();
	name.TrimRight();
	if(name.IsEmpty()) return false;

	size_t dropSize = sizeof(pEssence->drop_matters) / sizeof(pEssence->drop_matters[0]);
	for(size_t i=0;i<dropSize;i++)
	{
		if(pEssence->drop_matters[i].id == m_ItemID)
		{
			ptr->SetConstData(pEssence);
			return true;
		}
	}
	
	return false;
}

WikiSearcher::SearchCommand* WikiSearchMonsterDropByItemID::Clone() const
{
	return new WikiSearchMonsterDropByItemID(m_ItemID);
}

//------------------------------------------------------------------------
//
// WikiSearchEquipmentByMonsterDrop
//
//------------------------------------------------------------------------
WikiSearchEquipmentByMonsterDrop::WikiSearchEquipmentByMonsterDrop(const MONSTER_ESSENCE* p)
:m_pEssence(p)
{
	ASSERT(m_pEssence);
}

bool WikiSearchEquipmentByMonsterDrop::operator()(WikiEntityPtr p) const
{
	if(!m_pEssence) return false;
	
	CONVERT_ENTITY_PTR(WikiElementDataProvider::Entity, p, false);
	
	unsigned int idCandidate = ptr->GetID();

	size_t dropSize = sizeof(m_pEssence->drop_matters) / sizeof(m_pEssence->drop_matters[0]);
	for(size_t i=0;i<dropSize;i++)
	{
		if(m_pEssence->drop_matters[i].id == idCandidate)
		{
			return true;
		}
	}

	return false;
}

WikiSearcher::SearchCommand* WikiSearchEquipmentByMonsterDrop::Clone() const
{
	return new WikiSearchEquipmentByMonsterDrop(m_pEssence);
}

//------------------------------------------------------------------------
//
// WikiSearchRecipeByProductID
//
//------------------------------------------------------------------------
WikiSearchRecipeByProductID::WikiSearchRecipeByProductID(unsigned int item_id)
:m_ItemID(item_id)
{}

bool WikiSearchRecipeByProductID::operator()(WikiEntityPtr p) const
{
	if(m_ItemID == 0) return false;
	
	CONVERT_ENTITY_PTR(WikiElementDataProvider::Entity, p, false);
	unsigned int idCandidate = ptr->GetID();
	
	DATA_TYPE dataType = DT_INVALID;
	const RECIPE_ESSENCE *pEssence = 
		(const RECIPE_ESSENCE *)g_pGame->GetElementDataMan()->get_data_ptr(idCandidate, ID_SPACE_RECIPE, dataType);
	
	if(dataType != DT_RECIPE_ESSENCE)
	{
		return false;
	}

	size_t maxItem = sizeof(pEssence->targets) / sizeof(pEssence->targets[0]);
	for(size_t i=0;i<maxItem;i++)
	{
		if( pEssence->targets[i].id_to_make == m_ItemID &&
			pEssence->targets[i].probability > 0.f)
		{
			ptr->SetConstData(pEssence);
			return true;
		}
	}

	return false;
}

WikiSearcher::SearchCommand* WikiSearchRecipeByProductID::Clone() const
{
	return new WikiSearchRecipeByProductID(m_ItemID);
}

//------------------------------------------------------------------------
//
// WikiSearchRecipeByMaterialID
//
//------------------------------------------------------------------------
WikiSearchRecipeByMaterialID::WikiSearchRecipeByMaterialID(unsigned int item_id)
:m_ItemID(item_id)
{}

bool WikiSearchRecipeByMaterialID::operator()(WikiEntityPtr p) const
{
	if(m_ItemID == 0) return false;
	
	CONVERT_ENTITY_PTR(WikiElementDataProvider::Entity, p, false);
	unsigned int idCandidate = ptr->GetID();
	
	DATA_TYPE dataType = DT_INVALID;
	const RECIPE_ESSENCE *pEssence = 
		(const RECIPE_ESSENCE *)g_pGame->GetElementDataMan()->get_data_ptr(idCandidate, ID_SPACE_RECIPE, dataType);
	
	if(dataType != DT_RECIPE_ESSENCE)
	{
		return false;
	}
	
	if(pEssence->id_upgrade_equip == m_ItemID)
	{
		return true;
	}

	size_t maxItem = sizeof(pEssence->materials) / sizeof(pEssence->materials[0]);
	for(size_t i=0;i<maxItem;i++)
	{
		if(pEssence->materials[i].id == m_ItemID && pEssence->materials[i].num > 0)
		{
			ptr->SetConstData(pEssence);
			return true;
		}
	}
	
	return false;
}

WikiSearcher::SearchCommand* WikiSearchRecipeByMaterialID::Clone() const
{
	return new WikiSearchRecipeByMaterialID(m_ItemID);
}

//------------------------------------------------------------------------
//
// WikiSearchByName
//
//------------------------------------------------------------------------
WikiSearchByName::WikiSearchByName()
{}

void WikiSearchByName::SetPattern(const ACString& pattern)
{
	m_strPattern = pattern;
	m_strPattern.TrimLeft();
	m_strPattern.TrimRight();
	if(!m_strPattern.IsEmpty())
	{
		m_strPattern = _AL("*") + m_strPattern + _AL("*");
	}
}

bool WikiSearchByName::operator()(WikiEntityPtr p) const
{
	if(m_strPattern.IsEmpty()) return false;

	ACString szName = GetSearchName(p);

	// remove the color code in name string
	int pos = 0;
	const int COLOR_STR_SIZE = 7;
	while(szName.GetLength() > COLOR_STR_SIZE && pos + COLOR_STR_SIZE <= szName.GetLength())
	{
		if(szName[pos] != _AL('^'))
		{
			pos++;
			continue;
		}
		
		A3DCOLOR color;
		ACString szColor = szName.Mid(pos, COLOR_STR_SIZE);
		
		if(!STRING_TO_A3DCOLOR(szColor, color))
		{
			pos++;
			continue;
		}
		
		// skip the color
		if(pos > 0)
		{
			szName = szName.Mid(0, pos) + szName.Mid(pos + COLOR_STR_SIZE);
		}
		else
		{
			szName = szName.Mid(pos + COLOR_STR_SIZE);
		}
	}

	if( szName.IsEmpty() ||
		szName.Find(_AL("GM")) >= 0 ||
		szName.Find(_AL("废")) >= 0 ||
		szName.Find(_AL("测试")) >= 0 )
	{
		return false;
	}
	
	return IsMatch(szName, m_strPattern, false);
}

bool WikiSearchByName::IsSearchPatternValid(const ACHAR *szPattern)
{
	ACString strPattern = szPattern;
	strPattern.TrimLeft();
	strPattern.TrimRight();
	ACString strInvalidArray[] = {_AL("*"), _AL("?"), _AL(" "), _AL("-")};
	for (int i(0); i<sizeof(strInvalidArray)/sizeof(strInvalidArray[0]); ++i)
	{
		const ACString &strInvalid = strInvalidArray[i];
		int lenInvalid = strInvalid.GetLength();
		while (!strPattern.IsEmpty())
		{
			int p = strPattern.Find(strInvalid);
			int len = strPattern.GetLength();
			if (p<0 || p>=len)
				break;
			
			// 从 strPattern 里删除这些，看是否有其它字符剩余，有则允许搜索
			strPattern = strPattern.Left(p) + strPattern.Right(len-p-1);
		}
		if (strPattern.IsEmpty())
			break;
	}
	return !strPattern.IsEmpty();
}

//------------------------------------------------------------------------
//
// WikiSearchEquipmentByName
//
//------------------------------------------------------------------------
ACString WikiSearchEquipmentByName::GetSearchName(WikiEntityPtr p) const
{
	CONVERT_ENTITY_PTR(WikiElementDataProvider::Entity, p, false);
	unsigned int idCandidate = ptr->GetID();

	WikiEquipmentProp* pEP = WikiEquipmentProp::CreateProperty(idCandidate);
	if(pEP)
	{
		ptr->SetData(pEP);
		return pEP->GetName();
	}
	
	return _AL("");
}

WikiSearcher::SearchCommand* WikiSearchEquipmentByName::Clone() const
{
	WikiSearchEquipmentByName* pCmd = new WikiSearchEquipmentByName();
	pCmd->SetPattern(GetPattern());
	return pCmd;
}

//------------------------------------------------------------------------
//
// WikiSearchMonsterByName
//
//------------------------------------------------------------------------
ACString WikiSearchMonsterByName::GetSearchName(WikiEntityPtr p) const
{
	CONVERT_ENTITY_PTR(WikiElementDataProvider::Entity, p, false);
	unsigned int idCandidate = ptr->GetID();

	DATA_TYPE dataType = DT_INVALID;
	const MONSTER_ESSENCE *pEssence = 
		(const MONSTER_ESSENCE *)g_pGame->GetElementDataMan()->get_data_ptr(idCandidate, ID_SPACE_ESSENCE, dataType);
	
	if(dataType == DT_MONSTER_ESSENCE)
	{
		ptr->SetConstData(pEssence);
		return pEssence->name;
	}
	
	return _AL("");
}

WikiSearcher::SearchCommand* WikiSearchMonsterByName::Clone() const
{
	WikiSearchMonsterByName* pCmd = new WikiSearchMonsterByName();
	pCmd->SetPattern(GetPattern());
	return pCmd;
}

//------------------------------------------------------------------------
//
// WikiSearchRecipeByName
//
//------------------------------------------------------------------------
ACString WikiSearchRecipeByName::GetSearchName(WikiEntityPtr p) const
{
	CONVERT_ENTITY_PTR(WikiElementDataProvider::Entity, p, false);
	unsigned int idCandidate = ptr->GetID();
	
	DATA_TYPE dataType = DT_INVALID;
	const RECIPE_ESSENCE *pEssence = 
		(const RECIPE_ESSENCE *)g_pGame->GetElementDataMan()->get_data_ptr(idCandidate, ID_SPACE_RECIPE, dataType);
	
	if(dataType == DT_RECIPE_ESSENCE)
	{
		ptr->SetConstData(pEssence);
		return pEssence->name;
	}
	
	return _AL("");
}

WikiSearcher::SearchCommand* WikiSearchRecipeByName::Clone() const
{
	WikiSearchRecipeByName* pCmd = new WikiSearchRecipeByName();
	pCmd->SetPattern(GetPattern());
	return pCmd;
}

//------------------------------------------------------------------------
//
// WikiSearchItemByName
//
//------------------------------------------------------------------------
WikiSearchItemByName::WikiSearchItemByName(bool ignoreEquip)
:m_NoEquip(ignoreEquip)
{
}

ACString WikiSearchItemByName::GetSearchName(WikiEntityPtr p) const
{
	CONVERT_ENTITY_PTR(WikiElementDataProvider::Entity, p, false);
	unsigned int idCandidate = ptr->GetID();

	WikiItemProp* pEP = WikiItemProp::CreateProperty(idCandidate, m_NoEquip);
	if(pEP)
	{
		ptr->SetData(pEP);
		return pEP->GetName();
	}
	
	return _AL("");
}

WikiSearcher::SearchCommand* WikiSearchItemByName::Clone() const
{
	WikiSearchItemByName* pCmd = new WikiSearchItemByName(m_NoEquip);
	pCmd->SetPattern(GetPattern());
	return pCmd;
}

//------------------------------------------------------------------------
//
// WikiSkillDataProvider
//
//------------------------------------------------------------------------
WikiSkillDataProvider::WikiSkillDataProvider()
{}

WikiEntityPtr WikiSkillDataProvider::Next(WikiEntityPtr p)
{
	CONVERT_ENTITY_PTR(WikiSkillDataProvider::Entity, p, NULL);

	unsigned int cur = ptr->GetID();
	unsigned int next = cur;
	
	do 
	{
		next = GNET::ElementSkill::NextSkill(next);
	} while (next && !g_Checker(next));

	return next > 0 ? new Entity(next) : NULL;
}

WikiEntityPtr WikiSkillDataProvider::Begin()
{
	return new Entity(1);
}

WikiSearcher::ContentProvider* WikiSkillDataProvider::Clone() const
{
	return new WikiSkillDataProvider();
}

//------------------------------------------------------------------------
//
// WikiSearchSkillByName
//
//------------------------------------------------------------------------
ACString WikiSearchSkillByName::GetSearchName(WikiEntityPtr p) const
{
	CONVERT_ENTITY_PTR(WikiSkillDataProvider::Entity, p, NULL);

	unsigned int cur = ptr->GetID();

	CECSkill* pSkill = NULL;
	if(!ptr->GetData(pSkill))
	{
		pSkill = new CECSkill(cur, 1);
		ptr->SetData(pSkill);
	}

	// skipped the name that starts with '@'
	const ACHAR* pName = pSkill->GetName();
	if(*pName == _AL('@'))
	{
		return _AL("");
	}

	// skipped the name that is number only
	while(*pName)
	{
		if(*pName < '0' || *pName > '9')
		{
			break;
		}
		++pName;
	}
	if(!*pName)
	{
		return _AL("");
	}

	return GetLocalizedName(pSkill->GetSkillID());
}

const ACHAR* WikiSearchSkillByName::GetLocalizedName(int skillid)
{
	// because the localization issue we should not use 'pSkill->GetName()'
	return g_pGame->GetSkillDesc()->GetWideString(skillid * 10);
}

WikiSearcher::SearchCommand* WikiSearchSkillByName::Clone() const
{
	WikiSearchSkillByName* pCmd = new WikiSearchSkillByName();
	pCmd->SetPattern(GetPattern());
	return pCmd;
}

//------------------------------------------------------------------------
//
// WikiSearchSkillByLevelRequire
//
//------------------------------------------------------------------------
WikiSearchSkillByLevelRequire::WikiSearchSkillByLevelRequire(int levelmin, int levelmax)
:m_Min(levelmin)
,m_Max(levelmax)
{
	if(m_Min > m_Max && m_Max > 0 )
	{
		m_Min = levelmax;
		m_Max = levelmin;
	}
}

bool WikiSearchSkillByLevelRequire::operator()(WikiEntityPtr p) const
{
	CONVERT_ENTITY_PTR(WikiSkillDataProvider::Entity, p, NULL);
	
	unsigned int cur = ptr->GetID();

	// try to get data from provider
	CECSkill* pSkill = NULL;
	if(!ptr->GetData(pSkill))
	{
		pSkill = new CECSkill(cur, 1);
		ptr->SetData(pSkill);
	}

	int left = pSkill->GetSkillLevel();
	int levelmin = GNET::ElementSkill::GetRequiredLevel(cur, left);
	if( (m_Min <= 0 || levelmin >= m_Min) && 
		(m_Max <= 0 || levelmin <= m_Max) )
	{
		// already set data
		return true;
	}

	int right = pSkill->GetMaxLevel();
	int levelmax = right > left ? GNET::ElementSkill::GetRequiredLevel(cur, right) : levelmin;

	return BinarySearch(left, levelmin, right, levelmax, 0, ptr, cur);
}

bool WikiSearchSkillByLevelRequire::BinarySearch(int left, int levelmin, int right, int levelmax, 
												 int deep, WikiSkillDataProvider::Entity* ptr, unsigned cur) const
{
	if( m_Max > 0 && levelmin > m_Max ) return false;
	if( m_Min > 0 && levelmax < m_Min ) return false;
	
	if(right - left <= 1)
	{
		CECSkill* pSkill = NULL;

		if(m_Min <= 0 || levelmin >= m_Min) pSkill = new CECSkill(cur, left);
		else if(m_Max <= 0 || levelmax <= m_Max) pSkill = new CECSkill(cur, right);

		ptr->SetData(pSkill);
		return pSkill != NULL;
	}

	if( m_Min > 0 && levelmin == m_Min )
	{
		ptr->SetData(new CECSkill(cur, left));
		return true;
	}
	
	int middle = left + (right - left ) / 2;
	int levelmid = GNET::ElementSkill::GetRequiredLevel(cur, middle);

	return BinarySearch(left, levelmin, middle, levelmid, deep + 1, ptr, cur) ||
		   BinarySearch(middle, levelmid, right, levelmax,  deep + 1, ptr, cur) ;
}

WikiSearcher::SearchCommand* WikiSearchSkillByLevelRequire::Clone() const
{
	return new WikiSearchSkillByLevelRequire(m_Min, m_Max);
}

//------------------------------------------------------------------------
//
// WikiSearchSkillBySpecificID
//
//------------------------------------------------------------------------
WikiSearchSkillBySpecificID::WikiSearchSkillBySpecificID(unsigned int id)
:m_ID(id)
{}

bool WikiSearchSkillBySpecificID::operator()(WikiEntityPtr p) const
{
	CONVERT_ENTITY_PTR(WikiSkillDataProvider::Entity, p, NULL);
	
	if(ptr->GetID() == m_ID)
	{
		ptr->SetData(new CECSkill(m_ID, 1));
		return true;
	}

	return false;
}

WikiSearcher::SearchCommand* WikiSearchSkillBySpecificID::Clone() const
{
	return new WikiSearchSkillBySpecificID(m_ID);
}

//------------------------------------------------------------------------
//
// WikiSearchSkillByClassLimit
//
//------------------------------------------------------------------------
WikiSearchSkillByClassLimit::WikiSearchSkillByClassLimit(unsigned int clslimit)
:m_ClassLimit(clslimit)
{
	m_ClassLimit &= CECProfConfig::Instance().GetAllProfessionMask();
}

bool WikiSearchSkillByClassLimit::operator()(WikiEntityPtr p) const
{
	if(m_ClassLimit == 0)
	{
		return false;
	}
	
	if(m_ClassLimit == CECProfConfig::Instance().GetAllProfessionMask())
	{
		return true;
	}

	CONVERT_ENTITY_PTR(WikiSkillDataProvider::Entity, p, NULL);
	
	CECSkill* pSkill = NULL;
	if(!ptr->GetData(pSkill))
	{
		pSkill = new CECSkill(ptr->GetID(), 1);
		ptr->SetData(pSkill);
	}
	
	return 0 != (m_ClassLimit & (1 << pSkill->GetCls()));
}

WikiSearcher::SearchCommand* WikiSearchSkillByClassLimit::Clone() const
{
	return new WikiSearchSkillByClassLimit(m_ClassLimit);
}

//------------------------------------------------------------------------
//
// WikiSearchFeatureByName
//
//------------------------------------------------------------------------
// search feature by its name
//------------------------------------------------------------------------
ACString WikiSearchFeatureByName::GetSearchName(WikiEntityPtr p) const
{
	CONVERT_ENTITY_PTR(WikiSerialDataProvider::Entity, p, false);
	unsigned int id = ptr->GetID();
	
	const abase::vector<WikiGuideFeature>& features = WikiGuideData().Feature();
	if(id >= features.size())
	{
		return _AL("");
	}
	
	const WikiGuideFeature& feature = features[id];
	ptr->SetConstData(&feature);
	return feature.Name;
}

WikiSearcher::SearchCommand* WikiSearchFeatureByName::Clone() const
{
	WikiSearchFeatureByName* pCmd = new WikiSearchFeatureByName();
	pCmd->SetPattern(GetPattern());
	return pCmd;
}

//------------------------------------------------------------------------
//
// WikiSearchFeatureByLevel
//
//------------------------------------------------------------------------
WikiSearchFeatureByLevel::WikiSearchFeatureByLevel(int levelmin, int levelmax)
:m_Min(levelmin)
,m_Max(levelmax)
{
	if(m_Min > m_Max && m_Max > 0 )
	{
		m_Min = levelmax;
		m_Max = levelmin;
	}
}

WikiSearcher::SearchCommand* WikiSearchFeatureByLevel::Clone() const
{
	return new WikiSearchFeatureByLevel(m_Min, m_Max);
}

bool WikiSearchFeatureByLevel::operator()(WikiEntityPtr p) const
{
	CONVERT_ENTITY_PTR(WikiSerialDataProvider::Entity, p, false);
	unsigned int id = ptr->GetID();
	
	const abase::vector<WikiGuideFeature>& guides = WikiGuideData().Feature();
	if(id >= guides.size())
	{
		return false;
	}
	
	const WikiGuideFeature& guide = guides[id];
	if( guide.LevelMin > 0 && m_Max > 0 && guide.LevelMin > m_Max ) return false;
	if( guide.LevelMax > 0 && m_Min > 0 && guide.LevelMax < m_Min ) return false;
		
	ptr->SetConstData(&guide);
	return true;
}


//------------------------------------------------------------------------
//
// WikiSearchEquipmentGuide
//
//------------------------------------------------------------------------
WikiSearchEquipmentGuide::WikiSearchEquipmentGuide(int levelmin, int levelmax, unsigned int clslimit)
:m_Min(levelmin)
,m_Max(levelmax)
,m_ClassLimit(clslimit)
{
	m_ClassLimit &= CECProfConfig::Instance().GetAllProfessionMask();
	
	// swap to ensure the sequence
	if(m_Min > m_Max && m_Max > 0 )
	{
		m_Min = levelmax;
		m_Max = levelmin;
	}
}

WikiSearcher::SearchCommand* WikiSearchEquipmentGuide::Clone() const
{
	return new WikiSearchEquipmentGuide(m_Min, m_Max, m_ClassLimit);
}

bool WikiSearchEquipmentGuide::operator()(WikiEntityPtr p) const
{
	if( m_ClassLimit == 0 )
	{
		return false;
	}
	
	CONVERT_ENTITY_PTR(WikiSerialDataProvider::Entity, p, false);
	unsigned int id = ptr->GetID();
	
	const abase::vector<WikiGuideEqu>& guides = WikiGuideData().Equ();
	if(id >= guides.size())
	{
		return false;
	}
	
	const WikiGuideEqu& guide = guides[id];
	
	if( guide.LevelMin > 0 && m_Max > 0 && guide.LevelMin > m_Max ) return false;
	if( guide.LevelMax > 0 && m_Min > 0 && guide.LevelMax < m_Min ) return false;
	if( !(m_ClassLimit & (1 << guide.Prof)) ) return false;
	
	ptr->SetConstData(&guide);
	return true;
}

//------------------------------------------------------------------------
//
// WikiSearchAreaByLevel
//
//------------------------------------------------------------------------
// search a area guide by level range
//------------------------------------------------------------------------
WikiSearchAreaByLevel::WikiSearchAreaByLevel(int levelmin, int levelmax)
:m_Min(levelmin)
,m_Max(levelmax)
{
	if(m_Min > m_Max && m_Max > 0 )
	{
		m_Min = levelmax;
		m_Max = levelmin;
	}
}

WikiSearcher::SearchCommand* WikiSearchAreaByLevel::Clone() const
{
	return new WikiSearchAreaByLevel(m_Min, m_Max);
}

bool WikiSearchAreaByLevel::operator()(WikiEntityPtr p) const
{
	CONVERT_ENTITY_PTR(WikiSerialDataProvider::Entity, p, false);
	unsigned int id = ptr->GetID();
	
	const abase::vector<WikiGuideArea>& guides = WikiGuideData().Area();
	if(id >= guides.size())
	{
		return false;
	}
	
	const WikiGuideArea& guide = guides[id];
	if( guide.LevelMin > 0 && m_Max > 0 && guide.LevelMin > m_Max ) return false;
	if( guide.LevelMax > 0 && m_Min > 0 && guide.LevelMax < m_Min ) return false;
		
	ptr->SetConstData(&guide);
	return true;
}