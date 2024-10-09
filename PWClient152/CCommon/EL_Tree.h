/*
 * FILE: EL_Tree.h
 *
 * DESCRIPTION: Tree class for one type in element
 *
 * CREATED BY: Hedi, 2004/9/23
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
#include <A3DGeometry.h>

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
class CELForest;

class AFile;

#include "ConvexHullData.h"
#include "aabbcd.h"
using namespace CHBasedCD;

///////////////////////////////////////////////////////////////////////////
//	
//	Types and Global variables
//	
///////////////////////////////////////////////////////////////////////////

/*
#define TREE_BUMP_ENABLE
*/

///////////////////////////////////////////////////////////////////////  
//	Branch & Frond Vertex Formats
///////////////////////////////////////////////////////////////////////
#define TREE_FVF_VERTEX1		D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1
#define TREE_FVF_VERTEX2		D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX2

/////////////////////////////////////////////////////////////////////// 
// Branch Vertex Structure
///////////////////////////////////////////////////////////////////////

struct TREE_VERTEX1
{
	TREE_VERTEX1() {}
	TREE_VERTEX1(const A3DVECTOR3& _pos, const A3DVECTOR3& _normal, float _u, float _v)
	{
		pos = _pos;
		normal = _normal;
		u = _u;
		v = _v;
	}

	A3DVECTOR3			pos;
	A3DVECTOR3			normal;
	float				u;
	float				v;
};

struct TREE_VERTEX2
{
	TREE_VERTEX2() {}
	TREE_VERTEX2(const A3DVECTOR3& _pos, const A3DVECTOR3& _normal, float _u, float _v, float _su, float _sv)
	{
		pos = _pos;
		normal = _normal;
		u = _u;
		v = _v;
		su = _su;
		sv = _sv;
	}

	A3DVECTOR3			pos;
	A3DVECTOR3			normal;
	float				u;
	float				v;

	float				su;
	float				sv;
};

#ifdef TREE_BUMP_ENABLE
#define TREE_FVF_BRANCHVERTEX	D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX3 | D3DFVF_TEXCOORDSIZE3(1) | D3DFVF_TEXCOORDSIZE3(2)
struct TREE_BRANCHVERTEX
{
	TREE_BRANCHVERTEX() {}
	TREE_BRANCHVERTEX(const A3DVECTOR3& _pos, const A3DVECTOR3& _normal, float _u, float _v, float _tx, float _ty, float _tz, float _bx, float _by, float _bz)
	{
		pos = _pos;
		normal = _normal;
		u = _u;
		v = _v;
		tangent[0] = _tx;
		tangent[1] = _ty;
		tangent[2] = _tz;
		binormal[0] = _bx;
		binormal[1] = _by;
		binormal[2] = _bz;
	}

	A3DVECTOR3			pos;
	A3DVECTOR3			normal;
	float				u;
	float				v;

	float				tangent[3];
	float				binormal[3];
};
#else
#define TREE_FVF_BRANCHVERTEX		D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1
struct TREE_BRANCHVERTEX
{
	TREE_BRANCHVERTEX() {}
	TREE_BRANCHVERTEX(const A3DVECTOR3& _pos, const A3DVECTOR3& _normal, float _u, float _v)
	{
		pos = _pos;
		normal = _normal;
		u = _u;
		v = _v;
	}

	A3DVECTOR3			pos;
	A3DVECTOR3			normal;
	float				u;
	float				v;
};
#endif

///////////////////////////////////////////////////////////////////////////
//	
//	Declare of Global functions
//	
///////////////////////////////////////////////////////////////////////////

#define TREE_VERSION			0x10001000

///////////////////////////////////////////////////////////////////////////
//	
//	Class CELTree
//	
///////////////////////////////////////////////////////////////////////////

#define TREEUNIT_SCALE (0.0254f * 12)

class CELTree
{
	friend class CELForest;

public:		//	Types
	struct ELTREE
	{
		DWORD			dwID;			// id of this tree
		CSpeedTreeRT*	pTreeInstance;	// instanced tree object, it has same orient data as origin one
		A3DVECTOR3		vecPos;			// position of this tree object
		BOOL			bVisible;		// flag indicates whether it is visible for this tree

		int				nBranchLOD;		// current branch lod level
		int				nFrondLOD;		// current frond lod level
		int				nLeafLOD;		// current leaf lod level

		A3DAABB			aabb;			// aabb of this tree
	};

private:
	A3DDevice *						m_pA3DDevice;		// a3d device
	ALog *							m_pLogFile;			// log file to output messages
	CELForest *						m_pForest;			// forest this tree belongs to

	bool							m_bBadTree;			// flag indicates tree missing from disk

	DWORD							m_dwTypeID;			// id of this type of tree
	char							m_szTreeName[256];	// tree's name
	char							m_szSPTFile[256];	// speed tree file name
	char							m_szCompositeMap[256]; // composite map file name
	char							m_szBranchMap[256];	// branch map file name

#ifdef TREE_BUMP_ENABLE
	char							m_szBranchNormalMap[256]; // branch normal map file name
	A3DTexture *					m_pTextureBranchNormal;	// branch normal map
#endif

	DWORD							m_dwIDNext;			// id management

	// orient data of this type
	A3DVECTOR3						m_vecDir;			
	A3DVECTOR3						m_vecUp;
	A3DVECTOR3						m_vecPos;	

	// speed tree data section
	CSpeedTreeRT*					m_pSpeedTree;		// the original tree object
	CSpeedTreeRT::STextures*		m_pTextureInfo;		// the texture information of this type tree
	CSpeedTreeRT::SGeometry*		m_pGeometryCache;	// the geometry cache of this type tree
	float							m_afBoundingBox[6];	// tree's bounding box;

	// render data section
	A3DTexture *					m_pTextureComposite;	// composite map this tree uses
	A3DTexture *					m_pTextureBranch;		// branch texture
	
	// branch vertex buffers
	int								m_nBranchVertCount;		// vertex count of branch, verts are the same for all lod
	int *							m_pBranchIndexCount;	// index count of branch in all lod
	TREE_BRANCHVERTEX *				m_pBranchVerts;			// branch vertex buffer
	WORD *							m_pBranchIndices;		
	A3DStream *						m_pBranchStream;		// branch rendering stream

	// frond vertex buffers
	int								m_nFrondVertCount;		// vertex count of frond, verts are the same for all lod
	int *							m_pFrondIndexCount;		// index count of frond in all lod
	TREE_VERTEX1 *					m_pFrondVerts;			// frond vertex buffer
	WORD *							m_pFrondIndices;		
	A3DStream *						m_pFrondStream;			// frond rendering stream

	int								m_nCurLeavesCount;	// current leaves count
	int								m_nCurBillboardCount;// current billboard count

	int								m_nNumCDObject;			// number of collision objects
	CConvexHullData **				m_ppConvexHulls;		// convex hull objects for this tree type
	int								m_nNumCDBrushes;		// number of collision brushes
	CCDBrush **						m_ppCDBrushes;			// collision brushes

protected:
	DWORD GenerateID() { return m_dwIDNext ++; }

public:	
	CELTree();
	virtual ~CELTree();

	AArray<ELTREE, ELTREE&> m_Trees;

public:		//	Operations

	// initialize and finalize
	bool Init(A3DDevice * pA3DDevice, CELForest * pForest, DWORD dwTypeID, const char * szTreeFile, const char * szCompositeMap, ALog * pLogFile);
	bool Release();
	bool ReComputeAllTrees();

	// load and save
	bool LoadTrees(AFile * pFileToLoad);
	bool SaveTrees(AFile * pFileToSave);

	// Rendering methods
	bool UpdateForRender(A3DViewport * pViewport);

	bool RenderBranches(A3DViewport * pViewport);
	bool RenderFronds(A3DViewport * pViewport);
	bool DrawLeavesAndBillboardsToBuffer(TREE_VERTEX1 * pVerts, int nMaxBlade, int& nBladeCount, int& nTree);
	
	bool GetFrondsData();
	bool GetLeafData();
	bool GetBillboardData();

	// tree management
	// add one tree into this type tree
	bool AddTree(const A3DVECTOR3& vecPos, DWORD& dwID);
	// remove one tree from this type tree
	bool DeleteTree(DWORD dwID);
	// Get tree aabb
	bool GetTreeAABB(DWORD dwID, A3DAABB& aabb);
	
	// adjust one tree's position
	bool AdjustTreePosition(DWORD dwID, const A3DVECTOR3& vecPos);

	inline DWORD GetTypeID()					{ return m_dwTypeID; }
	inline const char * GetTreeName()			{ return m_szTreeName; }
	inline int GetTreeCount()					{ return m_Trees.GetSize(); }
	inline ELTREE& GetTree(int nIndex)			{ return m_Trees[nIndex]; }

	inline A3DTexture * GetCompositeTexture()	{ return m_pTextureComposite; }
	inline const char * GetTreeFile()			{ return m_szSPTFile; }
	inline const char * GetCompositeMap()		{ return m_szCompositeMap; }
	inline const char * GetBranchMap()			{ return m_szBranchMap; }
#ifdef TREE_BUMP_ENABLE
	inline const char * GetBranchNormalMap()	{ return m_szBranchNormalMap; }
#endif
	inline bool IsBadTree()						{ return m_bBadTree; }
	inline int GetNumBrushes()					{ return m_nNumCDBrushes; }
	inline CCDBrush * GetBrush(int nIndex)		{ return m_ppCDBrushes[nIndex]; }
	inline int GetNumHull()						{ return m_nNumCDObject; }
	inline CConvexHullData * GetHull(int nIndex)	{ return m_ppConvexHulls[nIndex]; }

protected:	//	Attributes

protected:	//	Operations
	bool ReleaseAllTrees();
	int GetTreeIndexByID(DWORD dwID);

	bool CreateVertexBuffers();
	bool ReleaseVertexBuffers();

	bool CreateBranchVertexBuffer();
	bool ReleaseBranchVertexBuffer();

	bool CreateFrondVertexBuffer();
	bool ReleaseFrondVertexBuffer();

protected:
	bool InitCDObjects();
	bool ReleaseCDObjects();

	// Ray trace with branch meshes
	bool RayTraceBranchMesh(const A3DVECTOR3& vecLocalStart, const A3DVECTOR3& vecDelta, A3DVECTOR3& vecHitPos, float * pvFraction, A3DVECTOR3& vecNormal);

public:
	// Collision detection methods.
	bool RayTraceConvexHull(const A3DVECTOR3& vecStart, const A3DVECTOR3& vecDelta, A3DVECTOR3& vecHitPos, float * pvFraction, A3DVECTOR3& vecNormal);

	bool TraceWithBrush(BrushTraceInfo * pInfo);

	// Sphere- Collision Detection, for editor only
	bool SphereCollideWithMe(const A3DVECTOR3& vecStart, float fRadius, const A3DVECTOR3& vecDelta, A3DVECTOR3& vecHitPos, float * pvFraction, A3DVECTOR3& vecNormal);
	
	//by liyi
	AArray<ELTREE, ELTREE&>* GetTreeArray(){ return &m_Trees;};

	int GetBranchVertexCount()	{	return m_nBranchVertCount;};
	TREE_BRANCHVERTEX* GetBranchVertexBuffer()	{ return m_pBranchVerts;};

	int GetBranchIndexCount(int nLod)		
	{ 
		if( !m_pBranchIndexCount )
		{
			return 0;
		}
		else
		{
			return m_pBranchIndexCount[nLod];
		}
	}
	unsigned short* GetBranchIndexBuffer() { return m_pBranchIndices;};

	int GetFrondVertexCount() { return m_nFrondVertCount;};
	TREE_VERTEX1* GetFrondVertexBuffer() { return m_pFrondVerts;};
	int GetFrondIndexCount( int nLod) 
	{ 
		if(!m_pFrondIndexCount)
		{
			return 0;
		}
		else
		{
			return m_pFrondIndexCount[nLod];
		}
	}
	
	unsigned short* GetFrondIndexBuffer() { return m_pFrondIndices;};
	
	bool DrawLeavesToBufferForShadowMap(TREE_VERTEX1 * pVerts, int nMaxBlade, int& nBladeCount, int& nTree);
};

///////////////////////////////////////////////////////////////////////////
//	
//	Inline functions
//	
///////////////////////////////////////////////////////////////////////////
#define AGBR2ARGB(dwColor) (dwColor & 0xff00ff00) + ((dwColor & 0x00ff0000) >> 16) + ((dwColor & 0x000000ff) << 16)


