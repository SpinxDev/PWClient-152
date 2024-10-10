/*
 * FILE: A3DMuscleMesh.h
 *
 * DESCRIPTION: Muscle controlled mesh
 *
 * CREATED BY: Hedi, 2004/7/5
 *
 * HISTORY: 
 *
 * Copyright (c) 2004 Archosaur Studio, All Rights Reserved.
 */

#ifndef _A3DMUSCLEMESH_H_
#define _A3DMUSCLEMESH_H_

#include "A3DMeshBase.h"
#include "A3DVertex.h"
#include "AArray.h"

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

class A3DStream;
class A3DTexture;
class A3DSkeleton;
class A3DMaterial;
class A3DSkinModel;
class A3DMuscleOrgan;
class A3DMuscleData;

///////////////////////////////////////////////////////////////////////////
//	
//	Declare of Global functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Class A3DMuscleMesh
//		original vertex data are stored in this mesh, it is shared by 
//		all skin which is using this mesh
//	
///////////////////////////////////////////////////////////////////////////

class A3DMuscleMesh : public A3DMeshBase
{
public:		//	Types

public:		//	Constructor and Destructor

	A3DMuscleMesh();
	virtual ~A3DMuscleMesh();

public:		//	Attributes

public:		//	Operations

	//	Initialize object
	virtual bool Init(A3DEngine* pA3DEngine);
	//	Release object
	virtual void Release();

	//	Load skin mesh data form file
	virtual bool Load(AFile* pFile, A3DSkin* pSkin, int nSkinVersion);
	//	Save skin mesh data from file
	virtual bool Save(AFile* pFile, int nSkinVersion, int& nReturnValue);

	//	Get approximate mesh data size
	virtual int GetDataSize();

	//	Create original data buffers
	bool CreateOriginalBuffers(int iNumVert, int iNumIdx);

	//	Get vertex buffer which contains original vertex data
	inline A3DBVERTEX3* GetOriginVertexBuf() { return m_aVerts; }
	//	Get index buffer which contains original index data
	inline WORD* GetOriginIndexBuf() { return m_aIndices; }

protected:	//	Attributes
	A3DBVERTEX3*	m_aVerts;			//	Original render vertex data buffer
	WORD*			m_aIndices;			//	Original indices data buffer
	
protected:	//	Operations

	//	Calculate initial mesh aabb
	void CalcInitMeshAABB();
	//	Release original data buffers
	void ReleaseOriginalBuffers();
};

///////////////////////////////////////////////////////////////////////////
//	
//	Class A3DMuscleMeshImp
//		the deformed mesh by the muscle organs.
//	
///////////////////////////////////////////////////////////////////////////

class A3DMuscleMeshImp : public A3DMeshBase
{
public:		//	Types

public:		//	Constructor and Destructor

	A3DMuscleMeshImp(A3DMuscleMesh* pCoreMesh, A3DSkinModel* pSkinModel);
	virtual ~A3DMuscleMeshImp();

public:		//	Attributes

public:		//	Operations

	//	Initialize object
	virtual bool Init(A3DEngine* pA3DEngine);
	//	Release object
	virtual void Release();

	//	Render mesh
	virtual bool Render(A3DViewport* pViewport, A3DSkinModel* pSkinModel, A3DSkin* pSkin);

	//	Get approximate mesh data size
	virtual int GetDataSize();

	//	Create muscle control organs using a set of muscle data;
	//	the meshes' vertex data will be transformed by blend matrices at first
	//	and then calculate the afr weight using the data from muscle data
	bool CreateMuscleOrgan(const A3DMuscleData& muscleData);

	//	Update mesh
	bool Update(int iDeltaTime, A3DSkinModel* pSkinModel);

	//	Get undeformed original vertex buffer
	A3DBVERTEX3 * GetOriginalVertexBuf();

	inline A3DMuscleOrgan* GetMuscleOrgan()		{ return m_pMuscleOrgan; }
	inline A3DBVERTEX3* GetDeformedBuffer()		{ return m_aDeformedVerts; }

protected:	//	Attributes

	A3DBVERTEX3*	m_aDeformedVerts;	//	Deformed render vertex data buffer
	A3DSkinModel*	m_pSkinModel;		//	Skin model this mesh belongs to
	A3DStream*		m_pA3DStream;		//	Stream used in hardware rendering
	A3DMuscleMesh*	m_pCoreMesh;		//	Core mesh object
	A3DMuscleOrgan*	m_pMuscleOrgan;		//	Muscle organ, used to do deforming

protected:	//	Operations

	//	Create deformed buffers
	bool CreateDeformedBuffers();
	//	Release deforemd buffers
	void ReleaseDeformedBuffers();
	//	Create A3D stream
	bool CreateStream();
	//	Release A3D stream;
	void ReleaseStream();

	// Release muscle control organ;
	void ReleaseMuscleOrgan();
	//	Calculate vertices positions
	void CalculateVertices(int iDeltaTime, A3DSkinModel* pSkinModel);
};

///////////////////////////////////////////////////////////////////////////
//	
//	Inline functions
//	
///////////////////////////////////////////////////////////////////////////

#endif	//	_A3DMUSCLEMESH_H_
