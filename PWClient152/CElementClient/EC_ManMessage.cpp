/*
 * FILE: EC_ManMessage.cpp
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Duyuxin, 2004/8/30
 *
 * HISTORY: 
 *
 * Copyright (c) 2004 Archosaur Studio, All Rights Reserved.
 */

#include "EC_Global.h"
#include "EC_ManMessage.h"
#include "EC_GameRun.h"
#include "EC_World.h"

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
//	Implement CECMessageMan
//	
///////////////////////////////////////////////////////////////////////////

CECMessageMan::CECMessageMan(CECGameRun* pGameRun)
{
	m_pGameRun	= pGameRun;
}

CECMessageMan::~CECMessageMan()
{
}

//	Release manager
void CECMessageMan::Release()
{
	//	Remove all un-dispatched messages
	m_MsgList.RemoveAll();
}

//	Tick routine
bool CECMessageMan::Tick(DWORD dwDeltaTime)
{
	int i, iNumMsg = m_MsgList.GetCount();

	//	Dispatch messages
	for (i=0; i < iNumMsg; i++)
	{
		ECMSG Msg = m_MsgList.RemoveHead();

		switch (Msg.iManager)
		{
		case -1:
			
			m_pGameRun->ProcessMessage(Msg);
			break;

		default:
			{
				//	pWorld 在此循环有可能被释放并重新创建，必须在循环内获取
				CECWorld* pWorld = m_pGameRun->GetWorld();
				ASSERT(pWorld);
				ASSERT(Msg.iManager >= 0 && Msg.iManager < NUM_MANAGER);
				if (pWorld)
				{
					CECManager *pManager = pWorld->GetManager(Msg.iManager);
					if (pManager)
						pManager->ProcessMessage(Msg);
				}
			}

			break;
		}
	}

	return true; 
}

//	Add a message
bool CECMessageMan::AddMessage(const ECMSG& Msg)
{
	m_MsgList.AddTail((ECMSG&)Msg);
	return true;
}

