/*
 * FILE: A3DMorphSkinMesh.h
 *
 * DESCRIPTION: A3D morph skin mesh class
 *
 * CREATED BY: duyuxin, 2003/11/9
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#ifndef _A3DMORPHSKINMESH_H_
#define _A3DMORPHSKINMESH_H_

#include "A3DSkinMesh.h"
#include "A3DMorphMesh.h"

///////////////////////////////////////////////////////////////////////////
//
//	Define and Macro
//
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//
//	Types and Global variables
//
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//
//	Declare of Global functions
//
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//
//	Class A3DMorphSkinMesh
//
///////////////////////////////////////////////////////////////////////////

class A3DMorphSkinMesh : public A3DSkinMesh
{
public:		//	Types

public:		//	Constructors and Destructors

	A3DMorphSkinMesh();
	virtual ~A3DMorphSkinMesh();

public:		//	Attributes

	A3DMorphMesh	m_MorphData;

public:		//	Operations

	//	Initialize object
	virtual bool Init(A3DEngine* pA3DEngine);
	//	Release object
	virtual void Release();

	//	Load skin mesh data form file
	virtual bool Load(AFile* pFile, A3DSkin* pSkin, int nSkinVersion);
	//	Save skin mesh data from file
	virtual bool Save(AFile* pFile,int nSkinVersion, int& nReturnValue);

	//	Render mesh
	virtual bool Render(A3DViewport* pViewport, A3DSkinModel* pSkinModel, A3DSkin* pSkin);
	//	Software rendering
	virtual bool RenderSoftware(A3DViewport* pViewport, A3DSkinModel* pSkinModel, A3DSkin* pSkin);

	//	Get approximate mesh data size
	virtual int GetDataSize();

	//	Get bounded bone
	int GetBoundBone() { return (m_aVerts[0].dwMatIndices & 0x000000ff); }

protected:	//	Attributes

protected:	//	Operations

	//	Calculate number of blend matrix can be used in vertex shader
	virtual void CalcVSBlendMatNum();
	//	Load mesh data form file
	virtual bool LoadData(AFile* pFile, A3DSkin* pSkin, int nSkinVersion);
	//	Create sub meshes
	virtual bool CreateSubMeshes(A3DSkin* pSkin);
};

///////////////////////////////////////////////////////////////////////////
//
//	Inline function
//
///////////////////////////////////////////////////////////////////////////


#endif	//	_A3DMORPHSKINMESH_H_

