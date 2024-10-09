/*
 * FILE: AutoHome.h
 *
 * DESCRIPTION: Class for a home area, include terrain, grass, forest, water...
 *
 * CREATED BY: Jiang Dalong, 2006/06/09
 *
 * HISTORY: 
 *
 * Copyright (c) 2001-2008 Archosaur Studio, All Rights Reserved.
 */

#ifndef _AUTOHOME_H_
#define _AUTOHOME_H_

#include <AString.h>
#include <ABaseDef.h>
#include <A3DVector.h>

class CAutoScene;
class CAutoTerrain;
class CAutoTexture;
class CAutoWater;
class CAutoForest;
class CAutoForestPoly;
class CAutoForestLine;
class CAutoGrass;
class CAutoGrassPoly;
class CAutoHillPoly;
class CAutoHillLine;
class CAutoTerrainFlat;
class CAutoRoad;
class CAutoPartTexture;
class CAutoHomeAction;
class CAutoSelectedArea;
class CAutoBuilding;
class CAutoHomeBorder;
/*
class CAutoLight;
class CAutoSky;
class CAutoFog;
class CAutoParameters;
*/
class A3DViewport;
class A3DEngine;
class AString;

class CAutoHome  
{
public:
	CAutoHome(CAutoScene* pAutoScene);
	virtual ~CAutoHome();

	bool Init(int nRowIndex, int nColIndex);
	bool Tick(DWORD dwTickTime);
	void Release();

	// Create a new home
	bool CreateNewHome();
	// Load home
	bool LoadHome();
	// Save home
	bool SaveHome();
	// Get home file name
	bool GetHomeFileName(AString& strFile);
	// Get world position
	bool HeightMapToWorld(int x, int z, A3DVECTOR3& vPos);
	// Local pos to world pos
	bool LocalToWorld(const A3DVECTOR3& vPosIn, A3DVECTOR3& vPosOut);
	// World pos to local pos
	bool WorldToLocal(const A3DVECTOR3& vPosIn, A3DVECTOR3& vPosOut);
	// Local to height map
	bool LocalToHeightMap(const A3DVECTOR3& vPos, int* x, int* z);
	// World pos to local height map
	bool WorldToHeightMap(const A3DVECTOR3& vPos, int* x, int* z);

	// Smooth border and apply result
	bool SmoothAndApplyBorder();
	
	// Ray trace
	bool RayTrace(const A3DVECTOR3& vStart, const A3DVECTOR3& vEnd, A3DVECTOR3& vTracePos, bool bCheckModel);

	inline CAutoScene*			GetAutoScene()			{ return m_pAutoScene; }
	inline CAutoTerrain*		GetAutoTerrain()		{ return m_pAutoTerrain; }
	inline CAutoTexture*		GetAutoTexture()		{ return m_pAutoTexture; }
	inline CAutoForest*			GetAutoForest()			{ return m_pAutoForest; }
	inline CAutoWater*			GetAutoWater()			{ return m_pAutoWater; }
	inline CAutoForestPoly*		GetAutoForestPoly()		{ return m_pAutoForestPoly; }
	inline CAutoForestLine*		GetAutoForestLine()		{ return m_pAutoForestLine; }
	inline CAutoGrass*			GetAutoGrass()			{ return m_pAutoGrass; }
	inline CAutoGrassPoly*		GetAutoGrassPoly()		{ return m_pAutoGrassPoly; }
	inline CAutoHillPoly*		GetAutoHillPoly()		{ return m_pAutoHillPoly; }
	inline CAutoHillLine*		GetAutoHillLine()		{ return m_pAutoHillLine; }
	inline CAutoTerrainFlat*	GetAutoTerrainFlat()	{ return m_pAutoTerrainFlat; }
	inline CAutoRoad*			GetAutoRoad()			{ return m_pAutoRoad; }
	inline CAutoPartTexture*	GetAutoPartTexture()	{ return m_pAutoPartTexture; }
	inline CAutoHomeAction*		GetAutoHomeAction()		{ return m_pAutoHomeAction; }
	inline CAutoSelectedArea*	GetAutoSelectedArea()	{ return m_pAutoSelectedArea; }
	inline CAutoBuilding*		GetAutoBuilding()		{ return m_pAutoBuilding; }
	inline CAutoHomeBorder*		GetAutoHomeBorder()		{ return m_pAutoHomeBorder; }
/*	inline CAutoLight*			GetAutoLight()			{ return m_pAutoLight; }
	inline CAutoSky*			GetAutoSky()			{ return m_pAutoSky; }
	inline CAutoFog*			GetAutoFog()			{ return m_pAutoFog; }
	inline CAutoParameters*		GetAutoParameters()		{ return m_pAutoParameters; }
*/	
	inline int					GetRowIndex()			{ return m_nRowIndex; }
	inline int					GetColIndex()			{ return m_nColIndex; }

protected:
	CAutoScene*				m_pAutoScene;			// Scene
	CAutoTerrain*			m_pAutoTerrain;			// Terrain
	CAutoTexture*			m_pAutoTexture;			// Texture mask controller
	CAutoForest*			m_pAutoForest;			// Forest
	CAutoWater*				m_pAutoWater;			// Water
	CAutoForestPoly*		m_pAutoForestPoly;		// Polygon forest
	CAutoForestLine*		m_pAutoForestLine;		// Linear forest
	CAutoGrass*				m_pAutoGrass;			// Grass
	CAutoGrassPoly*			m_pAutoGrassPoly;		// Polygon grass
	CAutoHillPoly*			m_pAutoHillPoly;		// Polygon hill
	CAutoHillLine*			m_pAutoHillLine;		// Line hill
	CAutoTerrainFlat*		m_pAutoTerrainFlat;		// Flat terrain
	CAutoRoad*				m_pAutoRoad;			// Road
	CAutoPartTexture*		m_pAutoPartTexture;		// Part texture
	CAutoHomeAction*		m_pAutoHomeAction;		// Terrain action
	CAutoSelectedArea*		m_pAutoSelectedArea;	// Selected area
	CAutoBuilding*			m_pAutoBuilding;		// Building
	CAutoHomeBorder*		m_pAutoHomeBorder;		// Border
/*	CAutoLight*				m_pAutoLight;			// Light
	CAutoSky*				m_pAutoSky;				// Sky
	CAutoFog*				m_pAutoFog;				// Fog
	CAutoParameters*		m_pAutoParameters;		// Prepared parameters
*/
	
	int						m_nRowIndex;			// Row index in the scene
	int						m_nColIndex;			// Column index in the scene

protected:
	//	Create auto texture
	bool CreateAutoTexture();
	//	Create auto terrain
	bool CreateAutoTerrain();
	//	Create auto forest
	bool CreateAutoForest();
	//	Create auto grass
	bool CreateAutoGrass();
	//	Create auto water
	bool CreateAutoWater();
	// Create name by row and col
	bool GetNameFromRowCol(int r, int c, AString& strName);
};

#endif // #ifndef _AUTOHOME_H_

