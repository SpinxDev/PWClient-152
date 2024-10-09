/*
 * FILE: EC_Manager.h
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Duyuxin, 2004/8/30
 *
 * HISTORY: 
 *
 * Copyright (c) 2004 Archosaur Studio, All Rights Reserved.
 */

#pragma once

#include "EC_MsgDef.h"

///////////////////////////////////////////////////////////////////////////
//	
//	Define and Macro
//	
///////////////////////////////////////////////////////////////////////////

//	Manager index
enum
{
	MAN_PLAYER = 0,		//	Player manager
	MAN_NPC,			//	NPC manager
	MAN_MATTER,			//	Matter manager
	MAN_ORNAMENT,		//	Ornament manager
	MAN_SKILLGFX,		//	Skill Gfx Manager
	MAN_ATTACKS,		//	Attack Manager
	MAN_DECAL,			//	Decal manager
	NUM_MANAGER,
};

//	Manager mask
enum
{
	MANMASK_PLAYER		= 0x0001,
	MANMASK_NPC			= 0x0002,
	MANMASK_MATTER		= 0x0004,
	MANMASK_ORNAMENT	= 0x0008,
	MANMASK_SKILLGFX	= 0x0010,
	MANMASK_ATTACKS		= 0x0020,
	MANMASK_DECAL		= 0x0040,
};

///////////////////////////////////////////////////////////////////////////
//	
//	Types and Global variables
//	
///////////////////////////////////////////////////////////////////////////

class CECViewport;
class CECGameRun;
struct ECRAYTRACE;

///////////////////////////////////////////////////////////////////////////
//	
//	Declare of Global functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Class CECManager
//	
///////////////////////////////////////////////////////////////////////////

class CECManager
{
public:		//	Types

public:		//	Constructor and Destructor

	CECManager(CECGameRun* pGameRun);
	virtual ~CECManager() {}

public:		//	Attributes

public:		//	Operations

	//	Release manager
	virtual void Release() {}

	//	Tick routine
	virtual bool Tick(DWORD dwDeltaTime) { return true; }
	//	Render routine
	virtual bool Render(CECViewport* pViewport) { return true; }
	//	RenderForReflected routine
	virtual bool RenderForReflect(CECViewport * pViewport) { return true; }
	//	RenderForRefract routine
	virtual bool RenderForRefract(CECViewport * pViewport) { return true; }
	//	Tick animation
	virtual bool TickAnimation() { return true; }

	//	Process message
	virtual bool ProcessMessage(const ECMSG& Msg) { return true; }

	//	On entering game world
	virtual bool OnEnterGameWorld() { return true; }
	//	On leaving game world
	virtual bool OnLeaveGameWorld() { return true; }

	//	Ray trace
	virtual bool RayTrace(ECRAYTRACE* pTraceInfo) { return false; }

	//	Get tick time (ms) of this frame
	DWORD GetTickTime() { return m_dwCurTickTime; }
	//	Get manager ID
	int GetManagerID() { return m_iManagerID; }

protected:	//	Attributes

	int			m_iManagerID;		//	Manager's ID
	CECGameRun* m_pGameRun;			//	Game run object
	DWORD		m_dwCurTickTime;	//	Logic tick time

protected:	//	Operations

	//	Begin recroding tick time
	void BeginTickTime();
	//	End recroding tick time
	void EndTickTime();
};

///////////////////////////////////////////////////////////////////////////
//	
//	Inline functions
//	
///////////////////////////////////////////////////////////////////////////



