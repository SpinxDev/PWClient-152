/*
 * FILE: EC_LoginPlayer.h
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Duyuxin, 2005/1/28
 *
 * HISTORY: 
 *
 * Copyright (c) 2005 Archosaur Studio, All Rights Reserved.
 */

#pragma once

#include "A3DSkinModel.h"
#include "EC_Player.h"

///////////////////////////////////////////////////////////////////////////
//	
//	Define and Macro
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Types and Global variables
//	
///////////////////////////////////////////////////////////////////////////

class CECIvtrItem;

namespace GNET
{
	class RoleInfo;
}

///////////////////////////////////////////////////////////////////////////
//	
//	Declare of Global functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Class CECLoginPlayer
//	
///////////////////////////////////////////////////////////////////////////

class CECLoginPlayer : public CECPlayer
{
public:		//	Types

public:		//	Constructor and Destructor

	CECLoginPlayer(CECPlayerMan* pPlayerMan);
	virtual ~CECLoginPlayer();

public:		//	Attributes

public:		//	Operations

	//	Release object
	virtual void Release();

	//	Tick routine
	virtual bool Tick(DWORD dwDeltaTime);
	//	Render routine
	virtual bool Render(CECViewport* pViewport, int iRenderFlag=0);

	//	Load player data
	bool Load(GNET::RoleInfo& Info);

	void StandForCustomize();
	void Stand(bool bRestart = false);
	void StandNow();
	void Run();
	void StandUp();
	void SitDown();
	bool TestShowDisplayAction(int displayActionWeapon);
	void ShowDisplayAction();
	void StopDisplayAction();
	void MoveFront();
	void GoBack();
	
protected:	//	Attributes

	CECIvtrItem*	m_aEquipItems[SIZE_ALL_EQUIPIVTR];

protected:	//	Operations

	//	Render for refelction
	bool RenderForReflect(CECViewport* pViewport);
};

///////////////////////////////////////////////////////////////////////////
//	
//	Inline functions
//	
///////////////////////////////////////////////////////////////////////////


