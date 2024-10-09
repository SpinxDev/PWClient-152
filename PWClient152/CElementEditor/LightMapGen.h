/*
 * FILE: LightMapGen.h
 *
 * DESCRIPTION: Light map generator
 *
 * CREATED BY: Duyuxin, 2004/7/14
 *
 * HISTORY: 
 *
 * Copyright (c) 2004 Archosaur Studio, All Rights Reserved.
 */

#pragma once

#include "Global.h"

class A3DViewport;

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

class CElementMap;
class CELBitmap;

///////////////////////////////////////////////////////////////////////////
//	
//	Declare of Global functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Class CLightMapGen
//	
///////////////////////////////////////////////////////////////////////////

class CLightMapGen
{
public:		//	Types

	//	Map index
	enum
	{
		MAP_CURRENT = 0,
		MAP_LEFTTOP,
		MAP_TOP,
		MAP_RIGHTTOP,
		MAP_RIGHT,
		MAP_RIGHTBOTTOM,
		MAP_BOTTOM,
		MAP_LEFTBOTTOM,
		MAP_LEFT,
		NUM_MAP,
	};

public:		//	Constructor and Destructor

	CLightMapGen();
	virtual ~CLightMapGen();

public:		//	Attributes

public:		//	Operations

	//	Initialize object
	bool Init(const LIGHTINGPARAMS& Params,bool bOnlyRenderShadow = false);
	//	Release object
	void Release();

	//	Set a map
	void SetMap(int iIndex, CElementMap* pMap);
	//	Clear all maps
	void ClearAllMaps();
	//	Do generate light map
	bool DoGenerate(bool bNight);

	//	Get map size
	int GetMapSize() { return m_iMapSize; }
	//	Get color values
	A3DCOLORVALUE* GetColorValues() { return m_aColors; }
	//	Get light map bitmap
	CELBitmap* GetLightMapBitmap() { return m_pLMBmp; }
	//  Set light map bitmap
	void SetLightMapBitmap( CELBitmap* pLMBmp){ m_pLMBmp = pLMBmp; };
	//	Get map
	CElementMap* GetMap(int iIndex) { return m_aMaps[iIndex]; }
	//	Get lighting parameters
	const LIGHTINGPARAMS& GetLightingParams() { return m_Params; }
	//	Set current map
	void SetCurrentMap(CElementMap * pMap)	{ m_pCurMap = pMap; }
	CElementMap* GetCurrentMap(){ return m_pCurMap; };

protected:	//	Attributes

	CElementMap*	m_aMaps[NUM_MAP];	//	Maps
	LIGHTINGPARAMS	m_Params;			//	Lighting parameters

	A3DCOLORVALUE*	m_aColors;			//	Colors of each pixel on light map
	CELBitmap*		m_pLMBmp;			//	Light map bitmap
	int				m_iMapSize;			//	Map size in pixel
	bool            m_bOnlyRenderShadow;

	CElementMap *	m_pCurMap;			//	Current map	need to be calculated scene objects lights

protected:	//	Operations

	//	Create color map
	bool CreateColorMap(int iMapSize);
	//	Release color map
	void ReleaseColorMap();
	//	Create light map bitmap
	bool CreateLMBitmap(int iMapSize);
	//	Release light map bitmap
	void ReleaseLMBitmap();
	//	Release resources allocated when generate lightmap
	void ReleaseRes();

	//	Lighting specified position
	void LightingPos(int r, int c, const A3DVECTOR3& vPos);
	//	Smooth light map
	void SmoothLightMap();
	//	Smooth light map
	void SmoothLightMap(A3DCOLORVALUE* pDstMap, A3DCOLORVALUE* pSrcMap);
	//	Fill colors around specified position
	void FillColorMat(A3DCOLORVALUE* pColMap, int iRow, int iCol, A3DCOLORVALUE* aCols, BYTE* aFlags);

public:
	friend bool RenderForLightMapShadow(A3DViewport * pViewport, LPVOID pArg);
};

///////////////////////////////////////////////////////////////////////////
//	
//	Inline functions
//	
///////////////////////////////////////////////////////////////////////////


