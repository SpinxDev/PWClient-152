// Filename	: WikiSearchTask.cpp
// Creator	: Feng Ning
// Date		: 2010/10/29

#include "WikiSearchTask.h"
#include "WikiSearchCommand.h"

#include "EC_Global.h"
#include "EC_Game.h"
#include "EC_GameRun.h"
#include "EC_HostPlayer.h"
#include "EC_TaskInterface.h"
#include "EC_ProfConfigs.h"

#include "DlgTask.h"

#include "TaskTemplMan.h"

#include "elementdataman.h"
#include <set>

#define new A_DEBUG_NEW

#define CONVERT_ENTITY_PTR(T, p, Ret)	T* ptr = dynamic_cast<T*>(p.Get());	\
										ASSERT(ptr);						\
										if(!ptr) return Ret;				\
										// should not input a invalid ptr

//------------------------------------------------------------------------
//
// WikiSearchTaskByName
//
//------------------------------------------------------------------------
ACString WikiSearchTaskByName::GetSearchName(WikiEntityPtr p) const
{
	CONVERT_ENTITY_PTR(WikiTaskDataProvider::Entity, p, false);
	
	ATaskTempl *pTempl = ptr->GetIterator()->second;
	//return pTempl->GetName();
	return CDlgTask::GetTaskNameWithColor(pTempl);
}

WikiSearcher::SearchCommand* WikiSearchTaskByName::Clone() const
{
	WikiSearchTaskByName* pCmd = new WikiSearchTaskByName();
	pCmd->SetPattern(GetPattern());
	return pCmd;
}

//------------------------------------------------------------------------
//
// WikiElementDataChecker
//
//------------------------------------------------------------------------
bool WikiTaskDataChecker::operator()(ATaskTempl* pTempl)
{
	if(!m_bInit) Init();
	
	if(!pTempl) return false;
	
	// task for gamemaster is filtered
	if(pTempl->m_bGM) return false;
	
	// level limitation
	if (pTempl->m_ulPremise_Lev_Min>WIKI_LEVEL_LIMIT) return false;
	
	if (pTempl->m_ulDelvNPC == 0) return false;
	
	if (!pTempl->m_bCanSeekOut) return false;
	
	if(pTempl->m_ulParent != 0) return false;
	
	// banned by designer
	if(!m_Task.empty() && m_Task.find(pTempl->GetID()) != m_Task.end() ) return false;
	
	return true;
}

	
void WikiTaskDataChecker::Init()
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
			
			// record the task id
			int i = 0;
			int size = sizeof(pConfig->task) / sizeof(pConfig->task[0]);
			for(i=0;i<size;i++)
			{
				if(pConfig->task[i]>0) m_Task.insert(pConfig->task[i]);
			}
		}
		configID = pDTMan->get_next_data_id(ID_SPACE_CONFIG, dataType);
	}
}
WikiTaskDataChecker g_Checker;
//------------------------------------------------------------------------
//
// WikiTaskDataProvider
//
//------------------------------------------------------------------------
WikiTaskDataProvider::WikiTaskDataProvider()
{}

WikiTaskDataProvider::Entity* WikiTaskDataProvider::GetNextValidEntity(TaskTemplMap::iterator next)
{
	TaskTemplMap& taskMan = g_pGame->GetTaskTemplateMan()->GetAllTemplMap();
	
	TaskTemplMap::iterator iCandidate = next;
	while(iCandidate != taskMan.end())
	{
		ATaskTempl *pTempl = iCandidate->second;
		if(g_Checker(pTempl))
		{
			return new Entity(iCandidate); 
		}

		++iCandidate;
	}

	return NULL;
}

WikiEntityPtr WikiTaskDataProvider::Next(WikiEntityPtr p)
{
	TaskTemplMap& taskMan = g_pGame->GetTaskTemplateMan()->GetAllTemplMap();
	
	CONVERT_ENTITY_PTR(Entity, p, WikiEntityPtr(NULL));
	TaskTemplMap::iterator iCandidate = ptr->GetIterator();
	if(iCandidate != taskMan.end())
	{
		return GetNextValidEntity(++iCandidate);
	}

	return NULL;
}

WikiEntityPtr WikiTaskDataProvider::Begin()
{
	TaskTemplMap& taskMan = g_pGame->GetTaskTemplateMan()->GetAllTemplMap();

	return GetNextValidEntity(taskMan.begin());
}

WikiSearcher::ContentProvider* WikiTaskDataProvider::Clone() const
{
	return new WikiTaskDataProvider();
}

//------------------------------------------------------------------------
//
// WikiSearchTaskByMonster
//
//------------------------------------------------------------------------
WikiSearchTaskByMonster::WikiSearchTaskByMonster(unsigned int monster_id)
:m_MonsterID(monster_id)
{
	ASSERT(monster_id != 0);
}

bool WikiSearchTaskByMonster::operator()(WikiEntityPtr p) const
{
	if(m_MonsterID == 0) return false;

	CONVERT_ENTITY_PTR(WikiTaskDataProvider::Entity, p, false);

	ATaskTempl *pTempl = ptr->GetIterator()->second;
	if(IsValid(pTempl)) return true;

	pTempl = pTempl->m_pFirstChild;
	while (pTempl)
	{
		if(IsValid(pTempl)) return true;
		pTempl = pTempl->m_pNextSibling;
	}
	
	return false;
}

bool WikiSearchTaskByMonster::IsValid(ATaskTempl* pTempl) const
{
	// search monster
	for (unsigned long j = 0; j < pTempl->m_ulMonsterWanted; j++)
	{
		const MONSTER_WANTED& mw = pTempl->m_MonsterWanted[j];
		if (mw.m_ulMonsterTemplId != 0 &&
			mw.m_ulMonsterNum > 0 &&
			mw.m_ulMonsterTemplId == m_MonsterID)
		{
			return true;
		}
	}
	
	return false;
}

WikiSearcher::SearchCommand* WikiSearchTaskByMonster::Clone() const
{
	return new WikiSearchTaskByMonster(m_MonsterID);
}

//------------------------------------------------------------------------
//
// WikiSearchTaskByItemID
//
//------------------------------------------------------------------------
WikiSearchTaskByItemID::WikiSearchTaskByItemID(unsigned int item_id)
:m_ItemID(item_id)
{
	ASSERT(item_id != 0);
}

bool WikiSearchTaskByItemID::operator()(WikiEntityPtr p) const
{
	if(m_ItemID == 0) return false;
	
	CONVERT_ENTITY_PTR(WikiTaskDataProvider::Entity, p, false);
	
	ATaskTempl *pTempl = ptr->GetIterator()->second;
	if(IsValid(pTempl)) return true;

	pTempl = pTempl->m_pFirstChild;
	while (pTempl)
	{
		if(IsValid(pTempl)) return true;
		pTempl = pTempl->m_pNextSibling;
	}

	return false;
}

bool WikiSearchTaskByItemID::IsValid(ATaskTempl *pTempl) const
{
	// search item
	for (unsigned long j = 0; j < pTempl->m_ulItemsWanted; j++)
	{
		const ITEM_WANTED& iw = pTempl->m_ItemsWanted[j];
		if (iw.m_ulItemTemplId != 0 &&
			iw.m_ulItemNum > 0 &&
			iw.m_ulItemTemplId == m_ItemID)
		{
			return true;
		}
	}
	
	return false;
}

WikiSearcher::SearchCommand* WikiSearchTaskByItemID::Clone() const
{
	return new WikiSearchTaskByItemID(m_ItemID);
}

//------------------------------------------------------------------------
//
// WikiSearchTaskBySpecificID
//
//------------------------------------------------------------------------
WikiSearchTaskBySpecificID::WikiSearchTaskBySpecificID(unsigned int id)
:m_ID(id)
{}

bool WikiSearchTaskBySpecificID::operator()(WikiEntityPtr p) const
{
	if(m_ID == 0) return false;
	
	CONVERT_ENTITY_PTR(WikiTaskDataProvider::Entity, p, false);

	ATaskTempl *pTempl = ptr->GetIterator()->second;
	return pTempl->GetID() == m_ID;
}

WikiSearcher::SearchCommand* WikiSearchTaskBySpecificID::Clone() const
{
	return new WikiSearchTaskBySpecificID(m_ID);
}

//------------------------------------------------------------------------
//
// WikiSearchTaskByLevel
//
//------------------------------------------------------------------------
// search a area guide by level range
//------------------------------------------------------------------------
WikiSearchTaskByLevel::WikiSearchTaskByLevel(int levelmin, int levelmax)
:m_Min(levelmin)
,m_Max(levelmax)
{
	if(m_Min > m_Max && m_Max > 0 )
	{
		m_Min = levelmax;
		m_Max = levelmin;
	}
}

WikiSearcher::SearchCommand* WikiSearchTaskByLevel::Clone() const
{
	return new WikiSearchTaskByLevel(m_Min, m_Max);
}

bool WikiSearchTaskByLevel::operator()(WikiEntityPtr p) const
{
	CONVERT_ENTITY_PTR(WikiTaskDataProvider::Entity, p, false);

	ATaskTempl *pTempl = ptr->GetIterator()->second;

	int levelMin = pTempl->m_ulPremise_Lev_Min;
	int levelMax = pTempl->m_ulPremise_Lev_Max;

	if( levelMin > 0 && m_Max > 0 && m_Max < levelMin ) return false;
	if( levelMax > 0 && m_Min > 0 && m_Min > levelMax ) return false;

	return true;
}

//------------------------------------------------------------------------
//
// WikiSearchTaskByClassLimit
//
//------------------------------------------------------------------------
WikiSearchTaskByClassLimit::WikiSearchTaskByClassLimit(unsigned int clslimit)
:m_ClassLimit(clslimit)
{
	m_ClassLimit &= CECProfConfig::Instance().GetAllProfessionMask();
}

WikiSearcher::SearchCommand* WikiSearchTaskByClassLimit::Clone() const
{
	return new WikiSearchTaskByClassLimit(m_ClassLimit);
}

bool WikiSearchTaskByClassLimit::operator()(WikiEntityPtr p) const
{
	if(m_ClassLimit == 0)
	{
		return false;
	}

	if(m_ClassLimit == CECProfConfig::Instance().GetAllProfessionMask())
	{
		return true;
	}

	CONVERT_ENTITY_PTR(WikiTaskDataProvider::Entity, p, false);
	ATaskTempl *pTempl = ptr->GetIterator()->second;
	// check by class limitation
	unsigned long ulOccupation = min(MAX_OCCUPATIONS, pTempl->m_ulOccupations);
	unsigned long* pOccupations = pTempl->m_Occupations;
	if(!pTempl->m_bShowByOccup || ulOccupation == 0)
	{
		return true;
	}

	unsigned int bits = 0;
	for (size_t i=0; i < ulOccupation; i++)
	{
		unsigned long cls = pOccupations[i];
		bits |= 1 << pOccupations[i];
	}
	bits &=  CECProfConfig::Instance().GetAllProfessionMask();

	return 0 != (m_ClassLimit & bits);
}

//------------------------------------------------------------------------
//
// WikiSearchTaskForGuide
//
//------------------------------------------------------------------------
// search a task for wiki guide
//------------------------------------------------------------------------
WikiSearchTaskForGuide::WikiSearchTaskForGuide()
{}

WikiSearcher::SearchCommand* WikiSearchTaskForGuide::Clone() const
{
	return new WikiSearchTaskForGuide();
}

bool WikiSearchTaskForGuide::operator()(WikiEntityPtr p) const
{
	WikiTaskDataProvider::Entity* ptr = dynamic_cast<WikiTaskDataProvider::Entity*>(p.Get());
	ASSERT(ptr);
	if(!ptr) return false;
	
	ATaskTempl *pTempl = ptr->GetIterator()->second;
	switch(pTempl->GetType())
	{
	case enumTTBranch:// 1）普通中的 非重复任务
		if(pTempl->m_bCanRedo)
		{
			return false;
		}
		break;

	case enumTTMajor:
	case enumTTLevel2:
	case enumTTEvent:
		break;
		
	default:
		return false;
	}

	CECTaskInterface *pTask = g_pGame->GetGameRun()->GetHostPlayer()->GetTaskInterface();
	unsigned long ret = pTempl->CheckPrerequisite(pTask, 
		static_cast<ActiveTaskList*>(pTask->GetActiveTaskList()), 
		pTask->GetCurTime(), true, true, false

		// ignore the level requirement
		, false);
	
	if(   ret != 0 ||
		  // still check this
		  pTempl->CheckLevel(pTask) == TASK_PREREQU_FAIL_ABOVE_LEVEL )
	{
		return false;
	}

	return true;
}