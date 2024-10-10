/*
 * FILE: A3DClothMesh.h
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Duyuxin, 2008/6/16
 *
 * HISTORY: 
 *
 * Copyright (c) 2004 Archosaur Studio, All Rights Reserved.
 */

#ifndef _A3DCLOTHMESH_H_
#define _A3DCLOTHMESH_H_

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

///////////////////////////////////////////////////////////////////////////
//	
//	Declare of Global functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Class A3DClothMeshImp
//	
///////////////////////////////////////////////////////////////////////////

class A3DClothMeshImp : public A3DMeshBase
{
public:		//	Types

	//	Vertex info
	struct VERTEX_INFO
	{
		float	tu;
		float	tv;
	};

	friend class A3DSkin;

public:		//	Constructor and Destructor

	A3DClothMeshImp(A3DMeshBase* pCoreMesh);
	virtual ~A3DClothMeshImp();

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

	//	Update mesh data
	bool UpdateMeshData(int iNumVert, int iNumIndex, const A3DVECTOR3* aPos, const A3DVECTOR3* aNormals, const int* aIndices, int iParentIdxNum, const int* aParentIdx);
	//	Check if mesh structure has been changed
	bool MeshChanged() { return m_bMeshChanged; }
	//	Reset mesh structure to initial state
	bool Reset();

protected:	//	Attributes

	A3DMeshBase*	m_pCoreMesh;		//	Core mesh
	A3DVERTEX*		m_aVerts;			//	Vertex data
	WORD*			m_aIndices;			//	Index data
	bool			m_bMeshChanged;		//	true, mesh structure has been changed

	AArray<VERTEX_INFO>		m_aVertInfo;	//	Vertex info

protected:	//	Operations

	//	Setup original vertex info
	bool ResetVertexInfo();
	//	Prepare stream for rendering
	A3DStream* PrepareStream(A3DSkin* pSkin);
};

///////////////////////////////////////////////////////////////////////////
//	
//	Inline functions
//	
///////////////////////////////////////////////////////////////////////////

#endif	//	_A3DCLOTHMESH_H_


