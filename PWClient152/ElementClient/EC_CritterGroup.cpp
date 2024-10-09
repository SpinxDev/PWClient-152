/*
 * FILE: EC_CritterGroup.cpp
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Duyuxin, 2004/12/25
 *
 * HISTORY: 
 *
 * Copyright (c) 2004 Archosaur Studio, All Rights Reserved.
 */

#include "EC_Global.h"
#include "EC_CritterGroup.h"
#include "EC_Game.h"
#include "EC_GameRun.h"
#include "EC_World.h"
#include "EC_Viewport.h"
#include "EC_WorldFile.h"
#include "EC_Scene.h"
#include "EC_Bezier.h"

#include "A3DAutonomousGroup.h"
#include "A3DAutonomousBehaviour.h"
#include "A3DAutonomousAgent.h"
#include "A3DAutoFish.h"
#include "A3DAutoEagle.h"
#include "A3DAutoButterfly.h"
#include "A3DAutoFirefly.h"
#include "A3DAutoCrow.h"

#include "AFile.h"

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
//	Implement CECCritterGroup
//	
///////////////////////////////////////////////////////////////////////////

CECCritterGroup::CECCritterGroup() : CECSceneObject(TYPE_CRITTER)
{
	m_pScene		= NULL;
	m_dwGroupID		= 0;
	m_iCritter		= CRITTER_BUTTERFLY;
	m_iBehavior		= BEHAVIOUR_WANDER;
	m_fRadius		= 0.0f;
	m_fSpeed		= 0.01f;
	m_iNumCritter	= 0;
	m_pGroup		= NULL;
	m_pBehavior		= NULL;
	m_dwBezier		= 0;
	m_pBezierWalker	= NULL;
}

CECCritterGroup::~CECCritterGroup()
{
	if (m_pGroup)
	{
		m_pGroup->Release();
		delete m_pGroup;
		m_pGroup = NULL;
	}

	if (m_pBehavior)
	{
		m_pBehavior->Release();
		delete m_pBehavior;
		m_pBehavior = NULL;
	}

	if (m_pBezierWalker)
	{
		delete m_pBezierWalker;
		m_pBezierWalker = NULL;
	}
}

//	Load data from file
bool CECCritterGroup::Load(CECScene* pScene, AFile* pFile, float fOffX, float fOffZ)
{
	m_pScene = pScene;

	DWORD dwRead;

	if (pScene->GetBSDFileVersion() < 5)
	{
		ECBSDFILECRITTER Data;
		if (!pFile->Read(&Data, sizeof (Data), &dwRead))
			return false;

		m_iCritter		= Data.iCritterType;
		m_iBehavior		= Data.iBehaviour;
		m_fRadius		= Data.fRadius;
		m_iNumCritter	= Data.iCount;
		m_dwBezier		= 0;

		m_vCenter.Set(Data.vPos[0] + fOffX, Data.vPos[1], Data.vPos[2]+fOffZ);
	}
	else	//	pScene->GetBSDFileVersion() >= 5
	{
		ECBSDFILECRITTER5 Data;
		if (!pFile->Read(&Data, sizeof (Data), &dwRead))
			return false;

		m_iCritter		= Data.iCritterType;
		m_iBehavior		= Data.iBehaviour;
		m_fRadius		= Data.fRadius;
		m_iNumCritter	= Data.iCount;
		m_dwBezier		= Data.dwBezier;
		m_fSpeed		= Data.fSpeed;

		m_vCenter.Set(Data.vPos[0]+fOffX, Data.vPos[1], Data.vPos[2]+fOffZ);
	}

	if (!pFile->ReadString(m_strModelFile))
		return false;

	if (!(m_pGroup = new A3DAutonomousGroup))
		return false;

	switch (m_iBehavior)
	{
	case BEHAVIOUR_WANDER:	
		m_pBehavior = new A3DAutoBehaviourWander;	
		break;
	case BEHAVIOUR_HOVER:	
		m_pBehavior = new A3DAutoBehaviourHover;
		((A3DAutoBehaviourHover*)m_pBehavior)->SetTargetPosition(m_vCenter);
		break;
	case BEHAVIOUR_FLEE:	
		m_pBehavior = new A3DAutoBehaviourFlee;		
		break;
	case BEHAVIOUR_ARRIVAL:	
		m_pBehavior = new A3DAutoBehaviourArrival;	
		((A3DAutoBehaviourArrival*)m_pBehavior)->SetTargetPosition(m_vCenter);
		break;
	case BEHAVIOUR_BIRD:	
		m_pBehavior = new A3DAutoBehaviourBoid;		
		break;
	case BEHAVIOUR_FLEE_AND_BACK: 
		m_pBehavior = new A3DAutoBehaviourFleeAndBack; 
		((A3DAutoBehaviourFleeAndBack*)m_pBehavior)->SetTargetPosition(m_vCenter);
		break;//Arrival	
	default:
		ASSERT(0);
		return false;
	}

	m_pGroup->SetActiveBehaviour(m_pBehavior);
	m_pGroup->SetCenter(m_vCenter);
	m_pGroup->SetBoundaryRadius(m_fRadius);
	
	for (int i=0; i < m_iNumCritter; i++)
	{
		A3DAutonomousAgent* pAgent = NULL;

		switch (m_iCritter)
		{
		case CRITTER_BUTTERFLY:		pAgent = new A3DAutoButterfly;		break;
		case CRITTER_EAGLE:			pAgent = new A3DAutoEagle;			break;
		case CRITTER_DOLTISH_FISH:	pAgent = new A3DAutoDoltishFish;	break;
		case CRITTER_NORMAL_FISH:	pAgent = new A3DAutoFish;			break;
		case CRITTER_BRISK_FISH:	pAgent = new A3DAutoBriskFish;		break;
		case CRITTER_FIREFLY:       pAgent = new A3DAutoFirefly;		break;
		case CRITTER_CROW:          pAgent = new A3DAutoCrow;			break;
		default:
			ASSERT(0);
			return false;
		}

		if (!pAgent->Init(g_pGame->GetA3DEngine()))
		{
			A3DRELEASE(pAgent);
			continue;
		}

		pAgent->SetPosition(m_vCenter);

		if (!pAgent->Load(m_strModelFile))
		{
			A3DRELEASE(pAgent);
			continue;
		}
		
		//	TODO: use real light information 
		A3DSkinModel::LIGHTINFO LightInfo;
		memset(&LightInfo, 0, sizeof (LightInfo));
		const A3DLIGHTPARAM& lp = g_pGame->GetDirLight()->GetLightparam();

		LightInfo.colAmbient	= g_pGame->GetA3DDevice()->GetAmbientValue();
		LightInfo.vLightDir		= lp.Direction;
		LightInfo.colDirDiff	= lp.Diffuse;
		LightInfo.colDirSpec	= lp.Specular;
		LightInfo.bPtLight		= false;
		
		pAgent->SetLightInfo(LightInfo);

		m_pGroup->AddMember(pAgent);
	}

	m_pGroup->Init();

	return true;
}

//	Tick routine
bool CECCritterGroup::Tick(DWORD dwDeltaTime)
{
	if (!m_pGroup)
		return true;

	CECWorld* pWorld = g_pGame->GetGameRun()->GetWorld();
	ASSERT(m_pScene);

	if (m_dwBezier)
	{
		CECBezier* pBezier = (CECBezier*)m_pScene->GetBezierObject(m_dwBezier);
		if (pBezier)
		{
			if (!m_pBezierWalker)
			{
				m_pBezierWalker = new CECBezierWalker;
				m_pBezierWalker->BindBezier(pBezier);
				m_pBezierWalker->SetSpeed(m_fSpeed);
				m_pBezierWalker->StartWalk(true, true);
			}
			else
				m_pBezierWalker->BindBezier(pBezier);

			m_pBezierWalker->Tick((int)dwDeltaTime);
			m_pGroup->SetCenter(m_pBezierWalker->GetPos());
		}
	}

	m_pGroup->Update(pWorld->GetTerrain(), pWorld->GetTerrainWater(), dwDeltaTime);

	return true;
}

//	Render routine
bool CECCritterGroup::Render(CECViewport* pViewport)
{
	if (m_pGroup)
		m_pGroup->Render(pViewport);

	return true;
}

