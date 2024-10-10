/*
 * FILE: A3DTerrain.h
 *
 * DESCRIPTION: Class for respresenting the terrain in A3D Engine
 *
 * CREATED BY: Hedi, 2001/12/4
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#ifndef _A3DTERRAIN_H_
#define _A3DTERRAIN_H_

#include "A3DPlatform.h"
#include "A3DVertex.h"
#include "A3DMacros.h"
#include "A3DControl.h"
#include "A3DTrace.h"

class A3DLight;
class A3DStream;
class A3DTexture;
class A3DDevice;
class A3DCameraBase;
class A3DViewport;

struct TERRAINPARAM
{
	int			nWidth;						// The width of terrain rectangle;
	int			nHeight;					// The height of terrain rectangle;
	int			nSightRange;				// The range of rendering area;
	int			nTextureCover;				// The width and height of one texture covered area;
	int			nNumTexture;				// The number of texture used;
	FLOAT		vCellSize;					// The size of each cell;

	FLOAT		vOffset;					// The horizontal plane's height;
	FLOAT		vScale;						// The scale factor of the grayscale map;
};

class A3DTerrain : public A3DControl
{
public:		//	Types

	enum
	{
		MAXNUM_TEXTURE		= 8,
		MAXNUM_MARKVERT		= 32,
		MAXNUM_MARKINDEX	= 64,
	};

	enum TRIANGLE_TYPE
	{
		TRIANGLE_HALF = 1,
		TRIANGLE_QUARTER
	};

	struct TERRAIN_MARKSPLIT
	{
		A3DLVERTEX*	pVerts;
		WORD*		pIndices;
		int			nNumVert;
		int			nNumIndices;
	};

private:
	char				m_szFolderName[MAX_PATH];

	// A rect range defining a region that will not be rendered;
	bool				m_bHasHollowRect;
	A3DRECT				m_rectHollow;

	A3DDevice			* m_pA3DDevice;
	A3DCameraBase		* m_pA3DCamera;
	A3DViewport			* m_pA3DViewport;

	bool				m_bHWITerrain; // Terrain that will not rendering;
	bool				m_bNoLight;	   // Flag indicate whether the terrain should not be light (if not light terrain will be pure white)

	A3DVECTOR3			m_vecCamPos;
	POINT				m_ptCamPos;
	POINT				m_ptCamPosOld;

	A3DStream			* m_pStreams[MAXNUM_TEXTURE];
	A3DTexture			* m_pTextures[MAXNUM_TEXTURE];

	FLOAT				m_vOptimizeStrength;

	//Normal vertex buffer and index buffers;
	A3DLVERTEX			* m_pVertexBuffers[MAXNUM_TEXTURE];
	WORD				* m_pIndexBuffers[MAXNUM_TEXTURE];
	int					m_nVisibleVerts[MAXNUM_TEXTURE];
	int					m_nVisibleFaces[MAXNUM_TEXTURE];

	int					m_nMaxVerts[MAXNUM_TEXTURE];
	int					m_nMaxIndices[MAXNUM_TEXTURE];
	int					m_nMaxExtraVerts[MAXNUM_TEXTURE];
	int					m_nMaxExtraIndices[MAXNUM_TEXTURE];

	//Extra vertex and index buffer which are used to add extra patch 
	//to avoid the seam between two cell;
	A3DLVERTEX			* m_pExtraVertexBuffers[MAXNUM_TEXTURE];
	WORD				* m_pExtraIndexBuffers[MAXNUM_TEXTURE];
	int					m_nExtraVisibleVerts[MAXNUM_TEXTURE];
	int					m_nExtraVisibleFaces[MAXNUM_TEXTURE];

	//Now max stage count is 8;
	int					m_nCellStageCount;
	int					m_nStageWidth[8];
	int					m_nStageHeight[8];
	int					m_nStageCover[8];

	CHAR				** m_ppCellLevelTable;
	FLOAT				** m_ppCellSquareErrorTable;

	FLOAT				* m_pCellDistanceTable;

	FLOAT				* m_pHeightBuffer;
	BYTE				* m_pTextureBuffer;
	FLOAT				m_vMaxHeight;
	FLOAT				m_vMinHeight;
	FLOAT				m_vOffset;
	FLOAT				m_vScale;

	A3DCOLOR			* m_pVertexColorBuffer;
	A3DVECTOR3			* m_pFaceNormalBuffer;
	A3DVECTOR3			* m_pVertexNormalBuffer;

	bool				m_bShowWire;
	bool				m_bShowTerrain;

	int					m_nNumTexture;
	char				m_szTextureMap[MAXNUM_TEXTURE][MAX_PATH];
	char				m_szTextureBaseName[MAX_PATH];
	char				m_szHeightMapName[MAX_PATH];
	char				m_szTextureMapName[MAX_PATH];

	int					m_nWidth;
	int					m_nHeight;
	int					m_nTextureCover;

	int					m_nSightRange;
	int					m_nVisibleBeginX;
	int					m_nVisibleBeginY;
	int					m_nVisibleBeginXOld;
	int					m_nVisibleBeginYOld;
	
	A3DVECTOR3			m_vecPos;
	A3DMATRIX4			m_matWorld;
	FLOAT				m_vCellSize;

	//Detail Texture Specification;
	int					m_nDetailWidth;
	int					m_nDetailHeight;
	int					m_nDetailVertCount;
	int					m_nDetailFaceCount;
	A3DStream			* m_pDetailStream;
	A3DLVERTEX			* m_pDetailVB;
	WORD				* m_pDetailIndex;
	A3DTexture			* m_pDetailTexture;
	int					m_nDetailCellCount;
	char				m_szDetailTextureName[MAX_PATH];

	bool				m_bRayTraceEnable;
	bool				m_bAABBTraceEnable;

	A3DLight *			m_pDirLight;

protected:
	BYTE GetTextureIndex(int x, int y);
	void SetTextureIndex(int x, int y, BYTE index);

public:
	A3DTerrain();
	virtual ~A3DTerrain();

	bool Init(A3DDevice * pA3DDevice, TERRAINPARAM* pTerrainParam, char * szTexture, char * szHeightMap, char * szTextureMap);
	bool Release();

protected:
	bool CalculateDistanceTable();
	bool CalculateSquareError();
	bool BuildNormals();
	bool LightTerrain();

public:
	bool SetCamera(A3DCameraBase * pA3DCamera);
	bool SetPosition(A3DVECTOR3 vecPos);
	
	bool GetCellPos(A3DVECTOR3 vecPos, int * pX, int * pY, FLOAT * pvX=NULL, FLOAT * pvY=NULL);

	bool SplitMark(const A3DAABB& aabb, A3DLVERTEX* aVerts, WORD* aIndices, bool bJog, int* piNumVert, int* piNumIdx);
	bool ClipInCell(const A3DAABB& aabb, int nCellX, int nCellY, TERRAIN_MARKSPLIT * pMarkSplit);

	A3DVECTOR3 GetVertexPos(int x, int y);
	A3DVECTOR3 GetCellCenterPos(int x, int y);
	bool GetCellAllVertex(int x, int y, A3DVECTOR3 * pVertexPos, WORD * pIndex);
	
	FLOAT GetHeight(A3DVECTOR3 vecPos);
	
	A3DVECTOR3 GetVertexNormal(int x, int y);
	A3DVECTOR3 GetFaceNormal(int x, int y, int nTriangleID);
	A3DVECTOR3 GetVertexNormal(A3DVECTOR3 vecPos);
	A3DVECTOR3 GetFaceNormal(A3DVECTOR3 vecPos);

	A3DCOLOR GetVertexColor(int x, int y);

protected:
	bool UpdateOneCellLevel(int stage, int sx, int sy);
	bool UpdateCellStream(int stage, int sx, int sy);

	bool SmoothConnection(int x, int y, int sx, int sy, int nTexIndex, int nOldVertCount, int nOldFaceCount);

	inline bool ConstructTriangleHalf(int stage, int sx, int sy, int index, int idTex, bool bCallFromQuarter=false, FLOAT vCornerHeight=0.0f);
	inline bool ConstructTriangleQuarter(int stage, int sx, int sy, int index, int idTex, bool bCallFromHalf=false, FLOAT vCenterHeight=0.0f);
	bool ConstructFirstLevelCell(int idTex, int stage, int sx, int sy);
	bool ConstructSecondLevelCell(int idTex, int stage, int sx, int sy);
	bool ConstructThirdLevelCell(int idTex, int stage, int sx, int sy);

	bool UpdateStream();

public:
	bool RenderExtraByTexture(int idTexture);
	bool RenderByTexture(int idTexture);
	bool Render(A3DViewport * pViewport);

	bool RayTrace(const A3DVECTOR3& vecStart, const A3DVECTOR3& vecVelocity, FLOAT vTime, RAYTRACERT* pRayTrace);
	bool RayPickInCell(int x, int y, const A3DVECTOR3& vecStart, const A3DVECTOR3& vecDir, FLOAT vDis, RAYTRACERT* pRayTrace);

protected:
	bool CalculateRenderRange();
	inline A3DLVERTEX GetVertex(int x, int y)
	{
		return A3DLVERTEX(GetVertexPos(x, y), GetVertexColor(x, y) | 0xff000000, A3DCOLORRGBA(0, 0, 0, GetFogData(GetDistance(x - m_ptCamPos.x, y - m_ptCamPos.y))), 
				x * 1.0f / m_nTextureCover, y * 1.0f / m_nTextureCover);
	}

public:
	inline int GetWidth() { return m_nWidth; }
	inline int GetHeight() { return m_nHeight; }

	inline void ToggleShowWire() { m_bShowWire = !m_bShowWire; }
	inline bool GetShowWire() { return m_bShowWire; }
	inline void ToggleShowTerrain() { m_bShowTerrain = !m_bShowTerrain; }
	inline bool GetShowTerrain() { return m_bShowTerrain; }
	inline void SetShowTerrain(bool bShow) { m_bShowTerrain = bShow; }
	inline void SetFolderName(char * szFolderName) { strcpy(m_szFolderName, szFolderName); }
	inline FLOAT GetCellSize() { return m_vCellSize; }
	inline int GetVertCount(int idTex) { return m_nVisibleVerts[idTex]; }
	inline int GetFaceCount(int idTex) { return m_nVisibleFaces[idTex]; }
	int GetTotalFaceCount();
	int GetTotalVertCount();

	BYTE GetFogData(FLOAT vDistance);
	inline void SetRayTraceEnable(bool bEnable) { m_bRayTraceEnable = bEnable; }
	inline void SetAABBTraceEnable(bool bEnable) { m_bAABBTraceEnable = bEnable; }
	inline bool GetRayTraceEnable() { return m_bRayTraceEnable; }
	inline bool GetAABBTraceEnable() { return m_bAABBTraceEnable; }

protected:
	inline FLOAT GetDistance(int dx, int dy) 
	{
		dx = abs(dx);
		dy = abs(dy);
		if( dx >= m_nWidth ) dx = m_nWidth - 1;
		if( dy >= m_nHeight ) dy = m_nHeight - 1;
		return m_pCellDistanceTable[dy * m_nWidth + dx];
	}
	inline FLOAT GetSquareError(int stage, int sx, int sy)
	{
		return m_ppCellSquareErrorTable[stage][sy * m_nStageWidth[stage] + sx];
	}
	inline int DetermineLevel(int stage, FLOAT vDistance, FLOAT vSquareError);
	inline void SetCellLevel(int stage, int sx, int sy, CHAR nLevel, bool bFillSub=true);
	inline CHAR GetCellLevel(int stage, int sx, int sy)
	{
		return m_ppCellLevelTable[stage][sy * m_nStageWidth[stage] + sx];
	}
	//This function determine which triangle contain the point specified with vDX, vDY
	//vDX and vDY should be the difference between vX and x; vY and y;
	inline int GetTriangleID(int x, int y, FLOAT vDX, FLOAT vDY)
	{
		if( (x + y) % 2 == 0 )
		{
			//  v0----v1
			//	| \	1 |
			//	| 0\  |
			//	|   \ |
			//  v2----v3
			if( vDX > vDY ) return 1;
			else return 0;
		}
		else
		{
			//  v0----v1
			//	|0  / |
			//	|  /  |
			//	| /	1 |
			//  v2----v3
			if( vDX + vDY > 1.0f ) return 1;
			else return 0;
		}
		return 0;
	}

	bool ShowOneCellLevelInfo(int stage, int x, int y);
	bool ShowStageCellLevelInfo();

public:
	bool GetTerrainColor(const A3DVECTOR3& vecPos, A3DCOLOR * pDiffuse, A3DCOLOR * pSpecular, BYTE byteAlpha=0xff);

public:
	// Interfaces for scene editor;

	// Initialize, this will create nNumTexture Empty Texture Slots;
	bool Create(A3DDevice * pA3DDevice, int nNumTexture, int nWidth, int nHeight);

	// Get current used texture number;
	bool IncreaseTextureNum(int nTextureIncrease);
	int  GetTextureNum() { return m_nNumTexture; }

	// Map a texture with an index or unmap them by set szTextureMap to "";
	bool SetTextureMap(int nTexIndex, char * szTextureMap, int nNewIndex=0); //Automatic change reference of each cell
	char * GetTextureMap(int nTexIndex) { return m_szTextureMap[nTexIndex]; }

	// Change detail texture;
	bool SetDetailMap(char * szTextureMap);

	// Adjust one vertex's height;
	bool AdjustVertexHeight(int x, int y, FLOAT vDeltaHeight);
	FLOAT GetVertexHeight(int x, int y);

	// Set all vertex's height;
	bool SetHeight(int nWidth, int nHeight, FLOAT * pHeightBuffer);
	FLOAT * GetHeightBuffer() { return m_pHeightBuffer; }

	// Set and retrieve cell'1`s texture index;
	// Notice, x y is in the texture index coordinates;
	void SetCellTexture(int x, int y, BYTE nTexIndex) { SetTextureIndex(x * 8, y * 8, nTexIndex); }
	int  GetCellTexture(int x, int y) { return GetTextureIndex(x * 8, y * 8); }
	
	//After you have made some changes to the terrain and want it show correctly now, call this
	bool UpdateAllChanges(bool bAdjustHeightOnly=false);

	inline void SetDirectionalLight(A3DLight * pA3DLight) { m_pDirLight = pA3DLight; }
	inline A3DLight * GetDirectionalLight() { return m_pDirLight; }

	inline FLOAT GetOptimizeStrength() { return m_vOptimizeStrength; }
	void SetOptimizeStrength(FLOAT vOptimizeStrength);

	inline void SetHollowRect(A3DRECT & rect) { m_rectHollow = rect; }

	inline void SetNoLight(bool bNoLight) { m_bNoLight = bNoLight; }
	inline bool GetNoLight()			  { return m_bNoLight; }
};

typedef A3DTerrain * PA3DTerrain;

#endif
