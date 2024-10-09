// Filename	: DlgWikiMonsterTask.cpp
// Creator	: Xu Wenbin
// Date		: 2010/04/07

#include "DlgWikiMonsterTask.h"
#include "WikiSearchTask.h"

#define new A_DEBUG_NEW

AUI_BEGIN_COMMAND_MAP(CDlgWikiMonsterTask, CDlgWikiTaskList)
AUI_END_COMMAND_MAP()

AUI_BEGIN_EVENT_MAP(CDlgWikiMonsterTask, CDlgWikiTaskList)
AUI_END_EVENT_MAP()

CDlgWikiMonsterTask::CDlgWikiMonsterTask()
{
}

void CDlgWikiMonsterTask::SetSearchDataPtr(void* ptr)
{
	const MONSTER_ESSENCE *pEssence = (const MONSTER_ESSENCE *)(ptr);
	SetSearchCommand(!pEssence ? NULL : &WikiSearchTaskByMonster(pEssence->id));
	CDlgWikiItemListBase::SetSearchDataPtr(ptr);
}