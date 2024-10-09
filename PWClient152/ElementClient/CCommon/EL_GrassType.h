/*
 * FILE: EL_GrassType.h
 *
 * DESCRIPTION: Grass Type class for element
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
#include <ALog.h>
#include <A3DTerrain2.h>
#include <vector.h>

using namespace abase;

///////////////////////////////////////////////////////////////////////  
//	Grass Vertex Formats
///////////////////////////////////////////////////////////////////////
#define GRASS_FVF_VERTEX		D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1

/////////////////////////////////////////////////////////////////////// 
// Grass Vertex Structure
///////////////////////////////////////////////////////////////////////
struct GRASSVERTEX
{
	GRASSVERTEX() {}
	GRASSVERTEX(const A3DVECTOR3& _pos, const A3DVECTOR3& _normal, float _u, float _v)
	{
		pos = _pos;
		normal = _normal;
		u = _u;
		v = _v;
	}

	A3DVECTOR3			pos;
	A3DVECTOR3			normal;	// we put wind index and vertex weight in this struct
	float				u;		// u, v coords
	float				v;
};


///////////////////////////////////////////////////////////////////////////
//	
//	Define and Macro
//	
///////////////////////////////////////////////////////////////////////////

class A3DDevice;
class A3DEngine;
class A3DViewport;
class A3DStream;
class A3DTexture;
class CELGrassLand;
class AFile;

// grass block's size (in bits)
#define GRASS_BLOCKSIZE	16

///////////////////////////////////////////////////////////////////////////
//	
//	Declare of Global functions
//	
///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////
//	
//	Class CELGrassBlock
//		b block of grasses in one rectangle on the terrain
//	
///////////////////////////////////////////////////////////////////////////

class CELGrassBitsMap;
class CELGrassType;

class CELGrassBlock
{
public:
	struct SCANMODE
	{
		SCANMODE() { x1=y1=x2=y2=xs=ys=-1; }
		int x1, x2, xs;
		int y1, y2, ys;

		inline bool operator == (const SCANMODE& param2) { return x1 == param2.x1 &&
			x2 == param2.x2 && xs == param2.xs && y1 == param2.y1 && y2 == param2.y2 && ys == param2.ys; }

		inline bool operator != (const SCANMODE& param2) { return x1 != param2.x1 ||
			x2 != param2.x2 || xs != param2.xs || y1 != param2.y1 || y2 != param2.y2 || ys != param2.ys; }
	};

private:
	CELGrassBitsMap*				m_pGrassBitsMap;// the grass's bits map which this block resides on
	
	int								m_nNumGrasses;	// number of grasses
	GRASSVERTEX *					m_pGrassesVerts;// verts in this block

	A3DAABB							m_aabb;			// bounding box for visible determination
	A3DVECTOR3						m_vecCenter;	// center of this block

	ARectI							m_rectBlock;	// the range of this block

	bool							m_bBuilt;		// flag indicates whether this block's grasses has been built

	bool							m_bAboutToBuild;// flag indicates this block needs build
	bool							m_bAboutToDestroy; // flag indicates this block need to be destroyed

	SCANMODE						m_scanMode;		// current scan mode

public:
	inline const ARectI& GetBlockRect()		{ return m_rectBlock; }
	inline const A3DAABB& GetAABB()			{ return m_aabb; }
	inline const A3DVECTOR3& GetCenter()	{ return m_vecCenter; }

	inline bool IsBuilt()					{ return m_bBuilt; }
	inline bool IsAboutToBuild()			{ return m_bAboutToBuild; }
	inline bool IsAboutToDestroy()			{ return m_bAboutToDestroy; }
	inline void SetAboutToBuild()			{ m_bAboutToBuild = true; }
	inline void SetAboutToDestroy()			{ m_bAboutToDestroy = true; }
	inline void ClearAboutToBuild()			{ m_bAboutToBuild = false; }
	inline void ClearAboutToDestroy()		{ m_bAboutToDestroy = false; }

	inline int GetNumGrasses()				{ return m_nNumGrasses; }
	inline GRASSVERTEX * GetGrassesVerts()	{ return m_pGrassesVerts; }

	inline const SCANMODE& GetCurScanMode() { return m_scanMode; }

public:
	CELGrassBlock();
	~CELGrassBlock();

	// Function Init(CELGrassBitsMap * pBitsMap, const ARectI& rectBlock)
	//		Initialize the block, but not build
	// IN:
	//	pBitsMap		bits map of the terrain which this block blongs to
	//	rectBlock		the range of this block in the bits map
	//
	bool Init(CELGrassBitsMap * pBitsMap, const ARectI& rectBlock);
	bool Release();

	// Function BuildGrasses()
	//		Build grasses from the bits map
	//
	bool BuildGrasses();

	// Function DestroyGrasses()
	//		release all grasses that have been built, and set m_bBuilt to false
	//
	bool DestroyGrasses();
};

///////////////////////////////////////////////////////////////////////////
//	
//	Class CELGrassBitMap
//		a map used to determine where the grasses grow
//	
///////////////////////////////////////////////////////////////////////////

class CELGrassBitsMap
{
	friend class CELGrassType;

private:
	A3DDevice *			m_pA3DDevice;			// pointer to device object
	CELGrassType *		m_pGrassType;			// pointer to the grass type
	DWORD				m_dwID;					// id of this bits map

	A3DTexture *		m_pTerrainLM;			// light map get from the terrain
	A3DTexture *		m_pTerrainLM_n;			// light map get from the terrain for night
	ARectF				m_rectLM;				// light map cover range

	// bits map data
	BYTE *				m_pBitsMap;				// the buffer that contains bits map data
	int					m_nWidth;				// width of the bits map (int bits)
	int					m_nHeight;				// height of the bits map

	int					m_nNumBlockX;			// block column count
	int					m_nNumBlockY;			// block row count

	// location information
	A3DVECTOR3			m_vecCenter;			// center position of this bits map
	A3DVECTOR3			m_vecOrigin;			// origin position of this bits map
	float				m_vGridSize;			// grid size for each bit

	// blocks data
	int					m_nNumBlocks;			// number of blocks this bits map is devided
	CELGrassBlock **	m_ppBlocks;				// the blocks buffer

	typedef vector<CELGrassBlock *> BLOCKARRAY;

	BLOCKARRAY			m_VisibleBlocks;

	DWORD				m_dwExportID;			// id used when exporting game data
	bool				m_bAllBitsZero;			// All bits is zero

public:
	inline void SetID(DWORD dwID)			{ m_dwID = dwID; }
	inline DWORD GetID()					{ return m_dwID; }
	inline CELGrassType * GetGrassType()	{ return m_pGrassType; }
	inline BYTE * GetBitsMap()				{ return m_pBitsMap; }
	inline int GetWidth()					{ return m_nWidth; }
	inline int GetHeight()					{ return m_nHeight; }

	inline const A3DVECTOR3& GetCenter()	{ return m_vecCenter; }
	inline const A3DVECTOR3& GetOrigin()	{ return m_vecOrigin; }
	inline float GetGridSize()				{ return m_vGridSize; }

		//	Get / Set export ID
	inline void SetExportID(DWORD dwID)		{ m_dwExportID = dwID; }
	inline DWORD GetExportID()				{ return m_dwExportID; }

	inline bool GetAllBitsZeroFlag()		{ return m_bAllBitsZero; }

protected:
	bool CreateBlocks();
	bool ReleaseBlocks();

	bool UpdateVisibleBlocks(const A3DVECTOR3& vecCenter);

public:
	CELGrassBitsMap();
	~CELGrassBitsMap();

	// Function Init(vecCenter, float vGridSize, nWidth, nHeight)
	//		Initialize this bits map for the type of grass
	//
	//	vecCenter	the center of this bits map
	//	vGridSize	the grid size of each bit
	//	nWidth		the width of the bits map
	//	nHeight		the height of the bits map												 
	//
	bool Init(A3DDevice * pA3DDevice, CELGrassType * pGrassType, DWORD dwID, const A3DVECTOR3& vecCenter, float vGridSize, int nWidth, int nHeight);

	// Function Release()
	//		Dealloc the memory resources
	//
	bool Release();

	// Function Update(pBitsMap, ARectI * pRectUpdate)
	//		Update this bits map according to the passed in bits map
	//
	// IN
	//		pBitsMap		new bits map information
	//
	bool UpdateBitsMap(BYTE * pBitsMap);

	// call UpdateGrassesForRender after you called UpdateBitsMap
	bool UpdateGrassesForRender(A3DViewport * pViewport);

	// adjust after changing lod level
	bool AdjustForLODLevel();

	// Function Update(A3DViewport * pCurViewport)
	//		Load and unload some blocks according to current camera position
	//
	bool Update(const A3DVECTOR3& vecCenter);
	
	// Function Render(A3DViewport * pViewport)
	//		Render these grasses in this map
	//
	// IN
	//	pViewport			viewport to show the grass
	//	pStream				stream used to render
	//
	// RETURN
	//		return the new grasses that have been put into pBuffer
	bool Render(A3DViewport * pViewport);

	// Function ShiftCenterTo(const A3DVECTOR3& vecPos)
	//		shift the center of this grass bits map to a new position
	bool ShiftCenterTo(const A3DVECTOR3& vecPos);

	//	Used when export grass data. Return true if not all bits are zero,
	//	this function also set m_bAllBitsZero flag
	bool BitsMapCheck();
};

///////////////////////////////////////////////////////////////////////////
//	
//	Class CELGrassType
//	
///////////////////////////////////////////////////////////////////////////

class CELGrassType
{
public:		//	Types
	struct GRASSDEFINE_DATA
	{
		bool	bAlphaBlendEnable;		// flag indicates whether this kind of grass use alpha blend
		int		nAlphaRefValue;			// if not using alpha blend, we use alpha compare, this is the reference alpha value (0~255)

		// size section
		float	vSize;					// size scale of this type of grass
		float	vSizeVar;				// size varation (0.0~1.0)

		// softness
		float	vSoftness;				// softness for wind effect 0.0 means very hard to blow, 2.0 means too soft to blow (0.0~2.0)

		// sight range
		float	vSightRange;			// sight range of this kind of grass

		float	foo[8];					// for future expanding
	};

private:
	A3DDevice *						m_pA3DDevice;		// a3d device
	ALog *							m_pLogFile;			// log file to output messages
	CELGrassLand *					m_pGrassLand;		// grassland managing class	object

	bool							m_bBadGrass;		// flag indicates grass file missing from disk

	DWORD							m_dwTypeID;			// id of this type of tree

	char							m_szGrassFile[256];	// grass file name

	GRASSDEFINE_DATA				m_GrassDefineData;	// grass define data

	// render data section
	char							m_szGrassMap[MAX_PATH];	// grass texture map
	A3DTexture *					m_pTextureGrass;	// grass texture

	vector<CELGrassBitsMap*>		m_GrassBitsMaps;

	// data for construst a specified grass
	int								m_nNumOneGrassVert;	// number of verts for one grass
	int								m_nNumOneGrassFace;	// number of faces for one grass
	GRASSVERTEX *					m_pVertsTemplate;	// the vertex position for one grass
	WORD *							m_pIndicesTemplate;	// the indices for one grass

	DWORD							m_dwIDNext;			// next grass map's id

public:
	inline CELGrassLand * GetGrassLand()		{ return m_pGrassLand; }

	inline int GetNumOneGrassVert()				{ return m_nNumOneGrassVert; }
	inline int GetNumOneGrassFace()				{ return m_nNumOneGrassFace; }
	inline GRASSVERTEX * GetVertsTemplate()		{ return m_pVertsTemplate; }
	inline WORD * GetIndicesTemplate()			{ return m_pIndicesTemplate; }

	inline const GRASSDEFINE_DATA& GetDefineData()			{ return m_GrassDefineData; }
	
	inline const char * GetGrassFile()			{ return m_szGrassFile; }
	inline bool IsBadGrass()					{ return m_bBadGrass; }

	inline const char * GetGrassMap()			{ return m_szGrassMap; }

protected:
	DWORD GenerateID() { return m_dwIDNext ++; }

	// rebuild all grasses that are rendering now, you should call this function
	// after you have adjust this grass type's information
	bool RebuildAllGrasses(A3DViewport * pViewport);

public:	
	CELGrassType();
	~CELGrassType();

public:		//	Operations

	// initialize and finalize
	bool Init(A3DDevice * pA3DDevice, CELGrassLand * pGrassLand, DWORD dwTypeID, int nMaxVisibleCount, const char * szGrassFile, ALog * pLogFile, GRASSDEFINE_DATA * pDefineData);
	bool Release();
	
	// load and save
	bool Load(AFile * pFileToLoad);
	bool Save(AFile * pFileToSave);

	// update methods
	bool Update(const A3DVECTOR3& vecCenter);
	
	// Rendering methods
	bool RenderGrasses(A3DViewport * pViewport);

	// grass map management
	// add one bits map into this type grass and return the id in dwID
	bool AddGrassBitsMap(BYTE * pBitsMap, const A3DVECTOR3& vecCenter, float vGridSize, int nWidth, int nHeight, DWORD& dwID);
	// Get a grass bits map according to the id
	CELGrassBitsMap * GetGrassBitsMap(DWORD dwID);
	// remove one tree from this type tree
	bool DeleteGrassBitsMap(DWORD dwID);
	
	// Adjust the grass type's define data
	bool AdjustDefineData(A3DViewport * pViewport, const GRASSDEFINE_DATA& data);
	// Adjust the max visible count for this grass type
	bool AdjustMaxVisibleCount(A3DViewport * pViewport, int nMaxVisibleCount);

	// Adjust the grass's lod level
	bool AdjustForLODLevel();
	
	inline DWORD GetTypeID() { return m_dwTypeID; }
	inline int GetGrassBitsMapCount() { return m_GrassBitsMaps.size(); }
	inline CELGrassBitsMap* GetGrassBitsMapByIndex(int n) { return m_GrassBitsMaps[n]; }

protected:	//	Attributes
};

///////////////////////////////////////////////////////////////////////////
//	
//	Inline functions
//	
///////////////////////////////////////////////////////////////////////////

