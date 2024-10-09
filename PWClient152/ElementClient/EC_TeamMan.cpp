/*
 * FILE: EC_TeamMan.cpp
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Duyuxin, 2004/12/13
 *
 * HISTORY: 
 *
 * Copyright (c) 2004 Archosaur Studio, All Rights Reserved.
 */

#pragma warning (disable: 4284)

#include "EC_Global.h"
#include "EC_TeamMan.h"
#include "EC_Team.h"

#define new A_DEBUG_NEW

///////////////////////////////////////////////////////////////////////////
//	
//	Define and Macro
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Reference to External variables and functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Local Types and Variables and Global variables
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Local functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Implement CECTeamMan
//	
///////////////////////////////////////////////////////////////////////////

CECTeamMan::CECTeamMan() : m_TeamTab(16)
{
}

CECTeamMan::~CECTeamMan()
{
}

//	Release object
void CECTeamMan::Release()
{
	//	Release all teams
	TeamTable::iterator it = m_TeamTab.begin();
	for (; it != m_TeamTab.end(); ++it)
	{
		CECTeam* pTeam = *it.value();
		delete pTeam;
	}

	m_TeamTab.clear();
}

//	Create team
CECTeam* CECTeamMan::CreateTeam(int idLeader)
{
	CECTeam* pTeam = new CECTeam(idLeader);
	if (!pTeam)
	{
		glb_ErrorOutput(ECERR_NOTENOUGHMEMORY, "CECTeamMan::CreateTeam", __LINE__);
		return NULL;
	}

	//	Add to team table
	m_TeamTab.put(idLeader, pTeam);

	return pTeam;
}

//	Release team
void CECTeamMan::ReleaseTeam(int idLeader)
{
	CECTeam* pTeam = GetTeam(idLeader);
	if (!pTeam)
		return;

	delete pTeam;

	//	Remove from table
	m_TeamTab.erase(idLeader);
}

//	Get team
CECTeam* CECTeamMan::GetTeam(int idLeader)
{
	//	Team exists ?
	TeamTable::pair_type Pair = m_TeamTab.get(idLeader);
	if (!Pair.second)
		return NULL;	//	Counldn't find this team

	return *Pair.first;
}

//	Change team leader
void CECTeamMan::ChangeTeamLeader(int idOldLeader, int idNewLeader)
{
	CECTeam* pTeam = GetTeam(idOldLeader);
	if (pTeam)
	{
		pTeam->ChangeTeamLeader(idNewLeader);

		m_TeamTab.erase(idOldLeader);
		m_TeamTab.put(idNewLeader, pTeam);
	}
}

