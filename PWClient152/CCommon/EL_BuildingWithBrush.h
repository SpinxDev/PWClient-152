/*
 * FILE: EL_BuildingWithBrush.h
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Hedi, 2006/6/9
 *
 * HISTORY: 
 *
 * Copyright (c) 2006 Archosaur Studio, All Rights Reserved.
 */

#ifndef _EL_BUILDINGWITHBRUSH_H_
#define _EL_BUILDINGWITHBRUSH_H_

#include <vector.h>
#include "ConvexHullData.h"
#include "A3DGeometry.h"
#include "aabbcd.h"

using namespace CHBasedCD;
using namespace abase;

#define BUILDINGWITHBRUSH_FADE_IN_TIME	2000

class AFile;
class A3DLitModel;
class A3DTexture;
class A3DStream;
class A3DDevice;

typedef vector<int>		MESHLIST;

class CELBuildingWithBrush
{
protected:
	A3DDevice *					m_pA3DDevice;
	A3DLitModel *				m_pA3DLitModel;

	bool						m_bManualUpdateBrush;
	int							m_nNumHull;
	int							m_nNumBrush;
	vector<MESHLIST>			m_HullMeshList;
	vector<CConvexHullData *>	m_ConvexHullData;
	vector<CCDBrush *>			m_CDBrushes;

	DWORD						m_dwTimeLived;				// time passed since this building appear
	bool						m_bCollideOnly;				// flag indicates this building is used for collision detection only
	bool						m_bHasLit;					// flag indicates whether this building has been lit
	
	A3DOBB						m_ModelOBB;					// Obb of model

protected:
	bool LoadHullMeshList(AFile * pFileToLoad);
	bool SaveHullMeshList(AFile * pFileToSave);
	bool LoadConvexHullData(AFile * pFileToLoad);
	bool SaveConvexHullData(AFile * pFileToSave);

	bool ReleaseHullMeshList();
	bool ReleaseCDBrushes();
	bool ReleaseConvexHullData();

	bool TransformHull(const A3DMATRIX4& tm);
	
public:
	A3DLitModel * GetA3DLitModel()		{ return m_pA3DLitModel; }

	const vector<CConvexHullData *>& GetConvexHulls() { return m_ConvexHullData;}

	A3DVECTOR3 GetPos();
	A3DVECTOR3 GetDir();
	A3DVECTOR3 GetUp();
	A3DVECTOR3 GetScale();
	A3DMATRIX4 GetAbsoluteTM();
	A3DAABB GetModelAABB();

	void SetLitFlag( bool bLit ) { m_bHasLit = bLit; }
	bool GetLitFlag() { return m_bHasLit; }

	void SetManualUpdateBrush(bool bFlag) { m_bManualUpdateBrush = bFlag; }
	bool GetManualUpdateBrush() { return m_bManualUpdateBrush; }
	
	bool RebuildBrushes();
	A3DOBB& GetModelOBB() { return m_ModelOBB; }
	bool SetOBBScale(float x, float y, float z);	
	bool UpdateOBBWithBuilding();

public:
	CELBuildingWithBrush();
	~CELBuildingWithBrush();

	// Initialize and finalize
	bool Init(A3DDevice * pA3DDevice);
	bool Release();

	// Render method
	bool Render(A3DViewport * pViewport, bool bRenderAlpha=true); 
	// Tick method
	bool Tick(DWORD dwDeltaTime);
	
	// Debug render
	bool RenderConvexHull(A3DViewport * pViewport);

	// Load from seperate file
	bool LoadFromMOXAndCHF(A3DDevice * pA3DDevice, const char * szFileMox, const char * szFileCHF, const A3DMATRIX4& matTM);

	// Set Collide Only Flag
	bool SetCollideOnly(bool bFlag);
	bool GetCollideOnly() { return m_bCollideOnly; }

	// Adjust scale factor
	bool SetScale(float vScale);

	// Direct Set the position and orientation methods
	bool SetPos(const A3DVECTOR3& vecPos);
	bool SetDirAndUp(const A3DVECTOR3& vecDir, const A3DVECTOR3& vecUp);

	// Relatively adjust orientation and position methods
	bool Move(const A3DVECTOR3& vecDeltaPos);
	bool RotateX(float vDeltaRad);
	bool RotateY(float vDeltaRad);
	bool RotateZ(float vDeltaRad);

	// Ray trace methods.
	// return true if hit, false if not.
	bool RayTraceAABB(const A3DVECTOR3& vecStart, const A3DVECTOR3& vecDelta, A3DVECTOR3& vecHitPos, float * pvFraction, A3DVECTOR3& vecNormal);
	bool RayTraceMesh(const A3DVECTOR3& vecStart, const A3DVECTOR3& vecDelta, A3DVECTOR3& vecHitPos, float * pvFraction, A3DVECTOR3& vecNormal);
	
	bool RayTraceConvexHull(const A3DVECTOR3& vecStart, const A3DVECTOR3& vecDelta, A3DVECTOR3& vecHitPos, float * pvFraction, A3DVECTOR3& vecNormal);
	bool RayTraceConvexHull(const A3DVECTOR3& vecStart, const A3DVECTOR3& vecDelta, A3DVECTOR3& vecHitPos, float * pvFraction, A3DVECTOR3& vecNormal, CConvexHullData* & pTraceCHData);		// a more return value: pCHData
	bool RayTraceConvexHull(const A3DVECTOR3& vecStart, const A3DVECTOR3& vecDelta, A3DVECTOR3& vecHitPos, float * pvFraction, A3DVECTOR3& vecNormal, CConvexHullData* & pTraceCHData, const CFace* & pTraceFace);		// a more return value: pTraceFace

	// Sphere- Collision Detection
	bool SphereCollideWithMe(const A3DVECTOR3& vecStart, float fRadius, const A3DVECTOR3& vecDelta, A3DVECTOR3& vecHitPos, float * pvFraction, A3DVECTOR3& vecNormal /* , bool bExactCD=false */);
	bool SphereCollideWithMe(const A3DVECTOR3& vecStart, float fRadius, const A3DVECTOR3& vecDelta, A3DVECTOR3& vecHitPos, float * pvFraction, A3DVECTOR3& vecNormal, CConvexHullData* & pCDCHData /*, bool bExactCD=false */);
	bool SphereCollideWithMe(const A3DVECTOR3& vecStart, float fRadius, const A3DVECTOR3& vecDelta, A3DVECTOR3& vecHitPos, float * pvFraction, A3DVECTOR3& vecNormal, CConvexHullData* & pCDCHData, const CFace* & pTraceFace /*, bool bExactCD=false */);

	bool TraceWithBrush(BrushTraceInfo * pInfo, bool bCheckCHFlags = true);

	virtual int GetBrushCount() { return m_CDBrushes.size(); }
	virtual CCDBrush * GetBrush(int index) { return m_CDBrushes[index]; }
};

#endif//_EL_BUILDING_H_