/*
 * FILE: A3DPlants.h
 *
 * DESCRIPTION: Class used to operating the plants in the world
 *
 * CREATED BY: Hedi, 2002/1/25
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#ifndef _A3DPLANTS_H_
#define _A3DPLANTS_H_

#include "A3DVertex.h"
#include "A3DTrace.h"

// Max textures that allowed in one tree
#define MAX_TEXTURE	4

// Max tree types in the world
#define TREE_MAX_TYPE		8
// Max grass types in the world
#define GRASS_MAX_TYPE		8

// Max trees in the world
#define TREE_MAX_NUM		1024
// Max grasses in the world
#define GRASS_MAX_NUM		1024

// Max vertex renderered at the same time;
#define VERTEX_MAX_NUM		1024
// Max faces renderered at the same time;
#define FACE_MAX_NUM		1024

class A3DDevice;
class A3DFrame;
class A3DMesh;
class A3DTexture;
class A3DStream;
class A3DViewport;

typedef struct _TREEINFO
{
	A3DVECTOR3		vecPos;
	A3DVECTOR3		vecDir;
	A3DVECTOR3		vecUp;
	//... other infos can follow here;
} TREEINFO, * PTREEINFO;

typedef struct _GRASSINFO
{
	A3DVECTOR3		vecPos;
	A3DVECTOR3		vecDir;
	A3DVECTOR3		vecUp;
	//... other infos can follow here;
	A3DVECTOR3		vecScale; // the scale factor of this grass
} GRASSINFO, * PGRASSINFO;

class A3DPlants
{
public:		//	Types

	struct TRACERT
	{
		int		iTreeIndex;		//	Index of tree
	};

private:
	// The tree's information for different type trees;
	typedef struct _TREETYPE
	{
		char			szTreeFile[MAX_PATH]; // the file that the tree was loaded;
		char			szSpriteFile[MAX_PATH]; // the sprite file

		A3DVECTOR3		vecScale; // Scale factor of this kind of tree

		A3DFrame		*pFrame;
		int				nNumTex;
		A3DMesh			*pMeshes[MAX_TEXTURE];
		A3DTexture		*pTreeTex[MAX_TEXTURE];

		A3DTexture		*pSpriteTex; // The texture that used to render a 2d sprite of the far away trees
		int				nDegNum; // How many degrees are there in the sprite texture
		FLOAT			vTexDU; // U delta for each degree;
		FLOAT			vTexDV; // V delta for each degree;
		int				nTexRow;
		int				nTexCol;
		FLOAT			vWidth;	//Used to represent the 2D sprite's width
		FLOAT			vHeight;//Used to represent the 2D sprite's height

		int				nNumTree; // How many trees of this type are there in the world;
		WORD			wTreeIndex[TREE_MAX_NUM]; // index into array m_pTrees;
	} TREETYPE;

	typedef struct _GRASSTYPE
	{
		char			szGrassFile[MAX_PATH]; // the file that the grass was loaded;

		A3DVECTOR3		vecScale; // Scale factor of this kind of grass
	
		A3DFrame		*pFrame;
		int				nNumTex;
		A3DMesh			*pMeshes[MAX_TEXTURE];
		A3DTexture		*pGrassTex[MAX_TEXTURE];

		int				nNumGrass; // How many grasses of this type are there in the world;
		WORD			wGrassIndex[GRASS_MAX_NUM];	// index into array m_pGrasses
	} GRASSTYPE;

	typedef struct _TREE
	{
		TREETYPE		*pType;
		int				nFrame;
		int				nTreeTypeIndex; // Index in the tree type record;

		A3DMATRIX4		matTM;
		A3DOBB			obb;		//	Used to perform ray tracing and obb tracing;
		A3DAABB			aabb;		//	Used to determine visiblity
		OBBBEVELS		Bevels;		//	OBB bevels

	} TREE;

	typedef struct _GRASS
	{
		GRASSTYPE		*pType;
		int				nFrame;
		int				nGrassTypeIndex; // Index in the tree type record;

		A3DMATRIX4		matTM;
	} GRASS;

	A3DDevice		*m_pA3DDevice;
	bool			m_bHWIPlants; // Using outside graphics engine;

	bool			m_bShowBox;  // Show bounding box of each tree;

	FLOAT			m_vTreeDis1; // Show 2D distance;
	FLOAT			m_vTreeDis2; // Begin Disappear distance;
	FLOAT			m_vTreeDis3; // Total Disappear distance;

	FLOAT			m_vGrassDis1; // Begin to fade distance;
	FLOAT			m_vGrassDis2; // End fading distance;
	
	int				m_nNumTree;
	int				m_nNumGrass;

	int				m_nNumTreeType;
	TREETYPE		m_pTreeTypes[TREE_MAX_TYPE];
	TREE			m_pTrees[TREE_MAX_NUM]; // an array that describe all trees

	int				m_nNumGrassType;
	GRASSTYPE		m_pGrassTypes[GRASS_MAX_TYPE];
	GRASS			m_pGrasses[GRASS_MAX_NUM];

	// We use a common vertex buffer for rendering the real trees or grasses, 
	// we render one type tree or grass for one time;
	int				m_nVertNum[MAX_TEXTURE];
	int				m_nFaceNum[MAX_TEXTURE];
	A3DLVERTEX		m_pVerts[MAX_TEXTURE][VERTEX_MAX_NUM];
	WORD			m_pIndices[MAX_TEXTURE][FACE_MAX_NUM * 3];
	
	// The vertex buffers for far away sprites rendering;
	int				m_nVertSpriteNum;
	int				m_nFaceSpriteNum;
	A3DLVERTEX		m_pVertsSprite[VERTEX_MAX_NUM];
	WORD			m_pIndicesSprite[FACE_MAX_NUM * 3];

	// A3DStream used to rendering;
	A3DStream		*m_pA3DStream;

	bool			m_bRayTraceEnable;
	bool			m_bAABBTraceEnable;

	TRACERT			m_RayTraceRt;
	TRACERT			m_AABBTraceRt;

protected:
	TREETYPE * FindTreeType(char * szTreeFile);
	GRASSTYPE * FindGrassType(char * szGrassFile);

	bool RenderToBuffer(A3DMesh * pMesh, int nStartVert, A3DLVERTEX * pVertexBuffer, WORD * pIndices, A3DMATRIX4 absoluteTM,  int nCurrentFrame, A3DCOLOR diffuse, A3DCOLOR specular);

	bool AddTree(char * szTreeFile, TREEINFO * pTreeInfo);
	bool AddGrass(char * szGrassFile, GRASSINFO * pGrassInfo);

	bool UpdateAllTreesScale(TREETYPE * pTreeType);
	bool UpdateAllGrassesScale(GRASSTYPE * pGrassType);

public:
	A3DPlants();
	~A3DPlants();

	bool Init(A3DDevice * pA3DDevice);
	bool Release();

	bool Reset();

	// return false means no collision detected;
	bool AABBTrace(AABBTRACEINFO* pInfo, AABBTRACERT* pTrace);
	bool RayTrace(const A3DVECTOR3& vecStart, const A3DVECTOR3& vecDelta, RAYTRACERT* pRayTrace);

	bool AddTreeType(char * szTreeFile, char * szSpriteFile, int nDegNum, int nTexRow, int nTexCol, FLOAT vTexDU, FLOAT vTexDV, DWORD *pTreeHandle, A3DVECTOR3& vecScale=A3DVECTOR3(1.0f, 1.0f, 1.0f));
	bool SetTreeTypeScale(DWORD dwTreeHandle, A3DVECTOR3& vecScale);
	bool AddTree(DWORD dwTreeHandle, TREEINFO * pTreeInfo, int * pTreeIndex=NULL);
	bool DelTree(int nTreeIndex, int * pNewTreeIndex=NULL);

	bool AddGrassType(char * szGrassFile, DWORD * pGrassHandle, A3DVECTOR3& vecScale=A3DVECTOR3(1.0f, 1.0f, 1.0f));
	bool SetGrassTypeScale(DWORD dwGrassHandle, A3DVECTOR3& vecScale);
	bool AddGrass(DWORD dwGrassHandle, GRASSINFO * pGrassInfo, int * pGrassIndex=NULL);
	bool DelGrass(int nGrassIndex, int * pNewGrassIndex=NULL);

	bool Render(A3DViewport * pCurrentViewport);

	bool RenderTrees(A3DViewport * pCurrentViewport);
	bool RenderGrasses(A3DViewport * pCurrentViewport);

	inline int GetTreeNum() { return m_nNumTree; }
	inline int GetGrassNum() { return m_nNumGrass; }
	inline TREE* GetTreeInfo(int nTreeIndex) 
	{
		if( nTreeIndex < m_nNumTree )
			return &m_pTrees[nTreeIndex]; 
		else
			return NULL;
	}
	void UpdateTreeInfo(int nTreeIndex, TREEINFO * pTreeInfo);
	
	inline GRASS* GetGrassInfo(int nGrassIndex)
	{ 
		if( nGrassIndex < m_nNumGrass )
			return &m_pGrasses[nGrassIndex]; 
		else
			return NULL;
	}
	void UpdateGrassInfo(int nGrassIndex, GRASSINFO * pGrassInfo);

	inline FLOAT GetGrassDis1()				{ return m_vGrassDis1; }
	inline FLOAT GetGrassDis2()				{ return m_vGrassDis2; }
	inline void SetGrassDis1(FLOAT vDis)	{ m_vGrassDis1 = vDis; }
	inline void SetGrassDis2(FLOAT vDis)	{ m_vGrassDis2 = vDis; }
	
	inline void SetRayTraceEnable(bool bEnable) { m_bRayTraceEnable = bEnable; }
	inline void SetAABBTraceEnable(bool bEnable) { m_bAABBTraceEnable = bEnable; }
	inline bool GetRayTraceEnable() { return m_bRayTraceEnable; }
	inline bool GetAABBTraceEnable() { return m_bAABBTraceEnable; }
	inline bool GetShowBox() { return m_bShowBox; }
	inline void SetShowBox(bool bShowBox) { m_bShowBox = bShowBox; }

	inline const TRACERT& GetRayTraceResult() { return m_RayTraceRt; }
	inline const TRACERT& GetAABBTraceResult() { return m_AABBTraceRt; }
};

typedef class A3DPlants* PA3DPlants;
#endif//_A3DPLANTS_H_
