/*
 * FILE: TerrainWater.h
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Hedi, 2004/9/11
 *
 * HISTORY: 
 *
 * Copyright (c) 2004 Archosaur Studio, All Rights Reserved.
 */

#pragma once

#include <AArray.h>
#include <ARect.h>
#include <A3DVector.h>
#include <ABaseDef.h>
#include <A3DTerrainWater.h>

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

#pragma pack (push, 1)

//	Terrain water file data struct
struct TERRAINWATERFILEDATA
{
	BYTE	aReserved[59];	//	Reserved
};

#pragma pack (pop)

class A3DCameraBase;
class A3DViewport;
class A3DTerrain2;
class A3DSky;
class A3DLight;

///////////////////////////////////////////////////////////////////////////
//	
//	Declare of Global functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Class CTerrain
//	
///////////////////////////////////////////////////////////////////////////

class CTerrainWater
{
public:

public:		//	Constructor and Destructor

	CTerrainWater();
	virtual ~CTerrainWater();

public:		//	Attributes

public:		//	Operations

	//	Initalize object
	bool Init(A3DTerrainWater* pA3DTerrainWater);
	//	Release object
	virtual void Release();

	//	Load data
	virtual bool Load(const char* szFileName);
	//	Save data
	virtual bool Save(const char * szFileName);

	//	Frame move routine
	bool FrameMove(DWORD dwDeltaTime);
	//	render routine
	bool Render(const A3DVECTOR3& vecSunDir, A3DViewport* pA3DViewport, A3DTerrain2 * pTerrain, A3DSky * pSky, LPRENDERFORREFLECT pRenderReflect, LPRENDERFORREFRACT pRenderRefract, LPVOID pArg);
	
	A3DTerrainWater* GetA3DTerrainWater()	{ return m_pA3DTerrainWater; }
protected:	//	Attributes
	// an array to store all water areas information
	APtrArray<A3DWaterArea *> m_pWaterAreas;
	AArray<DWORD, DWORD> m_aExportIDs;	//	Used when export data

	A3DTerrainWater*	m_pA3DTerrainWater;	//	Water object

protected:	//	Operations

public:
	// water area manipulation
	bool AddWaterArea(DWORD dwID, int nWidth, int nHeight, float vGridSize, const A3DVECTOR3& vecCenter, float vWaterHeight);
	bool DeleteWaterArea(int index);
	bool DeleteWaterAreaByID(DWORD dwID);
	int GetNumWaterAreas();
	A3DWaterArea * GetWaterArea(int index);
	A3DWaterArea * GetWaterAreaByID(DWORD dwID);
	AArray<DWORD, DWORD>& GetExportIDs() { return m_aExportIDs; }
	//	Does water area intersect with specified area ?
	bool WaterAreaIntersectWithArea(A3DWaterArea* pWaterArea, const A3DVECTOR3& vMin, const A3DVECTOR3& vMax);
	
	void DeleteAllWaterAreas();
};

///////////////////////////////////////////////////////////////////////////
//	
//	Inline functions
//	
///////////////////////////////////////////////////////////////////////////

