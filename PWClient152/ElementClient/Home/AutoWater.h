/*
 * FILE: AutoWater.h
 *
 * DESCRIPTION: Class for automaticlly generating water
 *
 * CREATED BY: Jiang Dalong, 2005/01/19
 *
 * HISTORY: 
 *
 * Copyright (c) 2001-2008 Archosaur Studio, All Rights Reserved.
 */

#ifndef _AUTOWATER_H_
#define _AUTOWATER_H_

#include "TerrainWater.h"
#include "BitChunk.h"
#include <AList2.h>

class CAutoHome;

//表示世界空间2D平面(X,Z)上的一个点
typedef struct WGRID
{
	float x;
	float z;
}* PWGRID;

//存储一条轮廓
typedef struct WPROFILE
{
	APtrList<PWGRID> listGrid;
}* PWPROFILE;

// Water type
enum WATERTYPE
{
	WT_NO_WATER		= 0,
	WT_SAND,
	WT_WATER,
};

class CAutoWater  
{
public:
	CAutoWater(CAutoHome* pAutoHome);
	virtual ~CAutoWater();

	// Initialize
	bool Init(A3DTerrainWater* pA3DTerrainWater);
	// Release Water
	void Release();
	// Reset water
	bool ResetWater();
	// Create water
	bool CreateWater(float fWaterHeight, float fSandWidth, float fWaveSize);
	// Calculate water area
	bool CalculateWater(float fHeight);
	// Caluculate sand area
	bool CalculateSand(float fWidth);
	// Save water
	bool SaveWaterToFile(const char* szFile);
	// Check a position is water or sand? 
	int CheckWater(int x, int y);
	// Load water
	bool Load(const char* szFile);
	// Remove water
	bool RemoveWater();
	// Is a position under water?
	bool IsUnderWater(A3DVECTOR3 vPos);
	// Set fog color
	void SetFogColor(A3DCOLOR dwColor);

	// Tick
	bool Tick(DWORD dwTime);
	// Render
	bool Render(const A3DVECTOR3& vecSunDir, A3DViewport* pA3DViewport, A3DTerrain2 * pTerrain, A3DSky * pSky, LPRENDERFORREFLECT pRenderReflect, LPRENDERFORREFRACT pRenderRefract, LPVOID pArg, float fDayWeight);
	
	inline float GetWaterHeight() { return m_fWaterHeight; }
	inline void SetWaterReady(bool bReady) { m_bWaterReady = bReady; }
	inline bool GetWaterReady() { return m_bWaterReady; }
	inline float GetSandWidth() { return m_fSandWidth; }
	inline CTerrainWater* GetWater() { return m_pWater; }

protected:
	CAutoHome*			m_pAutoHome;		// Home
	CTerrainWater*		m_pWater;			// Water
	bool*				m_pWaterArea;		// Water area
	int*				m_pSandArea;		// Sand area

	float				m_fWaterHeight;		// Water height
	float				m_fSandWidth;		// Sand width

	bool				m_bWaterReady;		// Water is ready?

	DWORD				m_dwWaterAreaID;		// Water area ID

	int					m_nGridSize;
	A3DVECTOR3			m_vPos;
	DWORD				m_dwColor;
	int					m_nAreaX;
	int					m_nAreaZ;
	CBitChunk			m_btWaterData;//这儿存放水块的数据，1表示当前块不存在，0表示存在
	//这儿是水的轮廓的列表
	APtrList<PWPROFILE> m_listWaterProfile;
	int					m_nStartPointX;
	int					m_nStartPointZ;
	int					m_nProfileWidth;
	int					m_nProfileHeight;

protected:
	// Calculate sande area
	void CalSandArea(float fWidth, int nRootX, int nRootY);
	// Make water and sand texture
	bool MakeTexture();
	// Create terrain water(CTerrainWater)
	bool CreateTerrainWater(float fWaterHeight);
	void SetWaterSize(int nWidth,int nHeight,int nGridSize);
	void CalculateVisibleChunk();
	//清除轮廓，释放内存
	void ReleaseProfile();
	void CreateProfile(BYTE*pData);
	//计算水面边缘轮廓，结果保存
	void CalculateProfile();
	bool FindGrid(PWPROFILE pProfile,BYTE *pData,BYTE *pFlag);
	bool FindStartPoint(int& sx, int& sy,BYTE *pData,BYTE *pFlag);
	bool GetNextPoint(int lastDir, int cx,int cy, int& nx, int& ny, int& newDir
									  ,BYTE *pData,BYTE *pFlag);
	void CutProfile();
	bool TestGridIntersectionWithWater(WGRID grid);
	void SmoothProfile();
	bool GridIsExist(APtrList<PWGRID> &listGrid,WGRID grid);
	bool RepairProfile();
	bool IsEdge(WGRID grid);
	void ProcessProfile();
	bool IsIntersectionWithWater(A3DVECTOR3 pt1,A3DVECTOR3 pt2);
	bool PlaneIntersectionWithLine(A3DVECTOR3 vStart,A3DVECTOR3 vVec,A3DVECTOR3 point,A3DVECTOR3 normal,A3DVECTOR3 &out);
	int FindGridDifferentPt(WGRID *pGridArray,int count,WGRID &grid1,WGRID &grid2);
	bool IsOrder(APtrList<PWGRID> &listGrid,WGRID w1,WGRID w2);
	bool IsOrder(WGRID oldGrid,WGRID w1,WGRID w2);
	void CircleProfile();
	void CalculateVisibleChunkFromProfile();
	void ReduceProfile();
	bool BuildRenderWater(A3DVECTOR3& vPos, float fWaveSize);
};

#endif // #ifndef _AUTOWATER_H_
