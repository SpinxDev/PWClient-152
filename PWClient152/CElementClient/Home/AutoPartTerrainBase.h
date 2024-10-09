/*
 * FILE: AutoPartTerrainBase.h
 *
 * DESCRIPTION: Class for base operation for part terrain
 *
 * CREATED BY: Jiang Dalong, 2005/05/06
 *
 * HISTORY: 
 *
 * Copyright (c) 2001-2008 Archosaur Studio, All Rights Reserved.
 */

#ifndef _AUTOPARTTERRAINBASE_H_
#define _AUTOPARTTERRAINBASE_H_

#include "AutoSceneFunc.h"
#include <ARect.h>
#include <WTYPES.h>

const float	POINT_RADIUS		= 5.0f;
const DWORD POINT_COLOR			= 0xffff0000;

class CAutoHome;
class CAutoTerrain;

class CAutoPartTerrainBase  
{
public:
	enum STATUS
	{
		ST_NOTSTART,
		ST_START,
		ST_END,
	};

public:
	CAutoPartTerrainBase(CAutoHome* pAutoHome);
	virtual ~CAutoPartTerrainBase();

public:
	virtual void OnLButtonDown(UINT nFlags, APointI point);
	virtual void OnRButtonUp(UINT nFlags, APointI point);
	virtual void OnRButtonDblClk(UINT nFlags, APointI point);
	virtual bool Render();
	virtual void Release();
	// Coordinate from screen to height map
//	bool ScreenToHeightmap(POINT_FLOAT& pt);

protected:
	CAutoHome*				m_pAutoHome;
	CAutoTerrain*			m_pAutoTerrain;

	STATUS					m_status;			// Area operation status
	int						m_nTerrainWidth;	// Height map width
	int						m_nTerrainHeight;	// Height map height

protected:
	// Coordinate from height map to 3D world
//	bool HeightmapToWorld(POINT_FLOAT pt, A3DVECTOR3& vPos);
	// Coordinate from 3D world to height map
//	bool WorldToHeightmap(const A3DVECTOR3& vecIn, APointI* ptOut);
	// Whole terrain rect on screen
//	ARectI GetOrthoTerrainRect();
};

#endif // #ifndef _AUTOPARTTERRAINBASE_H_
