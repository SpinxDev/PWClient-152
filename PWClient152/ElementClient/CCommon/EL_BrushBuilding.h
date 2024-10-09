/*
 * FILE: EL_BrushBuilding.h
 *
 * DESCRIPTION: building using brush for cd, only used in element client
 *
 * CREATED BY: Hedi, 2005/8/23
 *
 * HISTORY: 
 *
 * Copyright (c) 2005 Archosaur Studio, All Rights Reserved.
 */

#ifndef _EL_BRUSHBUILDING_H_
#define _EL_BRUSHBUILDING_H_

#include <vector.h>
#include "aabbcd.h"
#include "A3DGeometry.h"
#include "EC_BrushMan.h"

using namespace CHBasedCD;
using namespace abase;

#define BRUSHBUILDING_FADE_IN_TIME	2000
#define ELBRUSHBUILDING_VERSION		0x80000001

class AFile;
class A3DLitModel;
class A3DTexture;
class A3DStream;

typedef vector<int>		MESHLIST;

class CELBrushBuilding : public CBrushProvider
{
protected:
	A3DDevice *					m_pA3DDevice;
	A3DLitModel *				m_pA3DLitModel;

	int							m_nNumHull;
	vector<MESHLIST>			m_HullMeshList;
	vector<CCDBrush *>			m_CDBrushes;

	DWORD						m_dwTimeLived;				// time passed since this building appear
	bool						m_bCollideOnly;				// flag indicates this building is used for collision detection only

protected:
	bool LoadHullMeshList(AFile * pFileToLoad);
	bool SaveHullMeshList(AFile * pFileToSave);

	bool LoadCDBrushes(AFile * pFileToLoad,float cx,float cz);
	bool SaveCDBrushes(AFile * pFileToSave);

	bool ReleaseHullMeshList();
	bool ReleaseCDBrushes();

public:
	A3DLitModel * GetA3DLitModel()		{ return m_pA3DLitModel; }
	A3DAABB GetModelAABB();

	// Set Collide Only Flag
	bool SetCollideOnly(bool bFlag);
	bool GetCollideOnly() { return m_bCollideOnly; }

public:
	CELBrushBuilding();
	~CELBrushBuilding();

	// Initialize and finalize
	bool Init(A3DDevice * pA3DDevice);
	bool Release();

	// Render method
	bool Render(A3DViewport * pViewport, bool bRenderAlpha=true); 
	// Tick method
	bool Tick(DWORD dwDeltaTime);
	
	// Load and Save
	bool Save(const char * szFileName);
	bool Load(A3DDevice * pA3DDevice, const char * szFileName,float cx,float cz);

	bool Save(AFile * pFileToSave);
	bool Load(A3DDevice * pA3DDevice, AFile * pFileToLoad,float cx,float cz);

	// Revised by wenfeng, 05-09-06
	// bCheckCHFlags: whether we need to check the flags in Brush data 
	//		when we handle the trace.
	bool TraceWithBrush(BrushTraceInfo * pInfo, bool bCheckCHFlags = true);

	virtual int GetBrushCount() { return m_CDBrushes.size(); }
	virtual CCDBrush * GetBrush(int index) { return m_CDBrushes[index]; }
};

#endif//_EL_BRUSHBUILDING_H_