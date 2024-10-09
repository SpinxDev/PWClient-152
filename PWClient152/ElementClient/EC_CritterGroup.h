/*
 * FILE: EC_CritterGroup.h
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Duyuxin, 2004/12/25
 *
 * HISTORY: 
 *
 * Copyright (c) 2004 Archosaur Studio, All Rights Reserved.
 */

#pragma once

#include "EC_SceneObject.h"
#include "A3DVector.h"
#include "AString.h"

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

class CECScene;
class A3DAutonomousGroup;
class A3DAutonomousBehaviour;
class CECViewport;
class CECBezierWalker;
class AFile;

///////////////////////////////////////////////////////////////////////////
//	
//	Declare of Global functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Class CECCritterGroup
//	
///////////////////////////////////////////////////////////////////////////

class CECCritterGroup : public CECSceneObject
{
public:		//	Types

	//	Behaviour type
	enum
	{
		BEHAVIOUR_WANDER = 0,
		BEHAVIOUR_HOVER,
		BEHAVIOUR_ARRIVAL,
		BEHAVIOUR_BIRD,
		BEHAVIOUR_FLEE,
		BEHAVIOUR_FLEE_AND_BACK,
	};

	//	Critter type
	enum
	{
		CRITTER_BUTTERFLY = 0,
		CRITTER_NORMAL_FISH,
		CRITTER_DOLTISH_FISH,
		CRITTER_BRISK_FISH,
		CRITTER_EAGLE,
		CRITTER_FIREFLY,
		CRITTER_CROW,
	};

public:		//	Constructor and Destructor

	CECCritterGroup();
	virtual ~CECCritterGroup();

public:		//	Attributes

public:		//	Operations

	//	Load data from file
	bool Load(CECScene* pScene, AFile* pFile, float fOffX, float fOffZ);

	//	Tick routine
	bool Tick(DWORD dwDeltaTime);
	//	Render routine
	bool Render(CECViewport* pViewport);

	//	Set group ID
	void SetGroupID(DWORD dwID) { m_dwGroupID = dwID; }
	//	Get group ID
	DWORD GetGroupID() { return m_dwGroupID; }
	//	Get properties
	int GetBehaviorType() { return m_iBehavior; }
	int GetCritterType() { return m_iCritter; }
	float GetAreaFloat() { return m_fRadius; }
	const A3DVECTOR3& GetAreaCenter() { return m_vCenter; }
	int GetCritterNum() { return m_iNumCritter; }

protected:	//	Attributes

	CECScene*	m_pScene;		//	Scene object

	DWORD		m_dwGroupID;	//	Critter group ID
	int			m_iCritter;		//	Critter type
	int			m_iBehavior;	//	Behavior type
	float		m_fRadius;		//	Action area radius
	float		m_fSpeed;		//	Moving speed
	int			m_iNumCritter;	//	Critter number
	DWORD		m_dwBezier;		//	ID of bezier route

	A3DVECTOR3	m_vCenter;		//	Action area center
	AString		m_strModelFile;	//	Model file name

	A3DAutonomousGroup*			m_pGroup;
	A3DAutonomousBehaviour*		m_pBehavior;
	CECBezierWalker*			m_pBezierWalker;

protected:	//	Operations

};

///////////////////////////////////////////////////////////////////////////
//	
//	Inline functions
//	
///////////////////////////////////////////////////////////////////////////


