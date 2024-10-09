/*
 * FILE: AutoScene.h
 *
 * DESCRIPTION: Class for a scene
 *
 * CREATED BY: Jiang Dalong, 2006/06/12
 *
 * HISTORY: 
 *
 * Copyright (c) 2001-2008 Archosaur Studio, All Rights Reserved.
 */

#ifndef _AUTOSCENE_H_
#define _AUTOSCENE_H_

#include <AArray.h>
#include <AList2.h>
#include <ARect.h>
#include <A3DVector.h>

class CAutoHomeMan;
class CAutoHome;
class CHomeTerrain;
class CECWorld;
class CAutoSceneConfig;
class CAutoHomeBorderData;
class A3DEngine;
class A3DViewport;
class A3DCameraBase;

class CAutoScene  
{
public:
	typedef APtrArray<CAutoHome*>	BlockArray;

	//	Active blocks
	struct ACTBLOCKS
	{
		ACTBLOCKS() : aBlocks(0, 128) {}

		ARectI		rcArea;		//	Active area represented in blocks
		BlockArray	aBlocks;	//	Active block array

		//	Get block object at specified row, column.
		CAutoHome* GetBlock(int r, int c, bool bClear)
		{
			ASSERT(rcArea.PtInRect(c, r));

			int iIndex = GetBlockIndex(r, c);
			CAutoHome* pBlock = aBlocks[iIndex];

			if (bClear)
				aBlocks[iIndex] = NULL;

			return pBlock;
		}

		//	Set block object at specified row, column
		void SetBlock(int r, int c, CAutoHome* pBlock)
		{
			ASSERT(rcArea.PtInRect(c, r));
			int iIndex = GetBlockIndex(r, c);
			aBlocks[iIndex] = pBlock;
		}

		//	Get block index in aBlocks
		int GetBlockIndex(int r, int c)
		{
			return (r - rcArea.top) * rcArea.Width() + c - rcArea.left;
		}
	};

public:
	CAutoScene(CECWorld* pECWorld, A3DEngine* pA3DEngine);
	virtual ~CAutoScene();

	bool Init(const char* szConfigFile);
	void Release();
	bool Update(DWORD dwDeltaTime, A3DCameraBase* pCamera, const A3DVECTOR3& vLoadCenter);
	bool Render(A3DViewport* pA3DViewport);
	// Get terrain height at a specified position
	float GetPosHeight(const A3DVECTOR3& vPos);
	// Get a home block
	CAutoHome* GetHome(int r, int c);
	// Ray trace
	bool RayTrace(const A3DVECTOR3& vStart, const A3DVECTOR3& vEnd, A3DVECTOR3& vTracePos);
	// Generate unique ID
	DWORD GenerateID() { return m_dwUniqueID++; }
	// Set day weight
	void SetDayWeight(float fWeight);
	// Update sky
	bool UpdateSky();
	// Under water?
	bool IsUnderWater(const A3DVECTOR3& vPos);

	inline CAutoSceneConfig* GetAutoSceneConfig() { return m_pAutoSceneConfig; }
	inline CHomeTerrain* GetHomeTerrain() { return m_pHomeTerrain; }
	inline A3DEngine* GetA3DEngine() { return m_pA3DEngine; }
	inline CECWorld* GetECWorld() { return m_pECWorld; }
	inline CAutoHomeBorderData* GetAutoHomeBorderData() { return m_pAutoHomeBorderData; }
	inline CAutoHomeMan* GetAutoHomeMan() { return m_pAutoHomeMan; }

	inline float	GetBlockSize()				{ return m_fBlockSize; };
	inline float	GetWorldWid()				{ return m_fWorldWid; }
	inline float	GetWorldLen()				{ return m_fWorldLen; }
	inline int		GetBlockRowNum()			{ return m_iNumBlockRow; }
	inline int		GetBlockColNum()			{ return m_iNumBlockCol; }
	inline const ARectF& GetSceneRect()			{ return m_rcWorld; }
	inline float	GetActiveRadius()			{ return m_fActRadius; }
	inline float	GetDayWeight()				{ return m_fDayWeight; }
	inline float	GetCameraSpeed()			{ return m_fCameraSpeed; }
	inline void		SetCameraSpeed(float fSpeed)	{ m_fCameraSpeed = fSpeed; }

protected:

	A3DEngine*		m_pA3DEngine;		//	A3DEngine object
	CECWorld*		m_pECWorld;			//	World
	CHomeTerrain*	m_pHomeTerrain;		//	Terrain object
	CAutoSceneConfig*		m_pAutoSceneConfig;		// Scene config
	CAutoHomeBorderData*	m_pAutoHomeBorderData;	// Home border line
	CAutoHomeMan*	m_pAutoHomeMan;		// Home manager

	ACTBLOCKS		m_ActBlocks1;		//	Active block array
	ACTBLOCKS		m_ActBlocks2;		//	Active blocks array
	ACTBLOCKS*		m_pCurActBlocks;	//	Currently active block array
	ACTBLOCKS*		m_pOldActBlocks;	//	Old active block array
	int				m_iBlkLoadSpeed;	//	Number of block loaded every frame
	AList2<int, int>		m_CandidateBlkList;	//	Candidate block list
	A3DVECTOR3		m_vLoadCenter;		//	Load center
	float			m_fWorldWid;		//	world width (x axis) in logic unit (metres)
	float			m_fWorldLen;		//	World length (z axis) in logic unit (metres)
	float			m_fActRadius;		//	Active area radius in logic unit (metres)
	float			m_fBlockSize;		//	Block size (on x and z axis) in logic unit (metres)
	int				m_iNumBlock;		//	Number of block
	int				m_iNumBlockRow;		//	Block row number in whole world
	int				m_iNumBlockCol;		//	Block column number of whole world
	ARectF			m_rcWorld;			//	Whole world area in logic unit (metres)

	float			m_fDayWeight;		//	Day and night weight. 0: day, 1: night
	float			m_fDayWeightOld;

	DWORD			m_dwUniqueID;

	float			m_fCameraSpeed;		//	Camera speed


protected:
	//	Calculate area represented in blocks
	void CalcAreaInBlocks(const A3DVECTOR3& vCenter, float fRadius, ARectI& rcArea);
	//	Synchronously update active blocks
	bool UpdateActiveBlocks(const A3DVECTOR3& vCenter);
	//	Load all active blocks immediately
	bool LoadActiveBlocks(ACTBLOCKS& ActBlocks);
	//	Unload active blocks
	void UnloadActiveBlocks(ACTBLOCKS& ActBlocks);
	//	Load blocks from candidate list
	bool LoadCandidateBlocks();
	//	Load a block
	CAutoHome* LoadBlock(int r, int c, int iBlock);
	//	Unload a block
	void UnloadBlock(CAutoHome* pBlock);
	//	Release all loaded blocks
	void ReleaseAllBlocks();
	// Adjust environment
	bool AdjustEnvironment(A3DCameraBase* pCamera);
	// Update all visible home
	bool UpdateVisibleHome(DWORD dwDeltaTime);
};

#endif // #ifndef _AUTOSCENE_H_

