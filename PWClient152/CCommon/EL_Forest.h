/*
 * FILE: EL_Forest.h
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Hedi, 2004/9/24
 *
 * HISTORY: 
 *
 * Copyright (c) 2004 Archosaur Studio, All Rights Reserved.
 */

#pragma once

#include <A3DTypes.h>
#include <A3DVertex.h>
#include <SpeedTreeRT.h>
#include <AArray.h>
#include <ALog.h>
#include <AC.h>
#include <A3DMaterial.h>
#include "aabbcd.h"
#include "EL_Tree.h"

using namespace CHBasedCD;
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

class A3DDevice;
class A3DCamera;
class A3DTexture;
class A3DStream;
class A3DLight;	
class A3DPixelShader;
class A3DVertexShader;

#define FOREST_VERSION		0x10000002
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

class CELForest
{
protected:
	A3DDevice *			m_pA3DDevice;

	A3DLight *			m_pDirLight;
	A3DVECTOR3			m_vecLightDir;

#ifdef TREE_BUMP_ENABLE
	A3DPixelShader *	m_pBranchPixelShader;
	A3DVertexShader *	m_pBranchVertexShader;
#endif

	ALog *				m_pLogFile;
	char				m_szConfigFile[256];

	APerlinNoise1D		m_wind;
	float				m_vTime;
	
	APtrArray<CELTree*>	m_TreeTypes;

	int					m_nTreeCount;

	int					m_nMaxBlades;		// leaves and billboard's buffer
	int					m_nBladesCount;
	A3DStream *			m_pStream;

	float				m_vLODLevel;		// 0.0f~1.0f, lod level

	A3DMaterial			m_material;

protected:
	bool SortTreeTypeByCompositeMap();

	bool CreateStream(int nMaxBlades);
	bool ReleaseStream();

public:		//	Types
	bool LoadTypesFromConfigFile(const char* szConfigFile);

	inline int GetTreeCount()				{ return m_nTreeCount; }
	inline void AddTreeCount(int nCount)	{ m_nTreeCount += nCount; }
	inline float GetLODLevel()				{ return m_vLODLevel; }
	inline const A3DVECTOR3& GetLightDir()	{ return m_vecLightDir; }

#ifdef TREE_BUMP_ENABLE
	inline A3DPixelShader * GetBranchPixelShader() { return m_pBranchPixelShader; }
	inline A3DVertexShader * GetBranchVertexShader() { return m_pBranchVertexShader; }
#endif

public:		//	Constructor and Destructor

	CELForest();
	~CELForest();

	bool Init(A3DDevice * pA3DDevice, A3DLight * pDirLight, ALog * pLogFile);
	bool Release();

	bool Update(DWORD dwDeltaTime);
	bool Render(A3DViewport * pViewport);

	bool Load(const char * szForestFile);
	bool Save(const char * szForestFile);

	// Change forest's LOD distance level (0.0f~1.0f)
	bool SetLODLevel(float level);

	float GetWindStrength(const A3DVECTOR3& vecPos);

	// tree types manipulation
	CELTree* AddTreeType(DWORD dwTypeID, const char * szTreeFile, const char * szCompositeMap);
	bool DeleteTreeType(CELTree * pTreeType);
	
	inline int GetNumTreeType()				{ return m_TreeTypes.GetSize(); }
	inline CELTree* GetTreeType(int index)	{ return m_TreeTypes[index]; }

	CELTree * GetTreeTypeByID(DWORD dwTypeID);
	
	// Sphere- Collision Detection, for editor only
	bool SphereCollideWithMe(const A3DVECTOR3& vecStart, float fRadius, const A3DVECTOR3& vecDelta, A3DVECTOR3& vecHitPos, float * pvFraction, A3DVECTOR3& vecNormal);

	bool TraceWithBrush(BrushTraceInfo * pInfo);
	
	// Add tree, by jdl
	bool AddTree(int nIndex, const A3DVECTOR3& vPos);
	// Delete all tree types, by jdl
	bool DeleteAllTreeTypes();
	// Get tree type count, by jdl
	int GetTreeTypeNumber();

	//Get A3DStream ,by liyi
	A3DStream* GetStream(){ return m_pStream;};
};

///////////////////////////////////////////////////////////////////////////
//	
//	Inline functions
//	
///////////////////////////////////////////////////////////////////////////



