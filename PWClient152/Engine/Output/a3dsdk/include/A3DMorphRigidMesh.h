/*
 * FILE: A3DMorphRigidMesh.h
 *
 * DESCRIPTION: A3D morph rigid mesh class
 *
 * CREATED BY: duyuxin, 2003/11/9
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#ifndef _A3DMORPHRIGIDMESH_H_
#define _A3DMORPHRIGIDMESH_H_

#include "A3DRigidMesh.h"
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
//	Class A3DMorphRigidMesh
//
///////////////////////////////////////////////////////////////////////////

class A3DMorphRigidMesh : public A3DRigidMesh
{
public:		//	Types

public:		//	Constructors and Destructors

	A3DMorphRigidMesh();
	virtual ~A3DMorphRigidMesh();

public:		//	Attributes

	A3DMorphMesh	m_MorphData;

public:		//	Operations

	//	Initialize object
	virtual bool Init(A3DEngine* pA3DEngine);
	//	Release object
	virtual void Release();

	//	Load skin mesh data form file
	virtual bool Load(AFile* pFile, A3DSkin* pSkin,int nSkinVersion);
	//	Save skin mesh data from file
	virtual bool Save(AFile* pFile, int nSkinVersion,int& nReturnValue);

	//	Render mesh
	virtual bool Render(A3DViewport* pViewport, A3DSkinModel* pSkinModel, A3DSkin* pSkin);

	//	Get approximate mesh data size
	virtual int GetDataSize();

protected:	//	Attributes

protected:	//	Operations

	//	Software rendering
	virtual bool RenderSoftware(A3DViewport* pViewport, A3DSkinModel* pSkinModel, A3DSkin* pSkin);
};

///////////////////////////////////////////////////////////////////////////
//
//	Inline function
//
///////////////////////////////////////////////////////////////////////////


#endif	//	_A3DMORPHRIGIDMESH_H_

