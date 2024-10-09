/*
 * FILE: EC_AssureMove.cpp
 *
 * DESCRIPTION: AssureMove for the Element Client
 *
 * CREATED BY: Hedi, 2007/4/23 
 *
 * HISTORY:
 *
 * Copyright (c) 2007 Archosaur Studio, All Rights Reserved.
 */

#include <AString.h>
#include <AFile.h>
#include <AFileImage.h>
#include <A3DFrame.h>
#include <A3DLitModel.h>
#include <ACounter.h>

#include "EC_AssureMove.h"
#include "EC_SceneCheck.h"
#include "EC_Game.h"
#include "EC_GameRun.h"
#include "EC_World.h"
#include "EC_Global.h"
#include "EC_CDR.h"
#include "EC_ManMatter.h"
#include "EC_ManNPC.h"
#include "EL_Forest.h"

CECAssureMove::CECAssureMove()
{
	m_pBrushGrids = NULL;

	m_vecMapOrigin = A3DVECTOR3(0.0f);
	m_fGridSize = 32.0f;
	m_nGridRows = 0;
	m_nGridCols = 0;
	m_bHasCheatCD = false;
	m_bHasCheatFly = false;

	m_dwFloatStart = 0;
	m_nFloatTicks = 0;
}

CECAssureMove::~CECAssureMove()
{
	ReleaseMap();
}

bool CECAssureMove::LoadMap(const char * szMap, A3DVECTOR3 vecMapOrigin, float fMapWidth, float fMapHeight, float fGridSize)
{
	return true;
	ReleaseMap();

	m_vecMapOrigin	= vecMapOrigin;
	m_fGridSize		= fGridSize;

	m_nGridCols = int(fMapWidth / fGridSize) + 1;
	m_nGridRows = int(fMapHeight / fGridSize) + 1;
	m_pBrushGrids = new BRUSHGRID[m_nGridRows * m_nGridCols];

	vector<AString> listFiles;
	CECSceneCheck check;
	check.GetOrnamentList(szMap, listFiles);
	check.Release();

	int nStepSize;
	if( listFiles.size() < 100 )
		nStepSize = listFiles.size();
	else
		nStepSize = listFiles.size() / 7 + 1;
	size_t i;
	for(i=0; i<listFiles.size(); i++)
	{
		// see if there has the same one before
		size_t k;
		for(k=0; k<i; k++)
		{
			if( stricmp(listFiles[k], listFiles[i]) == 0 )
				break;
		}

		if( k != i )
			continue;

		m_listFiles[i/nStepSize].push_back(listFiles[i]);
	}
	
	return true;
}

bool CECAssureMove::StepLoadMap(int nStep)
{
	return true;
	if( nStep < 0 || nStep >= 7 )
		return false;

	size_t i;
	for(i=0; i<m_listFiles[nStep].size(); i++)
	{
		AString		strFile = m_listFiles[nStep][i];
		AFileImage	file;

		if( !file.Open(strFile, AFILE_OPENEXIST | AFILE_BINARY | AFILE_TEMPMEMORY) )
			continue;

		DWORD dwReadLen;
		DWORD version;
		if( !file.Read(&version, sizeof(DWORD), &dwReadLen) )
			continue;

		bool bCollideOnly = false;
		if( version == 0x80000001 )
		{
			if( !file.Read(&bCollideOnly, sizeof(bool), &dwReadLen) )
				return false;
		}
		else
		{
			file.Seek(-int(sizeof(DWORD)), AFILE_SEEK_CUR);
		}


		A3DLitModel litModel;
		if( !litModel.DummyLoad(&file) )
			continue;
		litModel.Release();

		int idHull;
		int nNumHull;
		if( !file.Read(&nNumHull, sizeof(int), &dwReadLen) )
			continue;
		for(idHull=0; idHull<nNumHull; idHull++)
		{
			int nNumMesh = 0;
			if( !file.Read(&nNumMesh, sizeof(int), &dwReadLen) )
				continue;

			file.Seek(sizeof(int) * nNumMesh, AFILE_SEEK_CUR);
		}

		for(idHull=0; idHull<nNumHull; idHull++)
		{
			CCDBrush * pCDBrush = new CCDBrush;
			if( !pCDBrush->Load(&file) )
			{
				delete pCDBrush;
				continue;
			}
			m_CDBrushes.push_back(pCDBrush);
		
			int index = m_CDBrushes.size() - 1;

			// now put these brushes into grid
			A3DAABB aabb = pCDBrush->GetAABB();

			// see if the aabb is not valid
			if( aabb.Maxs.x < aabb.Mins.x || aabb.Maxs.y < aabb.Mins.y || aabb.Maxs.z < aabb.Mins.z )
				continue;

			if( aabb.Maxs.x - aabb.Mins.x > 2000.0f || aabb.Maxs.y - aabb.Mins.y > 2000.0f || aabb.Maxs.z - aabb.Mins.z > 2000.0f )
				continue;

			int sx, sy, ex, ey;
			sx = int((aabb.Mins.x - m_vecMapOrigin.x) / m_fGridSize);
			ex = int((aabb.Maxs.x - m_vecMapOrigin.x) / m_fGridSize);
			sy = int((m_vecMapOrigin.z - aabb.Maxs.z) / m_fGridSize);
			ey = int((m_vecMapOrigin.z - aabb.Mins.z) / m_fGridSize);

			for(int x=sx; x<=ex; x++)
			{
				for(int y=sy; y<=ey; y++)
				{
					if( x < 0 || y < 0 || x >= m_nGridCols || y >= m_nGridRows )
						continue;

					m_pBrushGrids[y * m_nGridCols + x].listCCDBrushes.push_back(index);
				}
			}
		}
	}

	return true;
}

bool CECAssureMove::ReleaseMap()
{
	m_listFiles[0].clear();
	m_listFiles[1].clear();
	m_listFiles[2].clear();
	m_listFiles[3].clear();
	m_listFiles[4].clear();
	m_listFiles[5].clear();
	m_listFiles[6].clear();

	if( m_pBrushGrids )
	{
		delete [] m_pBrushGrids;
		m_pBrushGrids = NULL;
	}
	
	size_t i;
	for(i=0; i<m_CDBrushes.size(); i++)
	{
		CCDBrush * pCDBrush = m_CDBrushes[i];

		pCDBrush->Release();
		delete pCDBrush;
	}

	m_CDBrushes.clear();
	m_nGridCols = 0;
	m_nGridRows = 0;

	m_bHasCheatCD = false;
	m_bHasCheatFly = false;
	return true;
}

bool TraceWithOthers(BrushTraceInfo * pInfo)
{
	BrushTraceInfo info = *pInfo;
	CECMatterMan* pMatterMan = g_pGame->GetGameRun()->GetWorld()->GetMatterMan();
	if( pMatterMan && pMatterMan->TraceWithBrush(&info) )
		return true;

	info = *pInfo;
	CECNPCMan* pNPCMan = g_pGame->GetGameRun()->GetWorld()->GetNPCMan();
	if( pNPCMan && pNPCMan->TraceWithBrush(&info) )
		return true;

	info = *pInfo;
	CELForest * pForest = g_pGame->GetGameRun()->GetWorld()->GetForest();
	if (pForest && pForest->TraceWithBrush(&info) )
		return true;

	return false;
}

#define FLOAT_DIFF	1.3f
#define MAX_FLOAT_ALLOWED	30000

bool CECAssureMove::AssureMove(const A3DVECTOR3& vecStart, const A3DVECTOR3& vecEnd)
{
	if( !m_pBrushGrids )
		return true;

	// we only check the start grid and the end grid
	int sx, sy, ex, ey, t;
	sx = int((vecStart.x - m_vecMapOrigin.x) / m_fGridSize);
	ex = int((vecEnd.x - m_vecMapOrigin.x) / m_fGridSize);
	sy = int((m_vecMapOrigin.z - vecStart.z) / m_fGridSize);
	ey = int((m_vecMapOrigin.z - vecEnd.z) / m_fGridSize);
	if( sx > ex )
	{
		t = sx; sx = ex; ex = t;
	}
	if( sy > ey )
	{
		t = sy; sy = ey; ey = t;
	}

	if( sx < 0 || sx >= m_nGridCols || sy < 0 || sy >= m_nGridRows )
		return true;

	BrushTraceInfo info;
	info.Init(vecStart, vecEnd - vecStart, A3DVECTOR3(0.0f), true);

	A3DVECTOR3 vecCenter = (vecStart + vecEnd) * 0.5f;
	BrushTraceInfo vertInfo;
	vertInfo.Init(vecCenter, A3DVECTOR3(0.0f, -FLOAT_DIFF, 0.0f), A3DVECTOR3(0.7f, 0.7f, 0.7f), false);
	bool bNeedCheckFloat = false;
	bool bHasSupported = false;
	if( !TraceWithOthers(&vertInfo) && g_pGame->GetGameRun()->GetWorld()->GetTerrainHeight(vecCenter) < vecCenter.y - FLOAT_DIFF && 
		g_pGame->GetGameRun()->GetWorld()->GetWaterHeight(vecCenter) < vecCenter.y )
		bNeedCheckFloat = true;
	else
		bHasSupported = true;

	for(int x=sx; x<=ex; x++)
	{
		for(int y=sy; y<=ey; y++)
		{
			const BRUSHGRID& grid = m_pBrushGrids[y * m_nGridCols + x];

			size_t i;
			for(i=0; i<grid.listCCDBrushes.size(); i++)
			{
				// see if pass through walls
				if( grid.listCCDBrushes[i] < (int)m_CDBrushes.size() && m_CDBrushes[grid.listCCDBrushes[i]]->Trace(&info) )
				{
					m_bHasCheatCD = true;
					m_vecCheatCDPos = vecCenter;
					return false;
				}

				// see if float now
				if( bNeedCheckFloat )
				{
					if( grid.listCCDBrushes[i] < (int)m_CDBrushes.size() && m_CDBrushes[grid.listCCDBrushes[i]]->Trace(&vertInfo) )
						bHasSupported = true;
				}
			}
		}
	}

	if( !bHasSupported )
	{
		m_nFloatTicks ++;

		if( m_dwFloatStart == 0 )
			m_dwFloatStart = ACounter::GetMilliSecondNow();
		else if( ACounter::GetMilliSecondNow() - m_dwFloatStart > MAX_FLOAT_ALLOWED && m_nFloatTicks > MAX_FLOAT_ALLOWED / 500 )
		{
			m_bHasCheatFly = true;
			m_vecCheatFlyPos = vecCenter;
		}
	}
	else
	{
		m_dwFloatStart = 0;
		m_nFloatTicks = 0;
	}

	return true;
}

bool CECAssureMove::NoAssureMove()
{
	m_dwFloatStart = 0;
	m_nFloatTicks = 0;

	return true;
}