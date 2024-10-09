/*
 * FILE: EL_Building.h
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Hedi, 2005/1/29
 *
 * HISTORY: 
 *
 * Copyright (c) 2005 Archosaur Studio, All Rights Reserved.
 */

#ifndef _EL_BUILDING_H_
#define _EL_BUILDING_H_

#include <vector.h>
#include "ConvexHullData.h"
#include "A3DGeometry.h"
using namespace CHBasedCD;
using namespace abase;

#define BUILDING_FADE_IN_TIME	2000
#define ELBUILDING_VERSION		0x80000001

class AFile;
class A3DLitModel;
class A3DTexture;
class A3DStream;

typedef vector<int>		MESHLIST;

class CELBuilding
{
protected:
	A3DDevice *					m_pA3DDevice;
	A3DLitModel *				m_pA3DLitModel;

	int							m_nNumHull;
	vector<MESHLIST>			m_HullMeshList;
	vector<CConvexHullData *>	m_ConvexHullData;

	DWORD						m_dwTimeLived;				// time passed since this building appear
	bool						m_bCollideOnly;				// flag indicates this building is used for collision detection only

protected:
	bool LoadHullMeshList(AFile * pFileToLoad);
	bool SaveHullMeshList(AFile * pFileToSave);
	bool LoadConvexHullData(AFile * pFileToLoad);
	bool SaveConvexHullData(AFile * pFileToSave);
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
	
public:
	CELBuilding();
	~CELBuilding();

	// Initialize and finalize
	bool Init(A3DDevice * pA3DDevice);
	bool Release();

	// Render method
	bool Render(A3DViewport * pViewport, bool bRenderAlpha=true); 
	// Tick method
	bool Tick(DWORD dwDeltaTime);
	
	// Debug render
	bool RenderConvexHull(A3DViewport * pViewport);

	// Load and Save
	bool Save(const char * szFileName);
	bool Load(A3DDevice * pA3DDevice, const char * szFileName);

	bool Save(AFile * pFileToSave);
	bool Load(A3DDevice * pA3DDevice, AFile * pFileToLoad);

	bool ExportBrushBuilding(const char * szFileName);
	bool ExportBrushBuilding(AFile * pFileToSave);

	// Load from seperate file
	bool LoadFromMOXAndCHF(A3DDevice * pA3DDevice, const char * szFileMox, const char * szFileCHF);

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
	
	bool SaveCHFDataForEditor(AFile * pFileToSave);
	bool LoadCHFDataForEditor(AFile * pFileToRead);

	bool ReleaseHullMeshList();
	bool ReleaseConvexHullData();
};

#endif//_EL_BUILDING_H_