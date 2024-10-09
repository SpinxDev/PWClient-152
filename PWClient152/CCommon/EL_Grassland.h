/*
 * FILE: EL_Grassland.h
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Hedi, 2004/10/21
 *
 * HISTORY: 
 *
 * Copyright (c) 2004 Archosaur Studio, All Rights Reserved.
 */

#pragma once

#include <A3DTypes.h>
#include <A3DVertex.h>
#include <AArray.h>
#include <ALog.h>
#include <AC.h>
#include <A3DTerrain2.h>

#include "EL_GrassType.h"

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

class A3DCamera;
class A3DTerrain2;
class A3DPixelShader;
class A3DVertexShader;

///////////////////////////////////////////////////////////////////////////
//	
//	Declare of Global functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Class CELArchive
//	
///////////////////////////////////////////////////////////////////////////

#define WIND_BUF_LEN	80

///////////////////////////////////////////////////////////////////////////
// class to wave in the wind
///////////////////////////////////////////////////////////////////////////
class CELWindWaver
{
private:
	A3DVECTOR3		m_vecPos;
	A3DVECTOR3		m_vecVelocity;
	A3DVECTOR3		m_vecAccel;
	A3DVECTOR3		m_vecSpringForce;
	A3DVECTOR3		m_vecWindForce;
	A3DVECTOR3		m_vecForceCombined;

	float			m_vMass;

	float			m_k1;
	float			m_k2;
	float			m_f;

public:
	inline void SetWindForce(A3DVECTOR3 &vecForce) { m_vecWindForce = vecForce; }
	inline const A3DVECTOR3 & GetPos() { return m_vecPos; }

protected:
	bool CalculateForce(const A3DVECTOR3& vecPos, const A3DVECTOR3& vecVelocity);
	bool UpdatePos(float dt);

public:
	CELWindWaver();
	~CELWindWaver();

	// Calculate the particle's state after nDeltaTime (in milliseconds)
	bool Tick(int nDeltaTime);
};

class CELGrassLand
{
protected:
	A3DDevice *			m_pA3DDevice;			// a3d device object pointer
	A3DTerrain2 *		m_pA3DTerrain;			// terrain object for terrain height calculation
	ALog *				m_pLogFile;				// the log file pointer
	
	bool				m_bUseSW;				// flag indicates use software vertex processing

	bool				m_bProgressLoad;		// flag indicates we load the grasses progressed
	bool				m_bForceRebuild;		// force rebuild block each time when needed

	int					m_nGrassCount;			// current grass count for rendering

	CELWindWaver		m_windWaver;			// wind waver
	A3DVECTOR3			m_vecWindDir;			// wind direction
	APerlinNoise1D		m_wind;					// wind
	float				m_vTime;				// time counter
	A3DVECTOR4			m_vecWindOffset[WIND_BUF_LEN]; // offset caused by the wind
			 
	A3DVertexShader *	m_pGrassVertexShader;	// grass waving vertex shader
	A3DPixelShader *	m_pGrassRenderShader;	// grass render shader
	A3DVertexShader *	m_pGrassRefractVertexShader;	// grass waving refract vertex shader

	bool				m_bIsRenderForRefract;	// flag indicates we are rendering for refract now
	float				m_vRefractSurfaceHeight;// current water surface height
	A3DVertexShader *	m_pRefractVertexShader;	// vertex shader used to do refract rendering

	float				m_vLODLevel;			// grass lod level

	APtrArray<CELGrassType*>	m_GrassTypes; // the grass types on the land

	A3DStream *			m_pStream;				// stream used to render grasses
	A3DStream *			m_pSoftStream;			// stream used to do software vertex processing

	int					m_nMaxVerts;			// max verts that can be drawn in one pass
	int					m_nMaxFaces;			// max faces that can be drawn in one pass

	float				m_fDNFactor;			// day or night factor
	DWORD				m_dwUpdateStart;		// time that start update

public:
	inline A3DStream * GetStream()						{ return m_pStream; }
	inline A3DStream * GetSoftStream()					{ return m_pSoftStream; }

	inline int GetMaxVerts()							{ return m_nMaxVerts; }
	inline int GetMaxFaces()							{ return m_nMaxFaces; }

	inline A3DVertexShader * GetGrassVertexShader()		{ return m_pGrassVertexShader; }
	inline A3DVertexShader * GetGrassRefractVertexShader() { return m_pGrassRefractVertexShader; }
	inline A3DVertexShader * GetCurGrassVertexShader() 
	{
		return m_bIsRenderForRefract ? m_pGrassRefractVertexShader : m_pGrassVertexShader;
	}

	inline A3DPixelShader * GetGrassRenderShader()		{ return m_pGrassRenderShader; }

	inline int GetGrassCount()							{ return m_nGrassCount; }
	inline void AddGrassCount(int nCount)				{ m_nGrassCount += nCount; }
	inline void SetA3DTerrain(A3DTerrain2 * pTerrain)	{ m_pA3DTerrain = pTerrain; }
	inline A3DTerrain2 * GetA3DTerrain()				{ return m_pA3DTerrain; }

	inline bool GetProgressLoad()						{ return m_bProgressLoad; }
	inline void SetProgressLoad(bool bFlag)				{ m_bProgressLoad = bFlag; }
	inline bool GetForceRebuild()						{ return m_bForceRebuild; }
	inline void SetForceRebuild(bool bFlag)				{ m_bForceRebuild = bFlag; }

	inline float GetHeightAt(const A3DVECTOR3& vecPos) 
	{
		return m_pA3DTerrain ? m_pA3DTerrain->GetPosHeight(vecPos) : 0.0f;
	}

	inline bool GetTerrainLMAt(const A3DVECTOR3& vecPos, ARectF *pArea, A3DTexture** ppDayLM, A3DTexture** ppNightLM)
	{
		if (m_pA3DTerrain)
		{
			return m_pA3DTerrain->GetPosLightMap(vecPos, pArea, ppDayLM, ppNightLM);
		}
		else
		{
			*ppDayLM = NULL;
			*ppNightLM = NULL;
			return false;
		}
	}

	inline float GetLODLevel()							{ return m_vLODLevel; }
	inline A3DVECTOR4 * GetWindOffset()					{ return m_vecWindOffset; }

	inline DWORD GetUpdateStartTime()					{ return m_dwUpdateStart; }

public:		//	Types

public:		//	Constructor and Destructor

	CELGrassLand();
	~CELGrassLand();

	bool Init(A3DDevice * pA3DDevice, A3DTerrain2 * pTerrain, ALog * pLogFile);
	bool Release();

	bool Update(const A3DVECTOR3& vecCenter, DWORD dwDeltaTime);
	bool Render(A3DViewport * pViewport, bool bRenderAlpha);
	bool RenderForRefract(A3DViewport* pViewport, float vRefractSurfaceHeight);

	bool Load(const char * szGrassLandFile);
	bool Save(const char * szGrassLandFile);

	bool SetLODLevel(float level);

	// grass type manipulations.
	bool AddGrassType(DWORD dwTypeID, CELGrassType::GRASSDEFINE_DATA& defData, int nMaxVisibleCount, const char * szGrassFile, CELGrassType ** pGrassType);
	bool DeleteGrassType(CELGrassType * pGrassType);

	inline int GetNumGrassType()					{ return m_GrassTypes.GetSize(); }
	inline CELGrassType* GetGrassType(int index)	{ return m_GrassTypes[index]; }

	CELGrassType * GetGrassTypeByID(DWORD dwTypeID);

	inline float GetDNFactor()					{ return m_fDNFactor; }
	void SetDNFactor(float f);

	// Delete all grass types, by jdl
	bool DeleteAllGrassTypes();
};

///////////////////////////////////////////////////////////////////////////
//	
//	Inline functions
//	
///////////////////////////////////////////////////////////////////////////



