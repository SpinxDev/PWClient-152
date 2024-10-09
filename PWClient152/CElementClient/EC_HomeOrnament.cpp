/*
 * FILE: EC_HomeOrnament.cpp
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Hedi, 2006/6/27
 *
 * HISTORY: 
 *
 * Copyright (c) 2006 Archosaur Studio, All Rights Reserved.
 */

#pragma once

#include "EC_Global.h"
#include "EC_HomeOrnament.h"
#include "EC_Game.h"
#include "EC_Viewport.h"
#include "EC_WorldFile.h"
#include "EC_Scene.h"
#include "EL_BuildingWithBrush.h"
#include "EC_SceneLoader.h"
#include "EC_GameRun.h"
#include "EC_World.h"
#include "EC_ManOrnament.h"
#include "EC_BrushMan.h"
#include "EC_TriangleMan.h"
#include "EC_Utility.h"

#include <A3DFrame.h>
#include <A3DLitModel.h>
#include <A3DTerrainWater.h>
#include <A3DFuncs.h>

#include "AutoScene.h"
#include "AString.h"

CECHomeOrnament::CECHomeOrnament(CECOrnamentMan* pOrnamentMan) : CECSceneObject(TYPE_ORNAMENT)
{
	m_pOnmtMan	= pOrnamentMan;
	m_dwOnmtID	= 0;
	m_LoadTM = IdentityMatrix();
	m_pBuildingWithBrush = NULL;
}

CECHomeOrnament::~CECHomeOrnament()
{
}


//	Load ornament data from file
bool CECHomeOrnament::Load(CAutoScene* pScene, const char* szMox, const char* szCHF, const A3DMATRIX4& matTM)
{
	ASSERT(!m_pBuildingWithBrush);

	m_LoadTM = matTM;

	m_strMoxFile = szMox;
	m_strCHFFile = szCHF;

	QueueLoadInThread(static_cast<CECSceneThreadObj*>(this));
	return true;
}

void CECHomeOrnament::LoadInThread(bool bInLoaderThread)
{
	m_pBuildingWithBrush = new CELBuildingWithBrush();

	if (!m_pBuildingWithBrush->LoadFromMOXAndCHF(g_pGame->GetA3DDevice(), m_strMoxFile, m_strCHFFile, m_LoadTM))
	{
		a_LogOutput(1, "CECHomeOrnament::LoadInThread, failed to LoadFromMOXAndCHF()");
		g_dwFatalErrorFlag = FATAL_ERROR_LOAD_BUILDING;
		return;
	}

	if( !bInLoaderThread )
		m_pBuildingWithBrush->Tick(BUILDINGWITHBRUSH_FADE_IN_TIME + 1);
}

//	Release object
void CECHomeOrnament::Release()
{
	if( m_pBuildingWithBrush )
	{
		m_pBuildingWithBrush->Release();
		delete m_pBuildingWithBrush;
		m_pBuildingWithBrush = NULL;
	}
}

void CECHomeOrnament::ReleaseInThread()
{
	Release();
}

//	Tick routine
bool CECHomeOrnament::Tick(DWORD dwDeltaTime)
{
	if (IsLoaded() && m_pBuildingWithBrush)
	{
		m_pBuildingWithBrush->Tick(dwDeltaTime);
	}

	return true;
}

//	Render routine
bool CECHomeOrnament::Render(CECViewport* pViewport)
{
	if (IsLoaded() && m_pBuildingWithBrush)
	{
		m_pBuildingWithBrush->Render(pViewport->GetA3DViewport());
	}
 
	return true;
}

