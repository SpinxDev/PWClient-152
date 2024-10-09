/*
 * FILE: AutoTerrain.h
 *
 * DESCRIPTION: Class for automaticlly generating terrain
 *
 * CREATED BY: Jiang Dalong, 2005/01/04
 *
 * HISTORY: 
 *
 * Copyright (c) 2001-2008 Archosaur Studio, All Rights Reserved.
 */

#ifndef _AUTOTERRAIN_H_
#define _AUTOTERRAIN_H_

#include <ABaseDef.h>
#include "PerlinNoise2D.h"
#include "A3DTypes.h"

extern const int MASK_SIZE;
extern const int MAX_HEIGHT;

class CAutoHome;
class CHomeTerrain;
class CAutoScene;
class CELBitmap;
class A3DTexture;
class A3DStream;

class CAutoTerrain
{
public:	// Types

	//	Height map data
	struct HEIGHTMAP
	{
		int			iWidth;			//	Size of height map
		int			iHeight;
		float*		pHeightData;	//	Raw height data
		float*		pAbsHeight;		//	Absolute height data
		float*		pAbsHeightBak;	//	Backuped absolute height data
		float*		pAbsHeightOri;	//	Origin absolute height data
		float*		pAbsHeightUp;	//	Up displacement of height data
		float*		pAbsHeightDown;	//	Down displacement of height data
		float*		pAbsHeightRoad;	//	Road displacement of height data
		A3DVECTOR3*	aNormals;		//	Vertex normal data
	};

	//	Vertex info
	struct VERTEXINFO
	{
		int		x;			//	x coordinate
		int		z;			//	z coordinate
		float	fHeight;	//	height
	};

	//	Up and down PRI
	enum UPDOWNPRI
	{
		UDP_BOTH = 0,	//	Add up and down
		UDP_UP,			//	If up, no down
		UDP_DOWN,		//	If down, no up
	};

public:

	CAutoTerrain(CAutoHome* pAutoHome);
	virtual ~CAutoTerrain();

public:		//	Attributes

public:

	//	Initialize terrain
	bool Init();
	//	Release
	void Release();

	//	Update terrain
	bool CreateTerrainHeight(float fAmplitude, int nFrequency, float fPersistence, DWORD dwRandSeed);
	//	Reset terrain
	void ResetTerrain(bool bApply=true);
	// Restore base terrain
	bool RestoreBaseTerrain(bool bApply=true);

	//	Apply terrain height and normal
	bool ApplyHeightAndNormal(bool bUpdateNormal=true);
	//	Apply terrain vertices color
	bool ApplyTerrainColor(const DWORD* aColors1, const DWORD* aColors2);
	//	Apply terrain vertices color
	bool ApplyTerrainColor();

	// Get vertices of min and max height
	bool GetExtremeVertices(VERTEXINFO& min, VERTEXINFO& max);
	// Get vertices of min and max height in rect
	bool GetExtremeVerticesInRect(VERTEXINFO& min, VERTEXINFO& max, ARectI rect);
	// Update absolute height by up and down at a position
	bool UpdateAbsHeightAtPos(int nPos);
	// Update Absolute height by up and down
	bool UpdateAbsHeight();
	// Clear all hill
	bool ClearAllHill(bool bUpdate);
	// Clear road height
	bool ClearRoadHeight(bool bUpdate);
	// Get height weight by x,y weights
	float GetHeightWeight(float fWeightX, float fWeightY);
	//	Update vertex normals
	bool UpdateVertexNormals();
	//	Reset height data for file
	bool ResetHeightData(float fMaxHeight);
	
	//	Get terrain height of specified position
	float GetPosHeight(const A3DVECTOR3& vPos);
	// Get height at a special position from height map
	float GetPosHeightFromHeightmap(const A3DVECTOR3& vPos);
	// Get local terrain area
	void GetTerrainAreaLocal(ARectF& rcTerrain);
	// Smooth height map border
	bool SmoothHeightBorder();

	inline CAutoHome* GetAutoHome() { return m_pAutoHome; }
	inline HEIGHTMAP* GetHeightMap() { return &m_HeightMap; }
	inline bool GetReady() { return m_bReady; }
	inline void SetReady(bool bReady) { m_bReady = bReady; }
	inline float GetMaxTerrainHeight() { return m_MaxVertex.fHeight; }
	inline float GetMinTerrainHeight() { return m_MinVertex.fHeight; }
	inline float GetTerrainSize() { return m_fTerrainSize; }
	inline float GetTileSize() { return m_fTileSize; }
	inline void SetNeedRefreshHeight(bool bNeed) { m_bNeedRefreshHeight = bNeed; }
	inline void SetUpDownPri(int nPri) { m_nUpDownPri = nPri; }
	inline int GetUpDownPri() { return m_nUpDownPri; }
	
	//	Get terrain area in meters
	const ARectF& GetTerrainArea() { return m_rcTerrain; }
	//	Get height scale of terrain
	float GetMaxHeight() { return m_fMaxHeight; }
	//	Get offset of terrain on Y axis
	float GetYOffset() { return m_fYOffset; }
	//	Blur a area
	bool BlurArea(float* pData, int nWidth, int nHeight);

protected:	// Attributes

	CAutoHome*		m_pAutoHome;		// 	Home object
	CAutoScene*		m_pAutoScene;		//	Scene object
	CHomeTerrain*	m_pHomeTerrain;		// 	Terrain object

	HEIGHTMAP		m_HeightMap;		// 	Height map
	bool			m_bReady;			// 	Height map ready

	float			m_fTerrainSize;		// 	Terrain size(meter)
	float			m_fTileSize;		// 	Tile size(meter)
	ARectF			m_rcTerrain;		// 	Terrain rect(meter)
	VERTEXINFO		m_MinVertex;		// 	Vertex with min height
	VERTEXINFO		m_MaxVertex;		// 	Vertex with max height
	bool			m_bNeedRefreshHeight;//	 Need refresh height

	float			m_fMaxHeight;		//	Terrain's height scale, this isn't the maximum absolute height of terrain
	float			m_fYOffset;			//	Terrain's offset on Y axis
	int				m_iOldLODType;

	int				m_nUpDownPri;		// Terrain up and down PRI
										// 0:Add hill and valley; 1: If hill, ignore valley; 2:If valley, ignore hill

protected:	//	Operations

	//	Create height map
	bool CreateHeightMap(float fAmplitude, int nFrequency, float fPersistence, DWORD dwRandSeed);
	//	Release height map
	void ReleaseHeightMap();

	//	Calculate terrain area
	void CalcTerrainArea();
	//	Create absolute height data
	bool CreateAbsHeightData();
	//	Get real height
	float GetRealHeight(int n) { return m_fYOffset + m_HeightMap.pHeightData[n] * m_fMaxHeight; }
};

#endif // #ifndef _AUTOTERRAIN_H_
